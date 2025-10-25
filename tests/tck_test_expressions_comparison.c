// Generated TCK tests for language area: expressions.comparison
// Total scenarios: 33

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

void test_expressions_comparison_Comparison1_01(void) {
    // Parse/validate test for: [1] Number-typed integer comparison
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-01
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-01");

}

void test_expressions_comparison_Comparison1_02(void) {
    // Parse/validate test for: [2] Number-typed float comparison
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-02
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-02");

}

void test_expressions_comparison_Comparison1_03(void) {
    // Parse/validate test for: [3] Any-typed string comparison
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-03
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-03");

}

void test_expressions_comparison_Comparison1_04(void) {
    // Parse/validate test for: [4] Comparing nodes to nodes
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-04
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-04");

}

void test_expressions_comparison_Comparison1_05(void) {
    // Parse/validate test for: [5] Comparing relationships to relationships
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-05
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-05");

}

void test_expressions_comparison_Comparison1_06(void) {
    // Parse/validate test for: [6] Comparing lists to lists
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-06
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-06");

}

void test_expressions_comparison_Comparison1_07(void) {
    // Parse/validate test for: [7] Comparing maps to maps
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-07
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-07");

}

void test_expressions_comparison_Comparison1_08(void) {
    // Parse/validate test for: [8] Equality and inequality of NaN
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-08
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-08");

}

void test_expressions_comparison_Comparison1_09(void) {
    // Parse/validate test for: [9] Equality between strings and numbers
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-09
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-09");

}

void test_expressions_comparison_Comparison1_10(void) {
    // Parse/validate test for: [10] Handling inlined equality of large integer
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-10
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-10");

}

void test_expressions_comparison_Comparison1_11(void) {
    // Parse/validate test for: [11] Handling explicit equality of large integer
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-11
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-11");

}

void test_expressions_comparison_Comparison1_12(void) {
    // Parse/validate test for: [12] Handling inlined equality of large integer, non-equal values
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-12
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-12");

}

void test_expressions_comparison_Comparison1_13(void) {
    // Parse/validate test for: [13] Handling explicit equality of large integer, non-equal values
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-13
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-13");

}

void test_expressions_comparison_Comparison1_14(void) {
    // Parse/validate test for: [14] Direction of traversed relationship is not significant for path equality, simple
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-14
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-14");

}

void test_expressions_comparison_Comparison1_15(void) {
    // Parse/validate test for: [15] It is unknown - i.e. null - if a null is equal to a null
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-15
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-15");

}

void test_expressions_comparison_Comparison1_16(void) {
    // Parse/validate test for: [16] It is unknown - i.e. null - if a null is not equal to a null
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-16
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-16");

}

void test_expressions_comparison_Comparison1_17(void) {
    // Parse/validate test for: [17] Failing when comparing to an undefined variable
    // Feature: Comparison1 - Equality
    
    // TODO: Implement parsing/validation test for expressions-comparison-Comparison1-17
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison1-17");

}

void test_expressions_comparison_Comparison2_01(void) {
    // Runtime test for: [1] Comparing strings and integers using > in an AND'd predicate
    // Feature: Comparison2 - Half-bounded Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison2-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison2-01");

}

void test_expressions_comparison_Comparison2_02(void) {
    // Runtime test for: [2] Comparing strings and integers using > in a OR'd predicate
    // Feature: Comparison2 - Half-bounded Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison2-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison2-02");

}

void test_expressions_comparison_Comparison2_03(void) {
    // Runtime test for: [3] Comparing across types yields null, except numbers
    // Feature: Comparison2 - Half-bounded Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison2-03
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison2-03");

}

void test_expressions_comparison_Comparison2_04(void) {
    // Runtime test for: [4] Comparing lists
    // Feature: Comparison2 - Half-bounded Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison2-04
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison2-04");

}

void test_expressions_comparison_Comparison2_05(void) {
    // Runtime test for: [5] Comparing NaN
    // Feature: Comparison2 - Half-bounded Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison2-05
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison2-05");

}

void test_expressions_comparison_Comparison2_06(void) {
    // Runtime test for: [6] Comparability between numbers and strings
    // Feature: Comparison2 - Half-bounded Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison2-06
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison2-06");

}

