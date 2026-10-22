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
	ကိန်းပြည့် nr, alloc;
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

static ကိန်းပြည့် is_anchor(xpparam_t const *xpp, const var_na *line)
{
	size_t i;
	အတွက် (i = 0; i < xpp->anchors_nr; i++) {
		အကယ်၍ (!strncmp(line, xpp->anchors[i], strlen(xpp->anchors[i])))
			ပြန်ပေး 1;
	}
	ပြန်ပေး 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
static void insert_record(xpparam_t const *xpp, ကိန်းပြည့် line, sa_nracanaa hashmap *map,
			  ကိန်းပြည့် pass)
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
	ကိန်းပြည့် index = (ကိန်းပြည့်)((record->ha << 1) % map->alloc);

	နေစဉ် (map->entries[index].line1) {
		အကယ်၍ (map->entries[index].hash != record->ha) {
			အကယ်၍ (++index >= map->alloc)
				index = 0;
			continue;
		}
		အကယ်၍ (pass == 2)
			map->has_matches = 1;
		အကယ်၍ (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		မဟုတ်ပါက
			map->entries[index].line2 = line;
		ပြန်ပေး;
	}
	အကယ်၍ (pass == 2)
		ပြန်ပေး;
	map->entries[index].line1 = line;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[line - 1].ptr);
	အကယ်၍ (!map->first)
		map->first = map->entries + index;
	အကယ်၍ (map->last) {
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
static ကိန်းပြည့် fill_hashmap(xpparam_t const *xpp, xdfenv_t *env,
		sa_nracanaa hashmap *result,
		ကိန်းပြည့် line1, ကိန်းပြည့် count1, ကိန်းပြည့် line2, ကိန်းပြည့် count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	အကယ်၍ (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		ပြန်ပေး -1;

	/* First, fill with entries from the first file */
	နေစဉ် (count1--)
		insert_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	နေစဉ် (count2--)
		insert_record(xpp, line2++, result, 2);

	ပြန်ပေး 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
static ကိန်းပြည့် binary_search(sa_nracanaa entry **sequence, ကိန်းပြည့် longest,
		sa_nracanaa entry *entry)
{
	ကိန်းပြည့် left = -1, right = longest;

	နေစဉ် (left + 1 < right) {
		ကိန်းပြည့် middle = left + (right - left) / 2;
		/* by construction, no two entries can be equal */
		အကယ်၍ (sequence[middle]->line2 > entry->line2)
			right = middle;
		မဟုတ်ပါက
			left = middle;
	}
	/* return the index in "sequence", _not_ the sequence length */
	ပြန်ပေး left;
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
static ကိန်းပြည့် find_longest_common_sequence(sa_nracanaa hashmap *map, sa_nracanaa entry **res)
{
	sa_nracanaa entry **sequence;
	ကိန်းပြည့် longest = 0, i;
	sa_nracanaa entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	ကိန်းပြည့် anchor_i = -1;

	အကယ်၍ (!XDL_ALLOC_ARRAY(sequence, map->nr))
		ပြန်ပေး -1;

	အတွက် (entry = map->first; entry; entry = entry->next) {
		အကယ်၍ (!entry->line2 || entry->line2 == NON_UNIQUE)
			continue;
		i = binary_search(sequence, longest, entry);
		entry->previous = i < 0 ? NULL : sequence[i];
		++i;
		အကယ်၍ (i <= anchor_i)
			continue;
		sequence[i] = entry;
		အကယ်၍ (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} မဟုတ်ပါက အကယ်၍ (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	အကယ်၍ (!longest) {
		*res = NULL;
		xdl_free(sequence);
		ပြန်ပေး 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = NULL;
	နေစဉ် (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	ပြန်ပေး 0;
}

static ကိန်းပြည့် match(sa_nracanaa hashmap *map, ကိန်းပြည့် line1, ကိန်းပြည့် line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	ပြန်ပေး record1->ha == record2->ha;
}

static ကိန်းပြည့် patience_diff(xpparam_t const *xpp, xdfenv_t *env,
		ကိန်းပြည့် line1, ကိန်းပြည့် count1, ကိန်းပြည့် line2, ကိန်းပြည့် count2);

static ကိန်းပြည့် walk_common_sequence(sa_nracanaa hashmap *map, sa_nracanaa entry *first,
		ကိန်းပြည့် line1, ကိန်းပြည့် count1, ကိန်းပြည့် line2, ကိန်းပြည့် count2)
{
	ကိန်းပြည့် end1 = line1 + count1, end2 = line2 + count2;
	ကိန်းပြည့် next1, next2;

	အတွက် (;;) {
		/* Try to grow the line ranges of common lines */
		အကယ်၍ (first) {
			next1 = first->line1;
			next2 = first->line2;
			နေစဉ် (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} မဟုတ်ပါက {
			next1 = end1;
			next2 = end2;
		}
		နေစဉ် (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		အကယ်၍ (next1 > line1 || next2 > line2) {
			အကယ်၍ (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				ပြန်ပေး -1;
		}

		အကယ်၍ (!first)
			ပြန်ပေး 0;

		နေစဉ် (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

static ကိန်းပြည့် fall_back_to_classic_diff(sa_nracanaa hashmap *map,
		ကိန်းပြည့် line1, ကိန်းပြည့် count1, ကိန်းပြည့် line2, ကိန်းပြည့် count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, parimaa_na(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ပြန်ပေး xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
static ကိန်းပြည့် patience_diff(xpparam_t const *xpp, xdfenv_t *env,
		ကိန်းပြည့် line1, ကိန်းပြည့် count1, ကိန်းပြည့် line2, ကိန်းပြည့် count2)
{
	sa_nracanaa hashmap map;
	sa_nracanaa entry *first;
	ကိန်းပြည့် result = 0;

	/* trivial case: one side is empty */
	အကယ်၍ (!count1) {
		နေစဉ်(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ပြန်ပေး 0;
	} မဟုတ်ပါက အကယ်၍ (!count2) {
		နေစဉ်(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ပြန်ပေး 0;
	}

	memset(&map, 0, parimaa_na(map));
	အကယ်၍ (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		ပြန်ပေး -1;

	/* are there any matching lines at all? */
	အကယ်၍ (!map.has_matches) {
		နေစဉ်(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		နေစဉ်(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		ပြန်ပေး 0;
	}

	result = find_longest_common_sequence(&map, &first);
	အကယ်၍ (result)
		goto out;
	အကယ်၍ (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	မဟုတ်ပါက
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	ပြန်ပေး result;
}

ကိန်းပြည့် xdl_do_patience_diff(xpparam_t const *xpp, xdfenv_t *env)
{
	ပြန်ပေး patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
