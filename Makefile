aes: ./src/aes.cpp
	g++ -c ./src/utils.cpp -o ./bin/utils.o
	g++ ./src/aes.cpp -o ./bin/aes ./bin/utils.o
	./bin/aes

sm4: ./src/sm4.cpp
	g++ -c ./src/utils.cpp -o ./bin/utils.o
	g++ ./src/sm4.cpp -o ./bin/sm4 ./bin/utils.o
	./bin/sm4

clean: 
	rm -f -r ./bin/* ./test/*
