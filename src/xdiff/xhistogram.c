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

#அந்தர்பாவ "xinclude.h"

#define MAX_PTR	UINT_MAX
#define MAX_CNT	UINT_MAX

#define வரி_END(n) (வரி##n + மொத்தம்##n - 1)
#define வரி_END_PTR(n) (*வரி##n + *மொத்தம்##n - 1)

கட்டமைப்பு histindex {
	கட்டமைப்பு record {
		unsigned முழு ptr, cnt;
		கட்டமைப்பு record *next;
	} **records, /* an occurrence */
	  **வரி_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned முழு *next_ptrs;
	unsigned முழு table_bits,
		     records_size,
		     வரி_map_size;

	unsigned முழு max_chain_length,
		     சாவி_shift,
		     ptr_shift;

	unsigned முழு cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t மாறிலி *xpp;
};

கட்டமைப்பு region {
	unsigned முழு begin1, end1;
	unsigned முழு begin2, end2;
};

#define வரி_MAP(i, a) (i->வரி_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((வரி_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

நிலையான முழு cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	திரும்பு r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, வரி) \
	XDL_HASHLONG((REC(index->env, side, வரி))->ha, index->table_bits)

நிலையான முழு scanA(கட்டமைப்பு histindex *index, முழு line1, முழு count1)
{
	unsigned முழு ptr, tbl_idx;
	unsigned முழு chain_len;
	கட்டமைப்பு record **rec_chain, *rec;

	க்கு (ptr = வரி_END(1); (unsigned முழு)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		போது (rec) {
			எனில் (CMP(index, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				NEXT_PTR(index, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				வரி_MAP(index, ptr) = rec;
				செல் continue_scan;
			}

			rec = rec->next;
			chain_len++;
		}

		எனில் (chain_len == index->max_chain_length)
			திரும்பு -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		எனில் (!(rec = xdl_cha_alloc(&index->rcha)))
			திரும்பு -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		வரி_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	திரும்பு 0;
}

நிலையான முழு try_lcs(கட்டமைப்பு histindex *index, கட்டமைப்பு region *lcs, முழு b_ptr,
	முழு line1, முழு count1, முழு line2, முழு count2)
{
	unsigned முழு b_next = b_ptr + 1;
	கட்டமைப்பு record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned முழு as, ae, bs, be, np, rc;
	முழு should_break;

	க்கு (; rec; rec = rec->next) {
		எனில் (rec->cnt > index->cnt) {
			எனில் (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			தொடர்;
		}

		as = rec->ptr;
		எனில் (!CMP(index, 1, as, 2, b_ptr))
			தொடர்;

		index->has_common = 1;
		க்கு (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			போது ((unsigned முழு)line1 < as && (unsigned முழு)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				எனில் (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			போது (ae < (unsigned முழு)வரி_END(1) && be < (unsigned முழு)வரி_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				எனில் (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			எனில் (b_next <= be)
				b_next = be + 1;
			எனில் (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			எனில் (np == 0)
				நிறுத்து;

			போது (np <= ae) {
				np = NEXT_PTR(index, np);
				எனில் (np == 0) {
					should_break = 1;
					நிறுத்து;
				}
			}

			எனில் (should_break)
				நிறுத்து;

			as = np;
		}
	}
	திரும்பு b_next;
}

நிலையான முழு fall_back_to_classic_diff(xpparam_t மாறிலி *xpp, xdfenv_t *env,
		முழு line1, முழு count1, முழு line2, முழு count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, அளவு(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	திரும்பு xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

நிலையான உள்வரி வெறுமை free_index(கட்டமைப்பு histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->வரி_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

நிலையான முழு find_lcs(xpparam_t மாறிலி *xpp, xdfenv_t *env,
		    கட்டமைப்பு region *lcs,
		    முழு line1, முழு count1, முழு line2, முழு count2)
{
	முழு b_ptr;
	முழு ret = -1;
	கட்டமைப்பு histindex index;

	memset(&index, 0, அளவு(index));

	index.env = env;
	index.xpp = xpp;

	index.records = வெற்று;
	index.வரி_map = வெற்று;
	/* in case of early xdl_cha_free() */
	index.rcha.head = வெற்று;

	index.table_bits = xdl_hashbits(count1);
	index.records_size = 1 << index.table_bits;
	எனில் (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		செல் cleanup;

	index.வரி_map_size = count1;
	எனில் (!XDL_CALLOC_ARRAY(index.வரி_map, index.வரி_map_size))
		செல் cleanup;

	எனில் (!XDL_CALLOC_ARRAY(index.next_ptrs, index.வரி_map_size))
		செல் cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	எனில் (xdl_cha_init(&index.rcha, அளவு(கட்டமைப்பு record), count1 / 4 + 1) < 0)
		செல் cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	எனில் (scanA(&index, line1, count1))
		செல் cleanup;

	index.cnt = index.max_chain_length + 1;

	க்கு (b_ptr = line2; b_ptr <= வரி_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	எனில் (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	இல்லையெனில்
		ret = 0;

cleanup:
	free_index(&index);
	திரும்பு ret;
}

நிலையான முழு histogram_diff(xpparam_t மாறிலி *xpp, xdfenv_t *env,
	முழு line1, முழு count1, முழு line2, முழு count2)
{
	கட்டமைப்பு region lcs;
	முழு lcs_found;
	முழு result;
redo:
	result = -1;

	எனில் (count1 <= 0 && count2 <= 0)
		திரும்பு 0;

	எனில் ((unsigned முழு)வரி_END(1) >= MAX_PTR)
		திரும்பு -1;

	எனில் (!count1) {
		போது(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		திரும்பு 0;
	} இல்லையெனில் எனில் (!count2) {
		போது(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		திரும்பு 0;
	}

	memset(&lcs, 0, அளவு(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	எனில் (lcs_found < 0)
		செல் out;
	இல்லையெனில் எனில் (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	இல்லையெனில் {
		எனில் (lcs.begin1 == 0 && lcs.begin2 == 0) {
			போது (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			போது (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} இல்லையெனில் {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			எனில் (result)
				செல் out;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = வரி_END(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = வரி_END(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			செல் redo;
		}
	}
out:
	திரும்பு result;
}

முழு xdl_do_histogram_diff(xpparam_t மாறிலி *xpp, xdfenv_t *env)
{
	திரும்பு histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
