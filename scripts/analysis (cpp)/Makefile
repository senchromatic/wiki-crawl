all:
	make graph.o
	make analysis

clean:
	rm -f bin/graph.o bin/analysis

run:
	make all
	bin/analysis

graph.o:
	g++ -c graph.cpp -o bin/graph.o -std=c++14 

analysis: bin/graph.o
	g++ -o bin/analysis analysis.cpp bin/graph.o -std=c++14
