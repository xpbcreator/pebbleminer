// Copyright (c) 2013-2014 The Pebblecoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// Modified for CPUminer by Lucas Jones

#include "cpuminer-config.h"
#include "miner.h"

#include "crypto/hash-ops.h"

int scanhash_boulderhash(int thr_id, uint32_t *pdata, const uint32_t *ptarget,
                         uint32_t max_nonce, uint64_t *hashes_done,
                         boulderhash_ctx *ctx, char *result_hash) {
	uint32_t *nonceptr = (uint32_t*) (((char*)pdata) + 39);
	uint32_t n = *nonceptr - 1;
	const uint32_t first_nonce = n + 1;
	const uint32_t Htarg = ptarget[7];
	uint32_t hash[HASH_SIZE / 4] __attribute__((aligned(32)));

  do {
    *nonceptr = ++n;
    if (unlikely(!pc_boulderhash(ctx, pdata, 76, (char *)hash)))
    {
      printf("Failed to boulderhash\n");
      *hashes_done = n - first_nonce + 1;      
      return 0;
    }
    if (unlikely(hash[7] < ptarget[7])) {
      *hashes_done = n - first_nonce + 1;
      memcpy(result_hash, hash, sizeof(hash));
      return true;
    }
  } while (likely((n <= max_nonce && !work_restart[thr_id].restart)));

	*hashes_done = n - first_nonce + 1;
	return 0;
}
