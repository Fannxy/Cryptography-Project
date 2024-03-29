aes: ./src/aes.cpp
	g++ -c ./src/utils.cpp -o ./bin/utils.o
	g++ ./src/aes.cpp -o ./bin/aes ./bin/utils.o
	./bin/aes

sm4: ./src/sm4.cpp
	g++ -c ./src/utils.cpp -o ./bin/utils.o
	g++ ./src/sm4.cpp -o ./bin/sm4 ./bin/utils.o
	./bin/sm4

sha256: ./src/sha256.cpp
	g++ -c ./src/utils.cpp -o ./bin/utils.o
	g++ ./src/sha256.cpp -o ./bin/sha256 ./bin/utils.o
	./bin/sha256

sha3: ./src/sha3_256.cpp
	g++ -c ./src/utils.cpp -o ./bin/utils.o
	g++ ./src/sha3_256.cpp -o ./bin/sha3_256 ./bin/utils.o
	./bin/sha3_256

sm3: ./src/sm3.cpp
	g++ -c ./src/utils.cpp -o ./bin/utils.o
	g++ ./src/sm3.cpp -o ./bin/sm3 ./bin/utils.o
	./bin/sm3

clean: 
	rm -f -r ./bin/* ./test/* ./log
