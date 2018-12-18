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
    virtual const char *what() const noexcept {
        return "TriedToRemoveRoot";
    }
};

class PublicationNotFound : public std::exception {
public:
    virtual const char *what() const noexcept {
        return "PublicationNotFound";
    }
};

class PublicationAlreadyCreated : public std::exception {
public:
    virtual const char *what() const noexcept {
        return "PublicationAlreadyCreated";
    }
};

template<typename Publication>
class CitationGraph {

public:
    using id_type = typename Publication::id_type;
    
    CitationGraph(id_type const &stem_id) {
        root = std::make_shared<Node>(stem_id, this);
        auto insRes = nodes.emplace(stem_id, root);
        root->setIt(insRes.first);
    }
    
    CitationGraph(CitationGraph<Publication> &&other) noexcept : nodes(std::move(other.nodes)),
                                                                 root(std::move(other.root)) {
        for (auto &node : nodes) {
            node.second.lock()->setGraph(this);
        }
    }
    
    CitationGraph(const CitationGraph &) = delete;
    
    CitationGraph<Publication> &operator=(const CitationGraph<Publication> &) = delete;
    
    CitationGraph<Publication> &operator=(CitationGraph<Publication> &&other) noexcept {
        if (this != &other) {
            nodes = std::move(other.nodes);
            root = std::move(other.root);
            
            for (auto &node : nodes) {
                node.second.lock()->setGraph(this);
            }
        }
        
        return *this;
    }
    
    id_type get_root_id() const noexcept(noexcept(std::declval<Publication>().get_id())) {
        return root->getId();
    }
    
    bool exists(id_type const &id) const {
        return (nodes.find(id) != nodes.end());
    }
    
    std::vector<id_type> get_children(id_type const &id) const {
        auto nodeIt = nodes.find(id);
        
        if (nodeIt == nodes.end()) {
            throw PublicationNotFound();
        }
        
        auto node_ptr = nodeIt->second.lock();
        return node_ptr->children_ids();
        
    }
    
    std::vector<id_type> get_parents(id_type const &id) const {
        auto nodeIt = nodes.find(id);
        
        if (nodeIt == nodes.end()) {
            throw PublicationNotFound();
        }
        
        auto node_ptr = nodeIt->second.lock();
        return node_ptr->parents_ids();
    }
    
    Publication &operator[](id_type const &id) const {
        auto nodeIt = nodes.find(id);
        
        if (nodeIt == nodes.end()) {
            throw PublicationNotFound();
        }
        
        auto node_ptr = nodeIt->second.lock();
        return node_ptr->publication_ref();
        
    }
    
    void add_citation(id_type const &child_id, id_type const &parent_id) {
        auto childNodeIt = nodes.find(child_id);
        auto parentNodeIt = nodes.find(parent_id);
        if (childNodeIt == nodes.end() || parentNodeIt == nodes.end()) {
            throw PublicationNotFound();
        }
        auto parentPtr = parentNodeIt->second.lock();
        auto childPtr = childNodeIt->second.lock();
        
        parentPtr->children.insert(childPtr);
        childPtr->parents.insert(parentPtr);
    }
    
    void create(id_type const &id, id_type const &parent_id) {
        std::vector<id_type> parent_ids;
        parent_ids.push_back(parent_id);
        create(id, parent_ids);
    }
    
    void create(id_type const &id, std::vector<id_type> const &parent_ids) {
        if (nodes.find(id) != nodes.end()) {
            throw PublicationAlreadyCreated();
        }
        
        if (parent_ids.empty()) {
            throw PublicationNotFound();
        }
        
        std::shared_ptr<Node> newNode = std::make_shared<Node>(id, this);
        std::vector<ScopeGuard> guards;
        
        addEges(newNode, parent_ids, guards);
        
        auto insRes = nodes.emplace(id, newNode);
        newNode->setIt(insRes.first);
        
        for (auto &guard : guards) {
            guard.succeeded();
        }
    }
    
    void remove(id_type const &id) {
        auto node = nodes.find(id);
        
        if (node == nodes.end()) {
            throw PublicationNotFound();
        }
        if (node->first == get_root_id()) {
            throw TriedToRemoveRoot();
        }
        
        auto node_ptr = node->second.lock();
        
        for (auto &parent: node_ptr->parents) {
            parent.lock()->children.erase(node_ptr);
        }
        
        for (auto &child: node_ptr->children) {
            child->parents.erase(node_ptr);
        }
    }

private:
    class Node {
        friend class CitationGraph;
    
    public:
        Node(const id_type id, CitationGraph *graph) : publication(id), graph(graph) {}
        
        void setIt(const typename std::map<id_type, std::weak_ptr<Node>>::iterator &it) {
            this->it = it;
        }
        
        void setGraph(CitationGraph *graph) {
            this->graph = graph;
        }
        
        id_type getId() const noexcept(noexcept(std::declval<Publication>().get_id())) {
            return publication.get_id();
        }
        
        std::vector<id_type> children_ids() {
            std::vector<id_type> children_ids;
            for (auto &child : children) {
                children_ids.push_back(child.get()->getId());
            }
            return children_ids;
        }
        
        std::vector<id_type> parents_ids() {
            std::vector<id_type> parents_ids;
            for (auto &parent : parents) {
                parents_ids.push_back(parent.lock().get()->getId());
            }
            return parents_ids;
        }
        
        Publication &publication_ref() {
            return this->publication;
        }
        
        ~Node() {
            graph->nodes.erase(it);
        }
    
    private:
        Publication publication;
        CitationGraph *graph;
        typename std::map<id_type, std::weak_ptr<Node>>::iterator
                it; //iterator musi byc aktualny przed przenoszeniem grafu
        std::set<std::shared_ptr<Node>, std::owner_less<std::shared_ptr<Node>>> children;
        std::set<std::weak_ptr<Node>, std::owner_less<std::weak_ptr<Node>>> parents;
    };
    
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
    
    template<class Container, class Object>
    ScopeGuard guarded_insert(Container &container, Object &object) {
        auto it = container.insert(object).first;
        return ScopeGuard{[&container, it]() noexcept {
            container.erase(it);
        }};
    }
    
    template<typename Node, typename ScopeGuard>
    void
    addEges(std::shared_ptr<Node> &newNode, std::vector<id_type> const &parent_ids, std::vector<ScopeGuard> &guards) {
        for (const id_type &idType: parent_ids) {
            
            auto node = nodes.find(idType);
            
            if (node == nodes.end()) {
                throw PublicationNotFound();
            }
            std::shared_ptr<Node> parent = node->second.lock();
            
            guards.push_back(guarded_insert(parent->children, newNode));
            guards.push_back(guarded_insert(newNode->parents, parent));
        }
    }
    
    std::map<id_type, std::weak_ptr<Node>> nodes;
    std::shared_ptr<Node> root;
};

#endif //CITATION_GRAPH_H
