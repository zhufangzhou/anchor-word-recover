#include "Q_matrix.h"

MatrixXd* generate_Q_matrix(SparseMatrix<double, RowMajor> *wd_mat_tmp, int words_per_doc){
	clock_t simulation_start = clock();	
	int vocabSize, numdocs;
	SparseMatrix<double, ColMajor> *wd_mat = new SparseMatrix<double, ColMajor>(wd_mat_tmp->rows(), wd_mat_tmp->cols());
	// convert from csr to csc
	*wd_mat = *wd_mat_tmp;
	if(!wd_mat->isCompressed()) wd_mat->makeCompressed();
	vocabSize = wd_mat->rows();
	numdocs = wd_mat->cols();
	
	int *jc, *ir, start, end, wpd;
	double *val;

	MatrixXd diag_wd = MatrixXd::Random(vocabSize, vocabSize);
	for(int j = 0; j < vocabSize; j++) {
		start = jc[j];
		end = jc[j+1];
		wpd = 0;
		for(int i = start; i < end; i++) wpd += val[i];

		if(words_per_doc != -1 && words_per_doc != wpd) {
			cout << "Error: words per doc incorrect" << endl;
		}
		
		for(int i = start; i < end; i++) {
			diag_wd(ir[i], ir[i]) += val[i] / (wpd*(wpd-1));
			val[i] = val[i] / sqrt(1.0*wpd*(wpd-1));
		}
	}
	
	// !!!!!!!!!!
	return 0;
}
