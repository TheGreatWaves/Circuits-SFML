LOAD not;

TEST 'not 0' {
	VAR n: not;
	SET n.in = 0;
	EVAL;
	REQUIRE n.in IS 0 AND n.out IS 1;
}

TEST 'not 1' {
	VAR n: not;
	SET n.in = 1;
	EVAL;
	REQUIRE n.in IS 1 AND n.out IS 0;
}
