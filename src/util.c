#include "util.h"

int buffer_derelativize(char* buffer, int length)
{
	if (length < 2)
	{
		return length;
	}
	
	const _Bool is_absolute = (buffer[0] == '/');
	const int min_write_pos = is_absolute ? 1 : 0;
	int write_pos = 0;
	int read_pos = 0;
	
	while (read_pos < length)
	{
		if (read_pos + 1 < length && buffer[read_pos] == '/' && buffer[read_pos + 1] == '.')
		{
			if (read_pos + 2 < length && buffer[read_pos + 2] == '.'
				&& (read_pos + 3 >= length || buffer[read_pos + 3] == '/' || buffer[read_pos + 3] == '\n'))
			{
				read_pos += 3;
				if (write_pos > min_write_pos)
				{
					--write_pos;
					while (write_pos > min_write_pos && buffer[write_pos] != '/')
					{
						--write_pos;
					}
				}
				else if (write_pos == 0 && is_absolute)
				{
					buffer[0] = '/';
					write_pos = 1;
				}
				continue;
			}
			if (read_pos + 2 >= length || buffer[read_pos + 2] == '/' || buffer[read_pos + 2] == '\n')
			{
				read_pos += 2;
				continue;
			}
		}
		
		if (buffer[read_pos] == '/' && write_pos > 0 && buffer[write_pos - 1] == '/')
		{
			++read_pos;
			continue;
		}
		
		buffer[write_pos] = buffer[read_pos];
		++write_pos;
		++read_pos;
	}
	
	if (write_pos == 0 && is_absolute)
	{
		buffer[0] = '/';
		write_pos = 1;
	}
	
	buffer[write_pos] = '\0';
	return write_pos;
}
