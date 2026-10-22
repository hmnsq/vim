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

#அந்தர்பாவ "xinclude.h"

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
கட்டமைப்பு hashmap {
	முழு nr, alloc;
	கட்டமைப்பு entry {
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
		கட்டமைப்பு entry *next, *previous;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		unsigned anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	unsigned long has_matches;
	xdfenv_t *env;
	xpparam_t மாறிலி *xpp;
};

நிலையான முழு is_anchor(xpparam_t மாறிலி *xpp, மாறிலி எழுத்து *வரி)
{
	size_t i;
	க்கு (i = 0; i < xpp->anchors_nr; i++) {
		எனில் (!strncmp(வரி, xpp->anchors[i], strlen(xpp->anchors[i])))
			திரும்பு 1;
	}
	திரும்பு 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
நிலையான வெறுமை செருகு_record(xpparam_t மாறிலி *xpp, முழு வரி, கட்டமைப்பு hashmap *map,
			  முழு pass)
{
	xrecord_t *records = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *record = &records[வரி - 1];
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
	முழு index = (முழு)((record->ha << 1) % map->alloc);

	போது (map->entries[index].line1) {
		எனில் (map->entries[index].hash != record->ha) {
			எனில் (++index >= map->alloc)
				index = 0;
			தொடர்;
		}
		எனில் (pass == 2)
			map->has_matches = 1;
		எனில் (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		இல்லையெனில்
			map->entries[index].line2 = வரி;
		திரும்பு;
	}
	எனில் (pass == 2)
		திரும்பு;
	map->entries[index].line1 = வரி;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[வரி - 1].ptr);
	எனில் (!map->first)
		map->first = map->entries + index;
	எனில் (map->last) {
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
நிலையான முழு fill_hashmap(xpparam_t மாறிலி *xpp, xdfenv_t *env,
		கட்டமைப்பு hashmap *result,
		முழு line1, முழு count1, முழு line2, முழு count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	எனில் (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		திரும்பு -1;

	/* First, fill with entries from the first file */
	போது (count1--)
		செருகு_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	போது (count2--)
		செருகு_record(xpp, line2++, result, 2);

	திரும்பு 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
நிலையான முழு இருமத்_தேடல்(கட்டமைப்பு entry **sequence, முழு longest,
		கட்டமைப்பு entry *entry)
{
	முழு இடது = -1, வலது = longest;

	போது (இடது + 1 < வலது) {
		முழு நடு_2 = இடது + (வலது - இடது) / 2;
		/* by construction, no two entries can be equal */
		எனில் (sequence[நடு_2]->line2 > entry->line2)
			வலது = நடு_2;
		இல்லையெனில்
			இடது = நடு_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	திரும்பு இடது;
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
நிலையான முழு find_longest_common_sequence(கட்டமைப்பு hashmap *map, கட்டமைப்பு entry **res)
{
	கட்டமைப்பு entry **sequence;
	முழு longest = 0, i;
	கட்டமைப்பு entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	முழு anchor_i = -1;

	எனில் (!XDL_ALLOC_ARRAY(sequence, map->nr))
		திரும்பு -1;

	க்கு (entry = map->first; entry; entry = entry->next) {
		எனில் (!entry->line2 || entry->line2 == NON_UNIQUE)
			தொடர்;
		i = இருமத்_தேடல்(sequence, longest, entry);
		entry->previous = i < 0 ? வெற்று : sequence[i];
		++i;
		எனில் (i <= anchor_i)
			தொடர்;
		sequence[i] = entry;
		எனில் (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} இல்லையெனில் எனில் (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	எனில் (!longest) {
		*res = வெற்று;
		xdl_free(sequence);
		திரும்பு 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = வெற்று;
	போது (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	திரும்பு 0;
}

நிலையான முழு match(கட்டமைப்பு hashmap *map, முழு line1, முழு line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	திரும்பு record1->ha == record2->ha;
}

நிலையான முழு patience_diff(xpparam_t மாறிலி *xpp, xdfenv_t *env,
		முழு line1, முழு count1, முழு line2, முழு count2);

நிலையான முழு walk_common_sequence(கட்டமைப்பு hashmap *map, கட்டமைப்பு entry *first,
		முழு line1, முழு count1, முழு line2, முழு count2)
{
	முழு end1 = line1 + count1, end2 = line2 + count2;
	முழு next1, next2;

	க்கு (;;) {
		/* Try to grow the line ranges of common lines */
		எனில் (first) {
			next1 = first->line1;
			next2 = first->line2;
			போது (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} இல்லையெனில் {
			next1 = end1;
			next2 = end2;
		}
		போது (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		எனில் (next1 > line1 || next2 > line2) {
			எனில் (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				திரும்பு -1;
		}

		எனில் (!first)
			திரும்பு 0;

		போது (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

நிலையான முழு fall_back_to_classic_diff(கட்டமைப்பு hashmap *map,
		முழு line1, முழு count1, முழு line2, முழு count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, அளவு(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	திரும்பு xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
நிலையான முழு patience_diff(xpparam_t மாறிலி *xpp, xdfenv_t *env,
		முழு line1, முழு count1, முழு line2, முழு count2)
{
	கட்டமைப்பு hashmap map;
	கட்டமைப்பு entry *first;
	முழு result = 0;

	/* trivial case: one side is empty */
	எனில் (!count1) {
		போது(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		திரும்பு 0;
	} இல்லையெனில் எனில் (!count2) {
		போது(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		திரும்பு 0;
	}

	memset(&map, 0, அளவு(map));
	எனில் (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		திரும்பு -1;

	/* are there any matching lines at all? */
	எனில் (!map.has_matches) {
		போது(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		போது(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		திரும்பு 0;
	}

	result = find_longest_common_sequence(&map, &first);
	எனில் (result)
		செல் out;
	எனில் (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	இல்லையெனில்
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	திரும்பு result;
}

முழு xdl_do_patience_diff(xpparam_t மாறிலி *xpp, xdfenv_t *env)
{
	திரும்பு patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
