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
	साठी (i = 1; n > 0; n >>= 2)
		i <<= 1;

	परत i;
}


पूर्णांक xdl_emit_diffrec(अक्षर स्थिरांक *rec, दीर्घ size, अक्षर स्थिरांक *pre, दीर्घ psize,
		     xdemitcb_t *ecb) {
	पूर्णांक i = 2;
	mmbuffer_t mb[3];

	mb[0].ptr = (अक्षर *) pre;
	mb[0].size = psize;
	mb[1].ptr = (अक्षर *) rec;
	mb[1].size = size;
	जर (size > 0 && rec[size - 1] != '\n') {
		mb[2].ptr = (अक्षर *) "\n\\ No newline at end of file\n";
		mb[2].size = (दीर्घ)म_माप(mb[2].ptr);
		i++;
	}
	जर (ecb->out_ओळ(ecb->priv, mb, i) < 0) {

		परत -1;
	}

	परत 0;
}

शून्य *xdl_mmfile_first(mmfile_t *mmf, दीर्घ *size)
{
	*size = mmf->size;
	परत mmf->ptr;
}


दीर्घ xdl_mmfile_size(mmfile_t *mmf)
{
	परत mmf->size;
}


पूर्णांक xdl_cha_init(chastore_t *cha, दीर्घ isize, दीर्घ icount) {

	cha->head = cha->tail = रिक्त;
	cha->isize = isize;
	cha->nsize = icount * isize;
	cha->ancur = cha->sncur = रिक्त;
	cha->scurr = 0;

	परत 0;
}


शून्य xdl_cha_free(chastore_t *cha) {
	chanode_t *cur, *tmp;

	साठी (cur = cha->head; (tmp = cur) != रिक्त;) {
		cur = cur->next;
		xdl_free(tmp);
	}
}


शून्य *xdl_cha_alloc(chastore_t *cha) {
	chanode_t *ancur;
	शून्य *data;

	जर (!(ancur = cha->ancur) || ancur->icurr == cha->nsize) {
		जर (!(ancur = (chanode_t *) xdl_malloc(आकार(chanode_t) + cha->nsize))) {

			परत रिक्त;
		}
		ancur->icurr = 0;
		ancur->next = रिक्त;
		जर (cha->tail)
			cha->tail->next = ancur;
		जर (!cha->head)
			cha->head = ancur;
		cha->tail = ancur;
		cha->ancur = ancur;
	}

	data = (अक्षर *) ancur + आकार(chanode_t) + ancur->icurr;
	ancur->icurr += cha->isize;

	परत data;
}

दीर्घ xdl_guess_ओळी(mmfile_t *mf, दीर्घ sample) {
	दीर्घ nl = 0, size, tsize = 0;
	अक्षर स्थिरांक *data, *cur, *शीर्ष;

	जर ((cur = data = xdl_mmfile_first(mf, &size))) {
		साठी (शीर्ष = data + size; nl < sample && cur < शीर्ष; ) {
			nl++;
			जर (!(cur = स_प्रथम(cur, '\n', शीर्ष - cur)))
				cur = शीर्ष;
			अन्यथा
				cur++;
		}
		tsize += (दीर्घ) (cur - data);
	}

	जर (nl && tsize)
		nl = xdl_mmfile_size(mf) / (tsize / nl);

	परत nl + 1;
}

पूर्णांक xdl_blankline(स्थिरांक अक्षर *ओळ, दीर्घ size, दीर्घ flags)
{
	दीर्घ i;

	जर (!(flags & XDF_WHITESPACE_FLAGS))
		परत (size <= 1);

	साठी (i = 0; i < size && XDL_ISSPACE(ओळ[i]); i++)
		;

	परत (i == size);
}

/*
 * Have we eaten everything on the line, except for an optional
 * CR at the very end?
 */
