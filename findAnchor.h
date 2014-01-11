#ifndef __FINDANCHOR
#define __FINDANCHOR

#include <iostream>
#include <cmath>
#include <vector>
#include <Eigen/Dense>
#include "util.h"

using namespace std;
using namespace Eigen;

vector<int> findAnchors(MatrixXd Q, int K, struct Params *param, vector<int> candidates);

#endif
