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

TEST 'not16 in[0] = 1' {
	VAR n: not16;
	SET n.in[2] = 1;
	EVAL;
	REQUIRE (n.in IS 4) 
	    AND (n.in[2] IS 1)
	    AND (n.out IS 65531);
}
