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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    ଅକ୍ଷର	    once;		// "One shot": removed after execution
    ଅକ୍ଷର	    nested;		// If autocommands nest here.
    ଅକ୍ଷର	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    ସଂରଚନା AutoCmd  *next;		// next AutoCmd in list
} AutoCmd;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା AutoPat
{
    ସଂରଚନା AutoPat  *next;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    group;		// group ID
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    patlen;		// strlen() of pat
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    buflocal_nr;	// !=0 for buffer-local AutoPat
    ଅକ୍ଷର	    allow_dirs;		// Pattern may match whole path
    ଅକ୍ଷର	    last;		// last pattern for apply_autocmds()
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
ସ୍ଥିର keyvalue_T event_tab[ଅଙ୍କ_EVENTS] = {
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

ସ୍ଥିର AutoPat *first_autopat[ଅଙ୍କ_EVENTS] = { ଖାଲି };
ସ୍ଥିର AutoPat *last_autopat[ଅଙ୍କ_EVENTS] = { ଖାଲି };

#define AUGROUP_DEFAULT    (-1)	    // default autocmd group
#define AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#define AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
ସଂରଚନା AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

ସ୍ଥିର AutoPatCmd_T *active_apc_list = ଖାଲି; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    ପାଇଁ ((ap) = first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(event)]; (ap) != ଖାଲି; (ap) = (ap)->next)

/*
 * augroups stores a list of autocmd group names.
 */
ସ୍ଥିର garray_T augroups = {0, 0, ଆକାର(char_u *), 10, ଖାଲି};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])
// use get_deleted_augroup() to get this
ସ୍ଥିର char_u *deleted_augroup = ଖାଲି;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_augroup = AUGROUP_DEFAULT;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା au_need_clean = FALSE;   // need to delete marked patterns

