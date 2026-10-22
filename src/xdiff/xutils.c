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

#अन्तर्भाव "xinclude.h"


long xdl_bogosqrt(long n) {
	long i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	अत्थाय (i = 1; n > 0; n >>= 2)
		i <<= 1;

	पटिनिवत्त i;
}


पूर्णाङ्क xdl_emit_diffrec(वर्ण ध्रुव *rec, long size, वर्ण ध्रुव *pre, long psize,
		     xdemitcb_t *ecb) {
	पूर्णाङ्क i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (वर्ण *) pre;
	mb[0].size = psize;
	mb[1].ptr = (वर्ण *) rec;
	mb[1].size = size;
	सचे (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (वर्ण *) "\n\\ No newline at end of file\n";
		mb[2].size = (long)strlen(mb[2].ptr);
		i++;
	}
	सचे (ecb->out_पङ्क्तिः(ecb->priv, mb, i) < 0) {

		पटिनिवत्त -1;
	}

	पटिनिवत्त 0;
}

शून्य *xdl_mmfile_first(mmfile_t *mmf, long *size)
{
	*size = mmf->size;
	पटिनिवत्त mmf->ptr;
}


long xdl_mmfile_size(mmfile_t *mmf)
{
	पटिनिवत्त mmf->size;
}


पूर्णाङ्क xdl_cha_init(chastore_t *cha, long isize, long icount) {

	cha->head = cha->tail = रिक्त;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = रिक्त;
	cha->scurr = 0;

	पटिनिवत्त 0;
}


