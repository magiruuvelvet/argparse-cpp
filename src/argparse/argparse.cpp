#include "argparse.hpp"

#include "internal/parser.hpp"
#include "internal/printer.hpp"

#include <algorithm>

argparse::ArgumentParser::ArgumentParser(int argc, char **argv, const std::string &shortOptionPrefix, const std::string &longOptionPrefix)
    : shortOptionPrefix(shortOptionPrefix),
      longOptionPrefix(longOptionPrefix)
{
    for (int i = 0; i < argc; ++i)
    {
        this->args.emplace_back(std::string(argv[i]));
    }
}

argparse::ArgumentParser::ArgumentParser(const std::vector<std::string> &args, const std::string &shortOptionPrefix, const std::string &longOptionPrefix)
    : shortOptionPrefix(shortOptionPrefix),
      longOptionPrefix(longOptionPrefix),
      args(args)
{
}

argparse::ArgumentParserResult argparse::ArgumentParser::parse()
{
    // avoid multiple parsing steps
    if (this->parsed)
    {
        return this->parsingResult;
    }

    // do the actual parsing
    const Result result = argparse::parse(
        *this,
        this->args,
        this->arguments,
        this->shortOptionPrefix,
        this->longOptionPrefix,
        this->terminator,
        this->_missingArguments,
        this->_loseArguments,
        this->_remainingArguments
    );
    this->parsed = true;
    this->parsingResult = result;
    return result;
}

const std::string argparse::ArgumentParser::help(
    bool visualizeStringOptions, const std::string &stringOptionSuffix,
    std::uint8_t indentation, std::uint8_t spacing) const
{
    // don't do anything if there are no registered arguments
    if (this->arguments.size() == 0)
    {
        return "";
    }

    return argparse::format(
        *this,
        this->arguments,
        visualizeStringOptions,
        stringOptionSuffix,
        indentation,
        spacing,
        this->shortOptionPrefix,
        this->longOptionPrefix
    );
}

bool argparse::ArgumentParser::exists(const std::string &name) const
{
    if (this->parsingResult != Result::Success)
    {
        return false;
    }

    for (auto&& arg : this->arguments)
    {
        if (arg.present() && (arg.name() == name || arg.shortName() == name || arg.longName() == name))
        {
            return true;
        }
    }

    return false;
}

bool argparse::ArgumentParser::addArgumentInternal(Argument argument)
{
    // reset state before adding
    argument.reset();

    // refuse to add invalid argument
    if (!argument.isValid())
    {
        return false;
    }

    const auto find = [&](const std::list<std::string> &list, const std::string &value) -> bool {
        return std::find_if(list.begin(), list.end(), [&](const auto &item) {
            return item == value;
        }) != list.end();
    };

    // check if already added
    if (argument.longName().size() != 0 && find(this->argumentsLongRegistry, argument.longName()))
    {
        return false;
    }
    if (argument.shortName().size() != 0 && find(this->argumentsShortRegistry, argument.shortName()))
    {
        return false;
    }

    // add copy to internal array
    if (argument.longName().size() != 0)
    {
        this->argumentsLongRegistry.emplace_back(argument.longName());
    }
    if (argument.shortName().size() != 0)
    {
        this->argumentsShortRegistry.emplace_back(argument.shortName());
    }
    this->arguments.emplace_back(argument);
    return true;
}
