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

#અન્તર્ભાવ "xinclude.h"

#define MAX_PTR	UINT_MAX
#define MAX_CNT	UINT_MAX

#define લીટી_END(n) (લીટી##n + ગણતરી##n - 1)
#define લીટી_END_PTR(n) (*લીટી##n + *ગણતરી##n - 1)

સંરચના histindex {
	સંરચના record {
		unsigned પૂર્ણાંક ptr, cnt;
		સંરચના record *next;
	} **records, /* an occurrence */
	  **લીટી_map; /* map of line to record chain */
	chastore_t rcha;
	unsigned પૂર્ણાંક *next_ptrs;
	unsigned પૂર્ણાંક table_bits,
		     records_size,
		     લીટી_map_size;

	unsigned પૂર્ણાંક max_chain_length,
		     ચાવી_shift,
		     ptr_shift;

	unsigned પૂર્ણાંક cnt,
		     has_common;

	xdfenv_t *env;
	xpparam_t અચલ *xpp;
};

સંરચના region {
	unsigned પૂર્ણાંક begin1, end1;
	unsigned પૂર્ણાંક begin2, end2;
};

#define લીટી_MAP(i, a) (i->લીટી_map[(a) - i->ptr_shift])

#define NEXT_PTR(index, ptr) \
	(index->next_ptrs[(ptr) - index->ptr_shift])

#define CNT(index, ptr) \
	((લીટી_MAP(index, ptr))->cnt)

