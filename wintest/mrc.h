#ifndef MRC_H
#define MRC_H

typedef unsigned int crc32;

extern crc32 &mrc(crc32 &crc, const void *_data, unsigned short len, bool casesensitive = true, bool untilnull = false);


#endif