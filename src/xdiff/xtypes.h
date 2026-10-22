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

#إذا !defined(XTYPES_H)
#تعریف_کړه XTYPES_H



تعريفنوع بنية s_chanode {
	بنية s_chanode *بل;
	اوږد icurr;
} chanode_t;

تعريفنوع بنية s_chastore {
	chanode_t *سر, *لکۍ;
	اوږد isize, nsize;
	chanode_t *ancur;
	chanode_t *sncur;
	اوږد scurr;
} chastore_t;

تعريفنوع بنية s_xrecord {
	حرف ثابت *ptr;
	اوږد اندازه;
	بینښه اوږد ha;
} xrecord_t;

تعريفنوع بنية s_xdfile {
	xrecord_t *recs;
	اوږد nrec;
	اوږد dstart, dend;
	bool *بدل_شوی;
	اوږد *rindex;
	اوږد nreff;
} xdfile_t;

تعريفنوع بنية s_xdfenv {
	xdfile_t xdf1, xdf2;
} xdfenv_t;



#د_که_پای /* #if !defined(XTYPES_H) */
