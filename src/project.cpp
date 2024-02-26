
//===- SVF-Project -------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//
#include "SVF-LLVM/LLVMUtil.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "WPA/Andersen.h"
#include "Graphs/ICFG.h"

#include <iostream>
#include <vector>
#include <set>

using namespace llvm;
using namespace std;
using namespace SVF;

static llvm::cl::opt<std::string> InputFilename(cl::Positional,
		llvm::cl::desc("<input bitcode>"), llvm::cl::init("-"));

int main(int argc, char ** argv) {
	int arg_num = 0;
	char **arg_value = new char*[argc];
	std::vector<std::string> moduleNameVec;
	SVF::LLVMUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
	cl::ParseCommandLineOptions(arg_num, arg_value,
								"Whole Program Points-to Analysis\n");

	SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
	// build icfg 
	SVFIRBuilder builder(svfModule);
	SVFIR* pag = builder.build();
	ICFG* icfg = pag->getICFG();

	// Functions
	bool find_all_cycle_free_paths(ICFG* icfg, ICFGNode* source, ICFGNode* sink, 
		std::vector<std::vector<int>>& cycle_free_paths, std::vector<ICFGNode*>& segmentation_points);
	std::vector<std::vector<int>> find_cycles(ICFG* icfg, ICFGNode* segmentation_point, std::unordered_set<int>& visited);
	std::vector<std::string> get_permutations(int total_num_cycles);
	std::map<int, int> get_relevant_segmentation_points(std::vector<int>& cycle_free_path, const std::vector<ICFGNode*>& segmentation_points);
	std::vector<std::vector<int>> segment_cycle_free_path(std::vector<int>& cycle_free_path, std::map<int, int>& relevant);
	std::string get_string(std::vector<int>& path, bool final);
	std::string get_cycle_string(std::vector<int>& cycle, bool init);

	// icfg->dump("switch");

	// Find source and sink
	ICFGNode *source, *sink = nullptr;
	for(auto it = icfg->begin(); it != icfg->end(); ++it){
	  // FunEntryBlock
	  if(it->second->getNodeKind() == 1) {
		if(it->second->getFun()->getName() == "src") {
		  source = it->second;
		}
		else if(it->second->getFun()->getName() == "sink") {
		  sink = it->second;
		}
	  }
	}

	if(source == nullptr || sink == nullptr) {
	  cout << "No source or sink was found in the ICFG";
	  return 1;
	}

	// Finding all cycle free paths from source to sink
	// Also performing reachability analysis
	std::vector<std::vector<int>> cycle_free_paths;
	std::vector<ICFGNode*> segmentation_points;
	bool cycles_exist = find_all_cycle_free_paths(icfg, source, sink, cycle_free_paths, segmentation_points);
	
	// If cycles exist, find the cycles
	std::map<int, std::vector<std::vector<int>>> cycles_map;
	std::unordered_set<int> visited;
	if(cycles_exist) {
		// Finding the potential cycles corresponding to each potential segmentation point
		for(auto segmentation_point : segmentation_points) {
			visited.clear();
			std::vector<std::vector<int>> cycles;
			// Finding the indices of successor of each segmentation point in the cycle free path
			for(auto cycle_free_path : cycle_free_paths) {
				int successor_idx = std::distance(
					cycle_free_path.begin(), 
					std::find(cycle_free_path.begin(), cycle_free_path.end(), segmentation_point->getId())
				) + 1;
				visited.insert(cycle_free_path[successor_idx]);
			}
			// Performing a DFS starting from the segmentation point, with the same goal 
			// and visited set contains all children of the segmentation node that were found in cycle-free paths
			cycles = find_cycles(icfg, segmentation_point, visited);
			if(cycles.size() != 0) { // If the cycle exists (DFS terminates and reaches the same starting point)
				cycles_map[segmentation_point->getId()] = cycles;
			}
		}
	} else {
		for(auto cycle_free_path : cycle_free_paths) {
			cout << get_string(cycle_free_path, true) << "\n";
		}
		return 0;
	}

	// Assembly of paths and cycles
	// Applying all combinations of cycles on all cycle free paths
	for(auto cycle_free_path : cycle_free_paths) {
		std::map<int, int> relevant;
		relevant = get_relevant_segmentation_points(cycle_free_path, segmentation_points);

		// Finding corresponding cycles for the relevant segmentation points
		// std::vector<std::pair<int, std::vector<int>>> cycles_vector_flattened;
		int total_num_cycles = 0;
		for(auto [id, index] : relevant) {
			std::vector<std::vector<int>> cycles = cycles_map[id];
			total_num_cycles += cycles.size();
		}

		// Segmenting cycle free path based on relevant segmentation points
		std::vector<std::vector<int>> segments = segment_cycle_free_path(cycle_free_path, relevant);

		// Computing all permutations of cycle application
		std::vector<std::string> perms = get_permutations(total_num_cycles);

		// Applying cycles on current cycle free path according to obtained permutations
		for(int i = 0; i < perms.size(); i++) {
			std::string perm = perms[i];
			std::string out_path;
			int cycle_ctr = 0;
			bool init;
			if(segments[0].size() == 0) init = true;
			for(int j = 0; j < segments.size() - 1; j++) {
				// Append requisite segment
				out_path += get_string(segments[j], false);

				// Append appropriate cycles
				int segmentation_point_id = std::next(relevant.begin(), j)->first;
				std::vector<std::vector<int>> cycles_for_segmentation_point = cycles_map[segmentation_point_id];

				// Iterating over bit vector for requisite segmentation point's cycles
				for(auto cycle : cycles_for_segmentation_point) {
					if(perm[cycle_ctr] == '1') {
						out_path += get_cycle_string(cycle, init);
					}
					cycle_ctr++;
				}

				// When no cycles are applied
				if(i == 0) { // perm = 0...0
					out_path += std::to_string(segmentation_point_id) + "-->";
				}
			}

			// Apending last segment
			out_path += get_string(segments[segments.size() - 1], true);

			// Printing the path with appropriate cycles
			cout << out_path.c_str() << "\n";
		}
	}

	return 0;
}

