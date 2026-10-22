/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#അന്തര്ഭാവ "vim.h"

#define LN_MAX_BUFS 8
#define LN_DECISION_MAX 255  // pow(2, LN_MAX_BUFS(8)) - 1 = 255

// struct for running the diff linematch algorithm
തരനിർവചനം ഘടന diffcmppath_S diffcmppath_T;
ഘടന diffcmppath_S
{
    // to keep track of the total score of this path
    പൂർണ്ണസംഖ്യ			df_lev_score;
    size_t		df_path_n;	// current index of this path
    പൂർണ്ണസംഖ്യ			df_choice_mem[LN_DECISION_MAX + 1];
    പൂർണ്ണസംഖ്യ			df_choice[LN_DECISION_MAX];
    // to keep track of this path traveled
    diffcmppath_T	*df_decision[LN_DECISION_MAX];
    size_t		df_optimal_choice;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ matching_chars(സ്ഥിരം mmfile_t *m1, സ്ഥിരം mmfile_t *m2);
സ്ഥിതം size_t unwrap_indexes(സ്ഥിരം പൂർണ്ണസംഖ്യ *മൂല്യങ്ങൾ, സ്ഥിരം പൂർണ്ണസംഖ്യ *diff_len, സ്ഥിരം size_t ndiffs);
സ്ഥിതം size_t test_charmatch_paths(diffcmppath_T *കണ്ണി, പൂർണ്ണസംഖ്യ lastdecision);

    സ്ഥിതം size_t
വരി_len(സ്ഥിരം mmfile_t *m)
{
    അക്ഷരം	*s = m->ptr;
    അക്ഷരം	*end;

    end = memchr(s, '\n', (size_t)m->size);
    തിരികെ end ? (size_t)(end - s) : (size_t)m->size;
}

#define MATCH_CHAR_MAX_LEN 800

/// Same as matching_chars but ignore whitespace
///
/// @param s1
/// @param s2
    സ്ഥിതം പൂർണ്ണസംഖ്യ
matching_chars_iwhite(സ്ഥിരം mmfile_t *s1, സ്ഥിരം mmfile_t *s2)
{
    // the newly processed strings that will be compared
    // delete the white space characters
    mmfile_t	sp[2];
    അക്ഷരം	p[2][MATCH_CHAR_MAX_LEN];

    വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < 2; k++)
    {
	സ്ഥിരം	mmfile_t *s = k == 0 ? s1 : s2;
	size_t	pi = 0;
	size_t	slen = MIN(MATCH_CHAR_MAX_LEN - 1, വരി_len(s));

	വേണ്ടി (size_t i = 0; i <= slen; i++)
	{
	    അക്ഷരം e = s->ptr[i];

	    എങ്കിൽ (e != ' ' && e != '\t')
	    {
		p[k][pi] = e;
		pi++;
	    }
	}

	sp[k].ptr = p[k];
	sp[k].size = (പൂർണ്ണസംഖ്യ)pi;
    }

    തിരികെ matching_chars(&sp[0], &sp[1]);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
matching_chars(സ്ഥിരം mmfile_t *m1, സ്ഥിരം mmfile_t *m2)
{
    size_t	s1len = MIN(MATCH_CHAR_MAX_LEN - 1, വരി_len(m1));
    size_t	s2len = MIN(MATCH_CHAR_MAX_LEN - 1, വരി_len(m2));
    അക്ഷരം	*s1 = m1->ptr;
    അക്ഷരം	*s2 = m2->ptr;
    പൂർണ്ണസംഖ്യ		matrix[2][MATCH_CHAR_MAX_LEN] = { 0 };
    പൂർണ്ണസംഖ്യ		icur = 1;  // save space by storing only two rows for i axis

    വേണ്ടി (size_t i = 0; i < s1len; i++)
    {
	icur = (icur == 1 ? 0 : 1);
	പൂർണ്ണസംഖ്യ *e1 = matrix[icur];
	പൂർണ്ണസംഖ്യ *e2 = matrix[!icur];

	വേണ്ടി (size_t j = 0; j < s2len; j++)
	{
	    // skip char in s1
	    എങ്കിൽ (e2[j + 1] > e1[j + 1])
		e1[j + 1] = e2[j + 1];
	    // skip char in s2
	    എങ്കിൽ (e1[j] > e1[j + 1])
		e1[j + 1] = e1[j];
	    // compare char in s1 and s2
	    എങ്കിൽ ((s1[i] == s2[j]) && (e2[j] + 1) > e1[j + 1])
		e1[j + 1] = e2[j] + 1;
	}
    }

    തിരികെ matrix[icur][s2len];
}

/// count the matching characters between a variable number of strings "sp"
/// mark the strings that have already been compared to extract them later
/// without re-running the character match counting.
/// @param sp
/// @param fomvals
/// @param n
    സ്ഥിതം പൂർണ്ണസംഖ്യ
എണ്ണം_n_matched_chars(mmfile_t **sp, സ്ഥിരം size_t n, പൂർണ്ണസംഖ്യ iwhite)
{
    പൂർണ്ണസംഖ്യ matched_chars = 0;
    പൂർണ്ണസംഖ്യ matched = 0;

    വേണ്ടി (size_t i = 0; i < n; i++)
    {
	വേണ്ടി (size_t j = i + 1; j < n; j++)
	{
	    എങ്കിൽ (sp[i]->ptr != ശൂന്യ && sp[j]->ptr != ശൂന്യ)
	    {
		matched++;
		// TODO(lewis6991): handle whitespace ignoring higher up in the
		//അട്ടി
		matched_chars += iwhite ? matching_chars_iwhite(sp[i], sp[j])
					: matching_chars(sp[i], sp[j]);
	    }
	}
    }

    // prioritize a match of 3 (or more lines) equally to a match of 2 lines
    എങ്കിൽ (matched >= 2)
    {
	matched_chars *= 2;
	matched_chars /= matched;
    }

    തിരികെ matched_chars;
}

    സ്ഥിതം mmfile_t
fastforward_buf_to_lnum(mmfile_t s, linenr_T lnum)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < lnum - 1; i++)
    {
	അക്ഷരം *വരി_end;

	വരി_end = memchr(s.ptr, '\n', (size_t)s.size);
	s.size = വരി_end ? (പൂർണ്ണസംഖ്യ)(s.size - (വരി_end - s.ptr)) : 0;
	s.ptr = വരി_end;
	എങ്കിൽ (!s.ptr)
	    നിർത്തുക;
	s.ptr++;
	s.size--;
    }

    തിരികെ s;
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
    സ്ഥിതം ശൂന്യം
