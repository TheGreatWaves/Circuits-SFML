LOAD mux_8_way_16;

TEST 'choosing correct values' {
	VAR m: mux_8_way_16;

	SET m.a = 1;
	SET m.b = 2;
	SET m.c = 3;
	SET m.d = 4;
	SET m.e = 5;
	SET m.f = 6;
	SET m.g = 7;
	SET m.h = 8;

	REQUIRE m.a IS 1;
	REQUIRE m.b IS 2;
	REQUIRE m.c IS 3;
	REQUIRE m.d IS 4;
	REQUIRE m.e IS 5;
	REQUIRE m.f IS 6;
	REQUIRE m.g IS 7;
	REQUIRE m.h IS 8;

	SET m.sel = 0;
	EVAL;
	REQUIRE m.out IS 1;

	SET m.sel = 1;
	EVAL;
	REQUIRE m.out IS 2;

	SET m.sel = 2;
	EVAL;
	REQUIRE m.out IS 3;

	SET m.sel = 3;
	EVAL;
	REQUIRE m.out IS 4;

	SET m.sel = 4;
	EVAL;
	REQUIRE m.out IS 5;

	SET m.sel = 5;
	EVAL;
	REQUIRE m.out IS 6;

	SET m.sel = 6;
	EVAL;
	REQUIRE m.out IS 7;

	SET m.sel = 7;
	EVAL;
	REQUIRE m.out IS 8;
}
