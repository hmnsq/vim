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

#अन्तर्भाव "vim.h"

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
प्रकारव्याख्या संरचना AutoCmd
{
    char_u	    *cmd;		// The command to be executed (NULL
					// when command has been removed).
    अक्षर	    once;		// "One shot": removed after execution
    अक्षर	    nested;		// If autocommands nest here.
    अक्षर	    last;		// last command in list
    sctx_T	    script_ctx;		// script context where it is defined
    संरचना AutoCmd  *next;		// next AutoCmd in list
} AutoCmd;

प्रकारव्याख्या संरचना AutoPat
{
    संरचना AutoPat  *next;		// Next AutoPat in AutoPat list; MUST
					// be the first entry.
    char_u	    *pat;		// pattern as typed (NULL when pattern
					// has been removed)
    regprog_T	    *reg_prog;		// compiled regprog for pattern
    AutoCmd	    *cmds;		// list of commands to do
    पूर्णांक		    group;		// group ID
    पूर्णांक		    patlen;		// strlen() of pat
    पूर्णांक		    buflocal_nr;	// !=0 for buffer-local AutoPat
    अक्षर	    allow_dirs;		// Pattern may match whole path
    अक्षर	    last;		// last pattern for apply_autocmds()
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
स्थिर keyvalue_T event_tab[अंक_EVENTS] = {
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

स्थिर AutoPat *first_autopat[अंक_EVENTS] = { रिक्त };
स्थिर AutoPat *last_autopat[अंक_EVENTS] = { रिक्त };

#define AUGROUP_DEFAULT    (-1)	    // default autocmd group
#define AUGROUP_ERROR	   (-2)	    // erroneous autocmd group
#define AUGROUP_ALL	   (-3)	    // all autocmd groups

/*
 * struct used to keep status while executing autocommands for an event.
 */
संरचना AutoPatCmd_S
{
    AutoPat	*curpat;	// next AutoPat to examine
    AutoCmd	*nextcmd;	// next AutoCmd to execute
    पूर्णांक		group;		// group being used
    char_u	*fname;		// fname to match with
    char_u	*sfname;	// sfname to match with
    char_u	*tail;		// tail of fname
    event_T	event;		// current event
    sctx_T	script_ctx;	// script context where it is defined
    पूर्णांक		arg_bufnr;	// Initially equal to <abuf>, set to zero when
				// buf is deleted.
    AutoPatCmd_T *next;		// chain of active apc-s for auto-invalidation
};

स्थिर AutoPatCmd_T *active_apc_list = रिक्त; // stack of active autocommands

// Macro to loop over all the patterns for an autocmd event
#define FOR_ALL_AUTOCMD_PATTERNS(event, ap) \
    साठी ((ap) = first_autopat[(पूर्णांक)(event)]; (ap) != रिक्त; (ap) = (ap)->next)

/*
 * augroups stores a list of autocmd group names.
 */
स्थिर garray_T augroups = {0, 0, आकार(char_u *), 10, रिक्त};
#define AUGROUP_NAME(i) (((char_u **)augroups.ga_data)[i])
// use get_deleted_augroup() to get this
स्थिर char_u *deleted_augroup = रिक्त;

/*
 * The ID of the current group.  Group 0 is the default one.
 */
स्थिर पूर्णांक current_augroup = AUGROUP_DEFAULT;

स्थिर पूर्णांक au_need_clean = FALSE;   // need to delete marked patterns

स्थिर event_T event_name2nr(char_u *start, char_u **end);
स्थिर string_T *event_nr2name(event_T event);
स्थिर पूर्णांक au_get_grouparg(char_u **argp);
स्थिर पूर्णांक do_autocmd_event(event_T event, char_u *pat, पूर्णांक once, पूर्णांक nested, char_u *cmd, पूर्णांक forceit, पूर्णांक group, पूर्णांक flags);
स्थिर पूर्णांक apply_autocmds_group(event_T event, char_u *fname, char_u *fname_io, पूर्णांक force, पूर्णांक group, buf_T *buf, exarg_T *eap);
स्थिर शून्य auto_next_pat(AutoPatCmd_T *apc, पूर्णांक stop_at_last);
स्थिर पूर्णांक au_find_group(char_u *name);

स्थिर event_T	last_event;
स्थिर पूर्णांक	last_group;
स्थिर पूर्णांक	autocmd_blocked = 0;	// block all autocmds

    स्थिर char_u *
get_deleted_augroup(शून्य)
{
    जर (deleted_augroup == रिक्त)
	deleted_augroup = (char_u *)_("--Deleted--");
    परत deleted_augroup;
}

/*
 * Show the autocommands for one AutoPat.
 */
    स्थिर शून्य
show_autocmd(AutoPat *ap, event_T event)
{
    AutoCmd *ac;

    // Check for "got_int" (here and at various places below), which is set
    // when "q" has been hit for the "--more--" prompt
    जर (got_int)
	परत;
    जर (ap->pat == रिक्त)		// pattern has been removed
	परत;

    // Make sure no info referenced by "ap" is cleared, e.g. when a timer
    // clears an augroup.  Jump to "theend" after this!
    // "ap->pat" may be cleared anyway.
    ++autocmd_busy;

    संदेश_putchar('\n');
    जर (got_int)
	जा theend;
    जर (event != last_event || ap->group != last_group)
    {
	string_T    *event_name;

	जर (ap->group != AUGROUP_DEFAULT)
	{
	    जर (AUGROUP_NAME(ap->group) == रिक्त)
		संदेश_puts_attr((अक्षर *)get_deleted_augroup(), HL_ATTR(HLF_E));
	    अन्यथा
		संदेश_puts_attr((अक्षर *)AUGROUP_NAME(ap->group), HL_ATTR(HLF_T));
	    संदेश_puts("  ");
	}
	event_name = event_nr2name(event);
	संदेश_puts_attr((अक्षर *)event_name->string, HL_ATTR(HLF_T));
	last_event = event;
	last_group = ap->group;
	संदेश_putchar('\n');
	जर (got_int)
	    जा theend;
    }

    जर (ap->pat == रिक्त)
	जा theend;  // timer might have cleared the pattern or group

    संदेश_col = 4;
    संदेश_outtrans(ap->pat);

    साठी (ac = ap->cmds; ac != रिक्त; ac = ac->next)
    {
	जर (ac->cmd == रिक्त)		// skip removed commands
	    सुरूठेवा;

	जर (संदेश_col >= 14)
	    संदेश_putchar('\n');
	संदेश_col = 14;
	जर (got_int)
	    जा theend;
	संदेश_outtrans(ac->cmd);
#ifdef FEAT_EVAL
	जर (p_verbose > 0)
	    last_set_संदेश(ac->script_ctx);
#endif
	जर (got_int)
	    जा theend;
	जर (ac->next != रिक्त)
	{
	    संदेश_putchar('\n');
	    जर (got_int)
		जा theend;
	}
    }

theend:
    --autocmd_busy;
}

/*
 * Mark an autocommand pattern for deletion.
 */
    स्थिर शून्य
au_remove_pat(AutoPat *ap)
{
    VIM_CLEAR(ap->pat);
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}

/*
 * Mark all commands for a pattern for deletion.
 */
    स्थिर शून्य
au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    साठी (ac = ap->cmds; ac != रिक्त; ac = ac->next)
	VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

// Delete one command from an autocmd pattern.
स्थिर शून्य au_del_cmd(AutoCmd *ac)
{
    VIM_CLEAR(ac->cmd);
    au_need_clean = TRUE;
}

/*
 * Cleanup autocommands and patterns that have been deleted.
 * This is only done when not executing autocommands.
 */
    स्थिर शून्य
au_cleanup(शून्य)
{
    AutoPat	*ap, **prev_ap;
    AutoCmd	*ac, **prev_ac;
    event_T	event;

    जर (autocmd_busy || !au_need_clean)
	परत;

    // loop over all events
    साठी (event = (event_T)0; (पूर्णांक)event < अंक_EVENTS;
					    event = (event_T)((पूर्णांक)event + 1))
    {
	// loop over all autocommand patterns
	prev_ap = &(first_autopat[(पूर्णांक)event]);
	साठी (ap = *prev_ap; ap != रिक्त; ap = *prev_ap)
	{
	    पूर्णांक has_cmd = FALSE;

	    // loop over all commands for this pattern
	    prev_ac = &(ap->cmds);
	    साठी (ac = *prev_ac; ac != रिक्त; ac = *prev_ac)
	    {
		// remove the command if the pattern is to be deleted or when
		// the command has been marked for deletion
		जर (ap->pat == रिक्त || ac->cmd == रिक्त)
		{
		    *prev_ac = ac->next;
		    vim_free(ac->cmd);
		    vim_free(ac);
		}
		अन्यथा
		{
		    has_cmd = TRUE;
		    prev_ac = &(ac->next);
		}
	    }

	    जर (ap->pat != रिक्त && !has_cmd)
		// Pattern was not marked for deletion, but all of its
		// commands were.  So mark the pattern for deletion.
		au_remove_pat(ap);

	    // remove the pattern if it has been marked for deletion
	    जर (ap->pat == रिक्त)
	    {
		जर (ap->next == रिक्त)
		{
		    जर (prev_ap == &(first_autopat[(पूर्णांक)event]))
			last_autopat[(पूर्णांक)event] = रिक्त;
		    अन्यथा
			// this depends on the "next" field being the first in
			// the struct
			last_autopat[(पूर्णांक)event] = (AutoPat *)prev_ap;
		}
		*prev_ap = ap->next;
		vim_regfree(ap->reg_prog);
		vim_free(ap);
	    }
	    अन्यथा
		prev_ap = &(ap->next);
	}
    }

    au_need_clean = FALSE;
}

/*
 * Called when buffer is freed, to remove/invalidate related buffer-local
 * autocmds.
 */
    शून्य
aubuflocal_remove(buf_T *buf)
{
    AutoPat	    *ap;
    event_T	    event;
    AutoPatCmd_T    *apc;

    // invalidate currently executing autocommands
    साठी (apc = active_apc_list; apc; apc = apc->next)
	जर (buf->b_fnum == apc->arg_bufnr)
	    apc->arg_bufnr = 0;

    // invalidate buflocals looping through events
    साठी (event = (event_T)0; (पूर्णांक)event < अंक_EVENTS;
					    event = (event_T)((पूर्णांक)event + 1))
	// loop over all autocommand patterns
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    जर (ap->buflocal_nr == buf->b_fnum)
	    {
		au_remove_pat(ap);
		जर (p_verbose >= 6)
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
    स्थिर पूर्णांक
au_new_group(char_u *name)
{
    पूर्णांक		i;

    i = au_find_group(name);
    जर (i != AUGROUP_ERROR)
	परत i;

    // the group doesn't exist yet, add it.  First try using a free entry.
    साठी (i = 0; i < augroups.ga_len; ++i)
	जर (AUGROUP_NAME(i) == रिक्त)
	    थांबा;
    जर (i == augroups.ga_len && ga_grow(&augroups, 1) == FAIL)
	परत AUGROUP_ERROR;

    AUGROUP_NAME(i) = vim_strsave(name);
    जर (AUGROUP_NAME(i) == रिक्त)
	परत AUGROUP_ERROR;
    जर (i == augroups.ga_len)
	++augroups.ga_len;

    परत i;
}

    स्थिर शून्य
au_del_group(char_u *name)
{
    पूर्णांक		i;
    event_T	event;
    AutoPat	*ap;
    पूर्णांक		in_use = FALSE;


    i = au_find_group(name);
    जर (i == AUGROUP_ERROR)	// the group doesn't exist
    {
	semsg(_(e_no_such_group_str), name);
	परत;
    }
    जर (i == current_augroup)
    {
	emsg(_(e_cannot_delete_current_group));
	परत;
    }

    साठी (event = (event_T)0; (पूर्णांक)event < अंक_EVENTS;
	    event = (event_T)((पूर्णांक)event + 1))
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	    जर (ap->group == i && ap->pat != रिक्त)
	    {
		give_warning((char_u *)_("W19: Deleting augroup that is still in use"), TRUE);
		in_use = TRUE;
		event = अंक_EVENTS;
		थांबा;
	    }
    }
    vim_free(AUGROUP_NAME(i));
    जर (in_use)
	AUGROUP_NAME(i) = get_deleted_augroup();
    अन्यथा
	AUGROUP_NAME(i) = रिक्त;
}

/*
 * Find the ID of an autocmd group name.
 * Return its ID.  Returns AUGROUP_ERROR (< 0) for error.
 */
    स्थिर पूर्णांक
au_find_group(char_u *name)
{
    पूर्णांक	    i;

    साठी (i = 0; i < augroups.ga_len; ++i)
	जर (AUGROUP_NAME(i) != रिक्त && AUGROUP_NAME(i) != get_deleted_augroup()
		&& STRCMP(AUGROUP_NAME(i), name) == 0)
	    परत i;
    परत AUGROUP_ERROR;
}

/*
 * Return TRUE if augroup "name" exists.
 */
    पूर्णांक
au_has_group(char_u *name)
{
    परत au_find_group(name) != AUGROUP_ERROR;
}

/*
 * ":augroup {name}".
 */
    शून्य
do_augroup(char_u *arg, पूर्णांक del_group)
{
    पूर्णांक	    i;

    जर (del_group)
    {
	जर (*arg == NUL)
	    emsg(_(e_argument_required));
	अन्यथा
	    au_del_group(arg);
    }
    अन्यथा जर (STRICMP(arg, "end") == 0)   // ":aug end": back to group 0
	current_augroup = AUGROUP_DEFAULT;
    अन्यथा जर (*arg)		    // ":aug xxx": switch to group xxx
    {
	i = au_new_group(arg);
	जर (i != AUGROUP_ERROR)
	    current_augroup = i;
    }
    अन्यथा			    // ":aug": list the group names
    {
	संदेश_start();
	साठी (i = 0; i < augroups.ga_len; ++i)
	{
	    जर (AUGROUP_NAME(i) != रिक्त)
	    {
		संदेश_puts((अक्षर *)AUGROUP_NAME(i));
		संदेश_puts("  ");
	    }
	}
	संदेश_clr_eos();
	संदेश_end();
    }
}

    शून्य
autocmd_init(शून्य)
{
    CLEAR_FIELD(aucmd_win);
}

#जर defined(EXITFREE)
    शून्य
free_all_autocmds(शून्य)
{
    char_u	*s;

    साठी (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd(रिक्त, (char_u *)"", TRUE);

    साठी (पूर्णांक i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	जर (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);

    // aucmd_win[] is freed in win_free_all()
}
#endif

/*
 * Return TRUE if "win" is an active entry in aucmd_win[].
 */
    पूर्णांक
is_aucmd_win(win_T *win)
{
    साठी (पूर्णांक i = 0; i < AUCMD_WIN_मोजणी; ++i)
	जर (aucmd_win[i].auc_win_used && aucmd_win[i].auc_win == win)
	    परत TRUE;
    परत FALSE;
}

/*
 * Return the event number for event name "start".
 * Return NUM_EVENTS if the event name was not found.
 * Return a pointer to the next event name in "end".
 */
    स्थिर event_T
event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    keyvalue_T target;
    keyvalue_T *entry;
    स्थिर keyvalue_T *bufnewfile = &event_tab[BUFNEWFILE_INDEX];
    स्थिर keyvalue_T *bufread = &event_tab[BUFREAD_INDEX];

    // the event name ends with end of line, '|', a blank or a comma
    साठी (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;

    target.किल्ली = 0;
    target.मूल्य.string = start;
    target.मूल्य.length = (माप_प्रकार)(p - start);

    // special cases:
    // BufNewFile and BufRead are searched for ALOT (especially at startup)
    // so we check for them first.
    जर (cmp_keyvalue_मूल्य_ni(&target, bufnewfile) == 0)
	entry = bufnewfile;
    अन्यथा जर (cmp_keyvalue_मूल्य_ni(&target, bufread) == 0)
	entry = bufread;
    अन्यथा
	entry = (keyvalue_T *)द्वा_खोज(&target, &event_tab, अंक_EVENTS,
				आकार(event_tab[0]), cmp_keyvalue_मूल्य_ni);

    जर (*p == ',')
	++p;
    *end = p;

    परत (entry == रिक्त) ? अंक_EVENTS : (event_T)असल(entry->किल्ली);
}

/*
 * Return the name for event "event".
 */
    स्थिर string_T *
event_nr2name(event_T event)
{
    पूर्णांक	    i;
    गणना {CACHE_SIZE = 12};
    स्थिर पूर्णांक cache_tab[CACHE_SIZE];
    स्थिर पूर्णांक cache_last_index = -1;
    स्थिर string_T unknown = STR_LITERAL_INIT("Unknown");

    जर (cache_last_index < 0)
    {
	साठी (i = 0; i < CACHE_SIZE; ++i)
	    cache_tab[i] = -1;
	cache_last_index = CACHE_SIZE - 1;
    }

    // first look in the cache
    // the cache is circular. to search it we start at the most recent entry
    // and go backwards wrapping around when we get to index 0.
    साठी (i = cache_last_index; cache_tab[i] >= 0; )
    {
	जर ((event_T)असल(event_tab[cache_tab[i]].किल्ली) == event)
	    परत &event_tab[cache_tab[i]].मूल्य;

	जर (i == 0)
	    i = CACHE_SIZE - 1;
	अन्यथा
	    --i;

	// are we back at the start?
	जर (i == cache_last_index)
	    थांबा;
    }

    // look in the event table itself
    साठी (i = 0; i < अंक_EVENTS; ++i)
    {
	जर ((event_T)असल(event_tab[i].किल्ली) == event)
	{
	    // store the found entry in the next position in the cache,
	    // wrapping around when we get to the maximum index.
	    जर (cache_last_index == CACHE_SIZE - 1)
		cache_last_index = 0;
	    अन्यथा
		++cache_last_index;
	    cache_tab[cache_last_index] = i;
	    परत &event_tab[i].मूल्य;
	}
    }

    परत &unknown;
}

/*
 * Scan over the events.  "*" stands for all events.
 */
    स्थिर char_u *
find_end_event(
    char_u  *arg,
    पूर्णांक	    have_group)	    // TRUE when group name was found
{
    char_u  *pat;
    char_u  *p;

    जर (*arg == '*')
    {
	जर (arg[1] && !VIM_ISWHITE(arg[1]))
	{
	    semsg(_(e_illegal_character_after_star_str), arg);
	    परत रिक्त;
	}
	pat = arg + 1;
    }
    अन्यथा
    {
	साठी (pat = arg; *pat && *pat != '|' && !VIM_ISWHITE(*pat); pat = p)
	{
	    जर ((पूर्णांक)event_name2nr(pat, &p) >= अंक_EVENTS)
	    {
		जर (have_group)
		    semsg(_(e_no_such_event_str), pat);
		अन्यथा
		    semsg(_(e_no_such_group_or_event_str), pat);
		परत रिक्त;
	    }
	}
    }
    परत pat;
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
    पूर्णांक		group;

    जर (*arg == '|')
	परत रिक्त;

    // Errors are reported when the autocommand is actually defined.
    ++emsg_off;
    group = au_get_grouparg(&arg);
    pat = group == AUGROUP_ERROR
			  ? रिक्त : find_end_event(arg, group != AUGROUP_ALL);
    --emsg_off;
    जर (pat == रिक्त)
	परत रिक्त;

    pat = skipwhite(pat);
    जर (*pat == NUL || *pat == '|')
	परत रिक्त;

    // White space in the pattern can be escaped with a backslash.
    cmd = pat;
    पर्यंत (*cmd != NUL
	    && (!VIM_ISWHITE(*cmd) || (cmd > pat && *(cmd - 1) == '\\')))
	++cmd;
    cmd = skipwhite(cmd);

    // "++once" and "++nested" can come in any order.
    साठी (पूर्णांक i = 0; i < 2; ++i)
    {
	जर (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
	जर (STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8]))
	    cmd = skipwhite(cmd + 8);
	जर (!in_vim9script() && STRNCMP(cmd, "nested", 6) == 0
						       && VIM_ISWHITE(cmd[6]))
	    cmd = skipwhite(cmd + 6);
    }

    परत *cmd == NUL ? रिक्त : cmd;
}

/*
 * Return TRUE if "event" is included in 'eventignore(win)'.
 */
    पूर्णांक
event_ignored(event_T event, char_u *ei)
{
    पूर्णांक ignored = FALSE;
    पर्यंत (*ei != NUL)
    {
	पूर्णांक unignore = *ei == '-';
	ei += unignore;
	जर (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	{
	    ignored = ei == p_ei || (event_tab[event].किल्ली <= 0);
	    ei += 3 + (ei[3] == ',');
	}
	अन्यथा जर (event_name2nr(ei, &ei) == event)
	{
	    जर (unignore)
		परत FALSE;
	    अन्यथा
		ignored = TRUE;
	}
    }

    परत ignored;
}

/*
 * Return OK when the contents of 'eventignore' or 'eventignorewin' is valid,
 * FAIL otherwise.
 */
    पूर्णांक
check_ei(char_u *ei)
{
    पूर्णांक	win = ei != p_ei;

    पर्यंत (*ei)
    {
	जर (STRNICMP(ei, "all", 3) == 0 && (ei[3] == NUL || ei[3] == ','))
	    ei += 3 + (ei[3] == ',');
	अन्यथा
	{
	    ei += (*ei == '-');
	    event_T event = event_name2nr(ei, &ei);
	    जर (event == अंक_EVENTS || (win && event_tab[event].किल्ली > 0))
		परत FAIL;
	}
    }

    परत OK;
}

#जर defined(FEAT_SYN_HL)

/*
 * Add "what" to 'eventignore' to skip loading syntax highlighting for every
 * buffer loaded into the window.  "what" must start with a comma.
 * Returns the old value of 'eventignore' in allocated memory.
 */
    char_u *
au_event_disable(अक्षर *what)
{
    char_u	*new_ei;
    char_u	*save_ei;
    माप_प्रकार	p_ei_len;

    p_ei_len = STRLEN(p_ei);
    save_ei = vim_strnsave(p_ei, p_ei_len);
    जर (save_ei == रिक्त)
	परत रिक्त;

    new_ei = vim_strnsave(p_ei, p_ei_len + STRLEN(what));
    जर (new_ei == रिक्त)
    {
	vim_free(save_ei);
	परत रिक्त;
    }

    जर (*what == ',' && *p_ei == NUL)
	STRCPY(new_ei, what + 1);
    अन्यथा
	STRCPY(new_ei + p_ei_len, what);
    set_string_option_direct((char_u *)"ei", -1, new_ei,
	    OPT_FREE, SID_NONE);
    vim_free(new_ei);
    परत save_ei;
}

    शून्य
au_event_restore(char_u *old_ei)
{
    जर (old_ei != रिक्त)
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
    शून्य
do_autocmd(exarg_T *eap, char_u *arg_in, पूर्णांक forceit)
{
    char_u	*arg = arg_in;
    char_u	*pat;
    char_u	*envpat = रिक्त;
    char_u	*cmd;
    पूर्णांक		cmd_need_free = FALSE;
    event_T	event;
    char_u	*tofree = रिक्त;
    पूर्णांक		nested = FALSE;
    पूर्णांक		once = FALSE;
    पूर्णांक		group;
    पूर्णांक		i;
    पूर्णांक		flags = 0;

    जर (*arg == '|')
    {
	eap->nextcmd = arg + 1;
	arg = (char_u *)"";
	group = AUGROUP_ALL;	// no argument, use all groups
    }
    अन्यथा
    {
	/*
	 * Check for a legal group name.  If not, use AUGROUP_ALL.
	 */
	group = au_get_grouparg(&arg);
	जर (arg == रिक्त)	    // out of memory
	    परत;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    pat = find_end_event(arg, group != AUGROUP_ALL);
    जर (pat == रिक्त)
	परत;

    pat = skipwhite(pat);
    जर (*pat == '|')
    {
	eap->nextcmd = pat + 1;
	pat = (char_u *)"";
	cmd = (char_u *)"";
    }
    अन्यथा
    {
	/*
	 * Scan over the pattern.  Put a NUL at the end.
	 */
	cmd = pat;
	पर्यंत (*cmd && (!VIM_ISWHITE(*cmd) || cmd[-1] == '\\'))
	    cmd++;
	जर (*cmd)
	    *cmd++ = NUL;

	// Expand environment variables in the pattern.  Set 'shellslash', we
	// want forward slashes here.
	जर (vim_strchr(pat, '$') != रिक्त || vim_strchr(pat, '~') != रिक्त)
	{
#ifdef BACKSLASH_IN_FILENAME
	    पूर्णांक	p_ssl_save = p_ssl;

	    p_ssl = TRUE;
#endif
	    envpat = expand_env_save(pat);
#ifdef BACKSLASH_IN_FILENAME
	    p_ssl = p_ssl_save;
#endif
	    जर (envpat != रिक्त)
		pat = envpat;
	}

	cmd = skipwhite(cmd);
	साठी (i = 0; i < 2; i++)
	{
	    जर (*cmd == NUL)
		सुरूठेवा;

	    // Check for "++once" flag.
	    जर (STRNCMP(cmd, "++once", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		जर (once)
		{
		    semsg(_(e_duplicate_argument_str), "++once");
		    जा err_exit;
		}
		once = TRUE;
		cmd = skipwhite(cmd + 6);
	    }

	    // Check for "++nested" flag.
	    जर ((STRNCMP(cmd, "++nested", 8) == 0 && VIM_ISWHITE(cmd[8])))
	    {
		जर (nested)
		{
		    semsg(_(e_duplicate_argument_str), "++nested");
		    जा err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 8);
	    }

	    // Check for the old "nested" flag in legacy script.
	    जर (STRNCMP(cmd, "nested", 6) == 0 && VIM_ISWHITE(cmd[6]))
	    {
		जर (in_vim9script())
		{
		    // If there ever is a :nested command this error should
		    // be removed and "nested" accepted as the start of the
		    // command.
		    emsg(_(e_invalid_command_nested_did_you_mean_plusplus_nested));
		    जा err_exit;
		}
		जर (nested)
		{
		    semsg(_(e_duplicate_argument_str), "nested");
		    जा err_exit;
		}
		nested = TRUE;
		cmd = skipwhite(cmd + 6);
	    }
	}

	/*
	 * Find the start of the commands.
	 * Expand <sfile> in it.
	 */
	जर (*cmd != NUL)
	{
	    जर (eap != रिक्त)
		// Read a {} block if it follows.
		cmd = may_get_cmd_block(eap, cmd, &tofree, &flags);

	    cmd = expand_sfile(cmd);
	    जर (cmd == रिक्त)	    // some error
		जा err_exit;
	    cmd_need_free = TRUE;
	}
    }

    /*
     * Print header when showing autocommands.
     */
    जर (!forceit && *cmd == NUL)
	// Highlight title
	संदेश_puts_title(_("\n--- Autocommands ---"));

    /*
     * Loop over the events.
     */
    last_event = (event_T)-1;		// for listing the event name
    last_group = AUGROUP_ERROR;		// for listing the group name
    जर (*arg == '*' || *arg == NUL || *arg == '|')
    {
	जर (*cmd != NUL)
	    emsg(_(e_cannot_define_autocommands_for_all_events));
	अन्यथा
	    साठी (event = (event_T)0; (पूर्णांक)event < अंक_EVENTS;
					     event = (event_T)((पूर्णांक)event + 1))
		जर (do_autocmd_event(event, pat,
			     once, nested, cmd, forceit, group, flags) == FAIL)
		    थांबा;
    }
    अन्यथा
    {
	पर्यंत (*arg && *arg != '|' && !VIM_ISWHITE(*arg))
	    जर (do_autocmd_event(event_name2nr(arg, &arg), pat,
			  once, nested,	cmd, forceit, group, flags) == FAIL)
		थांबा;
    }

err_exit:
    जर (cmd_need_free)
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
    स्थिर पूर्णांक
au_get_grouparg(char_u **argp)
{
    char_u	*group_name;
    char_u	*p;
    char_u	*arg = *argp;
    पूर्णांक		group = AUGROUP_ALL;

    साठी (p = arg; *p && !VIM_ISWHITE(*p) && *p != '|'; ++p)
	;
    जर (p <= arg)
	परत AUGROUP_ALL;

    group_name = vim_strnsave(arg, p - arg);
    जर (group_name == रिक्त)		// out of memory
	परत AUGROUP_ERROR;
    group = au_find_group(group_name);
    जर (group == AUGROUP_ERROR)
	group = AUGROUP_ALL;	// no match, use all groups
    अन्यथा
	*argp = skipwhite(p);	// match, skip over group name
    vim_free(group_name);
    परत group;
}

/*
 * do_autocmd() for one event.
 * If *pat == NUL do for all patterns.
 * If *cmd == NUL show entries.
 * If forceit == TRUE delete entries.
 * If group is not AUGROUP_ALL, only use this group.
 */
    स्थिर पूर्णांक
do_autocmd_event(
    event_T	event,
    char_u	*pat,
    पूर्णांक		once,
    पूर्णांक		nested,
    char_u	*cmd,
    पूर्णांक		forceit,
    पूर्णांक		group,
    पूर्णांक		flags)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    पूर्णांक		brace_level;
    char_u	*endpat;
    पूर्णांक		findgroup;
    पूर्णांक		allgroups;
    पूर्णांक		patlen;
    पूर्णांक		is_buflocal;
    पूर्णांक		buflocal_nr;
    char_u	buflocal_pat[25];	// for "<buffer=X>"

    जर (group == AUGROUP_ALL)
	findgroup = current_augroup;
    अन्यथा
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    जर (*pat == NUL)
    {
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    जर (forceit)  // delete the AutoPat, if it's in the current group
	    {
		जर (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    अन्यथा जर (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    साठी ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	साठी (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    जर (*endpat == '{')
		brace_level++;
	    अन्यथा जर (*endpat == '}')
		brace_level--;
	}
	जर (pat == endpat)		// ignore single comma
	    सुरूठेवा;
	patlen = (पूर्णांक)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	जर (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    // "<buffer...>": Error will be printed only for addition.
	    // printing and removing will proceed silently.
	    is_buflocal = TRUE;
	    जर (patlen == 8)
		// "<buffer>"
		buflocal_nr = curbuf->b_fnum;
	    अन्यथा जर (patlen > 9 && pat[7] == '=')
	    {
		जर (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    // "<buffer=abuf>"
		    buflocal_nr = autocmd_bufnr;
		अन्यथा जर (skipdigits(pat + 8) == pat + patlen - 1)
		    // "<buffer=123>"
		    buflocal_nr = म_से_प((अक्षर *)pat + 8);
	    }
	}

	जर (is_buflocal)
	{
	    // normalize pat into standard "<buffer>#N" form
	    प्र_लिखो((अक्षर *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			// can modify pat and patlen
	    patlen = (पूर्णांक)STRLEN(buflocal_pat);	//   but not endpat
	}

	/*
	 * Find AutoPat entries with this pattern.  When adding a command it
	 * always goes at or after the last one, so start at the end.
	 */
	जर (!forceit && *cmd != NUL && last_autopat[(पूर्णांक)event] != रिक्त)
	    prev_ap = &last_autopat[(पूर्णांक)event];
	अन्यथा
	    prev_ap = &first_autopat[(पूर्णांक)event];
	पर्यंत ((ap = *prev_ap) != रिक्त)
	{
	    जर (ap->pat != रिक्त)
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
		जर ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    जर (forceit)
		    {
			जर (*cmd != NUL && ap->next == रिक्त)
			{
			    au_remove_cmds(ap);
			    थांबा;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    अन्यथा जर (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    अन्यथा जर (ap->next == रिक्त)
			थांबा;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	जर (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    जर (ap == रिक्त)
	    {
		// refuse to add buffer-local ap if buffer number is invalid
		जर (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == रिक्त))
		{
		    semsg(_(e_buffer_nr_invalid_buffer_संख्या), buflocal_nr);
		    परत FAIL;
		}

		ap = ALLOC_ONE(AutoPat);
		जर (ap == रिक्त)
		    परत FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		जर (ap->pat == रिक्त)
		{
		    vim_free(ap);
		    परत FAIL;
		}

#ifdef FEAT_EVAL
		// need to initialize last_mode for the first ModeChanged
		// autocmd
		जर (event == EVENT_MODECHANGED && !has_modechanged())
		    get_mode(last_mode);
#endif
		// Initialize the fields checked by the WinScrolled and
		// WinResized trigger to prevent them from firing right after
		// the first autocmd is defined.
		जर ((event == EVENT_WINSCROLLED || event == EVENT_WINRESIZED)
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

		जर (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = रिक्त;
		}
		अन्यथा
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    जर (reg_pat != रिक्त)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    जर (reg_pat == रिक्त || ap->reg_prog == रिक्त)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			परत FAIL;
		    }
		}
		ap->cmds = रिक्त;
		*prev_ap = ap;
		last_autopat[(पूर्णांक)event] = ap;
		ap->next = रिक्त;
		जर (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		अन्यथा
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    पर्यंत ((ac = *prev_ac) != रिक्त)
		prev_ac = &ac->next;
	    ac = ALLOC_ONE(AutoCmd);
	    जर (ac == रिक्त)
		परत FAIL;
	    ac->cmd = vim_strsave(cmd);
	    ac->script_ctx = current_sctx;
	    जर (flags & UC_VIM9)
		ac->script_ctx.sc_version = SCRIPT_VERSION_VIM9;
#ifdef FEAT_EVAL
	    ac->script_ctx.sc_lnum += SOURCING_LNUM;
#endif
	    जर (ac->cmd == रिक्त)
	    {
		vim_free(ac);
		परत FAIL;
	    }
	    ac->next = रिक्त;
	    *prev_ac = ac;
	    ac->once = once;
	    ac->nested = nested;
	}
    }

    au_cleanup();	// may really delete removed patterns/commands now
    परत OK;
}

/*
 * Implementation of ":doautocmd [group] event [fname]".
 * Return OK for success, FAIL for failure;
 */
    पूर्णांक
do_doautocmd(
    char_u	*arg_start,
    पूर्णांक		do_संदेश,	    // give message for no matching autocmds?
    पूर्णांक		*did_something)
{
    char_u	*arg = arg_start;
    char_u	*fname;
    पूर्णांक		nothing_done = TRUE;
    पूर्णांक		group;

    जर (did_something != रिक्त)
	*did_something = FALSE;

    /*
     * Check for a legal group name.  If not, use AUGROUP_ALL.
     */
    group = au_get_grouparg(&arg);
    जर (arg == रिक्त)	    // out of memory
	परत FAIL;

    जर (*arg == '*')
    {
	emsg(_(e_cant_execute_autocommands_for_all_events));
	परत FAIL;
    }

    /*
     * Scan over the events.
     * If we find an illegal name, return here, don't do anything.
     */
    fname = find_end_event(arg, group != AUGROUP_ALL);
    जर (fname == रिक्त)
	परत FAIL;

    fname = skipwhite(fname);

    /*
     * Loop over the events.
     */
    पर्यंत (*arg && !ends_excmd(*arg) && !VIM_ISWHITE(*arg))
	जर (apply_autocmds_group(event_name2nr(arg, &arg),
				      fname, रिक्त, TRUE, group, curbuf, रिक्त))
	    nothing_done = FALSE;

    जर (nothing_done && do_संदेश
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       )
	smsg(_("No matching autocommands: %s"), arg_start);
    जर (did_something != रिक्त)
	*did_something = !nothing_done;

#ifdef FEAT_EVAL
    परत aborting() ? FAIL : OK;
#अन्यथा
    परत OK;
#endif
}

/*
 * ":doautoall": execute autocommands for each loaded buffer.
 */
    शून्य
ex_doautoall(exarg_T *eap)
{
    पूर्णांक		retval = OK;
    aco_save_T	aco;
    buf_T	*buf;
    bufref_T	bufref;
    char_u	*arg = eap->arg;
    पूर्णांक		call_do_modelines = check_nomodeline(&arg);
    पूर्णांक		did_aucmd;

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
	जर (buf->b_ml.ml_mfp == रिक्त || buf == curbuf)
	    सुरूठेवा;

	// Find a window for this buffer and save some values.
	aucmd_prepbuf(&aco, buf);
	जर (curbuf != buf)
	{
	    // Failed to find a window for this buffer.  Better not execute
	    // autocommands then.
	    retval = FAIL;
	    थांबा;
	}

	set_bufref(&bufref, buf);

	// execute the autocommands for this buffer
	retval = do_doautocmd(arg, FALSE, &did_aucmd);

	जर (call_do_modelines && did_aucmd)
	    // Execute the modeline settings, but don't set window-local
	    // options if we are using the current window for another
	    // buffer.
	    do_modelines(is_aucmd_win(curwin) ? OPT_NOWIN : 0);

	// restore the current window
	aucmd_restbuf(&aco);

	// stop if there is some error or buffer was deleted
	जर (retval == FAIL || !bufref_valid(&bufref))
	{
	    retval = FAIL;
	    थांबा;
	}
    }

    // Execute autocommands for the current buffer last.
    जर (retval == OK)
    {
	do_doautocmd(arg, FALSE, &did_aucmd);
	जर (call_do_modelines && did_aucmd)
	    do_modelines(0);
    }
}

/*
 * Check *argp for <nomodeline>.  When it is present return FALSE, otherwise
 * return TRUE and advance *argp to after it.
 * Thus return TRUE when do_modelines() should be called.
 */
    पूर्णांक
check_nomodeline(char_u **argp)
{
    जर (STRNCMP(*argp, "<nomodeline>", 12) == 0)
    {
	*argp = skipwhite(*argp + 12);
	परत FALSE;
    }
    परत TRUE;
}

/*
 * Prepare for executing autocommands for (hidden) buffer "buf".
 * Search for a visible window containing the current buffer.  If there isn't
 * one then use an entry in "aucmd_win[]".
 * Set "curbuf" and "curwin" to match "buf".
 * When this fails "curbuf" is not equal "buf".
 */
    शून्य
aucmd_prepbuf(
    aco_save_T	*aco,		// structure to save values in
    buf_T	*buf)		// new curbuf
{
    win_T	*win;
    पूर्णांक		save_ea;
#ifdef FEAT_AUTOCHDIR
    पूर्णांक		save_acd;
#endif
    पूर्णांक		same_buffer = buf == curbuf;

    // Find a window that is for the new buffer
    जर (same_buffer)		// be quick when buf is curbuf
	win = curwin;
    अन्यथा
	FOR_ALL_WINDOWS(win)
	    जर (win->w_buffer == buf)
		थांबा;

    // Allocate a window when needed.
    win_T *auc_win = रिक्त;
    पूर्णांक auc_idx = AUCMD_WIN_मोजणी;
    जर (win == रिक्त)
    {
	साठी (auc_idx = 0; auc_idx < AUCMD_WIN_मोजणी; ++auc_idx)
	    जर (!aucmd_win[auc_idx].auc_win_used)
	    {
		जर (aucmd_win[auc_idx].auc_win == रिक्त)
		    aucmd_win[auc_idx].auc_win = win_alloc_popup_win();
		auc_win = aucmd_win[auc_idx].auc_win;
		जर (auc_win != रिक्त)
		    aucmd_win[auc_idx].auc_win_used = TRUE;
		थांबा;
	    }

	// If this fails (out of memory or using all AUCMD_WIN_COUNT
	// entries) then we can't reliable execute the autocmd, return with
	// "curbuf" unequal "buf".
	जर (auc_win == रिक्त)
	    परत;
    }

    aco->save_curwin_id = curwin->w_id;
    aco->save_prevwin_id = prevwin == रिक्त ? 0 : prevwin->w_id;
#ifdef FEAT_JOB_CHANNEL
    जर (bt_prompt(curbuf))
	aco->save_prompt_घाला = curbuf->b_prompt_घाला;
#endif

    जर (win != रिक्त)
    {
	// There is a window for "buf" in the current tab page, make it the
	// curwin.  This is preferred, it has the least side effects (esp. if
	// "buf" is curbuf).
	aco->use_aucmd_win_idx = -1;
	curwin = win;
    }
    अन्यथा
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
	curtab->tp_localdir = रिक्त;
	aco->globaldir = globaldir;
	globaldir = रिक्त;

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

	(शून्य)win_split_ins(0, WSP_शीर्ष | WSP_FORCE_ROOM, auc_win, 0, रिक्त);
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
    जर (!same_buffer)
	// disable the Visual area, position may be invalid in another buffer
	VIsual_active = FALSE;
}

/*
 * Cleanup after executing autocommands for a (hidden) buffer.
 * Restore the window as it was (if possible).
 */
    शून्य
aucmd_restbuf(
    aco_save_T	*aco)		// structure holding saved values
{
    पूर्णांक	    dummy;
    win_T   *save_curwin;

    जर (aco->use_aucmd_win_idx >= 0)
    {
	win_T *awp = aucmd_win[aco->use_aucmd_win_idx].auc_win;

	// Find "awp", it can't be closed, but it may be in another tab
	// page. Do not trigger autocommands here.
	block_autocmds();
	जर (curwin != awp)
	{
	    tabpage_T	*tp;
	    win_T	*wp;

	    FOR_ALL_TAB_WINDOWS(tp, wp)
	    {
		जर (wp == awp)
		{
		    जर (tp != curtab)
			goto_tabpage_tp(tp, TRUE, TRUE);
		    win_goto(awp);
		    जा win_found;
		}
	    }
	}
win_found:
	--curbuf->b_nwindows;
	// Remove the window and frame from the tree of frames.
	(शून्य)winframe_remove(curwin, &dummy, रिक्त, रिक्त);
	win_remove(curwin, रिक्त);

	// The window is marked as not used, but it is not freed, it can be
	// used again.
	aucmd_win[aco->use_aucmd_win_idx].auc_win_used = FALSE;
	last_status(FALSE);	    // may need to remove last status line

	जर (!valid_tabpage_win(curtab))
	    // no valid window in current tabpage
	    close_tabpage(curtab);

	restore_snapshot(SNAP_AUCMD_IDX, FALSE);
	win_comp_pos();   // recompute window positions
	unblock_autocmds();

	save_curwin = win_find_by_id(aco->save_curwin_id);
	जर (save_curwin != रिक्त)
	    curwin = save_curwin;
	अन्यथा
	    // Hmm, original window disappeared.  Just use the first one.
	    curwin = firstwin;
	curbuf = curwin->w_buffer;
#ifdef FEAT_JOB_CHANNEL
	// May need to restore insert mode for a prompt buffer.
	entering_window(curwin);
	जर (bt_prompt(curbuf))
	    curbuf->b_prompt_घाला = aco->save_prompt_घाला;
#endif
	prevwin = win_find_by_id(aco->save_prevwin_id);
#ifdef FEAT_EVAL
	vars_clear(&awp->w_vars->dv_hashtab);  // free all w: variables
	hash_init(&awp->w_vars->dv_hashtab);   // re-use the hashtab
#endif
	// If :lcd has been used in the autocommand window, correct current
	// directory before restoring tp_localdir and globaldir.
	जर (awp->w_localdir != रिक्त)
	    win_fix_current_dir();
	vim_free(curtab->tp_localdir);
	curtab->tp_localdir = aco->tp_localdir;
	vim_free(globaldir);
	globaldir = aco->globaldir;

	// the buffer contents may have changed
	VIsual_active = aco->save_VIsual_active;
	check_cursor();
	जर (curwin->w_topline > curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    curwin->w_topline = curbuf->b_ml.ml_ओळ_मोजणी;
#ifdef FEAT_DIFF
	    curwin->w_topfill = 0;
#endif
	}
#जर defined(FEAT_GUI)
	जर (gui.in_use)
	{
	    // Hide the scrollbars from the "awp" and update.
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_डावा], FALSE);
	    gui_mch_enable_scrollbar(&awp->w_scrollbars[SBAR_उजवा], FALSE);
	    gui_may_update_scrollbars();
	}
#endif
    }
    अन्यथा
    {
	// Restore curwin.  Use the window ID, a window may have been closed
	// and the memory re-used for another one.
	save_curwin = win_find_by_id(aco->save_curwin_id);
	जर (save_curwin != रिक्त)
	{
	    // Restore the buffer which was previously edited by curwin, if
	    // it was changed, we are still the same window and the buffer is
	    // valid.
	    जर (curwin->w_id == aco->new_curwin_id
		    && curbuf != aco->new_curbuf.br_buf
		    && bufref_valid(&aco->new_curbuf)
		    && aco->new_curbuf.br_buf->b_ml.ml_mfp != रिक्त)
	    {
#जर defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
		जर (curwin->w_s == &curbuf->b_s)
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
    जर (VIsual_active)
	check_pos(curbuf, &VIsual);
}

स्थिर पूर्णांक	autocmd_nested = FALSE;

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    पूर्णांक
apply_autocmds(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    पूर्णांक		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf)	    // buffer for <abuf>
{
    परत apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, रिक्त);
}

/*
 * Like apply_autocmds(), but with extra "eap" argument.  This takes care of
 * setting v:filearg.
 */
    पूर्णांक
apply_autocmds_exarg(
    event_T	event,
    char_u	*fname,
    char_u	*fname_io,
    पूर्णांक		force,
    buf_T	*buf,
    exarg_T	*eap)
{
    परत apply_autocmds_group(event, fname, fname_io, force,
						       AUGROUP_ALL, buf, eap);
}

/*
 * Like apply_autocmds(), but handles the caller's retval.  If the script
 * processing is being aborted or if retval is FAIL when inside a try
 * conditional, no autocommands are executed.  If otherwise the autocommands
 * cause the script to be aborted, retval is set to FAIL.
 */
    पूर्णांक
apply_autocmds_retval(
    event_T	event,
    char_u	*fname,	    // NULL or empty means use actual file name
    char_u	*fname_io,  // fname to use for <afile> on cmdline
    पूर्णांक		force,	    // when TRUE, ignore autocmd_busy
    buf_T	*buf,	    // buffer for <abuf>
    पूर्णांक		*retval)    // pointer to caller's retval
{
    पूर्णांक		did_cmd;

#ifdef FEAT_EVAL
    जर (should_abort(*retval))
	परत FALSE;
#endif

    did_cmd = apply_autocmds_group(event, fname, fname_io, force,
						      AUGROUP_ALL, buf, रिक्त);
    जर (did_cmd
#ifdef FEAT_EVAL
	    && aborting()
#endif
	    )
	*retval = FAIL;
    परत did_cmd;
}

/*
 * Return TRUE when there is a CursorHold autocommand defined.
 */
    स्थिर पूर्णांक
has_cursorhold(शून्य)
{
    परत (first_autopat[(पूर्णांक)(get_real_state() == MODE_NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != रिक्त);
}

/*
 * Return TRUE if the CursorHold event can be triggered.
 */
    पूर्णांक
trigger_cursorhold(शून्य)
{
    पूर्णांक		state;

    जर (!did_cursorhold
	    && has_cursorhold()
	    && reg_recording == 0
	    && typebuf.tb_len == 0
	    && !ins_compl_active())
    {
	state = get_real_state();
	जर (state == MODE_NORMAL_BUSY || (state & MODE_घाला) != 0)
	    परत TRUE;
    }
    परत FALSE;
}

/*
 * Return TRUE when there is a WinResized autocommand defined.
 */
    पूर्णांक
has_winresized(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_WINRESIZED] != रिक्त);
}

/*
 * Return TRUE when there is a WinScrolled autocommand defined.
 */
    पूर्णांक
has_winscrolled(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_WINSCROLLED] != रिक्त);
}

/*
 * Return TRUE when there is a CursorMoved autocommand defined.
 */
    पूर्णांक
has_cursormoved(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_CURSORMOVED] != रिक्त);
}

/*
 * Return TRUE when there is a CursorMovedI autocommand defined.
 */
    पूर्णांक
has_cursormovedI(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_CURSORMOVEDI] != रिक्त);
}

/*
 * Return TRUE when there is a TextChanged autocommand defined.
 */
    पूर्णांक
has_textchanged(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_TEXTCHANGED] != रिक्त);
}

/*
 * Return TRUE when there is a TextChangedI autocommand defined.
 */
    पूर्णांक
has_textchangedI(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_TEXTCHANGEDI] != रिक्त);
}

/*
 * Return TRUE when there is a TextChangedP autocommand defined.
 */
    पूर्णांक
has_textchangedP(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_TEXTCHANGEDP] != रिक्त);
}

/*
 * Return TRUE when there is an InsertCharPre autocommand defined.
 */
    पूर्णांक
has_insertcharpre(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_INSERTCHARPRE] != रिक्त);
}

/*
 * Return TRUE when there is an KeyInputPre autocommand defined.
 */
    पूर्णांक
has_keyinputpre(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_KEYINPUTPRE] != रिक्त);
}

/*
 * Return TRUE when there is an CmdUndefined autocommand defined.
 */
    पूर्णांक
has_cmdundefined(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_CMDUNDEFINED] != रिक्त);
}

