#include "astParser.h"

#include <functional>

#include <clang-c/Index.h>

void ASTParser::parse(SourceLoader& loader) 
{
    // Prepare the client data struct
    loader.execute<ASTParser>(
        [](CXTranslationUnit unit, ASTParser* parser_p) 
        {
            // Execute the callback for all the node of the AST graph
            CXCursor cursor = clang_getTranslationUnitCursor(unit);

            clang_visitChildren(
                cursor,
                [](CXCursor node, CXCursor parent, CXClientData client_data)
                {
                    ASTParser* parser =
                        static_cast<ASTParser*>(client_data);

                    // Call the callback
                    parser->nodeCallback(node, parent);

                    return CXChildVisit_Recurse;
                },
                static_cast<CXClientData>(parser_p)
            );
        }, 
        this
    );
}
