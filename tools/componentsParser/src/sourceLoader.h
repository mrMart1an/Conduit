#ifndef CNDT_COMPONENTS_PARSER_SOURCE_LOADER_H
#define CNDT_COMPONENTS_PARSER_SOURCE_LOADER_H

#include "argparser.h"
#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/Index.h>
#include <functional>
#include <vector>

class SourceLoader {
public:
    // Load the source files specified in the given commands line arguments
    SourceLoader(Args args);
    ~SourceLoader();

    // Execute the given callback for each of the translation unit 
    template <typename UserData>
    void execute(
        std::function<void(CXTranslationUnit, UserData*)> callback_fn,
        UserData* data
    );

private:
    // Store the Clang index
    CXIndex m_index;

    // Store the clang translation units
    std::vector<CXTranslationUnit> m_traslation_units;
};

template<typename UserData>
void SourceLoader::execute(
    std::function<void(CXTranslationUnit, UserData*)> callback_fn,
    UserData* data
) {
    for (auto& unit : m_traslation_units) {
        callback_fn(unit, data);
    }
}

#endif
