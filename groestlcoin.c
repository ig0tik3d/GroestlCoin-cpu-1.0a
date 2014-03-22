#include "cpuminer-config.h"
#include "miner.h"

#include <string.h>
#include <stdint.h>
#include "groestl-asm/grso.c"
#include "groestl-asm/grso-asm.c"


#if defined(__GNUC__)
      #define DATA_ALIGN16(x) x __attribute__ ((aligned(16)))
#else
      #define DATA_ALIGN16(x) __declspec(align(16)) x
#endif


static void GroestlCoinHash(void *state, const void *input,int mode)
{
	uint32_t hash1[16];		
	DATA_ALIGN16(unsigned char hashbuf[128]); 
	DATA_ALIGN16(unsigned char hash[128]);
    memset(hash, 0, 128);
	
	grsoState sts_grs,sts_grs1;
	
	GRS_I;
	GRS_U;
	GRS_C;
	
	GRS_I_64;
	GRS_U_64;
	GRS_C_64;
	
	asm volatile ("emms");
	
    memcpy(state, hash1, 32);	
}

int scanhash_GroestlCoin(int thr_id, uint32_t *pdata, const uint32_t *ptarget,
	uint32_t max_nonce, unsigned long *hashes_done,int alg)
{
	uint32_t n = pdata[19] - 1;
	const uint32_t first_nonce = pdata[19];
	const uint32_t Htarg = ptarget[7];

	uint32_t hash64[8] __attribute__((aligned(32)));
	uint32_t endiandata[32];
	
	//char testdata[] = {"\x70\x00\x00\x00\x5d\x38\x5b\xa1\x14\xd0\x79\x97\x0b\x29\xa9\x41\x8f\xd0\x54\x9e\x7d\x68\xa9\x5c\x7f\x16\x86\x21\xa3\x14\x20\x10\x00\x00\x00\x00\x57\x85\x86\xd1\x49\xfd\x07\xb2\x2f\x3a\x8a\x34\x7c\x51\x6d\xe7\x05\x2f\x03\x4d\x2b\x76\xff\x68\xe0\xd6\xec\xff\x9b\x77\xa4\x54\x89\xe3\xfd\x51\x17\x32\x01\x1d\xf0\x73\x10\x00"};
	
	//we need bigendian data...
	//lessons learned: do NOT endianchange directly in pdata, this will all proof-of-works be considered as stale from minerd.... 
	int kk=0;
	for (; kk < 32; kk++)
	{
		be32enc(&endiandata[kk], ((uint32_t*)pdata)[kk]);
	};

//	if (opt_debug) 
//	{
//		applog(LOG_DEBUG, "Thr: %02d, firstN: %08x, maxN: %08x, ToDo: %d", thr_id, first_nonce, max_nonce, max_nonce-first_nonce);
//	}
	
	/* I'm to lazy to put the loop in an inline function... so dirty copy'n'paste.... */
	/* i know that i could set a variable, but i don't know how the compiler will optimize it, not that then the cpu needs to load the value *everytime* in a register */
	if (ptarget[7]==0) {
		do {
			pdata[19] = ++n;
			be32enc(&endiandata[19], n); 
			GroestlCoinHash(hash64, &endiandata,alg);
			if (((hash64[7]&0xFFFFFFFF)==0) && 
					fulltest(hash64, ptarget)) {
				*hashes_done = n - first_nonce + 1;
				return true;
			}
		} while (n < max_nonce && !work_restart[thr_id].restart);	
	} 
	else if (ptarget[7]<=0xF) 
	{
		do {
			pdata[19] = ++n;
			be32enc(&endiandata[19], n); 
			GroestlCoinHash(hash64, &endiandata,alg);
			if (((hash64[7]&0xFFFFFFF0)==0) && 
					fulltest(hash64, ptarget)) {
				*hashes_done = n - first_nonce + 1;
				return true;
			}
		} while (n < max_nonce && !work_restart[thr_id].restart);	
	} 
	else if (ptarget[7]<=0xFF) 
	{
		do {
			pdata[19] = ++n;
			be32enc(&endiandata[19], n); 
			GroestlCoinHash(hash64, &endiandata,alg);
			if (((hash64[7]&0xFFFFFF00)==0) && 
					fulltest(hash64, ptarget)) {
				*hashes_done = n - first_nonce + 1;
				return true;
			}
		} while (n < max_nonce && !work_restart[thr_id].restart);	
	} 
	else if (ptarget[7]<=0xFFF) 
	{
		do {
			pdata[19] = ++n;
			be32enc(&endiandata[19], n); 
			GroestlCoinHash(hash64, &endiandata,alg);
			if (((hash64[7]&0xFFFFF000)==0) && 
					fulltest(hash64, ptarget)) {
				*hashes_done = n - first_nonce + 1;
				return true;
			}
		} while (n < max_nonce && !work_restart[thr_id].restart);	

	} 
	else if (ptarget[7]<=0xFFFF) 
	{
		do {
			pdata[19] = ++n;
			be32enc(&endiandata[19], n); 
			GroestlCoinHash(hash64, &endiandata,alg);
			if (((hash64[7]&0xFFFF0000)==0) && 
					fulltest(hash64, ptarget)) {
				*hashes_done = n - first_nonce + 1;
				return true;
			}
		} while (n < max_nonce && !work_restart[thr_id].restart);	

	} 
	else 
	{
		do {
			pdata[19] = ++n;
			be32enc(&endiandata[19], n); 
			GroestlCoinHash(hash64, &endiandata,alg);
			if (fulltest(hash64, ptarget)) {
				*hashes_done = n - first_nonce + 1;
				return true;
			}
		} while (n < max_nonce && !work_restart[thr_id].restart);	
	}
	
	
	*hashes_done = n - first_nonce + 1;
	pdata[19] = n;
	return 0;
}
