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

#تعریف_کړه MAX_PTR	UINT_MAX
#تعریف_کړه MAX_CNT	UINT_MAX

#تعریف_کړه کرښه_پای_ټکی_2(n) (کرښه##n + شمېر##n - 1)
#تعریف_کړه کرښه_پای_ټکی_PTR(n) (*کرښه##n + *شمېر##n - 1)

بنية histindex {
	بنية ریکارډ {
		بینښه صحيح ptr, cnt;
		بنية ریکارډ *بل;
	} **ریکارډونه, /* an occurrence */
	  **کرښه_map; /* map of line to record chain */
	chastore_t rcha;
	بینښه صحيح *بل_ptrs;
	بینښه صحيح جدول_بټونه,
		     ریکارډونه_اندازه,
		     کرښه_map_اندازه;

	بینښه صحيح max_chain_اوږدتیا,
		     کلی_ښويدنه,
		     ptr_ښويدنه;

	بینښه صحيح cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ثابت *xpp;
};

بنية region {
	بینښه صحيح begin1, end1;
	بینښه صحيح begin2, end2;
};

#تعریف_کړه کرښه_MAP(i, a) (i->کرښه_map[(a) - i->ptr_ښويدنه])

#تعریف_کړه بل_PTR(شاخص, ptr) \
	(شاخص->بل_ptrs[(ptr) - شاخص->ptr_ښويدنه])

#تعریف_کړه CNT(شاخص, ptr) \
	((کرښه_MAP(شاخص, ptr))->cnt)

#تعریف_کړه REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

ساكن صحيح cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	أرجع r1->ha == r2->ha;

}

#تعریف_کړه CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#تعریف_کړه جدول_HASH(شاخص, side, کرښه) \
	XDL_HASHLONG((REC(شاخص->env, side, کرښه))->ha, شاخص->جدول_بټونه)

