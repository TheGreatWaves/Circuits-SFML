LOAD or_8_way;

TEST 'or_8_way 0' {
	VAR o: or_8_way;
	EVAL;
	REQUIRE o.out IS 0;
}

TEST 'or_8_way index 0' {
	VAR o: or_8_way;
	SET o.in[0] = 1;
	EVAL;
	REQUIRE o.out IS 1;
}

TEST 'or_8_way index 1' {
	VAR o: or_8_way;
	SET o.in[1] = 1;
	EVAL;
	REQUIRE o.out IS 1;
}

TEST 'or_8_way index 2' {
	VAR o: or_8_way;
	SET o.in[2] = 1;
	EVAL;
	REQUIRE o.out IS 1;
}

TEST 'or_8_way index 3' {
	VAR o: or_8_way;
	SET o.in[3] = 1;
	EVAL;
	REQUIRE o.out IS 1;
}

TEST 'or_8_way index 4' {
	VAR o: or_8_way;
	SET o.in[4] = 1;
	EVAL;
	REQUIRE o.out IS 1;
}

TEST 'or_8_way index 5' {
	VAR o: or_8_way;
	SET o.in[5] = 1;
	EVAL;
	REQUIRE o.out IS 1;
}

TEST 'or_8_way index 6' {
	VAR o: or_8_way;
	SET o.in[6] = 1;
	EVAL;
	REQUIRE o.out IS 1;
}

TEST 'or_8_way index 7' {
	VAR o: or_8_way;
	SET o.in[7] = 1;
	EVAL;
	REQUIRE o.out IS 1;
}
