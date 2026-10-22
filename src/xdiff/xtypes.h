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

#जर !defined(XTYPES_H)
#define XTYPES_H



प्रकारव्याख्या संरचना s_chanode {
	संरचना s_chanode *next;
	दीर्घ icurr;
} chanode_t;

प्रकारव्याख्या संरचना s_chastore {
	chanode_t *head, *tail;
	दीर्घ isize, nsize;
	chanode_t *ancur;
	chanode_t *sncur;
	दीर्घ scurr;
} chastore_t;

प्रकारव्याख्या संरचना s_xrecord {
	अक्षर स्थिरांक *ptr;
	दीर्घ size;
	अचिन्हित दीर्घ ha;
} xrecord_t;

प्रकारव्याख्या संरचना s_xdfile {
	xrecord_t *recs;
	दीर्घ nrec;
	दीर्घ dstart, dend;
	bool *changed;
	दीर्घ *rindex;
	दीर्घ nreff;
} xdfile_t;

प्रकारव्याख्या संरचना s_xdfenv {
	xdfile_t xdf1, xdf2;
} xdfenv_t;



#endif /* #if !defined(XTYPES_H) */
