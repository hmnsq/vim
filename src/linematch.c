/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#અન્તર્ભાવ "vim.h"

#define LN_MAX_BUFS 8
#define LN_DECISION_MAX 255  // pow(2, LN_MAX_BUFS(8)) - 1 = 255

// struct for running the diff linematch algorithm
પ્રકારવ્યાખ્યા સંરચના diffcmppath_S diffcmppath_T;
સંરચના diffcmppath_S
{
    // to keep track of the total score of this path
    પૂર્ણાંક			df_lev_score;
    size_t		df_path_n;	// current index of this path
    પૂર્ણાંક			df_choice_mem[LN_DECISION_MAX + 1];
    પૂર્ણાંક			df_choice[LN_DECISION_MAX];
    // to keep track of this path traveled
    diffcmppath_T	*df_decision[LN_DECISION_MAX];
    size_t		df_optimal_choice;
};

સ્થિર પૂર્ણાંક matching_chars(અચલ mmfile_t *m1, અચલ mmfile_t *m2);
સ્થિર size_t unwrap_indexes(અચલ પૂર્ણાંક *કિંમતો, અચલ પૂર્ણાંક *diff_len, અચલ size_t ndiffs);
સ્થિર size_t test_charmatch_paths(diffcmppath_T *ગાંઠ, પૂર્ણાંક lastdecision);

    સ્થિર size_t
લીટી_len(અચલ mmfile_t *m)
{
    અક્ષર	*s = m->ptr;
    અક્ષર	*end;

    end = memchr(s, '\n', (size_t)m->size);
    પાછા end ? (size_t)(end - s) : (size_t)m->size;
}

#define MATCH_CHAR_MAX_LEN 800

/// Same as matching_chars but ignore whitespace
///
/// @param s1
/// @param s2
    સ્થિર પૂર્ણાંક
