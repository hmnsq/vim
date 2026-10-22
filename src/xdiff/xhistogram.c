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

#شامل "xinclude.h"

#تعریف_کرو MAX_PTR	UINT_MAX
#تعریف_کرو MAX_CNT	UINT_MAX

#تعریف_کرو سطر_انتہا_2(n) (سطر##n + گنتی##n - 1)
#تعریف_کرو سطر_انتہا_PTR(n) (*سطر##n + *گنتی##n - 1)

ساخت histindex {
	ساخت ریکارڈ {
		غیرعلامتی صحیح ptr, cnt;
		ساخت ریکارڈ *اگلا;
	} **اندراجات, /* an occurrence */
	  **سطر_map; /* map of line to record chain */
	chastore_t rcha;
	غیرعلامتی صحیح *اگلا_ptrs;
	غیرعلامتی صحیح جدول_بٹ,
		     اندراجات_سائز,
		     سطر_map_سائز;

	غیرعلامتی صحیح max_chain_لمبائی,
		     کلید_سرکاؤ,
		     ptr_سرکاؤ;

	غیرعلامتی صحیح cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ثابت *xpp;
};

ساخت region {
	غیرعلامتی صحیح begin1, end1;
	غیرعلامتی صحیح begin2, end2;
};

#تعریف_کرو سطر_MAP(i, a) (i->سطر_map[(a) - i->ptr_سرکاؤ])

#تعریف_کرو اگلا_PTR(اشاریہ, ptr) \
	(اشاریہ->اگلا_ptrs[(ptr) - اشاریہ->ptr_سرکاؤ])

#تعریف_کرو CNT(اشاریہ, ptr) \
	((سطر_MAP(اشاریہ, ptr))->cnt)

#تعریف_کرو REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

ساکن صحیح cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	واپس r1->ha == r2->ha;

}

#تعریف_کرو CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#تعریف_کرو جدول_HASH(اشاریہ, side, سطر) \
	XDL_HASHLONG((REC(اشاریہ->env, side, سطر))->ha, اشاریہ->جدول_بٹ)

