# tex-macro-processor

Command-line program TeX-like compiler built with [C](https://en.wikipedia.org/wiki/C_(programming_language)), [gdb](https://www.sourceware.org/gdb/), and [Valgrind](https://valgrind.org/)

## Setup

- Run `make all`

## Recursion and Evaluation Strategy Examples

```
// Fragmented macro

\def{testMacro}{some text #}
\def{macroFragment}{goes #}

\testMacro{\macroFragment}{here}

// Evaluates to
some text goes here
```
```
// Circular macro definition

\def{list}{\if{#}{#, \list}{..., omega}}
\list{alpha}{beta}{gamma}{}

// Evaluates to
alpha, beta, gamma, ..., omega
```

