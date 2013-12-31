source_file=truncate_vocabulary.cpp learn_topics.cpp
learn_topics: $(source_file)
	g++ -g $(source_file) -o learn_topics
