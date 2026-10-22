/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * autocmd.c: Autocommand related functions
 */

#include "vim.h"

/*
 * The autocommands are stored in a list for each event.
 * Autocommands for the same pattern, that are consecutive, are joined
 * together, to avoid having to match the pattern too often.
 * The result is an array of Autopat lists, which point to AutoCmd lists:
 *
 * last_autopat[0]  -----------------------------+
 *						 V
 * first_autopat[0] --> Autopat.next  -->  Autopat.next -->  NULL
 *			Autopat.cmds	   Autopat.cmds
 *			    |			 |
 *			    V			 V
 *			AutoCmd.next	   AutoCmd.next
 *			    |			 |
 *			    V			 V
 *			AutoCmd.next		NULL
 *			    |
 *			    V
 *			   NULL
 *
 * last_autopat[1]  --------+
 *			    V
 * first_autopat[1] --> Autopat.next  -->  NULL
 *			Autopat.cmds
 *			    |
 *			    V
 *			AutoCmd.next
 *			    |
 *			    V
 *			   NULL
 *   etc.
 *
 *   The order of AutoCmds is important, this is the order in which they were
 *   defined and will have to be executed.
 */
typedef struct AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    char	    once;		// "One shot": removed after execution
    char	    nested;		// If autocommands nest here.
    char	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    struct AutoCmd  *next;		// next AutoCmd in list
} AutoCmd;

typedef struct AutoPat
{
    struct AutoPat  *next;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    целое		    group;		// group ID
    целое		    patlen;		// strlen() of pat
    целое		    buflocal_nr;	// !=0 for buffer-local AutoPat
    char	    allow_dirs;		// Pattern may match whole path
    char	    last;		// last pattern for apply_autocmds()
} AutoPat;

//
// special cases:
// BufNewFile and BufRead are searched for ALOT (especially at startup)
// so we pre-determine their index into the event_tab[] table for fast access.
// Keep these values in sync with event_tab[]!
#define BUFNEWFILE_INDEX 9
#define BUFREAD_INDEX 10

