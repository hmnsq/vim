/*
 *  LibXDiff by Davide Libenzi ( File Differential Library )
 *  Copyright (C) 2003	Davide Libenzi
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

#ಅನ್ತರ್ಭಾವ "xinclude.h"


ದೀರ್ಘ xdl_bogosqrt(ದೀರ್ಘ n) {
	ದೀರ್ಘ i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	ಗಾಗಿ (i = 1; n > 0; n >>= 2)
		i <<= 1;

	ಹಿಂತಿರುಗಿ i;
}


ಪೂರ್ಣಾಂಕ xdl_emit_diffrec(ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *rec, ದೀರ್ಘ ಗಾತ್ರ_2, ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *pre, ದೀರ್ಘ psize,
		     xdemitcb_t *ecb) {
	ಪೂರ್ಣಾಂಕ i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (ಅಕ್ಷರ *) pre;
	mb[0].ಗಾತ್ರ_2 = psize;
	mb[1].ptr = (ಅಕ್ಷರ *) rec;
	mb[1].ಗಾತ್ರ_2 = ಗಾತ್ರ_2;
	ಆದರೆ (ಗಾತ್ರ_2 > 0 && rec[ಗಾತ್ರ_2 - 1] != '\n') {
		mb[2].ptr = (ಅಕ್ಷರ *) "\n\\ No newline at end of file\n";
		mb[2].ಗಾತ್ರ_2 = (ದೀರ್ಘ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(mb[2].ptr);
		i++;
	}
	ಆದರೆ (ecb->ಹೊರಹೋಗು_ಸಾಲು(ecb->priv, mb, i) < 0) {

		ಹಿಂತಿರುಗಿ -1;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ *xdl_mmfile_ಮೊದಲ(mmfile_t *mmf, ದೀರ್ಘ *ಗಾತ್ರ_2)
{
	*ಗಾತ್ರ_2 = mmf->ಗಾತ್ರ_2;
	ಹಿಂತಿರುಗಿ mmf->ptr;
}


ದೀರ್ಘ xdl_mmfile_ಗಾತ್ರ(mmfile_t *mmf)
{
	ಹಿಂತಿರುಗಿ mmf->ಗಾತ್ರ_2;
}


ಪೂರ್ಣಾಂಕ xdl_cha_init(chastore_t *cha, ದೀರ್ಘ isize, ದೀರ್ಘ icount) {

	cha->ತಲೆ = cha->ಬಾಲ = ಖಾಲಿ;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = ಖಾಲಿ;
	cha->scurr = 0;

	ಹಿಂತಿರುಗಿ 0;
}


ಶೂನ್ಯ xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	ಗಾಗಿ (cur = cha->ತಲೆ; (tmp = cur) != ಖಾಲಿ;) {
		cur = cur->ಮುಂದಿನ;
		xdl_free(tmp);
	}
}


ಶೂನ್ಯ *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	ಶೂನ್ಯ *ದತ್ತಾಂಶ;

	ಆದರೆ (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		ಆದರೆ (!(ancur = (chanode_t *) xdl_malloc(ಗಾತ್ರ(chanode_t) + cha->nsize))) {

			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ancur->icurr = 0;
		ancur->ಮುಂದಿನ = ಖಾಲಿ;
		ಆದರೆ (cha->ಬಾಲ)
			cha->ಬಾಲ->ಮುಂದಿನ = ancur;
		ಆದರೆ (!cha->ತಲೆ)
			cha->ತಲೆ = ancur;
		cha->ಬಾಲ = ancur;
		cha->ancur = ancur;
	}

	ದತ್ತಾಂಶ = (ಅಕ್ಷರ *) ancur + ಗಾತ್ರ(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

ದೀರ್ಘ xdl_guess_ಸಾಲುಗಳು(mmfile_t *mf, ದೀರ್ಘ sample) {
	ದೀರ್ಘ nl = 0, ಗಾತ್ರ_2, tsize = 0;
	ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *ದತ್ತಾಂಶ, *cur, *ತುದಿ;

	ಆದರೆ ((cur = ದತ್ತಾಂಶ = xdl_mmfile_ಮೊದಲ(mf, &ಗಾತ್ರ_2))) {
		ಗಾಗಿ (ತುದಿ = ದತ್ತಾಂಶ + ಗಾತ್ರ_2; nl < sample && cur < ತುದಿ; ) {
			nl++;
			ಆದರೆ (!(cur = memchr(cur, '\n', ತುದಿ - cur)))
				cur = ತುದಿ;
			ಇಲ್ಲದಿದ್ದರೆ
				cur++;
		}
		tsize += (ದೀರ್ಘ) (cur - ದತ್ತಾಂಶ);
	}

	ಆದರೆ (nl && tsize)
		nl = xdl_mmfile_ಗಾತ್ರ(mf) / (tsize / nl);

	ಹಿಂತಿರುಗಿ nl + 1;
}

ಪೂರ್ಣಾಂಕ xdl_blankline(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, ದೀರ್ಘ ಗಾತ್ರ_2, ದೀರ್ಘ flags)
{
	ದೀರ್ಘ i;

	ಆದರೆ (!(flags & XDF_WHITESPACE_FLAGS))
		ಹಿಂತಿರುಗಿ (ಗಾತ್ರ_2 <= 1);

	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2 && XDL_ISSPACE(ಸಾಲು[i]); i++)
		;

	ಹಿಂತಿರುಗಿ (i == ಗಾತ್ರ_2);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ತುದಿಗಳು_with_optional_cr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *l, ದೀರ್ಘ s, ದೀರ್ಘ i)
{
	ಪೂರ್ಣಾಂಕ complete = s && l[s-1] == '\n';

	ಆದರೆ (complete)
		s--;
	ಆದರೆ (s == i)
		ಹಿಂತಿರುಗಿ 1;
	/* do not ignore CR at the end of an incomplete line */
	ಆದರೆ (complete && s == i + 1 && l[i] == '\r')
		ಹಿಂತಿರುಗಿ 1;
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ xdl_recmatch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *l1, ದೀರ್ಘ s1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *l2, ದೀರ್ಘ s2, ದೀರ್ಘ flags)
{
	ಪೂರ್ಣಾಂಕ i1, i2;

	ಆದರೆ (s1 == s2 && !memcmp(l1, l2, s1))
		ಹಿಂತಿರುಗಿ 1;
	ಆದರೆ (!(flags & XDF_WHITESPACE_FLAGS))
		ಹಿಂತಿರುಗಿ 0;

	i1 = 0;
	i2 = 0;

	/*
	 * -w matches everything that matches with -b, and -b in turn
	 * matches everything that matches with --ignore-space-at-eol,
	 * which in turn matches everything that matches with --ignore-cr-at-eol.
	 *
	 * Each flavor of ignoring needs different logic to skip whitespaces
	 * while we have both sides to compare.
	 */
	ಆದರೆ (flags & XDF_IGNORE_WHITESPACE) {
		ಹೋಗು skip_ws;
		ಆಗ (i1 < s1 && i2 < s2) {
			ಆದರೆ (l1[i1++] != l2[i2++])
				ಹಿಂತಿರುಗಿ 0;
		skip_ws:
			ಆಗ (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			ಆಗ (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		ಆಗ (i1 < s1 && i2 < s2) {
			ಆದರೆ (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				ಆಗ (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				ಆಗ (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				ಮುಂದುವರಿ;
			}
			ಆದರೆ (l1[i1++] != l2[i2++])
				ಹಿಂತಿರುಗಿ 0;
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		ಆಗ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		ಆಗ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		ಹಿಂತಿರುಗಿ (ತುದಿಗಳು_with_optional_cr(l1, s1, i1) &&
			ತುದಿಗಳು_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	ಆದರೆ (i1 < s1) {
		ಆಗ (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		ಆದರೆ (s1 != i1)
			ಹಿಂತಿರುಗಿ 0;
	}
	ಆದರೆ (i2 < s2) {
		ಆಗ (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		ಹಿಂತಿರುಗಿ (s2 == i2);
	}
	ಹಿಂತಿರುಗಿ 1;
}

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ xdl_hash_ದಾಖಲೆ_with_whitespace(ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ **ದತ್ತಾಂಶ,
		ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *ತುದಿ, ದೀರ್ಘ flags) {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ha = 5381;
	ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *ptr = *ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	ಗಾಗಿ (; ptr < ತುದಿ && *ptr != '\n'; ptr++) {
		ಆದರೆ (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			ಆದರೆ (*ptr == '\r' &&
			    (ptr + 1 < ತುದಿ && ptr[1] == '\n'))
				ಮುಂದುವರಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (XDL_ISSPACE(*ptr)) {
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr2 = ptr;
			ಪೂರ್ಣಾಂಕ at_eol;
			ಆಗ (ptr + 1 < ತುದಿ && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (ತುದಿ <= ptr + 1 || ptr[1] == '\n');
			ಆದರೆ (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) ' ';
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				ಆಗ (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) *ptr2;
					ptr2++;
				}
			}
			ಮುಂದುವರಿ;
		}
		ha += (ha << 5);
		ha ^= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) *ptr;
	}
	*ದತ್ತಾಂಶ = ptr < ತುದಿ ? ptr + 1: ptr;

	ಹಿಂತಿರುಗಿ ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNUC__
#ವ್ಯಾಖ್ಯಾನಿಸು REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು REASSOC_FENCE(x, y)
#ಆದರೆ_ಅಂತ್ಯ

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ xdl_hash_ದಾಖಲೆ_verbatim(ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ **ದತ್ತಾಂಶ, ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *ತುದಿ) {
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ha = 5381, c0, c1;
	ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *ptr = *ದತ್ತಾಂಶ;
#ಆದರೆ 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	ಗಾಗಿ (; ptr < ತುದಿ && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) *ptr;
	}
	*ದತ್ತಾಂಶ = ptr < ತುದಿ ? ptr + 1: ptr;
#ಇಲ್ಲದಿದ್ದರೆ
	/* Process two characters per iteration. */
	ಆದರೆ (ತುದಿ - ptr >= 2) ಮಾಡು {
		ಆದರೆ ((c0 = ptr[0]) == '\n') {
			*ದತ್ತಾಂಶ = ptr + 1;
			ಹಿಂತಿರುಗಿ ha;
		}
		ಆದರೆ ((c1 = ptr[1]) == '\n') {
			*ದತ್ತಾಂಶ = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			ಹಿಂತಿರುಗಿ ha;
		}
		/*
		 * Combine characters C0 and C1 into the hash HA. We have
		 * HA = (HA * 33 + C0) * 33 + C1, and we want to ensure
		 * that dependency chain over HA is just one multiplication
		 * and one addition, i.e. we want to evaluate this as
		 * HA = HA * 33 * 33 + (C0 * 33 + C1), and likewise prefer
		 * (C0 * 32 + (C0 + C1)) for the expression in parenthesis.
		 */
		ha *= 33 * 33;
		c1 += c0;
		REASSOC_FENCE(c1, c0);
		c1 += c0 * 32;
		REASSOC_FENCE(c1, ha);
		ha += c1;

		ptr += 2;
	} ಆಗ (ptr < ತುದಿ - 1);
	*ದತ್ತಾಂಶ = ತುದಿ;
	ಆದರೆ (ptr < ತುದಿ && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ ha;
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ xdl_hashbits(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2) {
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val = 1, ಬಿಟ್ಟುಗಳು = 0;

	ಗಾಗಿ (; val < ಗಾತ್ರ_2 && ಬಿಟ್ಟುಗಳು < CHAR_BIT * ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ); val <<= 1, ಬಿಟ್ಟುಗಳು++);
	ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು ? ಬಿಟ್ಟುಗಳು: 1;
}


ಪೂರ್ಣಾಂಕ xdl_ಅಂಕೆ_ಹೊರಹೋಗು(ಅಕ್ಷರ *ಹೊರಹೋಗು, ದೀರ್ಘ val) {
	ಅಕ್ಷರ *ptr, *str = ಹೊರಹೋಗು;
	ಅಕ್ಷರ buf[32];

	ptr = buf + ಗಾತ್ರ(buf) - 1;
	*ptr = '\0';
	ಆದರೆ (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	ಗಾಗಿ (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	ಆದರೆ (*ptr)
		ಗಾಗಿ (; *ptr; ptr++, str++)
			*str = *ptr;
	ಇಲ್ಲದಿದ್ದರೆ
		*str++ = '0';
	*str = '\0';

	ಹಿಂತಿರುಗಿ str - ಹೊರಹೋಗು;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xdl_format_hunk_hdr(ದೀರ್ಘ s1, ದೀರ್ಘ c1, ದೀರ್ಘ s2, ದೀರ್ಘ c2,
			       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func, ದೀರ್ಘ funclen,
			       xdemitcb_t *ecb) {
	ಪೂರ್ಣಾಂಕ nb = 0;
	mmbuffer_t mb;
	ಅಕ್ಷರ buf[128];

	ಸ್ಮರಣೆ_ನಕಲು(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_ಅಂಕೆ_ಹೊರಹೋಗು(buf + nb, c1 ? s1: s1 - 1);

	ಆದರೆ (c1 != 1) {
		ಸ್ಮರಣೆ_ನಕಲು(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_ಅಂಕೆ_ಹೊರಹೋಗು(buf + nb, c1);
	}

	ಸ್ಮರಣೆ_ನಕಲು(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_ಅಂಕೆ_ಹೊರಹೋಗು(buf + nb, c2 ? s2: s2 - 1);

	ಆದರೆ (c2 != 1) {
		ಸ್ಮರಣೆ_ನಕಲು(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_ಅಂಕೆ_ಹೊರಹೋಗು(buf + nb, c2);
	}

	ಸ್ಮರಣೆ_ನಕಲು(buf + nb, " @@", 3);
	nb += 3;
	ಆದರೆ (func && funclen) {
		buf[nb++] = ' ';
		ಆದರೆ ((size_t)funclen > ಗಾತ್ರ(buf) - nb - 1)
			funclen = ಗಾತ್ರ(buf) - nb - 1;
		ಸ್ಮರಣೆ_ನಕಲು(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.ಗಾತ್ರ_2 = nb;
	ಆದರೆ (ecb->ಹೊರಹೋಗು_ಸಾಲು(ecb->priv, &mb, 1) < 0)
		ಹಿಂತಿರುಗಿ -1;
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ xdl_emit_hunk_hdr(ದೀರ್ಘ s1, ದೀರ್ಘ c1, ದೀರ್ಘ s2, ದೀರ್ಘ c2,
		      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func, ದೀರ್ಘ funclen,
		      xdemitcb_t *ecb) {
	ಆದರೆ (!ecb->ಹೊರಹೋಗು_hunk)
		ಹಿಂತಿರುಗಿ xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	ಆದರೆ (ecb->ಹೊರಹೋಗು_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		ಹಿಂತಿರುಗಿ -1;
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ xdl_fall_ಹಿಂಭಾಗ_diff(xdfenv_t *diff_env, xpparam_t ಸ್ಥಿರಾಂಕ *xpp,
		ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2)
{
	/*
	 * This probably does not work outside Git, since
	 * we have a very simple mmfile structure.
	 *
	 * Note: ideally, we would reuse the prepared environment, but
	 * the libxdiff interface does not (yet) allow for diffing only
	 * ranges of lines instead of the whole files.
	 */
	mmfile_t subfile1, subfile2;
	xdfenv_t env;

	subfile1.ptr = (ಅಕ್ಷರ *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.ಗಾತ್ರ_2 = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].ಗಾತ್ರ_2 - subfile1.ptr;
	subfile2.ptr = (ಅಕ್ಷರ *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.ಗಾತ್ರ_2 = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].ಗಾತ್ರ_2 - subfile2.ptr;
	ಆದರೆ (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		ಹಿಂತಿರುಗಿ -1;

	ಸ್ಮರಣೆ_ನಕಲು(diff_env->xdf1.ಬದಲಾಯಿತು + line1 - 1, env.xdf1.ಬದಲಾಯಿತು, count1);
	ಸ್ಮರಣೆ_ನಕಲು(diff_env->xdf2.ಬದಲಾಯಿತು + line2 - 1, env.xdf2.ಬದಲಾಯಿತು, count2);

	xdl_free_env(&env);

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ* xdl_alloc_grow_helper(ಶೂನ್ಯ *p, ದೀರ್ಘ nr, ದೀರ್ಘ *alloc, size_t ಗಾತ್ರ_2)
{
	ಶೂನ್ಯ *tmp = ಖಾಲಿ;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	ಆದರೆ ((size_t)nr > n)
		n = nr;
	ಆದರೆ (ಗಾತ್ರ_MAX / ಗಾತ್ರ_2 >= n)
		tmp = xdl_realloc(p, n * ಗಾತ್ರ_2);
	ಆದರೆ (tmp) {
		*alloc = (ದೀರ್ಘ)n;
	} ಇಲ್ಲದಿದ್ದರೆ {
		xdl_free(p);
		*alloc = 0;
	}
	ಹಿಂತಿರುಗಿ tmp;
}
