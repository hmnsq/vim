/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LN_MAX_BUFS 8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LN_DECISION_MAX 255  // pow(2, LN_MAX_BUFS(8)) - 1 = 255

// struct for running the diff linematch algorithm
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ diffcmppath_S diffcmppath_T;
ਬਣਤਰ diffcmppath_S
{
    // to keep track of the total score of this path
    ਪੂਰਨ			df_lev_score;
    size_t		df_ਰਸਤਾ_n;	// current index of this path
    ਪੂਰਨ			df_choice_mem[LN_DECISION_MAX + 1];
    ਪੂਰਨ			df_choice[LN_DECISION_MAX];
    // to keep track of this path traveled
    diffcmppath_T	*df_decision[LN_DECISION_MAX];
    size_t		df_optimal_choice;
};

ਸਥਿਰ ਪੂਰਨ matching_chars(ਅਚਲ mmfile_t *m1, ਅਚਲ mmfile_t *m2);
ਸਥਿਰ size_t unwrap_indexes(ਅਚਲ ਪੂਰਨ *ਮੁੱਲਾਂ, ਅਚਲ ਪੂਰਨ *diff_len, ਅਚਲ size_t ndiffs);
ਸਥਿਰ size_t test_charmatch_paths(diffcmppath_T *ਗੰਢ, ਪੂਰਨ lastdecision);

    ਸਥਿਰ size_t
