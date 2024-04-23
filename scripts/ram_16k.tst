LOAD ram_16k;

TEST 'loading and setting values' {
	VAR r: ram_16k;

	// Load values to address 3 and retrieve them back.
	SET r.address = 3;

	// Load value.
	SET r.load = 1;
	SET r.in = 3889;
	SET r.clock = 1;

	EVAL;

	// Reset.
	SET r.load = 0;
	SET r.in = 0;
	SET r.clock = 0;
	EVAL;

	REQUIRE r.out IS 3889;

	// Get value at address 0.
	SET r.address = 0;
	SET r.clock = 1;
	EVAL;
	REQUIRE r.out IS 0;

	// Load value at address 0.
	SET r.in = 1734;
	SET r.load = 1;
	EVAL;

	// Reset.
	SET r.load = 0;
	SET r.in = 0;
	SET r.clock = 0;
	EVAL;

	REQUIRE r.out IS 1734;

	// Get value at address 3 again.
	SET r.address = 3;
	SET r.clock = 1;
	EVAL;

	REQUIRE r.out IS 3889;
}

