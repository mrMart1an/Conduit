#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/Index.h>

#include "sourceLoader.h"
#include "argparser.h"

SourceLoader::SourceLoader(Args cli_args)
{
    // Create the clang index
    m_index = clang_createIndex(0, 0);

    // Parse the compile commands
    CXCompilationDatabase_Error db_error;
    CXCompilationDatabase comp_db = clang_CompilationDatabase_fromDirectory(
        cli_args.compileCommands().c_str(), 
        &db_error
    );

    if (db_error != CXCompilationDatabase_NoError) {
        std::stringstream s;
        s << "Error loading compilation database from ";
        s << cli_args.compileCommands().c_str() << ": ";
        s << db_error;

        throw std::runtime_error(s.str());
    }

    // Load all the translation units
    for (auto& file : cli_args.sourceFiles()) {
        // Get the compile commands
        CXCompileCommands compile_commands =
            clang_CompilationDatabase_getCompileCommands(
                comp_db, 
                file.c_str()
            );
        unsigned int num_commands = 
            clang_CompileCommands_getSize(compile_commands);

        if (num_commands == 0) {
            std::stringstream s;
            s << "No compile commands found for: ";
            s << file.c_str();

            throw std::runtime_error(s.str());
        }

        // For each compile command found create a translation unit
        // in general file will only have 1 command
        for (unsigned int i = 0; i < num_commands; ++i) {
            CXCompileCommand command = 
                clang_CompileCommands_getCommand(compile_commands, i);

            unsigned int num_args = clang_CompileCommand_getNumArgs(command);
            std::vector<const char*> args(num_args);

            for (unsigned int j = 0; j < num_args; ++j) {
                CXString arg_str = clang_CompileCommand_getArg(command, j);
                args[j] = clang_getCString(arg_str);
            }

            // Remove commands relative to the source path
            // TODO: Handle this better
            args.pop_back();
            args.pop_back();

            // Create the translation unit
            CXTranslationUnit unit = clang_parseTranslationUnit(
                m_index,
                file.c_str(),
                args.data(),
                args.size(),
                NULL, 0,
                CXTranslationUnit_None
            );

            if (unit == nullptr) {
                std::stringstream s;
                s << "Error parsing tralation unit: ";
                s << file.c_str();

                throw std::runtime_error(s.str());
            } else {
                m_traslation_units.push_back(unit);
            }
        }

        clang_CompileCommands_dispose(compile_commands);
    }

    clang_CompilationDatabase_dispose(comp_db);
}

SourceLoader::~SourceLoader()
{
    for (auto& unit : m_traslation_units) {
        clang_disposeTranslationUnit(unit);
    }

    clang_disposeIndex(m_index);
}
