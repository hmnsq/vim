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

#அந்தர்பாவ "vim.h"

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
வகைவரை கட்டமைப்பு AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    எழுத்து	    once;		// "One shot": removed after execution
    எழுத்து	    nested;		// If autocommands nest here.
    எழுத்து	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    கட்டமைப்பு AutoCmd  *next;		// next AutoCmd in list
} AutoCmd;

வகைவரை கட்டமைப்பு AutoPat
{
    கட்டமைப்பு AutoPat  *next;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    முழு		    group;		// group ID
    முழு		    patlen;		// strlen() of pat
    முழு		    buflocal_nr;	// !=0 for buffer-local AutoPat
    எழுத்து	    allow_dirs;		// Pattern may match whole path
    எழுத்து	    last;		// last pattern for apply_autocmds()
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
நிலையான keyvalue_T event_tab[இலக்கம்_EVENTS] = {
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

நிலையான AutoPat *first_autopat[இலக்கம்_EVENTS] = { வெற்று };
நிலையான AutoPat *last_autopat[இலக்கம்_EVENTS] = { வெற்று };

#define AUGROUP_DEFAULT    (-1)	    // default autocmd group
#define AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#define AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
கட்டமைப்பு AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    முழு		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    முழு		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

நிலையான AutoPatCmd_T *active_apc_list = வெற்று; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    க்கு ((ap) = first_autopat[(முழு)(event)]; (ap) != வெற்று; (ap) = (ap)->next)

/*
 * augroups stores a list of autocmd group names.
 */
நிலையான garray_T augroups = {0, 0, அளவு(char_u *), 10, வெற்று};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])
// use get_deleted_augroup() to get this
நிலையான char_u *deleted_augroup = வெற்று;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
நிலையான முழு current_augroup = AUGROUP_DEFAULT;

நிலையான முழு au_need_clean = FALSE;   // need to delete marked patterns

நிலையான event_T event_name2nr(char_u *start, char_u **end);
நிலையான string_T *event_nr2name(event_T event);
நிலையான முழு au_get_grouparg(char_u **argp);
நிலையான முழு do_autocmd_event(event_T event, char_u *pat, முழு once, முழு nested, char_u *cmd, முழு forceit, முழு group, முழு flags);
நிலையான முழு apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, முழு force, முழு group, buf_T *buf, exarg_T *eap);
நிலையான வெறுமை auto_next_pat(AutoPatCmd_T *apc, முழு stop_at_last);
நிலையான முழு au_find_group(char_u *name);

நிலையான event_T	last_event;
நிலையான முழு	last_group;
நிலையான முழு	autocmd_blocked = 0;	// block all autocmds

    நிலையான char_u *
