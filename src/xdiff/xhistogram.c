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

#অন্তর্ভাব "xinclude.h"

#define MAX_PTR	UINT_MAX
#define MAX_CNT	UINT_MAX

#define সারি_END(n) (সারি##n + গুনতি##n - 1)
#define সারি_END_PTR(n) (*সারি##n + *গুনতি##n - 1)

সংরচনা histindex {
	সংরচনা record {
		unsigned পূর্ণাঙ্ক ptr, cnt;
		সংরচনা record *next;
	} **records, /* an occurrence */
	  **সারি_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned পূর্ণাঙ্ক *next_ptrs;
	unsigned পূর্ণাঙ্ক table_bits,
		     records_size,
		     সারি_map_size;

	unsigned পূর্ণাঙ্ক max_chain_length,
		     চাবি_shift,
		     ptr_shift;

	unsigned পূর্ণাঙ্ক cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ধ্রুব *xpp;
};

সংরচনা region {
	unsigned পূর্ণাঙ্ক begin1, end1;
	unsigned পূর্ণাঙ্ক begin2, end2;
};

#define সারি_MAP(i, a) (i->সারি_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((সারি_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

স্থির পূর্ণাঙ্ক cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	প্রত্যাগম r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, সারি) \
	XDL_HASHLONG((REC(index->env, side, সারি))->ha, index->table_bits)

স্থির পূর্ণাঙ্ক scanA(সংরচনা histindex *index, পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1)
{
	unsigned পূর্ণাঙ্ক ptr, tbl_idx;
	unsigned পূর্ণাঙ্ক chain_len;
	সংরচনা record **rec_chain, *rec;

	প্রতি (ptr = সারি_END(1); (unsigned পূর্ণাঙ্ক)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		যাবত্ (rec) {
			যদি (CMP(index, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				NEXT_PTR(index, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				সারি_MAP(index, ptr) = rec;
				গচ্ছ continue_scan;
			}

			rec = rec->next;
			chain_len++;
		}

		যদি (chain_len == index->max_chain_length)
			প্রত্যাগম -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		যদি (!(rec = xdl_cha_alloc(&index->rcha)))
			প্রত্যাগম -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		সারি_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক try_lcs(সংরচনা histindex *index, সংরচনা region *lcs, পূর্ণাঙ্ক b_ptr,
	পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2)
{
	unsigned পূর্ণাঙ্ক b_next = b_ptr + 1;
	সংরচনা record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned পূর্ণাঙ্ক as, ae, bs, be, np, rc;
	পূর্ণাঙ্ক should_break;

	প্রতি (; rec; rec = rec->next) {
		যদি (rec->cnt > index->cnt) {
			যদি (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			অনুবর্ত;
		}

		as = rec->ptr;
		যদি (!CMP(index, 1, as, 2, b_ptr))
			অনুবর্ত;

		index->has_common = 1;
		প্রতি (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			যাবত্ ((unsigned পূর্ণাঙ্ক)line1 < as && (unsigned পূর্ণাঙ্ক)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				যদি (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			যাবত্ (ae < (unsigned পূর্ণাঙ্ক)সারি_END(1) && be < (unsigned পূর্ণাঙ্ক)সারি_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				যদি (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			যদি (b_next <= be)
				b_next = be + 1;
			যদি (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			যদি (np == 0)
				ভঙ্গ;

			যাবত্ (np <= ae) {
				np = NEXT_PTR(index, np);
				যদি (np == 0) {
					should_break = 1;
					ভঙ্গ;
				}
			}

			যদি (should_break)
				ভঙ্গ;

			as = np;
		}
	}
	প্রত্যাগম b_next;
}

স্থির পূর্ণাঙ্ক fall_back_to_classic_diff(xpparam_t ধ্রুব *xpp, xdfenv_t *env,
		পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, পরিমাণ(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	প্রত্যাগম xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

স্থির অন্তঃস্থ শূন্য free_index(সংরচনা histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->সারি_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

স্থির পূর্ণাঙ্ক find_lcs(xpparam_t ধ্রুব *xpp, xdfenv_t *env,
		    সংরচনা region *lcs,
		    পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2)
{
	পূর্ণাঙ্ক b_ptr;
	পূর্ণাঙ্ক ret = -1;
	সংরচনা histindex index;

	memset(&index, 0, পরিমাণ(index));

	index.env = env;
	index.xpp = xpp;

	index.records = রিক্ত;
	index.সারি_map = রিক্ত;
	/* in case of early xdl_cha_free() */
	index.rcha.head = রিক্ত;

	index.table_bits = xdl_hashbits(count1);
	index.records_size = 1 << index.table_bits;
	যদি (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		গচ্ছ cleanup;

	index.সারি_map_size = count1;
	যদি (!XDL_CALLOC_ARRAY(index.সারি_map, index.সারি_map_size))
		গচ্ছ cleanup;

	যদি (!XDL_CALLOC_ARRAY(index.next_ptrs, index.সারি_map_size))
		গচ্ছ cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	যদি (xdl_cha_init(&index.rcha, পরিমাণ(সংরচনা record), count1 / 4 + 1) < 0)
		গচ্ছ cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	যদি (scanA(&index, line1, count1))
		গচ্ছ cleanup;

	index.cnt = index.max_chain_length + 1;

	প্রতি (b_ptr = line2; b_ptr <= সারি_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	যদি (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	অন্যথা
		ret = 0;

cleanup:
	free_index(&index);
	প্রত্যাগম ret;
}

স্থির পূর্ণাঙ্ক histogram_diff(xpparam_t ধ্রুব *xpp, xdfenv_t *env,
	পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2)
{
	সংরচনা region lcs;
	পূর্ণাঙ্ক lcs_found;
	পূর্ণাঙ্ক result;
redo:
	result = -1;

	যদি (count1 <= 0 && count2 <= 0)
		প্রত্যাগম 0;

	যদি ((unsigned পূর্ণাঙ্ক)সারি_END(1) >= MAX_PTR)
		প্রত্যাগম -1;

	যদি (!count1) {
		যাবত্(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		প্রত্যাগম 0;
	} অন্যথা যদি (!count2) {
		যাবত্(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		প্রত্যাগম 0;
	}

	memset(&lcs, 0, পরিমাণ(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	যদি (lcs_found < 0)
		গচ্ছ out;
	অন্যথা যদি (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	অন্যথা {
		যদি (lcs.begin1 == 0 && lcs.begin2 == 0) {
			যাবত্ (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			যাবত্ (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} অন্যথা {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			যদি (result)
				গচ্ছ out;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = সারি_END(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = সারি_END(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			গচ্ছ redo;
		}
	}
out:
	প্রত্যাগম result;
}

পূর্ণাঙ্ক xdl_do_histogram_diff(xpparam_t ধ্রুব *xpp, xdfenv_t *env)
{
	প্রত্যাগম histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
