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
    จำนวนเต็ม		    group;		// group ID
    จำนวนเต็ม		    patlen;		// strlen() of pat
    จำนวนเต็ม		    buflocal_nr;	// !=0 for buffer-local AutoPat
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
    จำนวนเต็ม		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    จำนวนเต็ม		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

static AutoPatCmd_T *active_apc_list = NULL; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    สำหรับ ((ap) = first_autopat[(จำนวนเต็ม)(event)]; (ap) != NULL; (ap) = (ap)->next)

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
static จำนวนเต็ม current_augroup = AUGROUP_DEFAULT;

static จำนวนเต็ม au_need_clean = FALSE;   // need to delete marked patterns

static event_T event_name2nr(char_u *start, char_u **end);
static string_T *event_nr2name(event_T event);
static จำนวนเต็ม au_get_grouparg(char_u **argp);
static จำนวนเต็ม do_autocmd_event(event_T event, char_u *pat, จำนวนเต็ม once, จำนวนเต็ม nested, char_u *cmd, จำนวนเต็ม forceit, จำนวนเต็ม group, จำนวนเต็ม flags);
static จำนวนเต็ม apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, จำนวนเต็ม force, จำนวนเต็ม group, buf_T *buf, exarg_T *eap);
static void auto_next_pat(AutoPatCmd_T *apc, จำนวนเต็ม stop_at_last);
static จำนวนเต็ม au_find_group(char_u *name);

static event_T	last_event;
static จำนวนเต็ม	last_group;
static จำนวนเต็ม	autocmd_blocked = 0;	// block all autocmds

    static char_u *
