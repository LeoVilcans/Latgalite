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
| `garums(value)` | Array or text | Number |
| `pievienot(array, value)` | Array, any value | New length |
| `noņemt_pēdējo(array)` | Non-empty array | Removed value |
| `kods(text)` | One-byte text | Number |
| `rakstzīme(code)` | Integer `0..255` | One-byte text |
