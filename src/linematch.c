/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#அந்தர்பாவ "vim.h"

#define LN_MAX_BUFS 8
#define LN_DECISION_MAX 255  // pow(2, LN_MAX_BUFS(8)) - 1 = 255

// struct for running the diff linematch algorithm
வகைவரை கட்டமைப்பு diffcmppath_S diffcmppath_T;
கட்டமைப்பு diffcmppath_S
{
    // to keep track of the total score of this path
    முழு			df_lev_score;
    size_t		df_path_n;	// current index of this path
    முழு			df_choice_mem[LN_DECISION_MAX + 1];
    முழு			df_choice[LN_DECISION_MAX];
    // to keep track of this path traveled
    diffcmppath_T	*df_decision[LN_DECISION_MAX];
    size_t		df_optimal_choice;
};

நிலையான முழு matching_chars(மாறிலி mmfile_t *m1, மாறிலி mmfile_t *m2);
நிலையான size_t unwrap_indexes(மாறிலி முழு *மதிப்புகள், மாறிலி முழு *diff_len, மாறிலி size_t ndiffs);
நிலையான size_t test_charmatch_paths(diffcmppath_T *முனை, முழு lastdecision);

    நிலையான size_t
வரி_len(மாறிலி mmfile_t *m)
{
    எழுத்து	*s = m->ptr;
    எழுத்து	*end;

    end = memchr(s, '\n', (size_t)m->size);
    திரும்பு end ? (size_t)(end - s) : (size_t)m->size;
}

#define MATCH_CHAR_MAX_LEN 800

/// Same as matching_chars but ignore whitespace
///
/// @param s1
/// @param s2
    நிலையான முழு