get_deleted_augroup(void)
{
    ถ้า (deleted_augroup == NULL)
	deleted_augroup = (char_u *)_("--Deleted--");
    คืน deleted_augroup;
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
    ถ้า (got_int)
	คืน;
    ถ้า (ap->pat == NULL)		// pattern has been removed
	คืน;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    msg_putchar('\n');
    ถ้า (got_int)
	goto theend;
    ถ้า (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	ถ้า (ap->group != AUGROUP_DEFAULT)
	{
	    ถ้า (AUGROUP_NAME(ap->group) == NULL)
		msg_puts_attr((var_na *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    มิฉะนั้น
		msg_puts_attr((var_na *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    msg_puts("  ");
	}
	event_name = event_nr2name(event);
	msg_puts_attr((var_na *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	msg_putchar('\n');
	ถ้า (got_int)
	    goto theend;
    }

    ถ้า (ap->pat == NULL)
	goto theend;  // timer might have cleared the pattern or group

    msg_col = 4;
    msg_outtrans(ap->pat);

    สำหรับ (ac = ap->cmds; ac != NULL; ac = ac->next)
    {
	ถ้า (ac->cmd == NULL)		// skip removed commands
	    continue;

	ถ้า (msg_col >= 14)
	    msg_putchar('\n');
	msg_col = 14;
	ถ้า (got_int)
	    goto theend;
	msg_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	ถ้า (p_verbose > 0)
	    last_set_msg(ac->script_ctx);
#endif
	ถ้า (got_int)
	    goto theend;
	ถ้า (ac->next != NULL)
	{
	    msg_putchar('\n');
	    ถ้า (got_int)
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

    สำหรับ (ac = ap->cmds; ac != NULL; ac = ac->next)
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

    ถ้า (autocmd_busy || !au_need_clean)
	คืน;

    // loop over all events
    สำหรับ (event = (event_T)0; (จำนวนเต็ม)event < NUM_EVENTS;
					    event = (event_T)((จำนวนเต็ม)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(จำนวนเต็ม)event]);
	สำหรับ (ap = *prev_ap; ap != NULL; ap = *prev_ap)
	{
	    จำนวนเต็ม has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    สำหรับ (ac = *prev_ac; ac != NULL; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		ถ้า (ap->pat == NULL || ac->cmd == NULL)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		มิฉะนั้น
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    ถ้า (ap->pat != NULL && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    ถ้า (ap->pat == NULL)
	    {
		ถ้า (ap->next == NULL)
		{
		    ถ้า (prev_ap == &(first_autopat[(จำนวนเต็ม)event]))
			last_autopat[(จำนวนเต็ม)event] = NULL;
		    มิฉะนั้น
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(จำนวนเต็ม)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    มิฉะนั้น
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
    สำหรับ (apc = active_apc_list; apc; apc = apc->next)
	ถ้า (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    สำหรับ (event = (event_T)0; (จำนวนเต็ม)event < NUM_EVENTS;
					    event = (event_T)((จำนวนเต็ม)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ถ้า (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		ถ้า (p_verbose >= 6)
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
    static จำนวนเต็ม
au_new_group(char_u *name)
{
    จำนวนเต็ม		i;

    i = au_find_group(name);
    ถ้า (i != AUGROUP_ERROR)
	คืน i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    สำหรับ (i = 0; i < augroups.ga_len; ++i)
	ถ้า (AUGROUP_NAME(i) == NULL)
	    bha_nga;
    ถ้า (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	คืน AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    ถ้า (AUGROUP_NAME(i) == NULL)
	คืน AUGROUP_ERROR;
    ถ้า (i == augroups.ga_len)
	++augroups.ga_len;

    คืน i;
}

    static void
au_del_group(char_u *name)
{
    จำนวนเต็ม		i;
    event_T	event;
    AutoPat	*ap;
    จำนวนเต็ม		in_use = FALSE;


    i = au_find_group(name);
    ถ้า (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	คืน;
    }
    ถ้า (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	คืน;
    }

    สำหรับ (event = (event_T)0; (จำนวนเต็ม)event < NUM_EVENTS;
	    event = (event_T)((จำนวนเต็ม)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ถ้า (ap->group == i && ap->pat != NULL)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = NUM_EVENTS;
		bha_nga;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    ถ้า (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    มิฉะนั้น
	AUGROUP_NAME(i) = NULL;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    static จำนวนเต็ม
au_find_group(char_u *name)
{
    จำนวนเต็ม	    i;

    สำหรับ (i = 0; i < augroups.ga_len; ++i)
	ถ้า (AUGROUP_NAME(i) != NULL && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    คืน i;
    คืน AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    จำนวนเต็ม
au_has_group(char_u *name)
{
    คืน au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    void
do_augroup(char_u *arg, จำนวนเต็ม del_group)
{
    จำนวนเต็ม	    i;

    ถ้า (del_group)
    {
	ถ้า (*arg == NUL)
	    emsg(_(e_argument_required));
	มิฉะนั้น
	    au_del_group(arg);
    }
    มิฉะนั้น ถ้า (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    มิฉะนั้น ถ้า (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	ถ้า (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    มิฉะนั้น			    // ":aug": list the group names
    {
	msg_start();
	สำหรับ (i = 0; i < augroups.ga_len; ++i)
	{
	    ถ้า (AUGROUP_NAME(i) != NULL)
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

#ถ้า defined(EXITFREE)
    void
free_all_autocmds(void)
{
    char_u	*s;

    สำหรับ (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(NULL, (char_u *)"", TRUE);

    สำหรับ (จำนวนเต็ม i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	ถ้า (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    จำนวนเต็ม
is_aucmd_win(win_T *win)
{
    สำหรับ (จำนวนเต็ม i = 0; i < AUCMD_WIN_COUNT; ++i)
	ถ้า (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    คืน TRUE;
    คืน FALSE;
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
    สำหรับ (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.key = 0;
    target.value.string = start;
    target.value.length = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    ถ้า (cmp_keyvalue_value_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    มิฉะนั้น ถ้า (cmp_keyvalue_value_ni(&target, bufread) == 0)
	entry = bufread;
    มิฉะนั้น
	entry = (keyvalue_T *)bsearch(&target, &event_tab, NUM_EVENTS,
				parimaa_na(event_tab[0]), cmp_keyvalue_value_ni);

    ถ้า (*p == ',')
	++p;
    *end = p;

    คืน (entry == NULL) ? NUM_EVENTS : (event_T)abs(entry->key);
}

/*
 * Return the name for event "event".
 */
    static string_T *
event_nr2name(event_T event)
{
    จำนวนเต็ม	    i;
    ga_nanaa {CACHE_SIZE = 12};
    static จำนวนเต็ม cache_tab[CACHE_SIZE];
    static จำนวนเต็ม cache_last_index = -1;
    static string_T unknown = STR_LITERAL_INIT("Unknown");

    ถ้า (cache_last_index < 0)
    {
	สำหรับ (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    สำหรับ (i = cache_last_index; cache_tab[i] >= 0; )
    {
	ถ้า ((event_T)abs(event_tab[cache_tab[i]].key) == event)
	    คืน &event_tab[cache_tab[i]].value;

	ถ้า (i == 0)
	    i = CACHE_SIZE - 1;
	มิฉะนั้น
	    --i;

	// are we back at the start?
	ถ้า (i == cache_last_index)
	    bha_nga;
    }

    // look in the event table itself
    สำหรับ (i = 0; i < NUM_EVENTS; ++i)
    {
	ถ้า ((event_T)abs(event_tab[i].key) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    ถ้า (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    มิฉะนั้น
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    คืน &event_tab[i].value;
	}
    }

    คืน &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    static char_u *
find_end_event(
    char_u  *arg,
    จำนวนเต็ม	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    ถ้า (*arg == '*')
    {
	ถ้า (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    คืน NULL;
	}
	pat = arg + 1;
    }
    มิฉะนั้น
    {
	สำหรับ (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    ถ้า ((จำนวนเต็ม)event_name2nr(pat, &p) >= NUM_EVENTS)
	    {
		ถ้า (have_group)
		    semsg(_(e_no_such_event_str), pat);
		มิฉะนั้น
		    semsg(_(e_no_such_group_or_event_str), pat);
		คืน NULL;
	    }
	}
    }
    คืน pat;
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
    จำนวนเต็ม		group;

    ถ้า (*arg == '|')
	คืน NULL;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? NULL : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    ถ้า (pat == NULL)
	คืน NULL;

    pat = skipwhite(pat);
    ถ้า (*pat == NUL || *pat == '|')
	คืน NULL;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    ขณะ (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    สำหรับ (จำนวนเต็ม i = 0; i < 2; ++i)
    {
	ถ้า (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	ถ้า (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	ถ้า (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    คืน *cmd == NUL ? NULL : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    จำนวนเต็ม
event_ignored(event_T event, char_u *ei)
{
    จำนวนเต็ม ignored = FALSE;
    ขณะ (*ei != NUL)
    {
	จำนวนเต็ม unignore = *ei == '-';
	ei += unignore;
	ถ้า (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].key <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	มิฉะนั้น ถ้า (event_name2nr(ei, &ei) == event)
	{
	    ถ้า (unignore)
		คืน FALSE;
	    มิฉะนั้น
		ignored = TRUE;
	}
    }

    คืน ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    จำนวนเต็ม
check_ei(char_u *ei)
{
    จำนวนเต็ม	win = ei != p_ei;

    ขณะ (*ei)
    {
	ถ้า (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	มิฉะนั้น
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    ถ้า (event == NUM_EVENTS || (win && event_tab[event].key > 0))
		คืน FAIL;
	}
    }

    คืน OK;
}

#ถ้า defined(FEAT_SYN_HL)

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
    ถ้า (save_ei == NULL)
	คืน NULL;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    ถ้า (new_ei == NULL)
    {
	vim_free(save_ei);
	คืน NULL;
    }

    ถ้า (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    มิฉะนั้น
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    คืน save_ei;
}

    void
au_event_restore(char_u *old_ei)
{
    ถ้า (old_ei != NULL)
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
do_autocmd(exarg_T *eap, char_u *arg_in, จำนวนเต็ม forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = NULL;
    char_u	*cmd;
    จำนวนเต็ม		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = NULL;
    จำนวนเต็ม		nested = FALSE;
    จำนวนเต็ม		once = FALSE;
    จำนวนเต็ม		group;
    จำนวนเต็ม		i;
    จำนวนเต็ม		flags = 0;

    ถ้า (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    มิฉะนั้น
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	ถ้า (arg == NULL)	    // out of memory
	    คืน;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    ถ้า (pat == NULL)
	คืน;

    pat = skipwhite(pat);
    ถ้า (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    มิฉะนั้น
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	ขณะ (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	ถ้า (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	ถ้า (vim_strchr(pat, '$') != NULL || vim_strchr(pat, '~') != NULL)
	{
#ifdef BACKSLASH_IN_FILENAME
	    จำนวนเต็ม	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    ถ้า (envpat != NULL)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	สำหรับ (i = 0; i < 2; i++)
	{
	    ถ้า (*cmd == NUL)
		continue;

	    // Check for "++once" flag.
	    ถ้า (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ถ้า (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    goto err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    ถ้า ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		ถ้า (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    goto err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    ถ้า (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ถ้า (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    goto err_exit;
		}
		ถ้า (nested)
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
	ถ้า (*cmd != NUL)
	{
	    ถ้า (eap != NULL)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    ถ้า (cmd == NULL)	    // some error
		goto err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    ถ้า (!forceit && *cmd == NUL)
	// Highlight title
	msg_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    ถ้า (*arg == '*' || *arg == NUL || *arg == '|')
    {
	ถ้า (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	มิฉะนั้น
	    สำหรับ (event = (event_T)0; (จำนวนเต็ม)event < NUM_EVENTS;
					     event = (event_T)((จำนวนเต็ม)event + 1))
		ถ้า (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    bha_nga;
    }
    มิฉะนั้น
    {
	ขณะ (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    ถ้า (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		bha_nga;
    }

err_exit:
    ถ้า (cmd_need_free)
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
    static จำนวนเต็ม
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    จำนวนเต็ม		group = AUGROUP_ALL;

    สำหรับ (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    ถ้า (p <= arg)
	คืน AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    ถ้า (group_name == NULL)		// out of memory
	คืน AUGROUP_ERROR;
    group = au_find_group(group_name);
    ถ้า (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    มิฉะนั้น
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    คืน group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    static จำนวนเต็ม
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    จำนวนเต็ม		once,
    จำนวนเต็ม		nested,
    char_u	*cmd,
    จำนวนเต็ม		forceit,
    จำนวนเต็ม		group,
    จำนวนเต็ม		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    จำนวนเต็ม		brace_level;
    char_u	*endpat;
    จำนวนเต็ม		findgroup;
    จำนวนเต็ม		allgroups;
    จำนวนเต็ม		patlen;
    จำนวนเต็ม		is_buflocal;
    จำนวนเต็ม		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    ถ้า (group == AUGROUP_ALL)
	findgroup = current_augroup;
    มิฉะนั้น
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    ถ้า (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    ถ้า (forceit)  // delete the AutoPat, if it's in the current group
	    {
		ถ้า (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    มิฉะนั้น ถ้า (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    สำหรับ ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	สำหรับ (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    ถ้า (*endpat == '{')
		brace_level++;
	    มิฉะนั้น ถ้า (*endpat == '}')
		brace_level--;
	}
	ถ้า (pat == endpat)		// ignore single comma
	    continue;
	patlen = (จำนวนเต็ม)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	ถ้า (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    ถ้า (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    มิฉะนั้น ถ้า (patlen > 9 && pat[7] == '=')
	    {
		ถ้า (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		มิฉะนั้น ถ้า (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((var_na *)pat + 8);
	    }
	}

	ถ้า (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((var_na *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (จำนวนเต็ม)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	ถ้า (!forceit && *cmd != NUL && last_autopat[(จำนวนเต็ม)event] != NULL)
	    prev_ap = &last_autopat[(จำนวนเต็ม)event];
	มิฉะนั้น
	    prev_ap = &first_autopat[(จำนวนเต็ม)event];
	ขณะ ((ap = *prev_ap) != NULL)
	{
	    ถ้า (ap->pat != NULL)
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
		ถ้า ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    ถ้า (forceit)
		    {
			ถ้า (*cmd != NUL && ap->next == NULL)
			{
			    au_remove_cmds(ap);
			    bha_nga;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    มิฉะนั้น ถ้า (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    มิฉะนั้น ถ้า (ap->next == NULL)
			bha_nga;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	ถ้า (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    ถ้า (ap == NULL)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		ถ้า (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == NULL))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_number), buflocal_nr);
		    คืน FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		ถ้า (ap == NULL)
		    คืน FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		ถ้า (ap->pat == NULL)
		{
		    vim_free(ap);
		    คืน FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		ถ้า (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		ถ้า ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
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

		ถ้า (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = NULL;
		}
		มิฉะนั้น
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    ถ้า (reg_pat != NULL)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    ถ้า (reg_pat == NULL || ap->reg_prog == NULL)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			คืน FAIL;
		    }
		}
		ap->cmds = NULL;
		*prev_ap = ap;
		last_autopat[(จำนวนเต็ม)event] = ap;
		ap->next = NULL;
		ถ้า (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		มิฉะนั้น
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    ขณะ ((ac = *prev_ac) != NULL)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    ถ้า (ac == NULL)
		คืน FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    ถ้า (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    ถ้า (ac->cmd == NULL)
	    {
		vim_free(ac);
		คืน FAIL;
	    }
	    ac->next = NULL;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    คืน OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    จำนวนเต็ม
do_doautocmd(
    char_u	*arg_start,
    จำนวนเต็ม		do_msg,	    // give message for no matching autocmds?
    จำนวนเต็ม		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    จำนวนเต็ม		nothing_done = TRUE;
    จำนวนเต็ม		group;

    ถ้า (did_something != NULL)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    ถ้า (arg == NULL)	    // out of memory
	คืน FAIL;

    ถ้า (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	คืน FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    ถ้า (fname == NULL)
	คืน FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    ขณะ (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	ถ้า (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, NULL, TRUE, group, curbuf, NULL))
	    nothing_done = FALSE;

    ถ้า (nothing_done && do_msg
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    ถ้า (did_something != NULL)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    คืน aborting() ? FAIL : OK;
#มิฉะนั้น
    คืน OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    void
ex_doautoall(exarg_T *eap)
{
    จำนวนเต็ม		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    จำนวนเต็ม		call_do_modelines = check_nomodeline(&arg);
    จำนวนเต็ม		did_aucmd;

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
	ถ้า (buf->b_ml.ml_mfp == NULL || buf == curbuf)
	    continue;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	ถ้า (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    bha_nga;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	ถ้า (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	ถ้า (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    bha_nga;
	}
    }

    // Execute autocommands for the current buffer last.
    ถ้า (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	ถ้า (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    จำนวนเต็ม
check_nomodeline(char_u **argp)
{
    ถ้า (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	คืน FALSE;
    }
    คืน TRUE;
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
    จำนวนเต็ม		save_ea;
#ifdef FEAT_AUTOCHDIR
    จำนวนเต็ม		save_acd;
#endif
    จำนวนเต็ม		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    ถ้า (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    มิฉะนั้น
	FOR_ALL_WINDOWS(win)
	    ถ้า (win->w_buffer == buf)
		bha_nga;

    // Allocate a window when needed.
    win_T *auc_win = NULL;
    จำนวนเต็ม auc_idx = AUCMD_WIN_COUNT;
    ถ้า (win == NULL)
    {
	สำหรับ (auc_idx = 0; auc_idx < AUCMD_WIN_COUNT; ++auc_idx)
	    ถ้า (!aucmd_win[auc_idx].auc_win_used)
	    {
		ถ้า (aucmd_win[auc_idx].auc_win == NULL)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		ถ้า (auc_win != NULL)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		bha_nga;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	ถ้า (auc_win == NULL)
	    คืน;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == NULL ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    ถ้า (bt_prompt(curbuf))
	aco->save_prompt_insert = curbuf->b_prompt_insert;
#endif

    ถ้า (win != NULL)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    มิฉะนั้น
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
    ถ้า (!same_buffer)
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
    จำนวนเต็ม	    dummy;
    win_T   *save_curwin;

    ถ้า (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	ถ้า (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		ถ้า (wp == awp)
		{
		    ถ้า (tp != curtab)
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

	ถ้า (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	ถ้า (save_curwin != NULL)
	    curwin = save_curwin;
	มิฉะนั้น
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	ถ้า (bt_prompt(curbuf))
	    curbuf->b_prompt_insert = aco->save_prompt_insert;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	ถ้า (awp->w_localdir != NULL)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	ถ้า (curwin->w_topline > curbuf->b_ml.ml_line_count)
	{
	    curwin->w_topline = curbuf->b_ml.ml_line_count;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#ถ้า defined(FEAT_GUI)
	ถ้า (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_LEFT], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_RIGHT], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    มิฉะนั้น
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	ถ้า (save_curwin != NULL)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    ถ้า (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != NULL)
	    {
#ถ้า defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		ถ้า (curwin->w_s == &curbuf->b_s)
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
    ถ้า (VIsual_active)
	check_pos(curbuf, &VIsual);
}

static จำนวนเต็ม	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    จำนวนเต็ม
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    จำนวนเต็ม		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    คืน apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, NULL);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    จำนวนเต็ม
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    จำนวนเต็ม		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    คืน apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    จำนวนเต็ม
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    จำนวนเต็ม		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    จำนวนเต็ม		*retval)    // pointer to caller's retval
{
    จำนวนเต็ม		did_cmd;

#ifdef FEAT_EVAL
    ถ้า (should_abort(*retval))
	คืน FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, NULL);
    ถ้า (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    คืน did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    static จำนวนเต็ม
has_cursorhold(void)
{
    คืน (first_autopat[(จำนวนเต็ม)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != NULL);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    จำนวนเต็ม
trigger_cursorhold(void)
{
    จำนวนเต็ม		state;

    ถ้า (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	ถ้า (state == MODE_NORMAL_BUSY || (state & MODE_INSERT) != 0)
	    คืน TRUE;
    }
    คืน FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    จำนวนเต็ม
has_winresized(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_WINRESIZED] != NULL);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    จำนวนเต็ม
has_winscrolled(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_WINSCROLLED] != NULL);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    จำนวนเต็ม
has_cursormoved(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_CURSORMOVED] != NULL);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    จำนวนเต็ม
has_cursormovedI(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_CURSORMOVEDI] != NULL);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    จำนวนเต็ม
has_textchanged(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_TEXTCHANGED] != NULL);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    จำนวนเต็ม
has_textchangedI(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_TEXTCHANGEDI] != NULL);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    จำนวนเต็ม
has_textchangedP(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_TEXTCHANGEDP] != NULL);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    จำนวนเต็ม
has_insertcharpre(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_INSERTCHARPRE] != NULL);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    จำนวนเต็ม
has_keyinputpre(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_KEYINPUTPRE] != NULL);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    จำนวนเต็ม
has_cmdundefined(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_CMDUNDEFINED] != NULL);
}

#ถ้า defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    จำนวนเต็ม
has_textputpost(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_TEXTPUTPOST] != NULL);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    จำนวนเต็ม
has_textputpre(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_TEXTPUTPRE] != NULL);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    จำนวนเต็ม
has_textyankpost(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_TEXTYANKPOST] != NULL);
}
#endif

#ถ้า defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    จำนวนเต็ม
has_completechanged(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_COMPLETECHANGED] != NULL);
}
#endif

#ถ้า defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    จำนวนเต็ม
has_modechanged(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_MODECHANGED] != NULL);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    static จำนวนเต็ม
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    จำนวนเต็ม		force,	     // when TRUE, ignore autocmd_busy
    จำนวนเต็ม		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = NULL;	// short file name
    char_u	*tail;
    จำนวนเต็ม		save_changed;
    buf_T	*old_curbuf;
    จำนวนเต็ม		retval = FALSE;
    char_u	*save_autocmd_fname;
    จำนวนเต็ม		save_autocmd_fname_full;
    จำนวนเต็ม		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    จำนวนเต็ม		save_autocmd_busy;
    จำนวนเต็ม		save_autocmd_nested;
    static จำนวนเต็ม	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    static จำนวนเต็ม	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    จำนวนเต็ม		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    จำนวนเต็ม		save_KeyTyped = KeyTyped;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    ถ้า (event == NUM_EVENTS || first_autopat[(จำนวนเต็ม)event] == NULL
	    || autocmd_blocked > 0)
	goto BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    ถ้า (autocmd_busy && !(force || autocmd_nested))
	goto BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    ถ้า (aborting())
	goto BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    ถ้า (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	goto BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    ถ้า (event_ignored(event, p_ei))
	goto BYPASS_AU;

    จำนวนเต็ม win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    ถ้า (buf == curbuf && event_tab[event].key <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    มิฉะนั้น ถ้า (buf != NULL && event_tab[event].key <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ถ้า (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		bha_nga;
	    }
    }
    ถ้า (win_ignore)
	goto BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    ถ้า (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	goto BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    ถ้า (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	goto BYPASS_AU;

    ถ้า (event == EVENT_CMDLINECHANGED)
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
    ถ้า (fname_io == NULL)
    {
	ถ้า (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = NULL;
	มิฉะนั้น ถ้า (fname != NULL && !ends_excmd(*fname))
	    autocmd_fname = fname;
	มิฉะนั้น ถ้า (buf != NULL)
	    autocmd_fname = buf->b_ffname;
	มิฉะนั้น
	    autocmd_fname = NULL;
    }
    มิฉะนั้น
	autocmd_fname = fname_io;
    ถ้า (autocmd_fname != NULL)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    ถ้า (buf == NULL)
	autocmd_bufnr = 0;
    มิฉะนั้น
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    ถ้า (fname == NULL || *fname == NUL)
    {
	ถ้า (buf == NULL)
	    fname = NULL;
	มิฉะนั้น
	{
#ifdef FEAT_SYN_HL
	    ถ้า (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    มิฉะนั้น
#endif
		ถ้า (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		มิฉะนั้น
		{
		    ถ้า (buf->b_sfname != NULL)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	ถ้า (fname == NULL)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    มิฉะนั้น
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	ถ้า (event == EVENT_FILETYPE
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
	มิฉะนั้น
	    fname = FullName_save(fname, FALSE);
    }
    ถ้า (fname == NULL)	    // out of memory
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
    ถ้า (sfname != NULL)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    ถ้า (sfname != NULL)
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
    ถ้า (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    ถ้า (!autocmd_busy)
    {
	save_search_patterns();
	ถ้า (!ins_compl_active())
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
    ถ้า (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(จำนวนเต็ม)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    ถ้า (patcmd.curpat != NULL)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	ถ้า (eap != NULL)
	{
	    save_cmdarg = set_cmdarg(eap, NULL);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	มิฉะนั้น
	    save_cmdarg = NULL;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	สำหรับ (ap = patcmd.curpat; ap->next != NULL; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	ถ้า (nesting == 1)
	    check_lnums(TRUE);
	มิฉะนั้น
	    check_lnums_nested(TRUE);

	จำนวนเต็ม save_did_emsg = did_emsg;
	จำนวนเต็ม save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(NULL, getnextac, (void *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	ถ้า (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	ถ้า (eap != NULL)
	{
	    (void)set_cmdarg(NULL, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	ถ้า (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    ถ้า (RedrawingDisabled > 0)
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
    ถ้า (do_profiling == PROF_YES)
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
    ถ้า (!autocmd_busy)
    {
	restore_search_patterns();
	ถ้า (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	ขณะ (au_pending_free_buf != NULL)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	ขณะ (au_pending_free_win != NULL)
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
    ถ้า (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	ถ้า (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    ถ้า (event == EVENT_BUFWIPEOUT && buf != NULL)
	aubuflocal_remove(buf);

    ถ้า (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    คืน retval;
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
    ถ้า (autocmd_blocked == 0)
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
    ถ้า (autocmd_blocked == 0)
    {
	ถ้า (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, NULL, NULL, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", NULL, FALSE, curbuf);
	}
	ถ้า (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", NULL, FALSE, curbuf);
	}
	ถ้า (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", NULL, FALSE, curbuf);
	}
	ถ้า (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", NULL, FALSE, curbuf);
	}
	ถ้า (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", NULL, FALSE, curbuf);
	}
	ถ้า (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", NULL, FALSE, curbuf);
	}
    }
#endif
}

    จำนวนเต็ม
is_autocmd_blocked(void)
{
    คืน autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    static void
auto_next_pat(
    AutoPatCmd_T *apc,
    จำนวนเต็ม		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = NULL;

    สำหรับ (ap = apc->curpat; ap != NULL && !got_int; ap = ap->next)
    {
	apc->curpat = NULL;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	ถ้า (ap->pat != NULL && ap->cmds != NULL
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    ถ้า (ap->buflocal_nr == 0
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
		ถ้า (namep != NULL)
		{
		    sprintf((var_na *)namep, fmt, (var_na *)event_name->string, (var_na *)ap->pat);
		    ถ้า (p_verbose >= 8)
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
		สำหรับ (cp = ap->cmds; cp->next != NULL; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    line_breakcheck();
	    ถ้า (apc->curpat != NULL)	    // found a match
		bha_nga;
	}
	ถ้า (stop_at_last && ap->last)
	    bha_nga;
    }
}

#ถ้า defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    คืน &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	จำนวนเต็ม c UNUSED,
	void *cookie,
	จำนวนเต็ม indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    ถ้า (acp->curpat == NULL)
	คืน NULL;

    // repeat until we find an autocommand to execute
    สำหรับ (;;)
    {
	// skip removed commands
	ขณะ (acp->nextcmd != NULL && acp->nextcmd->cmd == NULL)
	    ถ้า (acp->nextcmd->last)
		acp->nextcmd = NULL;
	    มิฉะนั้น
		acp->nextcmd = acp->nextcmd->next;

	ถ้า (acp->nextcmd != NULL)
	    bha_nga;

	// at end of commands, find next pattern that matches
	ถ้า (acp->curpat->last)
	    acp->curpat = NULL;
	มิฉะนั้น
	    acp->curpat = acp->curpat->next;
	ถ้า (acp->curpat != NULL)
	    auto_next_pat(acp, TRUE);
	ถ้า (acp->curpat == NULL)
	    คืน NULL;
    }

    ac = acp->nextcmd;

    ถ้า (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	msg_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    ถ้า (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    ถ้า (ac->last)
	acp->nextcmd = NULL;
    มิฉะนั้น
	acp->nextcmd = ac->next;
    คืน retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    static จำนวนเต็ม
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    จำนวนเต็ม		retval = FALSE;

    ถ้า (first_autopat[(จำนวนเต็ม)event] == NULL)
	คืน FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    ถ้า (fname == NULL)
	คืน FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    ถ้า (sfname != NULL)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	ถ้า (ap->pat != NULL && ap->cmds != NULL
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

    คืน retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    จำนวนเต็ม
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    คืน has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    จำนวนเต็ม
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    คืน has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, จำนวนเต็ม idx)
{
    ถ้า (idx == augroups.ga_len)		// add "END" add the end
	คืน (char_u *)"END";
    ถ้า (idx < 0 || idx >= augroups.ga_len)	// end of list
	คืน NULL;
    ถ้า (AUGROUP_NAME(idx) == NULL || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	คืน (char_u *)"";
    คืน AUGROUP_NAME(idx);		// return a name
}

static จำนวนเต็ม include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    จำนวนเต็ม		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    จำนวนเต็ม		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    ถ้า (group == AUGROUP_ERROR)
	คืน NULL;
    // If there only is a group name that's what we expand.
    ถ้า (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    สำหรับ (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	ถ้า (*p == ',')
	    arg = p + 1;
    ถ้า (*p == NUL)
    {
	ถ้า (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	คืน NULL;
    }

    // skip over pattern
    arg = skipwhite(p);
    ขณะ (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    ถ้า (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	สำหรับ (จำนวนเต็ม i = 0; i < 2; i++)
	{
	    ถ้า (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    ถ้า (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    ถ้า (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	คืน arg;			    // expand (next) command
    }

    ถ้า (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    มิฉะนั้น
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    คืน NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, จำนวนเต็ม idx)
{
    จำนวนเต็ม i;

    ถ้า (idx < augroups.ga_len)		// First list group names, if wanted
    {
	ถ้า (!include_groups || AUGROUP_NAME(idx) == NULL
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    คืน (char_u *)"";	// skip deleted entries
	คืน AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    ถ้า (i < 0 || i >= NUM_EVENTS)
	คืน NULL;

    คืน event_tab[i].value.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, จำนวนเต็ม idx, จำนวนเต็ม win)
{
    ถ้า (idx < 0 || idx >= NUM_EVENTS)
	คืน NULL;

    ถ้า (!win)
	คืน event_tab[idx].value.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    จำนวนเต็ม j = 0;
    สำหรับ (จำนวนเต็ม i = 0; i < NUM_EVENTS; ++i)
    {
	j += event_tab[i].key <= 0;
	ถ้า (j == idx + 1)
	    คืน event_tab[i].value.string;
    }

    คืน NULL;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    จำนวนเต็ม
has_tabclosedpre(void)
{
    คืน (first_autopat[(จำนวนเต็ม)EVENT_TABCLOSEDPRE] != NULL);
}

#ถ้า defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    จำนวนเต็ม
autocmd_supported(char_u *name)
{
    char_u *p;

    คืน (event_name2nr(name, &p) != NUM_EVENTS);
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
    จำนวนเต็ม
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = NULL;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = NULL;
    จำนวนเต็ม		group;
    จำนวนเต็ม		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    ถ้า (arg_save == NULL)
	คืน FALSE;
    p = vim_strchr(arg_save, '#');
    ถ้า (p != NULL)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    ถ้า (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    มิฉะนั้น
    {
	ถ้า (p == NULL)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    goto theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	ถ้า (p != NULL)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    ถ้า (event == NUM_EVENTS)
	goto theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(จำนวนเต็ม)event];
    ถ้า (ap == NULL)
	goto theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    ถ้า (pattern != NULL && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    สำหรับ ( ; ap != NULL; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	ถ้า (ap->pat != NULL && ap->cmds != NULL
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
    คืน retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    static void
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, จำนวนเต็ม delete)
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
    จำนวนเต็ม		group;
    char_u	*pat = NULL;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = NULL;
    char_u	*end;
    จำนวนเต็ม		once;
    จำนวนเต็ม		nested;
    จำนวนเต็ม		replace;		// replace the cmd for a group/event
    จำนวนเต็ม		retval = VVAL_TRUE;
    จำนวนเต็ม		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_number = VVAL_FALSE;

    ถ้า (check_restricted() || check_secure())
	คืน;

    ถ้า (check_for_list_arg(argvars, 0) == FAIL)
	คืน;

    aucmd_list = argvars[0].vval.v_list;
    ถ้า (aucmd_list == NULL)
	คืน;

    FOR_ALL_LIST_ITEMS(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = NULL;
	event_list = NULL;
	pat = NULL;
	pat_list = NULL;

	ถ้า (li->li_tv.v_type != VAR_DICT)
	    continue;

	event_dict = li->li_tv.vval.v_dict;
	ถ้า (event_dict == NULL)
	    continue;

	di = dict_find(event_dict, (char_u *)"event", -1);
	ถ้า (di != NULL)
	{
	    ถ้า (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		ถ้า (event_name == NULL)
		{
		    emsg(_(e_string_required));
		    continue;
		}
	    }
	    มิฉะนั้น ถ้า (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		ถ้า (event_list == NULL)
		{
		    emsg(_(e_list_required));
		    continue;
		}
	    }
	    มิฉะนั้น
	    {
		emsg(_(e_string_or_list_expected));
		continue;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	ถ้า (group_name == NULL || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	มิฉะนั้น
	{
	    group = au_find_group(group_name);
	    ถ้า (group == AUGROUP_ERROR)
	    {
		ถ้า (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    bha_nga;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		ถ้า (group == AUGROUP_ERROR)
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
	ถ้า (dict_has_key(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_number_def(event_dict, "bufnr", -1);
	    ถ้า (bnum == -1)
		continue;

	    vim_snprintf((var_na *)IObuff, IOSIZE, "<buffer=%d>", (จำนวนเต็ม)bnum);
	    pat = IObuff;
	}
	มิฉะนั้น
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    ถ้า (di != NULL)
	    {
		ถ้า (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    ถ้า (pat == NULL)
		    {
			emsg(_(e_string_required));
			continue;
		    }
		}
		มิฉะนั้น ถ้า (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    ถ้า (pat_list == NULL)
		    {
			emsg(_(e_list_required));
			continue;
		    }
		}
		มิฉะนั้น
		{
		    emsg(_(e_string_or_list_expected));
		    continue;
		}
	    }
	    มิฉะนั้น ถ้า (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	ถ้า (cmd == NULL)
	{
	    ถ้า (delete)
		cmd = vim_strsave((char_u *)"");
	    มิฉะนั้น
		continue;
	}

	ถ้า (delete && (event_name == NULL
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    สำหรับ (event = (event_T)0; (จำนวนเต็ม)event < NUM_EVENTS;
		    event = (event_T)((จำนวนเต็ม)event + 1))
	    {
		ถ้า (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    bha_nga;
		}
	    }
	}
	มิฉะนั้น
	{
	    char_u *p = NULL;

	    eli = NULL;
	    end = NULL;
	    ขณะ (TRUE)
	    {
		ถ้า (event_list != NULL)
		{
		    ถ้า (eli == NULL)
			eli = event_list->lv_first;
		    มิฉะนั้น
			eli = eli->li_next;
		    ถ้า (eli == NULL)
			bha_nga;
		    ถ้า (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == NULL)
		    {
			emsg(_(e_string_required));
			bha_nga;
		    }
		}
		มิฉะนั้น
		{
		    ถ้า (p == NULL)
			p = event_name;
		    ถ้า (p == NULL || *p == NUL)
			bha_nga;
		}

		event = event_name2nr(p, &end);
		ถ้า (event == NUM_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    bha_nga;
		}
		ถ้า (pat != NULL)
		{
		    ถ้า (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			bha_nga;
		    }
		}
		มิฉะนั้น ถ้า (pat_list != NULL)
		{
		    FOR_ALL_LIST_ITEMS(pat_list, pli)
		    {
			ถ้า (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == NULL)
			{
			    emsg(_(e_string_required));
			    continue;
			}
			ถ้า (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    bha_nga;
			}
		    }
		    ถ้า (retval == VVAL_FALSE)
			bha_nga;
		}
		ถ้า (event_name != NULL)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	ถ้า (delete && group_name != NULL &&
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
    จำนวนเต็ม		group = AUGROUP_ALL;

    ถ้า (rettv_list_alloc(rettv) == FAIL)
	คืน;
    ถ้า (check_for_opt_dict_arg(argvars, 0) == FAIL)
	คืน;

    ถ้า (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	ถ้า (dict_has_key(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    ถ้า (name == NULL)
		คืน;

	    ถ้า (*name == NUL)
		group = AUGROUP_DEFAULT;
	    มิฉะนั้น
	    {
		group = au_find_group(name);
		ถ้า (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    คืน;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	ถ้า (dict_has_key(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    ถ้า (name == NULL)
		คืน;

	    ถ้า (name[0] == '*' && name[1] == NUL)
		event_arg = NUM_EVENTS;
	    มิฉะนั้น
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.key = 0;
		target.value.string = name;
		target.value.length = STRLEN(target.value.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    NUM_EVENTS, parimaa_na(event_tab[0]), cmp_keyvalue_value_ni);
		ถ้า (entry == NULL)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    คืน;
		}
		event_arg = (event_T)abs(entry->key);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	ถ้า (dict_has_key(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    ถ้า (pat == NULL)
		คืน;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    สำหรับ (event = (event_T)0; (จำนวนเต็ม)event < NUM_EVENTS;
	    event = (event_T)((จำนวนเต็ม)event + 1))
    {
	string_T    *event_name;

	ถ้า (event_arg != NUM_EVENTS && event != event_arg)
	    continue;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    ถ้า (ap->pat == NULL)		// pattern has been removed
		continue;

	    ถ้า (group != AUGROUP_ALL && group != ap->group)
		continue;

	    ถ้า (pat != NULL && STRCMP(pat, ap->pat) != 0)
		continue;

	    group_name.string = get_augroup_name(NULL, ap->group);
	    ถ้า (group_name.string == NULL)
		STR_LITERAL_SET(group_name, "");
	    มิฉะนั้น
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    สำหรับ (ac = ap->cmds; ac != NULL; ac = ac->next)
	    {
		event_dict = dict_alloc();
		ถ้า (event_dict == NULL
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    คืน;
		}

		ถ้า (dict_add_string_len(event_dict, "event",
			event_name->string, (จำนวนเต็ม)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (จำนวนเต็ม)group_name.length) == FAIL
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
		    คืน;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
