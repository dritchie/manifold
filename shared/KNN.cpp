#include "KNN.h"
#include <algorithm>

using namespace std;
using namespace Eigen;

KNNBruteForce::KNNBruteForce(const Eigen::MatrixXd& points)
{
	this->points.resize(points.cols());
	for (int i = 0; i < points.cols(); i++)
		this->points[i] = points.col(i);
}

void KNNBruteForce::addPoint(const Eigen::MatrixXd& p)
{
	points.push_back(p);
}

void KNNBruteForce::clear()
{
	points.clear();
}

class NeighborWithDist
{
public:
	int index;
	double dist;
	NeighborWithDist() {}
	NeighborWithDist(int i, double d) : index(i), dist(d) {}
	inline bool operator < (const NeighborWithDist& other) const
	{
		return this->dist < other.dist;
	}
};

void KNNBruteForce::kNearest(int k, const Eigen::VectorXd& point, NeighborList& ns)
{
	vector<NeighborWithDist> distRecords(points.size());
	for (size_t i = 0; i < points.size(); i++)
	{
		double dist = (point - points[i]).squaredNorm();
		distRecords[i] = NeighborWithDist(i, dist);
	}
	partial_sort(distRecords.begin(), distRecords.begin()+k, distRecords.end());
	ns.resize(k);
	for (int i = 0; i < k; i++)
		ns[i] = distRecords[i].index;
}

void KNNBruteForce::kNearest(int k, int whichPoint, NeighborList& ns)
{
	const VectorXd& point = points[whichPoint];
	// Get k+1 neighbors and remove the first, which will be the point itself
	kNearest(k+1, point, ns);
	ns.erase(ns.begin());
}

void KNNBruteForce::buildGraph(int k, Graph& graph)
{
	int numNodes = (int)points.size();
	graph = Graph(numNodes);
	NeighborList ns;
	for (size_t i = 0; i < points.size(); i++)
	{
		kNearest(k, i, ns);
		graph.addNeighbors(i, ns);
	}
}




