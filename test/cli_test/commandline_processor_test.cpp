#include "test/test.h"
#include <vector>

import scc.cli;

class CommandlineProcessorTest : public testing::Test {
protected:
    CommandlineProcessorTest()
    {
        m_commandlineProcessor.RegisterOption('h', "help", "print help", [this] {
            m_isHelpOptionOn = true;
        });

        m_commandlineProcessor.RegisterOption('v', "version", "print version", [this] {
            m_isVersionOptionOn = true;
        });

        m_commandlineProcessor.RegisterOption('s', "only short option", [this] {
            m_isOnlyShortOptionOn = true;
        });

        m_commandlineProcessor.RegisterOption("only-long", "only long option", [this] {
            m_isOnlyLongOptionOn = true;
        });
    }

    void SetCommandLine(const std::vector<const char*>& args)
    {
        m_commandlineProcessor.SetCommandLine(args.size(), args.data());
    }

    scc::cli::CommandlineProcessor m_commandlineProcessor {};
    bool m_isHelpOptionOn {};
    bool m_isVersionOptionOn {};
    bool m_isOnlyShortOptionOn {};
    bool m_isOnlyLongOptionOn {};
};

TEST_F(CommandlineProcessorTest, TestSingleShortOptions)
{
    SetCommandLine({});
    ASSERT_FALSE(m_isHelpOptionOn);
    ASSERT_FALSE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyShortOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_TRUE(m_commandlineProcessor.GetArgs().empty());

    SetCommandLine({ "-h" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_FALSE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyShortOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_TRUE(m_commandlineProcessor.GetArgs().empty());

    SetCommandLine({ "-v" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyShortOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_TRUE(m_commandlineProcessor.GetArgs().empty());

    SetCommandLine({ "-s" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_TRUE(m_isOnlyShortOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_TRUE(m_commandlineProcessor.GetArgs().empty());
}

TEST_F(CommandlineProcessorTest, TestMultipleShortOptions1)
{
    SetCommandLine({ "-h", "-v", "-s" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_TRUE(m_isOnlyShortOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_TRUE(m_commandlineProcessor.GetArgs().empty());
}

TEST_F(CommandlineProcessorTest, TestMultipleShortOptions2)
{
    SetCommandLine({ "-hvs" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_TRUE(m_isOnlyShortOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_TRUE(m_commandlineProcessor.GetArgs().empty());
}

TEST_F(CommandlineProcessorTest, TestArgs1)
{
    SetCommandLine({ "abc" });
    ASSERT_FALSE(m_isHelpOptionOn);
    ASSERT_FALSE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyShortOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 1);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "abc");

    SetCommandLine({ "def", "ghi" });
    ASSERT_FALSE(m_isHelpOptionOn);
    ASSERT_FALSE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyShortOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 2);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "def");
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[1], "ghi");

    SetCommandLine({ "-h", "123", "456" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_FALSE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyShortOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 2);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "123");
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[1], "456");

    SetCommandLine({ "-h", "789", "-v", "a b c" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 2);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "789");
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[1], "a b c");
}

TEST_F(CommandlineProcessorTest, TestArgs2)
{
    SetCommandLine({ "--help", "123", "456" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_FALSE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 2);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "123");
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[1], "456");

    SetCommandLine({ "--help", "789", "--version", "a b c" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 2);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "789");
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[1], "a b c");

    SetCommandLine({ "--help", "789", "--version", "a b c", "--only-long" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_TRUE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 2);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "789");
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[1], "a b c");
}

TEST_F(CommandlineProcessorTest, TestArgs3)
{
    SetCommandLine({ "--help", "789", "-v", "a b c" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 2);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "789");
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[1], "a b c");

    SetCommandLine({ "--help", "789", "-v", "--only-long", "a b c" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_TRUE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 2);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "789");
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[1], "a b c");
}

TEST_F(CommandlineProcessorTest, TestArgs4)
{
    SetCommandLine({ "-hv", "789", "a b c" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_FALSE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 2);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "789");
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[1], "a b c");

    SetCommandLine({ "-hv", "789", "--only-long", "a b c" });
    ASSERT_TRUE(m_isHelpOptionOn);
    ASSERT_TRUE(m_isVersionOptionOn);
    ASSERT_TRUE(m_isOnlyLongOptionOn);
    ASSERT_EQ(m_commandlineProcessor.GetArgs().size(), 2);
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[0], "789");
    ASSERT_EQ(m_commandlineProcessor.GetArgs()[1], "a b c");
}

TEST_F(CommandlineProcessorTest, TestUnknownOptions)
{
    ASSERT_THROW_MSG(SetCommandLine({ "-b" }), std::runtime_error, "unknown option: -b");
    ASSERT_THROW_MSG(SetCommandLine({ "-h", "-b" }), std::runtime_error, "unknown option: -b");
    ASSERT_THROW_MSG(SetCommandLine({ "-c", "-h" }), std::runtime_error, "unknown option: -c");
    ASSERT_THROW_MSG(SetCommandLine({ "-hd" }), std::runtime_error, "unknown option: -d");
    ASSERT_THROW_MSG(SetCommandLine({ "--abc" }), std::runtime_error, "unknown option: --abc");
    ASSERT_THROW_MSG(SetCommandLine({ "-h", "--abc" }), std::runtime_error, "unknown option: --abc");
    ASSERT_THROW_MSG(SetCommandLine({ "--abc", "-h" }), std::runtime_error, "unknown option: --abc");
    ASSERT_THROW_MSG(SetCommandLine({ "--abc", "--help" }), std::runtime_error, "unknown option: --abc");
    ASSERT_THROW_MSG(SetCommandLine({ "--help", "--abc" }), std::runtime_error, "unknown option: --abc");
}