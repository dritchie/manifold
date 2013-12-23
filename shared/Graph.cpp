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
	neighbors[node].insert(neighbor);
}

void Graph::addNeighbors(int node, const NodeList& neighbors)
{
	NodeSet& nset = this->neighbors[node];
	nset.insert(neighbors.begin(), neighbors.end());
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
		const NodeSet& ns = neighbors[node];
		for (NodeSet::const_iterator it = ns.begin(); it != ns.end(); it++)
		{
			int j = *it;
			if (!visited[j])
			{
				visited[j] = true;
				fringe.push(j);
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

void Graph::symmetrize()
{
	for (size_t i = 0; i < neighbors.size(); i++)
	{
		const NodeSet& ns = neighbors[i];
		for (NodeSet::const_iterator it = ns.begin(); it != ns.end(); it++)
		{
			int j = *it;
			neighbors[j].insert(i);
		}
	}
}

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



