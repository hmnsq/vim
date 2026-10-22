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

#అన్తర్భావ "xinclude.h"

#నిర్వచించు MAX_PTR	UINT_MAX
#నిర్వచించు MAX_CNT	UINT_MAX

#నిర్వచించు పంక్తి_చివర_2(n) (పంక్తి##n + లెక్క##n - 1)
#నిర్వచించు పంక్తి_చివర_PTR(n) (*పంక్తి##n + *లెక్క##n - 1)

నిర్మాణం histindex {
	నిర్మాణం రికార్డు {
		గుర్తులేని పూర్ణాంకం ptr, cnt;
		నిర్మాణం రికార్డు *తదుపరి;
	} **రికార్డులు, /* an occurrence */
	  **పంక్తి_map; /* map of line to record chain */
	chastore_t rcha;
	గుర్తులేని పూర్ణాంకం *తదుపరి_ptrs;
	గుర్తులేని పూర్ణాంకం పట్టిక_బిట్లు,
		     రికార్డులు_కొలత,
		     పంక్తి_map_కొలత;

	గుర్తులేని పూర్ణాంకం max_chain_నిడివి,
		     కీలకం_జరుపు,
		     ptr_జరుపు;

	గుర్తులేని పూర్ణాంకం cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t స్థిరరాశి *xpp;
};

నిర్మాణం region {
	గుర్తులేని పూర్ణాంకం begin1, end1;
	గుర్తులేని పూర్ణాంకం begin2, end2;
};

#నిర్వచించు పంక్తి_MAP(i, a) (i->పంక్తి_map[(a) - i->ptr_జరుపు])

#నిర్వచించు తదుపరి_PTR(సూచీ, ptr) \
	(సూచీ->తదుపరి_ptrs[(ptr) - సూచీ->ptr_జరుపు])

#నిర్వచించు CNT(సూచీ, ptr) \
	((పంక్తి_MAP(సూచీ, ptr))->cnt)

#నిర్వచించు REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

స్థిర పూర్ణాంకం cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	తిరిగి r1->ha == r2->ha;

}

#నిర్వచించు CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#నిర్వచించు పట్టిక_HASH(సూచీ, side, పంక్తి) \
	XDL_HASHLONG((REC(సూచీ->env, side, పంక్తి))->ha, సూచీ->పట్టిక_బిట్లు)

