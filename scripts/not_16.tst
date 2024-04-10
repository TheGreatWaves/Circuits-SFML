LOAD not_16;

TEST 'not_16 0' {
 VAR n: not_16;
 EVAL;
 REQUIRE (n.in IS 0)
  	 AND (n.out IS 65535);
}

TEST 'not_16 65535' {
 VAR n: not_16;
 SET n.in = 65535;
 EVAL;
 REQUIRE (n.in IS 65535)
    AND (n.out IS 0);
}

TEST 'not_16 43690' {
 VAR n: not_16;
 SET n.in = 43690;
 EVAL;
 REQUIRE (n.in IS 43690)
     AND (n.out IS 21845);
}

TEST 'not_16 in[0] = 1' {
	VAR n: not_16;
	SET n.in[2] = 1;
	EVAL;
	REQUIRE (n.in IS 4) 
	    AND (n.in[2] IS 1)
	    AND (n.out IS 65531);
}
