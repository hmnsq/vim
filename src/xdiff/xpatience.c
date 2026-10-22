/*
 *  LibXDiff by Davide Libenzi ( File Differential Library )
 *  Copyright (C) 2003-2016 Davide Libenzi, Johannes E. Schindelin
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

#شامل "xinclude.h"

/*
 * The basic idea of patience diff is to find lines that are unique in
 * both files.  These are intuitively the ones that we want to see as
 * common lines.
 *
 * The maximal ordered sequence of such line pairs (where ordered means
 * that the order in the sequence agrees with the order of the lines in
 * both files) naturally defines an initial set of common lines.
 *
 * Now, the algorithm tries to extend the set of common lines by growing
 * the line ranges where the files have identical lines.
 *
 * Between those common lines, the patience diff algorithm is applied
 * recursively, until no unique line pairs can be found; these line ranges
 * are handled by the well-known Myers algorithm.
 */

#تعریف_کرو NON_UNIQUE ULONG_MAX

/*
 * This is a hash mapping from line hash to line numbers in the first and
 * second file.
 */
ساخت hashmap {
	صحیح nr, alloc;
	ساخت اندراج_2 {
		غیرعلامتی طویل hash;
		/*
		 * 0 = unused entry, 1 = first line, 2 = second, etc.
		 * line2 is NON_UNIQUE if the line is not unique
		 * in either the first or the second file.
		 */
		غیرعلامتی طویل line1, line2;
		/*
		 * "next" & "previous" are used for the longest common
		 * sequence;
		 * initially, "next" reflects only the order in file1.
		 */
		ساخت اندراج_2 *اگلا, *پچھلا_2;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		غیرعلامتی anchor : 1;
	} *entries, *پہلا, *آخری;
	/* were common records found? */
	غیرعلامتی طویل has_matches;
	xdfenv_t *env;
	xpparam_t ثابت *xpp;
};

