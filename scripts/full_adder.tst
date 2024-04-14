LOAD full_adder;

TEST '0 0 0' {
	VAR f: full_adder;
	EVAL;
	REQUIRE f.sum IS 0 AND f.carry IS 0;
}

TEST '0 0 1' {
	VAR f: full_adder;
	SET f.c = 1;
	EVAL;
	REQUIRE f.sum IS 1 AND f.carry IS 0;
}

TEST '0 1 0' {
	VAR f: full_adder;
	SET f.b = 1;
	EVAL;
	REQUIRE f.sum IS 1 AND f.carry IS 0;
}

TEST '0 1 1' {
	VAR f: full_adder;
	SET f.b = 1;
	SET f.c = 1;
	EVAL;
	REQUIRE f.sum IS 0 AND f.carry IS 1;
}

TEST '1 0 0' {
	VAR f: full_adder;
	SET f.a = 1;
	EVAL;
	REQUIRE f.sum IS 1 AND f.carry IS 0;
}

TEST '1 0 1' {
	VAR f: full_adder;
	SET f.a = 1;
	SET f.c = 1;
	EVAL;
	REQUIRE f.sum IS 0 AND f.carry IS 1;
}

TEST '1 1 0' {
	VAR f: full_adder;
	SET f.a = 1;
	SET f.b = 1;
	EVAL;
	REQUIRE f.sum IS 0 AND f.carry IS 1;
}

TEST '1 1 1' {
	VAR f: full_adder;
	SET f.a = 1;
	SET f.b = 1;
	SET f.c = 1;
	EVAL;
	REQUIRE f.sum IS 1 AND f.carry IS 1;
}
