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

#ଅନ୍ତର୍ଭାଵ "xinclude.h"

#define MAX_PTR	UINT_MAX
#define MAX_CNT	UINT_MAX

#define ଧାଡ଼ି_END(n) (ଧାଡ଼ି##n + ଗଣନା_2##n - 1)
#define ଧାଡ଼ି_END_PTR(n) (*ଧାଡ଼ି##n + *ଗଣନା_2##n - 1)

ସଂରଚନା histindex {
	ସଂରଚନା record {
		unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ptr, cnt;
		ସଂରଚନା record *next;
	} **records, /* an occurrence */
	  **ଧାଡ଼ି_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *next_ptrs;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା table_bits,
		     records_size,
		     ଧାଡ଼ି_map_size;

	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_chain_length,
		     ଚାବି_shift,
		     ptr_shift;

	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ଅଚଳ *xpp;
};

ସଂରଚନା region {
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା begin1, end1;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା begin2, end2;
};

#define ଧାଡ଼ି_MAP(i, a) (i->ଧାଡ଼ି_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((ଧାଡ଼ି_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	ଫେରନ୍ତୁ r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, ଧାଡ଼ି) \
	XDL_HASHLONG((REC(index->env, side, ଧାଡ଼ି))->ha, index->table_bits)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା scanA(ସଂରଚନା histindex *index, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ptr, tbl_idx;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା chain_len;
	ସଂରଚନା record **rec_chain, *rec;

	ପାଇଁ (ptr = ଧାଡ଼ି_END(1); (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		ଯେତେବେଳେ (rec) {
			ଯଦି (CMP(index, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				NEXT_PTR(index, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				ଧାଡ଼ି_MAP(index, ptr) = rec;
				ଯାଅ continue_scan;
			}

			rec = rec->next;
			chain_len++;
		}

		ଯଦି (chain_len == index->max_chain_length)
			ଫେରନ୍ତୁ -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		ଯଦି (!(rec = xdl_cha_alloc(&index->rcha)))
			ଫେରନ୍ତୁ -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		ଧାଡ଼ି_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା try_lcs(ସଂରଚନା histindex *index, ସଂରଚନା region *lcs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_ptr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2)
{
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_next = b_ptr + 1;
	ସଂରଚନା record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା as, ae, bs, be, np, rc;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା should_break;

	ପାଇଁ (; rec; rec = rec->next) {
		ଯଦି (rec->cnt > index->cnt) {
			ଯଦି (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			ଜାରି;
		}

		as = rec->ptr;
		ଯଦି (!CMP(index, 1, as, 2, b_ptr))
			ଜାରି;

		index->has_common = 1;
		ପାଇଁ (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			ଯେତେବେଳେ ((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)line1 < as && (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				ଯଦି (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			ଯେତେବେଳେ (ae < (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଧାଡ଼ି_END(1) && be < (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଧାଡ଼ି_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				ଯଦି (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			ଯଦି (b_next <= be)
				b_next = be + 1;
			ଯଦି (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			ଯଦି (np == 0)
				ଭାଙ୍ଗ;

			ଯେତେବେଳେ (np <= ae) {
				np = NEXT_PTR(index, np);
				ଯଦି (np == 0) {
					should_break = 1;
					ଭାଙ୍ଗ;
				}
			}

			ଯଦି (should_break)
				ଭାଙ୍ଗ;

			as = np;
		}
	}
	ଫେରନ୍ତୁ b_next;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fall_back_to_classic_diff(xpparam_t ଅଚଳ *xpp, xdfenv_t *env,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, ଆକାର(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ଫେରନ୍ତୁ xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ free_index(ସଂରଚନା histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->ଧାଡ଼ି_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_lcs(xpparam_t ଅଚଳ *xpp, xdfenv_t *env,
		    ସଂରଚନା region *lcs,
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_ptr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = -1;
	ସଂରଚନା histindex index;

	memset(&index, 0, ଆକାର(index));

	index.env = env;
	index.xpp = xpp;

	index.records = ଖାଲି;
	index.ଧାଡ଼ି_map = ଖାଲି;
	/* in case of early xdl_cha_free() */
	index.rcha.head = ଖାଲି;

	index.table_bits = xdl_hashbits(count1);
	index.records_size = 1 << index.table_bits;
	ଯଦି (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		ଯାଅ cleanup;

	index.ଧାଡ଼ି_map_size = count1;
	ଯଦି (!XDL_CALLOC_ARRAY(index.ଧାଡ଼ି_map, index.ଧାଡ଼ି_map_size))
		ଯାଅ cleanup;

	ଯଦି (!XDL_CALLOC_ARRAY(index.next_ptrs, index.ଧାଡ଼ି_map_size))
		ଯାଅ cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	ଯଦି (xdl_cha_init(&index.rcha, ଆକାର(ସଂରଚନା record), count1 / 4 + 1) < 0)
		ଯାଅ cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	ଯଦି (scanA(&index, line1, count1))
		ଯାଅ cleanup;

	index.cnt = index.max_chain_length + 1;

	ପାଇଁ (b_ptr = line2; b_ptr <= ଧାଡ଼ି_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	ଯଦି (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	ଅନ୍ୟଥା
		ret = 0;

cleanup:
	free_index(&index);
	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା histogram_diff(xpparam_t ଅଚଳ *xpp, xdfenv_t *env,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2)
{
	ସଂରଚନା region lcs;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା lcs_found;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result;
redo:
	result = -1;

	ଯଦି (count1 <= 0 && count2 <= 0)
		ଫେରନ୍ତୁ 0;

	ଯଦି ((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଧାଡ଼ି_END(1) >= MAX_PTR)
		ଫେରନ୍ତୁ -1;

	ଯଦି (!count1) {
		ଯେତେବେଳେ(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ଫେରନ୍ତୁ 0;
	} ଅନ୍ୟଥା ଯଦି (!count2) {
		ଯେତେବେଳେ(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ଫେରନ୍ତୁ 0;
	}

	memset(&lcs, 0, ଆକାର(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	ଯଦି (lcs_found < 0)
		ଯାଅ out;
	ଅନ୍ୟଥା ଯଦି (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	ଅନ୍ୟଥା {
		ଯଦି (lcs.begin1 == 0 && lcs.begin2 == 0) {
			ଯେତେବେଳେ (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			ଯେତେବେଳେ (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} ଅନ୍ୟଥା {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			ଯଦି (result)
				ଯାଅ out;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = ଧାଡ଼ି_END(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = ଧାଡ଼ି_END(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			ଯାଅ redo;
		}
	}
out:
	ଫେରନ୍ତୁ result;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_do_histogram_diff(xpparam_t ଅଚଳ *xpp, xdfenv_t *env)
{
	ଫେରନ୍ତୁ histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
