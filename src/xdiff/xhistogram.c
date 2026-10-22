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
		unsigned số_nguyên ptr, cnt;
		struct record *next;
	} **records, /* an occurrence */
	  **line_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned số_nguyên *next_ptrs;
	unsigned số_nguyên table_bits,
		     records_size,
		     line_map_size;

	unsigned số_nguyên max_chain_length,
		     key_shift,
		     ptr_shift;

	unsigned số_nguyên cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t const *xpp;
};

struct region {
	unsigned số_nguyên begin1, end1;
	unsigned số_nguyên begin2, end2;
};

#define LINE_MAP(i, a) (i->line_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((LINE_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

static số_nguyên cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	trả_về r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, line) \
	XDL_HASHLONG((REC(index->env, side, line))->ha, index->table_bits)

static số_nguyên scanA(struct histindex *index, số_nguyên line1, số_nguyên count1)
{
	unsigned số_nguyên ptr, tbl_idx;
	unsigned số_nguyên chain_len;
	struct record **rec_chain, *rec;

	cho (ptr = LINE_END(1); (unsigned số_nguyên)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		trong_khi (rec) {
			nếu (CMP(index, 1, rec->ptr, 1, ptr)) {
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

		nếu (chain_len == index->max_chain_length)
			trả_về -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		nếu (!(rec = xdl_cha_alloc(&index->rcha)))
			trả_về -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		LINE_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	trả_về 0;
}

static số_nguyên try_lcs(struct histindex *index, struct region *lcs, số_nguyên b_ptr,
	số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2)
{
	unsigned số_nguyên b_next = b_ptr + 1;
	struct record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned số_nguyên as, ae, bs, be, np, rc;
	số_nguyên should_break;

	cho (; rec; rec = rec->next) {
		nếu (rec->cnt > index->cnt) {
			nếu (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			continue;
		}

		as = rec->ptr;
		nếu (!CMP(index, 1, as, 2, b_ptr))
			continue;

		index->has_common = 1;
		cho (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			trong_khi ((unsigned số_nguyên)line1 < as && (unsigned số_nguyên)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				nếu (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			trong_khi (ae < (unsigned số_nguyên)LINE_END(1) && be < (unsigned số_nguyên)LINE_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				nếu (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			nếu (b_next <= be)
				b_next = be + 1;
			nếu (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			nếu (np == 0)
				break;

			trong_khi (np <= ae) {
				np = NEXT_PTR(index, np);
				nếu (np == 0) {
					should_break = 1;
					break;
				}
			}

			nếu (should_break)
				break;

			as = np;
		}
	}
	trả_về b_next;
}

static số_nguyên fall_back_to_classic_diff(xpparam_t const *xpp, xdfenv_t *env,
		số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, sizeof(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	trả_về xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

static inline void free_index(struct histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->line_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

static số_nguyên find_lcs(xpparam_t const *xpp, xdfenv_t *env,
		    struct region *lcs,
		    số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2)
{
	số_nguyên b_ptr;
	số_nguyên ret = -1;
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
	nếu (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		goto cleanup;

	index.line_map_size = count1;
	nếu (!XDL_CALLOC_ARRAY(index.line_map, index.line_map_size))
		goto cleanup;

	nếu (!XDL_CALLOC_ARRAY(index.next_ptrs, index.line_map_size))
		goto cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	nếu (xdl_cha_init(&index.rcha, sizeof(struct record), count1 / 4 + 1) < 0)
		goto cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	nếu (scanA(&index, line1, count1))
		goto cleanup;

	index.cnt = index.max_chain_length + 1;

	cho (b_ptr = line2; b_ptr <= LINE_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	nếu (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	không_thì
		ret = 0;

cleanup:
	free_index(&index);
	trả_về ret;
}

static số_nguyên histogram_diff(xpparam_t const *xpp, xdfenv_t *env,
	số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2)
{
	struct region lcs;
	số_nguyên lcs_found;
	số_nguyên result;
redo:
	result = -1;

	nếu (count1 <= 0 && count2 <= 0)
		trả_về 0;

	nếu ((unsigned số_nguyên)LINE_END(1) >= MAX_PTR)
		trả_về -1;

	nếu (!count1) {
		trong_khi(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		trả_về 0;
	} không_thì nếu (!count2) {
		trong_khi(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		trả_về 0;
	}

	memset(&lcs, 0, sizeof(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	nếu (lcs_found < 0)
		goto out;
	không_thì nếu (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	không_thì {
		nếu (lcs.begin1 == 0 && lcs.begin2 == 0) {
			trong_khi (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			trong_khi (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} không_thì {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			nếu (result)
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
	trả_về result;
}

số_nguyên xdl_do_histogram_diff(xpparam_t const *xpp, xdfenv_t *env)
{
	trả_về histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
