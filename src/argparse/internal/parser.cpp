#include "parser.hpp"

#include <string>
#include <optional>

using namespace argparse;

using Arguments = ArgumentParser::Arguments;

namespace {

std::string shortOptionPrefix;
std::string longOptionPrefix;

enum class OptionType
{
    Short,
    Long,
    Value,
    Unknown,
};

// check registered arguments for required ones
static bool has_required_arguments(Arguments &registeredArguments)
{
    for (auto&& arg : registeredArguments)
    {
        if (arg.required())
        {
            return true;
        }
    }

    return false;
}

// is the option a long option
static bool is_long_option(const std::string &option)
{
    return
        option.size() >= longOptionPrefix.size() &&
        option.substr(0, longOptionPrefix.size()) == longOptionPrefix;
}

// is the option a short option
static bool is_short_option(const std::string &option)
{
    return
        option.size() >= shortOptionPrefix.size() &&
        option.substr(0, shortOptionPrefix.size()) == shortOptionPrefix && !is_long_option(option);
}

// is the option a value
static bool is_value(const std::string &option)
{
    return !is_short_option(option) && !is_long_option(option);
}

// get option type as enum
static const OptionType get_option_type(const std::string &option)
{
    if (is_short_option(option))
    { return OptionType::Short; }
    else if (is_long_option(option))
    { return OptionType::Long; }
    else if (is_value(option))
    { return OptionType::Value; }
    else return OptionType::Unknown;
}

// get option name without prefix or value
static std::string get_option_name(const std::string &option)
{
    switch (get_option_type(option))
    {
        case OptionType::Long:    return option.substr(longOptionPrefix.size());
        case OptionType::Short:   return option.substr(shortOptionPrefix.size());
        case OptionType::Value:   return option;
        case OptionType::Unknown: return option;
    }
}

static Argument *find_argument(Arguments &registeredArguments, const std::string &name)
{
    for (auto& argument : registeredArguments)
    {
        if (argument.name() == name || argument.longName() == name || argument.shortName() == name)
        {
            return &argument;
        }
    }

    return nullptr;
}

} // anonymous namespace

argparse::ArgumentParserResult argparse::parse(
    const argparse::ArgumentParser &parserInstance,
    const std::vector<std::string> &args, Arguments &registeredArguments,
    const std::string &_shortOptionPrefix, const std::string &_longOptionPrefix,
    const std::string &terminator,
    StringList &missingArguments, StringList &loseArguments, StringList &remainingArguments)
{
    using Res = ArgumentParserResult;

    shortOptionPrefix = _shortOptionPrefix;
    longOptionPrefix = _longOptionPrefix;

    // check for required arguments
    const bool requiredArguments = has_required_arguments(registeredArguments);

    // args contains only the command and arguments are required
    if (args.size() <= 1 && requiredArguments)
    {
        return Res::InsufficientArguments;
    }
    // no args given and no required arguments
    else if (args.size() <= 1)
    {
        return Res::Success;
    }

    for (auto i = 1u; i < args.size(); ++i)
    {
        if (terminator.size() > 0 && args.at(i) == terminator)
        {
            if ((i+1) < args.size())
            {
                // NOTE: in the D implemenation this is one line of code, thanks to array slicing
                // remainingArguments = args[i+1..$];
                const auto begin = args.cbegin() + (i+1);
                const auto end   = args.cend();
                for (auto it = begin; it < end; ++it)
                {
                    remainingArguments.emplace_back(*it);
                }
            }
            break;
        }

        // get next option or null
        const auto next_option = [&]() -> std::optional<std::string> {
            if ((i+1) >= args.size())
            {
                return std::nullopt;
            }
            else
            {
                return args.at(i+1);
            }
        };

        const auto name = get_option_name(args.at(i));
        const auto type = get_option_type(args.at(i));

        if (type == OptionType::Long || type == OptionType::Short)
        {
            auto argument = find_argument(registeredArguments, name);

            if (argument == nullptr)
            {
                continue;
            }

            // set argument to found
            argument->setPresent(true);

            if (argument->type() == Argument::Boolean)
            {
                argument->setValue("true");
                continue;
            }
            else if (argument->type() == Argument::String)
            {
                const auto next = next_option();
                if (!next.has_value())
                {
                    argument->setValue(""); // assume empty string, rather than an error
                    continue;
                }
                else
                {
                    if (is_value(next.value()))
                    {
                        argument->setValue(next.value());
                        i++;
                        continue;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
        }
        else if (type == OptionType::Value || type == OptionType::Unknown)
        {
            // found a lose argument
            loseArguments.emplace_back(name);
        }
    }

    // check if required arguments are missing
    for (auto& argument : registeredArguments)
    {
        if (argument.required() && !argument.present())
        {
            // check if there is a default value to fall back
            if (argument.hasDefaultValue())
            {
                // make argument present even though it was omitted on the command line
                // but don't set a value on it, so the default value is taken instead
                argument.setPresent(true);
            }
            else
            {
                missingArguments.emplace_back(argument.name());
            }
        }
    }

    if (missingArguments.size() != 0)
    {
        return Res::MissingArgument;
    }

    return Res::Success;
}
