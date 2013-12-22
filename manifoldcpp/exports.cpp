#include "exports.h"
#include "lle.h"
#include "KNN.h"

using namespace std;
using namespace Eigen;

void cDataToMatrix(int dim, int numPoints, double* cdata, MatrixXd& mat)
{
	mat.resize(dim, numPoints);
	for (int i = 0; i < numPoints; i++)
	{
		double* dataP = cdata + i*dim;
		VectorXd v(dim);
		for (int d = 0; d < dim; d++)
			v[d] = dataP[d];
		mat.col(i) = v;
	}
}

double* matrixToCdata(MatrixXd& mat)
{
	int dim = mat.rows();
	int numPoints = mat.cols();
	double* returnData = (double*)malloc(dim*numPoints*sizeof(double));
	for (int i = 0; i < numPoints; i++)
	{
		const VectorXd& v = mat.col(i);
		double* returnDataP = returnData + i*dim;
		for (int d = 0; d < dim; d++)
			returnDataP[d] = v[d];
	}
	return returnData;
}

extern "C"
{
	EXPORT double* LLE(int inDim, int outDim, int numPoints, int k, double* data)
	{
		// Convert data to Eigen format
		MatrixXd inData;
		cDataToMatrix(inDim, numPoints, data, inData);

		// Run LLE
		MatrixXd outData;
		lle(inData, outData, outDim, k);

		// Convert data back to interchange format
		return matrixToCdata(outData);
	}

	EXPORT int findIslands(int dim, int numPoints, int k, double* inData, int** outAssignments)
	{
		MatrixXd data;
		cDataToMatrix(dim, numPoints, inData, data);

		KNNBruteForce knn(data);
		Graph g(numPoints);
		knn.buildGraph(k, g);
		vector<Graph::NodeList> comps;
		g.connectedComponents(comps);

		*outAssignments = (int*)malloc(numPoints*sizeof(int));
		for (size_t c = 0; c < comps.size(); c++)
		{
			const Graph::NodeList& nlist = comps[c];
			for (size_t i = 0; i < nlist.size(); i++)
				(*outAssignments)[nlist[i]] = c;
		}
		return (int)comps.size();
	}
}





