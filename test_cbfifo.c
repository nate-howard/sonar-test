#include "test_cbfifo.h"

#define MAX_MSG_LEN 100
#define MAX_EXP_LEN 70
#define assertInit(str, funcName) snprintf((str), MAX_MSG_LEN, "cbfifo: %s FAILED\n\t", (funcName)); char expStr[MAX_EXP_LEN];
#define assertEq(was, exp, str, msg) if((was) != (exp)) { snprintf(expStr, MAX_EXP_LEN, (msg), (was), (exp)); strncat((str), expStr, MAX_EXP_LEN); return 1; }
#define assertf(cond, str, msg, p1, p2) if(!(cond)) { snprintf(expStr, MAX_EXP_LEN, (msg), (p1), (p2)); strncat((str), expStr, MAX_EXP_LEN); return 1; }

#define CB_SIZE 512

int resetTest(char* errMsg) {
    assertInit(errMsg, "resetTest");

    char padding[CB_SIZE];
    memset(padding, 'X', CB_SIZE - 1);
    padding[CB_SIZE - 1] = '\0';

    cbfifo_enqueue(padding, 4);
    cbfifo_reset();
    assertEq(cbfifo_length(), 0, errMsg, "Length after reset 1: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");
    
    cbfifo_enqueue(padding, 10);
    cbfifo_reset();
    assertEq(cbfifo_length(), 0, errMsg, "Length after reset 2: RECEIVED %zu, EXPECTED %d");

    cbfifo_enqueue(padding, 300);
    cbfifo_reset();
    assertEq(cbfifo_length(), 0, errMsg, "Length after reset 3: RECEIVED %zu, EXPECTED %d");

    cbfifo_enqueue(padding, CB_SIZE);
    cbfifo_reset();
    assertEq(cbfifo_length(), 0, errMsg, "Length after reset 4: RECEIVED %zu, EXPECTED %d");
    
    cbfifo_reset();
    assertEq(cbfifo_length(), 0, errMsg, "Length after reset 5: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap after resets: RECEIVED %zu, EXPECTED %d");

    return 0;
}

int errTest(char* errMsg) {
    assertInit(errMsg, "errTest");

    cbfifo_reset();

    assertEq(cbfifo_enqueue(NULL, 4), (size_t)-1, errMsg, "enqueue null: RECEIVED %zu, EXPECTED %zu");
    assertEq(cbfifo_length(), 0, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");

    int x = -1;
    assertEq(cbfifo_enqueue(&x, 0), 0, errMsg, "enqueue zero bytes: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_length(), 0, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");

    char padding[CB_SIZE];
    memset(padding, 'X', CB_SIZE - 1);
    padding[CB_SIZE - 1] = '\0';
    cbfifo_enqueue(padding, CB_SIZE);
    assertEq(cbfifo_enqueue(&x, 4), 0, errMsg, "enqueue when full: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_length(), CB_SIZE, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");

    assertEq(cbfifo_dequeue(NULL, 4), (size_t)-1, errMsg, "dequeue null: RECEIVED %zu, EXPECTED %zu");
    assertEq(cbfifo_length(), CB_SIZE, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");

    assertEq(cbfifo_dequeue(padding, 0), 0, errMsg, "dequeue zero bytes: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_length(), CB_SIZE, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");

    return 0;
}

int enqTest(char* errMsg) {
    assertInit(errMsg, "enqTest");

    cbfifo_reset();

    int x = -1;
    char padding[CB_SIZE];
    memset(padding, 'X', CB_SIZE - 1);
    padding[CB_SIZE - 1] = '\0';

    assertEq(cbfifo_enqueue(&x, 0), 0, errMsg, "enqueue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_enqueue(&x, 1), 1, errMsg, "enqueue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_enqueue(&x, 2), 2, errMsg, "enqueue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_enqueue(&x, 3), 3, errMsg, "enqueue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_enqueue(&x, 4), 4, errMsg, "enqueue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_length(), 10, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");

    assertEq(cbfifo_enqueue(padding, 100), 100, errMsg, "enqueue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_enqueue(padding, 200), 200, errMsg, "enqueue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_enqueue(padding, 300), 202, errMsg, "enqueue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_length(), CB_SIZE, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");
    
    assertEq(cbfifo_enqueue(&x, 0), 0, errMsg, "enqueue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_enqueue(&x, 4), 0, errMsg, "enqueue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_length(), CB_SIZE, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");

    return 0;
}

int deqTest(char* errMsg) {
    assertInit(errMsg, "deqTest");

    cbfifo_reset();

    int x = -1;
    char padding[CB_SIZE];
    memset(padding, 'X', CB_SIZE - 1);
    padding[CB_SIZE - 1] = '\0';
    cbfifo_enqueue(padding, CB_SIZE);

    assertEq(cbfifo_dequeue(&x, 0), 0, errMsg, "dequeue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_dequeue(&x, 1), 1, errMsg, "dequeue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_dequeue(&x, 2), 2, errMsg, "dequeue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_dequeue(&x, 3), 3, errMsg, "dequeue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_dequeue(&x, 4), 4, errMsg, "dequeue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_length(), CB_SIZE - 10, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");

    assertEq(cbfifo_dequeue(padding, 100), 100, errMsg, "dequeue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_dequeue(padding, 200), 200, errMsg, "dequeue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_dequeue(padding, 300), 202, errMsg, "dequeue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_length(), 0, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");
    
    assertEq(cbfifo_dequeue(&x, 0), 0, errMsg, "dequeue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_dequeue(&x, 4), 0, errMsg, "dequeue: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_length(), 0, errMsg, "Length: RECEIVED %zu, EXPECTED %d");
    assertEq(cbfifo_capacity(), CB_SIZE, errMsg, "Cap: RECEIVED %zu, EXPECTED %d");

    return 0;
}

int test_cbfifo() {
    char errMsg[100];
    int (*testFuncs[])(char*) = {resetTest, errTest, enqTest, deqTest};

    int nTests = sizeof(testFuncs) / sizeof(testFuncs[0]);
    int nFailed = 0;
    for(int i = 0; i < nTests; i++) {
        if((*testFuncs[i])(errMsg)) {
            printf("%s\n", errMsg);
            nFailed++;
        }
    }

    printf("cbfifo: %d/%d PASSED\n\n", nTests - nFailed, nTests);
    return nFailed == 0;
}