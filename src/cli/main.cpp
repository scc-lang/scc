#include <format>
#include <iostream>
#include <stdexcept>

import scc.cli;

struct Options {
    std::string inputFile {};
    bool needHelp {};
};

void PrintHelp(const std::string_view& optionsHelp);

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
        std::cout << "TODO: process input file: " << options.inputFile << std::endl;
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