शून्य xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	अत्थाय (cur = cha->head; (tmp = cur) != रिक्त;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


शून्य *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	शून्य *data;

	सचे (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		सचे (!(ancur = (chanode_t *) xdl_malloc(परिमाण(chanode_t) + cha->nsize))) {

			पटिनिवत्त रिक्त;
		}
		ancur->icurr = 0;
		ancur->next = रिक्त;
		सचे (cha->tail)
			cha->tail->next = ancur;
		सचे (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (वर्ण *) ancur + परिमाण(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	पटिनिवत्त data;
}

long xdl_guess_पङ्क्तयः(mmfile_t *mf, long sample) {
	long nl = 0, size, tsize = 0;
	वर्ण ध्रुव *data, *cur, *शिखरम्;

	सचे ((cur = data = xdl_mmfile_first(mf, &size))) {
		अत्थाय (शिखरम् = data + size; nl < sample && cur < शिखरम्; ) {
			nl++;
			सचे (!(cur = memchr(cur, '\n', शिखरम् - cur)))
				cur = शिखरम्;
			अञ्ञथा
				cur++;
		}
		tsize += (long) (cur - data);
	}

	सचे (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	पटिनिवत्त nl + 1;
}

पूर्णाङ्क xdl_blankline(ध्रुव वर्ण *पङ्क्तिः, long size, long flags)
{
	long i;

	सचे (!(flags & XDF_WHITESPACE_FLAGS))
		पटिनिवत्त (size <= 1);

	अत्थाय (i = 0; i < size && XDL_ISSPACE(पङ्क्तिः[i]); i++)
		;

	पटिनिवत्त (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
स्थिर पूर्णाङ्क ends_with_optional_cr(ध्रुव वर्ण *l, long s, long i)
{
	पूर्णाङ्क complete = s && l[s-1] == '\n';

	सचे (complete)
		s--;
	सचे (s == i)
		पटिनिवत्त 1;
	/* do not ignore CR at the end of an incomplete line */
	सचे (complete && s == i + 1 && l[i] == '\r')
		पटिनिवत्त 1;
	पटिनिवत्त 0;
}

पूर्णाङ्क xdl_recmatch(ध्रुव वर्ण *l1, long s1, ध्रुव वर्ण *l2, long s2, long flags)
{
	पूर्णाङ्क i1, i2;

	सचे (s1 == s2 && !memcmp(l1, l2, s1))
		पटिनिवत्त 1;
	सचे (!(flags & XDF_WHITESPACE_FLAGS))
		पटिनिवत्त 0;

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
	सचे (flags & XDF_IGNORE_WHITESPACE) {
		गच्छ skip_ws;
		याव (i1 < s1 && i2 < s2) {
			सचे (l1[i1++] != l2[i2++])
				पटिनिवत्त 0;
		skip_ws:
			याव (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			याव (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} अञ्ञथा सचे (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		याव (i1 < s1 && i2 < s2) {
			सचे (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				याव (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				याव (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				अनुवर्त;
			}
			सचे (l1[i1++] != l2[i2++])
				पटिनिवत्त 0;
		}
	} अञ्ञथा सचे (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		याव (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} अञ्ञथा सचे (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		याव (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		पटिनिवत्त (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	सचे (i1 < s1) {
		याव (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		सचे (s1 != i1)
			पटिनिवत्त 0;
	}
	सचे (i2 < s2) {
		याव (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		पटिनिवत्त (s2 == i2);
	}
	पटिनिवत्त 1;
}

unsigned long xdl_hash_record_with_whitespace(वर्ण ध्रुव **data,
		वर्ण ध्रुव *शिखरम्, long flags) {
	unsigned long ha = 5381;
	वर्ण ध्रुव *ptr = *data;
	पूर्णाङ्क cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	अत्थाय (; ptr < शिखरम् && *ptr != '\n'; ptr++) {
		सचे (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			सचे (*ptr == '\r' &&
			    (ptr + 1 < शिखरम् && ptr[1] == '\n'))
				अनुवर्त;
		}
		अञ्ञथा सचे (XDL_ISSPACE(*ptr)) {
			ध्रुव वर्ण *ptr2 = ptr;
			पूर्णाङ्क at_eol;
			याव (ptr + 1 < शिखरम् && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (शिखरम् <= ptr + 1 || ptr[1] == '\n');
			सचे (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			अञ्ञथा सचे (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (unsigned long) ' ';
			}
			अञ्ञथा सचे (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				याव (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (unsigned long) *ptr2;
					ptr2++;
				}
			}
			अनुवर्त;
		}
		ha += (ha << 5);
		ha ^= (unsigned long) *ptr;
	}
	*data = ptr < शिखरम् ? ptr + 1: ptr;

	पटिनिवत्त ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#अञ्ञथा
#define REASSOC_FENCE(x, y)
#endif

unsigned long xdl_hash_record_verbatim(वर्ण ध्रुव **data, वर्ण ध्रुव *शिखरम्) {
	unsigned long ha = 5381, c0, c1;
	वर्ण ध्रुव *ptr = *data;
#सचे 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	अत्थाय (; ptr < शिखरम् && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (unsigned long) *ptr;
	}
	*data = ptr < शिखरम् ? ptr + 1: ptr;
#अञ्ञथा
	/* Process two characters per iteration. */
	सचे (शिखरम् - ptr >= 2) करोहि {
		सचे ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			पटिनिवत्त ha;
		}
		सचे ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			पटिनिवत्त ha;
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
	} याव (ptr < शिखरम् - 1);
	*data = शिखरम्;
	सचे (ptr < शिखरम् && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	पटिनिवत्त ha;
}

unsigned पूर्णाङ्क xdl_hashbits(unsigned पूर्णाङ्क size) {
	unsigned पूर्णाङ्क val = 1, bits = 0;

	अत्थाय (; val < size && bits < CHAR_BIT * परिमाण(unsigned पूर्णाङ्क); val <<= 1, bits++);
	पटिनिवत्त bits ? bits: 1;
}


पूर्णाङ्क xdl_अङ्कः_out(वर्ण *out, long val) {
	वर्ण *ptr, *str = out;
	वर्ण buf[32];

	ptr = buf + परिमाण(buf) - 1;
	*ptr = '\0';
	सचे (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	अत्थाय (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	सचे (*ptr)
		अत्थाय (; *ptr; ptr++, str++)
			*str = *ptr;
	अञ्ञथा
		*str++ = '0';
	*str = '\0';

	पटिनिवत्त str - out;
}

स्थिर पूर्णाङ्क xdl_format_hunk_hdr(long s1, long c1, long s2, long c2,
			       ध्रुव वर्ण *func, long funclen,
			       xdemitcb_t *ecb) {
	पूर्णाङ्क nb = 0;
	mmbuffer_t mb;
	वर्ण buf[128];

	memcpy(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_अङ्कः_out(buf + nb, c1 ? s1: s1 - 1);

	सचे (c1 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_अङ्कः_out(buf + nb, c1);
	}

	memcpy(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_अङ्कः_out(buf + nb, c2 ? s2: s2 - 1);

	सचे (c2 != 1) {
		memcpy(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_अङ्कः_out(buf + nb, c2);
	}

	memcpy(buf + nb, " @@", 3);
	nb += 3;
	सचे (func && funclen) {
		buf[nb++] = ' ';
		सचे ((size_t)funclen > परिमाण(buf) - nb - 1)
			funclen = परिमाण(buf) - nb - 1;
		memcpy(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	सचे (ecb->out_पङ्क्तिः(ecb->priv, &mb, 1) < 0)
		पटिनिवत्त -1;
	पटिनिवत्त 0;
}

पूर्णाङ्क xdl_emit_hunk_hdr(long s1, long c1, long s2, long c2,
		      ध्रुव वर्ण *func, long funclen,
		      xdemitcb_t *ecb) {
	सचे (!ecb->out_hunk)
		पटिनिवत्त xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	सचे (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		पटिनिवत्त -1;
	पटिनिवत्त 0;
}

पूर्णाङ्क xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t ध्रुव *xpp,
		पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2)
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

	subfile1.ptr = (वर्ण *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (वर्ण *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	सचे (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		पटिनिवत्त -1;

	memcpy(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	memcpy(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	पटिनिवत्त 0;
}

शून्य* xdl_alloc_grow_helper(शून्य *p, long nr, long *alloc, size_t size)
{
	शून्य *tmp = रिक्त;
	size_t n = ((LONG_MAX - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : LONG_MAX;
	सचे ((size_t)nr > n)
		n = nr;
	सचे (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	सचे (tmp) {
		*alloc = (long)n;
	} अञ्ञथा {
		xdl_free(p);
		*alloc = 0;
	}
	पटिनिवत्त tmp;
}
