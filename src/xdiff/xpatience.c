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

#अन्तर्भाव "xinclude.h"

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
संरचना hashmap {
	पूर्णाङ्क nr, alloc;
	संरचना entry {
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
		संरचना entry *next, *previous;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		unsigned anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	unsigned long has_matches;
	xdfenv_t *env;
	xpparam_t ध्रुव *xpp;
};

स्थिर पूर्णाङ्क is_anchor(xpparam_t ध्रुव *xpp, ध्रुव वर्ण *ओळ)
{
	size_t i;
	प्रति (i = 0; i < xpp->anchors_nr; i++) {
		यदि (!strncmp(ओळ, xpp->anchors[i], strlen(xpp->anchors[i])))
			प्रत्यागम 1;
	}
	प्रत्यागम 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
स्थिर शून्य घाला_record(xpparam_t ध्रुव *xpp, पूर्णाङ्क ओळ, संरचना hashmap *map,
			  पूर्णाङ्क pass)
{
	xrecord_t *records = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *record = &records[ओळ - 1];
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
	पूर्णाङ्क index = (पूर्णाङ्क)((record->ha << 1) % map->alloc);

	यावत् (map->entries[index].line1) {
		यदि (map->entries[index].hash != record->ha) {
			यदि (++index >= map->alloc)
				index = 0;
			अनुवर्त;
		}
		यदि (pass == 2)
			map->has_matches = 1;
		यदि (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		अन्यथा
			map->entries[index].line2 = ओळ;
		प्रत्यागम;
	}
	यदि (pass == 2)
		प्रत्यागम;
	map->entries[index].line1 = ओळ;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[ओळ - 1].ptr);
	यदि (!map->first)
		map->first = map->entries + index;
	यदि (map->last) {
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
स्थिर पूर्णाङ्क fill_hashmap(xpparam_t ध्रुव *xpp, xdfenv_t *env,
		संरचना hashmap *result,
		पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	यदि (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		प्रत्यागम -1;

	/* First, fill with entries from the first file */
	यावत् (count1--)
		घाला_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	यावत् (count2--)
		घाला_record(xpp, line2++, result, 2);

	प्रत्यागम 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
स्थिर पूर्णाङ्क द्विमान_शोध(संरचना entry **sequence, पूर्णाङ्क longest,
		संरचना entry *entry)
{
	पूर्णाङ्क डावा = -1, उजवा = longest;

	यावत् (डावा + 1 < उजवा) {
		पूर्णाङ्क मधला_2 = डावा + (उजवा - डावा) / 2;
		/* by construction, no two entries can be equal */
		यदि (sequence[मधला_2]->line2 > entry->line2)
			उजवा = मधला_2;
		अन्यथा
			डावा = मधला_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	प्रत्यागम डावा;
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
स्थिर पूर्णाङ्क find_longest_common_sequence(संरचना hashmap *map, संरचना entry **res)
{
	संरचना entry **sequence;
	पूर्णाङ्क longest = 0, i;
	संरचना entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	पूर्णाङ्क anchor_i = -1;

	यदि (!XDL_ALLOC_ARRAY(sequence, map->nr))
		प्रत्यागम -1;

	प्रति (entry = map->first; entry; entry = entry->next) {
		यदि (!entry->line2 || entry->line2 == NON_UNIQUE)
			अनुवर्त;
		i = द्विमान_शोध(sequence, longest, entry);
		entry->previous = i < 0 ? रिक्त : sequence[i];
		++i;
		यदि (i <= anchor_i)
			अनुवर्त;
		sequence[i] = entry;
		यदि (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} अन्यथा यदि (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	यदि (!longest) {
		*res = रिक्त;
		xdl_free(sequence);
		प्रत्यागम 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = रिक्त;
	यावत् (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क match(संरचना hashmap *map, पूर्णाङ्क line1, पूर्णाङ्क line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	प्रत्यागम record1->ha == record2->ha;
}

स्थिर पूर्णाङ्क patience_diff(xpparam_t ध्रुव *xpp, xdfenv_t *env,
		पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2);

स्थिर पूर्णाङ्क walk_common_sequence(संरचना hashmap *map, संरचना entry *first,
		पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2)
{
	पूर्णाङ्क end1 = line1 + count1, end2 = line2 + count2;
	पूर्णाङ्क next1, next2;

	प्रति (;;) {
		/* Try to grow the line ranges of common lines */
		यदि (first) {
			next1 = first->line1;
			next2 = first->line2;
			यावत् (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} अन्यथा {
			next1 = end1;
			next2 = end2;
		}
		यावत् (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		यदि (next1 > line1 || next2 > line2) {
			यदि (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				प्रत्यागम -1;
		}

		यदि (!first)
			प्रत्यागम 0;

		यावत् (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

स्थिर पूर्णाङ्क fall_back_to_classic_diff(संरचना hashmap *map,
		पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, परिमाण(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	प्रत्यागम xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
स्थिर पूर्णाङ्क patience_diff(xpparam_t ध्रुव *xpp, xdfenv_t *env,
		पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2)
{
	संरचना hashmap map;
	संरचना entry *first;
	पूर्णाङ्क result = 0;

	/* trivial case: one side is empty */
	यदि (!count1) {
		यावत्(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		प्रत्यागम 0;
	} अन्यथा यदि (!count2) {
		यावत्(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		प्रत्यागम 0;
	}

	memset(&map, 0, परिमाण(map));
	यदि (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		प्रत्यागम -1;

	/* are there any matching lines at all? */
	यदि (!map.has_matches) {
		यावत्(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		यावत्(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		प्रत्यागम 0;
	}

	result = find_longest_common_sequence(&map, &first);
	यदि (result)
		गच्छ out;
	यदि (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	अन्यथा
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	प्रत्यागम result;
}

पूर्णाङ्क xdl_do_patience_diff(xpparam_t ध्रुव *xpp, xdfenv_t *env)
{
	प्रत्यागम patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
