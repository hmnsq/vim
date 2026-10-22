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

#যদি !defined(XDIFFI_H)
#define XDIFFI_H


প্রকারনির্দেশ সংরচনা s_xdalgoenv {
	long mxcost;
	long snake_cnt;
	long heur_min;
} xdalgoenv_t;

প্রকারনির্দেশ সংরচনা s_xdchange {
	সংরচনা s_xdchange *next;
	long i1, i2;
	long chg1, chg2;
	পূর্ণাঙ্ক ignore;
} xdchange_t;



পূর্ণাঙ্ক xdl_recs_cmp(xdfile_t *xdf1, long off1, long lim1,
		 xdfile_t *xdf2, long off2, long lim2,
		 long *kvdf, long *kvdb, পূর্ণাঙ্ক need_min, xdalgoenv_t *xenv);
পূর্ণাঙ্ক xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ধ্রুৱ *xpp,
		xdfenv_t *xe);
পূর্ণাঙ্ক xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, long flags);
পূর্ণাঙ্ক xdl_build_script(xdfenv_t *xe, xdchange_t **xscr);
শূন্য xdl_free_script(xdchange_t *xscr);
পূর্ণাঙ্ক xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ধ্রুৱ *xecfg);
পূর্ণাঙ্ক xdl_do_patience_diff(xpparam_t ধ্রুৱ *xpp, xdfenv_t *env);
পূর্ণাঙ্ক xdl_do_histogram_diff(xpparam_t ধ্রুৱ *xpp, xdfenv_t *env);

#endif /* #if !defined(XDIFFI_H) */
