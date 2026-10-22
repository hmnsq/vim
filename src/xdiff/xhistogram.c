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

#وصف_ڪريو MAX_PTR	UINT_MAX
#وصف_ڪريو MAX_CNT	UINT_MAX

#وصف_ڪريو سٽ_پڇاڙي_2(n) (سٽ##n + ڳڻپ##n - 1)
#وصف_ڪريو سٽ_پڇاڙي_PTR(n) (*سٽ##n + *ڳڻپ##n - 1)

بنية histindex {
	بنية رڪارڊ {
		بنانشان صحيح ptr, cnt;
		بنية رڪارڊ *ايندڙ;
	} **رڪارڊون, /* an occurrence */
	  **سٽ_map; /* map of line to record chain */
	chastore_t rcha;
	بنانشان صحيح *ايندڙ_ptrs;
	بنانشان صحيح جدول_بٽ,
		     رڪارڊون_سائيز,
		     سٽ_map_سائيز;

	بنانشان صحيح max_chain_ڊيگهائي,
		     ڪنجي_سرڪايو,
		     ptr_سرڪايو;

	بنانشان صحيح cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ثابت *xpp;
};

بنية region {
	بنانشان صحيح begin1, end1;
	بنانشان صحيح begin2, end2;
};

#وصف_ڪريو سٽ_MAP(i, a) (i->سٽ_map[(a) - i->ptr_سرڪايو])

#وصف_ڪريو ايندڙ_PTR(اشاريو, ptr) \
	(اشاريو->ايندڙ_ptrs[(ptr) - اشاريو->ptr_سرڪايو])

#وصف_ڪريو CNT(اشاريو, ptr) \
	((سٽ_MAP(اشاريو, ptr))->cnt)

#وصف_ڪريو REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

ساكن صحيح cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	أرجع r1->ha == r2->ha;

}

#وصف_ڪريو CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#وصف_ڪريو جدول_HASH(اشاريو, side, سٽ) \
	XDL_HASHLONG((REC(اشاريو->env, side, سٽ))->ha, اشاريو->جدول_بٽ)

