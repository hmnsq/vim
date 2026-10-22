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

#ਅਨ੍ਤਰ੍ਭਾਵ "xinclude.h"

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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NON_UNIQUE ULONG_MAX

/*
 * This is a hash mapping from line hash to line numbers in the first and
 * second file.
 */
ਬਣਤਰ hashmap {
	ਪੂਰਨ nr, alloc;
	ਬਣਤਰ ਇੰਦਰਾਜ {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ hash;
		/*
		 * 0 = unused entry, 1 = first line, 2 = second, etc.
		 * line2 is NON_UNIQUE if the line is not unique
		 * in either the first or the second file.
		 */
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ line1, line2;
		/*
		 * "next" & "previous" are used for the longest common
		 * sequence;
		 * initially, "next" reflects only the order in file1.
		 */
		ਬਣਤਰ ਇੰਦਰਾਜ *ਅਗਲਾ, *ਪਿਛਲਾ;

		/*
		 * If 1, this entry can serve as an anchor. See
		 * Documentation/diff-options.adoc for more information.
		 */
		ਬਿਨਾਨਿਸ਼ਾਨੀ anchor : 1;
	} *entries, *ਪਹਿਲਾ, *ਆਖਰੀ;
	/* were common records found? */
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ has_matches;
	xdfenv_t *env;
	xpparam_t ਅਚਲ *xpp;
};

ਸਥਿਰ ਪੂਰਨ is_anchor(xpparam_t ਅਚਲ *xpp, ਅਚਲ ਅੱਖਰ *ਸਤਰ)
{
	size_t i;
	ਲਈ (i = 0; i < xpp->anchors_nr; i++) {
		ਜੇ (!ਲੜੀ_ਤੁਲਨਾ(ਸਤਰ, xpp->anchors[i], ਲੜੀ_ਲੰਬਾਈ(xpp->anchors[i])))
			ਵਾਪਸ 1;
	}
	ਵਾਪਸ 0;
}

/* The argument "pass" is 1 for the first file, 2 for the second. */
ਸਥਿਰ ਖਾਲੀ ਪਾਓ_ਰਿਕਾਰਡ(xpparam_t ਅਚਲ *xpp, ਪੂਰਨ ਸਤਰ, ਬਣਤਰ hashmap *map,
			  ਪੂਰਨ pass)
{
	xrecord_t *ਰਿਕਾਰਡਾਂ = pass == 1 ?
		map->env->xdf1.recs : map->env->xdf2.recs;
	xrecord_t *ਰਿਕਾਰਡ = &ਰਿਕਾਰਡਾਂ[ਸਤਰ - 1];
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
	ਪੂਰਨ ਸੂਚਕਾਂਕ = (ਪੂਰਨ)((ਰਿਕਾਰਡ->ha << 1) % map->alloc);

	ਜਦੋਂ (map->entries[ਸੂਚਕਾਂਕ].line1) {
		ਜੇ (map->entries[ਸੂਚਕਾਂਕ].hash != ਰਿਕਾਰਡ->ha) {
			ਜੇ (++ਸੂਚਕਾਂਕ >= map->alloc)
				ਸੂਚਕਾਂਕ = 0;
			ਜਾਰੀ;
		}
		ਜੇ (pass == 2)
			map->has_matches = 1;
		ਜੇ (pass == 1 || map->entries[ਸੂਚਕਾਂਕ].line2)
			map->entries[ਸੂਚਕਾਂਕ].line2 = NON_UNIQUE;
		ਨਹੀਂਤਾਂ
			map->entries[ਸੂਚਕਾਂਕ].line2 = ਸਤਰ;
		ਵਾਪਸ;
	}
	ਜੇ (pass == 2)
		ਵਾਪਸ;
	map->entries[ਸੂਚਕਾਂਕ].line1 = ਸਤਰ;
	map->entries[ਸੂਚਕਾਂਕ].hash = ਰਿਕਾਰਡ->ha;
	map->entries[ਸੂਚਕਾਂਕ].anchor = is_anchor(xpp, map->env->xdf1.recs[ਸਤਰ - 1].ptr);
	ਜੇ (!map->ਪਹਿਲਾ)
		map->ਪਹਿਲਾ = map->entries + ਸੂਚਕਾਂਕ;
	ਜੇ (map->ਆਖਰੀ) {
		map->ਆਖਰੀ->ਅਗਲਾ = map->entries + ਸੂਚਕਾਂਕ;
		map->entries[ਸੂਚਕਾਂਕ].ਪਿਛਲਾ = map->ਆਖਰੀ;
	}
	map->ਆਖਰੀ = map->entries + ਸੂਚਕਾਂਕ;
	map->nr++;
}

