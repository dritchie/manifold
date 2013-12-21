#include "Graph.h"
#include <queue>

using namespace std;

Graph::Graph(int numNodes)
{
	neighbors.resize(numNodes);
}

void Graph::addNeighbor(int node, int neighbor)
{
	neighbors[node].push_back(neighbor);
}

void Graph::addNeighbors(int node, const NodeList& neighbors)
{
	NodeList& nlist = this->neighbors[node];
	nlist.insert(nlist.end(), neighbors.begin(), neighbors.end());
}

void Graph::BFS(int startNode, NodeList& reached)
{
	vector<bool> visited(neighbors.size(), false);
	queue<int> fringe;
	fringe.push(startNode);
	while (!fringe.empty())
	{
		int node = fringe.front();
		fringe.pop();
		visited[node] = true;
		reached.push_back(node);
		const NodeList& ns = neighbors[node];
		for (size_t i = 0; i < ns.size(); i++)
		{
			if (!visited[ns[i]])
				fringe.push(ns[i]);
		}
	}
}

void Graph::connectedComponents(std::vector<NodeList>& comps)
{
	vector<bool> visited(neighbors.size(), false);
	NodeList ns;
	for (size_t i = 0; i < neighbors.size(); i++)
	{
		if (!visited[i])
		{
			BFS(i, ns);
			for (size_t j = 0; j < ns.size(); j++)
				visited[ns[j]] = true;
			comps.push_back(ns);
		}
	}
}



