LOAD mux_16;

TEST 'mux_16 24 34 0' {
	VAR m: mux_16;
	SET m.a = 24;
	SET m.b = 34;
	SET m.sel = 0;
	EVAL;
	REQUIRE (m.out IS 24);
}

TEST 'mux_16 1 0 0' {
	VAR m: mux_16;
	SET m.a = 1;
	SET m.sel = 0;
	EVAL;
	REQUIRE (m.out IS 1);
}

TEST 'mux_16 24 34 1' {
	VAR m: mux_16;
	SET m.a = 24;
	SET m.b = 34;
	SET m.sel = 1;
	EVAL;
	REQUIRE (m.out IS 34);
}
