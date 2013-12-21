#ifndef _KNN__H
#define _KNN__H

#include <vector>
#include "Eigen/Core"
#include "Graph.h"

class KNNBruteForce
{
public:
	typedef std::vector<int> NeighborList;
	KNNBruteForce(const std::vector<Eigen::VectorXd>& points)
		: points(points) {}
	KNNBruteForce(const Eigen::MatrixXd& points);
	void kNearest(int k, const Eigen::VectorXd& point, NeighborList& ns);
	void kNearest(int k, int whichPoint, NeighborList& ns);
	void buildGraph(int k, Graph& graph);

	std::vector<Eigen::VectorXd> points;
};

#endif