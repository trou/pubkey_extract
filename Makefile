CC=gcc
COPTS=-W -Wall -fsanitize=address -O3 -ggdb -pipe -m64 -std=c99 -fPIC -fno-exceptions -fstack-protector -Wl,-z,relro -Wl,-z,now -fvisibility=hidden -W -Wall -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wpointer-arith -Wformat -Wreturn-type -Wsign-compare -Wmultichar -Wformat-nonliteral -Winit-self -Wuninitialized -Wno-deprecated -Wformat-security -Werror -fopenmp

all: main.c
	$(CC) $(COPTS) -o main main.c
