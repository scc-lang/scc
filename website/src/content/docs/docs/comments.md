---
title: Comments
---

SCC supports three comment styles:

1. C/C++ style single line comment.
1. C/C++ style multiple lines comment.
1. Bash style single line comment.

## Bash style single line comment

Bash style single line comment is similar with C/C++ style single line comment. All characters after
`# ` in the same line will be ignored.

:::caution
For using bash style comment, there must be a white space character after `#`. For example, the following
line is valid comment:

    # This is valid comment

But the following line is invalid comment:

    #This is invalid comment

You would get the compile error:

    a.scc:1:2: error: '#' comment must be followed by a whitespace character
    1 | #This is invalid comment
         ^
