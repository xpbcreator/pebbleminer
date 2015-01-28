// Copyright (c) 2012-2013 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#if !defined(__cplusplus)

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <pthread.h>

#include "int-util.h"
#include "threadpool.h"

static inline void *padd(void *p, size_t i) {
  return (char *) p + i;
}

static inline const void *cpadd(const void *p, size_t i) {
  return (const char *) p + i;
}

static inline void place_length(uint8_t *buffer, size_t bufsize, size_t length) {
  if (sizeof(size_t) == 4) {
    *(uint32_t *) padd(buffer, bufsize - 4) = swap32be(length);
  } else {
    *(uint64_t *) padd(buffer, bufsize - 8) = swap64be(length);
  }
}

#pragma pack(push, 1)
union hash_state {
  uint8_t b[200];
  uint64_t w[25];
};
#pragma pack(pop)

void hash_permutation(union hash_state *state);
void hash_process(union hash_state *state, const uint8_t *buf, size_t count);

#endif

enum {
  HASH_SIZE = 32,
  HASH_DATA_AREA = 136
};

void cn_fast_hash(const void *data, size_t length, char *hash);

#define BOULDERHASH_STATES 65
#define BOULDERHASH_STATE_SIZE 26738688 // 204 MiB * 65 = ~12.9 GiB
#define BOULDERHASH_ITERATIONS 42598400

typedef struct {
  uint64_t *state[BOULDERHASH_STATES];
  threadpool_t *pool;
  pthread_cond_t cond;
  pthread_mutex_t cond_mutex;
  uint64_t done_count;
} boulderhash_ctx;

boulderhash_ctx *create_boulderhash_ctx(threadpool_t *pool);
void destroy_boulderhash_ctx(boulderhash_ctx *ctx);
bool pc_boulderhash(boulderhash_ctx *ctx,
                    const void *data, size_t length, char *hash);

