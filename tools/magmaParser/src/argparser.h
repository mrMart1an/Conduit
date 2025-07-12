#ifndef CNDT_COMPONENTS_PARSER_ARGPARSER_H
#define CNDT_COMPONENTS_PARSER_ARGPARSER_H

#include <filesystem>
#include <vector>

class Args {
public:
    // Parse the argument and construct a Args object 
    Args() { };

    // Return the file path of the json output file
    const std::filesystem::path& outputJson() const {
        return m_output_json_file;
    }
    const std::filesystem::path& compileCommands() const {
        return m_commands_dir;
    }
    // Return the list of source file to parse
    const std::vector<std::filesystem::path>& sourceFiles() const {
        return m_source_files;
    }

    // Parse the arguments
    void parse(int argc, char *argv[]);

private:
    // Store the parsed output json
    std::filesystem::path m_output_json_file;
    // Path to the directory storing the build compiles commands
    std::filesystem::path m_commands_dir;
    // Store the source file to parse
    std::vector<std::filesystem::path> m_source_files;
};

#endif
