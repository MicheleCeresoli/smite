#ifndef SMITE_CORE_H
#define SMITE_CORE_H

// SMITE, a SMall unIt TEster for C/C++ codes.

/* 
The following code is inspired from the Unity test library 
freely available at: https://github.com/ThrowTheSwitch/Unity.
*/

#ifdef __cplusplus
extern "C" {
#endif

/* The limits.h header determines various properties of the various 
variable types. The macros defined in this header, limits the values of 
various variable types like char, int and long. These limits specify 
that a variable cannot store any value beyond them. */
#include <limits.h>

/*The stdint.h header is used to obtain the limits of integer 
types capable of holding object pointers. In C <limits.h> is already 
included in <stdint.h>. However, whereas <stdint.h> is implementation 
independent, the maximum and minimum integer values in <limits.h> 
depend upon the compiler used.  */
#include <stdint.h>

/*The math.h header is used to provide the isinf and isnan macros.*/
#include <math.h>

/* The <setjmp.h> header is exploited to handle test failures, allowing
SMITE to automatically jump out of a function and prevent death errors.*/ 
#include <setjmp.h>

// Infers from UINT_MAX the size of an INT
#ifndef SMITE_INT_WIDTH
    #ifdef UINT_MAX
        #if (UINT_MAX == 0xFFFF)
            #define SMITE_INT_WIDTH (16)
        #elif (UINT_MAX == 0xFFFFFFFF)
            #define SMITE_INT_WIDTH (32)
        #elif (UINT_MAX == 0xFFFFFFFFFFFFFFFF)
            #define SMITE_INT_WIDTH (64)
        #endif 
    #else 
        #define SMITE_INT_WIDTH (32) // Default int width
    #endif 
#endif

// Infers from ULONG_MAX the size of a LONG
#ifndef SMITE_LONG_WIDTH
    #ifdef ULONG_MAX
        #if (UINULONG_MAXT_MAX == 0xFFFF)
            #define SMITE_LONG_WIDTH (16)
        #elif (ULONG_MAX == 0xFFFFFFFF)
            #define SMITE_LONG_WIDTH (32)
        #elif (ULONG_MAX == 0xFFFFFFFFFFFFFFFF)
            #define SMITE_LONG_WIDTH (64)
        #endif 
    #else // Default long width
        #define SMITE_LONG_WIDTH (32) 
    #endif 
#endif

// Infers from UINTPTR_MAX the size of a POINTER 
#ifndef SMITE_POINTER_WIDTH 
    #ifdef UINTPTR_MAX
        #if (UINTPTR_MAX <= 0xFFFF)
            #define SMITE_POINTER_WDITH (16)
        #elif (UINTPTR_MAX <= 0xFFFFFFFF)
            #define SMITE_POINTER_WIDTH (32)
        #elif (UINTPTR_MAX <= 0xFFFFFFFFFFFFFFFF)
            #define SMITE_POINTER_WIDTH (64)
        #endif 
    #else // Default pointer width
        #define SMITE_LONG_WIDTH SMITE_LONG_WIDTH 
    #endif 
#endif

// SMITE int types definition 
#if (SMITE_INT_WIDTH == 32)
    typedef unsigned char   SMITE_UINT8;
    typedef unsigned short  SMITE_UINT16;
    typedef unsigned int    SMITE_UINT32;
    typedef signed char     SMITE_INT8;
    typedef signed short    SMITE_INT16;
    typedef signed int      SMITE_INT32;
#elif (SMITE_INT_WIDTH == 16)
    typedef unsigned char   SMITE_UINT8;
    typedef unsigned int    SMITE_UINT16;
    typedef unsigned long   SMITE_UINT32;
    typedef signed char     SMITE_INT8;
    typedef signed int      SMITE_INT16;
    typedef signed long     SMITE_INT32;
#else 
    #error Invalid SMITE_INT_WIDTH detected (only 16 and 32 are supported!)
#endif

// Introduce 64 Bit Support
#ifndef SMITE_SUPPORT_64
    // 64 bits are supported depending on the values inferred previously
    #if SMITE_LONG_WIDTH == 64 || SMITE_POINTER_WIDTH == 64
        #define SMITE_SUPPORT_64
    #endif
#endif

#ifndef SMITE_SUPPORT_64
    typedef SMITE_UINT32 SMITE_UINT; 
    typedef SMITE_INT32 SMITE_INT;
#else 
    #if (SMITE_LONG_WIDTH == 32)
        typedef unsigned long long  SMITE_UINT64;
        typedef signed long long    SMITE_INT64; 
    #elif (SMITE_LONG_WIDTH == 64)
        typedef unsigned long       SMITE_UINT64; 
        typedef signed long         SMITE_INT64;
    #else
        #error Invalid SMITE_LONG_WIDTH specified (only 32 and 64 are supported)
    #endif

    typedef SMITE_UINT64 SMITE_UINT; 
    typedef SMITE_INT64 SMITE_INT; 

#endif 

// SMITE pointer support 
#if (SMITE_POINTER_WIDTH == 16)
    #define SMITE_PTR_TO_INT SMITE_INT16
    #define SMITE_DISPLAY_STYLE_POINTER SMITE_DISPLAY_STYLE_HEX16;
#elif (SMITE_POINTER_WIDTH == 32)
    #define SMITE_PTR_TO_INT SMITE_INT32 
    #define SMITE_DISPLAY_STYLE_POINTER SMITE_DISPLAY_STYLE_HEX32;
#elif (SMITE_POINTER_WIDTH == 64)
    #define SMITE_PTR_TO_INT SMITE_INT64
    #define SMITE_DISPLAY_STYLE_POINTER SMITE_DISPLAY_STYLE_HEX64;
#else
    #error Invalid SMITE_POINTER_WIDTH (only 16, 32 and 64 are supported)
#endif

// #ifndef SMITE_VOID_POINTER
//     #define SMITE_VOID_POINTER const void*
// #endif

// SMITE floating point support 
#ifndef SMITE_FLOAT_PRECISION
#define SMITE_FLOAT_PRECISION (0.00001f)
#endif 
#ifndef SMITE_FLOAT_TYPE
#define SMITE_FLOAT_TYPE float 
#endif 

typedef SMITE_FLOAT_TYPE SMITE_FLOAT; 

// SMITE double floating point support 
#ifndef SMITE_EXCLUDE_FLOAT 
    #ifndef SMITE_DOUBLE_TYPE
        #define SMITE_DOUBLE_TYPE double 
    #endif
    typedef SMITE_DOUBLE_TYPE SMITE_DOUBLE; 
#endif 

// Here we do the thing to gather the terminal size! 
#if defined (_WIN32)
#elif defined (__unix__)
#endif 

/* SMITE Color Types */
#ifndef SMITE_AVOID_COLORS
    #define SMITE_FAILED    "\x1b[1;31mFAILED\x1b[0m"
    #define SMITE_PASSED    "\x1b[32mPASSED\x1b[0m"
    #define SMITE_RUNNING   "\x1b[35mRUNNING\x1b[0m"
#else 
    #define SMITE_PASSED    "PASSED"
    #define SMITE_FAILED    "FAILED"
    #define SMITE_RUNNING   "RUNNING"
#endif

#define SMITE_RED           "\x1b[31m"
#define SMITE_GREEN         "\x1b[32m"
#define SMITE_ORANGE        "\x1b[33m"
#define SMITE_MAGENTA       "\x1b[35m"

#define SMITE_RESET_COLOR   "\x1b[0m"
#define SMITE_DEFAULT_COLOR SMITE_RESET_COLOR

/* SMITE error strings */

#define SMITE_STR_EXPECTED                  "Expected "
#define SMITE_STR_ELEMENT                   "Element "
#define SMITE_STR_WAS                       "was "
#define SMITE_STR_AT_LINE                   "at line "
#define SMITE_EQUALITY_ASSERTION_ERROR      "Equality assertion error "

#define SMITE_DEFAULT_NUMBER_STR_SIZE 20

/* SMITE Core structures definition 
Main SMITE structure to store each test information */

typedef void (*SmiteTestFunction)(void); 

#define SMITE_DOUBLE_TOLERANCE 1e-12

#define SMITE_DEFAULT_TEST_NUMBER (100)
#define SMITE_DEFAULT_TEST_BATCH_EXPANSION_SIZE SMITE_DEFAULT_TEST_NUMBER

struct SMITE_TEST
{   
    SmiteTestFunction   TestFun; 
    const char*         TestFile; 
    const char*         TestName; 
    SMITE_UINT          TestLineNumber; 
    SMITE_UINT          TestFailed; 
    char*               ErrorType;
    int                 ErrorLine; 
    char                ErrorDetails[100]; 

};

// SMITE structure to store the entire batch of tests.
struct SMITE_SESSION
{
    int                     TestNumber; 
    int                     Fails;
    SMITE_UINT              TestBatchCurrentSize;
    struct SMITE_TEST*      TestBatch;
    jmp_buf                 AbortFrame;
};

#define TEST_PROTECT() (setjmp(SmiteSession.AbortFrame) == 0)
#define TEST_ABORT()   (longjmp(SmiteSession.AbortFrame, 1))
#define TEST_FAIL_AND_ABORT() do{ SmiteCurrentTest->TestFailed = 1; TEST_ABORT(); } while(0)

#define SMITE_FREE_BATCH()        free(SmiteSession.TestBatch);

// extern struct SMITE_SESSION SmiteTestBatch;

/* SMITE main routines */

int  SmiteRunSession(void);
void SmiteBeginSession(void); 
void SmiteExpandTestBatch(void);

void SmiteAddSingleTest(SmiteTestFunction Fun, 
                        const char* FunName, 
                        const char* FileName,
                        SMITE_UINT LineNumber);

/* SMITE utilities */

#define SMITE_CURSOR_FORWARD(x)     printf("\033[%dC", (x))
#define SMITE_CURSOR_BACKWARD(x)    printf("\033[%dD", (x))

void SmiteUpdatePrintedStatus(int currentTest, int status);
void SmitePrintStatus(int currentTest, const char* color);
void SmitePrintResult(int status); 
void SmitePrintBeginning();
void SmitePrintSessionSuccess(); 
void SmitePrintSessionFailure();

/* SMITE assertion functions */
void SmiteAssertEqualInteger(const SMITE_INT expected, 
                             const SMITE_INT actual, 
                             const SMITE_UINT line);

void SmiteAssertEqualDouble(const SMITE_DOUBLE expected, 
                            const SMITE_DOUBLE actual, 
                            const SMITE_UINT line);

void SmiteAssertEqualIntArray(const int* expected, 
                              const int* actual, 
                              const SMITE_UINT elements, 
                              const SMITE_UINT line);

void SmiteAssertEqualDoubleArray(const double* expected,
                                 const double* actual,
                                 const SMITE_UINT elements,
                                 const SMITE_UINT line);


/* SMITE macros to override the basic assert function */ 

#define ASSERT_EQUAL_INT(expected, actual)                  SmiteAssertEqualInteger(expected, actual, __LINE__)
#define ASSERT_EQUAL_INT_ARRAY(expected, actual, elements)  SmiteAssertEqualIntArray((const int*)expected, (const int*)actual, elements, __LINE__)

#define ASSERT_EQUAL_DOUBLE(expected, actual)                   SmiteAssertEqualDouble(expected, actual, __LINE__)
#define ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, elements)   SmiteAssertEqualDoubleArray((const double*)expected, (const double*)actual, elements, __LINE__)

// These series of variadic macros are supported only for versions of C >= C99
#define ADD_TEST(...) ADD_TEST_AT_LINE(__VA_ARGS__, __LINE__, __FILE__)
#define ADD_TEST_AT_LINE(fun, line, file, ...) SmiteAddSingleTest(fun, #fun, file, line)

#ifdef __cplusplus
}
#endif

#endif

