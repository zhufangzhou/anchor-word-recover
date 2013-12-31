#ifndef __MYUTIL
#define __MYUTIL

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

void trim(string & str ) {
	int s = str.find_first_not_of(" ");
	int e = str.find_last_not_of(" ");
	str = str.substr(s,e-s+1);
	return;
}

class Params {
	public:
		string log_prefix;
		string checkpoint_prefix;
		long seed;
		int max_threads;
		double eps;
		int new_dim;
		int anchor_thresh;
		int top_words;

		Params(string settings_file) {
			ifstream f_settings_file;
			string line, left, right;
			char *token, *line_cstr = new char[100];
			log_prefix = "";
			checkpoint_prefix = "";
			seed = (long)time(NULL);
			
			f_settings_file.open(settings_file.c_str());
			while( getline(f_settings_file, line) ) {
				if(line == "" || line == "\n" || line[0] == '#') continue;
				strcpy(line_cstr, line.c_str());
				token = strtok(line_cstr, "=");
				left = string(token);
				trim(left);
				right = string(strtok(NULL, "="));
				trim(right);

				if(left == "log_prefix") {
					log_prefix = right;
				} else if(left == "max_threads") {
					max_threads = atoi(right.c_str());
				} else if(left == "eps") {
					eps = atof(right.c_str());
				} else if(left == "checkpoint_prefix") {
					checkpoint_prefix = right;
				} else if(left == "new_dim") {
					new_dim = atoi(right.c_str());
				} else if(left == "seed") {
					seed = atoi(right.c_str());
				} else if(left == "anchor_thresh") {
					anchor_thresh = atoi(right.c_str());
				} else if(left == "top_words") {
					top_words = atoi(right.c_str());
				}
			}
		}
		// show the settings
		void display_settings() {
			cout << "===================== settings =========================" << endl;
			cout << "log_prefix:" << log_prefix << endl;
			cout << "max_threads:" << max_threads << endl;
			cout << "eps:" << eps << endl;
			cout << "checkpoint_prefix:" << checkpoint_prefix << endl;
			cout << "new_dim:" << new_dim << endl;
			cout << "seed:" << seed << endl;
			cout << "anchor_thresh" << anchor_thresh << endl;
			cout << "top_words" << top_words << endl;
			cout << "========================================================" << endl;
		}
};

#endif