void test_expressions_comparison_Comparison3_01(void) {
    // Runtime test for: [1] Handling numerical ranges 1
    // Feature: Comparison3 - Full-Bound Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison3-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison3-01");

}

void test_expressions_comparison_Comparison3_02(void) {
    // Runtime test for: [2] Handling numerical ranges 2
    // Feature: Comparison3 - Full-Bound Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison3-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison3-02");

}

void test_expressions_comparison_Comparison3_03(void) {
    // Runtime test for: [3] Handling numerical ranges 3
    // Feature: Comparison3 - Full-Bound Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison3-03
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison3-03");

}

void test_expressions_comparison_Comparison3_04(void) {
    // Runtime test for: [4] Handling numerical ranges 4
    // Feature: Comparison3 - Full-Bound Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison3-04
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison3-04");

}

void test_expressions_comparison_Comparison3_05(void) {
    // Runtime test for: [5] Handling string ranges 1
    // Feature: Comparison3 - Full-Bound Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison3-05
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison3-05");

}

void test_expressions_comparison_Comparison3_06(void) {
    // Runtime test for: [6] Handling string ranges 2
    // Feature: Comparison3 - Full-Bound Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison3-06
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison3-06");

}

void test_expressions_comparison_Comparison3_07(void) {
    // Runtime test for: [7] Handling string ranges 3
    // Feature: Comparison3 - Full-Bound Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison3-07
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison3-07");

}

void test_expressions_comparison_Comparison3_08(void) {
    // Runtime test for: [8] Handling string ranges 4
    // Feature: Comparison3 - Full-Bound Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison3-08
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison3-08");

}

void test_expressions_comparison_Comparison3_09(void) {
    // Runtime test for: [9] Handling empty range
    // Feature: Comparison3 - Full-Bound Range
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison3-09
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison3-09");

}

void test_expressions_comparison_Comparison4_01(void) {
    // Runtime test for: [1] Handling long chains of operators
    // Feature: Comparison4 - Combination of Comparisons
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-comparison-Comparison4-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-comparison-Comparison4-01");

}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_expressions_comparison_Comparison1_01);
    RUN_TEST(test_expressions_comparison_Comparison1_02);
    RUN_TEST(test_expressions_comparison_Comparison1_03);
    RUN_TEST(test_expressions_comparison_Comparison1_04);
    RUN_TEST(test_expressions_comparison_Comparison1_05);
    RUN_TEST(test_expressions_comparison_Comparison1_06);
    RUN_TEST(test_expressions_comparison_Comparison1_07);
    RUN_TEST(test_expressions_comparison_Comparison1_08);
    RUN_TEST(test_expressions_comparison_Comparison1_09);
    RUN_TEST(test_expressions_comparison_Comparison1_10);
    RUN_TEST(test_expressions_comparison_Comparison1_11);
    RUN_TEST(test_expressions_comparison_Comparison1_12);
    RUN_TEST(test_expressions_comparison_Comparison1_13);
    RUN_TEST(test_expressions_comparison_Comparison1_14);
    RUN_TEST(test_expressions_comparison_Comparison1_15);
    RUN_TEST(test_expressions_comparison_Comparison1_16);
    RUN_TEST(test_expressions_comparison_Comparison1_17);
    RUN_TEST(test_expressions_comparison_Comparison2_01);
    RUN_TEST(test_expressions_comparison_Comparison2_02);
    RUN_TEST(test_expressions_comparison_Comparison2_03);
    RUN_TEST(test_expressions_comparison_Comparison2_04);
    RUN_TEST(test_expressions_comparison_Comparison2_05);
    RUN_TEST(test_expressions_comparison_Comparison2_06);
    RUN_TEST(test_expressions_comparison_Comparison3_01);
    RUN_TEST(test_expressions_comparison_Comparison3_02);
    RUN_TEST(test_expressions_comparison_Comparison3_03);
    RUN_TEST(test_expressions_comparison_Comparison3_04);
    RUN_TEST(test_expressions_comparison_Comparison3_05);
    RUN_TEST(test_expressions_comparison_Comparison3_06);
    RUN_TEST(test_expressions_comparison_Comparison3_07);
    RUN_TEST(test_expressions_comparison_Comparison3_08);
    RUN_TEST(test_expressions_comparison_Comparison3_09);
    RUN_TEST(test_expressions_comparison_Comparison4_01);
    
    return UNITY_END();
}
