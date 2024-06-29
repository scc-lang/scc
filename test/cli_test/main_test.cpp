#include "test/test.h"
#include <format>

class MainTest : public testing::Test {
protected:
    static std::once_flag s_once;
    static std::filesystem::path s_sccExePath;
    static std::filesystem::path s_testDataFolder;

    MainTest()
    {
        std::call_once(s_once, [] {
            char result[PATH_MAX];
            auto count = readlink("/proc/self/exe", result, PATH_MAX);
            result[count] = '\0';
            for (auto path = std::filesystem::path { result }; !path.empty() && path != path.parent_path(); path = path.parent_path()) {
                if (auto sccPath = path / "build" / "src" / "cli" / "scc"; std::filesystem::exists(sccPath)) {
                    s_sccExePath = std::move(sccPath);
                    break;
                }
            }
            if (s_sccExePath.empty()) {
                throw std::runtime_error { "Can't find 'build/src/cli/scc'." };
            }

            for (auto path = std::filesystem::path { result }; !path.empty() && path != path.parent_path(); path = path.parent_path()) {
                if (auto samplePath = path / "test" / "cli_test" / "main_test_data"; std::filesystem::exists(samplePath)) {
                    s_testDataFolder = std::move(samplePath);
                    break;
                }
            }
            if (s_testDataFolder.empty()) {
                throw std::runtime_error { "Can't find 'test/cli_test/main_test_data' directory." };
            }
        });
    }

    void RunTest(std::string testId)
    {
        auto inputFile = s_testDataFolder / testId / (testId + ".scc");
        auto expectedOutputFile = s_testDataFolder / testId / (testId + ".result");
        auto outputFile = s_testDataFolder / testId / ".scc" / "a.output";
        std::filesystem::create_directories(outputFile.parent_path());
        std::system(std::format("{} {} > {} 2>&1", s_sccExePath.c_str(), inputFile.c_str(), outputFile.c_str()).c_str());

        auto actual = ReadFileAsString(outputFile);
        auto expected = ReadFileAsString(expectedOutputFile);
        ASSERT_EQ(actual, expected);
    }
};

std::once_flag MainTest::s_once;
std::filesystem::path MainTest::s_sccExePath;
std::filesystem::path MainTest::s_testDataFolder;

TEST_F(MainTest, HelloWorld)
{
    RunTest("hello_world");
}

TEST_F(MainTest, FahrenheitCelsiusTable)
{
    RunTest("fahrenheit_celsius_table");
}

TEST_F(MainTest, FibonacciSequence)
{
    RunTest("fibonacci_sequence");
}