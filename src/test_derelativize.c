#include "util.h"

#include <string.h>
#include <stdio.h>

#include <linux/limits.h>

static char buffer[PATH_MAX];

int main(int argc, const char** argv)
{
	if (argc != 3)
	{
		printf("%s: wrong nummber of arguments, expected 2\n", argv[0]);
		return 1;
	}
	
	const char* expected = argv[1];
	const char* actual = argv[2];
	unsigned long actual_length = strlen(actual);
	
	if (actual_length + 1 >= sizeof(buffer))
	{
		printf("%s: second argument is too long (%lu)\n", argv[0], actual_length);
		return 1;
	}
	
	memcpy(buffer, actual, actual_length);
	buffer[actual_length] = '\0';
	
	buffer_derelativize(buffer, (int)actual_length);
	
	int cmp = strcmp(expected, buffer);
	if (cmp != 0)
	{
		printf("expected: %s\n", expected);
		printf("  actual: %s\n", buffer);
		return 1;
	}
	else
	{
		printf("ok: %s -> %s\n", actual, expected);
	}
	return 0;
}
