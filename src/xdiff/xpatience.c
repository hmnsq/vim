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

#define NON_UNIQUE ULONG_MAX

/*
 * This is a hash mapping from line hash to line numbers in the first and
 * second file.
 */
بنية hashmap {
	صحيح nr, alloc;
	بنية entry {
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
		بنية entry *next, *previous;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		unsigned anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	unsigned long has_matches;
	xdfenv_t *env;
	xpparam_t ثابت *xpp;
};

ساكن صحيح is_anchor(xpparam_t ثابت *xpp, ثابت حرف *line)
{
	size_t i;
	لكل (i = 0; i < xpp->anchors_nr; i++) {
		ܐܢ (!strncmp(line, xpp->anchors[i], strlen(xpp->anchors[i])))
			ܦܢܐ 1;
	}
	ܦܢܐ 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
ساكن فراغ insert_record(xpparam_t ثابت *xpp, صحيح line, بنية hashmap *map,
			  صحيح pass)
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
	صحيح index = (صحيح)((record->ha << 1) % map->alloc);

	ܟܡܐ (map->entries[index].line1) {
		ܐܢ (map->entries[index].hash != record->ha) {
			ܐܢ (++index >= map->alloc)
				index = 0;
			استمر;
		}
		ܐܢ (pass == 2)
			map->has_matches = 1;
		ܐܢ (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		ܐܚܪܢܐ
			map->entries[index].line2 = line;
		ܦܢܐ;
	}
	ܐܢ (pass == 2)
		ܦܢܐ;
	map->entries[index].line1 = line;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[line - 1].ptr);
	ܐܢ (!map->first)
		map->first = map->entries + index;
	ܐܢ (map->last) {
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
ساكن صحيح fill_hashmap(xpparam_t ثابت *xpp, xdfenv_t *env,
		بنية hashmap *result,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	ܐܢ (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		ܦܢܐ -1;

	/* First, fill with entries from the first file */
	ܟܡܐ (count1--)
		insert_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	ܟܡܐ (count2--)
		insert_record(xpp, line2++, result, 2);

	ܦܢܐ 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
ساكن صحيح binary_search(بنية entry **sequence, صحيح longest,
		بنية entry *entry)
{
	صحيح left = -1, right = longest;

	ܟܡܐ (left + 1 < right) {
		صحيح middle = left + (right - left) / 2;
		/* by construction, no two entries can be equal */
		ܐܢ (sequence[middle]->line2 > entry->line2)
			right = middle;
		ܐܚܪܢܐ
			left = middle;
	}
	/* return the index in "sequence", _not_ the sequence length */
	ܦܢܐ left;
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
ساكن صحيح find_longest_common_sequence(بنية hashmap *map, بنية entry **res)
{
	بنية entry **sequence;
	صحيح longest = 0, i;
	بنية entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	صحيح anchor_i = -1;

	ܐܢ (!XDL_ALLOC_ARRAY(sequence, map->nr))
		ܦܢܐ -1;

	لكل (entry = map->first; entry; entry = entry->next) {
		ܐܢ (!entry->line2 || entry->line2 == NON_UNIQUE)
			استمر;
		i = binary_search(sequence, longest, entry);
		entry->previous = i < 0 ? عدم : sequence[i];
		++i;
		ܐܢ (i <= anchor_i)
			استمر;
		sequence[i] = entry;
		ܐܢ (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} ܐܚܪܢܐ ܐܢ (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	ܐܢ (!longest) {
		*res = عدم;
		xdl_free(sequence);
		ܦܢܐ 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = عدم;
	ܟܡܐ (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	ܦܢܐ 0;
}

ساكن صحيح match(بنية hashmap *map, صحيح line1, صحيح line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	ܦܢܐ record1->ha == record2->ha;
}

ساكن صحيح patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2);

ساكن صحيح walk_common_sequence(بنية hashmap *map, بنية entry *first,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	صحيح end1 = line1 + count1, end2 = line2 + count2;
	صحيح next1, next2;

	لكل (;;) {
		/* Try to grow the line ranges of common lines */
		ܐܢ (first) {
			next1 = first->line1;
			next2 = first->line2;
			ܟܡܐ (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} ܐܚܪܢܐ {
			next1 = end1;
			next2 = end2;
		}
		ܟܡܐ (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		ܐܢ (next1 > line1 || next2 > line2) {
			ܐܢ (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				ܦܢܐ -1;
		}

		ܐܢ (!first)
			ܦܢܐ 0;

		ܟܡܐ (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

ساكن صحيح fall_back_to_classic_diff(بنية hashmap *map,
		صحيح line1, صحيح count1, صحيح line2, صحيح count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, حجم(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ܦܢܐ xdl_fall_back_diff(map->env, &xpp,
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
	بنية entry *first;
	صحيح result = 0;

	/* trivial case: one side is empty */
	ܐܢ (!count1) {
		ܟܡܐ(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ܦܢܐ 0;
	} ܐܚܪܢܐ ܐܢ (!count2) {
		ܟܡܐ(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ܦܢܐ 0;
	}

	memset(&map, 0, حجم(map));
	ܐܢ (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		ܦܢܐ -1;

	/* are there any matching lines at all? */
	ܐܢ (!map.has_matches) {
		ܟܡܐ(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ܟܡܐ(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		ܦܢܐ 0;
	}

	result = find_longest_common_sequence(&map, &first);
	ܐܢ (result)
		اذهب out;
	ܐܢ (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	ܐܚܪܢܐ
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	ܦܢܐ result;
}

صحيح xdl_do_patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env)
{
	ܦܢܐ patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
