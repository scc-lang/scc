---
title: Comments
---

SCC supports three comment styles:

1. C/C++ style single line comment.
1. C/C++ style multiple lines comment.
1. Bash style single line comment.

## C/C++ style single line comment

The same as C/C++ syntax. All contents between `//` and a new line will be ignored.

For example:

```cpp
// this is comment.
int a + b; // this is comment also.
```

## C/C++ style multiple lines comment

_TODO_

## Bash style single line comment

Bash style single line comment is similar with C/C++ style single line comment. All characters after
`# ` in the same line will be ignored.

For example:

```bash
# this is comment.
int a + b; # this is comment also.
```

:::caution
For using bash style comment, there must be a white space character after `#`. For example, the following
line is invalid comment:

```bash
#this is invalid comment
```

You would get the compile error:

```bash
a.scc:1:2: error: '#' comment must be followed by a whitespace character
    1 | #this is invalid comment
        ^
```
:::

:::tip
`#` followed by other character is reserved for other usage. For example:

```bash
#!/usr/bin/env scc

std::println("hello world!");
```

`#!` is used by `bash` to invoke `scc` compiler.
:::
