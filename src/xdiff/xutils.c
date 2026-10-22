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

#അന്തര്ഭാവ "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	വേണ്ടി (i = 1; n > 0; n >>= 2)
		i <<= 1;

	തിരികെ i;
}


പൂർണ്ണസംഖ്യ xdl_emit_diffrec(അക്ഷരം സ്ഥിരം *rec, long size, അക്ഷരം സ്ഥിരം *pre, long psize,
		     xdemitcb_t *ecb) {
	പൂർണ്ണസംഖ്യ i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (അക്ഷരം *) pre;
	mb[0].size = psize;
	mb[1].ptr = (അക്ഷരം *) rec;
	mb[1].size = size;
	എങ്കിൽ (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (അക്ഷരം *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	എങ്കിൽ (ecb->out_വരി(ecb->priv, mb, i) < 0) {

		തിരികെ -1;
	}

	തിരികെ 0;
}

ശൂന്യം *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	തിരികെ mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	തിരികെ mmf->size;
}


പൂർണ്ണസംഖ്യ xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = ശൂന്യ;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = ശൂന്യ;
	cha->scurr = 0;

	തിരികെ 0;
}


ശൂന്യം xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	വേണ്ടി (cur = cha->head; (tmp = cur) != ശൂന്യ;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


ശൂന്യം *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	ശൂന്യം *data;

	എങ്കിൽ (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		എങ്കിൽ (!(ancur = (chanode_t *) xdl_malloc(വലുപ്പം(chanode_t) + cha->nsize))) {

			തിരികെ ശൂന്യ;
		}
		ancur->icurr = 0;
		ancur->next = ശൂന്യ;
		എങ്കിൽ (cha->tail)
			cha->tail->next = ancur;
		എങ്കിൽ (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (അക്ഷരം *) ancur + വലുപ്പം(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	തിരികെ data;
}

long xdl_guess_വരികൾ(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	അക്ഷരം സ്ഥിരം *data, *cur, *മുകൾ;

	എങ്കിൽ ((cur = data = xdl_mmfile_first(mf, &size))) {
		വേണ്ടി (മുകൾ = data + size; nl < sample && cur < മുകൾ; ) {
			nl++;
			എങ്കിൽ (!(cur = memchr(cur, '\n', മുകൾ - cur)))
				cur = മുകൾ;
			അല്ലെങ്കിൽ
				cur++;
		}
		tsize += (long) (cur - data);
	}

	എങ്കിൽ (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	തിരികെ nl + 1;
}

പൂർണ്ണസംഖ്യ xdl_blankline(സ്ഥിരം അക്ഷരം *വരി, long size, long flags)
{
	long i;

	എങ്കിൽ (!(flags & XDF_WHITESPACE_FLAGS))
		തിരികെ (size <= 1);

	വേണ്ടി (i = 0; i < size && XDL_ISSPACE(വരി[i]); i++)
		;

	തിരികെ (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ ends_with_optional_cr(സ്ഥിരം അക്ഷരം *l, long s, long i)
{
	പൂർണ്ണസംഖ്യ complete = s && l[s-1] == '\n';

	എങ്കിൽ (complete)
		s--;
	എങ്കിൽ (s == i)
		തിരികെ 1;
	/* do not ignore CR at the end of an incomplete line */
	എങ്കിൽ (complete && s == i + 1 && l[i] == '\r')
		തിരികെ 1;
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ xdl_recmatch(സ്ഥിരം അക്ഷരം *l1, long s1, സ്ഥിരം അക്ഷരം *l2, long s2, long flags)
{
	പൂർണ്ണസംഖ്യ i1, i2;

	എങ്കിൽ (s1 == s2 && !memcmp(l1, l2, s1))
		തിരികെ 1;
	എങ്കിൽ (!(flags & XDF_WHITESPACE_FLAGS))
		തിരികെ 0;

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
	എങ്കിൽ (flags & XDF_IGNORE_WHITESPACE) {
		പോകുക skip_ws;
		ആകുമ്പോൾ (i1 < s1 && i2 < s2) {
			എങ്കിൽ (l1[i1++] != l2[i2++])
				തിരികെ 0;
		skip_ws:
			ആകുമ്പോൾ (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			ആകുമ്പോൾ (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		ആകുമ്പോൾ (i1 < s1 && i2 < s2) {
			എങ്കിൽ (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				ആകുമ്പോൾ (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				ആകുമ്പോൾ (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				തുടരുക;
			}
			എങ്കിൽ (l1[i1++] != l2[i2++])
				തിരികെ 0;
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		ആകുമ്പോൾ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		ആകുമ്പോൾ (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		തിരികെ (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	എങ്കിൽ (i1 < s1) {
		ആകുമ്പോൾ (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		എങ്കിൽ (s1 != i1)
			തിരികെ 0;
	}
	എങ്കിൽ (i2 < s2) {
		ആകുമ്പോൾ (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		തിരികെ (s2 == i2);
	}
	തിരികെ 1;
}

unsigned long xdl_hash_record_with_whitespace(അക്ഷരം സ്ഥിരം **data,
		അക്ഷരം സ്ഥിരം *മുകൾ, long flags) {
	unsigned long ha = 5381;
	അക്ഷരം സ്ഥിരം *ptr = *data;
	പൂർണ്ണസംഖ്യ cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	വേണ്ടി (; ptr < മുകൾ && *ptr != '\n'; ptr++) {
		എങ്കിൽ (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			എങ്കിൽ (*ptr == '\r' &&
			    (ptr + 1 < മുകൾ && ptr[1] == '\n'))
				തുടരുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (XDL_ISSPACE(*ptr)) {
			സ്ഥിരം അക്ഷരം *ptr2 = ptr;
			പൂർണ്ണസംഖ്യ at_eol;
			ആകുമ്പോൾ (ptr + 1 < മുകൾ && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (മുകൾ <= ptr + 1 || ptr[1] == '\n');
			എങ്കിൽ (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			അല്ലെങ്കിൽ എങ്കിൽ (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			അല്ലെങ്കിൽ എങ്കിൽ (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				ആകുമ്പോൾ (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			തുടരുക;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < മുകൾ ? ptr + 1: ptr;

	തിരികെ ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#അല്ലെങ്കിൽ
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(അക്ഷരം സ്ഥിരം **data, അക്ഷരം സ്ഥിരം *മുകൾ) {
	unsigned long ha = 5381, c0, c1;
	അക്ഷരം സ്ഥിരം *ptr = *data;
#എങ്കിൽ 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	വേണ്ടി (; ptr < മുകൾ && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < മുകൾ ? ptr + 1: ptr;
#അല്ലെങ്കിൽ
	/* Process two characters per iteration. */
	എങ്കിൽ (മുകൾ - ptr >= 2) ചെയ്യുക {
		എങ്കിൽ ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			തിരികെ ha;
		}
		എങ്കിൽ ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			തിരികെ ha;
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
	} ആകുമ്പോൾ (ptr < മുകൾ - 1);
	*data = മുകൾ;
	എങ്കിൽ (ptr < മുകൾ && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	തിരികെ ha;
}

unsigned പൂർണ്ണസംഖ്യ xdl_hashbits(unsigned പൂർണ്ണസംഖ്യ size) {
	unsigned പൂർണ്ണസംഖ്യ val = 1, bits = 0;

	വേണ്ടി (; val < size && bits < CHAR_BIT * വലുപ്പം(unsigned പൂർണ്ണസംഖ്യ); val <<= 1, bits++);
	തിരികെ bits ? bits: 1;
}


പൂർണ്ണസംഖ്യ xdl_അക്കം_out(അക്ഷരം *out, long val) {
	അക്ഷരം *ptr, *str = out;
	അക്ഷരം buf[32];

	ptr = buf + വലുപ്പം(buf) - 1;
	*ptr = '\0';
	എങ്കിൽ (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	വേണ്ടി (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	എങ്കിൽ (*ptr)
		വേണ്ടി (; *ptr; ptr++, str++)
			*str = *ptr;
	അല്ലെങ്കിൽ
		*str++ = '0';
	*str = '\0';

	തിരികെ str - out;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       സ്ഥിരം അക്ഷരം *func, long funclen,
			       xdemitcb_t *ecb) {
	പൂർണ്ണസംഖ്യ nb = 0;
	mmbuffer_t mb;
	അക്ഷരം buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_അക്കം_out(buf + nb, c1 ? s1: s1 - 1);

	എങ്കിൽ (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_അക്കം_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_അക്കം_out(buf + nb, c2 ? s2: s2 - 1);

	എങ്കിൽ (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_അക്കം_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	എങ്കിൽ (func && funclen) {
		buf[nb++] = ' ';
		എങ്കിൽ ((size_t)funclen > വലുപ്പം(buf) - nb - 1)
			funclen = വലുപ്പം(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	എങ്കിൽ (ecb->out_വരി(ecb->priv, &mb, 1) < 0)
		തിരികെ -1;
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      സ്ഥിരം അക്ഷരം *func, long funclen,
		      xdemitcb_t *ecb) {
	എങ്കിൽ (!ecb->out_hunk)
		തിരികെ xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	എങ്കിൽ (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		തിരികെ -1;
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t സ്ഥിരം *xpp,
		പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2)
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

	subfile1.ptr = (അക്ഷരം *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (അക്ഷരം *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	എങ്കിൽ (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		തിരികെ -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	തിരികെ 0;
}

ശൂന്യം* xdl_alloc_grow_helper(ശൂന്യം *p, long nr, long *alloc, size_t size)
{
	ശൂന്യം *tmp = ശൂന്യ;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	എങ്കിൽ ((size_t)nr > n)
		n = nr;
	എങ്കിൽ (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	എങ്കിൽ (tmp) {
		*alloc = (long)n;
	} അല്ലെങ്കിൽ {
		xdl_free(p);
		*alloc = 0;
	}
	തിരികെ tmp;
}
