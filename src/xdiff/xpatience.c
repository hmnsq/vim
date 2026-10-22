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

#define NON_UNIQUE अच_दीर्घ_उच्च

/*
 * This is a hash mapping from line hash to line numbers in the first and
 * second file.
 */
संरचना hashmap {
	पूर्णांक nr, alloc;
	संरचना entry {
		अचिन्हित दीर्घ hash;
		/*
		 * 0 = unused entry, 1 = first line, 2 = second, etc.
		 * line2 is NON_UNIQUE if the line is not unique
		 * in either the first or the second file.
		 */
		अचिन्हित दीर्घ line1, line2;
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
		अचिन्हित anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	अचिन्हित दीर्घ has_matches;
	xdfenv_t *env;
	xpparam_t स्थिरांक *xpp;
};

स्थिर पूर्णांक is_anchor(xpparam_t स्थिरांक *xpp, स्थिरांक अक्षर *ओळ)
{
	माप_प्रकार i;
	साठी (i = 0; i < xpp->anchors_nr; i++) {
		जर (!म_भेदन(ओळ, xpp->anchors[i], म_माप(xpp->anchors[i])))
			परत 1;
	}
	परत 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
स्थिर शून्य घाला_record(xpparam_t स्थिरांक *xpp, पूर्णांक ओळ, संरचना hashmap *map,
			  पूर्णांक pass)
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
	पूर्णांक index = (पूर्णांक)((record->ha << 1) % map->alloc);

	पर्यंत (map->entries[index].line1) {
		जर (map->entries[index].hash != record->ha) {
			जर (++index >= map->alloc)
				index = 0;
			सुरूठेवा;
		}
		जर (pass == 2)
			map->has_matches = 1;
		जर (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		अन्यथा
			map->entries[index].line2 = ओळ;
		परत;
	}
	जर (pass == 2)
		परत;
	map->entries[index].line1 = ओळ;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[ओळ - 1].ptr);
	जर (!map->first)
		map->first = map->entries + index;
	जर (map->last) {
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
स्थिर पूर्णांक fill_hashmap(xpparam_t स्थिरांक *xpp, xdfenv_t *env,
		संरचना hashmap *result,
		पूर्णांक line1, पूर्णांक count1, पूर्णांक line2, पूर्णांक count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	जर (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		परत -1;

	/* First, fill with entries from the first file */
	पर्यंत (count1--)
		घाला_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	पर्यंत (count2--)
		घाला_record(xpp, line2++, result, 2);

	परत 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
स्थिर पूर्णांक द्विमान_शोध(संरचना entry **sequence, पूर्णांक longest,
		संरचना entry *entry)
{
	पूर्णांक डावा = -1, उजवा = longest;

	पर्यंत (डावा + 1 < उजवा) {
		पूर्णांक मधला_2 = डावा + (उजवा - डावा) / 2;
		/* by construction, no two entries can be equal */
		जर (sequence[मधला_2]->line2 > entry->line2)
			उजवा = मधला_2;
		अन्यथा
			डावा = मधला_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	परत डावा;
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
स्थिर पूर्णांक find_longest_common_sequence(संरचना hashmap *map, संरचना entry **res)
{
	संरचना entry **sequence;
	पूर्णांक longest = 0, i;
	संरचना entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	पूर्णांक anchor_i = -1;

	जर (!XDL_ALLOC_ARRAY(sequence, map->nr))
		परत -1;

	साठी (entry = map->first; entry; entry = entry->next) {
		जर (!entry->line2 || entry->line2 == NON_UNIQUE)
			सुरूठेवा;
		i = द्विमान_शोध(sequence, longest, entry);
		entry->previous = i < 0 ? रिक्त : sequence[i];
		++i;
		जर (i <= anchor_i)
			सुरूठेवा;
		sequence[i] = entry;
		जर (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} अन्यथा जर (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	जर (!longest) {
		*res = रिक्त;
		xdl_free(sequence);
		परत 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = रिक्त;
	पर्यंत (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	परत 0;
}

स्थिर पूर्णांक match(संरचना hashmap *map, पूर्णांक line1, पूर्णांक line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	परत record1->ha == record2->ha;
}

स्थिर पूर्णांक patience_diff(xpparam_t स्थिरांक *xpp, xdfenv_t *env,
		पूर्णांक line1, पूर्णांक count1, पूर्णांक line2, पूर्णांक count2);

स्थिर पूर्णांक walk_common_sequence(संरचना hashmap *map, संरचना entry *first,
		पूर्णांक line1, पूर्णांक count1, पूर्णांक line2, पूर्णांक count2)
{
	पूर्णांक end1 = line1 + count1, end2 = line2 + count2;
	पूर्णांक next1, next2;

	साठी (;;) {
		/* Try to grow the line ranges of common lines */
		जर (first) {
			next1 = first->line1;
			next2 = first->line2;
			पर्यंत (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} अन्यथा {
			next1 = end1;
			next2 = end2;
		}
		पर्यंत (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		जर (next1 > line1 || next2 > line2) {
			जर (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				परत -1;
		}

		जर (!first)
			परत 0;

		पर्यंत (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

स्थिर पूर्णांक fall_back_to_classic_diff(संरचना hashmap *map,
		पूर्णांक line1, पूर्णांक count1, पूर्णांक line2, पूर्णांक count2)
{
	xpparam_t xpp;

	स_रखो(&xpp, 0, आकार(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	परत xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
स्थिर पूर्णांक patience_diff(xpparam_t स्थिरांक *xpp, xdfenv_t *env,
		पूर्णांक line1, पूर्णांक count1, पूर्णांक line2, पूर्णांक count2)
{
	संरचना hashmap map;
	संरचना entry *first;
	पूर्णांक result = 0;

	/* trivial case: one side is empty */
	जर (!count1) {
		पर्यंत(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		परत 0;
	} अन्यथा जर (!count2) {
		पर्यंत(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		परत 0;
	}

	स_रखो(&map, 0, आकार(map));
	जर (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		परत -1;

	/* are there any matching lines at all? */
	जर (!map.has_matches) {
		पर्यंत(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		पर्यंत(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		परत 0;
	}

	result = find_longest_common_sequence(&map, &first);
	जर (result)
		जा out;
	जर (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	अन्यथा
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	परत result;
}

पूर्णांक xdl_do_patience_diff(xpparam_t स्थिरांक *xpp, xdfenv_t *env)
{
	परत patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
