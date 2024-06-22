LOAD cpu;

TEST 'a as 5' {
	VAR c: cpu;

	SET c.instruction = 5;

	SET c.clock = 1;
	EVAL;

	REQUIRE c.a_mux_out IS 5;
	REQUIRE c.a_reg_out IS 5;
}
