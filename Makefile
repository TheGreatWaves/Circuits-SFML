.PHONY: clean cmake build run

cmake:
	cmake -B build

build:
	cmake --build build

run:
	./build/bin/Sim

clean:
	rm -rf ./build