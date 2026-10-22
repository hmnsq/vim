/*
 * Copyright (C) 2010, Google Inc.
 * and other copyright owners as documented in JGit's IP log.
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Distribution License v1.0 which
 * accompanies this distribution, is reproduced below, and is
 * available at http://www.eclipse.org/org/documents/edl-v10.php
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 *
 * - Neither the name of the Eclipse Foundation, Inc. nor the
 *   names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior
 *   written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "xinclude.h"

#define MAX_PTR	UINT_MAX
#define MAX_CNT	UINT_MAX

#define LINE_END(n) (line##n + count##n - 1)
#define LINE_END_PTR(n) (*line##n + *count##n - 1)

struct histindex {
	struct record {
		unsigned 정수 ptr, cnt;
		struct record *next;
	} **records, /* an occurrence */
	  **line_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned 정수 *next_ptrs;
	unsigned 정수 table_bits,
		     records_size,
		     line_map_size;

	unsigned 정수 max_chain_length,
		     key_shift,
		     ptr_shift;

	unsigned 정수 cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t const *xpp;
};

struct region {
	unsigned 정수 begin1, end1;
	unsigned 정수 begin2, end2;
};

#define LINE_MAP(i, a) (i->line_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((LINE_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

static 정수 cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	반환 r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, line) \
	XDL_HASHLONG((REC(index->env, side, line))->ha, index->table_bits)

static 정수 scanA(struct histindex *index, 정수 line1, 정수 count1)
{
	unsigned 정수 ptr, tbl_idx;
	unsigned 정수 chain_len;
	struct record **rec_chain, *rec;

	위해 (ptr = LINE_END(1); (unsigned 정수)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		동안 (rec) {
			만약 (CMP(index, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				NEXT_PTR(index, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				LINE_MAP(index, ptr) = rec;
				goto continue_scan;
			}

			rec = rec->next;
			chain_len++;
		}

		만약 (chain_len == index->max_chain_length)
			반환 -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		만약 (!(rec = xdl_cha_alloc(&index->rcha)))
			반환 -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		LINE_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	반환 0;
}

static 정수 try_lcs(struct histindex *index, struct region *lcs, 정수 b_ptr,
	정수 line1, 정수 count1, 정수 line2, 정수 count2)
{
	unsigned 정수 b_next = b_ptr + 1;
	struct record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned 정수 as, ae, bs, be, np, rc;
	정수 should_break;

	위해 (; rec; rec = rec->next) {
		만약 (rec->cnt > index->cnt) {
			만약 (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			continue;
		}

		as = rec->ptr;
		만약 (!CMP(index, 1, as, 2, b_ptr))
			continue;

		index->has_common = 1;
		위해 (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			동안 ((unsigned 정수)line1 < as && (unsigned 정수)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				만약 (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			동안 (ae < (unsigned 정수)LINE_END(1) && be < (unsigned 정수)LINE_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				만약 (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			만약 (b_next <= be)
				b_next = be + 1;
			만약 (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			만약 (np == 0)
				break;

			동안 (np <= ae) {
				np = NEXT_PTR(index, np);
				만약 (np == 0) {
					should_break = 1;
					break;
				}
			}

			만약 (should_break)
				break;

			as = np;
		}
	}
	반환 b_next;
}

static 정수 fall_back_to_classic_diff(xpparam_t const *xpp, xdfenv_t *env,
		정수 line1, 정수 count1, 정수 line2, 정수 count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, sizeof(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	반환 xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

static inline void free_index(struct histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->line_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

static 정수 find_lcs(xpparam_t const *xpp, xdfenv_t *env,
		    struct region *lcs,
		    정수 line1, 정수 count1, 정수 line2, 정수 count2)
{
	정수 b_ptr;
	정수 ret = -1;
	struct histindex index;

	memset(&index, 0, sizeof(index));

	index.env = env;
	index.xpp = xpp;

	index.records = NULL;
	index.line_map = NULL;
	/* in case of early xdl_cha_free() */
	index.rcha.head = NULL;

	index.table_bits = xdl_hashbits(count1);
	index.records_size = 1 << index.table_bits;
	만약 (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		goto cleanup;

	index.line_map_size = count1;
	만약 (!XDL_CALLOC_ARRAY(index.line_map, index.line_map_size))
		goto cleanup;

	만약 (!XDL_CALLOC_ARRAY(index.next_ptrs, index.line_map_size))
		goto cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	만약 (xdl_cha_init(&index.rcha, sizeof(struct record), count1 / 4 + 1) < 0)
		goto cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	만약 (scanA(&index, line1, count1))
		goto cleanup;

	index.cnt = index.max_chain_length + 1;

	위해 (b_ptr = line2; b_ptr <= LINE_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	만약 (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	아니면
		ret = 0;

cleanup:
	free_index(&index);
	반환 ret;
}

static 정수 histogram_diff(xpparam_t const *xpp, xdfenv_t *env,
	정수 line1, 정수 count1, 정수 line2, 정수 count2)
{
	struct region lcs;
	정수 lcs_found;
	정수 result;
redo:
	result = -1;

	만약 (count1 <= 0 && count2 <= 0)
		반환 0;

	만약 ((unsigned 정수)LINE_END(1) >= MAX_PTR)
		반환 -1;

	만약 (!count1) {
		동안(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		반환 0;
	} 아니면 만약 (!count2) {
		동안(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		반환 0;
	}

	memset(&lcs, 0, sizeof(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	만약 (lcs_found < 0)
		goto out;
	아니면 만약 (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	아니면 {
		만약 (lcs.begin1 == 0 && lcs.begin2 == 0) {
			동안 (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			동안 (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} 아니면 {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			만약 (result)
				goto out;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = LINE_END(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = LINE_END(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			goto redo;
		}
	}
out:
	반환 result;
}

정수 xdl_do_histogram_diff(xpparam_t const *xpp, xdfenv_t *env)
{
	반환 histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