matching_chars_iwhite(மாறிலி mmfile_t *s1, மாறிலி mmfile_t *s2)
{
    // the newly processed strings that will be compared
    // delete the white space characters
    mmfile_t	sp[2];
    எழுத்து	p[2][MATCH_CHAR_MAX_LEN];

    க்கு (முழு k = 0; k < 2; k++)
    {
	மாறிலி	mmfile_t *s = k == 0 ? s1 : s2;
	size_t	pi = 0;
	size_t	slen = MIN(MATCH_CHAR_MAX_LEN - 1, வரி_len(s));

	க்கு (size_t i = 0; i <= slen; i++)
	{
	    எழுத்து e = s->ptr[i];

	    எனில் (e != ' ' && e != '\t')
	    {
		p[k][pi] = e;
		pi++;
	    }
	}

	sp[k].ptr = p[k];
	sp[k].size = (முழு)pi;
    }

    திரும்பு matching_chars(&sp[0], &sp[1]);
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
    நிலையான முழு
matching_chars(மாறிலி mmfile_t *m1, மாறிலி mmfile_t *m2)
{
    size_t	s1len = MIN(MATCH_CHAR_MAX_LEN - 1, வரி_len(m1));
    size_t	s2len = MIN(MATCH_CHAR_MAX_LEN - 1, வரி_len(m2));
    எழுத்து	*s1 = m1->ptr;
    எழுத்து	*s2 = m2->ptr;
    முழு		matrix[2][MATCH_CHAR_MAX_LEN] = { 0 };
    முழு		icur = 1;  // save space by storing only two rows for i axis

    க்கு (size_t i = 0; i < s1len; i++)
    {
	icur = (icur == 1 ? 0 : 1);
	முழு *e1 = matrix[icur];
	முழு *e2 = matrix[!icur];

	க்கு (size_t j = 0; j < s2len; j++)
	{
	    // skip char in s1
	    எனில் (e2[j + 1] > e1[j + 1])
		e1[j + 1] = e2[j + 1];
	    // skip char in s2
	    எனில் (e1[j] > e1[j + 1])
		e1[j + 1] = e1[j];
	    // compare char in s1 and s2
	    எனில் ((s1[i] == s2[j]) && (e2[j] + 1) > e1[j + 1])
		e1[j + 1] = e2[j] + 1;
	}
    }

    திரும்பு matrix[icur][s2len];
}

/// count the matching characters between a variable number of strings "sp"
/// mark the strings that have already been compared to extract them later
/// without re-running the character match counting.
/// @param sp
/// @param fomvals
/// @param n
    நிலையான முழு
மொத்தம்_n_matched_chars(mmfile_t **sp, மாறிலி size_t n, முழு iwhite)
{
    முழு matched_chars = 0;
    முழு matched = 0;

    க்கு (size_t i = 0; i < n; i++)
    {
	க்கு (size_t j = i + 1; j < n; j++)
	{
	    எனில் (sp[i]->ptr != வெற்று && sp[j]->ptr != வெற்று)
	    {
		matched++;
		// TODO(lewis6991): handle whitespace ignoring higher up in the
		//அடுக்கு
		matched_chars += iwhite ? matching_chars_iwhite(sp[i], sp[j])
					: matching_chars(sp[i], sp[j]);
	    }
	}
    }

    // prioritize a match of 3 (or more lines) equally to a match of 2 lines
    எனில் (matched >= 2)
    {
	matched_chars *= 2;
	matched_chars /= matched;
    }

    திரும்பு matched_chars;
}

    நிலையான mmfile_t
fastforward_buf_to_lnum(mmfile_t s, linenr_T lnum)
{
    க்கு (முழு i = 0; i < lnum - 1; i++)
    {
	எழுத்து *வரி_end;

	வரி_end = memchr(s.ptr, '\n', (size_t)s.size);
	s.size = வரி_end ? (முழு)(s.size - (வரி_end - s.ptr)) : 0;
	s.ptr = வரி_end;
	எனில் (!s.ptr)
	    நிறுத்து;
	s.ptr++;
	s.size--;
    }

    திரும்பு s;
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
    நிலையான வெறுமை
try_possible_paths(
    மாறிலி முழு		*df_iters,
    மாறிலி size_t	*paths,
    மாறிலி முழு		npaths,
    மாறிலி முழு		path_idx,
    முழு			*choice,
    diffcmppath_T	*diffcmppath,
    மாறிலி முழு		*diff_len,
    மாறிலி size_t	ndiffs,
    மாறிலி mmfile_t	**diff_blk,
    முழு			iwhite)
{
    எனில் (path_idx == npaths)
    {
	எனில் ((*choice) > 0)
	{
	    முழு from_vals[LN_MAX_BUFS] = { 0 };
	    மாறிலி முழு *to_vals = df_iters;

	    mmfile_t mm[LN_MAX_BUFS];  // stack memory for current_lines
	    mmfile_t *current_வரிகள்[LN_MAX_BUFS];
	    க்கு (size_t k = 0; k < ndiffs; k++)
	    {
		from_vals[k] = df_iters[k];
		// get the index at all of the places
		எனில் ((*choice) & (1 << k))
		{
		    from_vals[k]--;
		    mm[k] = fastforward_buf_to_lnum(*diff_blk[k], df_iters[k]);
		}
		இல்லையெனில்
		    CLEAR_FIELD(mm[k]);
		current_வரிகள்[k] = &mm[k];
	    }
	    size_t unwrapped_idx_from = unwrap_indexes(from_vals, diff_len, ndiffs);
	    size_t unwrapped_idx_to = unwrap_indexes(to_vals, diff_len, ndiffs);
	    முழு matched_chars = மொத்தம்_n_matched_chars(current_வரிகள், ndiffs, iwhite);
	    முழு score = diffcmppath[unwrapped_idx_from].df_lev_score + matched_chars;

	    எனில் (score > diffcmppath[unwrapped_idx_to].df_lev_score)
	    {
		diffcmppath[unwrapped_idx_to].df_path_n = 1;
		diffcmppath[unwrapped_idx_to].df_decision[0] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_to].df_choice[0] = *choice;
		diffcmppath[unwrapped_idx_to].df_lev_score = score;
	    }
	    இல்லையெனில் எனில் (score == diffcmppath[unwrapped_idx_to].df_lev_score)
	    {
		size_t k = diffcmppath[unwrapped_idx_to].df_path_n++;
		diffcmppath[unwrapped_idx_to].df_decision[k] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_to].df_choice[k] = *choice;
	    }
	}
	திரும்பு;
    }

    size_t bit_place = paths[path_idx];
    *(choice) |= (1 << bit_place);  // set it to 1
    try_possible_paths(df_iters, paths, npaths, path_idx + 1, choice,
	    diffcmppath, diff_len, ndiffs, diff_blk, iwhite);
    *(choice) &= ~(1 << bit_place);  // set it to 0
    try_possible_paths(df_iters, paths, npaths, path_idx + 1, choice,
	    diffcmppath, diff_len, ndiffs, diff_blk, iwhite);
}

/// unwrap indexes to access n dimensional tensor
/// @param values
/// @param diff_len
/// @param ndiffs
    நிலையான size_t
