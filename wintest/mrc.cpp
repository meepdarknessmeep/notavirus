#include "mrc.h"
#include <nmmintrin.h>

inline char lower(char input)
{
	if (input >= 'A' && input <= 'Z')
		input += 32;
	return input;
}

__declspec(noinline) crc32 &mrc(crc32 &crc, const void *_data, unsigned short len, bool casesensitive, bool untilnull)
{
	crc32 output = -1;
	unsigned char *data = (unsigned char *)_data;

	while (untilnull ? *data : len--)
	{
		unsigned char c = *data++;
		if (!casesensitive)
			c = lower(c);

		output = _mm_crc32_u8(output, c);
	}
	crc = output;
	return crc;
}