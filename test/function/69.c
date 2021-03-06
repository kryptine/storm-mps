/* 
TEST_HEADER
 id = $Id$
 summary = request.dylan.170563 (colour invariant and finalization)
 language = c
 link = testlib.o rankfmt.o
END_HEADER
*/

#include "testlib.h"
#include "mpscamc.h"
#include "mpsavm.h"
#include "rankfmt.h"


#define genCOUNT (3)

static mps_gen_param_s testChain[genCOUNT] = {
  { 6000, 0.90 }, { 8000, 0.65 }, { 16000, 0.50 } };


mps_arena_t arena;


static void test(void *stack_pointer)
{
 mps_pool_t pool;
 mps_root_t rootA, rootB;

 mps_fmt_t format;
 mps_chain_t chain;
 mps_addr_t ref;
 mps_message_t message;
 mps_ap_t ap;

 mycell *a, *b;
 tag myTag;

 long int j;

 cdie(mps_arena_create(&arena, mps_arena_class_vm(), mmqaArenaSIZE),
      "create arena");
 mps_arena_park(arena);

 die(mps_root_create_area(&rootA, arena, mps_rank_ambig(), (mps_rm_t)0,
                          &a, &a + 1, mps_scan_area, NULL), "create root");
 die(mps_root_create_area(&rootB, arena, mps_rank_ambig(), (mps_rm_t)0,
                          &b, &b + 1, mps_scan_area, NULL), "create root");

 die(mps_fmt_create_A(&format, arena, &fmtA), "create format");
 cdie(mps_chain_create(&chain, arena, genCOUNT, testChain), "chain_create");

 die(mmqa_pool_create_chain(&pool, arena, mps_class_amc(), format, chain),
     "create pool");

 cdie(
  mps_ap_create(&ap, pool, mps_rank_exact()),
  "create ap");

 mps_message_type_enable(arena, mps_message_type_finalization());

 b = allocone(ap, 400, 1);

 for (j=0; j<1000; j++) {
  a = allocone(ap, 400, 1);
  setref(a, 0, b);
  b = a;
 }

 a = allocone(ap, 2, 1);
 mps_finalize(arena, (mps_addr_t*)&a);
 myTag = a->tag;

 a = allocone(ap, 4000, 1);
 mps_arena_collect(arena);

 if (!mps_message_get(&message, arena, mps_message_type_finalization())) {
  error("No message on queue!");
 }

 for (j=0; j<50; j++) {
  comment("%d of 50", j);
  a = allocdumb(ap, 1024*1024*10, 1);
  mps_message_finalization_ref(&ref, arena, message);
  mps_arena_park(arena);
  a = ref;
  comment("                   %p", a);
  asserts(a->tag == myTag, "Bad reference!");
  a = NULL;
  ref = NULL;
  mps_arena_release(arena);
 }

 mps_message_discard(arena, message);

 mps_arena_park(arena);
 mps_root_destroy(rootA);
 mps_root_destroy(rootB);
 mps_ap_destroy(ap);
 mps_pool_destroy(pool);
 mps_chain_destroy(chain);
 mps_fmt_destroy(format);
 mps_arena_destroy(arena);
 comment("Destroyed arena.");
}


int main(void)
{
 run_test(test);
 pass();
 return 0;
}
