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

#ఒకవేళ !defined(XDIFFI_H)
#నిర్వచించు XDIFFI_H


రకనిర్వచనం నిర్మాణం s_xdalgoenv {
	దీర్ఘ mxcost;
	దీర్ఘ snake_cnt;
	దీర్ఘ heur_min;
} xdalgoenv_t;

రకనిర్వచనం నిర్మాణం s_xdchange {
	నిర్మాణం s_xdchange *తదుపరి;
	దీర్ఘ i1, i2;
	దీర్ఘ chg1, chg2;
	పూర్ణాంకం ignore;
} xdchange_t;



పూర్ణాంకం xdl_recs_cmp(xdfile_t *xdf1, దీర్ఘ off1, దీర్ఘ lim1,
		 xdfile_t *xdf2, దీర్ఘ off2, దీర్ఘ lim2,
		 దీర్ఘ *kvdf, దీర్ఘ *kvdb, పూర్ణాంకం need_min, xdalgoenv_t *xenv);
పూర్ణాంకం xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t స్థిరరాశి *xpp,
		xdfenv_t *xe);
పూర్ణాంకం xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, దీర్ఘ flags);
పూర్ణాంకం xdl_build_script(xdfenv_t *xe, xdchange_t **xscr);
శూన్య xdl_free_script(xdchange_t *xscr);
పూర్ణాంకం xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t స్థిరరాశి *xecfg);
పూర్ణాంకం xdl_do_patience_diff(xpparam_t స్థిరరాశి *xpp, xdfenv_t *env);
పూర్ణాంకం xdl_do_histogram_diff(xpparam_t స్థిరరాశి *xpp, xdfenv_t *env);

#అయితే_ముగింపు /* #if !defined(XDIFFI_H) */
