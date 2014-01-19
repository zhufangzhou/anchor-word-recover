#include "truncate_vocabulary.h"

/*
 * function: preprocess the dataset before learn topics
 * input: dataset path, vocabulary file path, cutoff
 * output: W*D matrix(word-document matrix)
 */
SparseMatrix<double, RowMajor> read_and_truncate_vocabulary(string input_dataset, string full_vocab, int cutoff, vector<string> &vocab_vec) {
	FILE *f_vocab, *f_stopwords, *f_dataset;
	map<string, int> table;
	string output_matrix, output_vocab;
	char word[100];
	bool *remove_words;
	int numwords, nword, ndoc, nnz;
	vector<string> vocab_full;

	output_matrix = input_dataset + ".trunc";
	output_vocab = full_vocab + ".trunc";
	
	
	// Read in the vocabulary and build a symbol table mapping wors to indices	
	f_vocab = fopen(full_vocab.c_str(), "r");
	numwords = 0;
	while(~fscanf(f_vocab, "%s", word)) {
		if(table.find(string(word)) == table.end()) {
			table.insert(pair<string, int>(string(word), numwords++));
			vocab_full.push_back(string(word));
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
	SparseMatrix<double, RowMajor> wd_mat(nword, ndoc);
	vector<T> wd_mat_vec;
	int i, j, val;
	// dataset format: doc word count
	while(~fscanf(f_dataset, "%d%d%d", &i, &j, &val)) {
		wd_mat_vec.push_back(T(j-1, i-1, val));
	}
	wd_mat.reserve(nnz);						// set the nnz of the matrix
	wd_mat.setFromTriplets(wd_mat_vec.begin(), wd_mat_vec.end());
	wd_mat.makeCompressed();					// compress the matrix with csc
	int *ir, *jc, start, end;
	double *ptr;
	jc = wd_mat.outerIndexPtr();
	ir = wd_mat.innerIndexPtr(); 
	ptr = wd_mat.valuePtr();
	wd_mat_vec.clear();							// clear the vector to create another matrix

	int removed_word_count = 0;
	for(int i = 0; i < nword; i++) {
		if(!remove_words[i]) {
			start = jc[i];
			end = jc[i+1];
			// if number of distinct documents that this word appears in is >= cutoff
			if((end - start) >= cutoff) {
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
	SparseMatrix<double, RowMajor> wd_mat_new(nword-removed_word_count, ndoc);
	wd_mat_new.setFromTriplets(wd_mat_vec.begin(), wd_mat_vec.end());

	for(int i = 0; i < numwords; i++) {
		if(!remove_words[i]) {
			vocab_vec.push_back(vocab_full[i]);
		}
	}
	
	fclose(f_vocab);
	fclose(f_stopwords);
	fclose(f_dataset);
	return wd_mat_new;
}
