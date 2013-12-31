#include <iostream>
#include <string>
#include <Eigen/Sparse>
#include "util.h"

using namespace std;
using namespace Eigen;

int main(int argc, char* argv[]) {
	string infile, settings_file, vocab_file, loss, outfile;	
	int K;

	// parse input args
	if(argc > 6) {
		infile = string(argv[1]);
		settings_file = string(argv[2]);
		vocab_file = string(argv[3]);
		K = atoi(argv[4]);
		loss = string(argv[5]);
		outfile = string(argv[6]);
	} else {
		cout << "usage: ./learn_topics dataset settings_file vocab_file K loss output_filename" << endl;
		cout << "for more infomation see readme.txt" << endl;
		exit(1);
	}
	Params *param = new Params(settings_file);
	param->display_settings();	

	return 0;
}
