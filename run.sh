
# TODO try out using tcc in case this is causing tcclib to reject the work.so file

gcc -c -fPIC -L/c/data/tools/tcc -ltcc -lc src/worklib.c -Iinc -I/c/data/tools/tcc/libtcc -I./dep/growable_buf -I./dep/logc -g
gcc -c -fPIC -L/c/data/tools/tcc -ltcc -lc dep/logc/log.c -Iinc -I/c/data/tools/tcc/libtcc -I./dep/growable_buf -I./dep/logc -g
gcc -shared -fPIC -o work.so -L/c/data/tools/tcc -ltcc -lc worklib.o log.o -Iinc -I/c/data/tools/tcc/libtcc -I./dep/growable_buf -I./dep/logc -g
gcc -L/c/data/tools/tcc -ltcc src/work.c log.o worklib.o -Iinc -I/c/data/tools/tcc/libtcc -I./dep/growable_buf -I./dep/logc -o work -g
./work example/wrk.c