try_possible_paths(
    സ്ഥിരം പൂർണ്ണസംഖ്യ		*df_iters,
    സ്ഥിരം size_t	*paths,
    സ്ഥിരം പൂർണ്ണസംഖ്യ		npaths,
    സ്ഥിരം പൂർണ്ണസംഖ്യ		path_idx,
    പൂർണ്ണസംഖ്യ			*choice,
    diffcmppath_T	*diffcmppath,
    സ്ഥിരം പൂർണ്ണസംഖ്യ		*diff_len,
    സ്ഥിരം size_t	ndiffs,
    സ്ഥിരം mmfile_t	**diff_blk,
    പൂർണ്ണസംഖ്യ			iwhite)
{
    എങ്കിൽ (path_idx == npaths)
    {
	എങ്കിൽ ((*choice) > 0)
	{
	    പൂർണ്ണസംഖ്യ from_vals[LN_MAX_BUFS] = { 0 };
	    സ്ഥിരം പൂർണ്ണസംഖ്യ *to_vals = df_iters;

	    mmfile_t mm[LN_MAX_BUFS];  // stack memory for current_lines
	    mmfile_t *current_വരികൾ[LN_MAX_BUFS];
	    വേണ്ടി (size_t k = 0; k < ndiffs; k++)
	    {
		from_vals[k] = df_iters[k];
		// get the index at all of the places
		എങ്കിൽ ((*choice) & (1 << k))
		{
		    from_vals[k]--;
		    mm[k] = fastforward_buf_to_lnum(*diff_blk[k], df_iters[k]);
		}
		അല്ലെങ്കിൽ
		    CLEAR_FIELD(mm[k]);
		current_വരികൾ[k] = &mm[k];
	    }
	    size_t unwrapped_idx_from = unwrap_indexes(from_vals, diff_len, ndiffs);
	    size_t unwrapped_idx_to = unwrap_indexes(to_vals, diff_len, ndiffs);
	    പൂർണ്ണസംഖ്യ matched_chars = എണ്ണം_n_matched_chars(current_വരികൾ, ndiffs, iwhite);
	    പൂർണ്ണസംഖ്യ score = diffcmppath[unwrapped_idx_from].df_lev_score + matched_chars;

	    എങ്കിൽ (score > diffcmppath[unwrapped_idx_to].df_lev_score)
	    {
		diffcmppath[unwrapped_idx_to].df_path_n = 1;
		diffcmppath[unwrapped_idx_to].df_decision[0] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_to].df_choice[0] = *choice;
		diffcmppath[unwrapped_idx_to].df_lev_score = score;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (score == diffcmppath[unwrapped_idx_to].df_lev_score)
	    {
		size_t k = diffcmppath[unwrapped_idx_to].df_path_n++;
		diffcmppath[unwrapped_idx_to].df_decision[k] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_to].df_choice[k] = *choice;
	    }
	}
	തിരികെ;
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
    സ്ഥിതം size_t
unwrap_indexes(സ്ഥിരം പൂർണ്ണസംഖ്യ *മൂല്യങ്ങൾ, സ്ഥിരം പൂർണ്ണസംഖ്യ *diff_len, സ്ഥിരം size_t ndiffs)
{
    size_t അക്കം_unwrap_scalar = 1;

    വേണ്ടി (size_t k = 0; k < ndiffs; k++)
	അക്കം_unwrap_scalar *= (size_t)diff_len[k] + 1;

    size_t path_idx = 0;
    വേണ്ടി (size_t k = 0; k < ndiffs; k++)
    {
	അക്കം_unwrap_scalar /= (size_t)diff_len[k] + 1;

	പൂർണ്ണസംഖ്യ n = മൂല്യങ്ങൾ[k];
	path_idx += അക്കം_unwrap_scalar * (size_t)n;
    }

    തിരികെ path_idx;
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
    സ്ഥിതം ശൂന്യം
populate_tensor(
    പൂർണ്ണസംഖ്യ			*df_iters,
    സ്ഥിരം size_t	ch_dim,
    diffcmppath_T	*diffcmppath,
    സ്ഥിരം പൂർണ്ണസംഖ്യ		*diff_len,
    സ്ഥിരം size_t	ndiffs,
    സ്ഥിരം mmfile_t	**diff_blk,
    പൂർണ്ണസംഖ്യ			iwhite)
{
    എങ്കിൽ (ch_dim == ndiffs)
    {
	പൂർണ്ണസംഖ്യ npaths = 0;
	size_t paths[LN_MAX_BUFS];

	വേണ്ടി (size_t j = 0; j < ndiffs; j++)
	{
	    എങ്കിൽ (df_iters[j] > 0)
	    {
		paths[npaths] = j;
		npaths++;
	    }
	}

	പൂർണ്ണസംഖ്യ choice = 0;
	size_t unwrapper_idx_to = unwrap_indexes(df_iters, diff_len, ndiffs);

	diffcmppath[unwrapper_idx_to].df_lev_score = -1;
	try_possible_paths(df_iters, paths, npaths, 0, &choice, diffcmppath,
					diff_len, ndiffs, diff_blk, iwhite);
	തിരികെ;
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i <= diff_len[ch_dim]; i++)
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
    സ്ഥിരം mmfile_t	**diff_blk,
    സ്ഥിരം പൂർണ്ണസംഖ്യ		*diff_len,
    സ്ഥിരം size_t	ndiffs,
    പൂർണ്ണസംഖ്യ			**decisions,
    പൂർണ്ണസംഖ്യ			iwhite)
{
    പ്രതിജ്ഞാ(ndiffs <= LN_MAX_BUFS);

    size_t memsize = 1;
    size_t memsize_decisions = 0;
    വേണ്ടി (size_t i = 0; i < ndiffs; i++)
    {
	പ്രതിജ്ഞാ(diff_len[i] >= 0);
	memsize *= (size_t)(diff_len[i] + 1);
	memsize_decisions += (size_t)diff_len[i];
    }

    // create the flattened path matrix
    diffcmppath_T *diffcmppath = lalloc(വലുപ്പം(diffcmppath_T) * memsize, TRUE);
    എങ്കിൽ (diffcmppath == ശൂന്യ)
	തിരികെ 0;

    // allocate memory here
    size_t n = (size_t)pow(2.0, (ഇരട്ട)ndiffs);
    വേണ്ടി (size_t i = 0; i < memsize; i++)
    {
	diffcmppath[i].df_lev_score = 0;
	diffcmppath[i].df_path_n = 0;
	വേണ്ടി (size_t j = 0; j < n; j++)
	    diffcmppath[i].df_choice_mem[j] = -1;
    }

    // memory for avoiding repetitive calculations of score
    പൂർണ്ണസംഖ്യ df_iters[LN_MAX_BUFS];
    populate_tensor(df_iters, 0, diffcmppath, diff_len, ndiffs, diff_blk,
		    iwhite);

    സ്ഥിരം size_t u = unwrap_indexes(diff_len, diff_len, ndiffs);
    diffcmppath_T *start_കണ്ണി = &diffcmppath[u];

    *decisions = lalloc(വലുപ്പം(പൂർണ്ണസംഖ്യ) * memsize_decisions, TRUE);
    എങ്കിൽ (*decisions == ശൂന്യ)
    {
	vim_free(diffcmppath);
	തിരികെ 0;
    }

    size_t n_optimal = 0;
    test_charmatch_paths(start_കണ്ണി, 0);
    ആകുമ്പോൾ (start_കണ്ണി->df_path_n > 0)
    {
	size_t j = start_കണ്ണി->df_optimal_choice;
	(*decisions)[n_optimal++] = start_കണ്ണി->df_choice[j];
	start_കണ്ണി = start_കണ്ണി->df_decision[j];
    }
    // reverse array
    വേണ്ടി (size_t i = 0; i < (n_optimal / 2); i++)
    {
	പൂർണ്ണസംഖ്യ tmp = (*decisions)[i];
	(*decisions)[i] = (*decisions)[n_optimal - 1 - i];
	(*decisions)[n_optimal - 1 - i] = tmp;
    }

    vim_free(diffcmppath);

    തിരികെ n_optimal;
}

// returns the minimum amount of path changes from start to end
    സ്ഥിതം size_t
test_charmatch_paths(diffcmppath_T *കണ്ണി, പൂർണ്ണസംഖ്യ lastdecision)
{
    // memoization
    എങ്കിൽ (കണ്ണി->df_choice_mem[lastdecision] == -1)
    {
	എങ്കിൽ (കണ്ണി->df_path_n == 0)
	    // we have reached the end of the tree
	    കണ്ണി->df_choice_mem[lastdecision] = 0;
	അല്ലെങ്കിൽ
	{
	    // the minimum amount of turns required to reach the end
	    size_t minimum_turns = SIZE_MAX;
	    വേണ്ടി (size_t i = 0; i < കണ്ണി->df_path_n; i++)
	    {
		// recurse
		size_t t = test_charmatch_paths(കണ്ണി->df_decision[i],
							കണ്ണി->df_choice[i]) +
		    (lastdecision != കണ്ണി->df_choice[i] ? 1 : 0);
		എങ്കിൽ (t < minimum_turns)
		{
		    കണ്ണി->df_optimal_choice = i;
		    minimum_turns = t;
		}
	    }
	    കണ്ണി->df_choice_mem[lastdecision] = (പൂർണ്ണസംഖ്യ)minimum_turns;
	}
    }

    തിരികെ (size_t)കണ്ണി->df_choice_mem[lastdecision];
}
