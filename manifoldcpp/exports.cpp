#include "exports.h"
#include "lle.h"

using namespace Eigen;

extern "C"
{
	EXPORT double* LLE(int inDim, int outDim, int numPoints, int k, double* data)
	{
		// Convert data to Eigen format
		MatrixXd inData(inDim, numPoints);
		for (int i = 0; i < numPoints; i++)
		{
			double* dataP = data + i*inDim;
			VectorXd v(inDim);
			for (int d = 0; d < inDim; d++)
				v[d] = dataP[d];
			inData.col(i) = v;
		}

		// Run LLE
		MatrixXd outData;
		lle(inData, outData, outDim, k);

		// Convert data back to interchange format
		// Important to use malloc, since the Terra side will free it
		double* returnData = (double*)malloc(outDim*numPoints*sizeof(double));
		for (int i = 0; i < numPoints; i++)
		{
			const VectorXd& v = outData.col(i);
			double* returnDataP = returnData + i*outDim;
			for (int d = 0; d < outDim; d++)
				returnDataP[d] = v[d];
		}
		return returnData;
	}
}