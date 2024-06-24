#include "test/test.h"

#include <filesystem>
#include <sstream>

import scc.compiler;

using namespace scc::compiler;

class TranslatorTest : public testing::Test {
protected:
    static std::filesystem::path s_testFolder;

    std::string Translate(const std::filesystem::path& path)
    {
        auto scope = std::make_shared<AstScope>();
        Lexer lexer { std::make_shared<std::istringstream>(ReadFileAsString(path)) };
        Parser {}.ParseCompileUnit(scope, lexer);

        auto output = std::make_shared<std::ostringstream>();
        Translator { output }.VisitAstScope(*scope);
        return output->str();
    }

    void RunTest(std::string testFileId)
    {
        auto actual = Translate(s_testFolder / (testFileId + ".scc"));
        auto expected = ReadFileAsString(s_testFolder / (testFileId + ".expected"));
        ASSERT_EQ(actual, expected);
    }
};

std::filesystem::path TranslatorTest::s_testFolder { []() {
    char result[PATH_MAX];
    auto count = readlink("/proc/self/exe", result, PATH_MAX);
    result[count] = '\0';
    for (auto path = std::filesystem::path { result }; !path.empty() && path != path.parent_path(); path = path.parent_path()) {
        if (auto samplePath = path / "test" / "compiler_test" / "translator_test_data"; std::filesystem::exists(samplePath)) {
            return samplePath;
        }
    }
    throw std::runtime_error { "Can't find 'test/compiler_test/translator_test_data' directory." };
}() };

TEST_F(TranslatorTest, Empty)
{
    RunTest("empty");
}

TEST_F(TranslatorTest, HelloWorld)
{
    RunTest("hello_world");
}