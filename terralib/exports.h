#ifndef _MANIFOLD__H
#define _MANIFOLD__H

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__ ((visibility ("default")))
#endif

#define EXTERN extern "C" {
#ifdef __cplusplus
EXTERN
#endif

EXPORT double* LLE(int inDim, int outDim, int numPoints, int k, double* data);

// Returns the number of islands (i.e. connected components)
EXPORT int findIslands(int dim, int numPoints, int k, double* inData, int** outAssignments);

#ifdef __cplusplus
}
#endif

#endif