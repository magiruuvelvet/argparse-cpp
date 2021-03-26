#pragma once

#include <argparse/argparse.hpp>

namespace argparse {

const std::string format(
    const ArgumentParser &parserInstance, const ArgumentParser::Arguments &arguments,
    bool visualizeStringOptions, const std::string &stringOptionSuffix,
    std::uint8_t indentation, std::uint8_t spacing,
    const std::string &shortOptionPrefix, const std::string &longOptionPrefix);

} // namespace argparse
