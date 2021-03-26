#include <bandit/bandit.h>

#include "benchmark.hpp"

using namespace snowhouse;
using namespace bandit;

#include <argparse/argparse.hpp>

go_bandit([]{
    describe("Printer", []{
        using namespace argparse;

        benchmark_it("[printerDefault]", [&]{
            ArgumentParser parser({"app"});
            parser.addHelpOption("Print this help and exit");
            parser.addArgument("", "version", "Show application version and exit", Argument::Boolean);
            parser.addArgument("v", "verbose", "Enable verbose output", Argument::Boolean);
            parser.addArgument("s", "", "Just a short option");

            const auto fmt = parser.help();
            const auto expected =
                "    -h, --help              Print this help and exit\n"
                "    --version               Show application version and exit\n"
                "    -v, --verbose           Enable verbose output\n"
                "    -s                      Just a short option\n";

            AssertThat(fmt, Equals(expected));
        });

        benchmark_it("[printerCustom]", [&]{
            ArgumentParser parser({"app"});
            parser.addHelpOption("Print this help and exit");
            parser.addArgument("", "version", "Show application version and exit", Argument::Boolean);
            parser.addArgument("v", "verbose", "Enable verbose output", Argument::Boolean);
            parser.addArgument("s", "", "Just a short option");

            const auto fmt = parser.help(0, 0);
            const auto expected =
                "-h, --help    Print this help and exit\n"
                "--version     Show application version and exit\n"
                "-v, --verbose Enable verbose output\n"
                "-s            Just a short option\n";

            AssertThat(fmt, Equals(expected));
        });

        benchmark_it("[printerCustomVisualized]", [&]{
            ArgumentParser parser({"app"});
            parser.addHelpOption("Print this help and exit");
            parser.addArgument("", "version", "Show application version and exit", Argument::Boolean);
            parser.addArgument("v", "verbose", "Enable verbose output", Argument::Boolean);
            parser.addArgument("s", "", "Just a short option");

            const auto fmt = parser.help(true);
            const auto expected =
                "    -h, --help              Print this help and exit\n"
                "    --version               Show application version and exit\n"
                "    -v, --verbose           Enable verbose output\n"
                "    -s [value]              Just a short option\n";

            AssertThat(fmt, Equals(expected));
        });

        benchmark_it("[unicode]", [&]{
            ArgumentParser parser({"app"});
            parser.addHelpOption("このヘルプを表示");
            parser.addArgument("コ", "コマンド", "コマンド");

            const auto fmt = parser.help();
            const auto expected =
                "    -h, --help                このヘルプを表示\n"
                "    -コ, --コマンド           コマンド\n";

            AssertThat(fmt, Equals(expected));
        });
    });
});
