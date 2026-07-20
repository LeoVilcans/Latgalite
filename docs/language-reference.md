# Language reference

## Source text and tokens

Latgalīte source files conventionally use the `.lat` extension and UTF-8
encoding. Keywords are case-sensitive: `Mainīgais` and `mainīgais` are different
tokens.

Identifiers may contain ASCII letters, digits after the first character,
underscores, and UTF-8 bytes. Consequently, names such as `summas_rezultāts` and
`x_2` are valid.

Two comment forms are supported:

```latgalite
// A line comment

/* A block
   comment */
```

Strings use double quotes. Escape sequences are not currently interpreted. A
string may span lines, although single-line strings are recommended.

Numbers are stored as double-precision values. Literals can be integral or
decimal, and negative numbers use the unary `-` operator:

```latgalite
14
3.5
-7
```

## Values and declared types

| Type | Meaning | Examples |
| --- | --- | --- |
| `skaitlis` | Number | `12`, `-4`, `2.5` |
| `teksts` | Text | `"sveiki"` |
| `loģisks` | Boolean | `patiess`, `aplams` |
| `masīvs` | Heterogeneous array | `[1, "divi", patiess]` |

Declare and assign variables with `Mainīgais` and `ir`:

```latgalite
Mainīgais skaitlis punkti ir 10;
Mainīgais teksts vārds ir "Līga";

punkti ir 15;
```

Assignments enforce the variable's declared type. Assigning any value to a
`teksts` variable converts it to its displayed representation. A `skaitlis`
accepts a number or valid numeric text, which makes
`Mainīgais skaitlis x ir ievadīt();` possible; the other types require a matching
value. Individual elements of heterogeneous arrays do not have declared types.

## Arrays

Array literals, indexing, and mutation use square brackets:

```latgalite
Mainīgais masīvs vērtības ir [10, 20, 30];
Izvadīt(vērtības[1]);

vērtības[1] ir 25;
vērtības[0] pieskaitīt 5;
```

Indexes are zero-based non-negative integers. Reading or writing beyond the
array boundary is a runtime error. Arrays can be nested:

```latgalite
Mainīgais masīvs tabula ir [[1, 2], [3, 4]];
tabula[1][0] ir 9;
```

Arrays have reference semantics. Assigning an array to another variable or
passing it to a function shares the same underlying array, so mutations remain
visible to the caller.

Text can also be indexed, producing one byte as a new string. Text indexing and
`garums` operate on UTF-8 bytes rather than Unicode characters.

## Arithmetic and concatenation

```latgalite
Mainīgais skaitlis x ir (8 + 4) * 2;
x ir x / 3;
Mainīgais skaitlis atlikums_no_divi ir x atlikums 2;
```

The arithmetic operators are `+`, `-`, `*`, `/`, and `atlikums`. Division or
remainder by zero is a runtime error.

If either operand of `+` is text, the displayed representations of both values
are concatenated:

```latgalite
Izvadīt("Punkti: " + 42);
```

`pieskaitīt` adds numbers in place or appends to a text target:

```latgalite
punkti pieskaitīt 1;
ziņa pieskaitīt "!";
```

## Comparisons and logic

Latgalīte uses word-based comparisons:

```latgalite
x mazāks par 10
x lielāks par 0
x ir vienāds ar 5
```

Equality compares displayed value representations. Relational comparisons
require numbers.

Logical operators are `nav`, `un`, and `vai`. `un` and `vai` short-circuit:

```latgalite
Mainīgais loģisks derīgs ir x lielāks par 0 un x mazāks par 10;
Ja(nav derīgs) {
    Izvadīt("Vērtība nav diapazonā.");
}
```

Conditions use these truthiness rules:

- `aplams`, zero, empty text, and an empty array are false;
- `patiess`, non-zero numbers, non-empty text, and non-empty arrays are true.

## Operator precedence

From lowest to highest precedence:

1. `vai`
2. `un`
3. `mazāks par`, `lielāks par`, `ir vienāds ar`
4. `+`, `-`
5. `*`, `/`, `atlikums`
6. unary `nav`, unary `-`
7. indexing and function calls

Parentheses can override precedence.

## Conditions and loops

```latgalite
Ja(punkti lielāks par 10) {
    Izvadīt("Daudz punktu");
} Citādi {
    Izvadīt("Vēl jāturpina");
}

Kamēr(punkti mazāks par 20) {
    punkti pieskaitīt 1;
}
```

`Citādi` is optional. Blocks create child scopes; assignment can update a
variable found in an enclosing scope.

## Multi-way selection

Use `Salīdzini` when one value should select among several branches:

```latgalite
Salīdzini(komanda) {
    Sakrīt ar "sākt":
        Izvadīt("Sākam!");
    Sakrīt ar "beigt":
        Izvadīt("Beidzam.");
    Citādi:
        Izvadīt("Nezināma komanda.");
}
```

`Salīdzini` evaluates its value once, then evaluates the `Sakrīt ar` values in
source order. The first equal value selects its branch; later branches do not
run, so no separate `break` statement is needed. Equality follows the same
displayed-value comparison as `ir vienāds ar`.

The optional `Citādi` branch runs when no value matches and must be last. If it
is omitted and nothing matches, the statement does nothing. A selected branch
has its own child scope, like a `Ja` block. Statements inside a branch retain
their usual semicolons, while branch labels end with `:`.

## Functions

Functions have typed parameters and dynamically typed return values:

```latgalite
Funkcija saskaitīt(skaitlis a, skaitlis b) {
    Atgriezt a + b;
}

Izvadīt(saskaitīt(4, 5));
```

There is no declared return type. `Atgriezt` immediately exits the function. A
function that reaches the end without returning produces `0`.

Function declarations take effect when execution reaches them, so declare a
function before its first call. Functions retain their defining environment and
can call themselves recursively.

## Expression statements

Function calls may be used as standalone statements, which is useful for array
mutation functions:

```latgalite
pievienot(vērtības, 40);
```

Declarations, assignments, returns, output calls, and expression statements end
with `;`. Blocks, function declarations, `Ja`, `Kamēr`, and `Salīdzini` do not
take a trailing semicolon.
