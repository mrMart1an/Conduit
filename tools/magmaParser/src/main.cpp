#include <exception>
#include <iostream>

#include <clang-c/Index.h>
#include <ostream>

#include "argparser.h"
#include "astParser.h"
#include "sourceLoader.h"

std::ostream& operator<<(std::ostream& stream, const CXString& str)
{
  stream << clang_getCString(str);
  clang_disposeString(str);
  return stream;
}

class MyParser : public ASTParser {
public:
    void nodeCallback(CXCursor node, CXCursor parent) override {
        std::cout 
            << "Cursor '" 
            << clang_getCursorSpelling(node) 
            << "' of kind '"
            << clang_getCursorKindSpelling(clang_getCursorKind(node)) 
            << "'" 
            << std::endl;
    }
};

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
        MyParser parser;

        parser.parse(loader);

    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    return 0;
}
