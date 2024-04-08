LOAD not16;

TEST 'not16 0' {
	VAR n: not16;
	EVAL;
	REQUIRE (n.in IS 0)
	    AND (n.out IS 65535);
}

TEST 'not16 65535' {
	VAR n: not16;
	SET n.in = 65535;
	EVAL;
	REQUIRE (n.in IS 65535)
	    AND (n.out IS 0);
}

TEST 'not16 43690' {
	VAR n: not16;
	SET n.in = 43690;
	EVAL;
	REQUIRE (n.in IS 43690)
	    AND (n.out IS 21845);
}
