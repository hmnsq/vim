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

#include "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	для (i = 1; n > 0; n >>= 2)
		i <<= 1;

	вернуть i;
}


целое xdl_emit_diffrec(char const *rec, long size, char const *pre, long psize,
		     xdemitcb_t *ecb) {
	целое i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (char *) pre;
	mb[0].size = psize;
	mb[1].ptr = (char *) rec;
	mb[1].size = size;
	если (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (char *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	если (ecb->out_line(ecb->priv, mb, i) < 0) {

		вернуть -1;
	}

	вернуть 0;
}

void *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	вернуть mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	вернуть mmf->size;
}


целое xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = NULL;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = NULL;
	cha->scurr = 0;

	вернуть 0;
}


void xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	для (cur = cha->head; (tmp = cur) != NULL;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


void *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	void *data;

	если (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		если (!(ancur = (chanode_t *) xdl_malloc(sizeof(chanode_t) + cha->nsize))) {

			вернуть NULL;
		}
		ancur->icurr = 0;
		ancur->next = NULL;
		если (cha->tail)
			cha->tail->next = ancur;
		если (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (char *) ancur + sizeof(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	вернуть data;
}

long xdl_guess_lines(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	char const *data, *cur, *top;

	если ((cur = data = xdl_mmfile_first(mf, &size))) {
		для (top = data + size; nl < sample && cur < top; ) {
			nl++;
			если (!(cur = memchr(cur, '\n', top - cur)))
				cur = top;
			иначе
				cur++;
		}
		tsize += (long) (cur - data);
	}

	если (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	вернуть nl + 1;
}

целое xdl_blankline(const char *line, long size, long flags)
{
	long i;

	если (!(flags & XDF_WHITESPACE_FLAGS))
		вернуть (size <= 1);

	для (i = 0; i < size && XDL_ISSPACE(line[i]); i++)
		;

	вернуть (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
static целое ends_with_optional_cr(const char *l, long s, long i)
{
	целое complete = s && l[s-1] == '\n';

	если (complete)
		s--;
	если (s == i)
		вернуть 1;
	/* do not ignore CR at the end of an incomplete line */
	если (complete && s == i + 1 && l[i] == '\r')
		вернуть 1;
	вернуть 0;
}

целое xdl_recmatch(const char *l1, long s1, const char *l2, long s2, long flags)
{
	целое i1, i2;

	если (s1 == s2 && !memcmp(l1, l2, s1))
		вернуть 1;
	если (!(flags & XDF_WHITESPACE_FLAGS))
		вернуть 0;

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
	если (flags & XDF_IGNORE_WHITESPACE) {
		goto skip_ws;
		пока (i1 < s1 && i2 < s2) {
			если (l1[i1++] != l2[i2++])
				вернуть 0;
		skip_ws:
			пока (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			пока (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} иначе если (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		пока (i1 < s1 && i2 < s2) {
			если (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				пока (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				пока (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				continue;
			}
			если (l1[i1++] != l2[i2++])
				вернуть 0;
		}
	} иначе если (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		пока (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} иначе если (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		пока (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		вернуть (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	если (i1 < s1) {
		пока (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		если (s1 != i1)
			вернуть 0;
	}
	если (i2 < s2) {
		пока (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		вернуть (s2 == i2);
	}
	вернуть 1;
}

unsigned long xdl_hash_record_with_whitespace(char const **data,
		char const *top, long flags) {
	unsigned long ha = 5381;
	char const *ptr = *data;
	целое cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	для (; ptr < top && *ptr != '\n'; ptr++) {
		если (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			если (*ptr == '\r' &&
			    (ptr + 1 < top && ptr[1] == '\n'))
				continue;
		}
		иначе если (XDL_ISSPACE(*ptr)) {
			const char *ptr2 = ptr;
			целое at_eol;
			пока (ptr + 1 < top && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (top <= ptr + 1 || ptr[1] == '\n');
			если (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			иначе если (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			иначе если (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				пока (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			continue;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < top ? ptr + 1: ptr;

	вернуть ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#иначе
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(char const **data, char const *top) {
	unsigned long ha = 5381, c0, c1;
	char const *ptr = *data;
#если 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	для (; ptr < top && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < top ? ptr + 1: ptr;
#иначе
	/* Process two characters per iteration. */
	если (top - ptr >= 2) do {
		если ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			вернуть ha;
		}
		если ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			вернуть ha;
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
	} пока (ptr < top - 1);
	*data = top;
	если (ptr < top && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	вернуть ha;
}

unsigned целое xdl_hashbits(unsigned целое size) {
	unsigned целое val = 1, bits = 0;

	для (; val < size && bits < CHAR_BIT * sizeof(unsigned целое); val <<= 1, bits++);
	вернуть bits ? bits: 1;
}


целое xdl_num_out(char *out, long val) {
	char *ptr, *str = out;
	char buf[32];

	ptr = buf + sizeof(buf) - 1;
	*ptr = '\0';
	если (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	для (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	если (*ptr)
		для (; *ptr; ptr++, str++)
			*str = *ptr;
	иначе
		*str++ = '0';
	*str = '\0';

	вернуть str - out;
}

static целое xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       const char *func, long funclen,
			       xdemitcb_t *ecb) {
	целое nb = 0;
	mmbuffer_t mb;
	char buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_num_out(buf + nb, c1 ? s1: s1 - 1);

	если (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_num_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_num_out(buf + nb, c2 ? s2: s2 - 1);

	если (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_num_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	если (func && funclen) {
		buf[nb++] = ' ';
		если ((size_t)funclen > sizeof(buf) - nb - 1)
			funclen = sizeof(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	если (ecb->out_line(ecb->priv, &mb, 1) < 0)
		вернуть -1;
	вернуть 0;
}

целое xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      const char *func, long funclen,
		      xdemitcb_t *ecb) {
	если (!ecb->out_hunk)
		вернуть xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	если (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		вернуть -1;
	вернуть 0;
}

целое xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t const *xpp,
		целое line1, целое count1, целое line2, целое count2)
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

	subfile1.ptr = (char *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (char *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	если (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		вернуть -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	вернуть 0;
}

void* xdl_alloc_grow_helper(void *p, long nr, long *alloc, size_t size)
{
	void *tmp = NULL;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	если ((size_t)nr > n)
		n = nr;
	если (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	если (tmp) {
		*alloc = (long)n;
	} иначе {
		xdl_free(p);
		*alloc = 0;
	}
	вернуть tmp;
}
