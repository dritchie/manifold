#include "Graph.h"
#include <queue>
#include <stack>

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

template<class T>
T first(const stack<T>& s) { return s.top(); }

template<class T>
T first(const queue<T>& s) { return s.front(); }

template <class FringeType>
void Graph::Traverse(int startNode, NodeList& reached)
{
	vector<bool> visited(neighbors.size(), false);
	FringeType fringe;
	fringe.push(startNode);
	visited[startNode] = true;
	while (!fringe.empty())
	{
		int node = first(fringe);
		fringe.pop();
		reached.push_back(node);
		const NodeList& ns = neighbors[node];
		for (size_t i = 0; i < ns.size(); i++)
		{
			if (!visited[ns[i]])
			{
				visited[ns[i]] = true;
				fringe.push(ns[i]);
			}
		}
	}
}

void Graph::BFS(int startNode, NodeList& reached)
{
	Traverse< queue<int> >(startNode, reached);
}

void Graph::DFS(int startNode, NodeList& reached)
{
	Traverse< stack<int> >(startNode, reached);
}

// void Graph::symmetrize()
// {
// 	for (size_t i = 0; i < neighbors.size(); i++)
// 	{
// 		const NodeList& ns = neighbors[i];
// 		for (size_t ni = 0; ni < ns.size(); ni++)
// 		{
// 			int j = ns[ni];
// 			// neighbors[j]
// 		}
// 	}
// }

void Graph::connectedComponents(std::vector<NodeList>& comps)
{
	vector<bool> visited(neighbors.size(), false);
	for (size_t i = 0; i < neighbors.size(); i++)
	{
		if (!visited[i])
		{
			NodeList ns;
			BFS(i, ns);
			for (size_t j = 0; j < ns.size(); j++)
				visited[ns[j]] = true;
			comps.push_back(ns);
		}
	}
}



