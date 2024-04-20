LOAD pc;

TEST 'inc' {
	VAR p: pc;
	SET p.clock = 1;
	SET p.inc = 1;
	EVAL;
	REQUIRE p.out IS 1;

	// tick-tock
	SET p.clock = 0;
	EVAL;
	SET p.clock = 1;
	EVAL;

	REQUIRE p.out IS 2;

	// tick-tock
	SET p.clock = 0;
	EVAL;
	SET p.clock = 1;
	EVAL;

	REQUIRE p.out IS 3;
}

TEST 'load' {
	VAR p: pc;

	SET p.clock = 1;
	SET p.load = 1;
	SET p.in = 1589;
	EVAL;
	REQUIRE p.out IS 1589;

	// tick-tock
	SET p.clock = 0;
	SET p.load = 0;
	EVAL;
	SET p.clock = 1;
	EVAL;

	REQUIRE p.out IS 1589;

	SET p.in = 2354;
	SET p.load = 1;
	EVAL;

	REQUIRE p.out IS 1589;

	// tick-tock
	SET p.clock = 0;
	EVAL;
	SET p.clock = 1;
	EVAL;

	REQUIRE p.out IS 2354;
}

TEST 'reset' {
	VAR p: pc;

	SET p.clock = 1;
	SET p.load = 1;
	SET p.in = 123;
	EVAL;

	SET p.clock = 0;
	SET p.load = 0;
	EVAL;
	REQUIRE p.out IS 123;

	SET p.clock = 1;
	SET p.in = 0;
	SET p.reset = 1;
	EVAL;

	REQUIRE p.out IS 0;
}

TEST 'load, inc, reset' {
	VAR p: pc;

	SET p.in = 256;
	SET p.load = 1;
	SET p.clock = 1;
	EVAL;

	REQUIRE p.out IS 256;

	SET p.load = 0;
	SET p.clock = 0;
	EVAL;

	SET p.inc = 1;
	SET p.clock = 1;
	EVAL;

	REQUIRE p.out IS 257;

	SET p.inc = 0;
	SET p.clock = 0;
	EVAL;

	REQUIRE p.out IS 257;

	SET p.reset = 1;
	SET p.clock = 1;
	EVAL;

	REQUIRE p.out IS 0;
	
}
