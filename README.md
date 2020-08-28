# Work
Work is a low tech build system for C programs.
C is a low tech langauge and deserves a low tech build system.


The idea is to build C programs by writing a C program. No
declarative DSL, no dependency resolution, no fancy
build-system-builds-my-build-system. Just C programs building
C programs.

The tcc compiler is used to create fast builds, allowing programs to
be build from source on every compile.

## Dependencies
Work uses the following libraries:


    * fs.c (MIT) - https://github.com/jwerle/fs.c
    * growable\_buf (unlicense) - https://github.com/skeeto/growable\_buf
    * logc (MIT) - https://github.com/rxi/log.c


## License
Work is released under the MIT license.

