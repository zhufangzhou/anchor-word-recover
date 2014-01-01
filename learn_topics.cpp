/*
 * function: learn topics
 * author: zhufangzhou
 * email: zhu.ark@gmail.com
 * date: 2013.12.31
 */
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include "util.h"
#include "truncate_vocabulary.h"
#include "Q_matrix.h"

using namespace std;

int main(int argc, char* argv[]) {
	string infile, settings_file, vocab_file, loss, outfile;	
	int K, cutoff;
	int *jc, *ir, start, end, vocabSize;
	SparseMatrix<double, RowMajor> wd_mat;
	// parse input args
	if(argc > 7) {
		infile = string(argv[1]);
		settings_file = string(argv[2]);
		vocab_file = string(argv[3]);
		cutoff = atoi(argv[4]);
		K = atoi(argv[5]);
		loss = string(argv[6]);
		outfile = string(argv[7]);
	} else {
		cout << "usage: ./learn_topics dataset settings_file vocab_file cutoff K loss output_filename" << endl;
		cout << "for more infomation see readme.txt" << endl;
		exit(1);
	}
	Params *param = new Params(settings_file);

	// read in the data and set cutoff fixed with 50
	wd_mat = read_and_truncate_vocabulary(infile, vocab_file, cutoff);
	wd_mat.makeCompressed();	

	jc = wd_mat.outerIndexPtr();
	ir = wd_mat.innerIndexPtr();
	vocabSize = wd_mat.rows();
	cout << "identifying candidate anchors" << endl;
	vector<int> candidate_anchor;
	// only accept anchors that appear in a significant number of docs
	for(int i = 0; i < vocabSize; i++) {
		start = jc[i];
		end = jc[i+1];
		if((end-start) > param->anchor_thresh) {
			candidate_anchor.push_back(i);
		}
	}
	cout << candidate_anchor.size() << " candidates" << endl;
	// form Q matrix from document-word matrix
	MatrixXd Q;
	Q = generate_Q_matrix(wd_mat); 
	cout << "Q matrix:" << endl;
	cout << "rows:" << Q.rows() << endl;
	cout << "cols:" << Q.cols() << endl;
	cout << "Sum of entries in Q is " << Q.sum() << endl;
	return 0;
}
