#include <bandit/bandit.h>

#include "benchmark.hpp"

using namespace snowhouse;
using namespace bandit;

#include <argparse/argparse.hpp>

go_bandit([]{
    describe("Parser", []{
        using namespace argparse;

        static const std::vector<std::string> args1 = {"app", "--help"};
        static const std::vector<std::string> args2 = {"app", "--version"};

        const auto registerDefaultArguments = [](ArgumentParser &parser) {
            parser.addHelpOption("Print this help and quit");
            parser.addArgument("v", "version", "Show application version", Argument::Boolean);
        };

        // this is purely a compiler test to check if the variadic template works correctly
        benchmark_it("[addArgument]", [&]{
            ArgumentParser parser(0, nullptr);
            parser.addArgument(Argument("", "", ""));
            parser.addArgument("", "", "");
        });

        benchmark_it("[simple1]", [&]{
            ArgumentParser parser(args1);
            registerDefaultArguments(parser);
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("help"), IsTrue());
            AssertThat(parser.exists("version"), IsFalse());
        });

        benchmark_it("[simple2]", [&]{
            ArgumentParser parser(args2);
            registerDefaultArguments(parser);
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("help"), IsFalse());
            AssertThat(parser.exists("version"), IsTrue());
        });

        benchmark_it("[requiredArgument]", [&]{
            ArgumentParser parser(args1);
            registerDefaultArguments(parser);
            AssertThat(parser.addArgument("", "required", "", Argument::String, true), IsTrue());
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::MissingArgument));
            AssertThat(parser.exists("required"), IsFalse());
            AssertThat(parser.missingArguments(), Equals(std::list<std::string>{"required"}));

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{}));
        });

        benchmark_it("[requiredArgumentDefault]", [&]{
            ArgumentParser parser(args1);
            registerDefaultArguments(parser);
            AssertThat(parser.addArgument("", "required", "", "default", Argument::String, true), IsTrue());
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("required"), IsTrue());
            AssertThat(parser.missingArguments(), Equals(std::list<std::string>{}));
            AssertThat(parser.get("required"), Equals("default"));

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{}));


            ArgumentParser parser2({"app", "--required", "another_value"});
            registerDefaultArguments(parser2);
            AssertThat(parser2.addArgument("", "required", "", "default", Argument::String, true), IsTrue());
            const auto res2 = parser2.parse();

            AssertThat(res2, Equals(ArgumentParserResult::Success));
            AssertThat(parser2.exists("required"), IsTrue());
            AssertThat(parser2.missingArguments(), Equals(std::list<std::string>{}));
            AssertThat(parser2.get("required"), Equals("another_value"));

            AssertThat(parser2.loseArguments(), Equals(std::list<std::string>{}));
        });

        benchmark_it("[booleanSwitch]", [&]{
            ArgumentParser parser({"app", "--enabled", "value", "--unused", "abc"});
            AssertThat(parser.addArgument("e", "enabled", "", Argument::Boolean), IsTrue());
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("e"), IsTrue());
            AssertThat(parser.exists("enabled"), IsTrue());
            AssertThat(parser.get("enabled"), Equals("true")); // boolean switches don't have values

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{"value", "abc"}));
        });

        benchmark_it("[valueSwitch]", [&]{
            ArgumentParser parser({"app", "--enabled", "--value", "abc"});
            AssertThat(parser.addArgument("e", "enabled", "", Argument::Boolean), IsTrue());
            AssertThat(parser.addArgument("", "value", ""), IsTrue());
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("e"), IsTrue());
            AssertThat(parser.exists("enabled"), IsTrue());
            AssertThat(parser.get("enabled"), Equals("true"));

            AssertThat(parser.get("value"), Equals("abc"));
            AssertThat(parser.get("nonexistend"), Equals(""));

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{}));
        });

        benchmark_it("[multipleValues]", [&]{
            ArgumentParser parser({"app", "--value", "abc", "def", "--value2", "xyz"});
            registerDefaultArguments(parser);
            AssertThat(parser.addArgument("", "value", ""), IsTrue());
            AssertThat(parser.addArgument("", "value2", ""), IsTrue());
            AssertThat(parser.addArgument("", "value", ""), IsFalse());
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("help"), IsFalse());
            AssertThat(parser.get("value"), Equals("abc"));
            AssertThat(parser.get("value2"), Equals("xyz"));

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{"def"}));
        });

        benchmark_it("[castingTestSuccess]", [&]{
            ArgumentParser parser({"app", "--value", "10"});
            parser.addArgument("", "value", "");
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("value"), IsTrue());

            bool ok;
            auto value = parser.get<int>("value", &ok);
            AssertThat(ok, IsTrue());
            AssertThat(value, Equals(10));

            value = parser.get<int>("value");
            AssertThat(value, Equals(10));

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{}));
        });

        benchmark_it("[castingTestFailure]", [&]{
            ArgumentParser parser({"app", "--value", "3.1415"});
            parser.addArgument("", "value", "");
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("value"), IsTrue());

            bool ok;
            auto value = parser.get<int>("value", &ok);
            AssertThat(ok, IsFalse());
            AssertThat(value, Equals(0));

            value = parser.get<int>("value");
            AssertThat(value, Equals(0));

            auto value2 = parser.get<float>("value", &ok);
            AssertThat(ok, IsTrue());
            AssertThat(value2, IsGreaterThanOrEqualTo(3.1f)); // don't do exact comparisson due to floating point madness

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{}));
        });

        benchmark_it("[unicode]", [&]{
            ArgumentParser parser({"app", "--コマンド", "表示", "--version"});
            registerDefaultArguments(parser);
            parser.addArgument("コ", "コマンド", "");
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("help"), IsFalse());
            AssertThat(parser.exists("version"), IsTrue());
            AssertThat(parser.exists("コ"), IsTrue());
            AssertThat(parser.exists("コマンド"), IsTrue());
            AssertThat(parser.get("コマンド"), Equals("表示"));
            AssertThat(parser.get("コ"), Equals("表示"));

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{}));
        });

        benchmark_it("[loseArguments]", [&]{
            ArgumentParser parser({"app", "--value", "abc", "def", "--value2", "xyz", "lose1", "lose2", "--unused", "lose3"});
            registerDefaultArguments(parser);
            AssertThat(parser.addArgument("", "value", ""), IsTrue());
            AssertThat(parser.addArgument("", "value2", ""), IsTrue());
            AssertThat(parser.addArgument("", "value", ""), IsFalse());
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("help"), IsFalse());
            AssertThat(parser.get("value"), Equals("abc"));
            AssertThat(parser.get("value2"), Equals("xyz"));
            AssertThat(parser.exists("unused"), IsFalse());

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{"def", "lose1", "lose2", "lose3"}));
        });

        benchmark_it("[termination]", [&]{
            ArgumentParser parser({"app", "--value", "abc", "--", "def", "--value2", "xyz", "lose1", "lose2", "--unused", "lose3"});
            registerDefaultArguments(parser);
            AssertThat(parser.addArgument("", "value", ""), IsTrue());
            AssertThat(parser.addArgument("", "value2", ""), IsTrue());
            AssertThat(parser.addArgument("", "value", ""), IsFalse());
            parser.setTerminator();
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("help"), IsFalse());
            AssertThat(parser.get("value"), Equals("abc"));
            AssertThat(parser.get("value2"), Equals(""));
            AssertThat(parser.exists("unused"), IsFalse());

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{}));
            AssertThat(parser.remainingArguments(), Equals(std::list<std::string>{"def", "--value2", "xyz", "lose1", "lose2", "--unused", "lose3"}));
        });

        benchmark_it("[termination2]", [&]{
            ArgumentParser parser({"app", "--value", "abc", "--"});
            registerDefaultArguments(parser);
            parser.setTerminator();
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{"abc"}));
            AssertThat(parser.remainingArguments(), Equals(std::list<std::string>{}));
        });

        benchmark_it("[emptyStringArgument]", [&]{
            ArgumentParser parser({"app", "--option"});
            parser.addArgument("", "option", "");
            const auto res = parser.parse();

            AssertThat(res, Equals(ArgumentParserResult::Success));
            AssertThat(parser.exists("option"), IsTrue());
            AssertThat(parser.get("option"), Equals(""));

            AssertThat(parser.loseArguments(), Equals(std::list<std::string>{}));
        });
    });
});