/*
 * This function has to be called for each recursion into the inter-hunk
 * parts, as previously non-unique lines can become unique when being
 * restricted to a smaller part of the files.
 *
 * It is assumed that env has been prepared using xdl_prepare().
 */
ਸਥਿਰ ਪੂਰਨ fill_hashmap(xpparam_t ਅਚਲ *xpp, xdfenv_t *env,
		ਬਣਤਰ hashmap *ਨਤੀਜਾ,
		ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2)
{
	ਨਤੀਜਾ->xpp = xpp;
	ਨਤੀਜਾ->env = env;

	/* We know exactly how large we want the hash map */
	ਨਤੀਜਾ->alloc = count1 * 2;
	ਜੇ (!XDL_CALLOC_ARRAY(ਨਤੀਜਾ->entries, ਨਤੀਜਾ->alloc))
		ਵਾਪਸ -1;

	/* First, fill with entries from the first file */
	ਜਦੋਂ (count1--)
		ਪਾਓ_ਰਿਕਾਰਡ(xpp, line1++, ਨਤੀਜਾ, 1);

	/* Then search for matches in the second file */
	ਜਦੋਂ (count2--)
		ਪਾਓ_ਰਿਕਾਰਡ(xpp, line2++, ਨਤੀਜਾ, 2);

	ਵਾਪਸ 0;
}

/*
 * Find the longest sequence with a smaller last element (meaning a smaller
 * line2, as we construct the sequence with entries ordered by line1).
 */
