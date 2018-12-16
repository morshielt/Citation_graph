#ifndef CITATION_GRAPH_H
#define CITATION_GRAPH_H

#include <exception>

class TriedToRemoveRoot : public std::exception {
public:
    virtual const char *what() const throw() {
        return "TriedToRemoveRoot";
    }
};

class PublicationNotFound : public std::exception {
public:
    virtual const char *what() const throw() {
        return "PublicationNotFound";
    }
};

class PublicationAlreadyCreated : public std::exception {
public:
    virtual const char *what() const throw() {
        return "PublicationAlreadyCreated";
    }
};

#endif //CITATION_GRAPH_H
