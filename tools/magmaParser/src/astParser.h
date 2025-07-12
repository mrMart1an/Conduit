#ifndef CNDT_COMPONENTS_PARSER_AST_PARSER_H
#define CNDT_COMPONENTS_PARSER_AST_PARSER_H

#include "sourceLoader.h"

class ASTParser {
public:
    ASTParser() { }

    // Parse the sources in the loader with the class node callback
    void parse(SourceLoader& loader);

    // Virtual function called for each node in the loader ASTs
    virtual void nodeCallback(CXCursor, CXCursor) = 0;
};

#endif
