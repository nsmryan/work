# Work
Work is a low tech build system for C programs.
C is a low tech langauge and deserves a low tech build system.


The idea is to build C programs by writing a C program. No
declarative DSL, no dependency resolution, no fancy
build-system-builds-my-build-system. Just C programs building
C programs.

The [tcc](https://github.com/TinyCC/tinycc) compiler is used to create fast
builds, allowing programs to be build from source on every compile.


There are some clear limitations to this approach, and this program
is mostly an exploration of the question of whether tools like
make or cmake are necessary if our toolchains were faster.
Also I just like tcc a lot, and I think it enables some interesting
programs that are otherwise hard to express, so I want to explore
this space a bit.


## Dependencies
Work uses the following libraries as source files:


    * fs.c (MIT) - https://github.com/jwerle/fs.c
    * growable\_buf (unlicense) - https://github.com/skeeto/growable-buf
    * logc (MIT) - https://github.com/rxi/log.c


## License
Work is released under the MIT license.

