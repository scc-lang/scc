#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unistd.h>

import scc.cli;
import scc.compiler;

struct Options {
    std::string inputFile {};
    bool needHelp {};
    bool compileOnly {};
};

void PrintHelp(const std::string_view& optionsHelp);
void CompileAndRun(const Options& options);
scc::compiler::AstScope Parse(const std::string& file);
std::string GetFileLine(const std::string& file, int line);
bool IsErrorColorSupported();

int main(int argc, const char* const argv[])
{
    Options options {};
    try {
        scc::cli::CommandlineProcessor cmdProcessor {};
        cmdProcessor.RegisterOption('h', "help", "Print help", [&options] { options.needHelp = true; });
        cmdProcessor.RegisterOption('c', "Compile only", [&options] { options.compileOnly = true; });
        cmdProcessor.SetCommandLine(argc - 1, argv + 1);

        if (options.needHelp) {
            PrintHelp(cmdProcessor.GetOptionsHelp());
            return 0;
        }

        if (cmdProcessor.GetArgs().empty()) {
            throw std::runtime_error { "no input file" };
            return 0;
        }

        if (cmdProcessor.GetArgs().size() > 1) {
            throw std::runtime_error { "too many input files" };
        }

        options.inputFile = std::move(cmdProcessor.GetArgs().front());
        CompileAndRun(options);
        return 0;
    } catch (const scc::compiler::Exception& ex) {
        bool hasColor = IsErrorColorSupported();
        std::string highlightTextColor = hasColor ? "\e[97m" : "";
        std::string highlightErrorColor = hasColor ? "\e[91m" : "";
        std::string turnOffColor = hasColor ? "\e[0m" : "";

        if (options.inputFile.empty()) {
            std::cerr << "scc";
        } else {
            std::cerr << highlightTextColor << options.inputFile << ":" << ex.startLine << ":" << ex.startColumn;
        }
        std::cerr << ": " << highlightErrorColor << "error: " << highlightTextColor << ex.what() << turnOffColor << std::endl;

        auto prefix = std::format("{:5} | ", ex.startLine);
        auto line = GetFileLine(options.inputFile, ex.startLine);
        std::cerr << prefix << line << std::endl;
        std::cerr << std::string(prefix.length() + ex.startColumn - 1, ' ')
                  << highlightErrorColor << std::string((ex.startLine == ex.endLine ? ex.endColumn : line.length() - 1) - ex.startColumn + 1, '^') << turnOffColor
                  << std::endl;
        return 1;
    } catch (const std::exception& ex) {
        if (IsErrorColorSupported()) {
            std::cerr << "scc: \e[91merror:\e[0m " << ex.what() << std::endl;
        } else {
            std::cerr << "scc: error: " << ex.what() << std::endl;
        }
        return 1;
    }
}

void PrintHelp(const std::string_view& optionsHelp)
{
    std::cout << std::endl
              << "Usage: scc [options] file" << std::endl
              << std::endl
              << optionsHelp << std::endl
              << std::endl;
}

void CompileAndRun(const Options& options)
{
    assert(!options.inputFile.empty());

    // Parse.
    auto scope = Parse(options.inputFile);

    // Translate.
    auto filePath = std::filesystem::path { options.inputFile };
    auto workingFolder = filePath.parent_path() / ".scc";
    std::filesystem::create_directories(workingFolder);

    auto outFile = workingFolder / (filePath.filename().string() + ".cpp");
    scc::compiler::Translator { std::make_shared<std::ofstream>(outFile) }.VisitAstScope(scope);

    if (!options.compileOnly) {
        // Invoke clang++ to compile.
        char result[PATH_MAX];
        result[readlink("/proc/self/exe", result, PATH_MAX)] = '\0';
        auto appPath = std::filesystem::path { result }.parent_path();
        auto stdModulePath = appPath / "std";
        auto stdLibPath = stdModulePath / "libscc.std.a";
        auto exePath = workingFolder / "a.out";
        auto res = std::system(std::format("clang++-18 -std=c++20 -fprebuilt-module-path={} -w {} {} -o {}", stdModulePath.string(), outFile.string(), stdLibPath.string(), exePath.string()).c_str());

        // Run.
        if (!res) {
            std::system(exePath.string().c_str());
        }
    }
}

scc::compiler::AstScope Parse(const std::string& file)
{
    scc::compiler::AstScope scope {};
    scc::compiler::Lexer lexer { std::make_shared<std::ifstream>(file) };
    scc::compiler::Parser {}.ParseCompileUnit(scope, lexer);
    return std::move(scope);
}

std::string GetFileLine(const std::string& file, int line)
{
    std::ifstream in { file };
    std::string str {};
    for (int i = 0; i < line; ++i) {
        if (std::getline(in, str).eof()) {
            return "";
        }
    }
    return str;
}

bool IsErrorColorSupported()
{
    return isatty(STDERR_FILENO);
}