/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

// Use PLURAL_MSG() for messages that are passed to ngettext(), so that the
// second one uses msgid_plural.
#నిర్వచితమైతే DO_INIT
# define PLURAL_సందేశం(var1, msg1, var2, msg2) \
	అక్షరం var1[] = msg1; \
	అక్షరం var2[] = msg2;
#లేకపోతే
# define PLURAL_సందేశం(var1, msg1, var2, msg2) \
	బాహ్య అక్షరం var1[]; \
	బాహ్య అక్షరం var2[];
#అయితే_ముగింపు

/*
 * Definition of error messages, sorted on error number.
 */

EXTERN అక్షరం e_interrupted[]
	INIT(= N_("Interrupted"));

EXTERN అక్షరం e_backslash_should_be_followed_by[]
	INIT(= N_("E10: \\ should be followed by /, ? or &"));
EXTERN అక్షరం e_invalid_లోపల_cmdline_కిటికీ[]
	INIT(= N_("E11: Invalid in command-line window; :q<CR> closes the window"));
EXTERN అక్షరం e_command_not_allowed_from_vimrc_లోపల_current_dir_or_ట్యాగ్_శోధన[]
	INIT(= N_("E12: Command not allowed from exrc/vimrc in current dir or tag search"));
EXTERN అక్షరం e_file_exists[]
	INIT(= N_("E13: File exists (add ! to override)"));
// E14 unused
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_invalid_వ్యక్తీకరణ_str[]
	INIT(= N_("E15: Invalid expression: \"%s\""));
#అయితే_ముగింపు
EXTERN అక్షరం e_invalid_range[]
	INIT(= N_("E16: Invalid range"));
#ఒకవేళ defined(UNIX) || defined(FEAT_SYN_HL) \
	    || defined(FEAT_SPELL) || defined(FEAT_EVAL)
