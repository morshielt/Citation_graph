#include "../src/citation_graph.h"

#include <cassert>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

int randomInt(int max) {
    return rand() % (max + 1);
}

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

class PublicationThrow {
public:
    typedef typename std::string id_type;
    PublicationThrow(id_type const &_id) : id(_id) {
    }
    id_type get_id() const {
        return id;
    }
private:
    id_type id;
};

long gThrowCounter; // The throw counter

void ThisCanThrow() {
    if (gThrowCounter-- == 0)
        throw 0;
}

struct customInt {
    int p;

    bool operator<(const customInt& lhs) const {
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
        const typename PublicationThrowEverything::id_type& lhs,
        const typename PublicationThrowEverything::id_type&rhs) {
    ThisCanThrow();
    return lhs.p == rhs.p;
};

bool operator>(const PublicationThrowEverything::id_type& lhs, const PublicationThrowEverything::id_type& rhs) {
    ThisCanThrow();
    return lhs.p > rhs.p;
};

bool operator<=(const PublicationThrowEverything::id_type& lhs, const PublicationThrowEverything::id_type& rhs) {
    ThisCanThrow();
    return lhs.p <= rhs.p;
};

bool operator>=(const PublicationThrowEverything::id_type& lhs, const PublicationThrowEverything::id_type& rhs) {
    ThisCanThrow();
    return lhs.p >= rhs.p;
};

bool operator!=(const PublicationThrowEverything::id_type& lhs, const PublicationThrowEverything::id_type& rhs) {
    ThisCanThrow();
    return lhs.p != rhs.p;
};


bool equalsVectors(std::vector<PublicationThrowEverything::id_type> &&v1,
                   std::vector<int> &&v2) {
    std::set<int> s1;
    for (const auto& v: v1) {
        s1.insert(v.p);
    }

    std::set<int> s2(v2.begin(), v2.end());

    return s1 == s2;
}

int main() {
/*    {
        CitationGraph<Publication> to_move("A");
        CitationGraph<Publication> graph(std::move(to_move));
    }

    srand(time(NULL));

    {
        CitationGraph<Publication> gen("A");
        gen.create("B", "A");
        gen.create("C", "B");
        gen.remove("B");
        assert(!gen.exists("B"));
        assert(!gen.exists("C"));
    }

    {
        CitationGraph<Publication> gen("Goto Considered Harmful");
        Publication::id_type const id1 = gen.get_root_id(); // Czy to jest noexcept?
        std::cout << id1 << std::endl;
        assert(gen.exists(id1));
        assert(gen.get_parents(id1).size() == 0);
        gen.create("A", id1);
        gen.create("B", id1);
        assert(gen.get_children(id1).size() == 2);
        gen.create("C", "A");
        gen.add_citation("C", "B");
        assert(gen.get_parents("C").size() == 2);
        assert(gen.get_children("A").size() == 1);
        std::vector<Publication::id_type> parents;
        parents.push_back("A");
        parents.push_back("B");
        gen.create("D", parents);
        assert(gen.get_parents("D").size() == parents.size());
        assert(gen.get_children("A").size() == 2);
        assert("D" == gen["D"].get_id());
        gen.remove("A");
        assert(!gen.exists("A"));
        assert(gen.exists("B"));
        assert(gen.exists("C"));
        assert(gen.exists("D"));
        gen.remove("B");
        assert(!gen.exists("A"));
        assert(!gen.exists("B"));
        assert(!gen.exists("C"));
        assert(!gen.exists("D"));

        try {
            gen["E"];
        }
        catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        try {
            gen.create("E", "Goto Considered Harmful");
            gen.create("E", "Goto Considered Harmful");
        }
        catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        try {
            gen.remove("Goto Considered Harmful");
        }
        catch (std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }

    {
        CitationGraph<Publication> gen("A");
        Publication::id_type const id1 = gen.get_root_id(); // Czy to jest noexcept?
        gen.create("B", id1);
        gen.create("C", id1);
        gen.add_citation("C", "B");

        gen.remove("B");
        assert(gen.exists("A"));
        assert(gen.exists("C"));

        assert(gen.get_children("C").empty());
        assert(gen.get_parents("C").size() == 1);
    }

    {
        CitationGraph<Publication> gen("Goto Considered Harmful");
        Publication::id_type const id1 = gen.get_root_id(); // Czy to jest noexcept?
        assert(gen.exists(id1));
        assert(gen.get_parents(id1).size() == 0);
        gen.create("A", id1);
        gen.create("B", id1);
        assert(gen.get_children(id1).size() == 2);
        gen.create("C", "A");
        gen.add_citation("C", "B");
        assert(gen.get_parents("C").size() == 2);
        assert(gen.get_children("A").size() == 1);
        std::vector<Publication::id_type> parents;
        parents.push_back("A");
        parents.push_back("B");
        gen.create("D", parents);
        assert(gen.get_parents("D").size() == parents.size());
        assert(gen.get_children("A").size() == 2);
        assert("D" == gen["D"].get_id());
        gen.remove("A");
        assert(!gen.exists("A"));
        assert(gen.exists("B"));
        assert(gen.exists("C"));
        assert(gen.exists("D"));
        gen.remove("B");
        assert(!gen.exists("A"));
        assert(!gen.exists("B"));
        assert(!gen.exists("C"));
        assert(!gen.exists("D"));
        gen.create("A", id1);
        assert(gen.exists("A"));
        assert(!gen.exists("B"));
        assert(!gen.exists("C"));
        assert(!gen.exists("D"));
        gen.create("C", id1);
        assert(gen.exists("A"));
        assert(!gen.exists("B"));
        assert(gen.exists("C"));
        assert(!gen.exists("D"));
        gen.create("D", id1);
        assert(gen.exists("A"));
        assert(!gen.exists("B"));
        assert(gen.exists("C"));
        assert(gen.exists("D"));
    }

    {
        static_assert(noexcept(std::declval<CitationGraph<Publication>>().get_root_id()));
        static_assert(!noexcept(std::declval<CitationGraph<PublicationThrow>>().get_root_id()));
    }

    {
        for (int j = 2; j <= 7; ++ j) {
            bool succeeded = false;
            gThrowCounter = 1000;
            CitationGraph<PublicationThrowEverything> gen({1});
            gen.create(PublicationThrowEverything::id_type{2}, PublicationThrowEverything::id_type{1});
            gen.create(PublicationThrowEverything::id_type{3}, PublicationThrowEverything::id_type{1});
            gen.create(PublicationThrowEverything::id_type{4}, PublicationThrowEverything::id_type{3});
            gen.create(PublicationThrowEverything::id_type{5}, PublicationThrowEverything::id_type{3});
            gen.create(PublicationThrowEverything::id_type{6}, PublicationThrowEverything::id_type{3});
            gen.create(PublicationThrowEverything::id_type{7}, PublicationThrowEverything::id_type{6});
            gen.add_citation({5}, {2});

            for (long nextThrowCount = 0; !succeeded; ++nextThrowCount) {
                try {
                    gThrowCounter = nextThrowCount;
                    gen.remove(PublicationThrowEverything::id_type{j});
                    succeeded = true;
                }
                catch (...) {
                    assert(gen.exists(PublicationThrowEverything::id_type{1}));
                    assert(gen.exists(PublicationThrowEverything::id_type{2}));
                    assert(gen.exists(PublicationThrowEverything::id_type{3}));
                    assert(gen.exists(PublicationThrowEverything::id_type{4}));
                    assert(gen.exists(PublicationThrowEverything::id_type{5}));
                    assert(gen.exists(PublicationThrowEverything::id_type{6}));
                    assert(gen.exists(PublicationThrowEverything::id_type{7}));
                    assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{1}), {2, 3}));
                    assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{2}), {5}));
                    assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{2}), {1}));
                    assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{3}), {1}));
                    assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{3}), {4, 5, 6}));
                    assert(gen.get_children(PublicationThrowEverything::id_type{4}).empty());
                    assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{4}), {3}));
                    assert(gen.get_children(PublicationThrowEverything::id_type{5}).empty());
                    assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{5}), {2, 3}));
                    assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{6}), {7}));
                    assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{6}), {3}));
                }
            }
        }
    }

    {
        bool succeeded = false;
        gThrowCounter = 1000;
        CitationGraph<PublicationThrowEverything> gen({1});

        for (long nextThrowCount = 0; !succeeded; ++nextThrowCount) {
            try {
                gThrowCounter = nextThrowCount;
                gen.get_root_id();
                succeeded = true;
            }
            catch (...) {
                assert(gen.exists(PublicationThrowEverything::id_type{1}));
            }
        }
    }

    {
        bool succeeded = false;
        gThrowCounter = 1000;
        CitationGraph<PublicationThrowEverything> gen({1});

        for (long nextThrowCount = 0; !succeeded; ++nextThrowCount) {
            try {
                gThrowCounter = nextThrowCount;
                gen[{1}];
                succeeded = true;
            }
            catch (...) {
                assert(gen.exists(PublicationThrowEverything::id_type{1}));
            }
        }
    }*/

//TODO #nie_działa
    {
        bool succeeded = false;
        gThrowCounter = 1000;
        CitationGraph<PublicationThrowEverything> gen({1});
        gen.create(PublicationThrowEverything::id_type{2}, PublicationThrowEverything::id_type{1});
        gen.create(PublicationThrowEverything::id_type{3}, PublicationThrowEverything::id_type{1});

        for (long nextThrowCount = 0; !succeeded; ++nextThrowCount) {
            try {
                gThrowCounter = nextThrowCount;
                gen.create(PublicationThrowEverything::id_type{4}, PublicationThrowEverything::id_type{3});
                succeeded = true;
            }
            catch (...) {
                assert(gen.exists(PublicationThrowEverything::id_type{1}));
                assert(gen.exists(PublicationThrowEverything::id_type{2}));
                assert(gen.exists(PublicationThrowEverything::id_type{3}));
                assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{1}), {2, 3}));
                assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{2}), {}));
                assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{3}), {}));
                assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{2}), {1}));
                assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{3}), {1}));
            }
        }
    }

