# Getting started

## Requirements

- CMake 3.16 or newer
- A compiler with C++20 support
- A UTF-8-capable editor and terminal

## Build the interpreter

From the project directory:

```bash
cmake -S . -B build
cmake --build build
```

The resulting executable is `build/Latgalite`.

## Write a program

Create a UTF-8 text file named `hello.lat`:

```latgalite
Mainīgais teksts vārds ir "Pasaule";
Izvadīt("Sveika, " + vārds + "!");
```

Run it:

```bash
./build/Latgalite hello.lat
```

Expected output:

```text
Sveika, Pasaule!
```

## Interactive input

`ievadīt()` reads one complete line from standard input:

```latgalite
Izvadīt("Kā tevi sauc?");
Mainīgais teksts vārds ir ievadīt();
Izvadīt("Sveiks, " + vārds + "!");
```

Run interactive programs by passing their filename:

```bash
./build/Latgalite hello.lat
```

If the program source itself is piped through standard input, that stream is
already exhausted when execution begins, so `ievadīt()` returns an empty string.

## Errors and exit codes

Lexer, parser, and runtime errors are written to standard error. The process
returns `0` after successful execution, `1` after a language or file error, and
`2` when too many command-line arguments are supplied.

Typical diagnostics include a source line and column:

```text
Runtime error at 3:12: Array index is out of bounds.
```

Continue with the [language reference](language-reference.md) or explore the
[included programs](../README.md#included-programs).
