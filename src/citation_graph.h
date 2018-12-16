#ifndef CITATION_GRAPH_H
#define CITATION_GRAPH_H

#include <exception>
#include <map>
#include <set>
#include <vector>
#include <memory>

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

    bool exists(id_type const &id) const {
        return (nodes.find(id) != nodes.end());
    }

    std::vector<id_type> get_children(id_type const &id) const {
        auto node = nodes.find(id);
        if (node == nodes.end()) throw PublicationNotFound();
        else {
            auto node_ptr = node->second.lock();
            return node_ptr.get()->children_ids();
        }
    }

    std::vector<id_type> get_parents(id_type const &id) const {
        auto node = nodes.find(id);
        if (node == nodes.end()) throw PublicationNotFound();
        else {
            auto node_ptr = node->second.lock();
            return node_ptr.get()->parents_ids();
        }
    }

    Publication &operator[](id_type const &id) const {
        auto node = nodes.find(id);
        if (node == nodes.end()) throw PublicationNotFound();
        else {
            auto node_ptr = node->second.lock();
            return node_ptr.get()->publication_ref();
        }
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

        id_type getId() const noexcept(noexcept(publication.Publication::get_id())) {
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
            return Node::publication;
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

#endif //CITATION_GRAPH_H
