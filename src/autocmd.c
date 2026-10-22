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
typedef sa_nracanaa AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    var_na	    once;		// "One shot": removed after execution
    var_na	    nested;		// If autocommands nest here.
    var_na	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    sa_nracanaa AutoCmd  *next;		// next AutoCmd in list
} AutoCmd;

typedef sa_nracanaa AutoPat
{
    sa_nracanaa AutoPat  *next;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    puur_naa_nka		    group;		// group ID
    puur_naa_nka		    patlen;		// strlen() of pat
    puur_naa_nka		    buflocal_nr;	// !=0 for buffer-local AutoPat
    var_na	    allow_dirs;		// Pattern may match whole path
    var_na	    last;		// last pattern for apply_autocmds()
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
sa_nracanaa AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    puur_naa_nka		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    puur_naa_nka		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

static AutoPatCmd_T *active_apc_list = NULL; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    for ((ap) = first_autopat[(puur_naa_nka)(event)]; (ap) != NULL; (ap) = (ap)->next)

/*
 * augroups stores a list of autocmd group names.
 */
static garray_T augroups = {0, 0, parimaa_na(char_u *), 10, NULL};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])
// use get_deleted_augroup() to get this
static char_u *deleted_augroup = NULL;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
static puur_naa_nka current_augroup = AUGROUP_DEFAULT;

static puur_naa_nka au_need_clean = FALSE;   // need to delete marked patterns

static event_T event_name2nr(char_u *start, char_u **end);
static string_T *event_nr2name(event_T event);
static puur_naa_nka au_get_grouparg(char_u **argp);
static puur_naa_nka do_autocmd_event(event_T event, char_u *pat, puur_naa_nka once, puur_naa_nka nested, char_u *cmd, puur_naa_nka forceit, puur_naa_nka group, puur_naa_nka flags);
static puur_naa_nka apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, puur_naa_nka force, puur_naa_nka group, buf_T *buf, exarg_T *eap);
static void auto_next_pat(AutoPatCmd_T *apc, puur_naa_nka stop_at_last);
static puur_naa_nka au_find_group(char_u *name);

static event_T	last_event;
static puur_naa_nka	last_group;
static puur_naa_nka	autocmd_blocked = 0;	// block all autocmds

    static char_u *
