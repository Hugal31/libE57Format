# libE57Format

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/asmaloney/libE57Format)](https://github.com/asmaloney/libE57Format/releases/latest) [![Docs](https://img.shields.io/badge/docs-online-orange)](https://asmaloney.github.io/libE57Format-docs/) [![GitHub](https://img.shields.io/github/license/asmaloney/libE57Format)](LICENSE) ![Build](https://github.com/asmaloney/libE57Format/actions/workflows/build.yml/badge.svg)

A library to provide read & write support for the E57 file format.

This is a fork of [E57RefImpl](https://sourceforge.net/projects/e57-3d-imgfmt/) v1.1.332. The original source is from [E57RefImpl 1.1.332](https://sourceforge.net/projects/e57-3d-imgfmt/files/E57Refimpl-src/) and then everything was stripped out except the main implementation for reading and writing E57.

This version also removes the dependency on [Boost](http://www.boost.org/) and - as of version 3.0 - requires [C++14](https://en.cppreference.com/w/cpp/14). (libE57Format version 2.x required [C++11](https://en.cppreference.com/w/cpp/11).)

Many, many other changes were made prior to the first release of this fork. See the [CHANGELOG](CHANGELOG.md) and git history for details.

## Documentation

The doxygen-generated documentation may be [found here](https://asmaloney.github.io/libE57Format-docs/). These docs are generated and saved in the [libE57Format-docs](https://github.com/asmaloney/libE57Format-docs) repo.

## Contributing

Please see [CONTRIBUTING](CONTRIBUTING.md).

## Why Fork?

The E57RefImpl code hasn't been touched in years and I wanted to make changes to compile this library with macOS. Forking it gives me a bit more freedom to update the code and make changes as required.

I changed the name of the project so that it is not confused with the **E57RefImpl** project.

I have also changed the main include file's name from `E57Foundation.h` to `E57Format.h` to make sure there is no inclusion confusion.

Versions of **libE57Format** started at 2.0.

### E57Simple API

Since the original fork, [Jiri Hörner](https://github.com/ptc-jhoerner) has added the E57Simple API from the old reference implementation and updated it.

### Tools

[Ryan Baumann](https://github.com/ryanfb) has updated the `e57unpack` and `e57validate` tools to work with **libE57Format**. You can find them in the [e57tools](https://github.com/ryanfb/e57tools) repo.

## Building

`$ mkdir -p build && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel`

### Dependencies

[Xerces-C++](https://xerces.apache.org/xerces-c/) validating XML parser is needed at runtime and
[clang-format](https://clang.llvm.org/docs/ClangFormat.html) for checking C++ formatting.

Linux (Ubuntu):

```sh
$ sudo apt install libxerces-c-dev clang-format
```

## License

This project as a whole is licensed under the [**BSL-1.0**](https://opensource.org/licenses/BSL-1.0) license - see the [LICENSE](LICENSE.md) file for details.

Individual source files may contain the following tag instead of the full license text:

    SPDX-License-Identifier: BSL-1.0

Some CMake files are licensed under the **MIT** license - see the [LICENSE-MIT](LICENSE-MIT.txt) file for details.

These files contain the following tag instead of the full license text:

    SPDX-License-Identifier: MIT

Using SPDX enables machine processing of license information based on the [SPDX License Identifiers](https://spdx.org/ids) and makes it easier for developers to see at a glance which license they are dealing with.