// Must be sorted by the 'value' field because it is used by bsearch()!
// Events with positive keys aren't allowed in 'eventignorewin'.
static keyvalue_T event_tab[NUM_EVENTS] = {
    KEYVALUE_ENTRY(-EVENT_BUFADD, "BufAdd"),
    KEYVALUE_ENTRY(-EVENT_BUFADD, "BufCreate"),
    KEYVALUE_ENTRY(-EVENT_BUFDELETE, "BufDelete"),
    KEYVALUE_ENTRY(-EVENT_BUFENTER, "BufEnter"),
    KEYVALUE_ENTRY(-EVENT_BUFFILEPOST, "BufFilePost"),
    KEYVALUE_ENTRY(-EVENT_BUFFILEPRE, "BufFilePre"),
    KEYVALUE_ENTRY(-EVENT_BUFHIDDEN, "BufHidden"),
    KEYVALUE_ENTRY(-EVENT_BUFLEAVE, "BufLeave"),
    KEYVALUE_ENTRY(-EVENT_BUFNEW, "BufNew"),
    KEYVALUE_ENTRY(-EVENT_BUFNEWFILE, "BufNewFile"),	// BUFNEWFILE_INDEX
    KEYVALUE_ENTRY(-EVENT_BUFREADPOST, "BufRead"),	// BUFREAD_INDEX
    KEYVALUE_ENTRY(-EVENT_BUFREADCMD, "BufReadCmd"),
    KEYVALUE_ENTRY(-EVENT_BUFREADPOST, "BufReadPost"),
    KEYVALUE_ENTRY(-EVENT_BUFREADPRE, "BufReadPre"),
    KEYVALUE_ENTRY(-EVENT_BUFUNLOAD, "BufUnload"),
    KEYVALUE_ENTRY(-EVENT_BUFWINENTER, "BufWinEnter"),
    KEYVALUE_ENTRY(-EVENT_BUFWINLEAVE, "BufWinLeave"),
    KEYVALUE_ENTRY(-EVENT_BUFWIPEOUT, "BufWipeout"),
    KEYVALUE_ENTRY(-EVENT_BUFWRITEPRE, "BufWrite"),
    KEYVALUE_ENTRY(-EVENT_BUFWRITECMD, "BufWriteCmd"),
    KEYVALUE_ENTRY(-EVENT_BUFWRITEPOST, "BufWritePost"),
    KEYVALUE_ENTRY(-EVENT_BUFWRITEPRE, "BufWritePre"),
    KEYVALUE_ENTRY(EVENT_CMDLINECHANGED, "CmdlineChanged"),
    KEYVALUE_ENTRY(EVENT_CMDLINEENTER, "CmdlineEnter"),
    KEYVALUE_ENTRY(EVENT_CMDLINELEAVE, "CmdlineLeave"),
    KEYVALUE_ENTRY(EVENT_CMDLINELEAVEPRE, "CmdlineLeavePre"),
    KEYVALUE_ENTRY(EVENT_CMDUNDEFINED, "CmdUndefined"),
    KEYVALUE_ENTRY(EVENT_CMDWINENTER, "CmdwinEnter"),
    KEYVALUE_ENTRY(EVENT_CMDWINLEAVE, "CmdwinLeave"),
    KEYVALUE_ENTRY(EVENT_COLORSCHEME, "ColorScheme"),
    KEYVALUE_ENTRY(EVENT_COLORSCHEMEPRE, "ColorSchemePre"),
    KEYVALUE_ENTRY(EVENT_COMPLETECHANGED, "CompleteChanged"),
    KEYVALUE_ENTRY(EVENT_COMPLETEDONE, "CompleteDone"),
    KEYVALUE_ENTRY(EVENT_COMPLETEDONEPRE, "CompleteDonePre"),
    KEYVALUE_ENTRY(-EVENT_CURSORHOLD, "CursorHold"),
    KEYVALUE_ENTRY(-EVENT_CURSORHOLDI, "CursorHoldI"),
    KEYVALUE_ENTRY(-EVENT_CURSORMOVED, "CursorMoved"),
    KEYVALUE_ENTRY(-EVENT_CURSORMOVEDC, "CursorMovedC"),
    KEYVALUE_ENTRY(-EVENT_CURSORMOVEDI, "CursorMovedI"),
    KEYVALUE_ENTRY(EVENT_DIFFUPDATED, "DiffUpdated"),
    KEYVALUE_ENTRY(EVENT_DIRCHANGED, "DirChanged"),
    KEYVALUE_ENTRY(EVENT_DIRCHANGEDPRE, "DirChangedPre"),
    KEYVALUE_ENTRY(EVENT_ENCODINGCHANGED, "EncodingChanged"),
    KEYVALUE_ENTRY(EVENT_EXITPRE, "ExitPre"),
    KEYVALUE_ENTRY(-EVENT_FILEAPPENDCMD, "FileAppendCmd"),
    KEYVALUE_ENTRY(-EVENT_FILEAPPENDPOST, "FileAppendPost"),
    KEYVALUE_ENTRY(-EVENT_FILEAPPENDPRE, "FileAppendPre"),
    KEYVALUE_ENTRY(-EVENT_FILECHANGEDRO, "FileChangedRO"),
    KEYVALUE_ENTRY(-EVENT_FILECHANGEDSHELL, "FileChangedShell"),
    KEYVALUE_ENTRY(-EVENT_FILECHANGEDSHELLPOST, "FileChangedShellPost"),
    KEYVALUE_ENTRY(EVENT_ENCODINGCHANGED, "FileEncoding"),
    KEYVALUE_ENTRY(-EVENT_FILEREADCMD, "FileReadCmd"),
    KEYVALUE_ENTRY(-EVENT_FILEREADPOST, "FileReadPost"),
    KEYVALUE_ENTRY(-EVENT_FILEREADPRE, "FileReadPre"),
    KEYVALUE_ENTRY(-EVENT_FILETYPE, "FileType"),
    KEYVALUE_ENTRY(-EVENT_FILEWRITECMD, "FileWriteCmd"),
    KEYVALUE_ENTRY(-EVENT_FILEWRITEPOST, "FileWritePost"),
    KEYVALUE_ENTRY(-EVENT_FILEWRITEPRE, "FileWritePre"),
    KEYVALUE_ENTRY(-EVENT_FILTERREADPOST, "FilterReadPost"),
    KEYVALUE_ENTRY(-EVENT_FILTERREADPRE, "FilterReadPre"),
    KEYVALUE_ENTRY(-EVENT_FILTERWRITEPOST, "FilterWritePost"),
    KEYVALUE_ENTRY(-EVENT_FILTERWRITEPRE, "FilterWritePre"),
    KEYVALUE_ENTRY(EVENT_FOCUSGAINED, "FocusGained"),
    KEYVALUE_ENTRY(EVENT_FOCUSLOST, "FocusLost"),
    KEYVALUE_ENTRY(EVENT_FUNCUNDEFINED, "FuncUndefined"),
    KEYVALUE_ENTRY(EVENT_GUIENTER, "GUIEnter"),
    KEYVALUE_ENTRY(EVENT_GUIFAILED, "GUIFailed"),
    KEYVALUE_ENTRY(-EVENT_INSERTCHANGE, "InsertChange"),
    KEYVALUE_ENTRY(-EVENT_INSERTCHARPRE, "InsertCharPre"),
    KEYVALUE_ENTRY(-EVENT_INSERTENTER, "InsertEnter"),
    KEYVALUE_ENTRY(-EVENT_INSERTLEAVE, "InsertLeave"),
    KEYVALUE_ENTRY(-EVENT_INSERTLEAVEPRE, "InsertLeavePre"),
    KEYVALUE_ENTRY(EVENT_KEYINPUTPRE, "KeyInputPre"),
    KEYVALUE_ENTRY(EVENT_MENUPOPUP, "MenuPopup"),
    KEYVALUE_ENTRY(EVENT_MODECHANGED, "ModeChanged"),
    KEYVALUE_ENTRY(EVENT_OPTIONSET, "OptionSet"),
    KEYVALUE_ENTRY(EVENT_QUICKFIXCMDPOST, "QuickFixCmdPost"),
    KEYVALUE_ENTRY(EVENT_QUICKFIXCMDPRE, "QuickFixCmdPre"),
    KEYVALUE_ENTRY(EVENT_QUITPRE, "QuitPre"),
    KEYVALUE_ENTRY(EVENT_REMOTEREPLY, "RemoteReply"),
    KEYVALUE_ENTRY(EVENT_SAFESTATE, "SafeState"),
    KEYVALUE_ENTRY(EVENT_SAFESTATEAGAIN, "SafeStateAgain"),
    KEYVALUE_ENTRY(EVENT_SESSIONLOADPOST, "SessionLoadPost"),
    KEYVALUE_ENTRY(EVENT_SESSIONLOADPRE, "SessionLoadPre"),
    KEYVALUE_ENTRY(EVENT_SESSIONWRITEPOST, "SessionWritePost"),
    KEYVALUE_ENTRY(EVENT_SHELLCMDPOST, "ShellCmdPost"),
    KEYVALUE_ENTRY(-EVENT_SHELLFILTERPOST, "ShellFilterPost"),
    KEYVALUE_ENTRY(EVENT_SIGUSR1, "SigUSR1"),
    KEYVALUE_ENTRY(EVENT_SOURCECMD, "SourceCmd"),
    KEYVALUE_ENTRY(EVENT_SOURCEPOST, "SourcePost"),
    KEYVALUE_ENTRY(EVENT_SOURCEPRE, "SourcePre"),
    KEYVALUE_ENTRY(EVENT_SPELLFILEMISSING, "SpellFileMissing"),
    KEYVALUE_ENTRY(EVENT_STDINREADPOST, "StdinReadPost"),
    KEYVALUE_ENTRY(EVENT_STDINREADPRE, "StdinReadPre"),
    KEYVALUE_ENTRY(EVENT_SWAPEXISTS, "SwapExists"),
    KEYVALUE_ENTRY(EVENT_SYNTAX, "Syntax"),
    KEYVALUE_ENTRY(EVENT_TABCLOSED, "TabClosed"),
    KEYVALUE_ENTRY(EVENT_TABCLOSEDPRE, "TabClosedPre"),
    KEYVALUE_ENTRY(EVENT_TABENTER, "TabEnter"),
    KEYVALUE_ENTRY(EVENT_TABLEAVE, "TabLeave"),
    KEYVALUE_ENTRY(EVENT_TABNEW, "TabNew"),
    KEYVALUE_ENTRY(EVENT_TERMCHANGED, "TermChanged"),
    KEYVALUE_ENTRY(EVENT_TERMINALOPEN, "TerminalOpen"),
    KEYVALUE_ENTRY(EVENT_TERMINALWINOPEN, "TerminalWinOpen"),
    KEYVALUE_ENTRY(EVENT_TERMRESPONSE, "TermResponse"),
    KEYVALUE_ENTRY(EVENT_TERMRESPONSEALL, "TermResponseAll"),
    KEYVALUE_ENTRY(-EVENT_TEXTCHANGED, "TextChanged"),
    KEYVALUE_ENTRY(-EVENT_TEXTCHANGEDI, "TextChangedI"),
    KEYVALUE_ENTRY(-EVENT_TEXTCHANGEDP, "TextChangedP"),
    KEYVALUE_ENTRY(-EVENT_TEXTCHANGEDT, "TextChangedT"),
    KEYVALUE_ENTRY(-EVENT_TEXTPUTPOST, "TextPutPost"),
    KEYVALUE_ENTRY(-EVENT_TEXTPUTPRE, "TextPutPre"),
    KEYVALUE_ENTRY(-EVENT_TEXTYANKPOST, "TextYankPost"),
    KEYVALUE_ENTRY(EVENT_USER, "User"),
    KEYVALUE_ENTRY(EVENT_VIMENTER, "VimEnter"),
    KEYVALUE_ENTRY(EVENT_VIMLEAVE, "VimLeave"),
    KEYVALUE_ENTRY(EVENT_VIMLEAVEPRE, "VimLeavePre"),
    KEYVALUE_ENTRY(EVENT_VIMRESIZED, "VimResized"),
    KEYVALUE_ENTRY(EVENT_VIMRESUME, "VimResume"),
    KEYVALUE_ENTRY(EVENT_VIMSUSPEND, "VimSuspend"),
    KEYVALUE_ENTRY(-EVENT_WINCLOSED, "WinClosed"),
    KEYVALUE_ENTRY(-EVENT_WINENTER, "WinEnter"),
    KEYVALUE_ENTRY(-EVENT_WINLEAVE, "WinLeave"),
    KEYVALUE_ENTRY(EVENT_WINNEW, "WinNew"),
    KEYVALUE_ENTRY(EVENT_WINNEWPRE, "WinNewPre"),
    KEYVALUE_ENTRY(-EVENT_WINRESIZED, "WinResized"),
    KEYVALUE_ENTRY(-EVENT_WINSCROLLED, "WinScrolled"),
};

static AutoPat *first_autopat[NUM_EVENTS] = { NULL };
static AutoPat *last_autopat[NUM_EVENTS] = { NULL };

#define AUGROUP_DEFAULT    (-1)	    // default autocmd group
#define AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#define AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
struct AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    целое		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    целое		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

