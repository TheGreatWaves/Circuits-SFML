LOAD alu;

TEST 'not x' {
	VAR a: alu;

	SET a.nx = 1;
	SET a.f = 1;
	EVAL;
	REQUIRE a.out IS 65535
	    AND a.ng IS 1;

	SET a.x = 43690;
	EVAL;
	REQUIRE a.out IS 21845
	    AND a.zr IS 0;

	SET a.x = 65535;
	EVAL;
	REQUIRE a.out IS 0 AND a.zr IS 1;
}

TEST 'not y' {
	VAR a: alu;

	SET a.ny = 1;
	SET a.f = 1;
	EVAL;
	REQUIRE a.out IS 65535
	    AND a.ng IS 1;

	SET a.y = 43690;
	EVAL;
	REQUIRE a.out IS 21845
	    AND a.zr IS 0;

	SET a.y = 65535;
	EVAL;
	REQUIRE a.out IS 0 AND a.zr IS 1;
}


TEST 'x and y' {
	VAR a: alu;

	SET a.ny = 1;
	SET a.nx = 1;
	SET a.f = 0;
	EVAL;
	REQUIRE a.out IS 65535
	    AND a.ng IS 1;

	// Test masking

	SET a.x = 65535;
	SET a.y = 21845;

	// Don't negate
	SET a.ny = 0;
	SET a.nx = 0;
	EVAL;
	REQUIRE a.out IS 21845;
}


TEST 'x plus y' {
	VAR a: alu;

	// Set to add
	SET a.f = 1;

	// 5 + 7
	SET a.x = 5;
	SET a.y = 7;
	EVAL;
	REQUIRE a.out IS 12;

	// 6133 + 1235
	SET a.x = 6133;
	SET a.y = 1235;
	EVAL;
	REQUIRE a.out IS 7368;
}

TEST 'negate output' {
	VAR a: alu;

	// Full bitmask for x and y
	SET a.nx = 1;
	SET a.ny = 1;

	// Choose AND operation
	SET a.f = 0;

	// Negate output, should be zero
	SET a.no = 1;

	EVAL;

	REQUIRE a.out IS 0;

	// Set x and y values to full bitmask, so their negation is 0
	SET a.x = 65535;
	SET a.y = 65535; 

	EVAL;

	// Negate zero should be full bitmask
	REQUIRE a.out IS 65535;
}


