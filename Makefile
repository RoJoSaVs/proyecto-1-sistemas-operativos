all: build

build: # Compile all files needed to run the project
	gcc code/initializer.c -o output/initializer
	gcc code/test.c -o output/test
	# ./output/test
	clear
	./output/initializer CE 10 5
	./output/test




reset: # Delete output files
	rm ./output/*
	clear



test: # Run the initializer
# 	./output/initializer
	./output/getter
