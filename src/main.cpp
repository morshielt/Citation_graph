#include "citation_graph.h"
#include <iostream>

int main() {

    try {
        throw PublicationAlreadyCreated();
    }
    catch (std::exception &e) {
        std::cout << e.what() << '\n';
    }

    return 0;
}