cc = gcc -g -Wall -Werror -Wextra -std=c11

OS = $(shell uname)

ifeq ($(OS),Linux)
	OPEN_CMD = xdg-open
endif
ifeq ($(OS),Darwin)
	OPEN_CMD = open
endif

all: decimal.a 

decimal.a:
	$(cc) -c *.c
	ar rc libdecimal.a *.o
	rm -rf *.o
	ranlib libdecimal.a
	cp libdecimal.a decimal.a

clang:
	clang-format -style=Google -n *.c *.h tests/*.c tests/*.h

clean:
	rm -rf *.o *.gcno *.gcda *.a *.info report .clang-format test *.out