ساكن صحيح جاچ_A(بنية histindex *اشاريو, صحيح line1, صحيح count1)
{
	بنانشان صحيح ptr, tbl_idx;
	بنانشان صحيح chain_len;
	بنية رڪارڊ **rec_chain, *rec;

	لكل (ptr = سٽ_پڇاڙي_2(1); (بنانشان صحيح)line1 <= ptr; ptr--) {
		tbl_idx = جدول_HASH(اشاريو, 1, ptr);
		rec_chain = اشاريو->رڪارڊون + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		طالما (rec) {
			إذا (CMP(اشاريو, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				ايندڙ_PTR(اشاريو, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				سٽ_MAP(اشاريو, ptr) = rec;
				اذهب continue_جاچ;
			}

			rec = rec->ايندڙ;
			chain_len++;
		}

		إذا (chain_len == اشاريو->max_chain_ڊيگهائي)
			أرجع -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		إذا (!(rec = xdl_cha_alloc(&اشاريو->rcha)))
			أرجع -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->ايندڙ = *rec_chain;
		*rec_chain = rec;
		سٽ_MAP(اشاريو, ptr) = rec;

continue_جاچ:
		; /* no op */
	}

	أرجع 0;
}

ساكن صحيح try_lcs(بنية histindex *اشاريو, بنية region *lcs, صحيح b_ptr,
	صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	بنانشان صحيح b_ايندڙ = b_ptr + 1;
	بنية رڪارڊ *rec = اشاريو->رڪارڊون[جدول_HASH(اشاريو, 2, b_ptr)];
	بنانشان صحيح as, ae, bs, be, np, rc;
	صحيح should_break;

	لكل (; rec; rec = rec->ايندڙ) {
		إذا (rec->cnt > اشاريو->cnt) {
			إذا (!اشاريو->has_common)
				اشاريو->has_common = CMP(اشاريو, 1, rec->ptr, 2, b_ptr);
			استمر;
		}

		as = rec->ptr;
		إذا (!CMP(اشاريو, 1, as, 2, b_ptr))
			استمر;

		اشاريو->has_common = 1;
		لكل (;;) {
			should_break = 0;
			np = ايندڙ_PTR(اشاريو, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			طالما ((بنانشان صحيح)line1 < as && (بنانشان صحيح)line2 < bs
				&& CMP(اشاريو, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				إذا (1 < rc)
					rc = XDL_MIN(rc, CNT(اشاريو, as));
			}
			طالما (ae < (بنانشان صحيح)سٽ_پڇاڙي_2(1) && be < (بنانشان صحيح)سٽ_پڇاڙي_2(2)
				&& CMP(اشاريو, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				إذا (1 < rc)
					rc = XDL_MIN(rc, CNT(اشاريو, ae));
			}

			إذا (b_ايندڙ <= be)
				b_ايندڙ = be + 1;
			إذا (lcs->end1 - lcs->begin1 < ae - as || rc < اشاريو->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				اشاريو->cnt = rc;
			}

			إذا (np == 0)
				اكسر;

			طالما (np <= ae) {
				np = ايندڙ_PTR(اشاريو, np);
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
	أرجع b_ايندڙ;
}

ساكن صحيح fall_پٺ_منزل_classic_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, حجم(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_نقاب;

	أرجع xdl_fall_پٺ_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

ساكن مضمن فراغ free_اشاريو(بنية histindex *اشاريو)
{
	xdl_free(اشاريو->رڪارڊون);
	xdl_free(اشاريو->سٽ_map);
	xdl_free(اشاريو->ايندڙ_ptrs);
	xdl_cha_free(&اشاريو->rcha);
}

ساكن صحيح ڳوليو_lcs(xpparam_t ثابت *xpp, xdfenv_t *env,
		    بنية region *lcs,
		    صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	صحيح b_ptr;
	صحيح ret = -1;
	بنية histindex اشاريو;

	memset(&اشاريو, 0, حجم(اشاريو));

	اشاريو.env = env;
	اشاريو.xpp = xpp;

	اشاريو.رڪارڊون = عدم;
	اشاريو.سٽ_map = عدم;
	/* in case of early xdl_cha_free() */
	اشاريو.rcha.سر = عدم;

	اشاريو.جدول_بٽ = xdl_hashbits(count1);
	اشاريو.رڪارڊون_سائيز = 1 << اشاريو.جدول_بٽ;
	إذا (!XDL_CALLOC_ARRAY(اشاريو.رڪارڊون, اشاريو.رڪارڊون_سائيز))
		اذهب cleanup;

	اشاريو.سٽ_map_سائيز = count1;
	إذا (!XDL_CALLOC_ARRAY(اشاريو.سٽ_map, اشاريو.سٽ_map_سائيز))
		اذهب cleanup;

	إذا (!XDL_CALLOC_ARRAY(اشاريو.ايندڙ_ptrs, اشاريو.سٽ_map_سائيز))
		اذهب cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	إذا (xdl_cha_init(&اشاريو.rcha, حجم(بنية رڪارڊ), count1 / 4 + 1) < 0)
		اذهب cleanup;

	اشاريو.ptr_سرڪايو = line1;
	اشاريو.max_chain_ڊيگهائي = 64;

	إذا (جاچ_A(&اشاريو, line1, count1))
		اذهب cleanup;

	اشاريو.cnt = اشاريو.max_chain_ڊيگهائي + 1;

	لكل (b_ptr = line2; b_ptr <= سٽ_پڇاڙي_2(2); )
		b_ptr = try_lcs(&اشاريو, lcs, b_ptr, line1, count1, line2, count2);

	إذا (اشاريو.has_common && اشاريو.max_chain_ڊيگهائي < اشاريو.cnt)
		ret = 1;
	وإلا
		ret = 0;

cleanup:
	free_اشاريو(&اشاريو);
	أرجع ret;
}

ساكن صحيح histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
	صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	بنية region lcs;
	صحيح lcs_مليو;
	صحيح نتيجو;
redo:
	نتيجو = -1;

	إذا (count1 <= 0 && count2 <= 0)
		أرجع 0;

	إذا ((بنانشان صحيح)سٽ_پڇاڙي_2(1) >= MAX_PTR)
		أرجع -1;

	إذا (!count1) {
		طالما(count2--)
			env->xdf2.بدليو[line2++ - 1] = true;
		أرجع 0;
	} وإلا إذا (!count2) {
		طالما(count1--)
			env->xdf1.بدليو[line1++ - 1] = true;
		أرجع 0;
	}

	memset(&lcs, 0, حجم(lcs));
	lcs_مليو = ڳوليو_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	إذا (lcs_مليو < 0)
		اذهب ٻاهريون;
	وإلا إذا (lcs_مليو)
		نتيجو = fall_پٺ_منزل_classic_diff(xpp, env, line1, count1, line2, count2);
	وإلا {
		إذا (lcs.begin1 == 0 && lcs.begin2 == 0) {
			طالما (count1--)
				env->xdf1.بدليو[line1++ - 1] = true;
			طالما (count2--)
				env->xdf2.بدليو[line2++ - 1] = true;
			نتيجو = 0;
		} وإلا {
			نتيجو = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			إذا (نتيجو)
				اذهب ٻاهريون;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = سٽ_پڇاڙي_2(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = سٽ_پڇاڙي_2(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			اذهب redo;
		}
	}
ٻاهريون:
	أرجع نتيجو;
}

صحيح xdl_do_histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env)
{
	أرجع histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
