#ifndef BANDIT_BENCHMARK_HPP
#define BANDIT_BENCHMARK_HPP

/**
 * Basic benchmarker for the bandit unit test framework.
 *
 * Only supports the "info" reporter.
 *
 */

#include <bandit/bandit.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <deque>

namespace bandit
{
    namespace detail
    {
        struct benchmark_logger
        {
            virtual void report(const std::string &desc, double ms) = 0;

            virtual ~benchmark_logger() = default;
        };

        struct default_benchmark_logger final : public benchmark_logger
        {
        public:
            void enable_printing(bool enable)
            {
                this->print_enabled = enable;
            }

            void set_precision(std::size_t precision)
            {
                this->precision = precision;
            }

            void report(const std::string &desc, double ms) override
            {
                // append time to info reporter
                if (print_enabled)
                {
                    std::cout << std::setprecision(this->precision) << std::fixed << " " << ms << " ms";
                }

                // append log item
                items.emplace_back(item{desc, ms});
            }

            double total_time() const
            {
                double time = 0;
                for (auto&& item : items)
                {
                    time += item.ms;
                }
                return time;
            }

            struct item final
            {
                const std::string desc;
                const double ms;
            };

        private:
            std::deque<item> items;
            std::size_t precision = 5;
            bool print_enabled = true;
        };

        struct disabled_benchmark_logger final : public benchmark_logger
        {
            void report(const std::string &, double) override
            {
            }
        };

        struct benchmark final
        {
            static void register_logger(benchmark_logger *logger)
            {
                if (!logger)
                {
                    throw std::runtime_error("Invalid null logger passed to bandit::detail::benchmark::register_logger");
                }
                registered_logger() = logger;
            }

            static benchmark_logger* &registered_logger()
            {
                static benchmark_logger *logger = nullptr;
                static std::unique_ptr<benchmark_logger> default_logger;

                if (!logger)
                {
                    default_logger = std::make_unique<default_benchmark_logger>();
                    logger = default_logger.get();
                }

                return logger;
            }
        };

        struct benchmark_timer final
        {
        public:
            using timer_t = std::chrono::high_resolution_clock::time_point;

            void stop()
            {
                end = std::chrono::high_resolution_clock::now();
            }

            double delta_ms()
            {
                return std::chrono::duration<double, std::milli>(end - start).count();
            }

        private:
            const timer_t start = std::chrono::high_resolution_clock::now();
            timer_t end;
        };
    }

    inline void benchmark_it(const std::string &desc, const std::function<void()> &func,
                             bool hard_skip = false, detail::controller_t &controller = detail::registered_controller())
    {
        it(desc, [&]{
            // run original test function and measure its execution time
            detail::benchmark_timer timer;
            func();
            timer.stop();

            // report results
            auto logger = detail::benchmark::registered_logger();
            logger->report(desc, timer.delta_ms());

        }, hard_skip, controller);
    }
}

#endif // BANDIT_BENCHMARK_HPP
