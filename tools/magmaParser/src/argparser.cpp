#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <argparse/argparse.hpp>

#include "argparser.h"

void Args::parse(int argc, char *argv[])
{
    argparse::ArgumentParser program(
        "Components parser",
        "1.0",
        argparse::default_arguments::help
    );

    // Json output file
    program.add_argument("-o", "--output")
        .required()
        .help("Specify the output file.");
    // Compile commands directory
    program.add_argument("-c", "--commands-dir")
        .required()
        .help("Specify the directory where to find the compile commands");
    // Source files
    program.add_argument("inputs")
        .required()
        .nargs(argparse::nargs_pattern::at_least_one)
        .help("Input source file to be parsed");

    // Parse and store the arguments
    program.parse_args(argc, argv);

    std::vector<std::string> sources = 
        program.get<std::vector<std::string>>("inputs"); 
    std::string output = program.get<std::string>("--output"); 
    std::string commands = program.get<std::string>("--commands-dir");

    // Convert the string to path types and check if they exist
    m_output_json_file = std::filesystem::path(output);

    if (!std::filesystem::is_directory(commands)) {
        std::stringstream s;
        s << "Compile commands directory: \"" << commands << "\" ";
        s << "not found";

        throw std::runtime_error(s.str());
    }
    m_commands_dir = std::filesystem::path(commands);

    for (auto& file : sources) {
        if (!std::filesystem::exists(file)) {
            std::stringstream s;
            s << "Source file: \"" << file << "\" ";
            s << "not found";

            throw std::runtime_error(s.str());
        }

        m_source_files.push_back(std::filesystem::path(file));
    }
}
