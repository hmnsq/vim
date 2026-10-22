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

#ಅನ್ತರ್ಭಾವ "xinclude.h"

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

#ವ್ಯಾಖ್ಯಾನಿಸು NON_UNIQUE ULONG_MAX

/*
 * This is a hash mapping from line hash to line numbers in the first and
 * second file.
 */
ರಚನೆ hashmap {
	ಪೂರ್ಣಾಂಕ nr, alloc;
	ರಚನೆ ನಮೂದು {
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ hash;
		/*
		 * 0 = unused entry, 1 = first line, 2 = second, etc.
		 * line2 is NON_UNIQUE if the line is not unique
		 * in either the first or the second file.
		 */
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ line1, line2;
		/*
		 * "next" & "previous" are used for the longest common
		 * sequence;
		 * initially, "next" reflects only the order in file1.
		 */
		ರಚನೆ ನಮೂದು *ಮುಂದಿನ, *ಹಿಂದಿನ;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		ಚಿಹ್ನೆರಹಿತ anchor : 1;
	} *entries, *ಮೊದಲ, *ಕೊನೆಯ;
	/* were common records found? */
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ has_matches;
	xdfenv_t *env;
	xpparam_t ಸ್ಥಿರಾಂಕ *xpp;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_anchor(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು)
{
	size_t i;
	ಗಾಗಿ (i = 0; i < xpp->anchors_nr; i++) {
		ಆದರೆ (!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಸಾಲು, xpp->anchors[i], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(xpp->anchors[i])))
			ಹಿಂತಿರುಗಿ 1;
	}
	ಹಿಂತಿರುಗಿ 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
ಸ್ಥಿರ ಶೂನ್ಯ ಸೇರಿಸು_ದಾಖಲೆ(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, ಪೂರ್ಣಾಂಕ ಸಾಲು, ರಚನೆ hashmap *map,
			  ಪೂರ್ಣಾಂಕ pass)
{
	xrecord_t *ದಾಖಲೆಗಳು = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *ದಾಖಲೆ = &ದಾಖಲೆಗಳು[ಸಾಲು - 1];
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
	ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = (ಪೂರ್ಣಾಂಕ)((ದಾಖಲೆ->ha << 1) % map->alloc);

	ಆಗ (map->entries[ಸೂಚ್ಯಂಕ].line1) {
		ಆದರೆ (map->entries[ಸೂಚ್ಯಂಕ].hash != ದಾಖಲೆ->ha) {
			ಆದರೆ (++ಸೂಚ್ಯಂಕ >= map->alloc)
				ಸೂಚ್ಯಂಕ = 0;
			ಮುಂದುವರಿ;
		}
		ಆದರೆ (pass == 2)
			map->has_matches = 1;
		ಆದರೆ (pass == 1 || map->entries[ಸೂಚ್ಯಂಕ].line2)
			map->entries[ಸೂಚ್ಯಂಕ].line2 = NON_UNIQUE;
		ಇಲ್ಲದಿದ್ದರೆ
			map->entries[ಸೂಚ್ಯಂಕ].line2 = ಸಾಲು;
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (pass == 2)
		ಹಿಂತಿರುಗಿ;
	map->entries[ಸೂಚ್ಯಂಕ].line1 = ಸಾಲು;
	map->entries[ಸೂಚ್ಯಂಕ].hash = ದಾಖಲೆ->ha;
	map->entries[ಸೂಚ್ಯಂಕ].anchor = is_anchor(xpp, map->env->xdf1.recs[ಸಾಲು - 1].ptr);
	ಆದರೆ (!map->ಮೊದಲ)
		map->ಮೊದಲ = map->entries + ಸೂಚ್ಯಂಕ;
	ಆದರೆ (map->ಕೊನೆಯ) {
		map->ಕೊನೆಯ->ಮುಂದಿನ = map->entries + ಸೂಚ್ಯಂಕ;
		map->entries[ಸೂಚ್ಯಂಕ].ಹಿಂದಿನ = map->ಕೊನೆಯ;
	}
	map->ಕೊನೆಯ = map->entries + ಸೂಚ್ಯಂಕ;
	map->nr++;
}

/*
 * This function has to be called for each recursion into the inter-hunk
 * parts, as previously non-unique lines can become unique when being
 * restricted to a smaller part of the files.
 *
 * It is assumed that env has been prepared using xdl_prepare().
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fill_hashmap(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, xdfenv_t *env,
		ರಚನೆ hashmap *ಫಲಿತಾಂಶ,
		ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2)
{
	ಫಲಿತಾಂಶ->xpp = xpp;
	ಫಲಿತಾಂಶ->env = env;

	/* We know exactly how large we want the hash map */
	ಫಲಿತಾಂಶ->alloc = count1 * 2;
	ಆದರೆ (!XDL_CALLOC_ARRAY(ಫಲಿತಾಂಶ->entries, ಫಲಿತಾಂಶ->alloc))
		ಹಿಂತಿರುಗಿ -1;

	/* First, fill with entries from the first file */
	ಆಗ (count1--)
		ಸೇರಿಸು_ದಾಖಲೆ(xpp, line1++, ಫಲಿತಾಂಶ, 1);

	/* Then search for matches in the second file */
	ಆಗ (count2--)
		ಸೇರಿಸು_ದಾಖಲೆ(xpp, line2++, ಫಲಿತಾಂಶ, 2);

	ಹಿಂತಿರುಗಿ 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ದ್ವಿಮಾನ_ಹುಡುಕು(ರಚನೆ ನಮೂದು **sequence, ಪೂರ್ಣಾಂಕ longest,
		ರಚನೆ ನಮೂದು *ನಮೂದು)
{
	ಪೂರ್ಣಾಂಕ ಎಡ_2 = -1, ಬಲ_2 = longest;

	ಆಗ (ಎಡ_2 + 1 < ಬಲ_2) {
		ಪೂರ್ಣಾಂಕ ಮಧ್ಯ_2 = ಎಡ_2 + (ಬಲ_2 - ಎಡ_2) / 2;
		/* by construction, no two entries can be equal */
		ಆದರೆ (sequence[ಮಧ್ಯ_2]->line2 > ನಮೂದು->line2)
			ಬಲ_2 = ಮಧ್ಯ_2;
		ಇಲ್ಲದಿದ್ದರೆ
			ಎಡ_2 = ಮಧ್ಯ_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	ಹಿಂತಿರುಗಿ ಎಡ_2;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_longest_common_sequence(ರಚನೆ hashmap *map, ರಚನೆ ನಮೂದು **res)
{
	ರಚನೆ ನಮೂದು **sequence;
	ಪೂರ್ಣಾಂಕ longest = 0, i;
	ರಚನೆ ನಮೂದು *ನಮೂದು;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	ಪೂರ್ಣಾಂಕ anchor_i = -1;

	ಆದರೆ (!XDL_ALLOC_ARRAY(sequence, map->nr))
		ಹಿಂತಿರುಗಿ -1;

	ಗಾಗಿ (ನಮೂದು = map->ಮೊದಲ; ನಮೂದು; ನಮೂದು = ನಮೂದು->ಮುಂದಿನ) {
		ಆದರೆ (!ನಮೂದು->line2 || ನಮೂದು->line2 == NON_UNIQUE)
			ಮುಂದುವರಿ;
		i = ದ್ವಿಮಾನ_ಹುಡುಕು(sequence, longest, ನಮೂದು);
		ನಮೂದು->ಹಿಂದಿನ = i < 0 ? ಖಾಲಿ : sequence[i];
		++i;
		ಆದರೆ (i <= anchor_i)
			ಮುಂದುವರಿ;
		sequence[i] = ನಮೂದು;
		ಆದರೆ (ನಮೂದು->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	ಆದರೆ (!longest) {
		*res = ಖಾಲಿ;
		xdl_free(sequence);
		ಹಿಂತಿರುಗಿ 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	ನಮೂದು = sequence[longest - 1];
	ನಮೂದು->ಮುಂದಿನ = ಖಾಲಿ;
	ಆಗ (ನಮೂದು->ಹಿಂದಿನ) {
		ನಮೂದು->ಹಿಂದಿನ->ಮುಂದಿನ = ನಮೂದು;
		ನಮೂದು = ನಮೂದು->ಹಿಂದಿನ;
	}
	*res = ನಮೂದು;
	xdl_free(sequence);
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ(ರಚನೆ hashmap *map, ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	ಹಿಂತಿರುಗಿ record1->ha == record2->ha;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ patience_diff(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, xdfenv_t *env,
		ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ walk_common_sequence(ರಚನೆ hashmap *map, ರಚನೆ ನಮೂದು *ಮೊದಲ,
		ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2)
{
	ಪೂರ್ಣಾಂಕ end1 = line1 + count1, end2 = line2 + count2;
	ಪೂರ್ಣಾಂಕ next1, next2;

	ಗಾಗಿ (;;) {
		/* Try to grow the line ranges of common lines */
		ಆದರೆ (ಮೊದಲ) {
			next1 = ಮೊದಲ->line1;
			next2 = ಮೊದಲ->line2;
			ಆಗ (next1 > line1 && next2 > line2 &&
					ಹೊಂದಿಕೆ(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
			next1 = end1;
			next2 = end2;
		}
		ಆಗ (line1 < next1 && line2 < next2 &&
				ಹೊಂದಿಕೆ(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		ಆದರೆ (next1 > line1 || next2 > line2) {
			ಆದರೆ (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				ಹಿಂತಿರುಗಿ -1;
		}

		ಆದರೆ (!ಮೊದಲ)
			ಹಿಂತಿರುಗಿ 0;

		ಆಗ (ಮೊದಲ->ಮುಂದಿನ &&
				ಮೊದಲ->ಮುಂದಿನ->line1 == ಮೊದಲ->line1 + 1 &&
				ಮೊದಲ->ಮುಂದಿನ->line2 == ಮೊದಲ->line2 + 1)
			ಮೊದಲ = ಮೊದಲ->ಮುಂದಿನ;

		line1 = ಮೊದಲ->line1 + 1;
		line2 = ಮೊದಲ->line2 + 1;

		ಮೊದಲ = ಮೊದಲ->ಮುಂದಿನ;
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fall_ಹಿಂಭಾಗ_ಗಮ್ಯ_classic_diff(ರಚನೆ hashmap *map,
		ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, ಗಾತ್ರ(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_ಮುಖವಾಡ;

	ಹಿಂತಿರುಗಿ xdl_fall_ಹಿಂಭಾಗ_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ patience_diff(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, xdfenv_t *env,
		ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ count1, ಪೂರ್ಣಾಂಕ line2, ಪೂರ್ಣಾಂಕ count2)
{
	ರಚನೆ hashmap map;
	ರಚನೆ ನಮೂದು *ಮೊದಲ;
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = 0;

	/* trivial case: one side is empty */
	ಆದರೆ (!count1) {
		ಆಗ(count2--)
			env->xdf2.ಬದಲಾಯಿತು[line2++ - 1] = true;
		ಹಿಂತಿರುಗಿ 0;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!count2) {
		ಆಗ(count1--)
			env->xdf1.ಬದಲಾಯಿತು[line1++ - 1] = true;
		ಹಿಂತಿರುಗಿ 0;
	}

	memset(&map, 0, ಗಾತ್ರ(map));
	ಆದರೆ (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		ಹಿಂತಿರುಗಿ -1;

	/* are there any matching lines at all? */
	ಆದರೆ (!map.has_matches) {
		ಆಗ(count1--)
			env->xdf1.ಬದಲಾಯಿತು[line1++ - 1] = true;
		ಆಗ(count2--)
			env->xdf2.ಬದಲಾಯಿತು[line2++ - 1] = true;
		xdl_free(map.entries);
		ಹಿಂತಿರುಗಿ 0;
	}

	ಫಲಿತಾಂಶ = ಕಂಡುಹಿಡಿ_longest_common_sequence(&map, &ಮೊದಲ);
	ಆದರೆ (ಫಲಿತಾಂಶ)
		ಹೋಗು ಹೊರಹೋಗು;
	ಆದರೆ (ಮೊದಲ)
		ಫಲಿತಾಂಶ = walk_common_sequence(&map, ಮೊದಲ,
			line1, count1, line2, count2);
	ಇಲ್ಲದಿದ್ದರೆ
		ಫಲಿತಾಂಶ = fall_ಹಿಂಭಾಗ_ಗಮ್ಯ_classic_diff(&map,
			line1, count1, line2, count2);
 ಹೊರಹೋಗು:
	xdl_free(map.entries);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪೂರ್ಣಾಂಕ xdl_do_patience_diff(xpparam_t ಸ್ಥಿರಾಂಕ *xpp, xdfenv_t *env)
{
	ಹಿಂತಿರುಗಿ patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
