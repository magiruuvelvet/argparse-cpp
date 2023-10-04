# Changelog

## `2.0.0`

**BREAKING CHANGE**

Avoid matching empty strings in `ArgumentParser::exists()` method.

This can be considered a bug or a feature, depending on how you use this library.
Matching against empty strings is no longer possible, but it is also a breaking change.
For that reason the major version number was increased to 2.

## `1.1.1`

 - initial release
