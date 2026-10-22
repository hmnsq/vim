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

#ಆದರೆ !defined(XTYPES_H)
#ವ್ಯಾಖ್ಯಾನಿಸು XTYPES_H



ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_chanode {
	ರಚನೆ s_chanode *ಮುಂದಿನ;
	ದೀರ್ಘ icurr;
} chanode_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_chastore {
	chanode_t *ತಲೆ, *ಬಾಲ;
	ದೀರ್ಘ isize, nsize;
	chanode_t *ancur;
	chanode_t *sncur;
	ದೀರ್ಘ scurr;
} chastore_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xrecord {
	ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *ptr;
	ದೀರ್ಘ ಗಾತ್ರ_2;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ha;
} xrecord_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xdfile {
	xrecord_t *recs;
	ದೀರ್ಘ nrec;
	ದೀರ್ಘ dstart, dend;
	bool *ಬದಲಾಯಿತು;
	ದೀರ್ಘ *rindex;
	ದೀರ್ಘ nreff;
} xdfile_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ s_xdfenv {
	xdfile_t xdf1, xdf2;
} xdfenv_t;



#ಆದರೆ_ಅಂತ್ಯ /* #if !defined(XTYPES_H) */
