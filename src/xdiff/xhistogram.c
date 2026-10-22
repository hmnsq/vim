/*
 * Copyright (C) 2010, Google Inc.
 * and other copyright owners as documented in JGit's IP log.
 *
 * This program and the accompanying materials are made available
 * under the terms of the Eclipse Distribution License v1.0 which
 * accompanies this distribution, is reproduced below, and is
 * available at http://www.eclipse.org/org/documents/edl-v10.php
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 *
 * - Neither the name of the Eclipse Foundation, Inc. nor the
 *   names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior
 *   written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#അന്തര്ഭാവ "xinclude.h"

#define MAX_PTR	UINT_MAX
#define MAX_CNT	UINT_MAX

#define വരി_END(n) (വരി##n + എണ്ണം##n - 1)
#define വരി_END_PTR(n) (*വരി##n + *എണ്ണം##n - 1)

ഘടന histindex {
	ഘടന record {
		unsigned പൂർണ്ണസംഖ്യ ptr, cnt;
		ഘടന record *next;
	} **records, /* an occurrence */
	  **വരി_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned പൂർണ്ണസംഖ്യ *next_ptrs;
	unsigned പൂർണ്ണസംഖ്യ table_bits,
		     records_size,
		     വരി_map_size;

	unsigned പൂർണ്ണസംഖ്യ max_chain_length,
		     താക്കോൽ_shift,
		     ptr_shift;

	unsigned പൂർണ്ണസംഖ്യ cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t സ്ഥിരം *xpp;
};

ഘടന region {
	unsigned പൂർണ്ണസംഖ്യ begin1, end1;
	unsigned പൂർണ്ണസംഖ്യ begin2, end2;
};

#define വരി_MAP(i, a) (i->വരി_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((വരി_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

സ്ഥിതം പൂർണ്ണസംഖ്യ cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	തിരികെ r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, വരി) \
	XDL_HASHLONG((REC(index->env, side, വരി))->ha, index->table_bits)