#define REC(env, s, l) \
	(&env->xdf##s.recs[l - 1])

સ્થિર પૂર્ણાંક cmp_recs(xrecord_t *r1, xrecord_t *r2)
{
	પાછા r1->ha == r2->ha;

}

#define CMP(i, s1, l1, s2, l2) \
	(cmp_recs(REC(i->env, s1, l1), REC(i->env, s2, l2)))

#define TABLE_HASH(index, side, લીટી) \
	XDL_HASHLONG((REC(index->env, side, લીટી))->ha, index->table_bits)

સ્થિર પૂર્ણાંક scanA(સંરચના histindex *index, પૂર્ણાંક line1, પૂર્ણાંક count1)
{
	unsigned પૂર્ણાંક ptr, tbl_idx;
	unsigned પૂર્ણાંક chain_len;
	સંરચના record **rec_chain, *rec;

	માટે (ptr = લીટી_END(1); (unsigned પૂર્ણાંક)line1 <= ptr; ptr--) {
		tbl_idx = TABLE_HASH(index, 1, ptr);
		rec_chain = index->records + tbl_idx;
		rec = *rec_chain;

		chain_len = 0;
		જ્યારે (rec) {
			જો (CMP(index, 1, rec->ptr, 1, ptr)) {
				/*
				 * ptr is identical to another element. Insert
				 * it onto the front of the existing element
				 * chain.
				 */
				NEXT_PTR(index, ptr) = rec->ptr;
				rec->ptr = ptr;
				/* cap rec->cnt at MAX_CNT */
				rec->cnt = (rec->cnt < MAX_CNT) ? rec->cnt + 1 : rec->cnt;
				લીટી_MAP(index, ptr) = rec;
				જાઓ continue_scan;
			}

			rec = rec->next;
			chain_len++;
		}

		જો (chain_len == index->max_chain_length)
			પાછા -1;

		/*
		 * This is the first time we have ever seen this particular
		 * element in the sequence. Construct a new chain for it.
		 */
		જો (!(rec = xdl_cha_alloc(&index->rcha)))
			પાછા -1;
		rec->ptr = ptr;
		rec->cnt = 1;
		rec->next = *rec_chain;
		*rec_chain = rec;
		લીટી_MAP(index, ptr) = rec;

continue_scan:
		; /* no op */
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક try_lcs(સંરચના histindex *index, સંરચના region *lcs, પૂર્ણાંક b_ptr,
	પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2)
{
	unsigned પૂર્ણાંક b_next = b_ptr + 1;
	સંરચના record *rec = index->records[TABLE_HASH(index, 2, b_ptr)];
	unsigned પૂર્ણાંક as, ae, bs, be, np, rc;
	પૂર્ણાંક should_break;

	માટે (; rec; rec = rec->next) {
		જો (rec->cnt > index->cnt) {
			જો (!index->has_common)
				index->has_common = CMP(index, 1, rec->ptr, 2, b_ptr);
			ચાલુ;
		}

		as = rec->ptr;
		જો (!CMP(index, 1, as, 2, b_ptr))
			ચાલુ;

		index->has_common = 1;
		માટે (;;) {
			should_break = 0;
			np = NEXT_PTR(index, as);
			bs = b_ptr;
			ae = as;
			be = bs;
			rc = rec->cnt;

			જ્યારે ((unsigned પૂર્ણાંક)line1 < as && (unsigned પૂર્ણાંક)line2 < bs
				&& CMP(index, 1, as - 1, 2, bs - 1)) {
				as--;
				bs--;
				જો (1 < rc)
					rc = XDL_MIN(rc, CNT(index, as));
			}
			જ્યારે (ae < (unsigned પૂર્ણાંક)લીટી_END(1) && be < (unsigned પૂર્ણાંક)લીટી_END(2)
				&& CMP(index, 1, ae + 1, 2, be + 1)) {
				ae++;
				be++;
				જો (1 < rc)
					rc = XDL_MIN(rc, CNT(index, ae));
			}

			જો (b_next <= be)
				b_next = be + 1;
			જો (lcs->end1 - lcs->begin1 < ae - as || rc < index->cnt) {
				lcs->begin1 = as;
				lcs->begin2 = bs;
				lcs->end1 = ae;
				lcs->end2 = be;
				index->cnt = rc;
			}

			જો (np == 0)
				તોડો;

			જ્યારે (np <= ae) {
				np = NEXT_PTR(index, np);
				જો (np == 0) {
					should_break = 1;
					તોડો;
				}
			}

			જો (should_break)
				તોડો;

			as = np;
		}
	}
	પાછા b_next;
}

સ્થિર પૂર્ણાંક fall_back_to_classic_diff(xpparam_t અચલ *xpp, xdfenv_t *env,
		પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2)
{
	xpparam_t xpparam;

	memset(&xpparam, 0, કદ(xpparam));
	xpparam.flags = xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	પાછા xdl_fall_back_diff(env, &xpparam,
				  line1, count1, line2, count2);
}

સ્થિર અંતઃરેખ શૂન્ય free_index(સંરચના histindex *index)
{
	xdl_free(index->records);
	xdl_free(index->લીટી_map);
	xdl_free(index->next_ptrs);
	xdl_cha_free(&index->rcha);
}

સ્થિર પૂર્ણાંક find_lcs(xpparam_t અચલ *xpp, xdfenv_t *env,
		    સંરચના region *lcs,
		    પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2)
{
	પૂર્ણાંક b_ptr;
	પૂર્ણાંક ret = -1;
	સંરચના histindex index;

	memset(&index, 0, કદ(index));

	index.env = env;
	index.xpp = xpp;

	index.records = ખાલી;
	index.લીટી_map = ખાલી;
	/* in case of early xdl_cha_free() */
	index.rcha.head = ખાલી;

	index.table_bits = xdl_hashbits(count1);
	index.records_size = 1 << index.table_bits;
	જો (!XDL_CALLOC_ARRAY(index.records, index.records_size))
		જાઓ cleanup;

	index.લીટી_map_size = count1;
	જો (!XDL_CALLOC_ARRAY(index.લીટી_map, index.લીટી_map_size))
		જાઓ cleanup;

	જો (!XDL_CALLOC_ARRAY(index.next_ptrs, index.લીટી_map_size))
		જાઓ cleanup;

	/* lines / 4 + 1 comes from xprepare.c:xdl_prepare_ctx() */
	જો (xdl_cha_init(&index.rcha, કદ(સંરચના record), count1 / 4 + 1) < 0)
		જાઓ cleanup;

	index.ptr_shift = line1;
	index.max_chain_length = 64;

	જો (scanA(&index, line1, count1))
		જાઓ cleanup;

	index.cnt = index.max_chain_length + 1;

	માટે (b_ptr = line2; b_ptr <= લીટી_END(2); )
		b_ptr = try_lcs(&index, lcs, b_ptr, line1, count1, line2, count2);

	જો (index.has_common && index.max_chain_length < index.cnt)
		ret = 1;
	નહીંતર
		ret = 0;

cleanup:
	free_index(&index);
	પાછા ret;
}

સ્થિર પૂર્ણાંક histogram_diff(xpparam_t અચલ *xpp, xdfenv_t *env,
	પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2)
{
	સંરચના region lcs;
	પૂર્ણાંક lcs_found;
	પૂર્ણાંક result;
redo:
	result = -1;

	જો (count1 <= 0 && count2 <= 0)
		પાછા 0;

	જો ((unsigned પૂર્ણાંક)લીટી_END(1) >= MAX_PTR)
		પાછા -1;

	જો (!count1) {
		જ્યારે(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		પાછા 0;
	} નહીંતર જો (!count2) {
		જ્યારે(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		પાછા 0;
	}

	memset(&lcs, 0, કદ(lcs));
	lcs_found = find_lcs(xpp, env, &lcs, line1, count1, line2, count2);
	જો (lcs_found < 0)
		જાઓ out;
	નહીંતર જો (lcs_found)
		result = fall_back_to_classic_diff(xpp, env, line1, count1, line2, count2);
	નહીંતર {
		જો (lcs.begin1 == 0 && lcs.begin2 == 0) {
			જ્યારે (count1--)
				env->xdf1.changed[line1++ - 1] = true;
			જ્યારે (count2--)
				env->xdf2.changed[line2++ - 1] = true;
			result = 0;
		} નહીંતર {
			result = histogram_diff(xpp, env,
						line1, lcs.begin1 - line1,
						line2, lcs.begin2 - line2);
			જો (result)
				જાઓ out;
			/*
			 * result = histogram_diff(xpp, env,
			 *            lcs.end1 + 1, LINE_END(1) - lcs.end1,
			 *            lcs.end2 + 1, LINE_END(2) - lcs.end2);
			 * but let's optimize tail recursion ourself:
			*/
			count1 = લીટી_END(1) - lcs.end1;
			line1 = lcs.end1 + 1;
			count2 = લીટી_END(2) - lcs.end2;
			line2 = lcs.end2 + 1;
			જાઓ redo;
		}
	}
out:
	પાછા result;
}

પૂર્ણાંક xdl_do_histogram_diff(xpparam_t અચલ *xpp, xdfenv_t *env)
{
	પાછા histogram_diff(xpp, env,
		env->xdf1.dstart + 1, env->xdf1.dend - env->xdf1.dstart + 1,
		env->xdf2.dstart + 1, env->xdf2.dend - env->xdf2.dstart + 1);
}
