


#define BOOST_TEST_MODULE citation_graph_TESTS

#include <boost/test/included/unit_test.hpp>
#include <boost/test/execution_monitor.hpp>
#include <boost/operators.hpp>
//#include "citation_graph_memory_testing.h"
#include "../src/citation_graph.h"

long long steps_left_to_throw = 0;
bool test_memory = false;

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

static long long constructor_passes_after;

static long long inf = std::numeric_limits<long long>::max();

void except() {
    --steps_left_to_throw;
    if (steps_left_to_throw <= 0) {
        throw "error";
    }
}

class Id : public boost::totally_ordered<Id> {
private:
    int id;
    static int steps_left_to_throw;

public:
    Id(int const &id) : id(id) {
        except();
    }
    
    Id(const Id &other) {
        except();
        this->id = other.id;
    }
    
    Id(Id &&other) {
        except();
        this->id = other.id;
    }
    
    Id &operator=(const Id &other) {
        except();
        this->id = other.id;
        return *this;
    }
    
    bool operator==(const Id &other) const {
        
        except();
        
        return this->id == other.id;
    }
    
    bool operator<(const Id &other) const {
        
        except();
        
        return this->id < other.id;
    }
    
    friend std::ostream &operator<<(std::ostream &os, const Id &id);
};

std::ostream &operator<<(std::ostream &os, const Id &id) {
    return (os << id.id);
}

class Publication2 {
public:
    typedef Id id_type;
    
    Publication2(id_type const &_id) : id(_id) {
        except();
    }
    
    Publication2(const Publication2 &other) : id(other.id) {
        except();
    }
    
    Publication2(Publication2 &&other) : id(std::move(other.id)) {
        except();
    }
    
