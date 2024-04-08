LOAD or;

TEST 'or 0 0' {
	VAR a: or;
	EVAL;
	REQUIRE (a.a IS 0)
	    AND (a.b IS 0)
		AND (a.out IS 0);
}

TEST 'or 1 0' {
	VAR a: or;
	SET a.a = 1;
	EVAL;
	REQUIRE (a.a IS 1)
	    AND (a.b IS 0)
		AND (a.out IS 1);
}

TEST 'or 0 1' {
	VAR a: or;
	SET a.b = 1;
	EVAL;
	REQUIRE (a.a IS 0)
	    AND (a.b IS 1)
		AND (a.out IS 1);
}

TEST 'or 1 1' {
	VAR a: or;
	SET a.a = 1;
	SET a.b = 1;
	EVAL;
	REQUIRE (a.a IS 1)
	    AND (a.b IS 1)
		AND (a.out IS 1);
}
