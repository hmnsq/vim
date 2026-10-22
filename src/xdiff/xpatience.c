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
কাঠামো hashmap {
	পূর্ণসংখ্যা nr, alloc;
	কাঠামো entry {
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
		কাঠামো entry *পরবর্তী, *previous;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		unsigned anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	unsigned long has_matches;
	xdfenv_t *env;
	xpparam_t ধ্রুবক *xpp;
};

স্থির পূর্ণসংখ্যা is_anchor(xpparam_t ধ্রুবক *xpp, ধ্রুবক অক্ষর *সারি)
{
	size_t i;
	জন্য (i = 0; i < xpp->anchors_nr; i++) {
		যদি (!strncmp(সারি, xpp->anchors[i], strlen(xpp->anchors[i])))
			ফেরত 1;
	}
	ফেরত 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
স্থির শূন্য ঢোকাও_record(xpparam_t ধ্রুবক *xpp, পূর্ণসংখ্যা সারি, কাঠামো hashmap *map,
			  পূর্ণসংখ্যা pass)
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
	পূর্ণসংখ্যা সূচক = (পূর্ণসংখ্যা)((record->ha << 1) % map->alloc);

	যখন (map->entries[সূচক].line1) {
		যদি (map->entries[সূচক].hash != record->ha) {
			যদি (++সূচক >= map->alloc)
				সূচক = 0;
			চালু;
		}
		যদি (pass == 2)
			map->has_matches = 1;
		যদি (pass == 1 || map->entries[সূচক].line2)
			map->entries[সূচক].line2 = NON_UNIQUE;
		অন্যথা
			map->entries[সূচক].line2 = সারি;
		ফেরত;
	}
	যদি (pass == 2)
		ফেরত;
	map->entries[সূচক].line1 = সারি;
	map->entries[সূচক].hash = record->ha;
	map->entries[সূচক].anchor = is_anchor(xpp, map->env->xdf1.recs[সারি - 1].ptr);
	যদি (!map->first)
		map->first = map->entries + সূচক;
	যদি (map->last) {
		map->last->পরবর্তী = map->entries + সূচক;
		map->entries[সূচক].previous = map->last;
	}
	map->last = map->entries + সূচক;
	map->nr++;
}

/*
 * This function has to be called for each recursion into the inter-hunk
 * parts, as previously non-unique lines can become unique when being
 * restricted to a smaller part of the files.
 *
 * It is assumed that env has been prepared using xdl_prepare().
 */
স্থির পূর্ণসংখ্যা fill_hashmap(xpparam_t ধ্রুবক *xpp, xdfenv_t *env,
		কাঠামো hashmap *ফলাফল,
		পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2)
{
	ফলাফল->xpp = xpp;
	ফলাফল->env = env;

	/* We know exactly how large we want the hash map */
	ফলাফল->alloc = count1 * 2;
	যদি (!XDL_CALLOC_ARRAY(ফলাফল->entries, ফলাফল->alloc))
		ফেরত -1;

	/* First, fill with entries from the first file */
	যখন (count1--)
		ঢোকাও_record(xpp, line1++, ফলাফল, 1);

	/* Then search for matches in the second file */
	যখন (count2--)
		ঢোকাও_record(xpp, line2++, ফলাফল, 2);

	ফেরত 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
স্থির পূর্ণসংখ্যা দ্বিমিক_অনুসন্ধান(কাঠামো entry **sequence, পূর্ণসংখ্যা longest,
		কাঠামো entry *entry)
{
	পূর্ণসংখ্যা বাম = -1, ডান = longest;

	যখন (বাম + 1 < ডান) {
		পূর্ণসংখ্যা মধ্য_2 = বাম + (ডান - বাম) / 2;
		/* by construction, no two entries can be equal */
		যদি (sequence[মধ্য_2]->line2 > entry->line2)
			ডান = মধ্য_2;
		অন্যথা
			বাম = মধ্য_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	ফেরত বাম;
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
স্থির পূর্ণসংখ্যা find_longest_common_sequence(কাঠামো hashmap *map, কাঠামো entry **res)
{
	কাঠামো entry **sequence;
	পূর্ণসংখ্যা longest = 0, i;
	কাঠামো entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	পূর্ণসংখ্যা anchor_i = -1;

	যদি (!XDL_ALLOC_ARRAY(sequence, map->nr))
		ফেরত -1;

	জন্য (entry = map->first; entry; entry = entry->পরবর্তী) {
		যদি (!entry->line2 || entry->line2 == NON_UNIQUE)
			চালু;
		i = দ্বিমিক_অনুসন্ধান(sequence, longest, entry);
		entry->previous = i < 0 ? ফাঁকা : sequence[i];
		++i;
		যদি (i <= anchor_i)
			চালু;
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
		*res = ফাঁকা;
		xdl_free(sequence);
		ফেরত 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->পরবর্তী = ফাঁকা;
	যখন (entry->previous) {
		entry->previous->পরবর্তী = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা match(কাঠামো hashmap *map, পূর্ণসংখ্যা line1, পূর্ণসংখ্যা line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	ফেরত record1->ha == record2->ha;
}

স্থির পূর্ণসংখ্যা patience_diff(xpparam_t ধ্রুবক *xpp, xdfenv_t *env,
		পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2);

স্থির পূর্ণসংখ্যা walk_common_sequence(কাঠামো hashmap *map, কাঠামো entry *first,
		পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2)
{
	পূর্ণসংখ্যা end1 = line1 + count1, end2 = line2 + count2;
	পূর্ণসংখ্যা next1, next2;

	জন্য (;;) {
		/* Try to grow the line ranges of common lines */
		যদি (first) {
			next1 = first->line1;
			next2 = first->line2;
			যখন (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} অন্যথা {
			next1 = end1;
			next2 = end2;
		}
		যখন (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		যদি (next1 > line1 || next2 > line2) {
			যদি (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				ফেরত -1;
		}

		যদি (!first)
			ফেরত 0;

		যখন (first->পরবর্তী &&
				first->পরবর্তী->line1 == first->line1 + 1 &&
				first->পরবর্তী->line2 == first->line2 + 1)
			first = first->পরবর্তী;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->পরবর্তী;
	}
}

স্থির পূর্ণসংখ্যা fall_back_to_classic_diff(কাঠামো hashmap *map,
		পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, আকার(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ফেরত xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
স্থির পূর্ণসংখ্যা patience_diff(xpparam_t ধ্রুবক *xpp, xdfenv_t *env,
		পূর্ণসংখ্যা line1, পূর্ণসংখ্যা count1, পূর্ণসংখ্যা line2, পূর্ণসংখ্যা count2)
{
	কাঠামো hashmap map;
	কাঠামো entry *first;
	পূর্ণসংখ্যা ফলাফল = 0;

	/* trivial case: one side is empty */
	যদি (!count1) {
		যখন(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ফেরত 0;
	} অন্যথা যদি (!count2) {
		যখন(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ফেরত 0;
	}

	memset(&map, 0, আকার(map));
	যদি (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		ফেরত -1;

	/* are there any matching lines at all? */
	যদি (!map.has_matches) {
		যখন(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		যখন(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		ফেরত 0;
	}

	ফলাফল = find_longest_common_sequence(&map, &first);
	যদি (ফলাফল)
		যাও out;
	যদি (first)
		ফলাফল = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	অন্যথা
		ফলাফল = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	ফেরত ফলাফল;
}

পূর্ণসংখ্যা xdl_do_patience_diff(xpparam_t ধ্রুবক *xpp, xdfenv_t *env)
{
	ফেরত patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
