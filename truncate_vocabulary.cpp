/*
 *	function: preprocess the dataset before learn topics
 *	author: zhufangzhou
 *	email: zhu.ark@gmail.com
 *	data: 2013.12.29
 */
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <Eigen/Sparse>

using namespace std;
using namespace Eigen; 

int main(int argc, char* argv[]) {
	ifstream f_vocab, f_stopwords, f_dataset;
	map<string, int> table;
	string input_dataset, full_vocab, output_matrix, output_vocab, word;
	bool *remove_words;
	int cutoff, numwords, nword, ndoc, nnz;

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
	f_vocab.open(full_vocab.c_str());
	numwords = 0;
	while(f_vocab >> word) {
		if(table.find(word) == table.end()) {
			table.insert(pair<string, int>(word, numwords++));
		}
	}
	remove_words = new bool[numwords];
	memset(remove_words, false, sizeof(bool)*numwords);
	f_vocab.close();

	// Read in the stopwords
	f_stopwords.open("stopwords.txt");	
	while(f_stopwords >> word) {
		if(table.find(word) != table.end()) {
			remove_words[table[word.c_str()]] = true;
		}
	}

	// Load the dataset and remove the stopwords
	f_dataset.open(input_dataset.c_str());
	f_dataset >> ndoc >> nword >> nnz;	
	if(nword != numwords) {
		cout << "Error: vocabulary file has different number of words" + nword + " " + numwords << endl;
		exit(1);
	}
	cout << "Number of words is " + nword << endl;
	cout << "Number of documents is " + ndoc << endl;

	// load the data
	SparseMatrix<double, ColMajor> *wd_mat = new SparseMatrix<double>(ndoc, nword);
	wd_mat.reserve(nnz);
	int *rowSum = new int[nword];
	int i, j, val;
	memset(rowSum, 0, sizeof(int)*nword);
	// dataset format: doc word count
	while(f_dataset >> i >> j >> val) {
		wd_mat->insert(j, i) = (double)val;
		rowSum[j]++;
	}
	wd_mat.makeCompressed();		// compress the matrix with csc
		

	delete[] rowSum;
	delete wd_mat;
	return 0;
}
