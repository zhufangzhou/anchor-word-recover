/*
 * author: zhufangzhou
 * email: zhu.ark@gmail.com
 * date: 2013.12.31
 */
#include "Q_matrix.h"

/*
 * function: generate W*W co-occurance matrix
 * input: W*D matrix(RowMajor) and number of words per documents(can omit)
 * output: co-occurance matrix Q (W*W)
 */
MatrixXd generate_Q_matrix(SparseMatrix<double, RowMajor> wd_mat_tmp, int words_per_doc){
	clock_t simulation_start = clock();	
	int vocabSize, numdocs;
	SparseMatrix<double, ColMajor> wd_mat(wd_mat_tmp.rows(), wd_mat_tmp.cols());
	// convert from csr to csc
	wd_mat = wd_mat_tmp;
	if(!wd_mat.isCompressed()) wd_mat.makeCompressed();
	vocabSize = wd_mat.rows();
	numdocs = wd_mat.cols();
	
	int *jc, *ir, start, end, wpd;
	double *ptr;

	MatrixXd diag_wd;
	VectorXd diag_wd_vec(vocabSize);
	
	jc = wd_mat.outerIndexPtr();
	ir = wd_mat.innerIndexPtr();
	ptr = wd_mat.valuePtr();
	for(int j = 0; j < numdocs; j++) {
		// start and end indices for column j
		start = jc[j];
		end = jc[j+1];
		wpd = 0;
		for(int i = start; i < end; i++) wpd += ptr[i];

		if(words_per_doc != -1 && words_per_doc != wpd) {
			cout << "Error: words per doc incorrect" << endl;
		}
		
		for(int i = start; i < end; i++) {
			diag_wd_vec[ir[i]] += ptr[i] / (wpd*(wpd-1));
			ptr[i] = ptr[i] / sqrt(1.0*wpd*(wpd-1));
		}
	}
	diag_wd = diag_wd_vec.asDiagonal();	

	MatrixXd Q(vocabSize, vocabSize);
	Q = wd_mat * wd_mat.transpose() / numdocs;
	diag_wd = diag_wd / numdocs;
	Q = Q - diag_wd;

	cout << "Sum of entries in Q is " << Q.sum() << endl; 
	cout << "Multiplying Q took " << (double)(clock() - simulation_start) / CLOCKS_PER_SEC << endl;
	return Q;
}
