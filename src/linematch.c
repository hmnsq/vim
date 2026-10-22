/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿಸು LN_MAX_BUFS 8
#ವ್ಯಾಖ್ಯಾನಿಸು LN_DECISION_MAX 255  // pow(2, LN_MAX_BUFS(8)) - 1 = 255

// struct for running the diff linematch algorithm
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ diffcmppath_S diffcmppath_T;
ರಚನೆ diffcmppath_S
{
    // to keep track of the total score of this path
    ಪೂರ್ಣಾಂಕ			df_lev_score;
    size_t		df_ಮಾರ್ಗ_n;	// current index of this path
    ಪೂರ್ಣಾಂಕ			df_choice_mem[LN_DECISION_MAX + 1];
    ಪೂರ್ಣಾಂಕ			df_choice[LN_DECISION_MAX];
    // to keep track of this path traveled
    diffcmppath_T	*df_decision[LN_DECISION_MAX];
    size_t		df_optimal_choice;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ matching_chars(ಸ್ಥಿರಾಂಕ mmfile_t *m1, ಸ್ಥಿರಾಂಕ mmfile_t *m2);
ಸ್ಥಿರ size_t unwrap_indexes(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *ಮೌಲ್ಯಗಳು, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *diff_len, ಸ್ಥಿರಾಂಕ size_t ndiffs);
ಸ್ಥಿರ size_t test_charmatch_paths(diffcmppath_T *ಗಂಟು, ಪೂರ್ಣಾಂಕ lastdecision);

    ಸ್ಥಿರ size_t
ಸಾಲು_len(ಸ್ಥಿರಾಂಕ mmfile_t *m)
{
    ಅಕ್ಷರ	*s = m->ptr;
    ಅಕ್ಷರ	*ಕೊನೆ;

    ಕೊನೆ = memchr(s, '\n', (size_t)m->ಗಾತ್ರ_2);
    ಹಿಂತಿರುಗಿ ಕೊನೆ ? (size_t)(ಕೊನೆ - s) : (size_t)m->ಗಾತ್ರ_2;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಹೊಂದಿಕೆ_CHAR_MAX_LEN 800

/// Same as matching_chars but ignore whitespace
///
/// @param s1
/// @param s2
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
matching_chars_iwhite(ಸ್ಥಿರಾಂಕ mmfile_t *s1, ಸ್ಥಿರಾಂಕ mmfile_t *s2)
{
    // the newly processed strings that will be compared
    // delete the white space characters
    mmfile_t	sp[2];
    ಅಕ್ಷರ	p[2][ಹೊಂದಿಕೆ_CHAR_MAX_LEN];

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < 2; k++)
    {
	ಸ್ಥಿರಾಂಕ	mmfile_t *s = k == 0 ? s1 : s2;
	size_t	pi = 0;
	size_t	slen = MIN(ಹೊಂದಿಕೆ_CHAR_MAX_LEN - 1, ಸಾಲು_len(s));

	ಗಾಗಿ (size_t i = 0; i <= slen; i++)
	{
	    ಅಕ್ಷರ e = s->ptr[i];

	    ಆದರೆ (e != ' ' && e != '\t')
	    {
		p[k][pi] = e;
		pi++;
	    }
	}

	sp[k].ptr = p[k];
	sp[k].ಗಾತ್ರ_2 = (ಪೂರ್ಣಾಂಕ)pi;
    }

    ಹಿಂತಿರುಗಿ matching_chars(&sp[0], &sp[1]);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
matching_chars(ಸ್ಥಿರಾಂಕ mmfile_t *m1, ಸ್ಥಿರಾಂಕ mmfile_t *m2)
{
    size_t	s1len = MIN(ಹೊಂದಿಕೆ_CHAR_MAX_LEN - 1, ಸಾಲು_len(m1));
    size_t	s2len = MIN(ಹೊಂದಿಕೆ_CHAR_MAX_LEN - 1, ಸಾಲು_len(m2));
    ಅಕ್ಷರ	*s1 = m1->ptr;
    ಅಕ್ಷರ	*s2 = m2->ptr;
    ಪೂರ್ಣಾಂಕ		ಮಾತೃಕೆ[2][ಹೊಂದಿಕೆ_CHAR_MAX_LEN] = { 0 };
    ಪೂರ್ಣಾಂಕ		icur = 1;  // save space by storing only two rows for i axis

    ಗಾಗಿ (size_t i = 0; i < s1len; i++)
    {
	icur = (icur == 1 ? 0 : 1);
	ಪೂರ್ಣಾಂಕ *e1 = ಮಾತೃಕೆ[icur];
	ಪೂರ್ಣಾಂಕ *e2 = ಮಾತೃಕೆ[!icur];

	ಗಾಗಿ (size_t j = 0; j < s2len; j++)
	{
	    // skip char in s1
	    ಆದರೆ (e2[j + 1] > e1[j + 1])
		e1[j + 1] = e2[j + 1];
	    // skip char in s2
	    ಆದರೆ (e1[j] > e1[j + 1])
		e1[j + 1] = e1[j];
	    // compare char in s1 and s2
	    ಆದರೆ ((s1[i] == s2[j]) && (e2[j] + 1) > e1[j + 1])
		e1[j + 1] = e2[j] + 1;
	}
    }

    ಹಿಂತಿರುಗಿ ಮಾತೃಕೆ[icur][s2len];
}

/// count the matching characters between a variable number of strings "sp"
/// mark the strings that have already been compared to extract them later
/// without re-running the character match counting.
/// @param sp
/// @param fomvals
/// @param n
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಎಣಿಕೆ_n_matched_chars(mmfile_t **sp, ಸ್ಥಿರಾಂಕ size_t n, ಪೂರ್ಣಾಂಕ iwhite)
{
    ಪೂರ್ಣಾಂಕ matched_chars = 0;
    ಪೂರ್ಣಾಂಕ matched = 0;

    ಗಾಗಿ (size_t i = 0; i < n; i++)
    {
	ಗಾಗಿ (size_t j = i + 1; j < n; j++)
	{
	    ಆದರೆ (sp[i]->ptr != ಖಾಲಿ && sp[j]->ptr != ಖಾಲಿ)
	    {
		matched++;
		// TODO(lewis6991): handle whitespace ignoring higher up in the
		//ರಾಶಿ
		matched_chars += iwhite ? matching_chars_iwhite(sp[i], sp[j])
					: matching_chars(sp[i], sp[j]);
	    }
	}
    }

    // prioritize a match of 3 (or more lines) equally to a match of 2 lines
    ಆದರೆ (matched >= 2)
    {
	matched_chars *= 2;
	matched_chars /= matched;
    }

    ಹಿಂತಿರುಗಿ matched_chars;
}

    ಸ್ಥಿರ mmfile_t
fastforward_buf_ಗಮ್ಯ_lnum(mmfile_t s, linenr_T lnum)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < lnum - 1; i++)
    {
	ಅಕ್ಷರ *ಸಾಲು_ಕೊನೆ;

	ಸಾಲು_ಕೊನೆ = memchr(s.ptr, '\n', (size_t)s.ಗಾತ್ರ_2);
	s.ಗಾತ್ರ_2 = ಸಾಲು_ಕೊನೆ ? (ಪೂರ್ಣಾಂಕ)(s.ಗಾತ್ರ_2 - (ಸಾಲು_ಕೊನೆ - s.ptr)) : 0;
	s.ptr = ಸಾಲು_ಕೊನೆ;
	ಆದರೆ (!s.ptr)
	    ನಿಲ್ಲಿಸು;
	s.ptr++;
	s.ಗಾತ್ರ_2--;
    }

    ಹಿಂತಿರುಗಿ s;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
try_possible_paths(
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ		*df_iters,
    ಸ್ಥಿರಾಂಕ size_t	*paths,
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ		npaths,
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ		ಮಾರ್ಗ_idx,
    ಪೂರ್ಣಾಂಕ			*choice,
    diffcmppath_T	*diffcmppath,
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ		*diff_len,
    ಸ್ಥಿರಾಂಕ size_t	ndiffs,
    ಸ್ಥಿರಾಂಕ mmfile_t	**diff_blk,
    ಪೂರ್ಣಾಂಕ			iwhite)
{
    ಆದರೆ (ಮಾರ್ಗ_idx == npaths)
    {
	ಆದರೆ ((*choice) > 0)
	{
	    ಪೂರ್ಣಾಂಕ from_vals[LN_MAX_BUFS] = { 0 };
	    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *ಗಮ್ಯ_vals = df_iters;

	    mmfile_t mm[LN_MAX_BUFS];  // stack memory for current_lines
	    mmfile_t *current_ಸಾಲುಗಳು[LN_MAX_BUFS];
	    ಗಾಗಿ (size_t k = 0; k < ndiffs; k++)
	    {
		from_vals[k] = df_iters[k];
		// get the index at all of the places
		ಆದರೆ ((*choice) & (1 << k))
		{
		    from_vals[k]--;
		    mm[k] = fastforward_buf_ಗಮ್ಯ_lnum(*diff_blk[k], df_iters[k]);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    CLEAR_ಕ್ಷೇತ್ರ(mm[k]);
		current_ಸಾಲುಗಳು[k] = &mm[k];
	    }
	    size_t unwrapped_idx_from = unwrap_indexes(from_vals, diff_len, ndiffs);
	    size_t unwrapped_idx_ಗಮ್ಯ = unwrap_indexes(ಗಮ್ಯ_vals, diff_len, ndiffs);
	    ಪೂರ್ಣಾಂಕ matched_chars = ಎಣಿಕೆ_n_matched_chars(current_ಸಾಲುಗಳು, ndiffs, iwhite);
	    ಪೂರ್ಣಾಂಕ score = diffcmppath[unwrapped_idx_from].df_lev_score + matched_chars;

	    ಆದರೆ (score > diffcmppath[unwrapped_idx_ಗಮ್ಯ].df_lev_score)
	    {
		diffcmppath[unwrapped_idx_ಗಮ್ಯ].df_ಮಾರ್ಗ_n = 1;
		diffcmppath[unwrapped_idx_ಗಮ್ಯ].df_decision[0] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_ಗಮ್ಯ].df_choice[0] = *choice;
		diffcmppath[unwrapped_idx_ಗಮ್ಯ].df_lev_score = score;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (score == diffcmppath[unwrapped_idx_ಗಮ್ಯ].df_lev_score)
	    {
		size_t k = diffcmppath[unwrapped_idx_ಗಮ್ಯ].df_ಮಾರ್ಗ_n++;
		diffcmppath[unwrapped_idx_ಗಮ್ಯ].df_decision[k] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_ಗಮ್ಯ].df_choice[k] = *choice;
	    }
	}
	ಹಿಂತಿರುಗಿ;
    }

    size_t ಬಿಟ್_ಇರಿಸು = paths[ಮಾರ್ಗ_idx];
    *(choice) |= (1 << ಬಿಟ್_ಇರಿಸು);  // set it to 1
    try_possible_paths(df_iters, paths, npaths, ಮಾರ್ಗ_idx + 1, choice,
	    diffcmppath, diff_len, ndiffs, diff_blk, iwhite);
    *(choice) &= ~(1 << ಬಿಟ್_ಇರಿಸು);  // set it to 0
    try_possible_paths(df_iters, paths, npaths, ಮಾರ್ಗ_idx + 1, choice,
	    diffcmppath, diff_len, ndiffs, diff_blk, iwhite);
}

/// unwrap indexes to access n dimensional tensor
/// @param values
/// @param diff_len
/// @param ndiffs
    ಸ್ಥಿರ size_t
unwrap_indexes(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *ಮೌಲ್ಯಗಳು, ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *diff_len, ಸ್ಥಿರಾಂಕ size_t ndiffs)
{
    size_t ಅಂಕೆ_unwrap_scalar = 1;

    ಗಾಗಿ (size_t k = 0; k < ndiffs; k++)
	ಅಂಕೆ_unwrap_scalar *= (size_t)diff_len[k] + 1;

    size_t ಮಾರ್ಗ_idx = 0;
    ಗಾಗಿ (size_t k = 0; k < ndiffs; k++)
    {
	ಅಂಕೆ_unwrap_scalar /= (size_t)diff_len[k] + 1;

	ಪೂರ್ಣಾಂಕ n = ಮೌಲ್ಯಗಳು[k];
	ಮಾರ್ಗ_idx += ಅಂಕೆ_unwrap_scalar * (size_t)n;
    }

    ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_idx;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
populate_tensor(
    ಪೂರ್ಣಾಂಕ			*df_iters,
    ಸ್ಥಿರಾಂಕ size_t	ch_dim,
    diffcmppath_T	*diffcmppath,
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ		*diff_len,
    ಸ್ಥಿರಾಂಕ size_t	ndiffs,
    ಸ್ಥಿರಾಂಕ mmfile_t	**diff_blk,
    ಪೂರ್ಣಾಂಕ			iwhite)
{
    ಆದರೆ (ch_dim == ndiffs)
    {
	ಪೂರ್ಣಾಂಕ npaths = 0;
	size_t paths[LN_MAX_BUFS];

	ಗಾಗಿ (size_t j = 0; j < ndiffs; j++)
	{
	    ಆದರೆ (df_iters[j] > 0)
	    {
		paths[npaths] = j;
		npaths++;
	    }
	}

	ಪೂರ್ಣಾಂಕ choice = 0;
	size_t unwrapper_idx_ಗಮ್ಯ = unwrap_indexes(df_iters, diff_len, ndiffs);

	diffcmppath[unwrapper_idx_ಗಮ್ಯ].df_lev_score = -1;
	try_possible_paths(df_iters, paths, npaths, 0, &choice, diffcmppath,
					diff_len, ndiffs, diff_blk, iwhite);
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i <= diff_len[ch_dim]; i++)
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
    ಸ್ಥಿರಾಂಕ mmfile_t	**diff_blk,
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ		*diff_len,
    ಸ್ಥಿರಾಂಕ size_t	ndiffs,
    ಪೂರ್ಣಾಂಕ			**decisions,
    ಪೂರ್ಣಾಂಕ			iwhite)
{
    ಪ್ರತಿಜ್ಞಾ(ndiffs <= LN_MAX_BUFS);

    size_t memsize = 1;
    size_t memsize_decisions = 0;
    ಗಾಗಿ (size_t i = 0; i < ndiffs; i++)
    {
	ಪ್ರತಿಜ್ಞಾ(diff_len[i] >= 0);
	memsize *= (size_t)(diff_len[i] + 1);
	memsize_decisions += (size_t)diff_len[i];
    }

    // create the flattened path matrix
    diffcmppath_T *diffcmppath = lalloc(ಗಾತ್ರ(diffcmppath_T) * memsize, TRUE);
    ಆದರೆ (diffcmppath == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    // allocate memory here
    size_t n = (size_t)ಘಾತ(2.0, (ದ್ವಿಗುಣ)ndiffs);
    ಗಾಗಿ (size_t i = 0; i < memsize; i++)
    {
	diffcmppath[i].df_lev_score = 0;
	diffcmppath[i].df_ಮಾರ್ಗ_n = 0;
	ಗಾಗಿ (size_t j = 0; j < n; j++)
	    diffcmppath[i].df_choice_mem[j] = -1;
    }

    // memory for avoiding repetitive calculations of score
    ಪೂರ್ಣಾಂಕ df_iters[LN_MAX_BUFS];
    populate_tensor(df_iters, 0, diffcmppath, diff_len, ndiffs, diff_blk,
		    iwhite);

    ಸ್ಥಿರಾಂಕ size_t u = unwrap_indexes(diff_len, diff_len, ndiffs);
    diffcmppath_T *ಆರಂಭ_ಗಂಟು = &diffcmppath[u];

    *decisions = lalloc(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * memsize_decisions, TRUE);
    ಆದರೆ (*decisions == ಖಾಲಿ)
    {
	vim_free(diffcmppath);
	ಹಿಂತಿರುಗಿ 0;
    }

    size_t n_optimal = 0;
    test_charmatch_paths(ಆರಂಭ_ಗಂಟು, 0);
    ಆಗ (ಆರಂಭ_ಗಂಟು->df_ಮಾರ್ಗ_n > 0)
    {
	size_t j = ಆರಂಭ_ಗಂಟು->df_optimal_choice;
	(*decisions)[n_optimal++] = ಆರಂಭ_ಗಂಟು->df_choice[j];
	ಆರಂಭ_ಗಂಟು = ಆರಂಭ_ಗಂಟು->df_decision[j];
    }
    // reverse array
    ಗಾಗಿ (size_t i = 0; i < (n_optimal / 2); i++)
    {
	ಪೂರ್ಣಾಂಕ tmp = (*decisions)[i];
	(*decisions)[i] = (*decisions)[n_optimal - 1 - i];
	(*decisions)[n_optimal - 1 - i] = tmp;
    }

    vim_free(diffcmppath);

    ಹಿಂತಿರುಗಿ n_optimal;
}

// returns the minimum amount of path changes from start to end
    ಸ್ಥಿರ size_t
test_charmatch_paths(diffcmppath_T *ಗಂಟು, ಪೂರ್ಣಾಂಕ lastdecision)
{
    // memoization
    ಆದರೆ (ಗಂಟು->df_choice_mem[lastdecision] == -1)
    {
	ಆದರೆ (ಗಂಟು->df_ಮಾರ್ಗ_n == 0)
	    // we have reached the end of the tree
	    ಗಂಟು->df_choice_mem[lastdecision] = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // the minimum amount of turns required to reach the end
	    size_t minimum_turns = ಗಾತ್ರ_MAX;
	    ಗಾಗಿ (size_t i = 0; i < ಗಂಟು->df_ಮಾರ್ಗ_n; i++)
	    {
		// recurse
		size_t t = test_charmatch_paths(ಗಂಟು->df_decision[i],
							ಗಂಟು->df_choice[i]) +
		    (lastdecision != ಗಂಟು->df_choice[i] ? 1 : 0);
		ಆದರೆ (t < minimum_turns)
		{
		    ಗಂಟು->df_optimal_choice = i;
		    minimum_turns = t;
		}
	    }
	    ಗಂಟು->df_choice_mem[lastdecision] = (ಪೂರ್ಣಾಂಕ)minimum_turns;
	}
    }

    ಹಿಂತಿರುಗಿ (size_t)ಗಂಟು->df_choice_mem[lastdecision];
}