స్థిర పూర్ణాంకం పరిశీలన_A(నిర్మాణం histindex *సూచీ, పూర్ణాంకం line1, పూర్ణాంకం count1)
{
	గుర్తులేని పూర్ణాంకం ptr, tbl_idx;
	గుర్తులేని పూర్ణాంకం chain_len;
	నిర్మాణం రికార్డు **rec_chain, *rec;

	కొరకు (ptr = పంక్తి_చివర_2(1); (గుర్తులేని పూర్ణాంకం)line1 <= ptr; ptr--) {
		tbl_idx = పట్టిక_HASH(సూచీ, 1, ptr);
		rec_chain = సూచీ->రికార్డులు + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		అయితే (rec) {
			ఒకవేళ (CMP(సూచీ, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				తదుపరి_PTR(సూచీ, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				పంక్తి_MAP(సూచీ, ptr) = rec;
				వెళ్ళు continue_పరిశీలన;
			}

			rec = rec->తదుపరి;
			chain_len++;
		}

		ఒకవేళ (chain_len == సూచీ->max_chain_నిడివి)
			తిరిగి -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		ఒకవేళ (!(rec = xdl_cha_alloc(&సూచీ->rcha)))
			తిరిగి -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->తదుపరి = *rec_chain;
		*rec_chain = rec;
		పంక్తి_MAP(సూచీ, ptr) = rec;

continue_పరిశీలన:
		; /* no op */
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం try_lcs(నిర్మాణం histindex *సూచీ, నిర్మాణం region *lcs, పూర్ణాంకం b_ptr,
	పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2)
{
	గుర్తులేని పూర్ణాంకం b_తదుపరి = b_ptr + 1;
	నిర్మాణం రికార్డు *rec = సూచీ->రికార్డులు[పట్టిక_HASH(సూచీ, 2, b_ptr)];
	గుర్తులేని పూర్ణాంకం as, ae, bs, be, np, rc;
	పూర్ణాంకం should_break;

	కొరకు (; rec; rec = rec->తదుపరి) {
		ఒకవేళ (rec->cnt > సూచీ->cnt) {
			ఒకవేళ (!సూచీ->has_common)
				సూచీ->has_common = CMP(సూచీ, 1, rec->ptr, 2, b_ptr);
			కొనసాగు;
		}

		as = rec->ptr;
		ఒకవేళ (!CMP(సూచీ, 1, as, 2, b_ptr))
			కొనసాగు;

		సూచీ->has_common = 1;
		కొరకు (;;) {
			should_break = 0;
			np = తదుపరి_PTR(సూచీ, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			అయితే ((గుర్తులేని పూర్ణాంకం)line1 < as && (గుర్తులేని పూర్ణాంకం)line2 < bs
				&& CMP(సూచీ, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				ఒకవేళ (1 < rc)
					rc = XDL_MIN(rc, CNT(సూచీ, as));
			}
			అయితే (ae < (గుర్తులేని పూర్ణాంకం)పంక్తి_చివర_2(1) && be < (గుర్తులేని పూర్ణాంకం)పంక్తి_చివర_2(2)
				&& CMP(సూచీ, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				ఒకవేళ (1 < rc)
					rc = XDL_MIN(rc, CNT(సూచీ, ae));
			}

			ఒకవేళ (b_తదుపరి <= be)
				b_తదుపరి = be + 1;
			ఒకవేళ (lcs->end1 - lcs->begin1 < ae - as || rc < సూచీ->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				సూచీ->cnt = rc;
			}

			ఒకవేళ (np == 0)
				ఆపు;

			అయితే (np <= ae) {
				np = తదుపరి_PTR(సూచీ, np);
				ఒకవేళ (np == 0) {
					should_break = 1;
					ఆపు;
				}
			}

			ఒకవేళ (should_break)
				ఆపు;

			as = np;
		}
	}
	తిరిగి b_తదుపరి;
}

స్థిర పూర్ణాంకం fall_వెనుక_గమ్యం_classic_diff(xpparam_t స్థిరరాశి *xpp, xdfenv_t *env,
		పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, పరిమాణం(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_ముసుగు;

	తిరిగి xdl_fall_వెనుక_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

స్థిర అంతఃవరుస శూన్య free_సూచీ(నిర్మాణం histindex *సూచీ)
{
	xdl_free(సూచీ->రికార్డులు);
	xdl_free(సూచీ->పంక్తి_map);
	xdl_free(సూచీ->తదుపరి_ptrs);
	xdl_cha_free(&సూచీ->rcha);
}

స్థిర పూర్ణాంకం కనుగొను_lcs(xpparam_t స్థిరరాశి *xpp, xdfenv_t *env,
		    నిర్మాణం region *lcs,
		    పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2)
{
	పూర్ణాంకం b_ptr;
	పూర్ణాంకం ret = -1;
	నిర్మాణం histindex సూచీ;

	memset(&సూచీ, 0, పరిమాణం(సూచీ));

	సూచీ.env = env;
	సూచీ.xpp = xpp;

	సూచీ.రికార్డులు = ఖాళీ;
	సూచీ.పంక్తి_map = ఖాళీ;
	/* in case of early xdl_cha_free() */
	సూచీ.rcha.తల = ఖాళీ;

	సూచీ.పట్టిక_బిట్లు = xdl_hashbits(count1);
	సూచీ.రికార్డులు_కొలత = 1 << సూచీ.పట్టిక_బిట్లు;
	ఒకవేళ (!XDL_CALLOC_ARRAY(సూచీ.రికార్డులు, సూచీ.రికార్డులు_కొలత))
		వెళ్ళు cleanup;

	సూచీ.పంక్తి_map_కొలత = count1;
	ఒకవేళ (!XDL_CALLOC_ARRAY(సూచీ.పంక్తి_map, సూచీ.పంక్తి_map_కొలత))
		వెళ్ళు cleanup;

	ఒకవేళ (!XDL_CALLOC_ARRAY(సూచీ.తదుపరి_ptrs, సూచీ.పంక్తి_map_కొలత))
		వెళ్ళు cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	ఒకవేళ (xdl_cha_init(&సూచీ.rcha, పరిమాణం(నిర్మాణం రికార్డు), count1 / 4 + 1) < 0)
		వెళ్ళు cleanup;

	సూచీ.ptr_జరుపు = line1;
	సూచీ.max_chain_నిడివి = 64;

	ఒకవేళ (పరిశీలన_A(&సూచీ, line1, count1))
		వెళ్ళు cleanup;

	సూచీ.cnt = సూచీ.max_chain_నిడివి + 1;

	కొరకు (b_ptr = line2; b_ptr <= పంక్తి_చివర_2(2); )
		b_ptr = try_lcs(&సూచీ, lcs, b_ptr, line1, count1, line2, count2);

	ఒకవేళ (సూచీ.has_common && సూచీ.max_chain_నిడివి < సూచీ.cnt)
		ret = 1;
	లేకపోతే
		ret = 0;

cleanup:
	free_సూచీ(&సూచీ);
	తిరిగి ret;
}

స్థిర పూర్ణాంకం histogram_diff(xpparam_t స్థిరరాశి *xpp, xdfenv_t *env,
	పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2)
{
	నిర్మాణం region lcs;
	పూర్ణాంకం lcs_దొరికింది;
	పూర్ణాంకం ఫలితం;
redo:
	ఫలితం = -1;

	ఒకవేళ (count1 <= 0 && count2 <= 0)
		తిరిగి 0;

	ఒకవేళ ((గుర్తులేని పూర్ణాంకం)పంక్తి_చివర_2(1) >= MAX_PTR)
		తిరిగి -1;

	ఒకవేళ (!count1) {
		అయితే(count2--)
			env->xdf2.మారింది[line2++ - 1] = true;
		తిరిగి 0;
	} లేకపోతే ఒకవేళ (!count2) {
		అయితే(count1--)
			env->xdf1.మారింది[line1++ - 1] = true;
		తిరిగి 0;
	}

	memset(&lcs, 0, పరిమాణం(lcs));
	lcs_దొరికింది = కనుగొను_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	ఒకవేళ (lcs_దొరికింది < 0)
		వెళ్ళు బహిర్గతం;
	లేకపోతే ఒకవేళ (lcs_దొరికింది)
		ఫలితం = fall_వెనుక_గమ్యం_classic_diff(xpp, env, line1, count1, line2, count2);
	లేకపోతే {
		ఒకవేళ (lcs.begin1 == 0 && lcs.begin2 == 0) {
			అయితే (count1--)
				env->xdf1.మారింది[line1++ - 1] = true;
			అయితే (count2--)
				env->xdf2.మారింది[line2++ - 1] = true;
			ఫలితం = 0;
		} లేకపోతే {
			ఫలితం = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			ఒకవేళ (ఫలితం)
				వెళ్ళు బహిర్గతం;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = పంక్తి_చివర_2(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = పంక్తి_చివర_2(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			వెళ్ళు redo;
		}
	}
బహిర్గతం:
	తిరిగి ఫలితం;
}

పూర్ణాంకం xdl_do_histogram_diff(xpparam_t స్థిరరాశి *xpp, xdfenv_t *env)
{
	తిరిగి histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
