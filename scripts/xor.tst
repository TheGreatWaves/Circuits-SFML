LOAD xor;

TEST 'xor 0 0' {
	VAR a: xor;
	EVAL;
	REQUIRE (a.a IS 0)
	    AND (a.b IS 0)
		AND (a.out IS 0);
}

TEST 'xor 1 0' {
	VAR a: xor;
	SET a.a = 1;
	EVAL;
	REQUIRE (a.a IS 1)
	    AND (a.b IS 0)
		AND (a.out IS 1);
}

TEST 'xor 0 1' {
	VAR a: xor;
	SET a.b = 1;
	EVAL;
	REQUIRE (a.a IS 0)
	    AND (a.b IS 1)
		AND (a.out IS 1);
}

TEST 'xor 1 1' {
	VAR a: xor;
	SET a.a = 1;
	SET a.b = 1;
	EVAL;
	REQUIRE (a.a IS 1)
	    AND (a.b IS 1)
		AND (a.out IS 0);
}
