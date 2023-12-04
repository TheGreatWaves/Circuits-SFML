LOAD and;

TEST 'Testing and for 1' {
    VAR and0: and;
    SET and0.a = 1;
    SET and0.b = 1;
    EVAL;
    REQUIRE (and0.out IS 1);
}

TEST 'Testing and for all' {
    VAR and0: and;
    VAR and1: and;
    VAR and2: and;
    VAR and3: and;

    SET and0.a = 1;
    SET and0.b = 1;

    SET and1.a = 1;
    SET and1.b = 0;

    SET and2.a = 0;
    SET and2.b = 1;

    SET and3.a = 0;
    SET and3.b = 0;
    
    EVAL;
    REQUIRE (and0.out IS 1) AND (and1.out IS 0) AND (and2.out IS 0) AND (and3.out IS 0);
}