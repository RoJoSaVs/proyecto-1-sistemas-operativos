all: build

build: # Compile all files needed to run the project
	# gcc code/initializer.c -o output/initializer
	gcc code/initializer.c -o output/initializer -lpthread -lrt
	# gcc code/test.c -o output/test
	gcc code/test.c -o output/test -lpthread -lrt
	# ./output/test
	clear
	./output/initializer CE 10 5
	./output/test

receptor:
	gcc code/Receptor.c -o output/Receptor -lpthread -lrt
	./output/Receptor $(mode) $(key) $(Name)


reset: # Delete output files
	rm ./output/*
	clear



test: # Run the initializer
# 	./output/initializer
	./output/getter
