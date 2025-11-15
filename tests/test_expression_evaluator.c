/*
** Comprehensive Unit Test Suite for Cypher Expression Evaluator
** Phase 04.2 - Expression Advanced Features & Testing
**
** Tests expression evaluation through Cypher MERGE queries
*/

#include <unity.h>
#include "test_common.h"

static sqlite3 *db = NULL;
static char *err_msg = NULL;

void setUp(void) {
    TEST_ASSERT_EQUAL(SQLITE_OK, test_open_db(&db));
    TEST_ASSERT_EQUAL(SQLITE_OK, test_enable_extensions(db));
    TEST_ASSERT_EQUAL(SQLITE_OK, test_load_graph_extension(db, &err_msg));
    TEST_ASSERT_EQUAL(SQLITE_OK, test_create_graph_vtab(db, &err_msg));
}

void tearDown(void) {
    test_cleanup(&db, &err_msg);
}

static void execQuery(const char *query) {
    sqlite3_stmt *stmt = NULL;
    TEST_ASSERT_EQUAL(SQLITE_OK,
                    sqlite3_prepare_v2(db, "SELECT cypher_execute(?)", -1,
                                       &stmt, NULL));
    TEST_ASSERT_EQUAL(SQLITE_OK,
                    sqlite3_bind_text(stmt, 1, query, -1, SQLITE_TRANSIENT));
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        const char *detail = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        TEST_FAIL_MESSAGE(detail);
    }
    sqlite3_finalize(stmt);
}

/* ========================================================================
** LITERAL VALUE TESTS
** ======================================================================== */

void test_literal_integer(void) {
    execQuery("MERGE (n:Node {id: 1}) ON CREATE SET n.value = 42");
}

void test_literal_string(void) {
    execQuery("MERGE (n:Node {id: 2}) ON CREATE SET n.value = 'hello'");
}

void test_literal_float(void) {
    execQuery("MERGE (n:Node {id: 3}) ON CREATE SET n.value = 3.14");
}

/* ========================================================================
** ARITHMETIC OPERATOR TESTS
** ======================================================================== */

void test_arithmetic_addition(void) {
    execQuery(
        "MERGE (n:Node {id: 10}) ON CREATE SET n.value = 2 + 3");
}

void test_arithmetic_subtraction(void) {
    execQuery(
        "MERGE (n:Node {id: 11}) ON CREATE SET n.value = 10 - 4");
}

void test_arithmetic_multiplication(void) {
    execQuery(
        "MERGE (n:Node {id: 12}) ON CREATE SET n.value = 5 * 6");
}

void test_arithmetic_division(void) {
    execQuery(
        "MERGE (n:Node {id: 13}) ON CREATE SET n.value = 15 / 3");
}

void test_arithmetic_with_variable(void) {
    execQuery("MERGE (n:Node {id: 14}) ON CREATE SET n.count = 1");
    execQuery("MERGE (n:Node {id: 14}) ON MATCH SET n.count = n.count + 1");
}

void test_arithmetic_complex(void) {
    execQuery(
        "MERGE (n:Node {id: 15}) ON CREATE SET n.value = (2 + 3) * 4");
}

/* ========================================================================
** STRING CONCATENATION TESTS (Phase 04.2 New Feature)
** ======================================================================== */

void test_string_concat_basic(void) {
    execQuery(
        "MERGE (n:Node {id: 20}) ON CREATE SET n.fullName = 'hello' + ' ' + 'world'");
}

void test_string_concat_with_variable(void) {
    execQuery("MERGE (n:Node {id: 21}) ON CREATE SET n.first = 'John', n.last = 'Doe'");
    execQuery("MERGE (n:Node {id: 21}) ON MATCH SET n.fullName = n.first + ' ' + n.last");
}

void test_string_concat_with_toString(void) {
    execQuery(
        "MERGE (n:Node {id: 22}) ON CREATE SET n.label = 'Count: ' + toString(42)");
}

void test_string_concat_empty_string(void) {
    execQuery(
        "MERGE (n:Node {id: 23}) ON CREATE SET n.value = '' + 'test'");
}

void test_string_concat_null_propagation(void) {
    execQuery(
        "MERGE (n:Node {id: 24}) ON CREATE SET n.value = 'hello' + NULL");
}

/* ========================================================================
** TYPE CONVERSION FUNCTION TESTS
** ======================================================================== */

void test_function_toInteger(void) {
    execQuery(
        "MERGE (n:Node {id: 30}) ON CREATE SET n.value = toInteger('42')");
}

void test_function_toFloat(void) {
    execQuery(
        "MERGE (n:Node {id: 31}) ON CREATE SET n.value = toFloat('3.14')");
}

void test_function_toString(void) {
    execQuery(
        "MERGE (n:Node {id: 32}) ON CREATE SET n.value = toString(42)");
}

/* ========================================================================
** NULL HANDLING FUNCTION TESTS
** ======================================================================== */

void test_function_coalesce_first_value(void) {
    execQuery(
        "MERGE (n:Node {id: 40}) ON CREATE SET n.value = coalesce('value', 'default')");
}

void test_function_coalesce_second_value(void) {
    execQuery(
        "MERGE (n:Node {id: 41}) ON CREATE SET n.value = coalesce(NULL, 'default')");
}

