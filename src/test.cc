#include <iostream>
#include <string>
#include <cassert>

#include "citation_graph.h"

class PublicationException : public std::exception {
public:
    virtual const char *what() const noexcept {
        return "PublicationException";
    }
};

class Publication {
public:
    typedef typename std::string id_type;
    Publication(id_type const &_id) : id(_id) {
        ctr++;
        if (ctr % 3 == 0) throw PublicationException();
    }

    id_type get_id() const /*noexcept*/ {
        ctr++;
        if (ctr % 3 == 0) throw PublicationException();
        return id;
    }

private:
    id_type id;
    static int ctr;
};

int Publication::ctr = 0;

int main() {
    std::string key1 = "1";
    std::string key12 = "12";
    std::string key13 = "13";
    std::string key14 = "14";

    CitationGraph<Publication> gen(key1);
    gen.create(key12, key1);

    try {
        gen.create(key13, key1); //wywala bo to 3 konstruktor
    } catch (std::exception &e) {
        std::cout << e.what() << '\n';
    }
    assert(gen.get_children(key1).size() == 1);
    assert(gen.exists(key13));

    gen.create(key14, key1);

    try {
        gen.get_children(key1); //wywala bo 3 razy robimy get_id() bo ma 3 dzieci
    } catch (std::exception &e) {
        std::cout << e.what() << '\n';
    }

}