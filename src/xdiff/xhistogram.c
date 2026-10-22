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

#ضمن "xinclude.h"

#define MAX_PTR	UINT_MAX
#define MAX_CNT	UINT_MAX

#define سطر_END(n) (سطر##n + گنتی##n - 1)
#define سطر_END_PTR(n) (*سطر##n + *گنتی##n - 1)

بنية histindex {
	بنية record {
		unsigned صحيح ptr, cnt;
		بنية record *next;
	} **records, /* an occurrence */
	  **سطر_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned صحيح *next_ptrs;
	unsigned صحيح table_bits,
		     records_size,
		     سطر_map_size;

	unsigned صحيح max_chain_length,
		     کلید_shift,
		     ptr_shift;

	unsigned صحيح cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ثابت *xpp;
};

بنية region {
	unsigned صحيح begin1, end1;
	unsigned صحيح begin2, end2;
};

#define سطر_MAP(i, a) (i->سطر_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((سطر_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

ساكن صحيح cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	أرجع r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, سطر) \
	XDL_HASHLONG((REC(index->env, side, سطر))->ha, index->table_bits)

ساكن صحيح scanA(بنية histindex *index, صحيح line1, صحيح count1)
{
	unsigned صحيح ptr, tbl_idx;
	unsigned صحيح chain_len;
	بنية record **rec_chain, *rec;

	لكل (ptr = سطر_END(1); (unsigned صحيح)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		طالما (rec) {
			إذا (CMP(index, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				NEXT_PTR(index, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				سطر_MAP(index, ptr) = rec;
				اذهب continue_scan;
			}

			rec = rec->next;
			chain_len++;
		}

		إذا (chain_len == index->max_chain_length)
			أرجع -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		إذا (!(rec = xdl_cha_alloc(&index->rcha)))
			أرجع -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		سطر_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	أرجع 0;
}

ساكن صحيح try_lcs(بنية histindex *index, بنية region *lcs, صحيح b_ptr,
	صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	unsigned صحيح b_next = b_ptr + 1;
	بنية record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned صحيح as, ae, bs, be, np, rc;
	صحيح should_break;

	لكل (; rec; rec = rec->next) {
		إذا (rec->cnt > index->cnt) {
			إذا (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			استمر;
		}

		as = rec->ptr;
		إذا (!CMP(index, 1, as, 2, b_ptr))
			استمر;

		index->has_common = 1;
		لكل (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			طالما ((unsigned صحيح)line1 < as && (unsigned صحيح)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				إذا (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			طالما (ae < (unsigned صحيح)سطر_END(1) && be < (unsigned صحيح)سطر_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				إذا (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			إذا (b_next <= be)
				b_next = be + 1;
			إذا (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			إذا (np == 0)
				اكسر;

			طالما (np <= ae) {
				np = NEXT_PTR(index, np);
				إذا (np == 0) {
					should_break = 1;
					اكسر;
				}
			}

			إذا (should_break)
				اكسر;

			as = np;
		}
	}
	أرجع b_next;
}

ساكن صحيح fall_back_to_classic_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, حجم(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	أرجع xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

ساكن مضمن فراغ free_index(بنية histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->سطر_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

ساكن صحيح find_lcs(xpparam_t ثابت *xpp, xdfenv_t *env,
		    بنية region *lcs,
		    صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	صحيح b_ptr;
	صحيح ret = -1;
	بنية histindex index;

	memset(&index, 0, حجم(index));

	index.env = env;
	index.xpp = xpp;

	index.records = عدم;
	index.سطر_map = عدم;
	/* in case of early xdl_cha_free() */
	index.rcha.head = عدم;

	index.table_bits = xdl_hashbits(count1);
	index.records_size = 1 << index.table_bits;
	إذا (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		اذهب cleanup;

	index.سطر_map_size = count1;
	إذا (!XDL_CALLOC_ARRAY(index.سطر_map, index.سطر_map_size))
		اذهب cleanup;

	إذا (!XDL_CALLOC_ARRAY(index.next_ptrs, index.سطر_map_size))
		اذهب cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	إذا (xdl_cha_init(&index.rcha, حجم(بنية record), count1 / 4 + 1) < 0)
		اذهب cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	إذا (scanA(&index, line1, count1))
		اذهب cleanup;

	index.cnt = index.max_chain_length + 1;

	لكل (b_ptr = line2; b_ptr <= سطر_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	إذا (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	وإلا
		ret = 0;

cleanup:
	free_index(&index);
	أرجع ret;
}

ساكن صحيح histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
	صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	بنية region lcs;
	صحيح lcs_found;
	صحيح result;
redo:
	result = -1;

	إذا (count1 <= 0 && count2 <= 0)
		أرجع 0;

	إذا ((unsigned صحيح)سطر_END(1) >= MAX_PTR)
		أرجع -1;

	إذا (!count1) {
		طالما(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		أرجع 0;
	} وإلا إذا (!count2) {
		طالما(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		أرجع 0;
	}

	memset(&lcs, 0, حجم(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	إذا (lcs_found < 0)
		اذهب out;
	وإلا إذا (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	وإلا {
		إذا (lcs.begin1 == 0 && lcs.begin2 == 0) {
			طالما (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			طالما (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} وإلا {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			إذا (result)
				اذهب out;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = سطر_END(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = سطر_END(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			اذهب redo;
		}
	}
out:
	أرجع result;
}

صحيح xdl_do_histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env)
{
	أرجع histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
