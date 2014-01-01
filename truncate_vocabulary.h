#ifndef __TRUNCATE_VOCABULARY
#define __TRUNCATE_VOCABULARY
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <Eigen/Sparse>
using namespace std;
using namespace Eigen;

typedef Triplet<double> T;
SparseMatrix<double, RowMajor> read_and_truncate_vocabulary(string input_dataset, string full_vocab, int cutoff);


#endif
