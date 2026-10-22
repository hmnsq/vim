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
#تعریف_کرو XTYPES_H



تعريفنوع بنية s_chanode {
	بنية s_chanode *اگلا;
	لما icurr;
} chanode_t;

تعريفنوع بنية s_chastore {
	chanode_t *سر, *پوچھ;
	لما isize, nsize;
	chanode_t *ancur;
	chanode_t *sncur;
	لما scurr;
} chastore_t;

تعريفنوع بنية s_xrecord {
	حرف ثابت *ptr;
	لما ناپ;
	بنانشانی لما ha;
} xrecord_t;

تعريفنوع بنية s_xdfile {
	xrecord_t *recs;
	لما nrec;
	لما dstart, dend;
	bool *بدلیا;
	لما *rindex;
	لما nreff;
} xdfile_t;

تعريفنوع بنية s_xdfenv {
	xdfile_t xdf1, xdf2;
} xdfenv_t;



#جے_انت /* #if !defined(XTYPES_H) */
