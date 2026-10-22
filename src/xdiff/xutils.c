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

#ਅਨ੍ਤਰ੍ਭਾਵ "xinclude.h"


ਲੰਮਾ xdl_bogosqrt(ਲੰਮਾ n) {
	ਲੰਮਾ i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	ਲਈ (i = 1; n > 0; n >>= 2)
		i <<= 1;

	ਵਾਪਸ i;
}


ਪੂਰਨ xdl_emit_diffrec(ਅੱਖਰ ਅਚਲ *rec, ਲੰਮਾ ਮਾਪ, ਅੱਖਰ ਅਚਲ *pre, ਲੰਮਾ psize,
		     xdemitcb_t *ecb) {
	ਪੂਰਨ i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (ਅੱਖਰ *) pre;
	mb[0].ਮਾਪ = psize;
	mb[1].ptr = (ਅੱਖਰ *) rec;
	mb[1].ਮਾਪ = ਮਾਪ;
	ਜੇ (ਮਾਪ > 0 && rec[ਮਾਪ - 1] != '\n') {
		mb[2].ptr = (ਅੱਖਰ *) "\n\\ No newline at end of file\n";
		mb[2].ਮਾਪ = (ਲੰਮਾ)ਲੜੀ_ਲੰਬਾਈ(mb[2].ptr);
		i++;
	}
	ਜੇ (ecb->ਬਾਹਰਲਾ_ਸਤਰ(ecb->priv, mb, i) < 0) {

		ਵਾਪਸ -1;
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ *xdl_mmfile_ਪਹਿਲਾ(mmfile_t *mmf, ਲੰਮਾ *ਮਾਪ)
{
	*ਮਾਪ = mmf->ਮਾਪ;
	ਵਾਪਸ mmf->ptr;
}


ਲੰਮਾ xdl_mmfile_ਮਾਪ(mmfile_t *mmf)
{
	ਵਾਪਸ mmf->ਮਾਪ;
}


ਪੂਰਨ xdl_cha_init(chastore_t *cha, ਲੰਮਾ isize, ਲੰਮਾ icount) {

	cha->ਸਿਰ = cha->ਪੂਛ = ਨਿਰਾ;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = ਨਿਰਾ;
	cha->scurr = 0;

	ਵਾਪਸ 0;
}


ਖਾਲੀ xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	ਲਈ (cur = cha->ਸਿਰ; (tmp = cur) != ਨਿਰਾ;) {
		cur = cur->ਅਗਲਾ;
		xdl_free(tmp);
	}
}


ਖਾਲੀ *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	ਖਾਲੀ *ਅੰਕੜੇ;

	ਜੇ (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		ਜੇ (!(ancur = (chanode_t *) xdl_malloc(ਆਕਾਰ(chanode_t) + cha->nsize))) {

			ਵਾਪਸ ਨਿਰਾ;
		}
		ancur->icurr = 0;
		ancur->ਅਗਲਾ = ਨਿਰਾ;
		ਜੇ (cha->ਪੂਛ)
			cha->ਪੂਛ->ਅਗਲਾ = ancur;
		ਜੇ (!cha->ਸਿਰ)
			cha->ਸਿਰ = ancur;
		cha->ਪੂਛ = ancur;
		cha->ancur = ancur;
	}

	ਅੰਕੜੇ = (ਅੱਖਰ *) ancur + ਆਕਾਰ(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	ਵਾਪਸ ਅੰਕੜੇ;
}

ਲੰਮਾ xdl_guess_ਸਤਰਾਂ(mmfile_t *mf, ਲੰਮਾ sample) {
	ਲੰਮਾ nl = 0, ਮਾਪ, tsize = 0;
	ਅੱਖਰ ਅਚਲ *ਅੰਕੜੇ, *cur, *ਸਿਖਰ;

	ਜੇ ((cur = ਅੰਕੜੇ = xdl_mmfile_ਪਹਿਲਾ(mf, &ਮਾਪ))) {
		ਲਈ (ਸਿਖਰ = ਅੰਕੜੇ + ਮਾਪ; nl < sample && cur < ਸਿਖਰ; ) {
			nl++;
			ਜੇ (!(cur = memchr(cur, '\n', ਸਿਖਰ - cur)))
				cur = ਸਿਖਰ;
			ਨਹੀਂਤਾਂ
				cur++;
		}
		tsize += (ਲੰਮਾ) (cur - ਅੰਕੜੇ);
	}

	ਜੇ (nl && tsize)
		nl = xdl_mmfile_ਮਾਪ(mf) / (tsize / nl);

	ਵਾਪਸ nl + 1;
}

ਪੂਰਨ xdl_blankline(ਅਚਲ ਅੱਖਰ *ਸਤਰ, ਲੰਮਾ ਮਾਪ, ਲੰਮਾ flags)
{
	ਲੰਮਾ i;

	ਜੇ (!(flags & XDF_WHITESPACE_FLAGS))
		ਵਾਪਸ (ਮਾਪ <= 1);

	ਲਈ (i = 0; i < ਮਾਪ && XDL_ISSPACE(ਸਤਰ[i]); i++)
		;

	ਵਾਪਸ (i == ਮਾਪ);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
ਸਥਿਰ ਪੂਰਨ ਸਿਰੇ_with_optional_cr(ਅਚਲ ਅੱਖਰ *l, ਲੰਮਾ s, ਲੰਮਾ i)
{
	ਪੂਰਨ complete = s && l[s-1] == '\n';

	ਜੇ (complete)
		s--;
	ਜੇ (s == i)
		ਵਾਪਸ 1;
	/* do not ignore CR at the end of an incomplete line */
	ਜੇ (complete && s == i + 1 && l[i] == '\r')
		ਵਾਪਸ 1;
	ਵਾਪਸ 0;
}

ਪੂਰਨ xdl_recmatch(ਅਚਲ ਅੱਖਰ *l1, ਲੰਮਾ s1, ਅਚਲ ਅੱਖਰ *l2, ਲੰਮਾ s2, ਲੰਮਾ flags)
{
	ਪੂਰਨ i1, i2;

	ਜੇ (s1 == s2 && !memcmp(l1, l2, s1))
		ਵਾਪਸ 1;
	ਜੇ (!(flags & XDF_WHITESPACE_FLAGS))
		ਵਾਪਸ 0;

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
	ਜੇ (flags & XDF_IGNORE_WHITESPACE) {
		ਜਾਓ skip_ws;
		ਜਦੋਂ (i1 < s1 && i2 < s2) {
			ਜੇ (l1[i1++] != l2[i2++])
				ਵਾਪਸ 0;
		skip_ws:
			ਜਦੋਂ (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			ਜਦੋਂ (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} ਨਹੀਂਤਾਂ ਜੇ (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		ਜਦੋਂ (i1 < s1 && i2 < s2) {
			ਜੇ (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				ਜਦੋਂ (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				ਜਦੋਂ (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				ਜਾਰੀ;
			}
			ਜੇ (l1[i1++] != l2[i2++])
				ਵਾਪਸ 0;
		}
	} ਨਹੀਂਤਾਂ ਜੇ (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		ਜਦੋਂ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} ਨਹੀਂਤਾਂ ਜੇ (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		ਜਦੋਂ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		ਵਾਪਸ (ਸਿਰੇ_with_optional_cr(l1, s1, i1) &&
			ਸਿਰੇ_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	ਜੇ (i1 < s1) {
		ਜਦੋਂ (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		ਜੇ (s1 != i1)
			ਵਾਪਸ 0;
	}
	ਜੇ (i2 < s2) {
		ਜਦੋਂ (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		ਵਾਪਸ (s2 == i2);
	}
	ਵਾਪਸ 1;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ xdl_hash_ਰਿਕਾਰਡ_with_whitespace(ਅੱਖਰ ਅਚਲ **ਅੰਕੜੇ,
		ਅੱਖਰ ਅਚਲ *ਸਿਖਰ, ਲੰਮਾ flags) {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ha = 5381;
	ਅੱਖਰ ਅਚਲ *ptr = *ਅੰਕੜੇ;
	ਪੂਰਨ cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	ਲਈ (; ptr < ਸਿਖਰ && *ptr != '\n'; ptr++) {
		ਜੇ (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			ਜੇ (*ptr == '\r' &&
			    (ptr + 1 < ਸਿਖਰ && ptr[1] == '\n'))
				ਜਾਰੀ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (XDL_ISSPACE(*ptr)) {
			ਅਚਲ ਅੱਖਰ *ptr2 = ptr;
			ਪੂਰਨ at_eol;
			ਜਦੋਂ (ptr + 1 < ਸਿਖਰ && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (ਸਿਖਰ <= ptr + 1 || ptr[1] == '\n');
			ਜੇ (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			ਨਹੀਂਤਾਂ ਜੇ (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) ' ';
			}
			ਨਹੀਂਤਾਂ ਜੇ (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				ਜਦੋਂ (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) *ptr2;
					ptr2++;
				}
			}
			ਜਾਰੀ;
		}
		ha += (ha << 5);
		ha ^= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) *ptr;
	}
	*ਅੰਕੜੇ = ptr < ਸਿਖਰ ? ptr + 1: ptr;

	ਵਾਪਸ ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNUC__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REASSOC_FENCE(x, y)
#ਜੇ_ਅੰਤ

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ xdl_hash_ਰਿਕਾਰਡ_verbatim(ਅੱਖਰ ਅਚਲ **ਅੰਕੜੇ, ਅੱਖਰ ਅਚਲ *ਸਿਖਰ) {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ha = 5381, c0, c1;
	ਅੱਖਰ ਅਚਲ *ptr = *ਅੰਕੜੇ;
#ਜੇ 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	ਲਈ (; ptr < ਸਿਖਰ && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) *ptr;
	}
	*ਅੰਕੜੇ = ptr < ਸਿਖਰ ? ptr + 1: ptr;
#ਨਹੀਂਤਾਂ
	/* Process two characters per iteration. */
	ਜੇ (ਸਿਖਰ - ptr >= 2) ਕਰੋ {
		ਜੇ ((c0 = ptr[0]) == '\n') {
			*ਅੰਕੜੇ = ptr + 1;
			ਵਾਪਸ ha;
		}
		ਜੇ ((c1 = ptr[1]) == '\n') {
			*ਅੰਕੜੇ = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			ਵਾਪਸ ha;
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
	} ਜਦੋਂ (ptr < ਸਿਖਰ - 1);
	*ਅੰਕੜੇ = ਸਿਖਰ;
	ਜੇ (ptr < ਸਿਖਰ && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#ਜੇ_ਅੰਤ
	ਵਾਪਸ ha;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ xdl_hashbits(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਮਾਪ) {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val = 1, ਬਿੱਟ = 0;

	ਲਈ (; val < ਮਾਪ && ਬਿੱਟ < CHAR_BIT * ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ); val <<= 1, ਬਿੱਟ++);
	ਵਾਪਸ ਬਿੱਟ ? ਬਿੱਟ: 1;
}


ਪੂਰਨ xdl_ਅੰਕ_ਬਾਹਰਲਾ(ਅੱਖਰ *ਬਾਹਰਲਾ, ਲੰਮਾ val) {
	ਅੱਖਰ *ptr, *str = ਬਾਹਰਲਾ;
	ਅੱਖਰ buf[32];

	ptr = buf + ਆਕਾਰ(buf) - 1;
	*ptr = '\0';
	ਜੇ (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	ਲਈ (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	ਜੇ (*ptr)
		ਲਈ (; *ptr; ptr++, str++)
			*str = *ptr;
	ਨਹੀਂਤਾਂ
		*str++ = '0';
	*str = '\0';

	ਵਾਪਸ str - ਬਾਹਰਲਾ;
}

ਸਥਿਰ ਪੂਰਨ xdl_format_hunk_hdr(ਲੰਮਾ s1, ਲੰਮਾ c1, ਲੰਮਾ s2, ਲੰਮਾ c2,
			       ਅਚਲ ਅੱਖਰ *func, ਲੰਮਾ funclen,
			       xdemitcb_t *ecb) {
	ਪੂਰਨ nb = 0;
	mmbuffer_t mb;
	ਅੱਖਰ buf[128];

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_ਅੰਕ_ਬਾਹਰਲਾ(buf + nb, c1 ? s1: s1 - 1);

	ਜੇ (c1 != 1) {
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_ਅੰਕ_ਬਾਹਰਲਾ(buf + nb, c1);
	}

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_ਅੰਕ_ਬਾਹਰਲਾ(buf + nb, c2 ? s2: s2 - 1);

	ਜੇ (c2 != 1) {
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_ਅੰਕ_ਬਾਹਰਲਾ(buf + nb, c2);
	}

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf + nb, " @@", 3);
	nb += 3;
	ਜੇ (func && funclen) {
		buf[nb++] = ' ';
		ਜੇ ((size_t)funclen > ਆਕਾਰ(buf) - nb - 1)
			funclen = ਆਕਾਰ(buf) - nb - 1;
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.ਮਾਪ = nb;
	ਜੇ (ecb->ਬਾਹਰਲਾ_ਸਤਰ(ecb->priv, &mb, 1) < 0)
		ਵਾਪਸ -1;
	ਵਾਪਸ 0;
}

ਪੂਰਨ xdl_emit_hunk_hdr(ਲੰਮਾ s1, ਲੰਮਾ c1, ਲੰਮਾ s2, ਲੰਮਾ c2,
		      ਅਚਲ ਅੱਖਰ *func, ਲੰਮਾ funclen,
		      xdemitcb_t *ecb) {
	ਜੇ (!ecb->ਬਾਹਰਲਾ_hunk)
		ਵਾਪਸ xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	ਜੇ (ecb->ਬਾਹਰਲਾ_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		ਵਾਪਸ -1;
	ਵਾਪਸ 0;
}

ਪੂਰਨ xdl_fall_ਪਿੱਠ_diff(xdfenv_t *diff_env, xpparam_t ਅਚਲ *xpp,
		ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2)
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

	subfile1.ptr = (ਅੱਖਰ *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.ਮਾਪ = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].ਮਾਪ - subfile1.ptr;
	subfile2.ptr = (ਅੱਖਰ *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.ਮਾਪ = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].ਮਾਪ - subfile2.ptr;
	ਜੇ (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		ਵਾਪਸ -1;

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(diff_env->xdf1.ਬਦਲਿਆ + line1 - 1, env.xdf1.ਬਦਲਿਆ, count1);
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(diff_env->xdf2.ਬਦਲਿਆ + line2 - 1, env.xdf2.ਬਦਲਿਆ, count2);

	xdl_free_env(&env);

	ਵਾਪਸ 0;
}

ਖਾਲੀ* xdl_alloc_grow_helper(ਖਾਲੀ *p, ਲੰਮਾ nr, ਲੰਮਾ *alloc, size_t ਮਾਪ)
{
	ਖਾਲੀ *tmp = ਨਿਰਾ;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	ਜੇ ((size_t)nr > n)
		n = nr;
	ਜੇ (ਮਾਪ_MAX / ਮਾਪ >= n)
		tmp = xdl_realloc(p, n * ਮਾਪ);
	ਜੇ (tmp) {
		*alloc = (ਲੰਮਾ)n;
	} ਨਹੀਂਤਾਂ {
		xdl_free(p);
		*alloc = 0;
	}
	ਵਾਪਸ tmp;
}
