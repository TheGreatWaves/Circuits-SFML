.PHONY: clean cmake build run

cmake:
	cmake -B build -DCMAKE_BUILD_TYPE=Release

build:
	cmake --build build

run:
	./build/bin/main

clean:
	rm -rf ./build
