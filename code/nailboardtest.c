/* nailboardtest.c: NAILBOARD TEST
 *
 * $Id$
 * Copyright (c) 2014-2020 Ravenbrook Limited.  See end of file for license.
 *
 */

#include "mpm.h"
#include "mps.h"
#include "mpsavm.h"
#include "testlib.h"
#include "bt.h"
#include "nailboard.h"

#include <stdio.h> /* printf */


static void test(mps_arena_t arena)
{
  BT bt;
  Nailboard board;
  Align align;
  Count nails;
  Addr base, limit;
  Index i, j, k;

  align = (Align)1 << (rnd() % 10);
  nails = (Count)1 << (rnd() % 16);
  nails += rnd() % nails;
  base = AddrAlignUp(0, align);
  limit = AddrAdd(base, nails * align);

  die(BTCreate(&bt, arena, nails), "BTCreate");
  BTResRange(bt, 0, nails);
  die(NailboardCreate(&board, arena, align, base, limit), "NailboardCreate");

  for (i = 0; i <= nails / 8; ++i) {
    Bool old;
    j = rnd() % nails;
    old = BTGet(bt, j);
    BTSet(bt, j);
    cdie(NailboardSet(board, AddrAdd(base, j * align)) == old, "NailboardSet");
    for (k = 0; k < nails / 8; ++k) {
      Index b, l;
      b = rnd() % nails;
      l = b + rnd() % (nails - b) + 1;
      cdie(BTIsResRange(bt, b, l)
           == NailboardIsResRange(board, AddrAdd(base, b * align),
                                  AddrAdd(base, l * align)),
           "NailboardIsResRange");
    }
  }

  die(NailboardDescribe(board, mps_lib_get_stdout(), 0), "NailboardDescribe");
}

int main(int argc, char *argv[])
{
  mps_arena_t arena;

  testlib_init(argc, argv);

  die(mps_arena_create(&arena, mps_arena_class_vm(), 1024 * 1024),
      "mps_arena_create");

  test(arena);

  mps_arena_destroy(arena);
  printf("%s: Conclusion: Failed to find any defects.\n", argv[0]);
  return 0;
}


/* C. COPYRIGHT AND LICENSE
 *
 * Copyright (C) 2014-2020 Ravenbrook Limited <http://www.ravenbrook.com/>.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

