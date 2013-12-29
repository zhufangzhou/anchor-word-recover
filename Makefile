source_file = truncate_vocabulary.cpp
out: $(source_file)
	g++ -g $(source_file) -o out
