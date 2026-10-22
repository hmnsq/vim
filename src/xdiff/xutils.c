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


दीर्घ xdl_bogosqrt(दीर्घ n) {
	दीर्घ i;

	/*
	 * Classical integer square root approximation using shifts.
	 */
	लागि (i = 1; n > 0; n >>= 2)
		i <<= 1;

	फर्काऊ i;
}


पूर्णाङ्क xdl_emit_diffrec(वर्ण अचर *rec, दीर्घ size, वर्ण अचर *pre, दीर्घ psize,
		     xdemitcb_t *ecb) {
	पूर्णाङ्क i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (वर्ण *) pre;
	mb[0].size = psize;
	mb[1].ptr = (वर्ण *) rec;
	mb[1].size = size;
	यदि (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (वर्ण *) "\n\\ No newline at end of file\n";
		mb[2].size = (दीर्घ)म_माप(mb[2].ptr);
		i++;
	}
	यदि (ecb->out_पङ्क्ति(ecb->priv, mb, i) < 0) {

		फर्काऊ -1;
	}

	फर्काऊ 0;
}

शून्य *xdl_mmfile_first(mmfile_t *mmf, दीर्घ *size)
{
	*size = mmf->size;
	फर्काऊ mmf->ptr;
}


दीर्घ xdl_mmfile_size(mmfile_t *mmf)
{
	फर्काऊ mmf->size;
}


पूर्णाङ्क xdl_cha_init(chastore_t *cha, दीर्घ isize, दीर्घ icount) {

	cha->head = cha->tail = रिक्त;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = रिक्त;
	cha->scurr = 0;

	फर्काऊ 0;
}


