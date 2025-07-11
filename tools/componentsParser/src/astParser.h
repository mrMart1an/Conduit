#ifndef CNDT_COMPONENTS_PARSER_AST_PARSER_H
#define CNDT_COMPONENTS_PARSER_AST_PARSER_H

#include "sourceLoader.h"
#include <clang-c/Index.h>
#include <functional>
class ASTParser {
private:
    // Store the data for the visit children callback
    template <typename UserData>
    struct ClientData {
        std::function<void(CXCursor, CXCursor, UserData*)> callback;
        UserData* data;
    };

public:
    // Create a AST parser from the provided loader
    ASTParser(SourceLoader& loader) : m_loader(loader) { }

    // Run the given callback for every node in the AST
    template <typename UserData>
    void execute(
        std::function<void(CXCursor, CXCursor, UserData*)> callback,
        UserData* data
    );

private:
    SourceLoader& m_loader;
};

template <typename UserData>
void ASTParser::execute(
    std::function<void(CXCursor, CXCursor, UserData*)> callback_fn,
    UserData* data
) {
    m_loader.execute<UserData>(
        [&](CXTranslationUnit unit, UserData* data) 
        {
            // Prepare the client data struct
            ClientData<UserData> c_data { callback_fn, data };

            // Execute the callback for all the node of the AST graph
            CXCursor cursor = clang_getTranslationUnitCursor(unit);

            clang_visitChildren(
                cursor,
                [](CXCursor node, CXCursor parent, CXClientData client_data)
                {
                    ClientData<UserData>* callback_data =
                        static_cast<ClientData<UserData>*>(client_data);

                    // Call the callback
                    callback_data->callback(node, parent, callback_data->data);

                    return CXChildVisit_Recurse;
                },
                static_cast<CXClientData>(&c_data)
            );
        }, 
        data
    );
}

#endif
