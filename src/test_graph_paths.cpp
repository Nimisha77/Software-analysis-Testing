#include <iostream>
#include <vector>
#include <map>

#include "graph_paths.cpp"

int main()
{
    // Test Case 1: Simple graph with a direct path from src to sink
    std::map<int, std::vector<int>> graph1 = {
        {0, {1, 2}},
        {1, {3}},
        {2, {3}},
        {3, {}}};
    // 0 1 3
    // 0 2 3
    std::cout << "### Test Case 1 ###\n";
    printPaths(graph1, 0, 3);
    std::cout << "\n";

    // Test Case 2: Graph with multiple paths from src to sink
    std::map<int, std::vector<int>> graph2 = {
        {0, {1, 2}},
        {1, {3}},
        {2, {3, 4}},
        {3, {5}},
        {4, {5}},
        {5, {}}};
    // 0 1 3 5
    // 0 2 3 5
    // 0 2 4 5
    std::cout << "### Test Case 2 ###\n";
    printPaths(graph2, 0, 5);
    std::cout << "\n";

    // Test Case 3: Graph with a cycle
    std::map<int, std::vector<int>> graph3 = {
        {0, {1, 2}},
        {1, {3}},
        {2, {3, 4}},
        {3, {5}},
        {4, {5}},
        {5, {2}} // Cycle: 5 2 4 5
    };
    // 0 1 3 5
    // 0 2 3 5
    // 0 2 4 5
    std::cout << "### Test Case 3 ###\n";
    printPaths(graph3, 0, 5);
    std::cout << "\n";

    // Test Case 4: Graph with no path from src to sink
    std::map<int, std::vector<int>> graph4 = {
        {0, {1}},
        {1, {2}},
        {2, {3}},
        {3, {}}};
    // None
    std::cout << "### Test Case 4 ###\n";
    printPaths(graph4, 0, 4);
    std::cout << "\n";

    // Test Case 5: Graph with disconnected components
    std::map<int, std::vector<int>> graph5 = {
        {0, {1}},
        {1, {2}},
        {2, {3}},
        {3, {}},
        {4, {5}},
        {5, {6}},
        {6, {}}};
    // None
    std::cout << "### Test Case 5 ###\n";
    printPaths(graph5, 0, 6);
    std::cout << "\n";

    // Test Case 6: Graph with self-loop
    std::map<int, std::vector<int>> graph6 = {
        {0, {1}},
        {1, {1}} // Self-loop: 1 1
    };
    // 0 1
    std::cout << "### Test Case 6 ###\n";
    printPaths(graph6, 0, 1);
    std::cout << "\n";

    // Test Case 7: Empty graph
    std::map<int, std::vector<int>> graph7;
    // None
    std::cout << "### Test Case 7 ###\n";
    printPaths(graph7, 0, 1);
    std::cout << "\n";

    // Test Case 8: Graph with single node
    std::map<int, std::vector<int>> graph8 = {
        {0, {}}};
    // 0
    std::cout << "### Test Case 8 ###\n";
    printPaths(graph8, 0, 0);
    std::cout << "\n";

    // Test Case 9: Random graph
    std::map<int, std::vector<int>> graph9 = {
        {0, {1, 2}},
        {1, {3}},
        {2, {3, 4}},
        {3, {5}},
        {4, {5}},
        {5, {6}},
        {6, {7}},
        {7, {8}},
        {8, {9}},
        {9, {}}};
    // 0 2 4 5 6 7 8 9
    std::cout << "### Test Case 10 ###\n";
    printPaths(graph9, 0, 9);

    return 0;
}
