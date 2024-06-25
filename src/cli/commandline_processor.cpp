module;

#include <format>
#include <functional>
#include <memory>
#include <unordered_map>

export module scc.cli:commandline_processor;

namespace scc::cli {

struct Option {
    char shortSwitch {};
    std::string longSwitch {};
    std::string description {};
    std::function<void()> action {};
};

export struct CommandlineProcessor {
    void RegisterOption(char shortSwitch, std::string longSwitch, std::string description, std::function<void()> action)
    {
        auto option = std::make_shared<Option>(Option {
            .shortSwitch = shortSwitch,
            .longSwitch = longSwitch,
            .description = std::move(description),
            .action = std::move(action),
        });
        m_shortOptions.emplace(shortSwitch, option);
        if (!longSwitch.empty()) {
            m_longOptions.emplace(std::move(longSwitch), option);
        }
        m_options.push_back(std::move(option));
    }

    void RegisterOption(char shortSwitch, std::string description, std::function<void()> action)
    {
        RegisterOption(shortSwitch, "", std::move(description), std::move(action));
    }

    void RegisterOption(std::string longSwitch, std::string description, std::function<void()> action)
    {
        RegisterOption('\0', std::move(longSwitch), std::move(description), std::move(action));
    }

    void SetCommandLine(int argc, const char* const argv[])
    {
        m_args.clear();

        for (int i = 0; i < argc; ++i) {
            const auto arg = argv[i];
            if (arg[0] == '-') {
                if (arg[1] == '-') {
                    if (auto it = m_longOptions.find(arg + 2); it == m_longOptions.end()) {
                        throw std::runtime_error { std::format("unknown option: {}", arg) };
                    } else {
                        it->second->action();
                    }
                } else {
                    for (const auto* p = &arg[1]; *p; ++p) {
                        if (auto it = m_shortOptions.find(*p); it == m_shortOptions.end()) {
                            throw std::runtime_error { std::format("unknown option: -{}", *p) };
                        } else {
                            it->second->action();
                        }
                    }
                }
            } else {
                m_args.emplace_back(arg);
            }
        }
    }

    const std::vector<std::string>& GetArgs() const
    {
        return m_args;
    }

    std::string GetOptionsHelp()
    {
        std::string str { "Options:\n\n" };
        for (const auto& opt : m_options) {
            if (opt->longSwitch.empty()) {
                str += std::format("\t-{}\t\t\t{}\n", opt->shortSwitch, opt->description);
            } else {
                str += std::format("\t-{}, --{}\t\t{}\n", opt->shortSwitch, opt->longSwitch, opt->description);
            }
        }
        return str;
    }

private:
    std::vector<std::shared_ptr<Option>> m_options;
    std::unordered_map<char, std::shared_ptr<Option>> m_shortOptions;
    std::unordered_map<std::string, std::shared_ptr<Option>> m_longOptions;
    std::vector<std::string> m_args;
};

}