matching_chars_iwhite(અચલ mmfile_t *s1, અચલ mmfile_t *s2)
{
    // the newly processed strings that will be compared
    // delete the white space characters
    mmfile_t	sp[2];
    અક્ષર	p[2][MATCH_CHAR_MAX_LEN];

    માટે (પૂર્ણાંક k = 0; k < 2; k++)
    {
	અચલ	mmfile_t *s = k == 0 ? s1 : s2;
	size_t	pi = 0;
	size_t	slen = MIN(MATCH_CHAR_MAX_LEN - 1, લીટી_len(s));

	માટે (size_t i = 0; i <= slen; i++)
	{
	    અક્ષર e = s->ptr[i];

	    જો (e != ' ' && e != '\t')
	    {
		p[k][pi] = e;
		pi++;
	    }
	}

	sp[k].ptr = p[k];
	sp[k].size = (પૂર્ણાંક)pi;
    }

    પાછા matching_chars(&sp[0], &sp[1]);
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
    સ્થિર પૂર્ણાંક
matching_chars(અચલ mmfile_t *m1, અચલ mmfile_t *m2)
{
    size_t	s1len = MIN(MATCH_CHAR_MAX_LEN - 1, લીટી_len(m1));
    size_t	s2len = MIN(MATCH_CHAR_MAX_LEN - 1, લીટી_len(m2));
    અક્ષર	*s1 = m1->ptr;
    અક્ષર	*s2 = m2->ptr;
    પૂર્ણાંક		matrix[2][MATCH_CHAR_MAX_LEN] = { 0 };
    પૂર્ણાંક		icur = 1;  // save space by storing only two rows for i axis

    માટે (size_t i = 0; i < s1len; i++)
    {
	icur = (icur == 1 ? 0 : 1);
	પૂર્ણાંક *e1 = matrix[icur];
	પૂર્ણાંક *e2 = matrix[!icur];

	માટે (size_t j = 0; j < s2len; j++)
	{
	    // skip char in s1
	    જો (e2[j + 1] > e1[j + 1])
		e1[j + 1] = e2[j + 1];
	    // skip char in s2
	    જો (e1[j] > e1[j + 1])
		e1[j + 1] = e1[j];
	    // compare char in s1 and s2
	    જો ((s1[i] == s2[j]) && (e2[j] + 1) > e1[j + 1])
		e1[j + 1] = e2[j] + 1;
	}
    }

    પાછા matrix[icur][s2len];
}

/// count the matching characters between a variable number of strings "sp"
/// mark the strings that have already been compared to extract them later
/// without re-running the character match counting.
/// @param sp
/// @param fomvals
/// @param n
    સ્થિર પૂર્ણાંક
ગણતરી_n_matched_chars(mmfile_t **sp, અચલ size_t n, પૂર્ણાંક iwhite)
{
    પૂર્ણાંક matched_chars = 0;
    પૂર્ણાંક matched = 0;

    માટે (size_t i = 0; i < n; i++)
    {
	માટે (size_t j = i + 1; j < n; j++)
	{
	    જો (sp[i]->ptr != ખાલી && sp[j]->ptr != ખાલી)
	    {
		matched++;
		// TODO(lewis6991): handle whitespace ignoring higher up in the
		//ઢગલો
		matched_chars += iwhite ? matching_chars_iwhite(sp[i], sp[j])
					: matching_chars(sp[i], sp[j]);
	    }
	}
    }

    // prioritize a match of 3 (or more lines) equally to a match of 2 lines
    જો (matched >= 2)
    {
	matched_chars *= 2;
	matched_chars /= matched;
    }

    પાછા matched_chars;
}

    સ્થિર mmfile_t
fastforward_buf_to_lnum(mmfile_t s, linenr_T lnum)
{
    માટે (પૂર્ણાંક i = 0; i < lnum - 1; i++)
    {
	અક્ષર *લીટી_end;

	લીટી_end = memchr(s.ptr, '\n', (size_t)s.size);
	s.size = લીટી_end ? (પૂર્ણાંક)(s.size - (લીટી_end - s.ptr)) : 0;
	s.ptr = લીટી_end;
	જો (!s.ptr)
	    તોડો;
	s.ptr++;
	s.size--;
    }

    પાછા s;
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
    સ્થિર શૂન્ય
try_possible_paths(
    અચલ પૂર્ણાંક		*df_iters,
    અચલ size_t	*paths,
    અચલ પૂર્ણાંક		npaths,
    અચલ પૂર્ણાંક		path_idx,
    પૂર્ણાંક			*choice,
    diffcmppath_T	*diffcmppath,
    અચલ પૂર્ણાંક		*diff_len,
    અચલ size_t	ndiffs,
    અચલ mmfile_t	**diff_blk,
    પૂર્ણાંક			iwhite)
{
    જો (path_idx == npaths)
    {
	જો ((*choice) > 0)
	{
	    પૂર્ણાંક from_vals[LN_MAX_BUFS] = { 0 };
	    અચલ પૂર્ણાંક *to_vals = df_iters;

	    mmfile_t mm[LN_MAX_BUFS];  // stack memory for current_lines
	    mmfile_t *current_લીટીઓ[LN_MAX_BUFS];
	    માટે (size_t k = 0; k < ndiffs; k++)
	    {
		from_vals[k] = df_iters[k];
		// get the index at all of the places
		જો ((*choice) & (1 << k))
		{
		    from_vals[k]--;
		    mm[k] = fastforward_buf_to_lnum(*diff_blk[k], df_iters[k]);
		}
		નહીંતર
		    CLEAR_FIELD(mm[k]);
		current_લીટીઓ[k] = &mm[k];
	    }
	    size_t unwrapped_idx_from = unwrap_indexes(from_vals, diff_len, ndiffs);
	    size_t unwrapped_idx_to = unwrap_indexes(to_vals, diff_len, ndiffs);
	    પૂર્ણાંક matched_chars = ગણતરી_n_matched_chars(current_લીટીઓ, ndiffs, iwhite);
	    પૂર્ણાંક score = diffcmppath[unwrapped_idx_from].df_lev_score + matched_chars;

	    જો (score > diffcmppath[unwrapped_idx_to].df_lev_score)
	    {
		diffcmppath[unwrapped_idx_to].df_path_n = 1;
		diffcmppath[unwrapped_idx_to].df_decision[0] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_to].df_choice[0] = *choice;
		diffcmppath[unwrapped_idx_to].df_lev_score = score;
	    }
	    નહીંતર જો (score == diffcmppath[unwrapped_idx_to].df_lev_score)
	    {
		size_t k = diffcmppath[unwrapped_idx_to].df_path_n++;
		diffcmppath[unwrapped_idx_to].df_decision[k] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_to].df_choice[k] = *choice;
	    }
	}
	પાછા;
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
    સ્થિર size_t
unwrap_indexes(અચલ પૂર્ણાંક *કિંમતો, અચલ પૂર્ણાંક *diff_len, અચલ size_t ndiffs)
{
    size_t અંક_unwrap_scalar = 1;

    માટે (size_t k = 0; k < ndiffs; k++)
	અંક_unwrap_scalar *= (size_t)diff_len[k] + 1;

    size_t path_idx = 0;
    માટે (size_t k = 0; k < ndiffs; k++)
    {
	અંક_unwrap_scalar /= (size_t)diff_len[k] + 1;

	પૂર્ણાંક n = કિંમતો[k];
	path_idx += અંક_unwrap_scalar * (size_t)n;
    }

    પાછા path_idx;
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
    સ્થિર શૂન્ય
populate_tensor(
    પૂર્ણાંક			*df_iters,
    અચલ size_t	ch_dim,
    diffcmppath_T	*diffcmppath,
    અચલ પૂર્ણાંક		*diff_len,
    અચલ size_t	ndiffs,
    અચલ mmfile_t	**diff_blk,
    પૂર્ણાંક			iwhite)
{
    જો (ch_dim == ndiffs)
    {
	પૂર્ણાંક npaths = 0;
	size_t paths[LN_MAX_BUFS];

	માટે (size_t j = 0; j < ndiffs; j++)
	{
	    જો (df_iters[j] > 0)
	    {
		paths[npaths] = j;
		npaths++;
	    }
	}

	પૂર્ણાંક choice = 0;
	size_t unwrapper_idx_to = unwrap_indexes(df_iters, diff_len, ndiffs);

	diffcmppath[unwrapper_idx_to].df_lev_score = -1;
	try_possible_paths(df_iters, paths, npaths, 0, &choice, diffcmppath,
					diff_len, ndiffs, diff_blk, iwhite);
	પાછા;
    }

    માટે (પૂર્ણાંક i = 0; i <= diff_len[ch_dim]; i++)
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
    અચલ mmfile_t	**diff_blk,
    અચલ પૂર્ણાંક		*diff_len,
    અચલ size_t	ndiffs,
    પૂર્ણાંક			**decisions,
    પૂર્ણાંક			iwhite)
{
    પ્રતિજ્ઞા(ndiffs <= LN_MAX_BUFS);

    size_t memsize = 1;
    size_t memsize_decisions = 0;
    માટે (size_t i = 0; i < ndiffs; i++)
    {
	પ્રતિજ્ઞા(diff_len[i] >= 0);
	memsize *= (size_t)(diff_len[i] + 1);
	memsize_decisions += (size_t)diff_len[i];
    }

    // create the flattened path matrix
    diffcmppath_T *diffcmppath = lalloc(કદ(diffcmppath_T) * memsize, TRUE);
    જો (diffcmppath == ખાલી)
	પાછા 0;

    // allocate memory here
    size_t n = (size_t)pow(2.0, (દ્વિગુણ)ndiffs);
    માટે (size_t i = 0; i < memsize; i++)
    {
	diffcmppath[i].df_lev_score = 0;
	diffcmppath[i].df_path_n = 0;
	માટે (size_t j = 0; j < n; j++)
	    diffcmppath[i].df_choice_mem[j] = -1;
    }

    // memory for avoiding repetitive calculations of score
    પૂર્ણાંક df_iters[LN_MAX_BUFS];
    populate_tensor(df_iters, 0, diffcmppath, diff_len, ndiffs, diff_blk,
		    iwhite);

    અચલ size_t u = unwrap_indexes(diff_len, diff_len, ndiffs);
    diffcmppath_T *start_ગાંઠ = &diffcmppath[u];

    *decisions = lalloc(કદ(પૂર્ણાંક) * memsize_decisions, TRUE);
    જો (*decisions == ખાલી)
    {
	vim_free(diffcmppath);
	પાછા 0;
    }

    size_t n_optimal = 0;
    test_charmatch_paths(start_ગાંઠ, 0);
    જ્યારે (start_ગાંઠ->df_path_n > 0)
    {
	size_t j = start_ગાંઠ->df_optimal_choice;
	(*decisions)[n_optimal++] = start_ગાંઠ->df_choice[j];
	start_ગાંઠ = start_ગાંઠ->df_decision[j];
    }
    // reverse array
    માટે (size_t i = 0; i < (n_optimal / 2); i++)
    {
	પૂર્ણાંક tmp = (*decisions)[i];
	(*decisions)[i] = (*decisions)[n_optimal - 1 - i];
	(*decisions)[n_optimal - 1 - i] = tmp;
    }

    vim_free(diffcmppath);

    પાછા n_optimal;
}

// returns the minimum amount of path changes from start to end
    સ્થિર size_t
test_charmatch_paths(diffcmppath_T *ગાંઠ, પૂર્ણાંક lastdecision)
{
    // memoization
    જો (ગાંઠ->df_choice_mem[lastdecision] == -1)
    {
	જો (ગાંઠ->df_path_n == 0)
	    // we have reached the end of the tree
	    ગાંઠ->df_choice_mem[lastdecision] = 0;
	નહીંતર
	{
	    // the minimum amount of turns required to reach the end
	    size_t minimum_turns = SIZE_MAX;
	    માટે (size_t i = 0; i < ગાંઠ->df_path_n; i++)
	    {
		// recurse
		size_t t = test_charmatch_paths(ગાંઠ->df_decision[i],
							ગાંઠ->df_choice[i]) +
		    (lastdecision != ગાંઠ->df_choice[i] ? 1 : 0);
		જો (t < minimum_turns)
		{
		    ગાંઠ->df_optimal_choice = i;
		    minimum_turns = t;
		}
	    }
	    ગાંઠ->df_choice_mem[lastdecision] = (પૂર્ણાંક)minimum_turns;
	}
    }

    પાછા (size_t)ગાંઠ->df_choice_mem[lastdecision];
}
