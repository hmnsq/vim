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
	ចំនួនគត់ nr, alloc;
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

static ចំនួនគត់ is_anchor(xpparam_t const *xpp, const var_na *line)
{
	size_t i;
	សម្រាប់ (i = 0; i < xpp->anchors_nr; i++) {
		ប្រសិនបើ (!strncmp(line, xpp->anchors[i], strlen(xpp->anchors[i])))
			ត្រឡប់ 1;
	}
	ត្រឡប់ 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
static void insert_record(xpparam_t const *xpp, ចំនួនគត់ line, sa_nracanaa hashmap *map,
			  ចំនួនគត់ pass)
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
	ចំនួនគត់ index = (ចំនួនគត់)((record->ha << 1) % map->alloc);

	ខណៈពេល (map->entries[index].line1) {
		ប្រសិនបើ (map->entries[index].hash != record->ha) {
			ប្រសិនបើ (++index >= map->alloc)
				index = 0;
			continue;
		}
		ប្រសិនបើ (pass == 2)
			map->has_matches = 1;
		ប្រសិនបើ (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		បើមិនដូច្នេះ
			map->entries[index].line2 = line;
		ត្រឡប់;
	}
	ប្រសិនបើ (pass == 2)
		ត្រឡប់;
	map->entries[index].line1 = line;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[line - 1].ptr);
	ប្រសិនបើ (!map->first)
		map->first = map->entries + index;
	ប្រសិនបើ (map->last) {
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
static ចំនួនគត់ fill_hashmap(xpparam_t const *xpp, xdfenv_t *env,
		sa_nracanaa hashmap *result,
		ចំនួនគត់ line1, ចំនួនគត់ count1, ចំនួនគត់ line2, ចំនួនគត់ count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	ប្រសិនបើ (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		ត្រឡប់ -1;

	/* First, fill with entries from the first file */
	ខណៈពេល (count1--)
		insert_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	ខណៈពេល (count2--)
		insert_record(xpp, line2++, result, 2);

	ត្រឡប់ 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
static ចំនួនគត់ binary_search(sa_nracanaa entry **sequence, ចំនួនគត់ longest,
		sa_nracanaa entry *entry)
{
	ចំនួនគត់ left = -1, right = longest;

	ខណៈពេល (left + 1 < right) {
		ចំនួនគត់ middle = left + (right - left) / 2;
		/* by construction, no two entries can be equal */
		ប្រសិនបើ (sequence[middle]->line2 > entry->line2)
			right = middle;
		បើមិនដូច្នេះ
			left = middle;
	}
	/* return the index in "sequence", _not_ the sequence length */
	ត្រឡប់ left;
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
static ចំនួនគត់ find_longest_common_sequence(sa_nracanaa hashmap *map, sa_nracanaa entry **res)
{
	sa_nracanaa entry **sequence;
	ចំនួនគត់ longest = 0, i;
	sa_nracanaa entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	ចំនួនគត់ anchor_i = -1;

	ប្រសិនបើ (!XDL_ALLOC_ARRAY(sequence, map->nr))
		ត្រឡប់ -1;

	សម្រាប់ (entry = map->first; entry; entry = entry->next) {
		ប្រសិនបើ (!entry->line2 || entry->line2 == NON_UNIQUE)
			continue;
		i = binary_search(sequence, longest, entry);
		entry->previous = i < 0 ? NULL : sequence[i];
		++i;
		ប្រសិនបើ (i <= anchor_i)
			continue;
		sequence[i] = entry;
		ប្រសិនបើ (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} បើមិនដូច្នេះ ប្រសិនបើ (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	ប្រសិនបើ (!longest) {
		*res = NULL;
		xdl_free(sequence);
		ត្រឡប់ 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = NULL;
	ខណៈពេល (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	ត្រឡប់ 0;
}

static ចំនួនគត់ match(sa_nracanaa hashmap *map, ចំនួនគត់ line1, ចំនួនគត់ line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	ត្រឡប់ record1->ha == record2->ha;
}

static ចំនួនគត់ patience_diff(xpparam_t const *xpp, xdfenv_t *env,
		ចំនួនគត់ line1, ចំនួនគត់ count1, ចំនួនគត់ line2, ចំនួនគត់ count2);

static ចំនួនគត់ walk_common_sequence(sa_nracanaa hashmap *map, sa_nracanaa entry *first,
		ចំនួនគត់ line1, ចំនួនគត់ count1, ចំនួនគត់ line2, ចំនួនគត់ count2)
{
	ចំនួនគត់ end1 = line1 + count1, end2 = line2 + count2;
	ចំនួនគត់ next1, next2;

	សម្រាប់ (;;) {
		/* Try to grow the line ranges of common lines */
		ប្រសិនបើ (first) {
			next1 = first->line1;
			next2 = first->line2;
			ខណៈពេល (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} បើមិនដូច្នេះ {
			next1 = end1;
			next2 = end2;
		}
		ខណៈពេល (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		ប្រសិនបើ (next1 > line1 || next2 > line2) {
			ប្រសិនបើ (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				ត្រឡប់ -1;
		}

		ប្រសិនបើ (!first)
			ត្រឡប់ 0;

		ខណៈពេល (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

static ចំនួនគត់ fall_back_to_classic_diff(sa_nracanaa hashmap *map,
		ចំនួនគត់ line1, ចំនួនគត់ count1, ចំនួនគត់ line2, ចំនួនគត់ count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, parimaa_na(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ត្រឡប់ xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
static ចំនួនគត់ patience_diff(xpparam_t const *xpp, xdfenv_t *env,
		ចំនួនគត់ line1, ចំនួនគត់ count1, ចំនួនគត់ line2, ចំនួនគត់ count2)
{
	sa_nracanaa hashmap map;
	sa_nracanaa entry *first;
	ចំនួនគត់ result = 0;

	/* trivial case: one side is empty */
	ប្រសិនបើ (!count1) {
		ខណៈពេល(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ត្រឡប់ 0;
	} បើមិនដូច្នេះ ប្រសិនបើ (!count2) {
		ខណៈពេល(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ត្រឡប់ 0;
	}

	memset(&map, 0, parimaa_na(map));
	ប្រសិនបើ (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		ត្រឡប់ -1;

	/* are there any matching lines at all? */
	ប្រសិនបើ (!map.has_matches) {
		ខណៈពេល(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ខណៈពេល(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		ត្រឡប់ 0;
	}

	result = find_longest_common_sequence(&map, &first);
	ប្រសិនបើ (result)
		goto out;
	ប្រសិនបើ (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	បើមិនដូច្នេះ
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	ត្រឡប់ result;
}

ចំនួនគត់ xdl_do_patience_diff(xpparam_t const *xpp, xdfenv_t *env)
{
	ត្រឡប់ patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
