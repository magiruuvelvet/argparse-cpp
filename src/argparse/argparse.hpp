#ifndef LIBARGPARSE_ARGPARSE_HPP
#define LIBARGPARSE_ARGPARSE_HPP

#include "argument.hpp"

#include <list>
#include <vector>
#include <limits>
#include <cstdint>

namespace argparse {

struct ArgumentParser final
{
public:
    /**
     * Parsing result status codes.
     */
    enum class Result : unsigned short
    {
        Success = 0,
        InsufficientArguments,      // too less arguments given
        MissingArgument,            // a required argument not provided

        Unknown = std::numeric_limits<unsigned short>::max(),
    };

    /**
     * Returns a list of missing arguments once they were `parsed()`.
     */
    inline const std::list<std::string> &missingArguments() const
    {
        return this->_missingArguments;
    }

    /**
     * Returns a list of all lose arguments which didn't belong to an string option.
     */
    inline const std::list<std::string> &loseArguments() const
    {
        return this->_loseArguments;
    }

    /**
     * Returns a list of all remaining arguments which weren't parsed
     * due to termination.
     */
    inline const std::list<std::string> &remainingArguments() const
    {
        return this->_remainingArguments;
    }

    /**
     * Create an instance of the ArgumentParser and pass it the given
     * arguments as argument. The constructor only prepares some internals.
     * To do the actual parsing, call the `parse()` method.
     */
    ArgumentParser(int argc, char **argv, const std::string &shortOptionPrefix = "-", const std::string &longOptionPrefix = "--");

    /**
     * Create an instance of the ArgumentParser and pass it the given
     * arguments as argument. The constructor only prepares some internals.
     * To do the actual parsing, call the `parse()` method.
     */
    ArgumentParser(const std::vector<std::string> &args, const std::string &shortOptionPrefix = "-", const std::string &longOptionPrefix = "--");

    /**
     * Adds a new argument to the argument parser.
     *
     * If only one parameter is given it is assumed it is of type `Argument`.
     * If multiple parameters are given, they are forwarded to the `Argument` constructor.
     */
    template<typename... Args>
    inline bool addArgument(Args&&... args)
    {
        if (this->parsed) return false;

        if constexpr (sizeof...(args) == 1)
        {
            return this->addArgumentInternal(args...);
        }
        else
        {
            return this->addArgumentInternal(Argument(args...));
        }
    }

    /**
     * Convenience function to add a help option.
     * Only the description must be provided.
     */
    inline bool addHelpOption(const std::string &description)
    {
        if (this->parsed) return false;
        return this->addArgument(Argument("h", "help", description, Argument::Boolean, false));
    }

    /**
     * Enables command line parsing termination.
     * Defaults to `--`.
     */
    inline void setTerminator(const std::string &terminator = "--")
    {
        if (this->parsed) return;
        this->terminator = terminator;
    }

    /**
     * Do the command line parsing. The status is returned
     * as an enum to check what happened.
     *
     * There is no exception based error handling available.
     */
    Result parse();

    /**
     * Formats a string for printing all registered command line arguments
     * and their description in a pretty way.
     */
    inline const std::string help(std::uint8_t indentation = 4, std::uint8_t spacing = 10) const
    {
        return this->help(false, "", indentation, spacing);
    }

    /**
     * Formats a string for printing all registered command line arguments
     * and their description in a pretty way.
     *
     * This overload supports visualizing string options with a custom
     * value suffix (defaults to `" [value]"`).
     */
    const std::string help(
        bool visualizeStringOptions, const std::string &stringOptionSuffix = " [value]",
        std::uint8_t indentation = 4, std::uint8_t spacing = 10) const;

    /**
     * Is the given argument present on the command line?
     */
    bool exists(const std::string &name) const;

    /**
     * Returns the parsed value of the given argument.
     *
     * If the argument was not found a default initialized value
     * will be returned.
     *
     * If a casting error ocurred a default initialized value
     * will be returned.
     *
     * If you need to check for the existence of a command line argument,
     * use the `exists()` method instead.
     *
     * For boolean switches always use the `exists()` method, rather than
     * casting a string representation of the boolean into an actual boolean.
     */
    template<typename T = std::string>
    auto get(const std::string name, bool *ok = nullptr) const
    {
        if (this->parsingResult != Result::Success)
        {
            if (ok) *ok = false;
            return T{};
        }

        if constexpr (std::is_same_v<T, bool>)
        {
            if (ok) *ok = true;
            return this->exists(name);
        }
        else
        {
            for (auto&& arg : this->arguments)
            {
                if (arg.present() && (arg.name() == name || arg.shortName() == name || arg.longName() == name))
                {
                    if (ok) *ok = true;
                    return arg.get<T>(ok);
                }
            }

            if (ok) *ok = false;
            return T{};
        }
    }

public:
    using Arguments = std::list<Argument>;

private:
    ArgumentParser() = delete;                       // disallow default construction
    ArgumentParser(const ArgumentParser &) = delete; // disallow copy

    bool parsed = false;
    Result parsingResult = Result::Unknown;

    // copy of command line arguments
    std::vector<std::string> args;

    // internal arguments array
    Arguments arguments;

    std::list<std::string> argumentsLongRegistry;
    std::list<std::string> argumentsShortRegistry;

    std::string shortOptionPrefix = "-";
    std::string longOptionPrefix = "--";
    std::string terminator = "";

    std::list<std::string> _missingArguments;
    std::list<std::string> _loseArguments;
    std::list<std::string> _remainingArguments;

private:
    bool addArgumentInternal(Argument argument);
};

using ArgumentParserResult = ArgumentParser::Result;

} // namespace argparse

#endif // LIBARGPARSE_ARGPARSE_HPP
