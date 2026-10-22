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
struct hashmap {
	целое nr, alloc;
	struct entry {
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
		struct entry *next, *previous;

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

static целое is_anchor(xpparam_t const *xpp, const char *line)
{
	size_t i;
	для (i = 0; i < xpp->anchors_nr; i++) {
		если (!strncmp(line, xpp->anchors[i], strlen(xpp->anchors[i])))
			вернуть 1;
	}
	вернуть 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
static void insert_record(xpparam_t const *xpp, целое line, struct hashmap *map,
			  целое pass)
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
	целое index = (целое)((record->ha << 1) % map->alloc);

	пока (map->entries[index].line1) {
		если (map->entries[index].hash != record->ha) {
			если (++index >= map->alloc)
				index = 0;
			continue;
		}
		если (pass == 2)
			map->has_matches = 1;
		если (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		иначе
			map->entries[index].line2 = line;
		вернуть;
	}
	если (pass == 2)
		вернуть;
	map->entries[index].line1 = line;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[line - 1].ptr);
	если (!map->first)
		map->first = map->entries + index;
	если (map->last) {
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
static целое fill_hashmap(xpparam_t const *xpp, xdfenv_t *env,
		struct hashmap *result,
		целое line1, целое count1, целое line2, целое count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	если (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		вернуть -1;

	/* First, fill with entries from the first file */
	пока (count1--)
		insert_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	пока (count2--)
		insert_record(xpp, line2++, result, 2);

	вернуть 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
static целое binary_search(struct entry **sequence, целое longest,
		struct entry *entry)
{
	целое left = -1, right = longest;

	пока (left + 1 < right) {
		целое middle = left + (right - left) / 2;
		/* by construction, no two entries can be equal */
		если (sequence[middle]->line2 > entry->line2)
			right = middle;
		иначе
			left = middle;
	}
	/* return the index in "sequence", _not_ the sequence length */
	вернуть left;
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
static целое find_longest_common_sequence(struct hashmap *map, struct entry **res)
{
	struct entry **sequence;
	целое longest = 0, i;
	struct entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	целое anchor_i = -1;

	если (!XDL_ALLOC_ARRAY(sequence, map->nr))
		вернуть -1;

	для (entry = map->first; entry; entry = entry->next) {
		если (!entry->line2 || entry->line2 == NON_UNIQUE)
			continue;
		i = binary_search(sequence, longest, entry);
		entry->previous = i < 0 ? NULL : sequence[i];
		++i;
		если (i <= anchor_i)
			continue;
		sequence[i] = entry;
		если (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} иначе если (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	если (!longest) {
		*res = NULL;
		xdl_free(sequence);
		вернуть 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = NULL;
	пока (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	вернуть 0;
}

static целое match(struct hashmap *map, целое line1, целое line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	вернуть record1->ha == record2->ha;
}

static целое patience_diff(xpparam_t const *xpp, xdfenv_t *env,
		целое line1, целое count1, целое line2, целое count2);

static целое walk_common_sequence(struct hashmap *map, struct entry *first,
		целое line1, целое count1, целое line2, целое count2)
{
	целое end1 = line1 + count1, end2 = line2 + count2;
	целое next1, next2;

	для (;;) {
		/* Try to grow the line ranges of common lines */
		если (first) {
			next1 = first->line1;
			next2 = first->line2;
			пока (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} иначе {
			next1 = end1;
			next2 = end2;
		}
		пока (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		если (next1 > line1 || next2 > line2) {
			если (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				вернуть -1;
		}

		если (!first)
			вернуть 0;

		пока (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

static целое fall_back_to_classic_diff(struct hashmap *map,
		целое line1, целое count1, целое line2, целое count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, sizeof(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	вернуть xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
static целое patience_diff(xpparam_t const *xpp, xdfenv_t *env,
		целое line1, целое count1, целое line2, целое count2)
{
	struct hashmap map;
	struct entry *first;
	целое result = 0;

	/* trivial case: one side is empty */
	если (!count1) {
		пока(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		вернуть 0;
	} иначе если (!count2) {
		пока(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		вернуть 0;
	}

	memset(&map, 0, sizeof(map));
	если (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		вернуть -1;

	/* are there any matching lines at all? */
	если (!map.has_matches) {
		пока(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		пока(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		вернуть 0;
	}

	result = find_longest_common_sequence(&map, &first);
	если (result)
		goto out;
	если (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	иначе
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	вернуть result;
}

целое xdl_do_patience_diff(xpparam_t const *xpp, xdfenv_t *env)
{
	вернуть patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
