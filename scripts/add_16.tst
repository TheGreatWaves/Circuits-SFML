LOAD add_16;

TEST 'add 1 1' {
	VAR a: add_16;
	SET a.a = 1;
	SET a.b = 1;
	EVAL;
	REQUIRE a.a IS 1 AND a.b IS 1 AND a.out IS 2;
}

TEST 'add 5000 0' {
	VAR a: add_16;
	SET a.a = 5000;
	EVAL;
	REQUIRE a.a IS 5000 AND a.b IS 0 AND a.out IS 5000;
}

TEST 'add 0 5000' {
	VAR a: add_16;
	SET a.b = 5000;
	EVAL;
	REQUIRE a.b IS 5000 AND a.a IS 0 AND a.out IS 5000;
}

TEST 'add 256 17' {
	VAR a: add_16;
	SET a.a = 256;
	SET a.b = 17;
	EVAL;
	REQUIRE a.a IS 256 AND a.b IS 17 AND a.out IS 273;
}

TEST 'add 2000 2000' {
	VAR a: add_16;
	SET a.a = 2000;
	SET a.b = 2000;
	EVAL;
	REQUIRE a.a IS 2000 AND a.b IS 2000 AND a.out IS 4000;
}
