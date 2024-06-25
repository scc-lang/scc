module;

#include <format>
#include <memory>
#include <ostream>
#include <stack>

export module scc.compiler:printer;

namespace scc::compiler {

export struct Printer final {
    Printer(std::shared_ptr<std::ostream> out)
        : m_out { std::move(out) }
    {
    }

    template <class... Args>
    void Print(const std::string_view& fmt, Args&&... args)
    {
        auto str = std::vformat(fmt, std::make_format_args(args...));
        auto s = 0;
        while (s < str.length()) {
            if (m_newLine && !m_indents.empty()) {
                *m_out << m_indents.top();
            }

            auto e = str.find('\n', s);
            if (e == std::string::npos) {
                *m_out << str.c_str() + s;
                m_newLine = false;
                break;
            } else {
                *m_out << std::string_view { str.c_str() + s, e - s + 1 };
                m_newLine = true;
                s = e + 1;
            }
        }
    }

    template <class... Args>
    void Println(const std::string_view& fmt, Args&&... args)
    {
        Print(fmt, std::forward<Args>(args)...);
        *m_out << std::endl;
        m_newLine = true;
    }

    void Println()
    {
        *m_out << std::endl;
        m_newLine = true;
    }

    void PushIndent()
    {
        int indents = (m_indents.empty() ? 0 : m_indents.top().length()) + m_defaultIdent;
        m_indents.emplace(indents, ' ');
    }

    void PopIndent()
    {
        m_indents.pop();
    }

private:
    bool m_newLine {};
    int m_defaultIdent { 4 };
    std::stack<std::string> m_indents {};
    std::shared_ptr<std::ostream> m_out {};
};

}