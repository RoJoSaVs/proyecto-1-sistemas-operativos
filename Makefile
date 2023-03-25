all: build

build: # Compile all files needed to run the project
	gcc code/initializer.c -o output/initializer
	gcc code/getter.c -o output/getter
	clear
	./output/initializer



reset: # Delete output files
	rm ./output/*
	clear



test: # Run the initializer
# 	./output/initializer
	./output/getter
