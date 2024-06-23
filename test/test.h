#pragma once

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