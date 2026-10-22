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

#ಅನ್ತರ್ಭಾವ "xinclude.h"

#define MAX_PTR	UINT_MAX
#define MAX_CNT	UINT_MAX

#define ಸಾಲು_END(n) (ಸಾಲು##n + ಎಣಿಕೆ##n - 1)
#define ಸಾಲು_END_PTR(n) (*ಸಾಲು##n + *ಎಣಿಕೆ##n - 1)

ಸಂರಚನಾ histindex {
	ಸಂರಚನಾ record {
		unsigned ಪೂರ್ಣಾಙ್ಕ ptr, cnt;
		ಸಂರಚನಾ record *next;
	} **records, /* an occurrence */
	  **ಸಾಲು_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned ಪೂರ್ಣಾಙ್ಕ *next_ptrs;
	unsigned ಪೂರ್ಣಾಙ್ಕ table_bits,
		     records_size,
		     ಸಾಲು_map_size;

	unsigned ಪೂರ್ಣಾಙ್ಕ max_chain_length,
		     ಕೀಲಿ_shift,
		     ptr_shift;

	unsigned ಪೂರ್ಣಾಙ್ಕ cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ಧ್ರುವ *xpp;
};

ಸಂರಚನಾ region {
	unsigned ಪೂರ್ಣಾಙ್ಕ begin1, end1;
	unsigned ಪೂರ್ಣಾಙ್ಕ begin2, end2;
};

