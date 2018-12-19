CXXFLAGS = -Wall -Wextra -Og -g -std=c++17

.PHONY: all
all: citation_graph_example citation_graph_test


citation_graph_example: zadanie/citation_graph_example.o src/citation_graph.h
	$(CXX) $^ $(CXXFLAGS) -o $@

citation_graph_test: test/citation_graph_unit_tests.o src/citation_graph.h
	$(CXX) $^ $(CXXFLAGS) -o $@

zadanie/%: CXXFLAGS += -I src
test/%: CXXFLAGS += -I src

%.o: %.cc
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	rm -f citation_graph_example
	rm -f citation_graph_test
	find zadanie src -type f -name '*.o' | xargs rm -f
