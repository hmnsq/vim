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

#ਜੇ !defined(XTYPES_H)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XTYPES_H



ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_chanode {
	ਬਣਤਰ s_chanode *ਅਗਲਾ;
	ਲੰਮਾ icurr;
} chanode_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_chastore {
	chanode_t *ਸਿਰ, *ਪੂਛ;
	ਲੰਮਾ isize, nsize;
	chanode_t *ancur;
	chanode_t *sncur;
	ਲੰਮਾ scurr;
} chastore_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xrecord {
	ਅੱਖਰ ਅਚਲ *ptr;
	ਲੰਮਾ ਮਾਪ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ha;
} xrecord_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xdfile {
	xrecord_t *recs;
	ਲੰਮਾ nrec;
	ਲੰਮਾ dstart, dend;
	bool *ਬਦਲਿਆ;
	ਲੰਮਾ *rindex;
	ਲੰਮਾ nreff;
} xdfile_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ s_xdfenv {
	xdfile_t xdf1, xdf2;
} xdfenv_t;



#ਜੇ_ਅੰਤ /* #if !defined(XTYPES_H) */
