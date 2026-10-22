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

#अन्तर्भाव "xinclude.h"

#define MAX_PTR	UINT_MAX
#define MAX_CNT	UINT_MAX

#define ओळ_END(n) (ओळ##n + मोजणी##n - 1)
#define ओळ_END_PTR(n) (*ओळ##n + *मोजणी##n - 1)

संरचना histindex {
	संरचना record {
		unsigned पूर्णाङ्क ptr, cnt;
		संरचना record *next;
	} **records, /* an occurrence */
	  **ओळ_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned पूर्णाङ्क *next_ptrs;
	unsigned पूर्णाङ्क table_bits,
		     records_size,
		     ओळ_map_size;

	unsigned पूर्णाङ्क max_chain_length,
		     किल्ली_shift,
		     ptr_shift;

	unsigned पूर्णाङ्क cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ध्रुव *xpp;
};

संरचना region {
	unsigned पूर्णाङ्क begin1, end1;
	unsigned पूर्णाङ्क begin2, end2;
};

#define ओळ_MAP(i, a) (i->ओळ_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((ओळ_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

स्थिर पूर्णाङ्क cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	प्रत्यागम r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, ओळ) \
	XDL_HASHLONG((REC(index->env, side, ओळ))->ha, index->table_bits)

स्थिर पूर्णाङ्क scanA(संरचना histindex *index, पूर्णाङ्क line1, पूर्णाङ्क count1)
{
	unsigned पूर्णाङ्क ptr, tbl_idx;
	unsigned पूर्णाङ्क chain_len;
	संरचना record **rec_chain, *rec;

	प्रति (ptr = ओळ_END(1); (unsigned पूर्णाङ्क)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		यावत् (rec) {
			यदि (CMP(index, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				NEXT_PTR(index, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				ओळ_MAP(index, ptr) = rec;
				गच्छ continue_scan;
			}

			rec = rec->next;
			chain_len++;
		}

		यदि (chain_len == index->max_chain_length)
			प्रत्यागम -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		यदि (!(rec = xdl_cha_alloc(&index->rcha)))
			प्रत्यागम -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		ओळ_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क try_lcs(संरचना histindex *index, संरचना region *lcs, पूर्णाङ्क b_ptr,
	पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2)
{
	unsigned पूर्णाङ्क b_next = b_ptr + 1;
	संरचना record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned पूर्णाङ्क as, ae, bs, be, np, rc;
	पूर्णाङ्क should_break;

	प्रति (; rec; rec = rec->next) {
		यदि (rec->cnt > index->cnt) {
			यदि (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			अनुवर्त;
		}

		as = rec->ptr;
		यदि (!CMP(index, 1, as, 2, b_ptr))
			अनुवर्त;

		index->has_common = 1;
		प्रति (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			यावत् ((unsigned पूर्णाङ्क)line1 < as && (unsigned पूर्णाङ्क)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				यदि (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			यावत् (ae < (unsigned पूर्णाङ्क)ओळ_END(1) && be < (unsigned पूर्णाङ्क)ओळ_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				यदि (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			यदि (b_next <= be)
				b_next = be + 1;
			यदि (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			यदि (np == 0)
				भङ्ग;

			यावत् (np <= ae) {
				np = NEXT_PTR(index, np);
				यदि (np == 0) {
					should_break = 1;
					भङ्ग;
				}
			}

			यदि (should_break)
				भङ्ग;

			as = np;
		}
	}
	प्रत्यागम b_next;
}

स्थिर पूर्णाङ्क fall_back_to_classic_diff(xpparam_t ध्रुव *xpp, xdfenv_t *env,
		पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, परिमाण(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	प्रत्यागम xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

स्थिर अन्तःस्थ शून्य free_index(संरचना histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->ओळ_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

स्थिर पूर्णाङ्क find_lcs(xpparam_t ध्रुव *xpp, xdfenv_t *env,
		    संरचना region *lcs,
		    पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2)
{
	पूर्णाङ्क b_ptr;
	पूर्णाङ्क ret = -1;
	संरचना histindex index;

	memset(&index, 0, परिमाण(index));

	index.env = env;
	index.xpp = xpp;

	index.records = रिक्त;
	index.ओळ_map = रिक्त;
	/* in case of early xdl_cha_free() */
	index.rcha.head = रिक्त;

	index.table_bits = xdl_hashbits(count1);
	index.records_size = 1 << index.table_bits;
	यदि (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		गच्छ cleanup;

	index.ओळ_map_size = count1;
	यदि (!XDL_CALLOC_ARRAY(index.ओळ_map, index.ओळ_map_size))
		गच्छ cleanup;

	यदि (!XDL_CALLOC_ARRAY(index.next_ptrs, index.ओळ_map_size))
		गच्छ cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	यदि (xdl_cha_init(&index.rcha, परिमाण(संरचना record), count1 / 4 + 1) < 0)
		गच्छ cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	यदि (scanA(&index, line1, count1))
		गच्छ cleanup;

	index.cnt = index.max_chain_length + 1;

	प्रति (b_ptr = line2; b_ptr <= ओळ_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	यदि (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	अन्यथा
		ret = 0;

cleanup:
	free_index(&index);
	प्रत्यागम ret;
}

स्थिर पूर्णाङ्क histogram_diff(xpparam_t ध्रुव *xpp, xdfenv_t *env,
	पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2)
{
	संरचना region lcs;
	पूर्णाङ्क lcs_found;
	पूर्णाङ्क result;
redo:
	result = -1;

	यदि (count1 <= 0 && count2 <= 0)
		प्रत्यागम 0;

	यदि ((unsigned पूर्णाङ्क)ओळ_END(1) >= MAX_PTR)
		प्रत्यागम -1;

	यदि (!count1) {
		यावत्(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		प्रत्यागम 0;
	} अन्यथा यदि (!count2) {
		यावत्(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		प्रत्यागम 0;
	}

	memset(&lcs, 0, परिमाण(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	यदि (lcs_found < 0)
		गच्छ out;
	अन्यथा यदि (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	अन्यथा {
		यदि (lcs.begin1 == 0 && lcs.begin2 == 0) {
			यावत् (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			यावत् (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} अन्यथा {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			यदि (result)
				गच्छ out;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = ओळ_END(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = ओळ_END(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			गच्छ redo;
		}
	}
out:
	प्रत्यागम result;
}

पूर्णाङ्क xdl_do_histogram_diff(xpparam_t ध्रुव *xpp, xdfenv_t *env)
{
	प्रत्यागम histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
