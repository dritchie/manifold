#ifndef _GRAPH__H
#define _GRAPH__H

#include <vector>

class Graph
{
public:
	typedef std::vector<int> NodeList;
	Graph(int numNodes);
	void addNeighbor(int node, int neighbor);
	void addNeighbors(int node, const NodeList& neighbors);

	// Traversal methods
	template <class FringeType> void Traverse(int startNode, NodeList& reached);
	void BFS(int startNode, NodeList& reached);
	void DFS(int startNode, NodeList& reached);

	// Only valid for undirected graphs.
	void connectedComponents(std::vector<NodeList>& comps);

	// // Make a graph undirected
	// void symmetrize()

	std::vector<NodeList> neighbors;
};


#endif