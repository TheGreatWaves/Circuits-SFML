LOAD or_16;

TEST 'or_16 65535 0' {
	VAR a: or_16;
	SET a.a = 65535;
	EVAL;
	REQUIRE (a.a IS 65535)
	    AND (a.b IS 0)
		AND (a.out IS 65535);
}


TEST 'or_16 65535 65535' {
	VAR a: or_16;
	SET a.a = 65535;
	SET a.b = 65535;
	EVAL;
	REQUIRE (a.a IS 65535)
	    AND (a.b IS 65535)
		AND (a.out IS 65535);
}

TEST 'or_16 43690 21845' {
	VAR a: or_16;
	SET a.a = 43690;
	SET a.b = 21845;
	EVAL;
	REQUIRE (a.a IS 43690)
	    AND (a.b IS 21845)
		AND (a.out IS 65535);
}
