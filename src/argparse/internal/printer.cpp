#include "printer.hpp"

#include "wcwidth.hpp"

#include <list>
#include <string>
#include <locale>
#include <codecvt>

using namespace argparse;

using Arguments = ArgumentParser::Arguments;

namespace {

static std::uint64_t wcwidth_string(const std::string &str)
{
    std::uint64_t length = 0;

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv;
    try
    {
        const std::u32string utf32 = utf32conv.from_bytes(str);
        for (auto&& codepoint : utf32)
        {
            length += wcwidth(codepoint);
        }
    }
    catch (...)
    {
    }

    return length;
}

static std::string make_spaces(std::uint8_t length)
{
    std::string spaces;
    for (std::uint8_t i = 0; i < length; ++i)
    {
        spaces += ' ';
    }
    return spaces;
}

} // anonymous namespace

const std::string argparse::format(
    const argparse::ArgumentParser &parserInstance, const Arguments &arguments,
    bool visualizeStringOptions, const std::string &stringOptionSuffix,
    std::uint8_t indentation, std::uint8_t spacing,
    const std::string &shortOptionPrefix, const std::string &longOptionPrefix)
{
    std::string help_text;
    std::uint64_t longest_option = 0;

    struct FormattedArgument
    {
        const std::string left;
        const std::string right;
    };

    std::list<FormattedArgument> formatted_arguments;
    for (auto&& arg : arguments)
    {
        const auto build_left_part = [&](std::uint64_t &length) {
            std::string part;
            if (arg.hasShortName())
            {
                part += shortOptionPrefix + arg.shortName();
                if (arg.hasLongName())
                {
                    part += ", ";
                }
            }
            if (arg.hasLongName())
            {
                part += longOptionPrefix + arg.longName();
            }
            if (visualizeStringOptions && arg.type() == Argument::String)
            {
                part += stringOptionSuffix;
            }
            length = wcwidth_string(part);
            return part;
        };

        std::uint64_t length = 0;
        formatted_arguments.emplace_back(FormattedArgument{build_left_part(length), arg.description()});

        if (length > longest_option)
        {
            longest_option = length;
        }
    }

    for (auto&&  arg : formatted_arguments)
    {
        std::string fill;
        for (std::uint64_t i = 0; i < longest_option - wcwidth_string(arg.left); ++i)
        {
            fill += ' ';
        }
        help_text += make_spaces(indentation) + arg.left + ' ' + fill + make_spaces(spacing) + arg.right + '\n';
    }

    return help_text;
}
