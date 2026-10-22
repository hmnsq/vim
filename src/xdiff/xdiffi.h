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

#ᤀᤡ᤺ !defined(XDIFFI_H)
#define XDIFFI_H


typedef sa_nracanaa s_xdalgoenv {
	long mxcost;
	long snake_cnt;
	long heur_min;
} xdalgoenv_t;

typedef sa_nracanaa s_xdchange {
	sa_nracanaa s_xdchange *next;
	long i1, i2;
	long chg1, chg2;
	puur_naa_nka ignore;
} xdchange_t;



puur_naa_nka xdl_recs_cmp(xdfile_t *xdf1, long off1, long lim1,
		 xdfile_t *xdf2, long off2, long lim2,
		 long *kvdf, long *kvdb, puur_naa_nka need_min, xdalgoenv_t *xenv);
puur_naa_nka xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
		xdfenv_t *xe);
puur_naa_nka xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, long flags);
puur_naa_nka xdl_build_script(xdfenv_t *xe, xdchange_t **xscr);
void xdl_free_script(xdchange_t *xscr);
puur_naa_nka xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t const *xecfg);
puur_naa_nka xdl_do_patience_diff(xpparam_t const *xpp, xdfenv_t *env);
puur_naa_nka xdl_do_histogram_diff(xpparam_t const *xpp, xdfenv_t *env);

#endif /* #if !defined(XDIFFI_H) */
