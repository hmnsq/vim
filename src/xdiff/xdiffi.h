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

#ಆದರೆ !defined(XDIFFI_H)
#ವ್ಯಾಖ್ಯಾನಿಸು XDIFFI_H


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xdalgoenv {
	ದೀರ್ಘ mxcost;
	ದೀರ್ಘ snake_cnt;
	ದೀರ್ಘ heur_min;
} xdalgoenv_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xdchange {
	ರಚನೆ s_xdchange *ಮುಂದಿನ;
	ದೀರ್ಘ i1, i2;
	ದೀರ್ಘ chg1, chg2;
	ಪೂರ್ಣಾಂಕ ignore;
} xdchange_t;



ಪೂರ್ಣಾಂಕ xdl_recs_cmp(xdfile_t *xdf1, ದೀರ್ಘ off1, ದೀರ್ಘ lim1,
		 xdfile_t *xdf2, ದೀರ್ಘ off2, ದೀರ್ಘ lim2,
		 ದೀರ್ಘ *kvdf, ದೀರ್ಘ *kvdb, ಪೂರ್ಣಾಂಕ need_min, xdalgoenv_t *xenv);
ಪೂರ್ಣಾಂಕ xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ಸ್ಥಿರಾಂಕ *xpp,
		xdfenv_t *xe);
ಪೂರ್ಣಾಂಕ xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, ದೀರ್ಘ flags);
ಪೂರ್ಣಾಂಕ xdl_build_script(xdfenv_t *xe, xdchange_t **xscr);
ಶೂನ್ಯ xdl_free_script(xdchange_t *xscr);
ಪೂರ್ಣಾಂಕ xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ಸ್ಥಿರಾಂಕ *xecfg);
ಪೂರ್ಣಾಂಕ xdl_do_patience_diff(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, xdfenv_t *env);
ಪೂರ್ಣಾಂಕ xdl_do_histogram_diff(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, xdfenv_t *env);

#ಆದರೆ_ಅಂತ್ಯ /* #if !defined(XDIFFI_H) */
