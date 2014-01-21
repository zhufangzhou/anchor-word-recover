#include "fastRecover.h"

double logsum_exp(RowVectorXd y) {
	double m = y.maxCoeff();
	return log((y.array()-m).exp().sum()) + m;
}

double KL(RowVectorXd p, RowVectorXd log_p, RowVectorXd q) {
	int N = p.cols();
	double ret = 0;
	RowVectorXd log_diff = log_p.array() - q.array().log();
	ret = p.dot(log_diff);
	return ret;
}

RowVectorXd KLSolveExpGrad(VectorXd oldrow, MatrixXd oldX, double eps) {
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
	
	// in the following comments, I define mask.size()==M for convenience

	RowVectorXd row(mask.size()); // 1*M
	MatrixXd X(K, mask.size()); // K*M

	for(int i = 0; i < mask.size(); i++) {
		row(i) = oldrow(mask[i]);
		X.col(i) = oldX.col(mask[i]);
	}
	V = mask.size(); // new V ---- the size of mask vector
	// normalize row to 1
	VectorXd  row_sums = X.rowwise().sum();
	for(int i = 0; i < K; i++) X.row(i) /= row_sums(i);

	
	RowVectorXd alpha = RowVectorXd::Ones(K) / K; // 1*K
	RowVectorXd old_alpha = alpha; // 1*K
	RowVectorXd log_alpha = alpha.array().log(); // 1*K
	RowVectorXd old_log_alpha = log_alpha; // 1*K
	
	RowVectorXd proj = alpha * X; // 1*M
	RowVectorXd old_proj = proj; // 1*M	
	
	RowVectorXd log_row = row.array().log(); // 1*M
	double new_obj = KL(row, log_row, proj);
	RowVectorXd row_over_proj = row.array() / proj.array();	// 1*M

	VectorXd grad = -(X * row_over_proj.transpose());
	VectorXd lam;
	VectorXd old_grad = grad;

	double eta, stepsize = 1, gap = DBL_MAX, grad_dot_deltaAlpha, convergence;
	bool old_obj, decreasing = false, repeat = false;
	
	while(true) {
		eta = stepsize;
		old_obj = new_obj;
		old_alpha = alpha;
		old_log_alpha = log_alpha;

		old_proj = proj;
		
		it++;
		// take a step
		log_alpha -= eta*grad;

		// normalize
		log_alpha = log_alpha.array() - logsum_exp(log_alpha);

		// compute new objective
		alpha = log_alpha.array().exp();
		proj = alpha*X;
		new_obj = KL(row, log_row, proj);
		if(new_obj < eps) break;

		grad_dot_deltaAlpha = grad.dot(alpha - old_alpha);
		if(!(new_obj <= old_obj + c1*stepsize*grad_dot_deltaAlpha)) { // sufficient decrease
			stepsize /= 2; // reudce stepsize
			if(stepsize < 1e-6) break;
			
			alpha = old_alpha;
			log_alpha = old_log_alpha;
			proj = old_proj;
			new_obj = old_obj;
			repeat = true;
			decreasing = true;
			continue;
		}

		// compute the new gradient
		old_grad = grad;
		row_over_proj = row.array() / proj.array();
		grad = -(X * row_over_proj.transpose());

		if(!(grad.dot(alpha-old_alpha) >= c2*grad_dot_deltaAlpha) && !(decreasing)) { // curvature
			stepsize *= 2.0; // increase stepsize
			alpha = old_alpha;
			log_alpha = old_log_alpha;
			grad = old_grad;
			proj = old_proj;
			new_obj = old_obj;
			repeat = true;
			continue;
		}

		decreasing = false;
		lam = grad;
		lam = lam.array() - lam.minCoeff();

		gap = alpha.dot(lam.transpose());
		convergence = gap;
		if(convergence < eps) break;
	}
	return alpha;
}

RowVectorXd fastRecover(VectorXd row, MatrixXd X, int v, string outfile_name, vector<int> anchors, string divergence, MatrixXd XXT, int initial_stepsize, double eps) {
	clock_t start_time = clock();	
	int K = anchors.size();
	RowVectorXd alpha = RowVectorXd::Zero(K);
	vector<int>::iterator iter;
	double gap, it, dist, stepsize;

	if((iter = find(anchors.begin(), anchors.end(), v)) != anchors.end()) {
		alpha(*iter) = 1;
		it = -1;
		dist = 0;
		stepsize = 0;
	} else {
		if(divergence == "KL") {
			alpha = KLSolveExpGrad(row, X, eps);
		} else if(divergence == "L2") {

		} else if(divergence == "fastL2") {

		} else {
			cout << "invalid divergence!" << endl;
		}
	}

}


// takes a writeable file recoveryLog to log performance
// comment out the recovery log if don't want it
MatrixXd nonNegativeRecover(MatrixXd Q, vector<int> anchors, string outfile_name, string divergence, int max_threads, double eps) {
	FILE *f_topic_likelihoodLog, *f_word_likelihoodLog, *f_alphaLog;
	int V, K;
	MatrixXd P_w;

	//f_topic_likelihoodLog = fopen((outfile_name+".word_likelihoods").c_str(), "w");
	//f_word_likelihoodLog = fopen((outfile_name+".word_likelihoods").c_str(), "w");
	//f_alphaLog = fopen((outfile_name+".alpha").c_str(), "w");

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
	RowVectorXd alpha;
	MatrixXd A(V, K);
	for(int w = 0; w < V; w++) {
		row = Q.row(w);
		alpha = fastRecover(row, X, w, outfile_name+".recoveryLog", anchors, divergence, XXT, 1, eps); // 1 is inital_stepsize
		A.row(w) = alpha;
	}

	// rescale A matrix
	// Bayes rule says P(w|z) proportional to P(z|w)P(w)
	
	A = P_w * A;

	// normalize columns of A. This is the normalization constant P(z)
	VectorXd col_sums = A.colwise().sum();

	for(int i = 0; i < K; i++) {
		A.col(i) /= col_sums(i);
	}
	return A;
}

void do_recovery(MatrixXd Q, vector<int> anchors, string loss, struct Params *param) {
	MatrixXd A;
	if(loss == "originalRecover") {
		return ;
	} else if(loss == "KL" || loss == "L2") {
		A = nonNegativeRecover(Q, anchors, param->log_prefix, loss, param->max_threads, param->eps);
	} else {
		cout << "unrecognized loss function " << loss << ". Options are KL, L2 or orginalRecover." << endl;
	}
}
