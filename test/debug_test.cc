#include <iostream>
#include <string>
#include <cassert>

#include "../src/citation_graph.h"

long gThrowCounter; // The throw counter

void ThisCanThrow() {
    if (gThrowCounter-- == 0)
        throw 0;
}

struct customInt {
    int p;
    bool operator<(const customInt &lhs) const {
        ThisCanThrow();
        return lhs.p < p;
    }
};

class PublicationThrowEverything {
public:
    typedef customInt id_type;
    PublicationThrowEverything(id_type const &_id) : id(_id) {
    }

    id_type get_id() const {
        ThisCanThrow();
        return id;
    }

private:
    id_type id;
};

bool operator==(
    const typename PublicationThrowEverything::id_type &lhs,
    const typename PublicationThrowEverything::id_type &rhs) {
    //ThisCanThrow();
    return lhs.p == rhs.p;
};

bool operator>(const PublicationThrowEverything::id_type &lhs, const PublicationThrowEverything::id_type &rhs) {
    //ThisCanThrow();
    return lhs.p > rhs.p;
};

bool operator<=(const PublicationThrowEverything::id_type &lhs, const PublicationThrowEverything::id_type &rhs) {
    //ThisCanThrow();
    return lhs.p <= rhs.p;
};

bool operator>=(const PublicationThrowEverything::id_type &lhs, const PublicationThrowEverything::id_type &rhs) {
    //ThisCanThrow();
    return lhs.p >= rhs.p;
};

bool operator!=(const PublicationThrowEverything::id_type &lhs, const PublicationThrowEverything::id_type &rhs) {
    //ThisCanThrow();
    return lhs.p != rhs.p;
};

int main() {
    bool succeeded = false;
    gThrowCounter = 1000;
    CitationGraph<PublicationThrowEverything> gen({1});
    gen.create(PublicationThrowEverything::id_type{2}, PublicationThrowEverything::id_type{1});
    gen.create(PublicationThrowEverything::id_type{3}, PublicationThrowEverything::id_type{1});

    for (long nextThrowCount = 0; !succeeded; ++nextThrowCount) {
        try {
            gThrowCounter = nextThrowCount;
            std::cout << "tututuututuut\n";
            gen.create(PublicationThrowEverything::id_type{4}, PublicationThrowEverything::id_type{3});
            succeeded = true;
        }
        catch (...) {
            /* assert(gen.exists(PublicationThrowEverything::id_type{1}));
             assert(gen.exists(PublicationThrowEverything::id_type{2}));
             assert(gen.exists(PublicationThrowEverything::id_type{3}));
             assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{1}), {2, 3}));
             assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{2}), {}));
             assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{3}), {}));
             assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{2}), {1}));
             assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{3}), {1}));*/
        }
    }

    return 0;
}