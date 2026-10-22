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

#اگر !defined(XDIFFI_H)
#تعریف_کرو XDIFFI_H


قسمتعریف ساخت s_xdalgoenv {
	طویل mxcost;
	طویل snake_cnt;
	طویل heur_min;
} xdalgoenv_t;

قسمتعریف ساخت s_xdchange {
	ساخت s_xdchange *اگلا;
	طویل i1, i2;
	طویل chg1, chg2;
	صحیح ignore;
} xdchange_t;



صحیح xdl_recs_cmp(xdfile_t *xdf1, طویل off1, طویل lim1,
		 xdfile_t *xdf2, طویل off2, طویل lim2,
		 طویل *kvdf, طویل *kvdb, صحیح need_min, xdalgoenv_t *xenv);
صحیح xdl_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t ثابت *xpp,
		xdfenv_t *xe);
صحیح xdl_change_compact(xdfile_t *xdf, xdfile_t *xdfo, طویل flags);
صحیح xdl_build_script(xdfenv_t *xe, xdchange_t **xscr);
خالی xdl_free_script(xdchange_t *xscr);
صحیح xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t ثابت *xecfg);
صحیح xdl_do_patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env);
صحیح xdl_do_histogram_diff(xpparam_t ثابت *xpp, xdfenv_t *env);

#اگر_ختم /* #if !defined(XDIFFI_H) */
