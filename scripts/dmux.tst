LOAD dmux;

TEST 'dmux 0 0' {
	VAR d: dmux;
	EVAL;
	REQUIRE (d.in IS 0)
	    AND (d.sel IS 0)
		AND (d.a IS 0)
		AND (d.b IS 0);
}

TEST 'dmux 0 1' {
	VAR d: dmux;
	SET d.sel = 1;
	EVAL;
	REQUIRE (d.in IS 0)
	    AND (d.sel IS 1)
		AND (d.a IS 0)
		AND (d.b IS 0);
}

TEST 'dmux 1 0' {
	VAR d: dmux;
	SET d.in = 1;
	EVAL;
	REQUIRE (d.in IS 1)
	    AND (d.sel IS 0)
		AND (d.a IS 1)
		AND (d.b IS 0);
}

TEST 'dmux 1 1' {
	VAR d: dmux;
	SET d.in = 1;
	SET d.sel = 1;
	EVAL;
	REQUIRE (d.in IS 1)
	    AND (d.sel IS 1)
		AND (d.a IS 0)
		AND (d.b IS 1);
}
