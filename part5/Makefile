BINARIES = benchmark check depthMap
CC = gcc
CFLAGS = -std=c99 -Wall
LFLAGS = -lm

default: all_release

debug: all_debug

all: $(BINARIES)

all_debug: CC += -DDEBUG -g
all_debug: all

all_release: CC += -O3 -DNDEBUG -g0
all_release: all

benchmark: benchmark.o calcDepthNaive.o calcDepthOptimized.o utils.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

check: calcDepthNaive.o calcDepthOptimized.o check.o utils.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

depthMap: calcDepthNaive.o calcDepthOptimized.o depthMap.o utils.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm -rf *.o
	rm -rf $(BINARIES)