unwrap_indexes(மாறிலி முழு *மதிப்புகள், மாறிலி முழு *diff_len, மாறிலி size_t ndiffs)
{
    size_t இலக்கம்_unwrap_scalar = 1;

    க்கு (size_t k = 0; k < ndiffs; k++)
	இலக்கம்_unwrap_scalar *= (size_t)diff_len[k] + 1;

    size_t path_idx = 0;
    க்கு (size_t k = 0; k < ndiffs; k++)
    {
	இலக்கம்_unwrap_scalar /= (size_t)diff_len[k] + 1;

	முழு n = மதிப்புகள்[k];
	path_idx += இலக்கம்_unwrap_scalar * (size_t)n;
    }

    திரும்பு path_idx;
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
    நிலையான வெறுமை
populate_tensor(
    முழு			*df_iters,
    மாறிலி size_t	ch_dim,
    diffcmppath_T	*diffcmppath,
    மாறிலி முழு		*diff_len,
    மாறிலி size_t	ndiffs,
    மாறிலி mmfile_t	**diff_blk,
    முழு			iwhite)
{
    எனில் (ch_dim == ndiffs)
    {
	முழு npaths = 0;
	size_t paths[LN_MAX_BUFS];

	க்கு (size_t j = 0; j < ndiffs; j++)
	{
	    எனில் (df_iters[j] > 0)
	    {
		paths[npaths] = j;
		npaths++;
	    }
	}

	முழு choice = 0;
	size_t unwrapper_idx_to = unwrap_indexes(df_iters, diff_len, ndiffs);

	diffcmppath[unwrapper_idx_to].df_lev_score = -1;
	try_possible_paths(df_iters, paths, npaths, 0, &choice, diffcmppath,
					diff_len, ndiffs, diff_blk, iwhite);
	திரும்பு;
    }

    க்கு (முழு i = 0; i <= diff_len[ch_dim]; i++)
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
    மாறிலி mmfile_t	**diff_blk,
    மாறிலி முழு		*diff_len,
    மாறிலி size_t	ndiffs,
    முழு			**decisions,
    முழு			iwhite)
{
    ப்ரதிஜ்ஞா(ndiffs <= LN_MAX_BUFS);

    size_t memsize = 1;
    size_t memsize_decisions = 0;
    க்கு (size_t i = 0; i < ndiffs; i++)
    {
	ப்ரதிஜ்ஞா(diff_len[i] >= 0);
	memsize *= (size_t)(diff_len[i] + 1);
	memsize_decisions += (size_t)diff_len[i];
    }

    // create the flattened path matrix
    diffcmppath_T *diffcmppath = lalloc(அளவு(diffcmppath_T) * memsize, TRUE);
    எனில் (diffcmppath == வெற்று)
	திரும்பு 0;

    // allocate memory here
    size_t n = (size_t)pow(2.0, (இரட்டை)ndiffs);
    க்கு (size_t i = 0; i < memsize; i++)
    {
	diffcmppath[i].df_lev_score = 0;
	diffcmppath[i].df_path_n = 0;
	க்கு (size_t j = 0; j < n; j++)
	    diffcmppath[i].df_choice_mem[j] = -1;
    }

    // memory for avoiding repetitive calculations of score
    முழு df_iters[LN_MAX_BUFS];
    populate_tensor(df_iters, 0, diffcmppath, diff_len, ndiffs, diff_blk,
		    iwhite);

    மாறிலி size_t u = unwrap_indexes(diff_len, diff_len, ndiffs);
    diffcmppath_T *start_முனை = &diffcmppath[u];

    *decisions = lalloc(அளவு(முழு) * memsize_decisions, TRUE);
    எனில் (*decisions == வெற்று)
    {
	vim_free(diffcmppath);
	திரும்பு 0;
    }

    size_t n_optimal = 0;
    test_charmatch_paths(start_முனை, 0);
    போது (start_முனை->df_path_n > 0)
    {
	size_t j = start_முனை->df_optimal_choice;
	(*decisions)[n_optimal++] = start_முனை->df_choice[j];
	start_முனை = start_முனை->df_decision[j];
    }
    // reverse array
    க்கு (size_t i = 0; i < (n_optimal / 2); i++)
    {
	முழு tmp = (*decisions)[i];
	(*decisions)[i] = (*decisions)[n_optimal - 1 - i];
	(*decisions)[n_optimal - 1 - i] = tmp;
    }

    vim_free(diffcmppath);

    திரும்பு n_optimal;
}

// returns the minimum amount of path changes from start to end
    நிலையான size_t
test_charmatch_paths(diffcmppath_T *முனை, முழு lastdecision)
{
    // memoization
    எனில் (முனை->df_choice_mem[lastdecision] == -1)
    {
	எனில் (முனை->df_path_n == 0)
	    // we have reached the end of the tree
	    முனை->df_choice_mem[lastdecision] = 0;
	இல்லையெனில்
	{
	    // the minimum amount of turns required to reach the end
	    size_t minimum_turns = SIZE_MAX;
	    க்கு (size_t i = 0; i < முனை->df_path_n; i++)
	    {
		// recurse
		size_t t = test_charmatch_paths(முனை->df_decision[i],
							முனை->df_choice[i]) +
		    (lastdecision != முனை->df_choice[i] ? 1 : 0);
		எனில் (t < minimum_turns)
		{
		    முனை->df_optimal_choice = i;
		    minimum_turns = t;
		}
	    }
	    முனை->df_choice_mem[lastdecision] = (முழு)minimum_turns;
	}
    }

    திரும்பு (size_t)முனை->df_choice_mem[lastdecision];
}
