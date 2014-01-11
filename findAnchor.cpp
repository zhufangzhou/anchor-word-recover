/*
 * function: find the anchor words and return the index in the vocabulary file 
 * author: zhufangzhou
 * email: zhu.ark@gmail.com
 * date: 2014.1.10
 */
#include "findAnchor.h"
MatrixXd Random_Projection(MatrixXd M, int new_dim, int seed) {
	int old_dim = M.rows();
	// default random values is between -1 and 1
	MatrixXd R = MatrixXd::Random(1,new_dim*old_dim);
	R = (R+MatrixXd::Constant(1,new_dim*old_dim,1)) / 2;
	double p[3] = {1.0/6, 2.0/3, 1.0/6};
	double sqrt3 = sqrt(3);
	p[1] += p[0];
	p[2] += p[1];
	
	for(int i = 0; i < new_dim*old_dim; i++) {
		if(R(0,i) < p[0]) R(0,i) = sqrt3*(0-1);
		else if(R(0,i) < p[1]) R(0,i) = sqrt3*(1-1);
		else R(0,i) = sqrt3*(2-1);
	}
	R.resize(new_dim, old_dim);
	return R*M;
}

vector<int> Projection_Find(MatrixXd Q_orig, int K, vector<int> candidates) {
	int W = Q_orig.rows(), dim = Q_orig.cols();
	MatrixXd Q = Q_orig;

	// stored recovered anchor words
	vector<VectorXd> anchor_words(K);
	vector<int> anchor_indices(K);

	// store the basis vectors of the subspace spanned by the anchor word vectors
	vector<VectorXd> basis(K-1);

	// find the farthest point p1 from the origin
	double max_dist = 0, dist;
	int index;
	for(vector<int>::iterator iter = candidates.begin(); iter != candidates.end(); iter++) {
		index = *iter;
		dist = Q.row(index).dot(Q.row(index));	
		if(dist > max_dist) {
			max_dist = dist;
			anchor_words[0] = Q_orig.row(index);
			anchor_indices[0] = index;
		}
	}

	// let p1 be the origin of our coordinate system
	for(vector<int>::iterator iter = candidates.begin(); iter != candidates.end(); iter++) {
		index = *iter;
		Q.row(index) -= anchor_words[index];
	}
	
	// find the farthest point from p1
	max_dist = 0;
	for(vector<int>::iterator iter = candidates.begin(); iter != candidates.end(); iter++) {
		index = *iter;
		dist = Q.row(index).dot(Q.row(index));
		if(dist > max_dist) {
			max_dist = dist;
			anchor_words[1] = Q_orig.row(index);
			anchor_indices[1] = index;
			basis[0] = Q.row(index) / sqrt(dist);
		}
	}

	// stabilized gram-schmidt which find new anchor words to expand our subspace
	for(int j = 1; j < K-1; j++) {
		// project all the points on to our basis and find the farthest point
		max_dist = 0;
		for(vector<int>::iterator iter = candidates.begin(); iter != candidates.end(); iter++) {
			index = *iter;
			Q.row(index) = Q.row(index) - Q.row(index).dot(basis[j-1])*basis[j-1];
			dist = Q.row(index).dot(Q.row(index));
			if(dist > max_dist) {
				max_dist = dist;
				anchor_words[j+1] = Q_orig.row(index);
				anchor_indices[j+1] = index;
				basis[j] = Q.row(index) / sqrt(dist);
			}
		}
	}

	return anchor_indices;
}

vector<int> findAnchors(MatrixXd Q, int K, struct Params* param, vector<int> candidates) {
	string checkpoint_prefix = param->checkpoint_prefix;
	vector<int> anchor_indices;
	VectorXd row_sums;
	MatrixXd Q_red;
	int new_dim = param->new_dim;

	// row normalize Q
	row_sums = Q.rowwise().sum();	
	for(int i = 0; i < Q.rows(); i++) {
		Q.row(i) /= row_sums(i);
	}

	// reduced dimension random projection method for recovering anchor words
	Q_red = Random_Projection(Q.transpose(), new_dim, param->seed);
	Q_red.transposeInPlace();

	anchor_indices = Projection_Find(Q_red, K, candidates);

	// restore the original Q
	for(int i = 0; i < Q.rows(); i++) {
		Q.row(i) *= row_sums(i);
	}
	return anchor_indices;
}