സ്ഥിതം പൂർണ്ണസംഖ്യ scanA(ഘടന histindex *index, പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1)
{
	unsigned പൂർണ്ണസംഖ്യ ptr, tbl_idx;
	unsigned പൂർണ്ണസംഖ്യ chain_len;
	ഘടന record **rec_chain, *rec;

	വേണ്ടി (ptr = വരി_END(1); (unsigned പൂർണ്ണസംഖ്യ)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		ആകുമ്പോൾ (rec) {
			എങ്കിൽ (CMP(index, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				NEXT_PTR(index, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				വരി_MAP(index, ptr) = rec;
				പോകുക continue_scan;
			}

			rec = rec->next;
			chain_len++;
		}

		എങ്കിൽ (chain_len == index->max_chain_length)
			തിരികെ -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		എങ്കിൽ (!(rec = xdl_cha_alloc(&index->rcha)))
			തിരികെ -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		വരി_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ try_lcs(ഘടന histindex *index, ഘടന region *lcs, പൂർണ്ണസംഖ്യ b_ptr,
	പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2)
{
	unsigned പൂർണ്ണസംഖ്യ b_next = b_ptr + 1;
	ഘടന record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned പൂർണ്ണസംഖ്യ as, ae, bs, be, np, rc;
	പൂർണ്ണസംഖ്യ should_break;

	വേണ്ടി (; rec; rec = rec->next) {
		എങ്കിൽ (rec->cnt > index->cnt) {
			എങ്കിൽ (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			തുടരുക;
		}

		as = rec->ptr;
		എങ്കിൽ (!CMP(index, 1, as, 2, b_ptr))
			തുടരുക;

		index->has_common = 1;
		വേണ്ടി (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			ആകുമ്പോൾ ((unsigned പൂർണ്ണസംഖ്യ)line1 < as && (unsigned പൂർണ്ണസംഖ്യ)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				എങ്കിൽ (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			ആകുമ്പോൾ (ae < (unsigned പൂർണ്ണസംഖ്യ)വരി_END(1) && be < (unsigned പൂർണ്ണസംഖ്യ)വരി_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				എങ്കിൽ (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			എങ്കിൽ (b_next <= be)
				b_next = be + 1;
			എങ്കിൽ (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			എങ്കിൽ (np == 0)
				നിർത്തുക;

			ആകുമ്പോൾ (np <= ae) {
				np = NEXT_PTR(index, np);
				എങ്കിൽ (np == 0) {
					should_break = 1;
					നിർത്തുക;
				}
			}

			എങ്കിൽ (should_break)
				നിർത്തുക;

			as = np;
		}
	}
	തിരികെ b_next;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fall_back_to_classic_diff(xpparam_t സ്ഥിരം *xpp, xdfenv_t *env,
		പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, വലുപ്പം(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	തിരികെ xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

സ്ഥിതം വരിയിൽ ശൂന്യം free_index(ഘടന histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->വരി_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ find_lcs(xpparam_t സ്ഥിരം *xpp, xdfenv_t *env,
		    ഘടന region *lcs,
		    പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2)
{
	പൂർണ്ണസംഖ്യ b_ptr;
	പൂർണ്ണസംഖ്യ ret = -1;
	ഘടന histindex index;

	memset(&index, 0, വലുപ്പം(index));

	index.env = env;
	index.xpp = xpp;

	index.records = ശൂന്യ;
	index.വരി_map = ശൂന്യ;
	/* in case of early xdl_cha_free() */
	index.rcha.head = ശൂന്യ;

	index.table_bits = xdl_hashbits(count1);
	index.records_size = 1 << index.table_bits;
	എങ്കിൽ (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		പോകുക cleanup;

	index.വരി_map_size = count1;
	എങ്കിൽ (!XDL_CALLOC_ARRAY(index.വരി_map, index.വരി_map_size))
		പോകുക cleanup;

	എങ്കിൽ (!XDL_CALLOC_ARRAY(index.next_ptrs, index.വരി_map_size))
		പോകുക cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	എങ്കിൽ (xdl_cha_init(&index.rcha, വലുപ്പം(ഘടന record), count1 / 4 + 1) < 0)
		പോകുക cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	എങ്കിൽ (scanA(&index, line1, count1))
		പോകുക cleanup;

	index.cnt = index.max_chain_length + 1;

	വേണ്ടി (b_ptr = line2; b_ptr <= വരി_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	എങ്കിൽ (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	അല്ലെങ്കിൽ
		ret = 0;

cleanup:
	free_index(&index);
	തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ histogram_diff(xpparam_t സ്ഥിരം *xpp, xdfenv_t *env,
	പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2)
{
	ഘടന region lcs;
	പൂർണ്ണസംഖ്യ lcs_found;
	പൂർണ്ണസംഖ്യ result;
redo:
	result = -1;

	എങ്കിൽ (count1 <= 0 && count2 <= 0)
		തിരികെ 0;

	എങ്കിൽ ((unsigned പൂർണ്ണസംഖ്യ)വരി_END(1) >= MAX_PTR)
		തിരികെ -1;

	എങ്കിൽ (!count1) {
		ആകുമ്പോൾ(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		തിരികെ 0;
	} അല്ലെങ്കിൽ എങ്കിൽ (!count2) {
		ആകുമ്പോൾ(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		തിരികെ 0;
	}

	memset(&lcs, 0, വലുപ്പം(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	എങ്കിൽ (lcs_found < 0)
		പോകുക out;
	അല്ലെങ്കിൽ എങ്കിൽ (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	അല്ലെങ്കിൽ {
		എങ്കിൽ (lcs.begin1 == 0 && lcs.begin2 == 0) {
			ആകുമ്പോൾ (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			ആകുമ്പോൾ (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} അല്ലെങ്കിൽ {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			എങ്കിൽ (result)
				പോകുക out;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = വരി_END(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = വരി_END(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			പോകുക redo;
		}
	}
out:
	തിരികെ result;
}

പൂർണ്ണസംഖ്യ xdl_do_histogram_diff(xpparam_t സ്ഥിരം *xpp, xdfenv_t *env)
{
	തിരികെ histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
