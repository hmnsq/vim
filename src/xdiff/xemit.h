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

#ఒకవేళ !defined(XEMIT_H)
#నిర్వచించు XEMIT_H


రకనిర్వచనం పూర్ణాంకం (*emit_func_t)(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
			   xdemitconf_t స్థిరరాశి *xecfg);

xdchange_t *xdl_get_hunk(xdchange_t **xscr, xdemitconf_t స్థిరరాశి *xecfg);
పూర్ణాంకం xdl_emit_diff(xdfenv_t *xe, xdchange_t *xscr, xdemitcb_t *ecb,
		  xdemitconf_t స్థిరరాశి *xecfg);



#అయితే_ముగింపు /* #if !defined(XEMIT_H) */