    id_type get_id() const {
        except();
        return id;
    }

private:
    id_type id;
};
BOOST_AUTO_TEST_SUITE(CitationGraphTests)
    
    std::shared_ptr<CitationGraph<Publication2>> build1() {
        steps_left_to_throw = inf;
        std::shared_ptr<CitationGraph<Publication2>> graph = std::shared_ptr<CitationGraph<Publication2>>(
                new CitationGraph<Publication2>(Id(1)));
        
        graph->create(Id(2), Id(1));
        graph->create(Id(3), Id(2));
        
        graph->create(Id(4), Id(1));
        graph->create(Id(5), Id(4));
        graph->create(Id(6), Id(4));
        
        graph->create(Id(7), std::vector<Id>{Id(6), Id(2)});
        graph->create(Id(8), std::vector<Id>{Id(5), Id(7)});
        graph->create(Id(9), Id(5));
        graph->add_citation(Id(9), Id(4));
        
        return graph;
        
    }
    
    inline void check_parents(std::shared_ptr<CitationGraph<Publication2>> &graph, Id id, std::vector<Id> expected) {
        steps_left_to_throw = inf;
        std::sort(expected.begin(), expected.end());
        std::vector<Id> result = graph->get_parents(id);
        
        std::sort(result.begin(), result.end());
        
        BOOST_REQUIRE_EQUAL(expected.size(), result.size());
        BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), result.begin(), result.end());
    }
    
    inline void check_children(std::shared_ptr<CitationGraph<Publication2>> &graph, Id id, std::vector<Id> expected) {
        steps_left_to_throw = inf;
        std::sort(expected.begin(), expected.end());
        std::vector<Id> result = graph->get_children(id);
        
        std::sort(result.begin(), result.end());
        
        BOOST_REQUIRE_EQUAL(expected.size(), result.size());
        
        BOOST_CHECK_EQUAL_COLLECTIONS(expected.begin(), expected.end(), result.begin(), result.end());
        
    }
    
    inline void check_build1(std::shared_ptr<CitationGraph<Publication2>> &graph) {
        steps_left_to_throw = inf;
        
        BOOST_CHECK_EQUAL(graph->get_parents(1).size(), 0);
        
        check_parents(graph, 2, std::vector<Id>{1});
        
        check_parents(graph, 3, std::vector<Id>{2});
        
        check_parents(graph, 4, std::vector<Id>{1});
        
        check_parents(graph, 5, std::vector<Id>{4});
        
        check_parents(graph, 6, std::vector<Id>{4});
        
        check_parents(graph, 7, std::vector<Id>{2, 6});
        
        check_parents(graph, 8, std::vector<Id>{7, 5});
        
        check_parents(graph, 9, std::vector<Id>{4, 5});
        
        
        check_children(graph, 1, std::vector<Id>{2, 4});
        
        check_children(graph, 2, std::vector<Id>{7, 3});
        
        check_children(graph, 3, std::vector<Id>{});
        
        check_children(graph, 4, std::vector<Id>{5, 6, 9});
        
        check_children(graph, 5, std::vector<Id>{8, 9});
        
        check_children(graph, 6, std::vector<Id>{7});
        
        check_children(graph, 7, std::vector<Id>{8});
        
        check_children(graph, 8, std::vector<Id>{});
        
        check_children(graph, 9, std::vector<Id>{});
        
        
    }
    
    constexpr size_t build2_list_size = 100;
    
    std::shared_ptr<CitationGraph<Publication2>> build2_list() {
        steps_left_to_throw = inf;
        std::shared_ptr<CitationGraph<Publication2>> graph = std::shared_ptr<CitationGraph<Publication2>>(
                new CitationGraph<Publication2>(Id(1)));
        
        for (size_t i = 2; i <= build2_list_size; ++i) {
            graph->create(i, i - 1);
        }
        
        return graph;
        
    }
    
    inline void check_build2_list(std::shared_ptr<CitationGraph<Publication2>> &graph) {
        steps_left_to_throw = inf;
        
        BOOST_CHECK_EQUAL(graph->get_parents(1).size(), 0);
        
        for (int i = 1; i < build2_list_size; ++i) {
            check_children(graph, i, std::vector<Id>{i + 1});
        }
        check_children(graph, build2_list_size, std::vector<Id>{});
        
        for (int i = 2; i <= build2_list_size; ++i) {
            check_parents(graph, i, std::vector<Id>{i - 1});
        }
    }
    
    void test(std::shared_ptr<CitationGraph<Publication2>> (*creator)(),
              void (*basic_check)(std::shared_ptr<CitationGraph<Publication2>> &),
              void (*operation)(std::shared_ptr<CitationGraph<Publication2>> &),
              void (*after_check)(std::shared_ptr<CitationGraph<Publication2>> &)) {
        for (long long step = 0;; ++step) {
            std::shared_ptr graph = creator();
            steps_left_to_throw = step;
            try {
                test_memory = true;
                operation(graph);
            } catch (...) {
                test_memory = false;
                //std::cerr << step << std::endl;
                basic_check(graph);
                continue;
            }
            test_memory = false;
            std::cerr << "Steps " << step << std::endl;
            if (after_check != nullptr) {
                after_check(graph);
            }
            break;
            
        }
    }
    
    BOOST_AUTO_TEST_CASE(example) {
        
        CitationGraph<Publication> gen("Goto Considered Harmful");
        Publication::id_type const id1 = gen.get_root_id(); // Czy to jest noexcept?
        BOOST_CHECK((gen.exists(id1)));
        BOOST_CHECK_EQUAL(gen.get_parents(id1).size(), 0);
        gen.create("A", id1);
        gen.create("B", id1);
        BOOST_CHECK_EQUAL(gen.get_children(id1).size(), 2);
        gen.create("C", "A");
        gen.add_citation("C", "B");
        BOOST_CHECK_EQUAL(gen.get_parents("C").size(), 2);
        BOOST_CHECK_EQUAL(gen.get_children("A").size(), 1);
        std::vector<Publication::id_type> parents;
        parents.push_back("A");
        parents.push_back("B");
        gen.create("D", parents);
        BOOST_CHECK_EQUAL(gen.get_parents("D").size(), parents.size());
        BOOST_CHECK_EQUAL(gen.get_children("A").size(), 2);
        BOOST_CHECK_EQUAL("D", gen["D"].get_id());
        gen.remove("A");
        BOOST_CHECK((!gen.exists("A")));
        BOOST_CHECK((gen.exists("B")));
        BOOST_CHECK((gen.exists("C")));
        BOOST_CHECK((gen.exists("D")));
        gen.remove("B");
        BOOST_CHECK((!gen.exists("A")));
        BOOST_CHECK((!gen.exists("B")));
        BOOST_CHECK((!gen.exists("C")));
        BOOST_CHECK((!gen.exists("D")));
        
        
        try {
            BOOST_CHECK_THROW(gen["E"], PublicationNotFound);
            gen["E"];
        }
        catch (std::exception &e) {
            BOOST_CHECK_EQUAL(e.what(), "PublicationNotFound");
        }
        try {
            gen.create("E", "Goto Considered Harmful");
            BOOST_CHECK_THROW(gen.create("E", "Goto Considered Harmful"), PublicationAlreadyCreated);
            gen.create("E", "Goto Considered Harmful");
        }
        catch (std::exception &e) {
            BOOST_CHECK_EQUAL(e.what(), "PublicationAlreadyCreated");
        }
        try {
            BOOST_CHECK_THROW(gen.remove("Goto Considered Harmful"), TriedToRemoveRoot);
            gen.remove("Goto Considered Harmful");
        }
        catch (std::exception &e) {
            BOOST_CHECK_EQUAL(e.what(), "TriedToRemoveRoot");
        }
    }
    
    BOOST_AUTO_TEST_CASE(remove) {
        std::cerr << "Test " << __LINE__ + 1 << std::endl;
        test(build1, check_build1, [](std::shared_ptr<CitationGraph<Publication2>> &graph) { graph->remove(4); },
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) {
                 steps_left_to_throw = inf;
            
                 BOOST_CHECK_EQUAL(graph->get_parents(1).size(), 0);
            
                 check_parents(graph, 2, std::vector<Id>{1});
            
                 check_parents(graph, 3, std::vector<Id>{2});
            
                 BOOST_CHECK_THROW((*graph)[4], PublicationNotFound);
                 BOOST_CHECK_THROW((*graph)[5], PublicationNotFound);
                 BOOST_CHECK_THROW((*graph)[6], PublicationNotFound);
            
            
                 check_parents(graph, 7, std::vector<Id>{2});
            
                 check_parents(graph, 8, std::vector<Id>{7});
            
                 BOOST_CHECK_THROW((*graph)[9], PublicationNotFound);
            
            
                 check_children(graph, 1, std::vector<Id>{2});
            
                 check_children(graph, 2, std::vector<Id>{7, 3});
            
                 check_children(graph, 3, std::vector<Id>{});
            
            
                 check_children(graph, 7, std::vector<Id>{8});
            
                 check_children(graph, 8, std::vector<Id>{});
            
             });
        
        std::cerr << "Test " << __LINE__ + 1 << std::endl;
        test(build1, check_build1, [](std::shared_ptr<CitationGraph<Publication2>> &graph) { graph->remove(7); },
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) {
                 steps_left_to_throw = inf;
            
                 BOOST_CHECK_EQUAL(graph->get_parents(1).size(), 0);
            
                 check_parents(graph, 2, std::vector<Id>{1});
            
                 check_parents(graph, 3, std::vector<Id>{2});
            
                 check_parents(graph, 4, std::vector<Id>{1});
            
                 check_parents(graph, 5, std::vector<Id>{4});
            
                 check_parents(graph, 6, std::vector<Id>{4});
            
                 BOOST_CHECK_THROW((*graph)[7], PublicationNotFound);
            
                 check_parents(graph, 8, std::vector<Id>{5});
            
                 check_parents(graph, 9, std::vector<Id>{4, 5});
            
            
                 check_children(graph, 1, std::vector<Id>{2, 4});
            
                 check_children(graph, 2, std::vector<Id>{3});
            
                 check_children(graph, 3, std::vector<Id>{});
            
                 check_children(graph, 4, std::vector<Id>{5, 6, 9});
            
                 check_children(graph, 5, std::vector<Id>{8, 9});
            
                 check_children(graph, 6, std::vector<Id>{});
            
                 check_children(graph, 8, std::vector<Id>{});
            
                 check_children(graph, 9, std::vector<Id>{});
             });
    }
    
    BOOST_AUTO_TEST_CASE(single_create) {
        std::cerr << "Test " << __LINE__ + 1 << std::endl;
        test(build1, check_build1, [](std::shared_ptr<CitationGraph<Publication2>> &graph) { graph->create(22, 1); },
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) {
                 steps_left_to_throw = inf;
            
                 BOOST_CHECK_EQUAL(graph->get_parents(1).size(), 0);
            
                 check_parents(graph, 2, std::vector<Id>{1});
            
                 check_parents(graph, 3, std::vector<Id>{2});
            
                 check_parents(graph, 4, std::vector<Id>{1});
            
                 check_parents(graph, 5, std::vector<Id>{4});
            
                 check_parents(graph, 6, std::vector<Id>{4});
            
                 check_parents(graph, 7, std::vector<Id>{2, 6});
            
                 check_parents(graph, 8, std::vector<Id>{7, 5});
            
                 check_parents(graph, 9, std::vector<Id>{4, 5});
            
                 check_parents(graph, 22, std::vector<Id>{1});
            
            
                 check_children(graph, 1, std::vector<Id>{2, 4, 22});
            
                 check_children(graph, 2, std::vector<Id>{7, 3});
            
                 check_children(graph, 3, std::vector<Id>{});
            
                 check_children(graph, 4, std::vector<Id>{5, 6, 9});
            
                 check_children(graph, 5, std::vector<Id>{8, 9});
            
                 check_children(graph, 6, std::vector<Id>{7});
            
                 check_children(graph, 7, std::vector<Id>{8});
            
                 check_children(graph, 8, std::vector<Id>{});
            
                 check_children(graph, 9, std::vector<Id>{});
             });
        
        std::cerr << "Test " << __LINE__ + 1 << std::endl;
        test(build1, check_build1, [](std::shared_ptr<CitationGraph<Publication2>> &graph) { graph->create(22, 9); },
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) {
                 steps_left_to_throw = inf;
            
                 BOOST_CHECK_EQUAL(graph->get_parents(1).size(), 0);
            
                 check_parents(graph, 2, std::vector<Id>{1});
            
                 check_parents(graph, 3, std::vector<Id>{2});
            
                 check_parents(graph, 4, std::vector<Id>{1});
            
                 check_parents(graph, 5, std::vector<Id>{4});
            
                 check_parents(graph, 6, std::vector<Id>{4});
            
                 check_parents(graph, 7, std::vector<Id>{2, 6});
            
                 check_parents(graph, 8, std::vector<Id>{7, 5});
            
                 check_parents(graph, 9, std::vector<Id>{4, 5});
            
                 check_parents(graph, 22, std::vector<Id>{9});
            
            
                 check_children(graph, 1, std::vector<Id>{2, 4});
            
                 check_children(graph, 2, std::vector<Id>{7, 3});
            
                 check_children(graph, 3, std::vector<Id>{});
            
                 check_children(graph, 4, std::vector<Id>{5, 6, 9});
            
                 check_children(graph, 5, std::vector<Id>{8, 9});
            
                 check_children(graph, 6, std::vector<Id>{7});
            
                 check_children(graph, 7, std::vector<Id>{8});
            
                 check_children(graph, 8, std::vector<Id>{});
            
                 check_children(graph, 9, std::vector<Id>{22});
             });
    }
    
    BOOST_AUTO_TEST_CASE(vector_create) {
        std::cerr << "Test " << __LINE__ + 1 << std::endl;
        test(build1, check_build1,
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) { graph->create(22, {1, 2, 3, 4, 6, 7, 8, 9}); },
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) {
                 steps_left_to_throw = inf;
            
                 BOOST_CHECK_EQUAL(graph->get_parents(1).size(), 0);
            
                 check_parents(graph, 2, std::vector<Id>{1});
            
                 check_parents(graph, 3, std::vector<Id>{2});
            
                 check_parents(graph, 4, std::vector<Id>{1});
            
                 check_parents(graph, 5, std::vector<Id>{4});
            
                 check_parents(graph, 6, std::vector<Id>{4});
            
                 check_parents(graph, 7, std::vector<Id>{2, 6});
            
                 check_parents(graph, 8, std::vector<Id>{7, 5});
            
                 check_parents(graph, 9, std::vector<Id>{4, 5});
            
                 check_parents(graph, 22, std::vector<Id>{1, 2, 3, 4, 6, 7, 8, 9});
            
            
                 check_children(graph, 1, std::vector<Id>{2, 4, 22});
            
                 check_children(graph, 2, std::vector<Id>{7, 3, 22});
            
                 check_children(graph, 3, std::vector<Id>{22});
            
                 check_children(graph, 4, std::vector<Id>{5, 6, 9, 22});
            
                 check_children(graph, 5, std::vector<Id>{8, 9});
            
                 check_children(graph, 6, std::vector<Id>{7, 22});
            
                 check_children(graph, 7, std::vector<Id>{8, 22});
            
                 check_children(graph, 8, std::vector<Id>{22});
            
                 check_children(graph, 9, std::vector<Id>{22});
            
                 check_children(graph, 22, std::vector<Id>{});
             });
        
        {
            CitationGraph<Publication> graph("A");
            BOOST_CHECK_THROW(graph.create("B", std::vector<Publication::id_type>{}), PublicationNotFound);
        }
    }
    
    BOOST_AUTO_TEST_CASE(add_citation) {
        
        std::cerr << "Test " << __LINE__ + 1 << std::endl;
        test(build1, check_build1,
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) { graph->add_citation(5, 1); },
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) {
                 steps_left_to_throw = inf;
            
                 BOOST_CHECK_EQUAL(graph->get_parents(1).size(), 0);
            
                 check_parents(graph, 2, std::vector<Id>{1});
            
                 check_parents(graph, 3, std::vector<Id>{2});
            
                 check_parents(graph, 4, std::vector<Id>{1});
            
                 check_parents(graph, 5, std::vector<Id>{4, 1});
            
                 check_parents(graph, 6, std::vector<Id>{4});
            
                 check_parents(graph, 7, std::vector<Id>{2, 6});
            
                 check_parents(graph, 8, std::vector<Id>{7, 5});
            
                 check_parents(graph, 9, std::vector<Id>{4, 5});
            
            
                 check_children(graph, 1, std::vector<Id>{2, 4, 5});
            
                 check_children(graph, 2, std::vector<Id>{7, 3});
            
                 check_children(graph, 3, std::vector<Id>{});
            
                 check_children(graph, 4, std::vector<Id>{5, 6, 9});
            
                 check_children(graph, 5, std::vector<Id>{8, 9});
            
                 check_children(graph, 6, std::vector<Id>{7});
            
                 check_children(graph, 7, std::vector<Id>{8});
            
                 check_children(graph, 8, std::vector<Id>{});
            
                 check_children(graph, 9, std::vector<Id>{});
             });
        
        std::cerr << "Test " << __LINE__ + 1 << std::endl;
        test(build1, check_build1,
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) { graph->add_citation(9, 2); },
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) {
                 steps_left_to_throw = inf;
            
                 BOOST_CHECK_EQUAL(graph->get_parents(1).size(), 0);
            
                 check_parents(graph, 2, std::vector<Id>{1});
            
                 check_parents(graph, 3, std::vector<Id>{2});
            
                 check_parents(graph, 4, std::vector<Id>{1});
            
                 check_parents(graph, 5, std::vector<Id>{4});
            
                 check_parents(graph, 6, std::vector<Id>{4});
            
                 check_parents(graph, 7, std::vector<Id>{2, 6});
            
                 check_parents(graph, 8, std::vector<Id>{7, 5});
            
                 check_parents(graph, 9, std::vector<Id>{4, 5, 2});
            
            
                 check_children(graph, 1, std::vector<Id>{2, 4});
            
                 check_children(graph, 2, std::vector<Id>{7, 3, 9});
            
                 check_children(graph, 3, std::vector<Id>{});
            
                 check_children(graph, 4, std::vector<Id>{5, 6, 9});
            
                 check_children(graph, 5, std::vector<Id>{8, 9});
            
                 check_children(graph, 6, std::vector<Id>{7});
            
                 check_children(graph, 7, std::vector<Id>{8});
            
                 check_children(graph, 8, std::vector<Id>{});
            
                 check_children(graph, 9, std::vector<Id>{});
             });
    }
    
    BOOST_AUTO_TEST_CASE(memoryCombined) {
        
        std::cerr << "Test " << __LINE__ + 1 << std::endl;
        test(build2_list, check_build2_list,
             [](std::shared_ptr<CitationGraph<Publication2>> &graph) {
                 auto cpy = steps_left_to_throw;
                 steps_left_to_throw = inf;
                 std::vector<Id> add_vector;
            
            
                 for (size_t i = 1; i <= 50; ++i) {
                     add_vector.push_back(i);
                 }
            
                 steps_left_to_throw = cpy;
            
            
                 graph->create(0, add_vector);
            
                 try {
                     graph->remove(10);
                 } catch (...) {
                     steps_left_to_throw = inf;
                     graph->remove(0);
                     throw;
                 }
             }, [](std::shared_ptr<CitationGraph<Publication2>> &graph) {
                    steps_left_to_throw = inf;
                    
                    for (size_t i = 0; i <= 9; ++i) {
                        BOOST_CHECK_NO_THROW((*graph)[i]);
                        BOOST_CHECK_EQUAL(graph->exists(i), true);
                    }
                    
                    
                    for (size_t i = 10; i <= build2_list_size; ++i) {
                        BOOST_CHECK_THROW((*graph)[i], PublicationNotFound);
                        BOOST_CHECK_EQUAL(graph->exists(i), false);
                    }
                    
                    for (int i = 1; i < 9; ++i) {
                        check_children(graph, i, std::vector<Id>{i + 1, 0});
                    }
                    
                    check_children(graph, 9, std::vector<Id>{0});
                    check_children(graph, 0, std::vector<Id>{});
                    
                    for (int i = 2; i <= 9; ++i) {
                        check_parents(graph, i, std::vector<Id>{i - 1});
                    }
                    
                    std::vector<Id> parentsOf0;
                    
                    for (size_t i = 1; i <= 9; ++i) {
                        parentsOf0.push_back(i);
                    }
                    
                    check_parents(graph, 0, parentsOf0);
                });
    }
    
    Publication::id_type
    no_except_id_pub_get_id_wrapper() noexcept(noexcept(std::declval<CitationGraph<Publication>>().get_root_id())) {
        return CitationGraph<Publication>("A").get_root_id();
    }
    
    Publication2::id_type
    except_id_pub_get_id_wrapper() noexcept(noexcept(std::declval<CitationGraph<Publication2>>().get_root_id())) {
        return CitationGraph<Publication2>(2).get_root_id();
    }
    
    BOOST_AUTO_TEST_CASE(get_id) {
        steps_left_to_throw = inf;
        BOOST_CHECK(
                (std::__nothrow_invokable_r<Publication::id_type, decltype(no_except_id_pub_get_id_wrapper)>::value));
        BOOST_CHECK(
                (!std::__nothrow_invokable_r<Publication2::id_type, decltype(except_id_pub_get_id_wrapper)>::value));
    }
    
    BOOST_AUTO_TEST_CASE(constructor) {
        { //creation
            for (long long step = 0;; ++step) {
                steps_left_to_throw = step;
                try {
                    test_memory = true;
                    CitationGraph<Publication2> graph(Id(1));
                } catch (...) {
                    test_memory = false;
                    continue;
                }
                test_memory = false;
                
                constructor_passes_after = step;
                //constructor_passes_after = inf;
                std::cerr << "constructor passes after:" << constructor_passes_after << std::endl;
                break;
                
            }
            
        }
        
        BOOST_CHECK((!std::is_copy_constructible<CitationGraph<Publication2>>::value));
        
        
        BOOST_CHECK((std::is_nothrow_move_constructible<CitationGraph<Publication2>>::value));
        { //move
            steps_left_to_throw = inf;
            CitationGraph<Publication2> to_move(Id(1));
            
            steps_left_to_throw = 0;
            auto test_lambda = [&to_move]() {
                test_memory = true;
                CitationGraph<Publication2> graph(std::move(to_move));
                test_memory = false;
            };
            BOOST_CHECK_NO_THROW(test_lambda());
            
        }
        
        {//move with some nodes
            steps_left_to_throw = inf;
            std::shared_ptr<CitationGraph<Publication2>> to_move = build1();
            
            steps_left_to_throw = 0;
            std::shared_ptr<CitationGraph<Publication2>> graph(new CitationGraph<Publication2>(std::move(*to_move)));
            
            steps_left_to_throw = inf;
            check_build1(graph);
            
        }
    }
    
    BOOST_AUTO_TEST_CASE(destructor) {
        BOOST_CHECK((std::is_nothrow_destructible<CitationGraph<Publication2>>::value));
    }
    
    BOOST_AUTO_TEST_CASE(assign) {
        BOOST_CHECK((!std::is_copy_assignable<CitationGraph<Publication2>>::value));
    }
    
    BOOST_AUTO_TEST_CASE(move) {
        
        BOOST_CHECK((std::is_nothrow_move_assignable<CitationGraph<Publication2>>::value));
        { //move assign
            steps_left_to_throw = inf;
            CitationGraph<Publication2> graph(Id(1));
            
            steps_left_to_throw = inf;
            CitationGraph<Publication2> graph2(Id(1));
            
            steps_left_to_throw = 0;
            BOOST_CHECK_NO_THROW(graph2 = std::move(graph));
        }
        
        { //move with some nodes
            steps_left_to_throw = inf;
            std::shared_ptr<CitationGraph<Publication2>> to_move = build1();
            std::shared_ptr<CitationGraph<Publication2>> graph(new CitationGraph<Publication2>(0));
            
            steps_left_to_throw = 0;
            test_memory = true;
            *graph = std::move(*to_move);
            test_memory = false;
            
            steps_left_to_throw = inf;
            check_build1(graph);
        }
        
        { //self move
            steps_left_to_throw = inf;
            std::shared_ptr<CitationGraph<Publication2>> to_move = build1();
            
            
            steps_left_to_throw = 0;
            test_memory = true;
            *to_move = std::move(*to_move);
            test_memory = false;
            
            steps_left_to_throw = inf;
            check_build1(to_move);
        }
    }

//    BOOST_AUTO_TEST_CASE(exitMemory) {
//        std::atexit(memCheck);
//    }


BOOST_AUTO_TEST_SUITE_END()
