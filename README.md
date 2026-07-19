# Latgalīte

Latgalīte is a small interpreted programming language implemented in C++. Its
keywords are based on Latvian, while its syntax follows familiar imperative
language conventions.

```latgalite
Mainīgais masīvs skaitļi ir [2, 3, 5];
Mainīgais skaitlis summa ir 0;
Mainīgais skaitlis i ir 0;

Kamēr(i mazāks par garums(skaitļi)) {
    summa pieskaitīt skaitļi[i];
    i pieskaitīt 1;
}

Izvadīt("Summa: " + summa);
```

The interpreter supports typed variables, functions, arrays, conditionals,
loops, interactive input, arithmetic, text operations, and boolean logic.

## Build

Latgalīte requires a C++20 compiler and CMake 3.16 or newer.

```bash
cmake -S . -B build
cmake --build build
```

Run a source file:

```bash
./build/Latgalite example.lat
```

Source can also be read from standard input:

```bash
./build/Latgalite < example.lat
```

Use the file form for interactive programs, because `ievadīt()` reads from the
same standard-input stream.

## Included programs

- [`example.lat`](example.lat) demonstrates the core syntax and arrays.
- [`desas.lat`](desas.lat) is an interactive two-player tic-tac-toe game.
- [`turing_machine.lat`](turing_machine.lat) implements a generic Turing-machine
  simulator.

Play tic-tac-toe with:

```bash
./build/Latgalite desas.lat
```

## Documentation

- [Documentation index](docs/index.md)
- [Getting started](docs/getting-started.md)
- [Language reference](docs/language-reference.md)
- [Built-in functions](docs/standard-library.md)
- [Interpreter architecture](docs/architecture.md)

Latgalīte is an educational tree-walk interpreter. The dynamically growing
arrays and looping facilities are sufficient to simulate an arbitrary Turing
machine, as demonstrated by `turing_machine.lat`; physical executions remain
limited by available memory and time.
