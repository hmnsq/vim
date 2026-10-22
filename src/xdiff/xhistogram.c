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

#ಅನ್ತರ್ಭಾವ "xinclude.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_PTR	UINT_MAX
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_CNT	UINT_MAX

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಾಲು_ಕೊನೆ_2(n) (ಸಾಲು##n + ಎಣಿಕೆ_2##n - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಾಲು_ಕೊನೆ_PTR(n) (*ಸಾಲು##n + *ಎಣಿಕೆ_2##n - 1)

ರಚನೆ histindex {
	ರಚನೆ ದಾಖಲೆ {
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ptr, cnt;
		ರಚನೆ ದಾಖಲೆ *ಮುಂದಿನ;
	} **ದಾಖಲೆಗಳು, /* an occurrence */
	  **ಸಾಲು_map; /* map of line to record chain */
	chastore_t rcha;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *ಮುಂದಿನ_ptrs;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು,
		     ದಾಖಲೆಗಳು_ಗಾತ್ರ,
		     ಸಾಲು_map_ಗಾತ್ರ;

	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ max_chain_ಉದ್ದಳತೆ,
		     ಕೀಲಿ_ಸರಿಸು,
		     ptr_ಸರಿಸು;

	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t ಸ್ಥಿರಾಂಕ *xpp;
};

ರಚನೆ region {
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ begin1, end1;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ begin2, end2;
};

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಾಲು_MAP(i, a) (i->ಸಾಲು_map[(a) - i->ptr_ಸರಿಸು])

#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಂದಿನ_PTR(ಸೂಚ್ಯಂಕ, ptr) \
	(ಸೂಚ್ಯಂಕ->ಮುಂದಿನ_ptrs[(ptr) - ಸೂಚ್ಯಂಕ->ptr_ಸರಿಸು])

#ವ್ಯಾಖ್ಯಾನಿಸು CNT(ಸೂಚ್ಯಂಕ, ptr) \
	((ಸಾಲು_MAP(ಸೂಚ್ಯಂಕ, ptr))->cnt)

#ವ್ಯಾಖ್ಯಾನಿಸು REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	ಹಿಂತಿರುಗಿ r1->ha == r2->ha;

}

