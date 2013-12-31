/*
 * function: learn topics
 * author: zhufangzhou
 * email: zhu.ark@gmail.com
 * date: 2013.12.31
 */
#include <iostream>
#include <string.h>
#include <string>
#include "util.h"
#include "truncate_vocabulary.h"

using namespace std;

int main(int argc, char* argv[]) {
	string infile, settings_file, vocab_file, loss, outfile;	
	int K, cutoff;
	SparseMatrix<double> *wd_mat;
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

	// set cutoff fixed with 50
	wd_mat = read_and_truncate_vocabulary(infile, vocab_file, cutoff);
	cout << wd_mat->rows() << endl;
	cout << wd_mat->cols() << endl;
	cout << wd_mat->nonZeros() << endl;

	cout << "identifying candidate anchors" << endl;
	
	// only accept anchors that appear in a significant number of docs
	return 0;
}
