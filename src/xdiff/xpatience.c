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

#ଅନ୍ତର୍ଭାଵ "xinclude.h"

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
ସଂରଚନା hashmap {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, alloc;
	ସଂରଚନା entry {
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
		ସଂରଚନା entry *next, *previous;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		unsigned anchor : 1;
	} *entries, *first, *last;
	/* were common records found? */
	unsigned long has_matches;
	xdfenv_t *env;
	xpparam_t ଅଚଳ *xpp;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_anchor(xpparam_t ଅଚଳ *xpp, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି)
{
	size_t i;
	ପାଇଁ (i = 0; i < xpp->anchors_nr; i++) {
		ଯଦି (!strncmp(ଧାଡ଼ି, xpp->anchors[i], strlen(xpp->anchors[i])))
			ଫେରନ୍ତୁ 1;
	}
	ଫେରନ୍ତୁ 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
ସ୍ଥିର ଶୂନ୍ୟ ଭର୍ତ୍ତି_record(xpparam_t ଅଚଳ *xpp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ସଂରଚନା hashmap *map,
			  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pass)
{
	xrecord_t *records = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *record = &records[ଧାଡ଼ି - 1];
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((record->ha << 1) % map->alloc);

	ଯେତେବେଳେ (map->entries[index].line1) {
		ଯଦି (map->entries[index].hash != record->ha) {
			ଯଦି (++index >= map->alloc)
				index = 0;
			ଜାରି;
		}
		ଯଦି (pass == 2)
			map->has_matches = 1;
		ଯଦି (pass == 1 || map->entries[index].line2)
			map->entries[index].line2 = NON_UNIQUE;
		ଅନ୍ୟଥା
			map->entries[index].line2 = ଧାଡ଼ି;
		ଫେରନ୍ତୁ;
	}
	ଯଦି (pass == 2)
		ଫେରନ୍ତୁ;
	map->entries[index].line1 = ଧାଡ଼ି;
	map->entries[index].hash = record->ha;
	map->entries[index].anchor = is_anchor(xpp, map->env->xdf1.recs[ଧାଡ଼ି - 1].ptr);
	ଯଦି (!map->first)
		map->first = map->entries + index;
	ଯଦି (map->last) {
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fill_hashmap(xpparam_t ଅଚଳ *xpp, xdfenv_t *env,
		ସଂରଚନା hashmap *result,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2)
{
	result->xpp = xpp;
	result->env = env;

	/* We know exactly how large we want the hash map */
	result->alloc = count1 * 2;
	ଯଦି (!XDL_CALLOC_ARRAY(result->entries, result->alloc))
		ଫେରନ୍ତୁ -1;

	/* First, fill with entries from the first file */
	ଯେତେବେଳେ (count1--)
		ଭର୍ତ୍ତି_record(xpp, line1++, result, 1);

	/* Then search for matches in the second file */
	ଯେତେବେଳେ (count2--)
		ଭର୍ତ୍ତି_record(xpp, line2++, result, 2);

	ଫେରନ୍ତୁ 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଦ୍ୱିମିକ_ସନ୍ଧାନ(ସଂରଚନା entry **sequence, ପୂର୍ଣ୍ଣସଂଖ୍ୟା longest,
		ସଂରଚନା entry *entry)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ = -1, ଡାହାଣ = longest;

	ଯେତେବେଳେ (ବାମ + 1 < ଡାହାଣ) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମଧ୍ୟ_2 = ବାମ + (ଡାହାଣ - ବାମ) / 2;
		/* by construction, no two entries can be equal */
		ଯଦି (sequence[ମଧ୍ୟ_2]->line2 > entry->line2)
			ଡାହାଣ = ମଧ୍ୟ_2;
		ଅନ୍ୟଥା
			ବାମ = ମଧ୍ୟ_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	ଫେରନ୍ତୁ ବାମ;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_longest_common_sequence(ସଂରଚନା hashmap *map, ସଂରଚନା entry **res)
{
	ସଂରଚନା entry **sequence;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା longest = 0, i;
	ସଂରଚନା entry *entry;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା anchor_i = -1;

	ଯଦି (!XDL_ALLOC_ARRAY(sequence, map->nr))
		ଫେରନ୍ତୁ -1;

	ପାଇଁ (entry = map->first; entry; entry = entry->next) {
		ଯଦି (!entry->line2 || entry->line2 == NON_UNIQUE)
			ଜାରି;
		i = ଦ୍ୱିମିକ_ସନ୍ଧାନ(sequence, longest, entry);
		entry->previous = i < 0 ? ଖାଲି : sequence[i];
		++i;
		ଯଦି (i <= anchor_i)
			ଜାରି;
		sequence[i] = entry;
		ଯଦି (entry->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} ଅନ୍ୟଥା ଯଦି (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	ଯଦି (!longest) {
		*res = ଖାଲି;
		xdl_free(sequence);
		ଫେରନ୍ତୁ 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	entry = sequence[longest - 1];
	entry->next = ଖାଲି;
	ଯେତେବେଳେ (entry->previous) {
		entry->previous->next = entry;
		entry = entry->previous;
	}
	*res = entry;
	xdl_free(sequence);
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା match(ସଂରଚନା hashmap *map, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	ଫେରନ୍ତୁ record1->ha == record2->ha;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା patience_diff(xpparam_t ଅଚଳ *xpp, xdfenv_t *env,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା walk_common_sequence(ସଂରଚନା hashmap *map, ସଂରଚନା entry *first,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା end1 = line1 + count1, end2 = line2 + count2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା next1, next2;

	ପାଇଁ (;;) {
		/* Try to grow the line ranges of common lines */
		ଯଦି (first) {
			next1 = first->line1;
			next2 = first->line2;
			ଯେତେବେଳେ (next1 > line1 && next2 > line2 &&
					match(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} ଅନ୍ୟଥା {
			next1 = end1;
			next2 = end2;
		}
		ଯେତେବେଳେ (line1 < next1 && line2 < next2 &&
				match(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		ଯଦି (next1 > line1 || next2 > line2) {
			ଯଦି (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				ଫେରନ୍ତୁ -1;
		}

		ଯଦି (!first)
			ଫେରନ୍ତୁ 0;

		ଯେତେବେଳେ (first->next &&
				first->next->line1 == first->line1 + 1 &&
				first->next->line2 == first->line2 + 1)
			first = first->next;

		line1 = first->line1 + 1;
		line2 = first->line2 + 1;

		first = first->next;
	}
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fall_back_to_classic_diff(ସଂରଚନା hashmap *map,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, ଆକାର(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_MASK;

	ଫେରନ୍ତୁ xdl_fall_back_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା patience_diff(xpparam_t ଅଚଳ *xpp, xdfenv_t *env,
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା count2)
{
	ସଂରଚନା hashmap map;
	ସଂରଚନା entry *first;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = 0;

	/* trivial case: one side is empty */
	ଯଦି (!count1) {
		ଯେତେବେଳେ(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		ଫେରନ୍ତୁ 0;
	} ଅନ୍ୟଥା ଯଦି (!count2) {
		ଯେତେବେଳେ(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ଫେରନ୍ତୁ 0;
	}

	memset(&map, 0, ଆକାର(map));
	ଯଦି (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		ଫେରନ୍ତୁ -1;

	/* are there any matching lines at all? */
	ଯଦି (!map.has_matches) {
		ଯେତେବେଳେ(count1--)
			env->xdf1.changed[line1++ - 1] = true;
		ଯେତେବେଳେ(count2--)
			env->xdf2.changed[line2++ - 1] = true;
		xdl_free(map.entries);
		ଫେରନ୍ତୁ 0;
	}

	result = find_longest_common_sequence(&map, &first);
	ଯଦି (result)
		ଯାଅ out;
	ଯଦି (first)
		result = walk_common_sequence(&map, first,
			line1, count1, line2, count2);
	ଅନ୍ୟଥା
		result = fall_back_to_classic_diff(&map,
			line1, count1, line2, count2);
 out:
	xdl_free(map.entries);
	ଫେରନ୍ତୁ result;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xdl_do_patience_diff(xpparam_t ଅଚଳ *xpp, xdfenv_t *env)
{
	ଫେରନ୍ତୁ patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
