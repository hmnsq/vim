/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"

#define LN_MAX_BUFS 8
#define LN_DECISION_MAX 255  // pow(2, LN_MAX_BUFS(8)) - 1 = 255

// struct for running the diff linematch algorithm
प्रकारव्याख्या संरचना diffcmppath_S diffcmppath_T;
संरचना diffcmppath_S
{
    // to keep track of the total score of this path
    पूर्णांक			df_lev_score;
    माप_प्रकार		df_path_n;	// current index of this path
    पूर्णांक			df_choice_mem[LN_DECISION_MAX + 1];
    पूर्णांक			df_choice[LN_DECISION_MAX];
    // to keep track of this path traveled
    diffcmppath_T	*df_decision[LN_DECISION_MAX];
    माप_प्रकार		df_optimal_choice;
};

स्थिर पूर्णांक matching_chars(स्थिरांक mmfile_t *m1, स्थिरांक mmfile_t *m2);
स्थिर माप_प्रकार unwrap_indexes(स्थिरांक पूर्णांक *मूल्ये, स्थिरांक पूर्णांक *diff_len, स्थिरांक माप_प्रकार ndiffs);
स्थिर माप_प्रकार test_charmatch_paths(diffcmppath_T *गाठ, पूर्णांक lastdecision);

    स्थिर माप_प्रकार
ओळ_len(स्थिरांक mmfile_t *m)
{
    अक्षर	*s = m->ptr;
    अक्षर	*end;

    end = स_प्रथम(s, '\n', (माप_प्रकार)m->size);
    परत end ? (माप_प्रकार)(end - s) : (माप_प्रकार)m->size;
}

#define MATCH_CHAR_MAX_LEN 800

/// Same as matching_chars but ignore whitespace
///
/// @param s1
/// @param s2
    स्थिर पूर्णांक
