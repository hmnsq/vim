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

#ضمن "xinclude.h"

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

#تعریف_کړه NON_UNIQUE ULONG_MAX

/*
 * This is a hash mapping from line hash to line numbers in the first and
 * second file.
 */
بنية hashmap {
	صحيح nr, alloc;
	بنية ثبت {
		بینښه اوږد hash;
		/*
		 * 0 = unused entry, 1 = first line, 2 = second, etc.
		 * line2 is NON_UNIQUE if the line is not unique
		 * in either the first or the second file.
		 */
		بینښه اوږد line1, line2;
		/*
		 * "next" & "previous" are used for the longest common
		 * sequence;
		 * initially, "next" reflects only the order in file1.
		 */
		بنية ثبت *بل, *پخوانی;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		بینښه anchor : 1;
	} *entries, *لومړی, *وروستی;
	/* were common records found? */
	بینښه اوږد has_matches;
	xdfenv_t *env;
	xpparam_t ثابت *xpp;
};

ساكن صحيح is_anchor(xpparam_t ثابت *xpp, ثابت حرف *کرښه)
{
	size_t i;
	لكل (i = 0; i < xpp->anchors_nr; i++) {
		إذا (!مزي_پرتله(کرښه, xpp->anchors[i], مزي_اوږدوالی(xpp->anchors[i])))
			أرجع 1;
	}
	أرجع 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
ساكن فراغ ورننباسه_ریکارډ(xpparam_t ثابت *xpp, صحيح کرښه, بنية hashmap *map,
			  صحيح pass)
{
	xrecord_t *ریکارډونه = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *ریکارډ = &ریکارډونه[کرښه - 1];
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
	صحيح شاخص = (صحيح)((ریکارډ->ha << 1) % map->alloc);

	طالما (map->entries[شاخص].line1) {
		إذا (map->entries[شاخص].hash != ریکارډ->ha) {
			إذا (++شاخص >= map->alloc)
				شاخص = 0;
			استمر;
		}
		إذا (pass == 2)
			map->has_matches = 1;
		إذا (pass == 1 || map->entries[شاخص].line2)
			map->entries[شاخص].line2 = NON_UNIQUE;
		وإلا
			map->entries[شاخص].line2 = کرښه;
		أرجع;
	}
	إذا (pass == 2)
		أرجع;
	map->entries[شاخص].line1 = کرښه;
	map->entries[شاخص].hash = ریکارډ->ha;
	map->entries[شاخص].anchor = is_anchor(xpp, map->env->xdf1.recs[کرښه - 1].ptr);
	إذا (!map->لومړی)
		map->لومړی = map->entries + شاخص;
	إذا (map->وروستی) {
		map->وروستی->بل = map->entries + شاخص;
		map->entries[شاخص].پخوانی = map->وروستی;
	}
	map->وروستی = map->entries + شاخص;
	map->nr++;
}

/*
 * This function has to be called for each recursion into the inter-hunk
 * parts, as previously non-unique lines can become unique when being
 * restricted to a smaller part of the files.
 *
 * It is assumed that env has been prepared using xdl_prepare().
 */
ساكن صحيح fill_hashmap(xpparam_t ثابت *xpp, xdfenv_t *env,
		بنية hashmap *پایله,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	پایله->xpp = xpp;
	پایله->env = env;

	/* We know exactly how large we want the hash map */
	پایله->alloc = count1 * 2;
	إذا (!XDL_CALLOC_ARRAY(پایله->entries, پایله->alloc))
		أرجع -1;

	/* First, fill with entries from the first file */
	طالما (count1--)
		ورننباسه_ریکارډ(xpp, line1++, پایله, 1);

	/* Then search for matches in the second file */
	طالما (count2--)
		ورننباسه_ریکارډ(xpp, line2++, پایله, 2);

	أرجع 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
ساكن صحيح دوګونې_پلټنه(بنية ثبت **sequence, صحيح longest,
		بنية ثبت *ثبت)
{
	صحيح کیڼ_2 = -1, ښی_2 = longest;

	طالما (کیڼ_2 + 1 < ښی_2) {
		صحيح منځ_2 = کیڼ_2 + (ښی_2 - کیڼ_2) / 2;
		/* by construction, no two entries can be equal */
		إذا (sequence[منځ_2]->line2 > ثبت->line2)
			ښی_2 = منځ_2;
		وإلا
			کیڼ_2 = منځ_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	أرجع کیڼ_2;
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
ساكن صحيح پیدا_کړه_longest_common_sequence(بنية hashmap *map, بنية ثبت **res)
{
	بنية ثبت **sequence;
	صحيح longest = 0, i;
	بنية ثبت *ثبت;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	صحيح anchor_i = -1;

	إذا (!XDL_ALLOC_ARRAY(sequence, map->nr))
		أرجع -1;

	لكل (ثبت = map->لومړی; ثبت; ثبت = ثبت->بل) {
		إذا (!ثبت->line2 || ثبت->line2 == NON_UNIQUE)
			استمر;
		i = دوګونې_پلټنه(sequence, longest, ثبت);
		ثبت->پخوانی = i < 0 ? عدم : sequence[i];
		++i;
		إذا (i <= anchor_i)
			استمر;
		sequence[i] = ثبت;
		إذا (ثبت->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} وإلا إذا (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	إذا (!longest) {
		*res = عدم;
		xdl_free(sequence);
		أرجع 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	ثبت = sequence[longest - 1];
	ثبت->بل = عدم;
	طالما (ثبت->پخوانی) {
		ثبت->پخوانی->بل = ثبت;
		ثبت = ثبت->پخوانی;
	}
	*res = ثبت;
	xdl_free(sequence);
	أرجع 0;
}

ساكن صحيح سمون(بنية hashmap *map, صحيح line1, صحيح line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	أرجع record1->ha == record2->ha;
}

ساكن صحيح patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2);

ساكن صحيح walk_common_sequence(بنية hashmap *map, بنية ثبت *لومړی,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	صحيح end1 = line1 + count1, end2 = line2 + count2;
	صحيح next1, next2;

	لكل (;;) {
		/* Try to grow the line ranges of common lines */
		إذا (لومړی) {
			next1 = لومړی->line1;
			next2 = لومړی->line2;
			طالما (next1 > line1 && next2 > line2 &&
					سمون(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} وإلا {
			next1 = end1;
			next2 = end2;
		}
		طالما (line1 < next1 && line2 < next2 &&
				سمون(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		إذا (next1 > line1 || next2 > line2) {
			إذا (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				أرجع -1;
		}

		إذا (!لومړی)
			أرجع 0;

		طالما (لومړی->بل &&
				لومړی->بل->line1 == لومړی->line1 + 1 &&
				لومړی->بل->line2 == لومړی->line2 + 1)
			لومړی = لومړی->بل;

		line1 = لومړی->line1 + 1;
		line2 = لومړی->line2 + 1;

		لومړی = لومړی->بل;
	}
}

ساكن صحيح fall_شا_منزل_classic_diff(بنية hashmap *map,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, حجم(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_ماسک;

	أرجع xdl_fall_شا_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
ساكن صحيح patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	بنية hashmap map;
	بنية ثبت *لومړی;
	صحيح پایله = 0;

	/* trivial case: one side is empty */
	إذا (!count1) {
		طالما(count2--)
			env->xdf2.بدل_شوی[line2++ - 1] = true;
		أرجع 0;
	} وإلا إذا (!count2) {
		طالما(count1--)
			env->xdf1.بدل_شوی[line1++ - 1] = true;
		أرجع 0;
	}

	memset(&map, 0, حجم(map));
	إذا (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		أرجع -1;

	/* are there any matching lines at all? */
	إذا (!map.has_matches) {
		طالما(count1--)
			env->xdf1.بدل_شوی[line1++ - 1] = true;
		طالما(count2--)
			env->xdf2.بدل_شوی[line2++ - 1] = true;
		xdl_free(map.entries);
		أرجع 0;
	}

	پایله = پیدا_کړه_longest_common_sequence(&map, &لومړی);
	إذا (پایله)
		اذهب بهر;
	إذا (لومړی)
		پایله = walk_common_sequence(&map, لومړی,
			line1, count1, line2, count2);
	وإلا
		پایله = fall_شا_منزل_classic_diff(&map,
			line1, count1, line2, count2);
 بهر:
	xdl_free(map.entries);
	أرجع پایله;
}

صحيح xdl_do_patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env)
{
	أرجع patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