#ವ್ಯಾಖ್ಯಾನಿಸು CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#ವ್ಯಾಖ್ಯಾನಿಸು ಕೋಷ್ಟಕ_HASH(ಸೂಚ್ಯಂಕ, side, ಸಾಲು) \
	XDL_HASHLONG((REC(ಸೂಚ್ಯಂಕ->env, side, ಸಾಲು))->ha, ಸೂಚ್ಯಂಕ->ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಪರಿಶೀಲನೆ_A(ರಚನೆ histindex *ಸೂಚ್ಯಂಕ, ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ptr, tbl_idx;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ chain_len;
	ರಚನೆ ದಾಖಲೆ **rec_chain, *rec;

	ಗಾಗಿ (ptr = ಸಾಲು_ಕೊನೆ_2(1); (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)line1 <= ptr; ptr--) {
		tbl_idx = ಕೋಷ್ಟಕ_HASH(ಸೂಚ್ಯಂಕ, 1, ptr);
		rec_chain = ಸೂಚ್ಯಂಕ->ದಾಖಲೆಗಳು + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		ಆಗ (rec) {
			ಆದರೆ (CMP(ಸೂಚ್ಯಂಕ, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				ಮುಂದಿನ_PTR(ಸೂಚ್ಯಂಕ, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				ಸಾಲು_MAP(ಸೂಚ್ಯಂಕ, ptr) = rec;
				ಹೋಗು continue_ಪರಿಶೀಲನೆ;
			}

			rec = rec->ಮುಂದಿನ;
			chain_len++;
		}

		ಆದರೆ (chain_len == ಸೂಚ್ಯಂಕ->max_chain_ಉದ್ದಳತೆ)
			ಹಿಂತಿರುಗಿ -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		ಆದರೆ (!(rec = xdl_cha_alloc(&ಸೂಚ್ಯಂಕ->rcha)))
			ಹಿಂತಿರುಗಿ -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->ಮುಂದಿನ = *rec_chain;
		*rec_chain = rec;
		ಸಾಲು_MAP(ಸೂಚ್ಯಂಕ, ptr) = rec;

continue_ಪರಿಶೀಲನೆ:
		; /* no op */
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ try_lcs(ರಚನೆ histindex *ಸೂಚ್ಯಂಕ, ರಚನೆ region *lcs, ಪೂರ್ಣಾಂಕ b_ptr,
	ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ b_ಮುಂದಿನ = b_ptr + 1;
	ರಚನೆ ದಾಖಲೆ *rec = ಸೂಚ್ಯಂಕ->ದಾಖಲೆಗಳು[ಕೋಷ್ಟಕ_HASH(ಸೂಚ್ಯಂಕ, 2, b_ptr)];
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ as, ae, bs, be, np, rc;
	ಪೂರ್ಣಾಂಕ should_break;

	ಗಾಗಿ (; rec; rec = rec->ಮುಂದಿನ) {
		ಆದರೆ (rec->cnt > ಸೂಚ್ಯಂಕ->cnt) {
			ಆದರೆ (!ಸೂಚ್ಯಂಕ->has_common)
				ಸೂಚ್ಯಂಕ->has_common = CMP(ಸೂಚ್ಯಂಕ, 1, rec->ptr, 2, b_ptr);
			ಮುಂದುವರಿ;
		}

		as = rec->ptr;
		ಆದರೆ (!CMP(ಸೂಚ್ಯಂಕ, 1, as, 2, b_ptr))
			ಮುಂದುವರಿ;

		ಸೂಚ್ಯಂಕ->has_common = 1;
		ಗಾಗಿ (;;) {
			should_break = 0;
			np = ಮುಂದಿನ_PTR(ಸೂಚ್ಯಂಕ, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			ಆಗ ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)line1 < as && (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)line2 < bs
				&& CMP(ಸೂಚ್ಯಂಕ, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				ಆದರೆ (1 < rc)
					rc = XDL_MIN(rc, CNT(ಸೂಚ್ಯಂಕ, as));
			}
			ಆಗ (ae < (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಸಾಲು_ಕೊನೆ_2(1) && be < (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಸಾಲು_ಕೊನೆ_2(2)
				&& CMP(ಸೂಚ್ಯಂಕ, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				ಆದರೆ (1 < rc)
					rc = XDL_MIN(rc, CNT(ಸೂಚ್ಯಂಕ, ae));
			}

			ಆದರೆ (b_ಮುಂದಿನ <= be)
				b_ಮುಂದಿನ = be + 1;
			ಆದರೆ (lcs->end1 - lcs->begin1 < ae - as || rc < ಸೂಚ್ಯಂಕ->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				ಸೂಚ್ಯಂಕ->cnt = rc;
			}

			ಆದರೆ (np == 0)
				ನಿಲ್ಲಿಸು;

			ಆಗ (np <= ae) {
				np = ಮುಂದಿನ_PTR(ಸೂಚ್ಯಂಕ, np);
				ಆದರೆ (np == 0) {
					should_break = 1;
					ನಿಲ್ಲಿಸು;
				}
			}

			ಆದರೆ (should_break)
				ನಿಲ್ಲಿಸು;

			as = np;
		}
	}
	ಹಿಂತಿರುಗಿ b_ಮುಂದಿನ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fall_ಹಿಂಭಾಗ_ಗಮ್ಯ_classic_diff(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, xdfenv_t *env,
		ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, ಗಾತ್ರ(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_ಮುಖವಾಡ;

	ಹಿಂತಿರುಗಿ xdl_fall_ಹಿಂಭಾಗ_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ free_ಸೂಚ್ಯಂಕ(ರಚನೆ histindex *ಸೂಚ್ಯಂಕ)
{
	xdl_free(ಸೂಚ್ಯಂಕ->ದಾಖಲೆಗಳು);
	xdl_free(ಸೂಚ್ಯಂಕ->ಸಾಲು_map);
	xdl_free(ಸೂಚ್ಯಂಕ->ಮುಂದಿನ_ptrs);
	xdl_cha_free(&ಸೂಚ್ಯಂಕ->rcha);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_lcs(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, xdfenv_t *env,
		    ರಚನೆ region *lcs,
		    ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2)
{
	ಪೂರ್ಣಾಂಕ b_ptr;
	ಪೂರ್ಣಾಂಕ ret = -1;
	ರಚನೆ histindex ಸೂಚ್ಯಂಕ;

	memset(&ಸೂಚ್ಯಂಕ, 0, ಗಾತ್ರ(ಸೂಚ್ಯಂಕ));

	ಸೂಚ್ಯಂಕ.env = env;
	ಸೂಚ್ಯಂಕ.xpp = xpp;

	ಸೂಚ್ಯಂಕ.ದಾಖಲೆಗಳು = ಖಾಲಿ;
	ಸೂಚ್ಯಂಕ.ಸಾಲು_map = ಖಾಲಿ;
	/* in case of early xdl_cha_free() */
	ಸೂಚ್ಯಂಕ.rcha.ತಲೆ = ಖಾಲಿ;

	ಸೂಚ್ಯಂಕ.ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು = xdl_hashbits(count1);
	ಸೂಚ್ಯಂಕ.ದಾಖಲೆಗಳು_ಗಾತ್ರ = 1 << ಸೂಚ್ಯಂಕ.ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು;
	ಆದರೆ (!XDL_CALLOC_ARRAY(ಸೂಚ್ಯಂಕ.ದಾಖಲೆಗಳು, ಸೂಚ್ಯಂಕ.ದಾಖಲೆಗಳು_ಗಾತ್ರ))
		ಹೋಗು cleanup;

	ಸೂಚ್ಯಂಕ.ಸಾಲು_map_ಗಾತ್ರ = count1;
	ಆದರೆ (!XDL_CALLOC_ARRAY(ಸೂಚ್ಯಂಕ.ಸಾಲು_map, ಸೂಚ್ಯಂಕ.ಸಾಲು_map_ಗಾತ್ರ))
		ಹೋಗು cleanup;

	ಆದರೆ (!XDL_CALLOC_ARRAY(ಸೂಚ್ಯಂಕ.ಮುಂದಿನ_ptrs, ಸೂಚ್ಯಂಕ.ಸಾಲು_map_ಗಾತ್ರ))
		ಹೋಗು cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	ಆದರೆ (xdl_cha_init(&ಸೂಚ್ಯಂಕ.rcha, ಗಾತ್ರ(ರಚನೆ ದಾಖಲೆ), count1 / 4 + 1) < 0)
		ಹೋಗು cleanup;

	ಸೂಚ್ಯಂಕ.ptr_ಸರಿಸು = line1;
	ಸೂಚ್ಯಂಕ.max_chain_ಉದ್ದಳತೆ = 64;

	ಆದರೆ (ಪರಿಶೀಲನೆ_A(&ಸೂಚ್ಯಂಕ, line1, count1))
		ಹೋಗು cleanup;

	ಸೂಚ್ಯಂಕ.cnt = ಸೂಚ್ಯಂಕ.max_chain_ಉದ್ದಳತೆ + 1;

	ಗಾಗಿ (b_ptr = line2; b_ptr <= ಸಾಲು_ಕೊನೆ_2(2); )
		b_ptr = try_lcs(&ಸೂಚ್ಯಂಕ, lcs, b_ptr, line1, count1, line2, count2);

	ಆದರೆ (ಸೂಚ್ಯಂಕ.has_common && ಸೂಚ್ಯಂಕ.max_chain_ಉದ್ದಳತೆ < ಸೂಚ್ಯಂಕ.cnt)
		ret = 1;
	ಇಲ್ಲದಿದ್ದರೆ
		ret = 0;

cleanup:
	free_ಸೂಚ್ಯಂಕ(&ಸೂಚ್ಯಂಕ);
	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ histogram_diff(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, xdfenv_t *env,
	ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2)
{
	ರಚನೆ region lcs;
	ಪೂರ್ಣಾಂಕ lcs_ಸಿಕ್ಕಿತು;
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ;
redo:
	ಫಲಿತಾಂಶ = -1;

	ಆದರೆ (count1 <= 0 && count2 <= 0)
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಸಾಲು_ಕೊನೆ_2(1) >= MAX_PTR)
		ಹಿಂತಿರುಗಿ -1;

	ಆದರೆ (!count1) {
		ಆಗ(count2--)
			env->xdf2.ಬದಲಾಯಿತು[line2++ - 1] = true;
		ಹಿಂತಿರುಗಿ 0;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!count2) {
		ಆಗ(count1--)
			env->xdf1.ಬದಲಾಯಿತು[line1++ - 1] = true;
		ಹಿಂತಿರುಗಿ 0;
	}

	memset(&lcs, 0, ಗಾತ್ರ(lcs));
	lcs_ಸಿಕ್ಕಿತು = ಕಂಡುಹಿಡಿ_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	ಆದರೆ (lcs_ಸಿಕ್ಕಿತು < 0)
		ಹೋಗು ಹೊರಹೋಗು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lcs_ಸಿಕ್ಕಿತು)
		ಫಲಿತಾಂಶ = fall_ಹಿಂಭಾಗ_ಗಮ್ಯ_classic_diff(xpp, env, line1, count1, line2, count2);
	ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (lcs.begin1 == 0 && lcs.begin2 == 0) {
			ಆಗ (count1--)
				env->xdf1.ಬದಲಾಯಿತು[line1++ - 1] = true;
			ಆಗ (count2--)
				env->xdf2.ಬದಲಾಯಿತು[line2++ - 1] = true;
			ಫಲಿತಾಂಶ = 0;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಫಲಿತಾಂಶ = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			ಆದರೆ (ಫಲಿತಾಂಶ)
				ಹೋಗು ಹೊರಹೋಗು;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = ಸಾಲು_ಕೊನೆ_2(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = ಸಾಲು_ಕೊನೆ_2(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			ಹೋಗು redo;
		}
	}
ಹೊರಹೋಗು:
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪೂರ್ಣಾಂಕ xdl_do_histogram_diff(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, xdfenv_t *env)
{
	ಹಿಂತಿರುಗಿ histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