ساكن صحيح لټون_A(بنية histindex *شاخص, صحيح line1, صحيح count1)
{
	بینښه صحيح ptr, tbl_idx;
	بینښه صحيح chain_len;
	بنية ریکارډ **rec_chain, *rec;

	لكل (ptr = کرښه_پای_ټکی_2(1); (بینښه صحيح)line1 <= ptr; ptr--) {
		tbl_idx = جدول_HASH(شاخص, 1, ptr);
		rec_chain = شاخص->ریکارډونه + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		طالما (rec) {
			إذا (CMP(شاخص, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				بل_PTR(شاخص, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				کرښه_MAP(شاخص, ptr) = rec;
				اذهب continue_لټون;
			}

			rec = rec->بل;
			chain_len++;
		}

		إذا (chain_len == شاخص->max_chain_اوږدتیا)
			أرجع -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		إذا (!(rec = xdl_cha_alloc(&شاخص->rcha)))
			أرجع -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->بل = *rec_chain;
		*rec_chain = rec;
		کرښه_MAP(شاخص, ptr) = rec;

continue_لټون:
		; /* no op */
	}

	أرجع 0;
}

ساكن صحيح try_lcs(بنية histindex *شاخص, بنية region *lcs, صحيح b_ptr,
	صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	بینښه صحيح b_بل = b_ptr + 1;
	بنية ریکارډ *rec = شاخص->ریکارډونه[جدول_HASH(شاخص, 2, b_ptr)];
	بینښه صحيح as, ae, bs, be, np, rc;
	صحيح should_break;

	لكل (; rec; rec = rec->بل) {
		إذا (rec->cnt > شاخص->cnt) {
			إذا (!شاخص->has_common)
				شاخص->has_common = CMP(شاخص, 1, rec->ptr, 2, b_ptr);
			استمر;
		}

		as = rec->ptr;
		إذا (!CMP(شاخص, 1, as, 2, b_ptr))
			استمر;

		شاخص->has_common = 1;
		لكل (;;) {
			should_break = 0;
			np = بل_PTR(شاخص, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			طالما ((بینښه صحيح)line1 < as && (بینښه صحيح)line2 < bs
				&& CMP(شاخص, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				إذا (1 < rc)
					rc = XDL_MIN(rc, CNT(شاخص, as));
			}
			طالما (ae < (بینښه صحيح)کرښه_پای_ټکی_2(1) && be < (بینښه صحيح)کرښه_پای_ټکی_2(2)
				&& CMP(شاخص, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				إذا (1 < rc)
					rc = XDL_MIN(rc, CNT(شاخص, ae));
			}

			إذا (b_بل <= be)
				b_بل = be + 1;
			إذا (lcs->end1 - lcs->begin1 < ae - as || rc < شاخص->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				شاخص->cnt = rc;
			}

			إذا (np == 0)
				اكسر;

			طالما (np <= ae) {
				np = بل_PTR(شاخص, np);
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
	أرجع b_بل;
}

ساكن صحيح fall_شا_منزل_classic_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, حجم(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_ماسک;

	أرجع xdl_fall_شا_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

ساكن مضمن فراغ free_شاخص(بنية histindex *شاخص)
{
	xdl_free(شاخص->ریکارډونه);
	xdl_free(شاخص->کرښه_map);
	xdl_free(شاخص->بل_ptrs);
	xdl_cha_free(&شاخص->rcha);
}

ساكن صحيح پیدا_کړه_lcs(xpparam_t ثابت *xpp, xdfenv_t *env,
		    بنية region *lcs,
		    صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	صحيح b_ptr;
	صحيح ret = -1;
	بنية histindex شاخص;

	memset(&شاخص, 0, حجم(شاخص));

	شاخص.env = env;
	شاخص.xpp = xpp;

	شاخص.ریکارډونه = عدم;
	شاخص.کرښه_map = عدم;
	/* in case of early xdl_cha_free() */
	شاخص.rcha.سر = عدم;

	شاخص.جدول_بټونه = xdl_hashbits(count1);
	شاخص.ریکارډونه_اندازه = 1 << شاخص.جدول_بټونه;
	إذا (!XDL_CALLOC_ARRAY(شاخص.ریکارډونه, شاخص.ریکارډونه_اندازه))
		اذهب cleanup;

	شاخص.کرښه_map_اندازه = count1;
	إذا (!XDL_CALLOC_ARRAY(شاخص.کرښه_map, شاخص.کرښه_map_اندازه))
		اذهب cleanup;

	إذا (!XDL_CALLOC_ARRAY(شاخص.بل_ptrs, شاخص.کرښه_map_اندازه))
		اذهب cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	إذا (xdl_cha_init(&شاخص.rcha, حجم(بنية ریکارډ), count1 / 4 + 1) < 0)
		اذهب cleanup;

	شاخص.ptr_ښويدنه = line1;
	شاخص.max_chain_اوږدتیا = 64;

	إذا (لټون_A(&شاخص, line1, count1))
		اذهب cleanup;

	شاخص.cnt = شاخص.max_chain_اوږدتیا + 1;

	لكل (b_ptr = line2; b_ptr <= کرښه_پای_ټکی_2(2); )
		b_ptr = try_lcs(&شاخص, lcs, b_ptr, line1, count1, line2, count2);

	إذا (شاخص.has_common && شاخص.max_chain_اوږدتیا < شاخص.cnt)
		ret = 1;
	وإلا
		ret = 0;

cleanup:
	free_شاخص(&شاخص);
	أرجع ret;
}

ساكن صحيح histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
	صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	بنية region lcs;
	صحيح lcs_وموندل;
	صحيح پایله;
redo:
	پایله = -1;

	إذا (count1 <= 0 && count2 <= 0)
		أرجع 0;

	إذا ((بینښه صحيح)کرښه_پای_ټکی_2(1) >= MAX_PTR)
		أرجع -1;

	إذا (!count1) {
		طالما(count2--)
			env->xdf2.بدل_شوی[line2++ - 1] = true;
		أرجع 0;
	} وإلا إذا (!count2) {
		طالما(count1--)
			env->xdf1.بدل_شوی[line1++ - 1] = true;
		أرجع 0;
	}

	memset(&lcs, 0, حجم(lcs));
	lcs_وموندل = پیدا_کړه_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	إذا (lcs_وموندل < 0)
		اذهب بهر;
	وإلا إذا (lcs_وموندل)
		پایله = fall_شا_منزل_classic_diff(xpp, env, line1, count1, line2, count2);
	وإلا {
		إذا (lcs.begin1 == 0 && lcs.begin2 == 0) {
			طالما (count1--)
				env->xdf1.بدل_شوی[line1++ - 1] = true;
			طالما (count2--)
				env->xdf2.بدل_شوی[line2++ - 1] = true;
			پایله = 0;
		} وإلا {
			پایله = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			إذا (پایله)
				اذهب بهر;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = کرښه_پای_ټکی_2(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = کرښه_پای_ټکی_2(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			اذهب redo;
		}
	}
بهر:
	أرجع پایله;
}

صحيح xdl_do_histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env)
{
	أرجع histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
