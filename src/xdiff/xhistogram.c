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

#ਅਨ੍ਤਰ੍ਭਾਵ "xinclude.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_PTR	UINT_MAX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_CNT	UINT_MAX

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਤਰ_ਸਿਰਾ_2(n) (ਸਤਰ##n + ਗਿਣਤੀ_2##n - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਤਰ_ਸਿਰਾ_PTR(n) (*ਸਤਰ##n + *ਗਿਣਤੀ_2##n - 1)

ਬਣਤਰ histindex {
	ਬਣਤਰ ਰਿਕਾਰਡ {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ptr, cnt;
		ਬਣਤਰ ਰਿਕਾਰਡ *ਅਗਲਾ;
	} **ਰਿਕਾਰਡਾਂ, /* an occurrence */
	  **ਸਤਰ_map; /* map of line to record chain */
	chastore_t rcha;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *ਅਗਲਾ_ptrs;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸਾਰਣੀ_ਬਿੱਟ,
		     ਰਿਕਾਰਡਾਂ_ਮਾਪ,
		     ਸਤਰ_map_ਮਾਪ;

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ max_chain_ਲੰਮਾਈ,
		     ਕੁੰਜੀ_ਖਿਸਕਾਓ,
		     ptr_ਖਿਸਕਾਓ;

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ਅਚਲ *xpp;
};

ਬਣਤਰ region {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ begin1, end1;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ begin2, end2;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਤਰ_MAP(i, a) (i->ਸਤਰ_map[(a) - i->ptr_ਖਿਸਕਾਓ])

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਗਲਾ_PTR(ਸੂਚਕਾਂਕ, ptr) \
	(ਸੂਚਕਾਂਕ->ਅਗਲਾ_ptrs[(ptr) - ਸੂਚਕਾਂਕ->ptr_ਖਿਸਕਾਓ])

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CNT(ਸੂਚਕਾਂਕ, ptr) \
	((ਸਤਰ_MAP(ਸੂਚਕਾਂਕ, ptr))->cnt)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

ਸਥਿਰ ਪੂਰਨ cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	ਵਾਪਸ r1->ha == r2->ha;

}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਾਰਣੀ_HASH(ਸੂਚਕਾਂਕ, side, ਸਤਰ) \
	XDL_HASHLONG((REC(ਸੂਚਕਾਂਕ->env, side, ਸਤਰ))->ha, ਸੂਚਕਾਂਕ->ਸਾਰਣੀ_ਬਿੱਟ)

ਸਥਿਰ ਪੂਰਨ ਜਾਂਚ_A(ਬਣਤਰ histindex *ਸੂਚਕਾਂਕ, ਪੂਰਨ line1, ਪੂਰਨ count1)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ptr, tbl_idx;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ chain_len;
	ਬਣਤਰ ਰਿਕਾਰਡ **rec_chain, *rec;

	ਲਈ (ptr = ਸਤਰ_ਸਿਰਾ_2(1); (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)line1 <= ptr; ptr--) {
		tbl_idx = ਸਾਰਣੀ_HASH(ਸੂਚਕਾਂਕ, 1, ptr);
		rec_chain = ਸੂਚਕਾਂਕ->ਰਿਕਾਰਡਾਂ + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		ਜਦੋਂ (rec) {
			ਜੇ (CMP(ਸੂਚਕਾਂਕ, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				ਅਗਲਾ_PTR(ਸੂਚਕਾਂਕ, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				ਸਤਰ_MAP(ਸੂਚਕਾਂਕ, ptr) = rec;
				ਜਾਓ continue_ਜਾਂਚ;
			}

			rec = rec->ਅਗਲਾ;
			chain_len++;
		}

		ਜੇ (chain_len == ਸੂਚਕਾਂਕ->max_chain_ਲੰਮਾਈ)
			ਵਾਪਸ -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		ਜੇ (!(rec = xdl_cha_alloc(&ਸੂਚਕਾਂਕ->rcha)))
			ਵਾਪਸ -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->ਅਗਲਾ = *rec_chain;
		*rec_chain = rec;
		ਸਤਰ_MAP(ਸੂਚਕਾਂਕ, ptr) = rec;

continue_ਜਾਂਚ:
		; /* no op */
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ try_lcs(ਬਣਤਰ histindex *ਸੂਚਕਾਂਕ, ਬਣਤਰ region *lcs, ਪੂਰਨ b_ptr,
	ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ b_ਅਗਲਾ = b_ptr + 1;
	ਬਣਤਰ ਰਿਕਾਰਡ *rec = ਸੂਚਕਾਂਕ->ਰਿਕਾਰਡਾਂ[ਸਾਰਣੀ_HASH(ਸੂਚਕਾਂਕ, 2, b_ptr)];
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ as, ae, bs, be, np, rc;
	ਪੂਰਨ should_break;

	ਲਈ (; rec; rec = rec->ਅਗਲਾ) {
		ਜੇ (rec->cnt > ਸੂਚਕਾਂਕ->cnt) {
			ਜੇ (!ਸੂਚਕਾਂਕ->has_common)
				ਸੂਚਕਾਂਕ->has_common = CMP(ਸੂਚਕਾਂਕ, 1, rec->ptr, 2, b_ptr);
			ਜਾਰੀ;
		}

		as = rec->ptr;
		ਜੇ (!CMP(ਸੂਚਕਾਂਕ, 1, as, 2, b_ptr))
			ਜਾਰੀ;

		ਸੂਚਕਾਂਕ->has_common = 1;
		ਲਈ (;;) {
			should_break = 0;
			np = ਅਗਲਾ_PTR(ਸੂਚਕਾਂਕ, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			ਜਦੋਂ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)line1 < as && (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)line2 < bs
				&& CMP(ਸੂਚਕਾਂਕ, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				ਜੇ (1 < rc)
					rc = XDL_MIN(rc, CNT(ਸੂਚਕਾਂਕ, as));
			}
			ਜਦੋਂ (ae < (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਸਤਰ_ਸਿਰਾ_2(1) && be < (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਸਤਰ_ਸਿਰਾ_2(2)
				&& CMP(ਸੂਚਕਾਂਕ, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				ਜੇ (1 < rc)
					rc = XDL_MIN(rc, CNT(ਸੂਚਕਾਂਕ, ae));
			}

			ਜੇ (b_ਅਗਲਾ <= be)
				b_ਅਗਲਾ = be + 1;
			ਜੇ (lcs->end1 - lcs->begin1 < ae - as || rc < ਸੂਚਕਾਂਕ->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				ਸੂਚਕਾਂਕ->cnt = rc;
			}

			ਜੇ (np == 0)
				ਰੋਕੋ;

			ਜਦੋਂ (np <= ae) {
				np = ਅਗਲਾ_PTR(ਸੂਚਕਾਂਕ, np);
				ਜੇ (np == 0) {
					should_break = 1;
					ਰੋਕੋ;
				}
			}

			ਜੇ (should_break)
				ਰੋਕੋ;

			as = np;
		}
	}
	ਵਾਪਸ b_ਅਗਲਾ;
}

ਸਥਿਰ ਪੂਰਨ fall_ਪਿੱਠ_ਮੰਜ਼ਿਲ_classic_diff(xpparam_t ਅਚਲ *xpp, xdfenv_t *env,
		ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, ਆਕਾਰ(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_ਨਕਾਬ;

	ਵਾਪਸ xdl_fall_ਪਿੱਠ_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ free_ਸੂਚਕਾਂਕ(ਬਣਤਰ histindex *ਸੂਚਕਾਂਕ)
{
	xdl_free(ਸੂਚਕਾਂਕ->ਰਿਕਾਰਡਾਂ);
	xdl_free(ਸੂਚਕਾਂਕ->ਸਤਰ_map);
	xdl_free(ਸੂਚਕਾਂਕ->ਅਗਲਾ_ptrs);
	xdl_cha_free(&ਸੂਚਕਾਂਕ->rcha);
}

ਸਥਿਰ ਪੂਰਨ ਲੱਭੋ_lcs(xpparam_t ਅਚਲ *xpp, xdfenv_t *env,
		    ਬਣਤਰ region *lcs,
		    ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2)
{
	ਪੂਰਨ b_ptr;
	ਪੂਰਨ ret = -1;
	ਬਣਤਰ histindex ਸੂਚਕਾਂਕ;

	memset(&ਸੂਚਕਾਂਕ, 0, ਆਕਾਰ(ਸੂਚਕਾਂਕ));

	ਸੂਚਕਾਂਕ.env = env;
	ਸੂਚਕਾਂਕ.xpp = xpp;

	ਸੂਚਕਾਂਕ.ਰਿਕਾਰਡਾਂ = ਨਿਰਾ;
	ਸੂਚਕਾਂਕ.ਸਤਰ_map = ਨਿਰਾ;
	/* in case of early xdl_cha_free() */
	ਸੂਚਕਾਂਕ.rcha.ਸਿਰ = ਨਿਰਾ;

	ਸੂਚਕਾਂਕ.ਸਾਰਣੀ_ਬਿੱਟ = xdl_hashbits(count1);
	ਸੂਚਕਾਂਕ.ਰਿਕਾਰਡਾਂ_ਮਾਪ = 1 << ਸੂਚਕਾਂਕ.ਸਾਰਣੀ_ਬਿੱਟ;
	ਜੇ (!XDL_CALLOC_ARRAY(ਸੂਚਕਾਂਕ.ਰਿਕਾਰਡਾਂ, ਸੂਚਕਾਂਕ.ਰਿਕਾਰਡਾਂ_ਮਾਪ))
		ਜਾਓ cleanup;

	ਸੂਚਕਾਂਕ.ਸਤਰ_map_ਮਾਪ = count1;
	ਜੇ (!XDL_CALLOC_ARRAY(ਸੂਚਕਾਂਕ.ਸਤਰ_map, ਸੂਚਕਾਂਕ.ਸਤਰ_map_ਮਾਪ))
		ਜਾਓ cleanup;

	ਜੇ (!XDL_CALLOC_ARRAY(ਸੂਚਕਾਂਕ.ਅਗਲਾ_ptrs, ਸੂਚਕਾਂਕ.ਸਤਰ_map_ਮਾਪ))
		ਜਾਓ cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	ਜੇ (xdl_cha_init(&ਸੂਚਕਾਂਕ.rcha, ਆਕਾਰ(ਬਣਤਰ ਰਿਕਾਰਡ), count1 / 4 + 1) < 0)
		ਜਾਓ cleanup;

	ਸੂਚਕਾਂਕ.ptr_ਖਿਸਕਾਓ = line1;
	ਸੂਚਕਾਂਕ.max_chain_ਲੰਮਾਈ = 64;

	ਜੇ (ਜਾਂਚ_A(&ਸੂਚਕਾਂਕ, line1, count1))
		ਜਾਓ cleanup;

	ਸੂਚਕਾਂਕ.cnt = ਸੂਚਕਾਂਕ.max_chain_ਲੰਮਾਈ + 1;

	ਲਈ (b_ptr = line2; b_ptr <= ਸਤਰ_ਸਿਰਾ_2(2); )
		b_ptr = try_lcs(&ਸੂਚਕਾਂਕ, lcs, b_ptr, line1, count1, line2, count2);

	ਜੇ (ਸੂਚਕਾਂਕ.has_common && ਸੂਚਕਾਂਕ.max_chain_ਲੰਮਾਈ < ਸੂਚਕਾਂਕ.cnt)
		ret = 1;
	ਨਹੀਂਤਾਂ
		ret = 0;

cleanup:
	free_ਸੂਚਕਾਂਕ(&ਸੂਚਕਾਂਕ);
	ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ histogram_diff(xpparam_t ਅਚਲ *xpp, xdfenv_t *env,
	ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2)
{
	ਬਣਤਰ region lcs;
	ਪੂਰਨ lcs_ਮਿਲਿਆ;
	ਪੂਰਨ ਨਤੀਜਾ;
redo:
	ਨਤੀਜਾ = -1;

	ਜੇ (count1 <= 0 && count2 <= 0)
		ਵਾਪਸ 0;

	ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਸਤਰ_ਸਿਰਾ_2(1) >= MAX_PTR)
		ਵਾਪਸ -1;

	ਜੇ (!count1) {
		ਜਦੋਂ(count2--)
			env->xdf2.ਬਦਲਿਆ[line2++ - 1] = true;
		ਵਾਪਸ 0;
	} ਨਹੀਂਤਾਂ ਜੇ (!count2) {
		ਜਦੋਂ(count1--)
			env->xdf1.ਬਦਲਿਆ[line1++ - 1] = true;
		ਵਾਪਸ 0;
	}

	memset(&lcs, 0, ਆਕਾਰ(lcs));
	lcs_ਮਿਲਿਆ = ਲੱਭੋ_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	ਜੇ (lcs_ਮਿਲਿਆ < 0)
		ਜਾਓ ਬਾਹਰਲਾ;
	ਨਹੀਂਤਾਂ ਜੇ (lcs_ਮਿਲਿਆ)
		ਨਤੀਜਾ = fall_ਪਿੱਠ_ਮੰਜ਼ਿਲ_classic_diff(xpp, env, line1, count1, line2, count2);
	ਨਹੀਂਤਾਂ {
		ਜੇ (lcs.begin1 == 0 && lcs.begin2 == 0) {
			ਜਦੋਂ (count1--)
				env->xdf1.ਬਦਲਿਆ[line1++ - 1] = true;
			ਜਦੋਂ (count2--)
				env->xdf2.ਬਦਲਿਆ[line2++ - 1] = true;
			ਨਤੀਜਾ = 0;
		} ਨਹੀਂਤਾਂ {
			ਨਤੀਜਾ = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			ਜੇ (ਨਤੀਜਾ)
				ਜਾਓ ਬਾਹਰਲਾ;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = ਸਤਰ_ਸਿਰਾ_2(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = ਸਤਰ_ਸਿਰਾ_2(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			ਜਾਓ redo;
		}
	}
ਬਾਹਰਲਾ:
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ xdl_do_histogram_diff(xpparam_t ਅਚਲ *xpp, xdfenv_t *env)
{
	ਵਾਪਸ histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
