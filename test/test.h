#pragma once

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#define ASSERT_THROW_MSG(statement, expected_exception, expected_msg) \
    ASSERT_THROW(                                                     \
        try {                                                         \
            statement;                                                \
        } catch (const expected_exception& e) {                       \
            ASSERT_EQ(std::string { e.what() }, expected_msg);        \
            throw;                                                    \
        },                                                            \
        expected_exception)

#define ASSERT_THROW_COMPILER_EXCEPTION(statement, expected_exception)  \
    do {                                                                \
        auto expectedEx = expected_exception;                           \
        ASSERT_THROW(                                                   \
            try {                                                       \
                statement;                                              \
            } catch (const scc::compiler::Exception& e) {               \
                ASSERT_EQ(e.startLine, expectedEx.startLine);           \
                ASSERT_EQ(e.startColumn, expectedEx.startColumn);       \
                ASSERT_EQ(e.endLine, expectedEx.endLine);               \
                ASSERT_EQ(e.endColumn, expectedEx.endColumn);           \
                ASSERT_EQ(std::string { e.what() }, expectedEx.what()); \
                throw;                                                  \
            },                                                          \
            scc::compiler::Exception);                                  \
    } while (false)

inline std::string ReadFileAsString(const std::filesystem::path& file)
{
    auto ifs = std::ifstream { file };
    return std::string { std::istreambuf_iterator<char> { ifs }, std::istreambuf_iterator<char> {} };
}