#pragma once
#include <string>

typedef struct {
	unsigned int state[4];                                   /* state (ABCD) */
	unsigned int count[2];        /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

class CMd5
{
private:
	CMd5() {};
	~CMd5() {};

public:
	static std::string md5(std::string&);
};

