all: build

build: # Compile all files needed to run the project
	gcc code/initializer.c -o output/initializer -lpthread -lrt
	gcc code/emitter.c -o output/emitter -lpthread -lrt
	gcc code/test.c -o output/test -lpthread -lrt
	
	./output/initializer CE 10 5
	./output/test
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



test:
	gcc code/getData.c -o output/getData -lpthread -lrt
	./output/getData

	# gcc code/test.c -o output/test
	# ./output/test