static AutoPatCmd_T *active_apc_list = NULL; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    для ((ap) = first_autopat[(целое)(event)]; (ap) != NULL; (ap) = (ap)->next)

/*
 * augroups stores a list of autocmd group names.
 */
static garray_T augroups = {0, 0, sizeof(char_u *), 10, NULL};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])
// use get_deleted_augroup() to get this
static char_u *deleted_augroup = NULL;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
static целое current_augroup = AUGROUP_DEFAULT;

static целое au_need_clean = FALSE;   // need to delete marked patterns

static event_T event_name2nr(char_u *start, char_u **end);
static string_T *event_nr2name(event_T event);
static целое au_get_grouparg(char_u **argp);
static целое do_autocmd_event(event_T event, char_u *pat, целое once, целое nested, char_u *cmd, целое forceit, целое group, целое flags);
static целое apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, целое force, целое group, buf_T *buf, exarg_T *eap);
static void auto_next_pat(AutoPatCmd_T *apc, целое stop_at_last);
static целое au_find_group(char_u *name);

static event_T	last_event;
static целое	last_group;
static целое	autocmd_blocked = 0;	// block all autocmds

    static char_u *
get_deleted_augroup(void)
{
    если (deleted_augroup == NULL)
	deleted_augroup = (char_u *)_("--Deleted--");
    вернуть deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    static void
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    если (got_int)
	вернуть;
    если (ap->pat == NULL)		// pattern has been removed
	вернуть;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    msg_putchar('\n');
    если (got_int)
	goto theend;
    если (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	если (ap->group != AUGROUP_DEFAULT)
	{
	    если (AUGROUP_NAME(ap->group) == NULL)
		msg_puts_attr((char *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    иначе
		msg_puts_attr((char *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    msg_puts("  ");
	}
	event_name = event_nr2name(event);
	msg_puts_attr((char *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	msg_putchar('\n');
	если (got_int)
	    goto theend;
    }

    если (ap->pat == NULL)
	goto theend;  // timer might have cleared the pattern or group

    msg_col = 4;
    msg_outtrans(ap->pat);

    для (ac = ap->cmds; ac != NULL; ac = ac->next)
    {
	если (ac->cmd == NULL)		// skip removed commands
	    continue;

	если (msg_col >= 14)
	    msg_putchar('\n');
	msg_col = 14;
	если (got_int)
	    goto theend;
	msg_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	если (p_verbose > 0)
	    last_set_msg(ac->script_ctx);
#endif
	если (got_int)
	    goto theend;
	если (ac->next != NULL)
	{
	    msg_putchar('\n');
	    если (got_int)
		goto theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    static void
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    static void
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    для (ac = ap->cmds; ac != NULL; ac = ac->next)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
static void au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    static void
au_cleanup(void)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    если (autocmd_busy || !au_need_clean)
	вернуть;

    // loop over all events
    для (event = (event_T)0; (целое)event < NUM_EVENTS;
					    event = (event_T)((целое)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(целое)event]);
	для (ap = *prev_ap; ap != NULL; ap = *prev_ap)
	{
	    целое has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    для (ac = *prev_ac; ac != NULL; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		если (ap->pat == NULL || ac->cmd == NULL)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		иначе
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    если (ap->pat != NULL && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    если (ap->pat == NULL)
	    {
		если (ap->next == NULL)
		{
		    если (prev_ap == &(first_autopat[(целое)event]))
			last_autopat[(целое)event] = NULL;
		    иначе
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(целое)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    иначе
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    void
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    для (apc = active_apc_list; apc; apc = apc->next)
	если (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    для (event = (event_T)0; (целое)event < NUM_EVENTS;
					    event = (event_T)((целое)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    если (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		если (p_verbose >= 6)
		{
		    string_T	*event_name;

		    verbose_enter();
		    event_name = event_nr2name(event);
		    smsg(_("auto-removing autocommand: %s <buffer=%d>"),
				       event_name->string, buf->b_fnum);
		    verbose_leave();
		}
	    }
    au_cleanup();
}

/*
 * Add an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    static целое
au_new_group(char_u *name)
{
    целое		i;

    i = au_find_group(name);
    если (i != AUGROUP_ERROR)
	вернуть i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    для (i = 0; i < augroups.ga_len; ++i)
	если (AUGROUP_NAME(i) == NULL)
	    break;
    если (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	вернуть AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    если (AUGROUP_NAME(i) == NULL)
	вернуть AUGROUP_ERROR;
    если (i == augroups.ga_len)
	++augroups.ga_len;

    вернуть i;
}

    static void
au_del_group(char_u *name)
{
    целое		i;
    event_T	event;
    AutoPat	*ap;
    целое		in_use = FALSE;


    i = au_find_group(name);
    если (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	вернуть;
    }
    если (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	вернуть;
    }

    для (event = (event_T)0; (целое)event < NUM_EVENTS;
	    event = (event_T)((целое)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    если (ap->group == i && ap->pat != NULL)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = NUM_EVENTS;
		break;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    если (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    иначе
	AUGROUP_NAME(i) = NULL;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    static целое
au_find_group(char_u *name)
{
    целое	    i;

    для (i = 0; i < augroups.ga_len; ++i)
	если (AUGROUP_NAME(i) != NULL && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    вернуть i;
    вернуть AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    целое
au_has_group(char_u *name)
{
    вернуть au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    void
do_augroup(char_u *arg, целое del_group)
{
    целое	    i;

    если (del_group)
    {
	если (*arg == NUL)
	    emsg(_(e_argument_required));
	иначе
	    au_del_group(arg);
    }
    иначе если (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    иначе если (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	если (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    иначе			    // ":aug": list the group names
    {
	msg_start();
	для (i = 0; i < augroups.ga_len; ++i)
	{
	    если (AUGROUP_NAME(i) != NULL)
	    {
		msg_puts((char *)AUGROUP_NAME(i));
		msg_puts("  ");
	    }
	}
	msg_clr_eos();
	msg_end();
    }
}

    void
autocmd_init(void)
{
    CLEAR_FIELD(aucmd_win);
}

#если defined(EXITFREE)
    void
free_all_autocmds(void)
{
    char_u	*s;

    для (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(NULL, (char_u *)"", TRUE);

    для (целое i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	если (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    целое
is_aucmd_win(win_T *win)
{
    для (целое i = 0; i < AUCMD_WIN_COUNT; ++i)
	если (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    вернуть TRUE;
    вернуть FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    static event_T
event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    keyvalue_T target;
    keyvalue_T *entry;
    static keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_INDEX];
    static keyvalue_T *bufread = &event_tab[BUFREAD_INDEX];

    // the event name ends with end of line, '|', a blank or a comma
    для (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.key = 0;
    target.value.string = start;
    target.value.length = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    если (cmp_keyvalue_value_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    иначе если (cmp_keyvalue_value_ni(&target, bufread) == 0)
	entry = bufread;
    иначе
	entry = (keyvalue_T *)bsearch(&target, &event_tab, NUM_EVENTS,
				sizeof(event_tab[0]), cmp_keyvalue_value_ni);

    если (*p == ',')
	++p;
    *end = p;

    вернуть (entry == NULL) ? NUM_EVENTS : (event_T)abs(entry->key);
}

/*
 * Return the name for event "event".
 */
    static string_T *
event_nr2name(event_T event)
{
    целое	    i;
    enum {CACHE_SIZE = 12};
    static целое cache_tab[CACHE_SIZE];
    static целое cache_last_index = -1;
    static string_T unknown = STR_LITERAL_INIT("Unknown");

    если (cache_last_index < 0)
    {
	для (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    для (i = cache_last_index; cache_tab[i] >= 0; )
    {
	если ((event_T)abs(event_tab[cache_tab[i]].key) == event)
	    вернуть &event_tab[cache_tab[i]].value;

	если (i == 0)
	    i = CACHE_SIZE - 1;
	иначе
	    --i;

	// are we back at the start?
	если (i == cache_last_index)
	    break;
    }

    // look in the event table itself
    для (i = 0; i < NUM_EVENTS; ++i)
    {
	если ((event_T)abs(event_tab[i].key) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    если (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    иначе
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    вернуть &event_tab[i].value;
	}
    }

    вернуть &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    static char_u *
find_end_event(
    char_u  *arg,
    целое	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    если (*arg == '*')
    {
	если (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    вернуть NULL;
	}
	pat = arg + 1;
    }
    иначе
    {
	для (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    если ((целое)event_name2nr(pat, &p) >= NUM_EVENTS)
	    {
		если (have_group)
		    semsg(_(e_no_such_event_str), pat);
		иначе
		    semsg(_(e_no_such_group_or_event_str), pat);
		вернуть NULL;
	    }
	}
    }
    вернуть pat;
}

/*
 * Find the command of an ":autocmd" with argument "arg", the part after the
 * events, the pattern and any "++once"/"++nested".  Returns NULL when there is
 * no command.  Does not modify "arg" and gives no error messages.
 */
    char_u *
au_find_cmd_arg(char_u *arg)
{
    char_u	*pat;
    char_u	*cmd;
    целое		group;

    если (*arg == '|')
	вернуть NULL;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? NULL : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    если (pat == NULL)
	вернуть NULL;

    pat = skipwhite(pat);
    если (*pat == NUL || *pat == '|')
	вернуть NULL;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    пока (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    для (целое i = 0; i < 2; ++i)
    {
	если (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	если (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	если (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    вернуть *cmd == NUL ? NULL : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    целое
event_ignored(event_T event, char_u *ei)
{
    целое ignored = FALSE;
    пока (*ei != NUL)
    {
	целое unignore = *ei == '-';
	ei += unignore;
	если (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].key <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	иначе если (event_name2nr(ei, &ei) == event)
	{
	    если (unignore)
		вернуть FALSE;
	    иначе
		ignored = TRUE;
	}
    }

    вернуть ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    целое
check_ei(char_u *ei)
{
    целое	win = ei != p_ei;

    пока (*ei)
    {
	если (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	иначе
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    если (event == NUM_EVENTS || (win && event_tab[event].key > 0))
		вернуть FAIL;
	}
    }

    вернуть OK;
}

#если defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(char *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    если (save_ei == NULL)
	вернуть NULL;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    если (new_ei == NULL)
    {
	vim_free(save_ei);
	вернуть NULL;
    }

    если (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    иначе
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    вернуть save_ei;
}

    void
au_event_restore(char_u *old_ei)
{
    если (old_ei != NULL)
    {
	set_string_option_direct((char_u *)"ei", -1, old_ei,
							  OPT_FREE, SID_NONE);
	vim_free(old_ei);
    }
}
#endif  // FEAT_SYN_HL

/*
 * do_autocmd() -- implements the :autocmd command.  Can be used in the
 *  following ways:
 *
 * :autocmd <event> <pat> <cmd>	    Add <cmd> to the list of commands that
 *				    will be automatically executed for <event>
 *				    when editing a file matching <pat>, in
 *				    the current group.
 * :autocmd <event> <pat>	    Show the autocommands associated with
 *				    <event> and <pat>.
 * :autocmd <event>		    Show the autocommands associated with
 *				    <event>.
 * :autocmd			    Show all autocommands.
 * :autocmd! <event> <pat> <cmd>    Remove all autocommands associated with
 *				    <event> and <pat>, and add the command
 *				    <cmd>, for the current group.
 * :autocmd! <event> <pat>	    Remove all autocommands associated with
 *				    <event> and <pat> for the current group.
 * :autocmd! <event>		    Remove all autocommands associated with
 *				    <event> for the current group.
 * :autocmd!			    Remove ALL autocommands for the current
 *				    group.
 *
 *  Multiple events and patterns may be given separated by commas.  Here are
 *  some examples:
 * :autocmd bufread,bufenter *.c,*.h	set tw=0 smartindent noic
 * :autocmd bufleave	     *		set tw=79 nosmartindent ic infercase
 *
 * :autocmd * *.c		show all autocommands for *.c files.
 *
 * Mostly a {group} argument can optionally appear before <event>.
 * "eap" can be NULL.
 */
    void
do_autocmd(exarg_T *eap, char_u *arg_in, целое forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = NULL;
    char_u	*cmd;
    целое		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = NULL;
    целое		nested = FALSE;
    целое		once = FALSE;
    целое		group;
    целое		i;
    целое		flags = 0;

    если (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    иначе
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	если (arg == NULL)	    // out of memory
	    вернуть;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    если (pat == NULL)
	вернуть;

    pat = skipwhite(pat);
    если (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    иначе
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	пока (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	если (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	если (vim_strchr(pat, '$') != NULL || vim_strchr(pat, '~') != NULL)
	{
#ifdef BACKSLASH_IN_FILENAME
	    целое	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    если (envpat != NULL)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	для (i = 0; i < 2; i++)
	{
	    если (*cmd == NUL)
		continue;

	    // Check for "++once" flag.
	    если (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		если (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    goto err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    если ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		если (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    goto err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    если (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		если (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    goto err_exit;
		}
		если (nested)
		{
		    semsg(_(e_duplicate_argument_str), "nested");
		    goto err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	если (*cmd != NUL)
	{
	    если (eap != NULL)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    если (cmd == NULL)	    // some error
		goto err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    если (!forceit && *cmd == NUL)
	// Highlight title
	msg_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    если (*arg == '*' || *arg == NUL || *arg == '|')
    {
	если (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	иначе
	    для (event = (event_T)0; (целое)event < NUM_EVENTS;
					     event = (event_T)((целое)event + 1))
		если (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    break;
    }
    иначе
    {
	пока (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    если (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		break;
    }

err_exit:
    если (cmd_need_free)
	vim_free(cmd);
    vim_free(tofree);
    vim_free(envpat);
}

/*
 * Find the group ID in a ":autocmd" or ":doautocmd" argument.
 * The "argp" argument is advanced to the following argument.
 *
 * Returns the group ID, AUGROUP_ERROR for error (out of memory).
 */
    static целое
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    целое		group = AUGROUP_ALL;

    для (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    если (p <= arg)
	вернуть AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    если (group_name == NULL)		// out of memory
	вернуть AUGROUP_ERROR;
    group = au_find_group(group_name);
    если (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    иначе
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    вернуть group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    static целое
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    целое		once,
    целое		nested,
    char_u	*cmd,
    целое		forceit,
    целое		group,
    целое		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    целое		brace_level;
    char_u	*endpat;
    целое		findgroup;
    целое		allgroups;
    целое		patlen;
    целое		is_buflocal;
    целое		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    если (group == AUGROUP_ALL)
	findgroup = current_augroup;
    иначе
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    если (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    если (forceit)  // delete the AutoPat, if it's in the current group
	    {
		если (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    иначе если (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    для ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	для (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    если (*endpat == '{')
		brace_level++;
	    иначе если (*endpat == '}')
		brace_level--;
	}
	если (pat == endpat)		// ignore single comma
	    continue;
	patlen = (целое)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	если (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    если (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    иначе если (patlen > 9 && pat[7] == '=')
	    {
		если (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		иначе если (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((char *)pat + 8);
	    }
	}

	если (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((char *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (целое)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	если (!forceit && *cmd != NUL && last_autopat[(целое)event] != NULL)
	    prev_ap = &last_autopat[(целое)event];
	иначе
	    prev_ap = &first_autopat[(целое)event];
	пока ((ap = *prev_ap) != NULL)
	{
	    если (ap->pat != NULL)
	    {
		/*
		 * Accept a pattern when:
		 * - a group was specified and it's that group, or a group was
		 *   not specified and it's the current group, or a group was
		 *   not specified and we are listing
		 * - the length of the pattern matches
		 * - the pattern matches.
		 * For <buffer[=X]>, this condition works because we normalize
		 * all buffer-local patterns.
		 */
		если ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    если (forceit)
		    {
			если (*cmd != NUL && ap->next == NULL)
			{
			    au_remove_cmds(ap);
			    break;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    иначе если (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    иначе если (ap->next == NULL)
			break;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	если (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    если (ap == NULL)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		если (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == NULL))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_number), buflocal_nr);
		    вернуть FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		если (ap == NULL)
		    вернуть FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		если (ap->pat == NULL)
		{
		    vim_free(ap);
		    вернуть FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		если (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		если ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
			&& !(has_winscrolled() || has_winresized()))
		{
		    tabpage_T *save_curtab = curtab;
		    tabpage_T *tp;
		    FOR_ALL_TABPAGES(tp)
		    {
			unuse_tabpage(curtab);
			use_tabpage(tp);
			snapshot_windows_scroll_size();
		    }
		    unuse_tabpage(curtab);
		    use_tabpage(save_curtab);
		}

		если (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = NULL;
		}
		иначе
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    если (reg_pat != NULL)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    если (reg_pat == NULL || ap->reg_prog == NULL)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			вернуть FAIL;
		    }
		}
		ap->cmds = NULL;
		*prev_ap = ap;
		last_autopat[(целое)event] = ap;
		ap->next = NULL;
		если (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		иначе
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    пока ((ac = *prev_ac) != NULL)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    если (ac == NULL)
		вернуть FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    если (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    если (ac->cmd == NULL)
	    {
		vim_free(ac);
		вернуть FAIL;
	    }
	    ac->next = NULL;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    вернуть OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    целое
do_doautocmd(
    char_u	*arg_start,
    целое		do_msg,	    // give message for no matching autocmds?
    целое		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    целое		nothing_done = TRUE;
    целое		group;

    если (did_something != NULL)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    если (arg == NULL)	    // out of memory
	вернуть FAIL;

    если (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	вернуть FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    если (fname == NULL)
	вернуть FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    пока (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	если (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, NULL, TRUE, group, curbuf, NULL))
	    nothing_done = FALSE;

    если (nothing_done && do_msg
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    если (did_something != NULL)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    вернуть aborting() ? FAIL : OK;
#иначе
    вернуть OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    void
ex_doautoall(exarg_T *eap)
{
    целое		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    целое		call_do_modelines = check_nomodeline(&arg);
    целое		did_aucmd;

    /*
     * This is a bit tricky: For some commands curwin->w_buffer needs to be
     * equal to curbuf, but for some buffers there may not be a window.
     * So we change the buffer for the current window for a moment.  This
     * gives problems when the autocommands make changes to the list of
     * buffers or windows...
     */
    FOR_ALL_BUFFERS(buf)
    {
	// Only do loaded buffers and skip the current buffer, it's done last.
	если (buf->b_ml.ml_mfp == NULL || buf == curbuf)
	    continue;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	если (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    break;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	если (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	если (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    break;
	}
    }

    // Execute autocommands for the current buffer last.
    если (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	если (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    целое
check_nomodeline(char_u **argp)
{
    если (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	вернуть FALSE;
    }
    вернуть TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    void
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    целое		save_ea;
#ifdef FEAT_AUTOCHDIR
    целое		save_acd;
#endif
    целое		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    если (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    иначе
	FOR_ALL_WINDOWS(win)
	    если (win->w_buffer == buf)
		break;

    // Allocate a window when needed.
    win_T *auc_win = NULL;
    целое auc_idx = AUCMD_WIN_COUNT;
    если (win == NULL)
    {
	для (auc_idx = 0; auc_idx < AUCMD_WIN_COUNT; ++auc_idx)
	    если (!aucmd_win[auc_idx].auc_win_used)
	    {
		если (aucmd_win[auc_idx].auc_win == NULL)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		если (auc_win != NULL)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		break;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	если (auc_win == NULL)
	    вернуть;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == NULL ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    если (bt_prompt(curbuf))
	aco->save_prompt_insert = curbuf->b_prompt_insert;
#endif

    если (win != NULL)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    иначе
    {
	// There is no window for "buf", use "auc_win".  To minimize the side
	// effects, insert it in the current tab page.
	// Anything related to a window (e.g., setting folds) may have
	// unexpected results.
	aco->use_aucmd_win_idx = auc_idx;

	win_init_popup_win(auc_win, buf);

	// Make sure tp_localdir and globaldir are NULL to avoid a
	// chdir() in win_enter_ext().
	// win_init_popup_win() has already set w_localdir to NULL.
	aco->tp_localdir = curtab->tp_localdir;
	curtab->tp_localdir = NULL;
	aco->globaldir = globaldir;
	globaldir = NULL;

	// Split the current window, put the auc_win in the upper half.
	// We don't want the BufEnter or WinEnter autocommands.
	block_autocmds();
	make_snapshot(SNAP_AUCMD_IDX);
	save_ea = p_ea;
	p_ea = FALSE;

#ifdef FEAT_AUTOCHDIR
	// Prevent chdir() call in win_enter_ext(), through do_autochdir().
	save_acd = p_acd;
	p_acd = FALSE;
#endif

	(void)win_split_ins(0, WSP_TOP | WSP_FORCE_ROOM, auc_win, 0, NULL);
	win_comp_pos();   // recompute window positions
	p_ea = save_ea;
#ifdef FEAT_AUTOCHDIR
	p_acd = save_acd;
#endif
	unblock_autocmds();
	curwin = auc_win;
    }
    curbuf = buf;
    aco->new_curwin_id = curwin->w_id;
    set_bufref(&aco->new_curbuf, curbuf);

    aco->save_VIsual_active = VIsual_active;
    если (!same_buffer)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_active = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    void
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    целое	    dummy;
    win_T   *save_curwin;

    если (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	если (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		если (wp == awp)
		{
		    если (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    goto win_found;
		}
	    }
	}
win_found:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(void)winframe_remove(curwin, &dummy, NULL, NULL);
	win_remove(curwin, NULL);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_used = FALSE;
	last_status(FALSE);	    // may need to remove last status line

	если (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	если (save_curwin != NULL)
	    curwin = save_curwin;
	иначе
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	если (bt_prompt(curbuf))
	    curbuf->b_prompt_insert = aco->save_prompt_insert;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	если (awp->w_localdir != NULL)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	если (curwin->w_topline > curbuf->b_ml.ml_line_count)
	{
	    curwin->w_topline = curbuf->b_ml.ml_line_count;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#если defined(FEAT_GUI)
	если (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_LEFT], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_RIGHT], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    иначе
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	если (save_curwin != NULL)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    если (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != NULL)
	    {
#если defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		если (curwin->w_s == &curbuf->b_s)
		    curwin->w_s = &aco->new_curbuf.br_buf->b_s;
#endif
		--curbuf->b_nwindows;
		curbuf = aco->new_curbuf.br_buf;
		curwin->w_buffer = curbuf;
		++curbuf->b_nwindows;
	    }

	    curwin = save_curwin;
	    curbuf = curwin->w_buffer;
	    prevwin = win_find_by_id(aco->save_prevwin_id);

	    // In case the autocommand moves the cursor to a position that
	    // does not exist in curbuf.
	    VIsual_active = aco->save_VIsual_active;
	    check_cursor();
	}
    }

    VIsual_active = aco->save_VIsual_active;
    check_cursor();	    // just in case lines got deleted
    если (VIsual_active)
	check_pos(curbuf, &VIsual);
}

static целое	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    целое
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    целое		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    вернуть apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, NULL);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    целое
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    целое		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    вернуть apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    целое
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    целое		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    целое		*retval)    // pointer to caller's retval
{
    целое		did_cmd;

#ifdef FEAT_EVAL
    если (should_abort(*retval))
	вернуть FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, NULL);
    если (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    вернуть did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    static целое
has_cursorhold(void)
{
    вернуть (first_autopat[(целое)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != NULL);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    целое
trigger_cursorhold(void)
{
    целое		state;

    если (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	если (state == MODE_NORMAL_BUSY || (state & MODE_INSERT) != 0)
	    вернуть TRUE;
    }
    вернуть FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    целое
has_winresized(void)
{
    вернуть (first_autopat[(целое)EVENT_WINRESIZED] != NULL);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    целое
has_winscrolled(void)
{
    вернуть (first_autopat[(целое)EVENT_WINSCROLLED] != NULL);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    целое
has_cursormoved(void)
{
    вернуть (first_autopat[(целое)EVENT_CURSORMOVED] != NULL);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    целое
has_cursormovedI(void)
{
    вернуть (first_autopat[(целое)EVENT_CURSORMOVEDI] != NULL);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    целое
has_textchanged(void)
{
    вернуть (first_autopat[(целое)EVENT_TEXTCHANGED] != NULL);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    целое
has_textchangedI(void)
{
    вернуть (first_autopat[(целое)EVENT_TEXTCHANGEDI] != NULL);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    целое
has_textchangedP(void)
{
    вернуть (first_autopat[(целое)EVENT_TEXTCHANGEDP] != NULL);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    целое
has_insertcharpre(void)
{
    вернуть (first_autopat[(целое)EVENT_INSERTCHARPRE] != NULL);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    целое
has_keyinputpre(void)
{
    вернуть (first_autopat[(целое)EVENT_KEYINPUTPRE] != NULL);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    целое
has_cmdundefined(void)
{
    вернуть (first_autopat[(целое)EVENT_CMDUNDEFINED] != NULL);
}

#если defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    целое
has_textputpost(void)
{
    вернуть (first_autopat[(целое)EVENT_TEXTPUTPOST] != NULL);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    целое
has_textputpre(void)
{
    вернуть (first_autopat[(целое)EVENT_TEXTPUTPRE] != NULL);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    целое
has_textyankpost(void)
{
    вернуть (first_autopat[(целое)EVENT_TEXTYANKPOST] != NULL);
}
#endif

#если defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    целое
has_completechanged(void)
{
    вернуть (first_autopat[(целое)EVENT_COMPLETECHANGED] != NULL);
}
#endif

#если defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    целое
has_modechanged(void)
{
    вернуть (first_autopat[(целое)EVENT_MODECHANGED] != NULL);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    static целое
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    целое		force,	     // when TRUE, ignore autocmd_busy
    целое		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = NULL;	// short file name
    char_u	*tail;
    целое		save_changed;
    buf_T	*old_curbuf;
    целое		retval = FALSE;
    char_u	*save_autocmd_fname;
    целое		save_autocmd_fname_full;
    целое		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    целое		save_autocmd_busy;
    целое		save_autocmd_nested;
    static целое	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    static целое	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    целое		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    целое		save_KeyTyped = KeyTyped;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    если (event == NUM_EVENTS || first_autopat[(целое)event] == NULL
	    || autocmd_blocked > 0)
	goto BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    если (autocmd_busy && !(force || autocmd_nested))
	goto BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    если (aborting())
	goto BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    если (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	goto BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    если (event_ignored(event, p_ei))
	goto BYPASS_AU;

    целое win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    если (buf == curbuf && event_tab[event].key <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    иначе если (buf != NULL && event_tab[event].key <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    если (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		break;
	    }
    }
    если (win_ignore)
	goto BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    если (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	goto BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    если (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	goto BYPASS_AU;

    если (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_changed_count;

    /*
     * Save the autocmd_* variables and info about the current buffer.
     */
    save_autocmd_fname = autocmd_fname;
    save_autocmd_fname_full = autocmd_fname_full;
    save_autocmd_bufnr = autocmd_bufnr;
    save_autocmd_match = autocmd_match;
    save_autocmd_busy = autocmd_busy;
    save_autocmd_nested = autocmd_nested;
    save_changed = curbuf->b_changed;
    old_curbuf = curbuf;

    /*
     * Set the file name to be used for <afile>.
     * Make a copy to avoid that changing a buffer name or directory makes it
     * invalid.
     */
    если (fname_io == NULL)
    {
	если (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = NULL;
	иначе если (fname != NULL && !ends_excmd(*fname))
	    autocmd_fname = fname;
	иначе если (buf != NULL)
	    autocmd_fname = buf->b_ffname;
	иначе
	    autocmd_fname = NULL;
    }
    иначе
	autocmd_fname = fname_io;
    если (autocmd_fname != NULL)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    если (buf == NULL)
	autocmd_bufnr = 0;
    иначе
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    если (fname == NULL || *fname == NUL)
    {
	если (buf == NULL)
	    fname = NULL;
	иначе
	{
#ifdef FEAT_SYN_HL
	    если (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    иначе
#endif
		если (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		иначе
		{
		    если (buf->b_sfname != NULL)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	если (fname == NULL)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    иначе
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	если (event == EVENT_FILETYPE
		|| event == EVENT_SYNTAX
		|| event == EVENT_CMDLINECHANGED
		|| event == EVENT_CMDLINEENTER
		|| event == EVENT_CMDLINELEAVEPRE
		|| event == EVENT_CMDLINELEAVE
		|| event == EVENT_CURSORMOVEDC
		|| event == EVENT_CMDWINENTER
		|| event == EVENT_CMDWINLEAVE
		|| event == EVENT_CMDUNDEFINED
		|| event == EVENT_FUNCUNDEFINED
		|| event == EVENT_KEYINPUTPRE
		|| event == EVENT_REMOTEREPLY
		|| event == EVENT_SPELLFILEMISSING
		|| event == EVENT_QUICKFIXCMDPRE
		|| event == EVENT_COLORSCHEME
		|| event == EVENT_COLORSCHEMEPRE
		|| event == EVENT_OPTIONSET
		|| event == EVENT_QUICKFIXCMDPOST
		|| event == EVENT_DIRCHANGED
		|| event == EVENT_DIRCHANGEDPRE
		|| event == EVENT_MODECHANGED
		|| event == EVENT_MENUPOPUP
		|| event == EVENT_USER
		|| event == EVENT_WINCLOSED
		|| event == EVENT_WINRESIZED
		|| event == EVENT_WINSCROLLED
		|| event == EVENT_TERMRESPONSEALL)
	{
	    fname = vim_strsave(fname);
	    autocmd_fname_full = TRUE; // don't expand it later
	}
	иначе
	    fname = FullName_save(fname, FALSE);
    }
    если (fname == NULL)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	goto BYPASS_AU;
    }

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    если (sfname != NULL)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    если (sfname != NULL)
	vms_remove_version(sfname);
    vms_remove_version(fname);
#endif

    /*
     * Set the name to be used for <amatch>.
     */
    autocmd_match = fname;


    // Don't redraw while doing autocommands.
    ++RedrawingDisabled;

    // name and lnum are filled in later
    estack_push(ETYPE_AUCMD, NULL, 0);
    ESTACK_CHECK_SETUP;

    save_current_sctx = current_sctx;

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    если (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    если (!autocmd_busy)
    {
	save_search_patterns();
	если (!ins_compl_active())
	{
	    saveRedobuff(&save_redo);
	    did_save_redobuff = TRUE;
	}
	curbuf->b_did_filetype = curbuf->b_keep_filetype;
    }

    /*
     * Note that we are applying autocmds.  Some commands need to know.
     */
    autocmd_busy = TRUE;
    filechangeshell_busy = (event == EVENT_FILECHANGEDSHELL);
    ++nesting;		// see matching decrement below

    // Remember that FileType was triggered.  Used for did_filetype().
    если (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(целое)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    если (patcmd.curpat != NULL)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	если (eap != NULL)
	{
	    save_cmdarg = set_cmdarg(eap, NULL);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	иначе
	    save_cmdarg = NULL;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	для (ap = patcmd.curpat; ap->next != NULL; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	если (nesting == 1)
	    check_lnums(TRUE);
	иначе
	    check_lnums_nested(TRUE);

	целое save_did_emsg = did_emsg;
	целое save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(NULL, getnextac, (void *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	если (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	если (eap != NULL)
	{
	    (void)set_cmdarg(NULL, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	если (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    если (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_pop();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_full = save_autocmd_fname_full;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_match = save_autocmd_match;
    current_sctx = save_current_sctx;
#ifdef FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    если (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif
    KeyTyped = save_KeyTyped;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    если (!autocmd_busy)
    {
	restore_search_patterns();
	если (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	пока (au_pending_free_buf != NULL)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	пока (au_pending_free_win != NULL)
	{
	    win_T *w = au_pending_free_win->w_next;

	    vim_free(au_pending_free_win);
	    au_pending_free_win = w;
	}
    }

    /*
     * Some events don't set or reset the Changed flag.
     * Check if still in the same buffer!
     */
    если (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	если (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    если (event == EVENT_BUFWIPEOUT && buf != NULL)
	aubuflocal_remove(buf);

    если (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    вернуть retval;
}

#ifdef FEAT_EVAL
static char_u	*old_termresponse = NULL;
static char_u	*old_termu7resp = NULL;
static char_u	*old_termblinkresp = NULL;
static char_u	*old_termrbgresp = NULL;
static char_u	*old_termrfgresp = NULL;
static char_u	*old_termstyleresp = NULL;
#endif

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    void
block_autocmds(void)
{
#ifdef FEAT_EVAL
    // Remember the value of v:termresponse.
    если (autocmd_blocked == 0)
    {
	old_termresponse = get_vim_var_str(VV_TERMRESPONSE);
	old_termu7resp = get_vim_var_str(VV_TERMU7RESP);
	old_termblinkresp = get_vim_var_str(VV_TERMBLINKRESP);
	old_termrbgresp = get_vim_var_str(VV_TERMRBGRESP);
	old_termrfgresp = get_vim_var_str(VV_TERMRFGRESP);
	old_termstyleresp = get_vim_var_str(VV_TERMSTYLERESP);
    }
#endif
    ++autocmd_blocked;
}

    void
unblock_autocmds(void)
{
    --autocmd_blocked;

#ifdef FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    если (autocmd_blocked == 0)
    {
	если (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, NULL, NULL, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", NULL, FALSE, curbuf);
	}
	если (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", NULL, FALSE, curbuf);
	}
	если (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", NULL, FALSE, curbuf);
	}
	если (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", NULL, FALSE, curbuf);
	}
	если (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", NULL, FALSE, curbuf);
	}
	если (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", NULL, FALSE, curbuf);
	}
    }
#endif
}

    целое
is_autocmd_blocked(void)
{
    вернуть autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    static void
auto_next_pat(
    AutoPatCmd_T *apc,
    целое		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = NULL;

    для (ap = apc->curpat; ap != NULL && !got_int; ap = ap->next)
    {
	apc->curpat = NULL;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	если (ap->pat != NULL && ap->cmds != NULL
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    если (ap->buflocal_nr == 0
		    ? (match_file_pat(NULL, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		char	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->length + ap->patlen + 1);
		если (namep != NULL)
		{
		    sprintf((char *)namep, fmt, (char *)event_name->string, (char *)ap->pat);
		    если (p_verbose >= 8)
		    {
			verbose_enter();
			smsg(_("Executing %s"), namep);
			verbose_leave();
		    }
		}

		// Update the exestack entry for this autocmd.
		entry->es_name = namep;
		entry->es_info.aucmd = apc;

		apc->curpat = ap;
		apc->nextcmd = ap->cmds;
		// mark last command
		для (cp = ap->cmds; cp->next != NULL; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    line_breakcheck();
	    если (apc->curpat != NULL)	    // found a match
		break;
	}
	если (stop_at_last && ap->last)
	    break;
    }
}

#если defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    вернуть &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	целое c UNUSED,
	void *cookie,
	целое indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    если (acp->curpat == NULL)
	вернуть NULL;

    // repeat until we find an autocommand to execute
    для (;;)
    {
	// skip removed commands
	пока (acp->nextcmd != NULL && acp->nextcmd->cmd == NULL)
	    если (acp->nextcmd->last)
		acp->nextcmd = NULL;
	    иначе
		acp->nextcmd = acp->nextcmd->next;

	если (acp->nextcmd != NULL)
	    break;

	// at end of commands, find next pattern that matches
	если (acp->curpat->last)
	    acp->curpat = NULL;
	иначе
	    acp->curpat = acp->curpat->next;
	если (acp->curpat != NULL)
	    auto_next_pat(acp, TRUE);
	если (acp->curpat == NULL)
	    вернуть NULL;
    }

    ac = acp->nextcmd;

    если (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	msg_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    если (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    если (ac->last)
	acp->nextcmd = NULL;
    иначе
	acp->nextcmd = ac->next;
    вернуть retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    static целое
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    целое		retval = FALSE;

    если (first_autopat[(целое)event] == NULL)
	вернуть FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    если (fname == NULL)
	вернуть FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    если (sfname != NULL)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	если (ap->pat != NULL && ap->cmds != NULL
	      && (ap->buflocal_nr == 0
		? match_file_pat(NULL, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != NULL && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    break;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    вернуть retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    целое
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    вернуть has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    целое
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    вернуть has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, целое idx)
{
    если (idx == augroups.ga_len)		// add "END" add the end
	вернуть (char_u *)"END";
    если (idx < 0 || idx >= augroups.ga_len)	// end of list
	вернуть NULL;
    если (AUGROUP_NAME(idx) == NULL || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	вернуть (char_u *)"";
    вернуть AUGROUP_NAME(idx);		// return a name
}

static целое include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    целое		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    целое		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    если (group == AUGROUP_ERROR)
	вернуть NULL;
    // If there only is a group name that's what we expand.
    если (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    для (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	если (*p == ',')
	    arg = p + 1;
    если (*p == NUL)
    {
	если (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	вернуть NULL;
    }

    // skip over pattern
    arg = skipwhite(p);
    пока (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    если (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	для (целое i = 0; i < 2; i++)
	{
	    если (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    если (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    если (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	вернуть arg;			    // expand (next) command
    }

    если (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    иначе
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    вернуть NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, целое idx)
{
    целое i;

    если (idx < augroups.ga_len)		// First list group names, if wanted
    {
	если (!include_groups || AUGROUP_NAME(idx) == NULL
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    вернуть (char_u *)"";	// skip deleted entries
	вернуть AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    если (i < 0 || i >= NUM_EVENTS)
	вернуть NULL;

    вернуть event_tab[i].value.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, целое idx, целое win)
{
    если (idx < 0 || idx >= NUM_EVENTS)
	вернуть NULL;

    если (!win)
	вернуть event_tab[idx].value.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    целое j = 0;
    для (целое i = 0; i < NUM_EVENTS; ++i)
    {
	j += event_tab[i].key <= 0;
	если (j == idx + 1)
	    вернуть event_tab[i].value.string;
    }

    вернуть NULL;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    целое
has_tabclosedpre(void)
{
    вернуть (first_autopat[(целое)EVENT_TABCLOSEDPRE] != NULL);
}

#если defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    целое
autocmd_supported(char_u *name)
{
    char_u *p;

    вернуть (event_name2nr(name, &p) != NUM_EVENTS);
}

/*
 * Return TRUE if an autocommand is defined for a group, event and
 * pattern:  The group can be omitted to accept any group. "event" and "pattern"
 * can be NULL to accept any event and pattern. "pattern" can be NULL to accept
 * any pattern. Buffer-local patterns <buffer> or <buffer=N> are accepted.
 * Used for:
 *	exists("#Group") or
 *	exists("#Group#Event") or
 *	exists("#Group#Event#pat") or
 *	exists("#Event") or
 *	exists("#Event#pat")
 */
    целое
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = NULL;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = NULL;
    целое		group;
    целое		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    если (arg_save == NULL)
	вернуть FALSE;
    p = vim_strchr(arg_save, '#');
    если (p != NULL)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    если (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    иначе
    {
	если (p == NULL)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    goto theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	если (p != NULL)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    если (event == NUM_EVENTS)
	goto theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(целое)event];
    если (ap == NULL)
	goto theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    если (pattern != NULL && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    для ( ; ap != NULL; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	если (ap->pat != NULL && ap->cmds != NULL
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == NULL
		|| (buflocal_buf == NULL
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    break;
	}

theend:
    vim_free(arg_save);
    вернуть retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    static void
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, целое delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = NULL;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*group_name = NULL;
    целое		group;
    char_u	*pat = NULL;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = NULL;
    char_u	*end;
    целое		once;
    целое		nested;
    целое		replace;		// replace the cmd for a group/event
    целое		retval = VVAL_TRUE;
    целое		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_FALSE;

    если (check_restricted() || check_secure())
	вернуть;

    если (check_for_list_arg(argvars, 0) == FAIL)
	вернуть;

    aucmd_list = argvars[0].vval.v_list;
    если (aucmd_list == NULL)
	вернуть;

    FOR_ALL_LIST_ITEMS(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = NULL;
	event_list = NULL;
	pat = NULL;
	pat_list = NULL;

	если (li->li_tv.v_type != VAR_DICT)
	    continue;

	event_dict = li->li_tv.vval.v_dict;
	если (event_dict == NULL)
	    continue;

	di = dict_find(event_dict, (char_u *)"event", -1);
	если (di != NULL)
	{
	    если (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		если (event_name == NULL)
		{
		    emsg(_(e_string_required));
		    continue;
		}
	    }
	    иначе если (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		если (event_list == NULL)
		{
		    emsg(_(e_list_required));
		    continue;
		}
	    }
	    иначе
	    {
		emsg(_(e_string_or_list_expected));
		continue;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	если (group_name == NULL || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	иначе
	{
	    group = au_find_group(group_name);
	    если (group == AUGROUP_ERROR)
	    {
		если (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    break;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		если (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    break;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	если (dict_has_key(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_number_def(event_dict, "bufnr", -1);
	    если (bnum == -1)
		continue;

	    vim_snprintf((char *)IObuff, IOSIZE, "<buffer=%d>", (целое)bnum);
	    pat = IObuff;
	}
	иначе
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    если (di != NULL)
	    {
		если (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    если (pat == NULL)
		    {
			emsg(_(e_string_required));
			continue;
		    }
		}
		иначе если (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    если (pat_list == NULL)
		    {
			emsg(_(e_list_required));
			continue;
		    }
		}
		иначе
		{
		    emsg(_(e_string_or_list_expected));
		    continue;
		}
	    }
	    иначе если (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	если (cmd == NULL)
	{
	    если (delete)
		cmd = vim_strsave((char_u *)"");
	    иначе
		continue;
	}

	если (delete && (event_name == NULL
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    для (event = (event_T)0; (целое)event < NUM_EVENTS;
		    event = (event_T)((целое)event + 1))
	    {
		если (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    break;
		}
	    }
	}
	иначе
	{
	    char_u *p = NULL;

	    eli = NULL;
	    end = NULL;
	    пока (TRUE)
	    {
		если (event_list != NULL)
		{
		    если (eli == NULL)
			eli = event_list->lv_first;
		    иначе
			eli = eli->li_next;
		    если (eli == NULL)
			break;
		    если (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == NULL)
		    {
			emsg(_(e_string_required));
			break;
		    }
		}
		иначе
		{
		    если (p == NULL)
			p = event_name;
		    если (p == NULL || *p == NUL)
			break;
		}

		event = event_name2nr(p, &end);
		если (event == NUM_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    break;
		}
		если (pat != NULL)
		{
		    если (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			break;
		    }
		}
		иначе если (pat_list != NULL)
		{
		    FOR_ALL_LIST_ITEMS(pat_list, pli)
		    {
			если (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == NULL)
			{
			    emsg(_(e_string_required));
			    continue;
			}
			если (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    break;
			}
		    }
		    если (retval == VVAL_FALSE)
			break;
		}
		если (event_name != NULL)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	если (delete && group_name != NULL &&
		(event_name == NULL || event_name[0] == NUL)
		&& (pat == NULL || pat[0] == NUL)
		&& (cmd == NULL || cmd[0] == NUL))
	    au_del_group(group_name);
    }

    VIM_CLEAR(group_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_number = retval;
}

/*
 * autocmd_add() function
 */
    void
f_autocmd_add(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    void
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    void
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = NUM_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = NULL;
    char_u	*name = NULL;
    целое		group = AUGROUP_ALL;

    если (rettv_list_alloc(rettv) == FAIL)
	вернуть;
    если (check_for_opt_dict_arg(argvars, 0) == FAIL)
	вернуть;

    если (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	если (dict_has_key(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    если (name == NULL)
		вернуть;

	    если (*name == NUL)
		group = AUGROUP_DEFAULT;
	    иначе
	    {
		group = au_find_group(name);
		если (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    вернуть;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	если (dict_has_key(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    если (name == NULL)
		вернуть;

	    если (name[0] == '*' && name[1] == NUL)
		event_arg = NUM_EVENTS;
	    иначе
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.key = 0;
		target.value.string = name;
		target.value.length = STRLEN(target.value.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    NUM_EVENTS, sizeof(event_tab[0]), cmp_keyvalue_value_ni);
		если (entry == NULL)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    вернуть;
		}
		event_arg = (event_T)abs(entry->key);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	если (dict_has_key(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    если (pat == NULL)
		вернуть;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    для (event = (event_T)0; (целое)event < NUM_EVENTS;
	    event = (event_T)((целое)event + 1))
    {
	string_T    *event_name;

	если (event_arg != NUM_EVENTS && event != event_arg)
	    continue;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    если (ap->pat == NULL)		// pattern has been removed
		continue;

	    если (group != AUGROUP_ALL && group != ap->group)
		continue;

	    если (pat != NULL && STRCMP(pat, ap->pat) != 0)
		continue;

	    group_name.string = get_augroup_name(NULL, ap->group);
	    если (group_name.string == NULL)
		STR_LITERAL_SET(group_name, "");
	    иначе
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    для (ac = ap->cmds; ac != NULL; ac = ac->next)
	    {
		event_dict = dict_alloc();
		если (event_dict == NULL
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    вернуть;
		}

		если (dict_add_string_len(event_dict, "event",
			event_name->string, (целое)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (целое)group_name.length) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_add_number(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_add_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_add_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_add_bool(event_dict, "once", ac->once) == FAIL
			|| dict_add_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    вернуть;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
