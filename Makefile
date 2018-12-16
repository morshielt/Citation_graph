CXXFLAGS = -Wall -Wextra -Og -g -std=c++17

.PHONY: all
all: citation_graph_example

citation_graph_example: zadanie/citation_graph_example.o src/citation_graph.o
	$(CXX) $^ $(CXXFLAGS) -o $@

zadanie/%: CXXFLAGS += -I src

%.o: %.cc
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	rm -f citation_graph_example
	find zadanie src -type f -name '*.o' | xargs rm -f
