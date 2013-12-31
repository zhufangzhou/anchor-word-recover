/*
 * function: preprocess the dataset before learn topics
 * author: zhufangzhou
 * email: zhu.ark@gmail.com
 * data: 2013.12.29
 */
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <Eigen/Sparse>

using namespace std;
using namespace Eigen; 
typedef Triplet<double> T;

int main(int argc, char* argv[]) {
	FILE *f_vocab, *f_stopwords, *f_dataset;
	map<string, int> table;
	string input_dataset, full_vocab, output_matrix, output_vocab;
	char word[100];
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
	f_vocab = fopen(full_vocab.c_str(), "r");
	numwords = 0;
	while(~fscanf(f_vocab, "%s", word)) {
		if(table.find(string(word)) == table.end()) {
			table.insert(pair<string, int>(string(word), numwords++));
		}
	}
	remove_words = new bool[numwords];
	memset(remove_words, false, sizeof(bool)*numwords);

	// Read in the stopwords
	f_stopwords = fopen("stopwords.txt", "r");	
	while(~fscanf(f_stopwords, "%s", word)) {
		if(table.find(string(word)) != table.end()) {
			remove_words[table[word]] = true;
		}
	}

	// Load the dataset and remove the stopwords
	f_dataset = fopen(input_dataset.c_str(), "r");
	fscanf(f_dataset, "%d%d%d", &ndoc, &nword, &nnz);
	if(nword != numwords) {
		cout << "Error: vocabulary file has different number of words" << nword << " " << numwords << endl;
		exit(1);
	}
	cout << "Number of words is " << nword << endl;
	cout << "Number of documents is " << ndoc << endl;

	// load the data, store schema is RowMajor, so jc[i+1]-jc[i] is the number of distinct documents that this word appears in 
	SparseMatrix<double, RowMajor> *wd_mat = new SparseMatrix<double, RowMajor>(nword, ndoc);
	vector<T> wd_mat_vec;
	int i, j, val;
	// dataset format: doc word count
	while(~fscanf(f_dataset, "%d%d%d", &i, &j, &val)) {
		wd_mat_vec.push_back(T(j-1, i-1, val));
	}
	wd_mat->reserve(nnz);						// set the nnz of the matrix
	wd_mat->setFromTriplets(wd_mat_vec.begin(), wd_mat_vec.end());
	wd_mat->makeCompressed();					// compress the matrix with csc
	int *ir, *jc, start, end;
	double *ptr;
	jc = wd_mat->outerIndexPtr();
	ir = wd_mat->innerIndexPtr(); 
	ptr = wd_mat->valuePtr();
	wd_mat_vec.clear();							// clear the vector to create another matrix

	int removed_word_count = 0;
	for(int i = 0; i < nword; i++) {
		if(!remove_words[i]) {
			start = jc[i];
			end = jc[i+1];
			// if number of distinct documents that this word appears in is >= cutoff
			if(end - start >= cutoff) {
				for(j = start; j < end; j++) {
					wd_mat_vec.push_back(T(i-removed_word_count, ir[j], ptr[j]));
				}
			} else {
				remove_words[i] = true;
				removed_word_count++;
			}
		} else {
			removed_word_count++;
		}
	}
	SparseMatrix<double, ColMajor> *wd_mat_new = new SparseMatrix<double, ColMajor>(nword-removed_word_count, ndoc);
	wd_mat_new->setFromTriplets(wd_mat_vec.begin(), wd_mat_vec.end());
	cout << wd_mat_new->rows() << endl;
	cout << wd_mat_new->cols() << endl;
	cout << wd_mat_new->nonZeros() << endl;

	delete[] remove_words;
	fclose(f_vocab);
	fclose(f_stopwords);
	fclose(f_dataset);
	return 0;
}
