main: ./src/aes.cpp
	g++ -o ./bin/main ./src/ase.cpp
	./bin/main
clean: 
	rm -f -r ./bin/* ./test/*
