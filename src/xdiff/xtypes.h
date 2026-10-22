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

#اگر !defined(XTYPES_H)
#تعریف_کرو XTYPES_H



قسمتعریف ساخت s_chanode {
	ساخت s_chanode *اگلا;
	طویل icurr;
} chanode_t;

قسمتعریف ساخت s_chastore {
	chanode_t *سر, *دم;
	طویل isize, nsize;
	chanode_t *ancur;
	chanode_t *sncur;
	طویل scurr;
} chastore_t;

قسمتعریف ساخت s_xrecord {
	حرف ثابت *ptr;
	طویل سائز;
	غیرعلامتی طویل ha;
} xrecord_t;

قسمتعریف ساخت s_xdfile {
	xrecord_t *recs;
	طویل nrec;
	طویل dstart, dend;
	bool *بدلا;
	طویل *rindex;
	طویل nreff;
} xdfile_t;

قسمتعریف ساخت s_xdfenv {
	xdfile_t xdf1, xdf2;
} xdfenv_t;



#اگر_ختم /* #if !defined(XTYPES_H) */
