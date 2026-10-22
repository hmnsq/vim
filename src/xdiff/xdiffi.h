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

#ਜੇ !defined(XDIFFI_H)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XDIFFI_H


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xdalgoenv {
	ਲੰਮਾ mxcost;
	ਲੰਮਾ snake_cnt;
	ਲੰਮਾ heur_min;
} xdalgoenv_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xdchange {
	ਬਣਤਰ s_xdchange *ਅਗਲਾ;
	ਲੰਮਾ i1, i2;
	ਲੰਮਾ chg1, chg2;
	ਪੂਰਨ ignore;
} xdchange_t;



ਪੂਰਨ xdl_recs_cmp(xdfile_t *xdf1, ਲੰਮਾ off1, ਲੰਮਾ lim1,
		 xdfile_t *xdf2, ਲੰਮਾ off2, ਲੰਮਾ lim2,
		 ਲੰਮਾ *kvdf, ਲੰਮਾ *kvdb, ਪੂਰਨ need_min, xdalgoenv_t *xenv);
ਪੂਰਨ xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ਅਚਲ *xpp,
		xdfenv_t *xe);
ਪੂਰਨ xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, ਲੰਮਾ flags);
ਪੂਰਨ xdl_build_script(xdfenv_t *xe, xdchange_t **xscr);
ਖਾਲੀ xdl_free_script(xdchange_t *xscr);
ਪੂਰਨ xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ਅਚਲ *xecfg);
ਪੂਰਨ xdl_do_patience_diff(xpparam_t ਅਚਲ *xpp, xdfenv_t *env);
ਪੂਰਨ xdl_do_histogram_diff(xpparam_t ਅਚਲ *xpp, xdfenv_t *env);

#ਜੇ_ਅੰਤ /* #if !defined(XDIFFI_H) */
