/*
 *  LibXDiff by Davide Libenzi ( File Differential Library )
 *  Copyright (C) 2003  Davide Libenzi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see
 *  <http://www.gnu.org/licenses/>.
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */

#ఒకవేళ !defined(XUTILS_H)
#నిర్వచించు XUTILS_H



దీర్ఘ xdl_bogosqrt(దీర్ఘ n);
పూర్ణాంకం xdl_emit_diffrec(అక్షరం స్థిరరాశి *rec, దీర్ఘ కొలత, అక్షరం స్థిరరాశి *pre, దీర్ఘ psize,
		     xdemitcb_t *ecb);
పూర్ణాంకం xdl_cha_init(chastore_t *cha, దీర్ఘ isize, దీర్ఘ icount);
శూన్య xdl_cha_free(chastore_t *cha);
శూన్య *xdl_cha_alloc(chastore_t *cha);
దీర్ఘ xdl_guess_పంక్తులు(mmfile_t *mf, దీర్ఘ sample);
పూర్ణాంకం xdl_blankline(స్థిరరాశి అక్షరం *పంక్తి, దీర్ఘ కొలత, దీర్ఘ flags);
పూర్ణాంకం xdl_recmatch(స్థిరరాశి అక్షరం *l1, దీర్ఘ s1, స్థిరరాశి అక్షరం *l2, దీర్ఘ s2, దీర్ఘ flags);
గుర్తులేని దీర్ఘ xdl_hash_రికార్డు_verbatim(అక్షరం స్థిరరాశి **దత్తాంశం, అక్షరం స్థిరరాశి *శిఖరం);
గుర్తులేని దీర్ఘ xdl_hash_రికార్డు_with_whitespace(అక్షరం స్థిరరాశి **దత్తాంశం, అక్షరం స్థిరరాశి *శిఖరం, దీర్ఘ flags);
స్థిర అంతఃవరుస గుర్తులేని దీర్ఘ xdl_hash_రికార్డు(అక్షరం స్థిరరాశి **దత్తాంశం, అక్షరం స్థిరరాశి *శిఖరం, దీర్ఘ flags)
{
	ఒకవేళ (flags & XDF_WHITESPACE_FLAGS)
		తిరిగి xdl_hash_రికార్డు_with_whitespace(దత్తాంశం, శిఖరం, flags);
	లేకపోతే
		తిరిగి xdl_hash_రికార్డు_verbatim(దత్తాంశం, శిఖరం);
}
గుర్తులేని పూర్ణాంకం xdl_hashbits(గుర్తులేని పూర్ణాంకం కొలత);
పూర్ణాంకం xdl_అంకె_బహిర్గతం(అక్షరం *బహిర్గతం, దీర్ఘ val);
పూర్ణాంకం xdl_emit_hunk_hdr(దీర్ఘ s1, దీర్ఘ c1, దీర్ఘ s2, దీర్ఘ c2,
		      స్థిరరాశి అక్షరం *func, దీర్ఘ funclen, xdemitcb_t *ecb);
పూర్ణాంకం xdl_fall_వెనుక_diff(xdfenv_t *diff_env, xpparam_t స్థిరరాశి *xpp,
		       పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2);

/* Do not call this function, use XDL_ALLOC_GROW instead */
శూన్య* xdl_alloc_grow_helper(శూన్య* p, దీర్ఘ nr, దీర్ఘ* alloc, size_t కొలత);

#అయితే_ముగింపు /* #if !defined(XUTILS_H) */
