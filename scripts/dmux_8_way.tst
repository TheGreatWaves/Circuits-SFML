LOAD dmux_8_way;

TEST 'correct channels' {
	VAR d: dmux_8_way;
	SET d.in = 1;

	// Choose a
	SET d.sel = 0;
	EVAL;

	REQUIRE (d.a IS 1)
	    AND (d.b IS 0)
	    AND (d.c IS 0)
	    AND (d.d IS 0)
	    AND (d.e IS 0)
	    AND (d.f IS 0)
	    AND (d.g IS 0)
	    AND (d.h IS 0);

	// Choose b
	SET d.sel = 1;
	EVAL;

	REQUIRE (d.a IS 0)
	    AND (d.b IS 1)
	    AND (d.c IS 0)
	    AND (d.d IS 0)
	    AND (d.e IS 0)
	    AND (d.f IS 0)
	    AND (d.g IS 0)
	    AND (d.h IS 0);

	// Choose c
	SET d.sel = 2;
	EVAL;

	REQUIRE (d.a IS 0)
	    AND (d.b IS 0)
	    AND (d.c IS 1)
	    AND (d.d IS 0)
	    AND (d.e IS 0)
	    AND (d.f IS 0)
	    AND (d.g IS 0)
	    AND (d.h IS 0);

	// Choose d
	SET d.sel = 3;
	EVAL;

	REQUIRE (d.a IS 0)
	    AND (d.b IS 0)
	    AND (d.c IS 0)
	    AND (d.d IS 1)
	    AND (d.e IS 0)
	    AND (d.f IS 0)
	    AND (d.g IS 0)
	    AND (d.h IS 0);

	// Choose e
	SET d.sel = 4;
	EVAL;

	REQUIRE (d.a IS 0)
	    AND (d.b IS 0)
	    AND (d.c IS 0)
	    AND (d.d IS 0)
	    AND (d.e IS 1)
	    AND (d.f IS 0)
	    AND (d.g IS 0)
	    AND (d.h IS 0);

	// Choose f
	SET d.sel = 5;
	EVAL;

	REQUIRE (d.a IS 0)
	    AND (d.b IS 0)
	    AND (d.c IS 0)
	    AND (d.d IS 0)
	    AND (d.e IS 0)
	    AND (d.f IS 1)
	    AND (d.g IS 0)
	    AND (d.h IS 0);

	// Choose g
	SET d.sel = 6;
	EVAL;

	REQUIRE (d.a IS 0)
	    AND (d.b IS 0)
	    AND (d.c IS 0)
	    AND (d.d IS 0)
	    AND (d.e IS 0)
	    AND (d.f IS 0)
	    AND (d.g IS 1)
	    AND (d.h IS 0);

	// Choose h
	SET d.sel = 7;
	EVAL;

	REQUIRE (d.a IS 0)
	    AND (d.b IS 0)
	    AND (d.c IS 0)
	    AND (d.d IS 0)
	    AND (d.e IS 0)
	    AND (d.f IS 0)
	    AND (d.g IS 0)
	    AND (d.h IS 1);
}
