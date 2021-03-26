#pragma once

#include <argparse/argparse.hpp>

namespace argparse {

using StringList = std::list<std::string>;

ArgumentParserResult parse(
    const ArgumentParser &parserInstance,
    const std::vector<std::string> &args, argparse::ArgumentParser::Arguments &registeredArguments,
    const std::string &shortOptionPrefix, const std::string &longOptionPrefix,
    const std::string &terminator,
    StringList &missingArguments, StringList &loseArguments, StringList &remainingArguments);

} // namespace argparse
