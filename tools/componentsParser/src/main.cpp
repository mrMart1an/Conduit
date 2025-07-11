#include <exception>
#include <iostream>

#include <argparse/argparse.hpp>
#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>

#include "argparser.h"
#include "astParser.h"
#include "sourceLoader.h"

std::ostream& operator<<(std::ostream& stream, const CXString& str)
{
  stream << clang_getCString(str);
  clang_disposeString(str);
  return stream;
}

void clb(CXCursor c, CXCursor parent, CXClientData client_data)
{
    std::cout << "Cursor '" << clang_getCursorSpelling(c) << "' of kind '"
        << clang_getCursorKindSpelling(clang_getCursorKind(c)) << "'\n";
}

int main(int argc, char *argv[]) {
    Args args;

    // Parse the command line arguments arguments
    try {
        args.parse(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    try {
        SourceLoader loader(args);
        ASTParser parser(loader);

        parser.execute<void*>(clb, nullptr);

    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    return 0;
}
