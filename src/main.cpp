#include <iostream>
#include <string>

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
    std::string key1 = "Goto Considered Harmful";
    std::string key2 = "A";

    CitationGraph<Publication> gen(key1);
    CitationGraph<Publication> gen2(std::move(gen));
    CitationGraph<Publication> gen3(key1);

    assert(gen3.get_children(key1).size() == 0);
    assert(gen3.get_parents(key1).size() == 0);

    assert(gen3.exists(key1));
    assert(!gen3.exists(key2));

    assert(key1 == gen2[key1].get_id());
    assert(key1 == gen3[key1].get_id());
    try {
        gen3[key2]; //rzuca PublicationNotFound
    } catch (std::exception &e) {
        std::cout << e.what() << '\n';
    }

    try {
        gen3.get_children(key2); //rzuca PublicationNotFound
    } catch (std::exception &e) {
        std::cout << e.what() << '\n';
    }

}