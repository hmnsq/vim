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

#تعریف_کرو MAX_PTR	UINT_MAX
#تعریف_کرو MAX_CNT	UINT_MAX

#تعریف_کرو سطر_سرا_2(n) (سطر##n + گنتی##n - 1)
#تعریف_کرو سطر_سرا_PTR(n) (*سطر##n + *گنتی##n - 1)

بنية histindex {
	بنية ریکارڈ {
		بنانشانی صحيح ptr, cnt;
		بنية ریکارڈ *اگلا;
	} **ریکارڈاں, /* an occurrence */
	  **سطر_map; /* map of line to record chain */
	chastore_t rcha;
	بنانشانی صحيح *اگلا_ptrs;
	بنانشانی صحيح جدول_بٹ,
		     ریکارڈاں_ناپ,
		     سطر_map_ناپ;

	بنانشانی صحيح max_chain_لمائی,
		     کلید_کھسکاؤ,
		     ptr_کھسکاؤ;

	بنانشانی صحيح cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ثابت *xpp;
};

بنية region {
	بنانشانی صحيح begin1, end1;
	بنانشانی صحيح begin2, end2;
};

#تعریف_کرو سطر_MAP(i, a) (i->سطر_map[(a) - i->ptr_کھسکاؤ])

#تعریف_کرو اگلا_PTR(اشاریہ, ptr) \
	(اشاریہ->اگلا_ptrs[(ptr) - اشاریہ->ptr_کھسکاؤ])

#تعریف_کرو CNT(اشاریہ, ptr) \
	((سطر_MAP(اشاریہ, ptr))->cnt)

#تعریف_کرو REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

ساكن صحيح cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	أرجع r1->ha == r2->ha;

}

#تعریف_کرو CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#تعریف_کرو جدول_HASH(اشاریہ, side, سطر) \
	XDL_HASHLONG((REC(اشاریہ->env, side, سطر))->ha, اشاریہ->جدول_بٹ)

ساكن صحيح جانچ_A(بنية histindex *اشاریہ, صحيح line1, صحيح count1)
{
	بنانشانی صحيح ptr, tbl_idx;
	بنانشانی صحيح chain_len;
	بنية ریکارڈ **rec_chain, *rec;

	لكل (ptr = سطر_سرا_2(1); (بنانشانی صحيح)line1 <= ptr; ptr--) {
		tbl_idx = جدول_HASH(اشاریہ, 1, ptr);
		rec_chain = اشاریہ->ریکارڈاں + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		طالما (rec) {
			إذا (CMP(اشاریہ, 1, rec->ptr, 1, ptr)) {
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
				اذهب continue_جانچ;
			}

			rec = rec->اگلا;
			chain_len++;
		}

		إذا (chain_len == اشاریہ->max_chain_لمائی)
			أرجع -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		إذا (!(rec = xdl_cha_alloc(&اشاریہ->rcha)))
			أرجع -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->اگلا = *rec_chain;
		*rec_chain = rec;
		سطر_MAP(اشاریہ, ptr) = rec;

continue_جانچ:
		; /* no op */
	}

	أرجع 0;
}

