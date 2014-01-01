#ifndef __Q_MATRIX
#define __Q_MATRIX

#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Sparse>

using namespace std;
using namespace Eigen;

MatrixXd generate_Q_matrix(SparseMatrix<double, RowMajor> wd_mat, int words_per_doc = -1);

#endif
