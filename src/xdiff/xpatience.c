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

स्थिर पूर्णाङ्क is_anchor(xpparam_t ध्रुव *xpp, ध्रुव वर्ण *पङ्क्तिः)
{
	size_t i;
	अत्थाय (i = 0; i < xpp->anchors_nr; i++) {
		सचे (!strncmp(पङ्क्तिः, xpp->anchors[i], strlen(xpp->anchors[i])))
			पटिनिवत्त 1;
	}
	पटिनिवत्त 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
स्थिर शून्य निवेशय_record(xpparam_t ध्रुव *xpp, पूर्णाङ्क पङ्क्तिः, संरचना hashmap *map,
			  पूर्णाङ्क pass)
{
	xrecord_t *records = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *record = &records[पङ्क्तिः - 1];
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

	याव (map->entries[index].line1) {
		सचे (map->entries[index].hash != record->ha) {
			सचे (++index >= map->alloc)
				index = 0;
			अनुवर्त;
		}
		सचे (pass == 2)
			map->has_matches = 1;
		सचे (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		अञ्ञथा
			map->entries[index].line2 = पङ्क्तिः;
		पटिनिवत्त;
	}
	सचे (pass == 2)
		पटिनिवत्त;
	map->entries[index].line1 = पङ्क्तिः;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[पङ्क्तिः - 1].ptr);
	सचे (!map->first)
		map->first = map->entries + index;
	सचे (map->last) {
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
	सचे (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		पटिनिवत्त -1;

	/* First, fill with entries from the first file */
	याव (count1--)
		निवेशय_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	याव (count2--)
		निवेशय_record(xpp, line2++, result, 2);

	पटिनिवत्त 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
स्थिर पूर्णाङ्क द्विमान_अन्वेषणम्(संरचना entry **sequence, पूर्णाङ्क longest,
		संरचना entry *entry)
{
	पूर्णाङ्क वामः = -1, दक्षिणः = longest;

	याव (वामः + 1 < दक्षिणः) {
		पूर्णाङ्क मध्यः_2 = वामः + (दक्षिणः - वामः) / 2;
		/* by construction, no two entries can be equal */
		सचे (sequence[मध्यः_2]->line2 > entry->line2)
			दक्षिणः = मध्यः_2;
		अञ्ञथा
			वामः = मध्यः_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	पटिनिवत्त वामः;
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

	सचे (!XDL_ALLOC_ARRAY(sequence, map->nr))
		पटिनिवत्त -1;

	अत्थाय (entry = map->first; entry; entry = entry->next) {
		सचे (!entry->line2 || entry->line2 == NON_UNIQUE)
			अनुवर्त;
		i = द्विमान_अन्वेषणम्(sequence, longest, entry);
		entry->previous = i < 0 ? रिक्त : sequence[i];
		++i;
		सचे (i <= anchor_i)
			अनुवर्त;
		sequence[i] = entry;
		सचे (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} अञ्ञथा सचे (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	सचे (!longest) {
		*res = रिक्त;
		xdl_free(sequence);
		पटिनिवत्त 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = रिक्त;
	याव (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क match(संरचना hashmap *map, पूर्णाङ्क line1, पूर्णाङ्क line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	पटिनिवत्त record1->ha == record2->ha;
}

स्थिर पूर्णाङ्क patience_diff(xpparam_t ध्रुव *xpp, xdfenv_t *env,
		पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2);

स्थिर पूर्णाङ्क walk_common_sequence(संरचना hashmap *map, संरचना entry *first,
		पूर्णाङ्क line1, पूर्णाङ्क count1, पूर्णाङ्क line2, पूर्णाङ्क count2)
{
	पूर्णाङ्क end1 = line1 + count1, end2 = line2 + count2;
	पूर्णाङ्क next1, next2;

	अत्थाय (;;) {
		/* Try to grow the line ranges of common lines */
		सचे (first) {
			next1 = first->line1;
			next2 = first->line2;
			याव (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} अञ्ञथा {
			next1 = end1;
			next2 = end2;
		}
		याव (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		सचे (next1 > line1 || next2 > line2) {
			सचे (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				पटिनिवत्त -1;
		}

		सचे (!first)
			पटिनिवत्त 0;

		याव (first->next &&
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

	पटिनिवत्त xdl_fall_back_diff(map->env, &xpp,
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
	सचे (!count1) {
		याव(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		पटिनिवत्त 0;
	} अञ्ञथा सचे (!count2) {
		याव(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		पटिनिवत्त 0;
	}

	memset(&map, 0, परिमाण(map));
	सचे (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		पटिनिवत्त -1;

	/* are there any matching lines at all? */
	सचे (!map.has_matches) {
		याव(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		याव(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		पटिनिवत्त 0;
	}

	result = find_longest_common_sequence(&map, &first);
	सचे (result)
		गच्छ out;
	सचे (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	अञ्ञथा
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	पटिनिवत्त result;
}

पूर्णाङ्क xdl_do_patience_diff(xpparam_t ध्रुव *xpp, xdfenv_t *env)
{
	पटिनिवत्त patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