ساكن صحيح try_lcs(بنية histindex *اشاریہ, بنية region *lcs, صحيح b_ptr,
	صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	بنانشانی صحيح b_اگلا = b_ptr + 1;
	بنية ریکارڈ *rec = اشاریہ->ریکارڈاں[جدول_HASH(اشاریہ, 2, b_ptr)];
	بنانشانی صحيح as, ae, bs, be, np, rc;
	صحيح should_break;

	لكل (; rec; rec = rec->اگلا) {
		إذا (rec->cnt > اشاریہ->cnt) {
			إذا (!اشاریہ->has_common)
				اشاریہ->has_common = CMP(اشاریہ, 1, rec->ptr, 2, b_ptr);
			استمر;
		}

		as = rec->ptr;
		إذا (!CMP(اشاریہ, 1, as, 2, b_ptr))
			استمر;

		اشاریہ->has_common = 1;
		لكل (;;) {
			should_break = 0;
			np = اگلا_PTR(اشاریہ, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			طالما ((بنانشانی صحيح)line1 < as && (بنانشانی صحيح)line2 < bs
				&& CMP(اشاریہ, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				إذا (1 < rc)
					rc = XDL_MIN(rc, CNT(اشاریہ, as));
			}
			طالما (ae < (بنانشانی صحيح)سطر_سرا_2(1) && be < (بنانشانی صحيح)سطر_سرا_2(2)
				&& CMP(اشاریہ, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				إذا (1 < rc)
					rc = XDL_MIN(rc, CNT(اشاریہ, ae));
			}

			إذا (b_اگلا <= be)
				b_اگلا = be + 1;
			إذا (lcs->end1 - lcs->begin1 < ae - as || rc < اشاریہ->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				اشاریہ->cnt = rc;
			}

			إذا (np == 0)
				اكسر;

			طالما (np <= ae) {
				np = اگلا_PTR(اشاریہ, np);
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
	أرجع b_اگلا;
}

ساكن صحيح fall_پٹھ_منزل_classic_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, حجم(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_نقاب;

	أرجع xdl_fall_پٹھ_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

ساكن مضمن فراغ free_اشاریہ(بنية histindex *اشاریہ)
{
	xdl_free(اشاریہ->ریکارڈاں);
	xdl_free(اشاریہ->سطر_map);
	xdl_free(اشاریہ->اگلا_ptrs);
	xdl_cha_free(&اشاریہ->rcha);
}

ساكن صحيح لبھو_lcs(xpparam_t ثابت *xpp, xdfenv_t *env,
		    بنية region *lcs,
		    صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	صحيح b_ptr;
	صحيح ret = -1;
	بنية histindex اشاریہ;

	memset(&اشاریہ, 0, حجم(اشاریہ));

	اشاریہ.env = env;
	اشاریہ.xpp = xpp;

	اشاریہ.ریکارڈاں = عدم;
	اشاریہ.سطر_map = عدم;
	/* in case of early xdl_cha_free() */
	اشاریہ.rcha.سر = عدم;

	اشاریہ.جدول_بٹ = xdl_hashbits(count1);
	اشاریہ.ریکارڈاں_ناپ = 1 << اشاریہ.جدول_بٹ;
	إذا (!XDL_CALLOC_ARRAY(اشاریہ.ریکارڈاں, اشاریہ.ریکارڈاں_ناپ))
		اذهب cleanup;

	اشاریہ.سطر_map_ناپ = count1;
	إذا (!XDL_CALLOC_ARRAY(اشاریہ.سطر_map, اشاریہ.سطر_map_ناپ))
		اذهب cleanup;

	إذا (!XDL_CALLOC_ARRAY(اشاریہ.اگلا_ptrs, اشاریہ.سطر_map_ناپ))
		اذهب cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	إذا (xdl_cha_init(&اشاریہ.rcha, حجم(بنية ریکارڈ), count1 / 4 + 1) < 0)
		اذهب cleanup;

	اشاریہ.ptr_کھسکاؤ = line1;
	اشاریہ.max_chain_لمائی = 64;

	إذا (جانچ_A(&اشاریہ, line1, count1))
		اذهب cleanup;

	اشاریہ.cnt = اشاریہ.max_chain_لمائی + 1;

	لكل (b_ptr = line2; b_ptr <= سطر_سرا_2(2); )
		b_ptr = try_lcs(&اشاریہ, lcs, b_ptr, line1, count1, line2, count2);

	إذا (اشاریہ.has_common && اشاریہ.max_chain_لمائی < اشاریہ.cnt)
		ret = 1;
	وإلا
		ret = 0;

cleanup:
	free_اشاریہ(&اشاریہ);
	أرجع ret;
}

ساكن صحيح histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
	صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	بنية region lcs;
	صحيح lcs_لبھیا;
	صحيح نتیجہ;
redo:
	نتیجہ = -1;

	إذا (count1 <= 0 && count2 <= 0)
		أرجع 0;

	إذا ((بنانشانی صحيح)سطر_سرا_2(1) >= MAX_PTR)
		أرجع -1;

	إذا (!count1) {
		طالما(count2--)
			env->xdf2.بدلیا[line2++ - 1] = true;
		أرجع 0;
	} وإلا إذا (!count2) {
		طالما(count1--)
			env->xdf1.بدلیا[line1++ - 1] = true;
		أرجع 0;
	}

	memset(&lcs, 0, حجم(lcs));
	lcs_لبھیا = لبھو_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	إذا (lcs_لبھیا < 0)
		اذهب باہرلا;
	وإلا إذا (lcs_لبھیا)
		نتیجہ = fall_پٹھ_منزل_classic_diff(xpp, env, line1, count1, line2, count2);
	وإلا {
		إذا (lcs.begin1 == 0 && lcs.begin2 == 0) {
			طالما (count1--)
				env->xdf1.بدلیا[line1++ - 1] = true;
			طالما (count2--)
				env->xdf2.بدلیا[line2++ - 1] = true;
			نتیجہ = 0;
		} وإلا {
			نتیجہ = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			إذا (نتیجہ)
				اذهب باہرلا;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = سطر_سرا_2(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = سطر_سرا_2(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			اذهب redo;
		}
	}
باہرلا:
	أرجع نتیجہ;
}

صحيح xdl_do_histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env)
{
	أرجع histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
