all: build

build: # Compile all files needed to run the project
	clear
	gcc code/initializer.c -o output/initializer
	gcc code/emitter.c -o output/emitter
	gcc code/receiver.c -o output/receiver
	gcc code/test.c -o output/test
	



initializer:
	clear
	gcc code/initializer.c -o output/initializer
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



test: #Used just for testing xD
	gcc code/test.c -o output/test
	./output/test

