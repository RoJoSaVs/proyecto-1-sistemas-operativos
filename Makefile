all: build

build: # Compile all files needed to run the project
	gcc code/initializer.c -o output/initializer -lpthread -lrt
	gcc code/emitter.c -o output/emitter -lpthread -lrt
	gcc code/finalizer.c -o output/finalizer -I /home/user/libs/include/SDL2 -L /home/user/libs/lib -lSDL2 -Wl,-rpath=/home/user/libs/lib
	./output/initializer CE 10 5
	./output/emitter CE auto 5


initializer:
	clear
	gcc code/initializer.c -o output/initializer -lpthread -lrt
	./output/initializer CE 10 5
	

emitter:
	clear
	gcc code/emitter.c -o output/emitter -lpthread -lrt
	./output/emitter CE auto 5


receptor:
	gcc code/Receptor.c -o output/Receptor -lpthread -lrt
	./output/Receptor $(mode) $(key) $(Name)


reset: # Delete output files
	rm ./output/*
	clear

test: # Run the initializer
	./output/initializer CE 15 4

test_finalizer: # Run the initializer
	./output/finalizer

test: # Run the initializer
# 	./output/initializer
	./output/getter

	# gcc code/test.c -o output/test
	# ./output/test

