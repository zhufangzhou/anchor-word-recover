#include "fastRecover.h"

void KLSolveExpGrad(VectorXd oldrow, MatrixXd oldX, double eps) {
	clock_t start_time = clock();
	double c1 = 1e-4, c2 = 0.9, it = 1;
	int K = oldX.rows(), V = oldX.cols();
	vector<int> mask;
	
	for(int i = 0; i < V; i++) {
		if(oldrow(i) <= 0) {
			oldrow(i) = 0;
		} else {
			mask.push_back(i);
			if(oldrow(i) > 1) oldrow(i) = 1;
		}
	}

	for(int i = 0; i < K; i++) {
		for(int j = 0; j < V; j++) {
			if(oldX(i,j) < 0) oldX(i, j) = 0;
			else if(oldX(i, j) > 1) oldX(i, j) = 1;

			oldX(i, j) += 1e-9;
		}
	}
	
	VectorXd row(mask.size());
	MatrixXd X(K, mask.size());

	for(int i = 0; i < mask.size(); i++) {
		row(i) = oldrow(mask[i]);
		X.col(i) = oldX.col(mask[i]);
	}
	V = mask.size(); // new V ---- the size of mask vector
	// normalize row to 1
	VectorXd  row_sums = X.rowwise().sum();
	for(int i = 0; i < K; i++) X.row(i) /= row_sums(i);
	VectorXd alpha = VectorXd::Ones(K) / K;
		
}

void fastRecover(VectorXd row, MatrixXd X, int v, string outfile_name, vector<int> anchors, string divergence, MatrixXd XXT, int initial_stepsize, double eps) {
	clock_t start_time = clock();	
	int K = anchors.size();
	VectorXd alpha = VectorXd::Zero(K);
	vector<int>::iterator iter;
	double gap, it, dist, stepsize;

	if((iter = find(anchors.begin(), anchors.end(), v)) != anchors.end()) {
		alpha(*iter) = 1;
		it = -1;
		dist = 0;
		stepsize = 0;
	} else {
		if(divergence == "KL") {
			KLSolveExpGrad(row, X, eps);
		} else if(divergence == "L2") {

		} else if(divergence == "fastL2") {

		} else {
			cout << "invalid divergence!" << endl;
		}
	}

}


// takes a writeable file recoveryLog to log performance
// comment out the recovery log if don't want it
void nonNegativeRecover(MatrixXd Q, vector<int> anchors, string outfile_name, string divergence, int max_threads, double eps) {
	FILE *f_topic_likelihoodLog, *f_word_likelihoodLog, *f_alphaLog;
	int V, K;
	MatrixXd P_w;

	f_topic_likelihoodLog = fopen((outfile_name+".word_likelihoods").c_str(), "w");
	f_word_likelihoodLog = fopen((outfile_name+".word_likelihoods").c_str(), "w");
	f_alphaLog = fopen((outfile_name+".alpha").c_str(), "w");

	V = Q.rows();
	K = anchors.size();	
	
	VectorXd row_sums = Q.rowwise().sum();
	P_w = row_sums.asDiagonal();
	for(int i; i < V; i++) {
		if(P_w(i, i) == 0) {
			P_w(i, i) == 1e-16;
		}
	}
	
	// normalize the rows of Q_prime
	for(int i = 0; i < V; i++) {
		Q.row(i) /= row_sums(i);
	}

	clock_t s = clock();

	// no multithread
	MatrixXd X(K,V), XXT;
	for(int i = 0; i < K; i++) {
		X.row(i) = Q.row(anchors[i]);
	}
	XXT = X * X.transpose();
	VectorXd row;
	for(int w = 0; w < V; w++) {
		row = Q.row(w);
		fastRecover(row, X, w, outfile_name+".recoveryLog", anchors, divergence, XXT, 1, eps); // 1 is inital_stepsize
	}
}

void do_recovery(MatrixXd Q, vector<int> anchors, string loss, struct Params *param) {
	if(loss == "originalRecover") {
		return ;
	} else if(loss == "KL" || loss == "L2") {
		nonNegativeRecover(Q, anchors, param->log_prefix, loss, param->max_threads, param->eps);
	} else {
		cout << "unrecognized loss function " << loss << ". Options are KL, L2 or orginalRecover." << endl;
	}
}
