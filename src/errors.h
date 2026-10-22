/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

// Use PLURAL_MSG() for messages that are passed to ngettext(), so that the
// second one uses msgid_plural.
#اگر_تعریف_شده DO_INIT
# define PLURAL_پیام(var1, msg1, var2, msg2) \
	حرف var1[] = msg1; \
	حرف var2[] = msg2;
#وإلا
# define PLURAL_پیام(var1, msg1, var2, msg2) \
	خارجي حرف var1[]; \
	خارجي حرف var2[];
#پایان_اگر

/*
 * Definition of error messages, sorted on error number.
 */

EXTERN حرف e_interrupted[]
	INIT(= N_("Interrupted"));

EXTERN حرف e_backslash_should_be_followed_by[]
	INIT(= N_("E10: \\ should be followed by /, ? or &"));
EXTERN حرف e_invalid_درون_cmdline_پنجره[]
	INIT(= N_("E11: Invalid in command-line window; :q<CR> closes the window"));
EXTERN حرف e_command_not_allowed_from_vimrc_درون_current_dir_or_برچسب_جستجو[]
	INIT(= N_("E12: Command not allowed from exrc/vimrc in current dir or tag search"));
EXTERN حرف e_file_exists[]
	INIT(= N_("E13: File exists (add ! to override)"));
// E14 unused
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_invalid_عبارت_str[]
	INIT(= N_("E15: Invalid expression: \"%s\""));
#پایان_اگر
EXTERN حرف e_invalid_range[]
	INIT(= N_("E16: Invalid range"));
#إذا defined(UNIX) || defined(FEAT_SYN_HL) \
	    || defined(FEAT_SPELL) || defined(FEAT_EVAL)
