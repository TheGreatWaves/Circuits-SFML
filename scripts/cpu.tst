LOAD cpu;

TEST 'A=5' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	SET c.instruction = 5;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 1
	    AND c.d_reg IS 0
	    AND c.a_reg IS 5;
}

TEST 'D=1' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	SET c.instruction = 61392;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 1
	    AND c.outM IS 1
	    AND c.a_reg IS 0
	    AND c.d_reg IS 1;
}

TEST 'A=5 D=1' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// D=1
	SET c.instruction = 61392;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 1
	    AND c.d_reg IS 1;

	// A=5
	SET c.instruction = 5;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 2
	    AND c.a_reg IS 5;
}

TEST 'D=1 A=5' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// A=5
	SET c.instruction = 5;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 1
	    AND c.a_reg IS 5;

	// D=1
	SET c.instruction = 61392;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 2
	    AND c.d_reg IS 1;
}

TEST 'D=1 A=5 D=D+1' {
	VAR c: cpu;
	REQUIRE c.pc IS 0;

	// A=5
	SET c.instruction = 5;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 1
	    AND c.a_reg IS 5;

	// D=1
	SET c.instruction = 61392;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 2
	    AND c.a_reg IS 5
	    AND c.d_reg IS 1;

	// D=D+1
	SET c.instruction = 59344;
	SET c.clock = 1; EVAL;
	SET c.clock = 0; EVAL;
	REQUIRE c.pc IS 3
		AND c.outM IS 2
		AND c.d_reg IS c.outM;
}

