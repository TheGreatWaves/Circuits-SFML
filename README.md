# Digital Logic Simulator

This project was inspired by Sebastian Lague's videos on digital logic and the amazing [nand2tetris](https://www.nand2tetris.org/) course.

## Build and Run
```sh
# Compile cmake.
make cmake

# Build the project.
make build

# Run the project.
make run

# Clean the build.
make clean
```
## Basic

The underlying logic library is implemented only using the notion of `pins` and `wires`. Signals travel from input pins and is propagated using breadth first search. The libray only offers one built-in chip: the `nand` gate. To help increase performance, chips may be precomputed and serialized. This will allow the simulation of the chip to simply be an index lookup with the value being the input.

### Pins

Input and output pins share the same container. This means that they have to be segmented somehow. This is done by having output pin entries start at 255 by default. This means that our first global output pin sits at index 255. This does imply that input pins are limited to 254, however this value can be easily changed.

A chip's description may be defined using these basic instructions:
- `need <chip>`: Declared to ensure that the image of the given chip is loaded.
- `create <chip>`: Creates a new chip with the given name.
- `input <N>`: Creates `N` inputs.
- `output <N>`: Creates `N` outputs.
- `add <chip>`: Add a subchip with the given name to the current chip.
- `wire <src> <dst>`: Wire `src` and `dst` pins together.
- `precompute`: Precompute the gate. Warning: Only do this for small arithemetic gates, stateful gates and gates with large input will cause problems.
- `save`: Placed at the end of the file to denote the end of a definition.

## CLI

> [!TIP]
> You can type `help` to show commands.

`compile <chip>`: Compiles HDL file. Specify `all` is compile all HDL files.

`gui`: Start GUI mode.

`info`: Display basic information about the simulator.

`list`: Display all chips defined in the simulator.

`load <chip>`: Load a chip image.

`serialize <chip>`: Precompute the result of the specified gate.

`test <chip>`: Run test. Specify `all` to run all test files.

`quit`: Exit the simulator.

