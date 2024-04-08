LOAD mux;

TEST 'mux 0 0 0' {
	VAR m: mux;
	EVAL;
	REQUIRE (m.a IS 0)
	    AND (m.b IS 0)
	    AND (m.sel IS 0)
	    AND (m.out IS 0);
}

TEST 'mux 0 0 1' {
	VAR m: mux;
	SET m.sel = 1;
	EVAL;
	REQUIRE (m.a IS 0)
	    AND (m.b IS 0)
	    AND (m.sel IS 1)
	    AND (m.out IS 0);
}

TEST 'mux 0 1 0' {
	VAR m: mux;
	SET m.b = 1;
	EVAL;
	REQUIRE (m.a IS 0)
	    AND (m.b IS 1)
	    AND (m.sel IS 0)
	    AND (m.out IS 0);
}

TEST 'mux 0 1 1' {
	VAR m: mux;
	SET m.b = 1;
	SET m.sel = 1;
	EVAL;
	REQUIRE (m.a IS 0)
	    AND (m.b IS 1)
	    AND (m.sel IS 1)
	    AND (m.out IS 1);
}

TEST 'mux 1 0 0' {
	VAR m: mux;
	SET m.a = 1;
	EVAL;
	REQUIRE (m.a IS 1)
	    AND (m.b IS 0)
	    AND (m.sel IS 0)
	    AND (m.out IS 1);
}

TEST 'mux 1 0 1' {
	VAR m: mux;
	SET m.a = 1;
	SET m.sel = 1;
	EVAL;
	REQUIRE (m.a IS 1)
	    AND (m.b IS 0)
	    AND (m.sel IS 1)
	    AND (m.out IS 0);
}

TEST 'mux 1 1 0' {
	VAR m: mux;
	SET m.a = 1;
	SET m.b = 1;
	EVAL;
	REQUIRE (m.a IS 1)
	    AND (m.b IS 1)
	    AND (m.sel IS 0)
	    AND (m.out IS 1);
}

TEST 'mux 1 1 1' {
	VAR m: mux;
	SET m.a = 1;
	SET m.b = 1;
	SET m.sel = 1;
	EVAL;
	REQUIRE (m.a IS 1)
	    AND (m.b IS 1)
	    AND (m.sel IS 1)
	    AND (m.out IS 1);
}
