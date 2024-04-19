LOAD nand;

TEST '0 0' {
	VAR n: nand;
	SET n.a = 0;
	SET n.b = 0;
	EVAL;
	REQUIRE n.out IS 1;
}

TEST '1 0' {
	VAR n: nand;
	SET n.a = 1;
	SET n.b = 0;
	EVAL;
	REQUIRE n.out IS 1;
}

TEST '0 1' {
	VAR n: nand;
	SET n.a = 0;
	SET n.b = 1;
	EVAL;
	REQUIRE n.out IS 1;
}

TEST '1 1' {
	VAR n: nand;
	SET n.a = 1;
	SET n.b = 1;
	EVAL;
	REQUIRE n.out IS 0;
}
