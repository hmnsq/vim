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

#ఒకవేళ !defined(XTYPES_H)
#నిర్వచించు XTYPES_H



రకనిర్వచనం నిర్మాణం s_chanode {
	నిర్మాణం s_chanode *తదుపరి;
	దీర్ఘ icurr;
} chanode_t;

రకనిర్వచనం నిర్మాణం s_chastore {
	chanode_t *తల, *తోక;
	దీర్ఘ isize, nsize;
	chanode_t *ancur;
	chanode_t *sncur;
	దీర్ఘ scurr;
} chastore_t;

రకనిర్వచనం నిర్మాణం s_xrecord {
	అక్షరం స్థిరరాశి *ptr;
	దీర్ఘ కొలత;
	గుర్తులేని దీర్ఘ ha;
} xrecord_t;

రకనిర్వచనం నిర్మాణం s_xdfile {
	xrecord_t *recs;
	దీర్ఘ nrec;
	దీర్ఘ dstart, dend;
	bool *మారింది;
	దీర్ఘ *rindex;
	దీర్ఘ nreff;
} xdfile_t;

రకనిర్వచనం నిర్మాణం s_xdfenv {
	xdfile_t xdf1, xdf2;
} xdfenv_t;



#అయితే_ముగింపు /* #if !defined(XTYPES_H) */