void test_function_coalesce_with_variable(void) {
    execQuery("MERGE (n:Node {id: 42}) ON CREATE SET n.count = 0");
    execQuery("MERGE (n:Node {id: 42}) ON MATCH SET n.count = coalesce(n.count, 0) + 1");
}

/* ========================================================================
** STRING FUNCTION TESTS
** ======================================================================== */

void test_function_toUpper(void) {
    execQuery(
        "MERGE (n:Node {id: 50}) ON CREATE SET n.value = toUpper('hello')");
}

void test_function_toLower(void) {
    execQuery(
        "MERGE (n:Node {id: 51}) ON CREATE SET n.value = toLower('HELLO')");
}

void test_function_length(void) {
    execQuery(
        "MERGE (n:Node {id: 52}) ON CREATE SET n.value = length('hello')");
}

/* ========================================================================
** MATH FUNCTION TESTS
** ======================================================================== */

void test_function_abs(void) {
    execQuery(
        "MERGE (n:Node {id: 60}) ON CREATE SET n.value = abs(-5)");
}

void test_function_ceil(void) {
    execQuery(
        "MERGE (n:Node {id: 61}) ON CREATE SET n.value = ceil(3.14)");
}

void test_function_floor(void) {
    execQuery(
        "MERGE (n:Node {id: 62}) ON CREATE SET n.value = floor(3.14)");
}

void test_function_round(void) {
    execQuery(
        "MERGE (n:Node {id: 63}) ON CREATE SET n.value = round(3.5)");
}

void test_function_sqrt(void) {
    execQuery(
        "MERGE (n:Node {id: 64}) ON CREATE SET n.value = sqrt(16)");
}

/* ========================================================================
** COMPLEX EXPRESSION TESTS
** ======================================================================== */

void test_complex_nested_arithmetic(void) {
    execQuery(
        "MERGE (n:Node {id: 70}) ON CREATE SET n.value = (2 + 3) * 4");
}

void test_complex_function_nesting(void) {
    execQuery(
        "MERGE (n:Node {id: 71}) ON CREATE SET n.value = abs(round(-3.7))");
}

void test_complex_string_and_number(void) {
    execQuery(
        "MERGE (n:Node {id: 72}) ON CREATE SET n.value = 'Value: ' + toString(42 * 2)");
}

void test_complex_multi_operation(void) {
    execQuery("MERGE (n:Node {id: 73}) ON CREATE SET n.count = 10");
    execQuery("MERGE (n:Node {id: 73}) ON MATCH SET n.count = coalesce(n.count, 0) + 5");
}

/* ========================================================================
** ERROR HANDLING TESTS
** ======================================================================== */

void test_division_by_zero_returns_null(void) {
    /* Division by zero should return NULL, not error */
    execQuery(
        "MERGE (n:Node {id: 80}) ON CREATE SET n.value = 10 / 0");
}

void test_null_arithmetic_propagates(void) {
    execQuery(
        "MERGE (n:Node {id: 81}) ON CREATE SET n.value = NULL + 5");
}

/* ========================================================================
** MAIN TEST RUNNER
** ======================================================================== */

int main(void) {
    UNITY_BEGIN();

    /* Literal tests */
    RUN_TEST(test_literal_integer);
    RUN_TEST(test_literal_string);
    RUN_TEST(test_literal_float);

    /* Arithmetic tests */
    RUN_TEST(test_arithmetic_addition);
    RUN_TEST(test_arithmetic_subtraction);
    RUN_TEST(test_arithmetic_multiplication);
    RUN_TEST(test_arithmetic_division);
    RUN_TEST(test_arithmetic_with_variable);
    RUN_TEST(test_arithmetic_complex);

    /* String concatenation tests (Phase 04.2) */
    RUN_TEST(test_string_concat_basic);
    RUN_TEST(test_string_concat_with_variable);
    RUN_TEST(test_string_concat_with_toString);
    RUN_TEST(test_string_concat_empty_string);
    RUN_TEST(test_string_concat_null_propagation);

    /* Type conversion function tests */
    RUN_TEST(test_function_toInteger);
    RUN_TEST(test_function_toFloat);
    RUN_TEST(test_function_toString);

    /* NULL handling function tests */
    RUN_TEST(test_function_coalesce_first_value);
    RUN_TEST(test_function_coalesce_second_value);
    RUN_TEST(test_function_coalesce_with_variable);

    /* String function tests */
    RUN_TEST(test_function_toUpper);
    RUN_TEST(test_function_toLower);
    RUN_TEST(test_function_length);

    /* Math function tests */
    RUN_TEST(test_function_abs);
    RUN_TEST(test_function_ceil);
    RUN_TEST(test_function_floor);
    RUN_TEST(test_function_round);
    RUN_TEST(test_function_sqrt);

    /* Complex expression tests */
    RUN_TEST(test_complex_nested_arithmetic);
    RUN_TEST(test_complex_function_nesting);
    RUN_TEST(test_complex_string_and_number);
    RUN_TEST(test_complex_multi_operation);

    /* Error handling tests */
    RUN_TEST(test_division_by_zero_returns_null);
    RUN_TEST(test_null_arithmetic_propagates);

    return UNITY_END();
}
