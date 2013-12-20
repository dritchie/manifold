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

#ifdef __cplusplus
}
#endif

#endif