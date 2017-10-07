///
/// author: Karol Trzcinski
/// date: 10-2017
///

#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include "../logger.h"

#define ARRAY_LEN(x) (sizeof(x)/sizeof(*x))

void fillBuff(char* buf, int size)
{
	for (int i = 0; i < 2 * LOGGER_BUF_SIZE / 2 + 5; ++i) {
		buf[i] = i % 10 + '0';
	}
}

int logger_test_main()
{
	printf("=================\n");
	printf("Logger test start\n");
	printf("==================\n");
	logger_init();
	LOG_INFO("start %d", 0);
	LOG_WARNING("watch %c", 'c');
	LOG_ERR("some error log with message ''%s'", "some message");
	LOG_CRITICAL("some terrible error");
	printf("\tThere shouldn't be any messages to this text\n");
	printf("\tand I, W, E, C messages below\n");
	logger_process(); Sleep(2);
	logger_process(); Sleep(2);
	logger_process(); Sleep(2);

	printf("\n\tTry to overload temp buffer\n");
	printf("\tand check if maximal size message will be copied to buf\n");
	printf("\tthere should be 3 identical messages\n");
	char bb[2*LOGGER_BUF_SIZE];
	fillBuff(bb, ARRAY_LEN(bb));
	LOG_INFO(bb);/*
	bb[LOGGER_BUF_SIZE] = 0;
	LOG_INFO(bb);
	bb[LOGGER_BUF_SIZE-1] = 0;
	LOG_INFO(bb);
	logger_process(); Sleep(2);
	//logger_process(); Sleep(2);
	//logger_process(); Sleep(2);

	printf("\n\tTry to overload all buffers with trimmed messages\n");
	printf("\tthere should be %d messages\n", LOGGER_BUF_COUNT+1);
	fillBuff(bb, ARRAY_LEN(bb));
	for (int i = 0; i < 100; i += 1) {
		bb[0] = i + 'a';
		LOG_INFO(bb);
	}
	logger_process(); Sleep(1);
	logger_process(); Sleep(1);
	logger_process(); Sleep(1);

	printf("\n\tNow process messages 2 times when buffers are empty\n");
	printf("second process\n");
	logger_process();
	printf("third process\n");
	logger_process();*/

	printf("Logger test end");
	return 0;
}
