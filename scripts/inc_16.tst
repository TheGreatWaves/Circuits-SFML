LOAD inc_16;

TEST 'increments correctly' {
	VAR i: inc_16;

	SET i.in = 1;
	EVAL;
	REQUIRE i.out IS 2;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 3;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 4;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 5;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 6;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 7;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 8;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 9;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 10;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 11;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 12;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 13;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 14;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 15;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 16;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 17;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 18;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 19;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 20;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 21;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 22;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 23;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 24;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 25;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 26;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 27;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 28;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 29;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 30;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 31;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 32;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 33;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 34;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 35;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 36;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 37;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 38;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 39;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 40;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 41;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 42;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 43;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 44;

	SET i.in = i.out;
	EVAL;
	REQUIRE i.out IS 45;
}
