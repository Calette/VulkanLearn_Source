#include "Guid.h"

namespace Palette
{
	static uint64_t xorshift128plus(uint64_t* s)
	{
		/* http://xorshift.di.unimi.it/xorshift128plus.c */
		uint64_t s1 = s[0];
		const uint64_t s0 = s[1];
		s[0] = s0;
		s1 ^= s1 << 23;
		s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
		return s[1] + s0;
	}

	int uuid4_init(void)
	{
		int res;
		HCRYPTPROV hCryptProv;
		res = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
		if (!res) 
		{
			return UUID4_EFAILURE;
		}

		res = CryptGenRandom(hCryptProv, (DWORD)sizeof(seed), (PBYTE)seed);
		CryptReleaseContext(hCryptProv, 0);
		if (res != sizeof(seed)) 
		{
			return UUID4_EFAILURE;
		}

		return UUID4_ESUCCESS;
	}

	void uuid4_generate(char* dst)
	{
		static const char* temp = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
		static const char* chars = "0123456789abcdef";
		union { unsigned char b[16]; uint64_t word[2]; } s;
		const char* p;
		int i, n;
		/* get random */
		s.word[0] = xorshift128plus(seed);
		s.word[1] = xorshift128plus(seed);
		/* build string */
		p = temp;
		i = 0;
		while (*p) {
			n = s.b[i >> 1];
			n = (i & 1) ? (n >> 4) : (n & 0xf);
			switch (*p) {
			case 'x': *dst = chars[n];              i++;  break;
			case 'y': *dst = chars[(n & 0x3) + 8];  i++;  break;
			default: *dst = *p;
			}
			dst++, p++;
		}
		*dst = '\0';
	}
}