EXTERN حرف e_str_is_directory[]
	INIT(= N_("E17: \"%s\" is a directory"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_unexpected_characters_درون_let[]
	INIT(= N_("E18: Unexpected characters in :let"));
EXTERN حرف e_unexpected_characters_درون_assignment[]
	INIT(= N_("E18: Unexpected characters in assignment"));
#پایان_اگر
EXTERN حرف e_mark_has_invalid_سطر_عدد[]
	INIT(= N_("E19: Mark has invalid line number"));
EXTERN حرف e_mark_not_مجموعه[]
	INIT(= N_("E20: Mark not set"));
EXTERN حرف e_cannot_make_changes_modifiable_is_off[]
	INIT(= N_("E21: Cannot make changes, 'modifiable' is off"));
EXTERN حرف e_scripts_nested_too_deep[]
	INIT(= N_("E22: Scripts nested too deep"));
EXTERN حرف e_no_alternate_file[]
	INIT(= N_("E23: No alternate file"));
EXTERN حرف e_no_such_abbreviation[]
	INIT(= N_("E24: No such abbreviation"));
#إذا !defined(FEAT_GUI) || defined(VIMDLL)
EXTERN حرف e_gui_cannot_be_استفاده_شده_not_enabled_at_compile_زمان[]
	INIT(= N_("E25: GUI cannot be used: Not enabled at compile time"));
#پایان_اگر
#اگر_تعریف_نشده FEAT_RIGHTLEFT
EXTERN حرف e_hebrew_cannot_be_استفاده_شده_not_enabled_at_compile_زمان[]
	INIT(= N_("E26: Hebrew cannot be used: Not enabled at compile time\n"));
#پایان_اگر
EXTERN حرف e_farsi_support_has_been_removed[]
	INIT(= N_("E27: Farsi support has been removed\n"));
#إذا defined(FEAT_جستجو_EXTRA) || defined(FEAT_SYN_HL)
EXTERN حرف e_no_such_highlight_گروه_name_str[]
	INIT(= N_("E28: No such highlight group name: %s"));
#پایان_اگر
EXTERN حرف e_no_inserted_متن_yet[]
	INIT(= N_("E29: No inserted text yet"));
EXTERN حرف e_no_قبلی_command_سطر[]
	INIT(= N_("E30: No previous command line"));
EXTERN حرف e_no_such_mapping[]
	INIT(= N_("E31: No such mapping"));
EXTERN حرف e_no_file_name[]
	INIT(= N_("E32: No file name"));
EXTERN حرف e_no_قبلی_substitute_regular_عبارت[]
	INIT(= N_("E33: No previous substitute regular expression"));
EXTERN حرف e_no_قبلی_command[]
	INIT(= N_("E34: No previous command"));
EXTERN حرف e_no_قبلی_regular_عبارت[]
	INIT(= N_("E35: No previous regular expression"));
EXTERN حرف e_not_enough_جا[]
	INIT(= N_("E36: Not enough room"));
EXTERN حرف e_no_write_since_آخرین_change[]
	INIT(= N_("E37: No write since last change"));
EXTERN حرف e_no_write_since_آخرین_change_جمع_کن_bang_مقصد_override[]
	INIT(= N_("E37: No write since last change (add ! to override)"));
EXTERN حرف e_null_آرگومان[]
	INIT(= "E38: Null argument");
#إذا defined(FEAT_DIGRAPHS) || defined(FEAT_TIMERS) || defined(FEAT_EVAL)
EXTERN حرف e_عدد_expected[]
	INIT(= N_("E39: Number expected"));
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_cant_open_errorfile_str[]
	INIT(= N_("E40: Can't open errorfile %s"));
#پایان_اگر
EXTERN حرف e_بیرون_از_حافظه[]
	INIT(= N_("E41: Out of memory!"));
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_no_errors[]
	INIT(= N_("E42: No Errors"));
#پایان_اگر
EXTERN حرف e_damaged_تطابق_string[]
	INIT(= "E43: Damaged match string");
EXTERN حرف e_corrupted_regexp_برنامه[]
	INIT(= "E44: Corrupted regexp program");
EXTERN حرف e_readonly_option_is_مجموعه_جمع_کن_bang_مقصد_override[]
	INIT(= N_("E45: 'readonly' option is set (add ! to override)"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_change_readonly_variable[]
	INIT(= N_("E46: Cannot change read-only variable"));
EXTERN حرف e_cannot_change_readonly_variable_str[]
	INIT(= N_("E46: Cannot change read-only variable \"%s\""));
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_error_while_reading_errorfile[]
	INIT(= N_("E47: Error while reading errorfile"));
#پایان_اگر
#اگر_تعریف_شده HAVE_SANDBOX
EXTERN حرف e_not_allowed_درون_sandbox[]
	INIT(= N_("E48: Not allowed in sandbox"));
#پایان_اگر
EXTERN حرف e_invalid_scroll_اندازه[]
	INIT(= N_("E49: Invalid scroll size"));
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN حرف e_too_many_z[]
	INIT(= N_("E50: Too many \\z("));
#پایان_اگر
EXTERN حرف e_too_many_str_open[]
	INIT(= N_("E51: Too many %s("));
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN حرف e_unmatched_z[]
	INIT(= N_("E52: Unmatched \\z("));
#پایان_اگر
EXTERN حرف e_unmatched_str_percent_open[]
	INIT(= N_("E53: Unmatched %s%%("));
EXTERN حرف e_unmatched_str_open[]
	INIT(= N_("E54: Unmatched %s("));
EXTERN حرف e_unmatched_str_close[]
	INIT(= N_("E55: Unmatched %s)"));
// E56 unused
// E57 unused
// E58 unused
EXTERN حرف e_invalid_character_after_str_at[]
	INIT(= N_("E59: Invalid character after %s@"));
EXTERN حرف e_too_many_complex_str_curly[]
	INIT(= N_("E60: Too many complex %s{...}s"));
EXTERN حرف e_nested_str[]
	INIT(= N_("E61: Nested %s*"));
EXTERN حرف e_nested_str_chr[]
	INIT(= N_("E62: Nested %s%c"));
EXTERN حرف e_invalid_use_از_underscore[]
	INIT(= N_("E63: Invalid use of \\_"));
EXTERN حرف e_str_chr_follows_nothing[]
	INIT(= N_("E64: %s%c follows nothing"));
EXTERN حرف e_illegal_پشت_reference[]
	INIT(= N_("E65: Illegal back reference"));
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN حرف e_z_not_allowed_اینجا[]
	INIT(= N_("E66: \\z( not allowed here"));
EXTERN حرف e_z1_z9_not_allowed_اینجا[]
	INIT(= N_("E67: \\z1 - \\z9 not allowed here"));
#پایان_اگر
EXTERN حرف e_invalid_character_after_bsl_z[]
	INIT(= N_("E68: Invalid character after \\z"));
EXTERN حرف e_missing_sb_after_str[]
	INIT(= N_("E69: Missing ] after %s%%["));
EXTERN حرف e_خالی_str_brackets[]
	INIT(= N_("E70: Empty %s%%[]"));
EXTERN حرف e_invalid_character_after_str[]
	INIT(= N_("E71: Invalid character after %s%%"));
EXTERN حرف e_close_error_on_جابجا_file[]
	INIT(= N_("E72: Close error on swap file"));
EXTERN حرف e_برچسب_پشته_خالی[]
	INIT(= N_("E73: Tag stack empty"));
EXTERN حرف e_command_too_complex[]
	INIT(= N_("E74: Command too complex"));
EXTERN حرف e_name_too_long[]
	INIT(= N_("E75: Name too long"));
EXTERN حرف e_too_many_brackets[]
	INIT(= N_("E76: Too many ["));
EXTERN حرف e_too_many_file_نامها[]
	INIT(= N_("E77: Too many file names"));
EXTERN حرف e_unknown_mark[]
	INIT(= N_("E78: Unknown mark"));
EXTERN حرف e_cannot_expand_wildcards[]
	INIT(= N_("E79: Cannot expand wildcards"));
EXTERN حرف e_error_while_writing[]
	INIT(= N_("E80: Error while writing"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_using_sid_not_درون_script_context[]
	INIT(= N_("E81: Using <SID> not in a script context"));
#پایان_اگر
EXTERN حرف e_cannot_allocate_any_میانگیر_exiting[]
	INIT(= N_("E82: Cannot allocate any buffer, exiting..."));
EXTERN حرف e_cannot_allocate_میانگیر_using_other_one[]
	INIT(= N_("E83: Cannot allocate buffer, using other one..."));
EXTERN حرف e_no_modified_میانگیر_یافت[]
	INIT(= N_("E84: No modified buffer found"));
EXTERN حرف e_there_is_no_listed_میانگیر[]
	INIT(= N_("E85: There is no listed buffer"));
EXTERN حرف e_میانگیر_nr_does_not_exist[]
	INIT(= N_("E86: Buffer %ld does not exist"));
EXTERN حرف e_cannot_go_beyond_آخرین_میانگیر[]
	INIT(= N_("E87: Cannot go beyond last buffer"));
EXTERN حرف e_cannot_go_before_اولی_میانگیر[]
	INIT(= N_("E88: Cannot go before first buffer"));
EXTERN حرف e_no_write_since_آخرین_change_for_میانگیر_nr_جمع_کن_bang_مقصد_override[]
	INIT(= N_("E89: No write since last change for buffer %d (add ! to override)"));
EXTERN حرف e_cannot_unload_آخرین_میانگیر[]
	INIT(= N_("E90: Cannot unload last buffer"));
EXTERN حرف e_shell_option_is_خالی[]
	INIT(= N_("E91: 'shell' option is empty"));
EXTERN حرف e_میانگیر_nr_not_یافت[]
	INIT(= N_("E92: Buffer %d not found"));
EXTERN حرف e_بیشتر_than_one_تطابق_for_str[]
	INIT(= N_("E93: More than one match for %s"));
EXTERN حرف e_no_matching_میانگیر_for_str[]
	INIT(= N_("E94: No matching buffer for %s"));
EXTERN حرف e_میانگیر_with_this_name_already_exists[]
	INIT(= N_("E95: Buffer with this name already exists"));
#إذا defined(FEAT_DIFF)
EXTERN حرف e_cannot_diff_بیشتر_than_nr_buffers[]
	INIT(= N_("E96: Cannot diff more than %d buffers"));
EXTERN حرف e_cannot_create_diffs[]
	INIT(= N_("E97: Cannot create diffs"));
EXTERN حرف e_cannot_read_diff_output[]
	INIT(= N_("E98: Cannot read diff output"));
EXTERN حرف e_current_میانگیر_is_not_درون_diff_نما[]
	INIT(= N_("E99: Current buffer is not in diff mode"));
EXTERN حرف e_no_other_میانگیر_درون_diff_نما[]
	INIT(= N_("E100: No other buffer in diff mode"));
EXTERN حرف e_بیشتر_than_two_buffers_درون_diff_نما_dont_know_which_one_مقصد_use[]
	INIT(= N_("E101: More than two buffers in diff mode, don't know which one to use"));
EXTERN حرف e_cant_پیدا_کن_میانگیر_str[]
	INIT(= N_("E102: Can't find buffer \"%s\""));
EXTERN حرف e_میانگیر_str_is_not_درون_diff_نما[]
	INIT(= N_("E103: Buffer \"%s\" is not in diff mode"));
#پایان_اگر
#اگر_تعریف_شده FEAT_DIGRAPHS
EXTERN حرف e_escape_not_allowed_درون_digraph[]
	INIT(= N_("E104: Escape not allowed in digraph"));
#پایان_اگر
#اگر_تعریف_شده FEAT_KEYMAP
EXTERN حرف e_using_loadkeymap_not_درون_sourced_file[]
	INIT(= N_("E105: Using :loadkeymap not in a sourced file"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_unsupported_diff_output_format_str[]
	INIT(= N_("E106: Unsupported diff output format: %s"));
EXTERN حرف e_missing_parenthesis_str[]
	INIT(= N_("E107: Missing parentheses: %s"));
EXTERN حرف e_no_such_variable_str[]
	INIT(= N_("E108: No such variable: \"%s\""));
EXTERN حرف e_missing_colon_after_questionmark[]
	INIT(= N_("E109: Missing ':' after '?'"));
EXTERN حرف e_missing_closing_paren[]
	INIT(= N_("E110: Missing ')'"));
EXTERN حرف e_missing_closing_مربع_brace[]
	INIT(= N_("E111: Missing ']'"));
EXTERN حرف e_option_name_missing_str[]
	INIT(= N_("E112: Option name missing: %s"));
EXTERN حرف e_unknown_option_str[]
	INIT(= N_("E113: Unknown option: %s"));
EXTERN حرف e_missing_double_quote_str[]
	INIT(= N_("E114: Missing double quote: %s"));
EXTERN حرف e_missing_single_quote_str[]
	INIT(= N_("E115: Missing single quote: %s"));
EXTERN حرف e_invalid_arguments_for_function_str[]
	INIT(= N_("E116: Invalid arguments for function %s"));
EXTERN حرف e_unknown_function_str[]
	INIT(= N_("E117: Unknown function: %s"));
EXTERN حرف e_too_many_arguments_for_function_str[]
	INIT(= N_("E118: Too many arguments for function: %s"));
EXTERN حرف e_not_enough_arguments_for_function_str[]
	INIT(= N_("E119: Not enough arguments for function: %s"));
EXTERN حرف e_using_sid_not_درون_script_context_str[]
	INIT(= N_("E120: Using <SID> not in a script context: %s"));
EXTERN حرف e_undefined_variable_str[]
	INIT(= N_("E121: Undefined variable: %s"));
EXTERN حرف e_undefined_variable_char_str[]
	INIT(= N_("E121: Undefined variable: %c:%s"));
EXTERN حرف e_function_str_already_exists_جمع_کن_bang_مقصد_replace[]
	INIT(= N_("E122: Function %s already exists, add ! to replace it"));
EXTERN حرف e_undefined_function_str[]
	INIT(= N_("E123: Undefined function: %s"));
EXTERN حرف e_missing_paren_str[]
	INIT(= N_("E124: Missing '(': %s"));
EXTERN حرف e_illegal_آرگومان_str[]
	INIT(= N_("E125: Illegal argument: %s"));
EXTERN حرف e_missing_endfunction[]
	INIT(= N_("E126: Missing :endfunction"));
EXTERN حرف e_cannot_redefine_function_str_it_is_درون_use[]
	INIT(= N_("E127: Cannot redefine function %s: It is in use"));
EXTERN حرف e_function_name_must_شروع_with_capital_or_s_str[]
	INIT(= N_("E128: Function name must start with a capital or \"s:\": %s"));
EXTERN حرف e_function_name_required[]
	INIT(= N_("E129: Function name required"));
// E130 unused
EXTERN حرف e_cannot_delete_function_str_it_is_درون_use[]
	INIT(= N_("E131: Cannot delete function %s: It is in use"));
EXTERN حرف e_function_call_عمق_is_higher_than_maxfuncdepth[]
	INIT(= N_("E132: Function call depth is higher than 'maxfuncdepth'"));
EXTERN حرف e_return_not_inside_function[]
	INIT(= N_("E133: :return not inside a function"));
#پایان_اگر
EXTERN حرف e_cannot_move_range_از_سطرها_into_itself[]
	INIT(= N_("E134: Cannot move a range of lines into itself"));
EXTERN حرف e_filter_autocommands_must_not_change_current_میانگیر[]
	INIT(= N_("E135: *Filter* Autocommands must not change current buffer"));
#إذا defined(FEAT_VIMINFO)
EXTERN حرف e_viminfo_too_many_errors_skipping_rest_از_file[]
	INIT(= N_("E136: viminfo: Too many errors, skipping rest of file"));
EXTERN حرف e_viminfo_file_is_not_writable_str[]
	INIT(= N_("E137: Viminfo file is not writable: %s"));
EXTERN حرف e_cant_write_viminfo_file_str[]
	INIT(= N_("E138: Can't write viminfo file %s!"));
#پایان_اگر
EXTERN حرف e_file_is_loaded_درون_another_میانگیر[]
	INIT(= N_("E139: File is loaded in another buffer"));
EXTERN حرف e_use_bang_مقصد_write_partial_میانگیر[]
	INIT(= N_("E140: Use ! to write partial buffer"));
EXTERN حرف e_no_file_name_for_میانگیر_nr[]
	INIT(= N_("E141: No file name for buffer %ld"));
EXTERN حرف e_file_not_written_writing_is_disabled_by_write_option[]
	INIT(= N_("E142: File not written: Writing is disabled by 'write' option"));
EXTERN حرف e_autocommands_unexpectedly_deleted_new_میانگیر_str[]
	INIT(= N_("E143: Autocommands unexpectedly deleted new buffer %s"));
EXTERN حرف e_non_numeric_آرگومان_مقصد_z[]
	INIT(= N_("E144: Non-numeric argument to :z"));
EXTERN حرف e_shell_commands_and_some_functionality_not_allowed_درون_rvim[]
	INIT(= N_("E145: Shell commands and some functionality not allowed in rvim"));
EXTERN حرف e_regular_expressions_cant_be_delimited_by_letters[]
	INIT(= N_("E146: Regular expressions can't be delimited by letters"));
EXTERN حرف e_cannot_do_global_recursive_with_range[]
	INIT(= N_("E147: Cannot do :global recursive with a range"));
EXTERN حرف e_regular_عبارت_missing_from_global[]
	INIT(= N_("E148: Regular expression missing from :global"));
EXTERN حرف e_sorry_no_help_for_str[]
	INIT(= N_("E149: Sorry, no help for %s"));
EXTERN حرف e_not_a_directory_str[]
	INIT(= N_("E150: Not a directory: %s"));
EXTERN حرف e_no_تطابق_str_1[]
	INIT(= N_("E151: No match: %s"));
EXTERN حرف e_cannot_open_str_for_writing_1[]
	INIT(= N_("E152: Cannot open %s for writing"));
EXTERN حرف e_unable_مقصد_open_str_for_reading[]
	INIT(= N_("E153: Unable to open %s for reading"));
EXTERN حرف e_duplicate_برچسب_str_درون_file_str_str[]
	INIT(= N_("E154: Duplicate tag \"%s\" in file %s/%s"));
#اگر_تعریف_شده FEAT_SIGNS
EXTERN حرف e_unknown_sign_str[]
	INIT(= N_("E155: Unknown sign: %s"));
EXTERN حرف e_missing_sign_name[]
	INIT(= N_("E156: Missing sign name"));
EXTERN حرف e_invalid_sign_id_nr[]
	INIT(= N_("E157: Invalid sign ID: %d"));
#پایان_اگر
#إذا defined(FEAT_SIGNS) || defined(FEAT_EVAL)
EXTERN حرف e_invalid_میانگیر_name_str[]
	INIT(= N_("E158: Invalid buffer name: %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
EXTERN حرف e_missing_sign_عدد[]
	INIT(= N_("E159: Missing sign number"));
EXTERN حرف e_unknown_sign_command_str[]
	INIT(= N_("E160: Unknown sign command: %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_breakpoint_not_یافت_str[]
	INIT(= N_("E161: Breakpoint not found: %s"));
#پایان_اگر
EXTERN حرف e_no_write_since_آخرین_change_for_میانگیر_str[]
	INIT(= N_("E162: No write since last change for buffer \"%s\""));
EXTERN حرف e_there_is_only_one_file_مقصد_edit[]
	INIT(= N_("E163: There is only one file to edit"));
EXTERN حرف e_cannot_go_before_اولی_file[]
	INIT(= N_("E164: Cannot go before first file"));
EXTERN حرف e_cannot_go_beyond_آخرین_file[]
	INIT(= N_("E165: Cannot go beyond last file"));
EXTERN حرف e_cant_open_linked_file_for_writing[]
	INIT(= N_("E166: Can't open linked file for writing"));
EXTERN حرف e_scriptencoding_استفاده_شده_outside_از_sourced_file[]
	INIT(= N_("E167: :scriptencoding used outside of a sourced file"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_پایان_استفاده_شده_outside_از_sourced_file[]
	INIT(= N_("E168: :finish used outside of a sourced file"));
#پایان_اگر
EXTERN حرف e_command_too_recursive[]
	INIT(= N_("E169: Command too recursive"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_missing_endwhile[]
	INIT(= N_("E170: Missing :endwhile"));
EXTERN حرف e_missing_endfor[]
	INIT(= N_("E170: Missing :endfor"));
EXTERN حرف e_missing_endif[]
	INIT(= N_("E171: Missing :endif"));
EXTERN حرف e_missing_marker[]
	INIT(= N_("E172: Missing marker"));
#پایان_اگر

PLURAL_پیام(e_nr_بیشتر_file_مقصد_edit, "E173: %d more file to edit",
		e_nr_بیشتر_files_مقصد_edit, "E173: %d more files to edit")

EXTERN حرف e_command_already_exists_جمع_کن_bang_مقصد_replace_it_str[]
	INIT(= N_("E174: Command already exists: add ! to replace it: %s"));
EXTERN حرف e_no_attribute_specified[]
	INIT(= N_("E175: No attribute specified"));
EXTERN حرف e_invalid_عدد_از_arguments[]
	INIT(= N_("E176: Invalid number of arguments"));
EXTERN حرف e_شمار_cannot_be_specified_twice[]
	INIT(= N_("E177: Count cannot be specified twice"));
EXTERN حرف e_invalid_default_مقدار_for_شمار[]
	INIT(= N_("E178: Invalid default value for count"));
EXTERN حرف e_آرگومان_required_for_str[]
	INIT(= N_("E179: Argument required for %s"));
EXTERN حرف e_invalid_complete_مقدار_str[]
	INIT(= N_("E180: Invalid complete value: %s"));
EXTERN حرف e_invalid_آدرس_type_مقدار_str[]
	INIT(= N_("E180: Invalid address type value: %s"));
EXTERN حرف e_invalid_attribute_str[]
	INIT(= N_("E181: Invalid attribute: %s"));
EXTERN حرف e_invalid_command_name[]
	INIT(= N_("E182: Invalid command name"));
EXTERN حرف e_user_defined_commands_must_شروع_with_an_uppercase_letter[]
	INIT(= N_("E183: User defined commands must start with an uppercase letter"));
EXTERN حرف e_no_such_user_defined_command_str[]
	INIT(= N_("E184: No such user-defined command: %s"));
EXTERN حرف e_cannot_پیدا_کن_color_scheme_str[]
	INIT(= N_("E185: Cannot find color scheme '%s'"));
EXTERN حرف e_no_قبلی_directory[]
	INIT(= N_("E186: No previous directory"));
EXTERN حرف e_directory_unknown[]
	INIT(= N_("E187: Directory unknown"));
EXTERN حرف e_obtaining_پنجره_position_not_implemented_for_this_platform[]
	INIT(= N_("E188: Obtaining window position not implemented for this platform"));
EXTERN حرف e_str_exists_جمع_کن_bang_مقصد_override[]
	INIT(= N_("E189: \"%s\" exists (add ! to override)"));
EXTERN حرف e_cannot_open_str_for_writing_2[]
	INIT(= N_("E190: Cannot open \"%s\" for writing"));
EXTERN حرف e_آرگومان_must_be_letter_or_forward_backward_quote[]
	INIT(= N_("E191: Argument must be a letter or forward/backward quote"));
EXTERN حرف e_recursive_use_از_نرمال_too_deep[]
	INIT(= N_("E192: Recursive use of :normal too deep"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_str_not_inside_function[]
	INIT(= N_("E193: %s not inside a function"));
#پایان_اگر
EXTERN حرف e_no_alternate_file_name_مقصد_substitute_for_hash[]
	INIT(= N_("E194: No alternate file name to substitute for '#'"));
#اگر_تعریف_شده FEAT_VIMINFO
EXTERN حرف e_cannot_open_viminfo_file_for_reading[]
	INIT(= N_("E195: Cannot open viminfo file for reading"));
#پایان_اگر
#اگر_تعریف_نشده FEAT_DIGRAPHS
EXTERN حرف e_no_digraphs_نسخه[]
	INIT(= N_("E196: No digraphs in this version"));
#پایان_اگر
EXTERN حرف e_cannot_مجموعه_language_مقصد_str[]
	INIT(= N_("E197: Cannot set language to \"%s\""));
// E198 unused
EXTERN حرف e_فعال_پنجره_or_میانگیر_تغییریافته_or_deleted[]
	INIT(= N_("E199: Active window or buffer changed or deleted"));
EXTERN حرف e_readpre_autocommands_made_file_unreadable[]
	INIT(= N_("E200: *ReadPre autocommands made the file unreadable"));
EXTERN حرف e_readpre_autocommands_must_not_change_current_میانگیر[]
	INIT(= N_("E201: *ReadPre autocommands must not change current buffer"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_conversion_mad_file_unreadable[]
	INIT(= N_("E202: Conversion made file unreadable!"));
#پایان_اگر
EXTERN حرف e_autocommands_deleted_or_unloaded_میانگیر_مقصد_be_written[]
	INIT(= N_("E203: Autocommands deleted or unloaded buffer to be written"));
EXTERN حرف e_autocommands_تغییریافته_عدد_از_سطرها_درون_unexpected_way[]
	INIT(= N_("E204: Autocommand changed number of lines in unexpected way"));
EXTERN حرف e_patchmode_cant_save_original_file[]
	INIT(= N_("E205: Patchmode: can't save original file"));
EXTERN حرف e_patchmode_cant_touch_خالی_original_file[]
	INIT(= N_("E206: Patchmode: can't touch empty original file"));
EXTERN حرف e_cant_delete_backup_file[]
	INIT(= N_("E207: Can't delete backup file"));
EXTERN حرف e_error_writing_مقصد_str[]
	INIT(= N_("E208: Error writing to \"%s\""));
EXTERN حرف e_error_closing_str[]
	INIT(= N_("E209: Error closing \"%s\""));
EXTERN حرف e_error_reading_str[]
	INIT(= N_("E210: Error reading \"%s\""));
EXTERN حرف e_file_str_no_longer_موجود[]
	INIT(= N_("E211: File \"%s\" no longer available"));
EXTERN حرف e_cant_open_file_for_writing[]
	INIT(= N_("E212: Can't open file for writing"));
EXTERN حرف e_cannot_convert_جمع_کن_bang_مقصد_write_without_conversion[]
	INIT(= N_("E213: Cannot convert (add ! to write without conversion)"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cant_پیدا_کن_موقت_file_for_writing[]
	INIT(= N_("E214: Can't find temp file for writing"));
#پایان_اگر
EXTERN حرف e_illegal_character_after_ستاره_str[]
	INIT(= N_("E215: Illegal character after *: %s"));
EXTERN حرف e_no_such_event_str[]
	INIT(= N_("E216: No such event: %s"));
EXTERN حرف e_no_such_گروه_or_event_str[]
	INIT(= N_("E216: No such group or event: %s"));
EXTERN حرف e_cant_execute_autocommands_for_all_events[]
	INIT(= N_("E217: Can't execute autocommands for ALL events"));
EXTERN حرف e_autocommand_nesting_too_deep[]
	INIT(= N_("E218: Autocommand nesting too deep"));
EXTERN حرف e_missing_open_curly[]
	INIT(= N_("E219: Missing {."));
EXTERN حرف e_missing_close_curly[]
	INIT(= N_("E220: Missing }."));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_marker_cannot_شروع_with_پایینی_case_letter[]
	INIT(= N_("E221: Marker cannot start with lower case letter"));
#پایان_اگر
EXTERN حرف e_جمع_کن_مقصد_داخلی_میانگیر_that_was_already_read_from[]
	INIT(= "E222: Add to internal buffer that was already read from");
EXTERN حرف e_recursive_mapping[]
	INIT(= N_("E223: Recursive mapping"));
EXTERN حرف e_global_abbreviation_already_exists_for_str[]
	INIT(= N_("E224: Global abbreviation already exists for %s"));
EXTERN حرف e_global_mapping_already_exists_for_str[]
	INIT(= N_("E225: Global mapping already exists for %s"));
EXTERN حرف e_abbreviation_already_exists_for_str[]
	INIT(= N_("E226: Abbreviation already exists for %s"));
EXTERN حرف e_mapping_already_exists_for_str[]
	INIT(= N_("E227: Mapping already exists for %s"));
EXTERN حرف e_makemap_illegal_نما[]
	INIT(= "E228: makemap: Illegal mode");
#اگر_تعریف_شده FEAT_GUI
EXTERN حرف e_cannot_شروع_the_GUI[]
	INIT(= N_("E229: Cannot start the GUI"));
EXTERN حرف e_cannot_read_from_str[]
	INIT(= N_("E230: Cannot read from \"%s\""));
EXTERN حرف e_guifontwide_invalid[]
	INIT(= N_("E231: 'guifontwide' invalid"));
# ifdef FEAT_BEVAL_GUI
EXTERN حرف e_cannot_create_ballooneval_with_both_پیام_and_callback[]
	INIT(= "E232: Cannot create BalloonEval with both message and callback");
# endif
# إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
EXTERN حرف e_cannot_open_display[]
	INIT(= N_("E233: Cannot open display"));
# endif
# إذا defined(FEAT_XFONTSET)
EXTERN حرف e_unknown_fontset_str[]
	INIT(= N_("E234: Unknown fontset: %s"));
# endif
# إذا defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_PHOTON) || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
EXTERN حرف e_unknown_font_str[]
	INIT(= N_("E235: Unknown font: %s"));
# endif
# إذا defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
EXTERN حرف e_font_str_is_not_fixed_عرض[]
	INIT(= N_("E236: Font \"%s\" is not fixed-width"));
# endif
#پایان_اگر
#إذا defined(MSWIN) || defined(FEAT_PRINT_PANGO)
EXTERN حرف e_printer_selection_failed[]
	INIT(= N_("E237: Printer selection failed"));
EXTERN حرف e_print_error_str[]
	INIT(= N_("E238: Print error: %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
EXTERN حرف e_invalid_sign_متن_str[]
	INIT(= N_("E239: Invalid sign text: %s"));
#پایان_اگر
#إذا defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
EXTERN حرف e_no_connection_مقصد_x_server[]
	INIT(= N_("E240: No connection to the X server"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CLIENTSERVER
EXTERN حرف e_unable_مقصد_send_مقصد_str[]
	INIT(= N_("E241: Unable to send to %s"));
#پایان_اگر
EXTERN حرف e_cant_تقسیم_پنجره_while_closing_another[]
	INIT(= N_("E242: Can't split a window while closing another"));
#إذا defined(FEAT_GUI_MSWIN) && !defined(FEAT_OLE)
EXTERN حرف e_آرگومان_not_supported_str_use_ole_نسخه[]
	INIT(= N_("E243: Argument not supported: \"-%s\"; Use the OLE version."));
#پایان_اگر
#اگر_تعریف_شده MSWIN
EXTERN حرف e_illegal_str_name_str_درون_font_name_str[]
	INIT(= N_("E244: Illegal %s name \"%s\" in font name \"%s\""));
EXTERN حرف e_illegal_char_nr_درون_font_name_str[]
	INIT(= N_("E245: Illegal char '%c' in font name \"%s\""));
#پایان_اگر
EXTERN حرف e_filechangedshell_autocommand_deleted_میانگیر[]
	INIT(= N_("E246: FileChangedShell autocommand deleted buffer"));
#اگر_تعریف_شده FEAT_CLIENTSERVER
EXTERN حرف e_no_registered_server_named_str[]
	INIT(= N_("E247: No registered server named \"%s\""));
EXTERN حرف e_failed_مقصد_send_command_مقصد_destination_برنامه[]
	INIT(= N_("E248: Failed to send command to the destination program"));
#پایان_اگر
EXTERN حرف e_پنجره_layout_تغییریافته_unexpectedly[]
	INIT(= N_("E249: Window layout changed unexpectedly"));
#اگر_تعریف_شده FEAT_XFONTSET
EXTERN حرف e_fonts_for_the_following_charsets_are_missing_درون_fontset[]
	INIT(= N_("E250: Fonts for the following charsets are missing in fontset %s:"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CLIENTSERVER
EXTERN حرف e_vim_instance_registry_property_is_badly_formed_deleted[]
	INIT(= N_("E251: VIM instance registry property is badly formed.  Deleted!"));
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_X11
EXTERN حرف e_fontsent_name_str_font_str_is_not_fixed_عرض[]
	INIT(= N_("E252: Fontset name: %s - Font '%s' is not fixed-width"));
EXTERN حرف e_fontset_name_str[]
	INIT(= N_("E253: Fontset name: %s"));
#پایان_اگر
#إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
EXTERN حرف e_cannot_allocate_color_str[]
	INIT(= N_("E254: Cannot allocate color %s"));
#پایان_اگر
#إذا defined(FEAT_SIGN_ICONS) && !defined(FEAT_GUI_GTK)
EXTERN حرف e_couldnt_read_درون_sign_داده[]
	INIT(= N_("E255: Couldn't read in sign data"));
#پایان_اگر
// E256 unused
#اگر_تعریف_شده FEAT_CSCOPE
EXTERN حرف e_cstag_برچسب_not_founc[]
	INIT(= N_("E257: cstag: Tag not found"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CLIENTSERVER
EXTERN حرف e_unable_مقصد_send_مقصد_client[]
	INIT(= N_("E258: Unable to send to client"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CSCOPE
EXTERN حرف e_no_matches_یافت_for_cscope_query_str_از_str[]
	INIT(= N_("E259: No matches found for cscope query %s of %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_missing_name_after_روش[]
	INIT(= N_("E260: Missing name after ->"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CSCOPE
EXTERN حرف e_cscope_connection_str_not_founc[]
	INIT(= N_("E261: Cscope connection %s not found"));
EXTERN حرف e_error_reading_cscope_connection_nr[]
	INIT(= N_("E262: Error reading cscope connection %d"));
#پایان_اگر
#إذا defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
EXTERN حرف e_sorry_this_command_is_disabled_python_library_could_not_be_یافت[]
	INIT(= N_("E263: Sorry, this command is disabled, the Python library could not be loaded."));
#پایان_اگر
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN حرف e_python_error_initialising_io_object[]
	INIT(= N_("E264: Python: Error initialising I/O objects"));
#پایان_اگر
#اگر_تعریف_شده FEAT_RUBY
EXTERN حرف e_dollar_must_be_an_instance_از_string[]
	INIT(= N_("E265: $_ must be an instance of String"));
#پایان_اگر
#اگر_تعریف_شده DYNAMIC_RUBY
EXTERN حرف e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded[]
	INIT(= N_("E266: Sorry, this command is disabled, the Ruby library could not be loaded."));
#پایان_اگر
#اگر_تعریف_شده FEAT_RUBY
EXTERN حرف e_unexpected_return[]
	INIT(= N_("E267: Unexpected return"));
EXTERN حرف e_unexpected_بعدی[]
	INIT(= N_("E268: Unexpected next"));
EXTERN حرف e_unexpected_break[]
	INIT(= N_("E269: Unexpected break"));
EXTERN حرف e_unexpected_redo[]
	INIT(= N_("E270: Unexpected redo"));
EXTERN حرف e_retry_outside_از_rescue_clause[]
	INIT(= N_("E271: Retry outside of rescue clause"));
EXTERN حرف e_unhandled_exception[]
	INIT(= N_("E272: Unhandled exception"));
EXTERN حرف e_unknown_longjmp_وضعیت_nr[]
	INIT(= N_("E273: Unknown longjmp status %d"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_no_white_space_allowed_before_parenthesis[]
	INIT(= N_("E274: No white space allowed before parenthesis"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_cannot_جمع_کن_متن_property_مقصد_unloaded_میانگیر[]
	INIT(= N_("E275: Cannot add text property to unloaded buffer"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_use_function_as_روش_str[]
	INIT(= N_("E276: Cannot use function as a method: %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CLIENTSERVER
EXTERN حرف e_unable_مقصد_read_server_پاسخ[]
	INIT(= N_("E277: Unable to read a server reply"));
#پایان_اگر
// E278 unused
#إذا defined(FEAT_TERMINAL) && !defined(UNIX) && !defined(MSWIN)
EXTERN حرف e_sorry_plusplusshell_not_supported_on_this_system[]
	INIT(= N_("E279: Sorry, ++shell is not supported on this system"));
#پایان_اگر
#اگر_تعریف_شده FEAT_TCL
EXTERN حرف e_tcl_fatal_error_reflist_corrupt_please_report_this[]
	INIT(= "E280: TCL FATAL ERROR: reflist corrupt!? Please report this to vim-dev@vim.org");
#پایان_اگر
// E281 unused
EXTERN حرف e_cannot_read_from_str_2[]
	INIT(= N_("E282: Cannot read from \"%s\""));
EXTERN حرف e_no_marks_matching_str[]
	INIT(= N_("E283: No marks matching \"%s\""));
#اگر_تعریف_شده FEAT_XIM
# ifndef FEAT_GUI_GTK
EXTERN حرف e_cannot_مجموعه_ic_مقادیر[]
	INIT(= N_("E284: Cannot set IC values"));
# endif
# ifdef FEAT_GUI_X11
EXTERN حرف e_failed_مقصد_create_درون_داد_context[]
	INIT(= N_("E285: Failed to create input context"));
EXTERN حرف e_failed_مقصد_open_درون_داد_روش[]
	INIT(= N_("E286: Failed to open input method"));
EXTERN حرف e_warning_could_not_مجموعه_destroy_callback_مقصد_im[]
	INIT(= N_("E287: Warning: Could not set destroy callback to IM"));
EXTERN حرف e_درون_داد_روش_doesnt_support_any_style[]
	INIT(= N_("E288: Input method doesn't support any style"));
EXTERN حرف e_درون_داد_روش_doesnt_support_my_preedit_type[]
	INIT(= N_("E289: Input method doesn't support my preedit type"));
# endif
#پایان_اگر
#اگر_تعریف_شده FEAT_جستجو_EXTRA
EXTERN حرف e_list_or_عدد_required[]
	INIT(= N_("E290: List or number required"));
#پایان_اگر
// E291 unused
EXTERN حرف e_invalid_شمار_for_del_بایتها_nr[]
	INIT(= "E292: Invalid count for del_bytes(): %ld");
EXTERN حرف e_بلوک_was_not_locked[]
	INIT(= "E293: Block was not locked");
EXTERN حرف e_seek_error_درون_جابجا_file_read[]
	INIT(= N_("E294: Seek error in swap file read"));
EXTERN حرف e_read_error_درون_جابجا_file[]
	INIT(= N_("E295: Read error in swap file"));
EXTERN حرف e_seek_error_درون_جابجا_file_write[]
	INIT(= N_("E296: Seek error in swap file write"));
EXTERN حرف e_write_error_درون_جابجا_file[]
	INIT(= N_("E297: Write error in swap file"));
EXTERN حرف e_didnt_get_بلوک_nr_zero[]
	INIT(= "E298: Didn't get block nr 0?");
EXTERN حرف e_didnt_get_بلوک_nr_one[]
	INIT(= "E298: Didn't get block nr 1?");
EXTERN حرف e_didnt_get_بلوک_nr_two[]
	INIT(= "E298: Didn't get block nr 2?");
#اگر_تعریف_شده FEAT_PERL
EXTERN حرف e_perl_evaluation_forbidden_درون_sandbox_without_امن_module[]
	INIT(= N_("E299: Perl evaluation forbidden in sandbox without the Safe module"));
#پایان_اگر
EXTERN حرف e_جابجا_file_already_exists_symlink_attack[]
	INIT(= N_("E300: Swap file already exists (symlink attack?)"));
EXTERN حرف e_oops_گمشده_the_جابجا_file[]
	INIT(= N_("E301: Oops, lost the swap file!!!"));
EXTERN حرف e_could_not_rename_جابجا_file[]
	INIT(= N_("E302: Could not rename swap file"));
EXTERN حرف e_unable_مقصد_open_جابجا_file_for_str_recovery_impossible[]
	INIT(= N_("E303: Unable to open swap file for \"%s\", recovery impossible"));
EXTERN حرف e_ml_upd_block0_didnt_get_بلوک_zero[]
	INIT(= "E304: ml_upd_block0(): Didn't get block 0??");
EXTERN حرف e_no_جابجا_file_یافت_for_str[]
	INIT(= N_("E305: No swap file found for %s"));
EXTERN حرف e_cannot_open_str[]
	INIT(= N_("E306: Cannot open %s"));
EXTERN حرف e_str_does_not_look_like_vim_جابجا_file[]
	INIT(= N_("E307: %s does not look like a Vim swap file"));
EXTERN حرف e_warning_original_file_may_have_been_تغییریافته[]
	INIT(= N_("E308: Warning: Original file may have been changed"));
EXTERN حرف e_unable_مقصد_read_بلوک_one_from_str[]
	INIT(= N_("E309: Unable to read block 1 from %s"));
EXTERN حرف e_بلوک_one_id_wrong_str_not_swp_file[]
	INIT(= N_("E310: Block 1 ID wrong (%s not a .swp file?)"));
EXTERN حرف e_recovery_interrupted[]
	INIT(= N_("E311: Recovery Interrupted"));
EXTERN حرف e_errors_detected_while_recovering_look_for_سطرها_starting_with_questions[]
	INIT(= N_("E312: Errors detected while recovering; look for lines starting with ???"));
EXTERN حرف e_cannot_preserve_there_is_no_جابجا_file[]
	INIT(= N_("E313: Cannot preserve, there is no swap file"));
EXTERN حرف e_preserve_failed[]
	INIT(= N_("E314: Preserve failed"));
EXTERN حرف e_ml_get_invalid_lnum_nr[]
	INIT(= "E315: ml_get: Invalid lnum: %ld");
EXTERN حرف e_ml_get_cannot_پیدا_کن_سطر_nr_درون_میانگیر_nr_str[]
	INIT(= "E316: ml_get: Cannot find line %ld in buffer %d %s");
EXTERN حرف e_اشاره_گر_بلوک_id_wrong[]
	INIT(= "E317: Pointer block id wrong");
EXTERN حرف e_اشاره_گر_بلوک_id_wrong_two[]
	INIT(= "E317: Pointer block id wrong 2");
EXTERN حرف e_اشاره_گر_بلوک_id_wrong_three[]
	INIT(= "E317: Pointer block id wrong 3");
EXTERN حرف e_اشاره_گر_بلوک_id_wrong_four[]
	INIT(= "E317: Pointer block id wrong 4");
EXTERN حرف e_updated_too_many_بلوکها[]
	INIT(= "E318: Updated too many blocks?");
EXTERN حرف e_sorry_command_is_not_موجود_درون_this_نسخه[]
	INIT(= N_("E319: Sorry, the command is not available in this version"));
EXTERN حرف e_cannot_پیدا_کن_سطر_nr[]
	INIT(= "E320: Cannot find line %ld");
EXTERN حرف e_could_not_reload_str[]
	INIT(= N_("E321: Could not reload \"%s\""));
EXTERN حرف e_سطر_عدد_بیرون_از_range_nr_past_the_انتها[]
	INIT(= "E322: Line number out of range: %ld past the end");
EXTERN حرف e_سطر_شمار_wrong_درون_بلوک_nr[]
	INIT(= "E323: Line count wrong in block %ld");
#إذا defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
EXTERN حرف e_cant_open_postscript_output_file[]
	INIT(= N_("E324: Can't open PostScript output file"));
#پایان_اگر
EXTERN حرف e_attention[]
	INIT(= N_("E325: ATTENTION"));
EXTERN حرف e_too_many_جابجا_files_یافت[]
	INIT(= N_("E326: Too many swap files found"));
#اگر_تعریف_شده FEAT_MENU
EXTERN char_u e_part_از_menu_قلم_مسیر_is_not_sub_menu[]
	INIT(= N_("E327: Part of menu-item path is not sub-menu"));
EXTERN حرف e_menu_only_exists_درون_another_نما[]
	INIT(= N_("E328: Menu only exists in another mode"));
EXTERN char_u e_no_menu_str[]
	INIT(= N_("E329: No menu \"%s\""));
EXTERN حرف e_menu_مسیر_must_not_lead_مقصد_sub_menu[]
	INIT(= N_("E330: Menu path must not lead to a sub-menu"));
EXTERN حرف e_must_not_جمع_کن_menu_اقلام_directly_مقصد_menu_bar[]
	INIT(= N_("E331: Must not add menu items directly to menu bar"));
EXTERN حرف e_separator_cannot_be_part_از_menu_مسیر[]
	INIT(= N_("E332: Separator cannot be part of a menu path"));
EXTERN حرف e_menu_مسیر_must_lead_مقصد_menu_قلم[]
	INIT(= N_("E333: Menu path must lead to a menu item"));
EXTERN حرف e_menu_not_یافت_str[]
	INIT(= N_("E334: Menu not found: %s"));
EXTERN حرف e_menu_not_defined_for_str_نما[]
	INIT(= N_("E335: Menu not defined for %s mode"));
EXTERN حرف e_menu_مسیر_must_lead_مقصد_sub_menu[]
	INIT(= N_("E336: Menu path must lead to a sub-menu"));
EXTERN حرف e_menu_not_یافت_check_menu_نامها[]
	INIT(= N_("E337: Menu not found - check menu names"));
#پایان_اگر
#اگر_تعریف_شده FEAT_BROWSE
EXTERN حرف e_sorry_no_file_browser_درون_console_نما[]
	INIT(= N_("E338: Sorry, no file browser in console mode"));
#پایان_اگر
EXTERN حرف e_الگو_too_long[]
	INIT(= N_("E339: Pattern too long"));
EXTERN حرف e_داخلی_error_please_report_a_bug[]
	INIT(= N_("E340: Internal error; if you can reproduce please report a bug"));
EXTERN حرف e_داخلی_error_lalloc_zero[]
	INIT(= "E341: Internal error: lalloc(0, )");
EXTERN حرف e_بیرون_از_حافظه_allocating_nr_بایتها[]
	INIT(= N_("E342: Out of memory!  (allocating %lu bytes)"));
EXTERN حرف e_invalid_مسیر_عدد_must_be_at_انتها_از_مسیر_or_be_followed_by_str[]
	INIT(= N_("E343: Invalid path: '**[number]' must be at the end of the path or be followed by '%s'."));
EXTERN حرف e_cant_پیدا_کن_directory_str_درون_cdpath[]
	INIT(= N_("E344: Can't find directory \"%s\" in cdpath"));
EXTERN حرف e_cant_پیدا_کن_file_str_درون_مسیر[]
	INIT(= N_("E345: Can't find file \"%s\" in path"));
EXTERN حرف e_no_بیشتر_directory_str_یافت_درون_cdpath[]
	INIT(= N_("E346: No more directory \"%s\" found in cdpath"));
EXTERN حرف e_no_بیشتر_file_str_یافت_درون_مسیر[]
	INIT(= N_("E347: No more file \"%s\" found in path"));
EXTERN حرف e_no_string_under_cursor[]
	INIT(= N_("E348: No string under cursor"));
EXTERN حرف e_no_identifier_under_cursor[]
	INIT(= N_("E349: No identifier under cursor"));
#اگر_تعریف_شده FEAT_FOLDING
EXTERN حرف e_cannot_create_fold_with_current_foldmethod[]
	INIT(= N_("E350: Cannot create fold with current 'foldmethod'"));
EXTERN حرف e_cannot_delete_fold_with_current_foldmethod[]
	INIT(= N_("E351: Cannot delete fold with current 'foldmethod'"));
EXTERN حرف e_cannot_erase_folds_with_current_foldmethod[]
	INIT(= N_("E352: Cannot erase folds with current 'foldmethod'"));
#پایان_اگر
EXTERN حرف e_nothing_درون_ثبات_str[]
	INIT(= N_("E353: Nothing in register %s"));
EXTERN حرف e_invalid_ثبات_name_str[]
	INIT(= N_("E354: Invalid register name: '%s'"));
EXTERN حرف e_unknown_option_str_2[]
	INIT(= N_("E355: Unknown option: %s"));
EXTERN حرف e_get_varp_error[]
	INIT(= "E356: get_varp ERROR");
#اگر_تعریف_شده FEAT_LANGMAP
EXTERN حرف e_langmap_matching_character_missing_for_str[]
	INIT(= N_("E357: 'langmap': Matching character missing for %s"));
EXTERN حرف e_langmap_extra_characters_after_semicolon_str[]
	INIT(= N_("E358: 'langmap': Extra characters after semicolon: %s"));
#پایان_اگر
#إذا defined(AMIGA) || defined(MACOS_X) || defined(MSWIN)  \
	|| defined(UNIX) || defined(VMS)
EXTERN حرف e_screen_نما_setting_not_supported[]
	INIT(= N_("E359: Screen mode setting not supported"));
#پایان_اگر
#اگر_تعریف_شده AMIGA
EXTERN حرف e_cannot_execute_shell_with_f_option[]
	INIT(= N_("E360: Cannot execute shell with -f option"));
#پایان_اگر
// E361 unused
#إذا defined(FEAT_EVAL)
EXTERN حرف e_using_boolean_مقدار_as_float[]
	INIT(= N_("E362: Using a boolean value as a Float"));
#پایان_اگر
EXTERN حرف e_الگو_استفاده_میکند_بیشتر_حافظه_than_maxmempattern[]
	INIT(= N_("E363: Pattern uses more memory than 'maxmempattern'"));
#اگر_تعریف_شده FEAT_LIBCALL
EXTERN حرف e_library_call_failed_for_str[]
	INIT(= N_("E364: Library call failed for \"%s()\""));
#پایان_اگر
#إذا defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
EXTERN حرف e_failed_مقصد_print_postscript_file[]
	INIT(= N_("E365: Failed to print PostScript file"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_not_allowed_مقصد_enter_popup_پنجره[]
	INIT(= N_("E366: Not allowed to enter a popup window"));
#پایان_اگر
EXTERN حرف e_no_such_گروه_str[]
	INIT(= N_("E367: No such group: \"%s\""));
#اگر_تعریف_شده FEAT_LIBCALL
EXTERN حرف e_got_sig_str_درون_libcall[]
	INIT(= N_("E368: Got SIG%s in libcall()"));
#پایان_اگر
EXTERN حرف e_invalid_قلم_درون_str_brackets[]
	INIT(= N_("E369: Invalid item in %s%%[]"));
#اگر_تعریف_شده USING_بارگیری_LIBRARY
EXTERN حرف e_could_not_بارگیری_library_str_str[]
	INIT(= N_("E370: Could not load library %s: %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_MSWIN
EXTERN حرف e_command_not_یافت[]
	INIT(= N_("E371: Command not found"));
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_too_many_chr_درون_format_string[]
	INIT(= N_("E372: Too many %%%c in format string"));
EXTERN حرف e_unexpected_chr_درون_format_str[]
	INIT(= N_("E373: Unexpected %%%c in format string"));
EXTERN حرف e_missing_rsb_درون_format_string[]
	INIT(= N_("E374: Missing ] in format string"));
EXTERN حرف e_unsupported_chr_درون_format_string[]
	INIT(= N_("E375: Unsupported %%%c in format string"));
EXTERN حرف e_invalid_chr_درون_format_string_پیشوند[]
	INIT(= N_("E376: Invalid %%%c in format string prefix"));
EXTERN حرف e_invalid_chr_درون_format_string[]
	INIT(= N_("E377: Invalid %%%c in format string"));
EXTERN حرف e_errorformat_شامل_است_no_الگو[]
	INIT(= N_("E378: 'errorformat' contains no pattern"));
EXTERN حرف e_missing_or_خالی_directory_name[]
	INIT(= N_("E379: Missing or empty directory name"));
EXTERN حرف e_at_bottom_از_quickfix_پشته[]
	INIT(= N_("E380: At bottom of quickfix stack"));
EXTERN حرف e_at_بالاترین_از_quickfix_پشته[]
	INIT(= N_("E381: At top of quickfix stack"));
#پایان_اگر
EXTERN حرف e_cannot_write_buftype_option_is_مجموعه[]
	INIT(= N_("E382: Cannot write, 'buftype' option is set"));
EXTERN حرف e_invalid_جستجو_string_str[]
	INIT(= N_("E383: Invalid search string: %s"));
EXTERN حرف e_جستجو_اصابت_بالاترین_without_تطابق_for_str[]
	INIT(= N_("E384: Search hit TOP without match for: %s"));
EXTERN حرف e_جستجو_اصابت_bottom_without_تطابق_for_str[]
	INIT(= N_("E385: Search hit BOTTOM without match for: %s"));
EXTERN حرف e_expected_question_or_slash_after_semicolon[]
	INIT(= N_("E386: Expected '?' or '/'  after ';'"));
#اگر_تعریف_شده FEAT_پیدا_کن_ID
EXTERN حرف e_تطابق_is_on_current_سطر[]
	INIT(= N_("E387: Match is on current line"));
EXTERN حرف e_couldnt_پیدا_کن_definition[]
	INIT(= N_("E388: Couldn't find definition"));
EXTERN حرف e_couldnt_پیدا_کن_الگو[]
	INIT(= N_("E389: Couldn't find pattern"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN حرف e_illegal_آرگومان_str_2[]
	INIT(= N_("E390: Illegal argument: %s"));
EXTERN حرف e_no_such_syntax_cluster_str_1[]
	INIT(= N_("E391: No such syntax cluster: %s"));
EXTERN حرف e_no_such_syntax_cluster_str_2[]
	INIT(= N_("E392: No such syntax cluster: %s"));
EXTERN حرف e_groupthere_not_accepted_اینجا[]
	INIT(= N_("E393: group[t]here not accepted here"));
EXTERN حرف e_didnt_پیدا_کن_region_قلم_for_str[]
	INIT(= N_("E394: Didn't find region item for %s"));
EXTERN حرف e_شامل_است_آرگومان_not_accepted_اینجا[]
	INIT(= N_("E395: Contains argument not accepted here"));
// E396 unused
EXTERN حرف e_filename_required[]
	INIT(= N_("E397: Filename required"));
EXTERN حرف e_missing_equal_str[]
	INIT(= N_("E398: Missing '=': %s"));
EXTERN حرف e_not_enough_arguments_syntax_region_str[]
	INIT(= N_("E399: Not enough arguments: syntax region %s"));
EXTERN حرف e_no_cluster_specified[]
	INIT(= N_("E400: No cluster specified"));
EXTERN حرف e_الگو_delimiter_not_یافت_str[]
	INIT(= N_("E401: Pattern delimiter not found: %s"));
EXTERN حرف e_garbage_after_الگو_str[]
	INIT(= N_("E402: Garbage after pattern: %s"));
EXTERN حرف e_syntax_sync_سطر_continuations_الگو_specified_twice[]
	INIT(= N_("E403: syntax sync: Line continuations pattern specified twice"));
EXTERN حرف e_illegal_arguments_str[]
	INIT(= N_("E404: Illegal arguments: %s"));
EXTERN حرف e_missing_equal_sign_str[]
	INIT(= N_("E405: Missing equal sign: %s"));
EXTERN حرف e_خالی_آرگومان_str[]
	INIT(= N_("E406: Empty argument: %s"));
EXTERN حرف e_str_not_allowed_اینجا[]
	INIT(= N_("E407: %s not allowed here"));
EXTERN حرف e_str_must_be_اولی_درون_شامل_است_list[]
	INIT(= N_("E408: %s must be first in contains list"));
EXTERN حرف e_unknown_گروه_name_str[]
	INIT(= N_("E409: Unknown group name: %s"));
EXTERN حرف e_invalid_syntax_subcommand_str[]
	INIT(= N_("E410: Invalid :syntax subcommand: %s"));
#پایان_اگر
EXTERN حرف e_highlight_گروه_name_not_یافت_str[]
	INIT(= N_("E411: Highlight group not found: %s"));
EXTERN حرف e_not_enough_arguments_highlight_link_str[]
	INIT(= N_("E412: Not enough arguments: \":highlight link %s\""));
EXTERN حرف e_too_many_arguments_highlight_link_str[]
	INIT(= N_("E413: Too many arguments: \":highlight link %s\""));
EXTERN حرف e_گروه_has_settings_highlight_link_ignored[]
	INIT(= N_("E414: Group has settings, highlight link ignored"));
EXTERN حرف e_unexpected_equal_sign_str[]
	INIT(= N_("E415: Unexpected equal sign: %s"));
EXTERN حرف e_missing_equal_sign_str_2[]
	INIT(= N_("E416: Missing equal sign: %s"));
EXTERN حرف e_missing_آرگومان_str[]
	INIT(= N_("E417: Missing argument: %s"));
EXTERN حرف e_illegal_مقدار_str[]
	INIT(= N_("E418: Illegal value: %s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_im_a_teapot[]
	INIT(= N_("E418: I'm a teapot"));
#پایان_اگر
EXTERN حرف e_fg_color_unknown[]
	INIT(= N_("E419: FG color unknown"));
EXTERN حرف e_bg_color_unknown[]
	INIT(= N_("E420: BG color unknown"));
EXTERN حرف e_color_name_or_عدد_not_recognized_str[]
	INIT(= N_("E421: Color name or number not recognized: %s"));
EXTERN حرف e_terminal_کد_too_long_str[]
	INIT(= N_("E422: Terminal code too long: %s"));
EXTERN حرف e_illegal_آرگومان_str_3[]
	INIT(= N_("E423: Illegal argument: %s"));
EXTERN حرف e_too_many_different_highlighting_صفات_درون_use[]
	INIT(= N_("E424: Too many different highlighting attributes in use"));
EXTERN حرف e_cannot_go_before_اولی_matching_برچسب[]
	INIT(= N_("E425: Cannot go before first matching tag"));
EXTERN حرف e_برچسب_not_یافت_str[]
	INIT(= N_("E426: Tag not found: %s"));
EXTERN حرف e_there_is_only_one_matching_برچسب[]
	INIT(= N_("E427: There is only one matching tag"));
EXTERN حرف e_cannot_go_beyond_آخرین_matching_برچسب[]
	INIT(= N_("E428: Cannot go beyond last matching tag"));
EXTERN حرف e_file_str_does_not_exist[]
	INIT(= N_("E429: File \"%s\" does not exist"));
#اگر_تعریف_شده FEAT_EMACS_برچسبها
EXTERN حرف e_برچسب_file_مسیر_truncated_for_str[]
	INIT(= N_("E430: Tag file path truncated for %s\n"));
#پایان_اگر
EXTERN حرف e_format_error_درون_برچسبها_file_str[]
	INIT(= N_("E431: Format error in tags file \"%s\""));
EXTERN حرف e_برچسبها_file_not_sorted_str[]
	INIT(= N_("E432: Tags file not sorted: %s"));
EXTERN حرف e_no_برچسبها_file[]
	INIT(= N_("E433: No tags file"));
EXTERN حرف e_cannot_پیدا_کن_برچسب_الگو[]
	INIT(= N_("E434: Can't find tag pattern"));
EXTERN حرف e_couldnt_پیدا_کن_برچسب_just_guessing[]
	INIT(= N_("E435: Couldn't find tag, just guessing!"));
EXTERN حرف e_no_str_مدخل_درون_termcap[]
	INIT(= N_("E436: No \"%s\" entry in termcap"));
EXTERN حرف e_terminal_capability_cm_required[]
	INIT(= N_("E437: Terminal capability \"cm\" required"));
EXTERN حرف e_u_undo_سطر_numbers_wrong[]
	INIT(= "E438: u_undo: Line numbers wrong");
EXTERN حرف e_undo_list_corrupt[]
	INIT(= "E439: Undo list corrupt");
EXTERN حرف e_undo_سطر_missing[]
	INIT(= "E440: Undo line missing");
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_there_is_no_preview_پنجره[]
	INIT(= N_("E441: There is no preview window"));
#پایان_اگر
EXTERN حرف e_cant_تقسیم_topleft_and_botright_at_the_same_زمان[]
	INIT(= N_("E442: Can't split topleft and botright at the same time"));
EXTERN حرف e_cannot_rotate_when_another_پنجره_is_تقسیم[]
	INIT(= N_("E443: Cannot rotate when another window is split"));
EXTERN حرف e_cannot_close_آخرین_پنجره[]
	INIT(= N_("E444: Cannot close last window"));
EXTERN حرف e_other_پنجره_شامل_است_changes[]
	INIT(= N_("E445: Other window contains changes"));
EXTERN حرف e_no_file_name_under_cursor[]
	INIT(= N_("E446: No file name under cursor"));
EXTERN حرف e_cant_پیدا_کن_file_str_درون_مسیر_2[]
	INIT(= N_("E447: Can't find file \"%s\" in path"));
#اگر_تعریف_شده USING_بارگیری_LIBRARY
EXTERN حرف e_could_not_بارگیری_library_function_str[]
	INIT(= N_("E448: Could not load library function %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CLIENTSERVER
EXTERN حرف e_invalid_عبارت_received[]
	INIT(= N_("E449: Invalid expression received"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_میانگیر_عدد_متن_or_list_required[]
	INIT(= N_("E450: Buffer number, text or a list required"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_expected_راست_curly_str[]
	INIT(= N_("E451: Expected }: %s"));
EXTERN حرف e_double_semicolon_درون_list_از_variables[]
	INIT(= N_("E452: Double ; in list of variables"));
#پایان_اگر
EXTERN حرف e_ul_color_unknown[]
	INIT(= N_("E453: UL color unknown"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_function_list_was_modified[]
	INIT(= N_("E454: Function list was modified"));
#پایان_اگر
#اگر_تعریف_شده FEAT_POSTSCRIPT
EXTERN حرف e_error_writing_مقصد_postscript_output_file[]
	INIT(= N_("E455: Error writing to PostScript output file"));
EXTERN حرف e_cant_open_file_str_2[]
	INIT(= N_("E456: Can't open file \"%s\""));
EXTERN حرف e_cant_پیدا_کن_postscript_resource_file_str_ps[]
	INIT(= N_("E456: Can't find PostScript resource file \"%s.ps\""));
EXTERN حرف e_cant_read_postscript_resource_file_str[]
	INIT(= N_("E457: Can't read PostScript resource file \"%s\""));
#پایان_اگر
// E458 unused
#إذا defined(UNIX) || defined(FEAT_SESSION) || defined(FEAT_SOCKETSERVER)
EXTERN حرف e_cannot_go_پشت_مقصد_قبلی_directory[]
	INIT(= N_("E459: Cannot go back to previous directory"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_entries_missing_درون_mapset_dict_آرگومان[]
	INIT(= N_("E460: Entries missing in mapset() dict argument"));
EXTERN حرف e_illegal_variable_name_str[]
	INIT(= N_("E461: Illegal variable name: %s"));
#پایان_اگر
EXTERN حرف e_could_not_prepare_for_reloading_str[]
	INIT(= N_("E462: Could not prepare for reloading \"%s\""));
#اگر_تعریف_شده FEAT_NETBEANS_INTG
EXTERN حرف e_region_is_guarded_cannot_modify[]
	INIT(= N_("E463: Region is guarded, cannot modify"));
#پایان_اگر
EXTERN حرف e_ambiguous_use_از_user_defined_command[]
	INIT(= N_("E464: Ambiguous use of user-defined command"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_ambiguous_use_از_user_defined_command_str[]
	INIT(= N_("E464: Ambiguous use of user-defined command: %s"));
#پایان_اگر
EXTERN حرف e_winsize_requires_two_عدد_arguments[]
	INIT(= N_("E465: :winsize requires two number arguments"));
EXTERN حرف e_winpos_requires_two_عدد_arguments[]
	INIT(= N_("E466: :winpos requires two number arguments"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_custom_completion_requires_function_آرگومان[]
	INIT(= N_("E467: Custom completion requires a function argument"));
#پایان_اگر
EXTERN حرف e_completion_آرگومان_only_allowed_for_custom_completion[]
	INIT(= N_("E468: Completion argument only allowed for custom completion"));
#اگر_تعریف_شده FEAT_CSCOPE
EXTERN حرف e_invalid_cscopequickfix_flag_chr_for_chr[]
	INIT(= N_("E469: Invalid cscopequickfix flag %c for %c"));
#پایان_اگر
EXTERN حرف e_command_aborted[]
	INIT(= N_("E470: Command aborted"));
EXTERN حرف e_آرگومان_required[]
	INIT(= N_("E471: Argument required"));
EXTERN حرف e_command_failed[]
	INIT(= N_("E472: Command failed"));
EXTERN حرف e_داخلی_error_درون_regexp[]
	INIT(= "E473: Internal error in regexp");
EXTERN حرف e_invalid_آرگومان[]
	INIT(= N_("E474: Invalid argument"));
EXTERN حرف e_invalid_آرگومان_str[]
	INIT(= N_("E475: Invalid argument: %s"));
EXTERN حرف e_invalid_مقدار_for_آرگومان_str[]
	INIT(= N_("E475: Invalid value for argument %s"));
#إذا defined(FEAT_کار_CHANNEL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL) || defined(FEAT_IMAGE)
EXTERN حرف e_invalid_مقدار_for_آرگومان_str_str[]
	INIT(= N_("E475: Invalid value for argument %s: %s"));
#پایان_اگر
EXTERN حرف e_invalid_command[]
	INIT(= N_("E476: Invalid command"));
EXTERN حرف e_invalid_command_str[]
	INIT(= N_("E476: Invalid command: %s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_invalid_command_str_expected_str[]
	INIT(= N_("E476: Invalid command: %s, expected %s"));
#پایان_اگر
EXTERN حرف e_no_bang_allowed[]
	INIT(= N_("E477: No ! allowed"));
EXTERN حرف e_dont_panic[]
	INIT(= N_("E478: Don't panic!"));
EXTERN حرف e_no_تطابق[]
	INIT(= N_("E479: No match"));
EXTERN حرف e_no_تطابق_str_2[]
	INIT(= N_("E480: No match: %s"));
EXTERN حرف e_no_range_allowed[]
	INIT(= N_("E481: No range allowed"));
EXTERN حرف e_cant_create_file_str[]
	INIT(= N_("E482: Can't create file %s"));
EXTERN حرف e_cant_get_موقت_file_name[]
	INIT(= N_("E483: Can't get temp file name"));
EXTERN حرف e_cant_open_file_str[]
	INIT(= N_("E484: Can't open file %s"));
EXTERN حرف e_cant_read_file_str[]
	INIT(= N_("E485: Can't read file %s"));
EXTERN حرف e_الگو_not_یافت[]
	INIT(= N_("E486: Pattern not found"));
EXTERN حرف e_الگو_not_یافت_str[]
	INIT(= N_("E486: Pattern not found: %s"));
EXTERN حرف e_آرگومان_must_be_مثبت[]
	INIT(= N_("E487: Argument must be positive"));
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_آرگومان_must_be_مثبت_str[]
	INIT(= N_("E487: Argument must be positive: %s"));
#پایان_اگر
EXTERN حرف e_trailing_characters[]
	INIT(= N_("E488: Trailing characters"));
EXTERN حرف e_trailing_characters_str[]
	INIT(= N_("E488: Trailing characters: %s"));
EXTERN حرف e_no_call_پشته_مقصد_substitute_for_پشته[]
	INIT(= N_("E489: No call stack to substitute for \"<stack>\""));
#اگر_تعریف_شده FEAT_FOLDING
EXTERN حرف e_no_fold_یافت[]
	INIT(= N_("E490: No fold found"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_json_decode_error_at_str[]
	INIT(= N_("E491: JSON decode error at '%s'"));
#پایان_اگر
EXTERN حرف e_not_an_editor_command[]
	INIT(= N_("E492: Not an editor command"));
EXTERN حرف e_backwards_range_given[]
	INIT(= N_("E493: Backwards range given"));
EXTERN حرف e_use_w_or_w_gt_gt[]
	INIT(= N_("E494: Use w or w>>"));
EXTERN حرف e_no_autocommand_file_name_مقصد_substitute_for_afile[]
	INIT(= N_("E495: No autocommand file name to substitute for \"<afile>\""));
EXTERN حرف e_no_autocommand_میانگیر_عدد_مقصد_substitute_for_abuf[]
	INIT(= N_("E496: No autocommand buffer number to substitute for \"<abuf>\""));
EXTERN حرف e_no_autocommand_تطابق_name_مقصد_substitute_for_amatch[]
	INIT(= N_("E497: No autocommand match name to substitute for \"<amatch>\""));
EXTERN حرف e_no_منبع_file_name_مقصد_substitute_for_sfile[]
	INIT(= N_("E498: No :source file name to substitute for \"<sfile>\""));
EXTERN حرف e_خالی_file_name_for_percent_or_hash_only_کار_میکند_with_ph[]
	// xgettext:no-c-format
	INIT(= N_("E499: Empty file name for '%' or '#', only works with \":p:h\""));
EXTERN حرف e_evaluates_مقصد_an_خالی_string[]
	INIT(= N_("E500: Evaluates to an empty string"));
EXTERN حرف e_at_انتها_از_file[]
	INIT(= N_("E501: At end-of-file"));
	// E502
EXTERN حرف e_is_a_directory[]
	INIT(= N_("is a directory"));
	// E503
EXTERN حرف e_is_not_file_or_writable_device[]
	INIT(= N_("is not a file or writable device"));
EXTERN حرف e_str_is_not_file_or_writable_device[]
	INIT(= N_("E503: \"%s\" is not a file or writable device"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_coffee_currently_not_موجود[]
	INIT(= N_("E503: Coffee is currently not available"));
#پایان_اگر
	// E504
EXTERN حرف e_is_read_only_cannot_override_W_درون_cpoptions[]
	INIT(= N_("is read-only (cannot override: \"W\" in 'cpoptions')"));
	// E505
EXTERN حرف e_is_read_only_جمع_کن_bang_مقصد_override[]
	INIT(= N_("is read-only (add ! to override)"));
EXTERN حرف e_str_is_read_only_جمع_کن_bang_مقصد_override[]
	INIT(= N_("E505: \"%s\" is read-only (add ! to override)"));
EXTERN حرف e_cant_write_مقصد_backup_file_جمع_کن_bang_مقصد_override[]
	INIT(= N_("E506: Can't write to backup file (add ! to override)"));
EXTERN حرف e_close_error_for_backup_file_جمع_کن_bang_مقصد_write_anyway[]
	INIT(= N_("E507: Close error for backup file (add ! to write anyway)"));
EXTERN حرف e_cant_read_file_for_backup_جمع_کن_bang_مقصد_write_anyway[]
	INIT(= N_("E508: Can't read file for backup (add ! to write anyway)"));
EXTERN حرف e_cannot_create_backup_file_جمع_کن_bang_مقصد_write_anyway[]
	INIT(= N_("E509: Cannot create backup file (add ! to override)"));
EXTERN حرف e_cant_make_backup_file_جمع_کن_bang_مقصد_write_anyway[]
	INIT(= N_("E510: Can't make backup file (add ! to write anyway)"));
#اگر_تعریف_شده FEAT_NETBEANS_INTG
EXTERN حرف e_netbeans_already_connected[]
	INIT(= N_("E511: NetBeans already connected"));
#پایان_اگر
EXTERN حرف e_close_failed[]
	INIT(= N_("E512: Close failed"));
EXTERN حرف e_write_error_conversion_failed_make_fenc_خالی_مقصد_override[]
	INIT(= N_("E513: Write error, conversion failed (make 'fenc' empty to override)"));
EXTERN حرف e_write_error_conversion_failed_درون_سطر_nr_make_fenc_خالی_مقصد_override[]
	INIT(= N_("E513: Write error, conversion failed in line %ld (make 'fenc' empty to override)"));
EXTERN حرف e_write_error_file_system_پر[]
	INIT(= N_("E514: Write error (file system full?)"));
EXTERN حرف e_no_buffers_were_unloaded[]
	INIT(= N_("E515: No buffers were unloaded"));
EXTERN حرف e_no_buffers_were_deleted[]
	INIT(= N_("E516: No buffers were deleted"));
EXTERN حرف e_no_buffers_were_wiped_بیرون[]
	INIT(= N_("E517: No buffers were wiped out"));
EXTERN حرف e_unknown_option[]
	INIT(= N_("E518: Unknown option"));
EXTERN حرف e_option_not_supported[]
	INIT(= N_("E519: Option not supported"));
EXTERN حرف e_not_allowed_درون_modeline[]
	INIT(= N_("E520: Not allowed in a modeline"));
EXTERN حرف e_عدد_required_after_equal[]
	INIT(= N_("E521: Number required after ="));
EXTERN حرف e_عدد_required_after_str_equal_str[]
	INIT(= N_("E521: Number required: &%s = '%s'"));
EXTERN حرف e_not_یافت_درون_termcap[]
	INIT(= N_("E522: Not found in termcap"));
EXTERN حرف e_not_allowed_اینجا[]
	INIT(= N_("E523: Not allowed here"));
EXTERN حرف e_missing_colon[]
	INIT(= N_("E524: Missing colon"));
EXTERN حرف e_zero_درازی_string[]
	INIT(= N_("E525: Zero length string"));
#اگر_تعریف_شده FEAT_VIMINFO
EXTERN حرف e_missing_عدد_after_angle_str_angle[]
	INIT(= N_("E526: Missing number after <%s>"));
EXTERN حرف e_missing_comma[]
	INIT(= N_("E527: Missing comma"));
EXTERN حرف e_must_specify_a_مقدار[]
	INIT(= N_("E528: Must specify a ' value"));
#پایان_اگر
EXTERN حرف e_cannot_مجموعه_جمله_مقصد_خالی_string[]
	INIT(= N_("E529: Cannot set 'term' to empty string"));
#اگر_تعریف_شده FEAT_GUI
EXTERN حرف e_cannot_change_جمله_درون_GUI[]
	INIT(= N_("E530: Cannot change 'term' in the GUI"));
EXTERN حرف e_use_gui_مقصد_شروع_GUI[]
	INIT(= N_("E531: Use \":gui\" to start the GUI"));
#پایان_اگر
#اگر_تعریف_شده FEAT_NETBEANS_INTG
EXTERN حرف e_highlighting_color_name_too_long_درون_define_Anno_Type[]
	INIT(= N_("E532: Highlighting color name too long in defineAnnoType"));
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI
EXTERN حرف e_cant_select_wide_font[]
	INIT(= N_("E533: Can't select wide font"));
EXTERN حرف e_invalid_wide_font[]
	INIT(= N_("E534: Invalid wide font"));
#پایان_اگر
EXTERN حرف e_illegal_character_after_chr[]
	INIT(= N_("E535: Illegal character after <%c>"));
#اگر_تعریف_شده FEAT_FOLDING
EXTERN حرف e_comma_required[]
	INIT(= N_("E536: Comma required"));
#پایان_اگر
EXTERN حرف e_commentstring_must_be_خالی_or_contain_str[]
	INIT(= N_("E537: 'commentstring' must be empty or contain %s"));
EXTERN حرف e_الگو_یافت_درون_every_سطر_str[]
	INIT(= N_("E538: Pattern found in every line: %s"));
EXTERN حرف e_illegal_character_str[]
	INIT(= N_("E539: Illegal character <%s>"));
#اگر_تعریف_شده FEAT_STL_OPT
EXTERN حرف e_unclosed_عبارت_sequence[]
	INIT(= N_("E540: Unclosed expression sequence"));
// E541 unused
EXTERN حرف e_unbalanced_groups[]
	INIT(= N_("E542: Unbalanced groups"));
#پایان_اگر
#اگر_تعریف_شده MSWIN
EXTERN حرف e_not_valid_codepage[]
	INIT(= N_("E543: Not a valid codepage"));
#پایان_اگر
#اگر_تعریف_شده FEAT_KEYMAP
EXTERN حرف e_keymap_file_not_یافت[]
	INIT(= N_("E544: Keymap file not found"));
#پایان_اگر
#اگر_تعریف_شده CURSOR_SHAPE
EXTERN حرف e_missing_colon_2[]
	INIT(= N_("E545: Missing colon"));
EXTERN حرف e_illegal_نما[]
	INIT(= N_("E546: Illegal mode"));
#پایان_اگر
#اگر_تعریف_شده FEAT_MOUSESHAPE
EXTERN حرف e_illegal_mouseshape[]
	INIT(= N_("E547: Illegal mouseshape"));
#پایان_اگر
#اگر_تعریف_شده CURSOR_SHAPE
EXTERN حرف e_digit_expected[]
	INIT(= N_("E548: Digit expected"));
EXTERN حرف e_illegal_percentage[]
	INIT(= N_("E549: Illegal percentage"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PRINTER
EXTERN حرف e_missing_colon_3[]
	INIT(= N_("E550: Missing colon"));
EXTERN حرف e_illegal_component[]
	INIT(= N_("E551: Illegal component"));
EXTERN حرف e_digit_expected_2[]
	INIT(= N_("E552: Digit expected"));
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_no_بیشتر_اقلام[]
	INIT(= N_("E553: No more items"));
#پایان_اگر
EXTERN حرف e_syntax_error_درون_str_curlies[]
	INIT(= N_("E554: Syntax error in %s{...}"));
EXTERN حرف e_at_bottom_از_برچسب_پشته[]
	INIT(= N_("E555: At bottom of tag stack"));
EXTERN حرف e_at_بالاترین_از_برچسب_پشته[]
	INIT(= N_("E556: At top of tag stack"));
EXTERN حرف e_cannot_open_termcap_file[]
	INIT(= N_("E557: Cannot open termcap file"));
EXTERN حرف e_terminal_مدخل_not_یافت_درون_terminfo[]
	INIT(= N_("E558: Terminal entry not found in terminfo"));
#إذا defined(HAVE_TGETENT) && !defined(TERMINFO)
EXTERN حرف e_terminal_مدخل_not_یافت_درون_termcap[]
	INIT(= N_("E559: Terminal entry not found in termcap"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CSCOPE
EXTERN حرف e_usage_cscope_str[]
	INIT(= N_("E560: Usage: cs[cope] %s"));
EXTERN حرف e_unknown_cscope_جستجو_type[]
	INIT(= N_("E561: Unknown cscope search type"));
EXTERN حرف e_usage_cstag_ident[]
	INIT(= N_("E562: Usage: cstag <ident>"));
EXTERN حرف e_stat_str_error_nr[]
	INIT(= N_("E563: stat(%s) error: %d"));
EXTERN حرف e_str_is_not_directory_or_valid_cscope_database[]
	INIT(= N_("E564: %s is not a directory or a valid cscope database"));
#پایان_اگر
EXTERN حرف e_not_allowed_مقصد_change_متن_or_change_پنجره[]
	INIT(= N_("E565: Not allowed to change text or change window"));
#اگر_تعریف_شده FEAT_CSCOPE
EXTERN حرف e_could_not_create_cscope_pipes[]
	INIT(= N_("E566: Could not create cscope pipes"));
EXTERN حرف e_no_cscope_connections[]
	INIT(= N_("E567: No cscope connections"));
EXTERN حرف e_duplicate_cscope_database_not_added[]
	INIT(= N_("E568: Duplicate cscope database not added"));
// E569 unused
EXTERN حرف e_fatal_error_درون_cs_manage_matches[]
	INIT(= "E570: Fatal error in cs_manage_matches");
#پایان_اگر
#اگر_تعریف_شده DYNAMIC_TCL
EXTERN حرف e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded[]
	INIT(= N_("E571: Sorry, this command is disabled: the Tcl library could not be loaded."));
#پایان_اگر
#اگر_تعریف_شده FEAT_TCL
EXTERN حرف e_exit_کد_nr[]
	INIT(= N_("E572: Exit code %d"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CLIENTSERVER
EXTERN حرف e_invalid_server_id_استفاده_شده_str[]
	INIT(= N_("E573: Invalid server id used: %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_VIMINFO
EXTERN حرف e_unknown_ثبات_type_nr[]
	INIT(= N_("E574: Unknown register type %d"));
	// E575
EXTERN حرف e_illegal_starting_char[]
	INIT(= N_("Illegal starting char"));
	// E576
EXTERN حرف e_nonr_missing_gt[]
	INIT(= N_("Missing '>'"));
	// E577
EXTERN حرف e_illegal_ثبات_name[]
	INIT(= N_("Illegal register name"));
#پایان_اگر
// E578 unused
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_if_nesting_too_deep[]
	INIT(= N_("E579: :if nesting too deep"));
EXTERN حرف e_بلوک_nesting_too_deep[]
	INIT(= N_("E579: Block nesting too deep"));
EXTERN حرف e_endif_without_if[]
	INIT(= N_("E580: :endif without :if"));
EXTERN حرف e_else_without_if[]
	INIT(= N_("E581: :else without :if"));
EXTERN حرف e_elseif_without_if[]
	INIT(= N_("E582: :elseif without :if"));
EXTERN حرف e_مضرب_else[]
	INIT(= N_("E583: Multiple :else"));
EXTERN حرف e_elseif_after_else[]
	INIT(= N_("E584: :elseif after :else"));
EXTERN حرف e_while_for_nesting_too_deep[]
	INIT(= N_("E585: :while/:for nesting too deep"));
EXTERN حرف e_continue_without_while_or_for[]
	INIT(= N_("E586: :continue without :while or :for"));
EXTERN حرف e_break_without_while_or_for[]
	INIT(= N_("E587: :break without :while or :for"));
EXTERN حرف e_endwhile_without_while[]
	INIT(= N_("E588: :endwhile without :while"));
EXTERN حرف e_endfor_without_for[]
	INIT(= N_("E588: :endfor without :for"));
#پایان_اگر
EXTERN حرف e_backupext_and_patchmode_are_equal[]
	INIT(= N_("E589: 'backupext' and 'patchmode' are equal"));
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_preview_پنجره_already_exists[]
	INIT(= N_("E590: A preview window already exists"));
#پایان_اگر
EXTERN حرف e_winheight_cannot_be_smaller_than_winminheight[]
	INIT(= N_("E591: 'winheight' cannot be smaller than 'winminheight'"));
EXTERN حرف e_winwidth_cannot_be_smaller_than_winminwidth[]
	INIT(= N_("E592: 'winwidth' cannot be smaller than 'winminwidth'"));
EXTERN حرف e_need_at_کمترین_nr_سطرها[]
	INIT(= N_("E593: Need at least %d lines"));
EXTERN حرف e_need_at_کمترین_nr_columns[]
	INIT(= N_("E594: Need at least %d columns"));
#اگر_تعریف_شده FEAT_LINEBREAK
EXTERN حرف e_showbreak_شامل_است_unprintable_or_wide_character[]
	INIT(= N_("E595: 'showbreak' contains unprintable or wide character"));
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI
EXTERN حرف e_invalid_fonts[]
	INIT(= N_("E596: Invalid font(s)"));
# ifdef FEAT_XFONTSET
EXTERN حرف e_cant_select_fontset[]
	INIT(= N_("E597: Can't select fontset"));
EXTERN حرف e_invalid_fontset[]
	INIT(= N_("E598: Invalid fontset"));
# endif
#پایان_اگر
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
EXTERN حرف e_مقدار_از_imactivatekey_is_invalid[]
	INIT(= N_("E599: Value of 'imactivatekey' is invalid"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_missing_endtry[]
	INIT(= N_("E600: Missing :endtry"));
EXTERN حرف e_try_nesting_too_deep[]
	INIT(= N_("E601: :try nesting too deep"));
EXTERN حرف e_endtry_without_try[]
	INIT(= N_("E602: :endtry without :try"));
EXTERN حرف e_catch_without_try[]
	INIT(= N_("E603: :catch without :try"));
EXTERN حرف e_catch_after_finally[]
	INIT(= N_("E604: :catch after :finally"));
EXTERN حرف e_exception_not_caught_str[]
	INIT(= N_("E605: Exception not caught: %s"));
EXTERN حرف e_finally_without_try[]
	INIT(= N_("E606: :finally without :try"));
EXTERN حرف e_مضرب_finally[]
	INIT(= N_("E607: Multiple :finally"));
EXTERN حرف e_cannot_throw_exceptions_with_vim_پیشوند[]
	INIT(= N_("E608: Cannot :throw exceptions with 'Vim' prefix"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CSCOPE
EXTERN حرف e_cscope_error_str[]
	INIT(= N_("E609: Cscope error: %s"));
#پایان_اگر
EXTERN حرف e_no_آرگومان_مقصد_delete[]
	INIT(= N_("E610: No argument to delete"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_using_special_as_عدد[]
	INIT(= N_("E611: Using a Special as a Number"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
EXTERN حرف e_too_many_signs_defined[]
	INIT(= N_("E612: Too many signs defined"));
#پایان_اگر
#إذا defined(FEAT_PRINTER) && (defined(MSWIN) || defined(FEAT_PRINT_PANGO))
EXTERN حرف e_unknown_printer_font_str[]
	INIT(= N_("E613: Unknown printer font: %s"));
#پایان_اگر
// E614 unused (deleted)
// E615 unused
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_object_required_for_آرگومان_nr[]
	INIT(= N_("E616: Object required for argument %d"));
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_GTK
EXTERN حرف e_cannot_be_تغییریافته_درون_gtk_GUI[]
	INIT(= N_("E617: Cannot be changed in the GTK GUI"));
#پایان_اگر
#اگر_تعریف_شده FEAT_POSTSCRIPT
EXTERN حرف e_file_str_is_not_postscript_resource_file[]
	INIT(= N_("E618: File \"%s\" is not a PostScript resource file"));
EXTERN حرف e_file_str_is_not_supported_postscript_resource_file[]
	INIT(= N_("E619: File \"%s\" is not a supported PostScript resource file"));
EXTERN حرف e_unable_مقصد_convert_مقصد_print_encoding_str[]
	INIT(= N_("E620: Unable to convert to print encoding \"%s\""));
EXTERN حرف e_str_resource_file_has_wrong_نسخه[]
	INIT(= N_("E621: \"%s\" resource file has wrong version"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CSCOPE
EXTERN حرف e_could_not_fork_for_cscope[]
	INIT(= N_("E622: Could not fork for cscope"));
# ifndef UNIX
EXTERN حرف e_could_not_spawn_cscope_process[]
	INIT(= N_("E623: Could not spawn cscope process"));
# endif
#پایان_اگر
#إذا defined(FEAT_PRINTER) && defined(FEAT_POSTSCRIPT)
EXTERN حرف e_cant_open_file_str_3[]
	INIT(= N_("E624: Can't open file \"%s\""));
#پایان_اگر
#إذا defined(FEAT_CSCOPE) && !defined(UNIX)
EXTERN حرف e_cannot_open_cscope_database_str[]
	INIT(= N_("E625: Cannot open cscope database: %s"));
EXTERN حرف e_cannot_get_cscope_database_information[]
	INIT(= N_("E626: Cannot get cscope database information"));
#پایان_اگر
#اگر_تعریف_شده FEAT_NETBEANS_INTG
EXTERN حرف e_missing_colon_str[]
	INIT(= N_("E627: Missing colon: %s"));
EXTERN حرف e_missing_bang_or_slash_درون_str[]
	INIT(= N_("E628: Missing ! or / in: %s"));
# ifdef NBDEBUG
EXTERN حرف e_bad_return_from_nb_do_cmd[]
	INIT(= "E629: Bad return from nb_do_cmd");
# endif
#پایان_اگر
#اگر_تعریف_شده FEAT_کار_CHANNEL
EXTERN حرف e_str_write_while_not_connected[]
	INIT(= N_("E630: %s(): Write while not connected"));
EXTERN حرف e_str_write_failed[]
	INIT(= N_("E631: %s(): Write failed"));
#پایان_اگر
#اگر_تعریف_شده FEAT_NETBEANS_INTG
EXTERN حرف e_invalid_میانگیر_identifier_درون_getlength[]
	INIT(= N_("E632: Invalid buffer identifier in getLength"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_gettext[]
	INIT(= N_("E633: Invalid buffer identifier in getText"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_remove[]
	INIT(= N_("E634: Invalid buffer identifier in remove"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_درج[]
	INIT(= N_("E635: Invalid buffer identifier in insert"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_create[]
	INIT(= N_("E636: Invalid buffer identifier in create"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_startdocumentlisten[]
	INIT(= N_("E637: Invalid buffer identifier in startDocumentListen"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_stopdocumentlisten[]
	INIT(= N_("E638: Invalid buffer identifier in stopDocumentListen"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_settitle[]
	INIT(= N_("E639: Invalid buffer identifier in setTitle"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_initdone[]
	INIT(= N_("E640: Invalid buffer identifier in initDone"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_setbuffernumber[]
	INIT(= N_("E641: Invalid buffer identifier in setBufferNumber"));
EXTERN حرف e_file_str_not_یافت_درون_setbuffernumber[]
	INIT(= N_("E642: File %s not found in setBufferNumber"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_setfullname[]
	INIT(= N_("E643: Invalid buffer identifier in setFullName"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_editfile[]
	INIT(= N_("E644: Invalid buffer identifier in editFile"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_setvisible[]
	INIT(= N_("E645: Invalid buffer identifier in setVisible"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_setmodified[]
	INIT(= N_("E646: Invalid buffer identifier in setModified"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_setdot[]
	INIT(= N_("E647: Invalid buffer identifier in setDot"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_close[]
	INIT(= N_("E648: Invalid buffer identifier in close"));
EXTERN حرف e_invalid_identifier_درون_defineannotype[]
	INIT(= N_("E649: Invalid identifier name in defineAnnoType"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_defineannotype[]
	INIT(= N_("E650: Invalid buffer identifier in defineAnnoType"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_addanno[]
	INIT(= N_("E651: Invalid buffer identifier in addAnno"));
EXTERN حرف e_invalid_میانگیر_identifier_درون_getanno[]
	INIT(= N_("E652: Invalid buffer identifier in getAnno"));
#پایان_اگر
// E653 unused
EXTERN حرف e_missing_delimiter_after_جستجو_الگو_str[]
	INIT(= N_("E654: Missing delimiter after search pattern: %s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_too_many_symbolic_links_چرخه[]
	INIT(= N_("E655: Too many symbolic links (cycle?)"));
#پایان_اگر
#اگر_تعریف_شده FEAT_NETBEANS_INTG
	// E656
EXTERN حرف e_netbeans_disallows_writes_از_unmodified_buffers[]
	INIT(= N_("NetBeans disallows writes of unmodified buffers"));
	// E657
EXTERN حرف e_partial_writes_disallowed_for_netbeans_buffers[]
	INIT(= N_("Partial writes disallowed for NetBeans buffers"));
EXTERN حرف e_netbeans_connection_گمشده_for_میانگیر_nr[]
	INIT(= N_("E658: NetBeans connection lost for buffer %d"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PYTHON
EXTERN حرف e_cannot_invoke_python_recursively[]
	INIT(= N_("E659: Cannot invoke Python recursively"));
#پایان_اگر
#اگر_تعریف_شده FEAT_NETBEANS_INTG
EXTERN حرف e_cannot_open_netbeans_connection_info_file[]
	INIT(= "E660: Cannot open NetBeans connection info file");
#پایان_اگر
#اگر_تعریف_شده FEAT_MULTI_LANG
EXTERN حرف e_sorry_no_str_help_for_str[]
	INIT(= N_("E661: Sorry, no '%s' help for %s"));
#پایان_اگر
EXTERN حرف e_at_شروع_از_changelist[]
	INIT(= N_("E662: At start of changelist"));
EXTERN حرف e_at_انتها_از_changelist[]
	INIT(= N_("E663: At end of changelist"));
EXTERN حرف e_changelist_is_خالی[]
	INIT(= N_("E664: Changelist is empty"));
#اگر_تعریف_شده FEAT_GUI
EXTERN حرف e_cannot_شروع_gui_no_valid_font_یافت[]
	INIT(= N_("E665: Cannot start GUI, no valid font found"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_compiler_not_supported_str[]
	INIT(= N_("E666: Compiler not supported: %s"));
#پایان_اگر
#اگر_تعریف_شده HAVE_FSYNC
EXTERN حرف e_fsync_failed[]
	INIT(= N_("E667: Fsync failed"));
#پایان_اگر
#اگر_تعریف_شده FEAT_NETBEANS_INTG
EXTERN حرف e_wrong_access_نما_for_netbeans_connection_info_file_str[]
	INIT(= N_("E668: Wrong access mode for NetBeans connection info file: \"%s\""));
#پایان_اگر
EXTERN حرف e_unprintable_character_درون_گروه_name[]
	INIT(= N_("E669: Unprintable character in group name"));
EXTERN حرف e_mix_از_help_file_encodings_within_language_str[]
	INIT(= N_("E670: Mix of help file encodings within a language: %s"));
#اگر_تعریف_شده FEAT_GUI_MSWIN
EXTERN حرف e_cannot_پیدا_کن_پنجره_title_str[]
	INIT(= N_("E671: Cannot find window title \"%s\""));
EXTERN حرف e_unable_مقصد_open_پنجره_inside_mdi_application[]
	INIT(= N_("E672: Unable to open window inside MDI application"));
#پایان_اگر
#إذا defined(FEAT_PRINTER) && defined(FEAT_POSTSCRIPT)
EXTERN حرف e_incompatible_multi_byte_encoding_and_character_مجموعه[]
	INIT(= N_("E673: Incompatible multi-byte encoding and character set"));
EXTERN حرف e_printmbcharset_cannot_be_خالی_with_multi_byte_encoding[]
	INIT(= N_("E674: printmbcharset cannot be empty with multi-byte encoding."));
EXTERN حرف e_no_default_font_specified_for_multi_byte_printing[]
	INIT(= N_("E675: No default font specified for multi-byte printing."));
#پایان_اگر
EXTERN حرف e_no_matching_autocommands_for_buftype_str_میانگیر[]
	INIT(= N_("E676: No matching autocommands for buftype=%s buffer"));
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN حرف e_error_writing_موقت_file[]
	INIT(= N_("E677: Error writing temp file"));
#پایان_اگر
EXTERN حرف e_invalid_character_after_str_2[]
	INIT(= N_("E678: Invalid character after %s%%[dxouU]"));
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN حرف e_recursive_loop_loading_syncolor_vim[]
	INIT(= N_("E679: Recursive loop loading syncolor.vim"));
#پایان_اگر
EXTERN حرف e_میانگیر_nr_invalid_میانگیر_عدد[]
	INIT(= N_("E680: <buffer=%d>: invalid buffer number"));
#إذا defined(FEAT_QUICKFIX) || defined(FEAT_EVAL)
EXTERN حرف e_میانگیر_is_not_loaded[]
	INIT(= N_("E681: Buffer is not loaded"));
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_invalid_جستجو_الگو_or_delimiter[]
	INIT(= N_("E682: Invalid search pattern or delimiter"));
EXTERN حرف e_file_name_missing_or_invalid_الگو[]
	INIT(= N_("E683: File name missing or invalid pattern"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_list_شاخص_بیرون_از_range_nr[]
	INIT(= N_("E684: List index out of range: %ld"));
#پایان_اگر
EXTERN حرف e_داخلی_error_str[]
	INIT(= N_("E685: Internal error: %s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_آرگومان_از_str_must_be_list[]
	INIT(= N_("E686: Argument of %s must be a List"));
EXTERN حرف e_less_targets_than_list_اقلام[]
	INIT(= N_("E687: Less targets than List items"));
EXTERN حرف e_بیشتر_targets_than_list_اقلام[]
	INIT(= N_("E688: More targets than List items"));
EXTERN حرف e_شاخص_not_allowed_after_str_str[]
	INIT(= N_("E689: Index not allowed after a %s: %s"));
EXTERN حرف e_missing_درون_after_for[]
	INIT(= N_("E690: Missing \"in\" after :for"));
EXTERN حرف e_can_only_compare_list_with_list[]
	INIT(= N_("E691: Can only compare List with List"));
EXTERN حرف e_invalid_operation_for_list[]
	INIT(= N_("E692: Invalid operation for List"));
EXTERN حرف e_class_or_typealias_required_for_آرگومان_nr[]
	INIT(= N_("E693: Class or class typealias required for argument %d"));
EXTERN حرف e_invalid_operation_for_funcrefs[]
	INIT(= N_("E694: Invalid operation for Funcrefs"));
EXTERN حرف e_cannot_شاخص_a_funcref[]
	INIT(= N_("E695: Cannot index a Funcref"));
EXTERN حرف e_missing_comma_درون_list_str[]
	INIT(= N_("E696: Missing comma in List: %s"));
EXTERN حرف e_missing_انتها_از_list_rsb_str[]
	INIT(= N_("E697: Missing end of List ']': %s"));
EXTERN حرف e_variable_nested_too_deep_for_making_copy[]
	INIT(= N_("E698: Variable nested too deep for making a copy"));
EXTERN حرف e_too_many_arguments[]
	INIT(= N_("E699: Too many arguments"));
EXTERN حرف e_unknown_function_str_2[]
	INIT(= N_("E700: Unknown function: %s"));
EXTERN حرف e_invalid_type_for_len[]
	INIT(= N_("E701: Invalid type for len()"));
EXTERN حرف e_مرتب_سازی_compare_function_failed[]
	INIT(= N_("E702: Sort compare function failed"));
EXTERN حرف e_using_funcref_as_عدد[]
	INIT(= N_("E703: Using a Funcref as a Number"));
EXTERN حرف e_funcref_variable_name_must_شروع_with_capital_str[]
	INIT(= N_("E704: Funcref variable name must start with a capital: %s"));
EXTERN حرف e_variable_name_conflicts_with_existing_function_str[]
	INIT(= N_("E705: Variable name conflicts with existing function: %s"));
EXTERN حرف e_آرگومان_از_str_must_be_list_string_or_dictionary[]
	INIT(= N_("E706: Argument of %s must be a List, String or Dictionary"));
EXTERN حرف e_function_name_conflicts_with_variable_str[]
	INIT(= N_("E707: Function name conflicts with variable: %s"));
EXTERN حرف e_برش_must_come_آخرین[]
	INIT(= N_("E708: [:] must come last"));
EXTERN حرف e_برش_requires_list_or_blob_مقدار[]
	INIT(= N_("E709: [:] requires a List or Blob value"));
EXTERN حرف e_list_مقدار_has_بیشتر_اقلام_than_targets[]
	INIT(= N_("E710: List value has more items than targets"));
EXTERN حرف e_list_مقدار_does_not_have_enough_اقلام[]
	INIT(= N_("E711: List value does not have enough items"));
EXTERN حرف e_آرگومان_از_str_must_be_list_or_dictionary[]
	INIT(= N_("E712: Argument of %s must be a List or Dictionary"));
EXTERN حرف e_cannot_use_خالی_کلید_for_dictionary[]
	INIT(= N_("E713: Cannot use empty key for Dictionary"));
EXTERN حرف e_list_required[]
	INIT(= N_("E714: List required"));
EXTERN حرف e_dictionary_required[]
	INIT(= N_("E715: Dictionary required"));
EXTERN حرف e_کلید_not_present_درون_dictionary_str[]
	INIT(= N_("E716: Key not present in Dictionary: \"%s\""));
EXTERN حرف e_dictionary_مدخل_already_exists[]
	INIT(= N_("E717: Dictionary entry already exists"));
EXTERN حرف e_funcref_required[]
	INIT(= N_("E718: Funcref required"));
EXTERN حرف e_cannot_برش_dictionary[]
	INIT(= N_("E719: Cannot slice a Dictionary"));
EXTERN حرف e_missing_colon_درون_dictionary_str[]
	INIT(= N_("E720: Missing colon in Dictionary: %s"));
EXTERN حرف e_duplicate_کلید_درون_dictionary_str[]
	INIT(= N_("E721: Duplicate key in Dictionary: \"%s\""));
EXTERN حرف e_missing_comma_درون_dictionary_str[]
	INIT(= N_("E722: Missing comma in Dictionary: %s"));
EXTERN حرف e_missing_dict_انتها_str[]
	INIT(= N_("E723: Missing end of Dictionary '}': %s"));
EXTERN حرف e_variable_nested_too_deep_for_displaying[]
	INIT(= N_("E724: Variable nested too deep for displaying"));
EXTERN حرف e_calling_dict_function_without_dictionary_str[]
	INIT(= N_("E725: Calling dict function without Dictionary: %s"));
EXTERN حرف e_stride_is_zero[]
	INIT(= N_("E726: Stride is zero"));
EXTERN حرف e_شروع_past_انتها[]
	INIT(= N_("E727: Start past end"));
EXTERN حرف e_using_dictionary_as_عدد[]
	INIT(= N_("E728: Using a Dictionary as a Number"));
EXTERN حرف e_using_funcref_as_string[]
	INIT(= N_("E729: Using a Funcref as a String"));
EXTERN حرف e_using_list_as_string[]
	INIT(= N_("E730: Using a List as a String"));
EXTERN حرف e_using_dictionary_as_string[]
	INIT(= N_("E731: Using a Dictionary as a String"));
EXTERN حرف e_using_endfor_with_while[]
	INIT(= N_("E732: Using :endfor with :while"));
EXTERN حرف e_using_endwhile_with_for[]
	INIT(= N_("E733: Using :endwhile with :for"));
EXTERN حرف e_wrong_variable_type_for_str_equal[]
	INIT(= N_("E734: Wrong variable type for %s="));
EXTERN حرف e_can_only_compare_dictionary_with_dictionary[]
	INIT(= N_("E735: Can only compare Dictionary with Dictionary"));
EXTERN حرف e_invalid_operation_for_dictionary[]
	INIT(= N_("E736: Invalid operation for Dictionary"));
EXTERN حرف e_کلید_already_exists_str[]
	INIT(= N_("E737: Key already exists: %s"));
EXTERN حرف e_cant_list_variables_for_str[]
	INIT(= N_("E738: Can't list variables for %s"));
EXTERN حرف e_cannot_create_directory_str[]
	INIT(= N_("E739: Cannot create directory: %s"));
EXTERN حرف e_too_many_arguments_for_function_str_2[]
	INIT(= N_("E740: Too many arguments for function %s"));
EXTERN حرف e_مقدار_is_locked[]
	INIT(= N_("E741: Value is locked"));
EXTERN حرف e_مقدار_is_locked_str[]
	INIT(= N_("E741: Value is locked: %s"));
EXTERN حرف e_cannot_change_مقدار[]
	INIT(= N_("E742: Cannot change value"));
EXTERN حرف e_cannot_change_مقدار_از_str[]
	INIT(= N_("E742: Cannot change value of %s"));
EXTERN حرف e_variable_nested_too_deep_for_unlock[]
	INIT(= N_("E743: Variable nested too deep for (un)lock"));
#پایان_اگر
#اگر_تعریف_شده FEAT_NETBEANS_INTG
EXTERN حرف e_netbeans_does_not_allow_changes_درون_read_only_files[]
	INIT(= N_("E744: NetBeans does not allow changes in read-only files"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_using_list_as_عدد[]
	INIT(= N_("E745: Using a List as a Number"));
EXTERN حرف e_function_name_does_not_تطابق_script_file_name_str[]
	INIT(= N_("E746: Function name does not match script file name: %s"));
#پایان_اگر
EXTERN حرف e_cannot_change_directory_میانگیر_is_modified_جمع_کن_bang_مقصد_override[]
	INIT(= N_("E747: Cannot change directory, buffer is modified (add ! to override)"));
EXTERN حرف e_no_previously_استفاده_شده_ثبات[]
	INIT(= N_("E748: No previously used register"));
EXTERN حرف e_خالی_میانگیر[]
	INIT(= N_("E749: Empty buffer"));
#اگر_تعریف_شده FEAT_PROFILE
EXTERN حرف e_اولی_use_profile_شروع_fname[]
	INIT(= N_("E750: First use \":profile start {fname}\""));
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
EXTERN حرف e_output_file_name_must_not_have_region_name[]
	INIT(= N_("E751: Output file name must not have region name"));
EXTERN حرف e_no_قبلی_spell_replacement[]
	INIT(= N_("E752: No previous spell replacement"));
EXTERN حرف e_not_یافت_str[]
	INIT(= N_("E753: Not found: %s"));
EXTERN حرف e_only_به_بالا_مقصد_nr_regions_supported[]
	INIT(= N_("E754: Only up to %d regions supported"));
EXTERN حرف e_invalid_region_درون_str[]
	INIT(= N_("E755: Invalid region in %s"));
EXTERN حرف e_spell_checking_is_not_possible[]
	INIT(= N_("E756: Spell checking is not possible"));
EXTERN حرف e_this_does_not_look_like_spell_file[]
	INIT(= N_("E757: This does not look like a spell file"));
EXTERN حرف e_truncated_spell_file[]
	INIT(= N_("E758: Truncated spell file"));
EXTERN حرف e_format_error_درون_spell_file[]
	INIT(= N_("E759: Format error in spell file"));
EXTERN حرف e_no_کلمه_شمار_درون_str[]
	INIT(= N_("E760: No word count in %s"));
EXTERN حرف e_format_error_درون_affix_file_fol_کم_or_upp[]
	INIT(= N_("E761: Format error in affix file FOL, LOW or UPP"));
EXTERN حرف e_character_درون_fol_کم_or_upp_is_بیرون_از_range[]
	INIT(= N_("E762: Character in FOL, LOW or UPP is out of range"));
EXTERN حرف e_کلمه_characters_تفاوت_between_spell_files[]
	INIT(= N_("E763: Word characters differ between spell files"));
#پایان_اگر
#إذا defined(FEAT_SYN_HL) || defined(FEAT_COMPL_FUNC) || defined(FEAT_SPELL)
EXTERN حرف e_option_str_is_not_مجموعه[]
	INIT(= N_("E764: Option '%s' is not set"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
EXTERN حرف e_spellfile_does_not_have_nr_entries[]
	INIT(= N_("E765: 'spellfile' does not have %d entries"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_insufficient_arguments_for_printf[]
	INIT(= N_("E766: Insufficient arguments for printf()"));
#پایان_اگر
EXTERN حرف e_too_many_arguments_مقصد_printf[]
	INIT(= N_("E767: Too many arguments for printf()"));
EXTERN حرف e_جابجا_file_exists_str_silent_overrides[]
	INIT(= N_("E768: Swap file exists: %s (:silent! overrides)"));
EXTERN حرف e_missing_rsb_after_str_lsb[]
	INIT(= N_("E769: Missing ] after %s["));
#اگر_تعریف_شده FEAT_SPELL
EXTERN حرف e_unsupported_section_درون_spell_file[]
	INIT(= N_("E770: Unsupported section in spell file"));
EXTERN حرف e_old_spell_file_needs_مقصد_be_updated[]
	INIT(= N_("E771: Old spell file, needs to be updated"));
EXTERN حرف e_spell_file_is_for_newer_نسخه_از_vim[]
	INIT(= N_("E772: Spell file is for newer version of Vim"));
#پایان_اگر
EXTERN حرف e_symlink_loop_for_str[]
	INIT(= N_("E773: Symlink loop for \"%s\""));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_operatorfunc_is_خالی[]
	INIT(= N_("E774: 'operatorfunc' is empty"));
#وإلا
EXTERN حرف e_eval_feature_not_موجود[]
	INIT(= N_("E775: Eval feature not available"));
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_no_location_list[]
	INIT(= N_("E776: No location list"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_string_or_list_expected[]
	INIT(= N_("E777: String or List expected"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
EXTERN حرف e_this_does_not_look_like_sug_file_str[]
	INIT(= N_("E778: This does not look like a .sug file: %s"));
EXTERN حرف e_old_sug_file_needs_مقصد_be_updated_str[]
	INIT(= N_("E779: Old .sug file, needs to be updated: %s"));
EXTERN حرف e_sug_file_is_for_newer_نسخه_از_vim_str[]
	INIT(= N_("E780: .sug file is for newer version of Vim: %s"));
EXTERN حرف e_sug_file_doesnt_تطابق_spl_file_str[]
	INIT(= N_("E781: .sug file doesn't match .spl file: %s"));
EXTERN حرف e_error_while_reading_sug_file_str[]
	INIT(= N_("E782: Error while reading .sug file: %s"));
EXTERN حرف e_duplicate_char_درون_map_مدخل[]
	INIT(= N_("E783: Duplicate char in MAP entry"));
#پایان_اگر
EXTERN حرف e_cannot_close_آخرین_tab_صفحه[]
	INIT(= N_("E784: Cannot close last tab page"));
#اگر_تعریف_شده FEAT_EVAL
# ifdef FEAT_COMPL_FUNC
EXTERN حرف e_complete_can_only_be_استفاده_شده_درون_درج_نما[]
	INIT(= N_("E785: complete() can only be used in Insert mode"));
# endif
EXTERN حرف e_range_not_allowed[]
	INIT(= N_("E786: Range not allowed"));
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
EXTERN حرف e_میانگیر_تغییریافته_unexpectedly[]
	INIT(= N_("E787: Buffer changed unexpectedly"));
#پایان_اگر
EXTERN حرف e_not_allowed_مقصد_edit_another_میانگیر_now[]
	INIT(= N_("E788: Not allowed to edit another buffer now"));
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN حرف e_error_missing_rsb_str[]
	INIT(= N_("E789: Missing ']': %s"));
#پایان_اگر
EXTERN حرف e_undojoin_is_not_allowed_after_undo[]
	INIT(= N_("E790: undojoin is not allowed after undo"));
#اگر_تعریف_شده FEAT_KEYMAP
EXTERN حرف e_خالی_keymap_مدخل[]
	INIT(= N_("E791: Empty keymap entry"));
#پایان_اگر
#اگر_تعریف_شده FEAT_MENU
EXTERN حرف e_خالی_menu_name[]
	INIT(= N_("E792: Empty menu name"));
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
EXTERN حرف e_no_other_میانگیر_درون_diff_نما_is_modifiable[]
	INIT(= N_("E793: No other buffer in diff mode is modifiable"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_مجموعه_variable_درون_sandbox[]
	INIT(= N_("E794: Cannot set variable in the sandbox"));
EXTERN حرف e_cannot_مجموعه_variable_درون_sandbox_str[]
	INIT(= N_("E794: Cannot set variable in the sandbox: \"%s\""));
EXTERN حرف e_cannot_delete_variable[]
	INIT(= N_("E795: Cannot delete variable"));
EXTERN حرف e_cannot_delete_variable_str[]
	INIT(= N_("E795: Cannot delete variable %s"));
#پایان_اگر
#اگر_تعریف_شده MSWIN
	// E796
EXTERN حرف e_writing_مقصد_device_disabled_with_opendevice_option[]
	INIT(= N_("writing to device disabled with 'opendevice' option"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
EXTERN حرف e_spellfilemising_autocommand_deleted_میانگیر[]
	INIT(= N_("E797: SpellFileMissing autocommand deleted buffer"));
#پایان_اگر
#اگر_تعریف_شده FEAT_جستجو_EXTRA
EXTERN حرف e_id_is_reserved_for_تطابق_nr[]
	INIT(= N_("E798: ID is reserved for \":match\": %d"));
EXTERN حرف e_invalid_id_nr_must_be_greater_than_or_equal_مقصد_one_1[]
	INIT(= N_("E799: Invalid ID: %d (must be greater than or equal to 1)"));
#پایان_اگر
#اگر_تعریف_نشده FEAT_ARABIC
EXTERN حرف e_arabic_cannot_be_استفاده_شده_not_enabled_at_compile_زمان[]
	INIT(= N_("E800: Arabic cannot be used: Not enabled at compile time\n"));
#پایان_اگر
#اگر_تعریف_شده FEAT_جستجو_EXTRA
EXTERN حرف e_id_already_گرفته_nr[]
	INIT(= N_("E801: ID already taken: %d"));
EXTERN حرف e_invalid_id_nr_must_be_greater_than_or_equal_مقصد_one_2[]
	INIT(= N_("E802: Invalid ID: %d (must be greater than or equal to 1)"));
EXTERN حرف e_id_not_یافت_nr[]
	INIT(= N_("E803: ID not found: %d"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_use_percent_with_float[]
	// xgettext:no-c-format
	INIT(= N_("E804: Cannot use '%' with Float"));
EXTERN حرف e_using_float_as_عدد[]
	INIT(= N_("E805: Using a Float as a Number"));
EXTERN حرف e_using_float_as_string[]
	INIT(= N_("E806: Using a Float as a String"));
EXTERN حرف e_expected_float_آرگومان_for_printf[]
	INIT(= N_("E807: Expected Float argument for printf()"));
EXTERN حرف e_عدد_or_float_required[]
	INIT(= N_("E808: Number or Float required"));
#پایان_اگر
#اگر_تعریف_نشده FEAT_EVAL
EXTERN حرف e_hashsmall_is_not_موجود_without_the_eval_feature[]
	INIT(= N_("E809: #< is not available without the +eval feature"));
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
EXTERN حرف e_cannot_read_or_write_موقت_files[]
	INIT(= N_("E810: Cannot read or write temp files"));
#پایان_اگر
EXTERN حرف e_not_allowed_مقصد_change_میانگیر_information_now[]
	INIT(= N_("E811: Not allowed to change buffer information now"));
EXTERN حرف e_autocommands_تغییریافته_میانگیر_or_میانگیر_name[]
	INIT(= N_("E812: Autocommands changed buffer or buffer name"));
EXTERN حرف e_cannot_close_autocmd_or_popup_پنجره[]
	INIT(= N_("E813: Cannot close autocmd or popup window"));
EXTERN حرف e_cannot_close_پنجره_only_autocmd_پنجره_would_remain[]
	INIT(= N_("E814: Cannot close window, only autocmd window would remain"));
#اگر_تعریف_شده FEAT_MZSCHEME
EXTERN حرف e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded[]
	INIT(= N_("E815: Sorry, this command is disabled, the MzScheme libraries could not be loaded."));
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
EXTERN حرف e_cannot_read_patch_output[]
	INIT(= N_("E816: Cannot read patch output"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CRYPT
EXTERN حرف e_blowfish_big_little_endian_use_wrong[]
	INIT(= N_("E817: Blowfish big/little endian use wrong"));
EXTERN حرف e_sha256_test_failed[]
	INIT(= N_("E818: sha256 test failed"));
EXTERN حرف e_blowfish_test_failed[]
	INIT(= N_("E819: Blowfish test failed"));
EXTERN حرف e_sizeof_uint32_isnot_four[]
	INIT(= N_("E820: sizeof(uint32_t) != 4"));
EXTERN حرف e_file_is_encrypted_with_unknown_روش[]
	INIT(= N_("E821: File is encrypted with unknown method"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PERSISTENT_UNDO
EXTERN حرف e_cannot_open_undo_file_for_reading_str[]
	INIT(= N_("E822: Cannot open undo file for reading: %s"));
EXTERN حرف e_not_an_undo_file_str[]
	INIT(= N_("E823: Not an undo file: %s"));
EXTERN حرف e_incompatible_undo_file_str[]
	INIT(= N_("E824: Incompatible undo file: %s"));
EXTERN حرف e_corrupted_undo_file_str_str[]
	INIT(= N_("E825: Corrupted undo file (%s): %s"));
# ifdef FEAT_CRYPT
EXTERN حرف e_undo_file_decryption_failed[]
	INIT(= N_("E826: Undo file decryption failed: %s"));
# وإلا
EXTERN حرف e_undo_file_is_encrypted_str[]
	INIT(= N_("E827: Undo file is encrypted: %s"));
# endif
EXTERN حرف e_cannot_open_undo_file_for_writing_str[]
	INIT(= N_("E828: Cannot open undo file for writing: %s"));
EXTERN حرف e_write_error_درون_undo_file_str[]
	INIT(= N_("E829: Write error in undo file: %s"));
#پایان_اگر
EXTERN حرف e_undo_عدد_nr_not_یافت[]
	INIT(= N_("E830: Undo number %ld not found"));
#اگر_تعریف_شده FEAT_CRYPT
EXTERN حرف e_bf_کلید_init_called_with_خالی_password[]
	INIT(= "E831: bf_key_init() called with empty password");
# ifdef FEAT_PERSISTENT_UNDO
EXTERN حرف e_non_encrypted_file_has_encrypted_undo_file_str[]
	INIT(= N_("E832: Non-encrypted file has encrypted undo file: %s"));
# endif
#وإلا
EXTERN حرف e_str_is_encrypted_and_this_نسخه_از_vim_does_not_support_encryption[]
	INIT(= N_("E833: %s is encrypted and this version of Vim does not support encryption"));
#پایان_اگر
EXTERN حرف e_conflicts_with_مقدار_از_listchars[]
	INIT(= N_("E834: Conflicts with value of 'listchars'"));
EXTERN حرف e_conflicts_with_مقدار_از_fillchars[]
	INIT(= N_("E835: Conflicts with value of 'fillchars'"));
#اگر_تعریف_شده DYNAMIC_PYTHON
EXTERN حرف e_this_vim_cannot_execute_python_after_using_py3[]
	INIT(= N_("E836: This Vim cannot execute :python after using :py3"));
EXTERN حرف e_this_vim_cannot_execute_py3_after_using_python[]
	INIT(= N_("E837: This Vim cannot execute :py3 after using :python"));
#پایان_اگر
#إذا defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI)
EXTERN حرف e_netbeans_is_not_supported_with_this_GUI[]
	INIT(= N_("E838: NetBeans is not supported with this GUI"));
#پایان_اگر
// E839 unused
#اگر_تعریف_شده FEAT_COMPL_FUNC
EXTERN حرف e_complete_function_deleted_متن[]
	INIT(= N_("E840: Completion function deleted text"));
#پایان_اگر
EXTERN حرف e_reserved_name_cannot_be_استفاده_شده_for_user_defined_command[]
	INIT(= N_("E841: Reserved name, cannot be used for user defined command"));
EXTERN حرف e_no_سطر_عدد_مقصد_use_for_slnum[]
	INIT(= N_("E842: No line number to use for \"<slnum>\""));
#اگر_تعریف_شده FEAT_CRYPT
EXTERN حرف e_error_while_updating_جابجا_file_crypt[]
	INIT(= N_("E843: Error while updating swap file crypt"));
#پایان_اگر
#اگر_تعریف_شده FEAT_CONCEAL
EXTERN حرف e_invalid_cchar_مقدار[]
	INIT(= N_("E844: Invalid cchar value"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
EXTERN حرف e_insufficient_حافظه_کلمه_list_will_be_incomplete[]
	INIT(= N_("E845: Insufficient memory, word list will be incomplete"));
#پایان_اگر
EXTERN حرف e_کلید_کد_not_مجموعه[]
	INIT(= N_("E846: Key code not set"));
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN حرف e_too_many_syntax_includes[]
	INIT(= N_("E847: Too many syntax includes"));
EXTERN حرف e_too_many_syntax_clusters[]
	INIT(= N_("E848: Too many syntax clusters"));
#پایان_اگر
EXTERN حرف e_too_many_highlight_and_syntax_groups[]
	INIT(= N_("E849: Too many highlight and syntax groups"));
#اگر_تعریف_نشده FEAT_CLIPBOARD
EXTERN حرف e_invalid_ثبات_name[]
	INIT(= N_("E850: Invalid register name"));
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI
EXTERN حرف e_failed_مقصد_create_new_process_for_GUI[]
	INIT(= N_("E851: Failed to create a new process for the GUI"));
EXTERN حرف e_the_فرزند_process_failed_مقصد_شروع_GUI[]
	INIT(= N_("E852: The child process failed to start the GUI"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_duplicate_آرگومان_name_str[]
	INIT(= N_("E853: Duplicate argument name: %s"));
#پایان_اگر
EXTERN حرف e_مسیر_too_long_for_completion[]
	INIT(= N_("E854: Path too long for completion"));
EXTERN حرف e_autocommands_caused_command_مقصد_abort[]
	INIT(= N_("E855: Autocommands caused command to abort"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_assert_fails_دومی_arg[]
	INIT(= N_("E856: \"assert_fails()\" second argument must be a string or a list with one or two strings"));
EXTERN حرف e_dictionary_کلید_str_required[]
	INIT(= N_("E857: Dictionary key \"%s\" required"));
#پایان_اگر
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN حرف e_eval_did_not_return_valid_python_object[]
	INIT(= N_("E858: Eval did not return a valid python object"));
EXTERN حرف e_failed_مقصد_convert_returned_python_object_مقصد_vim_مقدار[]
	INIT(= N_("E859: Failed to convert returned python object to a Vim value"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_need_id_and_type_or_types_with_both[]
	INIT(= N_("E860: Need 'id' and 'type' or 'types' with 'both'"));
# ifdef FEAT_TERMINAL
EXTERN حرف e_cannot_open_دومی_popup_with_terminal[]
	INIT(= N_("E861: Cannot open a second popup with a terminal"));
# endif
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_use_g_اینجا[]
	INIT(= N_("E862: Cannot use g: here"));
#پایان_اگر
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
EXTERN حرف e_not_allowed_for_terminal_درون_popup_پنجره[]
	INIT(= N_("E863: Not allowed for a terminal in a popup window"));
#پایان_اگر
EXTERN حرف e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_استفاده_شده[]
	// xgettext:no-c-format
	INIT(= N_("E864: \\%#= can only be followed by 0, 1, or 2. The automatic engine will be used"));
EXTERN حرف e_nfa_regexp_انتها_encountered_prematurely[]
	INIT(= N_("E865: (NFA) Regexp end encountered prematurely"));
EXTERN حرف e_nfa_regexp_misplaced_chr[]
	INIT(= N_("E866: (NFA regexp) Misplaced %c"));
EXTERN حرف e_nfa_regexp_unknown_operator_z_chr[]
	INIT(= N_("E867: (NFA regexp) Unknown operator '\\z%c'"));
EXTERN حرف e_nfa_regexp_unknown_operator_percent_chr[]
	INIT(= N_("E867: (NFA regexp) Unknown operator '\\%%%c'"));
EXTERN حرف e_error_building_nfa_with_equivalence_class[]
	INIT(= N_("E868: Error building NFA with equivalence class!"));
EXTERN حرف e_nfa_regexp_unknown_operator_at_chr[]
	INIT(= N_("E869: (NFA regexp) Unknown operator '\\@%c'"));
EXTERN حرف e_nfa_regexp_error_reading_repetition_limits[]
	INIT(= N_("E870: (NFA regexp) Error reading repetition limits"));
EXTERN حرف e_nfa_regexp_cant_have_multi_follow_multi[]
	INIT(= N_("E871: (NFA regexp) Can't have a multi follow a multi"));
EXTERN حرف e_nfa_regexp_too_many_parens[]
	INIT(= N_("E872: (NFA regexp) Too many '('"));
EXTERN حرف e_nfa_regexp_proper_termination_error[]
	INIT(= N_("E873: (NFA regexp) proper termination error"));
EXTERN حرف e_nfa_regexp_could_not_برداشت_پشته[]
	INIT(= N_("E874: (NFA regexp) Could not pop the stack!"));
EXTERN حرف e_nfa_regexp_while_converting_from_postfix_مقصد_nfa_too_many_stats_چپ_on_پشته[]
	INIT(= N_("E875: (NFA regexp) (While converting from postfix to NFA), too many states left on stack"));
EXTERN حرف e_nfa_regexp_not_enough_space_مقصد_store_کامل_nfa[]
	INIT(= N_("E876: (NFA regexp) Not enough space to store the whole NFA"));
EXTERN حرف e_nfa_regexp_invalid_character_class_nr[]
	INIT(= N_("E877: (NFA regexp) Invalid character class: %d"));
EXTERN حرف e_nfa_regexp_could_not_allocate_حافظه_for_branch_traversal[]
	INIT(= N_("E878: (NFA regexp) Could not allocate memory for branch traversal!"));
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN حرف e_nfa_regexp_too_many_z[]
	INIT(= N_("E879: (NFA regexp) Too many \\z("));
#پایان_اگر
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN حرف e_cant_handle_systemexit_از_python_exception_درون_vim[]
	INIT(= N_("E880: Can't handle SystemExit of python exception in vim"));
#پایان_اگر
EXTERN حرف e_سطر_شمار_تغییریافته_unexpectedly[]
	INIT(= N_("E881: Line count changed unexpectedly"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_uniq_compare_function_failed[]
	INIT(= N_("E882: Uniq compare function failed"));
EXTERN حرف e_ثبات_char_cannot_contain_مضرب_سطرها[]
	INIT(= N_("E883: Register '%c' cannot contain multiple lines"));
EXTERN حرف e_function_name_cannot_contain_colon_str[]
	INIT(= N_("E884: Function name cannot contain a colon: %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
EXTERN حرف e_not_possible_مقصد_change_sign_str[]
	INIT(= N_("E885: Not possible to change sign %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_VIMINFO
EXTERN حرف e_cant_rename_viminfo_file_مقصد_str[]
	INIT(= N_("E886: Can't rename viminfo file to %s!"));
#پایان_اگر
#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN حرف e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded[]
	INIT(= N_("E887: Sorry, this command is disabled, the Python's site module could not be loaded."));
#پایان_اگر
EXTERN حرف e_nfa_regexp_cannot_repeat_str[]
	INIT(= N_("E888: (NFA regexp) cannot repeat %s"));
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_عدد_required[]
	INIT(= N_("E889: Number required"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SYN_HL
EXTERN حرف e_trailing_char_after_rsb_str_str[]
	INIT(= N_("E890: Trailing char after ']': %s]%s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_using_funcref_as_float[]
	INIT(= N_("E891: Using a Funcref as a Float"));
EXTERN حرف e_using_string_as_float[]
	INIT(= N_("E892: Using a String as a Float"));
EXTERN حرف e_using_list_as_float[]
	INIT(= N_("E893: Using a List as a Float"));
EXTERN حرف e_using_dictionary_as_float[]
	INIT(= N_("E894: Using a Dictionary as a Float"));
#پایان_اگر
#اگر_تعریف_شده FEAT_MZSCHEME
EXTERN حرف e_sorry_this_command_is_disabled_the_mzscheme_racket_پایه_module_could_not_be_loaded[]
	INIT(= N_("E895: Sorry, this command is disabled, the MzScheme's racket/base module could not be loaded."));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_آرگومان_از_str_must_be_list_dictionary_or_blob[]
	INIT(= N_("E896: Argument of %s must be a List, Dictionary or Blob"));
EXTERN حرف e_list_or_blob_required[]
	INIT(= N_("E897: List or Blob required"));
#پایان_اگر
#اگر_تعریف_شده FEAT_کار_CHANNEL
EXTERN حرف e_socket_درون_channel_connect[]
	INIT(= N_("E898: socket() in channel_connect()"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_آرگومان_از_str_must_be_list_or_blob[]
	INIT(= N_("E899: Argument of %s must be a List or Blob"));
EXTERN حرف e_maxdepth_must_be_non_negative_عدد[]
	INIT(= N_("E900: maxdepth must be non-negative number"));
#پایان_اگر
#اگر_تعریف_شده FEAT_کار_CHANNEL
EXTERN حرف e_getaddrinfo_درون_channel_open_str[]
	INIT(= N_("E901: getaddrinfo() in channel_open(): %s"));
# ifndef FEAT_IPV6
EXTERN حرف e_gethostbyname_درون_channel_open[]
	INIT(= N_("E901: gethostbyname() in channel_open()"));
# endif
EXTERN حرف e_cannot_connect_مقصد_پورت[]
	INIT(= N_("E902: Cannot connect to port"));
EXTERN حرف e_received_command_with_non_string_آرگومان[]
	INIT(= N_("E903: Received command with non-string argument"));
EXTERN حرف e_آخرین_آرگومان_for_عبارت_call_must_be_عدد[]
	INIT(= N_("E904: Last argument for expr/call must be a number"));
EXTERN حرف e_سوم_آرگومان_for_call_must_be_list[]
	INIT(= N_("E904: Third argument for call must be a list"));
EXTERN حرف e_received_unknown_command_str[]
	INIT(= N_("E905: Received unknown command: %s"));
EXTERN حرف e_not_an_open_channel[]
	INIT(= N_("E906: Not an open channel"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_using_special_مقدار_as_float[]
	INIT(= N_("E907: Using a special value as a Float"));
EXTERN حرف e_using_invalid_مقدار_as_string_str[]
	INIT(= N_("E908: Using an invalid value as a String: %s"));
EXTERN حرف e_cannot_شاخص_special_variable[]
	INIT(= N_("E909: Cannot index a special variable"));
#پایان_اگر
#اگر_تعریف_شده FEAT_کار_CHANNEL
EXTERN حرف e_using_کار_as_عدد[]
	INIT(= N_("E910: Using a Job as a Number"));
EXTERN حرف e_using_کار_as_float[]
	INIT(= N_("E911: Using a Job as a Float"));
EXTERN حرف e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel[]
	INIT(= N_("E912: Cannot use ch_evalexpr()/ch_sendexpr() with a raw, nl or blob channel"));
EXTERN حرف e_using_channel_as_عدد[]
	INIT(= N_("E913: Using a Channel as a Number"));
EXTERN حرف e_using_channel_as_float[]
	INIT(= N_("E914: Using a Channel as a Float"));
EXTERN حرف e_درون_io_میانگیر_requires_درون_buf_or_درون_name_مقصد_be_مجموعه[]
	INIT(= N_("E915: in_io buffer requires in_buf or in_name to be set"));
EXTERN حرف e_not_valid_کار[]
	INIT(= N_("E916: Not a valid job"));
EXTERN حرف e_cannot_use_callback_with_str[]
	INIT(= N_("E917: Cannot use a callback with %s()"));
EXTERN حرف e_میانگیر_must_be_loaded_str[]
	INIT(= N_("E918: Buffer must be loaded: %s"));
#پایان_اگر
EXTERN حرف e_directory_not_یافت_درون_str_str[]
	INIT(= N_("E919: Directory not found in '%s': \"%s\""));
#اگر_تعریف_شده FEAT_کار_CHANNEL
EXTERN حرف e_io_file_requires_name_مقصد_be_مجموعه[]
	INIT(= N_("E920: _io file requires _name to be set"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_invalid_callback_آرگومان[]
	INIT(= N_("E921: Invalid callback argument"));
// E922 unused
EXTERN حرف e_دومی_آرگومان_از_function_must_be_list_or_dict[]
	INIT(= N_("E923: Second argument of function() must be a list or a dict"));
#پایان_اگر
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_current_پنجره_was_closed[]
	INIT(= N_("E924: Current window was closed"));
EXTERN حرف e_current_quickfix_list_was_تغییریافته[]
	INIT(= N_("E925: Current quickfix list was changed"));
EXTERN حرف e_current_location_list_was_تغییریافته[]
	INIT(= N_("E926: Current location list was changed"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
# ifdef FEAT_QUICKFIX
EXTERN حرف e_invalid_action_str_1[]
	INIT(= N_("E927: Invalid action: '%s'"));
# endif
EXTERN حرف e_string_required[]
	INIT(= N_("E928: String required"));
#پایان_اگر
#اگر_تعریف_شده FEAT_VIMINFO
EXTERN حرف e_too_many_viminfo_موقت_files_like_str[]
	INIT(= N_("E929: Too many viminfo temp files, like %s!"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_use_redir_inside_execute[]
	INIT(= N_("E930: Cannot use :redir inside execute()"));
#پایان_اگر
EXTERN حرف e_میانگیر_cannot_be_registered[]
	INIT(= N_("E931: Buffer cannot be registered"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_بستگی_function_should_not_be_at_بالاترین_level_str[]
	INIT(= N_("E932: Closure function should not be at top level: %s"));
EXTERN حرف e_function_was_deleted_str[]
	INIT(= N_("E933: Function was deleted: %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
EXTERN حرف e_cannot_jump_مقصد_میانگیر_that_does_not_have_name[]
	INIT(= N_("E934: Cannot jump to a buffer that does not have a name"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_invalid_submatch_عدد_nr[]
	INIT(= N_("E935: Invalid submatch number: %d"));
#پایان_اگر
EXTERN حرف e_cannot_delete_current_گروه[]
	INIT(= N_("E936: Cannot delete the current group"));
EXTERN حرف e_attempt_مقصد_delete_میانگیر_that_is_درون_use_str[]
	INIT(= N_("E937: Attempt to delete a buffer that is in use: %s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_duplicate_کلید_درون_json_str[]
	INIT(= N_("E938: Duplicate key in JSON: \"%s\""));
#پایان_اگر
EXTERN حرف e_مثبت_شمار_required[]
	INIT(= N_("E939: Positive count required"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_قفل_or_unlock_variable_str[]
	INIT(= N_("E940: Cannot lock or unlock variable %s"));
# ifdef FEAT_CLIENTSERVER
EXTERN حرف e_already_started_server[]
	INIT(= N_("E941: Already started a server"));
# وإلا
EXTERN حرف e_clientserver_feature_not_موجود[]
	INIT(= N_("E942: +clientserver feature not available"));
# endif
#پایان_اگر
EXTERN حرف e_command_جدول_needs_مقصد_be_updated_پیاپی_make_cmdidxs[]
	INIT(= "E943: Command table needs to be updated, run 'make cmdidxs'");
EXTERN حرف e_معکوس_range_درون_character_class[]
	INIT(= N_("E944: Reverse range in character class"));
EXTERN حرف e_range_too_large_درون_character_class[]
	INIT(= N_("E945: Range too large in character class"));
#اگر_تعریف_شده FEAT_TERMINAL
EXTERN حرف e_cannot_make_terminal_with_running_کار_modifiable[]
	INIT(= N_("E946: Cannot make a terminal with running job modifiable"));
EXTERN حرف e_کار_still_running_درون_میانگیر_str[]
	INIT(= N_("E947: Job still running in buffer \"%s\""));
EXTERN حرف e_کار_still_running[]
	INIT(= N_("E948: Job still running"));
EXTERN حرف e_کار_still_running_جمع_کن_bang_مقصد_انتها_the_کار[]
	INIT(= N_("E948: Job still running (add ! to end the job)"));
#پایان_اگر
EXTERN حرف e_file_تغییریافته_while_writing[]
	INIT(= N_("E949: File changed while writing"));
EXTERN حرف e_cannot_convert_between_str_and_str[]
	INIT(= N_("E950: Cannot convert between %s and %s"));
EXTERN حرف e_percent_مقدار_too_large[]
	// xgettext:no-c-format
	INIT(= N_("E951: \\% value too large"));
#إذا defined(FEAT_EVAL) && defined(FEAT_QUICKFIX)
EXTERN حرف e_autocommand_caused_recursive_behavior[]
	INIT(= N_("E952: Autocommand caused recursive behavior"));
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
EXTERN حرف e_file_exists_str[]
	INIT(= N_("E953: File exists: %s"));
#پایان_اگر
#إذا defined(FEAT_TERMGUICOLORS) && defined(FEAT_VTP)
EXTERN حرف e_24_بیت_colors_are_not_supported_on_this_environment[]
	INIT(= N_("E954: 24-bit colors are not supported on this environment"));
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
EXTERN حرف e_not_terminal_میانگیر[]
	INIT(= N_("E955: Not a terminal buffer"));
#پایان_اگر
EXTERN حرف e_cannot_use_الگو_recursively[]
	INIT(= N_("E956: Cannot use pattern recursively"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_invalid_پنجره_عدد[]
	INIT(= N_("E957: Invalid window number"));
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
EXTERN حرف e_کار_already_پایانیافته[]
	INIT(= N_("E958: Job already finished"));
#پایان_اگر
#اگر_تعریف_شده FEAT_DIFF
EXTERN حرف e_invalid_diff_format[]
	INIT(= N_("E959: Invalid diff format."));
EXTERN حرف e_problem_creating_داخلی_diff[]
	INIT(= N_("E960: Problem creating the internal diff"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_no_سطر_عدد_مقصد_use_for_sflnum[]
	INIT(= N_("E961: No line number to use for \"<sflnum>\""));
EXTERN حرف e_invalid_action_str_2[]
	INIT(= N_("E962: Invalid action: '%s'"));
EXTERN حرف e_setting_v_str_مقصد_مقدار_with_wrong_type[]
	INIT(= N_("E963: Setting v:%s to value with wrong type"));
#پایان_اگر
#إذا defined(FEAT_PROP_POPUP) || defined(FEAT_EVAL)
EXTERN char_u e_invalid_ستونی_عدد_nr[]
	INIT(= N_("E964: Invalid column number: %ld"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_missing_property_type_name[]
	INIT(= N_("E965: Missing property type name"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN char_u e_invalid_سطر_عدد_nr[]
	INIT(= N_("E966: Invalid line number: %ld"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_متن_property_info_corrupted[]
	INIT(= "E967: Text property info corrupted");
EXTERN حرف e_need_at_کمترین_one_از_id_or_type[]
	INIT(= N_("E968: Need at least one of 'id' or 'type'"));
EXTERN حرف e_property_type_str_already_defined[]
	INIT(= N_("E969: Property type %s already defined"));
EXTERN حرف e_unknown_highlight_گروه_name_str[]
	INIT(= N_("E970: Unknown highlight group name: '%s'"));
EXTERN حرف e_property_type_str_does_not_exist[]
	INIT(= N_("E971: Property type %s does not exist"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_blob_مقدار_does_not_have_راست_عدد_از_بایتها[]
	INIT(= N_("E972: Blob value does not have the right number of bytes"));
EXTERN حرف e_blob_literal_should_have_an_even_عدد_از_hex_characters[]
	INIT(= N_("E973: Blob literal should have an even number of hex characters"));
EXTERN حرف e_using_blob_as_عدد[]
	INIT(= N_("E974: Using a Blob as a Number"));
EXTERN حرف e_using_blob_as_float[]
	INIT(= N_("E975: Using a Blob as a Float"));
EXTERN حرف e_using_blob_as_string[]
	INIT(= N_("E976: Using a Blob as a String"));
EXTERN حرف e_can_only_compare_blob_with_blob[]
	INIT(= N_("E977: Can only compare Blob with Blob"));
EXTERN حرف e_invalid_operation_for_blob[]
	INIT(= N_("E978: Invalid operation for Blob"));
EXTERN حرف e_blob_شاخص_بیرون_از_range_nr[]
	INIT(= N_("E979: Blob index out of range: %ld"));
# ifndef USE_درون_داد_BUF
EXTERN حرف e_lowlevel_درون_داد_not_supported[]
	INIT(= N_("E980: Lowlevel input not supported"));
# endif
#پایان_اگر
EXTERN حرف e_command_not_allowed_درون_rvim[]
	INIT(= N_("E981: Command not allowed in rvim"));
#إذا defined(FEAT_TERMINAL) && defined(MSWIN)
EXTERN حرف e_conpty_is_not_موجود[]
	INIT(= N_("E982: ConPTY is not available"));
#پایان_اگر
EXTERN حرف e_duplicate_آرگومان_str[]
	INIT(= N_("E983: Duplicate argument: %s"));
EXTERN حرف e_scriptversion_استفاده_شده_outside_از_sourced_file[]
	INIT(= N_("E984: :scriptversion used outside of a sourced file"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_dot_equal_not_supported_with_script_نسخه_two[]
	INIT(= N_("E985: .= is not supported with script version >= 2"));
EXTERN حرف e_cannot_modify_برچسب_پشته_within_tagfunc[]
	INIT(= N_("E986: Cannot modify the tag stack within tagfunc"));
EXTERN حرف e_invalid_return_مقدار_from_tagfunc[]
	INIT(= N_("E987: Invalid return value from tagfunc"));
#پایان_اگر
#اگر_تعریف_شده GUI_MAY_SPAWN
EXTERN حرف e_gui_cannot_be_استفاده_شده_cannot_execute_gvim_exe[]
	INIT(= N_("E988: GUI cannot be used. Cannot execute gvim.exe."));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_non_default_آرگومان_follows_default_آرگومان[]
	INIT(= N_("E989: Non-default argument follows default argument"));
EXTERN حرف e_missing_انتها_marker_str[]
	INIT(= N_("E990: Missing end marker '%s'"));
EXTERN حرف e_cannot_use_heredoc_اینجا[]
	INIT(= N_("E991: Cannot use =<< here"));
#پایان_اگر
EXTERN حرف e_not_allowed_درون_modeline_when_modelineexpr_is_off[]
	INIT(= N_("E992: Not allowed in a modeline when 'modelineexpr' is off"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_پنجره_nr_is_not_popup_پنجره[]
	INIT(= N_("E993: Window %d is not a popup window"));
EXTERN حرف e_not_allowed_درون_popup_پنجره[]
	INIT(= N_("E994: Not allowed in a popup window"));
EXTERN حرف e_cannot_modify_existing_variable[]
	INIT(= N_("E995: Cannot modify existing variable"));
EXTERN حرف e_cannot_قفل_range[]
	INIT(= N_("E996: Cannot lock a range"));
EXTERN حرف e_cannot_قفل_option[]
	INIT(= N_("E996: Cannot lock an option"));
EXTERN حرف e_cannot_قفل_list_or_dict[]
	INIT(= N_("E996: Cannot lock a list or dict"));
EXTERN حرف e_cannot_قفل_environment_variable[]
	INIT(= N_("E996: Cannot lock an environment variable"));
EXTERN حرف e_cannot_قفل_ثبات[]
	INIT(= N_("E996: Cannot lock a register"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_tabpage_not_یافت_nr[]
	INIT(= N_("E997: Tabpage not found: %d"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_reduce_از_an_خالی_str_with_no_initial_مقدار[]
	INIT(= N_("E998: Reduce of an empty %s with no initial value"));
#پایان_اگر
EXTERN حرف e_scriptversion_not_supported_nr[]
	INIT(= N_("E999: scriptversion not supported: %d"));
// E1000 unused
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_variable_not_یافت_str[]
	INIT(= N_("E1001: Variable not found: %s"));
EXTERN حرف e_syntax_error_at_str[]
	INIT(= N_("E1002: Syntax error at %s"));
EXTERN حرف e_missing_return_مقدار[]
	INIT(= N_("E1003: Missing return value"));
EXTERN حرف e_white_space_required_before_and_after_str_at_str[]
	INIT(= N_("E1004: White space required before and after '%s' at \"%s\""));
EXTERN حرف e_too_many_آرگومان_types[]
	INIT(= N_("E1005: Too many argument types"));
EXTERN حرف e_str_is_استفاده_شده_as_آرگومان[]
	INIT(= N_("E1006: %s is used as an argument"));
EXTERN حرف e_mandatory_آرگومان_after_optional_آرگومان[]
	INIT(= N_("E1007: Mandatory argument after optional argument"));
EXTERN حرف e_missing_type_after_str[]
	INIT(= N_("E1008: Missing <type> after %s"));
EXTERN حرف e_missing_gt_after_type_str[]
	INIT(= N_("E1009: Missing > after type: %s"));
EXTERN حرف e_type_not_recognized_str[]
	INIT(= N_("E1010: Type not recognized: %s"));
EXTERN حرف e_name_too_long_str[]
	INIT(= N_("E1011: Name too long: %s"));
EXTERN حرف e_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1012: Type mismatch; expected %s but got %s"));
EXTERN حرف e_type_mismatch_expected_str_but_got_str_درون_str[]
	INIT(= N_("E1012: Type mismatch; expected %s but got %s in %s"));
EXTERN حرف e_آرگومان_nr_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1013: Argument %d: type mismatch, expected %s but got %s"));
EXTERN حرف e_آرگومان_nr_type_mismatch_expected_str_but_got_str_درون_str[]
	INIT(= N_("E1013: Argument %d: type mismatch, expected %s but got %s in %s"));
EXTERN حرف e_invalid_کلید_str[]
	INIT(= N_("E1014: Invalid key: %s"));
EXTERN حرف e_name_expected_str[]
	INIT(= N_("E1015: Name expected: %s"));
EXTERN حرف e_cannot_declare_a_scope_variable_str[]
	INIT(= N_("E1016: Cannot declare a %s variable: %s"));
EXTERN حرف e_cannot_declare_an_environment_variable_str[]
	INIT(= N_("E1016: Cannot declare an environment variable: %s"));
EXTERN حرف e_variable_already_declared_str[]
	INIT(= N_("E1017: Variable already declared: %s"));
EXTERN حرف e_cannot_تخصیص_بده_مقصد_constant_str[]
	INIT(= N_("E1018: Cannot assign to a constant: %s"));
EXTERN حرف e_can_only_concatenate_مقصد_string[]
	INIT(= N_("E1019: Can only concatenate to string"));
EXTERN حرف e_cannot_use_operator_on_new_variable_str[]
	INIT(= N_("E1020: Cannot use an operator on a new variable: %s"));
EXTERN حرف e_const_requires_a_مقدار[]
	INIT(= N_("E1021: Const requires a value"));
EXTERN حرف e_type_or_initialization_required[]
	INIT(= N_("E1022: Type or initialization required"));
EXTERN حرف e_using_عدد_as_bool_nr[]
	INIT(= N_("E1023: Using a Number as a Bool: %lld"));
EXTERN حرف e_using_عدد_as_string[]
	INIT(= N_("E1024: Using a Number as a String"));
EXTERN حرف e_using_rcurly_outside_if_بلوک_scope[]
	INIT(= N_("E1025: Using } outside of a block scope"));
#پایان_اگر
EXTERN حرف e_missing_rcurly[]
	INIT(= N_("E1026: Missing }"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_missing_return_statement[]
	INIT(= N_("E1027: Missing return statement"));
EXTERN حرف e_compiling_def_function_failed[]
	INIT(= N_("E1028: Compiling :def function failed"));
EXTERN حرف e_expected_str_but_got_str[]
	INIT(= N_("E1029: Expected %s but got %s"));
EXTERN حرف e_using_string_as_عدد_str[]
	INIT(= N_("E1030: Using a String as a Number: \"%s\""));
EXTERN حرف e_cannot_use_void_مقدار[]
	INIT(= N_("E1031: Cannot use void value"));
EXTERN حرف e_missing_catch_or_finally[]
	INIT(= N_("E1032: Missing :catch or :finally"));
EXTERN حرف e_catch_unreachable_after_catch_all[]
	INIT(= N_("E1033: Catch unreachable after catch-all"));
EXTERN حرف e_cannot_use_reserved_name_str[]
	INIT(= N_("E1034: Cannot use reserved name %s"));
EXTERN حرف e_percent_requires_عدد_arguments[]
	// xgettext:no-c-format
	INIT(= N_("E1035: % requires number arguments"));
EXTERN حرف e_char_requires_عدد_or_float_arguments[]
	INIT(= N_("E1036: %c requires number or float arguments"));
EXTERN حرف e_cannot_use_str_with_str[]
	INIT(= N_("E1037: Cannot use \"%s\" with %s"));
EXTERN حرف e_vim9script_can_only_be_استفاده_شده_درون_script[]
	INIT(= N_("E1038: \"vim9script\" can only be used in a script"));
EXTERN حرف e_vim9script_must_be_اولی_command_درون_script[]
	INIT(= N_("E1039: \"vim9script\" must be the first command in a script"));
#پایان_اگر
EXTERN حرف e_cannot_use_scriptversion_after_vim9script[]
	INIT(= N_("E1040: Cannot use :scriptversion after :vim9script"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_redefining_script_قلم_str[]
	INIT(= N_("E1041: Redefining script item: \"%s\""));
EXTERN حرف e_export_can_only_be_استفاده_شده_درون_vim9script[]
	INIT(= N_("E1042: Export can only be used in vim9script"));
EXTERN حرف e_invalid_command_after_export[]
	INIT(= N_("E1043: Invalid command after :export"));
EXTERN حرف e_export_with_invalid_آرگومان[]
	INIT(= N_("E1044: Export with invalid argument"));
// E1045 not used
// E1046 not used
EXTERN حرف e_syntax_error_درون_import_str[]
	INIT(= N_("E1047: Syntax error in import: %s"));
EXTERN حرف e_قلم_not_یافت_درون_script_str[]
	INIT(= N_("E1048: Item not found in script: %s"));
EXTERN حرف e_قلم_not_exported_درون_script_str[]
	INIT(= N_("E1049: Item not exported in script: %s"));
#پایان_اگر
EXTERN حرف e_colon_required_before_range_str[]
	INIT(= N_("E1050: Colon required before a range: %s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_wrong_آرگومان_type_for_plus[]
	INIT(= N_("E1051: Wrong argument type for +"));
EXTERN حرف e_cannot_declare_an_option_str[]
	INIT(= N_("E1052: Cannot declare an option: %s"));
EXTERN حرف e_could_not_import_str[]
	INIT(= N_("E1053: Could not import \"%s\""));
EXTERN حرف e_variable_already_declared_درون_script_str[]
	INIT(= N_("E1054: Variable already declared in the script: %s"));
EXTERN حرف e_missing_name_after_dots[]
	INIT(= N_("E1055: Missing name after ..."));
EXTERN حرف e_expected_type_str[]
	INIT(= N_("E1056: Expected a type: %s"));
EXTERN حرف e_missing_enddef[]
	INIT(= N_("E1057: Missing :enddef"));
EXTERN حرف e_function_nesting_too_deep[]
	INIT(= N_("E1058: Function nesting too deep"));
EXTERN حرف e_no_white_space_allowed_before_colon_str[]
	INIT(= N_("E1059: No white space allowed before colon: %s"));
EXTERN حرف e_expected_dot_after_name_str[]
	INIT(= N_("E1060: Expected dot after name: %s"));
EXTERN حرف e_cannot_پیدا_کن_function_str[]
	INIT(= N_("E1061: Cannot find function %s"));
EXTERN حرف e_cannot_شاخص_عدد[]
	INIT(= N_("E1062: Cannot index a Number"));
EXTERN حرف e_type_mismatch_for_v_variable[]
	INIT(= N_("E1063: Type mismatch for v: variable"));
#پایان_اگر
EXTERN حرف e_yank_ثبات_تغییریافته_while_using_it[]
	INIT(= N_("E1064: Yank register changed while using it"));
EXTERN حرف e_command_cannot_be_shortened_str[]
	INIT(= N_("E1065: Command cannot be shortened: %s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_declare_a_ثبات_str[]
	INIT(= N_("E1066: Cannot declare a register: %s"));
EXTERN حرف e_separator_mismatch_str[]
	INIT(= N_("E1067: Separator mismatch: %s"));
EXTERN حرف e_no_white_space_allowed_before_str_str[]
	INIT(= N_("E1068: No white space allowed before '%s': %s"));
EXTERN حرف e_white_space_required_after_str_str[]
	INIT(= N_("E1069: White space required after '%s': %s"));
EXTERN حرف e_invalid_string_for_import_str[]
	INIT(= N_("E1071: Invalid string for :import: %s"));
EXTERN حرف e_cannot_compare_str_with_str[]
	INIT(= N_("E1072: Cannot compare %s with %s"));
EXTERN حرف e_name_already_defined_str[]
	INIT(= N_("E1073: Name already defined: %s"));
EXTERN حرف e_no_white_space_allowed_after_dot[]
	INIT(= N_("E1074: No white space allowed after dot"));
EXTERN حرف e_namespace_not_supported_str[]
	INIT(= N_("E1075: Namespace not supported: %s"));
// E1076 unused (was deleted)
EXTERN حرف e_missing_آرگومان_type_for_str[]
	INIT(= N_("E1077: Missing argument type for %s"));
#پایان_اگر
EXTERN حرف e_invalid_command_nested_did_you_میانگین_plusplus_nested[]
	INIT(= N_("E1078: Invalid command \"nested\", did you mean \"++nested\"?"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_declare_variable_on_command_سطر[]
	INIT(= N_("E1079: Cannot declare a variable on the command line"));
EXTERN حرف e_invalid_assignment[]
	INIT(= N_("E1080: Invalid assignment"));
EXTERN حرف e_cannot_unlet_str[]
	INIT(= N_("E1081: Cannot unlet %s"));
#پایان_اگر
EXTERN حرف e_command_modifier_without_command[]
	INIT(= N_("E1082: Command modifier without command"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_missing_backtick[]
	INIT(= N_("E1083: Missing backtick"));
EXTERN حرف e_cannot_delete_vim9_script_function_str[]
	INIT(= N_("E1084: Cannot delete Vim9 script function %s"));
EXTERN حرف e_not_callable_type_str[]
	INIT(= N_("E1085: Not a callable type: %s"));
// E1086 unused
EXTERN حرف e_cannot_use_شاخص_when_declaring_variable[]
	INIT(= N_("E1087: Cannot use an index when declaring a variable"));
EXTERN حرف e_script_cannot_import_itself[]
	INIT(= N_("E1088: Script cannot import itself"));
EXTERN حرف e_unknown_variable_str[]
	INIT(= N_("E1089: Unknown variable: %s"));
EXTERN حرف e_cannot_تخصیص_بده_مقصد_آرگومان_str[]
	INIT(= N_("E1090: Cannot assign to argument %s"));
EXTERN حرف e_function_is_not_compiled_str[]
	INIT(= N_("E1091: Function is not compiled: %s"));
EXTERN حرف e_cannot_nest_redir[]
	INIT(= N_("E1092: Cannot nest :redir"));
EXTERN حرف e_expected_nr_اقلام_but_got_nr[]
	INIT(= N_("E1093: Expected %d items but got %d"));
EXTERN حرف e_import_can_only_be_استفاده_شده_درون_script[]
	INIT(= N_("E1094: Import can only be used in a script"));
EXTERN حرف e_unreachable_کد_after_str[]
	INIT(= N_("E1095: Unreachable code after :%s"));
EXTERN حرف e_returning_مقدار_درون_function_without_return_type[]
	INIT(= N_("E1096: Returning a value in a function without a return type"));
EXTERN حرف e_سطر_incomplete[]
	INIT(= N_("E1097: Line incomplete"));
EXTERN حرف e_string_list_or_blob_required[]
	INIT(= N_("E1098: String, List or Blob required"));
EXTERN حرف e_unknown_error_while_executing_str[]
	INIT(= N_("E1099: Unknown error while executing %s"));
EXTERN حرف e_command_not_supported_درون_vim9_script_missing_var_str[]
	INIT(= N_("E1100: Command not supported in Vim9 script (missing :var?): %s"));
EXTERN حرف e_cannot_declare_script_variable_درون_function_str[]
	INIT(= N_("E1101: Cannot declare a script variable in a function: %s"));
EXTERN حرف e_lambda_function_not_یافت_str[]
	INIT(= N_("E1102: Lambda function not found: %s"));
EXTERN حرف e_dictionary_not_مجموعه[]
	INIT(= N_("E1103: Dictionary not set"));
EXTERN حرف e_missing_gt[]
	INIT(= N_("E1104: Missing >"));
EXTERN حرف e_cannot_convert_str_مقصد_string[]
	INIT(= N_("E1105: Cannot convert %s to string"));

PLURAL_پیام(e_one_آرگومان_too_many, "E1106: One argument too many",
		e_nr_arguments_too_many, "E1106: %d arguments too many")

EXTERN حرف e_string_list_dict_or_blob_required[]
	INIT(= N_("E1107: String, List, Dict or Blob required"));
// E1108 unused
EXTERN حرف e_list_قلم_nr_is_not_list[]
	INIT(= N_("E1109: List item %d is not a List"));
EXTERN حرف e_list_قلم_nr_does_not_contain_3_numbers[]
	INIT(= N_("E1110: List item %d does not contain 3 numbers"));
EXTERN حرف e_list_قلم_nr_range_invalid[]
	INIT(= N_("E1111: List item %d range invalid"));
EXTERN حرف e_list_قلم_nr_cell_عرض_invalid[]
	INIT(= N_("E1112: List item %d cell width invalid"));
EXTERN حرف e_overlapping_ranges_for_nr[]
	INIT(= N_("E1113: Overlapping ranges for 0x%lx"));
EXTERN حرف e_only_مقادیر_از_0x80_and_higher_supported[]
	INIT(= N_("E1114: Only values of 0x80 and higher supported"));
EXTERN حرف e_assert_fails_fourth_آرگومان[]
	INIT(= N_("E1115: \"assert_fails()\" fourth argument must be a number"));
EXTERN حرف e_assert_fails_fifth_آرگومان[]
	INIT(= N_("E1116: \"assert_fails()\" fifth argument must be a string"));
EXTERN حرف e_cannot_use_bang_with_nested_def_str[]
	INIT(= N_("E1117: Cannot use ! with nested %s"));
EXTERN حرف e_cannot_change_locked_list[]
	INIT(= N_("E1118: Cannot change locked list"));
EXTERN حرف e_cannot_change_locked_list_قلم[]
	INIT(= N_("E1119: Cannot change locked list item"));
EXTERN حرف e_cannot_change_dict[]
	INIT(= N_("E1120: Cannot change dict"));
EXTERN حرف e_cannot_change_dict_قلم[]
	INIT(= N_("E1121: Cannot change dict item"));
EXTERN حرف e_variable_is_locked_str[]
	INIT(= N_("E1122: Variable is locked: %s"));
EXTERN حرف e_missing_comma_before_آرگومان_str[]
	INIT(= N_("E1123: Missing comma before argument: %s"));
EXTERN حرف e_str_cannot_be_استفاده_شده_درون_legacy_vim_script[]
	INIT(= N_("E1124: \"%s\" cannot be used in legacy Vim script"));
EXTERN حرف e_final_requires_a_مقدار[]
	INIT(= N_("E1125: Final requires a value"));
EXTERN حرف e_cannot_use_let_درون_vim9_script[]
	INIT(= N_("E1126: Cannot use :let in Vim9 script"));
EXTERN حرف e_missing_name_after_dot[]
	INIT(= N_("E1127: Missing name after dot"));
EXTERN حرف e_endblock_without_بلوک[]
	INIT(= N_("E1128: } without {"));
EXTERN حرف e_throw_with_خالی_string[]
	INIT(= N_("E1129: Throw with empty string"));
EXTERN حرف e_cannot_جمع_کن_مقصد_null_list[]
	INIT(= N_("E1130: Cannot add to null list"));
EXTERN حرف e_cannot_جمع_کن_مقصد_null_blob[]
	INIT(= N_("E1131: Cannot add to null blob"));
EXTERN حرف e_missing_function_آرگومان[]
	INIT(= N_("E1132: Missing function argument"));
EXTERN حرف e_cannot_extend_null_dict[]
	INIT(= N_("E1133: Cannot extend a null dict"));
EXTERN حرف e_cannot_extend_null_list[]
	INIT(= N_("E1134: Cannot extend a null list"));
EXTERN حرف e_using_string_as_bool_str[]
	INIT(= N_("E1135: Using a String as a Bool: \"%s\""));
#پایان_اگر
EXTERN حرف e_cmd_mapping_must_انتها_with_cr_before_دومی_cmd[]
	INIT(= N_("E1136: <Cmd> mapping must end with <CR> before second <Cmd>"));
// E1137 unused
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_using_bool_as_عدد[]
	INIT(= N_("E1138: Using a Bool as a Number"));
EXTERN حرف e_missing_matching_bracket_after_dict_کلید[]
	INIT(= N_("E1139: Missing matching bracket after dict key"));
EXTERN حرف e_for_آرگومان_must_be_sequence_از_lists_or_tuples[]
	INIT(= N_("E1140: :for argument must be a sequence of lists or tuples"));
EXTERN حرف e_indexable_type_required[]
	INIT(= N_("E1141: Indexable type required"));
EXTERN حرف e_calling_test_garbagecollect_now_while_v_testing_is_not_مجموعه[]
	INIT(= N_("E1142: Calling test_garbagecollect_now() while v:testing is not set"));
EXTERN حرف e_خالی_عبارت_str[]
	INIT(= N_("E1143: Empty expression: \"%s\""));
EXTERN حرف e_command_str_not_followed_by_white_space_str[]
	INIT(= N_("E1144: Command \"%s\" is not followed by white space: %s"));
EXTERN حرف e_missing_heredoc_انتها_marker_str[]
	INIT(= N_("E1145: Missing heredoc end marker: %s"));
EXTERN حرف e_command_not_recognized_str[]
	INIT(= N_("E1146: Command not recognized: %s"));
EXTERN حرف e_list_not_مجموعه[]
	INIT(= N_("E1147: List not set"));
EXTERN حرف e_cannot_شاخص_str[]
	INIT(= N_("E1148: Cannot index a %s"));
EXTERN حرف e_script_variable_invalid_after_reload_درون_function_str[]
	INIT(= N_("E1149: Script variable is invalid after reload in function %s"));
EXTERN حرف e_script_variable_type_تغییریافته[]
	INIT(= N_("E1150: Script variable type changed"));
EXTERN حرف e_mismatched_endfunction[]
	INIT(= N_("E1151: Mismatched endfunction"));
EXTERN حرف e_mismatched_enddef[]
	INIT(= N_("E1152: Mismatched enddef"));
EXTERN حرف e_invalid_operation_for_str[]
	INIT(= N_("E1153: Invalid operation for %s"));
EXTERN حرف e_divide_by_zero[]
	INIT(= N_("E1154: Divide by zero"));
#پایان_اگر
EXTERN حرف e_cannot_define_autocommands_for_all_events[]
	INIT(= N_("E1155: Cannot define autocommands for ALL events"));
EXTERN حرف e_cannot_change_arglist_recursively[]
	INIT(= N_("E1156: Cannot change the argument list recursively"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_missing_return_type[]
	INIT(= N_("E1157: Missing return type"));
EXTERN حرف e_cannot_use_flatten_درون_vim9_script[]
	INIT(= N_("E1158: Cannot use flatten() in Vim9 script, use flattennew()"));
#پایان_اگر
EXTERN حرف e_cannot_تقسیم_پنجره_when_closing_میانگیر[]
	INIT(= N_("E1159: Cannot split a window when closing the buffer"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_use_default_for_variable_arguments[]
	INIT(= N_("E1160: Cannot use a default for variable arguments"));
EXTERN حرف e_cannot_json_encode_str[]
	INIT(= N_("E1161: Cannot json encode a %s"));
EXTERN حرف e_ثبات_name_must_be_one_char_str[]
	INIT(= N_("E1162: Register name must be one character: %s"));
EXTERN حرف e_variable_nr_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1163: Variable %d: type mismatch, expected %s but got %s"));
EXTERN حرف e_variable_nr_type_mismatch_expected_str_but_got_str_درون_str[]
	INIT(= N_("E1163: Variable %d: type mismatch, expected %s but got %s in %s"));
#پایان_اگر
EXTERN حرف e_vim9cmd_must_be_followed_by_command[]
	INIT(= N_("E1164: vim9cmd must be followed by a command"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_use_range_with_assignment_str[]
	INIT(= N_("E1165: Cannot use a range with an assignment: %s"));
EXTERN حرف e_cannot_use_range_with_dictionary[]
	INIT(= N_("E1166: Cannot use a range with a dictionary"));
EXTERN حرف e_آرگومان_name_shadows_existing_variable_str[]
	INIT(= N_("E1167: Argument name shadows existing variable: %s"));
EXTERN حرف e_آرگومان_already_declared_درون_script_str[]
	INIT(= N_("E1168: Argument already declared in the script: %s"));
EXTERN حرف e_عبارت_too_recursive_str[]
	INIT(= N_("E1169: Expression too recursive: %s"));
EXTERN حرف e_cannot_use_hash_curly_مقصد_شروع_comment[]
	INIT(= N_("E1170: Cannot use #{ to start a comment"));
EXTERN حرف e_missing_انتها_بلوک[]
	INIT(= N_("E1171: Missing } after inline function"));
EXTERN حرف e_cannot_use_default_مقادیر_درون_lambda[]
	INIT(= N_("E1172: Cannot use default values in a lambda"));
EXTERN حرف e_متن_یافت_after_str_str[]
	INIT(= N_("E1173: Text found after %s: %s"));
EXTERN حرف e_string_required_for_آرگومان_nr[]
	INIT(= N_("E1174: String required for argument %d"));
EXTERN حرف e_non_خالی_string_required_for_آرگومان_nr[]
	INIT(= N_("E1175: Non-empty string required for argument %d"));
EXTERN حرف e_misplaced_command_modifier[]
	INIT(= N_("E1176: Misplaced command modifier"));
EXTERN حرف e_for_loop_on_str_not_supported[]
	INIT(= N_("E1177: For loop on %s not supported"));
EXTERN حرف e_cannot_قفل_unlock_local_variable[]
	INIT(= N_("E1178: Cannot lock or unlock a local variable"));
#پایان_اگر
#اگر_تعریف_شده FEAT_TERMINAL
EXTERN حرف e_failed_مقصد_استخراج_pwd_from_str_check_your_shell_config[]
	INIT(= N_("E1179: Failed to extract PWD from %s, check your shell's config related to OSC 7"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_variable_arguments_type_must_be_list_str[]
	INIT(= N_("E1180: Variable arguments type must be a list: %s"));
EXTERN حرف e_cannot_use_underscore_اینجا[]
	INIT(= N_("E1181: Cannot use an underscore here"));
EXTERN حرف e_cannot_define_dict_func_درون_vim9_script_str[]
	INIT(= N_("E1182: Cannot define a dict function in Vim9 script: %s"));
EXTERN حرف e_cannot_use_range_with_assignment_operator_str[]
	INIT(= N_("E1183: Cannot use a range with an assignment operator: %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_blob_not_مجموعه[]
	INIT(= N_("E1184: Blob not set"));
EXTERN حرف e_missing_redir_انتها[]
	INIT(= N_("E1185: Missing :redir END"));
EXTERN حرف e_عبارت_does_not_نتیجه_درون_مقدار_str[]
	INIT(= N_("E1186: Expression does not result in a value: %s"));
#پایان_اگر
EXTERN حرف e_failed_مقصد_منبع_defaults[]
	INIT(= N_("E1187: Failed to source defaults.vim"));
#إذا defined(FEAT_TERMINAL)
EXTERN حرف e_cannot_open_terminal_from_command_سطر_پنجره[]
	INIT(= N_("E1188: Cannot open a terminal from the command line window"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_use_legacy_with_command_str[]
	INIT(= N_("E1189: Cannot use :legacy with this command: %s"));

PLURAL_پیام(e_one_آرگومان_too_few, "E1190: One argument too few",
		e_nr_arguments_too_few, "E1190: %d arguments too few")

EXTERN حرف e_call_مقصد_function_that_failed_مقصد_compile_str[]
	INIT(= N_("E1191: Call to function that failed to compile: %s"));
EXTERN حرف e_خالی_function_name[]
	INIT(= N_("E1192: Empty function name"));
#پایان_اگر
// libsodium
#اگر_تعریف_شده FEAT_CRYPT
# ifndef FEAT_SODIUM
EXTERN حرف e_libsodium_not_built_درون[]
	INIT(= N_("E1193: cryptmethod xchacha20 not built into this Vim"));
# وإلا
#  إذا 0
EXTERN حرف e_libsodium_cannot_encrypt_سرآیند[]
	INIT(= N_("E1194: Cannot encrypt header, not enough space"));
EXTERN حرف e_libsodium_cannot_encrypt_میانگیر[]
	INIT(= N_("E1195: Cannot encrypt buffer, not enough space"));
EXTERN حرف e_libsodium_cannot_decrypt_سرآیند[]
	INIT(= N_("E1196: Cannot decrypt header, not enough space"));
#  endif
EXTERN حرف e_libsodium_cannot_allocate_میانگیر[]
	INIT(= N_("E1197: Cannot allocate_buffer for encryption"));
EXTERN حرف e_libsodium_decryption_failed_سرآیند_incomplete[]
	INIT(= N_("E1198: Decryption failed: Header incomplete!"));
#  إذا 0
EXTERN حرف e_libsodium_cannot_decrypt_میانگیر[]
	INIT(= N_("E1199: Cannot decrypt buffer, not enough space"));
#  endif
EXTERN حرف e_libsodium_decryption_failed[]
	INIT(= N_("E1200: Decryption failed!"));
EXTERN حرف e_libsodium_decryption_failed_premature[]
	INIT(= N_("E1201: Decryption failed: pre-mature end of file!"));
# endif
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_no_white_space_allowed_after_str_str[]
	INIT(= N_("E1202: No white space allowed after '%s': %s"));
EXTERN حرف e_dot_not_allowed_after_str_str[]
	INIT(= N_("E1203: Dot not allowed after a %s: %s"));
#پایان_اگر
EXTERN حرف e_regexp_عدد_after_dot_pos_جستجو_chr[]
	INIT(= N_("E1204: No Number allowed after .: '\\%%%c'"));
EXTERN حرف e_no_white_space_allowed_between_option_and[]
	INIT(= N_("E1205: No white space allowed between option and"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_dict_required_for_آرگومان_nr[]
	INIT(= N_("E1206: Dictionary required for argument %d"));
EXTERN حرف e_عبارت_without_effect_str[]
	INIT(= N_("E1207: Expression without an effect: %s"));
#پایان_اگر
EXTERN حرف e_complete_استفاده_شده_without_allowing_arguments[]
	INIT(= N_("E1208: -complete used without allowing arguments"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_invalid_مقدار_for_سطر_عدد_str[]
	INIT(= N_("E1209: Invalid value for a line number: \"%s\""));
EXTERN حرف e_عدد_required_for_آرگومان_nr[]
	INIT(= N_("E1210: Number required for argument %d"));
EXTERN حرف e_list_required_for_آرگومان_nr[]
	INIT(= N_("E1211: List required for argument %d"));
EXTERN حرف e_bool_required_for_آرگومان_nr[]
	INIT(= N_("E1212: Bool required for argument %d"));
EXTERN حرف e_redefining_imported_قلم_str[]
	INIT(= N_("E1213: Redefining imported item \"%s\""));
#پایان_اگر
#إذا defined(FEAT_DIGRAPHS)
EXTERN حرف e_digraph_must_be_just_two_characters_str[]
	INIT(= N_("E1214: Digraph must be just two characters: %s"));
EXTERN حرف e_digraph_آرگومان_must_be_one_character_str[]
	INIT(= N_("E1215: Digraph must be one character: %s"));
EXTERN حرف e_digraph_setlist_آرگومان_must_be_list_از_lists_with_two_اقلام[]
	INIT(= N_("E1216: digraph_setlist() argument must be a list of lists with two items"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
# ifdef FEAT_کار_CHANNEL
EXTERN حرف e_chan_or_کار_required_for_آرگومان_nr[]
	INIT(= N_("E1217: Channel or Job required for argument %d"));
EXTERN حرف e_کار_required_for_آرگومان_nr[]
	INIT(= N_("E1218: Job required for argument %d"));
# endif
EXTERN حرف e_float_or_عدد_required_for_آرگومان_nr[]
	INIT(= N_("E1219: Float or Number required for argument %d"));
EXTERN حرف e_string_or_عدد_required_for_آرگومان_nr[]
	INIT(= N_("E1220: String or Number required for argument %d"));
EXTERN حرف e_string_or_blob_required_for_آرگومان_nr[]
	INIT(= N_("E1221: String or Blob required for argument %d"));
EXTERN حرف e_string_or_list_required_for_آرگومان_nr[]
	INIT(= N_("E1222: String or List required for argument %d"));
EXTERN حرف e_string_or_dict_required_for_آرگومان_nr[]
	INIT(= N_("E1223: String or Dictionary required for argument %d"));
EXTERN حرف e_string_عدد_or_list_required_for_آرگومان_nr[]
	INIT(= N_("E1224: String, Number or List required for argument %d"));
EXTERN حرف e_string_list_tuple_or_dict_required_for_آرگومان_nr[]
	INIT(= N_("E1225: String, List, Tuple or Dictionary required for argument %d"));
EXTERN حرف e_list_or_blob_required_for_آرگومان_nr[]
	INIT(= N_("E1226: List or Blob required for argument %d"));
EXTERN حرف e_list_or_dict_required_for_آرگومان_nr[]
	INIT(= N_("E1227: List or Dictionary required for argument %d"));
EXTERN حرف e_list_dict_or_blob_required_for_آرگومان_nr[]
	INIT(= N_("E1228: List, Dictionary or Blob required for argument %d"));
EXTERN حرف e_expected_dictionary_for_using_کلید_str_but_got_str[]
	INIT(= N_("E1229: Expected dictionary for using key \"%s\", but got %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SODIUM
EXTERN حرف e_encryption_sodium_mlock_failed[]
	INIT(= N_("E1230: Encryption: sodium_mlock() failed"));
#پایان_اگر
EXTERN حرف e_cannot_use_bar_مقصد_separate_commands_اینجا_str[]
	INIT(= N_("E1231: Cannot use a bar to separate commands here: %s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_آرگومان_از_exists_compiled_must_be_literal_string[]
	INIT(= N_("E1232: Argument of exists_compiled() must be a literal string"));
EXTERN حرف e_exists_compiled_can_only_be_استفاده_شده_درون_def_function[]
	INIT(= N_("E1233: exists_compiled() can only be used in a :def function"));
#پایان_اگر
EXTERN حرف e_legacy_must_be_followed_by_command[]
	INIT(= N_("E1234: legacy must be followed by a command"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_bool_or_عدد_required_for_آرگومان_nr[]
	INIT(= N_("E1235: Bool or Number required for argument %d"));
EXTERN حرف e_cannot_use_str_itself_it_is_imported[]
	INIT(= N_("E1236: Cannot use %s itself, it is imported"));
#پایان_اگر
EXTERN حرف e_no_such_user_defined_command_درون_current_میانگیر_str[]
	INIT(= N_("E1237: No such user-defined command in current buffer: %s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_blob_required_for_آرگومان_nr[]
	INIT(= N_("E1238: Blob required for argument %d"));
EXTERN حرف e_invalid_مقدار_for_blob_nr[]
	INIT(= N_("E1239: Invalid value for blob: 0x%lX"));
#پایان_اگر
EXTERN حرف e_resulting_متن_too_long[]
	INIT(= N_("E1240: Resulting text too long"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_separator_not_supported_str[]
	INIT(= N_("E1241: Separator not supported: %s"));
EXTERN حرف e_no_white_space_allowed_before_separator_str[]
	INIT(= N_("E1242: No white space allowed before separator: %s"));
#پایان_اگر
#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
EXTERN حرف e_ascii_کد_not_درون_range[]
	INIT(= N_("E1243: ASCII code not in 32-127 range"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
# إذا defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
EXTERN حرف e_bad_color_string_str[]
	INIT(= N_("E1244: Bad color string: %s"));
# endif
EXTERN حرف e_cannot_expand_sfile_درون_vim9_function[]
	INIT(= N_("E1245: Cannot expand <sfile> in a Vim9 function"));
EXTERN حرف e_cannot_پیدا_کن_variable_مقصد_unlock_str[]
	INIT(= N_("E1246: Cannot find variable to (un)lock: %s"));
#پایان_اگر
EXTERN حرف e_سطر_عدد_بیرون_از_range[]
	INIT(= N_("E1247: Line number out of range"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_بستگی_called_from_invalid_context[]
	INIT(= N_("E1248: Closure called from invalid context"));
#پایان_اگر
EXTERN حرف e_highlight_گروه_name_too_long[]
	INIT(= N_("E1249: Highlight group name too long"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_آرگومان_از_str_must_be_list_string_dictionary_or_blob[]
	INIT(= N_("E1250: Argument of %s must be a List, String, Dictionary or Blob"));
EXTERN حرف e_list_tuple_dict_blob_or_string_required_for_آرگومان_nr[]
	INIT(= N_("E1251: List, Tuple, Dictionary, Blob or String required for argument %d"));
EXTERN حرف e_string_list_or_blob_required_for_آرگومان_nr[]
	INIT(= N_("E1252: String, List or Blob required for argument %d"));
EXTERN حرف e_string_list_tuple_or_blob_required_for_آرگومان_nr[]
	INIT(= N_("E1253: String, List, Tuple or Blob required for argument %d"));
EXTERN حرف e_cannot_use_script_variable_درون_for_loop[]
	INIT(= N_("E1254: Cannot use script variable in for loop"));
#پایان_اگر
EXTERN حرف e_cmd_mapping_must_انتها_with_cr[]
	INIT(= N_("E1255: <Cmd> mapping must end with <CR>"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_string_or_function_required_for_آرگومان_nr[]
	INIT(= N_("E1256: String or function required for argument %d"));
EXTERN حرف e_imported_script_must_use_as_or_انتها_درون_dot_vim_str[]
	INIT(= N_("E1257: Imported script must use \"as\" or end in .vim: %s"));
EXTERN حرف e_no_dot_after_imported_name_str[]
	INIT(= N_("E1258: No '.' after imported name: %s"));
EXTERN حرف e_missing_name_after_imported_name_str[]
	INIT(= N_("E1259: Missing name after imported name: %s"));
EXTERN حرف e_cannot_unlet_imported_قلم_str[]
	INIT(= N_("E1260: Cannot unlet an imported item: %s"));
EXTERN حرف e_cannot_import_dot_vim_without_using_as[]
	INIT(= N_("E1261: Cannot import .vim without using \"as\""));
EXTERN حرف e_cannot_import_same_script_twice_str[]
	INIT(= N_("E1262: Cannot import the same script twice: %s"));
EXTERN حرف e_cannot_use_name_with_hash_درون_vim9_script_use_export_instead[]
	INIT(= N_("E1263: Cannot use name with # in Vim9 script, use export instead"));
EXTERN حرف e_autoload_import_cannot_use_absolute_or_relative_مسیر[]
	INIT(= N_("E1264: Autoload import cannot use absolute or relative path: %s"));
EXTERN حرف e_cannot_use_partial_اینجا[]
	INIT(= N_("E1265: Cannot use a partial here"));
#پایان_اگر
#إذا defined(FEAT_PYTHON3) && defined(MSWIN)
EXTERN حرف e_critical_error_درون_python3_initialization_check_your_installation[]
	INIT(= N_("E1266: Critical error in python3 initialization, check your python3 installation"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_function_name_must_شروع_with_capital_str[]
	INIT(= N_("E1267: Function name must start with a capital: %s"));
EXTERN حرف e_cannot_use_s_colon_درون_vim9_script_str[]
	INIT(= N_("E1268: Cannot use s: in Vim9 script: %s"));
EXTERN حرف e_cannot_create_vim9_script_variable_درون_function_str[]
	INIT(= N_("E1269: Cannot create a Vim9 script variable in a function: %s"));
#پایان_اگر
EXTERN حرف e_cannot_use_s_backslash_درون_vim9_script[]
	INIT(= N_("E1270: Cannot use :s\\/sub/ in Vim9 script"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_compiling_بستگی_without_context_str[]
	INIT(= N_("E1271: Compiling closure without context: %s"));
EXTERN حرف e_using_type_not_درون_script_context_str[]
	INIT(= N_("E1272: Using type not in a script context: %s"));
#پایان_اگر
EXTERN حرف e_nfa_regexp_missing_مقدار_درون_chr[]
	INIT(= N_("E1273: (NFA regexp) missing value in '\\%%%c'"));
EXTERN حرف e_no_script_file_name_مقصد_substitute_for_script[]
	INIT(= N_("E1274: No script file name to substitute for \"<script>\""));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_string_or_function_required_for_arrow_parens_عبارت[]
	INIT(= N_("E1275: String or function required for ->(expr)"));
EXTERN حرف e_illegal_map_نما_string_str[]
	INIT(= N_("E1276: Illegal map mode string: '%s'"));
# إذا !defined(FEAT_کار_CHANNEL)
EXTERN حرف e_channel_کار_feature_not_موجود[]
	INIT(= N_("E1277: Channel and job feature is not available"));
# endif
EXTERN حرف e_stray_closing_curly_str[]
	INIT(= N_("E1278: Stray '}' without a matching '{': %s"));
EXTERN حرف e_missing_close_curly_str[]
	INIT(= N_("E1279: Missing '}': %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SPELL
EXTERN حرف e_illegal_character_درون_کلمه[]
	INIT(= N_("E1280: Illegal character in word"));
#پایان_اگر
EXTERN حرف e_atom_engine_must_be_at_شروع_از_الگو[]
	INIT(= N_("E1281: Atom '\\%%#=%c' must be at the start of the pattern"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_bitshift_ops_must_be_عدد[]
	INIT(= N_("E1282: Bitshift operands must be numbers"));
EXTERN حرف e_bitshift_ops_must_be_مثبت[]
	INIT(= N_("E1283: Bitshift amount must be a positive number"));
#پایان_اگر
#إذا defined(FEAT_PROP_POPUP)
EXTERN حرف e_آرگومان_1_list_قلم_nr_dictionary_required[]
	INIT(= N_("E1284: Argument 1, list item %d: Dictionary required"));
#پایان_اگر
#اگر_تعریف_شده FEAT_RELTIME
EXTERN حرف e_could_not_clear_timeout_str[]
	INIT(= N_("E1285: Could not clear timeout: %s"));
EXTERN حرف e_could_not_مجموعه_timeout_str[]
	INIT(= N_("E1286: Could not set timeout: %s"));
# ifndef PROF_NSEC
EXTERN حرف e_could_not_مجموعه_handler_for_timeout_str[]
	INIT(= N_("E1287: Could not set handler for timeout: %s"));
EXTERN حرف e_could_not_reset_handler_for_timeout_str[]
	INIT(= N_("E1288: Could not reset handler for timeout: %s"));
EXTERN حرف e_could_not_check_for_pending_sigalrm_str[]
	INIT(= N_("E1289: Could not check for pending SIGALRM: %s"));
# endif
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_substitute_nesting_too_deep[]
	INIT(= N_("E1290: substitute nesting too deep"));
# ifdef MSWIN
EXTERN حرف e_invalid_آرگومان_nr[]
	INIT(= N_("E1291: Invalid argument: %ld"));
# endif
#پایان_اگر
EXTERN حرف e_cmdline_پنجره_already_open[]
	INIT(= N_("E1292: Command-line window is already open"));
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_cannot_use_negative_id[]
	INIT(= N_("E1293: Cannot use a negative id for a text property"));
EXTERN حرف e_can_only_use_متن_align_when_ستونی_is_zero[]
	INIT(= N_("E1294: Can only use text_align when column is zero"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_cannot_specify_both_type_and_types[]
	INIT(= N_("E1295: Cannot specify both 'type' and 'types'"));
EXTERN حرف e_can_only_use_چپ_padding_when_ستونی_is_zero[]
	INIT(= N_("E1296: Can only use left padding when column is zero"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_non_null_dict_required_for_آرگومان_nr[]
	INIT(= N_("E1297: Non-NULL Dictionary required for argument %d"));
EXTERN حرف e_non_null_list_required_for_آرگومان_nr[]
	INIT(= N_("E1298: Non-NULL List required for argument %d"));
#پایان_اگر
EXTERN حرف e_پنجره_unexpectedly_close_while_searching_for_برچسبها[]
	INIT(= N_("E1299: Window unexpectedly closed while searching for tags"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_use_partial_with_dictionary_for_defer[]
	INIT(= N_("E1300: Cannot use a partial with dictionary for :defer"));
EXTERN حرف e_repeatable_type_required_for_آرگومان_nr[]
	INIT(= N_("E1301: String, Number, List, Tuple or Blob required for argument %d"));
EXTERN حرف e_script_variable_was_deleted[]
	INIT(= N_("E1302: Script variable was deleted"));
EXTERN حرف e_custom_list_completion_function_does_not_return_list_but_str[]
	INIT(= N_("E1303: Custom list completion function does not return a List but a %s"));
EXTERN حرف e_cannot_use_type_with_this_variable_str[]
	INIT(= N_("E1304: Cannot use type with this variable: %s"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_cannot_use_درازی_endcol_and_endlnum_with_متن[]
	INIT(= N_("E1305: Cannot use \"length\", \"end_col\" and \"end_lnum\" with \"text\""));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_loop_nesting_too_deep[]
	INIT(= N_("E1306: Loop nesting too deep"));
EXTERN حرف e_آرگومان_nr_trying_مقصد_modify_const_str[]
	INIT(= N_("E1307: Argument %d: Trying to modify a const %s"));
EXTERN حرف e_cannot_resize_پنجره_درون_another_tab_صفحه[]
	INIT(= N_("E1308: Cannot resize a window in another tab page"));
#پایان_اگر
EXTERN حرف e_cannot_change_mappings_while_listing[]
	INIT(= N_("E1309: Cannot change mappings while listing"));
#إذا defined(FEAT_MENU)
EXTERN حرف e_cannot_change_menus_while_listing[]
	INIT(= N_("E1310: Cannot change menus while listing"));
#پایان_اگر
EXTERN حرف e_cannot_change_user_commands_while_listing[]
	INIT(= N_("E1311: Cannot change user commands while listing"));
EXTERN حرف e_not_allowed_مقصد_change_پنجره_layout_درون_this_autocmd[]
	INIT(= N_("E1312: Not allowed to change the window layout in this autocmd"));
EXTERN حرف e_not_allowed_مقصد_جمع_کن_or_remove_entries_str[]
	INIT(= N_("E1313: Not allowed to add or remove entries (%s)"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_class_name_must_شروع_with_uppercase_letter_str[]
	INIT(= N_("E1314: Class name must start with an uppercase letter: %s"));
EXTERN حرف e_white_space_required_after_name_str[]
	INIT(= N_("E1315: White space required after name: %s"));
EXTERN حرف e_class_can_only_be_defined_درون_vim9_script[]
	INIT(= N_("E1316: Class can only be defined in Vim9 script"));
EXTERN حرف e_invalid_object_variable_declaration_str[]
	INIT(= N_("E1317: Invalid object variable declaration: %s"));
EXTERN حرف e_not_valid_command_درون_class_str[]
	INIT(= N_("E1318: Not a valid command in a class: %s"));
// E1319 unused
EXTERN حرف e_using_object_as_عدد[]
	INIT(= N_("E1320: Using an Object as a Number"));
// E1321 unused
EXTERN حرف e_using_object_as_float[]
	INIT(= N_("E1322: Using an Object as a Float"));
// E1323 unused
EXTERN حرف e_using_object_as_string[]
	INIT(= N_("E1324: Using an Object as a String"));
EXTERN حرف e_روش_not_یافت_on_class_str_str[]
	INIT(= N_("E1325: Method \"%s\" not found in class \"%s\""));
EXTERN حرف e_variable_not_یافت_on_object_str_str[]
	INIT(= N_("E1326: Variable \"%s\" not found in object \"%s\""));
EXTERN حرف e_object_required_یافت_str[]
	INIT(= N_("E1327: Object required, found %s"));
EXTERN حرف e_constructor_default_مقدار_must_be_vnone_str[]
	INIT(= N_("E1328: Constructor default value must be v:none: %s"));
EXTERN حرف e_invalid_class_variable_declaration_str[]
	INIT(= N_("E1329: Invalid class variable declaration: %s"));
EXTERN حرف e_invalid_type_درون_variable_declaration_str[]
	INIT(= N_("E1330: Invalid type used in variable declaration: %s"));
EXTERN حرف e_عمومی_must_be_followed_by_var_static_final_or_const[]
	INIT(= N_("E1331: public must be followed by \"var\" or \"static\" or \"final\" or \"const\""));
EXTERN حرف e_عمومی_variable_name_cannot_شروع_with_underscore_str[]
	INIT(= N_("E1332: public variable name cannot start with underscore: %s"));
EXTERN حرف e_cannot_access_protected_variable_str[]
	INIT(= N_("E1333: Cannot access protected variable \"%s\" in class \"%s\""));
// E1334 unused
EXTERN حرف e_variable_is_not_writable_str[]
	INIT(= N_("E1335: Variable \"%s\" in class \"%s\" is not writable"));
#پایان_اگر
EXTERN حرف e_داخلی_error_shortmess_too_long[]
	INIT(= "E1336: Internal error: shortmess too long");
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_class_variable_str_not_یافت_درون_class_str[]
	INIT(= N_("E1337: Class variable \"%s\" not found in class \"%s\""));
#پایان_اگر
// E1338 and E1339 unused
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_آرگومان_already_declared_درون_class_str[]
	INIT(= N_("E1340: Argument already declared in the class: %s"));
EXTERN حرف e_variable_already_declared_درون_class_str[]
	INIT(= N_("E1341: Variable already declared in the class: %s"));
EXTERN حرف e_interface_can_only_be_defined_درون_vim9_script[]
	INIT(= N_("E1342: Interface can only be defined in Vim9 script"));
EXTERN حرف e_interface_name_must_شروع_with_uppercase_letter_str[]
	INIT(= N_("E1343: Interface name must start with an uppercase letter: %s"));
EXTERN حرف e_cannot_initialize_variable_درون_interface[]
	INIT(= N_("E1344: Cannot initialize a variable in an interface"));
EXTERN حرف e_not_valid_command_درون_interface_str[]
	INIT(= N_("E1345: Not a valid command in an interface: %s"));
EXTERN حرف e_interface_name_not_یافت_str[]
	INIT(= N_("E1346: Interface name not found: %s"));
EXTERN حرف e_not_valid_interface_str[]
	INIT(= N_("E1347: Not a valid interface: %s"));
EXTERN حرف e_variable_str_از_interface_str_not_implemented[]
	INIT(= N_("E1348: Variable \"%s\" of interface \"%s\" is not implemented"));
EXTERN حرف e_روش_str_از_interface_str_not_implemented[]
	INIT(= N_("E1349: Method \"%s\" of interface \"%s\" is not implemented"));
EXTERN حرف e_duplicate_implements[]
	INIT(= N_("E1350: Duplicate \"implements\""));
EXTERN حرف e_duplicate_interface_after_implements_str[]
	INIT(= N_("E1351: Duplicate interface after \"implements\": %s"));
EXTERN حرف e_duplicate_extends[]
	INIT(= N_("E1352: Duplicate \"extends\""));
EXTERN حرف e_class_name_not_یافت_str[]
	INIT(= N_("E1353: Class name not found: %s"));
EXTERN حرف e_cannot_extend_str[]
	INIT(= N_("E1354: Cannot extend %s"));
EXTERN حرف e_duplicate_function_str[]
	INIT(= N_("E1355: Duplicate function: %s"));
EXTERN حرف e_super_must_be_followed_by_dot[]
	INIT(= N_("E1356: \"super\" must be followed by a dot"));
EXTERN حرف e_using_super_not_درون_class_روش[]
	INIT(= N_("E1357: Using \"super\" not in a class method"));
EXTERN حرف e_using_super_not_درون_فرزند_class[]
	INIT(= N_("E1358: Using \"super\" not in a child class"));
EXTERN حرف e_cannot_define_new_روش_درون_abstract_class[]
	INIT(= N_("E1359: Cannot define a \"new\" method in an abstract class"));
EXTERN حرف e_using_null_object[]
	INIT(= N_("E1360: Using a null object"));
#پایان_اگر
EXTERN حرف e_cannot_use_color_none_did_you_میانگین_none[]
	INIT(= N_("E1361: Cannot use color \"none\", did you mean \"NONE\"?"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_use_non_null_object[]
	INIT(= N_("E1362: Cannot use a non-null object"));
EXTERN حرف e_incomplete_type[]
	INIT(= N_("E1363: Incomplete type"));
#پایان_اگر
EXTERN حرف e_warning_اشاره_گر_بلوک_corrupted[]
	INIT(= N_("E1364: Warning: Pointer block corrupted"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_use_a_return_type_with_new_روش[]
	INIT(= N_("E1365: Cannot use a return type with the \"new\" method"));
EXTERN حرف e_cannot_access_protected_روش_str[]
	INIT(= N_("E1366: Cannot access protected method: %s"));
EXTERN حرف e_variable_str_از_interface_str_has_different_access[]
	INIT(= N_("E1367: Access level of variable \"%s\" of interface \"%s\" is different"));
EXTERN حرف e_static_must_be_followed_by_var_def_final_or_const[]
	INIT(= N_("E1368: Static must be followed by \"var\" or \"def\" or \"final\" or \"const\""));
EXTERN حرف e_duplicate_variable_str[]
	INIT(= N_("E1369: Duplicate variable: %s"));
EXTERN حرف e_cannot_define_new_روش_as_static[]
	INIT(= N_("E1370: Cannot define a \"new\" method as static"));
EXTERN حرف e_abstract_must_be_followed_by_def[]
	INIT(= N_("E1371: Abstract must be followed by \"def\""));
EXTERN حرف e_abstract_روش_درون_concrete_class[]
	INIT(= N_("E1372: Abstract method \"%s\" cannot be defined in a concrete class"));
EXTERN حرف e_abstract_روش_str_not_implemented[]
	INIT(= N_("E1373: Abstract method \"%s\" is not implemented"));
EXTERN حرف e_class_variable_str_accessible_only_inside_class_str[]
	INIT(= N_("E1374: Class variable \"%s\" accessible only inside class \"%s\""));
EXTERN حرف e_class_variable_str_accessible_only_using_class_str[]
	INIT(= N_("E1375: Class variable \"%s\" accessible only using class \"%s\""));
EXTERN حرف e_object_variable_str_accessible_only_using_object_str[]
	INIT(= N_("E1376: Object variable \"%s\" accessible only using class \"%s\" object"));
EXTERN حرف e_روش_str_از_class_str_has_different_access[]
	INIT(= N_("E1377: Access level of method \"%s\" is different in class \"%s\""));
EXTERN حرف e_static_member_not_supported_درون_interface[]
	INIT(= N_("E1378: Static member not supported in an interface"));
EXTERN حرف e_protected_variable_not_supported_درون_interface[]
	INIT(= N_("E1379: Protected variable not supported in an interface"));
EXTERN حرف e_protected_روش_not_supported_درون_interface[]
	INIT(= N_("E1380: Protected method not supported in an interface"));
EXTERN حرف e_interface_cannot_use_implements[]
	INIT(= N_("E1381: Interface cannot use \"implements\""));
EXTERN حرف e_variable_str_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1382: Variable \"%s\": type mismatch, expected %s but got %s"));
EXTERN حرف e_روش_str_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1383: Method \"%s\": type mismatch, expected %s but got %s"));
EXTERN حرف e_class_روش_str_accessible_only_inside_class_str[]
	INIT(= N_("E1384: Class method \"%s\" accessible only inside class \"%s\""));
EXTERN حرف e_class_روش_str_accessible_only_using_class_str[]
	INIT(= N_("E1385: Class method \"%s\" accessible only using class \"%s\""));
EXTERN حرف e_object_روش_str_accessible_only_using_object_str[]
	INIT(= N_("E1386: Object method \"%s\" accessible only using class \"%s\" object"));
EXTERN حرف e_عمومی_variable_not_supported_درون_interface[]
	INIT(= N_("E1387: public variable not supported in an interface"));
EXTERN حرف e_عمومی_keyword_not_supported_for_روش[]
	INIT(= N_("E1388: public keyword not supported for a method"));
EXTERN حرف e_missing_name_after_implements[]
	INIT(= N_("E1389: Missing name after implements"));
EXTERN حرف e_cannot_use_an_object_variable_except_with_the_new_روش_str[]
	INIT(= N_("E1390: Cannot use an object variable \"this.%s\" except with the \"new\" method"));
EXTERN حرف e_cannot_قفل_object_variable_str[]
	INIT(= N_("E1391: Cannot (un)lock variable \"%s\" in class \"%s\""));
EXTERN حرف e_cannot_قفل_class_variable_str[]
	INIT(= N_("E1392: Cannot (un)lock class variable \"%s\" in class \"%s\""));
EXTERN حرف e_type_can_only_be_defined_درون_vim9_script[]
	INIT(= N_("E1393: Type can only be defined in Vim9 script"));
EXTERN حرف e_type_name_must_شروع_with_uppercase_letter_str[]
	INIT(= N_("E1394: Type name must start with an uppercase letter: %s"));
EXTERN حرف e_using_null_class[]
	INIT(= N_("E1395: Using a null class"));
EXTERN حرف e_typealias_already_exists_for_str[]
	INIT(= N_("E1396: Type alias \"%s\" already exists"));
EXTERN حرف e_missing_typealias_name[]
	INIT(= N_("E1397: Missing type alias name"));
EXTERN حرف e_missing_typealias_type[]
	INIT(= N_("E1398: Missing type alias type"));
EXTERN حرف e_type_can_only_be_استفاده_شده_درون_script[]
	INIT(= N_("E1399: Type can only be used in a script"));
// E1400 unused
// E1401 unused
// E1402 unused
EXTERN حرف e_using_typealias_as_مقدار_str[]
	INIT(= N_("E1403: Type alias \"%s\" cannot be used as a value"));
EXTERN حرف e_abstract_cannot_be_استفاده_شده_درون_interface[]
	INIT(= N_("E1404: Abstract cannot be used in an interface"));
EXTERN حرف e_using_class_as_مقدار_str[]
	INIT(= N_("E1405: Class \"%s\" cannot be used as a value"));
EXTERN حرف e_عمومی_and_protected_member_have_same_name_str_str[]
	INIT(= N_("E1406: Public and protected member have the same name: %s and _%s"));
EXTERN حرف e_using_typealias_as_var_val[]
	INIT(= N_("E1407: Cannot use a Typealias as a variable or value"));
EXTERN حرف e_final_variable_not_supported_درون_interface[]
	INIT(= N_("E1408: Final variable not supported in an interface"));
EXTERN حرف e_cannot_change_readonly_variable_str_درون_class_str[]
	INIT(= N_("E1409: Cannot change read-only variable \"%s\" in class \"%s\""));
EXTERN حرف e_const_variable_not_supported_درون_interface[]
	INIT(= N_("E1410: Const variable not supported in an interface"));
EXTERN حرف e_missing_dot_after_object_str[]
	INIT(= N_("E1411: Missing dot after object \"%s\""));
EXTERN حرف e_builtin_object_روش_str_not_supported[]
	INIT(= N_("E1412: Builtin object method \"%s\" not supported"));
EXTERN حرف e_builtin_class_روش_not_supported[]
	INIT(= N_("E1413: Builtin class method not supported"));
EXTERN حرف e_enum_can_only_be_defined_درون_vim9_script[]
       INIT(= N_("E1414: Enum can only be defined in Vim9 script"));
EXTERN حرف e_enum_name_must_شروع_with_uppercase_letter_str[]
	INIT(= N_("E1415: Enum name must start with an uppercase letter: %s"));
EXTERN حرف e_enum_cannot_extend_class[]
	INIT(= N_("E1416: Enum cannot extend a class or enum"));
EXTERN حرف e_abstract_cannot_be_استفاده_شده_درون_enum[]
	INIT(= N_("E1417: Abstract cannot be used in an Enum"));
EXTERN حرف e_invalid_enum_مقدار_declaration_str[]
	INIT(= N_("E1418: Invalid enum value declaration: %s"));
EXTERN حرف e_not_valid_command_درون_enum_str[]
	INIT(= N_("E1419: Not a valid command in an Enum: %s"));
EXTERN حرف e_missing_endenum[]
	INIT(= N_("E1420: Missing :endenum"));
EXTERN حرف e_using_enum_as_مقدار_str[]
	INIT(= N_("E1421: Enum \"%s\" cannot be used as a value"));
EXTERN حرف e_enum_مقدار_str_not_یافت_درون_enum_str[]
	INIT(= N_("E1422: Enum value \"%s\" not found in enum \"%s\""));
EXTERN حرف e_enumvalue_str_cannot_be_modified[]
	INIT(= N_("E1423: Enum value \"%s.%s\" cannot be modified"));
EXTERN حرف e_using_enum_str_as_عدد[]
	INIT(= N_("E1424: Using an Enum \"%s\" as a Number"));
EXTERN حرف e_using_enum_str_as_string[]
	INIT(= N_("E1425: Using an Enum \"%s\" as a String"));
EXTERN حرف e_enum_str_ordinal_cannot_be_modified[]
	INIT(= N_("E1426: Enum \"%s\" ordinal value cannot be modified"));
EXTERN حرف e_enum_str_name_cannot_be_modified[]
	INIT(= N_("E1427: Enum \"%s\" name cannot be modified"));
EXTERN حرف e_duplicate_enum_str[]
	INIT(= N_("E1428: Duplicate enum value: %s"));
EXTERN حرف e_class_can_only_be_استفاده_شده_درون_script[]
	INIT(= N_("E1429: Class can only be used in a script"));
EXTERN حرف e_uninitialized_object_var_reference[]
	INIT(= N_("E1430: Uninitialized object variable '%s' referenced"));
EXTERN حرف e_abstract_روش_str_مستقیم[]
	INIT(= N_("E1431: Abstract method \"%s\" in class \"%s\" cannot be accessed directly"));
EXTERN حرف e_generic_روش_str_override_with_concrete_روش_درون_class_str[]
	INIT(= N_("E1432: Overriding generic method \"%s\" in class \"%s\" with a concrete method"));
EXTERN حرف e_concrete_روش_str_override_with_generic_روش_درون_class_str[]
	INIT(= N_("E1433: Overriding concrete method \"%s\" in class \"%s\" with a generic method"));
EXTERN حرف e_generic_روش_str_type_arguments_mismatch_درون_class_str[]
	INIT(= N_("E1434: Mismatched number of type variables for generic method  \"%s\" in class \"%s\""));
EXTERN حرف e_enum_can_only_be_استفاده_شده_درون_script[]
	INIT(= N_("E1435: Enum can only be used in a script"));
EXTERN حرف e_interface_can_only_be_استفاده_شده_درون_script[]
	INIT(= N_("E1436: Interface can only be used in a script"));
EXTERN حرف e_can_only_compare_object_with_object[]
	INIT(= N_("E1437: Can only compare Object with Object"));
EXTERN حرف e_type_var_شمار_از_روش_str_differs_from_interface_str[]
	INIT(= N_("E1438: Number of type variables of method \"%s\" differs from interface \"%s\""));
EXTERN حرف e_type_nested_too_deep[]
	INIT(= N_("E1439: Type nested too deep"));
#پایان_اگر
// E1440 - E1499 unused (reserved for Vim9 class support)
EXTERN حرف e_cannot_mix_positional_and_non_positional_str[]
	INIT(= N_("E1500: Cannot mix positional and non-positional arguments: %s"));
EXTERN حرف e_fmt_arg_nr_استفادهنشده_str[]
	INIT(= N_("E1501: format argument %d unused in $-style format: %s"));
EXTERN حرف e_positional_رقم_فیلد_spec_reused_str_str[]
	INIT(= N_("E1502: Positional argument %d used as field width reused as different type: %s/%s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_positional_nr_بیرون_از_bounds_str[]
	INIT(= N_("E1503: Positional argument %d out of bounds: %s"));
#پایان_اگر
EXTERN حرف e_positional_arg_رقم_type_inconsistent_str_str[]
	INIT(= N_("E1504: Positional argument %d type used inconsistently: %s/%s"));
EXTERN حرف e_invalid_format_specifier_str[]
	INIT(= N_("E1505: Invalid format specifier: %s"));
#اگر_تعریف_شده FEAT_XATTR
EXTERN حرف e_xattr_erange[]
	INIT(= N_("E1506: Buffer too small to copy xattr value or key"));
#پایان_اگر
EXTERN حرف e_aptypes_is_null_nr_str[]
	INIT(= "E1507: Internal error: ap_types or ap_types[idx] is NULL: %d: %s");
#اگر_تعریف_شده FEAT_XATTR
EXTERN حرف e_xattr_e2big[]
	INIT(= N_("E1508: Size of the extended attribute value is larger than the maximum size allowed"));
EXTERN حرف e_xattr_other[]
	INIT(= N_("E1509: Error occurred when reading or writing extended attribute"));
#پایان_اگر
EXTERN حرف e_val_too_large[]
	INIT(= N_("E1510: Value too large: %s"));
EXTERN حرف e_wrong_عدد_از_characters_for_فیلد_str[]
	INIT(= N_("E1511: Wrong number of characters for field \"%s\""));
EXTERN حرف e_wrong_character_عرض_for_فیلد_str[]
	INIT(= N_("E1512: Wrong character width for field \"%s\""));
EXTERN حرف e_winfixbuf_cannot_go_مقصد_میانگیر[]
	INIT(= N_("E1513: Cannot switch buffer. 'winfixbuf' is enabled"));
EXTERN حرف e_invalid_return_type_from_findfunc[]
	INIT(= N_("E1514: 'findfunc' did not return a List type"));
EXTERN حرف e_str_encoding_from_failed[]
	INIT(= N_("E1515: Unable to convert from '%s' encoding"));
EXTERN حرف e_str_encoding_مقصد_failed[]
	INIT(= N_("E1516: Unable to convert to '%s' encoding"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_can_only_compare_tuple_with_tuple[]
	INIT(= N_("E1517: Can only compare Tuple with Tuple"));
EXTERN حرف e_invalid_operation_for_tuple[]
	INIT(= N_("E1518: Invalid operation for Tuple"));
EXTERN حرف e_tuple_شاخص_بیرون_از_range_nr[]
	INIT(= N_("E1519: Tuple index out of range: %ld"));
EXTERN حرف e_using_tuple_as_عدد[]
	INIT(= N_("E1520: Using a Tuple as a Number"));
EXTERN حرف e_using_tuple_as_float[]
	INIT(= N_("E1521: Using a Tuple as a Float"));
EXTERN حرف e_using_tuple_as_string[]
	INIT(= N_("E1522: Using a Tuple as a String"));
EXTERN حرف e_string_list_tuple_or_blob_required[]
	INIT(= N_("E1523: String, List, Tuple or Blob required"));
EXTERN حرف e_cannot_use_tuple_with_function_str[]
	INIT(= N_("E1524: Cannot use a tuple with function %s"));
EXTERN حرف e_آرگومان_از_str_must_be_list_tuple_string_dictionary_or_blob[]
	INIT(= N_("E1525: Argument of %s must be a List, Tuple, String, Dictionary or Blob"));
EXTERN حرف e_missing_انتها_از_tuple_rsp_str[]
	INIT(= N_("E1526: Missing end of Tuple ')': %s"));
EXTERN حرف e_missing_comma_درون_tuple_str[]
	INIT(= N_("E1527: Missing comma in Tuple: %s"));
EXTERN حرف e_list_or_tuple_or_blob_required_for_آرگومان_nr[]
	INIT(= N_("E1528: List or Tuple or Blob required for argument %d"));
EXTERN حرف e_list_or_tuple_required_for_آرگومان_nr[]
	INIT(= N_("E1529: List or Tuple required for argument %d"));
EXTERN حرف e_list_or_tuple_or_dict_required_for_آرگومان_nr[]
	INIT(= N_("E1530: List or Tuple or Dictionary required for argument %d"));
EXTERN حرف e_آرگومان_از_str_must_be_list_tuple_dictionary_or_blob[]
	INIT(= N_("E1531: Argument of %s must be a List, Tuple, Dictionary or Blob"));
EXTERN حرف e_tuple_is_immutable[]
	INIT(= N_("E1532: Cannot modify a tuple"));
EXTERN حرف e_cannot_برش_tuple[]
	INIT(= N_("E1533: Cannot slice a tuple"));
EXTERN حرف e_tuple_required_for_آرگومان_nr[]
	INIT(= N_("E1534: Tuple required for argument %d"));
EXTERN حرف e_list_or_tuple_required[]
	INIT(= N_("E1535: List or Tuple required"));
EXTERN حرف e_tuple_required[]
	INIT(= N_("E1536: Tuple required"));
EXTERN حرف e_less_targets_than_tuple_اقلام[]
	INIT(= N_("E1537: Less targets than Tuple items"));
EXTERN حرف e_بیشتر_targets_than_tuple_اقلام[]
	INIT(= N_("E1538: More targets than Tuple items"));
EXTERN حرف e_variadic_tuple_must_انتها_with_list_type_str[]
	INIT(= N_("E1539: Variadic tuple must end with a list type: %s"));
EXTERN حرف e_cannot_use_variadic_tuple_درون_concatenation[]
	INIT(= N_("E1540: Cannot use a variadic tuple in concatenation"));
#پایان_اگر
EXTERN حرف e_unicode_val_too_large[]
	INIT(= N_("E1541: Value too large, max Unicode codepoint is U+10FFFF"));
#اگر_تعریف_شده FEAT_QUICKFIX
EXTERN حرف e_cannot_have_negative_or_zero_عدد_از_quickfix[]
	INIT(= N_("E1542: Cannot have a negative or zero number of quickfix/location lists"));
EXTERN حرف e_cannot_have_بیشتر_than_hundred_quickfix[]
	INIT(= N_("E1543: Cannot have more than a hundred quickfix/location lists"));
EXTERN حرف e_failed_resizing_quickfix_پشته[]
	INIT(= N_("E1544: Failed resizing the quickfix/location list stack"));
EXTERN حرف e_no_quickfix_پشته[]
	INIT(= N_("E1545: Quickfix list stack unavailable"));
#پایان_اگر
EXTERN حرف e_cannot_switch_مقصد_a_closing_میانگیر[]
	INIT(= N_("E1546: Cannot switch to a closing buffer"));
EXTERN حرف e_cannot_not_support_redrawtabpanel[]
	INIT(= N_("E1547: This version of Vim does not support :redrawtabpanel"));
#اگر_تعریف_شده FEAT_WAYLAND
EXTERN حرف e_wayland_connection_unavailable[]
	INIT(= N_("E1548: Wayland connection is unavailable"));
#پایان_اگر
#إذا defined(FEAT_DIFF)
EXTERN حرف e_cannot_have_بیشتر_than_nr_diff_anchors[]
	INIT(= N_("E1549: Cannot have more than %d diff anchors"));
EXTERN حرف e_failed_مقصد_پیدا_کن_all_diff_anchors[]
	INIT(= N_("E1550: Failed to find all diff anchors"));
#پایان_اگر
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_cannot_open_a_popup_پنجره_مقصد_a_closing_میانگیر[]
	INIT(= N_("E1551: Cannot open a popup window to a closing buffer"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_type_var_name_must_شروع_with_uppercase_letter_str[]
	INIT(= N_("E1552: Type variable name must start with an uppercase letter: %s"));
EXTERN حرف e_missing_comma_درون_generic_function_str[]
	INIT(= N_("E1553: Missing comma after type in generic function: %s"));
EXTERN حرف e_missing_closing_angle_bracket_درون_generic_function_str[]
	INIT(= N_("E1554: Missing '>' in generic function: %s"));
EXTERN حرف e_خالی_type_list_for_generic_function_str[]
	INIT(= N_("E1555: Empty type list specified for generic function '%s'"));
EXTERN حرف e_too_many_types_for_generic_function_str[]
	INIT(= N_("E1556: Too many types specified for generic function '%s'"));
EXTERN حرف e_not_enough_types_for_generic_function_str[]
	INIT(= N_("E1557: Not enough types specified for generic function '%s'"));
EXTERN حرف e_unknown_generic_function_str[]
	INIT(= N_("E1558: Unknown generic function: %s"));
EXTERN حرف e_generic_func_missing_type_آرگومانها_str[]
	INIT(= N_("E1559: Type arguments missing for generic function '%s'"));
EXTERN حرف e_not_a_generic_function_str[]
	INIT(= N_("E1560: Not a generic function: %s"));
EXTERN حرف e_duplicate_type_var_name_str[]
	INIT(= N_("E1561: Duplicate type variable name: %s"));
#پایان_اگر
#إذا defined(FEAT_DIFF)
EXTERN حرف e_diff_anchors_with_hidden_windows[]
	INIT(= N_("E1562: Diff anchors cannot be used with hidden diff windows"));
#پایان_اگر
#اگر_تعریف_شده FEAT_SOCKETSERVER
EXTERN حرف e_socket_name_no_slashes[]
	INIT(= N_("E1564: Socket name '%s' cannot have slashes in it without being a path"));
EXTERN حرف e_socket_server_not_online[]
	INIT(= N_("E1565: Socket server is not online, call remote_startserver() first"));
EXTERN حرف e_socket_server_failed_connecting[]
	INIT(= N_("E1566: Failed connecting to socket '%s'"));
EXTERN حرف e_socket_server_نسخه_mismatch[]
	INIT(= N_("E1567: Socket server protocol version mismatch, check what Vim version you are using"));
#پایان_اگر
EXTERN حرف e_osc_response_timed_بیرون[]
	INIT(= N_("E1568: OSC command response timed out: %.*s"));
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_جمع_کن_listener_درون_listener_callback[]
	INIT(= N_("E1569: Cannot use listener_add in a listener callback"));
EXTERN حرف e_cannot_جمع_کن_redraw_listener_درون_listener_callback[]
	INIT(= N_("E1570: Cannot use redraw_listener_add in a redraw listener callback"));
EXTERN حرف e_no_redraw_listener_callbacks_defined[]
	INIT(= N_("E1571: Must specify at least one callback for redraw_listener_add"));
#پایان_اگر
EXTERN حرف e_leadtab_requires_tab[]
	INIT(= N_("E1572: 'listchars' field \"leadtab\" requires \"tab\" to be specified"));
#اگر_تعریف_شده FEAT_کار_CHANNEL
EXTERN حرف e_cannot_listen_on_پورت[]
	INIT(= N_("E1573: Cannot listen on port"));
EXTERN حرف e_gethostbyname_درون_channel_listen[]
	INIT(= N_("E1574: gethostbyname(): cannot resolve hostname in channel_listen()"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_create_pipes[]
	INIT(= N_("E1575: Cannot create pipes"));
#پایان_اگر
EXTERN حرف e_برچسب_file_مدخل_must_not_be_url[]
	INIT(= N_("E1576: Tag file entry must not be a URL"));
EXTERN حرف e_invalid_format_string_single_percent_s[]
	INIT(= N_("E1577: Invalid format string, only one \"%s\" is allowed"));
#اگر_تعریف_شده FEAT_SPELL
EXTERN حرف e_too_many_postponed_prefixes_spell[]
	INIT(= N_("E1578: Too many postponed prefixes and/or compound flags"));
#پایان_اگر
EXTERN حرف e_completeopt_escape_cannot_be_استفاده_شده_with_nargs_underscore[]
	INIT(= N_("E1579: -completeopt=escape cannot be used with -nargs=_"));
#اگر_تعریف_شده FEAT_PROP_POPUP
EXTERN حرف e_too_many_متن_properties_on_a_single_سطر[]
	INIT(= N_("E1580: Too many text properties on a single line"));
#پایان_اگر
#اگر_تعریف_شده FEAT_EVAL
EXTERN حرف e_cannot_extend_null_blob[]
	INIT(= N_("E1581: Cannot extend a null blob"));
#پایان_اگر
#اگر_تعریف_شده FEAT_کار_CHANNEL
EXTERN حرف e_not_started_with_stdio_channel[]
	INIT(= N_("E1582: Not started with --stdio-channel"));
EXTERN حرف e_cannot_open_stdio_channel[]
	INIT(= N_("E1583: Cannot open stdio channel"));
#پایان_اگر
#اگر_تعریف_شده FEAT_IMAGE
EXTERN حرف e_invalid_image_dimensions[]
	INIT(= N_("E1584: Image dimensions are invalid"));
EXTERN حرف e_no_image_backend_موجود[]
	INIT(= N_("E1585: No image backend available"));
EXTERN حرف e_changing_image_backend_failed[]
	INIT(= N_("E1586: Failed changing image backends"));
EXTERN حرف e_image_id_nr_does_not_exist[]
	INIT(= N_("E1587: Image ID %d does not exist"));
#پایان_اگر
