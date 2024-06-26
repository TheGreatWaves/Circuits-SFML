LOAD computer;

// TEST 'setting instruction' {
// 	VAR com: computer;

// 	// @15
// 	SET com.clock   = 1;
// 	SET com.in      = 15;
// 	SET com.write_address = 0;
// 	SET com.read_address = com.write_address;
// 	SET com.load    = 1;
// 	EVAL;
// 	REQUIRE com.instruction_memory IS 15;

// 	// @16
// 	SET com.clock         = 1;
// 	SET com.in            = 16;
// 	SET com.write_address = 1;
// 	SET com.read_address = com.write_address;
// 	SET com.load          = 1;
// 	EVAL;
// 	REQUIRE com.instruction_memory IS 16;

// 	// Try getting the previous value
// 	SET com.clock   = 1;
// 	SET com.in      = 0;
// 	SET com.write_address = 0;
// 	SET com.read_address = com.write_address;
// 	SET com.load    = 0;
// 	EVAL;
// 	REQUIRE com.instruction_memory IS 15;
// }

TEST 'A=15 M=A' {
	VAR com: computer;

	// Write instructions
	SET com.clock         = 1;
	SET com.load          = 1;

	// @15
	SET com.in            = 15;
	SET com.write_address = 0;
	EVAL;

	// M=A
	SET com.in            = 60424;
	SET com.write_address = 1;
	EVAL;

	// D=M
	SET com.in            = 64528;
	SET com.write_address = 2;
	EVAL;

	// Execute instructions
	SET com.load = 0;
	SET com.in   = 0;
	SET com.reset = 1;
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.next_instruction_address IS 0
		AND com.writeM IS 0;
	SET com.reset = 0;

	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.next_instruction_address IS 1
		AND com.writeM IS 1
		AND com.addressM IS 15
		AND com.outM IS 15;

	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.next_instruction_address IS 2
		AND com.memory_out IS 15;
}

TEST 'A=1337 D=A A=15 M=D' {
	VAR com: computer;

	// Write instructions
	SET com.clock         = 1;
	SET com.load          = 1;

	// @1337
	SET com.in            = 1337;
	SET com.write_address = 0;
	EVAL;

	// D=A
	SET com.in            = 60432;
	SET com.write_address = 1;
	EVAL;

	// @15
	SET com.in            = 15;
	SET com.write_address = 2;
	EVAL;

	// M=D+A
	SET com.in            = 57480;
	SET com.write_address = 3;
	EVAL;

	// D=M
	SET com.in            = 64528;
	SET com.write_address = 4;
	EVAL;

	// Execute instructions
	SET com.load = 0;
	SET com.in   = 0;
	SET com.reset = 1;

	// @1337
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.next_instruction_address IS 0
		AND com.addressM IS 1337
		AND com.writeM IS 0;
	SET com.reset = 0;

	// D=A
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.next_instruction_address IS 1
		AND com.writeM IS 0
		AND com.outM IS 1337;

	// @15
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.next_instruction_address IS 2
		AND com.addressM IS 15;

	// M=D+A
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.next_instruction_address IS 3
		AND com.outM IS 1352
		AND com.writeM IS 1
		AND com.addressM IS 15;

	// D=M
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.next_instruction_address IS 4
		AND com.outM IS 1352
		AND com.writeM IS 0
		AND com.addressM IS 15;
}
