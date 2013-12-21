#include "lle.h"
#include "stddef.h"
#include <vector>
#include <cstdlib>
#include <iostream>

// Unfortunately, Eigen *still* does not provide sparse eigenvalue solvers...
//    so we're doing everything dense for now.

#include "Eigen/Dense"
// #include "Eigen/SparseCore"
#include "Eigen/Eigenvalues"

#include "KNN.h"

using namespace std;
using namespace Eigen;

typedef unsigned int UINT;
typedef vector< vector<int> > NeighborList;
// typedef SparseMatrix<double> SparseMatrixd;
// typedef Triplet<double> Tripletd;


void computeNeighbors(int k, const MatrixXd& inData, NeighborList& outNeighbors)
{
	// For now, just brute force KNN
	KNNBruteForce knn(inData);
	vector<int> ns;
	for (UINT i = 0; i < inData.cols(); i++)
	{
		knn.kNearest(k, i, ns);
		outNeighbors.push_back(ns);
	}
}

void computeReconstructionWeights(const MatrixXd& inData, const NeighborList& inNeighbors, MatrixXd& outW)
{
	int dim = inData.col(0).size();
	int k = inNeighbors[0].size();
	// vector<Tripletd> sparseWeights;
	outW = MatrixXd::Zero(outW.rows(), outW.cols());
	for (UINT i = 0; i < inData.cols(); i++)
	{
		MatrixXd Z(dim,k);
		const VectorXd& Xi = inData.col(i);
		for (UINT j = 0; j < k; j++)
		{
			const VectorXd& Xnj = inData.col(inNeighbors[i][j]);
			Z.col(j) = Xnj - Xi;
		}
		MatrixXd C = Z.transpose() * Z;
		// Have to regularize if num neighbors is greater than the space dimension
		if (k > dim)
		{
			const double scale = 1e-3;
			// const double scale = 1e-9;
			double eps = scale*C.trace();
			C += MatrixXd::Identity(k, k) * eps;
		}
		VectorXd w = C.ldlt().solve(VectorXd::Ones(k));
		double wsum = w.sum();
		for (UINT j = 0; j < k; j++)
			outW(i,inNeighbors[i][j]) = w[j]/wsum;
		// 	sparseWeights.push_back(Tripletd(i, inNeighbors[i][j], w[j]/wsum));
	}
	// outW.setFromTriplets(sparseWeights.begin(), sparseWeights.end());
}

void computeEmbeddingCoords(int outDim, const MatrixXd& inW, MatrixXd& outData)
{
	// SparseMatrixd I(inW.rows(), inW.cols());
	// vector<Tripletd> identTriplets;
	// for (UINT i = 0; i < inW.rows(); i++)
	// 	identTriplets.push_back(Tripletd(i, i, 1.0));
	// I.setFromTriplets(identTriplets.begin(), identTriplets.end());
	// SparseMatrixd IminusW = I - inW;
	// SparseMatrixd M = (IminusW.transpose() * IminusW).pruned();

	MatrixXd IminusW = MatrixXd::Identity(inW.rows(), inW.cols()) - inW;
	MatrixXd M = IminusW.transpose()*IminusW;
	SelfAdjointEigenSolver<MatrixXd> eigensolver(M);
	MatrixXd eigenvectors = eigensolver.eigenvectors();
	for (UINT i = 1; i < outDim+1; i++)
		outData.row(i-1) = eigenvectors.col(i);
}

void lle(const MatrixXd& inData, MatrixXd& outData, int outDim, int k)
{
	int numPoints = inData.cols();
	NeighborList Ns;
	MatrixXd W(numPoints, numPoints);
	outData.resize(outDim, numPoints);
	computeNeighbors(k, inData, Ns);
	computeReconstructionWeights(inData, Ns, W);
	computeEmbeddingCoords(outDim, W, outData);
}