//TODO #nie_działa
   /* {
        bool succeeded = false;
        gThrowCounter = 1000;
        CitationGraph<PublicationThrowEverything> gen({1});
        gen.create(PublicationThrowEverything::id_type{2}, PublicationThrowEverything::id_type{1});
        gen.create(PublicationThrowEverything::id_type{3}, PublicationThrowEverything::id_type{1});

        for (long nextThrowCount = 0; !succeeded; ++nextThrowCount) {
            try {
                gThrowCounter = nextThrowCount;
                gen.create(PublicationThrowEverything::id_type{4},
                           {PublicationThrowEverything::id_type{3},
                            PublicationThrowEverything::id_type{2},
                            PublicationThrowEverything::id_type{1}});
                succeeded = true;
            }
            catch (...) {
                assert(gen.exists(PublicationThrowEverything::id_type{1}));
                assert(gen.exists(PublicationThrowEverything::id_type{2}));
                assert(gen.exists(PublicationThrowEverything::id_type{3}));
                assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{1}), {2, 3}));
                assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{2}), {}));
                assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{3}), {}));
                assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{2}), {1}));
                assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{3}), {1}));
            }
        }
    }*/

    /*{
        bool succeeded = false;
        gThrowCounter = 1000;
        CitationGraph<PublicationThrowEverything> gen({1});
        gen.create(PublicationThrowEverything::id_type{2}, PublicationThrowEverything::id_type{1});
        gen.create(PublicationThrowEverything::id_type{3}, PublicationThrowEverything::id_type{1});

        for (long nextThrowCount = 0; !succeeded; ++nextThrowCount) {
            try {
                gThrowCounter = nextThrowCount;
                gen.add_citation(PublicationThrowEverything::id_type{3}, PublicationThrowEverything::id_type{2});
                succeeded = true;
            }
            catch (...) {
                assert(gen.exists(PublicationThrowEverything::id_type{1}));
                assert(gen.exists(PublicationThrowEverything::id_type{2}));
                assert(gen.exists(PublicationThrowEverything::id_type{3}));
                assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{1}), {2, 3}));
                assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{2}), {}));
                assert(equalsVectors(gen.get_children(PublicationThrowEverything::id_type{3}), {}));
                assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{2}), {1}));
                assert(equalsVectors(gen.get_parents(PublicationThrowEverything::id_type{3}), {1}));
            }
        }
    }*/

/*    {
        try {
            CitationGraph<Publication> gen("G");
            gen.create("A", std::vector<Publication::id_type>{});
            assert(false);
        } catch (...) {}
    }*/
}