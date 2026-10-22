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

#define NON_UNIQUE ULONG_MAX

/*
 * This is a hash mapping from line hash to line numbers in the first and
 * second file.
 */
ಸಂರಚನಾ hashmap {
	ಪೂರ್ಣಾಙ್ಕ nr, alloc;
	ಸಂರಚನಾ entry {
		unsigned long hash;
		/*
		 * 0 = unused entry, 1 = first line, 2 = second, etc.
		 * line2 is NON_UNIQUE if the line is not unique
		 * in either the first or the second file.
		 */
		unsigned long line1, line2;
		/*
		 * "next" & "previous" are used for the longest common
		 * sequence;
		 * initially, "next" reflects only the order in file1.
		 */
		ಸಂರಚನಾ entry *next, *previous;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		unsigned anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	unsigned long has_matches;
	xdfenv_t *env;
	xpparam_t ಧ್ರುವ *xpp;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_anchor(xpparam_t ಧ್ರುವ *xpp, ಧ್ರುವ ವರ್ಣ *ಸಾಲು)
{
	size_t i;
	ಪ್ರತಿ (i = 0; i < xpp->anchors_nr; i++) {
		ಯದಿ (!strncmp(ಸಾಲು, xpp->anchors[i], strlen(xpp->anchors[i])))
			ಪ್ರತ್ಯಾಗಮ 1;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
ಸ್ಥಿರ ಶೂನ್ಯ ಸೇರಿಸು_record(xpparam_t ಧ್ರುವ *xpp, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಸಂರಚನಾ hashmap *map,
			  ಪೂರ್ಣಾಙ್ಕ pass)
{
	xrecord_t *records = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *record = &records[ಸಾಲು - 1];
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
	ಪೂರ್ಣಾಙ್ಕ index = (ಪೂರ್ಣಾಙ್ಕ)((record->ha << 1) % map->alloc);

	ಯಾವತ್ (map->entries[index].line1) {
		ಯದಿ (map->entries[index].hash != record->ha) {
			ಯದಿ (++index >= map->alloc)
				index = 0;
			ಅನುವರ್ತ;
		}
		ಯದಿ (pass == 2)
			map->has_matches = 1;
		ಯದಿ (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		ಅನ್ಯಥಾ
			map->entries[index].line2 = ಸಾಲು;
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (pass == 2)
		ಪ್ರತ್ಯಾಗಮ;
	map->entries[index].line1 = ಸಾಲು;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[ಸಾಲು - 1].ptr);
	ಯದಿ (!map->first)
		map->first = map->entries + index;
	ಯದಿ (map->last) {
		map->last->next = map->entries + index;
		map->entries[index].previous = map->last;
	}
	map->last = map->entries + index;
	map->nr++;
}

/*
 * This function has to be called for each recursion into the inter-hunk
 * parts, as previously non-unique lines can become unique when being
 * restricted to a smaller part of the files.
 *
 * It is assumed that env has been prepared using xdl_prepare().
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fill_hashmap(xpparam_t ಧ್ರುವ *xpp, xdfenv_t *env,
		ಸಂರಚನಾ hashmap *result,
		ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	ಯದಿ (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		ಪ್ರತ್ಯಾಗಮ -1;

	/* First, fill with entries from the first file */
	ಯಾವತ್ (count1--)
		ಸೇರಿಸು_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	ಯಾವತ್ (count2--)
		ಸೇರಿಸು_record(xpp, line2++, result, 2);

	ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ದ್ವಿಮಾನ_ಹುಡುಕು(ಸಂರಚನಾ entry **sequence, ಪೂರ್ಣಾಙ್ಕ longest,
		ಸಂರಚನಾ entry *entry)
{
	ಪೂರ್ಣಾಙ್ಕ ಎಡ = -1, ಬಲ = longest;

	ಯಾವತ್ (ಎಡ + 1 < ಬಲ) {
		ಪೂರ್ಣಾಙ್ಕ ಮಧ್ಯ_2 = ಎಡ + (ಬಲ - ಎಡ) / 2;
		/* by construction, no two entries can be equal */
		ಯದಿ (sequence[ಮಧ್ಯ_2]->line2 > entry->line2)
			ಬಲ = ಮಧ್ಯ_2;
		ಅನ್ಯಥಾ
			ಎಡ = ಮಧ್ಯ_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	ಪ್ರತ್ಯಾಗಮ ಎಡ;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ find_longest_common_sequence(ಸಂರಚನಾ hashmap *map, ಸಂರಚನಾ entry **res)
{
	ಸಂರಚನಾ entry **sequence;
	ಪೂರ್ಣಾಙ್ಕ longest = 0, i;
	ಸಂರಚನಾ entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	ಪೂರ್ಣಾಙ್ಕ anchor_i = -1;

	ಯದಿ (!XDL_ALLOC_ARRAY(sequence, map->nr))
		ಪ್ರತ್ಯಾಗಮ -1;

	ಪ್ರತಿ (entry = map->first; entry; entry = entry->next) {
		ಯದಿ (!entry->line2 || entry->line2 == NON_UNIQUE)
			ಅನುವರ್ತ;
		i = ದ್ವಿಮಾನ_ಹುಡುಕು(sequence, longest, entry);
		entry->previous = i < 0 ? ರಿಕ್ತ : sequence[i];
		++i;
		ಯದಿ (i <= anchor_i)
			ಅನುವರ್ತ;
		sequence[i] = entry;
		ಯದಿ (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} ಅನ್ಯಥಾ ಯದಿ (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	ಯದಿ (!longest) {
		*res = ರಿಕ್ತ;
		xdl_free(sequence);
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = ರಿಕ್ತ;
	ಯಾವತ್ (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ match(ಸಂರಚನಾ hashmap *map, ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	ಪ್ರತ್ಯಾಗಮ record1->ha == record2->ha;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ patience_diff(xpparam_t ಧ್ರುವ *xpp, xdfenv_t *env,
		ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ walk_common_sequence(ಸಂರಚನಾ hashmap *map, ಸಂರಚನಾ entry *first,
		ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2)
{
	ಪೂರ್ಣಾಙ್ಕ end1 = line1 + count1, end2 = line2 + count2;
	ಪೂರ್ಣಾಙ್ಕ next1, next2;

	ಪ್ರತಿ (;;) {
		/* Try to grow the line ranges of common lines */
		ಯದಿ (first) {
			next1 = first->line1;
			next2 = first->line2;
			ಯಾವತ್ (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} ಅನ್ಯಥಾ {
			next1 = end1;
			next2 = end2;
		}
		ಯಾವತ್ (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		ಯದಿ (next1 > line1 || next2 > line2) {
			ಯದಿ (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				ಪ್ರತ್ಯಾಗಮ -1;
		}

		ಯದಿ (!first)
			ಪ್ರತ್ಯಾಗಮ 0;

		ಯಾವತ್ (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fall_back_to_classic_diff(ಸಂರಚನಾ hashmap *map,
		ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, ಪರಿಮಾಣ(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ಪ್ರತ್ಯಾಗಮ xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ patience_diff(xpparam_t ಧ್ರುವ *xpp, xdfenv_t *env,
		ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ count1, ಪೂರ್ಣಾಙ್ಕ line2, ಪೂರ್ಣಾಙ್ಕ count2)
{
	ಸಂರಚನಾ hashmap map;
	ಸಂರಚನಾ entry *first;
	ಪೂರ್ಣಾಙ್ಕ result = 0;

	/* trivial case: one side is empty */
	ಯದಿ (!count1) {
		ಯಾವತ್(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ಪ್ರತ್ಯಾಗಮ 0;
	} ಅನ್ಯಥಾ ಯದಿ (!count2) {
		ಯಾವತ್(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	memset(&map, 0, ಪರಿಮಾಣ(map));
	ಯದಿ (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		ಪ್ರತ್ಯಾಗಮ -1;

	/* are there any matching lines at all? */
	ಯದಿ (!map.has_matches) {
		ಯಾವತ್(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ಯಾವತ್(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	result = find_longest_common_sequence(&map, &first);
	ಯದಿ (result)
		ಗಚ್ಛ out;
	ಯದಿ (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	ಅನ್ಯಥಾ
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	ಪ್ರತ್ಯಾಗಮ result;
}

ಪೂರ್ಣಾಙ್ಕ xdl_do_patience_diff(xpparam_t ಧ್ರುವ *xpp, xdfenv_t *env)
{
	ಪ್ರತ್ಯಾಗಮ patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
