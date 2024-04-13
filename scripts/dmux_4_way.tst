LOAD dmux_4_way;

TEST 'correct channels' {
	VAR d: dmux_4_way;
	SET d.in = 1;

	// Choose a
	SET d.sel = 0;
	EVAL;

	REQUIRE (d.a IS 1)
	    AND (d.b IS 0)
	    AND (d.c IS 0)
	    AND (d.d IS 0);

	// Choose b
	SET d.sel = 1;
	EVAL;

	REQUIRE (d.a IS 0)
	    AND (d.b IS 1)
	    AND (d.c IS 0)
	    AND (d.d IS 0);

	// Choose c
	SET d.sel = 2;
	EVAL;

	REQUIRE (d.a IS 0)
	    AND (d.b IS 0)
	    AND (d.c IS 1)
	    AND (d.d IS 0);

	// Choose d
	SET d.sel = 3;
	EVAL;

	REQUIRE (d.a IS 0)
	    AND (d.b IS 0)
	    AND (d.c IS 0)
	    AND (d.d IS 1);
}