get_deleted_augroup(வெறுமை)
{
    எனில் (deleted_augroup == வெற்று)
	deleted_augroup = (char_u *)_("--Deleted--");
    திரும்பு deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    நிலையான வெறுமை
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    எனில் (got_int)
	திரும்பு;
    எனில் (ap->pat == வெற்று)		// pattern has been removed
	திரும்பு;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    செய்தி_putchar('\n');
    எனில் (got_int)
	செல் theend;
    எனில் (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	எனில் (ap->group != AUGROUP_DEFAULT)
	{
	    எனில் (AUGROUP_NAME(ap->group) == வெற்று)
		செய்தி_puts_attr((எழுத்து *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    இல்லையெனில்
		செய்தி_puts_attr((எழுத்து *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    செய்தி_puts("  ");
	}
	event_name = event_nr2name(event);
	செய்தி_puts_attr((எழுத்து *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	செய்தி_putchar('\n');
	எனில் (got_int)
	    செல் theend;
    }

    எனில் (ap->pat == வெற்று)
	செல் theend;  // timer might have cleared the pattern or group

    செய்தி_col = 4;
    செய்தி_outtrans(ap->pat);

    க்கு (ac = ap->cmds; ac != வெற்று; ac = ac->next)
    {
	எனில் (ac->cmd == வெற்று)		// skip removed commands
	    தொடர்;

	எனில் (செய்தி_col >= 14)
	    செய்தி_putchar('\n');
	செய்தி_col = 14;
	எனில் (got_int)
	    செல் theend;
	செய்தி_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	எனில் (p_verbose > 0)
	    last_set_செய்தி(ac->script_ctx);
#endif
	எனில் (got_int)
	    செல் theend;
	எனில் (ac->next != வெற்று)
	{
	    செய்தி_putchar('\n');
	    எனில் (got_int)
		செல் theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    நிலையான வெறுமை
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    நிலையான வெறுமை
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    க்கு (ac = ap->cmds; ac != வெற்று; ac = ac->next)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
நிலையான வெறுமை au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    நிலையான வெறுமை
au_cleanup(வெறுமை)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    எனில் (autocmd_busy || !au_need_clean)
	திரும்பு;

    // loop over all events
    க்கு (event = (event_T)0; (முழு)event < இலக்கம்_EVENTS;
					    event = (event_T)((முழு)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(முழு)event]);
	க்கு (ap = *prev_ap; ap != வெற்று; ap = *prev_ap)
	{
	    முழு has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    க்கு (ac = *prev_ac; ac != வெற்று; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		எனில் (ap->pat == வெற்று || ac->cmd == வெற்று)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		இல்லையெனில்
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    எனில் (ap->pat != வெற்று && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    எனில் (ap->pat == வெற்று)
	    {
		எனில் (ap->next == வெற்று)
		{
		    எனில் (prev_ap == &(first_autopat[(முழு)event]))
			last_autopat[(முழு)event] = வெற்று;
		    இல்லையெனில்
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(முழு)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    இல்லையெனில்
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    வெறுமை
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    க்கு (apc = active_apc_list; apc; apc = apc->next)
	எனில் (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    க்கு (event = (event_T)0; (முழு)event < இலக்கம்_EVENTS;
					    event = (event_T)((முழு)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    எனில் (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		எனில் (p_verbose >= 6)
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
    நிலையான முழு
au_new_group(char_u *name)
{
    முழு		i;

    i = au_find_group(name);
    எனில் (i != AUGROUP_ERROR)
	திரும்பு i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    க்கு (i = 0; i < augroups.ga_len; ++i)
	எனில் (AUGROUP_NAME(i) == வெற்று)
	    நிறுத்து;
    எனில் (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	திரும்பு AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    எனில் (AUGROUP_NAME(i) == வெற்று)
	திரும்பு AUGROUP_ERROR;
    எனில் (i == augroups.ga_len)
	++augroups.ga_len;

    திரும்பு i;
}

    நிலையான வெறுமை
au_del_group(char_u *name)
{
    முழு		i;
    event_T	event;
    AutoPat	*ap;
    முழு		in_use = FALSE;


    i = au_find_group(name);
    எனில் (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	திரும்பு;
    }
    எனில் (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	திரும்பு;
    }

    க்கு (event = (event_T)0; (முழு)event < இலக்கம்_EVENTS;
	    event = (event_T)((முழு)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    எனில் (ap->group == i && ap->pat != வெற்று)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = இலக்கம்_EVENTS;
		நிறுத்து;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    எனில் (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    இல்லையெனில்
	AUGROUP_NAME(i) = வெற்று;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    நிலையான முழு
au_find_group(char_u *name)
{
    முழு	    i;

    க்கு (i = 0; i < augroups.ga_len; ++i)
	எனில் (AUGROUP_NAME(i) != வெற்று && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    திரும்பு i;
    திரும்பு AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    முழு
au_has_group(char_u *name)
{
    திரும்பு au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    வெறுமை
do_augroup(char_u *arg, முழு del_group)
{
    முழு	    i;

    எனில் (del_group)
    {
	எனில் (*arg == NUL)
	    emsg(_(e_argument_required));
	இல்லையெனில்
	    au_del_group(arg);
    }
    இல்லையெனில் எனில் (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    இல்லையெனில் எனில் (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	எனில் (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    இல்லையெனில்			    // ":aug": list the group names
    {
	செய்தி_start();
	க்கு (i = 0; i < augroups.ga_len; ++i)
	{
	    எனில் (AUGROUP_NAME(i) != வெற்று)
	    {
		செய்தி_puts((எழுத்து *)AUGROUP_NAME(i));
		செய்தி_puts("  ");
	    }
	}
	செய்தி_clr_eos();
	செய்தி_end();
    }
}

    வெறுமை
autocmd_init(வெறுமை)
{
    CLEAR_FIELD(aucmd_win);
}

#எனில் defined(EXITFREE)
    வெறுமை
free_all_autocmds(வெறுமை)
{
    char_u	*s;

    க்கு (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(வெற்று, (char_u *)"", TRUE);

    க்கு (முழு i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	எனில் (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    முழு
is_aucmd_win(win_T *win)
{
    க்கு (முழு i = 0; i < AUCMD_WIN_மொத்தம்; ++i)
	எனில் (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    நிலையான event_T
event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    keyvalue_T target;
    keyvalue_T *entry;
    நிலையான keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_INDEX];
    நிலையான keyvalue_T *bufread = &event_tab[BUFREAD_INDEX];

    // the event name ends with end of line, '|', a blank or a comma
    க்கு (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.சாவி = 0;
    target.மதிப்பு.string = start;
    target.மதிப்பு.length = (size_t)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    எனில் (cmp_keyvalue_மதிப்பு_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    இல்லையெனில் எனில் (cmp_keyvalue_மதிப்பு_ni(&target, bufread) == 0)
	entry = bufread;
    இல்லையெனில்
	entry = (keyvalue_T *)bsearch(&target, &event_tab, இலக்கம்_EVENTS,
				அளவு(event_tab[0]), cmp_keyvalue_மதிப்பு_ni);

    எனில் (*p == ',')
	++p;
    *end = p;

    திரும்பு (entry == வெற்று) ? இலக்கம்_EVENTS : (event_T)abs(entry->சாவி);
}

/*
 * Return the name for event "event".
 */
    நிலையான string_T *
event_nr2name(event_T event)
{
    முழு	    i;
    எண்ணிக்கை {CACHE_SIZE = 12};
    நிலையான முழு cache_tab[CACHE_SIZE];
    நிலையான முழு cache_last_index = -1;
    நிலையான string_T unknown = STR_LITERAL_INIT("Unknown");

    எனில் (cache_last_index < 0)
    {
	க்கு (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    க்கு (i = cache_last_index; cache_tab[i] >= 0; )
    {
	எனில் ((event_T)abs(event_tab[cache_tab[i]].சாவி) == event)
	    திரும்பு &event_tab[cache_tab[i]].மதிப்பு;

	எனில் (i == 0)
	    i = CACHE_SIZE - 1;
	இல்லையெனில்
	    --i;

	// are we back at the start?
	எனில் (i == cache_last_index)
	    நிறுத்து;
    }

    // look in the event table itself
    க்கு (i = 0; i < இலக்கம்_EVENTS; ++i)
    {
	எனில் ((event_T)abs(event_tab[i].சாவி) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    எனில் (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    இல்லையெனில்
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    திரும்பு &event_tab[i].மதிப்பு;
	}
    }

    திரும்பு &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    நிலையான char_u *
find_end_event(
    char_u  *arg,
    முழு	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    எனில் (*arg == '*')
    {
	எனில் (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    திரும்பு வெற்று;
	}
	pat = arg + 1;
    }
    இல்லையெனில்
    {
	க்கு (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    எனில் ((முழு)event_name2nr(pat, &p) >= இலக்கம்_EVENTS)
	    {
		எனில் (have_group)
		    semsg(_(e_no_such_event_str), pat);
		இல்லையெனில்
		    semsg(_(e_no_such_group_or_event_str), pat);
		திரும்பு வெற்று;
	    }
	}
    }
    திரும்பு pat;
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
    முழு		group;

    எனில் (*arg == '|')
	திரும்பு வெற்று;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? வெற்று : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    எனில் (pat == வெற்று)
	திரும்பு வெற்று;

    pat = skipwhite(pat);
    எனில் (*pat == NUL || *pat == '|')
	திரும்பு வெற்று;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    போது (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    க்கு (முழு i = 0; i < 2; ++i)
    {
	எனில் (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	எனில் (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	எனில் (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    திரும்பு *cmd == NUL ? வெற்று : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    முழு
event_ignored(event_T event, char_u *ei)
{
    முழு ignored = FALSE;
    போது (*ei != NUL)
    {
	முழு unignore = *ei == '-';
	ei += unignore;
	எனில் (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].சாவி <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	இல்லையெனில் எனில் (event_name2nr(ei, &ei) == event)
	{
	    எனில் (unignore)
		திரும்பு FALSE;
	    இல்லையெனில்
		ignored = TRUE;
	}
    }

    திரும்பு ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    முழு
check_ei(char_u *ei)
{
    முழு	win = ei != p_ei;

    போது (*ei)
    {
	எனில் (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	இல்லையெனில்
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    எனில் (event == இலக்கம்_EVENTS || (win && event_tab[event].சாவி > 0))
		திரும்பு FAIL;
	}
    }

    திரும்பு OK;
}

#எனில் defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(எழுத்து *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    size_t	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    எனில் (save_ei == வெற்று)
	திரும்பு வெற்று;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    எனில் (new_ei == வெற்று)
    {
	vim_free(save_ei);
	திரும்பு வெற்று;
    }

    எனில் (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    இல்லையெனில்
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    திரும்பு save_ei;
}

    வெறுமை
au_event_restore(char_u *old_ei)
{
    எனில் (old_ei != வெற்று)
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
    வெறுமை
do_autocmd(exarg_T *eap, char_u *arg_in, முழு forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = வெற்று;
    char_u	*cmd;
    முழு		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = வெற்று;
    முழு		nested = FALSE;
    முழு		once = FALSE;
    முழு		group;
    முழு		i;
    முழு		flags = 0;

    எனில் (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    இல்லையெனில்
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	எனில் (arg == வெற்று)	    // out of memory
	    திரும்பு;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    எனில் (pat == வெற்று)
	திரும்பு;

    pat = skipwhite(pat);
    எனில் (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    இல்லையெனில்
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	போது (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	எனில் (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	எனில் (vim_strchr(pat, '$') != வெற்று || vim_strchr(pat, '~') != வெற்று)
	{
#ifdef BACKSLASH_IN_FILENAME
	    முழு	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    எனில் (envpat != வெற்று)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	க்கு (i = 0; i < 2; i++)
	{
	    எனில் (*cmd == NUL)
		தொடர்;

	    // Check for "++once" flag.
	    எனில் (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		எனில் (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    செல் err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    எனில் ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		எனில் (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    செல் err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    எனில் (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		எனில் (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    செல் err_exit;
		}
		எனில் (nested)
		{
		    semsg(_(e_duplicate_argument_str), "nested");
		    செல் err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	எனில் (*cmd != NUL)
	{
	    எனில் (eap != வெற்று)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    எனில் (cmd == வெற்று)	    // some error
		செல் err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    எனில் (!forceit && *cmd == NUL)
	// Highlight title
	செய்தி_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    எனில் (*arg == '*' || *arg == NUL || *arg == '|')
    {
	எனில் (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	இல்லையெனில்
	    க்கு (event = (event_T)0; (முழு)event < இலக்கம்_EVENTS;
					     event = (event_T)((முழு)event + 1))
		எனில் (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    நிறுத்து;
    }
    இல்லையெனில்
    {
	போது (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    எனில் (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		நிறுத்து;
    }

err_exit:
    எனில் (cmd_need_free)
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
    நிலையான முழு
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    முழு		group = AUGROUP_ALL;

    க்கு (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    எனில் (p <= arg)
	திரும்பு AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    எனில் (group_name == வெற்று)		// out of memory
	திரும்பு AUGROUP_ERROR;
    group = au_find_group(group_name);
    எனில் (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    இல்லையெனில்
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    திரும்பு group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    நிலையான முழு
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    முழு		once,
    முழு		nested,
    char_u	*cmd,
    முழு		forceit,
    முழு		group,
    முழு		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    முழு		brace_level;
    char_u	*endpat;
    முழு		findgroup;
    முழு		allgroups;
    முழு		patlen;
    முழு		is_buflocal;
    முழு		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    எனில் (group == AUGROUP_ALL)
	findgroup = current_augroup;
    இல்லையெனில்
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    எனில் (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    எனில் (forceit)  // delete the AutoPat, if it's in the current group
	    {
		எனில் (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    இல்லையெனில் எனில் (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    க்கு ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	க்கு (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    எனில் (*endpat == '{')
		brace_level++;
	    இல்லையெனில் எனில் (*endpat == '}')
		brace_level--;
	}
	எனில் (pat == endpat)		// ignore single comma
	    தொடர்;
	patlen = (முழு)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	எனில் (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    எனில் (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    இல்லையெனில் எனில் (patlen > 9 && pat[7] == '=')
	    {
		எனில் (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		இல்லையெனில் எனில் (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = atoi((எழுத்து *)pat + 8);
	    }
	}

	எனில் (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    sprintf((எழுத்து *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (முழு)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	எனில் (!forceit && *cmd != NUL && last_autopat[(முழு)event] != வெற்று)
	    prev_ap = &last_autopat[(முழு)event];
	இல்லையெனில்
	    prev_ap = &first_autopat[(முழு)event];
	போது ((ap = *prev_ap) != வெற்று)
	{
	    எனில் (ap->pat != வெற்று)
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
		எனில் ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    எனில் (forceit)
		    {
			எனில் (*cmd != NUL && ap->next == வெற்று)
			{
			    au_remove_cmds(ap);
			    நிறுத்து;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    இல்லையெனில் எனில் (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    இல்லையெனில் எனில் (ap->next == வெற்று)
			நிறுத்து;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	எனில் (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    எனில் (ap == வெற்று)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		எனில் (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == வெற்று))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_எண்), buflocal_nr);
		    திரும்பு FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		எனில் (ap == வெற்று)
		    திரும்பு FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		எனில் (ap->pat == வெற்று)
		{
		    vim_free(ap);
		    திரும்பு FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		எனில் (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		எனில் ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
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

		எனில் (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = வெற்று;
		}
		இல்லையெனில்
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    எனில் (reg_pat != வெற்று)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    எனில் (reg_pat == வெற்று || ap->reg_prog == வெற்று)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			திரும்பு FAIL;
		    }
		}
		ap->cmds = வெற்று;
		*prev_ap = ap;
		last_autopat[(முழு)event] = ap;
		ap->next = வெற்று;
		எனில் (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		இல்லையெனில்
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    போது ((ac = *prev_ac) != வெற்று)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    எனில் (ac == வெற்று)
		திரும்பு FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    எனில் (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    எனில் (ac->cmd == வெற்று)
	    {
		vim_free(ac);
		திரும்பு FAIL;
	    }
	    ac->next = வெற்று;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    திரும்பு OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    முழு
do_doautocmd(
    char_u	*arg_start,
    முழு		do_செய்தி,	    // give message for no matching autocmds?
    முழு		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    முழு		nothing_done = TRUE;
    முழு		group;

    எனில் (did_something != வெற்று)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    எனில் (arg == வெற்று)	    // out of memory
	திரும்பு FAIL;

    எனில் (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	திரும்பு FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    எனில் (fname == வெற்று)
	திரும்பு FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    போது (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	எனில் (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, வெற்று, TRUE, group, curbuf, வெற்று))
	    nothing_done = FALSE;

    எனில் (nothing_done && do_செய்தி
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    எனில் (did_something != வெற்று)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    திரும்பு aborting() ? FAIL : OK;
#இல்லையெனில்
    திரும்பு OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    வெறுமை
ex_doautoall(exarg_T *eap)
{
    முழு		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    முழு		call_do_modelines = check_nomodeline(&arg);
    முழு		did_aucmd;

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
	எனில் (buf->b_ml.ml_mfp == வெற்று || buf == curbuf)
	    தொடர்;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	எனில் (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    நிறுத்து;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	எனில் (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	எனில் (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    நிறுத்து;
	}
    }

    // Execute autocommands for the current buffer last.
    எனில் (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	எனில் (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    முழு
check_nomodeline(char_u **argp)
{
    எனில் (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    வெறுமை
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    முழு		save_ea;
#ifdef FEAT_AUTOCHDIR
    முழு		save_acd;
#endif
    முழு		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    எனில் (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    இல்லையெனில்
	FOR_ALL_WINDOWS(win)
	    எனில் (win->w_buffer == buf)
		நிறுத்து;

    // Allocate a window when needed.
    win_T *auc_win = வெற்று;
    முழு auc_idx = AUCMD_WIN_மொத்தம்;
    எனில் (win == வெற்று)
    {
	க்கு (auc_idx = 0; auc_idx < AUCMD_WIN_மொத்தம்; ++auc_idx)
	    எனில் (!aucmd_win[auc_idx].auc_win_used)
	    {
		எனில் (aucmd_win[auc_idx].auc_win == வெற்று)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		எனில் (auc_win != வெற்று)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		நிறுத்து;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	எனில் (auc_win == வெற்று)
	    திரும்பு;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == வெற்று ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    எனில் (bt_prompt(curbuf))
	aco->save_prompt_செருகு = curbuf->b_prompt_செருகு;
#endif

    எனில் (win != வெற்று)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    இல்லையெனில்
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
	curtab->tp_localdir = வெற்று;
	aco->globaldir = globaldir;
	globaldir = வெற்று;

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

	(வெறுமை)win_split_ins(0, WSP_உச்சி | WSP_FORCE_ROOM, auc_win, 0, வெற்று);
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
    எனில் (!same_buffer)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_active = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    வெறுமை
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    முழு	    dummy;
    win_T   *save_curwin;

    எனில் (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	எனில் (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		எனில் (wp == awp)
		{
		    எனில் (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    செல் win_found;
		}
	    }
	}
win_found:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(வெறுமை)winframe_remove(curwin, &dummy, வெற்று, வெற்று);
	win_remove(curwin, வெற்று);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_used = FALSE;
	last_status(FALSE);	    // may need to remove last status line

	எனில் (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	எனில் (save_curwin != வெற்று)
	    curwin = save_curwin;
	இல்லையெனில்
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	எனில் (bt_prompt(curbuf))
	    curbuf->b_prompt_செருகு = aco->save_prompt_செருகு;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	எனில் (awp->w_localdir != வெற்று)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	எனில் (curwin->w_topline > curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    curwin->w_topline = curbuf->b_ml.ml_வரி_மொத்தம்;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#எனில் defined(FEAT_GUI)
	எனில் (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_இடது], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_வலது], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    இல்லையெனில்
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	எனில் (save_curwin != வெற்று)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    எனில் (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != வெற்று)
	    {
#எனில் defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		எனில் (curwin->w_s == &curbuf->b_s)
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
    எனில் (VIsual_active)
	check_pos(curbuf, &VIsual);
}

நிலையான முழு	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    முழு
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    முழு		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    திரும்பு apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, வெற்று);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    முழு
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    முழு		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    திரும்பு apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    முழு
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    முழு		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    முழு		*retval)    // pointer to caller's retval
{
    முழு		did_cmd;

#ifdef FEAT_EVAL
    எனில் (should_abort(*retval))
	திரும்பு FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, வெற்று);
    எனில் (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    திரும்பு did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    நிலையான முழு
has_cursorhold(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != வெற்று);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    முழு
trigger_cursorhold(வெறுமை)
{
    முழு		state;

    எனில் (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	எனில் (state == MODE_NORMAL_BUSY || (state & MODE_செருகு) != 0)
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    முழு
has_winresized(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_WINRESIZED] != வெற்று);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    முழு
has_winscrolled(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_WINSCROLLED] != வெற்று);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    முழு
has_cursormoved(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_CURSORMOVED] != வெற்று);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    முழு
has_cursormovedI(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_CURSORMOVEDI] != வெற்று);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    முழு
has_textchanged(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_TEXTCHANGED] != வெற்று);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    முழு
has_textchangedI(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_TEXTCHANGEDI] != வெற்று);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    முழு
has_textchangedP(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_TEXTCHANGEDP] != வெற்று);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    முழு
has_insertcharpre(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_INSERTCHARPRE] != வெற்று);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    முழு
has_keyinputpre(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_KEYINPUTPRE] != வெற்று);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    முழு
has_cmdundefined(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_CMDUNDEFINED] != வெற்று);
}

#எனில் defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    முழு
has_textputpost(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_TEXTPUTPOST] != வெற்று);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    முழு
has_textputpre(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_TEXTPUTPRE] != வெற்று);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    முழு
has_textyankpost(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_TEXTYANKPOST] != வெற்று);
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    முழு
has_completechanged(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_COMPLETECHANGED] != வெற்று);
}
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    முழு
has_modechanged(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_MODECHANGED] != வெற்று);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    நிலையான முழு
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    முழு		force,	     // when TRUE, ignore autocmd_busy
    முழு		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = வெற்று;	// short file name
    char_u	*tail;
    முழு		save_changed;
    buf_T	*old_curbuf;
    முழு		retval = FALSE;
    char_u	*save_autocmd_fname;
    முழு		save_autocmd_fname_full;
    முழு		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    முழு		save_autocmd_busy;
    முழு		save_autocmd_nested;
    நிலையான முழு	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    long	save_cmdbang;
#endif
    நிலையான முழு	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    முழு		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    முழு		save_சாவி_Typed = சாவி_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    எனில் (event == இலக்கம்_EVENTS || first_autopat[(முழு)event] == வெற்று
	    || autocmd_blocked > 0)
	செல் BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    எனில் (autocmd_busy && !(force || autocmd_nested))
	செல் BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    எனில் (aborting())
	செல் BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    எனில் (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	செல் BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    எனில் (event_ignored(event, p_ei))
	செல் BYPASS_AU;

    முழு win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    எனில் (buf == curbuf && event_tab[event].சாவி <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    இல்லையெனில் எனில் (buf != வெற்று && event_tab[event].சாவி <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    எனில் (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		நிறுத்து;
	    }
    }
    எனில் (win_ignore)
	செல் BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    எனில் (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	செல் BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    எனில் (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	செல் BYPASS_AU;

    எனில் (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_changed_மொத்தம்;

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
    எனில் (fname_io == வெற்று)
    {
	எனில் (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = வெற்று;
	இல்லையெனில் எனில் (fname != வெற்று && !ends_excmd(*fname))
	    autocmd_fname = fname;
	இல்லையெனில் எனில் (buf != வெற்று)
	    autocmd_fname = buf->b_ffname;
	இல்லையெனில்
	    autocmd_fname = வெற்று;
    }
    இல்லையெனில்
	autocmd_fname = fname_io;
    எனில் (autocmd_fname != வெற்று)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    எனில் (buf == வெற்று)
	autocmd_bufnr = 0;
    இல்லையெனில்
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    எனில் (fname == வெற்று || *fname == NUL)
    {
	எனில் (buf == வெற்று)
	    fname = வெற்று;
	இல்லையெனில்
	{
#ifdef FEAT_SYN_HL
	    எனில் (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    இல்லையெனில்
#endif
		எனில் (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		இல்லையெனில்
		{
		    எனில் (buf->b_sfname != வெற்று)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	எனில் (fname == வெற்று)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    இல்லையெனில்
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	எனில் (event == EVENT_FILETYPE
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
	இல்லையெனில்
	    fname = FullName_save(fname, FALSE);
    }
    எனில் (fname == வெற்று)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	செல் BYPASS_AU;
    }

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    எனில் (sfname != வெற்று)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    எனில் (sfname != வெற்று)
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
    estack_தள்ளு(ETYPE_AUCMD, வெற்று, 0);
    ESTACK_CHECK_SETUP;

    save_current_sctx = current_sctx;

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    எனில் (!autocmd_busy)
    {
	save_தேடல்_patterns();
	எனில் (!ins_compl_active())
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
    எனில் (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(முழு)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    எனில் (patcmd.curpat != வெற்று)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (long)get_vim_var_nr(VV_CMDBANG);
	எனில் (eap != வெற்று)
	{
	    save_cmdarg = set_cmdarg(eap, வெற்று);
	    set_vim_var_nr(VV_CMDBANG, (long)eap->forceit);
	}
	இல்லையெனில்
	    save_cmdarg = வெற்று;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	க்கு (ap = patcmd.curpat; ap->next != வெற்று; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	எனில் (nesting == 1)
	    check_lnums(TRUE);
	இல்லையெனில்
	    check_lnums_nested(TRUE);

	முழு save_did_emsg = did_emsg;
	முழு save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(வெற்று, getnextac, (வெறுமை *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	எனில் (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	எனில் (eap != வெற்று)
	{
	    (வெறுமை)set_cmdarg(வெற்று, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	எனில் (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    எனில் (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_எடு();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_full = save_autocmd_fname_full;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_match = save_autocmd_match;
    current_sctx = save_current_sctx;
#ifdef FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif
    சாவி_Typed = save_சாவி_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    எனில் (!autocmd_busy)
    {
	restore_தேடல்_patterns();
	எனில் (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	போது (au_pending_free_buf != வெற்று)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	போது (au_pending_free_win != வெற்று)
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
    எனில் (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	எனில் (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    எனில் (event == EVENT_BUFWIPEOUT && buf != வெற்று)
	aubuflocal_remove(buf);

    எனில் (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    திரும்பு retval;
}

#ifdef FEAT_EVAL
நிலையான char_u	*old_termresponse = வெற்று;
நிலையான char_u	*old_termu7resp = வெற்று;
நிலையான char_u	*old_termblinkresp = வெற்று;
நிலையான char_u	*old_termrbgresp = வெற்று;
நிலையான char_u	*old_termrfgresp = வெற்று;
நிலையான char_u	*old_termstyleresp = வெற்று;
#endif

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    வெறுமை
block_autocmds(வெறுமை)
{
#ifdef FEAT_EVAL
    // Remember the value of v:termresponse.
    எனில் (autocmd_blocked == 0)
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

    வெறுமை
unblock_autocmds(வெறுமை)
{
    --autocmd_blocked;

#ifdef FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    எனில் (autocmd_blocked == 0)
    {
	எனில் (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, வெற்று, வெற்று, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", வெற்று, FALSE, curbuf);
	}
	எனில் (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", வெற்று, FALSE, curbuf);
	}
	எனில் (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", வெற்று, FALSE, curbuf);
	}
	எனில் (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", வெற்று, FALSE, curbuf);
	}
	எனில் (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", வெற்று, FALSE, curbuf);
	}
	எனில் (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", வெற்று, FALSE, curbuf);
	}
    }
#endif
}

    முழு
is_autocmd_blocked(வெறுமை)
{
    திரும்பு autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    நிலையான வெறுமை
auto_next_pat(
    AutoPatCmd_T *apc,
    முழு		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = வெற்று;

    க்கு (ap = apc->curpat; ap != வெற்று && !got_int; ap = ap->next)
    {
	apc->curpat = வெற்று;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	எனில் (ap->pat != வெற்று && ap->cmds != வெற்று
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    எனில் (ap->buflocal_nr == 0
		    ? (match_file_pat(வெற்று, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		எழுத்து	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->length + ap->patlen + 1);
		எனில் (namep != வெற்று)
		{
		    sprintf((எழுத்து *)namep, fmt, (எழுத்து *)event_name->string, (எழுத்து *)ap->pat);
		    எனில் (p_verbose >= 8)
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
		க்கு (cp = ap->cmds; cp->next != வெற்று; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    வரி_breakcheck();
	    எனில் (apc->curpat != வெற்று)	    // found a match
		நிறுத்து;
	}
	எனில் (stop_at_last && ap->last)
	    நிறுத்து;
    }
}

#எனில் defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    திரும்பு &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	முழு c UNUSED,
	வெறுமை *cookie,
	முழு indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    எனில் (acp->curpat == வெற்று)
	திரும்பு வெற்று;

    // repeat until we find an autocommand to execute
    க்கு (;;)
    {
	// skip removed commands
	போது (acp->nextcmd != வெற்று && acp->nextcmd->cmd == வெற்று)
	    எனில் (acp->nextcmd->last)
		acp->nextcmd = வெற்று;
	    இல்லையெனில்
		acp->nextcmd = acp->nextcmd->next;

	எனில் (acp->nextcmd != வெற்று)
	    நிறுத்து;

	// at end of commands, find next pattern that matches
	எனில் (acp->curpat->last)
	    acp->curpat = வெற்று;
	இல்லையெனில்
	    acp->curpat = acp->curpat->next;
	எனில் (acp->curpat != வெற்று)
	    auto_next_pat(acp, TRUE);
	எனில் (acp->curpat == வெற்று)
	    திரும்பு வெற்று;
    }

    ac = acp->nextcmd;

    எனில் (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	செய்தி_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    எனில் (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    எனில் (ac->last)
	acp->nextcmd = வெற்று;
    இல்லையெனில்
	acp->nextcmd = ac->next;
    திரும்பு retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    நிலையான முழு
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    முழு		retval = FALSE;

    எனில் (first_autopat[(முழு)event] == வெற்று)
	திரும்பு FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    எனில் (fname == வெற்று)
	திரும்பு FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    எனில் (sfname != வெற்று)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	எனில் (ap->pat != வெற்று && ap->cmds != வெற்று
	      && (ap->buflocal_nr == 0
		? match_file_pat(வெற்று, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != வெற்று && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    நிறுத்து;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    திரும்பு retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    முழு
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    திரும்பு has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    முழு
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    திரும்பு has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, முழு idx)
{
    எனில் (idx == augroups.ga_len)		// add "END" add the end
	திரும்பு (char_u *)"END";
    எனில் (idx < 0 || idx >= augroups.ga_len)	// end of list
	திரும்பு வெற்று;
    எனில் (AUGROUP_NAME(idx) == வெற்று || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	திரும்பு (char_u *)"";
    திரும்பு AUGROUP_NAME(idx);		// return a name
}

நிலையான முழு include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    முழு		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    முழு		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    எனில் (group == AUGROUP_ERROR)
	திரும்பு வெற்று;
    // If there only is a group name that's what we expand.
    எனில் (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    க்கு (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	எனில் (*p == ',')
	    arg = p + 1;
    எனில் (*p == NUL)
    {
	எனில் (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	திரும்பு வெற்று;
    }

    // skip over pattern
    arg = skipwhite(p);
    போது (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    எனில் (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	க்கு (முழு i = 0; i < 2; i++)
	{
	    எனில் (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    எனில் (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    எனில் (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	திரும்பு arg;			    // expand (next) command
    }

    எனில் (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    இல்லையெனில்
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    திரும்பு வெற்று;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, முழு idx)
{
    முழு i;

    எனில் (idx < augroups.ga_len)		// First list group names, if wanted
    {
	எனில் (!include_groups || AUGROUP_NAME(idx) == வெற்று
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    திரும்பு (char_u *)"";	// skip deleted entries
	திரும்பு AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    எனில் (i < 0 || i >= இலக்கம்_EVENTS)
	திரும்பு வெற்று;

    திரும்பு event_tab[i].மதிப்பு.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, முழு idx, முழு win)
{
    எனில் (idx < 0 || idx >= இலக்கம்_EVENTS)
	திரும்பு வெற்று;

    எனில் (!win)
	திரும்பு event_tab[idx].மதிப்பு.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    முழு j = 0;
    க்கு (முழு i = 0; i < இலக்கம்_EVENTS; ++i)
    {
	j += event_tab[i].சாவி <= 0;
	எனில் (j == idx + 1)
	    திரும்பு event_tab[i].மதிப்பு.string;
    }

    திரும்பு வெற்று;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    முழு
has_tabclosedpre(வெறுமை)
{
    திரும்பு (first_autopat[(முழு)EVENT_TABCLOSEDPRE] != வெற்று);
}

#எனில் defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    முழு
autocmd_supported(char_u *name)
{
    char_u *p;

    திரும்பு (event_name2nr(name, &p) != இலக்கம்_EVENTS);
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
    முழு
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = வெற்று;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = வெற்று;
    முழு		group;
    முழு		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    எனில் (arg_save == வெற்று)
	திரும்பு FALSE;
    p = vim_strchr(arg_save, '#');
    எனில் (p != வெற்று)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    எனில் (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    இல்லையெனில்
    {
	எனில் (p == வெற்று)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    செல் theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	எனில் (p != வெற்று)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    எனில் (event == இலக்கம்_EVENTS)
	செல் theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(முழு)event];
    எனில் (ap == வெற்று)
	செல் theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    எனில் (pattern != வெற்று && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    க்கு ( ; ap != வெற்று; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	எனில் (ap->pat != வெற்று && ap->cmds != வெற்று
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == வெற்று
		|| (buflocal_buf == வெற்று
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    நிறுத்து;
	}

theend:
    vim_free(arg_save);
    திரும்பு retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    நிலையான வெறுமை
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, முழு delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = வெற்று;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*group_name = வெற்று;
    முழு		group;
    char_u	*pat = வெற்று;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = வெற்று;
    char_u	*end;
    முழு		once;
    முழு		nested;
    முழு		replace;		// replace the cmd for a group/event
    முழு		retval = VVAL_TRUE;
    முழு		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_எண் = VVAL_FALSE;

    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (check_for_list_arg(argvars, 0) == FAIL)
	திரும்பு;

    aucmd_list = argvars[0].vval.v_list;
    எனில் (aucmd_list == வெற்று)
	திரும்பு;

    FOR_ALL_LIST_பொருட்கள்(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = வெற்று;
	event_list = வெற்று;
	pat = வெற்று;
	pat_list = வெற்று;

	எனில் (li->li_tv.v_type != VAR_DICT)
	    தொடர்;

	event_dict = li->li_tv.vval.v_dict;
	எனில் (event_dict == வெற்று)
	    தொடர்;

	di = dict_find(event_dict, (char_u *)"event", -1);
	எனில் (di != வெற்று)
	{
	    எனில் (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		எனில் (event_name == வெற்று)
		{
		    emsg(_(e_string_required));
		    தொடர்;
		}
	    }
	    இல்லையெனில் எனில் (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		எனில் (event_list == வெற்று)
		{
		    emsg(_(e_list_required));
		    தொடர்;
		}
	    }
	    இல்லையெனில்
	    {
		emsg(_(e_string_or_list_expected));
		தொடர்;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	எனில் (group_name == வெற்று || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	இல்லையெனில்
	{
	    group = au_find_group(group_name);
	    எனில் (group == AUGROUP_ERROR)
	    {
		எனில் (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    நிறுத்து;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		எனில் (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    நிறுத்து;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	எனில் (dict_has_சாவி(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_எண்_def(event_dict, "bufnr", -1);
	    எனில் (bnum == -1)
		தொடர்;

	    vim_snprintf((எழுத்து *)IObuff, IOSIZE, "<buffer=%d>", (முழு)bnum);
	    pat = IObuff;
	}
	இல்லையெனில்
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    எனில் (di != வெற்று)
	    {
		எனில் (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    எனில் (pat == வெற்று)
		    {
			emsg(_(e_string_required));
			தொடர்;
		    }
		}
		இல்லையெனில் எனில் (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    எனில் (pat_list == வெற்று)
		    {
			emsg(_(e_list_required));
			தொடர்;
		    }
		}
		இல்லையெனில்
		{
		    emsg(_(e_string_or_list_expected));
		    தொடர்;
		}
	    }
	    இல்லையெனில் எனில் (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	எனில் (cmd == வெற்று)
	{
	    எனில் (delete)
		cmd = vim_strsave((char_u *)"");
	    இல்லையெனில்
		தொடர்;
	}

	எனில் (delete && (event_name == வெற்று
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    க்கு (event = (event_T)0; (முழு)event < இலக்கம்_EVENTS;
		    event = (event_T)((முழு)event + 1))
	    {
		எனில் (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    நிறுத்து;
		}
	    }
	}
	இல்லையெனில்
	{
	    char_u *p = வெற்று;

	    eli = வெற்று;
	    end = வெற்று;
	    போது (TRUE)
	    {
		எனில் (event_list != வெற்று)
		{
		    எனில் (eli == வெற்று)
			eli = event_list->lv_first;
		    இல்லையெனில்
			eli = eli->li_next;
		    எனில் (eli == வெற்று)
			நிறுத்து;
		    எனில் (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == வெற்று)
		    {
			emsg(_(e_string_required));
			நிறுத்து;
		    }
		}
		இல்லையெனில்
		{
		    எனில் (p == வெற்று)
			p = event_name;
		    எனில் (p == வெற்று || *p == NUL)
			நிறுத்து;
		}

		event = event_name2nr(p, &end);
		எனில் (event == இலக்கம்_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    நிறுத்து;
		}
		எனில் (pat != வெற்று)
		{
		    எனில் (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			நிறுத்து;
		    }
		}
		இல்லையெனில் எனில் (pat_list != வெற்று)
		{
		    FOR_ALL_LIST_பொருட்கள்(pat_list, pli)
		    {
			எனில் (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == வெற்று)
			{
			    emsg(_(e_string_required));
			    தொடர்;
			}
			எனில் (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    நிறுத்து;
			}
		    }
		    எனில் (retval == VVAL_FALSE)
			நிறுத்து;
		}
		எனில் (event_name != வெற்று)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	எனில் (delete && group_name != வெற்று &&
		(event_name == வெற்று || event_name[0] == NUL)
		&& (pat == வெற்று || pat[0] == NUL)
		&& (cmd == வெற்று || cmd[0] == NUL))
	    au_del_group(group_name);
    }

    VIM_CLEAR(group_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_எண் = retval;
}

/*
 * autocmd_add() function
 */
    வெறுமை
f_autocmd_add(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    வெறுமை
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    வெறுமை
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = இலக்கம்_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = வெற்று;
    char_u	*name = வெற்று;
    முழு		group = AUGROUP_ALL;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;
    எனில் (check_for_opt_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	எனில் (dict_has_சாவி(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    எனில் (name == வெற்று)
		திரும்பு;

	    எனில் (*name == NUL)
		group = AUGROUP_DEFAULT;
	    இல்லையெனில்
	    {
		group = au_find_group(name);
		எனில் (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    திரும்பு;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	எனில் (dict_has_சாவி(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    எனில் (name == வெற்று)
		திரும்பு;

	    எனில் (name[0] == '*' && name[1] == NUL)
		event_arg = இலக்கம்_EVENTS;
	    இல்லையெனில்
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.சாவி = 0;
		target.மதிப்பு.string = name;
		target.மதிப்பு.length = STRLEN(target.மதிப்பு.string);
		entry = (keyvalue_T *)bsearch(&target, &event_tab,
		    இலக்கம்_EVENTS, அளவு(event_tab[0]), cmp_keyvalue_மதிப்பு_ni);
		எனில் (entry == வெற்று)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    திரும்பு;
		}
		event_arg = (event_T)abs(entry->சாவி);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	எனில் (dict_has_சாவி(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    எனில் (pat == வெற்று)
		திரும்பு;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    க்கு (event = (event_T)0; (முழு)event < இலக்கம்_EVENTS;
	    event = (event_T)((முழு)event + 1))
    {
	string_T    *event_name;

	எனில் (event_arg != இலக்கம்_EVENTS && event != event_arg)
	    தொடர்;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    எனில் (ap->pat == வெற்று)		// pattern has been removed
		தொடர்;

	    எனில் (group != AUGROUP_ALL && group != ap->group)
		தொடர்;

	    எனில் (pat != வெற்று && STRCMP(pat, ap->pat) != 0)
		தொடர்;

	    group_name.string = get_augroup_name(வெற்று, ap->group);
	    எனில் (group_name.string == வெற்று)
		STR_LITERAL_SET(group_name, "");
	    இல்லையெனில்
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    க்கு (ac = ap->cmds; ac != வெற்று; ac = ac->next)
	    {
		event_dict = dict_alloc();
		எனில் (event_dict == வெற்று
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    திரும்பு;
		}

		எனில் (dict_add_string_len(event_dict, "event",
			event_name->string, (முழு)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (முழு)group_name.length) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_add_எண்(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_add_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_add_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_add_bool(event_dict, "once", ac->once) == FAIL
			|| dict_add_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    திரும்பு;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
