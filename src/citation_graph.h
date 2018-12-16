#ifndef CITATION_GRAPH_H
#define CITATION_GRAPH_H

#include <exception>

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