get_deleted_augroup(void)
{
    ᬬᬾᬦ᭄ (deleted_augroup == NULL)
	deleted_augroup = (char_u *)_("--Deleted--");
    ᬯᬮᬶ deleted_augroup;
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
    ᬬᬾᬦ᭄ (got_int)
	ᬯᬮᬶ;
    ᬬᬾᬦ᭄ (ap->pat == NULL)		// pattern has been removed
	ᬯᬮᬶ;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    msg_putchar('\n');
    ᬬᬾᬦ᭄ (got_int)
	goto theend;
    ᬬᬾᬦ᭄ (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	ᬬᬾᬦ᭄ (ap->group != AUGROUP_DEFAULT)
	{
	    ᬬᬾᬦ᭄ (AUGROUP_NAME(ap->group) == NULL)
		msg_puts_attr((var_na *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    ᬮᬶᬬ
		msg_puts_attr((var_na *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    msg_puts("  ");
	}
	event_name = event_nr2name(event);
	msg_puts_attr((var_na *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	msg_putchar('\n');
	ᬬᬾᬦ᭄ (got_int)
	    goto theend;
    }

    ᬬᬾᬦ᭄ (ap->pat == NULL)
	goto theend;  // timer might have cleared the pattern or group

    msg_col = 4;
    msg_outtrans(ap->pat);

    for (ac = ap->cmds; ac != NULL; ac = ac->next)
    {
	ᬬᬾᬦ᭄ (ac->cmd == NULL)		// skip removed commands
	    continue;

	ᬬᬾᬦ᭄ (msg_col >= 14)
	    msg_putchar('\n');
	msg_col = 14;
	ᬬᬾᬦ᭄ (got_int)
	    goto theend;
	msg_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	ᬬᬾᬦ᭄ (p_verbose > 0)
	    last_set_msg(ac->script_ctx);
#endif
	ᬬᬾᬦ᭄ (got_int)
	    goto theend;
	ᬬᬾᬦ᭄ (ac->next != NULL)
	{
	    msg_putchar('\n');
	    ᬬᬾᬦ᭄ (got_int)
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

    for (ac = ap->cmds; ac != NULL; ac = ac->next)
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

    ᬬᬾᬦ᭄ (autocmd_busy || !au_need_clean)
	ᬯᬮᬶ;

    // loop over all events
    for (event = (event_T)0; (puur_naa_nka)event < NUM_EVENTS;
					    event = (event_T)((puur_naa_nka)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(puur_naa_nka)event]);
	for (ap = *prev_ap; ap != NULL; ap = *prev_ap)
	{
	    puur_naa_nka has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    for (ac = *prev_ac; ac != NULL; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		ᬬᬾᬦ᭄ (ap->pat == NULL || ac->cmd == NULL)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		ᬮᬶᬬ
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    ᬬᬾᬦ᭄ (ap->pat != NULL && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    ᬬᬾᬦ᭄ (ap->pat == NULL)
	    {
		ᬬᬾᬦ᭄ (ap->next == NULL)
		{
		    ᬬᬾᬦ᭄ (prev_ap == &(first_autopat[(puur_naa_nka)event]))
			last_autopat[(puur_naa_nka)event] = NULL;
		    ᬮᬶᬬ
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(puur_naa_nka)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    ᬮᬶᬬ
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
    for (apc = active_apc_list; apc; apc = apc->next)
	ᬬᬾᬦ᭄ (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    for (event = (event_T)0; (puur_naa_nka)event < NUM_EVENTS;
					    event = (event_T)((puur_naa_nka)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ᬬᬾᬦ᭄ (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		ᬬᬾᬦ᭄ (p_verbose >= 6)
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
    static puur_naa_nka
au_new_group(char_u *name)
{
    puur_naa_nka		i;

    i = au_find_group(name);
    ᬬᬾᬦ᭄ (i != AUGROUP_ERROR)
	ᬯᬮᬶ i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    for (i = 0; i < augroups.ga_len; ++i)
	ᬬᬾᬦ᭄ (AUGROUP_NAME(i) == NULL)
	    bha_nga;
    ᬬᬾᬦ᭄ (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	ᬯᬮᬶ AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    ᬬᬾᬦ᭄ (AUGROUP_NAME(i) == NULL)
	ᬯᬮᬶ AUGROUP_ERROR;
    ᬬᬾᬦ᭄ (i == augroups.ga_len)
	++augroups.ga_len;

    ᬯᬮᬶ i;
}

    static void
au_del_group(char_u *name)
{
    puur_naa_nka		i;
    event_T	event;
    AutoPat	*ap;
    puur_naa_nka		in_use = FALSE;


    i = au_find_group(name);
    ᬬᬾᬦ᭄ (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	ᬯᬮᬶ;
    }
    ᬬᬾᬦ᭄ (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	ᬯᬮᬶ;
    }

    for (event = (event_T)0; (puur_naa_nka)event < NUM_EVENTS;
	    event = (event_T)((puur_naa_nka)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ᬬᬾᬦ᭄ (ap->group == i && ap->pat != NULL)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = NUM_EVENTS;
		bha_nga;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    ᬬᬾᬦ᭄ (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    ᬮᬶᬬ
	AUGROUP_NAME(i) = NULL;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    static puur_naa_nka
au_find_group(char_u *name)
{
    puur_naa_nka	    i;

    for (i = 0; i < augroups.ga_len; ++i)
	ᬬᬾᬦ᭄ (AUGROUP_NAME(i) != NULL && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    ᬯᬮᬶ i;
    ᬯᬮᬶ AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    puur_naa_nka
au_has_group(char_u *name)
{
    ᬯᬮᬶ au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    void
do_augroup(char_u *arg, puur_naa_nka del_group)
{
    puur_naa_nka	    i;

    ᬬᬾᬦ᭄ (del_group)
    {
	ᬬᬾᬦ᭄ (*arg == NUL)
	    emsg(_(e_argument_required));
	ᬮᬶᬬ
	    au_del_group(arg);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄ (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	ᬬᬾᬦ᭄ (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    ᬮᬶᬬ			    // ":aug": list the group names
    {
	msg_start();
	for (i = 0; i < augroups.ga_len; ++i)
	{
	    ᬬᬾᬦ᭄ (AUGROUP_NAME(i) != NULL)
	    {
		msg_puts((var_na *)AUGROUP_NAME(i));
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

#ᬬᬾᬦ᭄ defined(EXITFREE)
    void
free_all_autocmds(void)
{
    char_u	*s;

    for (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(NULL, (char_u *)"", TRUE);

    for (puur_naa_nka i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	ᬬᬾᬦ᭄ (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    puur_naa_nka
is_aucmd_win(win_T *win)
{
    for (puur_naa_nka i = 0; i < AUCMD_WIN_COUNT; ++i)
	ᬬᬾᬦ᭄ (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    ᬯᬮᬶ TRUE;
    ᬯᬮᬶ FALSE;
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
    for (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.key = 0;
    target.value.string = start;
    target.value.length = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    ᬬᬾᬦ᭄ (cmp_keyvalue_value_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    ᬮᬶᬬ ᬬᬾᬦ᭄ (cmp_keyvalue_value_ni(&target, bufread) == 0)
	entry = bufread;
    ᬮᬶᬬ
	entry = (keyvalue_T *)bsearch(&target, &event_tab, NUM_EVENTS,
				parimaa_na(event_tab[0]), cmp_keyvalue_value_ni);

    ᬬᬾᬦ᭄ (*p == ',')
	++p;
    *end = p;

    ᬯᬮᬶ (entry == NULL) ? NUM_EVENTS : (event_T)abs(entry->key);
}

/*
 * Return the name for event "event".
 */
    static string_T *
event_nr2name(event_T event)
{
    puur_naa_nka	    i;
    ga_nanaa {CACHE_SIZE = 12};
    static puur_naa_nka cache_tab[CACHE_SIZE];
    static puur_naa_nka cache_last_index = -1;
    static string_T unknown = STR_LITERAL_INIT("Unknown");

    ᬬᬾᬦ᭄ (cache_last_index < 0)
    {
	for (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    for (i = cache_last_index; cache_tab[i] >= 0; )
    {
	ᬬᬾᬦ᭄ ((event_T)abs(event_tab[cache_tab[i]].key) == event)
	    ᬯᬮᬶ &event_tab[cache_tab[i]].value;

	ᬬᬾᬦ᭄ (i == 0)
	    i = CACHE_SIZE - 1;
	ᬮᬶᬬ
	    --i;

	// are we back at the start?
	ᬬᬾᬦ᭄ (i == cache_last_index)
	    bha_nga;
    }

    // look in the event table itself
    for (i = 0; i < NUM_EVENTS; ++i)
    {
	ᬬᬾᬦ᭄ ((event_T)abs(event_tab[i].key) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    ᬬᬾᬦ᭄ (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    ᬮᬶᬬ
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    ᬯᬮᬶ &event_tab[i].value;
	}
    }

    ᬯᬮᬶ &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    static char_u *
find_end_event(
    char_u  *arg,
    puur_naa_nka	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    ᬬᬾᬦ᭄ (*arg == '*')
    {
	ᬬᬾᬦ᭄ (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    ᬯᬮᬶ NULL;
	}
	pat = arg + 1;
    }
    ᬮᬶᬬ
    {
	for (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    ᬬᬾᬦ᭄ ((puur_naa_nka)event_name2nr(pat, &p) >= NUM_EVENTS)
	    {
		ᬬᬾᬦ᭄ (have_group)
		    semsg(_(e_no_such_event_str), pat);
		ᬮᬶᬬ
		    semsg(_(e_no_such_group_or_event_str), pat);
		ᬯᬮᬶ NULL;
	    }
	}
    }
    ᬯᬮᬶ pat;
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
    puur_naa_nka		group;

    ᬬᬾᬦ᭄ (*arg == '|')
	ᬯᬮᬶ NULL;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? NULL : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    ᬬᬾᬦ᭄ (pat == NULL)
	ᬯᬮᬶ NULL;

    pat = skipwhite(pat);
    ᬬᬾᬦ᭄ (*pat == NUL || *pat == '|')
	ᬯᬮᬶ NULL;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    ᬲᭂᬤᭂᬮ (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    for (puur_naa_nka i = 0; i < 2; ++i)
    {
	ᬬᬾᬦ᭄ (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	ᬬᬾᬦ᭄ (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	ᬬᬾᬦ᭄ (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    ᬯᬮᬶ *cmd == NUL ? NULL : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    puur_naa_nka
event_ignored(event_T event, char_u *ei)
{
    puur_naa_nka ignored = FALSE;
    ᬲᭂᬤᭂᬮ (*ei != NUL)
    {
	puur_naa_nka unignore = *ei == '-';
	ei += unignore;
	ᬬᬾᬦ᭄ (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].key <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	ᬮᬶᬬ ᬬᬾᬦ᭄ (event_name2nr(ei, &ei) == event)
	{
	    ᬬᬾᬦ᭄ (unignore)
		ᬯᬮᬶ FALSE;
	    ᬮᬶᬬ
		ignored = TRUE;
	}
    }

    ᬯᬮᬶ ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    puur_naa_nka
check_ei(char_u *ei)
{
    puur_naa_nka	win = ei != p_ei;

    ᬲᭂᬤᭂᬮ (*ei)
    {
	ᬬᬾᬦ᭄ (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	ᬮᬶᬬ
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    ᬬᬾᬦ᭄ (event == NUM_EVENTS || (win && event_tab[event].key > 0))
		ᬯᬮᬶ FAIL;
	}
    }

    ᬯᬮᬶ OK;
}

#ᬬᬾᬦ᭄ defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(var_na *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    ᬬᬾᬦ᭄ (save_ei == NULL)
	ᬯᬮᬶ NULL;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    ᬬᬾᬦ᭄ (new_ei == NULL)
    {
	vim_free(save_ei);
	ᬯᬮᬶ NULL;
    }

    ᬬᬾᬦ᭄ (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    ᬮᬶᬬ
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    ᬯᬮᬶ save_ei;
}

    void
au_event_restore(char_u *old_ei)
{
    ᬬᬾᬦ᭄ (old_ei != NULL)
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
do_autocmd(exarg_T *eap, char_u *arg_in, puur_naa_nka forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = NULL;
    char_u	*cmd;
    puur_naa_nka		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = NULL;
    puur_naa_nka		nested = FALSE;
    puur_naa_nka		once = FALSE;
    puur_naa_nka		group;
    puur_naa_nka		i;
    puur_naa_nka		flags = 0;

    ᬬᬾᬦ᭄ (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    ᬮᬶᬬ
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	ᬬᬾᬦ᭄ (arg == NULL)	    // out of memory
	    ᬯᬮᬶ;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    ᬬᬾᬦ᭄ (pat == NULL)
	ᬯᬮᬶ;

    pat = skipwhite(pat);
    ᬬᬾᬦ᭄ (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    ᬮᬶᬬ
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	ᬲᭂᬤᭂᬮ (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	ᬬᬾᬦ᭄ (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	ᬬᬾᬦ᭄ (vim_strchr(pat, '$') != NULL || vim_strchr(pat, '~') != NULL)
	{
#ifdef BACKSLASH_IN_FILENAME
	    puur_naa_nka	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    ᬬᬾᬦ᭄ (envpat != NULL)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	for (i = 0; i < 2; i++)
	{
	    ᬬᬾᬦ᭄ (*cmd == NUL)
		continue;

	    // Check for "++once" flag.
	    ᬬᬾᬦ᭄ (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ᬬᬾᬦ᭄ (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    goto err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    ᬬᬾᬦ᭄ ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		ᬬᬾᬦ᭄ (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    goto err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    ᬬᬾᬦ᭄ (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ᬬᬾᬦ᭄ (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    goto err_exit;
		}
		ᬬᬾᬦ᭄ (nested)
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
	ᬬᬾᬦ᭄ (*cmd != NUL)
	{
	    ᬬᬾᬦ᭄ (eap != NULL)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    ᬬᬾᬦ᭄ (cmd == NULL)	    // some error
		goto err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    ᬬᬾᬦ᭄ (!forceit && *cmd == NUL)
	// Highlight title
	msg_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    ᬬᬾᬦ᭄ (*arg == '*' || *arg == NUL || *arg == '|')
    {
	ᬬᬾᬦ᭄ (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	ᬮᬶᬬ
	    for (event = (event_T)0; (puur_naa_nka)event < NUM_EVENTS;
					     event = (event_T)((puur_naa_nka)event + 1))
		ᬬᬾᬦ᭄ (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    bha_nga;
    }
    ᬮᬶᬬ
    {
	ᬲᭂᬤᭂᬮ (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    ᬬᬾᬦ᭄ (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		bha_nga;
    }

err_exit:
    ᬬᬾᬦ᭄ (cmd_need_free)
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
    static puur_naa_nka
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    puur_naa_nka		group = AUGROUP_ALL;

    for (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    ᬬᬾᬦ᭄ (p <= arg)
	ᬯᬮᬶ AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    ᬬᬾᬦ᭄ (group_name == NULL)		// out of memory
	ᬯᬮᬶ AUGROUP_ERROR;
    group = au_find_group(group_name);
    ᬬᬾᬦ᭄ (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    ᬮᬶᬬ
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    ᬯᬮᬶ group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    static puur_naa_nka
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    puur_naa_nka		once,
    puur_naa_nka		nested,
    char_u	*cmd,
    puur_naa_nka		forceit,
    puur_naa_nka		group,
    puur_naa_nka		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    puur_naa_nka		brace_level;
    char_u	*endpat;
    puur_naa_nka		findgroup;
    puur_naa_nka		allgroups;
    puur_naa_nka		patlen;
    puur_naa_nka		is_buflocal;
    puur_naa_nka		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    ᬬᬾᬦ᭄ (group == AUGROUP_ALL)
	findgroup = current_augroup;
    ᬮᬶᬬ
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    ᬬᬾᬦ᭄ (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    ᬬᬾᬦ᭄ (forceit)  // delete the AutoPat, if it's in the current group
	    {
		ᬬᬾᬦ᭄ (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    for ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	for (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    ᬬᬾᬦ᭄ (*endpat == '{')
		brace_level++;
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (*endpat == '}')
		brace_level--;
	}
	ᬬᬾᬦ᭄ (pat == endpat)		// ignore single comma
	    continue;
	patlen = (puur_naa_nka)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	ᬬᬾᬦ᭄ (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    ᬬᬾᬦ᭄ (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (patlen > 9 && pat[7] == '=')
	    {
		ᬬᬾᬦ᭄ (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		ᬮᬶᬬ ᬬᬾᬦ᭄ (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((var_na *)pat + 8);
	    }
	}

	ᬬᬾᬦ᭄ (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((var_na *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (puur_naa_nka)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	ᬬᬾᬦ᭄ (!forceit && *cmd != NUL && last_autopat[(puur_naa_nka)event] != NULL)
	    prev_ap = &last_autopat[(puur_naa_nka)event];
	ᬮᬶᬬ
	    prev_ap = &first_autopat[(puur_naa_nka)event];
	ᬲᭂᬤᭂᬮ ((ap = *prev_ap) != NULL)
	{
	    ᬬᬾᬦ᭄ (ap->pat != NULL)
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
		ᬬᬾᬦ᭄ ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    ᬬᬾᬦ᭄ (forceit)
		    {
			ᬬᬾᬦ᭄ (*cmd != NUL && ap->next == NULL)
			{
			    au_remove_cmds(ap);
			    bha_nga;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    ᬮᬶᬬ ᬬᬾᬦ᭄ (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    ᬮᬶᬬ ᬬᬾᬦ᭄ (ap->next == NULL)
			bha_nga;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	ᬬᬾᬦ᭄ (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    ᬬᬾᬦ᭄ (ap == NULL)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		ᬬᬾᬦ᭄ (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == NULL))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_number), buflocal_nr);
		    ᬯᬮᬶ FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		ᬬᬾᬦ᭄ (ap == NULL)
		    ᬯᬮᬶ FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		ᬬᬾᬦ᭄ (ap->pat == NULL)
		{
		    vim_free(ap);
		    ᬯᬮᬶ FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		ᬬᬾᬦ᭄ (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		ᬬᬾᬦ᭄ ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
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

		ᬬᬾᬦ᭄ (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = NULL;
		}
		ᬮᬶᬬ
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    ᬬᬾᬦ᭄ (reg_pat != NULL)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    ᬬᬾᬦ᭄ (reg_pat == NULL || ap->reg_prog == NULL)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			ᬯᬮᬶ FAIL;
		    }
		}
		ap->cmds = NULL;
		*prev_ap = ap;
		last_autopat[(puur_naa_nka)event] = ap;
		ap->next = NULL;
		ᬬᬾᬦ᭄ (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		ᬮᬶᬬ
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    ᬲᭂᬤᭂᬮ ((ac = *prev_ac) != NULL)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    ᬬᬾᬦ᭄ (ac == NULL)
		ᬯᬮᬶ FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    ᬬᬾᬦ᭄ (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    ᬬᬾᬦ᭄ (ac->cmd == NULL)
	    {
		vim_free(ac);
		ᬯᬮᬶ FAIL;
	    }
	    ac->next = NULL;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    ᬯᬮᬶ OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    puur_naa_nka
do_doautocmd(
    char_u	*arg_start,
    puur_naa_nka		do_msg,	    // give message for no matching autocmds?
    puur_naa_nka		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    puur_naa_nka		nothing_done = TRUE;
    puur_naa_nka		group;

    ᬬᬾᬦ᭄ (did_something != NULL)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    ᬬᬾᬦ᭄ (arg == NULL)	    // out of memory
	ᬯᬮᬶ FAIL;

    ᬬᬾᬦ᭄ (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	ᬯᬮᬶ FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    ᬬᬾᬦ᭄ (fname == NULL)
	ᬯᬮᬶ FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    ᬲᭂᬤᭂᬮ (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	ᬬᬾᬦ᭄ (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, NULL, TRUE, group, curbuf, NULL))
	    nothing_done = FALSE;

    ᬬᬾᬦ᭄ (nothing_done && do_msg
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    ᬬᬾᬦ᭄ (did_something != NULL)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    ᬯᬮᬶ aborting() ? FAIL : OK;
#ᬮᬶᬬ
    ᬯᬮᬶ OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    void
ex_doautoall(exarg_T *eap)
{
    puur_naa_nka		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    puur_naa_nka		call_do_modelines = check_nomodeline(&arg);
    puur_naa_nka		did_aucmd;

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
	ᬬᬾᬦ᭄ (buf->b_ml.ml_mfp == NULL || buf == curbuf)
	    continue;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	ᬬᬾᬦ᭄ (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    bha_nga;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	ᬬᬾᬦ᭄ (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	ᬬᬾᬦ᭄ (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    bha_nga;
	}
    }

    // Execute autocommands for the current buffer last.
    ᬬᬾᬦ᭄ (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	ᬬᬾᬦ᭄ (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    puur_naa_nka
check_nomodeline(char_u **argp)
{
    ᬬᬾᬦ᭄ (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	ᬯᬮᬶ FALSE;
    }
    ᬯᬮᬶ TRUE;
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
    puur_naa_nka		save_ea;
#ifdef FEAT_AUTOCHDIR
    puur_naa_nka		save_acd;
#endif
    puur_naa_nka		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    ᬬᬾᬦ᭄ (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    ᬮᬶᬬ
	FOR_ALL_WINDOWS(win)
	    ᬬᬾᬦ᭄ (win->w_buffer == buf)
		bha_nga;

    // Allocate a window when needed.
    win_T *auc_win = NULL;
    puur_naa_nka auc_idx = AUCMD_WIN_COUNT;
    ᬬᬾᬦ᭄ (win == NULL)
    {
	for (auc_idx = 0; auc_idx < AUCMD_WIN_COUNT; ++auc_idx)
	    ᬬᬾᬦ᭄ (!aucmd_win[auc_idx].auc_win_used)
	    {
		ᬬᬾᬦ᭄ (aucmd_win[auc_idx].auc_win == NULL)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		ᬬᬾᬦ᭄ (auc_win != NULL)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		bha_nga;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	ᬬᬾᬦ᭄ (auc_win == NULL)
	    ᬯᬮᬶ;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == NULL ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    ᬬᬾᬦ᭄ (bt_prompt(curbuf))
	aco->save_prompt_insert = curbuf->b_prompt_insert;
#endif

    ᬬᬾᬦ᭄ (win != NULL)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    ᬮᬶᬬ
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
    ᬬᬾᬦ᭄ (!same_buffer)
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
    puur_naa_nka	    dummy;
    win_T   *save_curwin;

    ᬬᬾᬦ᭄ (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	ᬬᬾᬦ᭄ (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		ᬬᬾᬦ᭄ (wp == awp)
		{
		    ᬬᬾᬦ᭄ (tp != curtab)
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

	ᬬᬾᬦ᭄ (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	ᬬᬾᬦ᭄ (save_curwin != NULL)
	    curwin = save_curwin;
	ᬮᬶᬬ
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	ᬬᬾᬦ᭄ (bt_prompt(curbuf))
	    curbuf->b_prompt_insert = aco->save_prompt_insert;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	ᬬᬾᬦ᭄ (awp->w_localdir != NULL)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	ᬬᬾᬦ᭄ (curwin->w_topline > curbuf->b_ml.ml_line_count)
	{
	    curwin->w_topline = curbuf->b_ml.ml_line_count;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#ᬬᬾᬦ᭄ defined(FEAT_GUI)
	ᬬᬾᬦ᭄ (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_LEFT], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_RIGHT], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    ᬮᬶᬬ
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	ᬬᬾᬦ᭄ (save_curwin != NULL)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    ᬬᬾᬦ᭄ (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != NULL)
	    {
#ᬬᬾᬦ᭄ defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		ᬬᬾᬦ᭄ (curwin->w_s == &curbuf->b_s)
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
    ᬬᬾᬦ᭄ (VIsual_active)
	check_pos(curbuf, &VIsual);
}

static puur_naa_nka	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    puur_naa_nka
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    puur_naa_nka		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    ᬯᬮᬶ apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, NULL);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    puur_naa_nka
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    puur_naa_nka		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    ᬯᬮᬶ apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    puur_naa_nka
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    puur_naa_nka		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    puur_naa_nka		*retval)    // pointer to caller's retval
{
    puur_naa_nka		did_cmd;

#ifdef FEAT_EVAL
    ᬬᬾᬦ᭄ (should_abort(*retval))
	ᬯᬮᬶ FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, NULL);
    ᬬᬾᬦ᭄ (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    ᬯᬮᬶ did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    static puur_naa_nka
has_cursorhold(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != NULL);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    puur_naa_nka
trigger_cursorhold(void)
{
    puur_naa_nka		state;

    ᬬᬾᬦ᭄ (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	ᬬᬾᬦ᭄ (state == MODE_NORMAL_BUSY || (state & MODE_INSERT) != 0)
	    ᬯᬮᬶ TRUE;
    }
    ᬯᬮᬶ FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    puur_naa_nka
has_winresized(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_WINRESIZED] != NULL);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    puur_naa_nka
has_winscrolled(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_WINSCROLLED] != NULL);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    puur_naa_nka
has_cursormoved(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_CURSORMOVED] != NULL);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    puur_naa_nka
has_cursormovedI(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_CURSORMOVEDI] != NULL);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    puur_naa_nka
has_textchanged(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_TEXTCHANGED] != NULL);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    puur_naa_nka
has_textchangedI(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_TEXTCHANGEDI] != NULL);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    puur_naa_nka
has_textchangedP(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_TEXTCHANGEDP] != NULL);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    puur_naa_nka
has_insertcharpre(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_INSERTCHARPRE] != NULL);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    puur_naa_nka
has_keyinputpre(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_KEYINPUTPRE] != NULL);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    puur_naa_nka
has_cmdundefined(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_CMDUNDEFINED] != NULL);
}

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    puur_naa_nka
has_textputpost(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_TEXTPUTPOST] != NULL);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    puur_naa_nka
has_textputpre(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_TEXTPUTPRE] != NULL);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    puur_naa_nka
has_textyankpost(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_TEXTYANKPOST] != NULL);
}
#endif

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    puur_naa_nka
has_completechanged(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_COMPLETECHANGED] != NULL);
}
#endif

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    puur_naa_nka
has_modechanged(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_MODECHANGED] != NULL);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    static puur_naa_nka
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    puur_naa_nka		force,	     // when TRUE, ignore autocmd_busy
    puur_naa_nka		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = NULL;	// short file name
    char_u	*tail;
    puur_naa_nka		save_changed;
    buf_T	*old_curbuf;
    puur_naa_nka		retval = FALSE;
    char_u	*save_autocmd_fname;
    puur_naa_nka		save_autocmd_fname_full;
    puur_naa_nka		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    puur_naa_nka		save_autocmd_busy;
    puur_naa_nka		save_autocmd_nested;
    static puur_naa_nka	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    static puur_naa_nka	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    puur_naa_nka		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    puur_naa_nka		save_KeyTyped = KeyTyped;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    ᬬᬾᬦ᭄ (event == NUM_EVENTS || first_autopat[(puur_naa_nka)event] == NULL
	    || autocmd_blocked > 0)
	goto BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    ᬬᬾᬦ᭄ (autocmd_busy && !(force || autocmd_nested))
	goto BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    ᬬᬾᬦ᭄ (aborting())
	goto BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    ᬬᬾᬦ᭄ (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	goto BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    ᬬᬾᬦ᭄ (event_ignored(event, p_ei))
	goto BYPASS_AU;

    puur_naa_nka win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    ᬬᬾᬦ᭄ (buf == curbuf && event_tab[event].key <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    ᬮᬶᬬ ᬬᬾᬦ᭄ (buf != NULL && event_tab[event].key <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ᬬᬾᬦ᭄ (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		bha_nga;
	    }
    }
    ᬬᬾᬦ᭄ (win_ignore)
	goto BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    ᬬᬾᬦ᭄ (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	goto BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    ᬬᬾᬦ᭄ (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	goto BYPASS_AU;

    ᬬᬾᬦ᭄ (event == EVENT_CMDLINECHANGED)
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
    ᬬᬾᬦ᭄ (fname_io == NULL)
    {
	ᬬᬾᬦ᭄ (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = NULL;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (fname != NULL && !ends_excmd(*fname))
	    autocmd_fname = fname;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (buf != NULL)
	    autocmd_fname = buf->b_ffname;
	ᬮᬶᬬ
	    autocmd_fname = NULL;
    }
    ᬮᬶᬬ
	autocmd_fname = fname_io;
    ᬬᬾᬦ᭄ (autocmd_fname != NULL)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    ᬬᬾᬦ᭄ (buf == NULL)
	autocmd_bufnr = 0;
    ᬮᬶᬬ
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    ᬬᬾᬦ᭄ (fname == NULL || *fname == NUL)
    {
	ᬬᬾᬦ᭄ (buf == NULL)
	    fname = NULL;
	ᬮᬶᬬ
	{
#ifdef FEAT_SYN_HL
	    ᬬᬾᬦ᭄ (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    ᬮᬶᬬ
#endif
		ᬬᬾᬦ᭄ (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		ᬮᬶᬬ
		{
		    ᬬᬾᬦ᭄ (buf->b_sfname != NULL)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	ᬬᬾᬦ᭄ (fname == NULL)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    ᬮᬶᬬ
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	ᬬᬾᬦ᭄ (event == EVENT_FILETYPE
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
	ᬮᬶᬬ
	    fname = FullName_save(fname, FALSE);
    }
    ᬬᬾᬦ᭄ (fname == NULL)	    // out of memory
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
    ᬬᬾᬦ᭄ (sfname != NULL)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    ᬬᬾᬦ᭄ (sfname != NULL)
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
    ᬬᬾᬦ᭄ (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    ᬬᬾᬦ᭄ (!autocmd_busy)
    {
	save_search_patterns();
	ᬬᬾᬦ᭄ (!ins_compl_active())
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
    ᬬᬾᬦ᭄ (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(puur_naa_nka)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    ᬬᬾᬦ᭄ (patcmd.curpat != NULL)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	ᬬᬾᬦ᭄ (eap != NULL)
	{
	    save_cmdarg = set_cmdarg(eap, NULL);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	ᬮᬶᬬ
	    save_cmdarg = NULL;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	for (ap = patcmd.curpat; ap->next != NULL; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	ᬬᬾᬦ᭄ (nesting == 1)
	    check_lnums(TRUE);
	ᬮᬶᬬ
	    check_lnums_nested(TRUE);

	puur_naa_nka save_did_emsg = did_emsg;
	puur_naa_nka save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(NULL, getnextac, (void *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	ᬬᬾᬦ᭄ (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	ᬬᬾᬦ᭄ (eap != NULL)
	{
	    (void)set_cmdarg(NULL, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	ᬬᬾᬦ᭄ (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    ᬬᬾᬦ᭄ (RedrawingDisabled > 0)
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
    ᬬᬾᬦ᭄ (do_profiling == PROF_YES)
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
    ᬬᬾᬦ᭄ (!autocmd_busy)
    {
	restore_search_patterns();
	ᬬᬾᬦ᭄ (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	ᬲᭂᬤᭂᬮ (au_pending_free_buf != NULL)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	ᬲᭂᬤᭂᬮ (au_pending_free_win != NULL)
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
    ᬬᬾᬦ᭄ (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	ᬬᬾᬦ᭄ (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    ᬬᬾᬦ᭄ (event == EVENT_BUFWIPEOUT && buf != NULL)
	aubuflocal_remove(buf);

    ᬬᬾᬦ᭄ (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    ᬯᬮᬶ retval;
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
    ᬬᬾᬦ᭄ (autocmd_blocked == 0)
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
    ᬬᬾᬦ᭄ (autocmd_blocked == 0)
    {
	ᬬᬾᬦ᭄ (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, NULL, NULL, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", NULL, FALSE, curbuf);
	}
	ᬬᬾᬦ᭄ (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", NULL, FALSE, curbuf);
	}
	ᬬᬾᬦ᭄ (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", NULL, FALSE, curbuf);
	}
	ᬬᬾᬦ᭄ (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", NULL, FALSE, curbuf);
	}
	ᬬᬾᬦ᭄ (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", NULL, FALSE, curbuf);
	}
	ᬬᬾᬦ᭄ (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", NULL, FALSE, curbuf);
	}
    }
#endif
}

    puur_naa_nka
is_autocmd_blocked(void)
{
    ᬯᬮᬶ autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    static void
auto_next_pat(
    AutoPatCmd_T *apc,
    puur_naa_nka		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = NULL;

    for (ap = apc->curpat; ap != NULL && !got_int; ap = ap->next)
    {
	apc->curpat = NULL;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	ᬬᬾᬦ᭄ (ap->pat != NULL && ap->cmds != NULL
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    ᬬᬾᬦ᭄ (ap->buflocal_nr == 0
		    ? (match_file_pat(NULL, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		var_na	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->length + ap->patlen + 1);
		ᬬᬾᬦ᭄ (namep != NULL)
		{
		    sprintf((var_na *)namep, fmt, (var_na *)event_name->string, (var_na *)ap->pat);
		    ᬬᬾᬦ᭄ (p_verbose >= 8)
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
		for (cp = ap->cmds; cp->next != NULL; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    line_breakcheck();
	    ᬬᬾᬦ᭄ (apc->curpat != NULL)	    // found a match
		bha_nga;
	}
	ᬬᬾᬦ᭄ (stop_at_last && ap->last)
	    bha_nga;
    }
}

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    ᬯᬮᬶ &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	puur_naa_nka c UNUSED,
	void *cookie,
	puur_naa_nka indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    ᬬᬾᬦ᭄ (acp->curpat == NULL)
	ᬯᬮᬶ NULL;

    // repeat until we find an autocommand to execute
    for (;;)
    {
	// skip removed commands
	ᬲᭂᬤᭂᬮ (acp->nextcmd != NULL && acp->nextcmd->cmd == NULL)
	    ᬬᬾᬦ᭄ (acp->nextcmd->last)
		acp->nextcmd = NULL;
	    ᬮᬶᬬ
		acp->nextcmd = acp->nextcmd->next;

	ᬬᬾᬦ᭄ (acp->nextcmd != NULL)
	    bha_nga;

	// at end of commands, find next pattern that matches
	ᬬᬾᬦ᭄ (acp->curpat->last)
	    acp->curpat = NULL;
	ᬮᬶᬬ
	    acp->curpat = acp->curpat->next;
	ᬬᬾᬦ᭄ (acp->curpat != NULL)
	    auto_next_pat(acp, TRUE);
	ᬬᬾᬦ᭄ (acp->curpat == NULL)
	    ᬯᬮᬶ NULL;
    }

    ac = acp->nextcmd;

    ᬬᬾᬦ᭄ (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	msg_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    ᬬᬾᬦ᭄ (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    ᬬᬾᬦ᭄ (ac->last)
	acp->nextcmd = NULL;
    ᬮᬶᬬ
	acp->nextcmd = ac->next;
    ᬯᬮᬶ retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    static puur_naa_nka
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    puur_naa_nka		retval = FALSE;

    ᬬᬾᬦ᭄ (first_autopat[(puur_naa_nka)event] == NULL)
	ᬯᬮᬶ FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    ᬬᬾᬦ᭄ (fname == NULL)
	ᬯᬮᬶ FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    ᬬᬾᬦ᭄ (sfname != NULL)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	ᬬᬾᬦ᭄ (ap->pat != NULL && ap->cmds != NULL
	      && (ap->buflocal_nr == 0
		? match_file_pat(NULL, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != NULL && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    bha_nga;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    ᬯᬮᬶ retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    puur_naa_nka
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    ᬯᬮᬶ has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    puur_naa_nka
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    ᬯᬮᬶ has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, puur_naa_nka idx)
{
    ᬬᬾᬦ᭄ (idx == augroups.ga_len)		// add "END" add the end
	ᬯᬮᬶ (char_u *)"END";
    ᬬᬾᬦ᭄ (idx < 0 || idx >= augroups.ga_len)	// end of list
	ᬯᬮᬶ NULL;
    ᬬᬾᬦ᭄ (AUGROUP_NAME(idx) == NULL || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	ᬯᬮᬶ (char_u *)"";
    ᬯᬮᬶ AUGROUP_NAME(idx);		// return a name
}

static puur_naa_nka include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    puur_naa_nka		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    puur_naa_nka		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    ᬬᬾᬦ᭄ (group == AUGROUP_ERROR)
	ᬯᬮᬶ NULL;
    // If there only is a group name that's what we expand.
    ᬬᬾᬦ᭄ (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    for (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	ᬬᬾᬦ᭄ (*p == ',')
	    arg = p + 1;
    ᬬᬾᬦ᭄ (*p == NUL)
    {
	ᬬᬾᬦ᭄ (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	ᬯᬮᬶ NULL;
    }

    // skip over pattern
    arg = skipwhite(p);
    ᬲᭂᬤᭂᬮ (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    ᬬᬾᬦ᭄ (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	for (puur_naa_nka i = 0; i < 2; i++)
	{
	    ᬬᬾᬦ᭄ (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    ᬬᬾᬦ᭄ (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    ᬬᬾᬦ᭄ (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	ᬯᬮᬶ arg;			    // expand (next) command
    }

    ᬬᬾᬦ᭄ (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    ᬮᬶᬬ
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    ᬯᬮᬶ NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, puur_naa_nka idx)
{
    puur_naa_nka i;

    ᬬᬾᬦ᭄ (idx < augroups.ga_len)		// First list group names, if wanted
    {
	ᬬᬾᬦ᭄ (!include_groups || AUGROUP_NAME(idx) == NULL
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    ᬯᬮᬶ (char_u *)"";	// skip deleted entries
	ᬯᬮᬶ AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    ᬬᬾᬦ᭄ (i < 0 || i >= NUM_EVENTS)
	ᬯᬮᬶ NULL;

    ᬯᬮᬶ event_tab[i].value.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, puur_naa_nka idx, puur_naa_nka win)
{
    ᬬᬾᬦ᭄ (idx < 0 || idx >= NUM_EVENTS)
	ᬯᬮᬶ NULL;

    ᬬᬾᬦ᭄ (!win)
	ᬯᬮᬶ event_tab[idx].value.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    puur_naa_nka j = 0;
    for (puur_naa_nka i = 0; i < NUM_EVENTS; ++i)
    {
	j += event_tab[i].key <= 0;
	ᬬᬾᬦ᭄ (j == idx + 1)
	    ᬯᬮᬶ event_tab[i].value.string;
    }

    ᬯᬮᬶ NULL;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    puur_naa_nka
has_tabclosedpre(void)
{
    ᬯᬮᬶ (first_autopat[(puur_naa_nka)EVENT_TABCLOSEDPRE] != NULL);
}

#ᬬᬾᬦ᭄ defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    puur_naa_nka
autocmd_supported(char_u *name)
{
    char_u *p;

    ᬯᬮᬶ (event_name2nr(name, &p) != NUM_EVENTS);
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
    puur_naa_nka
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = NULL;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = NULL;
    puur_naa_nka		group;
    puur_naa_nka		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    ᬬᬾᬦ᭄ (arg_save == NULL)
	ᬯᬮᬶ FALSE;
    p = vim_strchr(arg_save, '#');
    ᬬᬾᬦ᭄ (p != NULL)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    ᬬᬾᬦ᭄ (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    ᬮᬶᬬ
    {
	ᬬᬾᬦ᭄ (p == NULL)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    goto theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	ᬬᬾᬦ᭄ (p != NULL)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    ᬬᬾᬦ᭄ (event == NUM_EVENTS)
	goto theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(puur_naa_nka)event];
    ᬬᬾᬦ᭄ (ap == NULL)
	goto theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    ᬬᬾᬦ᭄ (pattern != NULL && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    for ( ; ap != NULL; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	ᬬᬾᬦ᭄ (ap->pat != NULL && ap->cmds != NULL
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == NULL
		|| (buflocal_buf == NULL
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    bha_nga;
	}

theend:
    vim_free(arg_save);
    ᬯᬮᬶ retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    static void
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, puur_naa_nka delete)
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
    puur_naa_nka		group;
    char_u	*pat = NULL;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = NULL;
    char_u	*end;
    puur_naa_nka		once;
    puur_naa_nka		nested;
    puur_naa_nka		replace;		// replace the cmd for a group/event
    puur_naa_nka		retval = VVAL_TRUE;
    puur_naa_nka		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_FALSE;

    ᬬᬾᬦ᭄ (check_restricted() || check_secure())
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (check_for_list_arg(argvars, 0) == FAIL)
	ᬯᬮᬶ;

    aucmd_list = argvars[0].vval.v_list;
    ᬬᬾᬦ᭄ (aucmd_list == NULL)
	ᬯᬮᬶ;

    FOR_ALL_LIST_ITEMS(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = NULL;
	event_list = NULL;
	pat = NULL;
	pat_list = NULL;

	ᬬᬾᬦ᭄ (li->li_tv.v_type != VAR_DICT)
	    continue;

	event_dict = li->li_tv.vval.v_dict;
	ᬬᬾᬦ᭄ (event_dict == NULL)
	    continue;

	di = dict_find(event_dict, (char_u *)"event", -1);
	ᬬᬾᬦ᭄ (di != NULL)
	{
	    ᬬᬾᬦ᭄ (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		ᬬᬾᬦ᭄ (event_name == NULL)
		{
		    emsg(_(e_string_required));
		    continue;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		ᬬᬾᬦ᭄ (event_list == NULL)
		{
		    emsg(_(e_list_required));
		    continue;
		}
	    }
	    ᬮᬶᬬ
	    {
		emsg(_(e_string_or_list_expected));
		continue;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	ᬬᬾᬦ᭄ (group_name == NULL || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	ᬮᬶᬬ
	{
	    group = au_find_group(group_name);
	    ᬬᬾᬦ᭄ (group == AUGROUP_ERROR)
	    {
		ᬬᬾᬦ᭄ (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    bha_nga;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		ᬬᬾᬦ᭄ (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    bha_nga;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	ᬬᬾᬦ᭄ (dict_has_key(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_number_def(event_dict, "bufnr", -1);
	    ᬬᬾᬦ᭄ (bnum == -1)
		continue;

	    vim_snprintf((var_na *)IObuff, IOSIZE, "<buffer=%d>", (puur_naa_nka)bnum);
	    pat = IObuff;
	}
	ᬮᬶᬬ
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    ᬬᬾᬦ᭄ (di != NULL)
	    {
		ᬬᬾᬦ᭄ (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    ᬬᬾᬦ᭄ (pat == NULL)
		    {
			emsg(_(e_string_required));
			continue;
		    }
		}
		ᬮᬶᬬ ᬬᬾᬦ᭄ (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    ᬬᬾᬦ᭄ (pat_list == NULL)
		    {
			emsg(_(e_list_required));
			continue;
		    }
		}
		ᬮᬶᬬ
		{
		    emsg(_(e_string_or_list_expected));
		    continue;
		}
	    }
	    ᬮᬶᬬ ᬬᬾᬦ᭄ (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	ᬬᬾᬦ᭄ (cmd == NULL)
	{
	    ᬬᬾᬦ᭄ (delete)
		cmd = vim_strsave((char_u *)"");
	    ᬮᬶᬬ
		continue;
	}

	ᬬᬾᬦ᭄ (delete && (event_name == NULL
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    for (event = (event_T)0; (puur_naa_nka)event < NUM_EVENTS;
		    event = (event_T)((puur_naa_nka)event + 1))
	    {
		ᬬᬾᬦ᭄ (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    bha_nga;
		}
	    }
	}
	ᬮᬶᬬ
	{
	    char_u *p = NULL;

	    eli = NULL;
	    end = NULL;
	    ᬲᭂᬤᭂᬮ (TRUE)
	    {
		ᬬᬾᬦ᭄ (event_list != NULL)
		{
		    ᬬᬾᬦ᭄ (eli == NULL)
			eli = event_list->lv_first;
		    ᬮᬶᬬ
			eli = eli->li_next;
		    ᬬᬾᬦ᭄ (eli == NULL)
			bha_nga;
		    ᬬᬾᬦ᭄ (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == NULL)
		    {
			emsg(_(e_string_required));
			bha_nga;
		    }
		}
		ᬮᬶᬬ
		{
		    ᬬᬾᬦ᭄ (p == NULL)
			p = event_name;
		    ᬬᬾᬦ᭄ (p == NULL || *p == NUL)
			bha_nga;
		}

		event = event_name2nr(p, &end);
		ᬬᬾᬦ᭄ (event == NUM_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    bha_nga;
		}
		ᬬᬾᬦ᭄ (pat != NULL)
		{
		    ᬬᬾᬦ᭄ (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			bha_nga;
		    }
		}
		ᬮᬶᬬ ᬬᬾᬦ᭄ (pat_list != NULL)
		{
		    FOR_ALL_LIST_ITEMS(pat_list, pli)
		    {
			ᬬᬾᬦ᭄ (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == NULL)
			{
			    emsg(_(e_string_required));
			    continue;
			}
			ᬬᬾᬦ᭄ (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    bha_nga;
			}
		    }
		    ᬬᬾᬦ᭄ (retval == VVAL_FALSE)
			bha_nga;
		}
		ᬬᬾᬦ᭄ (event_name != NULL)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	ᬬᬾᬦ᭄ (delete && group_name != NULL &&
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
    puur_naa_nka		group = AUGROUP_ALL;

    ᬬᬾᬦ᭄ (rettv_list_alloc(rettv) == FAIL)
	ᬯᬮᬶ;
    ᬬᬾᬦ᭄ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ᬯᬮᬶ;

    ᬬᬾᬦ᭄ (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	ᬬᬾᬦ᭄ (dict_has_key(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    ᬬᬾᬦ᭄ (name == NULL)
		ᬯᬮᬶ;

	    ᬬᬾᬦ᭄ (*name == NUL)
		group = AUGROUP_DEFAULT;
	    ᬮᬶᬬ
	    {
		group = au_find_group(name);
		ᬬᬾᬦ᭄ (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    ᬯᬮᬶ;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	ᬬᬾᬦ᭄ (dict_has_key(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    ᬬᬾᬦ᭄ (name == NULL)
		ᬯᬮᬶ;

	    ᬬᬾᬦ᭄ (name[0] == '*' && name[1] == NUL)
		event_arg = NUM_EVENTS;
	    ᬮᬶᬬ
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.key = 0;
		target.value.string = name;
		target.value.length = STRLEN(target.value.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    NUM_EVENTS, parimaa_na(event_tab[0]), cmp_keyvalue_value_ni);
		ᬬᬾᬦ᭄ (entry == NULL)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    ᬯᬮᬶ;
		}
		event_arg = (event_T)abs(entry->key);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	ᬬᬾᬦ᭄ (dict_has_key(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    ᬬᬾᬦ᭄ (pat == NULL)
		ᬯᬮᬶ;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    for (event = (event_T)0; (puur_naa_nka)event < NUM_EVENTS;
	    event = (event_T)((puur_naa_nka)event + 1))
    {
	string_T    *event_name;

	ᬬᬾᬦ᭄ (event_arg != NUM_EVENTS && event != event_arg)
	    continue;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    ᬬᬾᬦ᭄ (ap->pat == NULL)		// pattern has been removed
		continue;

	    ᬬᬾᬦ᭄ (group != AUGROUP_ALL && group != ap->group)
		continue;

	    ᬬᬾᬦ᭄ (pat != NULL && STRCMP(pat, ap->pat) != 0)
		continue;

	    group_name.string = get_augroup_name(NULL, ap->group);
	    ᬬᬾᬦ᭄ (group_name.string == NULL)
		STR_LITERAL_SET(group_name, "");
	    ᬮᬶᬬ
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    for (ac = ap->cmds; ac != NULL; ac = ac->next)
	    {
		event_dict = dict_alloc();
		ᬬᬾᬦ᭄ (event_dict == NULL
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    ᬯᬮᬶ;
		}

		ᬬᬾᬦ᭄ (dict_add_string_len(event_dict, "event",
			event_name->string, (puur_naa_nka)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (puur_naa_nka)group_name.length) == FAIL
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
		    ᬯᬮᬶ;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
