all: truncate_vocabulary learn_topics

truncate_vocabulary: truncate_vocabulary.cpp
	g++ -g truncate_vocabulary.cpp -o truncate_vocabulary

learn_topics: learn_topics.cpp
	g++ -g learn_topics.cpp -o learn_topics
