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

#شامل "xinclude.h"

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
ساختار hashmap {
	صحیح nr, alloc;
	ساختار entry {
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
		ساختار entry *next, *previous;

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

ایستا صحیح is_anchor(xpparam_t ثابت *xpp, ثابت نویسه *سطر)
{
	size_t i;
	برای (i = 0; i < xpp->anchors_nr; i++) {
		اگر (!strncmp(سطر, xpp->anchors[i], strlen(xpp->anchors[i])))
			بازگشت 1;
	}
	بازگشت 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
ایستا تهی درج_record(xpparam_t ثابت *xpp, صحیح سطر, ساختار hashmap *map,
			  صحیح pass)
{
	xrecord_t *records = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *record = &records[سطر - 1];
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
	صحیح index = (صحیح)((record->ha << 1) % map->alloc);

	وقتی (map->entries[index].line1) {
		اگر (map->entries[index].hash != record->ha) {
			اگر (++index >= map->alloc)
				index = 0;
			ادامه;
		}
		اگر (pass == 2)
			map->has_matches = 1;
		اگر (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		وگرنه
			map->entries[index].line2 = سطر;
		بازگشت;
	}
	اگر (pass == 2)
		بازگشت;
	map->entries[index].line1 = سطر;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[سطر - 1].ptr);
	اگر (!map->first)
		map->first = map->entries + index;
	اگر (map->last) {
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
ایستا صحیح fill_hashmap(xpparam_t ثابت *xpp, xdfenv_t *env,
		ساختار hashmap *result,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	اگر (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		بازگشت -1;

	/* First, fill with entries from the first file */
	وقتی (count1--)
		درج_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	وقتی (count2--)
		درج_record(xpp, line2++, result, 2);

	بازگشت 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
ایستا صحیح جستجوی_دودویی(ساختار entry **sequence, صحیح longest,
		ساختار entry *entry)
{
	صحیح چپ = -1, راست = longest;

	وقتی (چپ + 1 < راست) {
		صحیح میانه_2 = چپ + (راست - چپ) / 2;
		/* by construction, no two entries can be equal */
		اگر (sequence[میانه_2]->line2 > entry->line2)
			راست = میانه_2;
		وگرنه
			چپ = میانه_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	بازگشت چپ;
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
ایستا صحیح find_longest_common_sequence(ساختار hashmap *map, ساختار entry **res)
{
	ساختار entry **sequence;
	صحیح longest = 0, i;
	ساختار entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	صحیح anchor_i = -1;

	اگر (!XDL_ALLOC_ARRAY(sequence, map->nr))
		بازگشت -1;

	برای (entry = map->first; entry; entry = entry->next) {
		اگر (!entry->line2 || entry->line2 == NON_UNIQUE)
			ادامه;
		i = جستجوی_دودویی(sequence, longest, entry);
		entry->previous = i < 0 ? پوچ : sequence[i];
		++i;
		اگر (i <= anchor_i)
			ادامه;
		sequence[i] = entry;
		اگر (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} وگرنه اگر (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	اگر (!longest) {
		*res = پوچ;
		xdl_free(sequence);
		بازگشت 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = پوچ;
	وقتی (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	بازگشت 0;
}

ایستا صحیح match(ساختار hashmap *map, صحیح line1, صحیح line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	بازگشت record1->ha == record2->ha;
}

ایستا صحیح patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2);

ایستا صحیح walk_common_sequence(ساختار hashmap *map, ساختار entry *first,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	صحیح end1 = line1 + count1, end2 = line2 + count2;
	صحیح next1, next2;

	برای (;;) {
		/* Try to grow the line ranges of common lines */
		اگر (first) {
			next1 = first->line1;
			next2 = first->line2;
			وقتی (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} وگرنه {
			next1 = end1;
			next2 = end2;
		}
		وقتی (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		اگر (next1 > line1 || next2 > line2) {
			اگر (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				بازگشت -1;
		}

		اگر (!first)
			بازگشت 0;

		وقتی (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

ایستا صحیح fall_back_to_classic_diff(ساختار hashmap *map,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, اندازه(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	بازگشت xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
ایستا صحیح patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env,
		صحیح line1, صحیح count1, صحیح line2, صحیح count2)
{
	ساختار hashmap map;
	ساختار entry *first;
	صحیح result = 0;

	/* trivial case: one side is empty */
	اگر (!count1) {
		وقتی(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		بازگشت 0;
	} وگرنه اگر (!count2) {
		وقتی(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		بازگشت 0;
	}

	memset(&map, 0, اندازه(map));
	اگر (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		بازگشت -1;

	/* are there any matching lines at all? */
	اگر (!map.has_matches) {
		وقتی(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		وقتی(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		بازگشت 0;
	}

	result = find_longest_common_sequence(&map, &first);
	اگر (result)
		برو out;
	اگر (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	وگرنه
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	بازگشت result;
}

صحیح xdl_do_patience_diff(xpparam_t ثابت *xpp, xdfenv_t *env)
{
	بازگشت patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