स्थिर पूर्णांक ends_with_optional_cr(स्थिरांक अक्षर *l, दीर्घ s, दीर्घ i)
{
	पूर्णांक complete = s && l[s-1] == '\n';

	जर (complete)
		s--;
	जर (s == i)
		परत 1;
	/* do not ignore CR at the end of an incomplete line */
	जर (complete && s == i + 1 && l[i] == '\r')
		परत 1;
	परत 0;
}

पूर्णांक xdl_recmatch(स्थिरांक अक्षर *l1, दीर्घ s1, स्थिरांक अक्षर *l2, दीर्घ s2, दीर्घ flags)
{
	पूर्णांक i1, i2;

	जर (s1 == s2 && !स_भेद(l1, l2, s1))
		परत 1;
	जर (!(flags & XDF_WHITESPACE_FLAGS))
		परत 0;

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
	जर (flags & XDF_IGNORE_WHITESPACE) {
		जा skip_ws;
		पर्यंत (i1 < s1 && i2 < s2) {
			जर (l1[i1++] != l2[i2++])
				परत 0;
		skip_ws:
			पर्यंत (i1 < s1 && XDL_ISSPACE(l1[i1]))
				i1++;
			पर्यंत (i2 < s2 && XDL_ISSPACE(l2[i2]))
				i2++;
		}
	} अन्यथा जर (flags & XDF_IGNORE_WHITESPACE_CHANGE) {
		पर्यंत (i1 < s1 && i2 < s2) {
			जर (XDL_ISSPACE(l1[i1]) && XDL_ISSPACE(l2[i2])) {
				/* Skip matching spaces and try again */
				पर्यंत (i1 < s1 && XDL_ISSPACE(l1[i1]))
					i1++;
				पर्यंत (i2 < s2 && XDL_ISSPACE(l2[i2]))
					i2++;
				सुरूठेवा;
			}
			जर (l1[i1++] != l2[i2++])
				परत 0;
		}
	} अन्यथा जर (flags & XDF_IGNORE_WHITESPACE_AT_EOL) {
		पर्यंत (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
	} अन्यथा जर (flags & XDF_IGNORE_CR_AT_EOL) {
		/* Find the first difference and see how the line ends */
		पर्यंत (i1 < s1 && i2 < s2 && l1[i1] == l2[i2]) {
			i1++;
			i2++;
		}
		परत (ends_with_optional_cr(l1, s1, i1) &&
			ends_with_optional_cr(l2, s2, i2));
	}

	/*
	 * After running out of one side, the remaining side must have
	 * nothing but whitespace for the lines to match.  Note that
	 * ignore-whitespace-at-eol case may break out of the loop
	 * while there still are characters remaining on both lines.
	 */
	जर (i1 < s1) {
		पर्यंत (i1 < s1 && XDL_ISSPACE(l1[i1]))
			i1++;
		जर (s1 != i1)
			परत 0;
	}
	जर (i2 < s2) {
		पर्यंत (i2 < s2 && XDL_ISSPACE(l2[i2]))
			i2++;
		परत (s2 == i2);
	}
	परत 1;
}

अचिन्हित दीर्घ xdl_hash_record_with_whitespace(अक्षर स्थिरांक **data,
		अक्षर स्थिरांक *शीर्ष, दीर्घ flags) {
	अचिन्हित दीर्घ ha = 5381;
	अक्षर स्थिरांक *ptr = *data;
	पूर्णांक cr_at_eol_only = (flags & XDF_WHITESPACE_FLAGS) == XDF_IGNORE_CR_AT_EOL;

	साठी (; ptr < शीर्ष && *ptr != '\n'; ptr++) {
		जर (cr_at_eol_only) {
			/* do not ignore CR at the end of an incomplete line */
			जर (*ptr == '\r' &&
			    (ptr + 1 < शीर्ष && ptr[1] == '\n'))
				सुरूठेवा;
		}
		अन्यथा जर (XDL_ISSPACE(*ptr)) {
			स्थिरांक अक्षर *ptr2 = ptr;
			पूर्णांक at_eol;
			पर्यंत (ptr + 1 < शीर्ष && XDL_ISSPACE(ptr[1])
					&& ptr[1] != '\n')
				ptr++;
			at_eol = (शीर्ष <= ptr + 1 || ptr[1] == '\n');
			जर (flags & XDF_IGNORE_WHITESPACE)
				; /* already handled */
			अन्यथा जर (flags & XDF_IGNORE_WHITESPACE_CHANGE
				 && !at_eol) {
				ha += (ha << 5);
				ha ^= (अचिन्हित दीर्घ) ' ';
			}
			अन्यथा जर (flags & XDF_IGNORE_WHITESPACE_AT_EOL
				 && !at_eol) {
				पर्यंत (ptr2 != ptr + 1) {
					ha += (ha << 5);
					ha ^= (अचिन्हित दीर्घ) *ptr2;
					ptr2++;
				}
			}
			सुरूठेवा;
		}
		ha += (ha << 5);
		ha ^= (अचिन्हित दीर्घ) *ptr;
	}
	*data = ptr < शीर्ष ? ptr + 1: ptr;

	परत ha;
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

अचिन्हित दीर्घ xdl_hash_record_verbatim(अक्षर स्थिरांक **data, अक्षर स्थिरांक *शीर्ष) {
	अचिन्हित दीर्घ ha = 5381, c0, c1;
	अक्षर स्थिरांक *ptr = *data;
#जर 0
	/*
	 * The baseline form of the optimized loop below. This is the djb2
	 * hash (the above function uses a variant with XOR instead of ADD).
	 */
	साठी (; ptr < शीर्ष && *ptr != '\n'; ptr++) {
		ha += (ha << 5);
		ha += (अचिन्हित दीर्घ) *ptr;
	}
	*data = ptr < शीर्ष ? ptr + 1: ptr;
#अन्यथा
	/* Process two characters per iteration. */
	जर (शीर्ष - ptr >= 2) करा {
		जर ((c0 = ptr[0]) == '\n') {
			*data = ptr + 1;
			परत ha;
		}
		जर ((c1 = ptr[1]) == '\n') {
			*data = ptr + 2;
			c0 += ha;
			REASSOC_FENCE(c0, ha);
			ha = ha * 32 + c0;
			परत ha;
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
	} पर्यंत (ptr < शीर्ष - 1);
	*data = शीर्ष;
	जर (ptr < शीर्ष && (c0 = ptr[0]) != '\n') {
		c0 += ha;
		REASSOC_FENCE(c0, ha);
		ha = ha * 32 + c0;
	}
#endif
	परत ha;
}

अचिन्हित पूर्णांक xdl_hashbits(अचिन्हित पूर्णांक size) {
	अचिन्हित पूर्णांक val = 1, bits = 0;

	साठी (; val < size && bits < अक्षर_बिट * आकार(अचिन्हित पूर्णांक); val <<= 1, bits++);
	परत bits ? bits: 1;
}


पूर्णांक xdl_अंक_out(अक्षर *out, दीर्घ val) {
	अक्षर *ptr, *str = out;
	अक्षर buf[32];

	ptr = buf + आकार(buf) - 1;
	*ptr = '\0';
	जर (val < 0) {
		*--ptr = '-';
		val = -val;
	}
	साठी (; val && ptr > buf; val /= 10)
		*--ptr = "0123456789"[val % 10];
	जर (*ptr)
		साठी (; *ptr; ptr++, str++)
			*str = *ptr;
	अन्यथा
		*str++ = '0';
	*str = '\0';

	परत str - out;
}

स्थिर पूर्णांक xdl_format_hunk_hdr(दीर्घ s1, दीर्घ c1, दीर्घ s2, दीर्घ c2,
			       स्थिरांक अक्षर *func, दीर्घ funclen,
			       xdemitcb_t *ecb) {
	पूर्णांक nb = 0;
	mmbuffer_t mb;
	अक्षर buf[128];

	स_नकल(buf, "@@ -", 4);
	nb += 4;

	nb += xdl_अंक_out(buf + nb, c1 ? s1: s1 - 1);

	जर (c1 != 1) {
		स_नकल(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_अंक_out(buf + nb, c1);
	}

	स_नकल(buf + nb, " +", 2);
	nb += 2;

	nb += xdl_अंक_out(buf + nb, c2 ? s2: s2 - 1);

	जर (c2 != 1) {
		स_नकल(buf + nb, ",", 1);
		nb += 1;

		nb += xdl_अंक_out(buf + nb, c2);
	}

	स_नकल(buf + nb, " @@", 3);
	nb += 3;
	जर (func && funclen) {
		buf[nb++] = ' ';
		जर ((माप_प्रकार)funclen > आकार(buf) - nb - 1)
			funclen = आकार(buf) - nb - 1;
		स_नकल(buf + nb, func, funclen);
		nb += funclen;
	}
	buf[nb++] = '\n';

	mb.ptr = buf;
	mb.size = nb;
	जर (ecb->out_ओळ(ecb->priv, &mb, 1) < 0)
		परत -1;
	परत 0;
}

पूर्णांक xdl_emit_hunk_hdr(दीर्घ s1, दीर्घ c1, दीर्घ s2, दीर्घ c2,
		      स्थिरांक अक्षर *func, दीर्घ funclen,
		      xdemitcb_t *ecb) {
	जर (!ecb->out_hunk)
		परत xdl_format_hunk_hdr(s1, c1, s2, c2, func, funclen, ecb);
	जर (ecb->out_hunk(ecb->priv,
			  c1 ? s1 : s1 - 1, c1,
			  c2 ? s2 : s2 - 1, c2,
			  func, funclen) < 0)
		परत -1;
	परत 0;
}

पूर्णांक xdl_fall_back_diff(xdfenv_t *diff_env, xpparam_t स्थिरांक *xpp,
		पूर्णांक line1, पूर्णांक count1, पूर्णांक line2, पूर्णांक count2)
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

	subfile1.ptr = (अक्षर *)diff_env->xdf1.recs[line1 - 1].ptr;
	subfile1.size = diff_env->xdf1.recs[line1 + count1 - 2].ptr +
		diff_env->xdf1.recs[line1 + count1 - 2].size - subfile1.ptr;
	subfile2.ptr = (अक्षर *)diff_env->xdf2.recs[line2 - 1].ptr;
	subfile2.size = diff_env->xdf2.recs[line2 + count2 - 2].ptr +
		diff_env->xdf2.recs[line2 + count2 - 2].size - subfile2.ptr;
	जर (xdl_do_diff(&subfile1, &subfile2, xpp, &env) < 0)
		परत -1;

	स_नकल(diff_env->xdf1.changed + line1 - 1, env.xdf1.changed, count1);
	स_नकल(diff_env->xdf2.changed + line2 - 1, env.xdf2.changed, count2);

	xdl_free_env(&env);

	परत 0;
}

शून्य* xdl_alloc_grow_helper(शून्य *p, दीर्घ nr, दीर्घ *alloc, माप_प्रकार size)
{
	शून्य *tmp = रिक्त;
	माप_प्रकार n = ((दीर्घ_उच्च - 16) / 2 >= *alloc) ? 2 * *alloc + 16 : दीर्घ_उच्च;
	जर ((माप_प्रकार)nr > n)
		n = nr;
	जर (SIZE_MAX / size >= n)
		tmp = xdl_realloc(p, n * size);
	जर (tmp) {
		*alloc = (दीर्घ)n;
	} अन्यथा {
		xdl_free(p);
		*alloc = 0;
	}
	परत tmp;
}