शून्य xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	लागि (cur = cha->head; (tmp = cur) != रिक्त;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


शून्य *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	शून्य *data;

	यदि (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		यदि (!(ancur = (chanode_t *) xdl_malloc(परिमाण(chanode_t) + cha->nsize))) {

			फर्काऊ रिक्त;
		}
		ancur->icurr = 0;
		ancur->next = रिक्त;
		यदि (cha->tail)
			cha->tail->next = ancur;
		यदि (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (वर्ण *) ancur + परिमाण(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	फर्काऊ data;
}

दीर्घ xdl_guess_पङ्क्तिहरू(mmfile_t *mf, दीर्घ sample) {
	दीर्घ nl = 0, size, tsize = 0;
	वर्ण अचर *data, *cur, *शीर्ष;

	यदि ((cur = data = xdl_mmfile_first(mf, &size))) {
		लागि (शीर्ष = data + size; nl < sample && cur < शीर्ष; ) {
			nl++;
			यदि (!(cur = स_प्रथम(cur, '\n', शीर्ष - cur)))
				cur = शीर्ष;
			अन्यथा
				cur++;
		}
		tsize += (दीर्घ) (cur - data);
	}

	यदि (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	फर्काऊ nl + 1;
}

पूर्णाङ्क xdl_blankline(अचर वर्ण *पङ्क्ति, दीर्घ size, दीर्घ flags)
{
	दीर्घ i;

	यदि (!(flags & XDF_WHITESPACE_FLAGS))
		फर्काऊ (size <= 1);

	लागि (i = 0; i < size && XDL_ISSPACE(पङ्क्ति[i]); i++)
		;

	फर्काऊ (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
स्थिर पूर्णाङ्क ends_with_optional_cr(अचर वर्ण *l, दीर्घ s, दीर्घ i)
{
	पूर्णाङ्क complete = s && l[s-1] == '\n';

	यदि (complete)
		s--;
	यदि (s == i)
		फर्काऊ 1;
	/* do not ignore CR at the end of an incomplete line */
	यदि (complete && s == i + 1 && l[i] == '\r')
		फर्काऊ 1;
	फर्काऊ 0;
}

पूर्णाङ्क xdl_recmatch(अचर वर्ण *l1, दीर्घ s1, अचर वर्ण *l2, दीर्घ s2, दीर्घ flags)
{
	पूर्णाङ्क i1, i2;

	यदि (s1 == s2 && !स_भेद(l1, l2, s1))
		फर्काऊ 1;
	यदि (!(flags & XDF_WHITESPACE_FLAGS))
		फर्काऊ 0;

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
	यदि (flags & XDF_IGNORE_WHITESPACE) {
		गच्छ skip_ws;
		जबसम्म (i1 < s1 && i2 < s2) {
			यदि (l1[i1++] != l2[i2++])
				फर्काऊ 0;
		skip_ws:
			जबसम्म (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			जबसम्म (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} अन्यथा यदि (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		जबसम्म (i1 < s1 && i2 < s2) {
			यदि (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				जबसम्म (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				जबसम्म (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				जारी;
			}
			यदि (l1[i1++] != l2[i2++])
				फर्काऊ 0;
		}
	} अन्यथा यदि (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		जबसम्म (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} अन्यथा यदि (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		जबसम्म (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		फर्काऊ (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	यदि (i1 < s1) {
		जबसम्म (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		यदि (s1 != i1)
			फर्काऊ 0;
	}
	यदि (i2 < s2) {
		जबसम्म (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		फर्काऊ (s2 == i2);
	}
	फर्काऊ 1;
}

अचिन्हित दीर्घ xdl_hash_record_with_whitespace(वर्ण अचर **data,
		वर्ण अचर *शीर्ष, दीर्घ flags) {
	अचिन्हित दीर्घ ha = 5381;
	वर्ण अचर *ptr = *data;
	पूर्णाङ्क cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	लागि (; ptr < शीर्ष && *ptr != '\n'; ptr++) {
		यदि (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			यदि (*ptr == '\r' &&
			    (ptr + 1 < शीर्ष && ptr[1] == '\n'))
				जारी;
		}
		अन्यथा यदि (XDL_ISSPACE(*ptr)) {
			अचर वर्ण *ptr2 = ptr;
			पूर्णाङ्क at_eol;
			जबसम्म (ptr + 1 < शीर्ष && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (शीर्ष <= ptr + 1 || ptr[1] == '\n');
			यदि (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			अन्यथा यदि (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (अचिन्हित दीर्घ) ' ';
			}
			अन्यथा यदि (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				जबसम्म (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (अचिन्हित दीर्घ) *ptr2;
					ptr2++;
				}
			}
			जारी;
		}
		ha += (ha << 5);
		ha ^= (अचिन्हित दीर्घ) *ptr;
	}
	*data = ptr < शीर्ष ? ptr + 1: ptr;

	फर्काऊ ha;
}

/*
 * Compiler reassociation barrier: pretend to modify X and Y to disallow
 * changing evaluation order with respect to following uses of X and Y.
 */
#ifdef __GNUC__
#define REASSOC_FENCE(x, y) __asm__("" : "+r"(x), "+r"(y))
#अन्यथा
#define REASSOC_FENCE(x, y)
#endif

अचिन्हित दीर्घ xdl_hash_record_verbatim(वर्ण अचर **data, वर्ण अचर *शीर्ष) {
	अचिन्हित दीर्घ ha = 5381, c0, c1;
	वर्ण अचर *ptr = *data;
#यदि 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	लागि (; ptr < शीर्ष && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (अचिन्हित दीर्घ) *ptr;
	}
	*data = ptr < शीर्ष ? ptr + 1: ptr;
#अन्यथा
	/* Process two characters per iteration. */
	यदि (शीर्ष - ptr >= 2) गर {
		यदि ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			फर्काऊ ha;
		}
		यदि ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			फर्काऊ ha;
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
	} जबसम्म (ptr < शीर्ष - 1);
	*data = शीर्ष;
	यदि (ptr < शीर्ष && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	फर्काऊ ha;
}

अचिन्हित पूर्णाङ्क xdl_hashbits(अचिन्हित पूर्णाङ्क size) {
	अचिन्हित पूर्णाङ्क val = 1, bits = 0;

	लागि (; val < size && bits < अक्षर_बिट * परिमाण(अचिन्हित पूर्णाङ्क); val <<= 1, bits++);
	फर्काऊ bits ? bits: 1;
}


पूर्णाङ्क xdl_अङ्क_out(वर्ण *out, दीर्घ val) {
	वर्ण *ptr, *str = out;
	वर्ण buf[32];

	ptr = buf + परिमाण(buf) - 1;
	*ptr = '\0';
	यदि (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	लागि (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	यदि (*ptr)
		लागि (; *ptr; ptr++, str++)
			*str = *ptr;
	अन्यथा
		*str++ = '0';
	*str = '\0';

	फर्काऊ str - out;
}

स्थिर पूर्णाङ्क xdl_format_hunk_hdr(दीर्घ s1, दीर्घ c1, दीर्घ s2, दीर्घ c2,
			       अचर वर्ण *func, दीर्घ funclen,
			       xdemitcb_t *ecb) {
	पूर्णाङ्क nb = 0;
	mmbuffer_t mb;
	वर्ण buf[128];

	स_नकल(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_अङ्क_out(buf + nb, c1 ? s1: s1 - 1);

	यदि (c1 != 1) {
		स_नकल(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_अङ्क_out(buf + nb, c1);
	}

	स_नकल(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_अङ्क_out(buf + nb, c2 ? s2: s2 - 1);

	यदि (c2 != 1) {
		स_नकल(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_अङ्क_out(buf + nb, c2);
	}

	स_नकल(buf + nb, " @@", 3);
	nb += 3;
	यदि (func && funclen) {
		buf[nb++] = ' ';
		यदि ((माप_प्रकार)funclen > परिमाण(buf) - nb - 1)
			funclen = परिमाण(buf) - nb - 1;
		स_नकल(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	यदि (ecb->out_पङ्क्ति(ecb->priv, &mb, 1) < 0)
		फर्काऊ -1;
	फर्काऊ 0;
}

पूर्णाङ्क xdl_emit_hunk_hdr(दीर्घ s1, दीर्घ c1, दीर्घ s2, दीर्घ c2,
		      अचर वर्ण *func, दीर्घ funclen,
		      xdemitcb_t *ecb) {
	यदि (!ecb->out_hunk)
		फर्काऊ xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	यदि (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		फर्काऊ -1;
	फर्काऊ 0;
}

पूर्णाङ्क xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t अचर *xpp,
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
	यदि (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		फर्काऊ -1;

	स_नकल(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	स_नकल(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	फर्काऊ 0;
}

शून्य* xdl_alloc_grow_helper(शून्य *p, दीर्घ nr, दीर्घ *alloc, माप_प्रकार size)
{
	शून्य *tmp = रिक्त;
	माप_प्रकार n = ((दीर्घ_उच्च - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : दीर्घ_उच्च;
	यदि ((माप_प्रकार)nr > n)
		n = nr;
	यदि (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	यदि (tmp) {
		*alloc = (दीर्घ)n;
	} अन्यथा {
		xdl_free(p);
		*alloc = 0;
	}
	फर्काऊ tmp;
}
