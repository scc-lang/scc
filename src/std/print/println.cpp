module;

#include <format>

export module scc.std:println;

namespace scc::std {

export template <class... Args>
void println(const ::std::string_view& fmt, Args&&... args)
{
    printf("%s\n", ::std::vformat(fmt, ::std::make_format_args(args...)).c_str());
}

export void println()
{
    printf("\n");
}

}