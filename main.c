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
 * @file    main.c
 * @brief
 *
 *
 * @author  <Student Name>
 * @date    <Date>
 *
 */

#include "test_lllifo.h"
#include "test_cbfifo.h"

int main(int argc, char **argv)
{
	test_lllifo();
	test_cbfifo();
	return 0;
}