#जर defined(FEAT_EVAL)

/*
 * Return TRUE when there is a TextPutPost autocommand defined.
 */
    पूर्णांक
has_textputpost(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_TEXTPUTPOST] != रिक्त);
}

/*
 * Return TRUE when there is a TextPutPre autocommand defined.
 */
    पूर्णांक
has_textputpre(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_TEXTPUTPRE] != रिक्त);
}

/*
 * Return TRUE when there is a TextYankPost autocommand defined.
 */
    पूर्णांक
has_textyankpost(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_TEXTYANKPOST] != रिक्त);
}
#endif

#जर defined(FEAT_EVAL)
/*
 * Return TRUE when there is a CompleteChanged autocommand defined.
 */
    पूर्णांक
has_completechanged(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_COMPLETECHANGED] != रिक्त);
}
#endif

#जर defined(FEAT_EVAL)
/*
 * Return TRUE when there is a ModeChanged autocommand defined.
 */
    पूर्णांक
has_modechanged(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_MODECHANGED] != रिक्त);
}
#endif

/*
 * Execute autocommands for "event" and file name "fname".
 * Return TRUE if some commands were executed.
 */
    स्थिर पूर्णांक
apply_autocmds_group(
    event_T	event,
    char_u	*fname,	     // NULL or empty means use actual file name
    char_u	*fname_io,   // fname to use for <afile> on cmdline, NULL means
			     // use fname
    पूर्णांक		force,	     // when TRUE, ignore autocmd_busy
    पूर्णांक		group,	     // group ID, or AUGROUP_ALL
    buf_T	*buf,	     // buffer for <abuf>
    exarg_T	*eap UNUSED) // command arguments
{
    char_u	*sfname = रिक्त;	// short file name
    char_u	*tail;
    पूर्णांक		save_changed;
    buf_T	*old_curbuf;
    पूर्णांक		retval = FALSE;
    char_u	*save_autocmd_fname;
    पूर्णांक		save_autocmd_fname_full;
    पूर्णांक		save_autocmd_bufnr;
    char_u	*save_autocmd_match;
    पूर्णांक		save_autocmd_busy;
    पूर्णांक		save_autocmd_nested;
    स्थिर पूर्णांक	nesting = 0;
    AutoPatCmd_T patcmd;
    AutoPat	*ap;
    sctx_T	save_current_sctx;
#ifdef FEAT_EVAL
    funccal_entry_T funccal_entry;
    char_u	*save_cmdarg;
    दीर्घ	save_cmdbang;
#endif
    स्थिर पूर्णांक	filechangeshell_busy = FALSE;
#ifdef FEAT_PROFILE
    proftime_T	wait_time;
#endif
    पूर्णांक		did_save_redobuff = FALSE;
    save_redo_T	save_redo;
    पूर्णांक		save_किल्ली_Typed = किल्ली_Typed;
    ESTACK_CHECK_DECLARATION;

    /*
     * Quickly return if there are no autocommands for this event or
     * autocommands are blocked.
     */
    जर (event == अंक_EVENTS || first_autopat[(पूर्णांक)event] == रिक्त
	    || autocmd_blocked > 0)
	जा BYPASS_AU;

    /*
     * When autocommands are busy, new autocommands are only executed when
     * explicitly enabled with the "nested" flag.
     */
    जर (autocmd_busy && !(force || autocmd_nested))
	जा BYPASS_AU;

#ifdef FEAT_EVAL
    /*
     * Quickly return when immediately aborting on error, or when an interrupt
     * occurred or an exception was thrown but not caught.
     */
    जर (aborting())
	जा BYPASS_AU;
#endif

    /*
     * FileChangedShell never nests, because it can create an endless loop.
     */
    जर (filechangeshell_busy && (event == EVENT_FILECHANGEDSHELL
				      || event == EVENT_FILECHANGEDSHELLPOST))
	जा BYPASS_AU;

    /*
     * Ignore events in 'eventignore'.
     */
    जर (event_ignored(event, p_ei))
	जा BYPASS_AU;

    पूर्णांक win_ignore = FALSE;
    // If event is allowed in 'eventignorewin', check if curwin or all windows
    // into "buf" are ignoring the event.
    जर (buf == curbuf && event_tab[event].किल्ली <= 0)
	win_ignore = event_ignored(event, curwin->w_p_eiw);
    अन्यथा जर (buf != रिक्त && event_tab[event].किल्ली <= 0 && buf->b_nwindows > 0)
    {
	tabpage_T *tp;
	win_T *wp;

	win_ignore = TRUE;
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    जर (wp->w_buffer == buf && !event_ignored(event, wp->w_p_eiw))
	    {
		win_ignore = FALSE;
		थांबा;
	    }
    }
    जर (win_ignore)
	जा BYPASS_AU;

    /*
     * Allow nesting of autocommands, but restrict the depth, because it's
     * possible to create an endless loop.
     */
    जर (nesting == 10)
    {
	emsg(_(e_autocommand_nesting_too_deep));
	जा BYPASS_AU;
    }

    /*
     * Check if these autocommands are disabled.  Used when doing ":all" or
     * ":ball".
     */
    जर (       (autocmd_no_enter
		&& (event == EVENT_WINENTER || event == EVENT_BUFENTER))
	    || (autocmd_no_leave
		&& (event == EVENT_WINLEAVE || event == EVENT_BUFLEAVE)))
	जा BYPASS_AU;

    जर (event == EVENT_CMDLINECHANGED)
	++aucmd_cmdline_changed_मोजणी;

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
    जर (fname_io == रिक्त)
    {
	जर (event == EVENT_COLORSCHEME || event == EVENT_COLORSCHEMEPRE
						 || event == EVENT_OPTIONSET
						 || event == EVENT_MODECHANGED
						 || event == EVENT_TERMRESPONSEALL)
	    autocmd_fname = रिक्त;
	अन्यथा जर (fname != रिक्त && !ends_excmd(*fname))
	    autocmd_fname = fname;
	अन्यथा जर (buf != रिक्त)
	    autocmd_fname = buf->b_ffname;
	अन्यथा
	    autocmd_fname = रिक्त;
    }
    अन्यथा
	autocmd_fname = fname_io;
    जर (autocmd_fname != रिक्त)
	autocmd_fname = vim_strsave(autocmd_fname);
    autocmd_fname_full = FALSE; // call FullName_save() later

    /*
     * Set the buffer number to be used for <abuf>.
     */
    जर (buf == रिक्त)
	autocmd_bufnr = 0;
    अन्यथा
	autocmd_bufnr = buf->b_fnum;

    /*
     * When the file name is NULL or empty, use the file name of buffer "buf".
     * Always use the full path of the file name to match with, in case
     * "allow_dirs" is set.
     */
    जर (fname == रिक्त || *fname == NUL)
    {
	जर (buf == रिक्त)
	    fname = रिक्त;
	अन्यथा
	{
#ifdef FEAT_SYN_HL
	    जर (event == EVENT_SYNTAX)
		fname = buf->b_p_syn;
	    अन्यथा
#endif
		जर (event == EVENT_FILETYPE)
		    fname = buf->b_p_ft;
		अन्यथा
		{
		    जर (buf->b_sfname != रिक्त)
			sfname = vim_strsave(buf->b_sfname);
		    fname = buf->b_ffname;
		}
	}
	जर (fname == रिक्त)
	    fname = (char_u *)"";
	fname = vim_strsave(fname);	// make a copy, so we can change it
    }
    अन्यथा
    {
	sfname = vim_strsave(fname);
	// Don't try expanding FileType, Syntax, FuncUndefined, WindowID,
	// ColorScheme, QuickFixCmd*, DirChanged and similar.
	जर (event == EVENT_FILETYPE
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
	अन्यथा
	    fname = FullName_save(fname, FALSE);
    }
    जर (fname == रिक्त)	    // out of memory
    {
	vim_free(sfname);
	retval = FALSE;
	जा BYPASS_AU;
    }

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    जर (sfname != रिक्त)
	forward_slash(sfname);
    forward_slash(fname);
#endif

#ifdef VMS
    // remove version for correct match
    जर (sfname != रिक्त)
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
    estack_ढकला(ETYPE_AUCMD, रिक्त, 0);
    ESTACK_CHECK_SETUP;

    save_current_sctx = current_sctx;

#ifdef FEAT_EVAL
# ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
	prof_child_enter(&wait_time); // doesn't count for the caller itself
# endif

    // Don't use local function variables, if called from a function.
    save_funccal(&funccal_entry);
#endif

    /*
     * When starting to execute autocommands, save the search patterns.
     */
    जर (!autocmd_busy)
    {
	save_शोध_patterns();
	जर (!ins_compl_active())
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
    जर (event == EVENT_FILETYPE)
	curbuf->b_did_filetype = true;

    tail = gettail(fname);

    // Find first autocommand that matches
    CLEAR_FIELD(patcmd);
    patcmd.curpat = first_autopat[(पूर्णांक)event];
    patcmd.group = group;
    patcmd.fname = fname;
    patcmd.sfname = sfname;
    patcmd.tail = tail;
    patcmd.event = event;
    patcmd.arg_bufnr = autocmd_bufnr;
    auto_next_pat(&patcmd, FALSE);

    // found one, start executing the autocommands
    जर (patcmd.curpat != रिक्त)
    {
	// add to active_apc_list
	patcmd.next = active_apc_list;
	active_apc_list = &patcmd;

#ifdef FEAT_EVAL
	// set v:cmdarg (only when there is a matching pattern)
	save_cmdbang = (दीर्घ)get_vim_var_nr(VV_CMDBANG);
	जर (eap != रिक्त)
	{
	    save_cmdarg = set_cmdarg(eap, रिक्त);
	    set_vim_var_nr(VV_CMDBANG, (दीर्घ)eap->forceit);
	}
	अन्यथा
	    save_cmdarg = रिक्त;	// avoid gcc warning
#endif
	retval = TRUE;
	// mark the last pattern, to avoid an endless loop when more patterns
	// are added when executing autocommands
	साठी (ap = patcmd.curpat; ap->next != रिक्त; ap = ap->next)
	    ap->last = FALSE;
	ap->last = TRUE;

	// Make sure cursor and topline are valid.  The first time the current
	// values are saved, restored by reset_lnums().  When nested only the
	// values are corrected when needed.
	जर (nesting == 1)
	    check_lnums(TRUE);
	अन्यथा
	    check_lnums_nested(TRUE);

	पूर्णांक save_did_emsg = did_emsg;
	पूर्णांक save_ex_pressedreturn = get_pressedreturn();

	do_cmdline(रिक्त, getnextac, (शून्य *)&patcmd,
				     DOCMD_NOWAIT|DOCMD_VERBOSE|DOCMD_REPEAT);

	did_emsg += save_did_emsg;
	set_pressedreturn(save_ex_pressedreturn);

	जर (nesting == 1)
	    // restore cursor and topline, unless they were changed
	    reset_lnums();

#ifdef FEAT_EVAL
	जर (eap != रिक्त)
	{
	    (शून्य)set_cmdarg(रिक्त, save_cmdarg);
	    set_vim_var_nr(VV_CMDBANG, save_cmdbang);
	}
#endif
	// delete from active_apc_list
	जर (active_apc_list == &patcmd)	    // just in case
	    active_apc_list = patcmd.next;
    }

    जर (RedrawingDisabled > 0)
	--RedrawingDisabled;
    autocmd_busy = save_autocmd_busy;
    filechangeshell_busy = FALSE;
    autocmd_nested = save_autocmd_nested;
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW;
    estack_काढा();
    vim_free(autocmd_fname);
    autocmd_fname = save_autocmd_fname;
    autocmd_fname_full = save_autocmd_fname_full;
    autocmd_bufnr = save_autocmd_bufnr;
    autocmd_match = save_autocmd_match;
    current_sctx = save_current_sctx;
#ifdef FEAT_EVAL
    restore_funccal();
# ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
	prof_child_exit(&wait_time);
# endif
#endif
    किल्ली_Typed = save_किल्ली_Typed;
    vim_free(fname);
    vim_free(sfname);
    --nesting;		// see matching increment above

    /*
     * When stopping to execute autocommands, restore the search patterns and
     * the redo buffer.  Free any buffers in the au_pending_free_buf list and
     * free any windows in the au_pending_free_win list.
     */
    जर (!autocmd_busy)
    {
	restore_शोध_patterns();
	जर (did_save_redobuff)
	    restoreRedobuff(&save_redo);
	curbuf->b_did_filetype = false;
	पर्यंत (au_pending_free_buf != रिक्त)
	{
	    buf_T *b = au_pending_free_buf->b_next;

	    vim_free(au_pending_free_buf);
	    au_pending_free_buf = b;
	}
	पर्यंत (au_pending_free_win != रिक्त)
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
    जर (curbuf == old_curbuf
	    && (event == EVENT_BUFREADPOST
		|| event == EVENT_BUFWRITEPOST
		|| event == EVENT_FILEAPPENDPOST
		|| event == EVENT_VIMLEAVE
		|| event == EVENT_VIMLEAVEPRE))
    {
	जर (curbuf->b_changed != save_changed)
	    need_maketitle = TRUE;
	curbuf->b_changed = save_changed;
    }

    au_cleanup();	// may really delete removed patterns/commands now

BYPASS_AU:
    // When wiping out a buffer make sure all its buffer-local autocommands
    // are deleted.
    जर (event == EVENT_BUFWIPEOUT && buf != रिक्त)
	aubuflocal_remove(buf);

    जर (retval == OK && event == EVENT_FILETYPE)
	curbuf->b_au_did_filetype = true;

    परत retval;
}

#ifdef FEAT_EVAL
स्थिर char_u	*old_termresponse = रिक्त;
स्थिर char_u	*old_termu7resp = रिक्त;
स्थिर char_u	*old_termblinkresp = रिक्त;
स्थिर char_u	*old_termrbgresp = रिक्त;
स्थिर char_u	*old_termrfgresp = रिक्त;
स्थिर char_u	*old_termstyleresp = रिक्त;
#endif

/*
 * Block triggering autocommands until unblock_autocmd() is called.
 * Can be used recursively, so long as it's symmetric.
 */
    शून्य
block_autocmds(शून्य)
{
#ifdef FEAT_EVAL
    // Remember the value of v:termresponse.
    जर (autocmd_blocked == 0)
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

    शून्य
unblock_autocmds(शून्य)
{
    --autocmd_blocked;

#ifdef FEAT_EVAL
    // When v:termresponse, etc, were set while autocommands were blocked,
    // trigger the autocommands now.  Esp. useful when executing a shell
    // command during startup (vimdiff).
    जर (autocmd_blocked == 0)
    {
	जर (get_vim_var_str(VV_TERMRESPONSE) != old_termresponse)
	{
	    apply_autocmds(EVENT_TERMRESPONSE, रिक्त, रिक्त, FALSE, curbuf);
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"version", रिक्त, FALSE, curbuf);
	}
	जर (get_vim_var_str(VV_TERMU7RESP) != old_termu7resp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"ambiguouswidth", रिक्त, FALSE, curbuf);
	}
	जर (get_vim_var_str(VV_TERMBLINKRESP) != old_termblinkresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorblink", रिक्त, FALSE, curbuf);
	}
	जर (get_vim_var_str(VV_TERMRBGRESP) != old_termrbgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"background", रिक्त, FALSE, curbuf);
	}
	जर (get_vim_var_str(VV_TERMRFGRESP) != old_termrfgresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"foreground", रिक्त, FALSE, curbuf);
	}
	जर (get_vim_var_str(VV_TERMSTYLERESP) != old_termstyleresp)
	{
	    apply_autocmds(EVENT_TERMRESPONSEALL, (char_u *)"cursorshape", रिक्त, FALSE, curbuf);
	}
    }
#endif
}

    पूर्णांक
