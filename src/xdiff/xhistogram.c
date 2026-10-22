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

কাঠামো histindex {
	কাঠামো record {
		unsigned পূর্ণসংখ্যা ptr, cnt;
		কাঠামো record *পরবর্তী;
	} **records, /* an occurrence */
	  **সারি_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned পূর্ণসংখ্যা *পরবর্তী_ptrs;
	unsigned পূর্ণসংখ্যা table_bits,
		     records_আকার,
		     সারি_map_আকার;

	unsigned পূর্ণসংখ্যা max_chain_দৈর্ঘ্য,
		     চাবি_shift,
		     ptr_shift;

	unsigned পূর্ণসংখ্যা cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ধ্রুবক *xpp;
};

কাঠামো region {
	unsigned পূর্ণসংখ্যা begin1, end1;
	unsigned পূর্ণসংখ্যা begin2, end2;
};

#define সারি_MAP(i, a) (i->সারি_map[(a) - i->ptr_shift])

#define পরবর্তী_PTR(সূচক, ptr) \
	(সূচক->পরবর্তী_ptrs[(ptr) - সূচক->ptr_shift])

#define CNT(সূচক, ptr) \
	((সারি_MAP(সূচক, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

স্থির পূর্ণসংখ্যা cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	ফেরত r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(সূচক, side, সারি) \
	XDL_HASHLONG((REC(সূচক->env, side, সারি))->ha, সূচক->table_bits)

স্থির পূর্ণসংখ্যা scanA(কাঠামো histindex *সূচক, পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1)
{
	unsigned পূর্ণসংখ্যা ptr, tbl_idx;
	unsigned পূর্ণসংখ্যা chain_len;
	কাঠামো record **rec_chain, *rec;

	জন্য (ptr = সারি_END(1); (unsigned পূর্ণসংখ্যা)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(সূচক, 1, ptr);
		rec_chain = সূচক->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		যখন (rec) {
			যদি (CMP(সূচক, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				পরবর্তী_PTR(সূচক, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				সারি_MAP(সূচক, ptr) = rec;
				যাও continue_scan;
			}

			rec = rec->পরবর্তী;
			chain_len++;
		}

		যদি (chain_len == সূচক->max_chain_দৈর্ঘ্য)
			ফেরত -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		যদি (!(rec = xdl_cha_alloc(&সূচক->rcha)))
			ফেরত -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->পরবর্তী = *rec_chain;
		*rec_chain = rec;
		সারি_MAP(সূচক, ptr) = rec;

continue_scan:
		; /* no op */
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা try_lcs(কাঠামো histindex *সূচক, কাঠামো region *lcs, পূর্ণসংখ্যা b_ptr,
	পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2)
{
	unsigned পূর্ণসংখ্যা b_পরবর্তী = b_ptr + 1;
	কাঠামো record *rec = সূচক->records[TABLE_HASH(সূচক, 2, b_ptr)];
	unsigned পূর্ণসংখ্যা as, ae, bs, be, np, rc;
	পূর্ণসংখ্যা should_break;

	জন্য (; rec; rec = rec->পরবর্তী) {
		যদি (rec->cnt > সূচক->cnt) {
			যদি (!সূচক->has_common)
				সূচক->has_common = CMP(সূচক, 1, rec->ptr, 2, b_ptr);
			চালু;
		}

		as = rec->ptr;
		যদি (!CMP(সূচক, 1, as, 2, b_ptr))
			চালু;

		সূচক->has_common = 1;
		জন্য (;;) {
			should_break = 0;
			np = পরবর্তী_PTR(সূচক, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			যখন ((unsigned পূর্ণসংখ্যা)line1 < as && (unsigned পূর্ণসংখ্যা)line2 < bs
				&& CMP(সূচক, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				যদি (1 < rc)
					rc = XDL_MIN(rc, CNT(সূচক, as));
			}
			যখন (ae < (unsigned পূর্ণসংখ্যা)সারি_END(1) && be < (unsigned পূর্ণসংখ্যা)সারি_END(2)
				&& CMP(সূচক, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				যদি (1 < rc)
					rc = XDL_MIN(rc, CNT(সূচক, ae));
			}

			যদি (b_পরবর্তী <= be)
				b_পরবর্তী = be + 1;
			যদি (lcs->end1 - lcs->begin1 < ae - as || rc < সূচক->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				সূচক->cnt = rc;
			}

			যদি (np == 0)
				বিরতি;

			যখন (np <= ae) {
				np = পরবর্তী_PTR(সূচক, np);
				যদি (np == 0) {
					should_break = 1;
					বিরতি;
				}
			}

			যদি (should_break)
				বিরতি;

			as = np;
		}
	}
	ফেরত b_পরবর্তী;
}

স্থির পূর্ণসংখ্যা fall_back_to_classic_diff(xpparam_t ধ্রুবক *xpp, xdfenv_t *env,
		পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, আকার(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ফেরত xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

স্থির অন্তঃরেখ শূন্য free_সূচক(কাঠামো histindex *সূচক)
{
	xdl_free(সূচক->records);
	xdl_free(সূচক->সারি_map);
	xdl_free(সূচক->পরবর্তী_ptrs);
	xdl_cha_free(&সূচক->rcha);
}

স্থির পূর্ণসংখ্যা find_lcs(xpparam_t ধ্রুবক *xpp, xdfenv_t *env,
		    কাঠামো region *lcs,
		    পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2)
{
	পূর্ণসংখ্যা b_ptr;
	পূর্ণসংখ্যা ret = -1;
	কাঠামো histindex সূচক;

	memset(&সূচক, 0, আকার(সূচক));

	সূচক.env = env;
	সূচক.xpp = xpp;

	সূচক.records = ফাঁকা;
	সূচক.সারি_map = ফাঁকা;
	/* in case of early xdl_cha_free() */
	সূচক.rcha.শীর্ষ = ফাঁকা;

	সূচক.table_bits = xdl_hashbits(count1);
	সূচক.records_আকার = 1 << সূচক.table_bits;
	যদি (!XDL_CALLOC_ARRAY(সূচক.records, সূচক.records_আকার))
		যাও cleanup;

	সূচক.সারি_map_আকার = count1;
	যদি (!XDL_CALLOC_ARRAY(সূচক.সারি_map, সূচক.সারি_map_আকার))
		যাও cleanup;

	যদি (!XDL_CALLOC_ARRAY(সূচক.পরবর্তী_ptrs, সূচক.সারি_map_আকার))
		যাও cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	যদি (xdl_cha_init(&সূচক.rcha, আকার(কাঠামো record), count1 / 4 + 1) < 0)
		যাও cleanup;

	সূচক.ptr_shift = line1;
	সূচক.max_chain_দৈর্ঘ্য = 64;

	যদি (scanA(&সূচক, line1, count1))
		যাও cleanup;

	সূচক.cnt = সূচক.max_chain_দৈর্ঘ্য + 1;

	জন্য (b_ptr = line2; b_ptr <= সারি_END(2); )
		b_ptr = try_lcs(&সূচক, lcs, b_ptr, line1, count1, line2, count2);

	যদি (সূচক.has_common && সূচক.max_chain_দৈর্ঘ্য < সূচক.cnt)
		ret = 1;
	অন্যথা
		ret = 0;

cleanup:
	free_সূচক(&সূচক);
	ফেরত ret;
}

স্থির পূর্ণসংখ্যা histogram_diff(xpparam_t ধ্রুবক *xpp, xdfenv_t *env,
	পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2)
{
	কাঠামো region lcs;
	পূর্ণসংখ্যা lcs_found;
	পূর্ণসংখ্যা ফলাফল;
redo:
	ফলাফল = -1;

	যদি (count1 <= 0 && count2 <= 0)
		ফেরত 0;

	যদি ((unsigned পূর্ণসংখ্যা)সারি_END(1) >= MAX_PTR)
		ফেরত -1;

	যদি (!count1) {
		যখন(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ফেরত 0;
	} অন্যথা যদি (!count2) {
		যখন(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ফেরত 0;
	}

	memset(&lcs, 0, আকার(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	যদি (lcs_found < 0)
		যাও out;
	অন্যথা যদি (lcs_found)
		ফলাফল = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	অন্যথা {
		যদি (lcs.begin1 == 0 && lcs.begin2 == 0) {
			যখন (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			যখন (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			ফলাফল = 0;
		} অন্যথা {
			ফলাফল = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			যদি (ফলাফল)
				যাও out;
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
			যাও redo;
		}
	}
out:
	ফেরত ফলাফল;
}

পূর্ণসংখ্যা xdl_do_histogram_diff(xpparam_t ধ্রুবক *xpp, xdfenv_t *env)
{
	ফেরত histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
