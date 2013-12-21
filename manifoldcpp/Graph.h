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
	void BFS(int startNode, NodeList& reached);
	void connectedComponents(std::vector<NodeList>& comps);

	std::vector<NodeList> neighbors;
};


#endif