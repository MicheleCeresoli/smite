#include <stdio.h>
#include <stdlib.h>
#include "smite.h"

struct SMITE_SESSION    SmiteSession;
struct SMITE_TEST*      SmiteCurrentTest;

/* SMITE Main Routine Functions */

void SmiteBeginSession()
{
    SmiteSession.TestNumber = 0;
    SmiteSession.Fails = 0;
    SmiteSession.TestBatchCurrentSize = SMITE_DEFAULT_TEST_NUMBER;

    // Initalises a test batch to support a minimum of 100 Tests.
    // If a greater number of tests is added, new memory will be allocated.
    SmiteSession.TestBatch = malloc(SMITE_DEFAULT_TEST_NUMBER*sizeof(struct SMITE_TEST));

    SmitePrintBeginning();
}

void SmiteExpandTestBatch()
{
    SmiteSession.TestBatchCurrentSize += SMITE_DEFAULT_TEST_BATCH_EXPANSION_SIZE;
    SmiteSession.TestBatch = realloc(SmiteSession.TestBatch,
                                         SmiteSession.TestBatchCurrentSize);
}

int SmiteRunSession()
{
    printf("Collected %d tests.\nRunning test session:\n\n", SmiteSession.TestNumber);

    for (int i = 0; i < SmiteSession.TestNumber; i++)
    {
        SmitePrintStatus(i, SMITE_MAGENTA);
        printf(" - %s::%s %s", SmiteSession.TestBatch[i].TestFile,
                               SmiteSession.TestBatch[i].TestName,
                               SMITE_RUNNING);
        fflush(stdout);

        SmiteCurrentTest = &SmiteSession.TestBatch[i];

        if (TEST_PROTECT()){
            SmiteSession.TestBatch[i].TestFun();
        } else {
             // when abort is called this else is entered!
        }

        if (SmiteCurrentTest->TestFailed) {
            SmiteSession.Fails++;
        }

        SmiteUpdatePrintedStatus(i, !SmiteSession.TestBatch[i].TestFailed);
        printf("\n");
    }
    printf("\n");

    if (SmiteSession.Fails > 0) {   
        SmitePrintSessionFailure();
        SMITE_FREE_BATCH();
        return 1;
    } else {

        SMITE_FREE_BATCH();
        return 0;
    }
}

void SmiteAddSingleTest(SmiteTestFunction Fun,
                        const char* FunName,
                        const char* FileName,
                        SMITE_UINT LineNumber)
{
    SmiteSession.TestBatch[SmiteSession.TestNumber].TestFun = Fun;
    SmiteSession.TestBatch[SmiteSession.TestNumber].TestFile = FileName;
    SmiteSession.TestBatch[SmiteSession.TestNumber].TestName = FunName;
    SmiteSession.TestBatch[SmiteSession.TestNumber].TestLineNumber = LineNumber;
    SmiteSession.TestNumber += 1;

    if (SmiteSession.TestNumber > SmiteSession.TestBatchCurrentSize){
        SmiteExpandTestBatch();
    }
}


/* SMITE assertion functions */

#define SMITE_REGISTER_ERROR(type, line) do {SmiteCurrentTest->ErrorType = type; SmiteCurrentTest->ErrorLine = line; } while (0);

#include <string.h>

void SmiteAssertEqualInteger(const SMITE_INT expected,
                             const SMITE_INT actual,
                             const SMITE_UINT line)
{
    if (expected != actual)
    {

        SMITE_REGISTER_ERROR(SMITE_EQUALITY_ASSERTION_ERROR, line);

        char errorDetails[100]; 
        snprintf(errorDetails, 100, SMITE_STR_EXPECTED "%d " SMITE_STR_WAS "%d.", 
                (int)expected, (int)actual);

        strcpy(SmiteCurrentTest->ErrorDetails, errorDetails);

        TEST_FAIL_AND_ABORT();
    }

}

void SmiteAssertEqualDouble(const SMITE_DOUBLE expected,
                            const SMITE_DOUBLE actual,
                            const SMITE_UINT line)
{
    if (fabs(expected - actual) >= SMITE_DOUBLE_TOLERANCE)
    {
        SMITE_REGISTER_ERROR(SMITE_EQUALITY_ASSERTION_ERROR, line);

        char errorDetails[100]; 
        snprintf(errorDetails, 100, SMITE_STR_EXPECTED "%10f " SMITE_STR_WAS "%10f.", 
                (double)expected, (double)actual);

        strcpy(SmiteCurrentTest->ErrorDetails, errorDetails);

        TEST_FAIL_AND_ABORT();
    }

}

