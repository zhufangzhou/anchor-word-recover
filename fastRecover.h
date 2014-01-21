#ifndef __FASTRECOVERY
#define __FASTRECOVERY
#include <iostream>
#include <Eigen/Dense>
#include "util.h"
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <float.h>

using namespace std;
using namespace Eigen;



void do_recovery(MatrixXd Q, vector<int> anchors, string loss, struct Params *param);
#endif
