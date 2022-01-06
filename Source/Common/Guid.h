#pragma once
#include <string>

#include <stdio.h>
#include <stdint.h>

#include <windows.h>

//32char uuid + 4 hyphens + 1EOS
#define UUID_LEN 37
namespace Palette
{
	enum
	{
		UUID4_ESUCCESS = 0,
		UUID4_EFAILURE = -1
	};

	static uint64_t seed[2];

	static uint64_t xorshift128plus(uint64_t* s);

	int uuid4_init(void);

	void uuid4_generate(char* dst);
}