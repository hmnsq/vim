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
		unsigned цео ptr, cnt;
		struct record *next;
	} **records, /* an occurrence */
	  **line_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned цео *next_ptrs;
	unsigned цео table_bits,
		     records_size,
		     line_map_size;

	unsigned цео max_chain_length,
		     key_shift,
		     ptr_shift;

	unsigned цео cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t const *xpp;
};

struct region {
	unsigned цео begin1, end1;
	unsigned цео begin2, end2;
};

#define LINE_MAP(i, a) (i->line_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((LINE_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

static цео cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	врати r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, line) \
	XDL_HASHLONG((REC(index->env, side, line))->ha, index->table_bits)

static цео scanA(struct histindex *index, цео line1, цео count1)
{
	unsigned цео ptr, tbl_idx;
	unsigned цео chain_len;
	struct record **rec_chain, *rec;

	за (ptr = LINE_END(1); (unsigned цео)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		док (rec) {
			ако (CMP(index, 1, rec->ptr, 1, ptr)) {
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

		ако (chain_len == index->max_chain_length)
			врати -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		ако (!(rec = xdl_cha_alloc(&index->rcha)))
			врати -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		LINE_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	врати 0;
}

static цео try_lcs(struct histindex *index, struct region *lcs, цео b_ptr,
	цео line1, цео count1, цео line2, цео count2)
{
	unsigned цео b_next = b_ptr + 1;
	struct record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned цео as, ae, bs, be, np, rc;
	цео should_break;

	за (; rec; rec = rec->next) {
		ако (rec->cnt > index->cnt) {
			ако (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			continue;
		}

		as = rec->ptr;
		ако (!CMP(index, 1, as, 2, b_ptr))
			continue;

		index->has_common = 1;
		за (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			док ((unsigned цео)line1 < as && (unsigned цео)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				ако (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			док (ae < (unsigned цео)LINE_END(1) && be < (unsigned цео)LINE_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				ако (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			ако (b_next <= be)
				b_next = be + 1;
			ако (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			ако (np == 0)
				break;

			док (np <= ae) {
				np = NEXT_PTR(index, np);
				ако (np == 0) {
					should_break = 1;
					break;
				}
			}

			ако (should_break)
				break;

			as = np;
		}
	}
	врати b_next;
}

static цео fall_back_to_classic_diff(xpparam_t const *xpp, xdfenv_t *env,
		цео line1, цео count1, цео line2, цео count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, sizeof(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	врати xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

static inline void free_index(struct histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->line_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

static цео find_lcs(xpparam_t const *xpp, xdfenv_t *env,
		    struct region *lcs,
		    цео line1, цео count1, цео line2, цео count2)
{
	цео b_ptr;
	цео ret = -1;
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
	ако (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		goto cleanup;

	index.line_map_size = count1;
	ако (!XDL_CALLOC_ARRAY(index.line_map, index.line_map_size))
		goto cleanup;

	ако (!XDL_CALLOC_ARRAY(index.next_ptrs, index.line_map_size))
		goto cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	ако (xdl_cha_init(&index.rcha, sizeof(struct record), count1 / 4 + 1) < 0)
		goto cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	ако (scanA(&index, line1, count1))
		goto cleanup;

	index.cnt = index.max_chain_length + 1;

	за (b_ptr = line2; b_ptr <= LINE_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	ако (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	инако
		ret = 0;

cleanup:
	free_index(&index);
	врати ret;
}

static цео histogram_diff(xpparam_t const *xpp, xdfenv_t *env,
	цео line1, цео count1, цео line2, цео count2)
{
	struct region lcs;
	цео lcs_found;
	цео result;
redo:
	result = -1;

	ако (count1 <= 0 && count2 <= 0)
		врати 0;

	ако ((unsigned цео)LINE_END(1) >= MAX_PTR)
		врати -1;

	ако (!count1) {
		док(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		врати 0;
	} инако ако (!count2) {
		док(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		врати 0;
	}

	memset(&lcs, 0, sizeof(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	ако (lcs_found < 0)
		goto out;
	инако ако (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	инако {
		ако (lcs.begin1 == 0 && lcs.begin2 == 0) {
			док (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			док (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} инако {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			ако (result)
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
	врати result;
}

цео xdl_do_histogram_diff(xpparam_t const *xpp, xdfenv_t *env)
{
	врати histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
