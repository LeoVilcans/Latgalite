# Latgalīte documentation

Latgalīte is an interpreted, dynamically evaluated language whose source-level
type names and control-flow keywords are Latvian words.

## Guides

- [Getting started](getting-started.md) explains how to build and run the
  interpreter and introduces a first program.
- [Language reference](language-reference.md) documents syntax, values,
  operators, functions, arrays, and control flow.
- [Built-in functions](standard-library.md) lists all currently available I/O,
  array, text, and character operations.
- [Interpreter architecture](architecture.md) describes the C++ implementation
  and how to extend it.

## Quick example

```latgalite
Funkcija sveiciens(teksts vārds) {
    Atgriezt "Sveiks, " + vārds + "!";
}

Izvadīt(sveiciens("Latgalīte"));
```

Every statement that is not a block or function/control-flow declaration ends
with a semicolon. Keywords and identifiers are case-sensitive.
