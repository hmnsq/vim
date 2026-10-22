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
	số_nguyên nr, alloc;
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

static số_nguyên is_anchor(xpparam_t const *xpp, const char *line)
{
	size_t i;
	cho (i = 0; i < xpp->anchors_nr; i++) {
		nếu (!strncmp(line, xpp->anchors[i], strlen(xpp->anchors[i])))
			trả_về 1;
	}
	trả_về 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
static void insert_record(xpparam_t const *xpp, số_nguyên line, struct hashmap *map,
			  số_nguyên pass)
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
	số_nguyên index = (số_nguyên)((record->ha << 1) % map->alloc);

	trong_khi (map->entries[index].line1) {
		nếu (map->entries[index].hash != record->ha) {
			nếu (++index >= map->alloc)
				index = 0;
			continue;
		}
		nếu (pass == 2)
			map->has_matches = 1;
		nếu (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		không_thì
			map->entries[index].line2 = line;
		trả_về;
	}
	nếu (pass == 2)
		trả_về;
	map->entries[index].line1 = line;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[line - 1].ptr);
	nếu (!map->first)
		map->first = map->entries + index;
	nếu (map->last) {
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
static số_nguyên fill_hashmap(xpparam_t const *xpp, xdfenv_t *env,
		struct hashmap *result,
		số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	nếu (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		trả_về -1;

	/* First, fill with entries from the first file */
	trong_khi (count1--)
		insert_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	trong_khi (count2--)
		insert_record(xpp, line2++, result, 2);

	trả_về 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
static số_nguyên binary_search(struct entry **sequence, số_nguyên longest,
		struct entry *entry)
{
	số_nguyên left = -1, right = longest;

	trong_khi (left + 1 < right) {
		số_nguyên middle = left + (right - left) / 2;
		/* by construction, no two entries can be equal */
		nếu (sequence[middle]->line2 > entry->line2)
			right = middle;
		không_thì
			left = middle;
	}
	/* return the index in "sequence", _not_ the sequence length */
	trả_về left;
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
static số_nguyên find_longest_common_sequence(struct hashmap *map, struct entry **res)
{
	struct entry **sequence;
	số_nguyên longest = 0, i;
	struct entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	số_nguyên anchor_i = -1;

	nếu (!XDL_ALLOC_ARRAY(sequence, map->nr))
		trả_về -1;

	cho (entry = map->first; entry; entry = entry->next) {
		nếu (!entry->line2 || entry->line2 == NON_UNIQUE)
			continue;
		i = binary_search(sequence, longest, entry);
		entry->previous = i < 0 ? NULL : sequence[i];
		++i;
		nếu (i <= anchor_i)
			continue;
		sequence[i] = entry;
		nếu (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} không_thì nếu (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	nếu (!longest) {
		*res = NULL;
		xdl_free(sequence);
		trả_về 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = NULL;
	trong_khi (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	trả_về 0;
}

static số_nguyên match(struct hashmap *map, số_nguyên line1, số_nguyên line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	trả_về record1->ha == record2->ha;
}

static số_nguyên patience_diff(xpparam_t const *xpp, xdfenv_t *env,
		số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2);

static số_nguyên walk_common_sequence(struct hashmap *map, struct entry *first,
		số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2)
{
	số_nguyên end1 = line1 + count1, end2 = line2 + count2;
	số_nguyên next1, next2;

	cho (;;) {
		/* Try to grow the line ranges of common lines */
		nếu (first) {
			next1 = first->line1;
			next2 = first->line2;
			trong_khi (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} không_thì {
			next1 = end1;
			next2 = end2;
		}
		trong_khi (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		nếu (next1 > line1 || next2 > line2) {
			nếu (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				trả_về -1;
		}

		nếu (!first)
			trả_về 0;

		trong_khi (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

static số_nguyên fall_back_to_classic_diff(struct hashmap *map,
		số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, sizeof(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	trả_về xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
static số_nguyên patience_diff(xpparam_t const *xpp, xdfenv_t *env,
		số_nguyên line1, số_nguyên count1, số_nguyên line2, số_nguyên count2)
{
	struct hashmap map;
	struct entry *first;
	số_nguyên result = 0;

	/* trivial case: one side is empty */
	nếu (!count1) {
		trong_khi(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		trả_về 0;
	} không_thì nếu (!count2) {
		trong_khi(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		trả_về 0;
	}

	memset(&map, 0, sizeof(map));
	nếu (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		trả_về -1;

	/* are there any matching lines at all? */
	nếu (!map.has_matches) {
		trong_khi(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		trong_khi(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		trả_về 0;
	}

	result = find_longest_common_sequence(&map, &first);
	nếu (result)
		goto out;
	nếu (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	không_thì
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	trả_về result;
}

số_nguyên xdl_do_patience_diff(xpparam_t const *xpp, xdfenv_t *env)
{
	trả_về patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
