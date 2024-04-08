LOAD and;

TEST 'and 0 0' {
	VAR a: and;
	EVAL;
	REQUIRE (a.a IS 0)
	    AND (a.b IS 0)
		AND (a.out IS 0);
}

TEST 'and 1 0' {
	VAR a: and;
	SET a.a = 1;
	EVAL;
	REQUIRE (a.a IS 1)
	    AND (a.b IS 0)
		AND (a.out IS 0);
}

TEST 'and 0 1' {
	VAR a: and;
	SET a.b = 1;
	EVAL;
	REQUIRE (a.a IS 0)
	    AND (a.b IS 1)
		AND (a.out IS 0);
}

TEST 'and 1 1' {
	VAR a: and;
	SET a.a = 1;
	SET a.b = 1;
	EVAL;
	REQUIRE (a.a IS 1)
	    AND (a.b IS 1)
		AND (a.out IS 1);
}
