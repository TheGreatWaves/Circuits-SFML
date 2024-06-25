LOAD cpu;

TEST 'A=5' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	SET c.instruction = 5;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 1
	    AND c.d_reg IS 0
	    AND c.addressM IS 5;
}

TEST 'D=1' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	SET c.instruction = 61392;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 1
	    AND c.outM IS 1
	    AND c.addressM IS 0
	    AND c.d_reg IS 1;
}

TEST 'D=1 A=5' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// D=1
	SET c.instruction = 61392;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 1
		AND c.addressM IS 0
	    AND c.d_reg IS 1;

	// A=5
	SET c.instruction = 5;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 2
	    AND c.addressM IS 5;
}

TEST 'A=5 D=1' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// A=5
	SET c.instruction = 5;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 1;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 1
		AND c.C_instruction IS 0
		AND c.jump IS 1
		AND c.load_pc IS 0
	    AND c.addressM IS 5;

	// D=1
	SET c.instruction = 61392;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 2
		AND c.addressM IS 5
	    AND c.d_reg IS 1;

}

TEST 'D=1 A=5 D=A' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// A=5
	SET c.instruction = 5;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 1
	    AND c.addressM IS 5;

	// D=1
	SET c.instruction = 61392;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 2
	    AND c.addressM IS 5
	    AND c.d_reg IS 1;

	// D=A
	SET c.instruction = 60432;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 3
		AND c.outM IS c.addressM
		AND c.d_reg IS c.addressM;
}

TEST 'D=1 D=D+1' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// D=1
	SET c.instruction = 61392;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 1
	    AND c.d_reg IS 1;

	// D=D+1
	SET c.instruction = 59344;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 2
		AND c.addressM IS 0
	    AND c.d_reg IS 2;
}

TEST 'D=1 A=15 D=D+1 D=A D=D+1' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// D=1
	SET c.instruction = 61392;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 1
	    AND c.d_reg IS 1;

	// A=15
	SET c.instruction = 15;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 2
		AND c.d_reg IS 1
	    AND c.addressM IS 15;

	// D=D+1
	SET c.instruction = 59344;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 3
	    AND c.d_reg IS 2
	    AND c.addressM IS 15;

	// D=A
	SET c.instruction = 60432;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 4
		AND c.addressM IS 15
		AND c.outM IS c.addressM
		AND c.d_reg IS c.addressM;

	// D=D+1
	SET c.instruction = 59344;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 5
	    AND c.addressM IS 15
	    AND c.d_reg IS 16;
	
}

TEST 'A=15 D=A' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// A=15
	SET c.instruction = 15;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 1
	    AND c.addressM IS 15;
	
	// D=A
	SET c.instruction = 60432;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 2
		AND c.addressM IS 15
		AND c.outM IS c.addressM
		AND c.d_reg IS c.addressM;
}

TEST 'D=1 A=D' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// D=1
	SET c.instruction = 61392;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 1
		AND c.addressM IS 0
		AND c.d_reg IS 1;

	// A=D
	SET c.instruction = 58144;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 2
		AND c.d_reg IS 1
		AND c.addressM IS c.d_reg;
}

TEST 'D=1 A=D A=A+1 D=A' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// D=1
	SET c.instruction = 61392;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 1
		AND c.addressM IS 0
		AND c.d_reg IS 1;

	// A=D
	SET c.instruction = 58144;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 2
		AND c.d_reg IS 1
		AND c.addressM IS c.d_reg;

	// A=A+1
	SET c.instruction = 60896;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 3
		AND c.addressM IS 2;

	// D=A
	SET c.instruction = 60432;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 4
		AND c.outM IS c.addressM
		AND c.d_reg IS c.addressM;
}

TEST 'A=15 D=A A=7 D=D-A' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// A=15
	SET c.instruction = 15;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.addressM IS 15;

	// D=A
	SET c.instruction = 60432;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.d_reg IS c.addressM;

	// A=7
	SET c.instruction = 7;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.addressM IS 7;

	// D=D-A
	SET c.instruction = 58576;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.d_reg IS 8;
}

TEST 'A=10 AD=A-1' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// A=10
	SET c.instruction = 10;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.addressM IS 10;

	// AD=A-1
	SET c.instruction = 60592;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.addressM IS 9
	    AND c.d_reg IS 9;
}

TEST 'D=!D' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// D=!D
	SET c.instruction = 58192;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.d_reg IS 65535;
}

TEST 'A=15' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// @15
	SET c.instruction = 15;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.addressM IS 15;

	// M=A
	SET c.instruction = 60424;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.writeM IS 1
		AND c.outM IS 15
		AND c.addressM IS 15;
}

TEST 'A=M' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	SET c.inM = 15;

	// A=M
	SET c.instruction = 64544;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.addressM IS 15;

}

TEST 'ADM=M' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	SET c.inM = 15;

	// ADM=M
	SET c.instruction = 64568;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.addressM IS 15
		AND c.d_reg IS 15
		AND c.outM IS 15;
}

TEST 'A=15 0;JMP' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// @15
	SET c.instruction = 15;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.addressM IS 15;

	// 0;JMP
	SET c.instruction = 60039;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 15;
}

TEST 'A=15 0;JEQ' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// @15
	SET c.instruction = 15;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.addressM IS 15;

	// 0;JEQ
	SET c.instruction = 60034;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 15;
}


TEST 'A=15 A;JGT' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// @15
	SET c.instruction = 15;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.addressM IS 15;

	// A;JGT
	SET c.instruction = 60417;
	SET c.clock = 0; EVAL;
	SET c.clock = 1; EVAL;
	REQUIRE c.pc IS 15;
}


