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

sa_nracanaa histindex {
	sa_nracanaa record {
		unsigned පූර්ණ_සංඛ්‍යා ptr, cnt;
		sa_nracanaa record *next;
	} **records, /* an occurrence */
	  **line_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned පූර්ණ_සංඛ්‍යා *next_ptrs;
	unsigned පූර්ණ_සංඛ්‍යා table_bits,
		     records_size,
		     line_map_size;

	unsigned පූර්ණ_සංඛ්‍යා max_chain_length,
		     key_shift,
		     ptr_shift;

	unsigned පූර්ණ_සංඛ්‍යා cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t const *xpp;
};

sa_nracanaa region {
	unsigned පූර්ණ_සංඛ්‍යා begin1, end1;
	unsigned පූර්ණ_සංඛ්‍යා begin2, end2;
};

#define LINE_MAP(i, a) (i->line_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((LINE_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

static පූර්ණ_සංඛ්‍යා cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	ආපසු r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, line) \
	XDL_HASHLONG((REC(index->env, side, line))->ha, index->table_bits)

static පූර්ණ_සංඛ්‍යා scanA(sa_nracanaa histindex *index, පූර්ණ_සංඛ්‍යා line1, පූර්ණ_සංඛ්‍යා count1)
{
	unsigned පූර්ණ_සංඛ්‍යා ptr, tbl_idx;
	unsigned පූර්ණ_සංඛ්‍යා chain_len;
	sa_nracanaa record **rec_chain, *rec;

	සඳහා (ptr = LINE_END(1); (unsigned පූර්ණ_සංඛ්‍යා)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		අතරතුර (rec) {
			නම් (CMP(index, 1, rec->ptr, 1, ptr)) {
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

		නම් (chain_len == index->max_chain_length)
			ආපසු -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		නම් (!(rec = xdl_cha_alloc(&index->rcha)))
			ආපසු -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		LINE_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා try_lcs(sa_nracanaa histindex *index, sa_nracanaa region *lcs, පූර්ණ_සංඛ්‍යා b_ptr,
	පූර්ණ_සංඛ්‍යා line1, පූර්ණ_සංඛ්‍යා count1, පූර්ණ_සංඛ්‍යා line2, පූර්ණ_සංඛ්‍යා count2)
{
	unsigned පූර්ණ_සංඛ්‍යා b_next = b_ptr + 1;
	sa_nracanaa record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned පූර්ණ_සංඛ්‍යා as, ae, bs, be, np, rc;
	පූර්ණ_සංඛ්‍යා should_break;

	සඳහා (; rec; rec = rec->next) {
		නම් (rec->cnt > index->cnt) {
			නම් (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			continue;
		}

		as = rec->ptr;
		නම් (!CMP(index, 1, as, 2, b_ptr))
			continue;

		index->has_common = 1;
		සඳහා (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			අතරතුර ((unsigned පූර්ණ_සංඛ්‍යා)line1 < as && (unsigned පූර්ණ_සංඛ්‍යා)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				නම් (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			අතරතුර (ae < (unsigned පූර්ණ_සංඛ්‍යා)LINE_END(1) && be < (unsigned පූර්ණ_සංඛ්‍යා)LINE_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				නම් (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			නම් (b_next <= be)
				b_next = be + 1;
			නම් (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			නම් (np == 0)
				bha_nga;

			අතරතුර (np <= ae) {
				np = NEXT_PTR(index, np);
				නම් (np == 0) {
					should_break = 1;
					bha_nga;
				}
			}

			නම් (should_break)
				bha_nga;

			as = np;
		}
	}
	ආපසු b_next;
}

static පූර්ණ_සංඛ්‍යා fall_back_to_classic_diff(xpparam_t const *xpp, xdfenv_t *env,
		පූර්ණ_සංඛ්‍යා line1, පූර්ණ_සංඛ්‍යා count1, පූර්ණ_සංඛ්‍යා line2, පූර්ණ_සංඛ්‍යා count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, parimaa_na(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ආපසු xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

static anta_hstha void free_index(sa_nracanaa histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->line_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

static පූර්ණ_සංඛ්‍යා find_lcs(xpparam_t const *xpp, xdfenv_t *env,
		    sa_nracanaa region *lcs,
		    පූර්ණ_සංඛ්‍යා line1, පූර්ණ_සංඛ්‍යා count1, පූර්ණ_සංඛ්‍යා line2, පූර්ණ_සංඛ්‍යා count2)
{
	පූර්ණ_සංඛ්‍යා b_ptr;
	පූර්ණ_සංඛ්‍යා ret = -1;
	sa_nracanaa histindex index;

	memset(&index, 0, parimaa_na(index));

	index.env = env;
	index.xpp = xpp;

	index.records = NULL;
	index.line_map = NULL;
	/* in case of early xdl_cha_free() */
	index.rcha.head = NULL;

	index.table_bits = xdl_hashbits(count1);
	index.records_size = 1 << index.table_bits;
	නම් (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		goto cleanup;

	index.line_map_size = count1;
	නම් (!XDL_CALLOC_ARRAY(index.line_map, index.line_map_size))
		goto cleanup;

	නම් (!XDL_CALLOC_ARRAY(index.next_ptrs, index.line_map_size))
		goto cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	නම් (xdl_cha_init(&index.rcha, parimaa_na(sa_nracanaa record), count1 / 4 + 1) < 0)
		goto cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	නම් (scanA(&index, line1, count1))
		goto cleanup;

	index.cnt = index.max_chain_length + 1;

	සඳහා (b_ptr = line2; b_ptr <= LINE_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	නම් (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	නැතහොත්
		ret = 0;

cleanup:
	free_index(&index);
	ආපසු ret;
}

static පූර්ණ_සංඛ්‍යා histogram_diff(xpparam_t const *xpp, xdfenv_t *env,
	පූර්ණ_සංඛ්‍යා line1, පූර්ණ_සංඛ්‍යා count1, පූර්ණ_සංඛ්‍යා line2, පූර්ණ_සංඛ්‍යා count2)
{
	sa_nracanaa region lcs;
	පූර්ණ_සංඛ්‍යා lcs_found;
	පූර්ණ_සංඛ්‍යා result;
redo:
	result = -1;

	නම් (count1 <= 0 && count2 <= 0)
		ආපසු 0;

	නම් ((unsigned පූර්ණ_සංඛ්‍යා)LINE_END(1) >= MAX_PTR)
		ආපසු -1;

	නම් (!count1) {
		අතරතුර(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ආපසු 0;
	} නැතහොත් නම් (!count2) {
		අතරතුර(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ආපසු 0;
	}

	memset(&lcs, 0, parimaa_na(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	නම් (lcs_found < 0)
		goto out;
	නැතහොත් නම් (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	නැතහොත් {
		නම් (lcs.begin1 == 0 && lcs.begin2 == 0) {
			අතරතුර (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			අතරතුර (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} නැතහොත් {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			නම් (result)
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
	ආපසු result;
}

පූර්ණ_සංඛ්‍යා xdl_do_histogram_diff(xpparam_t const *xpp, xdfenv_t *env)
{
	ආපසු histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
