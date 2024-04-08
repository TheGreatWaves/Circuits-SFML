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

## Pins

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

## HDL

> [!IMPORTANT]  
> The extension for a HDL file is `.hdl`. They should be declared inside the `scripts` folder.

Having to manually write chip descriptions is error-prone and too cumbersome to be considered productive. This problem is mitigated by introducing another layer of abstraction which can help us streamline the process of chip creation: the HDL. The HDL language is based on the HDL language which is used in the nand2tetris course.

The HDL consists of only 5 keywords.
- `CHIP`: Declaring a new chip.
- `IN`: Defining global input pins.
- `OUT`: Defining global output pins.
- `PARTS`: Defining subchips and wiring.
- `SERIALIZE`: Declare that the chip is precomputable.

Here is an example of how one might define a `not` chip using the HDL.

```rust
SERIALIZE CHIP not {
	IN in;
	OUT out;

	PARTS:
	nand(a=in, b=in, out=out);
}
```
Here is the corresponding `not.gate` which gets generated.
```rust
// THIS DEFINITION FILE IS AUTO GENERATED. DO NOT EDIT MANUALLY.

// DEPENDENCIES.
need nand

// BEGINNING OF GATE DECLARATION.
create not

// GLOBAL INPUT PINS.
// INPUT PINS MAPPING.
//  0 in
input 1

// GLOBAL OUTPUT PINS.
// OUTPUT PINS MAPPING.
// 255 out
output 1

// SUBGATES.
add nand

// LINKAGES.
wire 0 1
wire 0 2
wire 256 255

// SERIALIZATION.
precompute

// DEFINITION COMPLETE.
save

// END OF FILE.
```
Additionally, a meta file is also generated in order to retain chip input and output names.
```rust
// THIS META FILE IS AUTO GENERATED. DO NOT EDIT MANUALLY.

// GATE NAME.
not

// BUSES DECLARATION.
BUSES 0

// GLOBAL INPUT PINS.
INPUTS 1
in

// GLOBAL OUTPUT PINS.
OUTPUTS 1
out

// END OF FILE.
```
The HDL also support the construct of buses. Notice that in the real implementation of the chip, we have no notion of what a bus is, we only have pins and wires. The logic of buses is handled entirely by the compiler.

Here is how one might define a `not4`.
```rust
SERIALIZE CHIP not_4 {
    IN in[4];
    OUT out[4];

    PARTS:
    not(in=in[0], out=out[0]);
    not(in=in[1], out=out[1]);
    not(in=in[2], out=out[2]);
    not(in=in[3], out=out[3]);
}
```

This allows for something like this:
```rust
// This chip does nothing. It is only for demonstration.
CHIP nothing_4 {
    IN in[4];
    OUT out[4];

    PARTS:
    // Invert all, save in out1.
    // Notice that out1 is created dynamically.
    not_4(in=in, out=out1);
      
    // Invert out1, send to global out bus.
    not_4(in=out1, out=out);
}
```

## Testing

> [!IMPORTANT]  
> The extension for a test file is `.tst`. They should be declared inside the `scripts` folder.

Correctness is extremely important when developing anything, chips are no different. Since chips are expected to behave in a predictable manner, it is a no-brainer that testing facilities should be provided.

Tests can be written using only 7 keywords.
- `LOAD <chip>`: Loads the specified chip.
- `TEST <test name>`: Declaration of a new test.
- `VAR <name>: <chip>`: Declaration of a new variable.
- `SET <name>.<member> = <value>`: Setting chip member value.
- `EVAL`: Simulate one tick.
- `REQUIRE <condition>`: Assert condition.
- `AND <condition>`: Chaining conditions.

Here is an example of how a `not` chip can be tested.
```rust
LOAD not;

TEST 'not 0' {
	VAR n: not;
	SET n.in = 0;
	EVAL;
	REQUIRE n.in IS 0
            AND n.out IS 1;
}

TEST 'not 1' {
	VAR n: not;
	SET n.in = 1;
	EVAL;
	REQUIRE n.in IS 1
            AND n.out IS 0;
}
```

The test may be invoked in the CLI via `test <chip name>`.

## CLI

> [!TIP]
> You can type `help` to show commands.

`compile <chip>`: Compiles HDL file. Specify `all` to compile all HDL files.

`gui`: Start GUI mode.

`info`: Display basic information about the simulator.

`list`: Display all chips defined in the simulator.

`load <chip>`: Load a chip image.

`serialize <chip>`: Precompute the result of the specified gate.

`test <chip>`: Run test. Specify `all` to run all test files.

`quit`: Exit the simulator.

## GUI

The GUI mode is recommended to only be used for demonstrative/visualization purposes.

Keybinds:
- `E`: Toggle between `edit` and `wire` mode.
- `C`: Clear the board.

![droppers](https://github.com/TheGreatWaves/Digital-Logic-SFML/assets/106456906/abf0ab66-ec0c-4595-8169-20cb7cccebf0)

