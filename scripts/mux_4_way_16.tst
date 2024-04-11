LOAD mux_4_way_16;

TEST 'choosing correct values' {
	VAR m: mux_4_way_16;


	SET m.a = 1;
	SET m.b = 2;
	SET m.c = 3;
	SET m.d = 4;

	// Select the first one.
	SET m.sel = 0;
	EVAL;
	REQUIRE m.out IS 1;

	// Select the second one.
	SET m.sel = 1;
	EVAL;
	REQUIRE m.out IS 2;

	// Select the third one.
	SET m.sel = 2;
	EVAL;
	REQUIRE m.out IS 3;

	// Select the fourth one.
	SET m.sel = 3;
	EVAL;
	REQUIRE m.out IS 4;
}

TEST 'pipe value' {
	VAR m1: mux_4_way_16;
	VAR m2: mux_4_way_16;

	SET m1.a = 32;
	SET m1.c = 27;

	SET m2.b = m1.a;
	SET m2.c = m1.c;

	SET m2.sel = 1;
	EVAL;

	REQUIRE (m2.out IS m1.a);

	SET m2.sel = 2;
	EVAL;
	REQUIRE (m2.out IS m1.c);
	
}