is_autocmd_blocked(शून्य)
{
    परत autocmd_blocked != 0;
}

/*
 * Find next autocommand pattern that matches.
 */
    स्थिर शून्य
auto_next_pat(
    AutoPatCmd_T *apc,
    पूर्णांक		stop_at_last)	    // stop when 'last' flag is set
{
    AutoPat	*ap;
    estack_T	*entry;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;

    // Clear the exestack entry for this ETYPE_AUCMD entry.
    VIM_CLEAR(entry->es_name);
    entry->es_info.aucmd = रिक्त;

    साठी (ap = apc->curpat; ap != रिक्त && !got_int; ap = ap->next)
    {
	apc->curpat = रिक्त;

	// Only use a pattern when it has not been removed, has commands and
	// the group matches. For buffer-local autocommands only check the
	// buffer number.
	जर (ap->pat != रिक्त && ap->cmds != रिक्त
		&& (apc->group == AUGROUP_ALL || apc->group == ap->group))
	{
	    // execution-condition
	    जर (ap->buflocal_nr == 0
		    ? (match_file_pat(रिक्त, &ap->reg_prog, apc->fname,
				      apc->sfname, apc->tail, ap->allow_dirs))
		    : ap->buflocal_nr == apc->arg_bufnr)
	    {
		string_T    *event_name;
		अक्षर	    *fmt;
		char_u	    *namep;
		AutoCmd	    *cp;

		event_name = event_nr2name(apc->event);
		fmt = _("%s Autocommands for \"%s\"");
		namep = alloc(STRLEN(fmt) + event_name->length + ap->patlen + 1);
		जर (namep != रिक्त)
		{
		    प्र_लिखो((अक्षर *)namep, fmt, (अक्षर *)event_name->string, (अक्षर *)ap->pat);
		    जर (p_verbose >= 8)
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
		साठी (cp = ap->cmds; cp->next != रिक्त; cp = cp->next)
		    cp->last = FALSE;
		cp->last = TRUE;
	    }
	    ओळ_breakcheck();
	    जर (apc->curpat != रिक्त)	    // found a match
		थांबा;
	}
	जर (stop_at_last && ap->last)
	    थांबा;
    }
}

#जर defined(FEAT_EVAL)
/*
 * Get the script context where autocommand "acp" is defined.
 */
    sctx_T *
acp_script_ctx(AutoPatCmd_T *acp)
{
    परत &acp->script_ctx;
}
#endif

/*
 * Get next autocommand command.
 * Called by do_cmdline() to get the next line for ":if".
 * Returns allocated string, or NULL for end of autocommands.
 */
    char_u *
getnextac(
	पूर्णांक c UNUSED,
	शून्य *cookie,
	पूर्णांक indent UNUSED,
	getline_opt_T options UNUSED)
{
    AutoPatCmd_T    *acp = (AutoPatCmd_T *)cookie;
    char_u	    *retval;
    AutoCmd	    *ac;

    // Can be called again after returning the last line.
    जर (acp->curpat == रिक्त)
	परत रिक्त;

    // repeat until we find an autocommand to execute
    साठी (;;)
    {
	// skip removed commands
	पर्यंत (acp->nextcmd != रिक्त && acp->nextcmd->cmd == रिक्त)
	    जर (acp->nextcmd->last)
		acp->nextcmd = रिक्त;
	    अन्यथा
		acp->nextcmd = acp->nextcmd->next;

	जर (acp->nextcmd != रिक्त)
	    थांबा;

	// at end of commands, find next pattern that matches
	जर (acp->curpat->last)
	    acp->curpat = रिक्त;
	अन्यथा
	    acp->curpat = acp->curpat->next;
	जर (acp->curpat != रिक्त)
	    auto_next_pat(acp, TRUE);
	जर (acp->curpat == रिक्त)
	    परत रिक्त;
    }

    ac = acp->nextcmd;

    जर (p_verbose >= 9)
    {
	verbose_enter_scroll();
	smsg(_("autocommand %s"), ac->cmd);
	संदेश_puts("\n");   // don't overwrite this either
	verbose_leave_scroll();
    }
    retval = vim_strsave(ac->cmd);
    // Remove one-shot ("once") autocmd in anticipation of its execution.
    जर (ac->once)
	au_del_cmd(ac);
    autocmd_nested = ac->nested;
    current_sctx = ac->script_ctx;
    acp->script_ctx = current_sctx;
    जर (ac->last)
	acp->nextcmd = रिक्त;
    अन्यथा
	acp->nextcmd = ac->next;
    परत retval;
}

/*
 * Implementation of has_autocmd() and has_autocmd_fullname().
 * When "is_full" is TRUE "sfname" is already a full path name.
 */
    स्थिर पूर्णांक
has_autocmd_ext(event_T event, char_u *sfname, buf_T *buf, bool is_full)
{
    AutoPat	*ap;
    char_u	*fname;
    char_u	*tail = gettail(sfname);
    पूर्णांक		retval = FALSE;

    जर (first_autopat[(पूर्णांक)event] == रिक्त)
	परत FALSE;

    fname = is_full ? vim_strsave(sfname) : FullName_save(sfname, FALSE);
    जर (fname == रिक्त)
	परत FALSE;

#ifdef BACKSLASH_IN_FILENAME
    /*
     * Replace all backslashes with forward slashes.  This makes the
     * autocommand patterns portable between Unix and MS-DOS.
     */
    sfname = vim_strsave(sfname);
    जर (sfname != रिक्त)
	forward_slash(sfname);
    forward_slash(fname);
#endif

    FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	जर (ap->pat != रिक्त && ap->cmds != रिक्त
	      && (ap->buflocal_nr == 0
		? match_file_pat(रिक्त, &ap->reg_prog,
					  fname, sfname, tail, ap->allow_dirs)
		: buf != रिक्त && ap->buflocal_nr == buf->b_fnum
	   ))
	{
	    retval = TRUE;
	    थांबा;
	}

    vim_free(fname);
#ifdef BACKSLASH_IN_FILENAME
    vim_free(sfname);
#endif

    परत retval;
}

/*
 * Return TRUE if there is a matching autocommand for "fname".
 * To account for buffer-local autocommands, function needs to know
 * in which buffer the file will be opened.
 */
    पूर्णांक
has_autocmd(event_T event, char_u *sfname, buf_T *buf)
{
    परत has_autocmd_ext(event, sfname, buf, false);
}

/*
 * Like has_autocmd(), but "fname" is already a full path name, e.g. as
 * returned by fix_fname().  Avoids expanding it again, which is slow on
 * MS-Windows.
 */
    पूर्णांक
has_autocmd_fullname(event_T event, char_u *fname, buf_T *buf)
{
    परत has_autocmd_ext(event, fname, buf, true);
}

/*
 * Function given to ExpandGeneric() to obtain the list of autocommand group
 * names.
 */
    char_u *
get_augroup_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    जर (idx == augroups.ga_len)		// add "END" add the end
	परत (char_u *)"END";
    जर (idx < 0 || idx >= augroups.ga_len)	// end of list
	परत रिक्त;
    जर (AUGROUP_NAME(idx) == रिक्त || AUGROUP_NAME(idx) == get_deleted_augroup())
	// skip deleted entries
	परत (char_u *)"";
    परत AUGROUP_NAME(idx);		// return a name
}

