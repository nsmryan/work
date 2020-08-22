set -e

rm work worklib.so logc.o worklib.o -f

echo "Building worklib.o"
tcc -c -L/usr/local/lib -L. src/worklib.c -Iinc -I/usr/local/include -I./dep/growable_buf -I./dep/logc -g -o worklib.o

echo "Building log.o"
tcc -c -L/usr/local/lib dep/logc/log.c -Iinc -I/usr/local/include -I./dep/growable_buf -I./dep/logc -g -o log.o

echo "Building work.so"
tcc -shared -L/usr/local/lib -fPIC -o work.so -ltcc -lc worklib.o log.o -Iinc -I/usr/local/include -I./dep/growable_buf -I./dep/logc -g

echo "Building work"
tcc -v -L/usr/local/lib -L. src/main.c log.o worklib.o -Iinc -I/usr/local/include -I./dep/growable_buf -I./dep/logc -o work -g -ltcc -pthread -ldl

# echo "Run work"
# ./work example/work.c
