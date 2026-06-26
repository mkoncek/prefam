#include "util.h"

int buffer_derelativize(char* buffer, int length)
{
	if (length < 2)
	{
		return length;
	}
	
	_Bool absolute = (buffer[0] == '/');
	int min_w = absolute ? 1 : 0;
	int w = 0;
	int r = 0;
	
	while (r < length)
	{
		if (r + 1 < length && buffer[r] == '/' && buffer[r + 1] == '.')
		{
			if (r + 2 < length && buffer[r + 2] == '.'
				&& (r + 3 >= length || buffer[r + 3] == '/' || buffer[r + 3] == '\n'))
			{
				r += 3;
				if (w > min_w)
				{
					--w;
					while (w > min_w && buffer[w] != '/')
					{
						--w;
					}
				}
				else if (w == 0 && absolute)
				{
					buffer[0] = '/';
					w = 1;
				}
				continue;
			}
			if (r + 2 >= length || buffer[r + 2] == '/' || buffer[r + 2] == '\n')
			{
				r += 2;
				continue;
			}
		}
		
		if (buffer[r] == '/' && w > 0 && buffer[w - 1] == '/')
		{
			++r;
			continue;
		}
		
		buffer[w] = buffer[r];
		++w;
		++r;
	}
	
	if (w == 0 && absolute)
	{
		buffer[0] = '/';
		w = 1;
	}
	
	buffer[w] = '\0';
	return w;
}
