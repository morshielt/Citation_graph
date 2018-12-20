#ifndef CITATION_GRAPH_H
#define CITATION_GRAPH_H

#include <exception>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <functional>

class TriedToRemoveRoot : public std::exception {
public:
    const char *what() const noexcept override {
        return "TriedToRemoveRoot";
    }
};

class PublicationNotFound : public std::exception {
public:
    const char *what() const noexcept override {
        return "PublicationNotFound";
    }
};

class PublicationAlreadyCreated : public std::exception {
public:
    const char *what() const noexcept override {
        return "PublicationAlreadyCreated";
    }
};

template<typename Publication>
class CitationGraph {
private:
    using id_type = typename Publication::id_type;
    
    class Node {
    public:
        Node(const id_type id, CitationGraph *graph) : publication(id), graph(graph), it(graph->nodes.end()) {}
        
        std::vector<id_type> children_ids() {
            std::vector<id_type> children_ids;
            for (auto &child : children) {
                //can throw, basic guarantee, doesn't modify graph
                children_ids.push_back(child->publication.get_id());
            }
            return children_ids;
        }
        
        std::vector<id_type> parents_ids() {
            std::vector<id_type> parents_ids;
            for (auto &parent : parents) {
                //can throw, basic guarantee, doesn't modify graph
                parents_ids.push_back(parent.lock()->publication.get_id()); //lock() noexcept
            }
            return parents_ids;
        }
        
        ~Node() noexcept {
            if (it != graph->nodes.end()) {
                auto ptr = it->second;
                for (auto &child : children) {
                    child->parents.erase(ptr); // doesn't throw
                }
                graph->nodes.erase(it); // doesn't throw
            }
        }
        
        Publication publication;
        CitationGraph *graph;
        typename std::map<id_type, std::weak_ptr<Node>>::iterator it;
        std::set<std::shared_ptr<Node>, std::owner_less<std::shared_ptr<Node>>> children;
        std::set<std::weak_ptr<Node>, std::owner_less<std::weak_ptr<Node>>> parents;
    };
    
    std::map<id_type, std::weak_ptr<Node>> nodes;
    std::shared_ptr<Node> root;
    
    std::shared_ptr<Node> get_node(const id_type &id) const {
        auto node_it = nodes.find(id); // strong guarantee from stl
        if (node_it == nodes.end()) {
            throw PublicationNotFound();
        }
        return node_it->second.lock(); //lock() noexcept
    }

public:
    CitationGraph(id_type const &stem_id) {
        root = std::make_shared<Node>(stem_id, this); // can throw, nothing needs to be done
        auto insert_it = nodes.emplace(stem_id, root); // strong guarantee from stl
        root->it = insert_it.first;
    }
    
    CitationGraph(CitationGraph<Publication> &&other) noexcept : nodes(std::move(other.nodes)),
                                                                 root(std::move(other.root)) {
        for (auto &node : nodes) {
            node.second.lock()->graph = this; //lock() noexcept
        }
    }
    
    CitationGraph(const CitationGraph &) = delete;
    
    CitationGraph<Publication> &operator=(const CitationGraph<Publication> &) = delete;
    
    CitationGraph<Publication> &operator=(CitationGraph<Publication> &&other) noexcept {
        if (this != &other) {
            root = std::move(other.root);
            nodes.swap(other.nodes);
            
            for (auto &node : nodes) {
                node.second.lock()->graph = this; //lock() noexcept
            }
        }
        return *this;
    }
    
    id_type get_root_id() const noexcept(noexcept(std::declval<Publication>().get_id())) {
        return root->publication.get_id();
    }
    
    bool exists(id_type const &id) const {
        return nodes.find(id) != nodes.end(); // strong guarantee from stl
    }
    
    std::vector<id_type> get_children(id_type const &id) const {
        auto node_ptr = get_node(id);
        return node_ptr->children_ids();
    }
    
    std::vector<id_type> get_parents(id_type const &id) const {
        auto node_ptr = get_node(id);
        return node_ptr->parents_ids();
    }
    
    Publication &operator[](id_type const &id) const {
        auto node_ptr = get_node(id);
        return node_ptr->publication;
    }
    
    // ScopeGuard class guarantees reverting the changes in case of insert()
    // throwing exception
    void add_citation(id_type const &child_id, id_type const &parent_id) {
        auto parent = get_node(parent_id);
        auto child = get_node(child_id);
        
        ScopeGuard guard = guarded_insert(parent->children, child);
        child->parents.insert(parent); // if throws, ScopeGuard reverts changes
        guard.succeeded(); //signal that ScopeGuard doesn't have to revert changes
    }
    
    void create(id_type const &id, id_type const &parent_id) {
        create(id, std::vector<id_type>{parent_id});
    }
    
    // ScopeGuard class guarantees reverting the changes in case of insert()
    // throwing exception
    void create(id_type const &id, std::vector<id_type> const &parent_ids) {
        if (nodes.find(id) != nodes.end()) { // strong guarantee from stl
            throw PublicationAlreadyCreated();
        }
        
        if (parent_ids.empty()) {
            throw PublicationNotFound();
        }
        
        std::shared_ptr<Node> new_node = std::make_shared<Node>(id, this); //can throw, hasn't modified graph yet
        std::vector<ScopeGuard> guards;
        
        for (const id_type &parent_id: parent_ids) {
            std::shared_ptr<Node> parent = get_node(parent_id);
            guards.push_back(guarded_insert(parent->children, new_node));
            guards.push_back(guarded_insert(new_node->parents, parent));
        }
        
        auto result_iterator = nodes.emplace(id, new_node); // strong guarantee from stl
        new_node->it = result_iterator.first;
        
        for (auto &guard : guards) {
            guard.succeeded();  //signal that ScopeGuard doesn't have to revert changes
        }
    }
    
    //All shared pointers to node are removed and ~Node() is called.
    void remove(id_type const &id) {
        auto node = nodes.find(id); // strong guarantee from stl
        
        if (node == nodes.end()) {
            throw PublicationNotFound();
        }
        if (node->first == get_root_id()) {
            throw TriedToRemoveRoot();
        }
        
        auto node_ptr = node->second.lock(); //lock() noexcept
        
        for (auto &parent: node_ptr->parents) {
            parent.lock()->children.erase(node_ptr); //lock() noexcept // erase(pointer) doesn't throw
        }
    }
    
    // ScopeGuard reverts changes in case of exception
    class ScopeGuard {
    public:
        explicit ScopeGuard(std::function<void()> revert)
                : revert_{std::move(revert)} {}
        
        ScopeGuard(const ScopeGuard &) = delete;
        
        ScopeGuard(ScopeGuard &&other) noexcept {
            *this = std::move(other);
        }
        
        ScopeGuard &operator=(const ScopeGuard &) = delete;
        
        ScopeGuard &operator=(ScopeGuard &&other) noexcept {
            revert_ = std::move(other.revert_);
            other.succeeded();
            return *this;
        }
        
        ~ScopeGuard() noexcept {
            if (revert_) {
                revert_();
            }
        }
        
        void succeeded() noexcept {
            revert_ = nullptr;
        }
    
    private:
        std::function<void()> revert_;
    };
    
    //Creates ScopeGuards to provide safe container insertion
    template<class Container, class Object>
    ScopeGuard guarded_insert(Container &container, Object &object) {
        auto it = container.insert(object).first;
        return ScopeGuard{[&container, it]() noexcept {
            // in case of exception, ScopeGuard will clear state of the container
            container.erase(it);
        }};
    }
};

#endif //CITATION_GRAPH_H