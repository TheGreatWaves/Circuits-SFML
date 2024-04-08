.PHONY: clean cmake build run

cmake:
	cmake -B build

build:
	cmake --build build

run:
	./build/bin/Sim

mem:
	valgrind --leak-check=full -v ./build/bin/Sim

reset:
	rm -rf ./scripts/*
	rm -rf ./gates/*

clean:
	rm -rf ./build