LOAD computer;

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
	REQUIRE com.current_instruction_address IS 0
		AND com.writeM IS 0;
	SET com.reset = 0;

	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.current_instruction_address IS 1
		AND com.writeM IS 1
		AND com.addressM IS 15
		AND com.outM IS 15;

	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.current_instruction_address IS 2
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
	REQUIRE com.current_instruction_address IS 0
		AND com.addressM IS 1337
		AND com.writeM IS 0;
	SET com.reset = 0;

	// D=A
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.current_instruction_address IS 1
		AND com.writeM IS 0
		AND com.outM IS 1337;

	// @15
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.current_instruction_address IS 2
		AND com.addressM IS 15;

	// M=D+A
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.current_instruction_address IS 3
		AND com.outM IS 1352
		AND com.writeM IS 1
		AND com.addressM IS 15;

	// D=M
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.current_instruction_address IS 4
		AND com.outM IS 1352
		AND com.writeM IS 0
		AND com.addressM IS 15;
}

TEST 'jump' {
	VAR com: computer;

	// Write instructions
	SET com.clock         = 1;
	SET com.load          = 1;

	// @1337
	SET com.in            = 1337;
	SET com.write_address = 0;
	EVAL;

	// 0;JMP
	SET com.in            = 60039;
	SET com.write_address = 1;
	EVAL;

	// M=1
	SET com.in            = 61384;
	SET com.write_address = 1338;
	EVAL;

	// Execute instructions
	SET com.load = 0;
	SET com.in   = 0;
	SET com.reset = 1;
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	SET com.reset = 0;
	REQUIRE com.current_instruction_address IS 0;

	// @1337
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.current_instruction_address IS 1
		AND com.addressM IS 1337
		AND com.writeM IS 0;

	// 0;JMP
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.current_instruction_address IS 1337
		AND com.writeM IS 0
		AND com.outM IS 0;

	// M=1
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.current_instruction_address IS 1338
		AND com.writeM IS 1
		AND com.outM IS 1;
}

TEST 'jump cond' {
	VAR com: computer;

	// Write instructions
	SET com.clock         = 1;
	SET com.load          = 1;

	// D=1
	SET com.in            = 61392;
	SET com.write_address = 0;
	EVAL;

	// @6
	SET com.in            = 6;
	SET com.write_address = 1;
	EVAL;

	// D;JGT
	SET com.in            = 58113;
	SET com.write_address = 2;
	EVAL;

	// @666
	SET com.in            = 666;
	SET com.write_address = 3;
	EVAL;

	// @7
	SET com.in            = 7;
	SET com.write_address = 4;
	EVAL;

	// 0;JMP
	SET com.in            = 60039;
	SET com.write_address = 5;
	EVAL;

	// @155
	SET com.in            = 155;
	SET com.write_address = 6;
	EVAL;

	// Execute instructions
	SET com.load = 0;
	SET com.in   = 0;
	SET com.reset = 1;

	// D=1
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	SET com.reset = 0;
	REQUIRE com.instruction_memory IS 61392
		AND com.current_instruction_address IS 0;

	// @6
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.instruction_memory IS 6
		AND com.current_instruction_address IS 1;

	// D;JGT
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.instruction_memory IS 58113
		AND com.current_instruction_address IS 2;

	// @155
	SET com.clock = 0; EVAL;
	SET com.clock = 1; EVAL;
	REQUIRE com.instruction_memory IS 155
		AND com.addressM IS 155
		AND com.current_instruction_address IS 6;
}
