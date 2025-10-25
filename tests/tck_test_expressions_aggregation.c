// Generated TCK tests for language area: expressions.aggregation
// Total scenarios: 27

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

void test_expressions_aggregation_Aggregation1_01(void) {
    // Runtime test for: [1] Count only non-null values
    // Feature: Aggregation1 - Count
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation1-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation1-01");

}

void test_expressions_aggregation_Aggregation1_02(void) {
    // Runtime test for: [2] Counting loop relationships
    // Feature: Aggregation1 - Count
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation1-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation1-02");

}

void test_expressions_aggregation_Aggregation2_01(void) {
    // Runtime test for: [1] `max()` over integers
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-01");

}

void test_expressions_aggregation_Aggregation2_02(void) {
    // Runtime test for: [2] `min()` over integers
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-02");

}

void test_expressions_aggregation_Aggregation2_03(void) {
    // Runtime test for: [3] `max()` over floats
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-03
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-03");

}

void test_expressions_aggregation_Aggregation2_04(void) {
    // Runtime test for: [4] `min()` over floats
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-04
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-04");

}

void test_expressions_aggregation_Aggregation2_05(void) {
    // Runtime test for: [5] `max()` over mixed numeric values
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-05
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-05");

}

void test_expressions_aggregation_Aggregation2_06(void) {
    // Runtime test for: [6] `min()` over mixed numeric values
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-06
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-06");

}

void test_expressions_aggregation_Aggregation2_07(void) {
    // Runtime test for: [7] `max()` over strings
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-07
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-07");

}

void test_expressions_aggregation_Aggregation2_08(void) {
    // Runtime test for: [8] `min()` over strings
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-08
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-08");

}

void test_expressions_aggregation_Aggregation2_09(void) {
    // Runtime test for: [9] `max()` over list values
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-09
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-09");

}

void test_expressions_aggregation_Aggregation2_10(void) {
    // Runtime test for: [10] `min()` over list values
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-10
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-10");

}

void test_expressions_aggregation_Aggregation2_11(void) {
    // Runtime test for: [11] `max()` over mixed values
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-11
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-11");

}

void test_expressions_aggregation_Aggregation2_12(void) {
    // Runtime test for: [12] `min()` over mixed values
    // Feature: Aggregation2 - Min and Max
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation2-12
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation2-12");

}

void test_expressions_aggregation_Aggregation3_01(void) {
    // Runtime test for: [1] Sum only non-null values
    // Feature: Aggregation3 - Sum
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation3-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation3-01");

}

void test_expressions_aggregation_Aggregation3_02(void) {
    // Runtime test for: [2] No overflow during summation
    // Feature: Aggregation3 - Sum
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation3-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation3-02");

}

void test_expressions_aggregation_Aggregation5_01(void) {
    // Runtime test for: [1] `collect()` filtering nulls
    // Feature: Aggregation5 - Collect
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation5-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation5-01");

}

void test_expressions_aggregation_Aggregation5_02(void) {
    // Runtime test for: [2] OPTIONAL MATCH and `collect()` on node property
    // Feature: Aggregation5 - Collect
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation5-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation5-02");

}

void test_expressions_aggregation_Aggregation6_01(void) {
    // Runtime test for: [1] `percentileDisc()`
    // Feature: Aggregation6 - Percentiles
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation6-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation6-01");

}

void test_expressions_aggregation_Aggregation6_02(void) {
    // Runtime test for: [2] `percentileCont()`
    // Feature: Aggregation6 - Percentiles
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation6-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation6-02");

}

void test_expressions_aggregation_Aggregation6_03(void) {
    // Runtime test for: [3] `percentileCont()` failing on bad arguments
    // Feature: Aggregation6 - Percentiles
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation6-03
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation6-03");

}

void test_expressions_aggregation_Aggregation6_04(void) {
    // Runtime test for: [4] `percentileDisc()` failing on bad arguments
    // Feature: Aggregation6 - Percentiles
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation6-04
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation6-04");

}

void test_expressions_aggregation_Aggregation6_05(void) {
    // Runtime test for: [5] `percentileDisc()` failing in more involved query
    // Feature: Aggregation6 - Percentiles
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation6-05
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation6-05");

}

void test_expressions_aggregation_Aggregation8_01(void) {
    // Runtime test for: [1] Distinct on unbound node
    // Feature: Aggregation8 - DISTINCT
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation8-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation8-01");

}

void test_expressions_aggregation_Aggregation8_02(void) {
    // Runtime test for: [2] Distinct on null
    // Feature: Aggregation8 - DISTINCT
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation8-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation8-02");

}

void test_expressions_aggregation_Aggregation8_03(void) {
    // Runtime test for: [3] Collect distinct nulls
    // Feature: Aggregation8 - DISTINCT
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation8-03
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation8-03");

}

void test_expressions_aggregation_Aggregation8_04(void) {
    // Runtime test for: [4] Collect distinct values mixed with nulls
    // Feature: Aggregation8 - DISTINCT
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for expressions-aggregation-Aggregation8-04
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: expressions-aggregation-Aggregation8-04");

}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_expressions_aggregation_Aggregation1_01);
    RUN_TEST(test_expressions_aggregation_Aggregation1_02);
    RUN_TEST(test_expressions_aggregation_Aggregation2_01);
    RUN_TEST(test_expressions_aggregation_Aggregation2_02);
    RUN_TEST(test_expressions_aggregation_Aggregation2_03);
    RUN_TEST(test_expressions_aggregation_Aggregation2_04);
    RUN_TEST(test_expressions_aggregation_Aggregation2_05);
    RUN_TEST(test_expressions_aggregation_Aggregation2_06);
    RUN_TEST(test_expressions_aggregation_Aggregation2_07);
    RUN_TEST(test_expressions_aggregation_Aggregation2_08);
    RUN_TEST(test_expressions_aggregation_Aggregation2_09);
    RUN_TEST(test_expressions_aggregation_Aggregation2_10);
    RUN_TEST(test_expressions_aggregation_Aggregation2_11);
    RUN_TEST(test_expressions_aggregation_Aggregation2_12);
    RUN_TEST(test_expressions_aggregation_Aggregation3_01);
    RUN_TEST(test_expressions_aggregation_Aggregation3_02);
    RUN_TEST(test_expressions_aggregation_Aggregation5_01);
    RUN_TEST(test_expressions_aggregation_Aggregation5_02);
    RUN_TEST(test_expressions_aggregation_Aggregation6_01);
    RUN_TEST(test_expressions_aggregation_Aggregation6_02);
    RUN_TEST(test_expressions_aggregation_Aggregation6_03);
    RUN_TEST(test_expressions_aggregation_Aggregation6_04);
    RUN_TEST(test_expressions_aggregation_Aggregation6_05);
    RUN_TEST(test_expressions_aggregation_Aggregation8_01);
    RUN_TEST(test_expressions_aggregation_Aggregation8_02);
    RUN_TEST(test_expressions_aggregation_Aggregation8_03);
    RUN_TEST(test_expressions_aggregation_Aggregation8_04);
    
    return UNITY_END();
}