ساکن صحیح جائزہ_A(ساخت histindex *اشاریہ, صحیح line1, صحیح count1)
{
	غیرعلامتی صحیح ptr, tbl_idx;
	غیرعلامتی صحیح chain_len;
	ساخت ریکارڈ **rec_chain, *rec;

	برائے (ptr = سطر_انتہا_2(1); (غیرعلامتی صحیح)line1 <= ptr; ptr--) {
		tbl_idx = جدول_HASH(اشاریہ, 1, ptr);
		rec_chain = اشاریہ->اندراجات + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		جب (rec) {
			اگر (CMP(اشاریہ, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				اگلا_PTR(اشاریہ, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				سطر_MAP(اشاریہ, ptr) = rec;
				جاؤ continue_جائزہ;
			}

			rec = rec->اگلا;
			chain_len++;
		}

		اگر (chain_len == اشاریہ->max_chain_لمبائی)
			واپس -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		اگر (!(rec = xdl_cha_alloc(&اشاریہ->rcha)))
			واپس -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->اگلا = *rec_chain;
		*rec_chain = rec;
		سطر_MAP(اشاریہ, ptr) = rec;

continue_جائزہ:
		; /* no op */
	}

	واپس 0;
}

ساکن صحیح try_lcs(ساخت histindex *اشاریہ, ساخت region *lcs, صحیح b_ptr,
	صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	غیرعلامتی صحیح b_اگلا = b_ptr + 1;
	ساخت ریکارڈ *rec = اشاریہ->اندراجات[جدول_HASH(اشاریہ, 2, b_ptr)];
	غیرعلامتی صحیح as, ae, bs, be, np, rc;
	صحیح should_break;

	برائے (; rec; rec = rec->اگلا) {
		اگر (rec->cnt > اشاریہ->cnt) {
			اگر (!اشاریہ->has_common)
				اشاریہ->has_common = CMP(اشاریہ, 1, rec->ptr, 2, b_ptr);
			جاری;
		}

		as = rec->ptr;
		اگر (!CMP(اشاریہ, 1, as, 2, b_ptr))
			جاری;

		اشاریہ->has_common = 1;
		برائے (;;) {
			should_break = 0;
			np = اگلا_PTR(اشاریہ, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			جب ((غیرعلامتی صحیح)line1 < as && (غیرعلامتی صحیح)line2 < bs
				&& CMP(اشاریہ, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				اگر (1 < rc)
					rc = XDL_MIN(rc, CNT(اشاریہ, as));
			}
			جب (ae < (غیرعلامتی صحیح)سطر_انتہا_2(1) && be < (غیرعلامتی صحیح)سطر_انتہا_2(2)
				&& CMP(اشاریہ, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				اگر (1 < rc)
					rc = XDL_MIN(rc, CNT(اشاریہ, ae));
			}

			اگر (b_اگلا <= be)
				b_اگلا = be + 1;
			اگر (lcs->end1 - lcs->begin1 < ae - as || rc < اشاریہ->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				اشاریہ->cnt = rc;
			}

			اگر (np == 0)
				توڑو;

			جب (np <= ae) {
				np = اگلا_PTR(اشاریہ, np);
				اگر (np == 0) {
					should_break = 1;
					توڑو;
				}
			}

			اگر (should_break)
				توڑو;

			as = np;
		}
	}
	واپس b_اگلا;
}

ساکن صحیح fall_عقب_مقصد_classic_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, حجم(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_نقاب;

	واپس xdl_fall_عقب_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

ساکن اندرونی خالی free_اشاریہ(ساخت histindex *اشاریہ)
{
	xdl_free(اشاریہ->اندراجات);
	xdl_free(اشاریہ->سطر_map);
	xdl_free(اشاریہ->اگلا_ptrs);
	xdl_cha_free(&اشاریہ->rcha);
}

ساکن صحیح ڈھونڈو_lcs(xpparam_t ثابت *xpp, xdfenv_t *env,
		    ساخت region *lcs,
		    صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	صحیح b_ptr;
	صحیح ret = -1;
	ساخت histindex اشاریہ;

	memset(&اشاریہ, 0, حجم(اشاریہ));

	اشاریہ.env = env;
	اشاریہ.xpp = xpp;

	اشاریہ.اندراجات = تہی;
	اشاریہ.سطر_map = تہی;
	/* in case of early xdl_cha_free() */
	اشاریہ.rcha.سر = تہی;

	اشاریہ.جدول_بٹ = xdl_hashbits(count1);
	اشاریہ.اندراجات_سائز = 1 << اشاریہ.جدول_بٹ;
	اگر (!XDL_CALLOC_ARRAY(اشاریہ.اندراجات, اشاریہ.اندراجات_سائز))
		جاؤ cleanup;

	اشاریہ.سطر_map_سائز = count1;
	اگر (!XDL_CALLOC_ARRAY(اشاریہ.سطر_map, اشاریہ.سطر_map_سائز))
		جاؤ cleanup;

	اگر (!XDL_CALLOC_ARRAY(اشاریہ.اگلا_ptrs, اشاریہ.سطر_map_سائز))
		جاؤ cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	اگر (xdl_cha_init(&اشاریہ.rcha, حجم(ساخت ریکارڈ), count1 / 4 + 1) < 0)
		جاؤ cleanup;

	اشاریہ.ptr_سرکاؤ = line1;
	اشاریہ.max_chain_لمبائی = 64;

	اگر (جائزہ_A(&اشاریہ, line1, count1))
		جاؤ cleanup;

	اشاریہ.cnt = اشاریہ.max_chain_لمبائی + 1;

	برائے (b_ptr = line2; b_ptr <= سطر_انتہا_2(2); )
		b_ptr = try_lcs(&اشاریہ, lcs, b_ptr, line1, count1, line2, count2);

	اگر (اشاریہ.has_common && اشاریہ.max_chain_لمبائی < اشاریہ.cnt)
		ret = 1;
	ورنہ
		ret = 0;

cleanup:
	free_اشاریہ(&اشاریہ);
	واپس ret;
}

ساکن صحیح histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
	صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	ساخت region lcs;
	صحیح lcs_ملا;
	صحیح نتیجہ;
redo:
	نتیجہ = -1;

	اگر (count1 <= 0 && count2 <= 0)
		واپس 0;

	اگر ((غیرعلامتی صحیح)سطر_انتہا_2(1) >= MAX_PTR)
		واپس -1;

	اگر (!count1) {
		جب(count2--)
			env->xdf2.بدلا[line2++ - 1] = true;
		واپس 0;
	} ورنہ اگر (!count2) {
		جب(count1--)
			env->xdf1.بدلا[line1++ - 1] = true;
		واپس 0;
	}

	memset(&lcs, 0, حجم(lcs));
	lcs_ملا = ڈھونڈو_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	اگر (lcs_ملا < 0)
		جاؤ برآمد;
	ورنہ اگر (lcs_ملا)
		نتیجہ = fall_عقب_مقصد_classic_diff(xpp, env, line1, count1, line2, count2);
	ورنہ {
		اگر (lcs.begin1 == 0 && lcs.begin2 == 0) {
			جب (count1--)
				env->xdf1.بدلا[line1++ - 1] = true;
			جب (count2--)
				env->xdf2.بدلا[line2++ - 1] = true;
			نتیجہ = 0;
		} ورنہ {
			نتیجہ = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			اگر (نتیجہ)
				جاؤ برآمد;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = سطر_انتہا_2(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = سطر_انتہا_2(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			جاؤ redo;
		}
	}
برآمد:
	واپس نتیجہ;
}

صحیح xdl_do_histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env)
{
	واپس histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
