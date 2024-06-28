---
title: Functions
---

Functions in SCC is the same as functions in C/C++. Every function associates a function body and a function name.
For example:

```cpp
// function definition.
// defines a function with the name "sum" and witht the body "{ return x+y; }"
int sum(int x, int y)
{
    return x + y;
}
```

:::note
In SCC, `main` function is not required. If there is no `main` function, the application will be exected from the top to bottom.
But you can also define `main` function. If you define `main` function, you can't define other statements out from `main` function.

For example, the two following code have the same effect:
```cpp
int a = 10;
int b = 20;
std::println("a + b = {}", a + b);
```

the same as:
```cpp
void main() {
    int a = 10;
    int b = 20;
    std::println("a + b = {}", a + b);
}
```

But the following code is invalid:
```cpp
int a = 10;
int b = 10;

void main() {
    std::println("a + b = {}", a + b);
}
```

You will get compile error:
```bash
a.scc:1:1: error: unexpected global statement when 'main' function is defined (4:1)
    1 | int a = 10;
        ^^^^^^^^^^^
```
:::