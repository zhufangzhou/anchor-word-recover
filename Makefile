source_file=truncate_vocabulary.cpp Q_matrix.cpp learn_topics.cpp
learn_topics: $(source_file)
	g++ -g $(source_file) -o learn_topics -lm

clean:
	rm learn_topics
