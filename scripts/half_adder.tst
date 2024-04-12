LOAD half_adder;

TEST 'add 0 0' {
	VAR h: half_adder;
	EVAL;
	REQUIRE h.sum IS 0 AND h.carry IS 0;
}

TEST 'add 1 0' {
	VAR h: half_adder;
	SET h.a = 1;
	EVAL;
	REQUIRE h.sum IS 1 AND h.carry IS 0;
}

TEST 'add 0 1' {
	VAR h: half_adder;
	SET h.b = 1;
	EVAL;
	REQUIRE h.sum IS 1 AND h.carry IS 0;
}

TEST 'add 1 1' {
	VAR h: half_adder;
	SET h.a = 1;
	SET h.b = 1;
	EVAL;
	REQUIRE h.sum IS 0 AND h.carry IS 1;
}