ساکن صحیح is_anchor(xpparam_t ثابت *xpp, ثابت حرف *سطر)
{
	size_t i;
	برائے (i = 0; i < xpp->anchors_nr; i++) {
		اگر (!لڑی_موازنہ(سطر, xpp->anchors[i], لڑی_لمبائی(xpp->anchors[i])))
			واپس 1;
	}
	واپس 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
ساکن خالی داخل_کرو_ریکارڈ(xpparam_t ثابت *xpp, صحیح سطر, ساخت hashmap *map,
			  صحیح pass)
{
	xrecord_t *اندراجات = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *ریکارڈ = &اندراجات[سطر - 1];
	/*
	 * After xdl_prepare_env() (or more precisely, due to
	 * xdl_classify_record()), the "ha" member of the records (AKA lines)
	 * is _not_ the hash anymore, but a linearized version of it.  In
	 * other words, the "ha" member is guaranteed to start with 0 and
	 * the second record's ha can only be 0 or 1, etc.
	 *
	 * So we multiply ha by 2 in the hope that the hashing was
	 * "unique enough".
	 */
	صحیح اشاریہ = (صحیح)((ریکارڈ->ha << 1) % map->alloc);

	جب (map->entries[اشاریہ].line1) {
		اگر (map->entries[اشاریہ].hash != ریکارڈ->ha) {
			اگر (++اشاریہ >= map->alloc)
				اشاریہ = 0;
			جاری;
		}
		اگر (pass == 2)
			map->has_matches = 1;
		اگر (pass == 1 || map->entries[اشاریہ].line2)
			map->entries[اشاریہ].line2 = NON_UNIQUE;
		ورنہ
			map->entries[اشاریہ].line2 = سطر;
		واپس;
	}
	اگر (pass == 2)
		واپس;
	map->entries[اشاریہ].line1 = سطر;
	map->entries[اشاریہ].hash = ریکارڈ->ha;
	map->entries[اشاریہ].anchor = is_anchor(xpp, map->env->xdf1.recs[سطر - 1].ptr);
	اگر (!map->پہلا)
		map->پہلا = map->entries + اشاریہ;
	اگر (map->آخری) {
		map->آخری->اگلا = map->entries + اشاریہ;
		map->entries[اشاریہ].پچھلا_2 = map->آخری;
	}
	map->آخری = map->entries + اشاریہ;
	map->nr++;
}

/*
 * This function has to be called for each recursion into the inter-hunk
 * parts, as previously non-unique lines can become unique when being
 * restricted to a smaller part of the files.
 *
 * It is assumed that env has been prepared using xdl_prepare().
 */
ساکن صحیح fill_hashmap(xpparam_t ثابت *xpp, xdfenv_t *env,
		ساخت hashmap *نتیجہ,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	نتیجہ->xpp = xpp;
	نتیجہ->env = env;

	/* We know exactly how large we want the hash map */
	نتیجہ->alloc = count1 * 2;
	اگر (!XDL_CALLOC_ARRAY(نتیجہ->entries, نتیجہ->alloc))
		واپس -1;

	/* First, fill with entries from the first file */
	جب (count1--)
		داخل_کرو_ریکارڈ(xpp, line1++, نتیجہ, 1);

	/* Then search for matches in the second file */
	جب (count2--)
		داخل_کرو_ریکارڈ(xpp, line2++, نتیجہ, 2);

	واپس 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
ساکن صحیح ثنائی_تلاش(ساخت اندراج_2 **sequence, صحیح longest,
		ساخت اندراج_2 *اندراج_2)
{
	صحیح بایاں = -1, دایاں = longest;

	جب (بایاں + 1 < دایاں) {
		صحیح درمیان_2 = بایاں + (دایاں - بایاں) / 2;
		/* by construction, no two entries can be equal */
		اگر (sequence[درمیان_2]->line2 > اندراج_2->line2)
			دایاں = درمیان_2;
		ورنہ
			بایاں = درمیان_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	واپس بایاں;
}

/*
 * The idea is to start with the list of common unique lines sorted by
 * the order in file1.  For each of these pairs, the longest (partial)
 * sequence whose last element's line2 is smaller is determined.
 *
 * For efficiency, the sequences are kept in a list containing exactly one
 * item per sequence length: the sequence with the smallest last
 * element (in terms of line2).
 */
ساکن صحیح ڈھونڈو_longest_common_sequence(ساخت hashmap *map, ساخت اندراج_2 **res)
{
	ساخت اندراج_2 **sequence;
	صحیح longest = 0, i;
	ساخت اندراج_2 *اندراج_2;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	صحیح anchor_i = -1;

	اگر (!XDL_ALLOC_ARRAY(sequence, map->nr))
		واپس -1;

	برائے (اندراج_2 = map->پہلا; اندراج_2; اندراج_2 = اندراج_2->اگلا) {
		اگر (!اندراج_2->line2 || اندراج_2->line2 == NON_UNIQUE)
			جاری;
		i = ثنائی_تلاش(sequence, longest, اندراج_2);
		اندراج_2->پچھلا_2 = i < 0 ? تہی : sequence[i];
		++i;
		اگر (i <= anchor_i)
			جاری;
		sequence[i] = اندراج_2;
		اگر (اندراج_2->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} ورنہ اگر (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	اگر (!longest) {
		*res = تہی;
		xdl_free(sequence);
		واپس 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	اندراج_2 = sequence[longest - 1];
	اندراج_2->اگلا = تہی;
	جب (اندراج_2->پچھلا_2) {
		اندراج_2->پچھلا_2->اگلا = اندراج_2;
		اندراج_2 = اندراج_2->پچھلا_2;
	}
	*res = اندراج_2;
	xdl_free(sequence);
	واپس 0;
}

ساکن صحیح مطابقت(ساخت hashmap *map, صحیح line1, صحیح line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	واپس record1->ha == record2->ha;
}

ساکن صحیح patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2);

ساکن صحیح walk_common_sequence(ساخت hashmap *map, ساخت اندراج_2 *پہلا,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	صحیح end1 = line1 + count1, end2 = line2 + count2;
	صحیح next1, next2;

	برائے (;;) {
		/* Try to grow the line ranges of common lines */
		اگر (پہلا) {
			next1 = پہلا->line1;
			next2 = پہلا->line2;
			جب (next1 > line1 && next2 > line2 &&
					مطابقت(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} ورنہ {
			next1 = end1;
			next2 = end2;
		}
		جب (line1 < next1 && line2 < next2 &&
				مطابقت(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		اگر (next1 > line1 || next2 > line2) {
			اگر (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				واپس -1;
		}

		اگر (!پہلا)
			واپس 0;

		جب (پہلا->اگلا &&
				پہلا->اگلا->line1 == پہلا->line1 + 1 &&
				پہلا->اگلا->line2 == پہلا->line2 + 1)
			پہلا = پہلا->اگلا;

		line1 = پہلا->line1 + 1;
		line2 = پہلا->line2 + 1;

		پہلا = پہلا->اگلا;
	}
}

ساکن صحیح fall_عقب_مقصد_classic_diff(ساخت hashmap *map,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, حجم(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_نقاب;

	واپس xdl_fall_عقب_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
ساکن صحیح patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	ساخت hashmap map;
	ساخت اندراج_2 *پہلا;
	صحیح نتیجہ = 0;

	/* trivial case: one side is empty */
	اگر (!count1) {
		جب(count2--)
			env->xdf2.بدلا[line2++ - 1] = true;
		واپس 0;
	} ورنہ اگر (!count2) {
		جب(count1--)
			env->xdf1.بدلا[line1++ - 1] = true;
		واپس 0;
	}

	memset(&map, 0, حجم(map));
	اگر (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		واپس -1;

	/* are there any matching lines at all? */
	اگر (!map.has_matches) {
		جب(count1--)
			env->xdf1.بدلا[line1++ - 1] = true;
		جب(count2--)
			env->xdf2.بدلا[line2++ - 1] = true;
		xdl_free(map.entries);
		واپس 0;
	}

	نتیجہ = ڈھونڈو_longest_common_sequence(&map, &پہلا);
	اگر (نتیجہ)
		جاؤ برآمد;
	اگر (پہلا)
		نتیجہ = walk_common_sequence(&map, پہلا,
			line1, count1, line2, count2);
	ورنہ
		نتیجہ = fall_عقب_مقصد_classic_diff(&map,
			line1, count1, line2, count2);
 برآمد:
	xdl_free(map.entries);
	واپس نتیجہ;
}

صحیح xdl_do_patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env)
{
	واپس patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
