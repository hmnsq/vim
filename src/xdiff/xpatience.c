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

#অন্তর্ভাব "xinclude.h"

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
সংরচনা hashmap {
	পূর্ণাঙ্ক nr, alloc;
	সংরচনা entry {
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
		সংরচনা entry *next, *previous;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		unsigned anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	unsigned long has_matches;
	xdfenv_t *env;
	xpparam_t ধ্রুব *xpp;
};

স্থির পূর্ণাঙ্ক is_anchor(xpparam_t ধ্রুব *xpp, ধ্রুব বর্ণ *সারি)
{
	size_t i;
	প্রতি (i = 0; i < xpp->anchors_nr; i++) {
		যদি (!strncmp(সারি, xpp->anchors[i], strlen(xpp->anchors[i])))
			প্রত্যাগম 1;
	}
	প্রত্যাগম 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
স্থির শূন্য ঢোকাও_record(xpparam_t ধ্রুব *xpp, পূর্ণাঙ্ক সারি, সংরচনা hashmap *map,
			  পূর্ণাঙ্ক pass)
{
	xrecord_t *records = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *record = &records[সারি - 1];
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
	পূর্ণাঙ্ক index = (পূর্ণাঙ্ক)((record->ha << 1) % map->alloc);

	যাবত্ (map->entries[index].line1) {
		যদি (map->entries[index].hash != record->ha) {
			যদি (++index >= map->alloc)
				index = 0;
			অনুবর্ত;
		}
		যদি (pass == 2)
			map->has_matches = 1;
		যদি (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		অন্যথা
			map->entries[index].line2 = সারি;
		প্রত্যাগম;
	}
	যদি (pass == 2)
		প্রত্যাগম;
	map->entries[index].line1 = সারি;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[সারি - 1].ptr);
	যদি (!map->first)
		map->first = map->entries + index;
	যদি (map->last) {
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
স্থির পূর্ণাঙ্ক fill_hashmap(xpparam_t ধ্রুব *xpp, xdfenv_t *env,
		সংরচনা hashmap *result,
		পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	যদি (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		প্রত্যাগম -1;

	/* First, fill with entries from the first file */
	যাবত্ (count1--)
		ঢোকাও_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	যাবত্ (count2--)
		ঢোকাও_record(xpp, line2++, result, 2);

	প্রত্যাগম 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
স্থির পূর্ণাঙ্ক দ্বিমিক_অনুসন্ধান(সংরচনা entry **sequence, পূর্ণাঙ্ক longest,
		সংরচনা entry *entry)
{
	পূর্ণাঙ্ক বাম = -1, ডান = longest;

	যাবত্ (বাম + 1 < ডান) {
		পূর্ণাঙ্ক মধ্য_2 = বাম + (ডান - বাম) / 2;
		/* by construction, no two entries can be equal */
		যদি (sequence[মধ্য_2]->line2 > entry->line2)
			ডান = মধ্য_2;
		অন্যথা
			বাম = মধ্য_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	প্রত্যাগম বাম;
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
স্থির পূর্ণাঙ্ক find_longest_common_sequence(সংরচনা hashmap *map, সংরচনা entry **res)
{
	সংরচনা entry **sequence;
	পূর্ণাঙ্ক longest = 0, i;
	সংরচনা entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	পূর্ণাঙ্ক anchor_i = -1;

	যদি (!XDL_ALLOC_ARRAY(sequence, map->nr))
		প্রত্যাগম -1;

	প্রতি (entry = map->first; entry; entry = entry->next) {
		যদি (!entry->line2 || entry->line2 == NON_UNIQUE)
			অনুবর্ত;
		i = দ্বিমিক_অনুসন্ধান(sequence, longest, entry);
		entry->previous = i < 0 ? রিক্ত : sequence[i];
		++i;
		যদি (i <= anchor_i)
			অনুবর্ত;
		sequence[i] = entry;
		যদি (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} অন্যথা যদি (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	যদি (!longest) {
		*res = রিক্ত;
		xdl_free(sequence);
		প্রত্যাগম 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = রিক্ত;
	যাবত্ (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক match(সংরচনা hashmap *map, পূর্ণাঙ্ক line1, পূর্ণাঙ্ক line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	প্রত্যাগম record1->ha == record2->ha;
}

স্থির পূর্ণাঙ্ক patience_diff(xpparam_t ধ্রুব *xpp, xdfenv_t *env,
		পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2);

স্থির পূর্ণাঙ্ক walk_common_sequence(সংরচনা hashmap *map, সংরচনা entry *first,
		পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2)
{
	পূর্ণাঙ্ক end1 = line1 + count1, end2 = line2 + count2;
	পূর্ণাঙ্ক next1, next2;

	প্রতি (;;) {
		/* Try to grow the line ranges of common lines */
		যদি (first) {
			next1 = first->line1;
			next2 = first->line2;
			যাবত্ (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} অন্যথা {
			next1 = end1;
			next2 = end2;
		}
		যাবত্ (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		যদি (next1 > line1 || next2 > line2) {
			যদি (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				প্রত্যাগম -1;
		}

		যদি (!first)
			প্রত্যাগম 0;

		যাবত্ (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

স্থির পূর্ণাঙ্ক fall_back_to_classic_diff(সংরচনা hashmap *map,
		পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, পরিমাণ(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	প্রত্যাগম xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
স্থির পূর্ণাঙ্ক patience_diff(xpparam_t ধ্রুব *xpp, xdfenv_t *env,
		পূর্ণাঙ্ক line1, পূর্ণাঙ্ক count1, পূর্ণাঙ্ক line2, পূর্ণাঙ্ক count2)
{
	সংরচনা hashmap map;
	সংরচনা entry *first;
	পূর্ণাঙ্ক result = 0;

	/* trivial case: one side is empty */
	যদি (!count1) {
		যাবত্(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		প্রত্যাগম 0;
	} অন্যথা যদি (!count2) {
		যাবত্(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		প্রত্যাগম 0;
	}

	memset(&map, 0, পরিমাণ(map));
	যদি (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		প্রত্যাগম -1;

	/* are there any matching lines at all? */
	যদি (!map.has_matches) {
		যাবত্(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		যাবত্(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		প্রত্যাগম 0;
	}

	result = find_longest_common_sequence(&map, &first);
	যদি (result)
		গচ্ছ out;
	যদি (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	অন্যথা
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	প্রত্যাগম result;
}

পূর্ণাঙ্ক xdl_do_patience_diff(xpparam_t ধ্রুব *xpp, xdfenv_t *env)
{
	প্রত্যাগম patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
