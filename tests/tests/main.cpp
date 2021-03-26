#include <bandit/bandit.h>

#include "benchmark.hpp"

#include "argument_tests.hpp"
#include "parser_tests.hpp"
#include "printer_tests.hpp"

bool check_has_info_reporter(int argc, char **argv)
{
    //for (auto&& arg : args)
    for (auto i = 0; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--reporter=info") == 0)
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv)
{
    const bool has_info_reporter = check_has_info_reporter(argc, argv);
    auto *logger = static_cast<bandit::detail::default_benchmark_logger*>(bandit::detail::benchmark::registered_logger());
    if (!has_info_reporter)
    {
        logger->enable_printing(false);
    }
    else
    {
        logger->set_precision(2);
    }

    return bandit::run(argc, argv);
}
