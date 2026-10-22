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

#അന്തര്ഭാവ "xinclude.h"

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
ഘടന hashmap {
	പൂർണ്ണസംഖ്യ nr, alloc;
	ഘടന entry {
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
		ഘടന entry *next, *previous;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		unsigned anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	unsigned long has_matches;
	xdfenv_t *env;
	xpparam_t സ്ഥിരം *xpp;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ is_anchor(xpparam_t സ്ഥിരം *xpp, സ്ഥിരം അക്ഷരം *വരി)
{
	size_t i;
	വേണ്ടി (i = 0; i < xpp->anchors_nr; i++) {
		എങ്കിൽ (!strncmp(വരി, xpp->anchors[i], strlen(xpp->anchors[i])))
			തിരികെ 1;
	}
	തിരികെ 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
സ്ഥിതം ശൂന്യം തിരുകുക_record(xpparam_t സ്ഥിരം *xpp, പൂർണ്ണസംഖ്യ വരി, ഘടന hashmap *map,
			  പൂർണ്ണസംഖ്യ pass)
{
	xrecord_t *records = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *record = &records[വരി - 1];
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
	പൂർണ്ണസംഖ്യ index = (പൂർണ്ണസംഖ്യ)((record->ha << 1) % map->alloc);

	ആകുമ്പോൾ (map->entries[index].line1) {
		എങ്കിൽ (map->entries[index].hash != record->ha) {
			എങ്കിൽ (++index >= map->alloc)
				index = 0;
			തുടരുക;
		}
		എങ്കിൽ (pass == 2)
			map->has_matches = 1;
		എങ്കിൽ (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		അല്ലെങ്കിൽ
			map->entries[index].line2 = വരി;
		തിരികെ;
	}
	എങ്കിൽ (pass == 2)
		തിരികെ;
	map->entries[index].line1 = വരി;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[വരി - 1].ptr);
	എങ്കിൽ (!map->first)
		map->first = map->entries + index;
	എങ്കിൽ (map->last) {
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
സ്ഥിതം പൂർണ്ണസംഖ്യ fill_hashmap(xpparam_t സ്ഥിരം *xpp, xdfenv_t *env,
		ഘടന hashmap *result,
		പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	എങ്കിൽ (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		തിരികെ -1;

	/* First, fill with entries from the first file */
	ആകുമ്പോൾ (count1--)
		തിരുകുക_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	ആകുമ്പോൾ (count2--)
		തിരുകുക_record(xpp, line2++, result, 2);

	തിരികെ 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ ദ്വിമാന_തിരയൽ(ഘടന entry **sequence, പൂർണ്ണസംഖ്യ longest,
		ഘടന entry *entry)
{
	പൂർണ്ണസംഖ്യ ഇടത് = -1, വലത് = longest;

	ആകുമ്പോൾ (ഇടത് + 1 < വലത്) {
		പൂർണ്ണസംഖ്യ നടുവ്_2 = ഇടത് + (വലത് - ഇടത്) / 2;
		/* by construction, no two entries can be equal */
		എങ്കിൽ (sequence[നടുവ്_2]->line2 > entry->line2)
			വലത് = നടുവ്_2;
		അല്ലെങ്കിൽ
			ഇടത് = നടുവ്_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	തിരികെ ഇടത്;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ find_longest_common_sequence(ഘടന hashmap *map, ഘടന entry **res)
{
	ഘടന entry **sequence;
	പൂർണ്ണസംഖ്യ longest = 0, i;
	ഘടന entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	പൂർണ്ണസംഖ്യ anchor_i = -1;

	എങ്കിൽ (!XDL_ALLOC_ARRAY(sequence, map->nr))
		തിരികെ -1;

	വേണ്ടി (entry = map->first; entry; entry = entry->next) {
		എങ്കിൽ (!entry->line2 || entry->line2 == NON_UNIQUE)
			തുടരുക;
		i = ദ്വിമാന_തിരയൽ(sequence, longest, entry);
		entry->previous = i < 0 ? ശൂന്യ : sequence[i];
		++i;
		എങ്കിൽ (i <= anchor_i)
			തുടരുക;
		sequence[i] = entry;
		എങ്കിൽ (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} അല്ലെങ്കിൽ എങ്കിൽ (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	എങ്കിൽ (!longest) {
		*res = ശൂന്യ;
		xdl_free(sequence);
		തിരികെ 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = ശൂന്യ;
	ആകുമ്പോൾ (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ match(ഘടന hashmap *map, പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	തിരികെ record1->ha == record2->ha;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ patience_diff(xpparam_t സ്ഥിരം *xpp, xdfenv_t *env,
		പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2);

സ്ഥിതം പൂർണ്ണസംഖ്യ walk_common_sequence(ഘടന hashmap *map, ഘടന entry *first,
		പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2)
{
	പൂർണ്ണസംഖ്യ end1 = line1 + count1, end2 = line2 + count2;
	പൂർണ്ണസംഖ്യ next1, next2;

	വേണ്ടി (;;) {
		/* Try to grow the line ranges of common lines */
		എങ്കിൽ (first) {
			next1 = first->line1;
			next2 = first->line2;
			ആകുമ്പോൾ (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} അല്ലെങ്കിൽ {
			next1 = end1;
			next2 = end2;
		}
		ആകുമ്പോൾ (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		എങ്കിൽ (next1 > line1 || next2 > line2) {
			എങ്കിൽ (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				തിരികെ -1;
		}

		എങ്കിൽ (!first)
			തിരികെ 0;

		ആകുമ്പോൾ (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fall_back_to_classic_diff(ഘടന hashmap *map,
		പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, വലുപ്പം(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	തിരികെ xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ patience_diff(xpparam_t സ്ഥിരം *xpp, xdfenv_t *env,
		പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ count1, പൂർണ്ണസംഖ്യ line2, പൂർണ്ണസംഖ്യ count2)
{
	ഘടന hashmap map;
	ഘടന entry *first;
	പൂർണ്ണസംഖ്യ result = 0;

	/* trivial case: one side is empty */
	എങ്കിൽ (!count1) {
		ആകുമ്പോൾ(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		തിരികെ 0;
	} അല്ലെങ്കിൽ എങ്കിൽ (!count2) {
		ആകുമ്പോൾ(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		തിരികെ 0;
	}

	memset(&map, 0, വലുപ്പം(map));
	എങ്കിൽ (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		തിരികെ -1;

	/* are there any matching lines at all? */
	എങ്കിൽ (!map.has_matches) {
		ആകുമ്പോൾ(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ആകുമ്പോൾ(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		തിരികെ 0;
	}

	result = find_longest_common_sequence(&map, &first);
	എങ്കിൽ (result)
		പോകുക out;
	എങ്കിൽ (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	അല്ലെങ്കിൽ
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	തിരികെ result;
}

പൂർണ്ണസംഖ്യ xdl_do_patience_diff(xpparam_t സ്ഥിരം *xpp, xdfenv_t *env)
{
	തിരികെ patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
