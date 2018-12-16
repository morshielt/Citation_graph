#ifndef CITATION_GRAPH_H
#define CITATION_GRAPH_H

#include <exception>
#include <map>
#include <set>
#include <vector>
#include <memory>

template<typename Publication>
class CitationGraph {

public:
    using id_type = typename Publication::id_type;
    
    
    CitationGraph(id_type const &stem_id) {
        root = std::make_shared<Node>(stem_id, this);
        auto res = nodes.insert(std::pair<id_type, std::weak_ptr<Node>>(stem_id, root));
        root->setIt(res.first);
    }
    
    CitationGraph(CitationGraph<Publication> &&other) noexcept : nodes(std::move(other.nodes)),
                                                                 root(std::move(other.root)) {
        for (auto &node : nodes) {
            node.second.lock()->setGraph(this);
        }
    }
    
    CitationGraph<Publication> &operator=(CitationGraph<Publication> &&other) noexcept {
        if (this != other) {
            nodes = std::move(other.nodes);
            root = std::move(other.root);
            
            for (auto &node : nodes) {
                node.second.lock()->setGraph(this);
            }
        }
        
        return *this;
    }
    
    id_type get_root_id() const noexcept(Publication::get_id) {
        return root->getId();
    }
    
//    void create(id_type const &id, id_type const &parent_id) {
//        std::shared_ptr<Node> parent;
//        auto it
//
//    }

private:
    class Node {
    public:
        Node(const id_type id, const CitationGraph *graph) : publication(id), graph(graph) {}
        
        void setIt(const typename std::map<id_type, std::weak_ptr<Node>>::iterator &it) {
            Node::it = it;
        }
        
        void setGraph(const CitationGraph *graph) {
            Node::graph = graph;
        }
        
        id_type getId() const noexcept(Publication::get_id) {
            return publication.get_id;
        }
    
    
    private:
        Publication publication;
        const CitationGraph *graph;
    private:
        typename std::map<id_type, std::weak_ptr<Node>>::iterator it;
        std::set<std::shared_ptr<Node>, std::owner_less<std::shared_ptr<Node>>> children;
        std::set<std::weak_ptr<Node>, std::owner_less<std::weak_ptr<Node>>> parents;
        
        
    };
    
    std::map<id_type, std::weak_ptr<Node>> nodes;
    std::shared_ptr<Node> root;
    
};


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

#endif //CITATION_GRAPH_H
