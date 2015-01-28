// Copyright (c) 2013-2014 The Pebblecoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <inttypes.h>

#include "hash-ops.h"
#include "threadpool.h"

void print_uint64(uint64_t u)
{
  unsigned char *c = (unsigned char *)&u;
  printf("0x");
  for (int i=0; i < sizeof(u); i++)
  {
    printf("%02x", c[i]);
  }
  printf(" ");
}

void pc_boulderhash_init(const void *data, size_t length,
                         uint64_t **state, uint64_t *result, uint64_t *extra)
{
  char hash[HASH_SIZE];
  
  // init into hash
  cn_fast_hash(data, length, hash);
  
  // init each state
  for (int i=0; i < BOULDERHASH_STATES; i++) {
    cn_fast_hash(hash, HASH_SIZE, hash);
    memcpy(&state[i][0], hash, sizeof(uint64_t));
    /*printf("state[%d][0] = ", i);
    print_uint64(state[i][0]);
    printf("\n");*/
  }
  
  // init result
  cn_fast_hash(hash, HASH_SIZE, hash);
  memcpy(result, hash, HASH_SIZE);
  
  // init extra
  cn_fast_hash(hash, HASH_SIZE, hash);
  memcpy(extra, hash, sizeof(uint64_t));
}

inline uint64_t boulderhash_transform(uint64_t val)
{
	return UINT64_C(0x5851f42d4c957f2d) * val + UINT64_C(0x14057b7ef767814f);
}

inline uint32_t lookback_index(uint64_t val, uint32_t j)
{
  if (j < 5) return 0;
  return (val >> 32) % ((j - 1) / 4) + (j - 1) * 3 / 4;
}

void pc_boulderhash_fill_state(uint64_t *cur_state)
{
  cur_state[1] = boulderhash_transform(cur_state[0]);
  
  for (uint32_t j=2; j < BOULDERHASH_STATE_SIZE; j++)
  {
    cur_state[j] = boulderhash_transform(cur_state[j-1]);
    cur_state[j] ^= cur_state[lookback_index(cur_state[j], j)];
  }
}

void pc_boulderhash_calc_result(uint64_t *result, uint64_t extra, uint64_t **state)
{
  static const int result_size = HASH_SIZE / sizeof(uint64_t);
  static const int result_size_m1 = result_size - 1;
  static const int states_m1 = BOULDERHASH_STATES - 1;
  static const int state_size_m1 = BOULDERHASH_STATE_SIZE - 1;
  
  // gen result
  for (int k=0, c=0; k < BOULDERHASH_ITERATIONS; k++, c=(c+1)&result_size_m1) {
    result[c] = extra ^ state[(result[c]>>32) & states_m1][result[c] & state_size_m1];
    extra = boulderhash_transform(extra);
  }
}

boulderhash_ctx *create_boulderhash_ctx(threadpool_t *pool)
{
  boulderhash_ctx *ctx = (boulderhash_ctx *)malloc(sizeof(boulderhash_ctx));
  
  for (int i=0; i < BOULDERHASH_STATES; i++) {
    ctx->state[i] = (uint64_t *)malloc(sizeof(uint64_t)*BOULDERHASH_STATE_SIZE);
  }
  
  ctx->pool = pool;
  
  pthread_cond_init(&ctx->cond, NULL);
  pthread_mutex_init(&ctx->cond_mutex, NULL);
  
  ctx->done_count = 0;
  
  return ctx;
}

void destroy_boulderhash_ctx(boulderhash_ctx *ctx)
{
  pthread_mutex_destroy(&ctx->cond_mutex);
  pthread_cond_destroy(&ctx->cond);
  ctx->pool = NULL;
  for (int i=0; i < BOULDERHASH_STATES; i++) {
    if (ctx->state[i])
      free(ctx->state[i]);
    
    ctx->state[i] = NULL;
  }
}

typedef struct {
  boulderhash_ctx* ctx;
  int pad_i;
} pool_thread_arg;

void do_fill_state(void *arg)
{
  pool_thread_arg *ptarg = (pool_thread_arg *)arg;
  boulderhash_ctx *ctx = ptarg->ctx;
  
  /*printf("Filling state %d (start=", ptarg->pad_i);
  print_uint64(ctx->state[ptarg->pad_i][0]);
  printf(")\n");*/
  pc_boulderhash_fill_state(ctx->state[ptarg->pad_i]);
  
  pthread_mutex_lock(&ctx->cond_mutex);
  ctx->done_count++;
  //printf("Done count --> %d\n", (int)ctx->done_count);
  if (ctx->done_count >= BOULDERHASH_STATES)
  {
    pthread_cond_signal(&ctx->cond);
  }
  pthread_mutex_unlock(&ctx->cond_mutex);
}

bool fill_all_states(boulderhash_ctx *ctx)
{
  ctx->done_count = 0;
  pool_thread_arg args[BOULDERHASH_STATES];
  
  for (int i=0; i < BOULDERHASH_STATES; i++) {
    args[i].ctx = ctx;
    args[i].pad_i = i;
    
    if (threadpool_add(ctx->pool, do_fill_state, &args[i], 0) < 0)
      return false;
  }
  
  pthread_mutex_lock(&ctx->cond_mutex);
  while (ctx->done_count < BOULDERHASH_STATES)
  {
    pthread_cond_wait(&ctx->cond, &ctx->cond_mutex);
  }
  pthread_mutex_unlock(&ctx->cond_mutex);
  
  return true;
}

bool pc_boulderhash(boulderhash_ctx *ctx,
                    const void *data, size_t length, char *hash) {
  uint64_t result[HASH_SIZE / sizeof(uint64_t)];
  uint64_t extra;
  
  pc_boulderhash_init(data, length, ctx->state, &result[0], &extra);
  
  if (!fill_all_states(ctx))
    return false;
  
  pc_boulderhash_calc_result(result, extra, ctx->state);
  
  // final hash
  cn_fast_hash(result, HASH_SIZE, hash);
  
  return true;
}
