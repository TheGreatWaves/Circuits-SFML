LOAD and_16;

TEST 'and_16 65535 0' {
	VAR a: and_16;
	SET a.a = 65535;
	EVAL;
	REQUIRE (a.a IS 65535)
	    AND (a.b IS 0)
		AND (a.out IS 0);
}

TEST 'and_16 65535 1' {
	VAR a: and_16;
	SET a.a = 65535;
	SET a.b = 1;
	EVAL;
	REQUIRE (a.a IS 65535)
	    AND (a.b IS 1)
		AND (a.out IS 1);
}

TEST 'and_16 65535 65535' {
	VAR a: and_16;
	SET a.a = 65535;
	SET a.b = 65535;
	EVAL;
	REQUIRE (a.a IS 65535)
	    AND (a.b IS 65535)
		AND (a.out IS 65535);
}

TEST 'and_16 65535 21845' {
	VAR a: and_16;
	SET a.a = 65535;
	SET a.b = 21845;
	EVAL;
	REQUIRE (a.a IS 65535)
	    AND (a.b IS 21845)
		AND (a.out IS 21845);
}
