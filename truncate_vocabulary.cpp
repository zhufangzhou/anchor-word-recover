#include <iostream>
#include <map>
#include <Eigen/Sparse>

using namespace std;
using namespace Eigen; 

int main(int argc, char* argv[]) {
	string input_dataset, full_vocab, output_matrix, output_vocab;
	int cutoff;
	if(argc < 3) {
		cout << "input: input_dataset vocab_file cutoff";
		exit(0);
	}
	// get the input_matrix name and the vocab_file name	
	input_dataset = string(argv[1]);
	full_vocab = string(argv[2]);
	output_matrix = input_dataset + ".trunc";
	output_vocab = full_vocab + ".trunc";
	
	// cutoff for the number of distinct documents that each word appears in
	cutoff = atoi(argv[3]);
	
	// Read in the vocabulary and build a symbol table mapping wors to indices	
	map<string, int> table;
	return 0;
}