ਸਤਰ_len(ਅਚਲ mmfile_t *m)
{
    ਅੱਖਰ	*s = m->ptr;
    ਅੱਖਰ	*ਸਿਰਾ;

    ਸਿਰਾ = memchr(s, '\n', (size_t)m->ਮਾਪ);
    ਵਾਪਸ ਸਿਰਾ ? (size_t)(ਸਿਰਾ - s) : (size_t)m->ਮਾਪ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੇਲ_CHAR_MAX_LEN 800

/// Same as matching_chars but ignore whitespace
///
/// @param s1
/// @param s2
    ਸਥਿਰ ਪੂਰਨ
matching_chars_iwhite(ਅਚਲ mmfile_t *s1, ਅਚਲ mmfile_t *s2)
{
    // the newly processed strings that will be compared
    // delete the white space characters
    mmfile_t	sp[2];
    ਅੱਖਰ	p[2][ਮੇਲ_CHAR_MAX_LEN];

    ਲਈ (ਪੂਰਨ k = 0; k < 2; k++)
    {
	ਅਚਲ	mmfile_t *s = k == 0 ? s1 : s2;
	size_t	pi = 0;
	size_t	slen = MIN(ਮੇਲ_CHAR_MAX_LEN - 1, ਸਤਰ_len(s));

	ਲਈ (size_t i = 0; i <= slen; i++)
	{
	    ਅੱਖਰ e = s->ptr[i];

	    ਜੇ (e != ' ' && e != '\t')
	    {
		p[k][pi] = e;
		pi++;
	    }
	}

	sp[k].ptr = p[k];
	sp[k].ਮਾਪ = (ਪੂਰਨ)pi;
    }

    ਵਾਪਸ matching_chars(&sp[0], &sp[1]);
}

/// Return matching characters between "s1" and "s2" whilst respecting sequence
/// order.
/// Consider the case of two strings 'AAACCC' and 'CCCAAA', the
/// return value from this function will be 3, either to match
/// the 3 C's, or the 3 A's.
///
/// Examples:
///   matching_chars("aabc", "acba")               -> 2  // 'a' and 'b' in common
///   matching_chars("123hello567", "he123ll567o") -> 8  // '123', 'll' and '567' in common
///   matching_chars("abcdefg", "gfedcba")         -> 1  // all characters in common,
///                                                      // but only at most 1 in sequence
///
/// @param m1
/// @param m2
    ਸਥਿਰ ਪੂਰਨ
matching_chars(ਅਚਲ mmfile_t *m1, ਅਚਲ mmfile_t *m2)
{
    size_t	s1len = MIN(ਮੇਲ_CHAR_MAX_LEN - 1, ਸਤਰ_len(m1));
    size_t	s2len = MIN(ਮੇਲ_CHAR_MAX_LEN - 1, ਸਤਰ_len(m2));
    ਅੱਖਰ	*s1 = m1->ptr;
    ਅੱਖਰ	*s2 = m2->ptr;
    ਪੂਰਨ		ਮੈਟ੍ਰਿਕਸ[2][ਮੇਲ_CHAR_MAX_LEN] = { 0 };
    ਪੂਰਨ		icur = 1;  // save space by storing only two rows for i axis

    ਲਈ (size_t i = 0; i < s1len; i++)
    {
	icur = (icur == 1 ? 0 : 1);
	ਪੂਰਨ *e1 = ਮੈਟ੍ਰਿਕਸ[icur];
	ਪੂਰਨ *e2 = ਮੈਟ੍ਰਿਕਸ[!icur];

	ਲਈ (size_t j = 0; j < s2len; j++)
	{
	    // skip char in s1
	    ਜੇ (e2[j + 1] > e1[j + 1])
		e1[j + 1] = e2[j + 1];
	    // skip char in s2
	    ਜੇ (e1[j] > e1[j + 1])
		e1[j + 1] = e1[j];
	    // compare char in s1 and s2
	    ਜੇ ((s1[i] == s2[j]) && (e2[j] + 1) > e1[j + 1])
		e1[j + 1] = e2[j] + 1;
	}
    }

    ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ[icur][s2len];
}

/// count the matching characters between a variable number of strings "sp"
/// mark the strings that have already been compared to extract them later
/// without re-running the character match counting.
/// @param sp
/// @param fomvals
/// @param n
    ਸਥਿਰ ਪੂਰਨ
ਗਿਣਤੀ_n_matched_chars(mmfile_t **sp, ਅਚਲ size_t n, ਪੂਰਨ iwhite)
{
    ਪੂਰਨ matched_chars = 0;
    ਪੂਰਨ matched = 0;

    ਲਈ (size_t i = 0; i < n; i++)
    {
	ਲਈ (size_t j = i + 1; j < n; j++)
	{
	    ਜੇ (sp[i]->ptr != ਨਿਰਾ && sp[j]->ptr != ਨਿਰਾ)
	    {
		matched++;
		// TODO(lewis6991): handle whitespace ignoring higher up in the
		//ਢੇਰ
		matched_chars += iwhite ? matching_chars_iwhite(sp[i], sp[j])
					: matching_chars(sp[i], sp[j]);
	    }
	}
    }

    // prioritize a match of 3 (or more lines) equally to a match of 2 lines
    ਜੇ (matched >= 2)
    {
	matched_chars *= 2;
	matched_chars /= matched;
    }

    ਵਾਪਸ matched_chars;
}

    ਸਥਿਰ mmfile_t
fastforward_buf_ਮੰਜ਼ਿਲ_lnum(mmfile_t s, linenr_T lnum)
{
    ਲਈ (ਪੂਰਨ i = 0; i < lnum - 1; i++)
    {
	ਅੱਖਰ *ਸਤਰ_ਸਿਰਾ;

	ਸਤਰ_ਸਿਰਾ = memchr(s.ptr, '\n', (size_t)s.ਮਾਪ);
	s.ਮਾਪ = ਸਤਰ_ਸਿਰਾ ? (ਪੂਰਨ)(s.ਮਾਪ - (ਸਤਰ_ਸਿਰਾ - s.ptr)) : 0;
	s.ptr = ਸਤਰ_ਸਿਰਾ;
	ਜੇ (!s.ptr)
	    ਰੋਕੋ;
	s.ptr++;
	s.ਮਾਪ--;
    }

    ਵਾਪਸ s;
}

/// try all the different ways to compare these lines and use the one that
/// results in the most matching characters
/// @param df_iters
/// @param paths
/// @param npaths
/// @param path_idx
/// @param choice
/// @param diffcmppath
/// @param diff_len
/// @param ndiffs
/// @param diff_blk
    ਸਥਿਰ ਖਾਲੀ
try_possible_paths(
    ਅਚਲ ਪੂਰਨ		*df_iters,
    ਅਚਲ size_t	*paths,
    ਅਚਲ ਪੂਰਨ		npaths,
    ਅਚਲ ਪੂਰਨ		ਰਸਤਾ_idx,
    ਪੂਰਨ			*choice,
    diffcmppath_T	*diffcmppath,
    ਅਚਲ ਪੂਰਨ		*diff_len,
    ਅਚਲ size_t	ndiffs,
    ਅਚਲ mmfile_t	**diff_blk,
    ਪੂਰਨ			iwhite)
{
    ਜੇ (ਰਸਤਾ_idx == npaths)
    {
	ਜੇ ((*choice) > 0)
	{
	    ਪੂਰਨ from_vals[LN_MAX_BUFS] = { 0 };
	    ਅਚਲ ਪੂਰਨ *ਮੰਜ਼ਿਲ_vals = df_iters;

	    mmfile_t mm[LN_MAX_BUFS];  // stack memory for current_lines
	    mmfile_t *current_ਸਤਰਾਂ[LN_MAX_BUFS];
	    ਲਈ (size_t k = 0; k < ndiffs; k++)
	    {
		from_vals[k] = df_iters[k];
		// get the index at all of the places
		ਜੇ ((*choice) & (1 << k))
		{
		    from_vals[k]--;
		    mm[k] = fastforward_buf_ਮੰਜ਼ਿਲ_lnum(*diff_blk[k], df_iters[k]);
		}
		ਨਹੀਂਤਾਂ
		    CLEAR_ਖੇਤਰ(mm[k]);
		current_ਸਤਰਾਂ[k] = &mm[k];
	    }
	    size_t unwrapped_idx_from = unwrap_indexes(from_vals, diff_len, ndiffs);
	    size_t unwrapped_idx_ਮੰਜ਼ਿਲ = unwrap_indexes(ਮੰਜ਼ਿਲ_vals, diff_len, ndiffs);
	    ਪੂਰਨ matched_chars = ਗਿਣਤੀ_n_matched_chars(current_ਸਤਰਾਂ, ndiffs, iwhite);
	    ਪੂਰਨ score = diffcmppath[unwrapped_idx_from].df_lev_score + matched_chars;

	    ਜੇ (score > diffcmppath[unwrapped_idx_ਮੰਜ਼ਿਲ].df_lev_score)
	    {
		diffcmppath[unwrapped_idx_ਮੰਜ਼ਿਲ].df_ਰਸਤਾ_n = 1;
		diffcmppath[unwrapped_idx_ਮੰਜ਼ਿਲ].df_decision[0] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_ਮੰਜ਼ਿਲ].df_choice[0] = *choice;
		diffcmppath[unwrapped_idx_ਮੰਜ਼ਿਲ].df_lev_score = score;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (score == diffcmppath[unwrapped_idx_ਮੰਜ਼ਿਲ].df_lev_score)
	    {
		size_t k = diffcmppath[unwrapped_idx_ਮੰਜ਼ਿਲ].df_ਰਸਤਾ_n++;
		diffcmppath[unwrapped_idx_ਮੰਜ਼ਿਲ].df_decision[k] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_ਮੰਜ਼ਿਲ].df_choice[k] = *choice;
	    }
	}
	ਵਾਪਸ;
    }

    size_t ਦੋਆਧਾਰੀ_ਅੰਕ_ਰੱਖੋ = paths[ਰਸਤਾ_idx];
    *(choice) |= (1 << ਦੋਆਧਾਰੀ_ਅੰਕ_ਰੱਖੋ);  // set it to 1
    try_possible_paths(df_iters, paths, npaths, ਰਸਤਾ_idx + 1, choice,
	    diffcmppath, diff_len, ndiffs, diff_blk, iwhite);
    *(choice) &= ~(1 << ਦੋਆਧਾਰੀ_ਅੰਕ_ਰੱਖੋ);  // set it to 0
    try_possible_paths(df_iters, paths, npaths, ਰਸਤਾ_idx + 1, choice,
	    diffcmppath, diff_len, ndiffs, diff_blk, iwhite);
}

