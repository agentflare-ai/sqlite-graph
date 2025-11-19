// Generated TCK tests for language area: clauses.call
// Total scenarios: 41

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unity.h>

static sqlite3 *db = NULL;
static char *error_msg = NULL;

void setUp(void) {
    int rc = sqlite3_open(":memory:", &db);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Enable loading extensions
    sqlite3_enable_load_extension(db, 1);
    
    // Load graph extension
    #ifdef __APPLE__
    rc = sqlite3_load_extension(db, "../build/libgraph.dylib", "sqlite3_graph_init", &error_msg);
#else
    rc = sqlite3_load_extension(db, "../build/libgraph.so", "sqlite3_graph_init", &error_msg);
#endif
    if (rc != SQLITE_OK) {
        printf("Failed to load graph extension: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
}

void tearDown(void) {
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

void test_clauses_call_Call1_01(void) {
    // [1] Standalone call to procedure that takes no arguments and yields no results
    // Feature: Call1 - Basic procedure calling
    // Given: An empty graph and procedure test.doNothing() :: ()
    // Query: CALL test.doNothing()
    // Expected: Empty result, no side effects

    // CALL clause for stored procedures is not yet implemented
    // This test requires:
    // 1. CALL statement parsing
    // 2. Procedure registration/storage mechanism
    // 3. Procedure invocation during execution

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_02(void) {
    // [2] Standalone call to procedure that takes no arguments and yields no results, called with implicit arguments
    // Feature: Call1 - Basic procedure calling
    // Given: An empty graph and procedure test.doNothing() :: ()
    // Query: CALL test.doNothing (without parentheses)
    // Expected: Empty result, no side effects

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_03(void) {
    // [3] In-query call to procedure that takes no arguments and yields no results
    // Feature: Call1 - Basic procedure calling
    // Given: An empty graph and procedure test.doNothing() :: ()
    // Query: MATCH (n) CALL test.doNothing() RETURN n
    // Expected: Empty result (no nodes exist)

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_04(void) {
    // [4] In-query call to procedure that takes no arguments and yields no results and consumes no rows
    // Feature: Call1 - Basic procedure calling
    // Given: Nodes A, B, C and procedure test.doNothing() :: ()
    // Query: MATCH (n) CALL test.doNothing() RETURN n.name AS name
    // Expected: Result with 3 rows: 'a', 'b', 'c'

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_05(void) {
    // [5] Standalone call to STRING procedure that takes no arguments
    // Feature: Call1 - Basic procedure calling
    // Given: Procedure test.labels() :: (label :: STRING?) that yields 'A', 'B', 'C'
    // Query: CALL test.labels()
    // Expected: Result with 3 rows in order: label='A', 'B', 'C'

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_06(void) {
    // [6] In-query call to STRING procedure that takes no arguments
    // Feature: Call1 - Basic procedure calling
    // Given: Procedure test.labels() :: (label :: STRING?) that yields 'A', 'B', 'C'
    // Query: CALL test.labels() YIELD label RETURN label
    // Expected: Result with 3 rows in order: label='A', 'B', 'C'

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_07(void) {
    // [7] Standalone call to procedure should fail if explicit argument is missing
    // Feature: Call1 - Basic procedure calling
    // Given: Procedure test.my.proc(name :: STRING?, in :: INTEGER?) :: (out :: INTEGER?)
    // Query: CALL test.my.proc('Dobby')  -- missing second argument
    // Expected: SyntaxError: InvalidNumberOfArguments

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_08(void) {
    // [8] In-query call to procedure should fail if explicit argument is missing
    // Feature: Call1 - Basic procedure calling
    // Given: Procedure test.my.proc(name :: STRING?, in :: INTEGER?) :: (out :: INTEGER?)
    // Query: CALL test.my.proc('Dobby') YIELD out RETURN out  -- missing second argument
    // Expected: SyntaxError: InvalidNumberOfArguments

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_09(void) {
    // [9] Standalone call to procedure should fail if too many explicit argument are given
    // Feature: Call1 - Basic procedure calling
    // Given: Procedure test.my.proc(in :: INTEGER?) :: (out :: INTEGER?)
    // Query: CALL test.my.proc(1, 2, 3, 4)  -- expects 1 argument, given 4
    // Expected: SyntaxError: InvalidNumberOfArguments

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_10(void) {
    // [10] In-query call to procedure should fail if too many explicit argument are given
    // Feature: Call1 - Basic procedure calling
    // Given: Procedure test.my.proc(in :: INTEGER?) :: (out :: INTEGER?)
    // Query: CALL test.my.proc(1, 2, 3, 4) YIELD out RETURN out  -- expects 1 argument, given 4
    // Expected: SyntaxError: InvalidNumberOfArguments

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_11(void) {
    // [11] Standalone call to procedure should fail if implicit argument is missing
    // Feature: Call1 - Basic procedure calling
    // Given: Procedure test.my.proc(name :: STRING?, in :: INTEGER?) :: (out :: INTEGER?)
    //        Parameters: name = 'Stefan'
    // Query: CALL test.my.proc  -- called without parentheses, expects params to provide both args
    // Expected: ParameterMissing: MissingParameter (parameter 'in' is missing)

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_12(void) {
    // [12] In-query call to procedure that has outputs fails if no outputs are yielded
    // Feature: Call1 - Basic procedure calling
    // Given: Procedure test.my.proc(in :: INTEGER?) :: (out :: INTEGER?)
    // Query: CALL test.my.proc(1) RETURN out  -- missing YIELD clause
    // Expected: SyntaxError: UndefinedVariable (out is not defined without YIELD)

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_13(void) {
    // [13] Standalone call to unknown procedure should fail
    // Feature: Call1 - Basic procedure calling
    // Given: An empty graph (no procedures defined)
    // Query: CALL test.my.proc
    // Expected: ProcedureError: ProcedureNotFound

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_14(void) {
    // [14] In-query call to unknown procedure should fail
    // Feature: Call1 - Basic procedure calling
    // Given: An empty graph (no procedures defined)
    // Query: CALL test.my.proc() YIELD out RETURN out
    // Expected: ProcedureError: ProcedureNotFound

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_15(void) {
    // [15] In-query procedure call should fail if shadowing an already bound variable
    // Feature: Call1 - Basic procedure calling
    // Given: Procedure test.labels() :: (label :: STRING?) that yields 'A', 'B', 'C'
    // Query: WITH 'Hi' AS label CALL test.labels() YIELD label RETURN *
    // Expected: SyntaxError: VariableAlreadyBound (label is already bound from WITH clause)

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call1_16(void) {
    // [16] In-query procedure call should fail if one of the argument expressions uses an aggregation function
    // Feature: Call1 - Basic procedure calling
    // Given: Procedure test.labels(in :: INTEGER?) :: (label :: STRING?)
    // Query: MATCH (n) CALL test.labels(count(n)) YIELD label RETURN label
    // Expected: SyntaxError: InvalidAggregation (cannot use aggregation in CALL arguments without grouping)

    TEST_IGNORE_MESSAGE("CALL clause and stored procedures not yet implemented");
}

void test_clauses_call_Call2_01(void) {
    // Parse/validate test for: [1] In-query call to procedure with explicit arguments
    // Feature: Call2 - Procedure arguments
    
    // TODO: Implement parsing/validation test for clauses-call-Call2-01
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call2-01");

}

void test_clauses_call_Call2_02(void) {
    // Parse/validate test for: [2] Standalone call to procedure with explicit arguments
    // Feature: Call2 - Procedure arguments
    
    // TODO: Implement parsing/validation test for clauses-call-Call2-02
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call2-02");

}

void test_clauses_call_Call2_03(void) {
    // Parse/validate test for: [3] Standalone call to procedure with implicit arguments
    // Feature: Call2 - Procedure arguments
    
    // TODO: Implement parsing/validation test for clauses-call-Call2-03
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call2-03");

}

void test_clauses_call_Call2_04(void) {
    // Parse/validate test for: [4] In-query call to procedure that takes arguments fails when trying to pass them implicitly
    // Feature: Call2 - Procedure arguments
    
    // TODO: Implement parsing/validation test for clauses-call-Call2-04
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call2-04");

}

void test_clauses_call_Call2_05(void) {
    // Parse/validate test for: [5] Standalone call to procedure should fail if input type is wrong
    // Feature: Call2 - Procedure arguments
    
    // TODO: Implement parsing/validation test for clauses-call-Call2-05
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call2-05");

}

void test_clauses_call_Call2_06(void) {
    // Parse/validate test for: [6] In-query call to procedure should fail if input type is wrong
    // Feature: Call2 - Procedure arguments
    
    // TODO: Implement parsing/validation test for clauses-call-Call2-06
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call2-06");

}

void test_clauses_call_Call3_01(void) {
    // Runtime test for: [1] Standalone call to procedure with argument of type NUMBER accepts value of type INTEGER
    // Feature: Call3 - Assignable-type arguments
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call3-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call3-01");

}

void test_clauses_call_Call3_02(void) {
    // Runtime test for: [2] In-query call to procedure with argument of type NUMBER accepts value of type INTEGER
    // Feature: Call3 - Assignable-type arguments
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call3-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call3-02");

}

void test_clauses_call_Call3_03(void) {
    // Runtime test for: [3] Standalone call to procedure with argument of type NUMBER accepts value of type FLOAT
    // Feature: Call3 - Assignable-type arguments
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call3-03
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call3-03");

}

void test_clauses_call_Call3_04(void) {
    // Runtime test for: [4] In-query call to procedure with argument of type NUMBER accepts value of type FLOAT
    // Feature: Call3 - Assignable-type arguments
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call3-04
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call3-04");

}

void test_clauses_call_Call3_05(void) {
    // Runtime test for: [5] Standalone call to procedure with argument of type FLOAT accepts value of type INTEGER
    // Feature: Call3 - Assignable-type arguments
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call3-05
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call3-05");

}

void test_clauses_call_Call3_06(void) {
    // Runtime test for: [6] In-query call to procedure with argument of type FLOAT accepts value of type INTEGER
    // Feature: Call3 - Assignable-type arguments
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call3-06
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call3-06");

}

void test_clauses_call_Call4_01(void) {
    // Runtime test for: [1] Standalone call to procedure with null argument
    // Feature: Call4 - Null Arguments
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call4-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call4-01");

}

void test_clauses_call_Call4_02(void) {
    // Runtime test for: [2] In-query call to procedure with null argument
    // Feature: Call4 - Null Arguments
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call4-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call4-02");

}

void test_clauses_call_Call5_01(void) {
    // Parse/validate test for: [1] Explicit procedure result projection
    // Feature: Call5 - Results projection
    
    // TODO: Implement parsing/validation test for clauses-call-Call5-01
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call5-01");

}

void test_clauses_call_Call5_02(void) {
    // Parse/validate test for: [2] Explicit procedure result projection with RETURN *
    // Feature: Call5 - Results projection
    
    // TODO: Implement parsing/validation test for clauses-call-Call5-02
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call5-02");

}

void test_clauses_call_Call5_03(void) {
    // Parse/validate test for: [3] The order of yield items is irrelevant
    // Feature: Call5 - Results projection
    
    // TODO: Implement parsing/validation test for clauses-call-Call5-03
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call5-03");

}

void test_clauses_call_Call5_04(void) {
    // Parse/validate test for: [4] Rename outputs to unbound variable names
    // Feature: Call5 - Results projection
    
    // TODO: Implement parsing/validation test for clauses-call-Call5-04
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call5-04");

}

void test_clauses_call_Call5_05(void) {
    // Parse/validate test for: [5] Fail on renaming to an already bound variable name
    // Feature: Call5 - Results projection
    
    // TODO: Implement parsing/validation test for clauses-call-Call5-05
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call5-05");

}

void test_clauses_call_Call5_06(void) {
    // Parse/validate test for: [6] Fail on renaming all outputs to the same variable name
    // Feature: Call5 - Results projection
    
    // TODO: Implement parsing/validation test for clauses-call-Call5-06
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call5-06");

}

void test_clauses_call_Call5_07(void) {
    // Parse/validate test for: [7] Fail on in-query call to procedure with YIELD *
    // Feature: Call5 - Results projection
    
    // TODO: Implement parsing/validation test for clauses-call-Call5-07
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call5-07");

}

void test_clauses_call_Call5_08(void) {
    // Parse/validate test for: [8] Allow standalone call to procedure with YIELD *
    // Feature: Call5 - Results projection
    
    // TODO: Implement parsing/validation test for clauses-call-Call5-08
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call5-08");

}

void test_clauses_call_Call6_01(void) {
    // Runtime test for: [1] Calling the same STRING procedure twice using the same outputs in each call
    // Feature: Call6 - Call clause interoperation with other clauses
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call6-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call6-01");

}

void test_clauses_call_Call6_02(void) {
    // Runtime test for: [2] Project procedure results between query scopes with WITH clause
    // Feature: Call6 - Call clause interoperation with other clauses
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call6-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call6-02");

}

void test_clauses_call_Call6_03(void) {
    // Runtime test for: [3] Project procedure results between query scopes with WITH clause and rename the projection
    // Feature: Call6 - Call clause interoperation with other clauses
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-call-Call6-03
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call6-03");

}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_clauses_call_Call1_01);
    RUN_TEST(test_clauses_call_Call1_02);
    RUN_TEST(test_clauses_call_Call1_03);
    RUN_TEST(test_clauses_call_Call1_04);
    RUN_TEST(test_clauses_call_Call1_05);
    RUN_TEST(test_clauses_call_Call1_06);
    RUN_TEST(test_clauses_call_Call1_07);
    RUN_TEST(test_clauses_call_Call1_08);
    RUN_TEST(test_clauses_call_Call1_09);
    RUN_TEST(test_clauses_call_Call1_10);
    RUN_TEST(test_clauses_call_Call1_11);
    RUN_TEST(test_clauses_call_Call1_12);
    RUN_TEST(test_clauses_call_Call1_13);
    RUN_TEST(test_clauses_call_Call1_14);
    RUN_TEST(test_clauses_call_Call1_15);
    RUN_TEST(test_clauses_call_Call1_16);
    RUN_TEST(test_clauses_call_Call2_01);
    RUN_TEST(test_clauses_call_Call2_02);
    RUN_TEST(test_clauses_call_Call2_03);
    RUN_TEST(test_clauses_call_Call2_04);
    RUN_TEST(test_clauses_call_Call2_05);
    RUN_TEST(test_clauses_call_Call2_06);
    RUN_TEST(test_clauses_call_Call3_01);
    RUN_TEST(test_clauses_call_Call3_02);
    RUN_TEST(test_clauses_call_Call3_03);
    RUN_TEST(test_clauses_call_Call3_04);
    RUN_TEST(test_clauses_call_Call3_05);
    RUN_TEST(test_clauses_call_Call3_06);
    RUN_TEST(test_clauses_call_Call4_01);
    RUN_TEST(test_clauses_call_Call4_02);
    RUN_TEST(test_clauses_call_Call5_01);
    RUN_TEST(test_clauses_call_Call5_02);
    RUN_TEST(test_clauses_call_Call5_03);
    RUN_TEST(test_clauses_call_Call5_04);
    RUN_TEST(test_clauses_call_Call5_05);
    RUN_TEST(test_clauses_call_Call5_06);
    RUN_TEST(test_clauses_call_Call5_07);
    RUN_TEST(test_clauses_call_Call5_08);
    RUN_TEST(test_clauses_call_Call6_01);
    RUN_TEST(test_clauses_call_Call6_02);
    RUN_TEST(test_clauses_call_Call6_03);
    
    return UNITY_END();
}
