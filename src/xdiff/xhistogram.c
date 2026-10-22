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
		unsigned ចំនួនគត់ ptr, cnt;
		sa_nracanaa record *next;
	} **records, /* an occurrence */
	  **line_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned ចំនួនគត់ *next_ptrs;
	unsigned ចំនួនគត់ table_bits,
		     records_size,
		     line_map_size;

	unsigned ចំនួនគត់ max_chain_length,
		     key_shift,
		     ptr_shift;

	unsigned ចំនួនគត់ cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t const *xpp;
};

sa_nracanaa region {
	unsigned ចំនួនគត់ begin1, end1;
	unsigned ចំនួនគត់ begin2, end2;
};

#define LINE_MAP(i, a) (i->line_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((LINE_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

static ចំនួនគត់ cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	ត្រឡប់ r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, line) \
	XDL_HASHLONG((REC(index->env, side, line))->ha, index->table_bits)

static ចំនួនគត់ scanA(sa_nracanaa histindex *index, ចំនួនគត់ line1, ចំនួនគត់ count1)
{
	unsigned ចំនួនគត់ ptr, tbl_idx;
	unsigned ចំនួនគត់ chain_len;
	sa_nracanaa record **rec_chain, *rec;

	សម្រាប់ (ptr = LINE_END(1); (unsigned ចំនួនគត់)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		ខណៈពេល (rec) {
			ប្រសិនបើ (CMP(index, 1, rec->ptr, 1, ptr)) {
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

		ប្រសិនបើ (chain_len == index->max_chain_length)
			ត្រឡប់ -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		ប្រសិនបើ (!(rec = xdl_cha_alloc(&index->rcha)))
			ត្រឡប់ -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		LINE_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	ត្រឡប់ 0;
}

static ចំនួនគត់ try_lcs(sa_nracanaa histindex *index, sa_nracanaa region *lcs, ចំនួនគត់ b_ptr,
	ចំនួនគត់ line1, ចំនួនគត់ count1, ចំនួនគត់ line2, ចំនួនគត់ count2)
{
	unsigned ចំនួនគត់ b_next = b_ptr + 1;
	sa_nracanaa record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned ចំនួនគត់ as, ae, bs, be, np, rc;
	ចំនួនគត់ should_break;

	សម្រាប់ (; rec; rec = rec->next) {
		ប្រសិនបើ (rec->cnt > index->cnt) {
			ប្រសិនបើ (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			continue;
		}

		as = rec->ptr;
		ប្រសិនបើ (!CMP(index, 1, as, 2, b_ptr))
			continue;

		index->has_common = 1;
		សម្រាប់ (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			ខណៈពេល ((unsigned ចំនួនគត់)line1 < as && (unsigned ចំនួនគត់)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				ប្រសិនបើ (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			ខណៈពេល (ae < (unsigned ចំនួនគត់)LINE_END(1) && be < (unsigned ចំនួនគត់)LINE_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				ប្រសិនបើ (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			ប្រសិនបើ (b_next <= be)
				b_next = be + 1;
			ប្រសិនបើ (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			ប្រសិនបើ (np == 0)
				bha_nga;

			ខណៈពេល (np <= ae) {
				np = NEXT_PTR(index, np);
				ប្រសិនបើ (np == 0) {
					should_break = 1;
					bha_nga;
				}
			}

			ប្រសិនបើ (should_break)
				bha_nga;

			as = np;
		}
	}
	ត្រឡប់ b_next;
}

static ចំនួនគត់ fall_back_to_classic_diff(xpparam_t const *xpp, xdfenv_t *env,
		ចំនួនគត់ line1, ចំនួនគត់ count1, ចំនួនគត់ line2, ចំនួនគត់ count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, parimaa_na(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ត្រឡប់ xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

static anta_hstha void free_index(sa_nracanaa histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->line_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

static ចំនួនគត់ find_lcs(xpparam_t const *xpp, xdfenv_t *env,
		    sa_nracanaa region *lcs,
		    ចំនួនគត់ line1, ចំនួនគត់ count1, ចំនួនគត់ line2, ចំនួនគត់ count2)
{
	ចំនួនគត់ b_ptr;
	ចំនួនគត់ ret = -1;
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
	ប្រសិនបើ (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		goto cleanup;

	index.line_map_size = count1;
	ប្រសិនបើ (!XDL_CALLOC_ARRAY(index.line_map, index.line_map_size))
		goto cleanup;

	ប្រសិនបើ (!XDL_CALLOC_ARRAY(index.next_ptrs, index.line_map_size))
		goto cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	ប្រសិនបើ (xdl_cha_init(&index.rcha, parimaa_na(sa_nracanaa record), count1 / 4 + 1) < 0)
		goto cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	ប្រសិនបើ (scanA(&index, line1, count1))
		goto cleanup;

	index.cnt = index.max_chain_length + 1;

	សម្រាប់ (b_ptr = line2; b_ptr <= LINE_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	ប្រសិនបើ (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	បើមិនដូច្នេះ
		ret = 0;

cleanup:
	free_index(&index);
	ត្រឡប់ ret;
}

static ចំនួនគត់ histogram_diff(xpparam_t const *xpp, xdfenv_t *env,
	ចំនួនគត់ line1, ចំនួនគត់ count1, ចំនួនគត់ line2, ចំនួនគត់ count2)
{
	sa_nracanaa region lcs;
	ចំនួនគត់ lcs_found;
	ចំនួនគត់ result;
redo:
	result = -1;

	ប្រសិនបើ (count1 <= 0 && count2 <= 0)
		ត្រឡប់ 0;

	ប្រសិនបើ ((unsigned ចំនួនគត់)LINE_END(1) >= MAX_PTR)
		ត្រឡប់ -1;

	ប្រសិនបើ (!count1) {
		ខណៈពេល(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ត្រឡប់ 0;
	} បើមិនដូច្នេះ ប្រសិនបើ (!count2) {
		ខណៈពេល(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ត្រឡប់ 0;
	}

	memset(&lcs, 0, parimaa_na(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	ប្រសិនបើ (lcs_found < 0)
		goto out;
	បើមិនដូច្នេះ ប្រសិនបើ (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	បើមិនដូច្នេះ {
		ប្រសិនបើ (lcs.begin1 == 0 && lcs.begin2 == 0) {
			ខណៈពេល (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			ខណៈពេល (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} បើមិនដូច្នេះ {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			ប្រសិនបើ (result)
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
	ត្រឡប់ result;
}

ចំនួនគត់ xdl_do_histogram_diff(xpparam_t const *xpp, xdfenv_t *env)
{
	ត្រឡប់ histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