स्थिर पूर्णांक include_groups = FALSE;

    char_u  *
set_context_in_autocmd(
    expand_T	*xp,
    char_u	*arg,
    पूर्णांक		doautocmd)	// TRUE for :doauto*, FALSE for :autocmd
{
    char_u	*p;
    पूर्णांक		group;

    // check for a group name, skip it if present
    include_groups = FALSE;
    p = arg;
    group = au_get_grouparg(&arg);
    जर (group == AUGROUP_ERROR)
	परत रिक्त;
    // If there only is a group name that's what we expand.
    जर (*arg == NUL && group != AUGROUP_ALL && !VIM_ISWHITE(arg[-1]))
    {
	arg = p;
	group = AUGROUP_ALL;
    }

    // skip over event name
    साठी (p = arg; *p != NUL && !VIM_ISWHITE(*p); ++p)
	जर (*p == ',')
	    arg = p + 1;
    जर (*p == NUL)
    {
	जर (group == AUGROUP_ALL)
	    include_groups = TRUE;
	xp->xp_context = EXPAND_EVENTS;	    // expand event name
	xp->xp_pattern = arg;
	परत रिक्त;
    }

    // skip over pattern
    arg = skipwhite(p);
    पर्यंत (*arg && (!VIM_ISWHITE(*arg) || arg[-1] == '\\'))
	arg++;
    जर (*arg)
    {
	// skip over "++once", "++nested" and "nested"
	arg = skipwhite(arg);
	साठी (पूर्णांक i = 0; i < 2; i++)
	{
	    जर (STRNCMP(arg, "++once", 6) == 0 && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	    जर (STRNCMP(arg, "++nested", 8) == 0 && VIM_ISWHITE(arg[8]))
		arg = skipwhite(arg + 8);
	    जर (!in_vim9script() && STRNCMP(arg, "nested", 6) == 0
						       && VIM_ISWHITE(arg[6]))
		arg = skipwhite(arg + 6);
	}
	परत arg;			    // expand (next) command
    }

    जर (doautocmd)
	xp->xp_context = EXPAND_FILES;	    // expand file names
    अन्यथा
	xp->xp_context = EXPAND_NOTHING;    // pattern is not expanded
    परत रिक्त;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names.
 */
    char_u *
get_event_name(expand_T *xp UNUSED, पूर्णांक idx)
{
    पूर्णांक i;

    जर (idx < augroups.ga_len)		// First list group names, if wanted
    {
	जर (!include_groups || AUGROUP_NAME(idx) == रिक्त
				 || AUGROUP_NAME(idx) == get_deleted_augroup())
	    परत (char_u *)"";	// skip deleted entries
	परत AUGROUP_NAME(idx);	// return a name
    }

    i = idx - augroups.ga_len;
    जर (i < 0 || i >= अंक_EVENTS)
	परत रिक्त;

    परत event_tab[i].मूल्य.string;
}

/*
 * Function given to ExpandGeneric() to obtain the list of event names. Don't
 * include groups.
 */
    char_u *
get_event_name_no_group(expand_T *xp UNUSED, पूर्णांक idx, पूर्णांक win)
{
    जर (idx < 0 || idx >= अंक_EVENTS)
	परत रिक्त;

    जर (!win)
	परत event_tab[idx].मूल्य.string;

    // Need to check subset of allowed values for 'eventignorewin'.
    पूर्णांक j = 0;
    साठी (पूर्णांक i = 0; i < अंक_EVENTS; ++i)
    {
	j += event_tab[i].किल्ली <= 0;
	जर (j == idx + 1)
	    परत event_tab[i].मूल्य.string;
    }

    परत रिक्त;
}

/*
 * Return TRUE when there is a TabClosedPre autocommand defined.
 */
    पूर्णांक
has_tabclosedpre(शून्य)
{
    परत (first_autopat[(पूर्णांक)EVENT_TABCLOSEDPRE] != रिक्त);
}

#जर defined(FEAT_EVAL)
/*
 * Return TRUE if autocmd is supported.
 */
    पूर्णांक
autocmd_supported(char_u *name)
{
    char_u *p;

    परत (event_name2nr(name, &p) != अंक_EVENTS);
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
    पूर्णांक
au_exists(char_u *arg)
{
    char_u	*arg_save;
    char_u	*pattern = रिक्त;
    char_u	*event_name;
    char_u	*p;
    event_T	event;
    AutoPat	*ap;
    buf_T	*buflocal_buf = रिक्त;
    पूर्णांक		group;
    पूर्णांक		retval = FALSE;

    // Make a copy so that we can change the '#' chars to a NUL.
    arg_save = vim_strsave(arg);
    जर (arg_save == रिक्त)
	परत FALSE;
    p = vim_strchr(arg_save, '#');
    जर (p != रिक्त)
	*p++ = NUL;

    // First, look for an autocmd group name
    group = au_find_group(arg_save);
    जर (group == AUGROUP_ERROR)
    {
	// Didn't match a group name, assume the first argument is an event.
	group = AUGROUP_ALL;
	event_name = arg_save;
    }
    अन्यथा
    {
	जर (p == रिक्त)
	{
	    // "Group": group name is present and it's recognized
	    retval = TRUE;
	    जा theend;
	}

	// Must be "Group#Event" or "Group#Event#pat".
	event_name = p;
	p = vim_strchr(event_name, '#');
	जर (p != रिक्त)
	    *p++ = NUL;	    // "Group#Event#pat"
    }

    pattern = p;	    // "pattern" is NULL when there is no pattern

    // find the index (enum) for the event name
    event = event_name2nr(event_name, &p);

    // return FALSE if the event name is not recognized
    जर (event == अंक_EVENTS)
	जा theend;

    // Find the first autocommand for this event.
    // If there isn't any, return FALSE;
    // If there is one and no pattern given, return TRUE;
    ap = first_autopat[(पूर्णांक)event];
    जर (ap == रिक्त)
	जा theend;

    // if pattern is "<buffer>", special handling is needed which uses curbuf
    // for pattern "<buffer=N>, fnamecmp() will work fine
    जर (pattern != रिक्त && STRICMP(pattern, "<buffer>") == 0)
	buflocal_buf = curbuf;

    // Check if there is an autocommand with the given pattern.
    साठी ( ; ap != रिक्त; ap = ap->next)
	// only use a pattern when it has not been removed and has commands.
	// For buffer-local autocommands, fnamecmp() works fine.
	जर (ap->pat != रिक्त && ap->cmds != रिक्त
	    && (group == AUGROUP_ALL || ap->group == group)
	    && (pattern == रिक्त
		|| (buflocal_buf == रिक्त
		    ? fnamecmp(ap->pat, pattern) == 0
		    : ap->buflocal_nr == buflocal_buf->b_fnum)))
	{
	    retval = TRUE;
	    थांबा;
	}

theend:
    vim_free(arg_save);
    परत retval;
}

/*
 * autocmd_add() and autocmd_delete() functions
 */
    स्थिर शून्य
autocmd_add_or_delete(typval_T *argvars, typval_T *rettv, पूर्णांक delete)
{
    list_T	*aucmd_list;
    listitem_T	*li;
    dict_T	*event_dict;
    dictitem_T	*di;
    char_u	*event_name = रिक्त;
    list_T	*event_list;
    listitem_T	*eli;
    event_T	event;
    char_u	*group_name = रिक्त;
    पूर्णांक		group;
    char_u	*pat = रिक्त;
    list_T	*pat_list;
    listitem_T	*pli;
    char_u	*cmd = रिक्त;
    char_u	*end;
    पूर्णांक		once;
    पूर्णांक		nested;
    पूर्णांक		replace;		// replace the cmd for a group/event
    पूर्णांक		retval = VVAL_TRUE;
    पूर्णांक		save_augroup = current_augroup;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_संख्या = VVAL_FALSE;

    जर (check_restricted() || check_secure())
	परत;

    जर (check_for_list_arg(argvars, 0) == FAIL)
	परत;

    aucmd_list = argvars[0].vval.v_list;
    जर (aucmd_list == रिक्त)
	परत;

    FOR_ALL_LIST_वस्तुसंच(aucmd_list, li)
    {
	VIM_CLEAR(group_name);
	VIM_CLEAR(cmd);
	event_name = रिक्त;
	event_list = रिक्त;
	pat = रिक्त;
	pat_list = रिक्त;

	जर (li->li_tv.v_type != VAR_DICT)
	    सुरूठेवा;

	event_dict = li->li_tv.vval.v_dict;
	जर (event_dict == रिक्त)
	    सुरूठेवा;

	di = dict_find(event_dict, (char_u *)"event", -1);
	जर (di != रिक्त)
	{
	    जर (di->di_tv.v_type == VAR_STRING)
	    {
		event_name = di->di_tv.vval.v_string;
		जर (event_name == रिक्त)
		{
		    emsg(_(e_string_required));
		    सुरूठेवा;
		}
	    }
	    अन्यथा जर (di->di_tv.v_type == VAR_LIST)
	    {
		event_list = di->di_tv.vval.v_list;
		जर (event_list == रिक्त)
		{
		    emsg(_(e_list_required));
		    सुरूठेवा;
		}
	    }
	    अन्यथा
	    {
		emsg(_(e_string_or_list_expected));
		सुरूठेवा;
	    }
	}

	group_name = dict_get_string(event_dict, "group", TRUE);
	जर (group_name == रिक्त || *group_name == NUL)
	    // if the autocmd group name is not specified, then use the current
	    // autocmd group
	    group = current_augroup;
	अन्यथा
	{
	    group = au_find_group(group_name);
	    जर (group == AUGROUP_ERROR)
	    {
		जर (delete)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    थांबा;
		}
		// group is not found, create it now
		group = au_new_group(group_name);
		जर (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), group_name);
		    retval = VVAL_FALSE;
		    थांबा;
		}

		current_augroup = group;
	    }
	}

	// if a buffer number is specified, then generate a pattern of the form
	// "<buffer=n>. Otherwise, use the pattern supplied by the user.
	जर (dict_has_किल्ली(event_dict, "bufnr"))
	{
	    varnumber_T	bnum;

	    bnum = dict_get_संख्या_def(event_dict, "bufnr", -1);
	    जर (bnum == -1)
		सुरूठेवा;

	    vim_snprintf((अक्षर *)IObuff, IOSIZE, "<buffer=%d>", (पूर्णांक)bnum);
	    pat = IObuff;
	}
	अन्यथा
	{
	    di = dict_find(event_dict, (char_u *)"pattern", -1);
	    जर (di != रिक्त)
	    {
		जर (di->di_tv.v_type == VAR_STRING)
		{
		    pat = di->di_tv.vval.v_string;
		    जर (pat == रिक्त)
		    {
			emsg(_(e_string_required));
			सुरूठेवा;
		    }
		}
		अन्यथा जर (di->di_tv.v_type == VAR_LIST)
		{
		    pat_list = di->di_tv.vval.v_list;
		    जर (pat_list == रिक्त)
		    {
			emsg(_(e_list_required));
			सुरूठेवा;
		    }
		}
		अन्यथा
		{
		    emsg(_(e_string_or_list_expected));
		    सुरूठेवा;
		}
	    }
	    अन्यथा जर (delete)
		pat = (char_u *)"";
	}

	once = dict_get_bool(event_dict, "once", FALSE);
	nested = dict_get_bool(event_dict, "nested", FALSE);
	// if 'replace' is true, then remove all the commands associated with
	// this autocmd event/group and add the new command.
	replace = dict_get_bool(event_dict, "replace", FALSE);

	cmd = dict_get_string(event_dict, "cmd", TRUE);
	जर (cmd == रिक्त)
	{
	    जर (delete)
		cmd = vim_strsave((char_u *)"");
	    अन्यथा
		सुरूठेवा;
	}

	जर (delete && (event_name == रिक्त
		    || (event_name[0] == '*' && event_name[1] == NUL)))
	{
	    // if the event name is not specified or '*', delete all the events
	    साठी (event = (event_T)0; (पूर्णांक)event < अंक_EVENTS;
		    event = (event_T)((पूर्णांक)event + 1))
	    {
		जर (do_autocmd_event(event, pat, once, nested, cmd, delete,
							group, 0) == FAIL)
		{
		    retval = VVAL_FALSE;
		    थांबा;
		}
	    }
	}
	अन्यथा
	{
	    char_u *p = रिक्त;

	    eli = रिक्त;
	    end = रिक्त;
	    पर्यंत (TRUE)
	    {
		जर (event_list != रिक्त)
		{
		    जर (eli == रिक्त)
			eli = event_list->lv_first;
		    अन्यथा
			eli = eli->li_next;
		    जर (eli == रिक्त)
			थांबा;
		    जर (eli->li_tv.v_type != VAR_STRING
			    || (p = eli->li_tv.vval.v_string) == रिक्त)
		    {
			emsg(_(e_string_required));
			थांबा;
		    }
		}
		अन्यथा
		{
		    जर (p == रिक्त)
			p = event_name;
		    जर (p == रिक्त || *p == NUL)
			थांबा;
		}

		event = event_name2nr(p, &end);
		जर (event == अंक_EVENTS || *end != NUL)
		{
		    // this also catches something following a valid event name
		    semsg(_(e_no_such_event_str), p);
		    retval = VVAL_FALSE;
		    थांबा;
		}
		जर (pat != रिक्त)
		{
		    जर (do_autocmd_event(event, pat, once, nested, cmd,
				delete | replace, group, 0) == FAIL)
		    {
			retval = VVAL_FALSE;
			थांबा;
		    }
		}
		अन्यथा जर (pat_list != रिक्त)
		{
		    FOR_ALL_LIST_वस्तुसंच(pat_list, pli)
		    {
			जर (pli->li_tv.v_type != VAR_STRING
				|| pli->li_tv.vval.v_string == रिक्त)
			{
			    emsg(_(e_string_required));
			    सुरूठेवा;
			}
			जर (do_autocmd_event(event,
				    pli->li_tv.vval.v_string, once, nested,
				    cmd, delete | replace, group, 0) ==
				FAIL)
			{
			    retval = VVAL_FALSE;
			    थांबा;
			}
		    }
		    जर (retval == VVAL_FALSE)
			थांबा;
		}
		जर (event_name != रिक्त)
		    p = end;
	    }
	}

	// if only the autocmd group name is specified for delete and the
	// autocmd event, pattern and cmd are not specified, then delete the
	// autocmd group.
	जर (delete && group_name != रिक्त &&
		(event_name == रिक्त || event_name[0] == NUL)
		&& (pat == रिक्त || pat[0] == NUL)
		&& (cmd == रिक्त || cmd[0] == NUL))
	    au_del_group(group_name);
    }

    VIM_CLEAR(group_name);
    VIM_CLEAR(cmd);

    current_augroup = save_augroup;
    rettv->vval.v_संख्या = retval;
}

