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
    rc = sqlite3_load_extension(db, "../build/libgraph.so", "sqlite3_graph_init", &error_msg);
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
    // Parse/validate test for: [1] Standalone call to procedure that takes no arguments and yields no results
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-01
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-01");

}

void test_clauses_call_Call1_02(void) {
    // Parse/validate test for: [2] Standalone call to procedure that takes no arguments and yields no results, called with implicit arguments
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-02
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-02");

}

void test_clauses_call_Call1_03(void) {
    // Parse/validate test for: [3] In-query call to procedure that takes no arguments and yields no results
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-03
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-03");

}

void test_clauses_call_Call1_04(void) {
    // Parse/validate test for: [4] In-query call to procedure that takes no arguments and yields no results and consumes no rows
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-04
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-04");

}

void test_clauses_call_Call1_05(void) {
    // Parse/validate test for: [5] Standalone call to STRING procedure that takes no arguments
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-05
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-05");

}

void test_clauses_call_Call1_06(void) {
    // Parse/validate test for: [6] In-query call to STRING procedure that takes no arguments
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-06
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-06");

}

void test_clauses_call_Call1_07(void) {
    // Parse/validate test for: [7] Standalone call to procedure should fail if explicit argument is missing
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-07
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-07");

}

void test_clauses_call_Call1_08(void) {
    // Parse/validate test for: [8] In-query call to procedure should fail if explicit argument is missing
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-08
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-08");

}

void test_clauses_call_Call1_09(void) {
    // Parse/validate test for: [9] Standalone call to procedure should fail if too many explicit argument are given
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-09
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-09");

}

void test_clauses_call_Call1_10(void) {
    // Parse/validate test for: [10] In-query call to procedure should fail if too many explicit argument are given
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-10
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-10");

}

void test_clauses_call_Call1_11(void) {
    // Parse/validate test for: [11] Standalone call to procedure should fail if implicit argument is missing
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-11
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-11");

}

void test_clauses_call_Call1_12(void) {
    // Parse/validate test for: [12] In-query call to procedure that has outputs fails if no outputs are yielded
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-12
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-12");

}

void test_clauses_call_Call1_13(void) {
    // Parse/validate test for: [13] Standalone call to unknown procedure should fail
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-13
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-13");

}

void test_clauses_call_Call1_14(void) {
    // Parse/validate test for: [14] In-query call to unknown procedure should fail
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-14
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-14");

}

void test_clauses_call_Call1_15(void) {
    // Parse/validate test for: [15] In-query procedure call should fail if shadowing an already bound variable
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-15
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-15");

}

void test_clauses_call_Call1_16(void) {
    // Parse/validate test for: [16] In-query procedure call should fail if one of the argument expressions uses an aggregation function
    // Feature: Call1 - Basic procedure calling
    
    // TODO: Implement parsing/validation test for clauses-call-Call1-16
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-call-Call1-16");

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
