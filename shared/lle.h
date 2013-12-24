#ifndef _LLE__H
#define _LLE__H

#include "Eigen/Core"

void lle(const Eigen::MatrixXd& inData, Eigen::MatrixXd& outData, int outDim, int k);
void lle(const Eigen::MatrixXd& inData, Eigen::MatrixXd& outData, int outDim, double r);

#endif