/// unwrap indexes to access n dimensional tensor
/// @param values
/// @param diff_len
/// @param ndiffs
    ਸਥਿਰ size_t
unwrap_indexes(ਅਚਲ ਪੂਰਨ *ਮੁੱਲਾਂ, ਅਚਲ ਪੂਰਨ *diff_len, ਅਚਲ size_t ndiffs)
{
    size_t ਅੰਕ_unwrap_scalar = 1;

    ਲਈ (size_t k = 0; k < ndiffs; k++)
	ਅੰਕ_unwrap_scalar *= (size_t)diff_len[k] + 1;

    size_t ਰਸਤਾ_idx = 0;
    ਲਈ (size_t k = 0; k < ndiffs; k++)
    {
	ਅੰਕ_unwrap_scalar /= (size_t)diff_len[k] + 1;

	ਪੂਰਨ n = ਮੁੱਲਾਂ[k];
	ਰਸਤਾ_idx += ਅੰਕ_unwrap_scalar * (size_t)n;
    }

    ਵਾਪਸ ਰਸਤਾ_idx;
}

/// populate the values of the linematch algorithm tensor, and find the best
/// decision for how to compare the relevant lines from each of the buffers at
/// each point in the tensor
/// @param df_iters
/// @param ch_dim
/// @param diffcmppath
/// @param diff_len
/// @param ndiffs
/// @param diff_blk
    ਸਥਿਰ ਖਾਲੀ
