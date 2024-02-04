#include "test_lllifo.h"

#define MAX_MSG_LEN 100
#define MAX_EXP_LEN 70
#define assertInit(str, funcName) snprintf((str), MAX_MSG_LEN, "lllifo: %s FAILED\n\t", (funcName)); char expStr[MAX_EXP_LEN];
#define assertEq(was, exp, str, msg) if((was) != (exp)) { snprintf(expStr, MAX_EXP_LEN, (msg), (was), (exp)); strncat((str), expStr, MAX_EXP_LEN); return 1; }
#define assertf(cond, str, msg, p1, p2) if(!(cond)) { snprintf(expStr, MAX_EXP_LEN, (msg), (p1), (p2)); strncat((str), expStr, MAX_EXP_LEN); return 1; }


static int errTest(char* errMsg) {
    assertInit(errMsg, "errTest");

    lllifo_t *my_lifo = lllifo_create(-1, 3);
    assertEq(my_lifo, NULL, errMsg, "Negative cap: RECEIVED %p, EXPECTED %p");
    my_lifo = lllifo_create(3, -1);
    assertEq(my_lifo, NULL, errMsg, "Negative max cap: RECEIVED %p, EXPECTED %p");
    my_lifo = lllifo_create(0, 0);
    assertEq(my_lifo, NULL, errMsg, "Zero max cap: RECEIVED %p, EXPECTED %p");
    my_lifo = lllifo_create(3, 2);
    assertEq(my_lifo, NULL, errMsg, "cap > max cap: RECEIVED %p, EXPECTED %p");

    my_lifo = lllifo_create(1, 2);
    assertEq(lllifo_pop(my_lifo), NULL, errMsg, "pop when empty: RECEIVED %p, EXPECTED %p");
    assertEq(lllifo_pop(NULL), NULL, errMsg, "pop from null: RECEIVED %p, EXPECTED %p");

    assertEq(lllifo_push(NULL, "Hi"), -1, errMsg, "push to null: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_push(my_lifo, NULL), -1, errMsg, "pushed null: RECEIVED %d, EXPECTED %d");

    assertEq(lllifo_length(NULL), -1, errMsg, "got len of null: RECEIVED %d, EXPECTED %d");

    assertEq(lllifo_capacity(NULL), -1, errMsg, "got len of null: RECEIVED %d, EXPECTED %d");

    assertEq(lllifo_max_capacity(NULL), -1, errMsg, "got len of null: RECEIVED %d, EXPECTED %d");

    lllifo_reset(NULL);
    lllifo_destroy(NULL);

    lllifo_destroy(my_lifo);
    return 0;
}

static int pushPopTest(char* errMsg) {
    assertInit(errMsg, "pushPopTest");

    lllifo_t *my_lifo = lllifo_create(5, 10);
    assertEq(lllifo_length(my_lifo), 0, errMsg, "Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(my_lifo), 5, errMsg, "Capactity: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_max_capacity(my_lifo), 10, errMsg, "Max Capacity: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_push(my_lifo, "Sleepy"), 1, errMsg, "push return: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_push(my_lifo, "Grumpy"), 2, errMsg, "push return: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_push(my_lifo, "Sneezy"), 3, errMsg, "push return: RECEIVED %d, EXPECTED %d"); // does not result in a malloc (len = 3, cap = 5)
    assertEq(lllifo_push(my_lifo, "Happy"), 4, errMsg, "push return: RECEIVED %d, EXPECTED %d"); // does not result in a malloc (len = 4, cap = 5)
    assertEq(lllifo_push(my_lifo, "Bashful"), 5, errMsg, "push return: RECEIVED %d, EXPECTED %d"); // does not result in a malloc (len = 5, cap = 5)
    lllifo_pop(my_lifo); // removes "Bashful"; len = 4, cap = 5
    assertEq(lllifo_push(my_lifo, "Dopey"), 5, errMsg, "push return: RECEIVED %d, EXPECTED %d"); // does not result in a malloc (len = 5, cap = 5)
    assertEq(lllifo_push(my_lifo, "Doc"), 6, errMsg, "push return: RECEIVED %d, EXPECTED %d"); // DOES result in a malloc (len = 6, cap = 6)
    assertEq(lllifo_length(my_lifo), 6, errMsg, "Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(my_lifo), 6, errMsg, "Capactity: RECEIVED %d, EXPECTED %d");

    
    assertEq(lllifo_push(my_lifo, "Filler"), 7, errMsg, "push return: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_push(my_lifo, "Filler"), 8, errMsg, "push return: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_push(my_lifo, "Filler"), 9, errMsg, "push return: RECEIVED %d, EXPECTED %d");

    for(int i = 0; i < 100; i++) {
        assertEq(lllifo_push(my_lifo, "Tribble"), 10, errMsg, "push return: RECEIVED %d, EXPECTED %d");
    }

    char* str;
    for(int i = 0; i < 10; i++) {
        str = (char*)lllifo_pop(my_lifo);
        assertf(strcmp(str, "Tribble") == 0, errMsg, "10 pops: RECEIVED %s, EXPECTED %s", str, "Tribble");
    }
    assertEq(lllifo_length(my_lifo), 0, errMsg, "Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(my_lifo), 10, errMsg, "Capactity: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_max_capacity(my_lifo), 10, errMsg, "Max Capacity: RECEIVED %d, EXPECTED %d");

    assertEq(lllifo_pop(my_lifo), NULL, errMsg, "pop when empty: RECEIVED %p, EXPECTED %p");

    lllifo_destroy(my_lifo);
    return 0;
}

static int resetTest(char* errMsg) {
    assertInit(errMsg, "resetTest");

    lllifo_t *my_lifo = lllifo_create(5, 10);
    assertEq(lllifo_length(my_lifo), 0, errMsg, "Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(my_lifo), 5, errMsg, "Capactity: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_max_capacity(my_lifo), 10, errMsg, "Max Capacity: RECEIVED %d, EXPECTED %d");
    lllifo_push(my_lifo, "Sleepy");
    lllifo_push(my_lifo, "Grumpy");
    lllifo_push(my_lifo, "Sneezy");
    lllifo_push(my_lifo, "Happy");
    lllifo_push(my_lifo, "Bashful");
    lllifo_pop(my_lifo);
    lllifo_push(my_lifo, "Dopey");
    lllifo_push(my_lifo, "Doc");
    lllifo_pop(my_lifo);
    lllifo_pop(my_lifo);

    lllifo_reset(my_lifo);
    assertEq(lllifo_length(my_lifo), 0, errMsg, "Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(my_lifo), 6, errMsg, "Capactity: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_max_capacity(my_lifo), 10, errMsg, "Max Capacity: RECEIVED %d, EXPECTED %d");

    for(int i = 0; i < 100; i++) {
        lllifo_push(my_lifo, "Tribble");
    }
    lllifo_reset(my_lifo);
    assertEq(lllifo_length(my_lifo), 0, errMsg, "Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(my_lifo), 10, errMsg, "Capactity: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_max_capacity(my_lifo), 10, errMsg, "Max Capacity: RECEIVED %d, EXPECTED %d");

    lllifo_destroy(my_lifo);
    return 0;
}

static int multTest(char* errMsg) {
    assertInit(errMsg, "multTest");
    
    lllifo_t *lifo1 = lllifo_create(5, 10);
    lllifo_t *lifo2 = lllifo_create(4, 8);
    assertEq(lllifo_length(lifo1), 0, errMsg, "lifo1 Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(lifo1), 5, errMsg, "lifo1 Capactity: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_max_capacity(lifo1), 10, errMsg, "lifo1 Max Capacity: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_length(lifo2), 0, errMsg, "lifo2 Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(lifo2), 4, errMsg, "lifo2 Capactity: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_max_capacity(lifo2), 8, errMsg, "lifo2 Max Capacity: RECEIVED %d, EXPECTED %d");

    
    lllifo_push(lifo1, "Sleepy");
    lllifo_push(lifo1, "Sleepy");
    lllifo_push(lifo1, "Sleepy");
    lllifo_push(lifo1, "Sleepy");
    lllifo_push(lifo1, "Sleepy");
    lllifo_pop(lifo1);
    lllifo_push(lifo1, "Sleepy");
    lllifo_push(lifo1, "Sleepy");
    assertEq(lllifo_length(lifo1), 6, errMsg, "lifo1 Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(lifo1), 6, errMsg, "lifo1 Capactity: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_length(lifo2), 0, errMsg, "lifo2 Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(lifo2), 4, errMsg, "lifo2 Capactity: RECEIVED %d, EXPECTED %d");

    lllifo_destroy(lifo1);

    lllifo_push(lifo2, "Sleepy");
    lllifo_push(lifo2, "Sleepy");
    lllifo_push(lifo2, "Sleepy");
    lllifo_push(lifo2, "Sleepy");
    lllifo_push(lifo2, "Sleepy");
    lllifo_pop(lifo2);
    lllifo_pop(lifo2);
    lllifo_pop(lifo2);
    assertEq(lllifo_length(lifo2), 2, errMsg, "lifo2 Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(lifo2), 5, errMsg, "lifo2 Capactity: RECEIVED %d, EXPECTED %d");

    lllifo_destroy(lifo2);
    return 0;
}

static int limitTest(char* errMsg) {
    assertInit(errMsg, "limitTest")

    lllifo_t *my_lifo = lllifo_create(1, 2);
    lllifo_push(my_lifo, "Sleepy");
    lllifo_push(my_lifo, "Grumpy");
    lllifo_push(my_lifo, "Sneezy");
    assertEq(lllifo_length(my_lifo), 2, errMsg, "Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(my_lifo), 2, errMsg, "Capactiy: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_max_capacity(my_lifo), 2, errMsg, "Max Capactiy: RECEIVED %d, EXPECTED %d");
    char* str = (char*)lllifo_pop(my_lifo);
    assertf(strcmp(str, "Sneezy") == 0, errMsg, "First pop: RECEIVED %s, EXPECTED %s", str, "Sneezy");
    str = (char*)lllifo_pop(my_lifo);
    assertf(strcmp(str, "Grumpy") == 0, errMsg, "First pop: RECEIVED %s, EXPECTED %s", str, "Grumpy");
    str = (char*)lllifo_pop(my_lifo);
    assertf(str == NULL, errMsg, "First pop: RECEIVED %s, EXPECTED %s", str, "NULL");
    assertEq(lllifo_length(my_lifo), 0, errMsg, "Length: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_capacity(my_lifo), 2, errMsg, "Capactiy: RECEIVED %d, EXPECTED %d");
    assertEq(lllifo_max_capacity(my_lifo), 2, errMsg, "Max Capactiy: RECEIVED %d, EXPECTED %d");

    lllifo_destroy(my_lifo);
    return 0;
}

int test_lllifo() {
    char errMsg[100];
    int (*testFuncs[])(char*) = {errTest, pushPopTest, resetTest, multTest, limitTest};

    int nTests = sizeof(testFuncs) / sizeof(testFuncs[0]);
    int nFailed = 0;
    for(int i = 0; i < nTests; i++) {
        if((*testFuncs[i])(errMsg)) {
            printf("%s\n", errMsg);
            nFailed++;
        }
    }

    printf("lllifo: %d/%d PASSED\n\n", nTests - nFailed, nTests);
    return nFailed == 0;
}