EXTERN అక్షరం e_str_is_directory[]
	INIT(= N_("E17: \"%s\" is a directory"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_unexpected_characters_లోపల_let[]
	INIT(= N_("E18: Unexpected characters in :let"));
EXTERN అక్షరం e_unexpected_characters_లోపల_assignment[]
	INIT(= N_("E18: Unexpected characters in assignment"));
#అయితే_ముగింపు
EXTERN అక్షరం e_mark_has_invalid_పంక్తి_సంఖ్య[]
	INIT(= N_("E19: Mark has invalid line number"));
EXTERN అక్షరం e_mark_not_సమితి[]
	INIT(= N_("E20: Mark not set"));
EXTERN అక్షరం e_cannot_make_changes_modifiable_is_off[]
	INIT(= N_("E21: Cannot make changes, 'modifiable' is off"));
EXTERN అక్షరం e_scripts_nested_too_deep[]
	INIT(= N_("E22: Scripts nested too deep"));
EXTERN అక్షరం e_no_alternate_file[]
	INIT(= N_("E23: No alternate file"));
EXTERN అక్షరం e_no_such_abbreviation[]
	INIT(= N_("E24: No such abbreviation"));
#ఒకవేళ !defined(FEAT_GUI) || defined(VIMDLL)
EXTERN అక్షరం e_gui_cannot_be_వాడిన_not_enabled_at_compile_సమయం[]
	INIT(= N_("E25: GUI cannot be used: Not enabled at compile time"));
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_RIGHTLEFT
EXTERN అక్షరం e_hebrew_cannot_be_వాడిన_not_enabled_at_compile_సమయం[]
	INIT(= N_("E26: Hebrew cannot be used: Not enabled at compile time\n"));
#అయితే_ముగింపు
EXTERN అక్షరం e_farsi_support_has_been_removed[]
	INIT(= N_("E27: Farsi support has been removed\n"));
#ఒకవేళ defined(FEAT_శోధన_EXTRA) || defined(FEAT_SYN_HL)
EXTERN అక్షరం e_no_such_highlight_సమూహం_name_str[]
	INIT(= N_("E28: No such highlight group name: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_no_inserted_పాఠ్యం_yet[]
	INIT(= N_("E29: No inserted text yet"));
EXTERN అక్షరం e_no_మునుపటి_command_పంక్తి[]
	INIT(= N_("E30: No previous command line"));
EXTERN అక్షరం e_no_such_mapping[]
	INIT(= N_("E31: No such mapping"));
EXTERN అక్షరం e_no_file_name[]
	INIT(= N_("E32: No file name"));
EXTERN అక్షరం e_no_మునుపటి_substitute_regular_వ్యక్తీకరణ[]
	INIT(= N_("E33: No previous substitute regular expression"));
EXTERN అక్షరం e_no_మునుపటి_command[]
	INIT(= N_("E34: No previous command"));
EXTERN అక్షరం e_no_మునుపటి_regular_వ్యక్తీకరణ[]
	INIT(= N_("E35: No previous regular expression"));
EXTERN అక్షరం e_not_enough_స్థలం[]
	INIT(= N_("E36: Not enough room"));
EXTERN అక్షరం e_no_write_since_చివరి_change[]
	INIT(= N_("E37: No write since last change"));
EXTERN అక్షరం e_no_write_since_చివరి_change_కూడు_bang_గమ్యం_override[]
	INIT(= N_("E37: No write since last change (add ! to override)"));
EXTERN అక్షరం e_null_వాదం[]
	INIT(= "E38: Null argument");
#ఒకవేళ defined(FEAT_DIGRAPHS) || defined(FEAT_TIMERS) || defined(FEAT_EVAL)
EXTERN అక్షరం e_సంఖ్య_expected[]
	INIT(= N_("E39: Number expected"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_cant_open_errorfile_str[]
	INIT(= N_("E40: Can't open errorfile %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_బహిర్గతం_యొక్క_జ్ఞాపకం[]
	INIT(= N_("E41: Out of memory!"));
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_no_errors[]
	INIT(= N_("E42: No Errors"));
#అయితే_ముగింపు
EXTERN అక్షరం e_damaged_సరిపోలిక_string[]
	INIT(= "E43: Damaged match string");
EXTERN అక్షరం e_corrupted_regexp_కార్యక్రమం[]
	INIT(= "E44: Corrupted regexp program");
EXTERN అక్షరం e_readonly_option_is_సమితి_కూడు_bang_గమ్యం_override[]
	INIT(= N_("E45: 'readonly' option is set (add ! to override)"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_change_readonly_variable[]
	INIT(= N_("E46: Cannot change read-only variable"));
EXTERN అక్షరం e_cannot_change_readonly_variable_str[]
	INIT(= N_("E46: Cannot change read-only variable \"%s\""));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_error_while_reading_errorfile[]
	INIT(= N_("E47: Error while reading errorfile"));
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SANDBOX
EXTERN అక్షరం e_not_allowed_లోపల_sandbox[]
	INIT(= N_("E48: Not allowed in sandbox"));
#అయితే_ముగింపు
EXTERN అక్షరం e_invalid_scroll_కొలత[]
	INIT(= N_("E49: Invalid scroll size"));
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN అక్షరం e_too_many_z[]
	INIT(= N_("E50: Too many \\z("));
#అయితే_ముగింపు
EXTERN అక్షరం e_too_many_str_open[]
	INIT(= N_("E51: Too many %s("));
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN అక్షరం e_unmatched_z[]
	INIT(= N_("E52: Unmatched \\z("));
#అయితే_ముగింపు
EXTERN అక్షరం e_unmatched_str_percent_open[]
	INIT(= N_("E53: Unmatched %s%%("));
EXTERN అక్షరం e_unmatched_str_open[]
	INIT(= N_("E54: Unmatched %s("));
EXTERN అక్షరం e_unmatched_str_close[]
	INIT(= N_("E55: Unmatched %s)"));
// E56 unused
// E57 unused
// E58 unused
EXTERN అక్షరం e_invalid_character_after_str_at[]
	INIT(= N_("E59: Invalid character after %s@"));
EXTERN అక్షరం e_too_many_complex_str_curly[]
	INIT(= N_("E60: Too many complex %s{...}s"));
EXTERN అక్షరం e_nested_str[]
	INIT(= N_("E61: Nested %s*"));
EXTERN అక్షరం e_nested_str_chr[]
	INIT(= N_("E62: Nested %s%c"));
EXTERN అక్షరం e_invalid_use_యొక్క_underscore[]
	INIT(= N_("E63: Invalid use of \\_"));
EXTERN అక్షరం e_str_chr_follows_nothing[]
	INIT(= N_("E64: %s%c follows nothing"));
EXTERN అక్షరం e_illegal_వెనుక_reference[]
	INIT(= N_("E65: Illegal back reference"));
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN అక్షరం e_z_not_allowed_ఇక్కడ[]
	INIT(= N_("E66: \\z( not allowed here"));
EXTERN అక్షరం e_z1_z9_not_allowed_ఇక్కడ[]
	INIT(= N_("E67: \\z1 - \\z9 not allowed here"));
#అయితే_ముగింపు
EXTERN అక్షరం e_invalid_character_after_bsl_z[]
	INIT(= N_("E68: Invalid character after \\z"));
EXTERN అక్షరం e_missing_sb_after_str[]
	INIT(= N_("E69: Missing ] after %s%%["));
EXTERN అక్షరం e_ఖాళీలు_str_brackets[]
	INIT(= N_("E70: Empty %s%%[]"));
EXTERN అక్షరం e_invalid_character_after_str[]
	INIT(= N_("E71: Invalid character after %s%%"));
EXTERN అక్షరం e_close_error_on_మార్పిడి_file[]
	INIT(= N_("E72: Close error on swap file"));
EXTERN అక్షరం e_ట్యాగ్_దొంతర_ఖాళీలు[]
	INIT(= N_("E73: Tag stack empty"));
EXTERN అక్షరం e_command_too_complex[]
	INIT(= N_("E74: Command too complex"));
EXTERN అక్షరం e_name_too_long[]
	INIT(= N_("E75: Name too long"));
EXTERN అక్షరం e_too_many_brackets[]
	INIT(= N_("E76: Too many ["));
EXTERN అక్షరం e_too_many_file_పేర్లు[]
	INIT(= N_("E77: Too many file names"));
EXTERN అక్షరం e_unknown_mark[]
	INIT(= N_("E78: Unknown mark"));
EXTERN అక్షరం e_cannot_expand_wildcards[]
	INIT(= N_("E79: Cannot expand wildcards"));
EXTERN అక్షరం e_error_while_writing[]
	INIT(= N_("E80: Error while writing"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_using_sid_not_లోపల_script_context[]
	INIT(= N_("E81: Using <SID> not in a script context"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_allocate_any_బఫరు_exiting[]
	INIT(= N_("E82: Cannot allocate any buffer, exiting..."));
EXTERN అక్షరం e_cannot_allocate_బఫరు_using_other_one[]
	INIT(= N_("E83: Cannot allocate buffer, using other one..."));
EXTERN అక్షరం e_no_modified_బఫరు_దొరికింది[]
	INIT(= N_("E84: No modified buffer found"));
EXTERN అక్షరం e_there_is_no_listed_బఫరు[]
	INIT(= N_("E85: There is no listed buffer"));
EXTERN అక్షరం e_బఫరు_nr_does_not_exist[]
	INIT(= N_("E86: Buffer %ld does not exist"));
EXTERN అక్షరం e_cannot_go_beyond_చివరి_బఫరు[]
	INIT(= N_("E87: Cannot go beyond last buffer"));
EXTERN అక్షరం e_cannot_go_before_మొదటి_బఫరు[]
	INIT(= N_("E88: Cannot go before first buffer"));
EXTERN అక్షరం e_no_write_since_చివరి_change_for_బఫరు_nr_కూడు_bang_గమ్యం_override[]
	INIT(= N_("E89: No write since last change for buffer %d (add ! to override)"));
EXTERN అక్షరం e_cannot_unload_చివరి_బఫరు[]
	INIT(= N_("E90: Cannot unload last buffer"));
EXTERN అక్షరం e_shell_option_is_ఖాళీలు[]
	INIT(= N_("E91: 'shell' option is empty"));
EXTERN అక్షరం e_బఫరు_nr_not_దొరికింది[]
	INIT(= N_("E92: Buffer %d not found"));
EXTERN అక్షరం e_మరిన్ని_than_one_సరిపోలిక_for_str[]
	INIT(= N_("E93: More than one match for %s"));
EXTERN అక్షరం e_no_matching_బఫరు_for_str[]
	INIT(= N_("E94: No matching buffer for %s"));
EXTERN అక్షరం e_బఫరు_with_this_name_already_exists[]
	INIT(= N_("E95: Buffer with this name already exists"));
#ఒకవేళ defined(FEAT_DIFF)
EXTERN అక్షరం e_cannot_diff_మరిన్ని_than_nr_buffers[]
	INIT(= N_("E96: Cannot diff more than %d buffers"));
EXTERN అక్షరం e_cannot_create_diffs[]
	INIT(= N_("E97: Cannot create diffs"));
EXTERN అక్షరం e_cannot_read_diff_output[]
	INIT(= N_("E98: Cannot read diff output"));
EXTERN అక్షరం e_current_బఫరు_is_not_లోపల_diff_బహుళకం[]
	INIT(= N_("E99: Current buffer is not in diff mode"));
EXTERN అక్షరం e_no_other_బఫరు_లోపల_diff_బహుళకం[]
	INIT(= N_("E100: No other buffer in diff mode"));
EXTERN అక్షరం e_మరిన్ని_than_two_buffers_లోపల_diff_బహుళకం_dont_know_which_one_గమ్యం_use[]
	INIT(= N_("E101: More than two buffers in diff mode, don't know which one to use"));
EXTERN అక్షరం e_cant_కనుగొను_బఫరు_str[]
	INIT(= N_("E102: Can't find buffer \"%s\""));
EXTERN అక్షరం e_బఫరు_str_is_not_లోపల_diff_బహుళకం[]
	INIT(= N_("E103: Buffer \"%s\" is not in diff mode"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIGRAPHS
EXTERN అక్షరం e_escape_not_allowed_లోపల_digraph[]
	INIT(= N_("E104: Escape not allowed in digraph"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_KEYMAP
EXTERN అక్షరం e_using_loadkeymap_not_లోపల_sourced_file[]
	INIT(= N_("E105: Using :loadkeymap not in a sourced file"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_unsupported_diff_output_format_str[]
	INIT(= N_("E106: Unsupported diff output format: %s"));
EXTERN అక్షరం e_missing_parenthesis_str[]
	INIT(= N_("E107: Missing parentheses: %s"));
EXTERN అక్షరం e_no_such_variable_str[]
	INIT(= N_("E108: No such variable: \"%s\""));
EXTERN అక్షరం e_missing_colon_after_questionmark[]
	INIT(= N_("E109: Missing ':' after '?'"));
EXTERN అక్షరం e_missing_closing_paren[]
	INIT(= N_("E110: Missing ')'"));
EXTERN అక్షరం e_missing_closing_వర్గం_brace[]
	INIT(= N_("E111: Missing ']'"));
EXTERN అక్షరం e_option_name_missing_str[]
	INIT(= N_("E112: Option name missing: %s"));
EXTERN అక్షరం e_unknown_option_str[]
	INIT(= N_("E113: Unknown option: %s"));
EXTERN అక్షరం e_missing_double_quote_str[]
	INIT(= N_("E114: Missing double quote: %s"));
EXTERN అక్షరం e_missing_single_quote_str[]
	INIT(= N_("E115: Missing single quote: %s"));
EXTERN అక్షరం e_invalid_arguments_for_function_str[]
	INIT(= N_("E116: Invalid arguments for function %s"));
EXTERN అక్షరం e_unknown_function_str[]
	INIT(= N_("E117: Unknown function: %s"));
EXTERN అక్షరం e_too_many_arguments_for_function_str[]
	INIT(= N_("E118: Too many arguments for function: %s"));
EXTERN అక్షరం e_not_enough_arguments_for_function_str[]
	INIT(= N_("E119: Not enough arguments for function: %s"));
EXTERN అక్షరం e_using_sid_not_లోపల_script_context_str[]
	INIT(= N_("E120: Using <SID> not in a script context: %s"));
EXTERN అక్షరం e_undefined_variable_str[]
	INIT(= N_("E121: Undefined variable: %s"));
EXTERN అక్షరం e_undefined_variable_char_str[]
	INIT(= N_("E121: Undefined variable: %c:%s"));
EXTERN అక్షరం e_function_str_already_exists_కూడు_bang_గమ్యం_replace[]
	INIT(= N_("E122: Function %s already exists, add ! to replace it"));
EXTERN అక్షరం e_undefined_function_str[]
	INIT(= N_("E123: Undefined function: %s"));
EXTERN అక్షరం e_missing_paren_str[]
	INIT(= N_("E124: Missing '(': %s"));
EXTERN అక్షరం e_illegal_వాదం_str[]
	INIT(= N_("E125: Illegal argument: %s"));
EXTERN అక్షరం e_missing_endfunction[]
	INIT(= N_("E126: Missing :endfunction"));
EXTERN అక్షరం e_cannot_redefine_function_str_it_is_లోపల_use[]
	INIT(= N_("E127: Cannot redefine function %s: It is in use"));
EXTERN అక్షరం e_function_name_must_ప్రారంభం_with_capital_or_s_str[]
	INIT(= N_("E128: Function name must start with a capital or \"s:\": %s"));
EXTERN అక్షరం e_function_name_required[]
	INIT(= N_("E129: Function name required"));
// E130 unused
EXTERN అక్షరం e_cannot_delete_function_str_it_is_లోపల_use[]
	INIT(= N_("E131: Cannot delete function %s: It is in use"));
EXTERN అక్షరం e_function_call_లోతు_is_higher_than_maxfuncdepth[]
	INIT(= N_("E132: Function call depth is higher than 'maxfuncdepth'"));
EXTERN అక్షరం e_return_not_inside_function[]
	INIT(= N_("E133: :return not inside a function"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_move_range_యొక్క_పంక్తులు_into_itself[]
	INIT(= N_("E134: Cannot move a range of lines into itself"));
EXTERN అక్షరం e_filter_autocommands_must_not_change_current_బఫరు[]
	INIT(= N_("E135: *Filter* Autocommands must not change current buffer"));
#ఒకవేళ defined(FEAT_VIMINFO)
EXTERN అక్షరం e_viminfo_too_many_errors_skipping_rest_యొక్క_file[]
	INIT(= N_("E136: viminfo: Too many errors, skipping rest of file"));
EXTERN అక్షరం e_viminfo_file_is_not_writable_str[]
	INIT(= N_("E137: Viminfo file is not writable: %s"));
EXTERN అక్షరం e_cant_write_viminfo_file_str[]
	INIT(= N_("E138: Can't write viminfo file %s!"));
#అయితే_ముగింపు
EXTERN అక్షరం e_file_is_loaded_లోపల_another_బఫరు[]
	INIT(= N_("E139: File is loaded in another buffer"));
EXTERN అక్షరం e_use_bang_గమ్యం_write_partial_బఫరు[]
	INIT(= N_("E140: Use ! to write partial buffer"));
EXTERN అక్షరం e_no_file_name_for_బఫరు_nr[]
	INIT(= N_("E141: No file name for buffer %ld"));
EXTERN అక్షరం e_file_not_written_writing_is_disabled_by_write_option[]
	INIT(= N_("E142: File not written: Writing is disabled by 'write' option"));
EXTERN అక్షరం e_autocommands_unexpectedly_deleted_new_బఫరు_str[]
	INIT(= N_("E143: Autocommands unexpectedly deleted new buffer %s"));
EXTERN అక్షరం e_non_numeric_వాదం_గమ్యం_z[]
	INIT(= N_("E144: Non-numeric argument to :z"));
EXTERN అక్షరం e_shell_commands_and_some_functionality_not_allowed_లోపల_rvim[]
	INIT(= N_("E145: Shell commands and some functionality not allowed in rvim"));
EXTERN అక్షరం e_regular_expressions_cant_be_delimited_by_letters[]
	INIT(= N_("E146: Regular expressions can't be delimited by letters"));
EXTERN అక్షరం e_cannot_do_global_recursive_with_range[]
	INIT(= N_("E147: Cannot do :global recursive with a range"));
EXTERN అక్షరం e_regular_వ్యక్తీకరణ_missing_from_global[]
	INIT(= N_("E148: Regular expression missing from :global"));
EXTERN అక్షరం e_sorry_no_help_for_str[]
	INIT(= N_("E149: Sorry, no help for %s"));
EXTERN అక్షరం e_not_a_directory_str[]
	INIT(= N_("E150: Not a directory: %s"));
EXTERN అక్షరం e_no_సరిపోలిక_str_1[]
	INIT(= N_("E151: No match: %s"));
EXTERN అక్షరం e_cannot_open_str_for_writing_1[]
	INIT(= N_("E152: Cannot open %s for writing"));
EXTERN అక్షరం e_unable_గమ్యం_open_str_for_reading[]
	INIT(= N_("E153: Unable to open %s for reading"));
EXTERN అక్షరం e_duplicate_ట్యాగ్_str_లోపల_file_str_str[]
	INIT(= N_("E154: Duplicate tag \"%s\" in file %s/%s"));
#నిర్వచితమైతే FEAT_SIGNS
EXTERN అక్షరం e_unknown_sign_str[]
	INIT(= N_("E155: Unknown sign: %s"));
EXTERN అక్షరం e_missing_sign_name[]
	INIT(= N_("E156: Missing sign name"));
EXTERN అక్షరం e_invalid_sign_id_nr[]
	INIT(= N_("E157: Invalid sign ID: %d"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_SIGNS) || defined(FEAT_EVAL)
EXTERN అక్షరం e_invalid_బఫరు_name_str[]
	INIT(= N_("E158: Invalid buffer name: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
EXTERN అక్షరం e_missing_sign_సంఖ్య[]
	INIT(= N_("E159: Missing sign number"));
EXTERN అక్షరం e_unknown_sign_command_str[]
	INIT(= N_("E160: Unknown sign command: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_breakpoint_not_దొరికింది_str[]
	INIT(= N_("E161: Breakpoint not found: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_no_write_since_చివరి_change_for_బఫరు_str[]
	INIT(= N_("E162: No write since last change for buffer \"%s\""));
EXTERN అక్షరం e_there_is_only_one_file_గమ్యం_edit[]
	INIT(= N_("E163: There is only one file to edit"));
EXTERN అక్షరం e_cannot_go_before_మొదటి_file[]
	INIT(= N_("E164: Cannot go before first file"));
EXTERN అక్షరం e_cannot_go_beyond_చివరి_file[]
	INIT(= N_("E165: Cannot go beyond last file"));
EXTERN అక్షరం e_cant_open_linked_file_for_writing[]
	INIT(= N_("E166: Can't open linked file for writing"));
EXTERN అక్షరం e_scriptencoding_వాడిన_outside_యొక్క_sourced_file[]
	INIT(= N_("E167: :scriptencoding used outside of a sourced file"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_ముగింపు_వాడిన_outside_యొక్క_sourced_file[]
	INIT(= N_("E168: :finish used outside of a sourced file"));
#అయితే_ముగింపు
EXTERN అక్షరం e_command_too_recursive[]
	INIT(= N_("E169: Command too recursive"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_missing_endwhile[]
	INIT(= N_("E170: Missing :endwhile"));
EXTERN అక్షరం e_missing_endfor[]
	INIT(= N_("E170: Missing :endfor"));
EXTERN అక్షరం e_missing_endif[]
	INIT(= N_("E171: Missing :endif"));
EXTERN అక్షరం e_missing_marker[]
	INIT(= N_("E172: Missing marker"));
#అయితే_ముగింపు

PLURAL_సందేశం(e_nr_మరిన్ని_file_గమ్యం_edit, "E173: %d more file to edit",
		e_nr_మరిన్ని_files_గమ్యం_edit, "E173: %d more files to edit")

EXTERN అక్షరం e_command_already_exists_కూడు_bang_గమ్యం_replace_it_str[]
	INIT(= N_("E174: Command already exists: add ! to replace it: %s"));
EXTERN అక్షరం e_no_attribute_specified[]
	INIT(= N_("E175: No attribute specified"));
EXTERN అక్షరం e_invalid_సంఖ్య_యొక్క_arguments[]
	INIT(= N_("E176: Invalid number of arguments"));
EXTERN అక్షరం e_లెక్క_cannot_be_specified_twice[]
	INIT(= N_("E177: Count cannot be specified twice"));
EXTERN అక్షరం e_invalid_default_విలువ_for_లెక్క[]
	INIT(= N_("E178: Invalid default value for count"));
EXTERN అక్షరం e_వాదం_required_for_str[]
	INIT(= N_("E179: Argument required for %s"));
EXTERN అక్షరం e_invalid_complete_విలువ_str[]
	INIT(= N_("E180: Invalid complete value: %s"));
EXTERN అక్షరం e_invalid_చిరునామా_type_విలువ_str[]
	INIT(= N_("E180: Invalid address type value: %s"));
EXTERN అక్షరం e_invalid_attribute_str[]
	INIT(= N_("E181: Invalid attribute: %s"));
EXTERN అక్షరం e_invalid_command_name[]
	INIT(= N_("E182: Invalid command name"));
EXTERN అక్షరం e_user_defined_commands_must_ప్రారంభం_with_an_uppercase_letter[]
	INIT(= N_("E183: User defined commands must start with an uppercase letter"));
EXTERN అక్షరం e_no_such_user_defined_command_str[]
	INIT(= N_("E184: No such user-defined command: %s"));
EXTERN అక్షరం e_cannot_కనుగొను_color_scheme_str[]
	INIT(= N_("E185: Cannot find color scheme '%s'"));
EXTERN అక్షరం e_no_మునుపటి_directory[]
	INIT(= N_("E186: No previous directory"));
EXTERN అక్షరం e_directory_unknown[]
	INIT(= N_("E187: Directory unknown"));
EXTERN అక్షరం e_obtaining_కిటికీ_position_not_implemented_for_this_platform[]
	INIT(= N_("E188: Obtaining window position not implemented for this platform"));
EXTERN అక్షరం e_str_exists_కూడు_bang_గమ్యం_override[]
	INIT(= N_("E189: \"%s\" exists (add ! to override)"));
EXTERN అక్షరం e_cannot_open_str_for_writing_2[]
	INIT(= N_("E190: Cannot open \"%s\" for writing"));
EXTERN అక్షరం e_వాదం_must_be_letter_or_forward_backward_quote[]
	INIT(= N_("E191: Argument must be a letter or forward/backward quote"));
EXTERN అక్షరం e_recursive_use_యొక్క_సాధారణ_too_deep[]
	INIT(= N_("E192: Recursive use of :normal too deep"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_str_not_inside_function[]
	INIT(= N_("E193: %s not inside a function"));
#అయితే_ముగింపు
EXTERN అక్షరం e_no_alternate_file_name_గమ్యం_substitute_for_hash[]
	INIT(= N_("E194: No alternate file name to substitute for '#'"));
#నిర్వచితమైతే FEAT_VIMINFO
EXTERN అక్షరం e_cannot_open_viminfo_file_for_reading[]
	INIT(= N_("E195: Cannot open viminfo file for reading"));
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_DIGRAPHS
EXTERN అక్షరం e_no_digraphs_సంస్కరణ[]
	INIT(= N_("E196: No digraphs in this version"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_సమితి_language_గమ్యం_str[]
	INIT(= N_("E197: Cannot set language to \"%s\""));
// E198 unused
EXTERN అక్షరం e_చురుకైన_కిటికీ_or_బఫరు_మారింది_or_deleted[]
	INIT(= N_("E199: Active window or buffer changed or deleted"));
EXTERN అక్షరం e_readpre_autocommands_made_file_unreadable[]
	INIT(= N_("E200: *ReadPre autocommands made the file unreadable"));
EXTERN అక్షరం e_readpre_autocommands_must_not_change_current_బఫరు[]
	INIT(= N_("E201: *ReadPre autocommands must not change current buffer"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_conversion_mad_file_unreadable[]
	INIT(= N_("E202: Conversion made file unreadable!"));
#అయితే_ముగింపు
EXTERN అక్షరం e_autocommands_deleted_or_unloaded_బఫరు_గమ్యం_be_written[]
	INIT(= N_("E203: Autocommands deleted or unloaded buffer to be written"));
EXTERN అక్షరం e_autocommands_మారింది_సంఖ్య_యొక్క_పంక్తులు_లోపల_unexpected_way[]
	INIT(= N_("E204: Autocommand changed number of lines in unexpected way"));
EXTERN అక్షరం e_patchmode_cant_save_original_file[]
	INIT(= N_("E205: Patchmode: can't save original file"));
EXTERN అక్షరం e_patchmode_cant_touch_ఖాళీలు_original_file[]
	INIT(= N_("E206: Patchmode: can't touch empty original file"));
EXTERN అక్షరం e_cant_delete_backup_file[]
	INIT(= N_("E207: Can't delete backup file"));
EXTERN అక్షరం e_error_writing_గమ్యం_str[]
	INIT(= N_("E208: Error writing to \"%s\""));
EXTERN అక్షరం e_error_closing_str[]
	INIT(= N_("E209: Error closing \"%s\""));
EXTERN అక్షరం e_error_reading_str[]
	INIT(= N_("E210: Error reading \"%s\""));
EXTERN అక్షరం e_file_str_no_longer_అందుబాటు[]
	INIT(= N_("E211: File \"%s\" no longer available"));
EXTERN అక్షరం e_cant_open_file_for_writing[]
	INIT(= N_("E212: Can't open file for writing"));
EXTERN అక్షరం e_cannot_convert_కూడు_bang_గమ్యం_write_without_conversion[]
	INIT(= N_("E213: Cannot convert (add ! to write without conversion)"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cant_కనుగొను_తాత్కాలిక_file_for_writing[]
	INIT(= N_("E214: Can't find temp file for writing"));
#అయితే_ముగింపు
EXTERN అక్షరం e_illegal_character_after_నక్షత్రం_str[]
	INIT(= N_("E215: Illegal character after *: %s"));
EXTERN అక్షరం e_no_such_event_str[]
	INIT(= N_("E216: No such event: %s"));
EXTERN అక్షరం e_no_such_సమూహం_or_event_str[]
	INIT(= N_("E216: No such group or event: %s"));
EXTERN అక్షరం e_cant_execute_autocommands_for_all_events[]
	INIT(= N_("E217: Can't execute autocommands for ALL events"));
EXTERN అక్షరం e_autocommand_nesting_too_deep[]
	INIT(= N_("E218: Autocommand nesting too deep"));
EXTERN అక్షరం e_missing_open_curly[]
	INIT(= N_("E219: Missing {."));
EXTERN అక్షరం e_missing_close_curly[]
	INIT(= N_("E220: Missing }."));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_marker_cannot_ప్రారంభం_with_క్రింది_case_letter[]
	INIT(= N_("E221: Marker cannot start with lower case letter"));
#అయితే_ముగింపు
EXTERN అక్షరం e_కూడు_గమ్యం_అంతర్గత_బఫరు_that_was_already_read_from[]
	INIT(= "E222: Add to internal buffer that was already read from");
EXTERN అక్షరం e_recursive_mapping[]
	INIT(= N_("E223: Recursive mapping"));
EXTERN అక్షరం e_global_abbreviation_already_exists_for_str[]
	INIT(= N_("E224: Global abbreviation already exists for %s"));
EXTERN అక్షరం e_global_mapping_already_exists_for_str[]
	INIT(= N_("E225: Global mapping already exists for %s"));
EXTERN అక్షరం e_abbreviation_already_exists_for_str[]
	INIT(= N_("E226: Abbreviation already exists for %s"));
EXTERN అక్షరం e_mapping_already_exists_for_str[]
	INIT(= N_("E227: Mapping already exists for %s"));
EXTERN అక్షరం e_makemap_illegal_బహుళకం[]
	INIT(= "E228: makemap: Illegal mode");
#నిర్వచితమైతే FEAT_GUI
EXTERN అక్షరం e_cannot_ప్రారంభం_the_GUI[]
	INIT(= N_("E229: Cannot start the GUI"));
EXTERN అక్షరం e_cannot_read_from_str[]
	INIT(= N_("E230: Cannot read from \"%s\""));
EXTERN అక్షరం e_guifontwide_invalid[]
	INIT(= N_("E231: 'guifontwide' invalid"));
# ifdef FEAT_BEVAL_GUI
EXTERN అక్షరం e_cannot_create_ballooneval_with_both_సందేశం_and_callback[]
	INIT(= "E232: Cannot create BalloonEval with both message and callback");
# endif
# ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
EXTERN అక్షరం e_cannot_open_display[]
	INIT(= N_("E233: Cannot open display"));
# endif
# ఒకవేళ defined(FEAT_XFONTSET)
EXTERN అక్షరం e_unknown_fontset_str[]
	INIT(= N_("E234: Unknown fontset: %s"));
# endif
# ఒకవేళ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_PHOTON) || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
EXTERN అక్షరం e_unknown_font_str[]
	INIT(= N_("E235: Unknown font: %s"));
# endif
# ఒకవేళ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
EXTERN అక్షరం e_font_str_is_not_fixed_వెడల్పు[]
	INIT(= N_("E236: Font \"%s\" is not fixed-width"));
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(MSWIN) || defined(FEAT_PRINT_PANGO)
EXTERN అక్షరం e_printer_selection_failed[]
	INIT(= N_("E237: Printer selection failed"));
EXTERN అక్షరం e_print_error_str[]
	INIT(= N_("E238: Print error: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
EXTERN అక్షరం e_invalid_sign_పాఠ్యం_str[]
	INIT(= N_("E239: Invalid sign text: %s"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
EXTERN అక్షరం e_no_connection_గమ్యం_x_server[]
	INIT(= N_("E240: No connection to the X server"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIENTSERVER
EXTERN అక్షరం e_unable_గమ్యం_send_గమ్యం_str[]
	INIT(= N_("E241: Unable to send to %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cant_విభజించు_కిటికీ_while_closing_another[]
	INIT(= N_("E242: Can't split a window while closing another"));
#ఒకవేళ defined(FEAT_GUI_MSWIN) && !defined(FEAT_OLE)
EXTERN అక్షరం e_వాదం_not_supported_str_use_ole_సంస్కరణ[]
	INIT(= N_("E243: Argument not supported: \"-%s\"; Use the OLE version."));
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
EXTERN అక్షరం e_illegal_str_name_str_లోపల_font_name_str[]
	INIT(= N_("E244: Illegal %s name \"%s\" in font name \"%s\""));
EXTERN అక్షరం e_illegal_char_nr_లోపల_font_name_str[]
	INIT(= N_("E245: Illegal char '%c' in font name \"%s\""));
#అయితే_ముగింపు
EXTERN అక్షరం e_filechangedshell_autocommand_deleted_బఫరు[]
	INIT(= N_("E246: FileChangedShell autocommand deleted buffer"));
#నిర్వచితమైతే FEAT_CLIENTSERVER
EXTERN అక్షరం e_no_registered_server_named_str[]
	INIT(= N_("E247: No registered server named \"%s\""));
EXTERN అక్షరం e_failed_గమ్యం_send_command_గమ్యం_destination_కార్యక్రమం[]
	INIT(= N_("E248: Failed to send command to the destination program"));
#అయితే_ముగింపు
EXTERN అక్షరం e_కిటికీ_layout_మారింది_unexpectedly[]
	INIT(= N_("E249: Window layout changed unexpectedly"));
#నిర్వచితమైతే FEAT_XFONTSET
EXTERN అక్షరం e_fonts_for_the_following_charsets_are_missing_లోపల_fontset[]
	INIT(= N_("E250: Fonts for the following charsets are missing in fontset %s:"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIENTSERVER
EXTERN అక్షరం e_vim_instance_registry_property_is_badly_formed_deleted[]
	INIT(= N_("E251: VIM instance registry property is badly formed.  Deleted!"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_X11
EXTERN అక్షరం e_fontsent_name_str_font_str_is_not_fixed_వెడల్పు[]
	INIT(= N_("E252: Fontset name: %s - Font '%s' is not fixed-width"));
EXTERN అక్షరం e_fontset_name_str[]
	INIT(= N_("E253: Fontset name: %s"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
EXTERN అక్షరం e_cannot_allocate_color_str[]
	INIT(= N_("E254: Cannot allocate color %s"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_SIGN_ICONS) && !defined(FEAT_GUI_GTK)
EXTERN అక్షరం e_couldnt_read_లోపల_sign_దత్తాంశం[]
	INIT(= N_("E255: Couldn't read in sign data"));
#అయితే_ముగింపు
// E256 unused
#నిర్వచితమైతే FEAT_CSCOPE
EXTERN అక్షరం e_cstag_ట్యాగ్_not_founc[]
	INIT(= N_("E257: cstag: Tag not found"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIENTSERVER
EXTERN అక్షరం e_unable_గమ్యం_send_గమ్యం_client[]
	INIT(= N_("E258: Unable to send to client"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CSCOPE
EXTERN అక్షరం e_no_matches_దొరికింది_for_cscope_query_str_యొక్క_str[]
	INIT(= N_("E259: No matches found for cscope query %s of %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_missing_name_after_పద్ధతి[]
	INIT(= N_("E260: Missing name after ->"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CSCOPE
EXTERN అక్షరం e_cscope_connection_str_not_founc[]
	INIT(= N_("E261: Cscope connection %s not found"));
EXTERN అక్షరం e_error_reading_cscope_connection_nr[]
	INIT(= N_("E262: Error reading cscope connection %d"));
#అయితే_ముగింపు
#ఒకవేళ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
EXTERN అక్షరం e_sorry_this_command_is_disabled_python_library_could_not_be_దొరికింది[]
	INIT(= N_("E263: Sorry, this command is disabled, the Python library could not be loaded."));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN అక్షరం e_python_error_initialising_io_object[]
	INIT(= N_("E264: Python: Error initialising I/O objects"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RUBY
EXTERN అక్షరం e_dollar_must_be_an_instance_యొక్క_string[]
	INIT(= N_("E265: $_ must be an instance of String"));
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_RUBY
EXTERN అక్షరం e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded[]
	INIT(= N_("E266: Sorry, this command is disabled, the Ruby library could not be loaded."));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RUBY
EXTERN అక్షరం e_unexpected_return[]
	INIT(= N_("E267: Unexpected return"));
EXTERN అక్షరం e_unexpected_తదుపరి[]
	INIT(= N_("E268: Unexpected next"));
EXTERN అక్షరం e_unexpected_break[]
	INIT(= N_("E269: Unexpected break"));
EXTERN అక్షరం e_unexpected_redo[]
	INIT(= N_("E270: Unexpected redo"));
EXTERN అక్షరం e_retry_outside_యొక్క_rescue_clause[]
	INIT(= N_("E271: Retry outside of rescue clause"));
EXTERN అక్షరం e_unhandled_exception[]
	INIT(= N_("E272: Unhandled exception"));
EXTERN అక్షరం e_unknown_longjmp_స్థితి_nr[]
	INIT(= N_("E273: Unknown longjmp status %d"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_no_white_space_allowed_before_parenthesis[]
	INIT(= N_("E274: No white space allowed before parenthesis"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_cannot_కూడు_పాఠ్యం_property_గమ్యం_unloaded_బఫరు[]
	INIT(= N_("E275: Cannot add text property to unloaded buffer"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_use_function_as_పద్ధతి_str[]
	INIT(= N_("E276: Cannot use function as a method: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIENTSERVER
EXTERN అక్షరం e_unable_గమ్యం_read_server_జవాబు[]
	INIT(= N_("E277: Unable to read a server reply"));
#అయితే_ముగింపు
// E278 unused
#ఒకవేళ defined(FEAT_TERMINAL) && !defined(UNIX) && !defined(MSWIN)
EXTERN అక్షరం e_sorry_plusplusshell_not_supported_on_this_system[]
	INIT(= N_("E279: Sorry, ++shell is not supported on this system"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TCL
EXTERN అక్షరం e_tcl_fatal_error_reflist_corrupt_please_report_this[]
	INIT(= "E280: TCL FATAL ERROR: reflist corrupt!? Please report this to vim-dev@vim.org");
#అయితే_ముగింపు
// E281 unused
EXTERN అక్షరం e_cannot_read_from_str_2[]
	INIT(= N_("E282: Cannot read from \"%s\""));
EXTERN అక్షరం e_no_marks_matching_str[]
	INIT(= N_("E283: No marks matching \"%s\""));
#నిర్వచితమైతే FEAT_XIM
# ifndef FEAT_GUI_GTK
EXTERN అక్షరం e_cannot_సమితి_ic_విలువలు[]
	INIT(= N_("E284: Cannot set IC values"));
# endif
# ifdef FEAT_GUI_X11
EXTERN అక్షరం e_failed_గమ్యం_create_ఆదానం_context[]
	INIT(= N_("E285: Failed to create input context"));
EXTERN అక్షరం e_failed_గమ్యం_open_ఆదానం_పద్ధతి[]
	INIT(= N_("E286: Failed to open input method"));
EXTERN అక్షరం e_warning_could_not_సమితి_destroy_callback_గమ్యం_im[]
	INIT(= N_("E287: Warning: Could not set destroy callback to IM"));
EXTERN అక్షరం e_ఆదానం_పద్ధతి_doesnt_support_any_style[]
	INIT(= N_("E288: Input method doesn't support any style"));
EXTERN అక్షరం e_ఆదానం_పద్ధతి_doesnt_support_my_preedit_type[]
	INIT(= N_("E289: Input method doesn't support my preedit type"));
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_శోధన_EXTRA
EXTERN అక్షరం e_list_or_సంఖ్య_required[]
	INIT(= N_("E290: List or number required"));
#అయితే_ముగింపు
// E291 unused
EXTERN అక్షరం e_invalid_లెక్క_for_del_బైట్లు_nr[]
	INIT(= "E292: Invalid count for del_bytes(): %ld");
EXTERN అక్షరం e_ఖండం_was_not_locked[]
	INIT(= "E293: Block was not locked");
EXTERN అక్షరం e_seek_error_లోపల_మార్పిడి_file_read[]
	INIT(= N_("E294: Seek error in swap file read"));
EXTERN అక్షరం e_read_error_లోపల_మార్పిడి_file[]
	INIT(= N_("E295: Read error in swap file"));
EXTERN అక్షరం e_seek_error_లోపల_మార్పిడి_file_write[]
	INIT(= N_("E296: Seek error in swap file write"));
EXTERN అక్షరం e_write_error_లోపల_మార్పిడి_file[]
	INIT(= N_("E297: Write error in swap file"));
EXTERN అక్షరం e_didnt_get_ఖండం_nr_zero[]
	INIT(= "E298: Didn't get block nr 0?");
EXTERN అక్షరం e_didnt_get_ఖండం_nr_one[]
	INIT(= "E298: Didn't get block nr 1?");
EXTERN అక్షరం e_didnt_get_ఖండం_nr_two[]
	INIT(= "E298: Didn't get block nr 2?");
#నిర్వచితమైతే FEAT_PERL
EXTERN అక్షరం e_perl_evaluation_forbidden_లోపల_sandbox_without_సురక్షిత_module[]
	INIT(= N_("E299: Perl evaluation forbidden in sandbox without the Safe module"));
#అయితే_ముగింపు
EXTERN అక్షరం e_మార్పిడి_file_already_exists_symlink_attack[]
	INIT(= N_("E300: Swap file already exists (symlink attack?)"));
EXTERN అక్షరం e_oops_కోల్పోయిన_the_మార్పిడి_file[]
	INIT(= N_("E301: Oops, lost the swap file!!!"));
EXTERN అక్షరం e_could_not_rename_మార్పిడి_file[]
	INIT(= N_("E302: Could not rename swap file"));
EXTERN అక్షరం e_unable_గమ్యం_open_మార్పిడి_file_for_str_recovery_impossible[]
	INIT(= N_("E303: Unable to open swap file for \"%s\", recovery impossible"));
EXTERN అక్షరం e_ml_upd_block0_didnt_get_ఖండం_zero[]
	INIT(= "E304: ml_upd_block0(): Didn't get block 0??");
EXTERN అక్షరం e_no_మార్పిడి_file_దొరికింది_for_str[]
	INIT(= N_("E305: No swap file found for %s"));
EXTERN అక్షరం e_cannot_open_str[]
	INIT(= N_("E306: Cannot open %s"));
EXTERN అక్షరం e_str_does_not_look_like_vim_మార్పిడి_file[]
	INIT(= N_("E307: %s does not look like a Vim swap file"));
EXTERN అక్షరం e_warning_original_file_may_have_been_మారింది[]
	INIT(= N_("E308: Warning: Original file may have been changed"));
EXTERN అక్షరం e_unable_గమ్యం_read_ఖండం_one_from_str[]
	INIT(= N_("E309: Unable to read block 1 from %s"));
EXTERN అక్షరం e_ఖండం_one_id_wrong_str_not_swp_file[]
	INIT(= N_("E310: Block 1 ID wrong (%s not a .swp file?)"));
EXTERN అక్షరం e_recovery_interrupted[]
	INIT(= N_("E311: Recovery Interrupted"));
EXTERN అక్షరం e_errors_detected_while_recovering_look_for_పంక్తులు_starting_with_questions[]
	INIT(= N_("E312: Errors detected while recovering; look for lines starting with ???"));
EXTERN అక్షరం e_cannot_preserve_there_is_no_మార్పిడి_file[]
	INIT(= N_("E313: Cannot preserve, there is no swap file"));
EXTERN అక్షరం e_preserve_failed[]
	INIT(= N_("E314: Preserve failed"));
EXTERN అక్షరం e_ml_get_invalid_lnum_nr[]
	INIT(= "E315: ml_get: Invalid lnum: %ld");
EXTERN అక్షరం e_ml_get_cannot_కనుగొను_పంక్తి_nr_లోపల_బఫరు_nr_str[]
	INIT(= "E316: ml_get: Cannot find line %ld in buffer %d %s");
EXTERN అక్షరం e_సూచిక_ఖండం_id_wrong[]
	INIT(= "E317: Pointer block id wrong");
EXTERN అక్షరం e_సూచిక_ఖండం_id_wrong_two[]
	INIT(= "E317: Pointer block id wrong 2");
EXTERN అక్షరం e_సూచిక_ఖండం_id_wrong_three[]
	INIT(= "E317: Pointer block id wrong 3");
EXTERN అక్షరం e_సూచిక_ఖండం_id_wrong_four[]
	INIT(= "E317: Pointer block id wrong 4");
EXTERN అక్షరం e_updated_too_many_ఖండాలు[]
	INIT(= "E318: Updated too many blocks?");
EXTERN అక్షరం e_sorry_command_is_not_అందుబాటు_లోపల_this_సంస్కరణ[]
	INIT(= N_("E319: Sorry, the command is not available in this version"));
EXTERN అక్షరం e_cannot_కనుగొను_పంక్తి_nr[]
	INIT(= "E320: Cannot find line %ld");
EXTERN అక్షరం e_could_not_reload_str[]
	INIT(= N_("E321: Could not reload \"%s\""));
EXTERN అక్షరం e_పంక్తి_సంఖ్య_బహిర్గతం_యొక్క_range_nr_past_the_చివర[]
	INIT(= "E322: Line number out of range: %ld past the end");
EXTERN అక్షరం e_పంక్తి_లెక్క_wrong_లోపల_ఖండం_nr[]
	INIT(= "E323: Line count wrong in block %ld");
#ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
EXTERN అక్షరం e_cant_open_postscript_output_file[]
	INIT(= N_("E324: Can't open PostScript output file"));
#అయితే_ముగింపు
EXTERN అక్షరం e_attention[]
	INIT(= N_("E325: ATTENTION"));
EXTERN అక్షరం e_too_many_మార్పిడి_files_దొరికింది[]
	INIT(= N_("E326: Too many swap files found"));
#నిర్వచితమైతే FEAT_MENU
EXTERN char_u e_part_యొక్క_menu_అంశం_మార్గం_is_not_sub_menu[]
	INIT(= N_("E327: Part of menu-item path is not sub-menu"));
EXTERN అక్షరం e_menu_only_exists_లోపల_another_బహుళకం[]
	INIT(= N_("E328: Menu only exists in another mode"));
EXTERN char_u e_no_menu_str[]
	INIT(= N_("E329: No menu \"%s\""));
EXTERN అక్షరం e_menu_మార్గం_must_not_lead_గమ్యం_sub_menu[]
	INIT(= N_("E330: Menu path must not lead to a sub-menu"));
EXTERN అక్షరం e_must_not_కూడు_menu_వస్తువులు_directly_గమ్యం_menu_bar[]
	INIT(= N_("E331: Must not add menu items directly to menu bar"));
EXTERN అక్షరం e_separator_cannot_be_part_యొక్క_menu_మార్గం[]
	INIT(= N_("E332: Separator cannot be part of a menu path"));
EXTERN అక్షరం e_menu_మార్గం_must_lead_గమ్యం_menu_అంశం[]
	INIT(= N_("E333: Menu path must lead to a menu item"));
EXTERN అక్షరం e_menu_not_దొరికింది_str[]
	INIT(= N_("E334: Menu not found: %s"));
EXTERN అక్షరం e_menu_not_defined_for_str_బహుళకం[]
	INIT(= N_("E335: Menu not defined for %s mode"));
EXTERN అక్షరం e_menu_మార్గం_must_lead_గమ్యం_sub_menu[]
	INIT(= N_("E336: Menu path must lead to a sub-menu"));
EXTERN అక్షరం e_menu_not_దొరికింది_check_menu_పేర్లు[]
	INIT(= N_("E337: Menu not found - check menu names"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_BROWSE
EXTERN అక్షరం e_sorry_no_file_browser_లోపల_console_బహుళకం[]
	INIT(= N_("E338: Sorry, no file browser in console mode"));
#అయితే_ముగింపు
EXTERN అక్షరం e_నమూనా_too_long[]
	INIT(= N_("E339: Pattern too long"));
EXTERN అక్షరం e_అంతర్గత_error_please_report_a_bug[]
	INIT(= N_("E340: Internal error; if you can reproduce please report a bug"));
EXTERN అక్షరం e_అంతర్గత_error_lalloc_zero[]
	INIT(= "E341: Internal error: lalloc(0, )");
EXTERN అక్షరం e_బహిర్గతం_యొక్క_జ్ఞాపకం_allocating_nr_బైట్లు[]
	INIT(= N_("E342: Out of memory!  (allocating %lu bytes)"));
EXTERN అక్షరం e_invalid_మార్గం_సంఖ్య_must_be_at_చివర_యొక్క_మార్గం_or_be_followed_by_str[]
	INIT(= N_("E343: Invalid path: '**[number]' must be at the end of the path or be followed by '%s'."));
EXTERN అక్షరం e_cant_కనుగొను_directory_str_లోపల_cdpath[]
	INIT(= N_("E344: Can't find directory \"%s\" in cdpath"));
EXTERN అక్షరం e_cant_కనుగొను_file_str_లోపల_మార్గం[]
	INIT(= N_("E345: Can't find file \"%s\" in path"));
EXTERN అక్షరం e_no_మరిన్ని_directory_str_దొరికింది_లోపల_cdpath[]
	INIT(= N_("E346: No more directory \"%s\" found in cdpath"));
EXTERN అక్షరం e_no_మరిన్ని_file_str_దొరికింది_లోపల_మార్గం[]
	INIT(= N_("E347: No more file \"%s\" found in path"));
EXTERN అక్షరం e_no_string_under_cursor[]
	INIT(= N_("E348: No string under cursor"));
EXTERN అక్షరం e_no_identifier_under_cursor[]
	INIT(= N_("E349: No identifier under cursor"));
#నిర్వచితమైతే FEAT_FOLDING
EXTERN అక్షరం e_cannot_create_fold_with_current_foldmethod[]
	INIT(= N_("E350: Cannot create fold with current 'foldmethod'"));
EXTERN అక్షరం e_cannot_delete_fold_with_current_foldmethod[]
	INIT(= N_("E351: Cannot delete fold with current 'foldmethod'"));
EXTERN అక్షరం e_cannot_erase_folds_with_current_foldmethod[]
	INIT(= N_("E352: Cannot erase folds with current 'foldmethod'"));
#అయితే_ముగింపు
EXTERN అక్షరం e_nothing_లోపల_రిజిస్టరు_str[]
	INIT(= N_("E353: Nothing in register %s"));
EXTERN అక్షరం e_invalid_రిజిస్టరు_name_str[]
	INIT(= N_("E354: Invalid register name: '%s'"));
EXTERN అక్షరం e_unknown_option_str_2[]
	INIT(= N_("E355: Unknown option: %s"));
EXTERN అక్షరం e_get_varp_error[]
	INIT(= "E356: get_varp ERROR");
#నిర్వచితమైతే FEAT_LANGMAP
EXTERN అక్షరం e_langmap_matching_character_missing_for_str[]
	INIT(= N_("E357: 'langmap': Matching character missing for %s"));
EXTERN అక్షరం e_langmap_extra_characters_after_semicolon_str[]
	INIT(= N_("E358: 'langmap': Extra characters after semicolon: %s"));
#అయితే_ముగింపు
#ఒకవేళ defined(AMIGA) || defined(MACOS_X) || defined(MSWIN)  \
	|| defined(UNIX) || defined(VMS)
EXTERN అక్షరం e_screen_బహుళకం_setting_not_supported[]
	INIT(= N_("E359: Screen mode setting not supported"));
#అయితే_ముగింపు
#నిర్వచితమైతే AMIGA
EXTERN అక్షరం e_cannot_execute_shell_with_f_option[]
	INIT(= N_("E360: Cannot execute shell with -f option"));
#అయితే_ముగింపు
// E361 unused
#ఒకవేళ defined(FEAT_EVAL)
EXTERN అక్షరం e_using_boolean_విలువ_as_float[]
	INIT(= N_("E362: Using a boolean value as a Float"));
#అయితే_ముగింపు
EXTERN అక్షరం e_నమూనా_ఉపయోగిస్తుంది_మరిన్ని_జ్ఞాపకం_than_maxmempattern[]
	INIT(= N_("E363: Pattern uses more memory than 'maxmempattern'"));
#నిర్వచితమైతే FEAT_LIBCALL
EXTERN అక్షరం e_library_call_failed_for_str[]
	INIT(= N_("E364: Library call failed for \"%s()\""));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
EXTERN అక్షరం e_failed_గమ్యం_print_postscript_file[]
	INIT(= N_("E365: Failed to print PostScript file"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_not_allowed_గమ్యం_enter_popup_కిటికీ[]
	INIT(= N_("E366: Not allowed to enter a popup window"));
#అయితే_ముగింపు
EXTERN అక్షరం e_no_such_సమూహం_str[]
	INIT(= N_("E367: No such group: \"%s\""));
#నిర్వచితమైతే FEAT_LIBCALL
EXTERN అక్షరం e_got_sig_str_లోపల_libcall[]
	INIT(= N_("E368: Got SIG%s in libcall()"));
#అయితే_ముగింపు
EXTERN అక్షరం e_invalid_అంశం_లోపల_str_brackets[]
	INIT(= N_("E369: Invalid item in %s%%[]"));
#నిర్వచితమైతే USING_ఎక్కించు_LIBRARY
EXTERN అక్షరం e_could_not_ఎక్కించు_library_str_str[]
	INIT(= N_("E370: Could not load library %s: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_MSWIN
EXTERN అక్షరం e_command_not_దొరికింది[]
	INIT(= N_("E371: Command not found"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_too_many_chr_లోపల_format_string[]
	INIT(= N_("E372: Too many %%%c in format string"));
EXTERN అక్షరం e_unexpected_chr_లోపల_format_str[]
	INIT(= N_("E373: Unexpected %%%c in format string"));
EXTERN అక్షరం e_missing_rsb_లోపల_format_string[]
	INIT(= N_("E374: Missing ] in format string"));
EXTERN అక్షరం e_unsupported_chr_లోపల_format_string[]
	INIT(= N_("E375: Unsupported %%%c in format string"));
EXTERN అక్షరం e_invalid_chr_లోపల_format_string_ఉపసర్గ[]
	INIT(= N_("E376: Invalid %%%c in format string prefix"));
EXTERN అక్షరం e_invalid_chr_లోపల_format_string[]
	INIT(= N_("E377: Invalid %%%c in format string"));
EXTERN అక్షరం e_errorformat_కలిగియుంది_no_నమూనా[]
	INIT(= N_("E378: 'errorformat' contains no pattern"));
EXTERN అక్షరం e_missing_or_ఖాళీలు_directory_name[]
	INIT(= N_("E379: Missing or empty directory name"));
EXTERN అక్షరం e_at_bottom_యొక్క_quickfix_దొంతర[]
	INIT(= N_("E380: At bottom of quickfix stack"));
EXTERN అక్షరం e_at_శిఖరం_యొక్క_quickfix_దొంతర[]
	INIT(= N_("E381: At top of quickfix stack"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_write_buftype_option_is_సమితి[]
	INIT(= N_("E382: Cannot write, 'buftype' option is set"));
EXTERN అక్షరం e_invalid_శోధన_string_str[]
	INIT(= N_("E383: Invalid search string: %s"));
EXTERN అక్షరం e_శోధన_తగిలింది_శిఖరం_without_సరిపోలిక_for_str[]
	INIT(= N_("E384: Search hit TOP without match for: %s"));
EXTERN అక్షరం e_శోధన_తగిలింది_bottom_without_సరిపోలిక_for_str[]
	INIT(= N_("E385: Search hit BOTTOM without match for: %s"));
EXTERN అక్షరం e_expected_question_or_slash_after_semicolon[]
	INIT(= N_("E386: Expected '?' or '/'  after ';'"));
#నిర్వచితమైతే FEAT_కనుగొను_ID
EXTERN అక్షరం e_సరిపోలిక_is_on_current_పంక్తి[]
	INIT(= N_("E387: Match is on current line"));
EXTERN అక్షరం e_couldnt_కనుగొను_definition[]
	INIT(= N_("E388: Couldn't find definition"));
EXTERN అక్షరం e_couldnt_కనుగొను_నమూనా[]
	INIT(= N_("E389: Couldn't find pattern"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN అక్షరం e_illegal_వాదం_str_2[]
	INIT(= N_("E390: Illegal argument: %s"));
EXTERN అక్షరం e_no_such_syntax_cluster_str_1[]
	INIT(= N_("E391: No such syntax cluster: %s"));
EXTERN అక్షరం e_no_such_syntax_cluster_str_2[]
	INIT(= N_("E392: No such syntax cluster: %s"));
EXTERN అక్షరం e_groupthere_not_accepted_ఇక్కడ[]
	INIT(= N_("E393: group[t]here not accepted here"));
EXTERN అక్షరం e_didnt_కనుగొను_region_అంశం_for_str[]
	INIT(= N_("E394: Didn't find region item for %s"));
EXTERN అక్షరం e_కలిగియుంది_వాదం_not_accepted_ఇక్కడ[]
	INIT(= N_("E395: Contains argument not accepted here"));
// E396 unused
EXTERN అక్షరం e_filename_required[]
	INIT(= N_("E397: Filename required"));
EXTERN అక్షరం e_missing_equal_str[]
	INIT(= N_("E398: Missing '=': %s"));
EXTERN అక్షరం e_not_enough_arguments_syntax_region_str[]
	INIT(= N_("E399: Not enough arguments: syntax region %s"));
EXTERN అక్షరం e_no_cluster_specified[]
	INIT(= N_("E400: No cluster specified"));
EXTERN అక్షరం e_నమూనా_delimiter_not_దొరికింది_str[]
	INIT(= N_("E401: Pattern delimiter not found: %s"));
EXTERN అక్షరం e_garbage_after_నమూనా_str[]
	INIT(= N_("E402: Garbage after pattern: %s"));
EXTERN అక్షరం e_syntax_sync_పంక్తి_continuations_నమూనా_specified_twice[]
	INIT(= N_("E403: syntax sync: Line continuations pattern specified twice"));
EXTERN అక్షరం e_illegal_arguments_str[]
	INIT(= N_("E404: Illegal arguments: %s"));
EXTERN అక్షరం e_missing_equal_sign_str[]
	INIT(= N_("E405: Missing equal sign: %s"));
EXTERN అక్షరం e_ఖాళీలు_వాదం_str[]
	INIT(= N_("E406: Empty argument: %s"));
EXTERN అక్షరం e_str_not_allowed_ఇక్కడ[]
	INIT(= N_("E407: %s not allowed here"));
EXTERN అక్షరం e_str_must_be_మొదటి_లోపల_కలిగియుంది_list[]
	INIT(= N_("E408: %s must be first in contains list"));
EXTERN అక్షరం e_unknown_సమూహం_name_str[]
	INIT(= N_("E409: Unknown group name: %s"));
EXTERN అక్షరం e_invalid_syntax_subcommand_str[]
	INIT(= N_("E410: Invalid :syntax subcommand: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_highlight_సమూహం_name_not_దొరికింది_str[]
	INIT(= N_("E411: Highlight group not found: %s"));
EXTERN అక్షరం e_not_enough_arguments_highlight_link_str[]
	INIT(= N_("E412: Not enough arguments: \":highlight link %s\""));
EXTERN అక్షరం e_too_many_arguments_highlight_link_str[]
	INIT(= N_("E413: Too many arguments: \":highlight link %s\""));
EXTERN అక్షరం e_సమూహం_has_settings_highlight_link_ignored[]
	INIT(= N_("E414: Group has settings, highlight link ignored"));
EXTERN అక్షరం e_unexpected_equal_sign_str[]
	INIT(= N_("E415: Unexpected equal sign: %s"));
EXTERN అక్షరం e_missing_equal_sign_str_2[]
	INIT(= N_("E416: Missing equal sign: %s"));
EXTERN అక్షరం e_missing_వాదం_str[]
	INIT(= N_("E417: Missing argument: %s"));
EXTERN అక్షరం e_illegal_విలువ_str[]
	INIT(= N_("E418: Illegal value: %s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_im_a_teapot[]
	INIT(= N_("E418: I'm a teapot"));
#అయితే_ముగింపు
EXTERN అక్షరం e_fg_color_unknown[]
	INIT(= N_("E419: FG color unknown"));
EXTERN అక్షరం e_bg_color_unknown[]
	INIT(= N_("E420: BG color unknown"));
EXTERN అక్షరం e_color_name_or_సంఖ్య_not_recognized_str[]
	INIT(= N_("E421: Color name or number not recognized: %s"));
EXTERN అక్షరం e_terminal_సంకేతం_too_long_str[]
	INIT(= N_("E422: Terminal code too long: %s"));
EXTERN అక్షరం e_illegal_వాదం_str_3[]
	INIT(= N_("E423: Illegal argument: %s"));
EXTERN అక్షరం e_too_many_different_highlighting_లక్షణాలు_లోపల_use[]
	INIT(= N_("E424: Too many different highlighting attributes in use"));
EXTERN అక్షరం e_cannot_go_before_మొదటి_matching_ట్యాగ్[]
	INIT(= N_("E425: Cannot go before first matching tag"));
EXTERN అక్షరం e_ట్యాగ్_not_దొరికింది_str[]
	INIT(= N_("E426: Tag not found: %s"));
EXTERN అక్షరం e_there_is_only_one_matching_ట్యాగ్[]
	INIT(= N_("E427: There is only one matching tag"));
EXTERN అక్షరం e_cannot_go_beyond_చివరి_matching_ట్యాగ్[]
	INIT(= N_("E428: Cannot go beyond last matching tag"));
EXTERN అక్షరం e_file_str_does_not_exist[]
	INIT(= N_("E429: File \"%s\" does not exist"));
#నిర్వచితమైతే FEAT_EMACS_ట్యాగులు
EXTERN అక్షరం e_ట్యాగ్_file_మార్గం_truncated_for_str[]
	INIT(= N_("E430: Tag file path truncated for %s\n"));
#అయితే_ముగింపు
EXTERN అక్షరం e_format_error_లోపల_ట్యాగులు_file_str[]
	INIT(= N_("E431: Format error in tags file \"%s\""));
EXTERN అక్షరం e_ట్యాగులు_file_not_sorted_str[]
	INIT(= N_("E432: Tags file not sorted: %s"));
EXTERN అక్షరం e_no_ట్యాగులు_file[]
	INIT(= N_("E433: No tags file"));
EXTERN అక్షరం e_cannot_కనుగొను_ట్యాగ్_నమూనా[]
	INIT(= N_("E434: Can't find tag pattern"));
EXTERN అక్షరం e_couldnt_కనుగొను_ట్యాగ్_just_guessing[]
	INIT(= N_("E435: Couldn't find tag, just guessing!"));
EXTERN అక్షరం e_no_str_నమోదు_లోపల_termcap[]
	INIT(= N_("E436: No \"%s\" entry in termcap"));
EXTERN అక్షరం e_terminal_capability_cm_required[]
	INIT(= N_("E437: Terminal capability \"cm\" required"));
EXTERN అక్షరం e_u_undo_పంక్తి_numbers_wrong[]
	INIT(= "E438: u_undo: Line numbers wrong");
EXTERN అక్షరం e_undo_list_corrupt[]
	INIT(= "E439: Undo list corrupt");
EXTERN అక్షరం e_undo_పంక్తి_missing[]
	INIT(= "E440: Undo line missing");
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_there_is_no_preview_కిటికీ[]
	INIT(= N_("E441: There is no preview window"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cant_విభజించు_topleft_and_botright_at_the_same_సమయం[]
	INIT(= N_("E442: Can't split topleft and botright at the same time"));
EXTERN అక్షరం e_cannot_rotate_when_another_కిటికీ_is_విభజించు[]
	INIT(= N_("E443: Cannot rotate when another window is split"));
EXTERN అక్షరం e_cannot_close_చివరి_కిటికీ[]
	INIT(= N_("E444: Cannot close last window"));
EXTERN అక్షరం e_other_కిటికీ_కలిగియుంది_changes[]
	INIT(= N_("E445: Other window contains changes"));
EXTERN అక్షరం e_no_file_name_under_cursor[]
	INIT(= N_("E446: No file name under cursor"));
EXTERN అక్షరం e_cant_కనుగొను_file_str_లోపల_మార్గం_2[]
	INIT(= N_("E447: Can't find file \"%s\" in path"));
#నిర్వచితమైతే USING_ఎక్కించు_LIBRARY
EXTERN అక్షరం e_could_not_ఎక్కించు_library_function_str[]
	INIT(= N_("E448: Could not load library function %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIENTSERVER
EXTERN అక్షరం e_invalid_వ్యక్తీకరణ_received[]
	INIT(= N_("E449: Invalid expression received"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_బఫరు_సంఖ్య_పాఠ్యం_or_list_required[]
	INIT(= N_("E450: Buffer number, text or a list required"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_expected_కుడి_curly_str[]
	INIT(= N_("E451: Expected }: %s"));
EXTERN అక్షరం e_double_semicolon_లోపల_list_యొక్క_variables[]
	INIT(= N_("E452: Double ; in list of variables"));
#అయితే_ముగింపు
EXTERN అక్షరం e_ul_color_unknown[]
	INIT(= N_("E453: UL color unknown"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_function_list_was_modified[]
	INIT(= N_("E454: Function list was modified"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_POSTSCRIPT
EXTERN అక్షరం e_error_writing_గమ్యం_postscript_output_file[]
	INIT(= N_("E455: Error writing to PostScript output file"));
EXTERN అక్షరం e_cant_open_file_str_2[]
	INIT(= N_("E456: Can't open file \"%s\""));
EXTERN అక్షరం e_cant_కనుగొను_postscript_resource_file_str_ps[]
	INIT(= N_("E456: Can't find PostScript resource file \"%s.ps\""));
EXTERN అక్షరం e_cant_read_postscript_resource_file_str[]
	INIT(= N_("E457: Can't read PostScript resource file \"%s\""));
#అయితే_ముగింపు
// E458 unused
#ఒకవేళ defined(UNIX) || defined(FEAT_SESSION) || defined(FEAT_SOCKETSERVER)
EXTERN అక్షరం e_cannot_go_వెనుక_గమ్యం_మునుపటి_directory[]
	INIT(= N_("E459: Cannot go back to previous directory"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_entries_missing_లోపల_mapset_dict_వాదం[]
	INIT(= N_("E460: Entries missing in mapset() dict argument"));
EXTERN అక్షరం e_illegal_variable_name_str[]
	INIT(= N_("E461: Illegal variable name: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_could_not_prepare_for_reloading_str[]
	INIT(= N_("E462: Could not prepare for reloading \"%s\""));
#నిర్వచితమైతే FEAT_NETBEANS_INTG
EXTERN అక్షరం e_region_is_guarded_cannot_modify[]
	INIT(= N_("E463: Region is guarded, cannot modify"));
#అయితే_ముగింపు
EXTERN అక్షరం e_ambiguous_use_యొక్క_user_defined_command[]
	INIT(= N_("E464: Ambiguous use of user-defined command"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_ambiguous_use_యొక్క_user_defined_command_str[]
	INIT(= N_("E464: Ambiguous use of user-defined command: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_winsize_requires_two_సంఖ్య_arguments[]
	INIT(= N_("E465: :winsize requires two number arguments"));
EXTERN అక్షరం e_winpos_requires_two_సంఖ్య_arguments[]
	INIT(= N_("E466: :winpos requires two number arguments"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_custom_completion_requires_function_వాదం[]
	INIT(= N_("E467: Custom completion requires a function argument"));
#అయితే_ముగింపు
EXTERN అక్షరం e_completion_వాదం_only_allowed_for_custom_completion[]
	INIT(= N_("E468: Completion argument only allowed for custom completion"));
#నిర్వచితమైతే FEAT_CSCOPE
EXTERN అక్షరం e_invalid_cscopequickfix_flag_chr_for_chr[]
	INIT(= N_("E469: Invalid cscopequickfix flag %c for %c"));
#అయితే_ముగింపు
EXTERN అక్షరం e_command_aborted[]
	INIT(= N_("E470: Command aborted"));
EXTERN అక్షరం e_వాదం_required[]
	INIT(= N_("E471: Argument required"));
EXTERN అక్షరం e_command_failed[]
	INIT(= N_("E472: Command failed"));
EXTERN అక్షరం e_అంతర్గత_error_లోపల_regexp[]
	INIT(= "E473: Internal error in regexp");
EXTERN అక్షరం e_invalid_వాదం[]
	INIT(= N_("E474: Invalid argument"));
EXTERN అక్షరం e_invalid_వాదం_str[]
	INIT(= N_("E475: Invalid argument: %s"));
EXTERN అక్షరం e_invalid_విలువ_for_వాదం_str[]
	INIT(= N_("E475: Invalid value for argument %s"));
#ఒకవేళ defined(FEAT_పని_CHANNEL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL) || defined(FEAT_IMAGE)
EXTERN అక్షరం e_invalid_విలువ_for_వాదం_str_str[]
	INIT(= N_("E475: Invalid value for argument %s: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_invalid_command[]
	INIT(= N_("E476: Invalid command"));
EXTERN అక్షరం e_invalid_command_str[]
	INIT(= N_("E476: Invalid command: %s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_invalid_command_str_expected_str[]
	INIT(= N_("E476: Invalid command: %s, expected %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_no_bang_allowed[]
	INIT(= N_("E477: No ! allowed"));
EXTERN అక్షరం e_dont_panic[]
	INIT(= N_("E478: Don't panic!"));
EXTERN అక్షరం e_no_సరిపోలిక[]
	INIT(= N_("E479: No match"));
EXTERN అక్షరం e_no_సరిపోలిక_str_2[]
	INIT(= N_("E480: No match: %s"));
EXTERN అక్షరం e_no_range_allowed[]
	INIT(= N_("E481: No range allowed"));
EXTERN అక్షరం e_cant_create_file_str[]
	INIT(= N_("E482: Can't create file %s"));
EXTERN అక్షరం e_cant_get_తాత్కాలిక_file_name[]
	INIT(= N_("E483: Can't get temp file name"));
EXTERN అక్షరం e_cant_open_file_str[]
	INIT(= N_("E484: Can't open file %s"));
EXTERN అక్షరం e_cant_read_file_str[]
	INIT(= N_("E485: Can't read file %s"));
EXTERN అక్షరం e_నమూనా_not_దొరికింది[]
	INIT(= N_("E486: Pattern not found"));
EXTERN అక్షరం e_నమూనా_not_దొరికింది_str[]
	INIT(= N_("E486: Pattern not found: %s"));
EXTERN అక్షరం e_వాదం_must_be_ధనాత్మక[]
	INIT(= N_("E487: Argument must be positive"));
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_వాదం_must_be_ధనాత్మక_str[]
	INIT(= N_("E487: Argument must be positive: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_trailing_characters[]
	INIT(= N_("E488: Trailing characters"));
EXTERN అక్షరం e_trailing_characters_str[]
	INIT(= N_("E488: Trailing characters: %s"));
EXTERN అక్షరం e_no_call_దొంతర_గమ్యం_substitute_for_దొంతర[]
	INIT(= N_("E489: No call stack to substitute for \"<stack>\""));
#నిర్వచితమైతే FEAT_FOLDING
EXTERN అక్షరం e_no_fold_దొరికింది[]
	INIT(= N_("E490: No fold found"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_json_decode_error_at_str[]
	INIT(= N_("E491: JSON decode error at '%s'"));
#అయితే_ముగింపు
EXTERN అక్షరం e_not_an_editor_command[]
	INIT(= N_("E492: Not an editor command"));
EXTERN అక్షరం e_backwards_range_given[]
	INIT(= N_("E493: Backwards range given"));
EXTERN అక్షరం e_use_w_or_w_gt_gt[]
	INIT(= N_("E494: Use w or w>>"));
EXTERN అక్షరం e_no_autocommand_file_name_గమ్యం_substitute_for_afile[]
	INIT(= N_("E495: No autocommand file name to substitute for \"<afile>\""));
EXTERN అక్షరం e_no_autocommand_బఫరు_సంఖ్య_గమ్యం_substitute_for_abuf[]
	INIT(= N_("E496: No autocommand buffer number to substitute for \"<abuf>\""));
EXTERN అక్షరం e_no_autocommand_సరిపోలిక_name_గమ్యం_substitute_for_amatch[]
	INIT(= N_("E497: No autocommand match name to substitute for \"<amatch>\""));
EXTERN అక్షరం e_no_ఆధారం_file_name_గమ్యం_substitute_for_sfile[]
	INIT(= N_("E498: No :source file name to substitute for \"<sfile>\""));
EXTERN అక్షరం e_ఖాళీలు_file_name_for_percent_or_hash_only_పనిచేస్తుంది_with_ph[]
	// xgettext:no-c-format
	INIT(= N_("E499: Empty file name for '%' or '#', only works with \":p:h\""));
EXTERN అక్షరం e_evaluates_గమ్యం_an_ఖాళీలు_string[]
	INIT(= N_("E500: Evaluates to an empty string"));
EXTERN అక్షరం e_at_చివర_యొక్క_file[]
	INIT(= N_("E501: At end-of-file"));
	// E502
EXTERN అక్షరం e_is_a_directory[]
	INIT(= N_("is a directory"));
	// E503
EXTERN అక్షరం e_is_not_file_or_writable_device[]
	INIT(= N_("is not a file or writable device"));
EXTERN అక్షరం e_str_is_not_file_or_writable_device[]
	INIT(= N_("E503: \"%s\" is not a file or writable device"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_coffee_currently_not_అందుబాటు[]
	INIT(= N_("E503: Coffee is currently not available"));
#అయితే_ముగింపు
	// E504
EXTERN అక్షరం e_is_read_only_cannot_override_W_లోపల_cpoptions[]
	INIT(= N_("is read-only (cannot override: \"W\" in 'cpoptions')"));
	// E505
EXTERN అక్షరం e_is_read_only_కూడు_bang_గమ్యం_override[]
	INIT(= N_("is read-only (add ! to override)"));
EXTERN అక్షరం e_str_is_read_only_కూడు_bang_గమ్యం_override[]
	INIT(= N_("E505: \"%s\" is read-only (add ! to override)"));
EXTERN అక్షరం e_cant_write_గమ్యం_backup_file_కూడు_bang_గమ్యం_override[]
	INIT(= N_("E506: Can't write to backup file (add ! to override)"));
EXTERN అక్షరం e_close_error_for_backup_file_కూడు_bang_గమ్యం_write_anyway[]
	INIT(= N_("E507: Close error for backup file (add ! to write anyway)"));
EXTERN అక్షరం e_cant_read_file_for_backup_కూడు_bang_గమ్యం_write_anyway[]
	INIT(= N_("E508: Can't read file for backup (add ! to write anyway)"));
EXTERN అక్షరం e_cannot_create_backup_file_కూడు_bang_గమ్యం_write_anyway[]
	INIT(= N_("E509: Cannot create backup file (add ! to override)"));
EXTERN అక్షరం e_cant_make_backup_file_కూడు_bang_గమ్యం_write_anyway[]
	INIT(= N_("E510: Can't make backup file (add ! to write anyway)"));
#నిర్వచితమైతే FEAT_NETBEANS_INTG
EXTERN అక్షరం e_netbeans_already_connected[]
	INIT(= N_("E511: NetBeans already connected"));
#అయితే_ముగింపు
EXTERN అక్షరం e_close_failed[]
	INIT(= N_("E512: Close failed"));
EXTERN అక్షరం e_write_error_conversion_failed_make_fenc_ఖాళీలు_గమ్యం_override[]
	INIT(= N_("E513: Write error, conversion failed (make 'fenc' empty to override)"));
EXTERN అక్షరం e_write_error_conversion_failed_లోపల_పంక్తి_nr_make_fenc_ఖాళీలు_గమ్యం_override[]
	INIT(= N_("E513: Write error, conversion failed in line %ld (make 'fenc' empty to override)"));
EXTERN అక్షరం e_write_error_file_system_నిండిన[]
	INIT(= N_("E514: Write error (file system full?)"));
EXTERN అక్షరం e_no_buffers_were_unloaded[]
	INIT(= N_("E515: No buffers were unloaded"));
EXTERN అక్షరం e_no_buffers_were_deleted[]
	INIT(= N_("E516: No buffers were deleted"));
EXTERN అక్షరం e_no_buffers_were_wiped_బహిర్గతం[]
	INIT(= N_("E517: No buffers were wiped out"));
EXTERN అక్షరం e_unknown_option[]
	INIT(= N_("E518: Unknown option"));
EXTERN అక్షరం e_option_not_supported[]
	INIT(= N_("E519: Option not supported"));
EXTERN అక్షరం e_not_allowed_లోపల_modeline[]
	INIT(= N_("E520: Not allowed in a modeline"));
EXTERN అక్షరం e_సంఖ్య_required_after_equal[]
	INIT(= N_("E521: Number required after ="));
EXTERN అక్షరం e_సంఖ్య_required_after_str_equal_str[]
	INIT(= N_("E521: Number required: &%s = '%s'"));
EXTERN అక్షరం e_not_దొరికింది_లోపల_termcap[]
	INIT(= N_("E522: Not found in termcap"));
EXTERN అక్షరం e_not_allowed_ఇక్కడ[]
	INIT(= N_("E523: Not allowed here"));
EXTERN అక్షరం e_missing_colon[]
	INIT(= N_("E524: Missing colon"));
EXTERN అక్షరం e_zero_నిడివి_string[]
	INIT(= N_("E525: Zero length string"));
#నిర్వచితమైతే FEAT_VIMINFO
EXTERN అక్షరం e_missing_సంఖ్య_after_angle_str_angle[]
	INIT(= N_("E526: Missing number after <%s>"));
EXTERN అక్షరం e_missing_comma[]
	INIT(= N_("E527: Missing comma"));
EXTERN అక్షరం e_must_specify_a_విలువ[]
	INIT(= N_("E528: Must specify a ' value"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_సమితి_పదం_గమ్యం_ఖాళీలు_string[]
	INIT(= N_("E529: Cannot set 'term' to empty string"));
#నిర్వచితమైతే FEAT_GUI
EXTERN అక్షరం e_cannot_change_పదం_లోపల_GUI[]
	INIT(= N_("E530: Cannot change 'term' in the GUI"));
EXTERN అక్షరం e_use_gui_గమ్యం_ప్రారంభం_GUI[]
	INIT(= N_("E531: Use \":gui\" to start the GUI"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
EXTERN అక్షరం e_highlighting_color_name_too_long_లోపల_define_Anno_Type[]
	INIT(= N_("E532: Highlighting color name too long in defineAnnoType"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
EXTERN అక్షరం e_cant_select_wide_font[]
	INIT(= N_("E533: Can't select wide font"));
EXTERN అక్షరం e_invalid_wide_font[]
	INIT(= N_("E534: Invalid wide font"));
#అయితే_ముగింపు
EXTERN అక్షరం e_illegal_character_after_chr[]
	INIT(= N_("E535: Illegal character after <%c>"));
#నిర్వచితమైతే FEAT_FOLDING
EXTERN అక్షరం e_comma_required[]
	INIT(= N_("E536: Comma required"));
#అయితే_ముగింపు
EXTERN అక్షరం e_commentstring_must_be_ఖాళీలు_or_contain_str[]
	INIT(= N_("E537: 'commentstring' must be empty or contain %s"));
EXTERN అక్షరం e_నమూనా_దొరికింది_లోపల_every_పంక్తి_str[]
	INIT(= N_("E538: Pattern found in every line: %s"));
EXTERN అక్షరం e_illegal_character_str[]
	INIT(= N_("E539: Illegal character <%s>"));
#నిర్వచితమైతే FEAT_STL_OPT
EXTERN అక్షరం e_unclosed_వ్యక్తీకరణ_sequence[]
	INIT(= N_("E540: Unclosed expression sequence"));
// E541 unused
EXTERN అక్షరం e_unbalanced_groups[]
	INIT(= N_("E542: Unbalanced groups"));
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
EXTERN అక్షరం e_not_valid_codepage[]
	INIT(= N_("E543: Not a valid codepage"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_KEYMAP
EXTERN అక్షరం e_keymap_file_not_దొరికింది[]
	INIT(= N_("E544: Keymap file not found"));
#అయితే_ముగింపు
#నిర్వచితమైతే CURSOR_SHAPE
EXTERN అక్షరం e_missing_colon_2[]
	INIT(= N_("E545: Missing colon"));
EXTERN అక్షరం e_illegal_బహుళకం[]
	INIT(= N_("E546: Illegal mode"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSESHAPE
EXTERN అక్షరం e_illegal_mouseshape[]
	INIT(= N_("E547: Illegal mouseshape"));
#అయితే_ముగింపు
#నిర్వచితమైతే CURSOR_SHAPE
EXTERN అక్షరం e_digit_expected[]
	INIT(= N_("E548: Digit expected"));
EXTERN అక్షరం e_illegal_percentage[]
	INIT(= N_("E549: Illegal percentage"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PRINTER
EXTERN అక్షరం e_missing_colon_3[]
	INIT(= N_("E550: Missing colon"));
EXTERN అక్షరం e_illegal_component[]
	INIT(= N_("E551: Illegal component"));
EXTERN అక్షరం e_digit_expected_2[]
	INIT(= N_("E552: Digit expected"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_no_మరిన్ని_వస్తువులు[]
	INIT(= N_("E553: No more items"));
#అయితే_ముగింపు
EXTERN అక్షరం e_syntax_error_లోపల_str_curlies[]
	INIT(= N_("E554: Syntax error in %s{...}"));
EXTERN అక్షరం e_at_bottom_యొక్క_ట్యాగ్_దొంతర[]
	INIT(= N_("E555: At bottom of tag stack"));
EXTERN అక్షరం e_at_శిఖరం_యొక్క_ట్యాగ్_దొంతర[]
	INIT(= N_("E556: At top of tag stack"));
EXTERN అక్షరం e_cannot_open_termcap_file[]
	INIT(= N_("E557: Cannot open termcap file"));
EXTERN అక్షరం e_terminal_నమోదు_not_దొరికింది_లోపల_terminfo[]
	INIT(= N_("E558: Terminal entry not found in terminfo"));
#ఒకవేళ defined(HAVE_TGETENT) && !defined(TERMINFO)
EXTERN అక్షరం e_terminal_నమోదు_not_దొరికింది_లోపల_termcap[]
	INIT(= N_("E559: Terminal entry not found in termcap"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CSCOPE
EXTERN అక్షరం e_usage_cscope_str[]
	INIT(= N_("E560: Usage: cs[cope] %s"));
EXTERN అక్షరం e_unknown_cscope_శోధన_type[]
	INIT(= N_("E561: Unknown cscope search type"));
EXTERN అక్షరం e_usage_cstag_ident[]
	INIT(= N_("E562: Usage: cstag <ident>"));
EXTERN అక్షరం e_stat_str_error_nr[]
	INIT(= N_("E563: stat(%s) error: %d"));
EXTERN అక్షరం e_str_is_not_directory_or_valid_cscope_database[]
	INIT(= N_("E564: %s is not a directory or a valid cscope database"));
#అయితే_ముగింపు
EXTERN అక్షరం e_not_allowed_గమ్యం_change_పాఠ్యం_or_change_కిటికీ[]
	INIT(= N_("E565: Not allowed to change text or change window"));
#నిర్వచితమైతే FEAT_CSCOPE
EXTERN అక్షరం e_could_not_create_cscope_pipes[]
	INIT(= N_("E566: Could not create cscope pipes"));
EXTERN అక్షరం e_no_cscope_connections[]
	INIT(= N_("E567: No cscope connections"));
EXTERN అక్షరం e_duplicate_cscope_database_not_added[]
	INIT(= N_("E568: Duplicate cscope database not added"));
// E569 unused
EXTERN అక్షరం e_fatal_error_లోపల_cs_manage_matches[]
	INIT(= "E570: Fatal error in cs_manage_matches");
#అయితే_ముగింపు
#నిర్వచితమైతే DYNAMIC_TCL
EXTERN అక్షరం e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded[]
	INIT(= N_("E571: Sorry, this command is disabled: the Tcl library could not be loaded."));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TCL
EXTERN అక్షరం e_exit_సంకేతం_nr[]
	INIT(= N_("E572: Exit code %d"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIENTSERVER
EXTERN అక్షరం e_invalid_server_id_వాడిన_str[]
	INIT(= N_("E573: Invalid server id used: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_VIMINFO
EXTERN అక్షరం e_unknown_రిజిస్టరు_type_nr[]
	INIT(= N_("E574: Unknown register type %d"));
	// E575
EXTERN అక్షరం e_illegal_starting_char[]
	INIT(= N_("Illegal starting char"));
	// E576
EXTERN అక్షరం e_nonr_missing_gt[]
	INIT(= N_("Missing '>'"));
	// E577
EXTERN అక్షరం e_illegal_రిజిస్టరు_name[]
	INIT(= N_("Illegal register name"));
#అయితే_ముగింపు
// E578 unused
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_if_nesting_too_deep[]
	INIT(= N_("E579: :if nesting too deep"));
EXTERN అక్షరం e_ఖండం_nesting_too_deep[]
	INIT(= N_("E579: Block nesting too deep"));
EXTERN అక్షరం e_endif_without_if[]
	INIT(= N_("E580: :endif without :if"));
EXTERN అక్షరం e_else_without_if[]
	INIT(= N_("E581: :else without :if"));
EXTERN అక్షరం e_elseif_without_if[]
	INIT(= N_("E582: :elseif without :if"));
EXTERN అక్షరం e_గుణిజం_else[]
	INIT(= N_("E583: Multiple :else"));
EXTERN అక్షరం e_elseif_after_else[]
	INIT(= N_("E584: :elseif after :else"));
EXTERN అక్షరం e_while_for_nesting_too_deep[]
	INIT(= N_("E585: :while/:for nesting too deep"));
EXTERN అక్షరం e_continue_without_while_or_for[]
	INIT(= N_("E586: :continue without :while or :for"));
EXTERN అక్షరం e_break_without_while_or_for[]
	INIT(= N_("E587: :break without :while or :for"));
EXTERN అక్షరం e_endwhile_without_while[]
	INIT(= N_("E588: :endwhile without :while"));
EXTERN అక్షరం e_endfor_without_for[]
	INIT(= N_("E588: :endfor without :for"));
#అయితే_ముగింపు
EXTERN అక్షరం e_backupext_and_patchmode_are_equal[]
	INIT(= N_("E589: 'backupext' and 'patchmode' are equal"));
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_preview_కిటికీ_already_exists[]
	INIT(= N_("E590: A preview window already exists"));
#అయితే_ముగింపు
EXTERN అక్షరం e_winheight_cannot_be_smaller_than_winminheight[]
	INIT(= N_("E591: 'winheight' cannot be smaller than 'winminheight'"));
EXTERN అక్షరం e_winwidth_cannot_be_smaller_than_winminwidth[]
	INIT(= N_("E592: 'winwidth' cannot be smaller than 'winminwidth'"));
EXTERN అక్షరం e_need_at_కనిష్ఠ_nr_పంక్తులు[]
	INIT(= N_("E593: Need at least %d lines"));
EXTERN అక్షరం e_need_at_కనిష్ఠ_nr_columns[]
	INIT(= N_("E594: Need at least %d columns"));
#నిర్వచితమైతే FEAT_LINEBREAK
EXTERN అక్షరం e_showbreak_కలిగియుంది_unprintable_or_wide_character[]
	INIT(= N_("E595: 'showbreak' contains unprintable or wide character"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
EXTERN అక్షరం e_invalid_fonts[]
	INIT(= N_("E596: Invalid font(s)"));
# ifdef FEAT_XFONTSET
EXTERN అక్షరం e_cant_select_fontset[]
	INIT(= N_("E597: Can't select fontset"));
EXTERN అక్షరం e_invalid_fontset[]
	INIT(= N_("E598: Invalid fontset"));
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
EXTERN అక్షరం e_విలువ_యొక్క_imactivatekey_is_invalid[]
	INIT(= N_("E599: Value of 'imactivatekey' is invalid"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_missing_endtry[]
	INIT(= N_("E600: Missing :endtry"));
EXTERN అక్షరం e_try_nesting_too_deep[]
	INIT(= N_("E601: :try nesting too deep"));
EXTERN అక్షరం e_endtry_without_try[]
	INIT(= N_("E602: :endtry without :try"));
EXTERN అక్షరం e_catch_without_try[]
	INIT(= N_("E603: :catch without :try"));
EXTERN అక్షరం e_catch_after_finally[]
	INIT(= N_("E604: :catch after :finally"));
EXTERN అక్షరం e_exception_not_caught_str[]
	INIT(= N_("E605: Exception not caught: %s"));
EXTERN అక్షరం e_finally_without_try[]
	INIT(= N_("E606: :finally without :try"));
EXTERN అక్షరం e_గుణిజం_finally[]
	INIT(= N_("E607: Multiple :finally"));
EXTERN అక్షరం e_cannot_throw_exceptions_with_vim_ఉపసర్గ[]
	INIT(= N_("E608: Cannot :throw exceptions with 'Vim' prefix"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CSCOPE
EXTERN అక్షరం e_cscope_error_str[]
	INIT(= N_("E609: Cscope error: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_no_వాదం_గమ్యం_delete[]
	INIT(= N_("E610: No argument to delete"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_using_special_as_సంఖ్య[]
	INIT(= N_("E611: Using a Special as a Number"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
EXTERN అక్షరం e_too_many_signs_defined[]
	INIT(= N_("E612: Too many signs defined"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PRINTER) && (defined(MSWIN) || defined(FEAT_PRINT_PANGO))
EXTERN అక్షరం e_unknown_printer_font_str[]
	INIT(= N_("E613: Unknown printer font: %s"));
#అయితే_ముగింపు
// E614 unused (deleted)
// E615 unused
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_object_required_for_వాదం_nr[]
	INIT(= N_("E616: Object required for argument %d"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_GTK
EXTERN అక్షరం e_cannot_be_మారింది_లోపల_gtk_GUI[]
	INIT(= N_("E617: Cannot be changed in the GTK GUI"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_POSTSCRIPT
EXTERN అక్షరం e_file_str_is_not_postscript_resource_file[]
	INIT(= N_("E618: File \"%s\" is not a PostScript resource file"));
EXTERN అక్షరం e_file_str_is_not_supported_postscript_resource_file[]
	INIT(= N_("E619: File \"%s\" is not a supported PostScript resource file"));
EXTERN అక్షరం e_unable_గమ్యం_convert_గమ్యం_print_encoding_str[]
	INIT(= N_("E620: Unable to convert to print encoding \"%s\""));
EXTERN అక్షరం e_str_resource_file_has_wrong_సంస్కరణ[]
	INIT(= N_("E621: \"%s\" resource file has wrong version"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CSCOPE
EXTERN అక్షరం e_could_not_fork_for_cscope[]
	INIT(= N_("E622: Could not fork for cscope"));
# ifndef UNIX
EXTERN అక్షరం e_could_not_spawn_cscope_process[]
	INIT(= N_("E623: Could not spawn cscope process"));
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PRINTER) && defined(FEAT_POSTSCRIPT)
EXTERN అక్షరం e_cant_open_file_str_3[]
	INIT(= N_("E624: Can't open file \"%s\""));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CSCOPE) && !defined(UNIX)
EXTERN అక్షరం e_cannot_open_cscope_database_str[]
	INIT(= N_("E625: Cannot open cscope database: %s"));
EXTERN అక్షరం e_cannot_get_cscope_database_information[]
	INIT(= N_("E626: Cannot get cscope database information"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
EXTERN అక్షరం e_missing_colon_str[]
	INIT(= N_("E627: Missing colon: %s"));
EXTERN అక్షరం e_missing_bang_or_slash_లోపల_str[]
	INIT(= N_("E628: Missing ! or / in: %s"));
# ifdef NBDEBUG
EXTERN అక్షరం e_bad_return_from_nb_do_cmd[]
	INIT(= "E629: Bad return from nb_do_cmd");
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
EXTERN అక్షరం e_str_write_while_not_connected[]
	INIT(= N_("E630: %s(): Write while not connected"));
EXTERN అక్షరం e_str_write_failed[]
	INIT(= N_("E631: %s(): Write failed"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_getlength[]
	INIT(= N_("E632: Invalid buffer identifier in getLength"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_gettext[]
	INIT(= N_("E633: Invalid buffer identifier in getText"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_remove[]
	INIT(= N_("E634: Invalid buffer identifier in remove"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_చొప్పించు[]
	INIT(= N_("E635: Invalid buffer identifier in insert"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_create[]
	INIT(= N_("E636: Invalid buffer identifier in create"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_startdocumentlisten[]
	INIT(= N_("E637: Invalid buffer identifier in startDocumentListen"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_stopdocumentlisten[]
	INIT(= N_("E638: Invalid buffer identifier in stopDocumentListen"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_settitle[]
	INIT(= N_("E639: Invalid buffer identifier in setTitle"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_initdone[]
	INIT(= N_("E640: Invalid buffer identifier in initDone"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_setbuffernumber[]
	INIT(= N_("E641: Invalid buffer identifier in setBufferNumber"));
EXTERN అక్షరం e_file_str_not_దొరికింది_లోపల_setbuffernumber[]
	INIT(= N_("E642: File %s not found in setBufferNumber"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_setfullname[]
	INIT(= N_("E643: Invalid buffer identifier in setFullName"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_editfile[]
	INIT(= N_("E644: Invalid buffer identifier in editFile"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_setvisible[]
	INIT(= N_("E645: Invalid buffer identifier in setVisible"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_setmodified[]
	INIT(= N_("E646: Invalid buffer identifier in setModified"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_setdot[]
	INIT(= N_("E647: Invalid buffer identifier in setDot"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_close[]
	INIT(= N_("E648: Invalid buffer identifier in close"));
EXTERN అక్షరం e_invalid_identifier_లోపల_defineannotype[]
	INIT(= N_("E649: Invalid identifier name in defineAnnoType"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_defineannotype[]
	INIT(= N_("E650: Invalid buffer identifier in defineAnnoType"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_addanno[]
	INIT(= N_("E651: Invalid buffer identifier in addAnno"));
EXTERN అక్షరం e_invalid_బఫరు_identifier_లోపల_getanno[]
	INIT(= N_("E652: Invalid buffer identifier in getAnno"));
#అయితే_ముగింపు
// E653 unused
EXTERN అక్షరం e_missing_delimiter_after_శోధన_నమూనా_str[]
	INIT(= N_("E654: Missing delimiter after search pattern: %s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_too_many_symbolic_links_చక్రం[]
	INIT(= N_("E655: Too many symbolic links (cycle?)"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	// E656
EXTERN అక్షరం e_netbeans_disallows_writes_యొక్క_unmodified_buffers[]
	INIT(= N_("NetBeans disallows writes of unmodified buffers"));
	// E657
EXTERN అక్షరం e_partial_writes_disallowed_for_netbeans_buffers[]
	INIT(= N_("Partial writes disallowed for NetBeans buffers"));
EXTERN అక్షరం e_netbeans_connection_కోల్పోయిన_for_బఫరు_nr[]
	INIT(= N_("E658: NetBeans connection lost for buffer %d"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PYTHON
EXTERN అక్షరం e_cannot_invoke_python_recursively[]
	INIT(= N_("E659: Cannot invoke Python recursively"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
EXTERN అక్షరం e_cannot_open_netbeans_connection_info_file[]
	INIT(= "E660: Cannot open NetBeans connection info file");
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MULTI_LANG
EXTERN అక్షరం e_sorry_no_str_help_for_str[]
	INIT(= N_("E661: Sorry, no '%s' help for %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_at_ప్రారంభం_యొక్క_changelist[]
	INIT(= N_("E662: At start of changelist"));
EXTERN అక్షరం e_at_చివర_యొక్క_changelist[]
	INIT(= N_("E663: At end of changelist"));
EXTERN అక్షరం e_changelist_is_ఖాళీలు[]
	INIT(= N_("E664: Changelist is empty"));
#నిర్వచితమైతే FEAT_GUI
EXTERN అక్షరం e_cannot_ప్రారంభం_gui_no_valid_font_దొరికింది[]
	INIT(= N_("E665: Cannot start GUI, no valid font found"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_compiler_not_supported_str[]
	INIT(= N_("E666: Compiler not supported: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_FSYNC
EXTERN అక్షరం e_fsync_failed[]
	INIT(= N_("E667: Fsync failed"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
EXTERN అక్షరం e_wrong_access_బహుళకం_for_netbeans_connection_info_file_str[]
	INIT(= N_("E668: Wrong access mode for NetBeans connection info file: \"%s\""));
#అయితే_ముగింపు
EXTERN అక్షరం e_unprintable_character_లోపల_సమూహం_name[]
	INIT(= N_("E669: Unprintable character in group name"));
EXTERN అక్షరం e_mix_యొక్క_help_file_encodings_within_language_str[]
	INIT(= N_("E670: Mix of help file encodings within a language: %s"));
#నిర్వచితమైతే FEAT_GUI_MSWIN
EXTERN అక్షరం e_cannot_కనుగొను_కిటికీ_title_str[]
	INIT(= N_("E671: Cannot find window title \"%s\""));
EXTERN అక్షరం e_unable_గమ్యం_open_కిటికీ_inside_mdi_application[]
	INIT(= N_("E672: Unable to open window inside MDI application"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PRINTER) && defined(FEAT_POSTSCRIPT)
EXTERN అక్షరం e_incompatible_multi_byte_encoding_and_character_సమితి[]
	INIT(= N_("E673: Incompatible multi-byte encoding and character set"));
EXTERN అక్షరం e_printmbcharset_cannot_be_ఖాళీలు_with_multi_byte_encoding[]
	INIT(= N_("E674: printmbcharset cannot be empty with multi-byte encoding."));
EXTERN అక్షరం e_no_default_font_specified_for_multi_byte_printing[]
	INIT(= N_("E675: No default font specified for multi-byte printing."));
#అయితే_ముగింపు
EXTERN అక్షరం e_no_matching_autocommands_for_buftype_str_బఫరు[]
	INIT(= N_("E676: No matching autocommands for buftype=%s buffer"));
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN అక్షరం e_error_writing_తాత్కాలిక_file[]
	INIT(= N_("E677: Error writing temp file"));
#అయితే_ముగింపు
EXTERN అక్షరం e_invalid_character_after_str_2[]
	INIT(= N_("E678: Invalid character after %s%%[dxouU]"));
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN అక్షరం e_recursive_loop_loading_syncolor_vim[]
	INIT(= N_("E679: Recursive loop loading syncolor.vim"));
#అయితే_ముగింపు
EXTERN అక్షరం e_బఫరు_nr_invalid_బఫరు_సంఖ్య[]
	INIT(= N_("E680: <buffer=%d>: invalid buffer number"));
#ఒకవేళ defined(FEAT_QUICKFIX) || defined(FEAT_EVAL)
EXTERN అక్షరం e_బఫరు_is_not_loaded[]
	INIT(= N_("E681: Buffer is not loaded"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_invalid_శోధన_నమూనా_or_delimiter[]
	INIT(= N_("E682: Invalid search pattern or delimiter"));
EXTERN అక్షరం e_file_name_missing_or_invalid_నమూనా[]
	INIT(= N_("E683: File name missing or invalid pattern"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_list_సూచీ_బహిర్గతం_యొక్క_range_nr[]
	INIT(= N_("E684: List index out of range: %ld"));
#అయితే_ముగింపు
EXTERN అక్షరం e_అంతర్గత_error_str[]
	INIT(= N_("E685: Internal error: %s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_వాదం_యొక్క_str_must_be_list[]
	INIT(= N_("E686: Argument of %s must be a List"));
EXTERN అక్షరం e_less_targets_than_list_వస్తువులు[]
	INIT(= N_("E687: Less targets than List items"));
EXTERN అక్షరం e_మరిన్ని_targets_than_list_వస్తువులు[]
	INIT(= N_("E688: More targets than List items"));
EXTERN అక్షరం e_సూచీ_not_allowed_after_str_str[]
	INIT(= N_("E689: Index not allowed after a %s: %s"));
EXTERN అక్షరం e_missing_లోపల_after_for[]
	INIT(= N_("E690: Missing \"in\" after :for"));
EXTERN అక్షరం e_can_only_compare_list_with_list[]
	INIT(= N_("E691: Can only compare List with List"));
EXTERN అక్షరం e_invalid_operation_for_list[]
	INIT(= N_("E692: Invalid operation for List"));
EXTERN అక్షరం e_class_or_typealias_required_for_వాదం_nr[]
	INIT(= N_("E693: Class or class typealias required for argument %d"));
EXTERN అక్షరం e_invalid_operation_for_funcrefs[]
	INIT(= N_("E694: Invalid operation for Funcrefs"));
EXTERN అక్షరం e_cannot_సూచీ_a_funcref[]
	INIT(= N_("E695: Cannot index a Funcref"));
EXTERN అక్షరం e_missing_comma_లోపల_list_str[]
	INIT(= N_("E696: Missing comma in List: %s"));
EXTERN అక్షరం e_missing_చివర_యొక్క_list_rsb_str[]
	INIT(= N_("E697: Missing end of List ']': %s"));
EXTERN అక్షరం e_variable_nested_too_deep_for_making_copy[]
	INIT(= N_("E698: Variable nested too deep for making a copy"));
EXTERN అక్షరం e_too_many_arguments[]
	INIT(= N_("E699: Too many arguments"));
EXTERN అక్షరం e_unknown_function_str_2[]
	INIT(= N_("E700: Unknown function: %s"));
EXTERN అక్షరం e_invalid_type_for_len[]
	INIT(= N_("E701: Invalid type for len()"));
EXTERN అక్షరం e_క్రమీకరించు_compare_function_failed[]
	INIT(= N_("E702: Sort compare function failed"));
EXTERN అక్షరం e_using_funcref_as_సంఖ్య[]
	INIT(= N_("E703: Using a Funcref as a Number"));
EXTERN అక్షరం e_funcref_variable_name_must_ప్రారంభం_with_capital_str[]
	INIT(= N_("E704: Funcref variable name must start with a capital: %s"));
EXTERN అక్షరం e_variable_name_conflicts_with_existing_function_str[]
	INIT(= N_("E705: Variable name conflicts with existing function: %s"));
EXTERN అక్షరం e_వాదం_యొక్క_str_must_be_list_string_or_dictionary[]
	INIT(= N_("E706: Argument of %s must be a List, String or Dictionary"));
EXTERN అక్షరం e_function_name_conflicts_with_variable_str[]
	INIT(= N_("E707: Function name conflicts with variable: %s"));
EXTERN అక్షరం e_ముక్క_must_come_చివరి[]
	INIT(= N_("E708: [:] must come last"));
EXTERN అక్షరం e_ముక్క_requires_list_or_blob_విలువ[]
	INIT(= N_("E709: [:] requires a List or Blob value"));
EXTERN అక్షరం e_list_విలువ_has_మరిన్ని_వస్తువులు_than_targets[]
	INIT(= N_("E710: List value has more items than targets"));
EXTERN అక్షరం e_list_విలువ_does_not_have_enough_వస్తువులు[]
	INIT(= N_("E711: List value does not have enough items"));
EXTERN అక్షరం e_వాదం_యొక్క_str_must_be_list_or_dictionary[]
	INIT(= N_("E712: Argument of %s must be a List or Dictionary"));
EXTERN అక్షరం e_cannot_use_ఖాళీలు_కీలకం_for_dictionary[]
	INIT(= N_("E713: Cannot use empty key for Dictionary"));
EXTERN అక్షరం e_list_required[]
	INIT(= N_("E714: List required"));
EXTERN అక్షరం e_dictionary_required[]
	INIT(= N_("E715: Dictionary required"));
EXTERN అక్షరం e_కీలకం_not_present_లోపల_dictionary_str[]
	INIT(= N_("E716: Key not present in Dictionary: \"%s\""));
EXTERN అక్షరం e_dictionary_నమోదు_already_exists[]
	INIT(= N_("E717: Dictionary entry already exists"));
EXTERN అక్షరం e_funcref_required[]
	INIT(= N_("E718: Funcref required"));
EXTERN అక్షరం e_cannot_ముక్క_dictionary[]
	INIT(= N_("E719: Cannot slice a Dictionary"));
EXTERN అక్షరం e_missing_colon_లోపల_dictionary_str[]
	INIT(= N_("E720: Missing colon in Dictionary: %s"));
EXTERN అక్షరం e_duplicate_కీలకం_లోపల_dictionary_str[]
	INIT(= N_("E721: Duplicate key in Dictionary: \"%s\""));
EXTERN అక్షరం e_missing_comma_లోపల_dictionary_str[]
	INIT(= N_("E722: Missing comma in Dictionary: %s"));
EXTERN అక్షరం e_missing_dict_చివర_str[]
	INIT(= N_("E723: Missing end of Dictionary '}': %s"));
EXTERN అక్షరం e_variable_nested_too_deep_for_displaying[]
	INIT(= N_("E724: Variable nested too deep for displaying"));
EXTERN అక్షరం e_calling_dict_function_without_dictionary_str[]
	INIT(= N_("E725: Calling dict function without Dictionary: %s"));
EXTERN అక్షరం e_stride_is_zero[]
	INIT(= N_("E726: Stride is zero"));
EXTERN అక్షరం e_ప్రారంభం_past_చివర[]
	INIT(= N_("E727: Start past end"));
EXTERN అక్షరం e_using_dictionary_as_సంఖ్య[]
	INIT(= N_("E728: Using a Dictionary as a Number"));
EXTERN అక్షరం e_using_funcref_as_string[]
	INIT(= N_("E729: Using a Funcref as a String"));
EXTERN అక్షరం e_using_list_as_string[]
	INIT(= N_("E730: Using a List as a String"));
EXTERN అక్షరం e_using_dictionary_as_string[]
	INIT(= N_("E731: Using a Dictionary as a String"));
EXTERN అక్షరం e_using_endfor_with_while[]
	INIT(= N_("E732: Using :endfor with :while"));
EXTERN అక్షరం e_using_endwhile_with_for[]
	INIT(= N_("E733: Using :endwhile with :for"));
EXTERN అక్షరం e_wrong_variable_type_for_str_equal[]
	INIT(= N_("E734: Wrong variable type for %s="));
EXTERN అక్షరం e_can_only_compare_dictionary_with_dictionary[]
	INIT(= N_("E735: Can only compare Dictionary with Dictionary"));
EXTERN అక్షరం e_invalid_operation_for_dictionary[]
	INIT(= N_("E736: Invalid operation for Dictionary"));
EXTERN అక్షరం e_కీలకం_already_exists_str[]
	INIT(= N_("E737: Key already exists: %s"));
EXTERN అక్షరం e_cant_list_variables_for_str[]
	INIT(= N_("E738: Can't list variables for %s"));
EXTERN అక్షరం e_cannot_create_directory_str[]
	INIT(= N_("E739: Cannot create directory: %s"));
EXTERN అక్షరం e_too_many_arguments_for_function_str_2[]
	INIT(= N_("E740: Too many arguments for function %s"));
EXTERN అక్షరం e_విలువ_is_locked[]
	INIT(= N_("E741: Value is locked"));
EXTERN అక్షరం e_విలువ_is_locked_str[]
	INIT(= N_("E741: Value is locked: %s"));
EXTERN అక్షరం e_cannot_change_విలువ[]
	INIT(= N_("E742: Cannot change value"));
EXTERN అక్షరం e_cannot_change_విలువ_యొక్క_str[]
	INIT(= N_("E742: Cannot change value of %s"));
EXTERN అక్షరం e_variable_nested_too_deep_for_unlock[]
	INIT(= N_("E743: Variable nested too deep for (un)lock"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
EXTERN అక్షరం e_netbeans_does_not_allow_changes_లోపల_read_only_files[]
	INIT(= N_("E744: NetBeans does not allow changes in read-only files"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_using_list_as_సంఖ్య[]
	INIT(= N_("E745: Using a List as a Number"));
EXTERN అక్షరం e_function_name_does_not_సరిపోలిక_script_file_name_str[]
	INIT(= N_("E746: Function name does not match script file name: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_change_directory_బఫరు_is_modified_కూడు_bang_గమ్యం_override[]
	INIT(= N_("E747: Cannot change directory, buffer is modified (add ! to override)"));
EXTERN అక్షరం e_no_previously_వాడిన_రిజిస్టరు[]
	INIT(= N_("E748: No previously used register"));
EXTERN అక్షరం e_ఖాళీలు_బఫరు[]
	INIT(= N_("E749: Empty buffer"));
#నిర్వచితమైతే FEAT_PROFILE
EXTERN అక్షరం e_మొదటి_use_profile_ప్రారంభం_fname[]
	INIT(= N_("E750: First use \":profile start {fname}\""));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
EXTERN అక్షరం e_output_file_name_must_not_have_region_name[]
	INIT(= N_("E751: Output file name must not have region name"));
EXTERN అక్షరం e_no_మునుపటి_spell_replacement[]
	INIT(= N_("E752: No previous spell replacement"));
EXTERN అక్షరం e_not_దొరికింది_str[]
	INIT(= N_("E753: Not found: %s"));
EXTERN అక్షరం e_only_పైకి_గమ్యం_nr_regions_supported[]
	INIT(= N_("E754: Only up to %d regions supported"));
EXTERN అక్షరం e_invalid_region_లోపల_str[]
	INIT(= N_("E755: Invalid region in %s"));
EXTERN అక్షరం e_spell_checking_is_not_possible[]
	INIT(= N_("E756: Spell checking is not possible"));
EXTERN అక్షరం e_this_does_not_look_like_spell_file[]
	INIT(= N_("E757: This does not look like a spell file"));
EXTERN అక్షరం e_truncated_spell_file[]
	INIT(= N_("E758: Truncated spell file"));
EXTERN అక్షరం e_format_error_లోపల_spell_file[]
	INIT(= N_("E759: Format error in spell file"));
EXTERN అక్షరం e_no_మాట_లెక్క_లోపల_str[]
	INIT(= N_("E760: No word count in %s"));
EXTERN అక్షరం e_format_error_లోపల_affix_file_fol_దిగువ_or_upp[]
	INIT(= N_("E761: Format error in affix file FOL, LOW or UPP"));
EXTERN అక్షరం e_character_లోపల_fol_దిగువ_or_upp_is_బహిర్గతం_యొక్క_range[]
	INIT(= N_("E762: Character in FOL, LOW or UPP is out of range"));
EXTERN అక్షరం e_మాట_characters_తేడా_between_spell_files[]
	INIT(= N_("E763: Word characters differ between spell files"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_SYN_HL) || defined(FEAT_COMPL_FUNC) || defined(FEAT_SPELL)
EXTERN అక్షరం e_option_str_is_not_సమితి[]
	INIT(= N_("E764: Option '%s' is not set"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
EXTERN అక్షరం e_spellfile_does_not_have_nr_entries[]
	INIT(= N_("E765: 'spellfile' does not have %d entries"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_insufficient_arguments_for_printf[]
	INIT(= N_("E766: Insufficient arguments for printf()"));
#అయితే_ముగింపు
EXTERN అక్షరం e_too_many_arguments_గమ్యం_printf[]
	INIT(= N_("E767: Too many arguments for printf()"));
EXTERN అక్షరం e_మార్పిడి_file_exists_str_silent_overrides[]
	INIT(= N_("E768: Swap file exists: %s (:silent! overrides)"));
EXTERN అక్షరం e_missing_rsb_after_str_lsb[]
	INIT(= N_("E769: Missing ] after %s["));
#నిర్వచితమైతే FEAT_SPELL
EXTERN అక్షరం e_unsupported_section_లోపల_spell_file[]
	INIT(= N_("E770: Unsupported section in spell file"));
EXTERN అక్షరం e_old_spell_file_needs_గమ్యం_be_updated[]
	INIT(= N_("E771: Old spell file, needs to be updated"));
EXTERN అక్షరం e_spell_file_is_for_newer_సంస్కరణ_యొక్క_vim[]
	INIT(= N_("E772: Spell file is for newer version of Vim"));
#అయితే_ముగింపు
EXTERN అక్షరం e_symlink_loop_for_str[]
	INIT(= N_("E773: Symlink loop for \"%s\""));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_operatorfunc_is_ఖాళీలు[]
	INIT(= N_("E774: 'operatorfunc' is empty"));
#లేకపోతే
EXTERN అక్షరం e_eval_feature_not_అందుబాటు[]
	INIT(= N_("E775: Eval feature not available"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_no_location_list[]
	INIT(= N_("E776: No location list"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_string_or_list_expected[]
	INIT(= N_("E777: String or List expected"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
EXTERN అక్షరం e_this_does_not_look_like_sug_file_str[]
	INIT(= N_("E778: This does not look like a .sug file: %s"));
EXTERN అక్షరం e_old_sug_file_needs_గమ్యం_be_updated_str[]
	INIT(= N_("E779: Old .sug file, needs to be updated: %s"));
EXTERN అక్షరం e_sug_file_is_for_newer_సంస్కరణ_యొక్క_vim_str[]
	INIT(= N_("E780: .sug file is for newer version of Vim: %s"));
EXTERN అక్షరం e_sug_file_doesnt_సరిపోలిక_spl_file_str[]
	INIT(= N_("E781: .sug file doesn't match .spl file: %s"));
EXTERN అక్షరం e_error_while_reading_sug_file_str[]
	INIT(= N_("E782: Error while reading .sug file: %s"));
EXTERN అక్షరం e_duplicate_char_లోపల_map_నమోదు[]
	INIT(= N_("E783: Duplicate char in MAP entry"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_close_చివరి_tab_పుట[]
	INIT(= N_("E784: Cannot close last tab page"));
#నిర్వచితమైతే FEAT_EVAL
# ifdef FEAT_COMPL_FUNC
EXTERN అక్షరం e_complete_can_only_be_వాడిన_లోపల_చొప్పించు_బహుళకం[]
	INIT(= N_("E785: complete() can only be used in Insert mode"));
# endif
EXTERN అక్షరం e_range_not_allowed[]
	INIT(= N_("E786: Range not allowed"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
EXTERN అక్షరం e_బఫరు_మారింది_unexpectedly[]
	INIT(= N_("E787: Buffer changed unexpectedly"));
#అయితే_ముగింపు
EXTERN అక్షరం e_not_allowed_గమ్యం_edit_another_బఫరు_now[]
	INIT(= N_("E788: Not allowed to edit another buffer now"));
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN అక్షరం e_error_missing_rsb_str[]
	INIT(= N_("E789: Missing ']': %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_undojoin_is_not_allowed_after_undo[]
	INIT(= N_("E790: undojoin is not allowed after undo"));
#నిర్వచితమైతే FEAT_KEYMAP
EXTERN అక్షరం e_ఖాళీలు_keymap_నమోదు[]
	INIT(= N_("E791: Empty keymap entry"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
EXTERN అక్షరం e_ఖాళీలు_menu_name[]
	INIT(= N_("E792: Empty menu name"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
EXTERN అక్షరం e_no_other_బఫరు_లోపల_diff_బహుళకం_is_modifiable[]
	INIT(= N_("E793: No other buffer in diff mode is modifiable"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_సమితి_variable_లోపల_sandbox[]
	INIT(= N_("E794: Cannot set variable in the sandbox"));
EXTERN అక్షరం e_cannot_సమితి_variable_లోపల_sandbox_str[]
	INIT(= N_("E794: Cannot set variable in the sandbox: \"%s\""));
EXTERN అక్షరం e_cannot_delete_variable[]
	INIT(= N_("E795: Cannot delete variable"));
EXTERN అక్షరం e_cannot_delete_variable_str[]
	INIT(= N_("E795: Cannot delete variable %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
	// E796
EXTERN అక్షరం e_writing_గమ్యం_device_disabled_with_opendevice_option[]
	INIT(= N_("writing to device disabled with 'opendevice' option"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
EXTERN అక్షరం e_spellfilemising_autocommand_deleted_బఫరు[]
	INIT(= N_("E797: SpellFileMissing autocommand deleted buffer"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_శోధన_EXTRA
EXTERN అక్షరం e_id_is_reserved_for_సరిపోలిక_nr[]
	INIT(= N_("E798: ID is reserved for \":match\": %d"));
EXTERN అక్షరం e_invalid_id_nr_must_be_greater_than_or_equal_గమ్యం_one_1[]
	INIT(= N_("E799: Invalid ID: %d (must be greater than or equal to 1)"));
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_ARABIC
EXTERN అక్షరం e_arabic_cannot_be_వాడిన_not_enabled_at_compile_సమయం[]
	INIT(= N_("E800: Arabic cannot be used: Not enabled at compile time\n"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_శోధన_EXTRA
EXTERN అక్షరం e_id_already_తీసుకున్న_nr[]
	INIT(= N_("E801: ID already taken: %d"));
EXTERN అక్షరం e_invalid_id_nr_must_be_greater_than_or_equal_గమ్యం_one_2[]
	INIT(= N_("E802: Invalid ID: %d (must be greater than or equal to 1)"));
EXTERN అక్షరం e_id_not_దొరికింది_nr[]
	INIT(= N_("E803: ID not found: %d"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_use_percent_with_float[]
	// xgettext:no-c-format
	INIT(= N_("E804: Cannot use '%' with Float"));
EXTERN అక్షరం e_using_float_as_సంఖ్య[]
	INIT(= N_("E805: Using a Float as a Number"));
EXTERN అక్షరం e_using_float_as_string[]
	INIT(= N_("E806: Using a Float as a String"));
EXTERN అక్షరం e_expected_float_వాదం_for_printf[]
	INIT(= N_("E807: Expected Float argument for printf()"));
EXTERN అక్షరం e_సంఖ్య_or_float_required[]
	INIT(= N_("E808: Number or Float required"));
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే FEAT_EVAL
EXTERN అక్షరం e_hashsmall_is_not_అందుబాటు_without_the_eval_feature[]
	INIT(= N_("E809: #< is not available without the +eval feature"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
EXTERN అక్షరం e_cannot_read_or_write_తాత్కాలిక_files[]
	INIT(= N_("E810: Cannot read or write temp files"));
#అయితే_ముగింపు
EXTERN అక్షరం e_not_allowed_గమ్యం_change_బఫరు_information_now[]
	INIT(= N_("E811: Not allowed to change buffer information now"));
EXTERN అక్షరం e_autocommands_మారింది_బఫరు_or_బఫరు_name[]
	INIT(= N_("E812: Autocommands changed buffer or buffer name"));
EXTERN అక్షరం e_cannot_close_autocmd_or_popup_కిటికీ[]
	INIT(= N_("E813: Cannot close autocmd or popup window"));
EXTERN అక్షరం e_cannot_close_కిటికీ_only_autocmd_కిటికీ_would_remain[]
	INIT(= N_("E814: Cannot close window, only autocmd window would remain"));
#నిర్వచితమైతే FEAT_MZSCHEME
EXTERN అక్షరం e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded[]
	INIT(= N_("E815: Sorry, this command is disabled, the MzScheme libraries could not be loaded."));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
EXTERN అక్షరం e_cannot_read_patch_output[]
	INIT(= N_("E816: Cannot read patch output"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
EXTERN అక్షరం e_blowfish_big_little_endian_use_wrong[]
	INIT(= N_("E817: Blowfish big/little endian use wrong"));
EXTERN అక్షరం e_sha256_test_failed[]
	INIT(= N_("E818: sha256 test failed"));
EXTERN అక్షరం e_blowfish_test_failed[]
	INIT(= N_("E819: Blowfish test failed"));
EXTERN అక్షరం e_sizeof_uint32_isnot_four[]
	INIT(= N_("E820: sizeof(uint32_t) != 4"));
EXTERN అక్షరం e_file_is_encrypted_with_unknown_పద్ధతి[]
	INIT(= N_("E821: File is encrypted with unknown method"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
EXTERN అక్షరం e_cannot_open_undo_file_for_reading_str[]
	INIT(= N_("E822: Cannot open undo file for reading: %s"));
EXTERN అక్షరం e_not_an_undo_file_str[]
	INIT(= N_("E823: Not an undo file: %s"));
EXTERN అక్షరం e_incompatible_undo_file_str[]
	INIT(= N_("E824: Incompatible undo file: %s"));
EXTERN అక్షరం e_corrupted_undo_file_str_str[]
	INIT(= N_("E825: Corrupted undo file (%s): %s"));
# ifdef FEAT_CRYPT
EXTERN అక్షరం e_undo_file_decryption_failed[]
	INIT(= N_("E826: Undo file decryption failed: %s"));
# లేకపోతే
EXTERN అక్షరం e_undo_file_is_encrypted_str[]
	INIT(= N_("E827: Undo file is encrypted: %s"));
# endif
EXTERN అక్షరం e_cannot_open_undo_file_for_writing_str[]
	INIT(= N_("E828: Cannot open undo file for writing: %s"));
EXTERN అక్షరం e_write_error_లోపల_undo_file_str[]
	INIT(= N_("E829: Write error in undo file: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_undo_సంఖ్య_nr_not_దొరికింది[]
	INIT(= N_("E830: Undo number %ld not found"));
#నిర్వచితమైతే FEAT_CRYPT
EXTERN అక్షరం e_bf_కీలకం_init_called_with_ఖాళీలు_password[]
	INIT(= "E831: bf_key_init() called with empty password");
# ifdef FEAT_PERSISTENT_UNDO
EXTERN అక్షరం e_non_encrypted_file_has_encrypted_undo_file_str[]
	INIT(= N_("E832: Non-encrypted file has encrypted undo file: %s"));
# endif
#లేకపోతే
EXTERN అక్షరం e_str_is_encrypted_and_this_సంస్కరణ_యొక్క_vim_does_not_support_encryption[]
	INIT(= N_("E833: %s is encrypted and this version of Vim does not support encryption"));
#అయితే_ముగింపు
EXTERN అక్షరం e_conflicts_with_విలువ_యొక్క_listchars[]
	INIT(= N_("E834: Conflicts with value of 'listchars'"));
EXTERN అక్షరం e_conflicts_with_విలువ_యొక్క_fillchars[]
	INIT(= N_("E835: Conflicts with value of 'fillchars'"));
#నిర్వచితమైతే DYNAMIC_PYTHON
EXTERN అక్షరం e_this_vim_cannot_execute_python_after_using_py3[]
	INIT(= N_("E836: This Vim cannot execute :python after using :py3"));
EXTERN అక్షరం e_this_vim_cannot_execute_py3_after_using_python[]
	INIT(= N_("E837: This Vim cannot execute :py3 after using :python"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI)
EXTERN అక్షరం e_netbeans_is_not_supported_with_this_GUI[]
	INIT(= N_("E838: NetBeans is not supported with this GUI"));
#అయితే_ముగింపు
// E839 unused
#నిర్వచితమైతే FEAT_COMPL_FUNC
EXTERN అక్షరం e_complete_function_deleted_పాఠ్యం[]
	INIT(= N_("E840: Completion function deleted text"));
#అయితే_ముగింపు
EXTERN అక్షరం e_reserved_name_cannot_be_వాడిన_for_user_defined_command[]
	INIT(= N_("E841: Reserved name, cannot be used for user defined command"));
EXTERN అక్షరం e_no_పంక్తి_సంఖ్య_గమ్యం_use_for_slnum[]
	INIT(= N_("E842: No line number to use for \"<slnum>\""));
#నిర్వచితమైతే FEAT_CRYPT
EXTERN అక్షరం e_error_while_updating_మార్పిడి_file_crypt[]
	INIT(= N_("E843: Error while updating swap file crypt"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CONCEAL
EXTERN అక్షరం e_invalid_cchar_విలువ[]
	INIT(= N_("E844: Invalid cchar value"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
EXTERN అక్షరం e_insufficient_జ్ఞాపకం_మాట_list_will_be_incomplete[]
	INIT(= N_("E845: Insufficient memory, word list will be incomplete"));
#అయితే_ముగింపు
EXTERN అక్షరం e_కీలకం_సంకేతం_not_సమితి[]
	INIT(= N_("E846: Key code not set"));
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN అక్షరం e_too_many_syntax_includes[]
	INIT(= N_("E847: Too many syntax includes"));
EXTERN అక్షరం e_too_many_syntax_clusters[]
	INIT(= N_("E848: Too many syntax clusters"));
#అయితే_ముగింపు
EXTERN అక్షరం e_too_many_highlight_and_syntax_groups[]
	INIT(= N_("E849: Too many highlight and syntax groups"));
#నిర్వచితం_కాకపోతే FEAT_CLIPBOARD
EXTERN అక్షరం e_invalid_రిజిస్టరు_name[]
	INIT(= N_("E850: Invalid register name"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI
EXTERN అక్షరం e_failed_గమ్యం_create_new_process_for_GUI[]
	INIT(= N_("E851: Failed to create a new process for the GUI"));
EXTERN అక్షరం e_the_బిడ్డ_process_failed_గమ్యం_ప్రారంభం_GUI[]
	INIT(= N_("E852: The child process failed to start the GUI"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_duplicate_వాదం_name_str[]
	INIT(= N_("E853: Duplicate argument name: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_మార్గం_too_long_for_completion[]
	INIT(= N_("E854: Path too long for completion"));
EXTERN అక్షరం e_autocommands_caused_command_గమ్యం_abort[]
	INIT(= N_("E855: Autocommands caused command to abort"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_assert_fails_రెండవ_arg[]
	INIT(= N_("E856: \"assert_fails()\" second argument must be a string or a list with one or two strings"));
EXTERN అక్షరం e_dictionary_కీలకం_str_required[]
	INIT(= N_("E857: Dictionary key \"%s\" required"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN అక్షరం e_eval_did_not_return_valid_python_object[]
	INIT(= N_("E858: Eval did not return a valid python object"));
EXTERN అక్షరం e_failed_గమ్యం_convert_returned_python_object_గమ్యం_vim_విలువ[]
	INIT(= N_("E859: Failed to convert returned python object to a Vim value"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_need_id_and_type_or_types_with_both[]
	INIT(= N_("E860: Need 'id' and 'type' or 'types' with 'both'"));
# ifdef FEAT_TERMINAL
EXTERN అక్షరం e_cannot_open_రెండవ_popup_with_terminal[]
	INIT(= N_("E861: Cannot open a second popup with a terminal"));
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_use_g_ఇక్కడ[]
	INIT(= N_("E862: Cannot use g: here"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
EXTERN అక్షరం e_not_allowed_for_terminal_లోపల_popup_కిటికీ[]
	INIT(= N_("E863: Not allowed for a terminal in a popup window"));
#అయితే_ముగింపు
EXTERN అక్షరం e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_వాడిన[]
	// xgettext:no-c-format
	INIT(= N_("E864: \\%#= can only be followed by 0, 1, or 2. The automatic engine will be used"));
EXTERN అక్షరం e_nfa_regexp_చివర_encountered_prematurely[]
	INIT(= N_("E865: (NFA) Regexp end encountered prematurely"));
EXTERN అక్షరం e_nfa_regexp_misplaced_chr[]
	INIT(= N_("E866: (NFA regexp) Misplaced %c"));
EXTERN అక్షరం e_nfa_regexp_unknown_operator_z_chr[]
	INIT(= N_("E867: (NFA regexp) Unknown operator '\\z%c'"));
EXTERN అక్షరం e_nfa_regexp_unknown_operator_percent_chr[]
	INIT(= N_("E867: (NFA regexp) Unknown operator '\\%%%c'"));
EXTERN అక్షరం e_error_building_nfa_with_equivalence_class[]
	INIT(= N_("E868: Error building NFA with equivalence class!"));
EXTERN అక్షరం e_nfa_regexp_unknown_operator_at_chr[]
	INIT(= N_("E869: (NFA regexp) Unknown operator '\\@%c'"));
EXTERN అక్షరం e_nfa_regexp_error_reading_repetition_limits[]
	INIT(= N_("E870: (NFA regexp) Error reading repetition limits"));
EXTERN అక్షరం e_nfa_regexp_cant_have_multi_follow_multi[]
	INIT(= N_("E871: (NFA regexp) Can't have a multi follow a multi"));
EXTERN అక్షరం e_nfa_regexp_too_many_parens[]
	INIT(= N_("E872: (NFA regexp) Too many '('"));
EXTERN అక్షరం e_nfa_regexp_proper_termination_error[]
	INIT(= N_("E873: (NFA regexp) proper termination error"));
EXTERN అక్షరం e_nfa_regexp_could_not_తీయి_దొంతర[]
	INIT(= N_("E874: (NFA regexp) Could not pop the stack!"));
EXTERN అక్షరం e_nfa_regexp_while_converting_from_postfix_గమ్యం_nfa_too_many_stats_ఎడమ_on_దొంతర[]
	INIT(= N_("E875: (NFA regexp) (While converting from postfix to NFA), too many states left on stack"));
EXTERN అక్షరం e_nfa_regexp_not_enough_space_గమ్యం_store_సంపూర్ణ_nfa[]
	INIT(= N_("E876: (NFA regexp) Not enough space to store the whole NFA"));
EXTERN అక్షరం e_nfa_regexp_invalid_character_class_nr[]
	INIT(= N_("E877: (NFA regexp) Invalid character class: %d"));
EXTERN అక్షరం e_nfa_regexp_could_not_allocate_జ్ఞాపకం_for_branch_traversal[]
	INIT(= N_("E878: (NFA regexp) Could not allocate memory for branch traversal!"));
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN అక్షరం e_nfa_regexp_too_many_z[]
	INIT(= N_("E879: (NFA regexp) Too many \\z("));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN అక్షరం e_cant_handle_systemexit_యొక్క_python_exception_లోపల_vim[]
	INIT(= N_("E880: Can't handle SystemExit of python exception in vim"));
#అయితే_ముగింపు
EXTERN అక్షరం e_పంక్తి_లెక్క_మారింది_unexpectedly[]
	INIT(= N_("E881: Line count changed unexpectedly"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_uniq_compare_function_failed[]
	INIT(= N_("E882: Uniq compare function failed"));
EXTERN అక్షరం e_రిజిస్టరు_char_cannot_contain_గుణిజం_పంక్తులు[]
	INIT(= N_("E883: Register '%c' cannot contain multiple lines"));
EXTERN అక్షరం e_function_name_cannot_contain_colon_str[]
	INIT(= N_("E884: Function name cannot contain a colon: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
EXTERN అక్షరం e_not_possible_గమ్యం_change_sign_str[]
	INIT(= N_("E885: Not possible to change sign %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_VIMINFO
EXTERN అక్షరం e_cant_rename_viminfo_file_గమ్యం_str[]
	INIT(= N_("E886: Can't rename viminfo file to %s!"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN అక్షరం e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded[]
	INIT(= N_("E887: Sorry, this command is disabled, the Python's site module could not be loaded."));
#అయితే_ముగింపు
EXTERN అక్షరం e_nfa_regexp_cannot_repeat_str[]
	INIT(= N_("E888: (NFA regexp) cannot repeat %s"));
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_సంఖ్య_required[]
	INIT(= N_("E889: Number required"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
EXTERN అక్షరం e_trailing_char_after_rsb_str_str[]
	INIT(= N_("E890: Trailing char after ']': %s]%s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_using_funcref_as_float[]
	INIT(= N_("E891: Using a Funcref as a Float"));
EXTERN అక్షరం e_using_string_as_float[]
	INIT(= N_("E892: Using a String as a Float"));
EXTERN అక్షరం e_using_list_as_float[]
	INIT(= N_("E893: Using a List as a Float"));
EXTERN అక్షరం e_using_dictionary_as_float[]
	INIT(= N_("E894: Using a Dictionary as a Float"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MZSCHEME
EXTERN అక్షరం e_sorry_this_command_is_disabled_the_mzscheme_racket_పునాది_module_could_not_be_loaded[]
	INIT(= N_("E895: Sorry, this command is disabled, the MzScheme's racket/base module could not be loaded."));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_వాదం_యొక్క_str_must_be_list_dictionary_or_blob[]
	INIT(= N_("E896: Argument of %s must be a List, Dictionary or Blob"));
EXTERN అక్షరం e_list_or_blob_required[]
	INIT(= N_("E897: List or Blob required"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
EXTERN అక్షరం e_socket_లోపల_channel_connect[]
	INIT(= N_("E898: socket() in channel_connect()"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_వాదం_యొక్క_str_must_be_list_or_blob[]
	INIT(= N_("E899: Argument of %s must be a List or Blob"));
EXTERN అక్షరం e_maxdepth_must_be_non_negative_సంఖ్య[]
	INIT(= N_("E900: maxdepth must be non-negative number"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
EXTERN అక్షరం e_getaddrinfo_లోపల_channel_open_str[]
	INIT(= N_("E901: getaddrinfo() in channel_open(): %s"));
# ifndef FEAT_IPV6
EXTERN అక్షరం e_gethostbyname_లోపల_channel_open[]
	INIT(= N_("E901: gethostbyname() in channel_open()"));
# endif
EXTERN అక్షరం e_cannot_connect_గమ్యం_పోర్టు[]
	INIT(= N_("E902: Cannot connect to port"));
EXTERN అక్షరం e_received_command_with_non_string_వాదం[]
	INIT(= N_("E903: Received command with non-string argument"));
EXTERN అక్షరం e_చివరి_వాదం_for_సమాసం_call_must_be_సంఖ్య[]
	INIT(= N_("E904: Last argument for expr/call must be a number"));
EXTERN అక్షరం e_మూడవ_వాదం_for_call_must_be_list[]
	INIT(= N_("E904: Third argument for call must be a list"));
EXTERN అక్షరం e_received_unknown_command_str[]
	INIT(= N_("E905: Received unknown command: %s"));
EXTERN అక్షరం e_not_an_open_channel[]
	INIT(= N_("E906: Not an open channel"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_using_special_విలువ_as_float[]
	INIT(= N_("E907: Using a special value as a Float"));
EXTERN అక్షరం e_using_invalid_విలువ_as_string_str[]
	INIT(= N_("E908: Using an invalid value as a String: %s"));
EXTERN అక్షరం e_cannot_సూచీ_special_variable[]
	INIT(= N_("E909: Cannot index a special variable"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
EXTERN అక్షరం e_using_పని_as_సంఖ్య[]
	INIT(= N_("E910: Using a Job as a Number"));
EXTERN అక్షరం e_using_పని_as_float[]
	INIT(= N_("E911: Using a Job as a Float"));
EXTERN అక్షరం e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel[]
	INIT(= N_("E912: Cannot use ch_evalexpr()/ch_sendexpr() with a raw, nl or blob channel"));
EXTERN అక్షరం e_using_channel_as_సంఖ్య[]
	INIT(= N_("E913: Using a Channel as a Number"));
EXTERN అక్షరం e_using_channel_as_float[]
	INIT(= N_("E914: Using a Channel as a Float"));
EXTERN అక్షరం e_లోపల_io_బఫరు_requires_లోపల_buf_or_లోపల_name_గమ్యం_be_సమితి[]
	INIT(= N_("E915: in_io buffer requires in_buf or in_name to be set"));
EXTERN అక్షరం e_not_valid_పని[]
	INIT(= N_("E916: Not a valid job"));
EXTERN అక్షరం e_cannot_use_callback_with_str[]
	INIT(= N_("E917: Cannot use a callback with %s()"));
EXTERN అక్షరం e_బఫరు_must_be_loaded_str[]
	INIT(= N_("E918: Buffer must be loaded: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_directory_not_దొరికింది_లోపల_str_str[]
	INIT(= N_("E919: Directory not found in '%s': \"%s\""));
#నిర్వచితమైతే FEAT_పని_CHANNEL
EXTERN అక్షరం e_io_file_requires_name_గమ్యం_be_సమితి[]
	INIT(= N_("E920: _io file requires _name to be set"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_invalid_callback_వాదం[]
	INIT(= N_("E921: Invalid callback argument"));
// E922 unused
EXTERN అక్షరం e_రెండవ_వాదం_యొక్క_function_must_be_list_or_dict[]
	INIT(= N_("E923: Second argument of function() must be a list or a dict"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_current_కిటికీ_was_closed[]
	INIT(= N_("E924: Current window was closed"));
EXTERN అక్షరం e_current_quickfix_list_was_మారింది[]
	INIT(= N_("E925: Current quickfix list was changed"));
EXTERN అక్షరం e_current_location_list_was_మారింది[]
	INIT(= N_("E926: Current location list was changed"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
# ifdef FEAT_QUICKFIX
EXTERN అక్షరం e_invalid_action_str_1[]
	INIT(= N_("E927: Invalid action: '%s'"));
# endif
EXTERN అక్షరం e_string_required[]
	INIT(= N_("E928: String required"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_VIMINFO
EXTERN అక్షరం e_too_many_viminfo_తాత్కాలిక_files_like_str[]
	INIT(= N_("E929: Too many viminfo temp files, like %s!"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_use_redir_inside_execute[]
	INIT(= N_("E930: Cannot use :redir inside execute()"));
#అయితే_ముగింపు
EXTERN అక్షరం e_బఫరు_cannot_be_registered[]
	INIT(= N_("E931: Buffer cannot be registered"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_సంవృతం_function_should_not_be_at_శిఖరం_level_str[]
	INIT(= N_("E932: Closure function should not be at top level: %s"));
EXTERN అక్షరం e_function_was_deleted_str[]
	INIT(= N_("E933: Function was deleted: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SIGNS
EXTERN అక్షరం e_cannot_jump_గమ్యం_బఫరు_that_does_not_have_name[]
	INIT(= N_("E934: Cannot jump to a buffer that does not have a name"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_invalid_submatch_సంఖ్య_nr[]
	INIT(= N_("E935: Invalid submatch number: %d"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_delete_current_సమూహం[]
	INIT(= N_("E936: Cannot delete the current group"));
EXTERN అక్షరం e_attempt_గమ్యం_delete_బఫరు_that_is_లోపల_use_str[]
	INIT(= N_("E937: Attempt to delete a buffer that is in use: %s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_duplicate_కీలకం_లోపల_json_str[]
	INIT(= N_("E938: Duplicate key in JSON: \"%s\""));
#అయితే_ముగింపు
EXTERN అక్షరం e_ధనాత్మక_లెక్క_required[]
	INIT(= N_("E939: Positive count required"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_తాళం_or_unlock_variable_str[]
	INIT(= N_("E940: Cannot lock or unlock variable %s"));
# ifdef FEAT_CLIENTSERVER
EXTERN అక్షరం e_already_started_server[]
	INIT(= N_("E941: Already started a server"));
# లేకపోతే
EXTERN అక్షరం e_clientserver_feature_not_అందుబాటు[]
	INIT(= N_("E942: +clientserver feature not available"));
# endif
#అయితే_ముగింపు
EXTERN అక్షరం e_command_పట్టిక_needs_గమ్యం_be_updated_నిరంతరం_make_cmdidxs[]
	INIT(= "E943: Command table needs to be updated, run 'make cmdidxs'");
EXTERN అక్షరం e_తిరగేయి_range_లోపల_character_class[]
	INIT(= N_("E944: Reverse range in character class"));
EXTERN అక్షరం e_range_too_large_లోపల_character_class[]
	INIT(= N_("E945: Range too large in character class"));
#నిర్వచితమైతే FEAT_TERMINAL
EXTERN అక్షరం e_cannot_make_terminal_with_running_పని_modifiable[]
	INIT(= N_("E946: Cannot make a terminal with running job modifiable"));
EXTERN అక్షరం e_పని_still_running_లోపల_బఫరు_str[]
	INIT(= N_("E947: Job still running in buffer \"%s\""));
EXTERN అక్షరం e_పని_still_running[]
	INIT(= N_("E948: Job still running"));
EXTERN అక్షరం e_పని_still_running_కూడు_bang_గమ్యం_చివర_the_పని[]
	INIT(= N_("E948: Job still running (add ! to end the job)"));
#అయితే_ముగింపు
EXTERN అక్షరం e_file_మారింది_while_writing[]
	INIT(= N_("E949: File changed while writing"));
EXTERN అక్షరం e_cannot_convert_between_str_and_str[]
	INIT(= N_("E950: Cannot convert between %s and %s"));
EXTERN అక్షరం e_percent_విలువ_too_large[]
	// xgettext:no-c-format
	INIT(= N_("E951: \\% value too large"));
#ఒకవేళ defined(FEAT_EVAL) && defined(FEAT_QUICKFIX)
EXTERN అక్షరం e_autocommand_caused_recursive_behavior[]
	INIT(= N_("E952: Autocommand caused recursive behavior"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
EXTERN అక్షరం e_file_exists_str[]
	INIT(= N_("E953: File exists: %s"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_TERMGUICOLORS) && defined(FEAT_VTP)
EXTERN అక్షరం e_24_బిట్టు_colors_are_not_supported_on_this_environment[]
	INIT(= N_("E954: 24-bit colors are not supported on this environment"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
EXTERN అక్షరం e_not_terminal_బఫరు[]
	INIT(= N_("E955: Not a terminal buffer"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_use_నమూనా_recursively[]
	INIT(= N_("E956: Cannot use pattern recursively"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_invalid_కిటికీ_సంఖ్య[]
	INIT(= N_("E957: Invalid window number"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
EXTERN అక్షరం e_పని_already_ముగిసిన[]
	INIT(= N_("E958: Job already finished"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_DIFF
EXTERN అక్షరం e_invalid_diff_format[]
	INIT(= N_("E959: Invalid diff format."));
EXTERN అక్షరం e_problem_creating_అంతర్గత_diff[]
	INIT(= N_("E960: Problem creating the internal diff"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_no_పంక్తి_సంఖ్య_గమ్యం_use_for_sflnum[]
	INIT(= N_("E961: No line number to use for \"<sflnum>\""));
EXTERN అక్షరం e_invalid_action_str_2[]
	INIT(= N_("E962: Invalid action: '%s'"));
EXTERN అక్షరం e_setting_v_str_గమ్యం_విలువ_with_wrong_type[]
	INIT(= N_("E963: Setting v:%s to value with wrong type"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PROP_POPUP) || defined(FEAT_EVAL)
EXTERN char_u e_invalid_స్తంభం_సంఖ్య_nr[]
	INIT(= N_("E964: Invalid column number: %ld"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_missing_property_type_name[]
	INIT(= N_("E965: Missing property type name"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN char_u e_invalid_పంక్తి_సంఖ్య_nr[]
	INIT(= N_("E966: Invalid line number: %ld"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_పాఠ్యం_property_info_corrupted[]
	INIT(= "E967: Text property info corrupted");
EXTERN అక్షరం e_need_at_కనిష్ఠ_one_యొక్క_id_or_type[]
	INIT(= N_("E968: Need at least one of 'id' or 'type'"));
EXTERN అక్షరం e_property_type_str_already_defined[]
	INIT(= N_("E969: Property type %s already defined"));
EXTERN అక్షరం e_unknown_highlight_సమూహం_name_str[]
	INIT(= N_("E970: Unknown highlight group name: '%s'"));
EXTERN అక్షరం e_property_type_str_does_not_exist[]
	INIT(= N_("E971: Property type %s does not exist"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_blob_విలువ_does_not_have_కుడి_సంఖ్య_యొక్క_బైట్లు[]
	INIT(= N_("E972: Blob value does not have the right number of bytes"));
EXTERN అక్షరం e_blob_literal_should_have_an_even_సంఖ్య_యొక్క_hex_characters[]
	INIT(= N_("E973: Blob literal should have an even number of hex characters"));
EXTERN అక్షరం e_using_blob_as_సంఖ్య[]
	INIT(= N_("E974: Using a Blob as a Number"));
EXTERN అక్షరం e_using_blob_as_float[]
	INIT(= N_("E975: Using a Blob as a Float"));
EXTERN అక్షరం e_using_blob_as_string[]
	INIT(= N_("E976: Using a Blob as a String"));
EXTERN అక్షరం e_can_only_compare_blob_with_blob[]
	INIT(= N_("E977: Can only compare Blob with Blob"));
EXTERN అక్షరం e_invalid_operation_for_blob[]
	INIT(= N_("E978: Invalid operation for Blob"));
EXTERN అక్షరం e_blob_సూచీ_బహిర్గతం_యొక్క_range_nr[]
	INIT(= N_("E979: Blob index out of range: %ld"));
# ifndef USE_ఆదానం_BUF
EXTERN అక్షరం e_lowlevel_ఆదానం_not_supported[]
	INIT(= N_("E980: Lowlevel input not supported"));
# endif
#అయితే_ముగింపు
EXTERN అక్షరం e_command_not_allowed_లోపల_rvim[]
	INIT(= N_("E981: Command not allowed in rvim"));
#ఒకవేళ defined(FEAT_TERMINAL) && defined(MSWIN)
EXTERN అక్షరం e_conpty_is_not_అందుబాటు[]
	INIT(= N_("E982: ConPTY is not available"));
#అయితే_ముగింపు
EXTERN అక్షరం e_duplicate_వాదం_str[]
	INIT(= N_("E983: Duplicate argument: %s"));
EXTERN అక్షరం e_scriptversion_వాడిన_outside_యొక్క_sourced_file[]
	INIT(= N_("E984: :scriptversion used outside of a sourced file"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_dot_equal_not_supported_with_script_సంస్కరణ_two[]
	INIT(= N_("E985: .= is not supported with script version >= 2"));
EXTERN అక్షరం e_cannot_modify_ట్యాగ్_దొంతర_within_tagfunc[]
	INIT(= N_("E986: Cannot modify the tag stack within tagfunc"));
EXTERN అక్షరం e_invalid_return_విలువ_from_tagfunc[]
	INIT(= N_("E987: Invalid return value from tagfunc"));
#అయితే_ముగింపు
#నిర్వచితమైతే GUI_MAY_SPAWN
EXTERN అక్షరం e_gui_cannot_be_వాడిన_cannot_execute_gvim_exe[]
	INIT(= N_("E988: GUI cannot be used. Cannot execute gvim.exe."));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_non_default_వాదం_follows_default_వాదం[]
	INIT(= N_("E989: Non-default argument follows default argument"));
EXTERN అక్షరం e_missing_చివర_marker_str[]
	INIT(= N_("E990: Missing end marker '%s'"));
EXTERN అక్షరం e_cannot_use_heredoc_ఇక్కడ[]
	INIT(= N_("E991: Cannot use =<< here"));
#అయితే_ముగింపు
EXTERN అక్షరం e_not_allowed_లోపల_modeline_when_modelineexpr_is_off[]
	INIT(= N_("E992: Not allowed in a modeline when 'modelineexpr' is off"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_కిటికీ_nr_is_not_popup_కిటికీ[]
	INIT(= N_("E993: Window %d is not a popup window"));
EXTERN అక్షరం e_not_allowed_లోపల_popup_కిటికీ[]
	INIT(= N_("E994: Not allowed in a popup window"));
EXTERN అక్షరం e_cannot_modify_existing_variable[]
	INIT(= N_("E995: Cannot modify existing variable"));
EXTERN అక్షరం e_cannot_తాళం_range[]
	INIT(= N_("E996: Cannot lock a range"));
EXTERN అక్షరం e_cannot_తాళం_option[]
	INIT(= N_("E996: Cannot lock an option"));
EXTERN అక్షరం e_cannot_తాళం_list_or_dict[]
	INIT(= N_("E996: Cannot lock a list or dict"));
EXTERN అక్షరం e_cannot_తాళం_environment_variable[]
	INIT(= N_("E996: Cannot lock an environment variable"));
EXTERN అక్షరం e_cannot_తాళం_రిజిస్టరు[]
	INIT(= N_("E996: Cannot lock a register"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_tabpage_not_దొరికింది_nr[]
	INIT(= N_("E997: Tabpage not found: %d"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_reduce_యొక్క_an_ఖాళీలు_str_with_no_initial_విలువ[]
	INIT(= N_("E998: Reduce of an empty %s with no initial value"));
#అయితే_ముగింపు
EXTERN అక్షరం e_scriptversion_not_supported_nr[]
	INIT(= N_("E999: scriptversion not supported: %d"));
// E1000 unused
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_variable_not_దొరికింది_str[]
	INIT(= N_("E1001: Variable not found: %s"));
EXTERN అక్షరం e_syntax_error_at_str[]
	INIT(= N_("E1002: Syntax error at %s"));
EXTERN అక్షరం e_missing_return_విలువ[]
	INIT(= N_("E1003: Missing return value"));
EXTERN అక్షరం e_white_space_required_before_and_after_str_at_str[]
	INIT(= N_("E1004: White space required before and after '%s' at \"%s\""));
EXTERN అక్షరం e_too_many_వాదం_types[]
	INIT(= N_("E1005: Too many argument types"));
EXTERN అక్షరం e_str_is_వాడిన_as_వాదం[]
	INIT(= N_("E1006: %s is used as an argument"));
EXTERN అక్షరం e_mandatory_వాదం_after_optional_వాదం[]
	INIT(= N_("E1007: Mandatory argument after optional argument"));
EXTERN అక్షరం e_missing_type_after_str[]
	INIT(= N_("E1008: Missing <type> after %s"));
EXTERN అక్షరం e_missing_gt_after_type_str[]
	INIT(= N_("E1009: Missing > after type: %s"));
EXTERN అక్షరం e_type_not_recognized_str[]
	INIT(= N_("E1010: Type not recognized: %s"));
EXTERN అక్షరం e_name_too_long_str[]
	INIT(= N_("E1011: Name too long: %s"));
EXTERN అక్షరం e_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1012: Type mismatch; expected %s but got %s"));
EXTERN అక్షరం e_type_mismatch_expected_str_but_got_str_లోపల_str[]
	INIT(= N_("E1012: Type mismatch; expected %s but got %s in %s"));
EXTERN అక్షరం e_వాదం_nr_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1013: Argument %d: type mismatch, expected %s but got %s"));
EXTERN అక్షరం e_వాదం_nr_type_mismatch_expected_str_but_got_str_లోపల_str[]
	INIT(= N_("E1013: Argument %d: type mismatch, expected %s but got %s in %s"));
EXTERN అక్షరం e_invalid_కీలకం_str[]
	INIT(= N_("E1014: Invalid key: %s"));
EXTERN అక్షరం e_name_expected_str[]
	INIT(= N_("E1015: Name expected: %s"));
EXTERN అక్షరం e_cannot_declare_a_scope_variable_str[]
	INIT(= N_("E1016: Cannot declare a %s variable: %s"));
EXTERN అక్షరం e_cannot_declare_an_environment_variable_str[]
	INIT(= N_("E1016: Cannot declare an environment variable: %s"));
EXTERN అక్షరం e_variable_already_declared_str[]
	INIT(= N_("E1017: Variable already declared: %s"));
EXTERN అక్షరం e_cannot_కేటాయించు_గమ్యం_constant_str[]
	INIT(= N_("E1018: Cannot assign to a constant: %s"));
EXTERN అక్షరం e_can_only_concatenate_గమ్యం_string[]
	INIT(= N_("E1019: Can only concatenate to string"));
EXTERN అక్షరం e_cannot_use_operator_on_new_variable_str[]
	INIT(= N_("E1020: Cannot use an operator on a new variable: %s"));
EXTERN అక్షరం e_const_requires_a_విలువ[]
	INIT(= N_("E1021: Const requires a value"));
EXTERN అక్షరం e_type_or_initialization_required[]
	INIT(= N_("E1022: Type or initialization required"));
EXTERN అక్షరం e_using_సంఖ్య_as_bool_nr[]
	INIT(= N_("E1023: Using a Number as a Bool: %lld"));
EXTERN అక్షరం e_using_సంఖ్య_as_string[]
	INIT(= N_("E1024: Using a Number as a String"));
EXTERN అక్షరం e_using_rcurly_outside_if_ఖండం_scope[]
	INIT(= N_("E1025: Using } outside of a block scope"));
#అయితే_ముగింపు
EXTERN అక్షరం e_missing_rcurly[]
	INIT(= N_("E1026: Missing }"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_missing_return_statement[]
	INIT(= N_("E1027: Missing return statement"));
EXTERN అక్షరం e_compiling_def_function_failed[]
	INIT(= N_("E1028: Compiling :def function failed"));
EXTERN అక్షరం e_expected_str_but_got_str[]
	INIT(= N_("E1029: Expected %s but got %s"));
EXTERN అక్షరం e_using_string_as_సంఖ్య_str[]
	INIT(= N_("E1030: Using a String as a Number: \"%s\""));
EXTERN అక్షరం e_cannot_use_void_విలువ[]
	INIT(= N_("E1031: Cannot use void value"));
EXTERN అక్షరం e_missing_catch_or_finally[]
	INIT(= N_("E1032: Missing :catch or :finally"));
EXTERN అక్షరం e_catch_unreachable_after_catch_all[]
	INIT(= N_("E1033: Catch unreachable after catch-all"));
EXTERN అక్షరం e_cannot_use_reserved_name_str[]
	INIT(= N_("E1034: Cannot use reserved name %s"));
EXTERN అక్షరం e_percent_requires_సంఖ్య_arguments[]
	// xgettext:no-c-format
	INIT(= N_("E1035: % requires number arguments"));
EXTERN అక్షరం e_char_requires_సంఖ్య_or_float_arguments[]
	INIT(= N_("E1036: %c requires number or float arguments"));
EXTERN అక్షరం e_cannot_use_str_with_str[]
	INIT(= N_("E1037: Cannot use \"%s\" with %s"));
EXTERN అక్షరం e_vim9script_can_only_be_వాడిన_లోపల_script[]
	INIT(= N_("E1038: \"vim9script\" can only be used in a script"));
EXTERN అక్షరం e_vim9script_must_be_మొదటి_command_లోపల_script[]
	INIT(= N_("E1039: \"vim9script\" must be the first command in a script"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_use_scriptversion_after_vim9script[]
	INIT(= N_("E1040: Cannot use :scriptversion after :vim9script"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_redefining_script_అంశం_str[]
	INIT(= N_("E1041: Redefining script item: \"%s\""));
EXTERN అక్షరం e_export_can_only_be_వాడిన_లోపల_vim9script[]
	INIT(= N_("E1042: Export can only be used in vim9script"));
EXTERN అక్షరం e_invalid_command_after_export[]
	INIT(= N_("E1043: Invalid command after :export"));
EXTERN అక్షరం e_export_with_invalid_వాదం[]
	INIT(= N_("E1044: Export with invalid argument"));
// E1045 not used
// E1046 not used
EXTERN అక్షరం e_syntax_error_లోపల_import_str[]
	INIT(= N_("E1047: Syntax error in import: %s"));
EXTERN అక్షరం e_అంశం_not_దొరికింది_లోపల_script_str[]
	INIT(= N_("E1048: Item not found in script: %s"));
EXTERN అక్షరం e_అంశం_not_exported_లోపల_script_str[]
	INIT(= N_("E1049: Item not exported in script: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_colon_required_before_range_str[]
	INIT(= N_("E1050: Colon required before a range: %s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_wrong_వాదం_type_for_plus[]
	INIT(= N_("E1051: Wrong argument type for +"));
EXTERN అక్షరం e_cannot_declare_an_option_str[]
	INIT(= N_("E1052: Cannot declare an option: %s"));
EXTERN అక్షరం e_could_not_import_str[]
	INIT(= N_("E1053: Could not import \"%s\""));
EXTERN అక్షరం e_variable_already_declared_లోపల_script_str[]
	INIT(= N_("E1054: Variable already declared in the script: %s"));
EXTERN అక్షరం e_missing_name_after_dots[]
	INIT(= N_("E1055: Missing name after ..."));
EXTERN అక్షరం e_expected_type_str[]
	INIT(= N_("E1056: Expected a type: %s"));
EXTERN అక్షరం e_missing_enddef[]
	INIT(= N_("E1057: Missing :enddef"));
EXTERN అక్షరం e_function_nesting_too_deep[]
	INIT(= N_("E1058: Function nesting too deep"));
EXTERN అక్షరం e_no_white_space_allowed_before_colon_str[]
	INIT(= N_("E1059: No white space allowed before colon: %s"));
EXTERN అక్షరం e_expected_dot_after_name_str[]
	INIT(= N_("E1060: Expected dot after name: %s"));
EXTERN అక్షరం e_cannot_కనుగొను_function_str[]
	INIT(= N_("E1061: Cannot find function %s"));
EXTERN అక్షరం e_cannot_సూచీ_సంఖ్య[]
	INIT(= N_("E1062: Cannot index a Number"));
EXTERN అక్షరం e_type_mismatch_for_v_variable[]
	INIT(= N_("E1063: Type mismatch for v: variable"));
#అయితే_ముగింపు
EXTERN అక్షరం e_yank_రిజిస్టరు_మారింది_while_using_it[]
	INIT(= N_("E1064: Yank register changed while using it"));
EXTERN అక్షరం e_command_cannot_be_shortened_str[]
	INIT(= N_("E1065: Command cannot be shortened: %s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_declare_a_రిజిస్టరు_str[]
	INIT(= N_("E1066: Cannot declare a register: %s"));
EXTERN అక్షరం e_separator_mismatch_str[]
	INIT(= N_("E1067: Separator mismatch: %s"));
EXTERN అక్షరం e_no_white_space_allowed_before_str_str[]
	INIT(= N_("E1068: No white space allowed before '%s': %s"));
EXTERN అక్షరం e_white_space_required_after_str_str[]
	INIT(= N_("E1069: White space required after '%s': %s"));
EXTERN అక్షరం e_invalid_string_for_import_str[]
	INIT(= N_("E1071: Invalid string for :import: %s"));
EXTERN అక్షరం e_cannot_compare_str_with_str[]
	INIT(= N_("E1072: Cannot compare %s with %s"));
EXTERN అక్షరం e_name_already_defined_str[]
	INIT(= N_("E1073: Name already defined: %s"));
EXTERN అక్షరం e_no_white_space_allowed_after_dot[]
	INIT(= N_("E1074: No white space allowed after dot"));
EXTERN అక్షరం e_namespace_not_supported_str[]
	INIT(= N_("E1075: Namespace not supported: %s"));
// E1076 unused (was deleted)
EXTERN అక్షరం e_missing_వాదం_type_for_str[]
	INIT(= N_("E1077: Missing argument type for %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_invalid_command_nested_did_you_సగటు_plusplus_nested[]
	INIT(= N_("E1078: Invalid command \"nested\", did you mean \"++nested\"?"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_declare_variable_on_command_పంక్తి[]
	INIT(= N_("E1079: Cannot declare a variable on the command line"));
EXTERN అక్షరం e_invalid_assignment[]
	INIT(= N_("E1080: Invalid assignment"));
EXTERN అక్షరం e_cannot_unlet_str[]
	INIT(= N_("E1081: Cannot unlet %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_command_modifier_without_command[]
	INIT(= N_("E1082: Command modifier without command"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_missing_backtick[]
	INIT(= N_("E1083: Missing backtick"));
EXTERN అక్షరం e_cannot_delete_vim9_script_function_str[]
	INIT(= N_("E1084: Cannot delete Vim9 script function %s"));
EXTERN అక్షరం e_not_callable_type_str[]
	INIT(= N_("E1085: Not a callable type: %s"));
// E1086 unused
EXTERN అక్షరం e_cannot_use_సూచీ_when_declaring_variable[]
	INIT(= N_("E1087: Cannot use an index when declaring a variable"));
EXTERN అక్షరం e_script_cannot_import_itself[]
	INIT(= N_("E1088: Script cannot import itself"));
EXTERN అక్షరం e_unknown_variable_str[]
	INIT(= N_("E1089: Unknown variable: %s"));
EXTERN అక్షరం e_cannot_కేటాయించు_గమ్యం_వాదం_str[]
	INIT(= N_("E1090: Cannot assign to argument %s"));
EXTERN అక్షరం e_function_is_not_compiled_str[]
	INIT(= N_("E1091: Function is not compiled: %s"));
EXTERN అక్షరం e_cannot_nest_redir[]
	INIT(= N_("E1092: Cannot nest :redir"));
EXTERN అక్షరం e_expected_nr_వస్తువులు_but_got_nr[]
	INIT(= N_("E1093: Expected %d items but got %d"));
EXTERN అక్షరం e_import_can_only_be_వాడిన_లోపల_script[]
	INIT(= N_("E1094: Import can only be used in a script"));
EXTERN అక్షరం e_unreachable_సంకేతం_after_str[]
	INIT(= N_("E1095: Unreachable code after :%s"));
EXTERN అక్షరం e_returning_విలువ_లోపల_function_without_return_type[]
	INIT(= N_("E1096: Returning a value in a function without a return type"));
EXTERN అక్షరం e_పంక్తి_incomplete[]
	INIT(= N_("E1097: Line incomplete"));
EXTERN అక్షరం e_string_list_or_blob_required[]
	INIT(= N_("E1098: String, List or Blob required"));
EXTERN అక్షరం e_unknown_error_while_executing_str[]
	INIT(= N_("E1099: Unknown error while executing %s"));
EXTERN అక్షరం e_command_not_supported_లోపల_vim9_script_missing_var_str[]
	INIT(= N_("E1100: Command not supported in Vim9 script (missing :var?): %s"));
EXTERN అక్షరం e_cannot_declare_script_variable_లోపల_function_str[]
	INIT(= N_("E1101: Cannot declare a script variable in a function: %s"));
EXTERN అక్షరం e_lambda_function_not_దొరికింది_str[]
	INIT(= N_("E1102: Lambda function not found: %s"));
EXTERN అక్షరం e_dictionary_not_సమితి[]
	INIT(= N_("E1103: Dictionary not set"));
EXTERN అక్షరం e_missing_gt[]
	INIT(= N_("E1104: Missing >"));
EXTERN అక్షరం e_cannot_convert_str_గమ్యం_string[]
	INIT(= N_("E1105: Cannot convert %s to string"));

PLURAL_సందేశం(e_one_వాదం_too_many, "E1106: One argument too many",
		e_nr_arguments_too_many, "E1106: %d arguments too many")

EXTERN అక్షరం e_string_list_dict_or_blob_required[]
	INIT(= N_("E1107: String, List, Dict or Blob required"));
// E1108 unused
EXTERN అక్షరం e_list_అంశం_nr_is_not_list[]
	INIT(= N_("E1109: List item %d is not a List"));
EXTERN అక్షరం e_list_అంశం_nr_does_not_contain_3_numbers[]
	INIT(= N_("E1110: List item %d does not contain 3 numbers"));
EXTERN అక్షరం e_list_అంశం_nr_range_invalid[]
	INIT(= N_("E1111: List item %d range invalid"));
EXTERN అక్షరం e_list_అంశం_nr_cell_వెడల్పు_invalid[]
	INIT(= N_("E1112: List item %d cell width invalid"));
EXTERN అక్షరం e_overlapping_ranges_for_nr[]
	INIT(= N_("E1113: Overlapping ranges for 0x%lx"));
EXTERN అక్షరం e_only_విలువలు_యొక్క_0x80_and_higher_supported[]
	INIT(= N_("E1114: Only values of 0x80 and higher supported"));
EXTERN అక్షరం e_assert_fails_fourth_వాదం[]
	INIT(= N_("E1115: \"assert_fails()\" fourth argument must be a number"));
EXTERN అక్షరం e_assert_fails_fifth_వాదం[]
	INIT(= N_("E1116: \"assert_fails()\" fifth argument must be a string"));
EXTERN అక్షరం e_cannot_use_bang_with_nested_def_str[]
	INIT(= N_("E1117: Cannot use ! with nested %s"));
EXTERN అక్షరం e_cannot_change_locked_list[]
	INIT(= N_("E1118: Cannot change locked list"));
EXTERN అక్షరం e_cannot_change_locked_list_అంశం[]
	INIT(= N_("E1119: Cannot change locked list item"));
EXTERN అక్షరం e_cannot_change_dict[]
	INIT(= N_("E1120: Cannot change dict"));
EXTERN అక్షరం e_cannot_change_dict_అంశం[]
	INIT(= N_("E1121: Cannot change dict item"));
EXTERN అక్షరం e_variable_is_locked_str[]
	INIT(= N_("E1122: Variable is locked: %s"));
EXTERN అక్షరం e_missing_comma_before_వాదం_str[]
	INIT(= N_("E1123: Missing comma before argument: %s"));
EXTERN అక్షరం e_str_cannot_be_వాడిన_లోపల_legacy_vim_script[]
	INIT(= N_("E1124: \"%s\" cannot be used in legacy Vim script"));
EXTERN అక్షరం e_final_requires_a_విలువ[]
	INIT(= N_("E1125: Final requires a value"));
EXTERN అక్షరం e_cannot_use_let_లోపల_vim9_script[]
	INIT(= N_("E1126: Cannot use :let in Vim9 script"));
EXTERN అక్షరం e_missing_name_after_dot[]
	INIT(= N_("E1127: Missing name after dot"));
EXTERN అక్షరం e_endblock_without_ఖండం[]
	INIT(= N_("E1128: } without {"));
EXTERN అక్షరం e_throw_with_ఖాళీలు_string[]
	INIT(= N_("E1129: Throw with empty string"));
EXTERN అక్షరం e_cannot_కూడు_గమ్యం_null_list[]
	INIT(= N_("E1130: Cannot add to null list"));
EXTERN అక్షరం e_cannot_కూడు_గమ్యం_null_blob[]
	INIT(= N_("E1131: Cannot add to null blob"));
EXTERN అక్షరం e_missing_function_వాదం[]
	INIT(= N_("E1132: Missing function argument"));
EXTERN అక్షరం e_cannot_extend_null_dict[]
	INIT(= N_("E1133: Cannot extend a null dict"));
EXTERN అక్షరం e_cannot_extend_null_list[]
	INIT(= N_("E1134: Cannot extend a null list"));
EXTERN అక్షరం e_using_string_as_bool_str[]
	INIT(= N_("E1135: Using a String as a Bool: \"%s\""));
#అయితే_ముగింపు
EXTERN అక్షరం e_cmd_mapping_must_చివర_with_cr_before_రెండవ_cmd[]
	INIT(= N_("E1136: <Cmd> mapping must end with <CR> before second <Cmd>"));
// E1137 unused
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_using_bool_as_సంఖ్య[]
	INIT(= N_("E1138: Using a Bool as a Number"));
EXTERN అక్షరం e_missing_matching_bracket_after_dict_కీలకం[]
	INIT(= N_("E1139: Missing matching bracket after dict key"));
EXTERN అక్షరం e_for_వాదం_must_be_sequence_యొక్క_lists_or_tuples[]
	INIT(= N_("E1140: :for argument must be a sequence of lists or tuples"));
EXTERN అక్షరం e_indexable_type_required[]
	INIT(= N_("E1141: Indexable type required"));
EXTERN అక్షరం e_calling_test_garbagecollect_now_while_v_testing_is_not_సమితి[]
	INIT(= N_("E1142: Calling test_garbagecollect_now() while v:testing is not set"));
EXTERN అక్షరం e_ఖాళీలు_వ్యక్తీకరణ_str[]
	INIT(= N_("E1143: Empty expression: \"%s\""));
EXTERN అక్షరం e_command_str_not_followed_by_white_space_str[]
	INIT(= N_("E1144: Command \"%s\" is not followed by white space: %s"));
EXTERN అక్షరం e_missing_heredoc_చివర_marker_str[]
	INIT(= N_("E1145: Missing heredoc end marker: %s"));
EXTERN అక్షరం e_command_not_recognized_str[]
	INIT(= N_("E1146: Command not recognized: %s"));
EXTERN అక్షరం e_list_not_సమితి[]
	INIT(= N_("E1147: List not set"));
EXTERN అక్షరం e_cannot_సూచీ_str[]
	INIT(= N_("E1148: Cannot index a %s"));
EXTERN అక్షరం e_script_variable_invalid_after_reload_లోపల_function_str[]
	INIT(= N_("E1149: Script variable is invalid after reload in function %s"));
EXTERN అక్షరం e_script_variable_type_మారింది[]
	INIT(= N_("E1150: Script variable type changed"));
EXTERN అక్షరం e_mismatched_endfunction[]
	INIT(= N_("E1151: Mismatched endfunction"));
EXTERN అక్షరం e_mismatched_enddef[]
	INIT(= N_("E1152: Mismatched enddef"));
EXTERN అక్షరం e_invalid_operation_for_str[]
	INIT(= N_("E1153: Invalid operation for %s"));
EXTERN అక్షరం e_divide_by_zero[]
	INIT(= N_("E1154: Divide by zero"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_define_autocommands_for_all_events[]
	INIT(= N_("E1155: Cannot define autocommands for ALL events"));
EXTERN అక్షరం e_cannot_change_arglist_recursively[]
	INIT(= N_("E1156: Cannot change the argument list recursively"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_missing_return_type[]
	INIT(= N_("E1157: Missing return type"));
EXTERN అక్షరం e_cannot_use_flatten_లోపల_vim9_script[]
	INIT(= N_("E1158: Cannot use flatten() in Vim9 script, use flattennew()"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_విభజించు_కిటికీ_when_closing_బఫరు[]
	INIT(= N_("E1159: Cannot split a window when closing the buffer"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_use_default_for_variable_arguments[]
	INIT(= N_("E1160: Cannot use a default for variable arguments"));
EXTERN అక్షరం e_cannot_json_encode_str[]
	INIT(= N_("E1161: Cannot json encode a %s"));
EXTERN అక్షరం e_రిజిస్టరు_name_must_be_one_char_str[]
	INIT(= N_("E1162: Register name must be one character: %s"));
EXTERN అక్షరం e_variable_nr_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1163: Variable %d: type mismatch, expected %s but got %s"));
EXTERN అక్షరం e_variable_nr_type_mismatch_expected_str_but_got_str_లోపల_str[]
	INIT(= N_("E1163: Variable %d: type mismatch, expected %s but got %s in %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_vim9cmd_must_be_followed_by_command[]
	INIT(= N_("E1164: vim9cmd must be followed by a command"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_use_range_with_assignment_str[]
	INIT(= N_("E1165: Cannot use a range with an assignment: %s"));
EXTERN అక్షరం e_cannot_use_range_with_dictionary[]
	INIT(= N_("E1166: Cannot use a range with a dictionary"));
EXTERN అక్షరం e_వాదం_name_shadows_existing_variable_str[]
	INIT(= N_("E1167: Argument name shadows existing variable: %s"));
EXTERN అక్షరం e_వాదం_already_declared_లోపల_script_str[]
	INIT(= N_("E1168: Argument already declared in the script: %s"));
EXTERN అక్షరం e_వ్యక్తీకరణ_too_recursive_str[]
	INIT(= N_("E1169: Expression too recursive: %s"));
EXTERN అక్షరం e_cannot_use_hash_curly_గమ్యం_ప్రారంభం_comment[]
	INIT(= N_("E1170: Cannot use #{ to start a comment"));
EXTERN అక్షరం e_missing_చివర_ఖండం[]
	INIT(= N_("E1171: Missing } after inline function"));
EXTERN అక్షరం e_cannot_use_default_విలువలు_లోపల_lambda[]
	INIT(= N_("E1172: Cannot use default values in a lambda"));
EXTERN అక్షరం e_పాఠ్యం_దొరికింది_after_str_str[]
	INIT(= N_("E1173: Text found after %s: %s"));
EXTERN అక్షరం e_string_required_for_వాదం_nr[]
	INIT(= N_("E1174: String required for argument %d"));
EXTERN అక్షరం e_non_ఖాళీలు_string_required_for_వాదం_nr[]
	INIT(= N_("E1175: Non-empty string required for argument %d"));
EXTERN అక్షరం e_misplaced_command_modifier[]
	INIT(= N_("E1176: Misplaced command modifier"));
EXTERN అక్షరం e_for_loop_on_str_not_supported[]
	INIT(= N_("E1177: For loop on %s not supported"));
EXTERN అక్షరం e_cannot_తాళం_unlock_local_variable[]
	INIT(= N_("E1178: Cannot lock or unlock a local variable"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
EXTERN అక్షరం e_failed_గమ్యం_వెలికితీయి_pwd_from_str_check_your_shell_config[]
	INIT(= N_("E1179: Failed to extract PWD from %s, check your shell's config related to OSC 7"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_variable_arguments_type_must_be_list_str[]
	INIT(= N_("E1180: Variable arguments type must be a list: %s"));
EXTERN అక్షరం e_cannot_use_underscore_ఇక్కడ[]
	INIT(= N_("E1181: Cannot use an underscore here"));
EXTERN అక్షరం e_cannot_define_dict_func_లోపల_vim9_script_str[]
	INIT(= N_("E1182: Cannot define a dict function in Vim9 script: %s"));
EXTERN అక్షరం e_cannot_use_range_with_assignment_operator_str[]
	INIT(= N_("E1183: Cannot use a range with an assignment operator: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_blob_not_సమితి[]
	INIT(= N_("E1184: Blob not set"));
EXTERN అక్షరం e_missing_redir_చివర[]
	INIT(= N_("E1185: Missing :redir END"));
EXTERN అక్షరం e_వ్యక్తీకరణ_does_not_ఫలితం_లోపల_విలువ_str[]
	INIT(= N_("E1186: Expression does not result in a value: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_failed_గమ్యం_ఆధారం_defaults[]
	INIT(= N_("E1187: Failed to source defaults.vim"));
#ఒకవేళ defined(FEAT_TERMINAL)
EXTERN అక్షరం e_cannot_open_terminal_from_command_పంక్తి_కిటికీ[]
	INIT(= N_("E1188: Cannot open a terminal from the command line window"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_use_legacy_with_command_str[]
	INIT(= N_("E1189: Cannot use :legacy with this command: %s"));

PLURAL_సందేశం(e_one_వాదం_too_few, "E1190: One argument too few",
		e_nr_arguments_too_few, "E1190: %d arguments too few")

EXTERN అక్షరం e_call_గమ్యం_function_that_failed_గమ్యం_compile_str[]
	INIT(= N_("E1191: Call to function that failed to compile: %s"));
EXTERN అక్షరం e_ఖాళీలు_function_name[]
	INIT(= N_("E1192: Empty function name"));
#అయితే_ముగింపు
// libsodium
#నిర్వచితమైతే FEAT_CRYPT
# ifndef FEAT_SODIUM
EXTERN అక్షరం e_libsodium_not_built_లోపల[]
	INIT(= N_("E1193: cryptmethod xchacha20 not built into this Vim"));
# లేకపోతే
#  ఒకవేళ 0
EXTERN అక్షరం e_libsodium_cannot_encrypt_శీర్షిక[]
	INIT(= N_("E1194: Cannot encrypt header, not enough space"));
EXTERN అక్షరం e_libsodium_cannot_encrypt_బఫరు[]
	INIT(= N_("E1195: Cannot encrypt buffer, not enough space"));
EXTERN అక్షరం e_libsodium_cannot_decrypt_శీర్షిక[]
	INIT(= N_("E1196: Cannot decrypt header, not enough space"));
#  endif
EXTERN అక్షరం e_libsodium_cannot_allocate_బఫరు[]
	INIT(= N_("E1197: Cannot allocate_buffer for encryption"));
EXTERN అక్షరం e_libsodium_decryption_failed_శీర్షిక_incomplete[]
	INIT(= N_("E1198: Decryption failed: Header incomplete!"));
#  ఒకవేళ 0
EXTERN అక్షరం e_libsodium_cannot_decrypt_బఫరు[]
	INIT(= N_("E1199: Cannot decrypt buffer, not enough space"));
#  endif
EXTERN అక్షరం e_libsodium_decryption_failed[]
	INIT(= N_("E1200: Decryption failed!"));
EXTERN అక్షరం e_libsodium_decryption_failed_premature[]
	INIT(= N_("E1201: Decryption failed: pre-mature end of file!"));
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_no_white_space_allowed_after_str_str[]
	INIT(= N_("E1202: No white space allowed after '%s': %s"));
EXTERN అక్షరం e_dot_not_allowed_after_str_str[]
	INIT(= N_("E1203: Dot not allowed after a %s: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_regexp_సంఖ్య_after_dot_pos_శోధన_chr[]
	INIT(= N_("E1204: No Number allowed after .: '\\%%%c'"));
EXTERN అక్షరం e_no_white_space_allowed_between_option_and[]
	INIT(= N_("E1205: No white space allowed between option and"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_dict_required_for_వాదం_nr[]
	INIT(= N_("E1206: Dictionary required for argument %d"));
EXTERN అక్షరం e_వ్యక్తీకరణ_without_effect_str[]
	INIT(= N_("E1207: Expression without an effect: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_complete_వాడిన_without_allowing_arguments[]
	INIT(= N_("E1208: -complete used without allowing arguments"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_invalid_విలువ_for_పంక్తి_సంఖ్య_str[]
	INIT(= N_("E1209: Invalid value for a line number: \"%s\""));
EXTERN అక్షరం e_సంఖ్య_required_for_వాదం_nr[]
	INIT(= N_("E1210: Number required for argument %d"));
EXTERN అక్షరం e_list_required_for_వాదం_nr[]
	INIT(= N_("E1211: List required for argument %d"));
EXTERN అక్షరం e_bool_required_for_వాదం_nr[]
	INIT(= N_("E1212: Bool required for argument %d"));
EXTERN అక్షరం e_redefining_imported_అంశం_str[]
	INIT(= N_("E1213: Redefining imported item \"%s\""));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_DIGRAPHS)
EXTERN అక్షరం e_digraph_must_be_just_two_characters_str[]
	INIT(= N_("E1214: Digraph must be just two characters: %s"));
EXTERN అక్షరం e_digraph_వాదం_must_be_one_character_str[]
	INIT(= N_("E1215: Digraph must be one character: %s"));
EXTERN అక్షరం e_digraph_setlist_వాదం_must_be_list_యొక్క_lists_with_two_వస్తువులు[]
	INIT(= N_("E1216: digraph_setlist() argument must be a list of lists with two items"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
# ifdef FEAT_పని_CHANNEL
EXTERN అక్షరం e_chan_or_పని_required_for_వాదం_nr[]
	INIT(= N_("E1217: Channel or Job required for argument %d"));
EXTERN అక్షరం e_పని_required_for_వాదం_nr[]
	INIT(= N_("E1218: Job required for argument %d"));
# endif
EXTERN అక్షరం e_float_or_సంఖ్య_required_for_వాదం_nr[]
	INIT(= N_("E1219: Float or Number required for argument %d"));
EXTERN అక్షరం e_string_or_సంఖ్య_required_for_వాదం_nr[]
	INIT(= N_("E1220: String or Number required for argument %d"));
EXTERN అక్షరం e_string_or_blob_required_for_వాదం_nr[]
	INIT(= N_("E1221: String or Blob required for argument %d"));
EXTERN అక్షరం e_string_or_list_required_for_వాదం_nr[]
	INIT(= N_("E1222: String or List required for argument %d"));
EXTERN అక్షరం e_string_or_dict_required_for_వాదం_nr[]
	INIT(= N_("E1223: String or Dictionary required for argument %d"));
EXTERN అక్షరం e_string_సంఖ్య_or_list_required_for_వాదం_nr[]
	INIT(= N_("E1224: String, Number or List required for argument %d"));
EXTERN అక్షరం e_string_list_tuple_or_dict_required_for_వాదం_nr[]
	INIT(= N_("E1225: String, List, Tuple or Dictionary required for argument %d"));
EXTERN అక్షరం e_list_or_blob_required_for_వాదం_nr[]
	INIT(= N_("E1226: List or Blob required for argument %d"));
EXTERN అక్షరం e_list_or_dict_required_for_వాదం_nr[]
	INIT(= N_("E1227: List or Dictionary required for argument %d"));
EXTERN అక్షరం e_list_dict_or_blob_required_for_వాదం_nr[]
	INIT(= N_("E1228: List, Dictionary or Blob required for argument %d"));
EXTERN అక్షరం e_expected_dictionary_for_using_కీలకం_str_but_got_str[]
	INIT(= N_("E1229: Expected dictionary for using key \"%s\", but got %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SODIUM
EXTERN అక్షరం e_encryption_sodium_mlock_failed[]
	INIT(= N_("E1230: Encryption: sodium_mlock() failed"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_use_bar_గమ్యం_separate_commands_ఇక్కడ_str[]
	INIT(= N_("E1231: Cannot use a bar to separate commands here: %s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_వాదం_యొక్క_exists_compiled_must_be_literal_string[]
	INIT(= N_("E1232: Argument of exists_compiled() must be a literal string"));
EXTERN అక్షరం e_exists_compiled_can_only_be_వాడిన_లోపల_def_function[]
	INIT(= N_("E1233: exists_compiled() can only be used in a :def function"));
#అయితే_ముగింపు
EXTERN అక్షరం e_legacy_must_be_followed_by_command[]
	INIT(= N_("E1234: legacy must be followed by a command"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_bool_or_సంఖ్య_required_for_వాదం_nr[]
	INIT(= N_("E1235: Bool or Number required for argument %d"));
EXTERN అక్షరం e_cannot_use_str_itself_it_is_imported[]
	INIT(= N_("E1236: Cannot use %s itself, it is imported"));
#అయితే_ముగింపు
EXTERN అక్షరం e_no_such_user_defined_command_లోపల_current_బఫరు_str[]
	INIT(= N_("E1237: No such user-defined command in current buffer: %s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_blob_required_for_వాదం_nr[]
	INIT(= N_("E1238: Blob required for argument %d"));
EXTERN అక్షరం e_invalid_విలువ_for_blob_nr[]
	INIT(= N_("E1239: Invalid value for blob: 0x%lX"));
#అయితే_ముగింపు
EXTERN అక్షరం e_resulting_పాఠ్యం_too_long[]
	INIT(= N_("E1240: Resulting text too long"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_separator_not_supported_str[]
	INIT(= N_("E1241: Separator not supported: %s"));
EXTERN అక్షరం e_no_white_space_allowed_before_separator_str[]
	INIT(= N_("E1242: No white space allowed before separator: %s"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
EXTERN అక్షరం e_ascii_సంకేతం_not_లోపల_range[]
	INIT(= N_("E1243: ASCII code not in 32-127 range"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
EXTERN అక్షరం e_bad_color_string_str[]
	INIT(= N_("E1244: Bad color string: %s"));
# endif
EXTERN అక్షరం e_cannot_expand_sfile_లోపల_vim9_function[]
	INIT(= N_("E1245: Cannot expand <sfile> in a Vim9 function"));
EXTERN అక్షరం e_cannot_కనుగొను_variable_గమ్యం_unlock_str[]
	INIT(= N_("E1246: Cannot find variable to (un)lock: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_పంక్తి_సంఖ్య_బహిర్గతం_యొక్క_range[]
	INIT(= N_("E1247: Line number out of range"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_సంవృతం_called_from_invalid_context[]
	INIT(= N_("E1248: Closure called from invalid context"));
#అయితే_ముగింపు
EXTERN అక్షరం e_highlight_సమూహం_name_too_long[]
	INIT(= N_("E1249: Highlight group name too long"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_వాదం_యొక్క_str_must_be_list_string_dictionary_or_blob[]
	INIT(= N_("E1250: Argument of %s must be a List, String, Dictionary or Blob"));
EXTERN అక్షరం e_list_tuple_dict_blob_or_string_required_for_వాదం_nr[]
	INIT(= N_("E1251: List, Tuple, Dictionary, Blob or String required for argument %d"));
EXTERN అక్షరం e_string_list_or_blob_required_for_వాదం_nr[]
	INIT(= N_("E1252: String, List or Blob required for argument %d"));
EXTERN అక్షరం e_string_list_tuple_or_blob_required_for_వాదం_nr[]
	INIT(= N_("E1253: String, List, Tuple or Blob required for argument %d"));
EXTERN అక్షరం e_cannot_use_script_variable_లోపల_for_loop[]
	INIT(= N_("E1254: Cannot use script variable in for loop"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cmd_mapping_must_చివర_with_cr[]
	INIT(= N_("E1255: <Cmd> mapping must end with <CR>"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_string_or_function_required_for_వాదం_nr[]
	INIT(= N_("E1256: String or function required for argument %d"));
EXTERN అక్షరం e_imported_script_must_use_as_or_చివర_లోపల_dot_vim_str[]
	INIT(= N_("E1257: Imported script must use \"as\" or end in .vim: %s"));
EXTERN అక్షరం e_no_dot_after_imported_name_str[]
	INIT(= N_("E1258: No '.' after imported name: %s"));
EXTERN అక్షరం e_missing_name_after_imported_name_str[]
	INIT(= N_("E1259: Missing name after imported name: %s"));
EXTERN అక్షరం e_cannot_unlet_imported_అంశం_str[]
	INIT(= N_("E1260: Cannot unlet an imported item: %s"));
EXTERN అక్షరం e_cannot_import_dot_vim_without_using_as[]
	INIT(= N_("E1261: Cannot import .vim without using \"as\""));
EXTERN అక్షరం e_cannot_import_same_script_twice_str[]
	INIT(= N_("E1262: Cannot import the same script twice: %s"));
EXTERN అక్షరం e_cannot_use_name_with_hash_లోపల_vim9_script_use_export_instead[]
	INIT(= N_("E1263: Cannot use name with # in Vim9 script, use export instead"));
EXTERN అక్షరం e_autoload_import_cannot_use_absolute_or_relative_మార్గం[]
	INIT(= N_("E1264: Autoload import cannot use absolute or relative path: %s"));
EXTERN అక్షరం e_cannot_use_partial_ఇక్కడ[]
	INIT(= N_("E1265: Cannot use a partial here"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PYTHON3) && defined(MSWIN)
EXTERN అక్షరం e_critical_error_లోపల_python3_initialization_check_your_installation[]
	INIT(= N_("E1266: Critical error in python3 initialization, check your python3 installation"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_function_name_must_ప్రారంభం_with_capital_str[]
	INIT(= N_("E1267: Function name must start with a capital: %s"));
EXTERN అక్షరం e_cannot_use_s_colon_లోపల_vim9_script_str[]
	INIT(= N_("E1268: Cannot use s: in Vim9 script: %s"));
EXTERN అక్షరం e_cannot_create_vim9_script_variable_లోపల_function_str[]
	INIT(= N_("E1269: Cannot create a Vim9 script variable in a function: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_use_s_backslash_లోపల_vim9_script[]
	INIT(= N_("E1270: Cannot use :s\\/sub/ in Vim9 script"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_compiling_సంవృతం_without_context_str[]
	INIT(= N_("E1271: Compiling closure without context: %s"));
EXTERN అక్షరం e_using_type_not_లోపల_script_context_str[]
	INIT(= N_("E1272: Using type not in a script context: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_nfa_regexp_missing_విలువ_లోపల_chr[]
	INIT(= N_("E1273: (NFA regexp) missing value in '\\%%%c'"));
EXTERN అక్షరం e_no_script_file_name_గమ్యం_substitute_for_script[]
	INIT(= N_("E1274: No script file name to substitute for \"<script>\""));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_string_or_function_required_for_arrow_parens_సమాసం[]
	INIT(= N_("E1275: String or function required for ->(expr)"));
EXTERN అక్షరం e_illegal_map_బహుళకం_string_str[]
	INIT(= N_("E1276: Illegal map mode string: '%s'"));
# ఒకవేళ !defined(FEAT_పని_CHANNEL)
EXTERN అక్షరం e_channel_పని_feature_not_అందుబాటు[]
	INIT(= N_("E1277: Channel and job feature is not available"));
# endif
EXTERN అక్షరం e_stray_closing_curly_str[]
	INIT(= N_("E1278: Stray '}' without a matching '{': %s"));
EXTERN అక్షరం e_missing_close_curly_str[]
	INIT(= N_("E1279: Missing '}': %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
EXTERN అక్షరం e_illegal_character_లోపల_మాట[]
	INIT(= N_("E1280: Illegal character in word"));
#అయితే_ముగింపు
EXTERN అక్షరం e_atom_engine_must_be_at_ప్రారంభం_యొక్క_నమూనా[]
	INIT(= N_("E1281: Atom '\\%%#=%c' must be at the start of the pattern"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_bitshift_ops_must_be_సంఖ్య[]
	INIT(= N_("E1282: Bitshift operands must be numbers"));
EXTERN అక్షరం e_bitshift_ops_must_be_ధనాత్మక[]
	INIT(= N_("E1283: Bitshift amount must be a positive number"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_PROP_POPUP)
EXTERN అక్షరం e_వాదం_1_list_అంశం_nr_dictionary_required[]
	INIT(= N_("E1284: Argument 1, list item %d: Dictionary required"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RELTIME
EXTERN అక్షరం e_could_not_clear_timeout_str[]
	INIT(= N_("E1285: Could not clear timeout: %s"));
EXTERN అక్షరం e_could_not_సమితి_timeout_str[]
	INIT(= N_("E1286: Could not set timeout: %s"));
# ifndef PROF_NSEC
EXTERN అక్షరం e_could_not_సమితి_handler_for_timeout_str[]
	INIT(= N_("E1287: Could not set handler for timeout: %s"));
EXTERN అక్షరం e_could_not_reset_handler_for_timeout_str[]
	INIT(= N_("E1288: Could not reset handler for timeout: %s"));
EXTERN అక్షరం e_could_not_check_for_pending_sigalrm_str[]
	INIT(= N_("E1289: Could not check for pending SIGALRM: %s"));
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_substitute_nesting_too_deep[]
	INIT(= N_("E1290: substitute nesting too deep"));
# ifdef MSWIN
EXTERN అక్షరం e_invalid_వాదం_nr[]
	INIT(= N_("E1291: Invalid argument: %ld"));
# endif
#అయితే_ముగింపు
EXTERN అక్షరం e_cmdline_కిటికీ_already_open[]
	INIT(= N_("E1292: Command-line window is already open"));
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_cannot_use_negative_id[]
	INIT(= N_("E1293: Cannot use a negative id for a text property"));
EXTERN అక్షరం e_can_only_use_పాఠ్యం_align_when_స్తంభం_is_zero[]
	INIT(= N_("E1294: Can only use text_align when column is zero"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_cannot_specify_both_type_and_types[]
	INIT(= N_("E1295: Cannot specify both 'type' and 'types'"));
EXTERN అక్షరం e_can_only_use_ఎడమ_padding_when_స్తంభం_is_zero[]
	INIT(= N_("E1296: Can only use left padding when column is zero"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_non_null_dict_required_for_వాదం_nr[]
	INIT(= N_("E1297: Non-NULL Dictionary required for argument %d"));
EXTERN అక్షరం e_non_null_list_required_for_వాదం_nr[]
	INIT(= N_("E1298: Non-NULL List required for argument %d"));
#అయితే_ముగింపు
EXTERN అక్షరం e_కిటికీ_unexpectedly_close_while_searching_for_ట్యాగులు[]
	INIT(= N_("E1299: Window unexpectedly closed while searching for tags"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_use_partial_with_dictionary_for_defer[]
	INIT(= N_("E1300: Cannot use a partial with dictionary for :defer"));
EXTERN అక్షరం e_repeatable_type_required_for_వాదం_nr[]
	INIT(= N_("E1301: String, Number, List, Tuple or Blob required for argument %d"));
EXTERN అక్షరం e_script_variable_was_deleted[]
	INIT(= N_("E1302: Script variable was deleted"));
EXTERN అక్షరం e_custom_list_completion_function_does_not_return_list_but_str[]
	INIT(= N_("E1303: Custom list completion function does not return a List but a %s"));
EXTERN అక్షరం e_cannot_use_type_with_this_variable_str[]
	INIT(= N_("E1304: Cannot use type with this variable: %s"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_cannot_use_నిడివి_endcol_and_endlnum_with_పాఠ్యం[]
	INIT(= N_("E1305: Cannot use \"length\", \"end_col\" and \"end_lnum\" with \"text\""));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_loop_nesting_too_deep[]
	INIT(= N_("E1306: Loop nesting too deep"));
EXTERN అక్షరం e_వాదం_nr_trying_గమ్యం_modify_const_str[]
	INIT(= N_("E1307: Argument %d: Trying to modify a const %s"));
EXTERN అక్షరం e_cannot_resize_కిటికీ_లోపల_another_tab_పుట[]
	INIT(= N_("E1308: Cannot resize a window in another tab page"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_change_mappings_while_listing[]
	INIT(= N_("E1309: Cannot change mappings while listing"));
#ఒకవేళ defined(FEAT_MENU)
EXTERN అక్షరం e_cannot_change_menus_while_listing[]
	INIT(= N_("E1310: Cannot change menus while listing"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_change_user_commands_while_listing[]
	INIT(= N_("E1311: Cannot change user commands while listing"));
EXTERN అక్షరం e_not_allowed_గమ్యం_change_కిటికీ_layout_లోపల_this_autocmd[]
	INIT(= N_("E1312: Not allowed to change the window layout in this autocmd"));
EXTERN అక్షరం e_not_allowed_గమ్యం_కూడు_or_remove_entries_str[]
	INIT(= N_("E1313: Not allowed to add or remove entries (%s)"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_class_name_must_ప్రారంభం_with_uppercase_letter_str[]
	INIT(= N_("E1314: Class name must start with an uppercase letter: %s"));
EXTERN అక్షరం e_white_space_required_after_name_str[]
	INIT(= N_("E1315: White space required after name: %s"));
EXTERN అక్షరం e_class_can_only_be_defined_లోపల_vim9_script[]
	INIT(= N_("E1316: Class can only be defined in Vim9 script"));
EXTERN అక్షరం e_invalid_object_variable_declaration_str[]
	INIT(= N_("E1317: Invalid object variable declaration: %s"));
EXTERN అక్షరం e_not_valid_command_లోపల_class_str[]
	INIT(= N_("E1318: Not a valid command in a class: %s"));
// E1319 unused
EXTERN అక్షరం e_using_object_as_సంఖ్య[]
	INIT(= N_("E1320: Using an Object as a Number"));
// E1321 unused
EXTERN అక్షరం e_using_object_as_float[]
	INIT(= N_("E1322: Using an Object as a Float"));
// E1323 unused
EXTERN అక్షరం e_using_object_as_string[]
	INIT(= N_("E1324: Using an Object as a String"));
EXTERN అక్షరం e_పద్ధతి_not_దొరికింది_on_class_str_str[]
	INIT(= N_("E1325: Method \"%s\" not found in class \"%s\""));
EXTERN అక్షరం e_variable_not_దొరికింది_on_object_str_str[]
	INIT(= N_("E1326: Variable \"%s\" not found in object \"%s\""));
EXTERN అక్షరం e_object_required_దొరికింది_str[]
	INIT(= N_("E1327: Object required, found %s"));
EXTERN అక్షరం e_constructor_default_విలువ_must_be_vnone_str[]
	INIT(= N_("E1328: Constructor default value must be v:none: %s"));
EXTERN అక్షరం e_invalid_class_variable_declaration_str[]
	INIT(= N_("E1329: Invalid class variable declaration: %s"));
EXTERN అక్షరం e_invalid_type_లోపల_variable_declaration_str[]
	INIT(= N_("E1330: Invalid type used in variable declaration: %s"));
EXTERN అక్షరం e_ప్రజా_must_be_followed_by_var_static_final_or_const[]
	INIT(= N_("E1331: public must be followed by \"var\" or \"static\" or \"final\" or \"const\""));
EXTERN అక్షరం e_ప్రజా_variable_name_cannot_ప్రారంభం_with_underscore_str[]
	INIT(= N_("E1332: public variable name cannot start with underscore: %s"));
EXTERN అక్షరం e_cannot_access_protected_variable_str[]
	INIT(= N_("E1333: Cannot access protected variable \"%s\" in class \"%s\""));
// E1334 unused
EXTERN అక్షరం e_variable_is_not_writable_str[]
	INIT(= N_("E1335: Variable \"%s\" in class \"%s\" is not writable"));
#అయితే_ముగింపు
EXTERN అక్షరం e_అంతర్గత_error_shortmess_too_long[]
	INIT(= "E1336: Internal error: shortmess too long");
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_class_variable_str_not_దొరికింది_లోపల_class_str[]
	INIT(= N_("E1337: Class variable \"%s\" not found in class \"%s\""));
#అయితే_ముగింపు
// E1338 and E1339 unused
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_వాదం_already_declared_లోపల_class_str[]
	INIT(= N_("E1340: Argument already declared in the class: %s"));
EXTERN అక్షరం e_variable_already_declared_లోపల_class_str[]
	INIT(= N_("E1341: Variable already declared in the class: %s"));
EXTERN అక్షరం e_interface_can_only_be_defined_లోపల_vim9_script[]
	INIT(= N_("E1342: Interface can only be defined in Vim9 script"));
EXTERN అక్షరం e_interface_name_must_ప్రారంభం_with_uppercase_letter_str[]
	INIT(= N_("E1343: Interface name must start with an uppercase letter: %s"));
EXTERN అక్షరం e_cannot_initialize_variable_లోపల_interface[]
	INIT(= N_("E1344: Cannot initialize a variable in an interface"));
EXTERN అక్షరం e_not_valid_command_లోపల_interface_str[]
	INIT(= N_("E1345: Not a valid command in an interface: %s"));
EXTERN అక్షరం e_interface_name_not_దొరికింది_str[]
	INIT(= N_("E1346: Interface name not found: %s"));
EXTERN అక్షరం e_not_valid_interface_str[]
	INIT(= N_("E1347: Not a valid interface: %s"));
EXTERN అక్షరం e_variable_str_యొక్క_interface_str_not_implemented[]
	INIT(= N_("E1348: Variable \"%s\" of interface \"%s\" is not implemented"));
EXTERN అక్షరం e_పద్ధతి_str_యొక్క_interface_str_not_implemented[]
	INIT(= N_("E1349: Method \"%s\" of interface \"%s\" is not implemented"));
EXTERN అక్షరం e_duplicate_implements[]
	INIT(= N_("E1350: Duplicate \"implements\""));
EXTERN అక్షరం e_duplicate_interface_after_implements_str[]
	INIT(= N_("E1351: Duplicate interface after \"implements\": %s"));
EXTERN అక్షరం e_duplicate_extends[]
	INIT(= N_("E1352: Duplicate \"extends\""));
EXTERN అక్షరం e_class_name_not_దొరికింది_str[]
	INIT(= N_("E1353: Class name not found: %s"));
EXTERN అక్షరం e_cannot_extend_str[]
	INIT(= N_("E1354: Cannot extend %s"));
EXTERN అక్షరం e_duplicate_function_str[]
	INIT(= N_("E1355: Duplicate function: %s"));
EXTERN అక్షరం e_super_must_be_followed_by_dot[]
	INIT(= N_("E1356: \"super\" must be followed by a dot"));
EXTERN అక్షరం e_using_super_not_లోపల_class_పద్ధతి[]
	INIT(= N_("E1357: Using \"super\" not in a class method"));
EXTERN అక్షరం e_using_super_not_లోపల_బిడ్డ_class[]
	INIT(= N_("E1358: Using \"super\" not in a child class"));
EXTERN అక్షరం e_cannot_define_new_పద్ధతి_లోపల_abstract_class[]
	INIT(= N_("E1359: Cannot define a \"new\" method in an abstract class"));
EXTERN అక్షరం e_using_null_object[]
	INIT(= N_("E1360: Using a null object"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_use_color_none_did_you_సగటు_none[]
	INIT(= N_("E1361: Cannot use color \"none\", did you mean \"NONE\"?"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_use_non_null_object[]
	INIT(= N_("E1362: Cannot use a non-null object"));
EXTERN అక్షరం e_incomplete_type[]
	INIT(= N_("E1363: Incomplete type"));
#అయితే_ముగింపు
EXTERN అక్షరం e_warning_సూచిక_ఖండం_corrupted[]
	INIT(= N_("E1364: Warning: Pointer block corrupted"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_use_a_return_type_with_new_పద్ధతి[]
	INIT(= N_("E1365: Cannot use a return type with the \"new\" method"));
EXTERN అక్షరం e_cannot_access_protected_పద్ధతి_str[]
	INIT(= N_("E1366: Cannot access protected method: %s"));
EXTERN అక్షరం e_variable_str_యొక్క_interface_str_has_different_access[]
	INIT(= N_("E1367: Access level of variable \"%s\" of interface \"%s\" is different"));
EXTERN అక్షరం e_static_must_be_followed_by_var_def_final_or_const[]
	INIT(= N_("E1368: Static must be followed by \"var\" or \"def\" or \"final\" or \"const\""));
EXTERN అక్షరం e_duplicate_variable_str[]
	INIT(= N_("E1369: Duplicate variable: %s"));
EXTERN అక్షరం e_cannot_define_new_పద్ధతి_as_static[]
	INIT(= N_("E1370: Cannot define a \"new\" method as static"));
EXTERN అక్షరం e_abstract_must_be_followed_by_def[]
	INIT(= N_("E1371: Abstract must be followed by \"def\""));
EXTERN అక్షరం e_abstract_పద్ధతి_లోపల_concrete_class[]
	INIT(= N_("E1372: Abstract method \"%s\" cannot be defined in a concrete class"));
EXTERN అక్షరం e_abstract_పద్ధతి_str_not_implemented[]
	INIT(= N_("E1373: Abstract method \"%s\" is not implemented"));
EXTERN అక్షరం e_class_variable_str_accessible_only_inside_class_str[]
	INIT(= N_("E1374: Class variable \"%s\" accessible only inside class \"%s\""));
EXTERN అక్షరం e_class_variable_str_accessible_only_using_class_str[]
	INIT(= N_("E1375: Class variable \"%s\" accessible only using class \"%s\""));
EXTERN అక్షరం e_object_variable_str_accessible_only_using_object_str[]
	INIT(= N_("E1376: Object variable \"%s\" accessible only using class \"%s\" object"));
EXTERN అక్షరం e_పద్ధతి_str_యొక్క_class_str_has_different_access[]
	INIT(= N_("E1377: Access level of method \"%s\" is different in class \"%s\""));
EXTERN అక్షరం e_static_member_not_supported_లోపల_interface[]
	INIT(= N_("E1378: Static member not supported in an interface"));
EXTERN అక్షరం e_protected_variable_not_supported_లోపల_interface[]
	INIT(= N_("E1379: Protected variable not supported in an interface"));
EXTERN అక్షరం e_protected_పద్ధతి_not_supported_లోపల_interface[]
	INIT(= N_("E1380: Protected method not supported in an interface"));
EXTERN అక్షరం e_interface_cannot_use_implements[]
	INIT(= N_("E1381: Interface cannot use \"implements\""));
EXTERN అక్షరం e_variable_str_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1382: Variable \"%s\": type mismatch, expected %s but got %s"));
EXTERN అక్షరం e_పద్ధతి_str_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1383: Method \"%s\": type mismatch, expected %s but got %s"));
EXTERN అక్షరం e_class_పద్ధతి_str_accessible_only_inside_class_str[]
	INIT(= N_("E1384: Class method \"%s\" accessible only inside class \"%s\""));
EXTERN అక్షరం e_class_పద్ధతి_str_accessible_only_using_class_str[]
	INIT(= N_("E1385: Class method \"%s\" accessible only using class \"%s\""));
EXTERN అక్షరం e_object_పద్ధతి_str_accessible_only_using_object_str[]
	INIT(= N_("E1386: Object method \"%s\" accessible only using class \"%s\" object"));
EXTERN అక్షరం e_ప్రజా_variable_not_supported_లోపల_interface[]
	INIT(= N_("E1387: public variable not supported in an interface"));
EXTERN అక్షరం e_ప్రజా_keyword_not_supported_for_పద్ధతి[]
	INIT(= N_("E1388: public keyword not supported for a method"));
EXTERN అక్షరం e_missing_name_after_implements[]
	INIT(= N_("E1389: Missing name after implements"));
EXTERN అక్షరం e_cannot_use_an_object_variable_except_with_the_new_పద్ధతి_str[]
	INIT(= N_("E1390: Cannot use an object variable \"this.%s\" except with the \"new\" method"));
EXTERN అక్షరం e_cannot_తాళం_object_variable_str[]
	INIT(= N_("E1391: Cannot (un)lock variable \"%s\" in class \"%s\""));
EXTERN అక్షరం e_cannot_తాళం_class_variable_str[]
	INIT(= N_("E1392: Cannot (un)lock class variable \"%s\" in class \"%s\""));
EXTERN అక్షరం e_type_can_only_be_defined_లోపల_vim9_script[]
	INIT(= N_("E1393: Type can only be defined in Vim9 script"));
EXTERN అక్షరం e_type_name_must_ప్రారంభం_with_uppercase_letter_str[]
	INIT(= N_("E1394: Type name must start with an uppercase letter: %s"));
EXTERN అక్షరం e_using_null_class[]
	INIT(= N_("E1395: Using a null class"));
EXTERN అక్షరం e_typealias_already_exists_for_str[]
	INIT(= N_("E1396: Type alias \"%s\" already exists"));
EXTERN అక్షరం e_missing_typealias_name[]
	INIT(= N_("E1397: Missing type alias name"));
EXTERN అక్షరం e_missing_typealias_type[]
	INIT(= N_("E1398: Missing type alias type"));
EXTERN అక్షరం e_type_can_only_be_వాడిన_లోపల_script[]
	INIT(= N_("E1399: Type can only be used in a script"));
// E1400 unused
// E1401 unused
// E1402 unused
EXTERN అక్షరం e_using_typealias_as_విలువ_str[]
	INIT(= N_("E1403: Type alias \"%s\" cannot be used as a value"));
EXTERN అక్షరం e_abstract_cannot_be_వాడిన_లోపల_interface[]
	INIT(= N_("E1404: Abstract cannot be used in an interface"));
EXTERN అక్షరం e_using_class_as_విలువ_str[]
	INIT(= N_("E1405: Class \"%s\" cannot be used as a value"));
EXTERN అక్షరం e_ప్రజా_and_protected_member_have_same_name_str_str[]
	INIT(= N_("E1406: Public and protected member have the same name: %s and _%s"));
EXTERN అక్షరం e_using_typealias_as_var_val[]
	INIT(= N_("E1407: Cannot use a Typealias as a variable or value"));
EXTERN అక్షరం e_final_variable_not_supported_లోపల_interface[]
	INIT(= N_("E1408: Final variable not supported in an interface"));
EXTERN అక్షరం e_cannot_change_readonly_variable_str_లోపల_class_str[]
	INIT(= N_("E1409: Cannot change read-only variable \"%s\" in class \"%s\""));
EXTERN అక్షరం e_const_variable_not_supported_లోపల_interface[]
	INIT(= N_("E1410: Const variable not supported in an interface"));
EXTERN అక్షరం e_missing_dot_after_object_str[]
	INIT(= N_("E1411: Missing dot after object \"%s\""));
EXTERN అక్షరం e_builtin_object_పద్ధతి_str_not_supported[]
	INIT(= N_("E1412: Builtin object method \"%s\" not supported"));
EXTERN అక్షరం e_builtin_class_పద్ధతి_not_supported[]
	INIT(= N_("E1413: Builtin class method not supported"));
EXTERN అక్షరం e_enum_can_only_be_defined_లోపల_vim9_script[]
       INIT(= N_("E1414: Enum can only be defined in Vim9 script"));
EXTERN అక్షరం e_enum_name_must_ప్రారంభం_with_uppercase_letter_str[]
	INIT(= N_("E1415: Enum name must start with an uppercase letter: %s"));
EXTERN అక్షరం e_enum_cannot_extend_class[]
	INIT(= N_("E1416: Enum cannot extend a class or enum"));
EXTERN అక్షరం e_abstract_cannot_be_వాడిన_లోపల_enum[]
	INIT(= N_("E1417: Abstract cannot be used in an Enum"));
EXTERN అక్షరం e_invalid_enum_విలువ_declaration_str[]
	INIT(= N_("E1418: Invalid enum value declaration: %s"));
EXTERN అక్షరం e_not_valid_command_లోపల_enum_str[]
	INIT(= N_("E1419: Not a valid command in an Enum: %s"));
EXTERN అక్షరం e_missing_endenum[]
	INIT(= N_("E1420: Missing :endenum"));
EXTERN అక్షరం e_using_enum_as_విలువ_str[]
	INIT(= N_("E1421: Enum \"%s\" cannot be used as a value"));
EXTERN అక్షరం e_enum_విలువ_str_not_దొరికింది_లోపల_enum_str[]
	INIT(= N_("E1422: Enum value \"%s\" not found in enum \"%s\""));
EXTERN అక్షరం e_enumvalue_str_cannot_be_modified[]
	INIT(= N_("E1423: Enum value \"%s.%s\" cannot be modified"));
EXTERN అక్షరం e_using_enum_str_as_సంఖ్య[]
	INIT(= N_("E1424: Using an Enum \"%s\" as a Number"));
EXTERN అక్షరం e_using_enum_str_as_string[]
	INIT(= N_("E1425: Using an Enum \"%s\" as a String"));
EXTERN అక్షరం e_enum_str_ordinal_cannot_be_modified[]
	INIT(= N_("E1426: Enum \"%s\" ordinal value cannot be modified"));
EXTERN అక్షరం e_enum_str_name_cannot_be_modified[]
	INIT(= N_("E1427: Enum \"%s\" name cannot be modified"));
EXTERN అక్షరం e_duplicate_enum_str[]
	INIT(= N_("E1428: Duplicate enum value: %s"));
EXTERN అక్షరం e_class_can_only_be_వాడిన_లోపల_script[]
	INIT(= N_("E1429: Class can only be used in a script"));
EXTERN అక్షరం e_uninitialized_object_var_reference[]
	INIT(= N_("E1430: Uninitialized object variable '%s' referenced"));
EXTERN అక్షరం e_abstract_పద్ధతి_str_ప్రత్యక్ష[]
	INIT(= N_("E1431: Abstract method \"%s\" in class \"%s\" cannot be accessed directly"));
EXTERN అక్షరం e_generic_పద్ధతి_str_override_with_concrete_పద్ధతి_లోపల_class_str[]
	INIT(= N_("E1432: Overriding generic method \"%s\" in class \"%s\" with a concrete method"));
EXTERN అక్షరం e_concrete_పద్ధతి_str_override_with_generic_పద్ధతి_లోపల_class_str[]
	INIT(= N_("E1433: Overriding concrete method \"%s\" in class \"%s\" with a generic method"));
EXTERN అక్షరం e_generic_పద్ధతి_str_type_arguments_mismatch_లోపల_class_str[]
	INIT(= N_("E1434: Mismatched number of type variables for generic method  \"%s\" in class \"%s\""));
EXTERN అక్షరం e_enum_can_only_be_వాడిన_లోపల_script[]
	INIT(= N_("E1435: Enum can only be used in a script"));
EXTERN అక్షరం e_interface_can_only_be_వాడిన_లోపల_script[]
	INIT(= N_("E1436: Interface can only be used in a script"));
EXTERN అక్షరం e_can_only_compare_object_with_object[]
	INIT(= N_("E1437: Can only compare Object with Object"));
EXTERN అక్షరం e_type_var_లెక్క_యొక్క_పద్ధతి_str_differs_from_interface_str[]
	INIT(= N_("E1438: Number of type variables of method \"%s\" differs from interface \"%s\""));
EXTERN అక్షరం e_type_nested_too_deep[]
	INIT(= N_("E1439: Type nested too deep"));
#అయితే_ముగింపు
// E1440 - E1499 unused (reserved for Vim9 class support)
EXTERN అక్షరం e_cannot_mix_positional_and_non_positional_str[]
	INIT(= N_("E1500: Cannot mix positional and non-positional arguments: %s"));
EXTERN అక్షరం e_fmt_arg_nr_వాడని_str[]
	INIT(= N_("E1501: format argument %d unused in $-style format: %s"));
EXTERN అక్షరం e_positional_అంకె_క్షేత్రం_spec_reused_str_str[]
	INIT(= N_("E1502: Positional argument %d used as field width reused as different type: %s/%s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_positional_nr_బహిర్గతం_యొక్క_bounds_str[]
	INIT(= N_("E1503: Positional argument %d out of bounds: %s"));
#అయితే_ముగింపు
EXTERN అక్షరం e_positional_arg_అంకె_type_inconsistent_str_str[]
	INIT(= N_("E1504: Positional argument %d type used inconsistently: %s/%s"));
EXTERN అక్షరం e_invalid_format_specifier_str[]
	INIT(= N_("E1505: Invalid format specifier: %s"));
#నిర్వచితమైతే FEAT_XATTR
EXTERN అక్షరం e_xattr_erange[]
	INIT(= N_("E1506: Buffer too small to copy xattr value or key"));
#అయితే_ముగింపు
EXTERN అక్షరం e_aptypes_is_null_nr_str[]
	INIT(= "E1507: Internal error: ap_types or ap_types[idx] is NULL: %d: %s");
#నిర్వచితమైతే FEAT_XATTR
EXTERN అక్షరం e_xattr_e2big[]
	INIT(= N_("E1508: Size of the extended attribute value is larger than the maximum size allowed"));
EXTERN అక్షరం e_xattr_other[]
	INIT(= N_("E1509: Error occurred when reading or writing extended attribute"));
#అయితే_ముగింపు
EXTERN అక్షరం e_val_too_large[]
	INIT(= N_("E1510: Value too large: %s"));
EXTERN అక్షరం e_wrong_సంఖ్య_యొక్క_characters_for_క్షేత్రం_str[]
	INIT(= N_("E1511: Wrong number of characters for field \"%s\""));
EXTERN అక్షరం e_wrong_character_వెడల్పు_for_క్షేత్రం_str[]
	INIT(= N_("E1512: Wrong character width for field \"%s\""));
EXTERN అక్షరం e_winfixbuf_cannot_go_గమ్యం_బఫరు[]
	INIT(= N_("E1513: Cannot switch buffer. 'winfixbuf' is enabled"));
EXTERN అక్షరం e_invalid_return_type_from_findfunc[]
	INIT(= N_("E1514: 'findfunc' did not return a List type"));
EXTERN అక్షరం e_str_encoding_from_failed[]
	INIT(= N_("E1515: Unable to convert from '%s' encoding"));
EXTERN అక్షరం e_str_encoding_గమ్యం_failed[]
	INIT(= N_("E1516: Unable to convert to '%s' encoding"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_can_only_compare_tuple_with_tuple[]
	INIT(= N_("E1517: Can only compare Tuple with Tuple"));
EXTERN అక్షరం e_invalid_operation_for_tuple[]
	INIT(= N_("E1518: Invalid operation for Tuple"));
EXTERN అక్షరం e_tuple_సూచీ_బహిర్గతం_యొక్క_range_nr[]
	INIT(= N_("E1519: Tuple index out of range: %ld"));
EXTERN అక్షరం e_using_tuple_as_సంఖ్య[]
	INIT(= N_("E1520: Using a Tuple as a Number"));
EXTERN అక్షరం e_using_tuple_as_float[]
	INIT(= N_("E1521: Using a Tuple as a Float"));
EXTERN అక్షరం e_using_tuple_as_string[]
	INIT(= N_("E1522: Using a Tuple as a String"));
EXTERN అక్షరం e_string_list_tuple_or_blob_required[]
	INIT(= N_("E1523: String, List, Tuple or Blob required"));
EXTERN అక్షరం e_cannot_use_tuple_with_function_str[]
	INIT(= N_("E1524: Cannot use a tuple with function %s"));
EXTERN అక్షరం e_వాదం_యొక్క_str_must_be_list_tuple_string_dictionary_or_blob[]
	INIT(= N_("E1525: Argument of %s must be a List, Tuple, String, Dictionary or Blob"));
EXTERN అక్షరం e_missing_చివర_యొక్క_tuple_rsp_str[]
	INIT(= N_("E1526: Missing end of Tuple ')': %s"));
EXTERN అక్షరం e_missing_comma_లోపల_tuple_str[]
	INIT(= N_("E1527: Missing comma in Tuple: %s"));
EXTERN అక్షరం e_list_or_tuple_or_blob_required_for_వాదం_nr[]
	INIT(= N_("E1528: List or Tuple or Blob required for argument %d"));
EXTERN అక్షరం e_list_or_tuple_required_for_వాదం_nr[]
	INIT(= N_("E1529: List or Tuple required for argument %d"));
EXTERN అక్షరం e_list_or_tuple_or_dict_required_for_వాదం_nr[]
	INIT(= N_("E1530: List or Tuple or Dictionary required for argument %d"));
EXTERN అక్షరం e_వాదం_యొక్క_str_must_be_list_tuple_dictionary_or_blob[]
	INIT(= N_("E1531: Argument of %s must be a List, Tuple, Dictionary or Blob"));
EXTERN అక్షరం e_tuple_is_immutable[]
	INIT(= N_("E1532: Cannot modify a tuple"));
EXTERN అక్షరం e_cannot_ముక్క_tuple[]
	INIT(= N_("E1533: Cannot slice a tuple"));
EXTERN అక్షరం e_tuple_required_for_వాదం_nr[]
	INIT(= N_("E1534: Tuple required for argument %d"));
EXTERN అక్షరం e_list_or_tuple_required[]
	INIT(= N_("E1535: List or Tuple required"));
EXTERN అక్షరం e_tuple_required[]
	INIT(= N_("E1536: Tuple required"));
EXTERN అక్షరం e_less_targets_than_tuple_వస్తువులు[]
	INIT(= N_("E1537: Less targets than Tuple items"));
EXTERN అక్షరం e_మరిన్ని_targets_than_tuple_వస్తువులు[]
	INIT(= N_("E1538: More targets than Tuple items"));
EXTERN అక్షరం e_variadic_tuple_must_చివర_with_list_type_str[]
	INIT(= N_("E1539: Variadic tuple must end with a list type: %s"));
EXTERN అక్షరం e_cannot_use_variadic_tuple_లోపల_concatenation[]
	INIT(= N_("E1540: Cannot use a variadic tuple in concatenation"));
#అయితే_ముగింపు
EXTERN అక్షరం e_unicode_val_too_large[]
	INIT(= N_("E1541: Value too large, max Unicode codepoint is U+10FFFF"));
#నిర్వచితమైతే FEAT_QUICKFIX
EXTERN అక్షరం e_cannot_have_negative_or_zero_సంఖ్య_యొక్క_quickfix[]
	INIT(= N_("E1542: Cannot have a negative or zero number of quickfix/location lists"));
EXTERN అక్షరం e_cannot_have_మరిన్ని_than_hundred_quickfix[]
	INIT(= N_("E1543: Cannot have more than a hundred quickfix/location lists"));
EXTERN అక్షరం e_failed_resizing_quickfix_దొంతర[]
	INIT(= N_("E1544: Failed resizing the quickfix/location list stack"));
EXTERN అక్షరం e_no_quickfix_దొంతర[]
	INIT(= N_("E1545: Quickfix list stack unavailable"));
#అయితే_ముగింపు
EXTERN అక్షరం e_cannot_switch_గమ్యం_a_closing_బఫరు[]
	INIT(= N_("E1546: Cannot switch to a closing buffer"));
EXTERN అక్షరం e_cannot_not_support_redrawtabpanel[]
	INIT(= N_("E1547: This version of Vim does not support :redrawtabpanel"));
#నిర్వచితమైతే FEAT_WAYLAND
EXTERN అక్షరం e_wayland_connection_unavailable[]
	INIT(= N_("E1548: Wayland connection is unavailable"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_DIFF)
EXTERN అక్షరం e_cannot_have_మరిన్ని_than_nr_diff_anchors[]
	INIT(= N_("E1549: Cannot have more than %d diff anchors"));
EXTERN అక్షరం e_failed_గమ్యం_కనుగొను_all_diff_anchors[]
	INIT(= N_("E1550: Failed to find all diff anchors"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_cannot_open_a_popup_కిటికీ_గమ్యం_a_closing_బఫరు[]
	INIT(= N_("E1551: Cannot open a popup window to a closing buffer"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_type_var_name_must_ప్రారంభం_with_uppercase_letter_str[]
	INIT(= N_("E1552: Type variable name must start with an uppercase letter: %s"));
EXTERN అక్షరం e_missing_comma_లోపల_generic_function_str[]
	INIT(= N_("E1553: Missing comma after type in generic function: %s"));
EXTERN అక్షరం e_missing_closing_angle_bracket_లోపల_generic_function_str[]
	INIT(= N_("E1554: Missing '>' in generic function: %s"));
EXTERN అక్షరం e_ఖాళీలు_type_list_for_generic_function_str[]
	INIT(= N_("E1555: Empty type list specified for generic function '%s'"));
EXTERN అక్షరం e_too_many_types_for_generic_function_str[]
	INIT(= N_("E1556: Too many types specified for generic function '%s'"));
EXTERN అక్షరం e_not_enough_types_for_generic_function_str[]
	INIT(= N_("E1557: Not enough types specified for generic function '%s'"));
EXTERN అక్షరం e_unknown_generic_function_str[]
	INIT(= N_("E1558: Unknown generic function: %s"));
EXTERN అక్షరం e_generic_func_missing_type_వాదనలు_str[]
	INIT(= N_("E1559: Type arguments missing for generic function '%s'"));
EXTERN అక్షరం e_not_a_generic_function_str[]
	INIT(= N_("E1560: Not a generic function: %s"));
EXTERN అక్షరం e_duplicate_type_var_name_str[]
	INIT(= N_("E1561: Duplicate type variable name: %s"));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_DIFF)
EXTERN అక్షరం e_diff_anchors_with_hidden_windows[]
	INIT(= N_("E1562: Diff anchors cannot be used with hidden diff windows"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SOCKETSERVER
EXTERN అక్షరం e_socket_name_no_slashes[]
	INIT(= N_("E1564: Socket name '%s' cannot have slashes in it without being a path"));
EXTERN అక్షరం e_socket_server_not_online[]
	INIT(= N_("E1565: Socket server is not online, call remote_startserver() first"));
EXTERN అక్షరం e_socket_server_failed_connecting[]
	INIT(= N_("E1566: Failed connecting to socket '%s'"));
EXTERN అక్షరం e_socket_server_సంస్కరణ_mismatch[]
	INIT(= N_("E1567: Socket server protocol version mismatch, check what Vim version you are using"));
#అయితే_ముగింపు
EXTERN అక్షరం e_osc_response_timed_బహిర్గతం[]
	INIT(= N_("E1568: OSC command response timed out: %.*s"));
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_కూడు_listener_లోపల_listener_callback[]
	INIT(= N_("E1569: Cannot use listener_add in a listener callback"));
EXTERN అక్షరం e_cannot_కూడు_redraw_listener_లోపల_listener_callback[]
	INIT(= N_("E1570: Cannot use redraw_listener_add in a redraw listener callback"));
EXTERN అక్షరం e_no_redraw_listener_callbacks_defined[]
	INIT(= N_("E1571: Must specify at least one callback for redraw_listener_add"));
#అయితే_ముగింపు
EXTERN అక్షరం e_leadtab_requires_tab[]
	INIT(= N_("E1572: 'listchars' field \"leadtab\" requires \"tab\" to be specified"));
#నిర్వచితమైతే FEAT_పని_CHANNEL
EXTERN అక్షరం e_cannot_listen_on_పోర్టు[]
	INIT(= N_("E1573: Cannot listen on port"));
EXTERN అక్షరం e_gethostbyname_లోపల_channel_listen[]
	INIT(= N_("E1574: gethostbyname(): cannot resolve hostname in channel_listen()"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_create_pipes[]
	INIT(= N_("E1575: Cannot create pipes"));
#అయితే_ముగింపు
EXTERN అక్షరం e_ట్యాగ్_file_నమోదు_must_not_be_url[]
	INIT(= N_("E1576: Tag file entry must not be a URL"));
EXTERN అక్షరం e_invalid_format_string_single_percent_s[]
	INIT(= N_("E1577: Invalid format string, only one \"%s\" is allowed"));
#నిర్వచితమైతే FEAT_SPELL
EXTERN అక్షరం e_too_many_postponed_prefixes_spell[]
	INIT(= N_("E1578: Too many postponed prefixes and/or compound flags"));
#అయితే_ముగింపు
EXTERN అక్షరం e_completeopt_escape_cannot_be_వాడిన_with_nargs_underscore[]
	INIT(= N_("E1579: -completeopt=escape cannot be used with -nargs=_"));
#నిర్వచితమైతే FEAT_PROP_POPUP
EXTERN అక్షరం e_too_many_పాఠ్యం_properties_on_a_single_పంక్తి[]
	INIT(= N_("E1580: Too many text properties on a single line"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_EVAL
EXTERN అక్షరం e_cannot_extend_null_blob[]
	INIT(= N_("E1581: Cannot extend a null blob"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
EXTERN అక్షరం e_not_started_with_stdio_channel[]
	INIT(= N_("E1582: Not started with --stdio-channel"));
EXTERN అక్షరం e_cannot_open_stdio_channel[]
	INIT(= N_("E1583: Cannot open stdio channel"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_IMAGE
EXTERN అక్షరం e_invalid_image_dimensions[]
	INIT(= N_("E1584: Image dimensions are invalid"));
EXTERN అక్షరం e_no_image_backend_అందుబాటు[]
	INIT(= N_("E1585: No image backend available"));
EXTERN అక్షరం e_changing_image_backend_failed[]
	INIT(= N_("E1586: Failed changing image backends"));
EXTERN అక్షరం e_image_id_nr_does_not_exist[]
	INIT(= N_("E1587: Image ID %d does not exist"));
#అయితే_ముగింపు
