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

#అన్తర్భావ "xinclude.h"

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

#నిర్వచించు NON_UNIQUE ULONG_MAX

/*
 * This is a hash mapping from line hash to line numbers in the first and
 * second file.
 */
నిర్మాణం hashmap {
	పూర్ణాంకం nr, alloc;
	నిర్మాణం నమోదు_2 {
		గుర్తులేని దీర్ఘ hash;
		/*
		 * 0 = unused entry, 1 = first line, 2 = second, etc.
		 * line2 is NON_UNIQUE if the line is not unique
		 * in either the first or the second file.
		 */
		గుర్తులేని దీర్ఘ line1, line2;
		/*
		 * "next" & "previous" are used for the longest common
		 * sequence;
		 * initially, "next" reflects only the order in file1.
		 */
		నిర్మాణం నమోదు_2 *తదుపరి, *మునుపటి;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		గుర్తులేని anchor : 1;
	} *entries, *మొదటి, *చివరి;
	/* were common records found? */
	గుర్తులేని దీర్ఘ has_matches;
	xdfenv_t *env;
	xpparam_t స్థిరరాశి *xpp;
};

స్థిర పూర్ణాంకం is_anchor(xpparam_t స్థిరరాశి *xpp, స్థిరరాశి అక్షరం *పంక్తి)
{
	size_t i;
	కొరకు (i = 0; i < xpp->anchors_nr; i++) {
		ఒకవేళ (!అక్షరశ్రేణి_పోలిక(పంక్తి, xpp->anchors[i], అక్షరశ్రేణి_పొడవు(xpp->anchors[i])))
			తిరిగి 1;
	}
	తిరిగి 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
స్థిర శూన్య చొప్పించు_రికార్డు(xpparam_t స్థిరరాశి *xpp, పూర్ణాంకం పంక్తి, నిర్మాణం hashmap *map,
			  పూర్ణాంకం pass)
{
	xrecord_t *రికార్డులు = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *రికార్డు = &రికార్డులు[పంక్తి - 1];
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
	పూర్ణాంకం సూచీ = (పూర్ణాంకం)((రికార్డు->ha << 1) % map->alloc);

	అయితే (map->entries[సూచీ].line1) {
		ఒకవేళ (map->entries[సూచీ].hash != రికార్డు->ha) {
			ఒకవేళ (++సూచీ >= map->alloc)
				సూచీ = 0;
			కొనసాగు;
		}
		ఒకవేళ (pass == 2)
			map->has_matches = 1;
		ఒకవేళ (pass == 1 || map->entries[సూచీ].line2)
			map->entries[సూచీ].line2 = NON_UNIQUE;
		లేకపోతే
			map->entries[సూచీ].line2 = పంక్తి;
		తిరిగి;
	}
	ఒకవేళ (pass == 2)
		తిరిగి;
	map->entries[సూచీ].line1 = పంక్తి;
	map->entries[సూచీ].hash = రికార్డు->ha;
	map->entries[సూచీ].anchor = is_anchor(xpp, map->env->xdf1.recs[పంక్తి - 1].ptr);
	ఒకవేళ (!map->మొదటి)
		map->మొదటి = map->entries + సూచీ;
	ఒకవేళ (map->చివరి) {
		map->చివరి->తదుపరి = map->entries + సూచీ;
		map->entries[సూచీ].మునుపటి = map->చివరి;
	}
	map->చివరి = map->entries + సూచీ;
	map->nr++;
}

/*
 * This function has to be called for each recursion into the inter-hunk
 * parts, as previously non-unique lines can become unique when being
 * restricted to a smaller part of the files.
 *
 * It is assumed that env has been prepared using xdl_prepare().
 */
స్థిర పూర్ణాంకం fill_hashmap(xpparam_t స్థిరరాశి *xpp, xdfenv_t *env,
		నిర్మాణం hashmap *ఫలితం,
		పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2)
{
	ఫలితం->xpp = xpp;
	ఫలితం->env = env;

	/* We know exactly how large we want the hash map */
	ఫలితం->alloc = count1 * 2;
	ఒకవేళ (!XDL_CALLOC_ARRAY(ఫలితం->entries, ఫలితం->alloc))
		తిరిగి -1;

	/* First, fill with entries from the first file */
	అయితే (count1--)
		చొప్పించు_రికార్డు(xpp, line1++, ఫలితం, 1);

	/* Then search for matches in the second file */
	అయితే (count2--)
		చొప్పించు_రికార్డు(xpp, line2++, ఫలితం, 2);

	తిరిగి 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
స్థిర పూర్ణాంకం ద్విమాన_శోధన(నిర్మాణం నమోదు_2 **sequence, పూర్ణాంకం longest,
		నిర్మాణం నమోదు_2 *నమోదు_2)
{
	పూర్ణాంకం ఎడమ_2 = -1, కుడి_2 = longest;

	అయితే (ఎడమ_2 + 1 < కుడి_2) {
		పూర్ణాంకం మధ్య_2 = ఎడమ_2 + (కుడి_2 - ఎడమ_2) / 2;
		/* by construction, no two entries can be equal */
		ఒకవేళ (sequence[మధ్య_2]->line2 > నమోదు_2->line2)
			కుడి_2 = మధ్య_2;
		లేకపోతే
			ఎడమ_2 = మధ్య_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	తిరిగి ఎడమ_2;
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
స్థిర పూర్ణాంకం కనుగొను_longest_common_sequence(నిర్మాణం hashmap *map, నిర్మాణం నమోదు_2 **res)
{
	నిర్మాణం నమోదు_2 **sequence;
	పూర్ణాంకం longest = 0, i;
	నిర్మాణం నమోదు_2 *నమోదు_2;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	పూర్ణాంకం anchor_i = -1;

	ఒకవేళ (!XDL_ALLOC_ARRAY(sequence, map->nr))
		తిరిగి -1;

	కొరకు (నమోదు_2 = map->మొదటి; నమోదు_2; నమోదు_2 = నమోదు_2->తదుపరి) {
		ఒకవేళ (!నమోదు_2->line2 || నమోదు_2->line2 == NON_UNIQUE)
			కొనసాగు;
		i = ద్విమాన_శోధన(sequence, longest, నమోదు_2);
		నమోదు_2->మునుపటి = i < 0 ? ఖాళీ : sequence[i];
		++i;
		ఒకవేళ (i <= anchor_i)
			కొనసాగు;
		sequence[i] = నమోదు_2;
		ఒకవేళ (నమోదు_2->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} లేకపోతే ఒకవేళ (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	ఒకవేళ (!longest) {
		*res = ఖాళీ;
		xdl_free(sequence);
		తిరిగి 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	నమోదు_2 = sequence[longest - 1];
	నమోదు_2->తదుపరి = ఖాళీ;
	అయితే (నమోదు_2->మునుపటి) {
		నమోదు_2->మునుపటి->తదుపరి = నమోదు_2;
		నమోదు_2 = నమోదు_2->మునుపటి;
	}
	*res = నమోదు_2;
	xdl_free(sequence);
	తిరిగి 0;
}

స్థిర పూర్ణాంకం సరిపోలిక(నిర్మాణం hashmap *map, పూర్ణాంకం line1, పూర్ణాంకం line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	తిరిగి record1->ha == record2->ha;
}

స్థిర పూర్ణాంకం patience_diff(xpparam_t స్థిరరాశి *xpp, xdfenv_t *env,
		పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2);

స్థిర పూర్ణాంకం walk_common_sequence(నిర్మాణం hashmap *map, నిర్మాణం నమోదు_2 *మొదటి,
		పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2)
{
	పూర్ణాంకం end1 = line1 + count1, end2 = line2 + count2;
	పూర్ణాంకం next1, next2;

	కొరకు (;;) {
		/* Try to grow the line ranges of common lines */
		ఒకవేళ (మొదటి) {
			next1 = మొదటి->line1;
			next2 = మొదటి->line2;
			అయితే (next1 > line1 && next2 > line2 &&
					సరిపోలిక(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} లేకపోతే {
			next1 = end1;
			next2 = end2;
		}
		అయితే (line1 < next1 && line2 < next2 &&
				సరిపోలిక(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		ఒకవేళ (next1 > line1 || next2 > line2) {
			ఒకవేళ (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				తిరిగి -1;
		}

		ఒకవేళ (!మొదటి)
			తిరిగి 0;

		అయితే (మొదటి->తదుపరి &&
				మొదటి->తదుపరి->line1 == మొదటి->line1 + 1 &&
				మొదటి->తదుపరి->line2 == మొదటి->line2 + 1)
			మొదటి = మొదటి->తదుపరి;

		line1 = మొదటి->line1 + 1;
		line2 = మొదటి->line2 + 1;

		మొదటి = మొదటి->తదుపరి;
	}
}

స్థిర పూర్ణాంకం fall_వెనుక_గమ్యం_classic_diff(నిర్మాణం hashmap *map,
		పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, పరిమాణం(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_ముసుగు;

	తిరిగి xdl_fall_వెనుక_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
స్థిర పూర్ణాంకం patience_diff(xpparam_t స్థిరరాశి *xpp, xdfenv_t *env,
		పూర్ణాంకం line1, పూర్ణాంకం count1, పూర్ణాంకం line2, పూర్ణాంకం count2)
{
	నిర్మాణం hashmap map;
	నిర్మాణం నమోదు_2 *మొదటి;
	పూర్ణాంకం ఫలితం = 0;

	/* trivial case: one side is empty */
	ఒకవేళ (!count1) {
		అయితే(count2--)
			env->xdf2.మారింది[line2++ - 1] = true;
		తిరిగి 0;
	} లేకపోతే ఒకవేళ (!count2) {
		అయితే(count1--)
			env->xdf1.మారింది[line1++ - 1] = true;
		తిరిగి 0;
	}

	memset(&map, 0, పరిమాణం(map));
	ఒకవేళ (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		తిరిగి -1;

	/* are there any matching lines at all? */
	ఒకవేళ (!map.has_matches) {
		అయితే(count1--)
			env->xdf1.మారింది[line1++ - 1] = true;
		అయితే(count2--)
			env->xdf2.మారింది[line2++ - 1] = true;
		xdl_free(map.entries);
		తిరిగి 0;
	}

	ఫలితం = కనుగొను_longest_common_sequence(&map, &మొదటి);
	ఒకవేళ (ఫలితం)
		వెళ్ళు బహిర్గతం;
	ఒకవేళ (మొదటి)
		ఫలితం = walk_common_sequence(&map, మొదటి,
			line1, count1, line2, count2);
	లేకపోతే
		ఫలితం = fall_వెనుక_గమ్యం_classic_diff(&map,
			line1, count1, line2, count2);
 బహిర్గతం:
	xdl_free(map.entries);
	తిరిగి ఫలితం;
}

పూర్ణాంకం xdl_do_patience_diff(xpparam_t స్థిరరాశి *xpp, xdfenv_t *env)
{
	తిరిగి patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
