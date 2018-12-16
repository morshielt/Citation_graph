#include <iostream>
#include "citation_graph.h"

class Publication {
public:
    typedef typename std::string id_type;
    Publication(id_type const &_id) : id(_id) {
    }
    id_type get_id() const noexcept {
        return id;
    }
private:
    id_type id;
};

int main() {

    CitationGraph<Publication> gen("Goto Considered Harmful");
    CitationGraph<Publication> gen2(std::move(gen));\
    CitationGraph<Publication> ge3("Goto Considered Harmful");
    
    try {
        throw PublicationAlreadyCreated();
    }
    catch (std::exception &e) {
        std::cout << e.what() << '\n';
    }
}