void SmiteAssertEqualIntArray(const int* expected, 
                              const int* actual, 
                              const SMITE_UINT elements, 
                              const SMITE_UINT line)
{
    if (elements == 0){ // Nothing to Compare
        return; 
    }

    if (expected == actual) { // Same Pointers
        return; 
    }

    for (int i = 0; i < elements; i++){

        int expected_val = expected[i];
        int actual_val = actual[i];

        if (expected_val != actual_val)
        {
            SMITE_REGISTER_ERROR(SMITE_EQUALITY_ASSERTION_ERROR, line);

            char errorDetails[100]; 
            snprintf(errorDetails, 100, SMITE_STR_ELEMENT "%d: " SMITE_STR_EXPECTED "%d " SMITE_STR_WAS "%d.", 
                    i, expected_val, actual_val);

            strcpy(SmiteCurrentTest->ErrorDetails, errorDetails);

            TEST_FAIL_AND_ABORT();
        }
    }
}

void SmiteAssertEqualDoubleArray(const double* expected,
                                 const double* actual,
                                 const SMITE_UINT elements,
                                 const SMITE_UINT line)
{
    if (elements == 0){ // Nothing to Compare
        return; 
    }

    if (expected == actual) { // Same Pointers
        return; 
    }

    for (int i = 0; i < elements; i++){

        double expected_val = expected[i];
        double actual_val = actual[i];

        if (fabs(expected_val - actual_val) >= SMITE_DOUBLE_TOLERANCE)
        {
            SMITE_REGISTER_ERROR(SMITE_EQUALITY_ASSERTION_ERROR, line);

            char errorDetails[100]; 
            snprintf(errorDetails, 100, SMITE_STR_ELEMENT "%d: " SMITE_STR_EXPECTED "%10f " SMITE_STR_WAS "%10f.", 
                    i, expected_val, actual_val);

            strcpy(SmiteCurrentTest->ErrorDetails, errorDetails);

            TEST_FAIL_AND_ABORT();
        }
    }

}




/* SMITE utilities */
#define SMITE_REPEAT_CHAR(x, y) do {for (int i = 0; i < y; i++){printf("%s", x);}} while(0)

void SmiteUpdatePrintedStatus(int currentTest, int status)
{
    SMITE_CURSOR_BACKWARD(8);
    SmitePrintResult(status);

    if (SmiteSession.Fails > 0){
        if (status) {
            SmitePrintStatus(currentTest, SMITE_ORANGE);
        } else{
            SmitePrintStatus(currentTest, SMITE_RED);
        }
    } else {
        SmitePrintStatus(currentTest, SMITE_GREEN);
    }

}

void SmitePrintResult(int status){
    if (status){
        printf(" %s \r", SMITE_PASSED);
    } else {
        printf(" %s \r", SMITE_FAILED);
    }
}

void SmitePrintStatus(int currentTest, const char* color)
{
    int percentage = (int)((currentTest+1.)/SmiteSession.TestNumber*100);
    char* spaces;
    if (percentage < 10) {
        spaces = "[  ";
    } else if (percentage < 100) {
        spaces = "[ ";
    } else {
        spaces = "[";
    }

    printf("%s%s%d%%]%s", color, spaces, percentage, SMITE_RESET_COLOR);

}

void SmitePrintBeginning()
{
    printf("\n");
    SMITE_REPEAT_CHAR("=", 20);
    printf("\033[1m Beginning Test Session \033[0m");
    SMITE_REPEAT_CHAR("=", 20);
    printf("\n\n");
}

void SmitePrintSessionSuccess()
{
    printf(SMITE_GREEN);

    SMITE_REPEAT_CHAR("=", 24);
    printf("\033[1m %d Tests Passed \033[0m" SMITE_GREEN, SmiteSession.TestNumber);
    SMITE_REPEAT_CHAR("=", 24);

    printf(SMITE_RESET_COLOR "\n\n");

}

void SmitePrintSessionFailure()
{
    printf(SMITE_RED);
    SMITE_REPEAT_CHAR("=", 21);
    printf(" Test Failures Summary ");
    SMITE_REPEAT_CHAR("=", 21);
    printf("\n\n");

    for (int i = 0; i < SmiteSession.TestNumber; i++)
    {
        if (SmiteSession.TestBatch[i].TestFailed)
        {
            printf(SMITE_RED "FAILED " SMITE_DEFAULT_COLOR);
            printf("%s::%s", SmiteSession.TestBatch[i].TestFile,
                             SmiteSession.TestBatch[i].TestName);
            printf(SMITE_RED "\n\t%sat line %d. %s\n" SMITE_DEFAULT_COLOR,
                   SmiteSession.TestBatch[i].ErrorType, 
                   SmiteSession.TestBatch[i].ErrorLine, 
                   SmiteSession.TestBatch[i].ErrorDetails);
        }
    }

    printf("\n" SMITE_RED);
    SMITE_REPEAT_CHAR("=", 16);
    printf(" \033[1m%d Tests Failed, \033[0m" SMITE_ORANGE "%d Tests Passed " SMITE_RED,
            SmiteSession.Fails, SmiteSession.TestNumber - SmiteSession.Fails);

    SMITE_REPEAT_CHAR("=", 16);
    printf("\n");

    printf(SMITE_RESET_COLOR "\n\n");

}