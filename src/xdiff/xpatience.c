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

#અન્તર્ભાવ "xinclude.h"

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
સંરચના hashmap {
	પૂર્ણાંક nr, alloc;
	સંરચના entry {
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
		સંરચના entry *next, *previous;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		unsigned anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	unsigned long has_matches;
	xdfenv_t *env;
	xpparam_t અચલ *xpp;
};

સ્થિર પૂર્ણાંક is_anchor(xpparam_t અચલ *xpp, અચલ અક્ષર *લીટી)
{
	size_t i;
	માટે (i = 0; i < xpp->anchors_nr; i++) {
		જો (!strncmp(લીટી, xpp->anchors[i], strlen(xpp->anchors[i])))
			પાછા 1;
	}
	પાછા 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
સ્થિર શૂન્ય ઉમેરો_record(xpparam_t અચલ *xpp, પૂર્ણાંક લીટી, સંરચના hashmap *map,
			  પૂર્ણાંક pass)
{
	xrecord_t *records = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *record = &records[લીટી - 1];
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
	પૂર્ણાંક index = (પૂર્ણાંક)((record->ha << 1) % map->alloc);

	જ્યારે (map->entries[index].line1) {
		જો (map->entries[index].hash != record->ha) {
			જો (++index >= map->alloc)
				index = 0;
			ચાલુ;
		}
		જો (pass == 2)
			map->has_matches = 1;
		જો (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		નહીંતર
			map->entries[index].line2 = લીટી;
		પાછા;
	}
	જો (pass == 2)
		પાછા;
	map->entries[index].line1 = લીટી;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[લીટી - 1].ptr);
	જો (!map->first)
		map->first = map->entries + index;
	જો (map->last) {
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
સ્થિર પૂર્ણાંક fill_hashmap(xpparam_t અચલ *xpp, xdfenv_t *env,
		સંરચના hashmap *result,
		પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	જો (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		પાછા -1;

	/* First, fill with entries from the first file */
	જ્યારે (count1--)
		ઉમેરો_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	જ્યારે (count2--)
		ઉમેરો_record(xpp, line2++, result, 2);

	પાછા 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
સ્થિર પૂર્ણાંક દ્વિઅંકી_શોધ(સંરચના entry **sequence, પૂર્ણાંક longest,
		સંરચના entry *entry)
{
	પૂર્ણાંક ડાબું = -1, જમણું = longest;

	જ્યારે (ડાબું + 1 < જમણું) {
		પૂર્ણાંક મધ્ય_2 = ડાબું + (જમણું - ડાબું) / 2;
		/* by construction, no two entries can be equal */
		જો (sequence[મધ્ય_2]->line2 > entry->line2)
			જમણું = મધ્ય_2;
		નહીંતર
			ડાબું = મધ્ય_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	પાછા ડાબું;
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
સ્થિર પૂર્ણાંક find_longest_common_sequence(સંરચના hashmap *map, સંરચના entry **res)
{
	સંરચના entry **sequence;
	પૂર્ણાંક longest = 0, i;
	સંરચના entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	પૂર્ણાંક anchor_i = -1;

	જો (!XDL_ALLOC_ARRAY(sequence, map->nr))
		પાછા -1;

	માટે (entry = map->first; entry; entry = entry->next) {
		જો (!entry->line2 || entry->line2 == NON_UNIQUE)
			ચાલુ;
		i = દ્વિઅંકી_શોધ(sequence, longest, entry);
		entry->previous = i < 0 ? ખાલી : sequence[i];
		++i;
		જો (i <= anchor_i)
			ચાલુ;
		sequence[i] = entry;
		જો (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} નહીંતર જો (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	જો (!longest) {
		*res = ખાલી;
		xdl_free(sequence);
		પાછા 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = ખાલી;
	જ્યારે (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	પાછા 0;
}

સ્થિર પૂર્ણાંક match(સંરચના hashmap *map, પૂર્ણાંક line1, પૂર્ણાંક line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	પાછા record1->ha == record2->ha;
}

સ્થિર પૂર્ણાંક patience_diff(xpparam_t અચલ *xpp, xdfenv_t *env,
		પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2);

સ્થિર પૂર્ણાંક walk_common_sequence(સંરચના hashmap *map, સંરચના entry *first,
		પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2)
{
	પૂર્ણાંક end1 = line1 + count1, end2 = line2 + count2;
	પૂર્ણાંક next1, next2;

	માટે (;;) {
		/* Try to grow the line ranges of common lines */
		જો (first) {
			next1 = first->line1;
			next2 = first->line2;
			જ્યારે (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} નહીંતર {
			next1 = end1;
			next2 = end2;
		}
		જ્યારે (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		જો (next1 > line1 || next2 > line2) {
			જો (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				પાછા -1;
		}

		જો (!first)
			પાછા 0;

		જ્યારે (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

સ્થિર પૂર્ણાંક fall_back_to_classic_diff(સંરચના hashmap *map,
		પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, કદ(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	પાછા xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
સ્થિર પૂર્ણાંક patience_diff(xpparam_t અચલ *xpp, xdfenv_t *env,
		પૂર્ણાંક line1, પૂર્ણાંક count1, પૂર્ણાંક line2, પૂર્ણાંક count2)
{
	સંરચના hashmap map;
	સંરચના entry *first;
	પૂર્ણાંક result = 0;

	/* trivial case: one side is empty */
	જો (!count1) {
		જ્યારે(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		પાછા 0;
	} નહીંતર જો (!count2) {
		જ્યારે(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		પાછા 0;
	}

	memset(&map, 0, કદ(map));
	જો (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		પાછા -1;

	/* are there any matching lines at all? */
	જો (!map.has_matches) {
		જ્યારે(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		જ્યારે(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		પાછા 0;
	}

	result = find_longest_common_sequence(&map, &first);
	જો (result)
		જાઓ out;
	જો (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	નહીંતર
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	પાછા result;
}

પૂર્ણાંક xdl_do_patience_diff(xpparam_t અચલ *xpp, xdfenv_t *env)
{
	પાછા patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
