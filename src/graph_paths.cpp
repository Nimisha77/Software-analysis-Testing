#include <iostream>
#include <vector>
#include <map>

void printPathsDFS(const std::map<int, std::vector<int>> &graph, int src, int sink, std::vector<int> &path)
{
    // Add the current node to the path
    path.push_back(src);

    // If the current node is the sink, print the path
    if (src == sink)
    {
        for (int node : path)
        {
            std::cout << node << " ";
        }
        std::cout << std::endl;
    }
    else
    {
        // Recursively visit neighbors
        for (int neighbor : graph.at(src))
        {
            printPathsDFS(graph, neighbor, sink, path);
        }
    }

    // Remove the current node from the path to backtrack
    path.pop_back();
}

void printPaths(const std::map<int, std::vector<int>> &graph, int src, int sink)
{
    // Check if the source and sink nodes exist in the graph
    if (graph.find(src) == graph.end() || graph.find(sink) == graph.end())
    {
        std::cerr << "Source or sink node not found in the graph." << std::endl;
        return;
    }

    std::vector<int> path;
    std::cout << "Paths from " << src << " to " << sink << ":" << std::endl;
    printPathsDFS(graph, src, sink, path);
}
