# Brainfreak
> Yet another brainfuck interpreter

## Summary
1. [Presentation](#presentation)
2. [Documentation](#documentation)
    1. [Command line interface](#command-line-interface)
    2. [C api](#c-api)
3. [Example](#example)
4. [Bug Report](#bugs)
5. [Changelog](changelog.md)
---

# Presentation
### - What is this ?
Brainfreak is a [brainf*ck](https://en.wikipedia.org/wiki/Brainfuck) interpreter which include a C api with a [Lua](https://www.lua.org/) style.

It is separated in two mainly parts : 
- engine
- command line interface

Making it easy to implement in other projects
### - Why ?
I wanted to explore the world of interpreters with a simple language, and brainf*ck is the perfect language to begin with.

# Documentation
## Command line interface
Compiled with the 'main.c' file, the program offer some way to manipulate the engine, you can see them using `brainfreak -h` once installed.

To explain the various options:
- `-n <number>` : Brainf*ck use a single dimensional array to allow the code to store data. This option allow you to vary the size of this array. It default use a value of `30000` "cells".
- `-b <byte>` : The array used by Brainf*ck programms have a fixed size. When using this option, you can change this size. It default use a value of `1` byte.
> Note 1 : A valid byte is either 1, 2, 4 or 8. Greater values are not yet supported.<br>
> Note 2 : Bigger the byte is, bigger the number stored will be. With 1 byte, the number goes from -128 to 127 or 0 to 255.
- `-e <string>` : The default behavior of the cli is to read and execute the given file (e.g. : `brainfreak file.bf`), but with the -e option, it execute the given `string` the exit.
## C API
The interpreter include a C API made for integration into existing project. It follow the C API of the lua iterpreter, example :
 
```c
#include <stdio.h>

#include <bf.h>
#include <bf_error.h>

char *bf_str = "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.";

int main(){
    BF_data *bf = BF_new(BF_DEFAULT_SIZE, BF_DEFAULT_BYTES);

    puts("Executing a bf string : \n")
    unsigned int res = BF_do_string(bf, bf_str);

    if(res != BF_OK){
        printf("An error occured : %s\n", BF_read_error(res));
        BF_close(bf);
        return 1;
    }
        
    BF_close(bf);

    return 0;
}
```
--TODO