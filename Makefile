all: build

build: # Compile all files needed to run the project
	gcc code/initializer.c -o output/initializer -lpthread -lrt
	gcc code/emitter.c -o output/emitter -lpthread -lrt
	gcc code/finalizer.c -o output/finalizer -I /home/user/libs/include/SDL2 -L /home/user/libs/lib -lSDL2 -Wl,-rpath=/home/user/libs/lib


initializer:
	clear
	gcc code/initializer.c -o output/initializer -lpthread -lrt
	./output/initializer CE 10 5
	

emitter:
	# ./output/emitter manual 5 CE
	clear
	gcc code/emitter.c -o output/emitter
	./output/emitter auto 5 CE


receiver:
	# ./output/receiver manual 5 CE
	clear
	gcc code/receiver.c -o output/receiver
	./output/receiver auto 5 CE


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

