# argparse

**Simple stupid command line parser for C++**

*This library has a D equivalent available with the exact same API: [argparse-cpp](https://github.com/magiruuvelvet/argparse-d)*

This library is partially inspired by and older version of [argparse](https://github.com/jamolnng/argparse).
The parser only supports boolean and string options, while boolean options are always true when present.
The public API is similar, but exceptions were replaced with status codes, so you can use it in
exception-free code easily.

This library doesn't throw exceptions. All possible exceptions which can be thrown are handled
internally and are translated to status codes. Exceptions must be enabled though for this library
and your application because some template functions in header files contain them.

## Features

 - boolean options
 - string options
 - i18n-friendly
 - very customizable and unobtrusive
 - doesn't inject any options by default (everything provided by the developer)
 - termination (disabled by default, default once activated: `--`)
 - lose values (there are no positional arguments)
 - default values

## Usage

Add this library as git submodule somewhere in your project and then add the target.
Please note that you must add the subdirectory `src` and not the project root!

```
add_subdirectory(argparse/src EXCLUDE_FROM_ALL)
target_link_libraries(YourApp PRIVATE libs::argparse)
```

## Example

`./app --value abc`

```cpp
#include <argparse/argparse.hpp>

int main(int argc, char **argv)
{
    using namespace argparse;
    ArgumentParser parser(argc, argv);
    parser.addArgument("h", "help", "Show this help and exit", Argument::Boolean);
    parser.addArgument("", "version", "Show version information and exit", Argument::Boolean);
    parser.addArgument("v", "value", "Provide a value");
    const ArgumentParserResult res = parser.parse();

    // ArgumentParserResult can be one of Success, InsufficientArguments, MissingArgument and Unknown

    bool hasValue = parser.exists("value"); // was --value provided on the command line?
    std::string value = parser.get("value"); // get value of --value, contains "abc"
}
```

## API

 - `bool ArgumentParser::addArgument(string shortName, string longName, string description, ...)`:\
   Registers a new argument in the parser to process. An argument can consist of either a
   short name, long name or both. Duplicate names are not allowed. The order or registration
   is preserved when generating help output using `ArgumentParser::help()`. The return value
   can be used to check whenever the argument was added or not, returns false only on duplicates.
   Overlapping of short and long options is correctly handled.

 - `bool ArgumentParser::addHelpOption(string description)`:\
   Convenience function to add a help option. Only the description must be provided.
   The parser doesn't include a help option by default and must be explicitly provided
   by the developer. The library has a built-in help printer though when desired.
   One can always write their own help printer. The rationale behind this design decision
   is i18n (Internationalization) and fine gained customization of all command line arguments.
   I don't want to inject something English hardcoded by default.

 - `string ArgumentParser::help()`:\
   Generates a pretty formatted help output. This string contains only the options
   with their description and nothing else. Additional output when printing help
   is up to the developer. The indentation and spacing can be customized.

 - `void ArgumentParser::setTerminator(string)`:\
   Sets the parsing terminator at were to stop parsing arguments. By default no termination
   is performed. Calling this function without arguments makes `--` the terminator.
   Passing an empty string disables the terminator again.

 - `Result ArgumentParser::parse()`:\
   Does the parsing, once arguments were registered. The status is returned as enum.
   This function does nothing when there are no registered arguments or when `parse()`
   was already called (protected against multiple invocations).

 - `string[] ArgumentParser::missingArguments()`:\
   Contains a list of all registered arguments which were missing during the parsing when they
   were marked as required.

 - `string[] ArgumentParser::loseArguments()`:\
   Contains a list of all unused and lose arguments which weren't matched during parsing.

 - `string[] ArgumentParser::remainingArguments()`:\
   Contains a list of all arguments after the terminator, excluding the terminator itself.

 - `bool ArgumentParser::exists(string)`:\
   Check if the argument with the name is present on the command line. Only registered arguments are checked.

 - `T ArgumentParser::get<T>(string)`:\
   Receive the value from the argument, defaults to string if template argument is omitted.
   On casting errors a default initialized value of the data type is returned.

**Format of command line arguments:** `-s value --long-option value value2 --boolean -a value -b`

 - `-s`(String) has value `"value"`
 - `--long-option`(String) has value `"value"`
 - `--boolean`(Boolean) is present and true
 - `-a`(String) has value `"value"`
 - `-b`(String) has value `""` (empty string options are allowed)
 - unused and lose values are stored in an array (`"value2"`)

## Why?

This is the C++ equivalent of the same named library for the D programming language I wrote.
It exists just because to have a library with the exact same API for both languages.

This command line parser is supposed to be simple and **stupid**. You need to handle most
of the things yourself as developer. It just implements a convenient way to receive arguments
in a safe manner so you don't need to worry about index out of bounds, etc.

Supported are:

 - boolean switches which are always true once present, there is **no** `-b=false` syntax, and
 - string switches which can take a value or not, if value is omitted an empty string is assumed.

## Credits

argparse has a slightly changed copy of [termux/wcwidth](https://github.com/termux/wcwidth)
embedded for usage with the built-in help printer. The changes include moving the functions
into the library namespace to avoid conflicts when you already have another version of wcwidth
in your project, the second change is to accept UTF-32 input rather than UTF-16 to avoid issues
with UTF-32 input and overflows. The help printer takes UTF-8 input and converts everything
to UTF-32 before sending the individual code points to the wcwidth function.
