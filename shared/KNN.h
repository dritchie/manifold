#ifndef _KNN__H
#define _KNN__H

#include <vector>
#include "Eigen/Core"
#include "Graph.h"

class KNNBruteForce
{
public:
	typedef std::vector<int> NeighborList;
	KNNBruteForce() {}
	KNNBruteForce(const std::vector<Eigen::VectorXd>& points)
		: points(points) {}
	KNNBruteForce(const Eigen::MatrixXd& points);
	void addPoint(const Eigen::MatrixXd& p);
	void clear();

	void kNearest(int k, const Eigen::VectorXd& point, NeighborList& ns);
	void kNearest(int k, int whichPoint, NeighborList& ns);
	void buildGraph(int k, Graph& graph);

	void withinRadius(double r, const Eigen::VectorXd& point, NeighborList& ns);
	void withinRadius(double r, int whichPoint, NeighborList& ns);
	void buildGraph(double r, Graph& graph);

	std::vector<Eigen::VectorXd> points;
};

#endif