ସ୍ଥିର event_T event_name2nr(char_u *start, char_u **end);
ସ୍ଥିର string_T *event_nr2name(event_T event);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା au_get_grouparg(char_u **argp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_autocmd_event(event_T event, char_u *pat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା once, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nested, char_u *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା group, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force, ପୂର୍ଣ୍ଣସଂଖ୍ୟା group, buf_T *buf, exarg_T *eap);
ସ୍ଥିର ଶୂନ୍ୟ auto_next_pat(AutoPatCmd_T *apc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା stop_at_last);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା au_find_group(char_u *name);

ସ୍ଥିର event_T	last_event;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	last_group;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	autocmd_blocked = 0;	// block all autocmds

    ସ୍ଥିର char_u *
get_deleted_augroup(ଶୂନ୍ୟ)
{
    ଯଦି (deleted_augroup == ଖାଲି)
	deleted_augroup = (char_u *)_("--Deleted--");
    ଫେରନ୍ତୁ deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    ଯଦି (got_int)
	ଫେରନ୍ତୁ;
    ଯଦି (ap->pat == ଖାଲି)		// pattern has been removed
	ଫେରନ୍ତୁ;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    ସନ୍ଦେଶ_putchar('\n');
    ଯଦି (got_int)
	ଯାଅ theend;
    ଯଦି (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	ଯଦି (ap->group != AUGROUP_DEFAULT)
	{
	    ଯଦି (AUGROUP_NAME(ap->group) == ଖାଲି)
		ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    ଅନ୍ୟଥା
		ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    ସନ୍ଦେଶ_puts("  ");
	}
	event_name = event_nr2name(event);
	ସନ୍ଦେଶ_puts_attr((ଅକ୍ଷର *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	ସନ୍ଦେଶ_putchar('\n');
	ଯଦି (got_int)
	    ଯାଅ theend;
    }

    ଯଦି (ap->pat == ଖାଲି)
	ଯାଅ theend;  // timer might have cleared the pattern or group

    ସନ୍ଦେଶ_col = 4;
    ସନ୍ଦେଶ_outtrans(ap->pat);

    ପାଇଁ (ac = ap->cmds; ac != ଖାଲି; ac = ac->next)
    {
	ଯଦି (ac->cmd == ଖାଲି)		// skip removed commands
	    ଜାରି;

	ଯଦି (ସନ୍ଦେଶ_col >= 14)
	    ସନ୍ଦେଶ_putchar('\n');
	ସନ୍ଦେଶ_col = 14;
	ଯଦି (got_int)
	    ଯାଅ theend;
	ସନ୍ଦେଶ_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	ଯଦି (p_verbose > 0)
	    last_set_ସନ୍ଦେଶ(ac->script_ctx);
#endif
	ଯଦି (got_int)
	    ଯାଅ theend;
	ଯଦି (ac->next != ଖାଲି)
	{
	    ସନ୍ଦେଶ_putchar('\n');
	    ଯଦି (got_int)
		ଯାଅ theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    ପାଇଁ (ac = ap->cmds; ac != ଖାଲି; ac = ac->next)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
ସ୍ଥିର ଶୂନ୍ୟ au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
au_cleanup(ଶୂନ୍ୟ)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    ଯଦି (autocmd_busy || !au_need_clean)
	ଫେରନ୍ତୁ;

    // loop over all events
    ପାଇଁ (event = (event_T)0; (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event < ଅଙ୍କ_EVENTS;
					    event = (event_T)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event]);
	ପାଇଁ (ap = *prev_ap; ap != ଖାଲି; ap = *prev_ap)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    ପାଇଁ (ac = *prev_ac; ac != ଖାଲି; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		ଯଦି (ap->pat == ଖାଲି || ac->cmd == ଖାଲି)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		ଅନ୍ୟଥା
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    ଯଦି (ap->pat != ଖାଲି && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    ଯଦି (ap->pat == ଖାଲି)
	    {
		ଯଦି (ap->next == ଖାଲି)
		{
		    ଯଦି (prev_ap == &(first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event]))
			last_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event] = ଖାଲି;
		    ଅନ୍ୟଥା
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    ଅନ୍ୟଥା
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    ଶୂନ୍ୟ
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    ପାଇଁ (apc = active_apc_list; apc; apc = apc->next)
	ଯଦି (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    ପାଇଁ (event = (event_T)0; (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event < ଅଙ୍କ_EVENTS;
					    event = (event_T)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ଯଦି (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		ଯଦି (p_verbose >= 6)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
au_new_group(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    i = au_find_group(name);
    ଯଦି (i != AUGROUP_ERROR)
	ଫେରନ୍ତୁ i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    ପାଇଁ (i = 0; i < augroups.ga_len; ++i)
	ଯଦି (AUGROUP_NAME(i) == ଖାଲି)
	    ଭାଙ୍ଗ;
    ଯଦି (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	ଫେରନ୍ତୁ AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    ଯଦି (AUGROUP_NAME(i) == ଖାଲି)
	ଫେରନ୍ତୁ AUGROUP_ERROR;
    ଯଦି (i == augroups.ga_len)
	++augroups.ga_len;

    ଫେରନ୍ତୁ i;
}

    ସ୍ଥିର ଶୂନ୍ୟ
au_del_group(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    event_T	event;
    AutoPat	*ap;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_use = FALSE;


    i = au_find_group(name);
    ଯଦି (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (event = (event_T)0; (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event < ଅଙ୍କ_EVENTS;
	    event = (event_T)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    ଯଦି (ap->group == i && ap->pat != ଖାଲି)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = ଅଙ୍କ_EVENTS;
		ଭାଙ୍ଗ;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    ଯଦି (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    ଅନ୍ୟଥା
	AUGROUP_NAME(i) = ଖାଲି;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
au_find_group(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ପାଇଁ (i = 0; i < augroups.ga_len; ++i)
	ଯଦି (AUGROUP_NAME(i) != ଖାଲି && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    ଫେରନ୍ତୁ i;
    ଫେରନ୍ତୁ AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
au_has_group(char_u *name)
{
    ଫେରନ୍ତୁ au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    ଶୂନ୍ୟ
do_augroup(char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା del_group)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ଯଦି (del_group)
    {
	ଯଦି (*arg == NUL)
	    emsg(_(e_argument_required));
	ଅନ୍ୟଥା
	    au_del_group(arg);
    }
    ଅନ୍ୟଥା ଯଦି (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    ଅନ୍ୟଥା ଯଦି (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	ଯଦି (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    ଅନ୍ୟଥା			    // ":aug": list the group names
    {
	ସନ୍ଦେଶ_start();
	ପାଇଁ (i = 0; i < augroups.ga_len; ++i)
	{
	    ଯଦି (AUGROUP_NAME(i) != ଖାଲି)
	    {
		ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)AUGROUP_NAME(i));
		ସନ୍ଦେଶ_puts("  ");
	    }
	}
	ସନ୍ଦେଶ_clr_eos();
	ସନ୍ଦେଶ_end();
    }
}

    ଶୂନ୍ୟ
autocmd_init(ଶୂନ୍ୟ)
{
    CLEAR_FIELD(aucmd_win);
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_all_autocmds(ଶୂନ୍ୟ)
{
    char_u	*s;

    ପାଇଁ (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(ଖାଲି, (char_u *)"", TRUE);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	ଯଦି (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_aucmd_win(win_T *win)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < AUCMD_WIN_ଗଣନା; ++i)
	ଯଦି (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    ସ୍ଥିର event_T
event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    keyvalue_T target;
    keyvalue_T *entry;
    ସ୍ଥିର keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_INDEX];
    ସ୍ଥିର keyvalue_T *bufread = &event_tab[BUFREAD_INDEX];

    // the event name ends with end of line, '|', a blank or a comma
    ପାଇଁ (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.ଚାବି = 0;
    target.ମୂଲ୍ୟ.string = start;
    target.ମୂଲ୍ୟ.length = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    ଯଦି (cmp_keyvalue_ମୂଲ୍ୟ_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    ଅନ୍ୟଥା ଯଦି (cmp_keyvalue_ମୂଲ୍ୟ_ni(&target, bufread) == 0)
	entry = bufread;
    ଅନ୍ୟଥା
	entry = (keyvalue_T *)bsearch(&target, &event_tab, ଅଙ୍କ_EVENTS,
				ଆକାର(event_tab[0]), cmp_keyvalue_ମୂଲ୍ୟ_ni);

    ଯଦି (*p == ',')
	++p;
    *end = p;

    ଫେରନ୍ତୁ (entry == ଖାଲି) ? ଅଙ୍କ_EVENTS : (event_T)abs(entry->ଚାବି);
}

/*
 * Return the name for event "event".
 */
    ସ୍ଥିର string_T *
event_nr2name(event_T event)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ଗଣନା {CACHE_SIZE = 12};
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache_tab[CACHE_SIZE];
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache_last_index = -1;
    ସ୍ଥିର string_T unknown = STR_LITERAL_INIT("Unknown");

    ଯଦି (cache_last_index < 0)
    {
	ପାଇଁ (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    ପାଇଁ (i = cache_last_index; cache_tab[i] >= 0; )
    {
	ଯଦି ((event_T)abs(event_tab[cache_tab[i]].ଚାବି) == event)
	    ଫେରନ୍ତୁ &event_tab[cache_tab[i]].ମୂଲ୍ୟ;

	ଯଦି (i == 0)
	    i = CACHE_SIZE - 1;
	ଅନ୍ୟଥା
	    --i;

	// are we back at the start?
	ଯଦି (i == cache_last_index)
	    ଭାଙ୍ଗ;
    }

    // look in the event table itself
    ପାଇଁ (i = 0; i < ଅଙ୍କ_EVENTS; ++i)
    {
	ଯଦି ((event_T)abs(event_tab[i].ଚାବି) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    ଯଦି (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    ଅନ୍ୟଥା
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    ଫେରନ୍ତୁ &event_tab[i].ମୂଲ୍ୟ;
	}
    }

    ଫେରନ୍ତୁ &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    ସ୍ଥିର char_u *
find_end_event(
    char_u  *arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    ଯଦି (*arg == '*')
    {
	ଯଦି (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	pat = arg + 1;
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event_name2nr(pat, &p) >= ଅଙ୍କ_EVENTS)
	    {
		ଯଦି (have_group)
		    semsg(_(e_no_such_event_str), pat);
		ଅନ୍ୟଥା
		    semsg(_(e_no_such_group_or_event_str), pat);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
    }
    ଫେରନ୍ତୁ pat;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group;

    ଯଦି (*arg == '|')
	ଫେରନ୍ତୁ ଖାଲି;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? ଖାଲି : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    ଯଦି (pat == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    pat = skipwhite(pat);
    ଯଦି (*pat == NUL || *pat == '|')
	ଫେରନ୍ତୁ ଖାଲି;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    ଯେତେବେଳେ (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 2; ++i)
    {
	ଯଦି (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	ଯଦି (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	ଯଦି (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    ଫେରନ୍ତୁ *cmd == NUL ? ଖାଲି : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
event_ignored(event_T event, char_u *ei)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignored = FALSE;
    ଯେତେବେଳେ (*ei != NUL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା unignore = *ei == '-';
	ei += unignore;
	ଯଦି (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].ଚାବି <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	ଅନ୍ୟଥା ଯଦି (event_name2nr(ei, &ei) == event)
	{
	    ଯଦି (unignore)
		ଫେରନ୍ତୁ FALSE;
	    ଅନ୍ୟଥା
		ignored = TRUE;
	}
    }

    ଫେରନ୍ତୁ ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_ei(char_u *ei)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	win = ei != p_ei;

    ଯେତେବେଳେ (*ei)
    {
	ଯଦି (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	ଅନ୍ୟଥା
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    ଯଦି (event == ଅଙ୍କ_EVENTS || (win && event_tab[event].ଚାବି > 0))
		ଫେରନ୍ତୁ FAIL;
	}
    }

    ଫେରନ୍ତୁ OK;
}

#ଯଦି defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(ଅକ୍ଷର *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    ଯଦି (save_ei == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    ଯଦି (new_ei == ଖାଲି)
    {
	vim_free(save_ei);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    ଅନ୍ୟଥା
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    ଫେରନ୍ତୁ save_ei;
}

    ଶୂନ୍ୟ
au_event_restore(char_u *old_ei)
{
    ଯଦି (old_ei != ଖାଲି)
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
    ଶୂନ୍ୟ
do_autocmd(exarg_T *eap, char_u *arg_in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = ଖାଲି;
    char_u	*cmd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nested = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		once = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;

    ଯଦି (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    ଅନ୍ୟଥା
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	ଯଦି (arg == ଖାଲି)	    // out of memory
	    ଫେରନ୍ତୁ;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    ଯଦି (pat == ଖାଲି)
	ଫେରନ୍ତୁ;

    pat = skipwhite(pat);
    ଯଦି (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    ଅନ୍ୟଥା
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	ଯେତେବେଳେ (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	ଯଦି (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	ଯଦି (vim_strchr(pat, '$') != ଖାଲି || vim_strchr(pat, '~') != ଖାଲି)
	{
#ifdef BACKSLASH_IN_FILENAME
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    ଯଦି (envpat != ଖାଲି)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	ପାଇଁ (i = 0; i < 2; i++)
	{
	    ଯଦି (*cmd == NUL)
		ଜାରି;

	    // Check for "++once" flag.
	    ଯଦି (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ଯଦି (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    ଯାଅ err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    ଯଦି ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		ଯଦି (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    ଯାଅ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    ଯଦି (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		ଯଦି (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    ଯାଅ err_exit;
		}
		ଯଦି (nested)
		{
		    semsg(_(e_duplicate_argument_str), "nested");
		    ଯାଅ err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	ଯଦି (*cmd != NUL)
	{
	    ଯଦି (eap != ଖାଲି)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    ଯଦି (cmd == ଖାଲି)	    // some error
		ଯାଅ err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    ଯଦି (!forceit && *cmd == NUL)
	// Highlight title
	ସନ୍ଦେଶ_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    ଯଦି (*arg == '*' || *arg == NUL || *arg == '|')
    {
	ଯଦି (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	ଅନ୍ୟଥା
	    ପାଇଁ (event = (event_T)0; (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event < ଅଙ୍କ_EVENTS;
					     event = (event_T)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event + 1))
		ଯଦି (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା
    {
	ଯେତେବେଳେ (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    ଯଦି (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		ଭାଙ୍ଗ;
    }

err_exit:
    ଯଦି (cmd_need_free)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group = AUGROUP_ALL;

    ପାଇଁ (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    ଯଦି (p <= arg)
	ଫେରନ୍ତୁ AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    ଯଦି (group_name == ଖାଲି)		// out of memory
	ଫେରନ୍ତୁ AUGROUP_ERROR;
    group = au_find_group(group_name);
    ଯଦି (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    ଅନ୍ୟଥା
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    ଫେରନ୍ତୁ group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		once,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nested,
    char_u	*cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		brace_level;
    char_u	*endpat;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		findgroup;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		allgroups;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		patlen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_buflocal;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    ଯଦି (group == AUGROUP_ALL)
	findgroup = current_augroup;
    ଅନ୍ୟଥା
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    ଯଦି (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    ଯଦି (forceit)  // delete the AutoPat, if it's in the current group
	    {
		ଯଦି (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    ଅନ୍ୟଥା ଯଦି (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    ପାଇଁ ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	ପାଇଁ (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    ଯଦି (*endpat == '{')
		brace_level++;
	    ଅନ୍ୟଥା ଯଦି (*endpat == '}')
		brace_level--;
	}
	ଯଦି (pat == endpat)		// ignore single comma
	    ଜାରି;
	patlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	ଯଦି (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    ଯଦି (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    ଅନ୍ୟଥା ଯଦି (patlen > 9 && pat[7] == '=')
	    {
		ଯଦି (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		ଅନ୍ୟଥା ଯଦି (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((ଅକ୍ଷର *)pat + 8);
	    }
	}

	ଯଦି (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((ଅକ୍ଷର *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	ଯଦି (!forceit && *cmd != NUL && last_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event] != ଖାଲି)
	    prev_ap = &last_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event];
	ଅନ୍ୟଥା
	    prev_ap = &first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event];
	ଯେତେବେଳେ ((ap = *prev_ap) != ଖାଲି)
	{
	    ଯଦି (ap->pat != ଖାଲି)
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
		ଯଦି ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    ଯଦି (forceit)
		    {
			ଯଦି (*cmd != NUL && ap->next == ଖାଲି)
			{
			    au_remove_cmds(ap);
			    ଭାଙ୍ଗ;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    ଅନ୍ୟଥା ଯଦି (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    ଅନ୍ୟଥା ଯଦି (ap->next == ଖାଲି)
			ଭାଙ୍ଗ;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	ଯଦି (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    ଯଦି (ap == ଖାଲି)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		ଯଦି (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == ଖାଲି))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_ସଂଖ୍ୟା), buflocal_nr);
		    ଫେରନ୍ତୁ FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		ଯଦି (ap == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		ଯଦି (ap->pat == ଖାଲି)
		{
		    vim_free(ap);
		    ଫେରନ୍ତୁ FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		ଯଦି (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		ଯଦି ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
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

		ଯଦି (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = ଖାଲି;
		}
		ଅନ୍ୟଥା
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    ଯଦି (reg_pat != ଖାଲି)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    ଯଦି (reg_pat == ଖାଲି || ap->reg_prog == ଖାଲି)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			ଫେରନ୍ତୁ FAIL;
		    }
		}
		ap->cmds = ଖାଲି;
		*prev_ap = ap;
		last_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event] = ap;
		ap->next = ଖାଲି;
		ଯଦି (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		ଅନ୍ୟଥା
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    ଯେତେବେଳେ ((ac = *prev_ac) != ଖାଲି)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    ଯଦି (ac == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    ଯଦି (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    ଯଦି (ac->cmd == ଖାଲି)
	    {
		vim_free(ac);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ac->next = ଖାଲି;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    ଫେରନ୍ତୁ OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_doautocmd(
    char_u	*arg_start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_ସନ୍ଦେଶ,	    // give message for no matching autocmds?
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nothing_done = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group;

    ଯଦି (did_something != ଖାଲି)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    ଯଦି (arg == ଖାଲି)	    // out of memory
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	ଫେରନ୍ତୁ FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    ଯେତେବେଳେ (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	ଯଦି (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, ଖାଲି, TRUE, group, curbuf, ଖାଲି))
	    nothing_done = FALSE;

    ଯଦି (nothing_done && do_ସନ୍ଦେଶ
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    ଯଦି (did_something != ଖାଲି)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    ଫେରନ୍ତୁ aborting() ? FAIL : OK;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    ଶୂନ୍ୟ
ex_doautoall(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		call_do_modelines = check_nomodeline(&arg);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_aucmd;

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
	ଯଦି (buf->b_ml.ml_mfp == ଖାଲି || buf == curbuf)
	    ଜାରି;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	ଯଦି (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    ଭାଙ୍ଗ;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	ଯଦି (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	ଯଦି (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    ଭାଙ୍ଗ;
	}
    }

    // Execute autocommands for the current buffer last.
    ଯଦି (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	ଯଦି (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_nomodeline(char_u **argp)
{
    ଯଦି (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    ଶୂନ୍ୟ
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ea;
#ifdef FEAT_AUTOCHDIR
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_acd;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    ଯଦି (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    ଅନ୍ୟଥା
	FOR_ALL_WINDOWS(win)
	    ଯଦି (win->w_buffer == buf)
		ଭାଙ୍ଗ;

    // Allocate a window when needed.
    win_T *auc_win = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା auc_idx = AUCMD_WIN_ଗଣନା;
    ଯଦି (win == ଖାଲି)
    {
	ପାଇଁ (auc_idx = 0; auc_idx < AUCMD_WIN_ଗଣନା; ++auc_idx)
	    ଯଦି (!aucmd_win[auc_idx].auc_win_used)
	    {
		ଯଦି (aucmd_win[auc_idx].auc_win == ଖାଲି)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		ଯଦି (auc_win != ଖାଲି)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		ଭାଙ୍ଗ;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	ଯଦି (auc_win == ଖାଲି)
	    ଫେରନ୍ତୁ;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == ଖାଲି ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    ଯଦି (bt_prompt(curbuf))
	aco->save_prompt_ଭର୍ତ୍ତି = curbuf->b_prompt_ଭର୍ତ୍ତି;
#endif

    ଯଦି (win != ଖାଲି)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    ଅନ୍ୟଥା
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
	curtab->tp_localdir = ଖାଲି;
	aco->globaldir = globaldir;
	globaldir = ଖାଲି;

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

	(ଶୂନ୍ୟ)win_split_ins(0, WSP_ଶୀର୍ଷ | WSP_FORCE_ROOM, auc_win, 0, ଖାଲି);
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
    ଯଦି (!same_buffer)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_active = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    ଶୂନ୍ୟ
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    dummy;
    win_T   *save_curwin;

    ଯଦି (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	ଯଦି (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		ଯଦି (wp == awp)
		{
		    ଯଦି (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    ଯାଅ win_found;
		}
	    }
	}
win_found:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(ଶୂନ୍ୟ)winframe_remove(curwin, &dummy, ଖାଲି, ଖାଲି);
	win_remove(curwin, ଖାଲି);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_used = FALSE;
	last_status(FALSE);	    // may need to remove last status line

	ଯଦି (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	ଯଦି (save_curwin != ଖାଲି)
	    curwin = save_curwin;
	ଅନ୍ୟଥା
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	ଯଦି (bt_prompt(curbuf))
	    curbuf->b_prompt_ଭର୍ତ୍ତି = aco->save_prompt_ଭର୍ତ୍ତି;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	ଯଦି (awp->w_localdir != ଖାଲି)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	ଯଦି (curwin->w_topline > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    curwin->w_topline = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#ଯଦି defined(FEAT_GUI)
	ଯଦି (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ବାମ], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_ଡାହାଣ], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    ଅନ୍ୟଥା
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	ଯଦି (save_curwin != ଖାଲି)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    ଯଦି (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != ଖାଲି)
	    {
#ଯଦି defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		ଯଦି (curwin->w_s == &curbuf->b_s)
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
    ଯଦି (VIsual_active)
	check_pos(curbuf, &VIsual);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    ଫେରନ୍ତୁ apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ଖାଲି);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    ଫେରନ୍ତୁ apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*retval)    // pointer to caller's retval
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_cmd;

#ifdef FEAT_EVAL
    ଯଦି (should_abort(*retval))
	ଫେରନ୍ତୁ FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, ଖାଲି);
    ଯଦି (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    ଫେରନ୍ତୁ did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_cursorhold(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != ଖାଲି);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
trigger_cursorhold(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		state;

    ଯଦି (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	ଯଦି (state == MODE_NORMAL_BUSY || (state & MODE_ଭର୍ତ୍ତି) != 0)
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_winresized(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_WINRESIZED] != ଖାଲି);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_winscrolled(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_WINSCROLLED] != ଖାଲି);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_cursormoved(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_CURSORMOVED] != ଖାଲି);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_cursormovedI(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_CURSORMOVEDI] != ଖାଲି);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_textchanged(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_TEXTCHANGED] != ଖାଲି);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_textchangedI(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_TEXTCHANGEDI] != ଖାଲି);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_textchangedP(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_TEXTCHANGEDP] != ଖାଲି);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_insertcharpre(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_INSERTCHARPRE] != ଖାଲି);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_keyinputpre(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_KEYINPUTPRE] != ଖାଲି);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_cmdundefined(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_CMDUNDEFINED] != ଖାଲି);
}

#ଯଦି defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_textputpost(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_TEXTPUTPOST] != ଖାଲି);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_textputpre(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_TEXTPUTPRE] != ଖାଲି);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_textyankpost(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_TEXTYANKPOST] != ଖାଲି);
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_completechanged(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_COMPLETECHANGED] != ଖାଲି);
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_modechanged(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_MODECHANGED] != ଖାଲି);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force,	     // when TRUE, ignore autocmd_busy
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = ଖାଲି;	// short file name
    char_u	*tail;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_changed;
    buf_T	*old_curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;
    char_u	*save_autocmd_fname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_autocmd_fname_full;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_autocmd_busy;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_autocmd_nested;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ଚାବି_Typed = ଚାବି_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    ଯଦି (event == ଅଙ୍କ_EVENTS || first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event] == ଖାଲି
	    || autocmd_blocked > 0)
	ଯାଅ BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    ଯଦି (autocmd_busy && !(force || autocmd_nested))
	ଯାଅ BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    ଯଦି (aborting())
	ଯାଅ BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    ଯଦି (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	ଯାଅ BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    ଯଦି (event_ignored(event, p_ei))
	ଯାଅ BYPASS_AU;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    ଯଦି (buf == curbuf && event_tab[event].ଚାବି <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    ଅନ୍ୟଥା ଯଦି (buf != ଖାଲି && event_tab[event].ଚାବି <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ଯଦି (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		ଭାଙ୍ଗ;
	    }
    }
    ଯଦି (win_ignore)
	ଯାଅ BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    ଯଦି (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	ଯାଅ BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    ଯଦି (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	ଯାଅ BYPASS_AU;

    ଯଦି (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_changed_ଗଣନା;

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
    ଯଦି (fname_io == ଖାଲି)
    {
	ଯଦି (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = ଖାଲି;
	ଅନ୍ୟଥା ଯଦି (fname != ଖାଲି && !ends_excmd(*fname))
	    autocmd_fname = fname;
	ଅନ୍ୟଥା ଯଦି (buf != ଖାଲି)
	    autocmd_fname = buf->b_ffname;
	ଅନ୍ୟଥା
	    autocmd_fname = ଖାଲି;
    }
    ଅନ୍ୟଥା
	autocmd_fname = fname_io;
    ଯଦି (autocmd_fname != ଖାଲି)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    ଯଦି (buf == ଖାଲି)
	autocmd_bufnr = 0;
    ଅନ୍ୟଥା
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    ଯଦି (fname == ଖାଲି || *fname == NUL)
    {
	ଯଦି (buf == ଖାଲି)
	    fname = ଖାଲି;
	ଅନ୍ୟଥା
	{
#ifdef FEAT_SYN_HL
	    ଯଦି (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    ଅନ୍ୟଥା
#endif
		ଯଦି (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		ଅନ୍ୟଥା
		{
		    ଯଦି (buf->b_sfname != ଖାଲି)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	ଯଦି (fname == ଖାଲି)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    ଅନ୍ୟଥା
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	ଯଦି (event == EVENT_FILETYPE
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
	ଅନ୍ୟଥା
	    fname = FullName_save(fname, FALSE);
    }
    ଯଦି (fname == ଖାଲି)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	ଯାଅ BYPASS_AU;
    }

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    ଯଦି (sfname != ଖାଲି)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    ଯଦି (sfname != ଖାଲି)
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
    estack_ଠେଲ(ETYPE_AUCMD, ଖାଲି, 0);
    ESTACK_CHECK_SETUP;

    save_current_sctx = current_sctx;

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    ଯଦି (!autocmd_busy)
    {
	save_ସନ୍ଧାନ_patterns();
	ଯଦି (!ins_compl_active())
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
    ଯଦି (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    ଯଦି (patcmd.curpat != ଖାଲି)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	ଯଦି (eap != ଖାଲି)
	{
	    save_cmdarg = set_cmdarg(eap, ଖାଲି);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	ଅନ୍ୟଥା
	    save_cmdarg = ଖାଲି;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	ପାଇଁ (ap = patcmd.curpat; ap->next != ଖାଲି; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	ଯଦି (nesting == 1)
	    check_lnums(TRUE);
	ଅନ୍ୟଥା
	    check_lnums_nested(TRUE);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_did_emsg = did_emsg;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(ଖାଲି, getnextac, (ଶୂନ୍ୟ *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	ଯଦି (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	ଯଦି (eap != ଖାଲି)
	{
	    (ଶୂନ୍ୟ)set_cmdarg(ଖାଲି, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	ଯଦି (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    ଯଦି (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_କାଢ଼();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_full = save_autocmd_fname_full;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_match = save_autocmd_match;
    current_sctx = save_current_sctx;
#ifdef FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif
    ଚାବି_Typed = save_ଚାବି_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    ଯଦି (!autocmd_busy)
    {
	restore_ସନ୍ଧାନ_patterns();
	ଯଦି (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	ଯେତେବେଳେ (au_pending_free_buf != ଖାଲି)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	ଯେତେବେଳେ (au_pending_free_win != ଖାଲି)
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
    ଯଦି (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	ଯଦି (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    ଯଦି (event == EVENT_BUFWIPEOUT && buf != ଖାଲି)
	aubuflocal_remove(buf);

    ଯଦି (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    ଫେରନ୍ତୁ retval;
}

#ifdef FEAT_EVAL
ସ୍ଥିର char_u	*old_termresponse = ଖାଲି;
ସ୍ଥିର char_u	*old_termu7resp = ଖାଲି;
ସ୍ଥିର char_u	*old_termblinkresp = ଖାଲି;
ସ୍ଥିର char_u	*old_termrbgresp = ଖାଲି;
ସ୍ଥିର char_u	*old_termrfgresp = ଖାଲି;
ସ୍ଥିର char_u	*old_termstyleresp = ଖାଲି;
#endif

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    ଶୂନ୍ୟ
block_autocmds(ଶୂନ୍ୟ)
{
#ifdef FEAT_EVAL
    // Remember the value of v:termresponse.
    ଯଦି (autocmd_blocked == 0)
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

    ଶୂନ୍ୟ
unblock_autocmds(ଶୂନ୍ୟ)
{
    --autocmd_blocked;

#ifdef FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    ଯଦି (autocmd_blocked == 0)
    {
	ଯଦି (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, ଖାଲି, ଖାଲି, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", ଖାଲି, FALSE, curbuf);
	}
	ଯଦି (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", ଖାଲି, FALSE, curbuf);
	}
	ଯଦି (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", ଖାଲି, FALSE, curbuf);
	}
	ଯଦି (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", ଖାଲି, FALSE, curbuf);
	}
	ଯଦି (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", ଖାଲି, FALSE, curbuf);
	}
	ଯଦି (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", ଖାଲି, FALSE, curbuf);
	}
    }
#endif
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_autocmd_blocked(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
auto_next_pat(
    AutoPatCmd_T *apc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = ଖାଲି;

    ପାଇଁ (ap = apc->curpat; ap != ଖାଲି && !got_int; ap = ap->next)
    {
	apc->curpat = ଖାଲି;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	ଯଦି (ap->pat != ଖାଲି && ap->cmds != ଖାଲି
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    ଯଦି (ap->buflocal_nr == 0
		    ? (match_file_pat(ଖାଲି, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		ଅକ୍ଷର	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->length + ap->patlen + 1);
		ଯଦି (namep != ଖାଲି)
		{
		    sprintf((ଅକ୍ଷର *)namep, fmt, (ଅକ୍ଷର *)event_name->string, (ଅକ୍ଷର *)ap->pat);
		    ଯଦି (p_verbose >= 8)
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
		ପାଇଁ (cp = ap->cmds; cp->next != ଖାଲି; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    ଧାଡ଼ି_breakcheck();
	    ଯଦି (apc->curpat != ଖାଲି)	    // found a match
		ଭାଙ୍ଗ;
	}
	ଯଦି (stop_at_last && ap->last)
	    ଭାଙ୍ଗ;
    }
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    ଫେରନ୍ତୁ &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c UNUSED,
	ଶୂନ୍ୟ *cookie,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    ଯଦି (acp->curpat == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // repeat until we find an autocommand to execute
    ପାଇଁ (;;)
    {
	// skip removed commands
	ଯେତେବେଳେ (acp->nextcmd != ଖାଲି && acp->nextcmd->cmd == ଖାଲି)
	    ଯଦି (acp->nextcmd->last)
		acp->nextcmd = ଖାଲି;
	    ଅନ୍ୟଥା
		acp->nextcmd = acp->nextcmd->next;

	ଯଦି (acp->nextcmd != ଖାଲି)
	    ଭାଙ୍ଗ;

	// at end of commands, find next pattern that matches
	ଯଦି (acp->curpat->last)
	    acp->curpat = ଖାଲି;
	ଅନ୍ୟଥା
	    acp->curpat = acp->curpat->next;
	ଯଦି (acp->curpat != ଖାଲି)
	    auto_next_pat(acp, TRUE);
	ଯଦି (acp->curpat == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    ac = acp->nextcmd;

    ଯଦି (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	ସନ୍ଦେଶ_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    ଯଦି (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    ଯଦି (ac->last)
	acp->nextcmd = ଖାଲି;
    ଅନ୍ୟଥା
	acp->nextcmd = ac->next;
    ଫେରନ୍ତୁ retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;

    ଯଦି (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event] == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    ଯଦି (sfname != ଖାଲି)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	ଯଦି (ap->pat != ଖାଲି && ap->cmds != ଖାଲି
	      && (ap->buflocal_nr == 0
		? match_file_pat(ଖାଲି, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != ଖାଲି && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    ଭାଙ୍ଗ;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    ଫେରନ୍ତୁ retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    ଫେରନ୍ତୁ has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    ଫେରନ୍ତୁ has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == augroups.ga_len)		// add "END" add the end
	ଫେରନ୍ତୁ (char_u *)"END";
    ଯଦି (idx < 0 || idx >= augroups.ga_len)	// end of list
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (AUGROUP_NAME(idx) == ଖାଲି || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	ଫେରନ୍ତୁ (char_u *)"";
    ଫେରନ୍ତୁ AUGROUP_NAME(idx);		// return a name
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    ଯଦି (group == AUGROUP_ERROR)
	ଫେରନ୍ତୁ ଖାଲି;
    // If there only is a group name that's what we expand.
    ଯଦି (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    ପାଇଁ (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	ଯଦି (*p == ',')
	    arg = p + 1;
    ଯଦି (*p == NUL)
    {
	ଯଦି (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // skip over pattern
    arg = skipwhite(p);
    ଯେତେବେଳେ (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    ଯଦି (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 2; i++)
	{
	    ଯଦି (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    ଯଦି (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    ଯଦି (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	ଫେରନ୍ତୁ arg;			    // expand (next) command
    }

    ଯଦି (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    ଅନ୍ୟଥା
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (idx < augroups.ga_len)		// First list group names, if wanted
    {
	ଯଦି (!include_groups || AUGROUP_NAME(idx) == ଖାଲି
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    ଫେରନ୍ତୁ (char_u *)"";	// skip deleted entries
	ଫେରନ୍ତୁ AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    ଯଦି (i < 0 || i >= ଅଙ୍କ_EVENTS)
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ event_tab[i].ମୂଲ୍ୟ.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା win)
{
    ଯଦି (idx < 0 || idx >= ଅଙ୍କ_EVENTS)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (!win)
	ଫେରନ୍ତୁ event_tab[idx].ମୂଲ୍ୟ.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_EVENTS; ++i)
    {
	j += event_tab[i].ଚାବି <= 0;
	ଯଦି (j == idx + 1)
	    ଫେରନ୍ତୁ event_tab[i].ମୂଲ୍ୟ.string;
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_tabclosedpre(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)EVENT_TABCLOSEDPRE] != ଖାଲି);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
autocmd_supported(char_u *name)
{
    char_u *p;

    ଫେରନ୍ତୁ (event_name2nr(name, &p) != ଅଙ୍କ_EVENTS);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = ଖାଲି;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    ଯଦି (arg_save == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    p = vim_strchr(arg_save, '#');
    ଯଦି (p != ଖାଲି)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    ଯଦି (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (p == ଖାଲି)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    ଯାଅ theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	ଯଦି (p != ଖାଲି)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    ଯଦି (event == ଅଙ୍କ_EVENTS)
	ଯାଅ theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event];
    ଯଦି (ap == ଖାଲି)
	ଯାଅ theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    ଯଦି (pattern != ଖାଲି && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    ପାଇଁ ( ; ap != ଖାଲି; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	ଯଦି (ap->pat != ଖାଲି && ap->cmds != ଖାଲି
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == ଖାଲି
		|| (buflocal_buf == ଖାଲି
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    ଭାଙ୍ଗ;
	}

theend:
    vim_free(arg_save);
    ଫେରନ୍ତୁ retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    ସ୍ଥିର ଶୂନ୍ୟ
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = ଖାଲି;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*group_name = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group;
    char_u	*pat = ଖାଲି;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = ଖାଲି;
    char_u	*end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		once;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nested;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		replace;		// replace the cmd for a group/event
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = VVAL_TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_ସଂଖ୍ୟା = VVAL_FALSE;

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    aucmd_list = argvars[0].vval.v_list;
    ଯଦି (aucmd_list == ଖାଲି)
	ଫେରନ୍ତୁ;

    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = ଖାଲି;
	event_list = ଖାଲି;
	pat = ଖାଲି;
	pat_list = ଖାଲି;

	ଯଦି (li->li_tv.v_type != VAR_DICT)
	    ଜାରି;

	event_dict = li->li_tv.vval.v_dict;
	ଯଦି (event_dict == ଖାଲି)
	    ଜାରି;

	di = dict_find(event_dict, (char_u *)"event", -1);
	ଯଦି (di != ଖାଲି)
	{
	    ଯଦି (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		ଯଦି (event_name == ଖାଲି)
		{
		    emsg(_(e_string_required));
		    ଜାରି;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		ଯଦି (event_list == ଖାଲି)
		{
		    emsg(_(e_list_required));
		    ଜାରି;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		emsg(_(e_string_or_list_expected));
		ଜାରି;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	ଯଦି (group_name == ଖାଲି || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	ଅନ୍ୟଥା
	{
	    group = au_find_group(group_name);
	    ଯଦି (group == AUGROUP_ERROR)
	    {
		ଯଦି (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    ଭାଙ୍ଗ;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		ଯଦି (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    ଭାଙ୍ଗ;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	ଯଦି (dict_has_ଚାବି(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_ସଂଖ୍ୟା_def(event_dict, "bufnr", -1);
	    ଯଦି (bnum == -1)
		ଜାରି;

	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "<buffer=%d>", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)bnum);
	    pat = IObuff;
	}
	ଅନ୍ୟଥା
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    ଯଦି (di != ଖାଲି)
	    {
		ଯଦି (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    ଯଦି (pat == ଖାଲି)
		    {
			emsg(_(e_string_required));
			ଜାରି;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    ଯଦି (pat_list == ଖାଲି)
		    {
			emsg(_(e_list_required));
			ଜାରି;
		    }
		}
		ଅନ୍ୟଥା
		{
		    emsg(_(e_string_or_list_expected));
		    ଜାରି;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	ଯଦି (cmd == ଖାଲି)
	{
	    ଯଦି (delete)
		cmd = vim_strsave((char_u *)"");
	    ଅନ୍ୟଥା
		ଜାରି;
	}

	ଯଦି (delete && (event_name == ଖାଲି
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    ପାଇଁ (event = (event_T)0; (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event < ଅଙ୍କ_EVENTS;
		    event = (event_T)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event + 1))
	    {
		ଯଦି (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    ଭାଙ୍ଗ;
		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    char_u *p = ଖାଲି;

	    eli = ଖାଲି;
	    end = ଖାଲି;
	    ଯେତେବେଳେ (TRUE)
	    {
		ଯଦି (event_list != ଖାଲି)
		{
		    ଯଦି (eli == ଖାଲି)
			eli = event_list->lv_first;
		    ଅନ୍ୟଥା
			eli = eli->li_next;
		    ଯଦି (eli == ଖାଲି)
			ଭାଙ୍ଗ;
		    ଯଦି (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == ଖାଲି)
		    {
			emsg(_(e_string_required));
			ଭାଙ୍ଗ;
		    }
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (p == ଖାଲି)
			p = event_name;
		    ଯଦି (p == ଖାଲି || *p == NUL)
			ଭାଙ୍ଗ;
		}

		event = event_name2nr(p, &end);
		ଯଦି (event == ଅଙ୍କ_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    ଭାଙ୍ଗ;
		}
		ଯଦି (pat != ଖାଲି)
		{
		    ଯଦି (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			ଭାଙ୍ଗ;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (pat_list != ଖାଲି)
		{
		    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(pat_list, pli)
		    {
			ଯଦି (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == ଖାଲି)
			{
			    emsg(_(e_string_required));
			    ଜାରି;
			}
			ଯଦି (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    ଭାଙ୍ଗ;
			}
		    }
		    ଯଦି (retval == VVAL_FALSE)
			ଭାଙ୍ଗ;
		}
		ଯଦି (event_name != ଖାଲି)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	ଯଦି (delete && group_name != ଖାଲି &&
		(event_name == ଖାଲି || event_name[0] == NUL)
		&& (pat == ଖାଲି || pat[0] == NUL)
		&& (cmd == ଖାଲି || cmd[0] == NUL))
	    au_del_group(group_name);
    }

    VIM_CLEAR(group_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_ସଂଖ୍ୟା = retval;
}

/*
 * autocmd_add() function
 */
    ଶୂନ୍ୟ
f_autocmd_add(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    ଶୂନ୍ୟ
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    ଶୂନ୍ୟ
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = ଅଙ୍କ_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = ଖାଲି;
    char_u	*name = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		group = AUGROUP_ALL;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	ଯଦି (dict_has_ଚାବି(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    ଯଦି (name == ଖାଲି)
		ଫେରନ୍ତୁ;

	    ଯଦି (*name == NUL)
		group = AUGROUP_DEFAULT;
	    ଅନ୍ୟଥା
	    {
		group = au_find_group(name);
		ଯଦି (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    ଫେରନ୍ତୁ;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	ଯଦି (dict_has_ଚାବି(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    ଯଦି (name == ଖାଲି)
		ଫେରନ୍ତୁ;

	    ଯଦି (name[0] == '*' && name[1] == NUL)
		event_arg = ଅଙ୍କ_EVENTS;
	    ଅନ୍ୟଥା
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.ଚାବି = 0;
		target.ମୂଲ୍ୟ.string = name;
		target.ମୂଲ୍ୟ.length = STRLEN(target.ମୂଲ୍ୟ.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    ଅଙ୍କ_EVENTS, ଆକାର(event_tab[0]), cmp_keyvalue_ମୂଲ୍ୟ_ni);
		ଯଦି (entry == ଖାଲି)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    ଫେରନ୍ତୁ;
		}
		event_arg = (event_T)abs(entry->ଚାବି);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	ଯଦି (dict_has_ଚାବି(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    ଯଦି (pat == ଖାଲି)
		ଫେରନ୍ତୁ;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    ପାଇଁ (event = (event_T)0; (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event < ଅଙ୍କ_EVENTS;
	    event = (event_T)((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event + 1))
    {
	string_T    *event_name;

	ଯଦି (event_arg != ଅଙ୍କ_EVENTS && event != event_arg)
	    ଜାରି;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    ଯଦି (ap->pat == ଖାଲି)		// pattern has been removed
		ଜାରି;

	    ଯଦି (group != AUGROUP_ALL && group != ap->group)
		ଜାରି;

	    ଯଦି (pat != ଖାଲି && STRCMP(pat, ap->pat) != 0)
		ଜାରି;

	    group_name.string = get_augroup_name(ଖାଲି, ap->group);
	    ଯଦି (group_name.string == ଖାଲି)
		STR_LITERAL_SET(group_name, "");
	    ଅନ୍ୟଥା
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    ପାଇଁ (ac = ap->cmds; ac != ଖାଲି; ac = ac->next)
	    {
		event_dict = dict_alloc();
		ଯଦି (event_dict == ଖାଲି
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    ଫେରନ୍ତୁ;
		}

		ଯଦି (dict_add_string_len(event_dict, "event",
			event_name->string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)group_name.length) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_add_ସଂଖ୍ୟା(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_add_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_add_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_add_bool(event_dict, "once", ac->once) == FAIL
			|| dict_add_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    ଫେରନ୍ତୁ;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
