
LOAD register;

TEST 'storing a register' {
	VAR b: register;

	// Should be zero by default
	EVAL;
	REQUIRE b.out IS 0;

	// Set flags for loading value
	SET b.load = 1;
	SET b.in = 2847;

	// Since clock isn't on, nothing should happen.
	EVAL;
	REQUIRE b.out IS 0;

	// Clock on, now store
	SET b.clock = 1;
	EVAL;
	REQUIRE b.out IS 2847;

	// Check that value is stored properly
	SET b.clock = 0;
	SET b.in = 0;
	SET b.load = 0;
	EVAL;
	REQUIRE b.out IS 2847;

	// Nothing should happen at the next clock cycle
	SET b.clock = 1;
	EVAL;
	REQUIRE b.out IS 2847;

	// Set flags for storing 0, nothing should happen
	SET b.clock = 0;
	SET b.load = 1;
	SET b.in = 65535;
	EVAL;
	REQUIRE b.out IS 2847;

	// Next clock cycle, value 0 is stored
	SET b.clock = 1;
	EVAL;
	REQUIRE b.out IS 65535;
}
