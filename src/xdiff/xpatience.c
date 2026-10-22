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

#include "xinclude.h"

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
sa_nracanaa hashmap {
	puur_naa_nka nr, alloc;
	sa_nracanaa entry {
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
		sa_nracanaa entry *next, *previous;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		unsigned anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	unsigned long has_matches;
	xdfenv_t *env;
	xpparam_t const *xpp;
};

static puur_naa_nka is_anchor(xpparam_t const *xpp, const var_na *line)
{
	size_t i;
	for (i = 0; i < xpp->anchors_nr; i++) {
		ᯀᯮᯝ (!strncmp(line, xpp->anchors[i], strlen(xpp->anchors[i])))
			ᯀᯮᯞ 1;
	}
	ᯀᯮᯞ 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
static void insert_record(xpparam_t const *xpp, puur_naa_nka line, sa_nracanaa hashmap *map,
			  puur_naa_nka pass)
{
	xrecord_t *records = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *record = &records[line - 1];
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
	puur_naa_nka index = (puur_naa_nka)((record->ha << 1) % map->alloc);

	ᯀᯬᯝ (map->entries[index].line1) {
		ᯀᯮᯝ (map->entries[index].hash != record->ha) {
			ᯀᯮᯝ (++index >= map->alloc)
				index = 0;
			continue;
		}
		ᯀᯮᯝ (pass == 2)
			map->has_matches = 1;
		ᯀᯮᯝ (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		ᯀᯮᯝᯂᯪ
			map->entries[index].line2 = line;
		ᯀᯮᯞ;
	}
	ᯀᯮᯝ (pass == 2)
		ᯀᯮᯞ;
	map->entries[index].line1 = line;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[line - 1].ptr);
	ᯀᯮᯝ (!map->first)
		map->first = map->entries + index;
	ᯀᯮᯝ (map->last) {
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
static puur_naa_nka fill_hashmap(xpparam_t const *xpp, xdfenv_t *env,
		sa_nracanaa hashmap *result,
		puur_naa_nka line1, puur_naa_nka count1, puur_naa_nka line2, puur_naa_nka count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	ᯀᯮᯝ (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		ᯀᯮᯞ -1;

	/* First, fill with entries from the first file */
	ᯀᯬᯝ (count1--)
		insert_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	ᯀᯬᯝ (count2--)
		insert_record(xpp, line2++, result, 2);

	ᯀᯮᯞ 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
static puur_naa_nka binary_search(sa_nracanaa entry **sequence, puur_naa_nka longest,
		sa_nracanaa entry *entry)
{
	puur_naa_nka left = -1, right = longest;

	ᯀᯬᯝ (left + 1 < right) {
		puur_naa_nka middle = left + (right - left) / 2;
		/* by construction, no two entries can be equal */
		ᯀᯮᯝ (sequence[middle]->line2 > entry->line2)
			right = middle;
		ᯀᯮᯝᯂᯪ
			left = middle;
	}
	/* return the index in "sequence", _not_ the sequence length */
	ᯀᯮᯞ left;
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
static puur_naa_nka find_longest_common_sequence(sa_nracanaa hashmap *map, sa_nracanaa entry **res)
{
	sa_nracanaa entry **sequence;
	puur_naa_nka longest = 0, i;
	sa_nracanaa entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	puur_naa_nka anchor_i = -1;

	ᯀᯮᯝ (!XDL_ALLOC_ARRAY(sequence, map->nr))
		ᯀᯮᯞ -1;

	for (entry = map->first; entry; entry = entry->next) {
		ᯀᯮᯝ (!entry->line2 || entry->line2 == NON_UNIQUE)
			continue;
		i = binary_search(sequence, longest, entry);
		entry->previous = i < 0 ? NULL : sequence[i];
		++i;
		ᯀᯮᯝ (i <= anchor_i)
			continue;
		sequence[i] = entry;
		ᯀᯮᯝ (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} ᯀᯮᯝᯂᯪ ᯀᯮᯝ (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	ᯀᯮᯝ (!longest) {
		*res = NULL;
		xdl_free(sequence);
		ᯀᯮᯞ 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = NULL;
	ᯀᯬᯝ (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	ᯀᯮᯞ 0;
}

static puur_naa_nka match(sa_nracanaa hashmap *map, puur_naa_nka line1, puur_naa_nka line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	ᯀᯮᯞ record1->ha == record2->ha;
}

static puur_naa_nka patience_diff(xpparam_t const *xpp, xdfenv_t *env,
		puur_naa_nka line1, puur_naa_nka count1, puur_naa_nka line2, puur_naa_nka count2);

static puur_naa_nka walk_common_sequence(sa_nracanaa hashmap *map, sa_nracanaa entry *first,
		puur_naa_nka line1, puur_naa_nka count1, puur_naa_nka line2, puur_naa_nka count2)
{
	puur_naa_nka end1 = line1 + count1, end2 = line2 + count2;
	puur_naa_nka next1, next2;

	for (;;) {
		/* Try to grow the line ranges of common lines */
		ᯀᯮᯝ (first) {
			next1 = first->line1;
			next2 = first->line2;
			ᯀᯬᯝ (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} ᯀᯮᯝᯂᯪ {
			next1 = end1;
			next2 = end2;
		}
		ᯀᯬᯝ (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		ᯀᯮᯝ (next1 > line1 || next2 > line2) {
			ᯀᯮᯝ (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				ᯀᯮᯞ -1;
		}

		ᯀᯮᯝ (!first)
			ᯀᯮᯞ 0;

		ᯀᯬᯝ (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

static puur_naa_nka fall_back_to_classic_diff(sa_nracanaa hashmap *map,
		puur_naa_nka line1, puur_naa_nka count1, puur_naa_nka line2, puur_naa_nka count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, parimaa_na(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ᯀᯮᯞ xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
static puur_naa_nka patience_diff(xpparam_t const *xpp, xdfenv_t *env,
		puur_naa_nka line1, puur_naa_nka count1, puur_naa_nka line2, puur_naa_nka count2)
{
	sa_nracanaa hashmap map;
	sa_nracanaa entry *first;
	puur_naa_nka result = 0;

	/* trivial case: one side is empty */
	ᯀᯮᯝ (!count1) {
		ᯀᯬᯝ(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ᯀᯮᯞ 0;
	} ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!count2) {
		ᯀᯬᯝ(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ᯀᯮᯞ 0;
	}

	memset(&map, 0, parimaa_na(map));
	ᯀᯮᯝ (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		ᯀᯮᯞ -1;

	/* are there any matching lines at all? */
	ᯀᯮᯝ (!map.has_matches) {
		ᯀᯬᯝ(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ᯀᯬᯝ(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		ᯀᯮᯞ 0;
	}

	result = find_longest_common_sequence(&map, &first);
	ᯀᯮᯝ (result)
		goto out;
	ᯀᯮᯝ (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	ᯀᯮᯝᯂᯪ
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	ᯀᯮᯞ result;
}

puur_naa_nka xdl_do_patience_diff(xpparam_t const *xpp, xdfenv_t *env)
{
	ᯀᯮᯞ patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