ਸਥਿਰ ਪੂਰਨ ਦੁਆਧਾਰੀ_ਖੋਜ(ਬਣਤਰ ਇੰਦਰਾਜ **sequence, ਪੂਰਨ longest,
		ਬਣਤਰ ਇੰਦਰਾਜ *ਇੰਦਰਾਜ)
{
	ਪੂਰਨ ਖੱਬਾ = -1, ਸੱਜਾ = longest;

	ਜਦੋਂ (ਖੱਬਾ + 1 < ਸੱਜਾ) {
		ਪੂਰਨ ਵਿਚਕਾਰ_2 = ਖੱਬਾ + (ਸੱਜਾ - ਖੱਬਾ) / 2;
		/* by construction, no two entries can be equal */
		ਜੇ (sequence[ਵਿਚਕਾਰ_2]->line2 > ਇੰਦਰਾਜ->line2)
			ਸੱਜਾ = ਵਿਚਕਾਰ_2;
		ਨਹੀਂਤਾਂ
			ਖੱਬਾ = ਵਿਚਕਾਰ_2;
	}
	/* return the index in "sequence", _not_ the sequence length */
	ਵਾਪਸ ਖੱਬਾ;
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
ਸਥਿਰ ਪੂਰਨ ਲੱਭੋ_longest_common_sequence(ਬਣਤਰ hashmap *map, ਬਣਤਰ ਇੰਦਰਾਜ **res)
{
	ਬਣਤਰ ਇੰਦਰਾਜ **sequence;
	ਪੂਰਨ longest = 0, i;
	ਬਣਤਰ ਇੰਦਰਾਜ *ਇੰਦਰਾਜ;

	/*
	 * If not -1, this entry in sequence must never be overridden.
	 * Therefore, overriding entries before this has no effect, so
	 * do not do that either.
	 */
	ਪੂਰਨ anchor_i = -1;

	ਜੇ (!XDL_ALLOC_ARRAY(sequence, map->nr))
		ਵਾਪਸ -1;

	ਲਈ (ਇੰਦਰਾਜ = map->ਪਹਿਲਾ; ਇੰਦਰਾਜ; ਇੰਦਰਾਜ = ਇੰਦਰਾਜ->ਅਗਲਾ) {
		ਜੇ (!ਇੰਦਰਾਜ->line2 || ਇੰਦਰਾਜ->line2 == NON_UNIQUE)
			ਜਾਰੀ;
		i = ਦੁਆਧਾਰੀ_ਖੋਜ(sequence, longest, ਇੰਦਰਾਜ);
		ਇੰਦਰਾਜ->ਪਿਛਲਾ = i < 0 ? ਨਿਰਾ : sequence[i];
		++i;
		ਜੇ (i <= anchor_i)
			ਜਾਰੀ;
		sequence[i] = ਇੰਦਰਾਜ;
		ਜੇ (ਇੰਦਰਾਜ->anchor) {
			anchor_i = i;
			longest = anchor_i + 1;
		} ਨਹੀਂਤਾਂ ਜੇ (i == longest) {
			longest++;
		}
	}

	/* No common unique lines were found */
	ਜੇ (!longest) {
		*res = ਨਿਰਾ;
		xdl_free(sequence);
		ਵਾਪਸ 0;
	}

	/* Iterate starting at the last element, adjusting the "next" members */
	ਇੰਦਰਾਜ = sequence[longest - 1];
	ਇੰਦਰਾਜ->ਅਗਲਾ = ਨਿਰਾ;
	ਜਦੋਂ (ਇੰਦਰਾਜ->ਪਿਛਲਾ) {
		ਇੰਦਰਾਜ->ਪਿਛਲਾ->ਅਗਲਾ = ਇੰਦਰਾਜ;
		ਇੰਦਰਾਜ = ਇੰਦਰਾਜ->ਪਿਛਲਾ;
	}
	*res = ਇੰਦਰਾਜ;
	xdl_free(sequence);
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ ਮੇਲ(ਬਣਤਰ hashmap *map, ਪੂਰਨ line1, ਪੂਰਨ line2)
{
	xrecord_t *record1 = &map->env->xdf1.recs[line1 - 1];
	xrecord_t *record2 = &map->env->xdf2.recs[line2 - 1];
	ਵਾਪਸ record1->ha == record2->ha;
}

ਸਥਿਰ ਪੂਰਨ patience_diff(xpparam_t ਅਚਲ *xpp, xdfenv_t *env,
		ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2);

ਸਥਿਰ ਪੂਰਨ walk_common_sequence(ਬਣਤਰ hashmap *map, ਬਣਤਰ ਇੰਦਰਾਜ *ਪਹਿਲਾ,
		ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2)
{
	ਪੂਰਨ end1 = line1 + count1, end2 = line2 + count2;
	ਪੂਰਨ next1, next2;

	ਲਈ (;;) {
		/* Try to grow the line ranges of common lines */
		ਜੇ (ਪਹਿਲਾ) {
			next1 = ਪਹਿਲਾ->line1;
			next2 = ਪਹਿਲਾ->line2;
			ਜਦੋਂ (next1 > line1 && next2 > line2 &&
					ਮੇਲ(map, next1 - 1, next2 - 1)) {
				next1--;
				next2--;
			}
		} ਨਹੀਂਤਾਂ {
			next1 = end1;
			next2 = end2;
		}
		ਜਦੋਂ (line1 < next1 && line2 < next2 &&
				ਮੇਲ(map, line1, line2)) {
			line1++;
			line2++;
		}

		/* Recurse */
		ਜੇ (next1 > line1 || next2 > line2) {
			ਜੇ (patience_diff(map->xpp, map->env,
					line1, next1 - line1,
					line2, next2 - line2))
				ਵਾਪਸ -1;
		}

		ਜੇ (!ਪਹਿਲਾ)
			ਵਾਪਸ 0;

		ਜਦੋਂ (ਪਹਿਲਾ->ਅਗਲਾ &&
				ਪਹਿਲਾ->ਅਗਲਾ->line1 == ਪਹਿਲਾ->line1 + 1 &&
				ਪਹਿਲਾ->ਅਗਲਾ->line2 == ਪਹਿਲਾ->line2 + 1)
			ਪਹਿਲਾ = ਪਹਿਲਾ->ਅਗਲਾ;

		line1 = ਪਹਿਲਾ->line1 + 1;
		line2 = ਪਹਿਲਾ->line2 + 1;

		ਪਹਿਲਾ = ਪਹਿਲਾ->ਅਗਲਾ;
	}
}

ਸਥਿਰ ਪੂਰਨ fall_ਪਿੱਠ_ਮੰਜ਼ਿਲ_classic_diff(ਬਣਤਰ hashmap *map,
		ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2)
{
	xpparam_t xpp;

	memset(&xpp, 0, ਆਕਾਰ(xpp));
	xpp.flags = map->xpp->flags & ~XDF_DIFF_ALGORITHM_ਨਕਾਬ;

	ਵਾਪਸ xdl_fall_ਪਿੱਠ_diff(map->env, &xpp,
				  line1, count1, line2, count2);
}

/*
 * Recursively find the longest common sequence of unique lines,
 * and if none was found, ask xdl_do_diff() to do the job.
 *
 * This function assumes that env was prepared with xdl_prepare_env().
 */
ਸਥਿਰ ਪੂਰਨ patience_diff(xpparam_t ਅਚਲ *xpp, xdfenv_t *env,
		ਪੂਰਨ line1, ਪੂਰਨ count1, ਪੂਰਨ line2, ਪੂਰਨ count2)
{
	ਬਣਤਰ hashmap map;
	ਬਣਤਰ ਇੰਦਰਾਜ *ਪਹਿਲਾ;
	ਪੂਰਨ ਨਤੀਜਾ = 0;

	/* trivial case: one side is empty */
	ਜੇ (!count1) {
		ਜਦੋਂ(count2--)
			env->xdf2.ਬਦਲਿਆ[line2++ - 1] = true;
		ਵਾਪਸ 0;
	} ਨਹੀਂਤਾਂ ਜੇ (!count2) {
		ਜਦੋਂ(count1--)
			env->xdf1.ਬਦਲਿਆ[line1++ - 1] = true;
		ਵਾਪਸ 0;
	}

	memset(&map, 0, ਆਕਾਰ(map));
	ਜੇ (fill_hashmap(xpp, env, &map,
			line1, count1, line2, count2))
		ਵਾਪਸ -1;

	/* are there any matching lines at all? */
	ਜੇ (!map.has_matches) {
		ਜਦੋਂ(count1--)
			env->xdf1.ਬਦਲਿਆ[line1++ - 1] = true;
		ਜਦੋਂ(count2--)
			env->xdf2.ਬਦਲਿਆ[line2++ - 1] = true;
		xdl_free(map.entries);
		ਵਾਪਸ 0;
	}

	ਨਤੀਜਾ = ਲੱਭੋ_longest_common_sequence(&map, &ਪਹਿਲਾ);
	ਜੇ (ਨਤੀਜਾ)
		ਜਾਓ ਬਾਹਰਲਾ;
	ਜੇ (ਪਹਿਲਾ)
		ਨਤੀਜਾ = walk_common_sequence(&map, ਪਹਿਲਾ,
			line1, count1, line2, count2);
	ਨਹੀਂਤਾਂ
		ਨਤੀਜਾ = fall_ਪਿੱਠ_ਮੰਜ਼ਿਲ_classic_diff(&map,
			line1, count1, line2, count2);
 ਬਾਹਰਲਾ:
	xdl_free(map.entries);
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ xdl_do_patience_diff(xpparam_t ਅਚਲ *xpp, xdfenv_t *env)
{
	ਵਾਪਸ patience_diff(xpp, env, 1, env->xdf1.nrec, 1, env->xdf2.nrec);
}