bool find_all_cycle_free_paths(ICFG* icfg, ICFGNode* source, ICFGNode* sink, 
	std::vector<std::vector<int>>& cycle_free_paths, 
	std::vector<ICFGNode*>& segmentation_points) {

	bool is_reachable, cycles_exist = false;
	std::stack<std::tuple<ICFGNode*, std::vector<int>, std::unordered_set<int>>> stack;
	stack.push(std::make_tuple(source, std::vector<int>{source->getId()}, std::unordered_set<int>{}));

	while (!stack.empty()) {
		auto [current, path, visited] = stack.top();
		stack.pop();

		// Sink found
		if (current->getId() == sink->getId()) {
			if(!is_reachable) {
				is_reachable = true;
				cout << "Reachable\n";
			}
			cycle_free_paths.push_back(path);
			continue;
		}

		visited.insert(current->getId());

		for (auto neighborEdge : current->getOutEdges()) {
			if (visited.find(neighborEdge->getDstID()) == visited.end()) { // Neighbor is not in visited
				std::vector<int> new_path = path;
				new_path.push_back(neighborEdge->getDstID());
				stack.push(std::make_tuple(neighborEdge->getDstNode(), new_path, visited));
			} else if (std::find(path.begin(), path.end(), neighborEdge->getDstID()) != path.end()) { // Neighbor is found in path
				cycles_exist = true;
			}
		}

		// Storing potential segmentation points
		if(current->getOutEdges().size() > 1) {
			segmentation_points.push_back(current);
		}
	}
	if(!is_reachable) {
		cout << "Unreachable";
	}
	return cycles_exist;
}
	