#define ಸಾಲು_MAP(i, a) (i->ಸಾಲು_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((ಸಾಲು_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	ಪ್ರತ್ಯಾಗಮ r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, ಸಾಲು) \
	XDL_HASHLONG((REC(index->env, side, ಸಾಲು))->ha, index->table_bits)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ scanA(ಸಂರಚನಾ histindex *index, ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ ptr, tbl_idx;
	unsigned ಪೂರ್ಣಾಙ್ಕ chain_len;
	ಸಂರಚನಾ record **rec_chain, *rec;

	ಪ್ರತಿ (ptr = ಸಾಲು_END(1); (unsigned ಪೂರ್ಣಾಙ್ಕ)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		ಯಾವತ್ (rec) {
			ಯದಿ (CMP(index, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				NEXT_PTR(index, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				ಸಾಲು_MAP(index, ptr) = rec;
				ಗಚ್ಛ continue_scan;
			}

			rec = rec->next;
			chain_len++;
		}

		ಯದಿ (chain_len == index->max_chain_length)
			ಪ್ರತ್ಯಾಗಮ -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		ಯದಿ (!(rec = xdl_cha_alloc(&index->rcha)))
			ಪ್ರತ್ಯಾಗಮ -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		ಸಾಲು_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ try_lcs(ಸಂರಚನಾ histindex *index, ಸಂರಚನಾ region *lcs, ಪೂರ್ಣಾಙ್ಕ b_ptr,
	ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ b_next = b_ptr + 1;
	ಸಂರಚನಾ record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned ಪೂರ್ಣಾಙ್ಕ as, ae, bs, be, np, rc;
	ಪೂರ್ಣಾಙ್ಕ should_break;

	ಪ್ರತಿ (; rec; rec = rec->next) {
		ಯದಿ (rec->cnt > index->cnt) {
			ಯದಿ (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			ಅನುವರ್ತ;
		}

		as = rec->ptr;
		ಯದಿ (!CMP(index, 1, as, 2, b_ptr))
			ಅನುವರ್ತ;

		index->has_common = 1;
		ಪ್ರತಿ (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			ಯಾವತ್ ((unsigned ಪೂರ್ಣಾಙ್ಕ)line1 < as && (unsigned ಪೂರ್ಣಾಙ್ಕ)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				ಯದಿ (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			ಯಾವತ್ (ae < (unsigned ಪೂರ್ಣಾಙ್ಕ)ಸಾಲು_END(1) && be < (unsigned ಪೂರ್ಣಾಙ್ಕ)ಸಾಲು_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				ಯದಿ (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			ಯದಿ (b_next <= be)
				b_next = be + 1;
			ಯದಿ (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			ಯದಿ (np == 0)
				ಭಙ್ಗ;

			ಯಾವತ್ (np <= ae) {
				np = NEXT_PTR(index, np);
				ಯದಿ (np == 0) {
					should_break = 1;
					ಭಙ್ಗ;
				}
			}

			ಯದಿ (should_break)
				ಭಙ್ಗ;

			as = np;
		}
	}
	ಪ್ರತ್ಯಾಗಮ b_next;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fall_back_to_classic_diff(xpparam_t ಧ್ರುವ *xpp, xdfenv_t *env,
		ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, ಪರಿಮಾಣ(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ಪ್ರತ್ಯಾಗಮ xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ free_index(ಸಂರಚನಾ histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->ಸಾಲು_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ find_lcs(xpparam_t ಧ್ರುವ *xpp, xdfenv_t *env,
		    ಸಂರಚನಾ region *lcs,
		    ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2)
{
	ಪೂರ್ಣಾಙ್ಕ b_ptr;
	ಪೂರ್ಣಾಙ್ಕ ret = -1;
	ಸಂರಚನಾ histindex index;

	memset(&index, 0, ಪರಿಮಾಣ(index));

	index.env = env;
	index.xpp = xpp;

	index.records = ರಿಕ್ತ;
	index.ಸಾಲು_map = ರಿಕ್ತ;
	/* in case of early xdl_cha_free() */
	index.rcha.head = ರಿಕ್ತ;

	index.table_bits = xdl_hashbits(count1);
	index.records_size = 1 << index.table_bits;
	ಯದಿ (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		ಗಚ್ಛ cleanup;

	index.ಸಾಲು_map_size = count1;
	ಯದಿ (!XDL_CALLOC_ARRAY(index.ಸಾಲು_map, index.ಸಾಲು_map_size))
		ಗಚ್ಛ cleanup;

	ಯದಿ (!XDL_CALLOC_ARRAY(index.next_ptrs, index.ಸಾಲು_map_size))
		ಗಚ್ಛ cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	ಯದಿ (xdl_cha_init(&index.rcha, ಪರಿಮಾಣ(ಸಂರಚನಾ record), count1 / 4 + 1) < 0)
		ಗಚ್ಛ cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	ಯದಿ (scanA(&index, line1, count1))
		ಗಚ್ಛ cleanup;

	index.cnt = index.max_chain_length + 1;

	ಪ್ರತಿ (b_ptr = line2; b_ptr <= ಸಾಲು_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	ಯದಿ (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	ಅನ್ಯಥಾ
		ret = 0;

cleanup:
	free_index(&index);
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ histogram_diff(xpparam_t ಧ್ರುವ *xpp, xdfenv_t *env,
	ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2)
{
	ಸಂರಚನಾ region lcs;
	ಪೂರ್ಣಾಙ್ಕ lcs_found;
	ಪೂರ್ಣಾಙ್ಕ result;
redo:
	result = -1;

	ಯದಿ (count1 <= 0 && count2 <= 0)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಯದಿ ((unsigned ಪೂರ್ಣಾಙ್ಕ)ಸಾಲು_END(1) >= MAX_PTR)
		ಪ್ರತ್ಯಾಗಮ -1;

	ಯದಿ (!count1) {
		ಯಾವತ್(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ಪ್ರತ್ಯಾಗಮ 0;
	} ಅನ್ಯಥಾ ಯದಿ (!count2) {
		ಯಾವತ್(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	memset(&lcs, 0, ಪರಿಮಾಣ(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	ಯದಿ (lcs_found < 0)
		ಗಚ್ಛ out;
	ಅನ್ಯಥಾ ಯದಿ (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	ಅನ್ಯಥಾ {
		ಯದಿ (lcs.begin1 == 0 && lcs.begin2 == 0) {
			ಯಾವತ್ (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			ಯಾವತ್ (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} ಅನ್ಯಥಾ {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			ಯದಿ (result)
				ಗಚ್ಛ out;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = ಸಾಲು_END(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = ಸಾಲು_END(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			ಗಚ್ಛ redo;
		}
	}
out:
	ಪ್ರತ್ಯಾಗಮ result;
}

ಪೂರ್ಣಾಙ್ಕ xdl_do_histogram_diff(xpparam_t ಧ್ರುವ *xpp, xdfenv_t *env)
{
	ಪ್ರತ್ಯಾಗಮ histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
