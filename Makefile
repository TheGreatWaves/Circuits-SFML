.PHONY: clean cmake build run

cmake:
	cmake -B build

build:
	cmake --build build

run:
	./build/bin/Sim

mem:
	valgrind --leak-check=full -v ./build/bin/Sim

clean:
	rm -rf ./build