std::vector<std::vector<int>> find_cycles(ICFG* icfg, ICFGNode* segmentation_point, std::unordered_set<int>& visited) {

	std::vector<std::vector<int>> cycles;
	int init_visited_size = visited.size();
	std::stack<std::tuple<ICFGNode*, std::vector<int>, std::unordered_set<int>>> stack;
	stack.push(std::make_tuple(segmentation_point, std::vector<int>{segmentation_point->getId()}, visited));

	bool init = true;
	while (!stack.empty()) {
		auto [current, path, visited] = stack.top();
		stack.pop();
		
		// std::cout<< " current"<< current->getId()<<" ";
		// std::cout<< " segmentation_point"<<  segmentation_point->getId()<<"\n";

		// Sink (which is same as the segmentation point) found
		if (current->getId() == segmentation_point->getId() && visited.size() != init_visited_size) {
			//std::cout<< "going in" << " ";
			cycles.push_back(path);
			continue;
		}

		// Preventing the source (segmentation point) from being put into visited to promote re-exploration
		if(!init) {
			visited.insert(current->getId());
		}
		init = false;

		for (auto neighborEdge : current->getOutEdges()) {
			if (visited.find(neighborEdge->getDstID()) == visited.end()) { // Neighbor is not in visited
				std::vector<int> new_path = path;
				new_path.push_back(neighborEdge->getDstID());
				stack.push(std::make_tuple(neighborEdge->getDstNode(), new_path, visited));
			} 
		}
	}
	return cycles;
}

std::vector<std::string> get_permutations(int total_num_cycles) {
	std::vector<std::string> perms;
	for (int i = 0; i < (1 << total_num_cycles); ++i) {
        perms.push_back(std::bitset<8>(i).to_string().substr(8 - total_num_cycles));
    }
	return perms;
}

std::map<int, int> get_relevant_segmentation_points(std::vector<int>& cycle_free_path, 
	const std::vector<ICFGNode*>& segmentation_points) {
	
	std::vector<std::pair<int, int>> relevant; // key: node_id, value: index
	for(auto point : segmentation_points) {
		auto it = std::find(cycle_free_path.begin(), cycle_free_path.end(), point->getId());
		if(it != cycle_free_path.end()) {
			relevant.push_back(std::make_pair(point->getId(), std::distance(cycle_free_path.begin(), it)));
		}
	}

	// Sorting relevant segmentation points
	std::sort(relevant.begin(), relevant.end(), [=](std::pair<int, int>& a, std::pair<int, int>& b) {
			return a.second < b.second;
		}
	);

	// Storing the data into a map
	std::map<int, int> relevant_;
	for(auto [id, index] : relevant) {
		relevant_[id] = index;
	}
	return relevant_;
}

std::vector<std::vector<int>> segment_cycle_free_path(std::vector<int>& cycle_free_path, std::map<int, int>& relevant) {
	std::vector<std::vector<int>> segments;
	std::vector<int> segment;
	for(auto node_id : cycle_free_path) {
		if(relevant.find(node_id) == relevant.end()) { // Not a segmentation point
			segment.push_back(node_id);
		} else { // Is a segmentation point, so we've reached the end of this segment
			segments.push_back(segment);
			segment.clear();
		}
	}
	segments.push_back(segment); // Pushing back last segment
	return segments;
}

std::string get_string(std::vector<int>& path, bool final) {
	std::string out;
	for (int node : path) {
		out += std::to_string(node);
		if (node != path.back()) {
			out += "-->";
		}
	}
	if(!final) {
		out += "-->";
	}
	return out;
}

std::string get_cycle_string(std::vector<int>& cycle, bool init) {
	std::string out;
	if(!init) {
		out += "-->";
	}
	out = "Cycle[";
	for (int i = 0; i < cycle.size(); i++) {
		out += std::to_string(cycle[i]);
		if (i != cycle.size() - 1) {
			out += "-->";
		}
	}
	out += "]-->";
	return out;
}



/*
struct Cycle {
	std::vector<int> nums; // 6
	std::vector<Cycle> collapsed_cycles; // 1
	std::vector<int> index; // 0,1,0,0,0,0,0


	string build()
	{
		out_string = "";
		for(index) {
			if(index[i] == 0) {
				// out_string += nums[num_ctr++]
			} else {
				// out_string += collapsed_cycles[collapsed_cycle_ctr++].build()
			}
			return out;
		}
	}

}
*/