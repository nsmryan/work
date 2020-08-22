# Work Notes

## TCC Notes
tcc builds a single object file- multiple symbols are not allowed

multiple tcc states can be used to create separate namespaces

tcc output exes which depend on libc.so, ld-linux, and linux-vdso.so.
Potentially could link musl for self contained exe, which may even
start faster. May be able to do this when running from memory as well.


potential for a unity built style, could make it even faster then outputing
object files.

potential for user-provided entry point- if no main, then build a main
function that calls the user function.

## Design Notes

### Options with Targets
Work may want to list targets or commands, run instead of outputting files,
or some other action with targets.

This prevents targets from being dependant on the results of other targets.
This might be okay.

If so, just return targets from wrk\_main. May link them or return an array.


### Object Files
can add object files- could build each file in project, then add them each in turn?
this seems the same as compiling them in from source, but perhaps slightly faster?

### Work Files vs Main Program
can have separate tcc states for each wrk.c file, each declaring a wrk\_main function
then have one tcc state for the program, which each wrk\_main function receives and
adds files to

### Passing Arguments
read wrk.c file- if it starts with #!, each up to tcc, then to -run, and pass remaining
argments to libtcc. this allows linking wrk.c files against libraries such as pthreads
generally better to add functions to wrk, or use single header files ones, but allows
normal libraries as well.

### Build Requirements
haven't worked out passing requirements upwards- code that must must be compiled
with a certain flag that needs to be used in the other build files?
Or, if this library is build with these flags, another library
must be build in a different way.

### LibC
can the work exe statically link musl or another libc, to avoid as many dependancies as possible?
would also improve start times, as there is less linking needed.


## Requirements
    * work shall find a work.c file and run its wrk\_main function.
    * wrk.c files can build executables, shared objects, object files, and archives.
    * wrk.c files can run arbitrary shell commands.

Is it important that 'work run' be able to run the program without outputting an exe?
This is technically redundant- the exe could just be run. However it may be faster,
and convenient, to build and run in one step.


Is it important to return a description of targets? this gets back to the graph thing-
it may be very complex, and I'm not going to track dependencies.
perhaps there is no way to list commands without running, or if you do there
is a flag in the state that can be checked when appropriate.