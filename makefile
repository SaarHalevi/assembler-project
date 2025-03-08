# Set include path
INCLUDES = -I./src/include

all: assembler

assembler: pre_assembler.o general.o front_end.o first_pass.o second_pass.o back_end.o assembler.o
	gcc $(INCLUDES) -ansi -pedantic -Wall -o assembler pre_assembler.o general.o front_end.o first_pass.o second_pass.o back_end.o assembler.o

pre_assembler.o: src/pre_assembler.c
	gcc $(INCLUDES) -ansi -pedantic -Wall -c src/pre_assembler.c

general.o: src/general.c
	gcc $(INCLUDES) -ansi -pedantic -Wall -c src/general.c

front_end.o: src/front_end.c
	gcc $(INCLUDES) -ansi -pedantic -Wall -c src/front_end.c

first_pass.o: src/first_pass.c
	gcc $(INCLUDES) -ansi -pedantic -Wall -c src/first_pass.c

second_pass.o: src/second_pass.c
	gcc $(INCLUDES) -ansi -pedantic -Wall -c src/second_pass.c

back_end.o: src/back_end.c
	gcc $(INCLUDES) -ansi -pedantic -Wall -c src/back_end.c

assembler.o: src/assembler.c
	gcc $(INCLUDES) -ansi -pedantic -Wall -c src/assembler.c

clean:
	rm *.o assembler

# Optional: test target (if you want to run tests)
test: test_main.o assembler.o
	gcc $(INCLUDES) -o test_runner test_main.o assembler.o
	./test_runner

test_main.o: test/test_main.c
	gcc $(INCLUDES) -ansi -pedantic -Wall -c test/test_main.c
