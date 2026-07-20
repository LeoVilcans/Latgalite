# Built-in functions

Built-ins use ordinary call syntax but are provided directly by the interpreter.
Calling them with the wrong number or type of arguments produces a runtime error.

## Input and output

### `Izvadīt(value)`

Writes the displayed value followed by a newline. `Izvadīt` is a language
statement rather than a value-returning function.

```latgalite
Izvadīt("Sveiki!");
Izvadīt([1, 2, 3]);
```

Displayed booleans use `patiess` and `aplams`. Integral numeric values are shown
without a decimal suffix.

### `ievadīt()`

Reads and returns one line from standard input without the trailing newline. It
returns empty text for either an empty line or end-of-file.

```latgalite
Izvadīt("Ievadi savu vārdu:");
Mainīgais teksts vārds ir ievadīt();
```

Numeric text is converted automatically when assigned to a `skaitlis` variable:

```latgalite
Izvadīt("Ievadi skaitli:");
Mainīgais skaitlis vērtība ir ievadīt();
```

### `ievadīt_skaitli()`

Reads one line and returns it as a number. This form is useful when the input is
used directly in an expression:

```latgalite
Mainīgais skaitlis summa ir ievadīt_skaitli() + ievadīt_skaitli();
```

Numeric input may have surrounding whitespace, an optional `+` or `-`, and a
decimal part. Both `12.5` and the Latvian-style `12,5` are accepted. Empty,
malformed, out-of-range, and non-finite inputs produce a runtime error;
`ievadīt_skaitli()` also reports end-of-file as an error.

## Text

Text functions operate on UTF-8 byte sequences, consistently with text indexing
and `garums`.

### `saskaldīt(text, separator)`

Splits text at every occurrence of a non-empty separator and returns an array of
text pieces. Consecutive and trailing separators produce empty pieces:

```latgalite
Mainīgais masīvs daļas ir saskaldīt("viens,,trīs,", ",");
// ["viens", "", "trīs", ""]
```

### `savienot(array, separator)`

Joins the displayed representation of every array element with text between
them. An empty array produces empty text.

```latgalite
Izvadīt(savienot(["viens", "divi", 3], ", "));
// viens, divi, 3
```

### `satur(text, fragment)`

Returns `patiess` when the exact fragment occurs in the text, otherwise
`aplams`. Every text contains the empty fragment.

### `atrast(text, fragment)`

Returns the zero-based byte index of the fragment's first occurrence, or `-1`
when it is absent.

### `aizstāt(text, fragment, replacement)`

Returns new text with every non-overlapping occurrence of a non-empty fragment
replaced. The original text is unchanged.

```latgalite
Izvadīt(aizstāt("2026-07-20", "-", "/"));
// 2026/07/20
```

## Arrays and lengths

### `garums(value)`

Returns the number of array elements or the number of bytes in text.

```latgalite
garums([10, 20, 30]) // 3
garums("abc")        // 3
```

### `pievienot(array, value)`

Appends a value to an array and returns its new length.

```latgalite
Mainīgais masīvs burti ir ["A"];
pievienot(burti, "B");
```

### `noņemt_pēdējo(array)`

Removes and returns the final array element. Calling it on an empty array is a
runtime error.

```latgalite
Mainīgais teksts pēdējais ir noņemt_pēdējo(burti);
```

## Character conversion

Character conversion currently operates on individual bytes, making it suitable
for ASCII and byte-oriented interpreters such as `brainfuck.lat`.

### `kods(text)`

Returns the unsigned numeric code of a one-byte string. Longer strings and
multi-byte UTF-8 characters produce a runtime error.

```latgalite
Izvadīt(kods("A")); // 65
```

### `rakstzīme(code)`

Converts an integral code from `0` through `255` into a one-byte string.

```latgalite
Izvadīt(rakstzīme(65)); // A
```

## Summary

| Operation | Arguments | Result |
| --- | --- | --- |
| `Izvadīt(value)` | Any value | Writes a line |
| `ievadīt()` | None | Text line |
| `ievadīt_skaitli()` | None | Number |
| `saskaldīt(text, separator)` | Two texts | Array of text pieces |
| `savienot(array, separator)` | Array and text | Text |
| `satur(text, fragment)` | Two texts | Boolean |
| `atrast(text, fragment)` | Two texts | Byte index or `-1` |
| `aizstāt(text, fragment, replacement)` | Three texts | Text |
| `garums(value)` | Array or text | Number |
| `pievienot(array, value)` | Array, any value | New length |
| `noņemt_pēdējo(array)` | Non-empty array | Removed value |
| `kods(text)` | One-byte text | Number |
| `rakstzīme(code)` | Integer `0..255` | One-byte text |