/*
 * autocmd_add() function
 */
    शून्य
f_autocmd_add(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, FALSE);
}

/*
 * autocmd_delete() function
 */
    शून्य
f_autocmd_delete(typval_T *argvars, typval_T *rettv)
{
    autocmd_add_or_delete(argvars, rettv, TRUE);
}

/*
 * autocmd_get() function
 * Returns a List of autocmds.
 */
    शून्य
f_autocmd_get(typval_T *argvars, typval_T *rettv)
{
    event_T	event_arg = अंक_EVENTS;
    event_T	event;
    AutoPat	*ap;
    AutoCmd	*ac;
    list_T	*event_list;
    dict_T	*event_dict;
    char_u	*pat = रिक्त;
    char_u	*name = रिक्त;
    पूर्णांक		group = AUGROUP_ALL;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;
    जर (check_for_opt_dict_arg(argvars, 0) == FAIL)
	परत;

    जर (argvars[0].v_type == VAR_DICT)
    {
	// return only the autocmds in the specified group
	जर (dict_has_किल्ली(argvars[0].vval.v_dict, "group"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "group", TRUE);
	    जर (name == रिक्त)
		परत;

	    जर (*name == NUL)
		group = AUGROUP_DEFAULT;
	    अन्यथा
	    {
		group = au_find_group(name);
		जर (group == AUGROUP_ERROR)
		{
		    semsg(_(e_no_such_group_str), name);
		    vim_free(name);
		    परत;
		}
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified event
	जर (dict_has_किल्ली(argvars[0].vval.v_dict, "event"))
	{
	    name = dict_get_string(argvars[0].vval.v_dict, "event", TRUE);
	    जर (name == रिक्त)
		परत;

	    जर (name[0] == '*' && name[1] == NUL)
		event_arg = अंक_EVENTS;
	    अन्यथा
	    {
		keyvalue_T target;
		keyvalue_T *entry;

		target.किल्ली = 0;
		target.मूल्य.string = name;
		target.मूल्य.length = STRLEN(target.मूल्य.string);
		entry = (keyvalue_T *)द्वा_खोज(&target, &event_tab,
		    अंक_EVENTS, आकार(event_tab[0]), cmp_keyvalue_मूल्य_ni);
		जर (entry == रिक्त)
		{
		    semsg(_(e_no_such_event_str), name);
		    vim_free(name);
		    परत;
		}
		event_arg = (event_T)असल(entry->किल्ली);
	    }
	    vim_free(name);
	}

	// return only the autocmds for the specified pattern
	जर (dict_has_किल्ली(argvars[0].vval.v_dict, "pattern"))
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "pattern", TRUE);
	    जर (pat == रिक्त)
		परत;
	}
    }

    event_list = rettv->vval.v_list;

    // iterate through all the autocmd events
    साठी (event = (event_T)0; (पूर्णांक)event < अंक_EVENTS;
	    event = (event_T)((पूर्णांक)event + 1))
    {
	string_T    *event_name;

	जर (event_arg != अंक_EVENTS && event != event_arg)
	    सुरूठेवा;

	event_name = event_nr2name(event);

	// iterate through all the patterns for this autocmd event
	FOR_ALL_AUTOCMD_PATTERNS(event, ap)
	{
	    string_T	group_name;

	    जर (ap->pat == रिक्त)		// pattern has been removed
		सुरूठेवा;

	    जर (group != AUGROUP_ALL && group != ap->group)
		सुरूठेवा;

	    जर (pat != रिक्त && STRCMP(pat, ap->pat) != 0)
		सुरूठेवा;

	    group_name.string = get_augroup_name(रिक्त, ap->group);
	    जर (group_name.string == रिक्त)
		STR_LITERAL_SET(group_name, "");
	    अन्यथा
		group_name.length = STRLEN(group_name.string);

	    // iterate through all the commands for this pattern and add one
	    // item for each cmd.
	    साठी (ac = ap->cmds; ac != रिक्त; ac = ac->next)
	    {
		event_dict = dict_alloc();
		जर (event_dict == रिक्त
			|| list_append_dict(event_list, event_dict) == FAIL)
		{
		    vim_free(pat);
		    परत;
		}

		जर (dict_add_string_len(event_dict, "event",
			event_name->string, (पूर्णांक)event_name->length) == FAIL
			|| dict_add_string_len(event_dict, "group",
			    group_name.string, (पूर्णांक)group_name.length) == FAIL
			|| (ap->buflocal_nr != 0
				&& (dict_add_संख्या(event_dict, "bufnr",
						    ap->buflocal_nr) == FAIL))
			|| dict_add_string(event_dict, "pattern",
							      ap->pat) == FAIL
			|| dict_add_string(event_dict, "cmd", ac->cmd) == FAIL
			|| dict_add_bool(event_dict, "once", ac->once) == FAIL
			|| dict_add_bool(event_dict, "nested",
							   ac->nested) == FAIL)
		{
		    vim_free(pat);
		    परत;
		}
	    }
	}
    }

    vim_free(pat);
}

#endif