matching_chars_iwhite(स्थिरांक mmfile_t *s1, स्थिरांक mmfile_t *s2)
{
    // the newly processed strings that will be compared
    // delete the white space characters
    mmfile_t	sp[2];
    अक्षर	p[2][MATCH_CHAR_MAX_LEN];

    साठी (पूर्णांक k = 0; k < 2; k++)
    {
	स्थिरांक	mmfile_t *s = k == 0 ? s1 : s2;
	माप_प्रकार	pi = 0;
	माप_प्रकार	slen = MIN(MATCH_CHAR_MAX_LEN - 1, ओळ_len(s));

	साठी (माप_प्रकार i = 0; i <= slen; i++)
	{
	    अक्षर e = s->ptr[i];

	    जर (e != ' ' && e != '\t')
	    {
		p[k][pi] = e;
		pi++;
	    }
	}

	sp[k].ptr = p[k];
	sp[k].size = (पूर्णांक)pi;
    }

    परत matching_chars(&sp[0], &sp[1]);
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
    स्थिर पूर्णांक
matching_chars(स्थिरांक mmfile_t *m1, स्थिरांक mmfile_t *m2)
{
    माप_प्रकार	s1len = MIN(MATCH_CHAR_MAX_LEN - 1, ओळ_len(m1));
    माप_प्रकार	s2len = MIN(MATCH_CHAR_MAX_LEN - 1, ओळ_len(m2));
    अक्षर	*s1 = m1->ptr;
    अक्षर	*s2 = m2->ptr;
    पूर्णांक		matrix[2][MATCH_CHAR_MAX_LEN] = { 0 };
    पूर्णांक		icur = 1;  // save space by storing only two rows for i axis

    साठी (माप_प्रकार i = 0; i < s1len; i++)
    {
	icur = (icur == 1 ? 0 : 1);
	पूर्णांक *e1 = matrix[icur];
	पूर्णांक *e2 = matrix[!icur];

	साठी (माप_प्रकार j = 0; j < s2len; j++)
	{
	    // skip char in s1
	    जर (e2[j + 1] > e1[j + 1])
		e1[j + 1] = e2[j + 1];
	    // skip char in s2
	    जर (e1[j] > e1[j + 1])
		e1[j + 1] = e1[j];
	    // compare char in s1 and s2
	    जर ((s1[i] == s2[j]) && (e2[j] + 1) > e1[j + 1])
		e1[j + 1] = e2[j] + 1;
	}
    }

    परत matrix[icur][s2len];
}

/// count the matching characters between a variable number of strings "sp"
/// mark the strings that have already been compared to extract them later
/// without re-running the character match counting.
/// @param sp
/// @param fomvals
/// @param n
    स्थिर पूर्णांक
मोजणी_n_matched_chars(mmfile_t **sp, स्थिरांक माप_प्रकार n, पूर्णांक iwhite)
{
    पूर्णांक matched_chars = 0;
    पूर्णांक matched = 0;

    साठी (माप_प्रकार i = 0; i < n; i++)
    {
	साठी (माप_प्रकार j = i + 1; j < n; j++)
	{
	    जर (sp[i]->ptr != रिक्त && sp[j]->ptr != रिक्त)
	    {
		matched++;
		// TODO(lewis6991): handle whitespace ignoring higher up in the
		//चळत
		matched_chars += iwhite ? matching_chars_iwhite(sp[i], sp[j])
					: matching_chars(sp[i], sp[j]);
	    }
	}
    }

    // prioritize a match of 3 (or more lines) equally to a match of 2 lines
    जर (matched >= 2)
    {
	matched_chars *= 2;
	matched_chars /= matched;
    }

    परत matched_chars;
}

    स्थिर mmfile_t
fastforward_buf_to_lnum(mmfile_t s, linenr_T lnum)
{
    साठी (पूर्णांक i = 0; i < lnum - 1; i++)
    {
	अक्षर *ओळ_end;

	ओळ_end = स_प्रथम(s.ptr, '\n', (माप_प्रकार)s.size);
	s.size = ओळ_end ? (पूर्णांक)(s.size - (ओळ_end - s.ptr)) : 0;
	s.ptr = ओळ_end;
	जर (!s.ptr)
	    थांबा;
	s.ptr++;
	s.size--;
    }

    परत s;
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
    स्थिर शून्य
try_possible_paths(
    स्थिरांक पूर्णांक		*df_iters,
    स्थिरांक माप_प्रकार	*paths,
    स्थिरांक पूर्णांक		npaths,
    स्थिरांक पूर्णांक		path_idx,
    पूर्णांक			*choice,
    diffcmppath_T	*diffcmppath,
    स्थिरांक पूर्णांक		*diff_len,
    स्थिरांक माप_प्रकार	ndiffs,
    स्थिरांक mmfile_t	**diff_blk,
    पूर्णांक			iwhite)
{
    जर (path_idx == npaths)
    {
	जर ((*choice) > 0)
	{
	    पूर्णांक from_vals[LN_MAX_BUFS] = { 0 };
	    स्थिरांक पूर्णांक *to_vals = df_iters;

	    mmfile_t mm[LN_MAX_BUFS];  // stack memory for current_lines
	    mmfile_t *current_ओळी[LN_MAX_BUFS];
	    साठी (माप_प्रकार k = 0; k < ndiffs; k++)
	    {
		from_vals[k] = df_iters[k];
		// get the index at all of the places
		जर ((*choice) & (1 << k))
		{
		    from_vals[k]--;
		    mm[k] = fastforward_buf_to_lnum(*diff_blk[k], df_iters[k]);
		}
		अन्यथा
		    CLEAR_FIELD(mm[k]);
		current_ओळी[k] = &mm[k];
	    }
	    माप_प्रकार unwrapped_idx_from = unwrap_indexes(from_vals, diff_len, ndiffs);
	    माप_प्रकार unwrapped_idx_to = unwrap_indexes(to_vals, diff_len, ndiffs);
	    पूर्णांक matched_chars = मोजणी_n_matched_chars(current_ओळी, ndiffs, iwhite);
	    पूर्णांक score = diffcmppath[unwrapped_idx_from].df_lev_score + matched_chars;

	    जर (score > diffcmppath[unwrapped_idx_to].df_lev_score)
	    {
		diffcmppath[unwrapped_idx_to].df_path_n = 1;
		diffcmppath[unwrapped_idx_to].df_decision[0] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_to].df_choice[0] = *choice;
		diffcmppath[unwrapped_idx_to].df_lev_score = score;
	    }
	    अन्यथा जर (score == diffcmppath[unwrapped_idx_to].df_lev_score)
	    {
		माप_प्रकार k = diffcmppath[unwrapped_idx_to].df_path_n++;
		diffcmppath[unwrapped_idx_to].df_decision[k] =
					&diffcmppath[unwrapped_idx_from];
		diffcmppath[unwrapped_idx_to].df_choice[k] = *choice;
	    }
	}
	परत;
    }

    माप_प्रकार bit_place = paths[path_idx];
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
    स्थिर माप_प्रकार
unwrap_indexes(स्थिरांक पूर्णांक *मूल्ये, स्थिरांक पूर्णांक *diff_len, स्थिरांक माप_प्रकार ndiffs)
{
    माप_प्रकार अंक_unwrap_scalar = 1;

    साठी (माप_प्रकार k = 0; k < ndiffs; k++)
	अंक_unwrap_scalar *= (माप_प्रकार)diff_len[k] + 1;

    माप_प्रकार path_idx = 0;
    साठी (माप_प्रकार k = 0; k < ndiffs; k++)
    {
	अंक_unwrap_scalar /= (माप_प्रकार)diff_len[k] + 1;

	पूर्णांक n = मूल्ये[k];
	path_idx += अंक_unwrap_scalar * (माप_प्रकार)n;
    }

    परत path_idx;
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
    स्थिर शून्य
populate_tensor(
    पूर्णांक			*df_iters,
    स्थिरांक माप_प्रकार	ch_dim,
    diffcmppath_T	*diffcmppath,
    स्थिरांक पूर्णांक		*diff_len,
    स्थिरांक माप_प्रकार	ndiffs,
    स्थिरांक mmfile_t	**diff_blk,
    पूर्णांक			iwhite)
{
    जर (ch_dim == ndiffs)
    {
	पूर्णांक npaths = 0;
	माप_प्रकार paths[LN_MAX_BUFS];

	साठी (माप_प्रकार j = 0; j < ndiffs; j++)
	{
	    जर (df_iters[j] > 0)
	    {
		paths[npaths] = j;
		npaths++;
	    }
	}

	पूर्णांक choice = 0;
	माप_प्रकार unwrapper_idx_to = unwrap_indexes(df_iters, diff_len, ndiffs);

	diffcmppath[unwrapper_idx_to].df_lev_score = -1;
	try_possible_paths(df_iters, paths, npaths, 0, &choice, diffcmppath,
					diff_len, ndiffs, diff_blk, iwhite);
	परत;
    }

    साठी (पूर्णांक i = 0; i <= diff_len[ch_dim]; i++)
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
    माप_प्रकार
linematch_nbuffers(
    स्थिरांक mmfile_t	**diff_blk,
    स्थिरांक पूर्णांक		*diff_len,
    स्थिरांक माप_प्रकार	ndiffs,
    पूर्णांक			**decisions,
    पूर्णांक			iwhite)
{
    प्रतिज्ञा(ndiffs <= LN_MAX_BUFS);

    माप_प्रकार memsize = 1;
    माप_प्रकार memsize_decisions = 0;
    साठी (माप_प्रकार i = 0; i < ndiffs; i++)
    {
	प्रतिज्ञा(diff_len[i] >= 0);
	memsize *= (माप_प्रकार)(diff_len[i] + 1);
	memsize_decisions += (माप_प्रकार)diff_len[i];
    }

    // create the flattened path matrix
    diffcmppath_T *diffcmppath = lalloc(आकार(diffcmppath_T) * memsize, TRUE);
    जर (diffcmppath == रिक्त)
	परत 0;

    // allocate memory here
    माप_प्रकार n = (माप_प्रकार)घात(2.0, (द्विगुण)ndiffs);
    साठी (माप_प्रकार i = 0; i < memsize; i++)
    {
	diffcmppath[i].df_lev_score = 0;
	diffcmppath[i].df_path_n = 0;
	साठी (माप_प्रकार j = 0; j < n; j++)
	    diffcmppath[i].df_choice_mem[j] = -1;
    }

    // memory for avoiding repetitive calculations of score
    पूर्णांक df_iters[LN_MAX_BUFS];
    populate_tensor(df_iters, 0, diffcmppath, diff_len, ndiffs, diff_blk,
		    iwhite);

    स्थिरांक माप_प्रकार u = unwrap_indexes(diff_len, diff_len, ndiffs);
    diffcmppath_T *start_गाठ = &diffcmppath[u];

    *decisions = lalloc(आकार(पूर्णांक) * memsize_decisions, TRUE);
    जर (*decisions == रिक्त)
    {
	vim_free(diffcmppath);
	परत 0;
    }

    माप_प्रकार n_optimal = 0;
    test_charmatch_paths(start_गाठ, 0);
    पर्यंत (start_गाठ->df_path_n > 0)
    {
	माप_प्रकार j = start_गाठ->df_optimal_choice;
	(*decisions)[n_optimal++] = start_गाठ->df_choice[j];
	start_गाठ = start_गाठ->df_decision[j];
    }
    // reverse array
    साठी (माप_प्रकार i = 0; i < (n_optimal / 2); i++)
    {
	पूर्णांक tmp = (*decisions)[i];
	(*decisions)[i] = (*decisions)[n_optimal - 1 - i];
	(*decisions)[n_optimal - 1 - i] = tmp;
    }

    vim_free(diffcmppath);

    परत n_optimal;
}

// returns the minimum amount of path changes from start to end
    स्थिर माप_प्रकार
test_charmatch_paths(diffcmppath_T *गाठ, पूर्णांक lastdecision)
{
    // memoization
    जर (गाठ->df_choice_mem[lastdecision] == -1)
    {
	जर (गाठ->df_path_n == 0)
	    // we have reached the end of the tree
	    गाठ->df_choice_mem[lastdecision] = 0;
	अन्यथा
	{
	    // the minimum amount of turns required to reach the end
	    माप_प्रकार minimum_turns = SIZE_MAX;
	    साठी (माप_प्रकार i = 0; i < गाठ->df_path_n; i++)
	    {
		// recurse
		माप_प्रकार t = test_charmatch_paths(गाठ->df_decision[i],
							गाठ->df_choice[i]) +
		    (lastdecision != गाठ->df_choice[i] ? 1 : 0);
		जर (t < minimum_turns)
		{
		    गाठ->df_optimal_choice = i;
		    minimum_turns = t;
		}
	    }
	    गाठ->df_choice_mem[lastdecision] = (पूर्णांक)minimum_turns;
	}
    }

    परत (माप_प्रकार)गाठ->df_choice_mem[lastdecision];
}
