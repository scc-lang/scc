#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

import scc.cli;
import scc.compiler;

struct Options {
    std::string inputFile {};
    bool needHelp {};
};

void PrintHelp(const std::string_view& optionsHelp);
void CompileAndRun(const Options& options);
std::shared_ptr<scc::compiler::AstScope> Parse(const std::string& file);

int main(int argc, const char* const argv[])
{
    try {
        Options options {};
        scc::cli::CommandlineProcessor cmdProcessor {};
        cmdProcessor.RegisterOption('h', "help", "Print help", [&options] { options.needHelp = true; });
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
    } catch (const std::exception& ex) {
        std::cerr << "scc: \e[91merror:\e[0m " << ex.what() << std::endl;
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
    scc::compiler::Translator { std::make_shared<std::ofstream>(outFile) }.VisitAstScope(*scope);

    // Invoke clang++ to compile.
    char result[PATH_MAX];
    result[readlink("/proc/self/exe", result, PATH_MAX)] = '\0';
    auto appPath = std::filesystem::path { result }.parent_path();
    auto stdModulePath = appPath / "std";
    auto stdLibPath = stdModulePath / "libscc.std.a";
    auto exePath = workingFolder / "a.out";
    std::system(std::format("clang++-18 -std=c++20 -fprebuilt-module-path={} -w {} {} -o {}", stdModulePath.string(), outFile.string(), stdLibPath.string(), exePath.string()).c_str());

    // Run.
    std::system(exePath.string().c_str());
}

std::shared_ptr<scc::compiler::AstScope> Parse(const std::string& file)
{
    auto scope = std::make_shared<scc::compiler::AstScope>();
    scc::compiler::Lexer lexer { std::make_shared<std::ifstream>(file) };
    scc::compiler::Parser {}.ParseCompileUnit(scope, lexer);
    return std::move(scope);
}