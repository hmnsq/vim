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

#وصف_ڪريو NON_UNIQUE ULONG_MAX

/*
 * This is a hash mapping from line hash to line numbers in the first and
 * second file.
 */
بنية hashmap {
	صحيح nr, alloc;
	بنية داخلا {
		بنانشان ڊگهو hash;
		/*
		 * 0 = unused entry, 1 = first line, 2 = second, etc.
		 * line2 is NON_UNIQUE if the line is not unique
		 * in either the first or the second file.
		 */
		بنانشان ڊگهو line1, line2;
		/*
		 * "next" & "previous" are used for the longest common
		 * sequence;
		 * initially, "next" reflects only the order in file1.
		 */
		بنية داخلا *ايندڙ, *اڳيون;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		بنانشان anchor : 1;
	} *entries, *پهريون, *آخري;
	/* were common records found? */
	بنانشان ڊگهو has_matches;
	xdfenv_t *env;
	xpparam_t ثابت *xpp;
};

ساكن صحيح is_anchor(xpparam_t ثابت *xpp, ثابت حرف *سٽ)
{
	size_t i;
	لكل (i = 0; i < xpp->anchors_nr; i++) {
		إذا (!لڙي_ڀيٽ(سٽ, xpp->anchors[i], لڙي_ڊيگهه(xpp->anchors[i])))
			أرجع 1;
	}
	أرجع 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
ساكن فراغ وجهو_رڪارڊ(xpparam_t ثابت *xpp, صحيح سٽ, بنية hashmap *map,
			  صحيح pass)
{
	xrecord_t *رڪارڊون = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *رڪارڊ = &رڪارڊون[سٽ - 1];
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
	صحيح اشاريو = (صحيح)((رڪارڊ->ha << 1) % map->alloc);

	طالما (map->entries[اشاريو].line1) {
		إذا (map->entries[اشاريو].hash != رڪارڊ->ha) {
			إذا (++اشاريو >= map->alloc)
				اشاريو = 0;
			استمر;
		}
		إذا (pass == 2)
			map->has_matches = 1;
		إذا (pass == 1 || map->entries[اشاريو].line2)
			map->entries[اشاريو].line2 = NON_UNIQUE;
		وإلا
			map->entries[اشاريو].line2 = سٽ;
		أرجع;
	}
	إذا (pass == 2)
		أرجع;
	map->entries[اشاريو].line1 = سٽ;
	map->entries[اشاريو].hash = رڪارڊ->ha;
	map->entries[اشاريو].anchor = is_anchor(xpp, map->env->xdf1.recs[سٽ - 1].ptr);
	إذا (!map->پهريون)
		map->پهريون = map->entries + اشاريو;
	إذا (map->آخري) {
		map->آخري->ايندڙ = map->entries + اشاريو;
		map->entries[اشاريو].اڳيون = map->آخري;
	}
	map->آخري = map->entries + اشاريو;
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
		بنية hashmap *نتيجو,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	نتيجو->xpp = xpp;
	نتيجو->env = env;

	/* We know exactly how large we want the hash map */
	نتيجو->alloc = count1 * 2;
	إذا (!XDL_CALLOC_ARRAY(نتيجو->entries, نتيجو->alloc))
		أرجع -1;

	/* First, fill with entries from the first file */
	طالما (count1--)
		وجهو_رڪارڊ(xpp, line1++, نتيجو, 1);

	/* Then search for matches in the second file */
	طالما (count2--)
		وجهو_رڪارڊ(xpp, line2++, نتيجو, 2);

	أرجع 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
ساكن صحيح ٻٽي_ڳولا(بنية داخلا **sequence, صحيح longest,
		بنية داخلا *داخلا)
{
	صحيح کاٻو = -1, ساڄو = longest;

	طالما (کاٻو + 1 < ساڄو) {
		صحيح وچ_2 = کاٻو + (ساڄو - کاٻو) / 2;
		/* by construction, no two entries can be equal */
		إذا (sequence[وچ_2]->line2 > داخلا->line2)
			ساڄو = وچ_2;
		وإلا
			کاٻو = وچ_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	أرجع کاٻو;
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
ساكن صحيح ڳوليو_longest_common_sequence(بنية hashmap *map, بنية داخلا **res)
{
	بنية داخلا **sequence;
	صحيح longest = 0, i;
	بنية داخلا *داخلا;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	صحيح anchor_i = -1;

	إذا (!XDL_ALLOC_ARRAY(sequence, map->nr))
		أرجع -1;

	لكل (داخلا = map->پهريون; داخلا; داخلا = داخلا->ايندڙ) {
		إذا (!داخلا->line2 || داخلا->line2 == NON_UNIQUE)
			استمر;
		i = ٻٽي_ڳولا(sequence, longest, داخلا);
		داخلا->اڳيون = i < 0 ? عدم : sequence[i];
		++i;
		إذا (i <= anchor_i)
			استمر;
		sequence[i] = داخلا;
		إذا (داخلا->anchor) {
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
	داخلا = sequence[longest - 1];
	داخلا->ايندڙ = عدم;
	طالما (داخلا->اڳيون) {
		داخلا->اڳيون->ايندڙ = داخلا;
		داخلا = داخلا->اڳيون;
	}
	*res = داخلا;
	xdl_free(sequence);
	أرجع 0;
}

ساكن صحيح ميلاپ(بنية hashmap *map, صحيح line1, صحيح line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	أرجع record1->ha == record2->ha;
}

ساكن صحيح patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2);

ساكن صحيح walk_common_sequence(بنية hashmap *map, بنية داخلا *پهريون,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	صحيح end1 = line1 + count1, end2 = line2 + count2;
	صحيح next1, next2;

	لكل (;;) {
		/* Try to grow the line ranges of common lines */
		إذا (پهريون) {
			next1 = پهريون->line1;
			next2 = پهريون->line2;
			طالما (next1 > line1 && next2 > line2 &&
					ميلاپ(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} وإلا {
			next1 = end1;
			next2 = end2;
		}
		طالما (line1 < next1 && line2 < next2 &&
				ميلاپ(map, line1, line2)) {
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

		إذا (!پهريون)
			أرجع 0;

		طالما (پهريون->ايندڙ &&
				پهريون->ايندڙ->line1 == پهريون->line1 + 1 &&
				پهريون->ايندڙ->line2 == پهريون->line2 + 1)
			پهريون = پهريون->ايندڙ;

		line1 = پهريون->line1 + 1;
		line2 = پهريون->line2 + 1;

		پهريون = پهريون->ايندڙ;
	}
}

ساكن صحيح fall_پٺ_منزل_classic_diff(بنية hashmap *map,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, حجم(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_نقاب;

	أرجع xdl_fall_پٺ_diff(map->env, &xpp,
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
	بنية داخلا *پهريون;
	صحيح نتيجو = 0;

	/* trivial case: one side is empty */
	إذا (!count1) {
		طالما(count2--)
			env->xdf2.بدليو[line2++ - 1] = true;
		أرجع 0;
	} وإلا إذا (!count2) {
		طالما(count1--)
			env->xdf1.بدليو[line1++ - 1] = true;
		أرجع 0;
	}

	memset(&map, 0, حجم(map));
	إذا (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		أرجع -1;

	/* are there any matching lines at all? */
	إذا (!map.has_matches) {
		طالما(count1--)
			env->xdf1.بدليو[line1++ - 1] = true;
		طالما(count2--)
			env->xdf2.بدليو[line2++ - 1] = true;
		xdl_free(map.entries);
		أرجع 0;
	}

	نتيجو = ڳوليو_longest_common_sequence(&map, &پهريون);
	إذا (نتيجو)
		اذهب ٻاهريون;
	إذا (پهريون)
		نتيجو = walk_common_sequence(&map, پهريون,
			line1, count1, line2, count2);
	وإلا
		نتيجو = fall_پٺ_منزل_classic_diff(&map,
			line1, count1, line2, count2);
 ٻاهريون:
	xdl_free(map.entries);
	أرجع نتيجو;
}

صحيح xdl_do_patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env)
{
	أرجع patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