populate_tensor(
    ਪੂਰਨ			*df_iters,
    ਅਚਲ size_t	ch_dim,
    diffcmppath_T	*diffcmppath,
    ਅਚਲ ਪੂਰਨ		*diff_len,
    ਅਚਲ size_t	ndiffs,
    ਅਚਲ mmfile_t	**diff_blk,
    ਪੂਰਨ			iwhite)
{
    ਜੇ (ch_dim == ndiffs)
    {
	ਪੂਰਨ npaths = 0;
	size_t paths[LN_MAX_BUFS];

	ਲਈ (size_t j = 0; j < ndiffs; j++)
	{
	    ਜੇ (df_iters[j] > 0)
	    {
		paths[npaths] = j;
		npaths++;
	    }
	}

	ਪੂਰਨ choice = 0;
	size_t unwrapper_idx_ਮੰਜ਼ਿਲ = unwrap_indexes(df_iters, diff_len, ndiffs);

	diffcmppath[unwrapper_idx_ਮੰਜ਼ਿਲ].df_lev_score = -1;
	try_possible_paths(df_iters, paths, npaths, 0, &choice, diffcmppath,
					diff_len, ndiffs, diff_blk, iwhite);
	ਵਾਪਸ;
    }

    ਲਈ (ਪੂਰਨ i = 0; i <= diff_len[ch_dim]; i++)
    {
	df_iters[ch_dim] = i;
	populate_tensor(df_iters, ch_dim + 1, diffcmppath, diff_len,
		ndiffs, diff_blk, iwhite);
    }
}

/// algorithm to find an optimal alignment of lines of a diff block with 2 or
/// more files. The algorithm is generalized to work for any number of files
/// which corresponds to another dimension added to the tensor used in the
/// algorithm
///
/// for questions and information about the linematch algorithm please contact
/// Jonathon White (jonathonwhite@protonmail.com)
///
/// for explanation, a summary of the algorithm in 3 dimensions (3 files
///     compared) follows
///
/// The 3d case (for 3 buffers) of the algorithm implemented when diffopt
/// 'linematch' is enabled. The algorithm constructs a 3d tensor to
/// compare a diff between 3 buffers. The dimensions of the tensor are
/// the length of the diff in each buffer plus 1 A path is constructed by
/// moving from one edge of the cube/3d tensor to the opposite edge.
/// Motions from one cell of the cube to the next represent decisions. In
/// a 3d cube, there are a total of 7 decisions that can be made,
/// represented by the enum df_path3_choice which is defined in
/// buffer_defs.h a comparison of buffer 0 and 1 represents a motion
/// toward the opposite edge of the cube with components along the 0 and
/// 1 axes.  a comparison of buffer 0, 1, and 2 represents a motion
/// toward the opposite edge of the cube with components along the 0, 1,
/// and 2 axes. A skip of buffer 0 represents a motion along only the 0
/// axis. For each action, a point value is awarded, and the path is
/// saved for reference later, if it is found to have been the optimal
/// path. The optimal path has the highest score.  The score is
/// calculated as the summation of the total characters matching between
/// all of the lines which were compared. The structure of the algorithm
/// is that of a dynamic programming problem.  We can calculate a point
/// i,j,k in the cube as a function of i-1, j-1, and k-1. To find the
/// score and path at point i,j,k, we must determine which path we want
/// to use, this is done by looking at the possibilities and choosing
/// the one which results in the local highest score.  The total highest
/// scored path is, then in the end represented by the cell in the
/// opposite corner from the start location.  The entire algorithm
/// consists of populating the 3d cube with the optimal paths from which
/// it may have came.
///
/// Optimizations:
/// As the function to calculate the cell of a tensor at point i,j,k is a
/// function of the cells at i-1, j-1, k-1, the whole tensor doesn't need
/// to be stored in memory at once. In the case of the 3d cube, only two
/// slices (along k and j axis) are stored in memory. For the 2d matrix
/// (for 2 files), only two rows are stored at a time. The next/previous
/// slice (or row) is always calculated from the other, and they alternate
/// at each iteration.
/// In the 3d case, 3 arrays are populated to memorize the score (matched
/// characters) of the 3 buffers, so a redundant calculation of the
/// scores does not occur
/// @param diff_blk
/// @param diff_len
/// @param ndiffs
/// @param [out] [allocated] decisions
/// @return the length of decisions
    size_t
