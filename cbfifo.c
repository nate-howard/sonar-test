/*******************************************************************************
 * Copyright (C) 2024 by <Student Name>
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it to learn about the field of embedded
 * software. <Student Name> and the University of Colorado are not liable for
 * any misuse of this material.
 * ****************************************************************************/

/**
 * @file    cbfifo.c
 * @brief
 *
 *
 * @author  <Student Name>
 * @date    <Date>
 *
 */
#include "cbfifo.h"

#define CB_SIZE 512

static char buffer[CB_SIZE];
static int read = 0;
static int write = 0;
static char isFull = 0;

/*
 * Enqueues data onto the FIFO, up to the limit of the available FIFO
 * capacity.
 *
 * Parameters:
 *   buf      Pointer to the data
 *   nbyte    Max number of bytes to enqueue
 * 
 * Returns:
 *   The number of bytes actually enqueued, which could be 0. In case
 *   of an error, returns (size_t) -1. For example, if the capacity is 30,
 *   and current length of FIFO is 0. A call to enqueue 35 bytes, will
 *   only enqueue 30 to the capacity and return 30 from this call. Application
 *   needs to retry enqueuing the remaining 5 bytes subsequently.
 */
size_t cbfifo_enqueue(void *buf, size_t nbyte) {
    if(buf == NULL) return -1;

    char* readBuf = (char*) buf;
    int readIdx = 0;
    while(!isFull && readIdx < nbyte) {
        buffer[write] = readBuf[readIdx];
        readIdx++;

        write = (write+1) & (CB_SIZE-1); // Increment and mod
        isFull = write == read;
    }

    return readIdx;
}


/*
 * Attempts to remove ("dequeue") up to nbyte bytes of data from the
 * FIFO. Removed data will be copied into the buffer pointed to by buf.
 *
 * Parameters:
 *   buf      Destination for the dequeued data
 *   nbyte    Bytes of data requested
 * 
 * Returns:
 *   The number of bytes actually copied, which will be between 0 and
 *   nbyte(inclusive of both). 
 * 
 * To further explain the behavior: If the FIFO's current length is 24
 * bytes, and the caller requests 30 bytes, cbfifo_dequeue should
 * return the 24 bytes it has, and the new FIFO length will be 0. If
 * the FIFO is empty (current length is 0 bytes), a request to dequeue
 * any number of bytes will result in a return of 0 from
 * cbfifo_dequeue.
 */
size_t cbfifo_dequeue(void *buf, size_t nbyte) {
    if(buf == NULL) return -1;

    char* writeBuf = (char*) buf;
    int writeIdx = 0;
    while((read != write || isFull) && writeIdx < nbyte) {
        writeBuf[writeIdx] = buffer[read];
        writeIdx++;

        read = (read+1) & (CB_SIZE-1); // Increment and mod
        if(isFull) isFull = 0;
    }

    return writeIdx;
}


/*
 * Returns the number of bytes currently on the FIFO. 
 *
 * Parameters:
 *   none
 * 
 * Returns:
 *   Number of bytes currently available to be dequeued from the FIFO
 */
size_t cbfifo_length() {
    return isFull ? CB_SIZE : ((write - read) & (CB_SIZE-1));
}


/*
 * Returns the FIFO's capacity
 *
 * Parameters:
 *   none
 * 
 * Returns:
 *   The capacity, in bytes, for the FIFO
 */
size_t cbfifo_capacity() {
    return CB_SIZE;
}

/*
 * Resets the FIFO clearing all elements resulting in a 0 length. Basically,
 * the length of the FIFO goes to 0 after reset but capacity 
 * stays intact.
 *
 * Parameters:
 *   none
 *
 * Returns:
 *   Nothing
 */
void cbfifo_reset() {
    read = write;
    isFull = 0;
}
