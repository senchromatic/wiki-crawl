all:
	make graph.o
	make analysis

clean:
	rm -f bin/graph.o bin/analysis

run:
	make all
	bin/analysis

graph.o:
	g++ -std=c++14 -O3 -c graph.cpp -o bin/graph.o

analysis: bin/graph.o
	g++ -std=c++14 -O3 -o bin/analysis analysis.cpp bin/graph.o