linematch_nbuffers(
    ਅਚਲ mmfile_t	**diff_blk,
    ਅਚਲ ਪੂਰਨ		*diff_len,
    ਅਚਲ size_t	ndiffs,
    ਪੂਰਨ			**decisions,
    ਪੂਰਨ			iwhite)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ndiffs <= LN_MAX_BUFS);

    size_t memsize = 1;
    size_t memsize_decisions = 0;
    ਲਈ (size_t i = 0; i < ndiffs; i++)
    {
	ਪ੍ਰਤਿਜ੍ਞਾ(diff_len[i] >= 0);
	memsize *= (size_t)(diff_len[i] + 1);
	memsize_decisions += (size_t)diff_len[i];
    }

    // create the flattened path matrix
    diffcmppath_T *diffcmppath = lalloc(ਆਕਾਰ(diffcmppath_T) * memsize, TRUE);
    ਜੇ (diffcmppath == ਨਿਰਾ)
	ਵਾਪਸ 0;

    // allocate memory here
    size_t n = (size_t)ਘਾਤ(2.0, (ਦੁੱਗਣਾ)ndiffs);
    ਲਈ (size_t i = 0; i < memsize; i++)
    {
	diffcmppath[i].df_lev_score = 0;
	diffcmppath[i].df_ਰਸਤਾ_n = 0;
	ਲਈ (size_t j = 0; j < n; j++)
	    diffcmppath[i].df_choice_mem[j] = -1;
    }

    // memory for avoiding repetitive calculations of score
    ਪੂਰਨ df_iters[LN_MAX_BUFS];
    populate_tensor(df_iters, 0, diffcmppath, diff_len, ndiffs, diff_blk,
		    iwhite);

    ਅਚਲ size_t u = unwrap_indexes(diff_len, diff_len, ndiffs);
    diffcmppath_T *ਸ਼ੁਰੂ_ਗੰਢ = &diffcmppath[u];

    *decisions = lalloc(ਆਕਾਰ(ਪੂਰਨ) * memsize_decisions, TRUE);
    ਜੇ (*decisions == ਨਿਰਾ)
    {
	vim_free(diffcmppath);
	ਵਾਪਸ 0;
    }

    size_t n_optimal = 0;
    test_charmatch_paths(ਸ਼ੁਰੂ_ਗੰਢ, 0);
    ਜਦੋਂ (ਸ਼ੁਰੂ_ਗੰਢ->df_ਰਸਤਾ_n > 0)
    {
	size_t j = ਸ਼ੁਰੂ_ਗੰਢ->df_optimal_choice;
	(*decisions)[n_optimal++] = ਸ਼ੁਰੂ_ਗੰਢ->df_choice[j];
	ਸ਼ੁਰੂ_ਗੰਢ = ਸ਼ੁਰੂ_ਗੰਢ->df_decision[j];
    }
    // reverse array
    ਲਈ (size_t i = 0; i < (n_optimal / 2); i++)
    {
	ਪੂਰਨ tmp = (*decisions)[i];
	(*decisions)[i] = (*decisions)[n_optimal - 1 - i];
	(*decisions)[n_optimal - 1 - i] = tmp;
    }

    vim_free(diffcmppath);

    ਵਾਪਸ n_optimal;
}

// returns the minimum amount of path changes from start to end
    ਸਥਿਰ size_t
test_charmatch_paths(diffcmppath_T *ਗੰਢ, ਪੂਰਨ lastdecision)
{
    // memoization
    ਜੇ (ਗੰਢ->df_choice_mem[lastdecision] == -1)
    {
	ਜੇ (ਗੰਢ->df_ਰਸਤਾ_n == 0)
	    // we have reached the end of the tree
	    ਗੰਢ->df_choice_mem[lastdecision] = 0;
	ਨਹੀਂਤਾਂ
	{
	    // the minimum amount of turns required to reach the end
	    size_t minimum_turns = ਮਾਪ_MAX;
	    ਲਈ (size_t i = 0; i < ਗੰਢ->df_ਰਸਤਾ_n; i++)
	    {
		// recurse
		size_t t = test_charmatch_paths(ਗੰਢ->df_decision[i],
							ਗੰਢ->df_choice[i]) +
		    (lastdecision != ਗੰਢ->df_choice[i] ? 1 : 0);
		ਜੇ (t < minimum_turns)
		{
		    ਗੰਢ->df_optimal_choice = i;
		    minimum_turns = t;
		}
	    }
	    ਗੰਢ->df_choice_mem[lastdecision] = (ਪੂਰਨ)minimum_turns;
	}
    }

    ਵਾਪਸ (size_t)ਗੰਢ->df_choice_mem[lastdecision];
}
