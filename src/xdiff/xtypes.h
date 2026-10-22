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

#যদি !defined(XTYPES_H)
#define XTYPES_H



প্রকারলেখ কাঠামো s_chanode {
	কাঠামো s_chanode *পরবর্তী;
	long icurr;
} chanode_t;

প্রকারলেখ কাঠামো s_chastore {
	chanode_t *শীর্ষ, *লেজ;
	long isize, nsize;
	chanode_t *ancur;
	chanode_t *sncur;
	long scurr;
} chastore_t;

প্রকারলেখ কাঠামো s_xrecord {
	অক্ষর ধ্রুবক *ptr;
	long আকার_2;
	unsigned long ha;
} xrecord_t;

প্রকারলেখ কাঠামো s_xdfile {
	xrecord_t *recs;
	long nrec;
	long dstart, dend;
	bool *changed;
	long *rindex;
	long nreff;
} xdfile_t;

প্রকারলেখ কাঠামো s_xdfenv {
	xdfile_t xdf1, xdf2;
} xdfenv_t;



#endif /* #if !defined(XTYPES_H) */
