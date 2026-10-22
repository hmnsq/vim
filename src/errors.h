/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

// Use PLURAL_MSG() for messages that are passed to ngettext(), so that the
// second one uses msgid_plural.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DO_INIT
# define PLURAL_ಸಂದೇಶ(var1, msg1, var2, msg2) \
	ಅಕ್ಷರ var1[] = msg1; \
	ಅಕ್ಷರ var2[] = msg2;
#ಇಲ್ಲದಿದ್ದರೆ
# define PLURAL_ಸಂದೇಶ(var1, msg1, var2, msg2) \
	ಬಾಹ್ಯ ಅಕ್ಷರ var1[]; \
	ಬಾಹ್ಯ ಅಕ್ಷರ var2[];
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Definition of error messages, sorted on error number.
 */

EXTERN ಅಕ್ಷರ e_interrupted[]
	INIT(= N_("Interrupted"));

EXTERN ಅಕ್ಷರ e_backslash_should_be_followed_by[]
	INIT(= N_("E10: \\ should be followed by /, ? or &"));
EXTERN ಅಕ್ಷರ e_invalid_ಒಳ_cmdline_ಕಿಟಕಿ[]
	INIT(= N_("E11: Invalid in command-line window; :q<CR> closes the window"));
EXTERN ಅಕ್ಷರ e_command_not_allowed_from_vimrc_ಒಳ_current_dir_or_ಟ್ಯಾಗ್_ಹುಡುಕು[]
	INIT(= N_("E12: Command not allowed from exrc/vimrc in current dir or tag search"));
EXTERN ಅಕ್ಷರ e_file_exists[]
	INIT(= N_("E13: File exists (add ! to override)"));
// E14 unused
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str[]
	INIT(= N_("E15: Invalid expression: \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_invalid_range[]
	INIT(= N_("E16: Invalid range"));
#ಆದರೆ defined(UNIX) || defined(FEAT_SYN_HL) \
	    || defined(FEAT_SPELL) || defined(FEAT_EVAL)
EXTERN ಅಕ್ಷರ e_str_is_directory[]
	INIT(= N_("E17: \"%s\" is a directory"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_unexpected_characters_ಒಳ_let[]
	INIT(= N_("E18: Unexpected characters in :let"));
EXTERN ಅಕ್ಷರ e_unexpected_characters_ಒಳ_assignment[]
	INIT(= N_("E18: Unexpected characters in assignment"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_mark_has_invalid_ಸಾಲು_ಸಂಖ್ಯೆ[]
	INIT(= N_("E19: Mark has invalid line number"));
EXTERN ಅಕ್ಷರ e_mark_not_ಗಣ[]
	INIT(= N_("E20: Mark not set"));
EXTERN ಅಕ್ಷರ e_cannot_make_changes_modifiable_is_off[]
	INIT(= N_("E21: Cannot make changes, 'modifiable' is off"));
EXTERN ಅಕ್ಷರ e_scripts_nested_too_deep[]
	INIT(= N_("E22: Scripts nested too deep"));
EXTERN ಅಕ್ಷರ e_no_alternate_file[]
	INIT(= N_("E23: No alternate file"));
EXTERN ಅಕ್ಷರ e_no_such_abbreviation[]
	INIT(= N_("E24: No such abbreviation"));
#ಆದರೆ !defined(FEAT_GUI) || defined(VIMDLL)
EXTERN ಅಕ್ಷರ e_gui_cannot_be_ಬಳಸಿದ_not_enabled_at_compile_ಸಮಯ[]
	INIT(= N_("E25: GUI cannot be used: Not enabled at compile time"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_RIGHTLEFT
EXTERN ಅಕ್ಷರ e_hebrew_cannot_be_ಬಳಸಿದ_not_enabled_at_compile_ಸಮಯ[]
	INIT(= N_("E26: Hebrew cannot be used: Not enabled at compile time\n"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_farsi_support_has_been_removed[]
	INIT(= N_("E27: Farsi support has been removed\n"));
#ಆದರೆ defined(FEAT_ಹುಡುಕು_EXTRA) || defined(FEAT_SYN_HL)
EXTERN ಅಕ್ಷರ e_no_such_highlight_ಗುಂಪು_name_str[]
	INIT(= N_("E28: No such highlight group name: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_no_inserted_ಪಠ್ಯ_yet[]
	INIT(= N_("E29: No inserted text yet"));
EXTERN ಅಕ್ಷರ e_no_ಹಿಂದಿನ_command_ಸಾಲು[]
	INIT(= N_("E30: No previous command line"));
EXTERN ಅಕ್ಷರ e_no_such_mapping[]
	INIT(= N_("E31: No such mapping"));
EXTERN ಅಕ್ಷರ e_no_file_name[]
	INIT(= N_("E32: No file name"));
EXTERN ಅಕ್ಷರ e_no_ಹಿಂದಿನ_substitute_regular_ಅಭಿವ್ಯಕ್ತಿ[]
	INIT(= N_("E33: No previous substitute regular expression"));
EXTERN ಅಕ್ಷರ e_no_ಹಿಂದಿನ_command[]
	INIT(= N_("E34: No previous command"));
EXTERN ಅಕ್ಷರ e_no_ಹಿಂದಿನ_regular_ಅಭಿವ್ಯಕ್ತಿ[]
	INIT(= N_("E35: No previous regular expression"));
EXTERN ಅಕ್ಷರ e_not_enough_ಸ್ಥಳ[]
	INIT(= N_("E36: Not enough room"));
EXTERN ಅಕ್ಷರ e_no_write_since_ಕೊನೆಯ_change[]
	INIT(= N_("E37: No write since last change"));
EXTERN ಅಕ್ಷರ e_no_write_since_ಕೊನೆಯ_change_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override[]
	INIT(= N_("E37: No write since last change (add ! to override)"));
EXTERN ಅಕ್ಷರ e_null_ವಾದ[]
	INIT(= "E38: Null argument");
#ಆದರೆ defined(FEAT_DIGRAPHS) || defined(FEAT_TIMERS) || defined(FEAT_EVAL)
EXTERN ಅಕ್ಷರ e_ಸಂಖ್ಯೆ_expected[]
	INIT(= N_("E39: Number expected"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_cant_open_errorfile_str[]
	INIT(= N_("E40: Can't open errorfile %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ[]
	INIT(= N_("E41: Out of memory!"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_no_errors[]
	INIT(= N_("E42: No Errors"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_damaged_ಹೊಂದಿಕೆ_string[]
	INIT(= "E43: Damaged match string");
EXTERN ಅಕ್ಷರ e_corrupted_regexp_ಕಾರ್ಯಕ್ರಮ[]
	INIT(= "E44: Corrupted regexp program");
EXTERN ಅಕ್ಷರ e_readonly_option_is_ಗಣ_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override[]
	INIT(= N_("E45: 'readonly' option is set (add ! to override)"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_change_readonly_variable[]
	INIT(= N_("E46: Cannot change read-only variable"));
EXTERN ಅಕ್ಷರ e_cannot_change_readonly_variable_str[]
	INIT(= N_("E46: Cannot change read-only variable \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_error_while_reading_errorfile[]
	INIT(= N_("E47: Error while reading errorfile"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
EXTERN ಅಕ್ಷರ e_not_allowed_ಒಳ_sandbox[]
	INIT(= N_("E48: Not allowed in sandbox"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_invalid_scroll_ಗಾತ್ರ[]
	INIT(= N_("E49: Invalid scroll size"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಅಕ್ಷರ e_too_many_z[]
	INIT(= N_("E50: Too many \\z("));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_too_many_str_open[]
	INIT(= N_("E51: Too many %s("));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಅಕ್ಷರ e_unmatched_z[]
	INIT(= N_("E52: Unmatched \\z("));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_unmatched_str_percent_open[]
	INIT(= N_("E53: Unmatched %s%%("));
EXTERN ಅಕ್ಷರ e_unmatched_str_open[]
	INIT(= N_("E54: Unmatched %s("));
EXTERN ಅಕ್ಷರ e_unmatched_str_close[]
	INIT(= N_("E55: Unmatched %s)"));
// E56 unused
// E57 unused
// E58 unused
EXTERN ಅಕ್ಷರ e_invalid_character_after_str_at[]
	INIT(= N_("E59: Invalid character after %s@"));
EXTERN ಅಕ್ಷರ e_too_many_complex_str_curly[]
	INIT(= N_("E60: Too many complex %s{...}s"));
EXTERN ಅಕ್ಷರ e_nested_str[]
	INIT(= N_("E61: Nested %s*"));
EXTERN ಅಕ್ಷರ e_nested_str_chr[]
	INIT(= N_("E62: Nested %s%c"));
EXTERN ಅಕ್ಷರ e_invalid_use_ದ_underscore[]
	INIT(= N_("E63: Invalid use of \\_"));
EXTERN ಅಕ್ಷರ e_str_chr_follows_nothing[]
	INIT(= N_("E64: %s%c follows nothing"));
EXTERN ಅಕ್ಷರ e_illegal_ಹಿಂಭಾಗ_reference[]
	INIT(= N_("E65: Illegal back reference"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಅಕ್ಷರ e_z_not_allowed_ಇಲ್ಲಿ[]
	INIT(= N_("E66: \\z( not allowed here"));
EXTERN ಅಕ್ಷರ e_z1_z9_not_allowed_ಇಲ್ಲಿ[]
	INIT(= N_("E67: \\z1 - \\z9 not allowed here"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_invalid_character_after_bsl_z[]
	INIT(= N_("E68: Invalid character after \\z"));
EXTERN ಅಕ್ಷರ e_missing_sb_after_str[]
	INIT(= N_("E69: Missing ] after %s%%["));
EXTERN ಅಕ್ಷರ e_ಖಾಲಿಗಳು_str_brackets[]
	INIT(= N_("E70: Empty %s%%[]"));
EXTERN ಅಕ್ಷರ e_invalid_character_after_str[]
	INIT(= N_("E71: Invalid character after %s%%"));
EXTERN ಅಕ್ಷರ e_close_error_on_ಅದಲುಬದಲು_file[]
	INIT(= N_("E72: Close error on swap file"));
EXTERN ಅಕ್ಷರ e_ಟ್ಯಾಗ್_ರಾಶಿ_ಖಾಲಿಗಳು[]
	INIT(= N_("E73: Tag stack empty"));
EXTERN ಅಕ್ಷರ e_command_too_complex[]
	INIT(= N_("E74: Command too complex"));
EXTERN ಅಕ್ಷರ e_name_too_long[]
	INIT(= N_("E75: Name too long"));
EXTERN ಅಕ್ಷರ e_too_many_brackets[]
	INIT(= N_("E76: Too many ["));
EXTERN ಅಕ್ಷರ e_too_many_file_ಹೆಸರುಗಳು[]
	INIT(= N_("E77: Too many file names"));
EXTERN ಅಕ್ಷರ e_unknown_mark[]
	INIT(= N_("E78: Unknown mark"));
EXTERN ಅಕ್ಷರ e_cannot_expand_wildcards[]
	INIT(= N_("E79: Cannot expand wildcards"));
EXTERN ಅಕ್ಷರ e_error_while_writing[]
	INIT(= N_("E80: Error while writing"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_using_sid_not_ಒಳ_script_context[]
	INIT(= N_("E81: Using <SID> not in a script context"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_allocate_any_ಬಫರ್_exiting[]
	INIT(= N_("E82: Cannot allocate any buffer, exiting..."));
EXTERN ಅಕ್ಷರ e_cannot_allocate_ಬಫರ್_using_other_one[]
	INIT(= N_("E83: Cannot allocate buffer, using other one..."));
EXTERN ಅಕ್ಷರ e_no_modified_ಬಫರ್_ಸಿಕ್ಕಿತು[]
	INIT(= N_("E84: No modified buffer found"));
EXTERN ಅಕ್ಷರ e_there_is_no_listed_ಬಫರ್[]
	INIT(= N_("E85: There is no listed buffer"));
EXTERN ಅಕ್ಷರ e_ಬಫರ್_nr_does_not_exist[]
	INIT(= N_("E86: Buffer %ld does not exist"));
EXTERN ಅಕ್ಷರ e_cannot_go_beyond_ಕೊನೆಯ_ಬಫರ್[]
	INIT(= N_("E87: Cannot go beyond last buffer"));
EXTERN ಅಕ್ಷರ e_cannot_go_before_ಮೊದಲ_ಬಫರ್[]
	INIT(= N_("E88: Cannot go before first buffer"));
EXTERN ಅಕ್ಷರ e_no_write_since_ಕೊನೆಯ_change_for_ಬಫರ್_nr_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override[]
	INIT(= N_("E89: No write since last change for buffer %d (add ! to override)"));
EXTERN ಅಕ್ಷರ e_cannot_unload_ಕೊನೆಯ_ಬಫರ್[]
	INIT(= N_("E90: Cannot unload last buffer"));
EXTERN ಅಕ್ಷರ e_shell_option_is_ಖಾಲಿಗಳು[]
	INIT(= N_("E91: 'shell' option is empty"));
EXTERN ಅಕ್ಷರ e_ಬಫರ್_nr_not_ಸಿಕ್ಕಿತು[]
	INIT(= N_("E92: Buffer %d not found"));
EXTERN ಅಕ್ಷರ e_ಇನ್ನಷ್ಟು_than_one_ಹೊಂದಿಕೆ_for_str[]
	INIT(= N_("E93: More than one match for %s"));
EXTERN ಅಕ್ಷರ e_no_matching_ಬಫರ್_for_str[]
	INIT(= N_("E94: No matching buffer for %s"));
EXTERN ಅಕ್ಷರ e_ಬಫರ್_with_this_name_already_exists[]
	INIT(= N_("E95: Buffer with this name already exists"));
#ಆದರೆ defined(FEAT_DIFF)
EXTERN ಅಕ್ಷರ e_cannot_diff_ಇನ್ನಷ್ಟು_than_nr_buffers[]
	INIT(= N_("E96: Cannot diff more than %d buffers"));
EXTERN ಅಕ್ಷರ e_cannot_create_diffs[]
	INIT(= N_("E97: Cannot create diffs"));
EXTERN ಅಕ್ಷರ e_cannot_read_diff_output[]
	INIT(= N_("E98: Cannot read diff output"));
EXTERN ಅಕ್ಷರ e_current_ಬಫರ್_is_not_ಒಳ_diff_ಬಹುಲಕ[]
	INIT(= N_("E99: Current buffer is not in diff mode"));
EXTERN ಅಕ್ಷರ e_no_other_ಬಫರ್_ಒಳ_diff_ಬಹುಲಕ[]
	INIT(= N_("E100: No other buffer in diff mode"));
EXTERN ಅಕ್ಷರ e_ಇನ್ನಷ್ಟು_than_two_buffers_ಒಳ_diff_ಬಹುಲಕ_dont_know_which_one_ಗಮ್ಯ_use[]
	INIT(= N_("E101: More than two buffers in diff mode, don't know which one to use"));
EXTERN ಅಕ್ಷರ e_cant_ಕಂಡುಹಿಡಿ_ಬಫರ್_str[]
	INIT(= N_("E102: Can't find buffer \"%s\""));
EXTERN ಅಕ್ಷರ e_ಬಫರ್_str_is_not_ಒಳ_diff_ಬಹುಲಕ[]
	INIT(= N_("E103: Buffer \"%s\" is not in diff mode"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIGRAPHS
EXTERN ಅಕ್ಷರ e_escape_not_allowed_ಒಳ_digraph[]
	INIT(= N_("E104: Escape not allowed in digraph"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
EXTERN ಅಕ್ಷರ e_using_loadkeymap_not_ಒಳ_sourced_file[]
	INIT(= N_("E105: Using :loadkeymap not in a sourced file"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_unsupported_diff_output_format_str[]
	INIT(= N_("E106: Unsupported diff output format: %s"));
EXTERN ಅಕ್ಷರ e_missing_parenthesis_str[]
	INIT(= N_("E107: Missing parentheses: %s"));
EXTERN ಅಕ್ಷರ e_no_such_variable_str[]
	INIT(= N_("E108: No such variable: \"%s\""));
EXTERN ಅಕ್ಷರ e_missing_colon_after_questionmark[]
	INIT(= N_("E109: Missing ':' after '?'"));
EXTERN ಅಕ್ಷರ e_missing_closing_paren[]
	INIT(= N_("E110: Missing ')'"));
EXTERN ಅಕ್ಷರ e_missing_closing_ವರ್ಗಫಲ_brace[]
	INIT(= N_("E111: Missing ']'"));
EXTERN ಅಕ್ಷರ e_option_name_missing_str[]
	INIT(= N_("E112: Option name missing: %s"));
EXTERN ಅಕ್ಷರ e_unknown_option_str[]
	INIT(= N_("E113: Unknown option: %s"));
EXTERN ಅಕ್ಷರ e_missing_double_quote_str[]
	INIT(= N_("E114: Missing double quote: %s"));
EXTERN ಅಕ್ಷರ e_missing_single_quote_str[]
	INIT(= N_("E115: Missing single quote: %s"));
EXTERN ಅಕ್ಷರ e_invalid_arguments_for_function_str[]
	INIT(= N_("E116: Invalid arguments for function %s"));
EXTERN ಅಕ್ಷರ e_unknown_function_str[]
	INIT(= N_("E117: Unknown function: %s"));
EXTERN ಅಕ್ಷರ e_too_many_arguments_for_function_str[]
	INIT(= N_("E118: Too many arguments for function: %s"));
EXTERN ಅಕ್ಷರ e_not_enough_arguments_for_function_str[]
	INIT(= N_("E119: Not enough arguments for function: %s"));
EXTERN ಅಕ್ಷರ e_using_sid_not_ಒಳ_script_context_str[]
	INIT(= N_("E120: Using <SID> not in a script context: %s"));
EXTERN ಅಕ್ಷರ e_undefined_variable_str[]
	INIT(= N_("E121: Undefined variable: %s"));
EXTERN ಅಕ್ಷರ e_undefined_variable_char_str[]
	INIT(= N_("E121: Undefined variable: %c:%s"));
EXTERN ಅಕ್ಷರ e_function_str_already_exists_ಕೂಡಿಸು_bang_ಗಮ್ಯ_replace[]
	INIT(= N_("E122: Function %s already exists, add ! to replace it"));
EXTERN ಅಕ್ಷರ e_undefined_function_str[]
	INIT(= N_("E123: Undefined function: %s"));
EXTERN ಅಕ್ಷರ e_missing_paren_str[]
	INIT(= N_("E124: Missing '(': %s"));
EXTERN ಅಕ್ಷರ e_illegal_ವಾದ_str[]
	INIT(= N_("E125: Illegal argument: %s"));
EXTERN ಅಕ್ಷರ e_missing_endfunction[]
	INIT(= N_("E126: Missing :endfunction"));
EXTERN ಅಕ್ಷರ e_cannot_redefine_function_str_it_is_ಒಳ_use[]
	INIT(= N_("E127: Cannot redefine function %s: It is in use"));
EXTERN ಅಕ್ಷರ e_function_name_must_ಆರಂಭ_with_capital_or_s_str[]
	INIT(= N_("E128: Function name must start with a capital or \"s:\": %s"));
EXTERN ಅಕ್ಷರ e_function_name_required[]
	INIT(= N_("E129: Function name required"));
// E130 unused
EXTERN ಅಕ್ಷರ e_cannot_delete_function_str_it_is_ಒಳ_use[]
	INIT(= N_("E131: Cannot delete function %s: It is in use"));
EXTERN ಅಕ್ಷರ e_function_call_ಆಳ_is_higher_than_maxfuncdepth[]
	INIT(= N_("E132: Function call depth is higher than 'maxfuncdepth'"));
EXTERN ಅಕ್ಷರ e_return_not_inside_function[]
	INIT(= N_("E133: :return not inside a function"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_move_range_ದ_ಸಾಲುಗಳು_into_itself[]
	INIT(= N_("E134: Cannot move a range of lines into itself"));
EXTERN ಅಕ್ಷರ e_filter_autocommands_must_not_change_current_ಬಫರ್[]
	INIT(= N_("E135: *Filter* Autocommands must not change current buffer"));
#ಆದರೆ defined(FEAT_VIMINFO)
EXTERN ಅಕ್ಷರ e_viminfo_too_many_errors_skipping_rest_ದ_file[]
	INIT(= N_("E136: viminfo: Too many errors, skipping rest of file"));
EXTERN ಅಕ್ಷರ e_viminfo_file_is_not_writable_str[]
	INIT(= N_("E137: Viminfo file is not writable: %s"));
EXTERN ಅಕ್ಷರ e_cant_write_viminfo_file_str[]
	INIT(= N_("E138: Can't write viminfo file %s!"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_file_is_loaded_ಒಳ_another_ಬಫರ್[]
	INIT(= N_("E139: File is loaded in another buffer"));
EXTERN ಅಕ್ಷರ e_use_bang_ಗಮ್ಯ_write_partial_ಬಫರ್[]
	INIT(= N_("E140: Use ! to write partial buffer"));
EXTERN ಅಕ್ಷರ e_no_file_name_for_ಬಫರ್_nr[]
	INIT(= N_("E141: No file name for buffer %ld"));
EXTERN ಅಕ್ಷರ e_file_not_written_writing_is_disabled_by_write_option[]
	INIT(= N_("E142: File not written: Writing is disabled by 'write' option"));
EXTERN ಅಕ್ಷರ e_autocommands_unexpectedly_deleted_new_ಬಫರ್_str[]
	INIT(= N_("E143: Autocommands unexpectedly deleted new buffer %s"));
EXTERN ಅಕ್ಷರ e_non_numeric_ವಾದ_ಗಮ್ಯ_z[]
	INIT(= N_("E144: Non-numeric argument to :z"));
EXTERN ಅಕ್ಷರ e_shell_commands_and_some_functionality_not_allowed_ಒಳ_rvim[]
	INIT(= N_("E145: Shell commands and some functionality not allowed in rvim"));
EXTERN ಅಕ್ಷರ e_regular_expressions_cant_be_delimited_by_letters[]
	INIT(= N_("E146: Regular expressions can't be delimited by letters"));
EXTERN ಅಕ್ಷರ e_cannot_do_global_recursive_with_range[]
	INIT(= N_("E147: Cannot do :global recursive with a range"));
EXTERN ಅಕ್ಷರ e_regular_ಅಭಿವ್ಯಕ್ತಿ_missing_from_global[]
	INIT(= N_("E148: Regular expression missing from :global"));
EXTERN ಅಕ್ಷರ e_sorry_no_help_for_str[]
	INIT(= N_("E149: Sorry, no help for %s"));
EXTERN ಅಕ್ಷರ e_not_a_directory_str[]
	INIT(= N_("E150: Not a directory: %s"));
EXTERN ಅಕ್ಷರ e_no_ಹೊಂದಿಕೆ_str_1[]
	INIT(= N_("E151: No match: %s"));
EXTERN ಅಕ್ಷರ e_cannot_open_str_for_writing_1[]
	INIT(= N_("E152: Cannot open %s for writing"));
EXTERN ಅಕ್ಷರ e_unable_ಗಮ್ಯ_open_str_for_reading[]
	INIT(= N_("E153: Unable to open %s for reading"));
EXTERN ಅಕ್ಷರ e_duplicate_ಟ್ಯಾಗ್_str_ಒಳ_file_str_str[]
	INIT(= N_("E154: Duplicate tag \"%s\" in file %s/%s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
EXTERN ಅಕ್ಷರ e_unknown_sign_str[]
	INIT(= N_("E155: Unknown sign: %s"));
EXTERN ಅಕ್ಷರ e_missing_sign_name[]
	INIT(= N_("E156: Missing sign name"));
EXTERN ಅಕ್ಷರ e_invalid_sign_id_nr[]
	INIT(= N_("E157: Invalid sign ID: %d"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_SIGNS) || defined(FEAT_EVAL)
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_name_str[]
	INIT(= N_("E158: Invalid buffer name: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
EXTERN ಅಕ್ಷರ e_missing_sign_ಸಂಖ್ಯೆ[]
	INIT(= N_("E159: Missing sign number"));
EXTERN ಅಕ್ಷರ e_unknown_sign_command_str[]
	INIT(= N_("E160: Unknown sign command: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_breakpoint_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E161: Breakpoint not found: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_no_write_since_ಕೊನೆಯ_change_for_ಬಫರ್_str[]
	INIT(= N_("E162: No write since last change for buffer \"%s\""));
EXTERN ಅಕ್ಷರ e_there_is_only_one_file_ಗಮ್ಯ_edit[]
	INIT(= N_("E163: There is only one file to edit"));
EXTERN ಅಕ್ಷರ e_cannot_go_before_ಮೊದಲ_file[]
	INIT(= N_("E164: Cannot go before first file"));
EXTERN ಅಕ್ಷರ e_cannot_go_beyond_ಕೊನೆಯ_file[]
	INIT(= N_("E165: Cannot go beyond last file"));
EXTERN ಅಕ್ಷರ e_cant_open_linked_file_for_writing[]
	INIT(= N_("E166: Can't open linked file for writing"));
EXTERN ಅಕ್ಷರ e_scriptencoding_ಬಳಸಿದ_outside_ದ_sourced_file[]
	INIT(= N_("E167: :scriptencoding used outside of a sourced file"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ಮುಕ್ತಾಯ_ಬಳಸಿದ_outside_ದ_sourced_file[]
	INIT(= N_("E168: :finish used outside of a sourced file"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_command_too_recursive[]
	INIT(= N_("E169: Command too recursive"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_missing_endwhile[]
	INIT(= N_("E170: Missing :endwhile"));
EXTERN ಅಕ್ಷರ e_missing_endfor[]
	INIT(= N_("E170: Missing :endfor"));
EXTERN ಅಕ್ಷರ e_missing_endif[]
	INIT(= N_("E171: Missing :endif"));
EXTERN ಅಕ್ಷರ e_missing_marker[]
	INIT(= N_("E172: Missing marker"));
#ಆದರೆ_ಅಂತ್ಯ

PLURAL_ಸಂದೇಶ(e_nr_ಇನ್ನಷ್ಟು_file_ಗಮ್ಯ_edit, "E173: %d more file to edit",
		e_nr_ಇನ್ನಷ್ಟು_files_ಗಮ್ಯ_edit, "E173: %d more files to edit")

EXTERN ಅಕ್ಷರ e_command_already_exists_ಕೂಡಿಸು_bang_ಗಮ್ಯ_replace_it_str[]
	INIT(= N_("E174: Command already exists: add ! to replace it: %s"));
EXTERN ಅಕ್ಷರ e_no_attribute_specified[]
	INIT(= N_("E175: No attribute specified"));
EXTERN ಅಕ್ಷರ e_invalid_ಸಂಖ್ಯೆ_ದ_arguments[]
	INIT(= N_("E176: Invalid number of arguments"));
EXTERN ಅಕ್ಷರ e_ಎಣಿಕೆ_cannot_be_specified_twice[]
	INIT(= N_("E177: Count cannot be specified twice"));
EXTERN ಅಕ್ಷರ e_invalid_default_ಮೌಲ್ಯ_for_ಎಣಿಕೆ[]
	INIT(= N_("E178: Invalid default value for count"));
EXTERN ಅಕ್ಷರ e_ವಾದ_required_for_str[]
	INIT(= N_("E179: Argument required for %s"));
EXTERN ಅಕ್ಷರ e_invalid_complete_ಮೌಲ್ಯ_str[]
	INIT(= N_("E180: Invalid complete value: %s"));
EXTERN ಅಕ್ಷರ e_invalid_ವಿಳಾಸ_type_ಮೌಲ್ಯ_str[]
	INIT(= N_("E180: Invalid address type value: %s"));
EXTERN ಅಕ್ಷರ e_invalid_attribute_str[]
	INIT(= N_("E181: Invalid attribute: %s"));
EXTERN ಅಕ್ಷರ e_invalid_command_name[]
	INIT(= N_("E182: Invalid command name"));
EXTERN ಅಕ್ಷರ e_user_defined_commands_must_ಆರಂಭ_with_an_uppercase_letter[]
	INIT(= N_("E183: User defined commands must start with an uppercase letter"));
EXTERN ಅಕ್ಷರ e_no_such_user_defined_command_str[]
	INIT(= N_("E184: No such user-defined command: %s"));
EXTERN ಅಕ್ಷರ e_cannot_ಕಂಡುಹಿಡಿ_color_scheme_str[]
	INIT(= N_("E185: Cannot find color scheme '%s'"));
EXTERN ಅಕ್ಷರ e_no_ಹಿಂದಿನ_directory[]
	INIT(= N_("E186: No previous directory"));
EXTERN ಅಕ್ಷರ e_directory_unknown[]
	INIT(= N_("E187: Directory unknown"));
EXTERN ಅಕ್ಷರ e_obtaining_ಕಿಟಕಿ_position_not_implemented_for_this_platform[]
	INIT(= N_("E188: Obtaining window position not implemented for this platform"));
EXTERN ಅಕ್ಷರ e_str_exists_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override[]
	INIT(= N_("E189: \"%s\" exists (add ! to override)"));
EXTERN ಅಕ್ಷರ e_cannot_open_str_for_writing_2[]
	INIT(= N_("E190: Cannot open \"%s\" for writing"));
EXTERN ಅಕ್ಷರ e_ವಾದ_must_be_letter_or_forward_backward_quote[]
	INIT(= N_("E191: Argument must be a letter or forward/backward quote"));
EXTERN ಅಕ್ಷರ e_recursive_use_ದ_ಸಾಮಾನ್ಯ_too_deep[]
	INIT(= N_("E192: Recursive use of :normal too deep"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_str_not_inside_function[]
	INIT(= N_("E193: %s not inside a function"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_no_alternate_file_name_ಗಮ್ಯ_substitute_for_hash[]
	INIT(= N_("E194: No alternate file name to substitute for '#'"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
EXTERN ಅಕ್ಷರ e_cannot_open_viminfo_file_for_reading[]
	INIT(= N_("E195: Cannot open viminfo file for reading"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_DIGRAPHS
EXTERN ಅಕ್ಷರ e_no_digraphs_ಆವೃತ್ತಿ[]
	INIT(= N_("E196: No digraphs in this version"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_ಗಣ_language_ಗಮ್ಯ_str[]
	INIT(= N_("E197: Cannot set language to \"%s\""));
// E198 unused
EXTERN ಅಕ್ಷರ e_ಸಕ್ರಿಯ_ಕಿಟಕಿ_or_ಬಫರ್_ಬದಲಾಯಿತು_or_deleted[]
	INIT(= N_("E199: Active window or buffer changed or deleted"));
EXTERN ಅಕ್ಷರ e_readpre_autocommands_made_file_unreadable[]
	INIT(= N_("E200: *ReadPre autocommands made the file unreadable"));
EXTERN ಅಕ್ಷರ e_readpre_autocommands_must_not_change_current_ಬಫರ್[]
	INIT(= N_("E201: *ReadPre autocommands must not change current buffer"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_conversion_mad_file_unreadable[]
	INIT(= N_("E202: Conversion made file unreadable!"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_autocommands_deleted_or_unloaded_ಬಫರ್_ಗಮ್ಯ_be_written[]
	INIT(= N_("E203: Autocommands deleted or unloaded buffer to be written"));
EXTERN ಅಕ್ಷರ e_autocommands_ಬದಲಾಯಿತು_ಸಂಖ್ಯೆ_ದ_ಸಾಲುಗಳು_ಒಳ_unexpected_way[]
	INIT(= N_("E204: Autocommand changed number of lines in unexpected way"));
EXTERN ಅಕ್ಷರ e_patchmode_cant_save_original_file[]
	INIT(= N_("E205: Patchmode: can't save original file"));
EXTERN ಅಕ್ಷರ e_patchmode_cant_touch_ಖಾಲಿಗಳು_original_file[]
	INIT(= N_("E206: Patchmode: can't touch empty original file"));
EXTERN ಅಕ್ಷರ e_cant_delete_backup_file[]
	INIT(= N_("E207: Can't delete backup file"));
EXTERN ಅಕ್ಷರ e_error_writing_ಗಮ್ಯ_str[]
	INIT(= N_("E208: Error writing to \"%s\""));
EXTERN ಅಕ್ಷರ e_error_closing_str[]
	INIT(= N_("E209: Error closing \"%s\""));
EXTERN ಅಕ್ಷರ e_error_reading_str[]
	INIT(= N_("E210: Error reading \"%s\""));
EXTERN ಅಕ್ಷರ e_file_str_no_longer_ಲಭ್ಯ[]
	INIT(= N_("E211: File \"%s\" no longer available"));
EXTERN ಅಕ್ಷರ e_cant_open_file_for_writing[]
	INIT(= N_("E212: Can't open file for writing"));
EXTERN ಅಕ್ಷರ e_cannot_convert_ಕೂಡಿಸು_bang_ಗಮ್ಯ_write_without_conversion[]
	INIT(= N_("E213: Cannot convert (add ! to write without conversion)"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cant_ಕಂಡುಹಿಡಿ_ತಾತ್ಕಾಲಿಕ_file_for_writing[]
	INIT(= N_("E214: Can't find temp file for writing"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_illegal_character_after_ನಕ್ಷತ್ರ_str[]
	INIT(= N_("E215: Illegal character after *: %s"));
EXTERN ಅಕ್ಷರ e_no_such_event_str[]
	INIT(= N_("E216: No such event: %s"));
EXTERN ಅಕ್ಷರ e_no_such_ಗುಂಪು_or_event_str[]
	INIT(= N_("E216: No such group or event: %s"));
EXTERN ಅಕ್ಷರ e_cant_execute_autocommands_for_all_events[]
	INIT(= N_("E217: Can't execute autocommands for ALL events"));
EXTERN ಅಕ್ಷರ e_autocommand_nesting_too_deep[]
	INIT(= N_("E218: Autocommand nesting too deep"));
EXTERN ಅಕ್ಷರ e_missing_open_curly[]
	INIT(= N_("E219: Missing {."));
EXTERN ಅಕ್ಷರ e_missing_close_curly[]
	INIT(= N_("E220: Missing }."));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_marker_cannot_ಆರಂಭ_with_ಅಧೋ_case_letter[]
	INIT(= N_("E221: Marker cannot start with lower case letter"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಕೂಡಿಸು_ಗಮ್ಯ_ಆಂತರಿಕ_ಬಫರ್_that_was_already_read_from[]
	INIT(= "E222: Add to internal buffer that was already read from");
EXTERN ಅಕ್ಷರ e_recursive_mapping[]
	INIT(= N_("E223: Recursive mapping"));
EXTERN ಅಕ್ಷರ e_global_abbreviation_already_exists_for_str[]
	INIT(= N_("E224: Global abbreviation already exists for %s"));
EXTERN ಅಕ್ಷರ e_global_mapping_already_exists_for_str[]
	INIT(= N_("E225: Global mapping already exists for %s"));
EXTERN ಅಕ್ಷರ e_abbreviation_already_exists_for_str[]
	INIT(= N_("E226: Abbreviation already exists for %s"));
EXTERN ಅಕ್ಷರ e_mapping_already_exists_for_str[]
	INIT(= N_("E227: Mapping already exists for %s"));
EXTERN ಅಕ್ಷರ e_makemap_illegal_ಬಹುಲಕ[]
	INIT(= "E228: makemap: Illegal mode");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN ಅಕ್ಷರ e_cannot_ಆರಂಭ_the_GUI[]
	INIT(= N_("E229: Cannot start the GUI"));
EXTERN ಅಕ್ಷರ e_cannot_read_from_str[]
	INIT(= N_("E230: Cannot read from \"%s\""));
EXTERN ಅಕ್ಷರ e_guifontwide_invalid[]
	INIT(= N_("E231: 'guifontwide' invalid"));
# ifdef FEAT_BEVAL_GUI
EXTERN ಅಕ್ಷರ e_cannot_create_ballooneval_with_both_ಸಂದೇಶ_and_callback[]
	INIT(= "E232: Cannot create BalloonEval with both message and callback");
# endif
# ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_X11)
EXTERN ಅಕ್ಷರ e_cannot_open_display[]
	INIT(= N_("E233: Cannot open display"));
# endif
# ಆದರೆ defined(FEAT_XFONTSET)
EXTERN ಅಕ್ಷರ e_unknown_fontset_str[]
	INIT(= N_("E234: Unknown fontset: %s"));
# endif
# ಆದರೆ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_PHOTON) || defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_HAIKU)
EXTERN ಅಕ್ಷರ e_unknown_font_str[]
	INIT(= N_("E235: Unknown font: %s"));
# endif
# ಆದರೆ defined(FEAT_GUI_X11) && !defined(FEAT_GUI_GTK)
EXTERN ಅಕ್ಷರ e_font_str_is_not_fixed_ಅಗಲ[]
	INIT(= N_("E236: Font \"%s\" is not fixed-width"));
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MSWIN) || defined(FEAT_PRINT_PANGO)
EXTERN ಅಕ್ಷರ e_printer_selection_failed[]
	INIT(= N_("E237: Printer selection failed"));
EXTERN ಅಕ್ಷರ e_print_error_str[]
	INIT(= N_("E238: Print error: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
EXTERN ಅಕ್ಷರ e_invalid_sign_ಪಠ್ಯ_str[]
	INIT(= N_("E239: Invalid sign text: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
EXTERN ಅಕ್ಷರ e_no_connection_ಗಮ್ಯ_x_server[]
	INIT(= N_("E240: No connection to the X server"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
EXTERN ಅಕ್ಷರ e_unable_ಗಮ್ಯ_send_ಗಮ್ಯ_str[]
	INIT(= N_("E241: Unable to send to %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cant_ವಿಭಜಿಸು_ಕಿಟಕಿ_while_closing_another[]
	INIT(= N_("E242: Can't split a window while closing another"));
#ಆದರೆ defined(FEAT_GUI_MSWIN) && !defined(FEAT_OLE)
EXTERN ಅಕ್ಷರ e_ವಾದ_not_supported_str_use_ole_ಆವೃತ್ತಿ[]
	INIT(= N_("E243: Argument not supported: \"-%s\"; Use the OLE version."));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
EXTERN ಅಕ್ಷರ e_illegal_str_name_str_ಒಳ_font_name_str[]
	INIT(= N_("E244: Illegal %s name \"%s\" in font name \"%s\""));
EXTERN ಅಕ್ಷರ e_illegal_char_nr_ಒಳ_font_name_str[]
	INIT(= N_("E245: Illegal char '%c' in font name \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_filechangedshell_autocommand_deleted_ಬಫರ್[]
	INIT(= N_("E246: FileChangedShell autocommand deleted buffer"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
EXTERN ಅಕ್ಷರ e_no_registered_server_named_str[]
	INIT(= N_("E247: No registered server named \"%s\""));
EXTERN ಅಕ್ಷರ e_failed_ಗಮ್ಯ_send_command_ಗಮ್ಯ_destination_ಕಾರ್ಯಕ್ರಮ[]
	INIT(= N_("E248: Failed to send command to the destination program"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಕಿಟಕಿ_layout_ಬದಲಾಯಿತು_unexpectedly[]
	INIT(= N_("E249: Window layout changed unexpectedly"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
EXTERN ಅಕ್ಷರ e_fonts_for_the_following_charsets_are_missing_ಒಳ_fontset[]
	INIT(= N_("E250: Fonts for the following charsets are missing in fontset %s:"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
EXTERN ಅಕ್ಷರ e_vim_instance_registry_property_is_badly_formed_deleted[]
	INIT(= N_("E251: VIM instance registry property is badly formed.  Deleted!"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_X11
EXTERN ಅಕ್ಷರ e_fontsent_name_str_font_str_is_not_fixed_ಅಗಲ[]
	INIT(= N_("E252: Fontset name: %s - Font '%s' is not fixed-width"));
EXTERN ಅಕ್ಷರ e_fontset_name_str[]
	INIT(= N_("E253: Fontset name: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
EXTERN ಅಕ್ಷರ e_cannot_allocate_color_str[]
	INIT(= N_("E254: Cannot allocate color %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_SIGN_ICONS) && !defined(FEAT_GUI_GTK)
EXTERN ಅಕ್ಷರ e_couldnt_read_ಒಳ_sign_ದತ್ತಾಂಶ[]
	INIT(= N_("E255: Couldn't read in sign data"));
#ಆದರೆ_ಅಂತ್ಯ
// E256 unused
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
EXTERN ಅಕ್ಷರ e_cstag_ಟ್ಯಾಗ್_not_founc[]
	INIT(= N_("E257: cstag: Tag not found"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
EXTERN ಅಕ್ಷರ e_unable_ಗಮ್ಯ_send_ಗಮ್ಯ_client[]
	INIT(= N_("E258: Unable to send to client"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
EXTERN ಅಕ್ಷರ e_no_matches_ಸಿಕ್ಕಿತು_for_cscope_query_str_ದ_str[]
	INIT(= N_("E259: No matches found for cscope query %s of %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_missing_name_after_ವಿಧಾನ[]
	INIT(= N_("E260: Missing name after ->"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
EXTERN ಅಕ್ಷರ e_cscope_connection_str_not_founc[]
	INIT(= N_("E261: Cscope connection %s not found"));
EXTERN ಅಕ್ಷರ e_error_reading_cscope_connection_nr[]
	INIT(= N_("E262: Error reading cscope connection %d"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3)
EXTERN ಅಕ್ಷರ e_sorry_this_command_is_disabled_python_library_could_not_be_ಸಿಕ್ಕಿತು[]
	INIT(= N_("E263: Sorry, this command is disabled, the Python library could not be loaded."));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN ಅಕ್ಷರ e_python_error_initialising_io_object[]
	INIT(= N_("E264: Python: Error initialising I/O objects"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
EXTERN ಅಕ್ಷರ e_dollar_must_be_an_instance_ದ_string[]
	INIT(= N_("E265: $_ must be an instance of String"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_RUBY
EXTERN ಅಕ್ಷರ e_sorry_this_command_is_disabled_the_ruby_library_could_not_be_loaded[]
	INIT(= N_("E266: Sorry, this command is disabled, the Ruby library could not be loaded."));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
EXTERN ಅಕ್ಷರ e_unexpected_return[]
	INIT(= N_("E267: Unexpected return"));
EXTERN ಅಕ್ಷರ e_unexpected_ಮುಂದಿನ[]
	INIT(= N_("E268: Unexpected next"));
EXTERN ಅಕ್ಷರ e_unexpected_break[]
	INIT(= N_("E269: Unexpected break"));
EXTERN ಅಕ್ಷರ e_unexpected_redo[]
	INIT(= N_("E270: Unexpected redo"));
EXTERN ಅಕ್ಷರ e_retry_outside_ದ_rescue_clause[]
	INIT(= N_("E271: Retry outside of rescue clause"));
EXTERN ಅಕ್ಷರ e_unhandled_exception[]
	INIT(= N_("E272: Unhandled exception"));
EXTERN ಅಕ್ಷರ e_unknown_longjmp_ಸ್ಥಿತಿ_nr[]
	INIT(= N_("E273: Unknown longjmp status %d"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_no_white_space_allowed_before_parenthesis[]
	INIT(= N_("E274: No white space allowed before parenthesis"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_cannot_ಕೂಡಿಸು_ಪಠ್ಯ_property_ಗಮ್ಯ_unloaded_ಬಫರ್[]
	INIT(= N_("E275: Cannot add text property to unloaded buffer"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_use_function_as_ವಿಧಾನ_str[]
	INIT(= N_("E276: Cannot use function as a method: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
EXTERN ಅಕ್ಷರ e_unable_ಗಮ್ಯ_read_server_ಉತ್ತರ[]
	INIT(= N_("E277: Unable to read a server reply"));
#ಆದರೆ_ಅಂತ್ಯ
// E278 unused
#ಆದರೆ defined(FEAT_TERMINAL) && !defined(UNIX) && !defined(MSWIN)
EXTERN ಅಕ್ಷರ e_sorry_plusplusshell_not_supported_on_this_system[]
	INIT(= N_("E279: Sorry, ++shell is not supported on this system"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TCL
EXTERN ಅಕ್ಷರ e_tcl_fatal_error_reflist_corrupt_please_report_this[]
	INIT(= "E280: TCL FATAL ERROR: reflist corrupt!? Please report this to vim-dev@vim.org");
#ಆದರೆ_ಅಂತ್ಯ
// E281 unused
EXTERN ಅಕ್ಷರ e_cannot_read_from_str_2[]
	INIT(= N_("E282: Cannot read from \"%s\""));
EXTERN ಅಕ್ಷರ e_no_marks_matching_str[]
	INIT(= N_("E283: No marks matching \"%s\""));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
# ifndef FEAT_GUI_GTK
EXTERN ಅಕ್ಷರ e_cannot_ಗಣ_ic_ಮೌಲ್ಯಗಳು[]
	INIT(= N_("E284: Cannot set IC values"));
# endif
# ifdef FEAT_GUI_X11
EXTERN ಅಕ್ಷರ e_failed_ಗಮ್ಯ_create_ಒಳಹರಿವು_context[]
	INIT(= N_("E285: Failed to create input context"));
EXTERN ಅಕ್ಷರ e_failed_ಗಮ್ಯ_open_ಒಳಹರಿವು_ವಿಧಾನ[]
	INIT(= N_("E286: Failed to open input method"));
EXTERN ಅಕ್ಷರ e_warning_could_not_ಗಣ_destroy_callback_ಗಮ್ಯ_im[]
	INIT(= N_("E287: Warning: Could not set destroy callback to IM"));
EXTERN ಅಕ್ಷರ e_ಒಳಹರಿವು_ವಿಧಾನ_doesnt_support_any_style[]
	INIT(= N_("E288: Input method doesn't support any style"));
EXTERN ಅಕ್ಷರ e_ಒಳಹರಿವು_ವಿಧಾನ_doesnt_support_my_preedit_type[]
	INIT(= N_("E289: Input method doesn't support my preedit type"));
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
EXTERN ಅಕ್ಷರ e_list_or_ಸಂಖ್ಯೆ_required[]
	INIT(= N_("E290: List or number required"));
#ಆದರೆ_ಅಂತ್ಯ
// E291 unused
EXTERN ಅಕ್ಷರ e_invalid_ಎಣಿಕೆ_for_del_ಬೈಟುಗಳು_nr[]
	INIT(= "E292: Invalid count for del_bytes(): %ld");
EXTERN ಅಕ್ಷರ e_ಖಂಡ_was_not_locked[]
	INIT(= "E293: Block was not locked");
EXTERN ಅಕ್ಷರ e_seek_error_ಒಳ_ಅದಲುಬದಲು_file_read[]
	INIT(= N_("E294: Seek error in swap file read"));
EXTERN ಅಕ್ಷರ e_read_error_ಒಳ_ಅದಲುಬದಲು_file[]
	INIT(= N_("E295: Read error in swap file"));
EXTERN ಅಕ್ಷರ e_seek_error_ಒಳ_ಅದಲುಬದಲು_file_write[]
	INIT(= N_("E296: Seek error in swap file write"));
EXTERN ಅಕ್ಷರ e_write_error_ಒಳ_ಅದಲುಬದಲು_file[]
	INIT(= N_("E297: Write error in swap file"));
EXTERN ಅಕ್ಷರ e_didnt_get_ಖಂಡ_nr_zero[]
	INIT(= "E298: Didn't get block nr 0?");
EXTERN ಅಕ್ಷರ e_didnt_get_ಖಂಡ_nr_one[]
	INIT(= "E298: Didn't get block nr 1?");
EXTERN ಅಕ್ಷರ e_didnt_get_ಖಂಡ_nr_two[]
	INIT(= "E298: Didn't get block nr 2?");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERL
EXTERN ಅಕ್ಷರ e_perl_evaluation_forbidden_ಒಳ_sandbox_without_ಸುರಕ್ಷಿತ_module[]
	INIT(= N_("E299: Perl evaluation forbidden in sandbox without the Safe module"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಅದಲುಬದಲು_file_already_exists_symlink_attack[]
	INIT(= N_("E300: Swap file already exists (symlink attack?)"));
EXTERN ಅಕ್ಷರ e_oops_ಕಳೆದುಹೋದ_the_ಅದಲುಬದಲು_file[]
	INIT(= N_("E301: Oops, lost the swap file!!!"));
EXTERN ಅಕ್ಷರ e_could_not_rename_ಅದಲುಬದಲು_file[]
	INIT(= N_("E302: Could not rename swap file"));
EXTERN ಅಕ್ಷರ e_unable_ಗಮ್ಯ_open_ಅದಲುಬದಲು_file_for_str_recovery_impossible[]
	INIT(= N_("E303: Unable to open swap file for \"%s\", recovery impossible"));
EXTERN ಅಕ್ಷರ e_ml_upd_block0_didnt_get_ಖಂಡ_zero[]
	INIT(= "E304: ml_upd_block0(): Didn't get block 0??");
EXTERN ಅಕ್ಷರ e_no_ಅದಲುಬದಲು_file_ಸಿಕ್ಕಿತು_for_str[]
	INIT(= N_("E305: No swap file found for %s"));
EXTERN ಅಕ್ಷರ e_cannot_open_str[]
	INIT(= N_("E306: Cannot open %s"));
EXTERN ಅಕ್ಷರ e_str_does_not_look_like_vim_ಅದಲುಬದಲು_file[]
	INIT(= N_("E307: %s does not look like a Vim swap file"));
EXTERN ಅಕ್ಷರ e_warning_original_file_may_have_been_ಬದಲಾಯಿತು[]
	INIT(= N_("E308: Warning: Original file may have been changed"));
EXTERN ಅಕ್ಷರ e_unable_ಗಮ್ಯ_read_ಖಂಡ_one_from_str[]
	INIT(= N_("E309: Unable to read block 1 from %s"));
EXTERN ಅಕ್ಷರ e_ಖಂಡ_one_id_wrong_str_not_swp_file[]
	INIT(= N_("E310: Block 1 ID wrong (%s not a .swp file?)"));
EXTERN ಅಕ್ಷರ e_recovery_interrupted[]
	INIT(= N_("E311: Recovery Interrupted"));
EXTERN ಅಕ್ಷರ e_errors_detected_while_recovering_look_for_ಸಾಲುಗಳು_starting_with_questions[]
	INIT(= N_("E312: Errors detected while recovering; look for lines starting with ???"));
EXTERN ಅಕ್ಷರ e_cannot_preserve_there_is_no_ಅದಲುಬದಲು_file[]
	INIT(= N_("E313: Cannot preserve, there is no swap file"));
EXTERN ಅಕ್ಷರ e_preserve_failed[]
	INIT(= N_("E314: Preserve failed"));
EXTERN ಅಕ್ಷರ e_ml_get_invalid_lnum_nr[]
	INIT(= "E315: ml_get: Invalid lnum: %ld");
EXTERN ಅಕ್ಷರ e_ml_get_cannot_ಕಂಡುಹಿಡಿ_ಸಾಲು_nr_ಒಳ_ಬಫರ್_nr_str[]
	INIT(= "E316: ml_get: Cannot find line %ld in buffer %d %s");
EXTERN ಅಕ್ಷರ e_ಸೂಚಕ_ಖಂಡ_id_wrong[]
	INIT(= "E317: Pointer block id wrong");
EXTERN ಅಕ್ಷರ e_ಸೂಚಕ_ಖಂಡ_id_wrong_two[]
	INIT(= "E317: Pointer block id wrong 2");
EXTERN ಅಕ್ಷರ e_ಸೂಚಕ_ಖಂಡ_id_wrong_three[]
	INIT(= "E317: Pointer block id wrong 3");
EXTERN ಅಕ್ಷರ e_ಸೂಚಕ_ಖಂಡ_id_wrong_four[]
	INIT(= "E317: Pointer block id wrong 4");
EXTERN ಅಕ್ಷರ e_updated_too_many_ಖಂಡಗಳು[]
	INIT(= "E318: Updated too many blocks?");
EXTERN ಅಕ್ಷರ e_sorry_command_is_not_ಲಭ್ಯ_ಒಳ_this_ಆವೃತ್ತಿ[]
	INIT(= N_("E319: Sorry, the command is not available in this version"));
EXTERN ಅಕ್ಷರ e_cannot_ಕಂಡುಹಿಡಿ_ಸಾಲು_nr[]
	INIT(= "E320: Cannot find line %ld");
EXTERN ಅಕ್ಷರ e_could_not_reload_str[]
	INIT(= N_("E321: Could not reload \"%s\""));
EXTERN ಅಕ್ಷರ e_ಸಾಲು_ಸಂಖ್ಯೆ_ಹೊರಹೋಗು_ದ_range_nr_past_the_ಕೊನೆ[]
	INIT(= "E322: Line number out of range: %ld past the end");
EXTERN ಅಕ್ಷರ e_ಸಾಲು_ಎಣಿಕೆ_wrong_ಒಳ_ಖಂಡ_nr[]
	INIT(= "E323: Line count wrong in block %ld");
#ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
EXTERN ಅಕ್ಷರ e_cant_open_postscript_output_file[]
	INIT(= N_("E324: Can't open PostScript output file"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_attention[]
	INIT(= N_("E325: ATTENTION"));
EXTERN ಅಕ್ಷರ e_too_many_ಅದಲುಬದಲು_files_ಸಿಕ್ಕಿತು[]
	INIT(= N_("E326: Too many swap files found"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
EXTERN char_u e_part_ದ_menu_ಅಂಶ_ಮಾರ್ಗ_is_not_sub_menu[]
	INIT(= N_("E327: Part of menu-item path is not sub-menu"));
EXTERN ಅಕ್ಷರ e_menu_only_exists_ಒಳ_another_ಬಹುಲಕ[]
	INIT(= N_("E328: Menu only exists in another mode"));
EXTERN char_u e_no_menu_str[]
	INIT(= N_("E329: No menu \"%s\""));
EXTERN ಅಕ್ಷರ e_menu_ಮಾರ್ಗ_must_not_lead_ಗಮ್ಯ_sub_menu[]
	INIT(= N_("E330: Menu path must not lead to a sub-menu"));
EXTERN ಅಕ್ಷರ e_must_not_ಕೂಡಿಸು_menu_ವಸ್ತುಗಳು_directly_ಗಮ್ಯ_menu_bar[]
	INIT(= N_("E331: Must not add menu items directly to menu bar"));
EXTERN ಅಕ್ಷರ e_separator_cannot_be_part_ದ_menu_ಮಾರ್ಗ[]
	INIT(= N_("E332: Separator cannot be part of a menu path"));
EXTERN ಅಕ್ಷರ e_menu_ಮಾರ್ಗ_must_lead_ಗಮ್ಯ_menu_ಅಂಶ[]
	INIT(= N_("E333: Menu path must lead to a menu item"));
EXTERN ಅಕ್ಷರ e_menu_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E334: Menu not found: %s"));
EXTERN ಅಕ್ಷರ e_menu_not_defined_for_str_ಬಹುಲಕ[]
	INIT(= N_("E335: Menu not defined for %s mode"));
EXTERN ಅಕ್ಷರ e_menu_ಮಾರ್ಗ_must_lead_ಗಮ್ಯ_sub_menu[]
	INIT(= N_("E336: Menu path must lead to a sub-menu"));
EXTERN ಅಕ್ಷರ e_menu_not_ಸಿಕ್ಕಿತು_check_menu_ಹೆಸರುಗಳು[]
	INIT(= N_("E337: Menu not found - check menu names"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
EXTERN ಅಕ್ಷರ e_sorry_no_file_browser_ಒಳ_console_ಬಹುಲಕ[]
	INIT(= N_("E338: Sorry, no file browser in console mode"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ನಮೂನೆ_too_long[]
	INIT(= N_("E339: Pattern too long"));
EXTERN ಅಕ್ಷರ e_ಆಂತರಿಕ_error_please_report_a_bug[]
	INIT(= N_("E340: Internal error; if you can reproduce please report a bug"));
EXTERN ಅಕ್ಷರ e_ಆಂತರಿಕ_error_lalloc_zero[]
	INIT(= "E341: Internal error: lalloc(0, )");
EXTERN ಅಕ್ಷರ e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ_allocating_nr_ಬೈಟುಗಳು[]
	INIT(= N_("E342: Out of memory!  (allocating %lu bytes)"));
EXTERN ಅಕ್ಷರ e_invalid_ಮಾರ್ಗ_ಸಂಖ್ಯೆ_must_be_at_ಕೊನೆ_ದ_ಮಾರ್ಗ_or_be_followed_by_str[]
	INIT(= N_("E343: Invalid path: '**[number]' must be at the end of the path or be followed by '%s'."));
EXTERN ಅಕ್ಷರ e_cant_ಕಂಡುಹಿಡಿ_directory_str_ಒಳ_cdpath[]
	INIT(= N_("E344: Can't find directory \"%s\" in cdpath"));
EXTERN ಅಕ್ಷರ e_cant_ಕಂಡುಹಿಡಿ_file_str_ಒಳ_ಮಾರ್ಗ[]
	INIT(= N_("E345: Can't find file \"%s\" in path"));
EXTERN ಅಕ್ಷರ e_no_ಇನ್ನಷ್ಟು_directory_str_ಸಿಕ್ಕಿತು_ಒಳ_cdpath[]
	INIT(= N_("E346: No more directory \"%s\" found in cdpath"));
EXTERN ಅಕ್ಷರ e_no_ಇನ್ನಷ್ಟು_file_str_ಸಿಕ್ಕಿತು_ಒಳ_ಮಾರ್ಗ[]
	INIT(= N_("E347: No more file \"%s\" found in path"));
EXTERN ಅಕ್ಷರ e_no_string_under_cursor[]
	INIT(= N_("E348: No string under cursor"));
EXTERN ಅಕ್ಷರ e_no_identifier_under_cursor[]
	INIT(= N_("E349: No identifier under cursor"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
EXTERN ಅಕ್ಷರ e_cannot_create_fold_with_current_foldmethod[]
	INIT(= N_("E350: Cannot create fold with current 'foldmethod'"));
EXTERN ಅಕ್ಷರ e_cannot_delete_fold_with_current_foldmethod[]
	INIT(= N_("E351: Cannot delete fold with current 'foldmethod'"));
EXTERN ಅಕ್ಷರ e_cannot_erase_folds_with_current_foldmethod[]
	INIT(= N_("E352: Cannot erase folds with current 'foldmethod'"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_nothing_ಒಳ_ರಿಜಿಸ್ಟರ್_str[]
	INIT(= N_("E353: Nothing in register %s"));
EXTERN ಅಕ್ಷರ e_invalid_ರಿಜಿಸ್ಟರ್_name_str[]
	INIT(= N_("E354: Invalid register name: '%s'"));
EXTERN ಅಕ್ಷರ e_unknown_option_str_2[]
	INIT(= N_("E355: Unknown option: %s"));
EXTERN ಅಕ್ಷರ e_get_varp_error[]
	INIT(= "E356: get_varp ERROR");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LANGMAP
EXTERN ಅಕ್ಷರ e_langmap_matching_character_missing_for_str[]
	INIT(= N_("E357: 'langmap': Matching character missing for %s"));
EXTERN ಅಕ್ಷರ e_langmap_extra_characters_after_semicolon_str[]
	INIT(= N_("E358: 'langmap': Extra characters after semicolon: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(AMIGA) || defined(MACOS_X) || defined(MSWIN)  \
	|| defined(UNIX) || defined(VMS)
EXTERN ಅಕ್ಷರ e_screen_ಬಹುಲಕ_setting_not_supported[]
	INIT(= N_("E359: Screen mode setting not supported"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
EXTERN ಅಕ್ಷರ e_cannot_execute_shell_with_f_option[]
	INIT(= N_("E360: Cannot execute shell with -f option"));
#ಆದರೆ_ಅಂತ್ಯ
// E361 unused
#ಆದರೆ defined(FEAT_EVAL)
EXTERN ಅಕ್ಷರ e_using_boolean_ಮೌಲ್ಯ_as_float[]
	INIT(= N_("E362: Using a boolean value as a Float"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ನಮೂನೆ_ಬಳಸುತ್ತದೆ_ಇನ್ನಷ್ಟು_ಸ್ಮರಣೆ_than_maxmempattern[]
	INIT(= N_("E363: Pattern uses more memory than 'maxmempattern'"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LIBCALL
EXTERN ಅಕ್ಷರ e_library_call_failed_for_str[]
	INIT(= N_("E364: Library call failed for \"%s()\""));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
EXTERN ಅಕ್ಷರ e_failed_ಗಮ್ಯ_print_postscript_file[]
	INIT(= N_("E365: Failed to print PostScript file"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_not_allowed_ಗಮ್ಯ_enter_popup_ಕಿಟಕಿ[]
	INIT(= N_("E366: Not allowed to enter a popup window"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_no_such_ಗುಂಪು_str[]
	INIT(= N_("E367: No such group: \"%s\""));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LIBCALL
EXTERN ಅಕ್ಷರ e_got_sig_str_ಒಳ_libcall[]
	INIT(= N_("E368: Got SIG%s in libcall()"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_invalid_ಅಂಶ_ಒಳ_str_brackets[]
	INIT(= N_("E369: Invalid item in %s%%[]"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USING_ತುಂಬು_LIBRARY
EXTERN ಅಕ್ಷರ e_could_not_ತುಂಬು_library_str_str[]
	INIT(= N_("E370: Could not load library %s: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
EXTERN ಅಕ್ಷರ e_command_not_ಸಿಕ್ಕಿತು[]
	INIT(= N_("E371: Command not found"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_too_many_chr_ಒಳ_format_string[]
	INIT(= N_("E372: Too many %%%c in format string"));
EXTERN ಅಕ್ಷರ e_unexpected_chr_ಒಳ_format_str[]
	INIT(= N_("E373: Unexpected %%%c in format string"));
EXTERN ಅಕ್ಷರ e_missing_rsb_ಒಳ_format_string[]
	INIT(= N_("E374: Missing ] in format string"));
EXTERN ಅಕ್ಷರ e_unsupported_chr_ಒಳ_format_string[]
	INIT(= N_("E375: Unsupported %%%c in format string"));
EXTERN ಅಕ್ಷರ e_invalid_chr_ಒಳ_format_string_ಪೂರ್ವಪ್ರತ್ಯಯ[]
	INIT(= N_("E376: Invalid %%%c in format string prefix"));
EXTERN ಅಕ್ಷರ e_invalid_chr_ಒಳ_format_string[]
	INIT(= N_("E377: Invalid %%%c in format string"));
EXTERN ಅಕ್ಷರ e_errorformat_ಒಳಗೊಂಡಿದೆ_no_ನಮೂನೆ[]
	INIT(= N_("E378: 'errorformat' contains no pattern"));
EXTERN ಅಕ್ಷರ e_missing_or_ಖಾಲಿಗಳು_directory_name[]
	INIT(= N_("E379: Missing or empty directory name"));
EXTERN ಅಕ್ಷರ e_at_bottom_ದ_quickfix_ರಾಶಿ[]
	INIT(= N_("E380: At bottom of quickfix stack"));
EXTERN ಅಕ್ಷರ e_at_ತುದಿ_ದ_quickfix_ರಾಶಿ[]
	INIT(= N_("E381: At top of quickfix stack"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_write_buftype_option_is_ಗಣ[]
	INIT(= N_("E382: Cannot write, 'buftype' option is set"));
EXTERN ಅಕ್ಷರ e_invalid_ಹುಡುಕು_string_str[]
	INIT(= N_("E383: Invalid search string: %s"));
EXTERN ಅಕ್ಷರ e_ಹುಡುಕು_ದೊರಕಿತು_ತುದಿ_without_ಹೊಂದಿಕೆ_for_str[]
	INIT(= N_("E384: Search hit TOP without match for: %s"));
EXTERN ಅಕ್ಷರ e_ಹುಡುಕು_ದೊರಕಿತು_bottom_without_ಹೊಂದಿಕೆ_for_str[]
	INIT(= N_("E385: Search hit BOTTOM without match for: %s"));
EXTERN ಅಕ್ಷರ e_expected_question_or_slash_after_semicolon[]
	INIT(= N_("E386: Expected '?' or '/'  after ';'"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
EXTERN ಅಕ್ಷರ e_ಹೊಂದಿಕೆ_is_on_current_ಸಾಲು[]
	INIT(= N_("E387: Match is on current line"));
EXTERN ಅಕ್ಷರ e_couldnt_ಕಂಡುಹಿಡಿ_definition[]
	INIT(= N_("E388: Couldn't find definition"));
EXTERN ಅಕ್ಷರ e_couldnt_ಕಂಡುಹಿಡಿ_ನಮೂನೆ[]
	INIT(= N_("E389: Couldn't find pattern"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಅಕ್ಷರ e_illegal_ವಾದ_str_2[]
	INIT(= N_("E390: Illegal argument: %s"));
EXTERN ಅಕ್ಷರ e_no_such_syntax_cluster_str_1[]
	INIT(= N_("E391: No such syntax cluster: %s"));
EXTERN ಅಕ್ಷರ e_no_such_syntax_cluster_str_2[]
	INIT(= N_("E392: No such syntax cluster: %s"));
EXTERN ಅಕ್ಷರ e_groupthere_not_accepted_ಇಲ್ಲಿ[]
	INIT(= N_("E393: group[t]here not accepted here"));
EXTERN ಅಕ್ಷರ e_didnt_ಕಂಡುಹಿಡಿ_region_ಅಂಶ_for_str[]
	INIT(= N_("E394: Didn't find region item for %s"));
EXTERN ಅಕ್ಷರ e_ಒಳಗೊಂಡಿದೆ_ವಾದ_not_accepted_ಇಲ್ಲಿ[]
	INIT(= N_("E395: Contains argument not accepted here"));
// E396 unused
EXTERN ಅಕ್ಷರ e_filename_required[]
	INIT(= N_("E397: Filename required"));
EXTERN ಅಕ್ಷರ e_missing_equal_str[]
	INIT(= N_("E398: Missing '=': %s"));
EXTERN ಅಕ್ಷರ e_not_enough_arguments_syntax_region_str[]
	INIT(= N_("E399: Not enough arguments: syntax region %s"));
EXTERN ಅಕ್ಷರ e_no_cluster_specified[]
	INIT(= N_("E400: No cluster specified"));
EXTERN ಅಕ್ಷರ e_ನಮೂನೆ_delimiter_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E401: Pattern delimiter not found: %s"));
EXTERN ಅಕ್ಷರ e_garbage_after_ನಮೂನೆ_str[]
	INIT(= N_("E402: Garbage after pattern: %s"));
EXTERN ಅಕ್ಷರ e_syntax_sync_ಸಾಲು_continuations_ನಮೂನೆ_specified_twice[]
	INIT(= N_("E403: syntax sync: Line continuations pattern specified twice"));
EXTERN ಅಕ್ಷರ e_illegal_arguments_str[]
	INIT(= N_("E404: Illegal arguments: %s"));
EXTERN ಅಕ್ಷರ e_missing_equal_sign_str[]
	INIT(= N_("E405: Missing equal sign: %s"));
EXTERN ಅಕ್ಷರ e_ಖಾಲಿಗಳು_ವಾದ_str[]
	INIT(= N_("E406: Empty argument: %s"));
EXTERN ಅಕ್ಷರ e_str_not_allowed_ಇಲ್ಲಿ[]
	INIT(= N_("E407: %s not allowed here"));
EXTERN ಅಕ್ಷರ e_str_must_be_ಮೊದಲ_ಒಳ_ಒಳಗೊಂಡಿದೆ_list[]
	INIT(= N_("E408: %s must be first in contains list"));
EXTERN ಅಕ್ಷರ e_unknown_ಗುಂಪು_name_str[]
	INIT(= N_("E409: Unknown group name: %s"));
EXTERN ಅಕ್ಷರ e_invalid_syntax_subcommand_str[]
	INIT(= N_("E410: Invalid :syntax subcommand: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_highlight_ಗುಂಪು_name_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E411: Highlight group not found: %s"));
EXTERN ಅಕ್ಷರ e_not_enough_arguments_highlight_link_str[]
	INIT(= N_("E412: Not enough arguments: \":highlight link %s\""));
EXTERN ಅಕ್ಷರ e_too_many_arguments_highlight_link_str[]
	INIT(= N_("E413: Too many arguments: \":highlight link %s\""));
EXTERN ಅಕ್ಷರ e_ಗುಂಪು_has_settings_highlight_link_ignored[]
	INIT(= N_("E414: Group has settings, highlight link ignored"));
EXTERN ಅಕ್ಷರ e_unexpected_equal_sign_str[]
	INIT(= N_("E415: Unexpected equal sign: %s"));
EXTERN ಅಕ್ಷರ e_missing_equal_sign_str_2[]
	INIT(= N_("E416: Missing equal sign: %s"));
EXTERN ಅಕ್ಷರ e_missing_ವಾದ_str[]
	INIT(= N_("E417: Missing argument: %s"));
EXTERN ಅಕ್ಷರ e_illegal_ಮೌಲ್ಯ_str[]
	INIT(= N_("E418: Illegal value: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_im_a_teapot[]
	INIT(= N_("E418: I'm a teapot"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_fg_color_unknown[]
	INIT(= N_("E419: FG color unknown"));
EXTERN ಅಕ್ಷರ e_bg_color_unknown[]
	INIT(= N_("E420: BG color unknown"));
EXTERN ಅಕ್ಷರ e_color_name_or_ಸಂಖ್ಯೆ_not_recognized_str[]
	INIT(= N_("E421: Color name or number not recognized: %s"));
EXTERN ಅಕ್ಷರ e_terminal_ಸಂಕೇತ_too_long_str[]
	INIT(= N_("E422: Terminal code too long: %s"));
EXTERN ಅಕ್ಷರ e_illegal_ವಾದ_str_3[]
	INIT(= N_("E423: Illegal argument: %s"));
EXTERN ಅಕ್ಷರ e_too_many_different_highlighting_ಗುಣಲಕ್ಷಣಗಳು_ಒಳ_use[]
	INIT(= N_("E424: Too many different highlighting attributes in use"));
EXTERN ಅಕ್ಷರ e_cannot_go_before_ಮೊದಲ_matching_ಟ್ಯಾಗ್[]
	INIT(= N_("E425: Cannot go before first matching tag"));
EXTERN ಅಕ್ಷರ e_ಟ್ಯಾಗ್_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E426: Tag not found: %s"));
EXTERN ಅಕ್ಷರ e_there_is_only_one_matching_ಟ್ಯಾಗ್[]
	INIT(= N_("E427: There is only one matching tag"));
EXTERN ಅಕ್ಷರ e_cannot_go_beyond_ಕೊನೆಯ_matching_ಟ್ಯಾಗ್[]
	INIT(= N_("E428: Cannot go beyond last matching tag"));
EXTERN ಅಕ್ಷರ e_file_str_does_not_exist[]
	INIT(= N_("E429: File \"%s\" does not exist"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EMACS_ಟ್ಯಾಗುಗಳು
EXTERN ಅಕ್ಷರ e_ಟ್ಯಾಗ್_file_ಮಾರ್ಗ_truncated_for_str[]
	INIT(= N_("E430: Tag file path truncated for %s\n"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_format_error_ಒಳ_ಟ್ಯಾಗುಗಳು_file_str[]
	INIT(= N_("E431: Format error in tags file \"%s\""));
EXTERN ಅಕ್ಷರ e_ಟ್ಯಾಗುಗಳು_file_not_sorted_str[]
	INIT(= N_("E432: Tags file not sorted: %s"));
EXTERN ಅಕ್ಷರ e_no_ಟ್ಯಾಗುಗಳು_file[]
	INIT(= N_("E433: No tags file"));
EXTERN ಅಕ್ಷರ e_cannot_ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗ್_ನಮೂನೆ[]
	INIT(= N_("E434: Can't find tag pattern"));
EXTERN ಅಕ್ಷರ e_couldnt_ಕಂಡುಹಿಡಿ_ಟ್ಯಾಗ್_just_guessing[]
	INIT(= N_("E435: Couldn't find tag, just guessing!"));
EXTERN ಅಕ್ಷರ e_no_str_ನಮೂದು_ಒಳ_termcap[]
	INIT(= N_("E436: No \"%s\" entry in termcap"));
EXTERN ಅಕ್ಷರ e_terminal_capability_cm_required[]
	INIT(= N_("E437: Terminal capability \"cm\" required"));
EXTERN ಅಕ್ಷರ e_u_undo_ಸಾಲು_numbers_wrong[]
	INIT(= "E438: u_undo: Line numbers wrong");
EXTERN ಅಕ್ಷರ e_undo_list_corrupt[]
	INIT(= "E439: Undo list corrupt");
EXTERN ಅಕ್ಷರ e_undo_ಸಾಲು_missing[]
	INIT(= "E440: Undo line missing");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_there_is_no_preview_ಕಿಟಕಿ[]
	INIT(= N_("E441: There is no preview window"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cant_ವಿಭಜಿಸು_topleft_and_botright_at_the_same_ಸಮಯ[]
	INIT(= N_("E442: Can't split topleft and botright at the same time"));
EXTERN ಅಕ್ಷರ e_cannot_rotate_when_another_ಕಿಟಕಿ_is_ವಿಭಜಿಸು[]
	INIT(= N_("E443: Cannot rotate when another window is split"));
EXTERN ಅಕ್ಷರ e_cannot_close_ಕೊನೆಯ_ಕಿಟಕಿ[]
	INIT(= N_("E444: Cannot close last window"));
EXTERN ಅಕ್ಷರ e_other_ಕಿಟಕಿ_ಒಳಗೊಂಡಿದೆ_changes[]
	INIT(= N_("E445: Other window contains changes"));
EXTERN ಅಕ್ಷರ e_no_file_name_under_cursor[]
	INIT(= N_("E446: No file name under cursor"));
EXTERN ಅಕ್ಷರ e_cant_ಕಂಡುಹಿಡಿ_file_str_ಒಳ_ಮಾರ್ಗ_2[]
	INIT(= N_("E447: Can't find file \"%s\" in path"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USING_ತುಂಬು_LIBRARY
EXTERN ಅಕ್ಷರ e_could_not_ತುಂಬು_library_function_str[]
	INIT(= N_("E448: Could not load library function %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
EXTERN ಅಕ್ಷರ e_invalid_ಅಭಿವ್ಯಕ್ತಿ_received[]
	INIT(= N_("E449: Invalid expression received"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_ಬಫರ್_ಸಂಖ್ಯೆ_ಪಠ್ಯ_or_list_required[]
	INIT(= N_("E450: Buffer number, text or a list required"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_expected_ಬಲ_curly_str[]
	INIT(= N_("E451: Expected }: %s"));
EXTERN ಅಕ್ಷರ e_double_semicolon_ಒಳ_list_ದ_variables[]
	INIT(= N_("E452: Double ; in list of variables"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ul_color_unknown[]
	INIT(= N_("E453: UL color unknown"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_function_list_was_modified[]
	INIT(= N_("E454: Function list was modified"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_POSTSCRIPT
EXTERN ಅಕ್ಷರ e_error_writing_ಗಮ್ಯ_postscript_output_file[]
	INIT(= N_("E455: Error writing to PostScript output file"));
EXTERN ಅಕ್ಷರ e_cant_open_file_str_2[]
	INIT(= N_("E456: Can't open file \"%s\""));
EXTERN ಅಕ್ಷರ e_cant_ಕಂಡುಹಿಡಿ_postscript_resource_file_str_ps[]
	INIT(= N_("E456: Can't find PostScript resource file \"%s.ps\""));
EXTERN ಅಕ್ಷರ e_cant_read_postscript_resource_file_str[]
	INIT(= N_("E457: Can't read PostScript resource file \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ
// E458 unused
#ಆದರೆ defined(UNIX) || defined(FEAT_SESSION) || defined(FEAT_SOCKETSERVER)
EXTERN ಅಕ್ಷರ e_cannot_go_ಹಿಂಭಾಗ_ಗಮ್ಯ_ಹಿಂದಿನ_directory[]
	INIT(= N_("E459: Cannot go back to previous directory"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_entries_missing_ಒಳ_mapset_dict_ವಾದ[]
	INIT(= N_("E460: Entries missing in mapset() dict argument"));
EXTERN ಅಕ್ಷರ e_illegal_variable_name_str[]
	INIT(= N_("E461: Illegal variable name: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_could_not_prepare_for_reloading_str[]
	INIT(= N_("E462: Could not prepare for reloading \"%s\""));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
EXTERN ಅಕ್ಷರ e_region_is_guarded_cannot_modify[]
	INIT(= N_("E463: Region is guarded, cannot modify"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ambiguous_use_ದ_user_defined_command[]
	INIT(= N_("E464: Ambiguous use of user-defined command"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ambiguous_use_ದ_user_defined_command_str[]
	INIT(= N_("E464: Ambiguous use of user-defined command: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_winsize_requires_two_ಸಂಖ್ಯೆ_arguments[]
	INIT(= N_("E465: :winsize requires two number arguments"));
EXTERN ಅಕ್ಷರ e_winpos_requires_two_ಸಂಖ್ಯೆ_arguments[]
	INIT(= N_("E466: :winpos requires two number arguments"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_custom_completion_requires_function_ವಾದ[]
	INIT(= N_("E467: Custom completion requires a function argument"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_completion_ವಾದ_only_allowed_for_custom_completion[]
	INIT(= N_("E468: Completion argument only allowed for custom completion"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
EXTERN ಅಕ್ಷರ e_invalid_cscopequickfix_flag_chr_for_chr[]
	INIT(= N_("E469: Invalid cscopequickfix flag %c for %c"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_command_aborted[]
	INIT(= N_("E470: Command aborted"));
EXTERN ಅಕ್ಷರ e_ವಾದ_required[]
	INIT(= N_("E471: Argument required"));
EXTERN ಅಕ್ಷರ e_command_failed[]
	INIT(= N_("E472: Command failed"));
EXTERN ಅಕ್ಷರ e_ಆಂತರಿಕ_error_ಒಳ_regexp[]
	INIT(= "E473: Internal error in regexp");
EXTERN ಅಕ್ಷರ e_invalid_ವಾದ[]
	INIT(= N_("E474: Invalid argument"));
EXTERN ಅಕ್ಷರ e_invalid_ವಾದ_str[]
	INIT(= N_("E475: Invalid argument: %s"));
EXTERN ಅಕ್ಷರ e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str[]
	INIT(= N_("E475: Invalid value for argument %s"));
#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL) || defined(FEAT_PROP_POPUP) \
	|| defined(FEAT_EVAL) || defined(FEAT_IMAGE)
EXTERN ಅಕ್ಷರ e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str[]
	INIT(= N_("E475: Invalid value for argument %s: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_invalid_command[]
	INIT(= N_("E476: Invalid command"));
EXTERN ಅಕ್ಷರ e_invalid_command_str[]
	INIT(= N_("E476: Invalid command: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_invalid_command_str_expected_str[]
	INIT(= N_("E476: Invalid command: %s, expected %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_no_bang_allowed[]
	INIT(= N_("E477: No ! allowed"));
EXTERN ಅಕ್ಷರ e_dont_panic[]
	INIT(= N_("E478: Don't panic!"));
EXTERN ಅಕ್ಷರ e_no_ಹೊಂದಿಕೆ[]
	INIT(= N_("E479: No match"));
EXTERN ಅಕ್ಷರ e_no_ಹೊಂದಿಕೆ_str_2[]
	INIT(= N_("E480: No match: %s"));
EXTERN ಅಕ್ಷರ e_no_range_allowed[]
	INIT(= N_("E481: No range allowed"));
EXTERN ಅಕ್ಷರ e_cant_create_file_str[]
	INIT(= N_("E482: Can't create file %s"));
EXTERN ಅಕ್ಷರ e_cant_get_ತಾತ್ಕಾಲಿಕ_file_name[]
	INIT(= N_("E483: Can't get temp file name"));
EXTERN ಅಕ್ಷರ e_cant_open_file_str[]
	INIT(= N_("E484: Can't open file %s"));
EXTERN ಅಕ್ಷರ e_cant_read_file_str[]
	INIT(= N_("E485: Can't read file %s"));
EXTERN ಅಕ್ಷರ e_ನಮೂನೆ_not_ಸಿಕ್ಕಿತು[]
	INIT(= N_("E486: Pattern not found"));
EXTERN ಅಕ್ಷರ e_ನಮೂನೆ_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E486: Pattern not found: %s"));
EXTERN ಅಕ್ಷರ e_ವಾದ_must_be_ಧನಾತ್ಮಕ[]
	INIT(= N_("E487: Argument must be positive"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_ವಾದ_must_be_ಧನಾತ್ಮಕ_str[]
	INIT(= N_("E487: Argument must be positive: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_trailing_characters[]
	INIT(= N_("E488: Trailing characters"));
EXTERN ಅಕ್ಷರ e_trailing_characters_str[]
	INIT(= N_("E488: Trailing characters: %s"));
EXTERN ಅಕ್ಷರ e_no_call_ರಾಶಿ_ಗಮ್ಯ_substitute_for_ರಾಶಿ[]
	INIT(= N_("E489: No call stack to substitute for \"<stack>\""));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
EXTERN ಅಕ್ಷರ e_no_fold_ಸಿಕ್ಕಿತು[]
	INIT(= N_("E490: No fold found"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_json_decode_error_at_str[]
	INIT(= N_("E491: JSON decode error at '%s'"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_not_an_editor_command[]
	INIT(= N_("E492: Not an editor command"));
EXTERN ಅಕ್ಷರ e_backwards_range_given[]
	INIT(= N_("E493: Backwards range given"));
EXTERN ಅಕ್ಷರ e_use_w_or_w_gt_gt[]
	INIT(= N_("E494: Use w or w>>"));
EXTERN ಅಕ್ಷರ e_no_autocommand_file_name_ಗಮ್ಯ_substitute_for_afile[]
	INIT(= N_("E495: No autocommand file name to substitute for \"<afile>\""));
EXTERN ಅಕ್ಷರ e_no_autocommand_ಬಫರ್_ಸಂಖ್ಯೆ_ಗಮ್ಯ_substitute_for_abuf[]
	INIT(= N_("E496: No autocommand buffer number to substitute for \"<abuf>\""));
EXTERN ಅಕ್ಷರ e_no_autocommand_ಹೊಂದಿಕೆ_name_ಗಮ್ಯ_substitute_for_amatch[]
	INIT(= N_("E497: No autocommand match name to substitute for \"<amatch>\""));
EXTERN ಅಕ್ಷರ e_no_ಮೂಲ_file_name_ಗಮ್ಯ_substitute_for_sfile[]
	INIT(= N_("E498: No :source file name to substitute for \"<sfile>\""));
EXTERN ಅಕ್ಷರ e_ಖಾಲಿಗಳು_file_name_for_percent_or_hash_only_ಕೆಲಸಮಾಡುತ್ತದೆ_with_ph[]
	// xgettext:no-c-format
	INIT(= N_("E499: Empty file name for '%' or '#', only works with \":p:h\""));
EXTERN ಅಕ್ಷರ e_evaluates_ಗಮ್ಯ_an_ಖಾಲಿಗಳು_string[]
	INIT(= N_("E500: Evaluates to an empty string"));
EXTERN ಅಕ್ಷರ e_at_ಕೊನೆ_ದ_file[]
	INIT(= N_("E501: At end-of-file"));
	// E502
EXTERN ಅಕ್ಷರ e_is_a_directory[]
	INIT(= N_("is a directory"));
	// E503
EXTERN ಅಕ್ಷರ e_is_not_file_or_writable_device[]
	INIT(= N_("is not a file or writable device"));
EXTERN ಅಕ್ಷರ e_str_is_not_file_or_writable_device[]
	INIT(= N_("E503: \"%s\" is not a file or writable device"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_coffee_currently_not_ಲಭ್ಯ[]
	INIT(= N_("E503: Coffee is currently not available"));
#ಆದರೆ_ಅಂತ್ಯ
	// E504
EXTERN ಅಕ್ಷರ e_is_read_only_cannot_override_W_ಒಳ_cpoptions[]
	INIT(= N_("is read-only (cannot override: \"W\" in 'cpoptions')"));
	// E505
EXTERN ಅಕ್ಷರ e_is_read_only_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override[]
	INIT(= N_("is read-only (add ! to override)"));
EXTERN ಅಕ್ಷರ e_str_is_read_only_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override[]
	INIT(= N_("E505: \"%s\" is read-only (add ! to override)"));
EXTERN ಅಕ್ಷರ e_cant_write_ಗಮ್ಯ_backup_file_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override[]
	INIT(= N_("E506: Can't write to backup file (add ! to override)"));
EXTERN ಅಕ್ಷರ e_close_error_for_backup_file_ಕೂಡಿಸು_bang_ಗಮ್ಯ_write_anyway[]
	INIT(= N_("E507: Close error for backup file (add ! to write anyway)"));
EXTERN ಅಕ್ಷರ e_cant_read_file_for_backup_ಕೂಡಿಸು_bang_ಗಮ್ಯ_write_anyway[]
	INIT(= N_("E508: Can't read file for backup (add ! to write anyway)"));
EXTERN ಅಕ್ಷರ e_cannot_create_backup_file_ಕೂಡಿಸು_bang_ಗಮ್ಯ_write_anyway[]
	INIT(= N_("E509: Cannot create backup file (add ! to override)"));
EXTERN ಅಕ್ಷರ e_cant_make_backup_file_ಕೂಡಿಸು_bang_ಗಮ್ಯ_write_anyway[]
	INIT(= N_("E510: Can't make backup file (add ! to write anyway)"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
EXTERN ಅಕ್ಷರ e_netbeans_already_connected[]
	INIT(= N_("E511: NetBeans already connected"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_close_failed[]
	INIT(= N_("E512: Close failed"));
EXTERN ಅಕ್ಷರ e_write_error_conversion_failed_make_fenc_ಖಾಲಿಗಳು_ಗಮ್ಯ_override[]
	INIT(= N_("E513: Write error, conversion failed (make 'fenc' empty to override)"));
EXTERN ಅಕ್ಷರ e_write_error_conversion_failed_ಒಳ_ಸಾಲು_nr_make_fenc_ಖಾಲಿಗಳು_ಗಮ್ಯ_override[]
	INIT(= N_("E513: Write error, conversion failed in line %ld (make 'fenc' empty to override)"));
EXTERN ಅಕ್ಷರ e_write_error_file_system_ತುಂಬಿದ[]
	INIT(= N_("E514: Write error (file system full?)"));
EXTERN ಅಕ್ಷರ e_no_buffers_were_unloaded[]
	INIT(= N_("E515: No buffers were unloaded"));
EXTERN ಅಕ್ಷರ e_no_buffers_were_deleted[]
	INIT(= N_("E516: No buffers were deleted"));
EXTERN ಅಕ್ಷರ e_no_buffers_were_wiped_ಹೊರಹೋಗು[]
	INIT(= N_("E517: No buffers were wiped out"));
EXTERN ಅಕ್ಷರ e_unknown_option[]
	INIT(= N_("E518: Unknown option"));
EXTERN ಅಕ್ಷರ e_option_not_supported[]
	INIT(= N_("E519: Option not supported"));
EXTERN ಅಕ್ಷರ e_not_allowed_ಒಳ_modeline[]
	INIT(= N_("E520: Not allowed in a modeline"));
EXTERN ಅಕ್ಷರ e_ಸಂಖ್ಯೆ_required_after_equal[]
	INIT(= N_("E521: Number required after ="));
EXTERN ಅಕ್ಷರ e_ಸಂಖ್ಯೆ_required_after_str_equal_str[]
	INIT(= N_("E521: Number required: &%s = '%s'"));
EXTERN ಅಕ್ಷರ e_not_ಸಿಕ್ಕಿತು_ಒಳ_termcap[]
	INIT(= N_("E522: Not found in termcap"));
EXTERN ಅಕ್ಷರ e_not_allowed_ಇಲ್ಲಿ[]
	INIT(= N_("E523: Not allowed here"));
EXTERN ಅಕ್ಷರ e_missing_colon[]
	INIT(= N_("E524: Missing colon"));
EXTERN ಅಕ್ಷರ e_zero_ಉದ್ದಳತೆ_string[]
	INIT(= N_("E525: Zero length string"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
EXTERN ಅಕ್ಷರ e_missing_ಸಂಖ್ಯೆ_after_angle_str_angle[]
	INIT(= N_("E526: Missing number after <%s>"));
EXTERN ಅಕ್ಷರ e_missing_comma[]
	INIT(= N_("E527: Missing comma"));
EXTERN ಅಕ್ಷರ e_must_specify_a_ಮೌಲ್ಯ[]
	INIT(= N_("E528: Must specify a ' value"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_ಗಣ_ಪದ_ಗಮ್ಯ_ಖಾಲಿಗಳು_string[]
	INIT(= N_("E529: Cannot set 'term' to empty string"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN ಅಕ್ಷರ e_cannot_change_ಪದ_ಒಳ_GUI[]
	INIT(= N_("E530: Cannot change 'term' in the GUI"));
EXTERN ಅಕ್ಷರ e_use_gui_ಗಮ್ಯ_ಆರಂಭ_GUI[]
	INIT(= N_("E531: Use \":gui\" to start the GUI"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
EXTERN ಅಕ್ಷರ e_highlighting_color_name_too_long_ಒಳ_define_Anno_Type[]
	INIT(= N_("E532: Highlighting color name too long in defineAnnoType"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN ಅಕ್ಷರ e_cant_select_wide_font[]
	INIT(= N_("E533: Can't select wide font"));
EXTERN ಅಕ್ಷರ e_invalid_wide_font[]
	INIT(= N_("E534: Invalid wide font"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_illegal_character_after_chr[]
	INIT(= N_("E535: Illegal character after <%c>"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
EXTERN ಅಕ್ಷರ e_comma_required[]
	INIT(= N_("E536: Comma required"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_commentstring_must_be_ಖಾಲಿಗಳು_or_contain_str[]
	INIT(= N_("E537: 'commentstring' must be empty or contain %s"));
EXTERN ಅಕ್ಷರ e_ನಮೂನೆ_ಸಿಕ್ಕಿತು_ಒಳ_every_ಸಾಲು_str[]
	INIT(= N_("E538: Pattern found in every line: %s"));
EXTERN ಅಕ್ಷರ e_illegal_character_str[]
	INIT(= N_("E539: Illegal character <%s>"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
EXTERN ಅಕ್ಷರ e_unclosed_ಅಭಿವ್ಯಕ್ತಿ_sequence[]
	INIT(= N_("E540: Unclosed expression sequence"));
// E541 unused
EXTERN ಅಕ್ಷರ e_unbalanced_groups[]
	INIT(= N_("E542: Unbalanced groups"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
EXTERN ಅಕ್ಷರ e_not_valid_codepage[]
	INIT(= N_("E543: Not a valid codepage"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
EXTERN ಅಕ್ಷರ e_keymap_file_not_ಸಿಕ್ಕಿತು[]
	INIT(= N_("E544: Keymap file not found"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
EXTERN ಅಕ್ಷರ e_missing_colon_2[]
	INIT(= N_("E545: Missing colon"));
EXTERN ಅಕ್ಷರ e_illegal_ಬಹುಲಕ[]
	INIT(= N_("E546: Illegal mode"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
EXTERN ಅಕ್ಷರ e_illegal_mouseshape[]
	INIT(= N_("E547: Illegal mouseshape"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURSOR_SHAPE
EXTERN ಅಕ್ಷರ e_digit_expected[]
	INIT(= N_("E548: Digit expected"));
EXTERN ಅಕ್ಷರ e_illegal_percentage[]
	INIT(= N_("E549: Illegal percentage"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PRINTER
EXTERN ಅಕ್ಷರ e_missing_colon_3[]
	INIT(= N_("E550: Missing colon"));
EXTERN ಅಕ್ಷರ e_illegal_component[]
	INIT(= N_("E551: Illegal component"));
EXTERN ಅಕ್ಷರ e_digit_expected_2[]
	INIT(= N_("E552: Digit expected"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_no_ಇನ್ನಷ್ಟು_ವಸ್ತುಗಳು[]
	INIT(= N_("E553: No more items"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_syntax_error_ಒಳ_str_curlies[]
	INIT(= N_("E554: Syntax error in %s{...}"));
EXTERN ಅಕ್ಷರ e_at_bottom_ದ_ಟ್ಯಾಗ್_ರಾಶಿ[]
	INIT(= N_("E555: At bottom of tag stack"));
EXTERN ಅಕ್ಷರ e_at_ತುದಿ_ದ_ಟ್ಯಾಗ್_ರಾಶಿ[]
	INIT(= N_("E556: At top of tag stack"));
EXTERN ಅಕ್ಷರ e_cannot_open_termcap_file[]
	INIT(= N_("E557: Cannot open termcap file"));
EXTERN ಅಕ್ಷರ e_terminal_ನಮೂದು_not_ಸಿಕ್ಕಿತು_ಒಳ_terminfo[]
	INIT(= N_("E558: Terminal entry not found in terminfo"));
#ಆದರೆ defined(HAVE_TGETENT) && !defined(TERMINFO)
EXTERN ಅಕ್ಷರ e_terminal_ನಮೂದು_not_ಸಿಕ್ಕಿತು_ಒಳ_termcap[]
	INIT(= N_("E559: Terminal entry not found in termcap"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
EXTERN ಅಕ್ಷರ e_usage_cscope_str[]
	INIT(= N_("E560: Usage: cs[cope] %s"));
EXTERN ಅಕ್ಷರ e_unknown_cscope_ಹುಡುಕು_type[]
	INIT(= N_("E561: Unknown cscope search type"));
EXTERN ಅಕ್ಷರ e_usage_cstag_ident[]
	INIT(= N_("E562: Usage: cstag <ident>"));
EXTERN ಅಕ್ಷರ e_stat_str_error_nr[]
	INIT(= N_("E563: stat(%s) error: %d"));
EXTERN ಅಕ್ಷರ e_str_is_not_directory_or_valid_cscope_database[]
	INIT(= N_("E564: %s is not a directory or a valid cscope database"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_not_allowed_ಗಮ್ಯ_change_ಪಠ್ಯ_or_change_ಕಿಟಕಿ[]
	INIT(= N_("E565: Not allowed to change text or change window"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
EXTERN ಅಕ್ಷರ e_could_not_create_cscope_pipes[]
	INIT(= N_("E566: Could not create cscope pipes"));
EXTERN ಅಕ್ಷರ e_no_cscope_connections[]
	INIT(= N_("E567: No cscope connections"));
EXTERN ಅಕ್ಷರ e_duplicate_cscope_database_not_added[]
	INIT(= N_("E568: Duplicate cscope database not added"));
// E569 unused
EXTERN ಅಕ್ಷರ e_fatal_error_ಒಳ_cs_manage_matches[]
	INIT(= "E570: Fatal error in cs_manage_matches");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_TCL
EXTERN ಅಕ್ಷರ e_sorry_this_command_is_disabled_tcl_library_could_not_be_loaded[]
	INIT(= N_("E571: Sorry, this command is disabled: the Tcl library could not be loaded."));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TCL
EXTERN ಅಕ್ಷರ e_exit_ಸಂಕೇತ_nr[]
	INIT(= N_("E572: Exit code %d"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
EXTERN ಅಕ್ಷರ e_invalid_server_id_ಬಳಸಿದ_str[]
	INIT(= N_("E573: Invalid server id used: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
EXTERN ಅಕ್ಷರ e_unknown_ರಿಜಿಸ್ಟರ್_type_nr[]
	INIT(= N_("E574: Unknown register type %d"));
	// E575
EXTERN ಅಕ್ಷರ e_illegal_starting_char[]
	INIT(= N_("Illegal starting char"));
	// E576
EXTERN ಅಕ್ಷರ e_nonr_missing_gt[]
	INIT(= N_("Missing '>'"));
	// E577
EXTERN ಅಕ್ಷರ e_illegal_ರಿಜಿಸ್ಟರ್_name[]
	INIT(= N_("Illegal register name"));
#ಆದರೆ_ಅಂತ್ಯ
// E578 unused
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_if_nesting_too_deep[]
	INIT(= N_("E579: :if nesting too deep"));
EXTERN ಅಕ್ಷರ e_ಖಂಡ_nesting_too_deep[]
	INIT(= N_("E579: Block nesting too deep"));
EXTERN ಅಕ್ಷರ e_endif_without_if[]
	INIT(= N_("E580: :endif without :if"));
EXTERN ಅಕ್ಷರ e_else_without_if[]
	INIT(= N_("E581: :else without :if"));
EXTERN ಅಕ್ಷರ e_elseif_without_if[]
	INIT(= N_("E582: :elseif without :if"));
EXTERN ಅಕ್ಷರ e_ಅಪವರ್ತ್ಯ_else[]
	INIT(= N_("E583: Multiple :else"));
EXTERN ಅಕ್ಷರ e_elseif_after_else[]
	INIT(= N_("E584: :elseif after :else"));
EXTERN ಅಕ್ಷರ e_while_for_nesting_too_deep[]
	INIT(= N_("E585: :while/:for nesting too deep"));
EXTERN ಅಕ್ಷರ e_continue_without_while_or_for[]
	INIT(= N_("E586: :continue without :while or :for"));
EXTERN ಅಕ್ಷರ e_break_without_while_or_for[]
	INIT(= N_("E587: :break without :while or :for"));
EXTERN ಅಕ್ಷರ e_endwhile_without_while[]
	INIT(= N_("E588: :endwhile without :while"));
EXTERN ಅಕ್ಷರ e_endfor_without_for[]
	INIT(= N_("E588: :endfor without :for"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_backupext_and_patchmode_are_equal[]
	INIT(= N_("E589: 'backupext' and 'patchmode' are equal"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_preview_ಕಿಟಕಿ_already_exists[]
	INIT(= N_("E590: A preview window already exists"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_winheight_cannot_be_smaller_than_winminheight[]
	INIT(= N_("E591: 'winheight' cannot be smaller than 'winminheight'"));
EXTERN ಅಕ್ಷರ e_winwidth_cannot_be_smaller_than_winminwidth[]
	INIT(= N_("E592: 'winwidth' cannot be smaller than 'winminwidth'"));
EXTERN ಅಕ್ಷರ e_need_at_ಕನಿಷ್ಠ_nr_ಸಾಲುಗಳು[]
	INIT(= N_("E593: Need at least %d lines"));
EXTERN ಅಕ್ಷರ e_need_at_ಕನಿಷ್ಠ_nr_columns[]
	INIT(= N_("E594: Need at least %d columns"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LINEBREAK
EXTERN ಅಕ್ಷರ e_showbreak_ಒಳಗೊಂಡಿದೆ_unprintable_or_wide_character[]
	INIT(= N_("E595: 'showbreak' contains unprintable or wide character"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN ಅಕ್ಷರ e_invalid_fonts[]
	INIT(= N_("E596: Invalid font(s)"));
# ifdef FEAT_XFONTSET
EXTERN ಅಕ್ಷರ e_cant_select_fontset[]
	INIT(= N_("E597: Can't select fontset"));
EXTERN ಅಕ್ಷರ e_invalid_fontset[]
	INIT(= N_("E598: Invalid fontset"));
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
EXTERN ಅಕ್ಷರ e_ಮೌಲ್ಯ_ದ_imactivatekey_is_invalid[]
	INIT(= N_("E599: Value of 'imactivatekey' is invalid"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_missing_endtry[]
	INIT(= N_("E600: Missing :endtry"));
EXTERN ಅಕ್ಷರ e_try_nesting_too_deep[]
	INIT(= N_("E601: :try nesting too deep"));
EXTERN ಅಕ್ಷರ e_endtry_without_try[]
	INIT(= N_("E602: :endtry without :try"));
EXTERN ಅಕ್ಷರ e_catch_without_try[]
	INIT(= N_("E603: :catch without :try"));
EXTERN ಅಕ್ಷರ e_catch_after_finally[]
	INIT(= N_("E604: :catch after :finally"));
EXTERN ಅಕ್ಷರ e_exception_not_caught_str[]
	INIT(= N_("E605: Exception not caught: %s"));
EXTERN ಅಕ್ಷರ e_finally_without_try[]
	INIT(= N_("E606: :finally without :try"));
EXTERN ಅಕ್ಷರ e_ಅಪವರ್ತ್ಯ_finally[]
	INIT(= N_("E607: Multiple :finally"));
EXTERN ಅಕ್ಷರ e_cannot_throw_exceptions_with_vim_ಪೂರ್ವಪ್ರತ್ಯಯ[]
	INIT(= N_("E608: Cannot :throw exceptions with 'Vim' prefix"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
EXTERN ಅಕ್ಷರ e_cscope_error_str[]
	INIT(= N_("E609: Cscope error: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_no_ವಾದ_ಗಮ್ಯ_delete[]
	INIT(= N_("E610: No argument to delete"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_using_special_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E611: Using a Special as a Number"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
EXTERN ಅಕ್ಷರ e_too_many_signs_defined[]
	INIT(= N_("E612: Too many signs defined"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PRINTER) && (defined(MSWIN) || defined(FEAT_PRINT_PANGO))
EXTERN ಅಕ್ಷರ e_unknown_printer_font_str[]
	INIT(= N_("E613: Unknown printer font: %s"));
#ಆದರೆ_ಅಂತ್ಯ
// E614 unused (deleted)
// E615 unused
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_object_required_for_ವಾದ_nr[]
	INIT(= N_("E616: Object required for argument %d"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
EXTERN ಅಕ್ಷರ e_cannot_be_ಬದಲಾಯಿತು_ಒಳ_gtk_GUI[]
	INIT(= N_("E617: Cannot be changed in the GTK GUI"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_POSTSCRIPT
EXTERN ಅಕ್ಷರ e_file_str_is_not_postscript_resource_file[]
	INIT(= N_("E618: File \"%s\" is not a PostScript resource file"));
EXTERN ಅಕ್ಷರ e_file_str_is_not_supported_postscript_resource_file[]
	INIT(= N_("E619: File \"%s\" is not a supported PostScript resource file"));
EXTERN ಅಕ್ಷರ e_unable_ಗಮ್ಯ_convert_ಗಮ್ಯ_print_encoding_str[]
	INIT(= N_("E620: Unable to convert to print encoding \"%s\""));
EXTERN ಅಕ್ಷರ e_str_resource_file_has_wrong_ಆವೃತ್ತಿ[]
	INIT(= N_("E621: \"%s\" resource file has wrong version"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CSCOPE
EXTERN ಅಕ್ಷರ e_could_not_fork_for_cscope[]
	INIT(= N_("E622: Could not fork for cscope"));
# ifndef UNIX
EXTERN ಅಕ್ಷರ e_could_not_spawn_cscope_process[]
	INIT(= N_("E623: Could not spawn cscope process"));
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PRINTER) && defined(FEAT_POSTSCRIPT)
EXTERN ಅಕ್ಷರ e_cant_open_file_str_3[]
	INIT(= N_("E624: Can't open file \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CSCOPE) && !defined(UNIX)
EXTERN ಅಕ್ಷರ e_cannot_open_cscope_database_str[]
	INIT(= N_("E625: Cannot open cscope database: %s"));
EXTERN ಅಕ್ಷರ e_cannot_get_cscope_database_information[]
	INIT(= N_("E626: Cannot get cscope database information"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
EXTERN ಅಕ್ಷರ e_missing_colon_str[]
	INIT(= N_("E627: Missing colon: %s"));
EXTERN ಅಕ್ಷರ e_missing_bang_or_slash_ಒಳ_str[]
	INIT(= N_("E628: Missing ! or / in: %s"));
# ifdef NBDEBUG
EXTERN ಅಕ್ಷರ e_bad_return_from_nb_do_cmd[]
	INIT(= "E629: Bad return from nb_do_cmd");
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
EXTERN ಅಕ್ಷರ e_str_write_while_not_connected[]
	INIT(= N_("E630: %s(): Write while not connected"));
EXTERN ಅಕ್ಷರ e_str_write_failed[]
	INIT(= N_("E631: %s(): Write failed"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_getlength[]
	INIT(= N_("E632: Invalid buffer identifier in getLength"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_gettext[]
	INIT(= N_("E633: Invalid buffer identifier in getText"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_remove[]
	INIT(= N_("E634: Invalid buffer identifier in remove"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_ಸೇರಿಸು[]
	INIT(= N_("E635: Invalid buffer identifier in insert"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_create[]
	INIT(= N_("E636: Invalid buffer identifier in create"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_startdocumentlisten[]
	INIT(= N_("E637: Invalid buffer identifier in startDocumentListen"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_stopdocumentlisten[]
	INIT(= N_("E638: Invalid buffer identifier in stopDocumentListen"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_settitle[]
	INIT(= N_("E639: Invalid buffer identifier in setTitle"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_initdone[]
	INIT(= N_("E640: Invalid buffer identifier in initDone"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_setbuffernumber[]
	INIT(= N_("E641: Invalid buffer identifier in setBufferNumber"));
EXTERN ಅಕ್ಷರ e_file_str_not_ಸಿಕ್ಕಿತು_ಒಳ_setbuffernumber[]
	INIT(= N_("E642: File %s not found in setBufferNumber"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_setfullname[]
	INIT(= N_("E643: Invalid buffer identifier in setFullName"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_editfile[]
	INIT(= N_("E644: Invalid buffer identifier in editFile"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_setvisible[]
	INIT(= N_("E645: Invalid buffer identifier in setVisible"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_setmodified[]
	INIT(= N_("E646: Invalid buffer identifier in setModified"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_setdot[]
	INIT(= N_("E647: Invalid buffer identifier in setDot"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_close[]
	INIT(= N_("E648: Invalid buffer identifier in close"));
EXTERN ಅಕ್ಷರ e_invalid_identifier_ಒಳ_defineannotype[]
	INIT(= N_("E649: Invalid identifier name in defineAnnoType"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_defineannotype[]
	INIT(= N_("E650: Invalid buffer identifier in defineAnnoType"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_addanno[]
	INIT(= N_("E651: Invalid buffer identifier in addAnno"));
EXTERN ಅಕ್ಷರ e_invalid_ಬಫರ್_identifier_ಒಳ_getanno[]
	INIT(= N_("E652: Invalid buffer identifier in getAnno"));
#ಆದರೆ_ಅಂತ್ಯ
// E653 unused
EXTERN ಅಕ್ಷರ e_missing_delimiter_after_ಹುಡುಕು_ನಮೂನೆ_str[]
	INIT(= N_("E654: Missing delimiter after search pattern: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_too_many_symbolic_links_ಚಕ್ರ[]
	INIT(= N_("E655: Too many symbolic links (cycle?)"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	// E656
EXTERN ಅಕ್ಷರ e_netbeans_disallows_writes_ದ_unmodified_buffers[]
	INIT(= N_("NetBeans disallows writes of unmodified buffers"));
	// E657
EXTERN ಅಕ್ಷರ e_partial_writes_disallowed_for_netbeans_buffers[]
	INIT(= N_("Partial writes disallowed for NetBeans buffers"));
EXTERN ಅಕ್ಷರ e_netbeans_connection_ಕಳೆದುಹೋದ_for_ಬಫರ್_nr[]
	INIT(= N_("E658: NetBeans connection lost for buffer %d"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
EXTERN ಅಕ್ಷರ e_cannot_invoke_python_recursively[]
	INIT(= N_("E659: Cannot invoke Python recursively"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
EXTERN ಅಕ್ಷರ e_cannot_open_netbeans_connection_info_file[]
	INIT(= "E660: Cannot open NetBeans connection info file");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MULTI_LANG
EXTERN ಅಕ್ಷರ e_sorry_no_str_help_for_str[]
	INIT(= N_("E661: Sorry, no '%s' help for %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_at_ಆರಂಭ_ದ_changelist[]
	INIT(= N_("E662: At start of changelist"));
EXTERN ಅಕ್ಷರ e_at_ಕೊನೆ_ದ_changelist[]
	INIT(= N_("E663: At end of changelist"));
EXTERN ಅಕ್ಷರ e_changelist_is_ಖಾಲಿಗಳು[]
	INIT(= N_("E664: Changelist is empty"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN ಅಕ್ಷರ e_cannot_ಆರಂಭ_gui_no_valid_font_ಸಿಕ್ಕಿತು[]
	INIT(= N_("E665: Cannot start GUI, no valid font found"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_compiler_not_supported_str[]
	INIT(= N_("E666: Compiler not supported: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
EXTERN ಅಕ್ಷರ e_fsync_failed[]
	INIT(= N_("E667: Fsync failed"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
EXTERN ಅಕ್ಷರ e_wrong_access_ಬಹುಲಕ_for_netbeans_connection_info_file_str[]
	INIT(= N_("E668: Wrong access mode for NetBeans connection info file: \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_unprintable_character_ಒಳ_ಗುಂಪು_name[]
	INIT(= N_("E669: Unprintable character in group name"));
EXTERN ಅಕ್ಷರ e_mix_ದ_help_file_encodings_within_language_str[]
	INIT(= N_("E670: Mix of help file encodings within a language: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MSWIN
EXTERN ಅಕ್ಷರ e_cannot_ಕಂಡುಹಿಡಿ_ಕಿಟಕಿ_title_str[]
	INIT(= N_("E671: Cannot find window title \"%s\""));
EXTERN ಅಕ್ಷರ e_unable_ಗಮ್ಯ_open_ಕಿಟಕಿ_inside_mdi_application[]
	INIT(= N_("E672: Unable to open window inside MDI application"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PRINTER) && defined(FEAT_POSTSCRIPT)
EXTERN ಅಕ್ಷರ e_incompatible_multi_byte_encoding_and_character_ಗಣ[]
	INIT(= N_("E673: Incompatible multi-byte encoding and character set"));
EXTERN ಅಕ್ಷರ e_printmbcharset_cannot_be_ಖಾಲಿಗಳು_with_multi_byte_encoding[]
	INIT(= N_("E674: printmbcharset cannot be empty with multi-byte encoding."));
EXTERN ಅಕ್ಷರ e_no_default_font_specified_for_multi_byte_printing[]
	INIT(= N_("E675: No default font specified for multi-byte printing."));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_no_matching_autocommands_for_buftype_str_ಬಫರ್[]
	INIT(= N_("E676: No matching autocommands for buftype=%s buffer"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಅಕ್ಷರ e_error_writing_ತಾತ್ಕಾಲಿಕ_file[]
	INIT(= N_("E677: Error writing temp file"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_invalid_character_after_str_2[]
	INIT(= N_("E678: Invalid character after %s%%[dxouU]"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಅಕ್ಷರ e_recursive_loop_loading_syncolor_vim[]
	INIT(= N_("E679: Recursive loop loading syncolor.vim"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಬಫರ್_nr_invalid_ಬಫರ್_ಸಂಖ್ಯೆ[]
	INIT(= N_("E680: <buffer=%d>: invalid buffer number"));
#ಆದರೆ defined(FEAT_QUICKFIX) || defined(FEAT_EVAL)
EXTERN ಅಕ್ಷರ e_ಬಫರ್_is_not_loaded[]
	INIT(= N_("E681: Buffer is not loaded"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_invalid_ಹುಡುಕು_ನಮೂನೆ_or_delimiter[]
	INIT(= N_("E682: Invalid search pattern or delimiter"));
EXTERN ಅಕ್ಷರ e_file_name_missing_or_invalid_ನಮೂನೆ[]
	INIT(= N_("E683: File name missing or invalid pattern"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr[]
	INIT(= N_("E684: List index out of range: %ld"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಆಂತರಿಕ_error_str[]
	INIT(= N_("E685: Internal error: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ವಾದ_ದ_str_must_be_list[]
	INIT(= N_("E686: Argument of %s must be a List"));
EXTERN ಅಕ್ಷರ e_less_targets_than_list_ವಸ್ತುಗಳು[]
	INIT(= N_("E687: Less targets than List items"));
EXTERN ಅಕ್ಷರ e_ಇನ್ನಷ್ಟು_targets_than_list_ವಸ್ತುಗಳು[]
	INIT(= N_("E688: More targets than List items"));
EXTERN ಅಕ್ಷರ e_ಸೂಚ್ಯಂಕ_not_allowed_after_str_str[]
	INIT(= N_("E689: Index not allowed after a %s: %s"));
EXTERN ಅಕ್ಷರ e_missing_ಒಳ_after_for[]
	INIT(= N_("E690: Missing \"in\" after :for"));
EXTERN ಅಕ್ಷರ e_can_only_compare_list_with_list[]
	INIT(= N_("E691: Can only compare List with List"));
EXTERN ಅಕ್ಷರ e_invalid_operation_for_list[]
	INIT(= N_("E692: Invalid operation for List"));
EXTERN ಅಕ್ಷರ e_class_or_typealias_required_for_ವಾದ_nr[]
	INIT(= N_("E693: Class or class typealias required for argument %d"));
EXTERN ಅಕ್ಷರ e_invalid_operation_for_funcrefs[]
	INIT(= N_("E694: Invalid operation for Funcrefs"));
EXTERN ಅಕ್ಷರ e_cannot_ಸೂಚ್ಯಂಕ_a_funcref[]
	INIT(= N_("E695: Cannot index a Funcref"));
EXTERN ಅಕ್ಷರ e_missing_comma_ಒಳ_list_str[]
	INIT(= N_("E696: Missing comma in List: %s"));
EXTERN ಅಕ್ಷರ e_missing_ಕೊನೆ_ದ_list_rsb_str[]
	INIT(= N_("E697: Missing end of List ']': %s"));
EXTERN ಅಕ್ಷರ e_variable_nested_too_deep_for_making_copy[]
	INIT(= N_("E698: Variable nested too deep for making a copy"));
EXTERN ಅಕ್ಷರ e_too_many_arguments[]
	INIT(= N_("E699: Too many arguments"));
EXTERN ಅಕ್ಷರ e_unknown_function_str_2[]
	INIT(= N_("E700: Unknown function: %s"));
EXTERN ಅಕ್ಷರ e_invalid_type_for_len[]
	INIT(= N_("E701: Invalid type for len()"));
EXTERN ಅಕ್ಷರ e_ವಿಂಗಡಿಸು_compare_function_failed[]
	INIT(= N_("E702: Sort compare function failed"));
EXTERN ಅಕ್ಷರ e_using_funcref_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E703: Using a Funcref as a Number"));
EXTERN ಅಕ್ಷರ e_funcref_variable_name_must_ಆರಂಭ_with_capital_str[]
	INIT(= N_("E704: Funcref variable name must start with a capital: %s"));
EXTERN ಅಕ್ಷರ e_variable_name_conflicts_with_existing_function_str[]
	INIT(= N_("E705: Variable name conflicts with existing function: %s"));
EXTERN ಅಕ್ಷರ e_ವಾದ_ದ_str_must_be_list_string_or_dictionary[]
	INIT(= N_("E706: Argument of %s must be a List, String or Dictionary"));
EXTERN ಅಕ್ಷರ e_function_name_conflicts_with_variable_str[]
	INIT(= N_("E707: Function name conflicts with variable: %s"));
EXTERN ಅಕ್ಷರ e_ತುಂಡು_must_come_ಕೊನೆಯ[]
	INIT(= N_("E708: [:] must come last"));
EXTERN ಅಕ್ಷರ e_ತುಂಡು_requires_list_or_blob_ಮೌಲ್ಯ[]
	INIT(= N_("E709: [:] requires a List or Blob value"));
EXTERN ಅಕ್ಷರ e_list_ಮೌಲ್ಯ_has_ಇನ್ನಷ್ಟು_ವಸ್ತುಗಳು_than_targets[]
	INIT(= N_("E710: List value has more items than targets"));
EXTERN ಅಕ್ಷರ e_list_ಮೌಲ್ಯ_does_not_have_enough_ವಸ್ತುಗಳು[]
	INIT(= N_("E711: List value does not have enough items"));
EXTERN ಅಕ್ಷರ e_ವಾದ_ದ_str_must_be_list_or_dictionary[]
	INIT(= N_("E712: Argument of %s must be a List or Dictionary"));
EXTERN ಅಕ್ಷರ e_cannot_use_ಖಾಲಿಗಳು_ಕೀಲಿ_for_dictionary[]
	INIT(= N_("E713: Cannot use empty key for Dictionary"));
EXTERN ಅಕ್ಷರ e_list_required[]
	INIT(= N_("E714: List required"));
EXTERN ಅಕ್ಷರ e_dictionary_required[]
	INIT(= N_("E715: Dictionary required"));
EXTERN ಅಕ್ಷರ e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str[]
	INIT(= N_("E716: Key not present in Dictionary: \"%s\""));
EXTERN ಅಕ್ಷರ e_dictionary_ನಮೂದು_already_exists[]
	INIT(= N_("E717: Dictionary entry already exists"));
EXTERN ಅಕ್ಷರ e_funcref_required[]
	INIT(= N_("E718: Funcref required"));
EXTERN ಅಕ್ಷರ e_cannot_ತುಂಡು_dictionary[]
	INIT(= N_("E719: Cannot slice a Dictionary"));
EXTERN ಅಕ್ಷರ e_missing_colon_ಒಳ_dictionary_str[]
	INIT(= N_("E720: Missing colon in Dictionary: %s"));
EXTERN ಅಕ್ಷರ e_duplicate_ಕೀಲಿ_ಒಳ_dictionary_str[]
	INIT(= N_("E721: Duplicate key in Dictionary: \"%s\""));
EXTERN ಅಕ್ಷರ e_missing_comma_ಒಳ_dictionary_str[]
	INIT(= N_("E722: Missing comma in Dictionary: %s"));
EXTERN ಅಕ್ಷರ e_missing_dict_ಕೊನೆ_str[]
	INIT(= N_("E723: Missing end of Dictionary '}': %s"));
EXTERN ಅಕ್ಷರ e_variable_nested_too_deep_for_displaying[]
	INIT(= N_("E724: Variable nested too deep for displaying"));
EXTERN ಅಕ್ಷರ e_calling_dict_function_without_dictionary_str[]
	INIT(= N_("E725: Calling dict function without Dictionary: %s"));
EXTERN ಅಕ್ಷರ e_stride_is_zero[]
	INIT(= N_("E726: Stride is zero"));
EXTERN ಅಕ್ಷರ e_ಆರಂಭ_past_ಕೊನೆ[]
	INIT(= N_("E727: Start past end"));
EXTERN ಅಕ್ಷರ e_using_dictionary_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E728: Using a Dictionary as a Number"));
EXTERN ಅಕ್ಷರ e_using_funcref_as_string[]
	INIT(= N_("E729: Using a Funcref as a String"));
EXTERN ಅಕ್ಷರ e_using_list_as_string[]
	INIT(= N_("E730: Using a List as a String"));
EXTERN ಅಕ್ಷರ e_using_dictionary_as_string[]
	INIT(= N_("E731: Using a Dictionary as a String"));
EXTERN ಅಕ್ಷರ e_using_endfor_with_while[]
	INIT(= N_("E732: Using :endfor with :while"));
EXTERN ಅಕ್ಷರ e_using_endwhile_with_for[]
	INIT(= N_("E733: Using :endwhile with :for"));
EXTERN ಅಕ್ಷರ e_wrong_variable_type_for_str_equal[]
	INIT(= N_("E734: Wrong variable type for %s="));
EXTERN ಅಕ್ಷರ e_can_only_compare_dictionary_with_dictionary[]
	INIT(= N_("E735: Can only compare Dictionary with Dictionary"));
EXTERN ಅಕ್ಷರ e_invalid_operation_for_dictionary[]
	INIT(= N_("E736: Invalid operation for Dictionary"));
EXTERN ಅಕ್ಷರ e_ಕೀಲಿ_already_exists_str[]
	INIT(= N_("E737: Key already exists: %s"));
EXTERN ಅಕ್ಷರ e_cant_list_variables_for_str[]
	INIT(= N_("E738: Can't list variables for %s"));
EXTERN ಅಕ್ಷರ e_cannot_create_directory_str[]
	INIT(= N_("E739: Cannot create directory: %s"));
EXTERN ಅಕ್ಷರ e_too_many_arguments_for_function_str_2[]
	INIT(= N_("E740: Too many arguments for function %s"));
EXTERN ಅಕ್ಷರ e_ಮೌಲ್ಯ_is_locked[]
	INIT(= N_("E741: Value is locked"));
EXTERN ಅಕ್ಷರ e_ಮೌಲ್ಯ_is_locked_str[]
	INIT(= N_("E741: Value is locked: %s"));
EXTERN ಅಕ್ಷರ e_cannot_change_ಮೌಲ್ಯ[]
	INIT(= N_("E742: Cannot change value"));
EXTERN ಅಕ್ಷರ e_cannot_change_ಮೌಲ್ಯ_ದ_str[]
	INIT(= N_("E742: Cannot change value of %s"));
EXTERN ಅಕ್ಷರ e_variable_nested_too_deep_for_unlock[]
	INIT(= N_("E743: Variable nested too deep for (un)lock"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
EXTERN ಅಕ್ಷರ e_netbeans_does_not_allow_changes_ಒಳ_read_only_files[]
	INIT(= N_("E744: NetBeans does not allow changes in read-only files"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_using_list_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E745: Using a List as a Number"));
EXTERN ಅಕ್ಷರ e_function_name_does_not_ಹೊಂದಿಕೆ_script_file_name_str[]
	INIT(= N_("E746: Function name does not match script file name: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_change_directory_ಬಫರ್_is_modified_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override[]
	INIT(= N_("E747: Cannot change directory, buffer is modified (add ! to override)"));
EXTERN ಅಕ್ಷರ e_no_previously_ಬಳಸಿದ_ರಿಜಿಸ್ಟರ್[]
	INIT(= N_("E748: No previously used register"));
EXTERN ಅಕ್ಷರ e_ಖಾಲಿಗಳು_ಬಫರ್[]
	INIT(= N_("E749: Empty buffer"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
EXTERN ಅಕ್ಷರ e_ಮೊದಲ_use_profile_ಆರಂಭ_fname[]
	INIT(= N_("E750: First use \":profile start {fname}\""));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
EXTERN ಅಕ್ಷರ e_output_file_name_must_not_have_region_name[]
	INIT(= N_("E751: Output file name must not have region name"));
EXTERN ಅಕ್ಷರ e_no_ಹಿಂದಿನ_spell_replacement[]
	INIT(= N_("E752: No previous spell replacement"));
EXTERN ಅಕ್ಷರ e_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E753: Not found: %s"));
EXTERN ಅಕ್ಷರ e_only_ಮೇಲೆ_ಗಮ್ಯ_nr_regions_supported[]
	INIT(= N_("E754: Only up to %d regions supported"));
EXTERN ಅಕ್ಷರ e_invalid_region_ಒಳ_str[]
	INIT(= N_("E755: Invalid region in %s"));
EXTERN ಅಕ್ಷರ e_spell_checking_is_not_possible[]
	INIT(= N_("E756: Spell checking is not possible"));
EXTERN ಅಕ್ಷರ e_this_does_not_look_like_spell_file[]
	INIT(= N_("E757: This does not look like a spell file"));
EXTERN ಅಕ್ಷರ e_truncated_spell_file[]
	INIT(= N_("E758: Truncated spell file"));
EXTERN ಅಕ್ಷರ e_format_error_ಒಳ_spell_file[]
	INIT(= N_("E759: Format error in spell file"));
EXTERN ಅಕ್ಷರ e_no_ಶಬ್ದ_ಎಣಿಕೆ_ಒಳ_str[]
	INIT(= N_("E760: No word count in %s"));
EXTERN ಅಕ್ಷರ e_format_error_ಒಳ_affix_file_fol_ಕೆಳಗಿನ_or_upp[]
	INIT(= N_("E761: Format error in affix file FOL, LOW or UPP"));
EXTERN ಅಕ್ಷರ e_character_ಒಳ_fol_ಕೆಳಗಿನ_or_upp_is_ಹೊರಹೋಗು_ದ_range[]
	INIT(= N_("E762: Character in FOL, LOW or UPP is out of range"));
EXTERN ಅಕ್ಷರ e_ಶಬ್ದ_characters_ವ್ಯತ್ಯಾಸ_between_spell_files[]
	INIT(= N_("E763: Word characters differ between spell files"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_SYN_HL) || defined(FEAT_COMPL_FUNC) || defined(FEAT_SPELL)
EXTERN ಅಕ್ಷರ e_option_str_is_not_ಗಣ[]
	INIT(= N_("E764: Option '%s' is not set"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
EXTERN ಅಕ್ಷರ e_spellfile_does_not_have_nr_entries[]
	INIT(= N_("E765: 'spellfile' does not have %d entries"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_insufficient_arguments_for_printf[]
	INIT(= N_("E766: Insufficient arguments for printf()"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_too_many_arguments_ಗಮ್ಯ_printf[]
	INIT(= N_("E767: Too many arguments for printf()"));
EXTERN ಅಕ್ಷರ e_ಅದಲುಬದಲು_file_exists_str_silent_overrides[]
	INIT(= N_("E768: Swap file exists: %s (:silent! overrides)"));
EXTERN ಅಕ್ಷರ e_missing_rsb_after_str_lsb[]
	INIT(= N_("E769: Missing ] after %s["));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
EXTERN ಅಕ್ಷರ e_unsupported_section_ಒಳ_spell_file[]
	INIT(= N_("E770: Unsupported section in spell file"));
EXTERN ಅಕ್ಷರ e_old_spell_file_needs_ಗಮ್ಯ_be_updated[]
	INIT(= N_("E771: Old spell file, needs to be updated"));
EXTERN ಅಕ್ಷರ e_spell_file_is_for_newer_ಆವೃತ್ತಿ_ದ_vim[]
	INIT(= N_("E772: Spell file is for newer version of Vim"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_symlink_loop_for_str[]
	INIT(= N_("E773: Symlink loop for \"%s\""));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_operatorfunc_is_ಖಾಲಿಗಳು[]
	INIT(= N_("E774: 'operatorfunc' is empty"));
#ಇಲ್ಲದಿದ್ದರೆ
EXTERN ಅಕ್ಷರ e_eval_feature_not_ಲಭ್ಯ[]
	INIT(= N_("E775: Eval feature not available"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_no_location_list[]
	INIT(= N_("E776: No location list"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_string_or_list_expected[]
	INIT(= N_("E777: String or List expected"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
EXTERN ಅಕ್ಷರ e_this_does_not_look_like_sug_file_str[]
	INIT(= N_("E778: This does not look like a .sug file: %s"));
EXTERN ಅಕ್ಷರ e_old_sug_file_needs_ಗಮ್ಯ_be_updated_str[]
	INIT(= N_("E779: Old .sug file, needs to be updated: %s"));
EXTERN ಅಕ್ಷರ e_sug_file_is_for_newer_ಆವೃತ್ತಿ_ದ_vim_str[]
	INIT(= N_("E780: .sug file is for newer version of Vim: %s"));
EXTERN ಅಕ್ಷರ e_sug_file_doesnt_ಹೊಂದಿಕೆ_spl_file_str[]
	INIT(= N_("E781: .sug file doesn't match .spl file: %s"));
EXTERN ಅಕ್ಷರ e_error_while_reading_sug_file_str[]
	INIT(= N_("E782: Error while reading .sug file: %s"));
EXTERN ಅಕ್ಷರ e_duplicate_char_ಒಳ_map_ನಮೂದು[]
	INIT(= N_("E783: Duplicate char in MAP entry"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_close_ಕೊನೆಯ_tab_ಪುಟ[]
	INIT(= N_("E784: Cannot close last tab page"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# ifdef FEAT_COMPL_FUNC
EXTERN ಅಕ್ಷರ e_complete_can_only_be_ಬಳಸಿದ_ಒಳ_ಸೇರಿಸು_ಬಹುಲಕ[]
	INIT(= N_("E785: complete() can only be used in Insert mode"));
# endif
EXTERN ಅಕ್ಷರ e_range_not_allowed[]
	INIT(= N_("E786: Range not allowed"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
EXTERN ಅಕ್ಷರ e_ಬಫರ್_ಬದಲಾಯಿತು_unexpectedly[]
	INIT(= N_("E787: Buffer changed unexpectedly"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_not_allowed_ಗಮ್ಯ_edit_another_ಬಫರ್_now[]
	INIT(= N_("E788: Not allowed to edit another buffer now"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಅಕ್ಷರ e_error_missing_rsb_str[]
	INIT(= N_("E789: Missing ']': %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_undojoin_is_not_allowed_after_undo[]
	INIT(= N_("E790: undojoin is not allowed after undo"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
EXTERN ಅಕ್ಷರ e_ಖಾಲಿಗಳು_keymap_ನಮೂದು[]
	INIT(= N_("E791: Empty keymap entry"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
EXTERN ಅಕ್ಷರ e_ಖಾಲಿಗಳು_menu_name[]
	INIT(= N_("E792: Empty menu name"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
EXTERN ಅಕ್ಷರ e_no_other_ಬಫರ್_ಒಳ_diff_ಬಹುಲಕ_is_modifiable[]
	INIT(= N_("E793: No other buffer in diff mode is modifiable"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_ಗಣ_variable_ಒಳ_sandbox[]
	INIT(= N_("E794: Cannot set variable in the sandbox"));
EXTERN ಅಕ್ಷರ e_cannot_ಗಣ_variable_ಒಳ_sandbox_str[]
	INIT(= N_("E794: Cannot set variable in the sandbox: \"%s\""));
EXTERN ಅಕ್ಷರ e_cannot_delete_variable[]
	INIT(= N_("E795: Cannot delete variable"));
EXTERN ಅಕ್ಷರ e_cannot_delete_variable_str[]
	INIT(= N_("E795: Cannot delete variable %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	// E796
EXTERN ಅಕ್ಷರ e_writing_ಗಮ್ಯ_device_disabled_with_opendevice_option[]
	INIT(= N_("writing to device disabled with 'opendevice' option"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
EXTERN ಅಕ್ಷರ e_spellfilemising_autocommand_deleted_ಬಫರ್[]
	INIT(= N_("E797: SpellFileMissing autocommand deleted buffer"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
EXTERN ಅಕ್ಷರ e_id_is_reserved_for_ಹೊಂದಿಕೆ_nr[]
	INIT(= N_("E798: ID is reserved for \":match\": %d"));
EXTERN ಅಕ್ಷರ e_invalid_id_nr_must_be_greater_than_or_equal_ಗಮ್ಯ_one_1[]
	INIT(= N_("E799: Invalid ID: %d (must be greater than or equal to 1)"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_ARABIC
EXTERN ಅಕ್ಷರ e_arabic_cannot_be_ಬಳಸಿದ_not_enabled_at_compile_ಸಮಯ[]
	INIT(= N_("E800: Arabic cannot be used: Not enabled at compile time\n"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
EXTERN ಅಕ್ಷರ e_id_already_ತೆಗೆದುಕೊಂಡ_nr[]
	INIT(= N_("E801: ID already taken: %d"));
EXTERN ಅಕ್ಷರ e_invalid_id_nr_must_be_greater_than_or_equal_ಗಮ್ಯ_one_2[]
	INIT(= N_("E802: Invalid ID: %d (must be greater than or equal to 1)"));
EXTERN ಅಕ್ಷರ e_id_not_ಸಿಕ್ಕಿತು_nr[]
	INIT(= N_("E803: ID not found: %d"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_use_percent_with_float[]
	// xgettext:no-c-format
	INIT(= N_("E804: Cannot use '%' with Float"));
EXTERN ಅಕ್ಷರ e_using_float_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E805: Using a Float as a Number"));
EXTERN ಅಕ್ಷರ e_using_float_as_string[]
	INIT(= N_("E806: Using a Float as a String"));
EXTERN ಅಕ್ಷರ e_expected_float_ವಾದ_for_printf[]
	INIT(= N_("E807: Expected Float argument for printf()"));
EXTERN ಅಕ್ಷರ e_ಸಂಖ್ಯೆ_or_float_required[]
	INIT(= N_("E808: Number or Float required"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_hashsmall_is_not_ಲಭ್ಯ_without_the_eval_feature[]
	INIT(= N_("E809: #< is not available without the +eval feature"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
EXTERN ಅಕ್ಷರ e_cannot_read_or_write_ತಾತ್ಕಾಲಿಕ_files[]
	INIT(= N_("E810: Cannot read or write temp files"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_not_allowed_ಗಮ್ಯ_change_ಬಫರ್_information_now[]
	INIT(= N_("E811: Not allowed to change buffer information now"));
EXTERN ಅಕ್ಷರ e_autocommands_ಬದಲಾಯಿತು_ಬಫರ್_or_ಬಫರ್_name[]
	INIT(= N_("E812: Autocommands changed buffer or buffer name"));
EXTERN ಅಕ್ಷರ e_cannot_close_autocmd_or_popup_ಕಿಟಕಿ[]
	INIT(= N_("E813: Cannot close autocmd or popup window"));
EXTERN ಅಕ್ಷರ e_cannot_close_ಕಿಟಕಿ_only_autocmd_ಕಿಟಕಿ_would_remain[]
	INIT(= N_("E814: Cannot close window, only autocmd window would remain"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
EXTERN ಅಕ್ಷರ e_sorry_this_command_is_disabled_the_mzscheme_libraries_could_not_be_loaded[]
	INIT(= N_("E815: Sorry, this command is disabled, the MzScheme libraries could not be loaded."));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
EXTERN ಅಕ್ಷರ e_cannot_read_patch_output[]
	INIT(= N_("E816: Cannot read patch output"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
EXTERN ಅಕ್ಷರ e_blowfish_big_little_endian_use_wrong[]
	INIT(= N_("E817: Blowfish big/little endian use wrong"));
EXTERN ಅಕ್ಷರ e_sha256_test_failed[]
	INIT(= N_("E818: sha256 test failed"));
EXTERN ಅಕ್ಷರ e_blowfish_test_failed[]
	INIT(= N_("E819: Blowfish test failed"));
EXTERN ಅಕ್ಷರ e_sizeof_uint32_isnot_four[]
	INIT(= N_("E820: sizeof(uint32_t) != 4"));
EXTERN ಅಕ್ಷರ e_file_is_encrypted_with_unknown_ವಿಧಾನ[]
	INIT(= N_("E821: File is encrypted with unknown method"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
EXTERN ಅಕ್ಷರ e_cannot_open_undo_file_for_reading_str[]
	INIT(= N_("E822: Cannot open undo file for reading: %s"));
EXTERN ಅಕ್ಷರ e_not_an_undo_file_str[]
	INIT(= N_("E823: Not an undo file: %s"));
EXTERN ಅಕ್ಷರ e_incompatible_undo_file_str[]
	INIT(= N_("E824: Incompatible undo file: %s"));
EXTERN ಅಕ್ಷರ e_corrupted_undo_file_str_str[]
	INIT(= N_("E825: Corrupted undo file (%s): %s"));
# ifdef FEAT_CRYPT
EXTERN ಅಕ್ಷರ e_undo_file_decryption_failed[]
	INIT(= N_("E826: Undo file decryption failed: %s"));
# ಇಲ್ಲದಿದ್ದರೆ
EXTERN ಅಕ್ಷರ e_undo_file_is_encrypted_str[]
	INIT(= N_("E827: Undo file is encrypted: %s"));
# endif
EXTERN ಅಕ್ಷರ e_cannot_open_undo_file_for_writing_str[]
	INIT(= N_("E828: Cannot open undo file for writing: %s"));
EXTERN ಅಕ್ಷರ e_write_error_ಒಳ_undo_file_str[]
	INIT(= N_("E829: Write error in undo file: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_undo_ಸಂಖ್ಯೆ_nr_not_ಸಿಕ್ಕಿತು[]
	INIT(= N_("E830: Undo number %ld not found"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
EXTERN ಅಕ್ಷರ e_bf_ಕೀಲಿ_init_called_with_ಖಾಲಿಗಳು_password[]
	INIT(= "E831: bf_key_init() called with empty password");
# ifdef FEAT_PERSISTENT_UNDO
EXTERN ಅಕ್ಷರ e_non_encrypted_file_has_encrypted_undo_file_str[]
	INIT(= N_("E832: Non-encrypted file has encrypted undo file: %s"));
# endif
#ಇಲ್ಲದಿದ್ದರೆ
EXTERN ಅಕ್ಷರ e_str_is_encrypted_and_this_ಆವೃತ್ತಿ_ದ_vim_does_not_support_encryption[]
	INIT(= N_("E833: %s is encrypted and this version of Vim does not support encryption"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_conflicts_with_ಮೌಲ್ಯ_ದ_listchars[]
	INIT(= N_("E834: Conflicts with value of 'listchars'"));
EXTERN ಅಕ್ಷರ e_conflicts_with_ಮೌಲ್ಯ_ದ_fillchars[]
	INIT(= N_("E835: Conflicts with value of 'fillchars'"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DYNAMIC_PYTHON
EXTERN ಅಕ್ಷರ e_this_vim_cannot_execute_python_after_using_py3[]
	INIT(= N_("E836: This Vim cannot execute :python after using :py3"));
EXTERN ಅಕ್ಷರ e_this_vim_cannot_execute_py3_after_using_python[]
	INIT(= N_("E837: This Vim cannot execute :py3 after using :python"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI)
EXTERN ಅಕ್ಷರ e_netbeans_is_not_supported_with_this_GUI[]
	INIT(= N_("E838: NetBeans is not supported with this GUI"));
#ಆದರೆ_ಅಂತ್ಯ
// E839 unused
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
EXTERN ಅಕ್ಷರ e_complete_function_deleted_ಪಠ್ಯ[]
	INIT(= N_("E840: Completion function deleted text"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_reserved_name_cannot_be_ಬಳಸಿದ_for_user_defined_command[]
	INIT(= N_("E841: Reserved name, cannot be used for user defined command"));
EXTERN ಅಕ್ಷರ e_no_ಸಾಲು_ಸಂಖ್ಯೆ_ಗಮ್ಯ_use_for_slnum[]
	INIT(= N_("E842: No line number to use for \"<slnum>\""));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
EXTERN ಅಕ್ಷರ e_error_while_updating_ಅದಲುಬದಲು_file_crypt[]
	INIT(= N_("E843: Error while updating swap file crypt"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
EXTERN ಅಕ್ಷರ e_invalid_cchar_ಮೌಲ್ಯ[]
	INIT(= N_("E844: Invalid cchar value"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
EXTERN ಅಕ್ಷರ e_insufficient_ಸ್ಮರಣೆ_ಶಬ್ದ_list_will_be_incomplete[]
	INIT(= N_("E845: Insufficient memory, word list will be incomplete"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಕೀಲಿ_ಸಂಕೇತ_not_ಗಣ[]
	INIT(= N_("E846: Key code not set"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಅಕ್ಷರ e_too_many_syntax_includes[]
	INIT(= N_("E847: Too many syntax includes"));
EXTERN ಅಕ್ಷರ e_too_many_syntax_clusters[]
	INIT(= N_("E848: Too many syntax clusters"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_too_many_highlight_and_syntax_groups[]
	INIT(= N_("E849: Too many highlight and syntax groups"));
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_CLIPBOARD
EXTERN ಅಕ್ಷರ e_invalid_ರಿಜಿಸ್ಟರ್_name[]
	INIT(= N_("E850: Invalid register name"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
EXTERN ಅಕ್ಷರ e_failed_ಗಮ್ಯ_create_new_process_for_GUI[]
	INIT(= N_("E851: Failed to create a new process for the GUI"));
EXTERN ಅಕ್ಷರ e_the_ಮಗು_process_failed_ಗಮ್ಯ_ಆರಂಭ_GUI[]
	INIT(= N_("E852: The child process failed to start the GUI"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_duplicate_ವಾದ_name_str[]
	INIT(= N_("E853: Duplicate argument name: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಮಾರ್ಗ_too_long_for_completion[]
	INIT(= N_("E854: Path too long for completion"));
EXTERN ಅಕ್ಷರ e_autocommands_caused_command_ಗಮ್ಯ_abort[]
	INIT(= N_("E855: Autocommands caused command to abort"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_assert_fails_ಎರಡನೇ_arg[]
	INIT(= N_("E856: \"assert_fails()\" second argument must be a string or a list with one or two strings"));
EXTERN ಅಕ್ಷರ e_dictionary_ಕೀಲಿ_str_required[]
	INIT(= N_("E857: Dictionary key \"%s\" required"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN ಅಕ್ಷರ e_eval_did_not_return_valid_python_object[]
	INIT(= N_("E858: Eval did not return a valid python object"));
EXTERN ಅಕ್ಷರ e_failed_ಗಮ್ಯ_convert_returned_python_object_ಗಮ್ಯ_vim_ಮೌಲ್ಯ[]
	INIT(= N_("E859: Failed to convert returned python object to a Vim value"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_need_id_and_type_or_types_with_both[]
	INIT(= N_("E860: Need 'id' and 'type' or 'types' with 'both'"));
# ifdef FEAT_TERMINAL
EXTERN ಅಕ್ಷರ e_cannot_open_ಎರಡನೇ_popup_with_terminal[]
	INIT(= N_("E861: Cannot open a second popup with a terminal"));
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_use_g_ಇಲ್ಲಿ[]
	INIT(= N_("E862: Cannot use g: here"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
EXTERN ಅಕ್ಷರ e_not_allowed_for_terminal_ಒಳ_popup_ಕಿಟಕಿ[]
	INIT(= N_("E863: Not allowed for a terminal in a popup window"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_percent_hash_can_only_be_followed_by_zero_one_two_automatic_engine_will_be_ಬಳಸಿದ[]
	// xgettext:no-c-format
	INIT(= N_("E864: \\%#= can only be followed by 0, 1, or 2. The automatic engine will be used"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_ಕೊನೆ_encountered_prematurely[]
	INIT(= N_("E865: (NFA) Regexp end encountered prematurely"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_misplaced_chr[]
	INIT(= N_("E866: (NFA regexp) Misplaced %c"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_unknown_operator_z_chr[]
	INIT(= N_("E867: (NFA regexp) Unknown operator '\\z%c'"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_unknown_operator_percent_chr[]
	INIT(= N_("E867: (NFA regexp) Unknown operator '\\%%%c'"));
EXTERN ಅಕ್ಷರ e_error_building_nfa_with_equivalence_class[]
	INIT(= N_("E868: Error building NFA with equivalence class!"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_unknown_operator_at_chr[]
	INIT(= N_("E869: (NFA regexp) Unknown operator '\\@%c'"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_error_reading_repetition_limits[]
	INIT(= N_("E870: (NFA regexp) Error reading repetition limits"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_cant_have_multi_follow_multi[]
	INIT(= N_("E871: (NFA regexp) Can't have a multi follow a multi"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_too_many_parens[]
	INIT(= N_("E872: (NFA regexp) Too many '('"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_proper_termination_error[]
	INIT(= N_("E873: (NFA regexp) proper termination error"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_could_not_ತೆಗೆ_ರಾಶಿ[]
	INIT(= N_("E874: (NFA regexp) Could not pop the stack!"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_while_converting_from_postfix_ಗಮ್ಯ_nfa_too_many_stats_ಎಡ_on_ರಾಶಿ[]
	INIT(= N_("E875: (NFA regexp) (While converting from postfix to NFA), too many states left on stack"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_not_enough_space_ಗಮ್ಯ_store_ಸಂಪೂರ್ಣ_nfa[]
	INIT(= N_("E876: (NFA regexp) Not enough space to store the whole NFA"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_invalid_character_class_nr[]
	INIT(= N_("E877: (NFA regexp) Invalid character class: %d"));
EXTERN ಅಕ್ಷರ e_nfa_regexp_could_not_allocate_ಸ್ಮರಣೆ_for_branch_traversal[]
	INIT(= N_("E878: (NFA regexp) Could not allocate memory for branch traversal!"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಅಕ್ಷರ e_nfa_regexp_too_many_z[]
	INIT(= N_("E879: (NFA regexp) Too many \\z("));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN ಅಕ್ಷರ e_cant_handle_systemexit_ದ_python_exception_ಒಳ_vim[]
	INIT(= N_("E880: Can't handle SystemExit of python exception in vim"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಸಾಲು_ಎಣಿಕೆ_ಬದಲಾಯಿತು_unexpectedly[]
	INIT(= N_("E881: Line count changed unexpectedly"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_uniq_compare_function_failed[]
	INIT(= N_("E882: Uniq compare function failed"));
EXTERN ಅಕ್ಷರ e_ರಿಜಿಸ್ಟರ್_char_cannot_contain_ಅಪವರ್ತ್ಯ_ಸಾಲುಗಳು[]
	INIT(= N_("E883: Register '%c' cannot contain multiple lines"));
EXTERN ಅಕ್ಷರ e_function_name_cannot_contain_colon_str[]
	INIT(= N_("E884: Function name cannot contain a colon: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
EXTERN ಅಕ್ಷರ e_not_possible_ಗಮ್ಯ_change_sign_str[]
	INIT(= N_("E885: Not possible to change sign %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
EXTERN ಅಕ್ಷರ e_cant_rename_viminfo_file_ಗಮ್ಯ_str[]
	INIT(= N_("E886: Can't rename viminfo file to %s!"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
EXTERN ಅಕ್ಷರ e_sorry_this_command_is_disabled_python_site_module_could_not_be_loaded[]
	INIT(= N_("E887: Sorry, this command is disabled, the Python's site module could not be loaded."));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_nfa_regexp_cannot_repeat_str[]
	INIT(= N_("E888: (NFA regexp) cannot repeat %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_ಸಂಖ್ಯೆ_required[]
	INIT(= N_("E889: Number required"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
EXTERN ಅಕ್ಷರ e_trailing_char_after_rsb_str_str[]
	INIT(= N_("E890: Trailing char after ']': %s]%s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_using_funcref_as_float[]
	INIT(= N_("E891: Using a Funcref as a Float"));
EXTERN ಅಕ್ಷರ e_using_string_as_float[]
	INIT(= N_("E892: Using a String as a Float"));
EXTERN ಅಕ್ಷರ e_using_list_as_float[]
	INIT(= N_("E893: Using a List as a Float"));
EXTERN ಅಕ್ಷರ e_using_dictionary_as_float[]
	INIT(= N_("E894: Using a Dictionary as a Float"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
EXTERN ಅಕ್ಷರ e_sorry_this_command_is_disabled_the_mzscheme_racket_ತಳ_module_could_not_be_loaded[]
	INIT(= N_("E895: Sorry, this command is disabled, the MzScheme's racket/base module could not be loaded."));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ವಾದ_ದ_str_must_be_list_dictionary_or_blob[]
	INIT(= N_("E896: Argument of %s must be a List, Dictionary or Blob"));
EXTERN ಅಕ್ಷರ e_list_or_blob_required[]
	INIT(= N_("E897: List or Blob required"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
EXTERN ಅಕ್ಷರ e_socket_ಒಳ_channel_connect[]
	INIT(= N_("E898: socket() in channel_connect()"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ವಾದ_ದ_str_must_be_list_or_blob[]
	INIT(= N_("E899: Argument of %s must be a List or Blob"));
EXTERN ಅಕ್ಷರ e_maxdepth_must_be_non_negative_ಸಂಖ್ಯೆ[]
	INIT(= N_("E900: maxdepth must be non-negative number"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
EXTERN ಅಕ್ಷರ e_getaddrinfo_ಒಳ_channel_open_str[]
	INIT(= N_("E901: getaddrinfo() in channel_open(): %s"));
# ifndef FEAT_IPV6
EXTERN ಅಕ್ಷರ e_gethostbyname_ಒಳ_channel_open[]
	INIT(= N_("E901: gethostbyname() in channel_open()"));
# endif
EXTERN ಅಕ್ಷರ e_cannot_connect_ಗಮ್ಯ_ಪೋರ್ಟ್[]
	INIT(= N_("E902: Cannot connect to port"));
EXTERN ಅಕ್ಷರ e_received_command_with_non_string_ವಾದ[]
	INIT(= N_("E903: Received command with non-string argument"));
EXTERN ಅಕ್ಷರ e_ಕೊನೆಯ_ವಾದ_for_ಅಭಿವ್ಯಕ್ತಿ_call_must_be_ಸಂಖ್ಯೆ[]
	INIT(= N_("E904: Last argument for expr/call must be a number"));
EXTERN ಅಕ್ಷರ e_ಮೂರನೇ_ವಾದ_for_call_must_be_list[]
	INIT(= N_("E904: Third argument for call must be a list"));
EXTERN ಅಕ್ಷರ e_received_unknown_command_str[]
	INIT(= N_("E905: Received unknown command: %s"));
EXTERN ಅಕ್ಷರ e_not_an_open_channel[]
	INIT(= N_("E906: Not an open channel"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_using_special_ಮೌಲ್ಯ_as_float[]
	INIT(= N_("E907: Using a special value as a Float"));
EXTERN ಅಕ್ಷರ e_using_invalid_ಮೌಲ್ಯ_as_string_str[]
	INIT(= N_("E908: Using an invalid value as a String: %s"));
EXTERN ಅಕ್ಷರ e_cannot_ಸೂಚ್ಯಂಕ_special_variable[]
	INIT(= N_("E909: Cannot index a special variable"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
EXTERN ಅಕ್ಷರ e_using_ಕೆಲಸ_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E910: Using a Job as a Number"));
EXTERN ಅಕ್ಷರ e_using_ಕೆಲಸ_as_float[]
	INIT(= N_("E911: Using a Job as a Float"));
EXTERN ಅಕ್ಷರ e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel[]
	INIT(= N_("E912: Cannot use ch_evalexpr()/ch_sendexpr() with a raw, nl or blob channel"));
EXTERN ಅಕ್ಷರ e_using_channel_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E913: Using a Channel as a Number"));
EXTERN ಅಕ್ಷರ e_using_channel_as_float[]
	INIT(= N_("E914: Using a Channel as a Float"));
EXTERN ಅಕ್ಷರ e_ಒಳ_io_ಬಫರ್_requires_ಒಳ_buf_or_ಒಳ_name_ಗಮ್ಯ_be_ಗಣ[]
	INIT(= N_("E915: in_io buffer requires in_buf or in_name to be set"));
EXTERN ಅಕ್ಷರ e_not_valid_ಕೆಲಸ[]
	INIT(= N_("E916: Not a valid job"));
EXTERN ಅಕ್ಷರ e_cannot_use_callback_with_str[]
	INIT(= N_("E917: Cannot use a callback with %s()"));
EXTERN ಅಕ್ಷರ e_ಬಫರ್_must_be_loaded_str[]
	INIT(= N_("E918: Buffer must be loaded: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_directory_not_ಸಿಕ್ಕಿತು_ಒಳ_str_str[]
	INIT(= N_("E919: Directory not found in '%s': \"%s\""));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
EXTERN ಅಕ್ಷರ e_io_file_requires_name_ಗಮ್ಯ_be_ಗಣ[]
	INIT(= N_("E920: _io file requires _name to be set"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_invalid_callback_ವಾದ[]
	INIT(= N_("E921: Invalid callback argument"));
// E922 unused
EXTERN ಅಕ್ಷರ e_ಎರಡನೇ_ವಾದ_ದ_function_must_be_list_or_dict[]
	INIT(= N_("E923: Second argument of function() must be a list or a dict"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_current_ಕಿಟಕಿ_was_closed[]
	INIT(= N_("E924: Current window was closed"));
EXTERN ಅಕ್ಷರ e_current_quickfix_list_was_ಬದಲಾಯಿತು[]
	INIT(= N_("E925: Current quickfix list was changed"));
EXTERN ಅಕ್ಷರ e_current_location_list_was_ಬದಲಾಯಿತು[]
	INIT(= N_("E926: Current location list was changed"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# ifdef FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_invalid_action_str_1[]
	INIT(= N_("E927: Invalid action: '%s'"));
# endif
EXTERN ಅಕ್ಷರ e_string_required[]
	INIT(= N_("E928: String required"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
EXTERN ಅಕ್ಷರ e_too_many_viminfo_ತಾತ್ಕಾಲಿಕ_files_like_str[]
	INIT(= N_("E929: Too many viminfo temp files, like %s!"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_use_redir_inside_execute[]
	INIT(= N_("E930: Cannot use :redir inside execute()"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಬಫರ್_cannot_be_registered[]
	INIT(= N_("E931: Buffer cannot be registered"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ಆವರಣ_function_should_not_be_at_ತುದಿ_level_str[]
	INIT(= N_("E932: Closure function should not be at top level: %s"));
EXTERN ಅಕ್ಷರ e_function_was_deleted_str[]
	INIT(= N_("E933: Function was deleted: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
EXTERN ಅಕ್ಷರ e_cannot_jump_ಗಮ್ಯ_ಬಫರ್_that_does_not_have_name[]
	INIT(= N_("E934: Cannot jump to a buffer that does not have a name"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_invalid_submatch_ಸಂಖ್ಯೆ_nr[]
	INIT(= N_("E935: Invalid submatch number: %d"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_delete_current_ಗುಂಪು[]
	INIT(= N_("E936: Cannot delete the current group"));
EXTERN ಅಕ್ಷರ e_attempt_ಗಮ್ಯ_delete_ಬಫರ್_that_is_ಒಳ_use_str[]
	INIT(= N_("E937: Attempt to delete a buffer that is in use: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_duplicate_ಕೀಲಿ_ಒಳ_json_str[]
	INIT(= N_("E938: Duplicate key in JSON: \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಧನಾತ್ಮಕ_ಎಣಿಕೆ_required[]
	INIT(= N_("E939: Positive count required"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_ಬೀಗ_or_unlock_variable_str[]
	INIT(= N_("E940: Cannot lock or unlock variable %s"));
# ifdef FEAT_CLIENTSERVER
EXTERN ಅಕ್ಷರ e_already_started_server[]
	INIT(= N_("E941: Already started a server"));
# ಇಲ್ಲದಿದ್ದರೆ
EXTERN ಅಕ್ಷರ e_clientserver_feature_not_ಲಭ್ಯ[]
	INIT(= N_("E942: +clientserver feature not available"));
# endif
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_command_ಕೋಷ್ಟಕ_needs_ಗಮ್ಯ_be_updated_ಸತತ_make_cmdidxs[]
	INIT(= "E943: Command table needs to be updated, run 'make cmdidxs'");
EXTERN ಅಕ್ಷರ e_ಹಿಮ್ಮುಖ_range_ಒಳ_character_class[]
	INIT(= N_("E944: Reverse range in character class"));
EXTERN ಅಕ್ಷರ e_range_too_large_ಒಳ_character_class[]
	INIT(= N_("E945: Range too large in character class"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
EXTERN ಅಕ್ಷರ e_cannot_make_terminal_with_running_ಕೆಲಸ_modifiable[]
	INIT(= N_("E946: Cannot make a terminal with running job modifiable"));
EXTERN ಅಕ್ಷರ e_ಕೆಲಸ_still_running_ಒಳ_ಬಫರ್_str[]
	INIT(= N_("E947: Job still running in buffer \"%s\""));
EXTERN ಅಕ್ಷರ e_ಕೆಲಸ_still_running[]
	INIT(= N_("E948: Job still running"));
EXTERN ಅಕ್ಷರ e_ಕೆಲಸ_still_running_ಕೂಡಿಸು_bang_ಗಮ್ಯ_ಕೊನೆ_the_ಕೆಲಸ[]
	INIT(= N_("E948: Job still running (add ! to end the job)"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_file_ಬದಲಾಯಿತು_while_writing[]
	INIT(= N_("E949: File changed while writing"));
EXTERN ಅಕ್ಷರ e_cannot_convert_between_str_and_str[]
	INIT(= N_("E950: Cannot convert between %s and %s"));
EXTERN ಅಕ್ಷರ e_percent_ಮೌಲ್ಯ_too_large[]
	// xgettext:no-c-format
	INIT(= N_("E951: \\% value too large"));
#ಆದರೆ defined(FEAT_EVAL) && defined(FEAT_QUICKFIX)
EXTERN ಅಕ್ಷರ e_autocommand_caused_recursive_behavior[]
	INIT(= N_("E952: Autocommand caused recursive behavior"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
EXTERN ಅಕ್ಷರ e_file_exists_str[]
	INIT(= N_("E953: File exists: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TERMGUICOLORS) && defined(FEAT_VTP)
EXTERN ಅಕ್ಷರ e_24_ಬಿಟ್_colors_are_not_supported_on_this_environment[]
	INIT(= N_("E954: 24-bit colors are not supported on this environment"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
EXTERN ಅಕ್ಷರ e_not_terminal_ಬಫರ್[]
	INIT(= N_("E955: Not a terminal buffer"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_use_ನಮೂನೆ_recursively[]
	INIT(= N_("E956: Cannot use pattern recursively"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_invalid_ಕಿಟಕಿ_ಸಂಖ್ಯೆ[]
	INIT(= N_("E957: Invalid window number"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
EXTERN ಅಕ್ಷರ e_ಕೆಲಸ_already_ಪೂರ್ಣಗೊಂಡ[]
	INIT(= N_("E958: Job already finished"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
EXTERN ಅಕ್ಷರ e_invalid_diff_format[]
	INIT(= N_("E959: Invalid diff format."));
EXTERN ಅಕ್ಷರ e_problem_creating_ಆಂತರಿಕ_diff[]
	INIT(= N_("E960: Problem creating the internal diff"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_no_ಸಾಲು_ಸಂಖ್ಯೆ_ಗಮ್ಯ_use_for_sflnum[]
	INIT(= N_("E961: No line number to use for \"<sflnum>\""));
EXTERN ಅಕ್ಷರ e_invalid_action_str_2[]
	INIT(= N_("E962: Invalid action: '%s'"));
EXTERN ಅಕ್ಷರ e_setting_v_str_ಗಮ್ಯ_ಮೌಲ್ಯ_with_wrong_type[]
	INIT(= N_("E963: Setting v:%s to value with wrong type"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PROP_POPUP) || defined(FEAT_EVAL)
EXTERN char_u e_invalid_ಸ್ತಂಭ_ಸಂಖ್ಯೆ_nr[]
	INIT(= N_("E964: Invalid column number: %ld"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_missing_property_type_name[]
	INIT(= N_("E965: Missing property type name"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN char_u e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr[]
	INIT(= N_("E966: Invalid line number: %ld"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_ಪಠ್ಯ_property_info_corrupted[]
	INIT(= "E967: Text property info corrupted");
EXTERN ಅಕ್ಷರ e_need_at_ಕನಿಷ್ಠ_one_ದ_id_or_type[]
	INIT(= N_("E968: Need at least one of 'id' or 'type'"));
EXTERN ಅಕ್ಷರ e_property_type_str_already_defined[]
	INIT(= N_("E969: Property type %s already defined"));
EXTERN ಅಕ್ಷರ e_unknown_highlight_ಗುಂಪು_name_str[]
	INIT(= N_("E970: Unknown highlight group name: '%s'"));
EXTERN ಅಕ್ಷರ e_property_type_str_does_not_exist[]
	INIT(= N_("E971: Property type %s does not exist"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_blob_ಮೌಲ್ಯ_does_not_have_ಬಲ_ಸಂಖ್ಯೆ_ದ_ಬೈಟುಗಳು[]
	INIT(= N_("E972: Blob value does not have the right number of bytes"));
EXTERN ಅಕ್ಷರ e_blob_literal_should_have_an_even_ಸಂಖ್ಯೆ_ದ_hex_characters[]
	INIT(= N_("E973: Blob literal should have an even number of hex characters"));
EXTERN ಅಕ್ಷರ e_using_blob_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E974: Using a Blob as a Number"));
EXTERN ಅಕ್ಷರ e_using_blob_as_float[]
	INIT(= N_("E975: Using a Blob as a Float"));
EXTERN ಅಕ್ಷರ e_using_blob_as_string[]
	INIT(= N_("E976: Using a Blob as a String"));
EXTERN ಅಕ್ಷರ e_can_only_compare_blob_with_blob[]
	INIT(= N_("E977: Can only compare Blob with Blob"));
EXTERN ಅಕ್ಷರ e_invalid_operation_for_blob[]
	INIT(= N_("E978: Invalid operation for Blob"));
EXTERN ಅಕ್ಷರ e_blob_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr[]
	INIT(= N_("E979: Blob index out of range: %ld"));
# ifndef USE_ಒಳಹರಿವು_BUF
EXTERN ಅಕ್ಷರ e_lowlevel_ಒಳಹರಿವು_not_supported[]
	INIT(= N_("E980: Lowlevel input not supported"));
# endif
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_command_not_allowed_ಒಳ_rvim[]
	INIT(= N_("E981: Command not allowed in rvim"));
#ಆದರೆ defined(FEAT_TERMINAL) && defined(MSWIN)
EXTERN ಅಕ್ಷರ e_conpty_is_not_ಲಭ್ಯ[]
	INIT(= N_("E982: ConPTY is not available"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_duplicate_ವಾದ_str[]
	INIT(= N_("E983: Duplicate argument: %s"));
EXTERN ಅಕ್ಷರ e_scriptversion_ಬಳಸಿದ_outside_ದ_sourced_file[]
	INIT(= N_("E984: :scriptversion used outside of a sourced file"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_dot_equal_not_supported_with_script_ಆವೃತ್ತಿ_two[]
	INIT(= N_("E985: .= is not supported with script version >= 2"));
EXTERN ಅಕ್ಷರ e_cannot_modify_ಟ್ಯಾಗ್_ರಾಶಿ_within_tagfunc[]
	INIT(= N_("E986: Cannot modify the tag stack within tagfunc"));
EXTERN ಅಕ್ಷರ e_invalid_return_ಮೌಲ್ಯ_from_tagfunc[]
	INIT(= N_("E987: Invalid return value from tagfunc"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GUI_MAY_SPAWN
EXTERN ಅಕ್ಷರ e_gui_cannot_be_ಬಳಸಿದ_cannot_execute_gvim_exe[]
	INIT(= N_("E988: GUI cannot be used. Cannot execute gvim.exe."));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_non_default_ವಾದ_follows_default_ವಾದ[]
	INIT(= N_("E989: Non-default argument follows default argument"));
EXTERN ಅಕ್ಷರ e_missing_ಕೊನೆ_marker_str[]
	INIT(= N_("E990: Missing end marker '%s'"));
EXTERN ಅಕ್ಷರ e_cannot_use_heredoc_ಇಲ್ಲಿ[]
	INIT(= N_("E991: Cannot use =<< here"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_not_allowed_ಒಳ_modeline_when_modelineexpr_is_off[]
	INIT(= N_("E992: Not allowed in a modeline when 'modelineexpr' is off"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ಕಿಟಕಿ_nr_is_not_popup_ಕಿಟಕಿ[]
	INIT(= N_("E993: Window %d is not a popup window"));
EXTERN ಅಕ್ಷರ e_not_allowed_ಒಳ_popup_ಕಿಟಕಿ[]
	INIT(= N_("E994: Not allowed in a popup window"));
EXTERN ಅಕ್ಷರ e_cannot_modify_existing_variable[]
	INIT(= N_("E995: Cannot modify existing variable"));
EXTERN ಅಕ್ಷರ e_cannot_ಬೀಗ_range[]
	INIT(= N_("E996: Cannot lock a range"));
EXTERN ಅಕ್ಷರ e_cannot_ಬೀಗ_option[]
	INIT(= N_("E996: Cannot lock an option"));
EXTERN ಅಕ್ಷರ e_cannot_ಬೀಗ_list_or_dict[]
	INIT(= N_("E996: Cannot lock a list or dict"));
EXTERN ಅಕ್ಷರ e_cannot_ಬೀಗ_environment_variable[]
	INIT(= N_("E996: Cannot lock an environment variable"));
EXTERN ಅಕ್ಷರ e_cannot_ಬೀಗ_ರಿಜಿಸ್ಟರ್[]
	INIT(= N_("E996: Cannot lock a register"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_tabpage_not_ಸಿಕ್ಕಿತು_nr[]
	INIT(= N_("E997: Tabpage not found: %d"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_reduce_ದ_an_ಖಾಲಿಗಳು_str_with_no_initial_ಮೌಲ್ಯ[]
	INIT(= N_("E998: Reduce of an empty %s with no initial value"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_scriptversion_not_supported_nr[]
	INIT(= N_("E999: scriptversion not supported: %d"));
// E1000 unused
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_variable_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E1001: Variable not found: %s"));
EXTERN ಅಕ್ಷರ e_syntax_error_at_str[]
	INIT(= N_("E1002: Syntax error at %s"));
EXTERN ಅಕ್ಷರ e_missing_return_ಮೌಲ್ಯ[]
	INIT(= N_("E1003: Missing return value"));
EXTERN ಅಕ್ಷರ e_white_space_required_before_and_after_str_at_str[]
	INIT(= N_("E1004: White space required before and after '%s' at \"%s\""));
EXTERN ಅಕ್ಷರ e_too_many_ವಾದ_types[]
	INIT(= N_("E1005: Too many argument types"));
EXTERN ಅಕ್ಷರ e_str_is_ಬಳಸಿದ_as_ವಾದ[]
	INIT(= N_("E1006: %s is used as an argument"));
EXTERN ಅಕ್ಷರ e_mandatory_ವಾದ_after_optional_ವಾದ[]
	INIT(= N_("E1007: Mandatory argument after optional argument"));
EXTERN ಅಕ್ಷರ e_missing_type_after_str[]
	INIT(= N_("E1008: Missing <type> after %s"));
EXTERN ಅಕ್ಷರ e_missing_gt_after_type_str[]
	INIT(= N_("E1009: Missing > after type: %s"));
EXTERN ಅಕ್ಷರ e_type_not_recognized_str[]
	INIT(= N_("E1010: Type not recognized: %s"));
EXTERN ಅಕ್ಷರ e_name_too_long_str[]
	INIT(= N_("E1011: Name too long: %s"));
EXTERN ಅಕ್ಷರ e_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1012: Type mismatch; expected %s but got %s"));
EXTERN ಅಕ್ಷರ e_type_mismatch_expected_str_but_got_str_ಒಳ_str[]
	INIT(= N_("E1012: Type mismatch; expected %s but got %s in %s"));
EXTERN ಅಕ್ಷರ e_ವಾದ_nr_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1013: Argument %d: type mismatch, expected %s but got %s"));
EXTERN ಅಕ್ಷರ e_ವಾದ_nr_type_mismatch_expected_str_but_got_str_ಒಳ_str[]
	INIT(= N_("E1013: Argument %d: type mismatch, expected %s but got %s in %s"));
EXTERN ಅಕ್ಷರ e_invalid_ಕೀಲಿ_str[]
	INIT(= N_("E1014: Invalid key: %s"));
EXTERN ಅಕ್ಷರ e_name_expected_str[]
	INIT(= N_("E1015: Name expected: %s"));
EXTERN ಅಕ್ಷರ e_cannot_declare_a_scope_variable_str[]
	INIT(= N_("E1016: Cannot declare a %s variable: %s"));
EXTERN ಅಕ್ಷರ e_cannot_declare_an_environment_variable_str[]
	INIT(= N_("E1016: Cannot declare an environment variable: %s"));
EXTERN ಅಕ್ಷರ e_variable_already_declared_str[]
	INIT(= N_("E1017: Variable already declared: %s"));
EXTERN ಅಕ್ಷರ e_cannot_ನಿಯೋಜಿಸು_ಗಮ್ಯ_constant_str[]
	INIT(= N_("E1018: Cannot assign to a constant: %s"));
EXTERN ಅಕ್ಷರ e_can_only_concatenate_ಗಮ್ಯ_string[]
	INIT(= N_("E1019: Can only concatenate to string"));
EXTERN ಅಕ್ಷರ e_cannot_use_operator_on_new_variable_str[]
	INIT(= N_("E1020: Cannot use an operator on a new variable: %s"));
EXTERN ಅಕ್ಷರ e_const_requires_a_ಮೌಲ್ಯ[]
	INIT(= N_("E1021: Const requires a value"));
EXTERN ಅಕ್ಷರ e_type_or_initialization_required[]
	INIT(= N_("E1022: Type or initialization required"));
EXTERN ಅಕ್ಷರ e_using_ಸಂಖ್ಯೆ_as_bool_nr[]
	INIT(= N_("E1023: Using a Number as a Bool: %lld"));
EXTERN ಅಕ್ಷರ e_using_ಸಂಖ್ಯೆ_as_string[]
	INIT(= N_("E1024: Using a Number as a String"));
EXTERN ಅಕ್ಷರ e_using_rcurly_outside_if_ಖಂಡ_scope[]
	INIT(= N_("E1025: Using } outside of a block scope"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_missing_rcurly[]
	INIT(= N_("E1026: Missing }"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_missing_return_statement[]
	INIT(= N_("E1027: Missing return statement"));
EXTERN ಅಕ್ಷರ e_compiling_def_function_failed[]
	INIT(= N_("E1028: Compiling :def function failed"));
EXTERN ಅಕ್ಷರ e_expected_str_but_got_str[]
	INIT(= N_("E1029: Expected %s but got %s"));
EXTERN ಅಕ್ಷರ e_using_string_as_ಸಂಖ್ಯೆ_str[]
	INIT(= N_("E1030: Using a String as a Number: \"%s\""));
EXTERN ಅಕ್ಷರ e_cannot_use_void_ಮೌಲ್ಯ[]
	INIT(= N_("E1031: Cannot use void value"));
EXTERN ಅಕ್ಷರ e_missing_catch_or_finally[]
	INIT(= N_("E1032: Missing :catch or :finally"));
EXTERN ಅಕ್ಷರ e_catch_unreachable_after_catch_all[]
	INIT(= N_("E1033: Catch unreachable after catch-all"));
EXTERN ಅಕ್ಷರ e_cannot_use_reserved_name_str[]
	INIT(= N_("E1034: Cannot use reserved name %s"));
EXTERN ಅಕ್ಷರ e_percent_requires_ಸಂಖ್ಯೆ_arguments[]
	// xgettext:no-c-format
	INIT(= N_("E1035: % requires number arguments"));
EXTERN ಅಕ್ಷರ e_char_requires_ಸಂಖ್ಯೆ_or_float_arguments[]
	INIT(= N_("E1036: %c requires number or float arguments"));
EXTERN ಅಕ್ಷರ e_cannot_use_str_with_str[]
	INIT(= N_("E1037: Cannot use \"%s\" with %s"));
EXTERN ಅಕ್ಷರ e_vim9script_can_only_be_ಬಳಸಿದ_ಒಳ_script[]
	INIT(= N_("E1038: \"vim9script\" can only be used in a script"));
EXTERN ಅಕ್ಷರ e_vim9script_must_be_ಮೊದಲ_command_ಒಳ_script[]
	INIT(= N_("E1039: \"vim9script\" must be the first command in a script"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_use_scriptversion_after_vim9script[]
	INIT(= N_("E1040: Cannot use :scriptversion after :vim9script"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_redefining_script_ಅಂಶ_str[]
	INIT(= N_("E1041: Redefining script item: \"%s\""));
EXTERN ಅಕ್ಷರ e_export_can_only_be_ಬಳಸಿದ_ಒಳ_vim9script[]
	INIT(= N_("E1042: Export can only be used in vim9script"));
EXTERN ಅಕ್ಷರ e_invalid_command_after_export[]
	INIT(= N_("E1043: Invalid command after :export"));
EXTERN ಅಕ್ಷರ e_export_with_invalid_ವಾದ[]
	INIT(= N_("E1044: Export with invalid argument"));
// E1045 not used
// E1046 not used
EXTERN ಅಕ್ಷರ e_syntax_error_ಒಳ_import_str[]
	INIT(= N_("E1047: Syntax error in import: %s"));
EXTERN ಅಕ್ಷರ e_ಅಂಶ_not_ಸಿಕ್ಕಿತು_ಒಳ_script_str[]
	INIT(= N_("E1048: Item not found in script: %s"));
EXTERN ಅಕ್ಷರ e_ಅಂಶ_not_exported_ಒಳ_script_str[]
	INIT(= N_("E1049: Item not exported in script: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_colon_required_before_range_str[]
	INIT(= N_("E1050: Colon required before a range: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_wrong_ವಾದ_type_for_plus[]
	INIT(= N_("E1051: Wrong argument type for +"));
EXTERN ಅಕ್ಷರ e_cannot_declare_an_option_str[]
	INIT(= N_("E1052: Cannot declare an option: %s"));
EXTERN ಅಕ್ಷರ e_could_not_import_str[]
	INIT(= N_("E1053: Could not import \"%s\""));
EXTERN ಅಕ್ಷರ e_variable_already_declared_ಒಳ_script_str[]
	INIT(= N_("E1054: Variable already declared in the script: %s"));
EXTERN ಅಕ್ಷರ e_missing_name_after_dots[]
	INIT(= N_("E1055: Missing name after ..."));
EXTERN ಅಕ್ಷರ e_expected_type_str[]
	INIT(= N_("E1056: Expected a type: %s"));
EXTERN ಅಕ್ಷರ e_missing_enddef[]
	INIT(= N_("E1057: Missing :enddef"));
EXTERN ಅಕ್ಷರ e_function_nesting_too_deep[]
	INIT(= N_("E1058: Function nesting too deep"));
EXTERN ಅಕ್ಷರ e_no_white_space_allowed_before_colon_str[]
	INIT(= N_("E1059: No white space allowed before colon: %s"));
EXTERN ಅಕ್ಷರ e_expected_dot_after_name_str[]
	INIT(= N_("E1060: Expected dot after name: %s"));
EXTERN ಅಕ್ಷರ e_cannot_ಕಂಡುಹಿಡಿ_function_str[]
	INIT(= N_("E1061: Cannot find function %s"));
EXTERN ಅಕ್ಷರ e_cannot_ಸೂಚ್ಯಂಕ_ಸಂಖ್ಯೆ[]
	INIT(= N_("E1062: Cannot index a Number"));
EXTERN ಅಕ್ಷರ e_type_mismatch_for_v_variable[]
	INIT(= N_("E1063: Type mismatch for v: variable"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_yank_ರಿಜಿಸ್ಟರ್_ಬದಲಾಯಿತು_while_using_it[]
	INIT(= N_("E1064: Yank register changed while using it"));
EXTERN ಅಕ್ಷರ e_command_cannot_be_shortened_str[]
	INIT(= N_("E1065: Command cannot be shortened: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_declare_a_ರಿಜಿಸ್ಟರ್_str[]
	INIT(= N_("E1066: Cannot declare a register: %s"));
EXTERN ಅಕ್ಷರ e_separator_mismatch_str[]
	INIT(= N_("E1067: Separator mismatch: %s"));
EXTERN ಅಕ್ಷರ e_no_white_space_allowed_before_str_str[]
	INIT(= N_("E1068: No white space allowed before '%s': %s"));
EXTERN ಅಕ್ಷರ e_white_space_required_after_str_str[]
	INIT(= N_("E1069: White space required after '%s': %s"));
EXTERN ಅಕ್ಷರ e_invalid_string_for_import_str[]
	INIT(= N_("E1071: Invalid string for :import: %s"));
EXTERN ಅಕ್ಷರ e_cannot_compare_str_with_str[]
	INIT(= N_("E1072: Cannot compare %s with %s"));
EXTERN ಅಕ್ಷರ e_name_already_defined_str[]
	INIT(= N_("E1073: Name already defined: %s"));
EXTERN ಅಕ್ಷರ e_no_white_space_allowed_after_dot[]
	INIT(= N_("E1074: No white space allowed after dot"));
EXTERN ಅಕ್ಷರ e_namespace_not_supported_str[]
	INIT(= N_("E1075: Namespace not supported: %s"));
// E1076 unused (was deleted)
EXTERN ಅಕ್ಷರ e_missing_ವಾದ_type_for_str[]
	INIT(= N_("E1077: Missing argument type for %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_invalid_command_nested_did_you_ಸರಾಸರಿ_plusplus_nested[]
	INIT(= N_("E1078: Invalid command \"nested\", did you mean \"++nested\"?"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_declare_variable_on_command_ಸಾಲು[]
	INIT(= N_("E1079: Cannot declare a variable on the command line"));
EXTERN ಅಕ್ಷರ e_invalid_assignment[]
	INIT(= N_("E1080: Invalid assignment"));
EXTERN ಅಕ್ಷರ e_cannot_unlet_str[]
	INIT(= N_("E1081: Cannot unlet %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_command_modifier_without_command[]
	INIT(= N_("E1082: Command modifier without command"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_missing_backtick[]
	INIT(= N_("E1083: Missing backtick"));
EXTERN ಅಕ್ಷರ e_cannot_delete_vim9_script_function_str[]
	INIT(= N_("E1084: Cannot delete Vim9 script function %s"));
EXTERN ಅಕ್ಷರ e_not_callable_type_str[]
	INIT(= N_("E1085: Not a callable type: %s"));
// E1086 unused
EXTERN ಅಕ್ಷರ e_cannot_use_ಸೂಚ್ಯಂಕ_when_declaring_variable[]
	INIT(= N_("E1087: Cannot use an index when declaring a variable"));
EXTERN ಅಕ್ಷರ e_script_cannot_import_itself[]
	INIT(= N_("E1088: Script cannot import itself"));
EXTERN ಅಕ್ಷರ e_unknown_variable_str[]
	INIT(= N_("E1089: Unknown variable: %s"));
EXTERN ಅಕ್ಷರ e_cannot_ನಿಯೋಜಿಸು_ಗಮ್ಯ_ವಾದ_str[]
	INIT(= N_("E1090: Cannot assign to argument %s"));
EXTERN ಅಕ್ಷರ e_function_is_not_compiled_str[]
	INIT(= N_("E1091: Function is not compiled: %s"));
EXTERN ಅಕ್ಷರ e_cannot_nest_redir[]
	INIT(= N_("E1092: Cannot nest :redir"));
EXTERN ಅಕ್ಷರ e_expected_nr_ವಸ್ತುಗಳು_but_got_nr[]
	INIT(= N_("E1093: Expected %d items but got %d"));
EXTERN ಅಕ್ಷರ e_import_can_only_be_ಬಳಸಿದ_ಒಳ_script[]
	INIT(= N_("E1094: Import can only be used in a script"));
EXTERN ಅಕ್ಷರ e_unreachable_ಸಂಕೇತ_after_str[]
	INIT(= N_("E1095: Unreachable code after :%s"));
EXTERN ಅಕ್ಷರ e_returning_ಮೌಲ್ಯ_ಒಳ_function_without_return_type[]
	INIT(= N_("E1096: Returning a value in a function without a return type"));
EXTERN ಅಕ್ಷರ e_ಸಾಲು_incomplete[]
	INIT(= N_("E1097: Line incomplete"));
EXTERN ಅಕ್ಷರ e_string_list_or_blob_required[]
	INIT(= N_("E1098: String, List or Blob required"));
EXTERN ಅಕ್ಷರ e_unknown_error_while_executing_str[]
	INIT(= N_("E1099: Unknown error while executing %s"));
EXTERN ಅಕ್ಷರ e_command_not_supported_ಒಳ_vim9_script_missing_var_str[]
	INIT(= N_("E1100: Command not supported in Vim9 script (missing :var?): %s"));
EXTERN ಅಕ್ಷರ e_cannot_declare_script_variable_ಒಳ_function_str[]
	INIT(= N_("E1101: Cannot declare a script variable in a function: %s"));
EXTERN ಅಕ್ಷರ e_lambda_function_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E1102: Lambda function not found: %s"));
EXTERN ಅಕ್ಷರ e_dictionary_not_ಗಣ[]
	INIT(= N_("E1103: Dictionary not set"));
EXTERN ಅಕ್ಷರ e_missing_gt[]
	INIT(= N_("E1104: Missing >"));
EXTERN ಅಕ್ಷರ e_cannot_convert_str_ಗಮ್ಯ_string[]
	INIT(= N_("E1105: Cannot convert %s to string"));

PLURAL_ಸಂದೇಶ(e_one_ವಾದ_too_many, "E1106: One argument too many",
		e_nr_arguments_too_many, "E1106: %d arguments too many")

EXTERN ಅಕ್ಷರ e_string_list_dict_or_blob_required[]
	INIT(= N_("E1107: String, List, Dict or Blob required"));
// E1108 unused
EXTERN ಅಕ್ಷರ e_list_ಅಂಶ_nr_is_not_list[]
	INIT(= N_("E1109: List item %d is not a List"));
EXTERN ಅಕ್ಷರ e_list_ಅಂಶ_nr_does_not_contain_3_numbers[]
	INIT(= N_("E1110: List item %d does not contain 3 numbers"));
EXTERN ಅಕ್ಷರ e_list_ಅಂಶ_nr_range_invalid[]
	INIT(= N_("E1111: List item %d range invalid"));
EXTERN ಅಕ್ಷರ e_list_ಅಂಶ_nr_cell_ಅಗಲ_invalid[]
	INIT(= N_("E1112: List item %d cell width invalid"));
EXTERN ಅಕ್ಷರ e_overlapping_ranges_for_nr[]
	INIT(= N_("E1113: Overlapping ranges for 0x%lx"));
EXTERN ಅಕ್ಷರ e_only_ಮೌಲ್ಯಗಳು_ದ_0x80_and_higher_supported[]
	INIT(= N_("E1114: Only values of 0x80 and higher supported"));
EXTERN ಅಕ್ಷರ e_assert_fails_fourth_ವಾದ[]
	INIT(= N_("E1115: \"assert_fails()\" fourth argument must be a number"));
EXTERN ಅಕ್ಷರ e_assert_fails_fifth_ವಾದ[]
	INIT(= N_("E1116: \"assert_fails()\" fifth argument must be a string"));
EXTERN ಅಕ್ಷರ e_cannot_use_bang_with_nested_def_str[]
	INIT(= N_("E1117: Cannot use ! with nested %s"));
EXTERN ಅಕ್ಷರ e_cannot_change_locked_list[]
	INIT(= N_("E1118: Cannot change locked list"));
EXTERN ಅಕ್ಷರ e_cannot_change_locked_list_ಅಂಶ[]
	INIT(= N_("E1119: Cannot change locked list item"));
EXTERN ಅಕ್ಷರ e_cannot_change_dict[]
	INIT(= N_("E1120: Cannot change dict"));
EXTERN ಅಕ್ಷರ e_cannot_change_dict_ಅಂಶ[]
	INIT(= N_("E1121: Cannot change dict item"));
EXTERN ಅಕ್ಷರ e_variable_is_locked_str[]
	INIT(= N_("E1122: Variable is locked: %s"));
EXTERN ಅಕ್ಷರ e_missing_comma_before_ವಾದ_str[]
	INIT(= N_("E1123: Missing comma before argument: %s"));
EXTERN ಅಕ್ಷರ e_str_cannot_be_ಬಳಸಿದ_ಒಳ_legacy_vim_script[]
	INIT(= N_("E1124: \"%s\" cannot be used in legacy Vim script"));
EXTERN ಅಕ್ಷರ e_final_requires_a_ಮೌಲ್ಯ[]
	INIT(= N_("E1125: Final requires a value"));
EXTERN ಅಕ್ಷರ e_cannot_use_let_ಒಳ_vim9_script[]
	INIT(= N_("E1126: Cannot use :let in Vim9 script"));
EXTERN ಅಕ್ಷರ e_missing_name_after_dot[]
	INIT(= N_("E1127: Missing name after dot"));
EXTERN ಅಕ್ಷರ e_endblock_without_ಖಂಡ[]
	INIT(= N_("E1128: } without {"));
EXTERN ಅಕ್ಷರ e_throw_with_ಖಾಲಿಗಳು_string[]
	INIT(= N_("E1129: Throw with empty string"));
EXTERN ಅಕ್ಷರ e_cannot_ಕೂಡಿಸು_ಗಮ್ಯ_null_list[]
	INIT(= N_("E1130: Cannot add to null list"));
EXTERN ಅಕ್ಷರ e_cannot_ಕೂಡಿಸು_ಗಮ್ಯ_null_blob[]
	INIT(= N_("E1131: Cannot add to null blob"));
EXTERN ಅಕ್ಷರ e_missing_function_ವಾದ[]
	INIT(= N_("E1132: Missing function argument"));
EXTERN ಅಕ್ಷರ e_cannot_extend_null_dict[]
	INIT(= N_("E1133: Cannot extend a null dict"));
EXTERN ಅಕ್ಷರ e_cannot_extend_null_list[]
	INIT(= N_("E1134: Cannot extend a null list"));
EXTERN ಅಕ್ಷರ e_using_string_as_bool_str[]
	INIT(= N_("E1135: Using a String as a Bool: \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cmd_mapping_must_ಕೊನೆ_with_cr_before_ಎರಡನೇ_cmd[]
	INIT(= N_("E1136: <Cmd> mapping must end with <CR> before second <Cmd>"));
// E1137 unused
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_using_bool_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E1138: Using a Bool as a Number"));
EXTERN ಅಕ್ಷರ e_missing_matching_bracket_after_dict_ಕೀಲಿ[]
	INIT(= N_("E1139: Missing matching bracket after dict key"));
EXTERN ಅಕ್ಷರ e_for_ವಾದ_must_be_sequence_ದ_lists_or_tuples[]
	INIT(= N_("E1140: :for argument must be a sequence of lists or tuples"));
EXTERN ಅಕ್ಷರ e_indexable_type_required[]
	INIT(= N_("E1141: Indexable type required"));
EXTERN ಅಕ್ಷರ e_calling_test_garbagecollect_now_while_v_testing_is_not_ಗಣ[]
	INIT(= N_("E1142: Calling test_garbagecollect_now() while v:testing is not set"));
EXTERN ಅಕ್ಷರ e_ಖಾಲಿಗಳು_ಅಭಿವ್ಯಕ್ತಿ_str[]
	INIT(= N_("E1143: Empty expression: \"%s\""));
EXTERN ಅಕ್ಷರ e_command_str_not_followed_by_white_space_str[]
	INIT(= N_("E1144: Command \"%s\" is not followed by white space: %s"));
EXTERN ಅಕ್ಷರ e_missing_heredoc_ಕೊನೆ_marker_str[]
	INIT(= N_("E1145: Missing heredoc end marker: %s"));
EXTERN ಅಕ್ಷರ e_command_not_recognized_str[]
	INIT(= N_("E1146: Command not recognized: %s"));
EXTERN ಅಕ್ಷರ e_list_not_ಗಣ[]
	INIT(= N_("E1147: List not set"));
EXTERN ಅಕ್ಷರ e_cannot_ಸೂಚ್ಯಂಕ_str[]
	INIT(= N_("E1148: Cannot index a %s"));
EXTERN ಅಕ್ಷರ e_script_variable_invalid_after_reload_ಒಳ_function_str[]
	INIT(= N_("E1149: Script variable is invalid after reload in function %s"));
EXTERN ಅಕ್ಷರ e_script_variable_type_ಬದಲಾಯಿತು[]
	INIT(= N_("E1150: Script variable type changed"));
EXTERN ಅಕ್ಷರ e_mismatched_endfunction[]
	INIT(= N_("E1151: Mismatched endfunction"));
EXTERN ಅಕ್ಷರ e_mismatched_enddef[]
	INIT(= N_("E1152: Mismatched enddef"));
EXTERN ಅಕ್ಷರ e_invalid_operation_for_str[]
	INIT(= N_("E1153: Invalid operation for %s"));
EXTERN ಅಕ್ಷರ e_divide_by_zero[]
	INIT(= N_("E1154: Divide by zero"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_define_autocommands_for_all_events[]
	INIT(= N_("E1155: Cannot define autocommands for ALL events"));
EXTERN ಅಕ್ಷರ e_cannot_change_arglist_recursively[]
	INIT(= N_("E1156: Cannot change the argument list recursively"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_missing_return_type[]
	INIT(= N_("E1157: Missing return type"));
EXTERN ಅಕ್ಷರ e_cannot_use_flatten_ಒಳ_vim9_script[]
	INIT(= N_("E1158: Cannot use flatten() in Vim9 script, use flattennew()"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_ವಿಭಜಿಸು_ಕಿಟಕಿ_when_closing_ಬಫರ್[]
	INIT(= N_("E1159: Cannot split a window when closing the buffer"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_use_default_for_variable_arguments[]
	INIT(= N_("E1160: Cannot use a default for variable arguments"));
EXTERN ಅಕ್ಷರ e_cannot_json_encode_str[]
	INIT(= N_("E1161: Cannot json encode a %s"));
EXTERN ಅಕ್ಷರ e_ರಿಜಿಸ್ಟರ್_name_must_be_one_char_str[]
	INIT(= N_("E1162: Register name must be one character: %s"));
EXTERN ಅಕ್ಷರ e_variable_nr_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1163: Variable %d: type mismatch, expected %s but got %s"));
EXTERN ಅಕ್ಷರ e_variable_nr_type_mismatch_expected_str_but_got_str_ಒಳ_str[]
	INIT(= N_("E1163: Variable %d: type mismatch, expected %s but got %s in %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_vim9cmd_must_be_followed_by_command[]
	INIT(= N_("E1164: vim9cmd must be followed by a command"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_use_range_with_assignment_str[]
	INIT(= N_("E1165: Cannot use a range with an assignment: %s"));
EXTERN ಅಕ್ಷರ e_cannot_use_range_with_dictionary[]
	INIT(= N_("E1166: Cannot use a range with a dictionary"));
EXTERN ಅಕ್ಷರ e_ವಾದ_name_shadows_existing_variable_str[]
	INIT(= N_("E1167: Argument name shadows existing variable: %s"));
EXTERN ಅಕ್ಷರ e_ವಾದ_already_declared_ಒಳ_script_str[]
	INIT(= N_("E1168: Argument already declared in the script: %s"));
EXTERN ಅಕ್ಷರ e_ಅಭಿವ್ಯಕ್ತಿ_too_recursive_str[]
	INIT(= N_("E1169: Expression too recursive: %s"));
EXTERN ಅಕ್ಷರ e_cannot_use_hash_curly_ಗಮ್ಯ_ಆರಂಭ_comment[]
	INIT(= N_("E1170: Cannot use #{ to start a comment"));
EXTERN ಅಕ್ಷರ e_missing_ಕೊನೆ_ಖಂಡ[]
	INIT(= N_("E1171: Missing } after inline function"));
EXTERN ಅಕ್ಷರ e_cannot_use_default_ಮೌಲ್ಯಗಳು_ಒಳ_lambda[]
	INIT(= N_("E1172: Cannot use default values in a lambda"));
EXTERN ಅಕ್ಷರ e_ಪಠ್ಯ_ಸಿಕ್ಕಿತು_after_str_str[]
	INIT(= N_("E1173: Text found after %s: %s"));
EXTERN ಅಕ್ಷರ e_string_required_for_ವಾದ_nr[]
	INIT(= N_("E1174: String required for argument %d"));
EXTERN ಅಕ್ಷರ e_non_ಖಾಲಿಗಳು_string_required_for_ವಾದ_nr[]
	INIT(= N_("E1175: Non-empty string required for argument %d"));
EXTERN ಅಕ್ಷರ e_misplaced_command_modifier[]
	INIT(= N_("E1176: Misplaced command modifier"));
EXTERN ಅಕ್ಷರ e_for_loop_on_str_not_supported[]
	INIT(= N_("E1177: For loop on %s not supported"));
EXTERN ಅಕ್ಷರ e_cannot_ಬೀಗ_unlock_local_variable[]
	INIT(= N_("E1178: Cannot lock or unlock a local variable"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
EXTERN ಅಕ್ಷರ e_failed_ಗಮ್ಯ_ಹೊರತೆಗೆ_pwd_from_str_check_your_shell_config[]
	INIT(= N_("E1179: Failed to extract PWD from %s, check your shell's config related to OSC 7"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_variable_arguments_type_must_be_list_str[]
	INIT(= N_("E1180: Variable arguments type must be a list: %s"));
EXTERN ಅಕ್ಷರ e_cannot_use_underscore_ಇಲ್ಲಿ[]
	INIT(= N_("E1181: Cannot use an underscore here"));
EXTERN ಅಕ್ಷರ e_cannot_define_dict_func_ಒಳ_vim9_script_str[]
	INIT(= N_("E1182: Cannot define a dict function in Vim9 script: %s"));
EXTERN ಅಕ್ಷರ e_cannot_use_range_with_assignment_operator_str[]
	INIT(= N_("E1183: Cannot use a range with an assignment operator: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_blob_not_ಗಣ[]
	INIT(= N_("E1184: Blob not set"));
EXTERN ಅಕ್ಷರ e_missing_redir_ಕೊನೆ[]
	INIT(= N_("E1185: Missing :redir END"));
EXTERN ಅಕ್ಷರ e_ಅಭಿವ್ಯಕ್ತಿ_does_not_ಫಲಿತಾಂಶ_ಒಳ_ಮೌಲ್ಯ_str[]
	INIT(= N_("E1186: Expression does not result in a value: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_failed_ಗಮ್ಯ_ಮೂಲ_defaults[]
	INIT(= N_("E1187: Failed to source defaults.vim"));
#ಆದರೆ defined(FEAT_TERMINAL)
EXTERN ಅಕ್ಷರ e_cannot_open_terminal_from_command_ಸಾಲು_ಕಿಟಕಿ[]
	INIT(= N_("E1188: Cannot open a terminal from the command line window"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_use_legacy_with_command_str[]
	INIT(= N_("E1189: Cannot use :legacy with this command: %s"));

PLURAL_ಸಂದೇಶ(e_one_ವಾದ_too_few, "E1190: One argument too few",
		e_nr_arguments_too_few, "E1190: %d arguments too few")

EXTERN ಅಕ್ಷರ e_call_ಗಮ್ಯ_function_that_failed_ಗಮ್ಯ_compile_str[]
	INIT(= N_("E1191: Call to function that failed to compile: %s"));
EXTERN ಅಕ್ಷರ e_ಖಾಲಿಗಳು_function_name[]
	INIT(= N_("E1192: Empty function name"));
#ಆದರೆ_ಅಂತ್ಯ
// libsodium
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
# ifndef FEAT_SODIUM
EXTERN ಅಕ್ಷರ e_libsodium_not_built_ಒಳ[]
	INIT(= N_("E1193: cryptmethod xchacha20 not built into this Vim"));
# ಇಲ್ಲದಿದ್ದರೆ
#  ಆದರೆ 0
EXTERN ಅಕ್ಷರ e_libsodium_cannot_encrypt_ಶೀರ್ಷಿಕೆ[]
	INIT(= N_("E1194: Cannot encrypt header, not enough space"));
EXTERN ಅಕ್ಷರ e_libsodium_cannot_encrypt_ಬಫರ್[]
	INIT(= N_("E1195: Cannot encrypt buffer, not enough space"));
EXTERN ಅಕ್ಷರ e_libsodium_cannot_decrypt_ಶೀರ್ಷಿಕೆ[]
	INIT(= N_("E1196: Cannot decrypt header, not enough space"));
#  endif
EXTERN ಅಕ್ಷರ e_libsodium_cannot_allocate_ಬಫರ್[]
	INIT(= N_("E1197: Cannot allocate_buffer for encryption"));
EXTERN ಅಕ್ಷರ e_libsodium_decryption_failed_ಶೀರ್ಷಿಕೆ_incomplete[]
	INIT(= N_("E1198: Decryption failed: Header incomplete!"));
#  ಆದರೆ 0
EXTERN ಅಕ್ಷರ e_libsodium_cannot_decrypt_ಬಫರ್[]
	INIT(= N_("E1199: Cannot decrypt buffer, not enough space"));
#  endif
EXTERN ಅಕ್ಷರ e_libsodium_decryption_failed[]
	INIT(= N_("E1200: Decryption failed!"));
EXTERN ಅಕ್ಷರ e_libsodium_decryption_failed_premature[]
	INIT(= N_("E1201: Decryption failed: pre-mature end of file!"));
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_no_white_space_allowed_after_str_str[]
	INIT(= N_("E1202: No white space allowed after '%s': %s"));
EXTERN ಅಕ್ಷರ e_dot_not_allowed_after_str_str[]
	INIT(= N_("E1203: Dot not allowed after a %s: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_regexp_ಸಂಖ್ಯೆ_after_dot_pos_ಹುಡುಕು_chr[]
	INIT(= N_("E1204: No Number allowed after .: '\\%%%c'"));
EXTERN ಅಕ್ಷರ e_no_white_space_allowed_between_option_and[]
	INIT(= N_("E1205: No white space allowed between option and"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_dict_required_for_ವಾದ_nr[]
	INIT(= N_("E1206: Dictionary required for argument %d"));
EXTERN ಅಕ್ಷರ e_ಅಭಿವ್ಯಕ್ತಿ_without_effect_str[]
	INIT(= N_("E1207: Expression without an effect: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_complete_ಬಳಸಿದ_without_allowing_arguments[]
	INIT(= N_("E1208: -complete used without allowing arguments"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_invalid_ಮೌಲ್ಯ_for_ಸಾಲು_ಸಂಖ್ಯೆ_str[]
	INIT(= N_("E1209: Invalid value for a line number: \"%s\""));
EXTERN ಅಕ್ಷರ e_ಸಂಖ್ಯೆ_required_for_ವಾದ_nr[]
	INIT(= N_("E1210: Number required for argument %d"));
EXTERN ಅಕ್ಷರ e_list_required_for_ವಾದ_nr[]
	INIT(= N_("E1211: List required for argument %d"));
EXTERN ಅಕ್ಷರ e_bool_required_for_ವಾದ_nr[]
	INIT(= N_("E1212: Bool required for argument %d"));
EXTERN ಅಕ್ಷರ e_redefining_imported_ಅಂಶ_str[]
	INIT(= N_("E1213: Redefining imported item \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_DIGRAPHS)
EXTERN ಅಕ್ಷರ e_digraph_must_be_just_two_characters_str[]
	INIT(= N_("E1214: Digraph must be just two characters: %s"));
EXTERN ಅಕ್ಷರ e_digraph_ವಾದ_must_be_one_character_str[]
	INIT(= N_("E1215: Digraph must be one character: %s"));
EXTERN ಅಕ್ಷರ e_digraph_setlist_ವಾದ_must_be_list_ದ_lists_with_two_ವಸ್ತುಗಳು[]
	INIT(= N_("E1216: digraph_setlist() argument must be a list of lists with two items"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# ifdef FEAT_ಕೆಲಸ_CHANNEL
EXTERN ಅಕ್ಷರ e_chan_or_ಕೆಲಸ_required_for_ವಾದ_nr[]
	INIT(= N_("E1217: Channel or Job required for argument %d"));
EXTERN ಅಕ್ಷರ e_ಕೆಲಸ_required_for_ವಾದ_nr[]
	INIT(= N_("E1218: Job required for argument %d"));
# endif
EXTERN ಅಕ್ಷರ e_float_or_ಸಂಖ್ಯೆ_required_for_ವಾದ_nr[]
	INIT(= N_("E1219: Float or Number required for argument %d"));
EXTERN ಅಕ್ಷರ e_string_or_ಸಂಖ್ಯೆ_required_for_ವಾದ_nr[]
	INIT(= N_("E1220: String or Number required for argument %d"));
EXTERN ಅಕ್ಷರ e_string_or_blob_required_for_ವಾದ_nr[]
	INIT(= N_("E1221: String or Blob required for argument %d"));
EXTERN ಅಕ್ಷರ e_string_or_list_required_for_ವಾದ_nr[]
	INIT(= N_("E1222: String or List required for argument %d"));
EXTERN ಅಕ್ಷರ e_string_or_dict_required_for_ವಾದ_nr[]
	INIT(= N_("E1223: String or Dictionary required for argument %d"));
EXTERN ಅಕ್ಷರ e_string_ಸಂಖ್ಯೆ_or_list_required_for_ವಾದ_nr[]
	INIT(= N_("E1224: String, Number or List required for argument %d"));
EXTERN ಅಕ್ಷರ e_string_list_tuple_or_dict_required_for_ವಾದ_nr[]
	INIT(= N_("E1225: String, List, Tuple or Dictionary required for argument %d"));
EXTERN ಅಕ್ಷರ e_list_or_blob_required_for_ವಾದ_nr[]
	INIT(= N_("E1226: List or Blob required for argument %d"));
EXTERN ಅಕ್ಷರ e_list_or_dict_required_for_ವಾದ_nr[]
	INIT(= N_("E1227: List or Dictionary required for argument %d"));
EXTERN ಅಕ್ಷರ e_list_dict_or_blob_required_for_ವಾದ_nr[]
	INIT(= N_("E1228: List, Dictionary or Blob required for argument %d"));
EXTERN ಅಕ್ಷರ e_expected_dictionary_for_using_ಕೀಲಿ_str_but_got_str[]
	INIT(= N_("E1229: Expected dictionary for using key \"%s\", but got %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SODIUM
EXTERN ಅಕ್ಷರ e_encryption_sodium_mlock_failed[]
	INIT(= N_("E1230: Encryption: sodium_mlock() failed"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_use_bar_ಗಮ್ಯ_separate_commands_ಇಲ್ಲಿ_str[]
	INIT(= N_("E1231: Cannot use a bar to separate commands here: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ವಾದ_ದ_exists_compiled_must_be_literal_string[]
	INIT(= N_("E1232: Argument of exists_compiled() must be a literal string"));
EXTERN ಅಕ್ಷರ e_exists_compiled_can_only_be_ಬಳಸಿದ_ಒಳ_def_function[]
	INIT(= N_("E1233: exists_compiled() can only be used in a :def function"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_legacy_must_be_followed_by_command[]
	INIT(= N_("E1234: legacy must be followed by a command"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_bool_or_ಸಂಖ್ಯೆ_required_for_ವಾದ_nr[]
	INIT(= N_("E1235: Bool or Number required for argument %d"));
EXTERN ಅಕ್ಷರ e_cannot_use_str_itself_it_is_imported[]
	INIT(= N_("E1236: Cannot use %s itself, it is imported"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_no_such_user_defined_command_ಒಳ_current_ಬಫರ್_str[]
	INIT(= N_("E1237: No such user-defined command in current buffer: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_blob_required_for_ವಾದ_nr[]
	INIT(= N_("E1238: Blob required for argument %d"));
EXTERN ಅಕ್ಷರ e_invalid_ಮೌಲ್ಯ_for_blob_nr[]
	INIT(= N_("E1239: Invalid value for blob: 0x%lX"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_resulting_ಪಠ್ಯ_too_long[]
	INIT(= N_("E1240: Resulting text too long"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_separator_not_supported_str[]
	INIT(= N_("E1241: Separator not supported: %s"));
EXTERN ಅಕ್ಷರ e_no_white_space_allowed_before_separator_str[]
	INIT(= N_("E1242: No white space allowed before separator: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)
EXTERN ಅಕ್ಷರ e_ascii_ಸಂಕೇತ_not_ಒಳ_range[]
	INIT(= N_("E1243: ASCII code not in 32-127 range"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
EXTERN ಅಕ್ಷರ e_bad_color_string_str[]
	INIT(= N_("E1244: Bad color string: %s"));
# endif
EXTERN ಅಕ್ಷರ e_cannot_expand_sfile_ಒಳ_vim9_function[]
	INIT(= N_("E1245: Cannot expand <sfile> in a Vim9 function"));
EXTERN ಅಕ್ಷರ e_cannot_ಕಂಡುಹಿಡಿ_variable_ಗಮ್ಯ_unlock_str[]
	INIT(= N_("E1246: Cannot find variable to (un)lock: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಸಾಲು_ಸಂಖ್ಯೆ_ಹೊರಹೋಗು_ದ_range[]
	INIT(= N_("E1247: Line number out of range"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ಆವರಣ_called_from_invalid_context[]
	INIT(= N_("E1248: Closure called from invalid context"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_highlight_ಗುಂಪು_name_too_long[]
	INIT(= N_("E1249: Highlight group name too long"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ವಾದ_ದ_str_must_be_list_string_dictionary_or_blob[]
	INIT(= N_("E1250: Argument of %s must be a List, String, Dictionary or Blob"));
EXTERN ಅಕ್ಷರ e_list_tuple_dict_blob_or_string_required_for_ವಾದ_nr[]
	INIT(= N_("E1251: List, Tuple, Dictionary, Blob or String required for argument %d"));
EXTERN ಅಕ್ಷರ e_string_list_or_blob_required_for_ವಾದ_nr[]
	INIT(= N_("E1252: String, List or Blob required for argument %d"));
EXTERN ಅಕ್ಷರ e_string_list_tuple_or_blob_required_for_ವಾದ_nr[]
	INIT(= N_("E1253: String, List, Tuple or Blob required for argument %d"));
EXTERN ಅಕ್ಷರ e_cannot_use_script_variable_ಒಳ_for_loop[]
	INIT(= N_("E1254: Cannot use script variable in for loop"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cmd_mapping_must_ಕೊನೆ_with_cr[]
	INIT(= N_("E1255: <Cmd> mapping must end with <CR>"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_string_or_function_required_for_ವಾದ_nr[]
	INIT(= N_("E1256: String or function required for argument %d"));
EXTERN ಅಕ್ಷರ e_imported_script_must_use_as_or_ಕೊನೆ_ಒಳ_dot_vim_str[]
	INIT(= N_("E1257: Imported script must use \"as\" or end in .vim: %s"));
EXTERN ಅಕ್ಷರ e_no_dot_after_imported_name_str[]
	INIT(= N_("E1258: No '.' after imported name: %s"));
EXTERN ಅಕ್ಷರ e_missing_name_after_imported_name_str[]
	INIT(= N_("E1259: Missing name after imported name: %s"));
EXTERN ಅಕ್ಷರ e_cannot_unlet_imported_ಅಂಶ_str[]
	INIT(= N_("E1260: Cannot unlet an imported item: %s"));
EXTERN ಅಕ್ಷರ e_cannot_import_dot_vim_without_using_as[]
	INIT(= N_("E1261: Cannot import .vim without using \"as\""));
EXTERN ಅಕ್ಷರ e_cannot_import_same_script_twice_str[]
	INIT(= N_("E1262: Cannot import the same script twice: %s"));
EXTERN ಅಕ್ಷರ e_cannot_use_name_with_hash_ಒಳ_vim9_script_use_export_instead[]
	INIT(= N_("E1263: Cannot use name with # in Vim9 script, use export instead"));
EXTERN ಅಕ್ಷರ e_autoload_import_cannot_use_absolute_or_relative_ಮಾರ್ಗ[]
	INIT(= N_("E1264: Autoload import cannot use absolute or relative path: %s"));
EXTERN ಅಕ್ಷರ e_cannot_use_partial_ಇಲ್ಲಿ[]
	INIT(= N_("E1265: Cannot use a partial here"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PYTHON3) && defined(MSWIN)
EXTERN ಅಕ್ಷರ e_critical_error_ಒಳ_python3_initialization_check_your_installation[]
	INIT(= N_("E1266: Critical error in python3 initialization, check your python3 installation"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_function_name_must_ಆರಂಭ_with_capital_str[]
	INIT(= N_("E1267: Function name must start with a capital: %s"));
EXTERN ಅಕ್ಷರ e_cannot_use_s_colon_ಒಳ_vim9_script_str[]
	INIT(= N_("E1268: Cannot use s: in Vim9 script: %s"));
EXTERN ಅಕ್ಷರ e_cannot_create_vim9_script_variable_ಒಳ_function_str[]
	INIT(= N_("E1269: Cannot create a Vim9 script variable in a function: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_use_s_backslash_ಒಳ_vim9_script[]
	INIT(= N_("E1270: Cannot use :s\\/sub/ in Vim9 script"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_compiling_ಆವರಣ_without_context_str[]
	INIT(= N_("E1271: Compiling closure without context: %s"));
EXTERN ಅಕ್ಷರ e_using_type_not_ಒಳ_script_context_str[]
	INIT(= N_("E1272: Using type not in a script context: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_nfa_regexp_missing_ಮೌಲ್ಯ_ಒಳ_chr[]
	INIT(= N_("E1273: (NFA regexp) missing value in '\\%%%c'"));
EXTERN ಅಕ್ಷರ e_no_script_file_name_ಗಮ್ಯ_substitute_for_script[]
	INIT(= N_("E1274: No script file name to substitute for \"<script>\""));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_string_or_function_required_for_arrow_parens_ಅಭಿವ್ಯಕ್ತಿ[]
	INIT(= N_("E1275: String or function required for ->(expr)"));
EXTERN ಅಕ್ಷರ e_illegal_map_ಬಹುಲಕ_string_str[]
	INIT(= N_("E1276: Illegal map mode string: '%s'"));
# ಆದರೆ !defined(FEAT_ಕೆಲಸ_CHANNEL)
EXTERN ಅಕ್ಷರ e_channel_ಕೆಲಸ_feature_not_ಲಭ್ಯ[]
	INIT(= N_("E1277: Channel and job feature is not available"));
# endif
EXTERN ಅಕ್ಷರ e_stray_closing_curly_str[]
	INIT(= N_("E1278: Stray '}' without a matching '{': %s"));
EXTERN ಅಕ್ಷರ e_missing_close_curly_str[]
	INIT(= N_("E1279: Missing '}': %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
EXTERN ಅಕ್ಷರ e_illegal_character_ಒಳ_ಶಬ್ದ[]
	INIT(= N_("E1280: Illegal character in word"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_atom_engine_must_be_at_ಆರಂಭ_ದ_ನಮೂನೆ[]
	INIT(= N_("E1281: Atom '\\%%#=%c' must be at the start of the pattern"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_bitshift_ops_must_be_ಸಂಖ್ಯೆ[]
	INIT(= N_("E1282: Bitshift operands must be numbers"));
EXTERN ಅಕ್ಷರ e_bitshift_ops_must_be_ಧನಾತ್ಮಕ[]
	INIT(= N_("E1283: Bitshift amount must be a positive number"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PROP_POPUP)
EXTERN ಅಕ್ಷರ e_ವಾದ_1_list_ಅಂಶ_nr_dictionary_required[]
	INIT(= N_("E1284: Argument 1, list item %d: Dictionary required"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME
EXTERN ಅಕ್ಷರ e_could_not_clear_timeout_str[]
	INIT(= N_("E1285: Could not clear timeout: %s"));
EXTERN ಅಕ್ಷರ e_could_not_ಗಣ_timeout_str[]
	INIT(= N_("E1286: Could not set timeout: %s"));
# ifndef PROF_NSEC
EXTERN ಅಕ್ಷರ e_could_not_ಗಣ_handler_for_timeout_str[]
	INIT(= N_("E1287: Could not set handler for timeout: %s"));
EXTERN ಅಕ್ಷರ e_could_not_reset_handler_for_timeout_str[]
	INIT(= N_("E1288: Could not reset handler for timeout: %s"));
EXTERN ಅಕ್ಷರ e_could_not_check_for_pending_sigalrm_str[]
	INIT(= N_("E1289: Could not check for pending SIGALRM: %s"));
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_substitute_nesting_too_deep[]
	INIT(= N_("E1290: substitute nesting too deep"));
# ifdef MSWIN
EXTERN ಅಕ್ಷರ e_invalid_ವಾದ_nr[]
	INIT(= N_("E1291: Invalid argument: %ld"));
# endif
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cmdline_ಕಿಟಕಿ_already_open[]
	INIT(= N_("E1292: Command-line window is already open"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_cannot_use_negative_id[]
	INIT(= N_("E1293: Cannot use a negative id for a text property"));
EXTERN ಅಕ್ಷರ e_can_only_use_ಪಠ್ಯ_align_when_ಸ್ತಂಭ_is_zero[]
	INIT(= N_("E1294: Can only use text_align when column is zero"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_cannot_specify_both_type_and_types[]
	INIT(= N_("E1295: Cannot specify both 'type' and 'types'"));
EXTERN ಅಕ್ಷರ e_can_only_use_ಎಡ_padding_when_ಸ್ತಂಭ_is_zero[]
	INIT(= N_("E1296: Can only use left padding when column is zero"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_non_null_dict_required_for_ವಾದ_nr[]
	INIT(= N_("E1297: Non-NULL Dictionary required for argument %d"));
EXTERN ಅಕ್ಷರ e_non_null_list_required_for_ವಾದ_nr[]
	INIT(= N_("E1298: Non-NULL List required for argument %d"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಕಿಟಕಿ_unexpectedly_close_while_searching_for_ಟ್ಯಾಗುಗಳು[]
	INIT(= N_("E1299: Window unexpectedly closed while searching for tags"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_use_partial_with_dictionary_for_defer[]
	INIT(= N_("E1300: Cannot use a partial with dictionary for :defer"));
EXTERN ಅಕ್ಷರ e_repeatable_type_required_for_ವಾದ_nr[]
	INIT(= N_("E1301: String, Number, List, Tuple or Blob required for argument %d"));
EXTERN ಅಕ್ಷರ e_script_variable_was_deleted[]
	INIT(= N_("E1302: Script variable was deleted"));
EXTERN ಅಕ್ಷರ e_custom_list_completion_function_does_not_return_list_but_str[]
	INIT(= N_("E1303: Custom list completion function does not return a List but a %s"));
EXTERN ಅಕ್ಷರ e_cannot_use_type_with_this_variable_str[]
	INIT(= N_("E1304: Cannot use type with this variable: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_cannot_use_ಉದ್ದಳತೆ_endcol_and_endlnum_with_ಪಠ್ಯ[]
	INIT(= N_("E1305: Cannot use \"length\", \"end_col\" and \"end_lnum\" with \"text\""));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_loop_nesting_too_deep[]
	INIT(= N_("E1306: Loop nesting too deep"));
EXTERN ಅಕ್ಷರ e_ವಾದ_nr_trying_ಗಮ್ಯ_modify_const_str[]
	INIT(= N_("E1307: Argument %d: Trying to modify a const %s"));
EXTERN ಅಕ್ಷರ e_cannot_resize_ಕಿಟಕಿ_ಒಳ_another_tab_ಪುಟ[]
	INIT(= N_("E1308: Cannot resize a window in another tab page"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_change_mappings_while_listing[]
	INIT(= N_("E1309: Cannot change mappings while listing"));
#ಆದರೆ defined(FEAT_MENU)
EXTERN ಅಕ್ಷರ e_cannot_change_menus_while_listing[]
	INIT(= N_("E1310: Cannot change menus while listing"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_change_user_commands_while_listing[]
	INIT(= N_("E1311: Cannot change user commands while listing"));
EXTERN ಅಕ್ಷರ e_not_allowed_ಗಮ್ಯ_change_ಕಿಟಕಿ_layout_ಒಳ_this_autocmd[]
	INIT(= N_("E1312: Not allowed to change the window layout in this autocmd"));
EXTERN ಅಕ್ಷರ e_not_allowed_ಗಮ್ಯ_ಕೂಡಿಸು_or_remove_entries_str[]
	INIT(= N_("E1313: Not allowed to add or remove entries (%s)"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_class_name_must_ಆರಂಭ_with_uppercase_letter_str[]
	INIT(= N_("E1314: Class name must start with an uppercase letter: %s"));
EXTERN ಅಕ್ಷರ e_white_space_required_after_name_str[]
	INIT(= N_("E1315: White space required after name: %s"));
EXTERN ಅಕ್ಷರ e_class_can_only_be_defined_ಒಳ_vim9_script[]
	INIT(= N_("E1316: Class can only be defined in Vim9 script"));
EXTERN ಅಕ್ಷರ e_invalid_object_variable_declaration_str[]
	INIT(= N_("E1317: Invalid object variable declaration: %s"));
EXTERN ಅಕ್ಷರ e_not_valid_command_ಒಳ_class_str[]
	INIT(= N_("E1318: Not a valid command in a class: %s"));
// E1319 unused
EXTERN ಅಕ್ಷರ e_using_object_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E1320: Using an Object as a Number"));
// E1321 unused
EXTERN ಅಕ್ಷರ e_using_object_as_float[]
	INIT(= N_("E1322: Using an Object as a Float"));
// E1323 unused
EXTERN ಅಕ್ಷರ e_using_object_as_string[]
	INIT(= N_("E1324: Using an Object as a String"));
EXTERN ಅಕ್ಷರ e_ವಿಧಾನ_not_ಸಿಕ್ಕಿತು_on_class_str_str[]
	INIT(= N_("E1325: Method \"%s\" not found in class \"%s\""));
EXTERN ಅಕ್ಷರ e_variable_not_ಸಿಕ್ಕಿತು_on_object_str_str[]
	INIT(= N_("E1326: Variable \"%s\" not found in object \"%s\""));
EXTERN ಅಕ್ಷರ e_object_required_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E1327: Object required, found %s"));
EXTERN ಅಕ್ಷರ e_constructor_default_ಮೌಲ್ಯ_must_be_vnone_str[]
	INIT(= N_("E1328: Constructor default value must be v:none: %s"));
EXTERN ಅಕ್ಷರ e_invalid_class_variable_declaration_str[]
	INIT(= N_("E1329: Invalid class variable declaration: %s"));
EXTERN ಅಕ್ಷರ e_invalid_type_ಒಳ_variable_declaration_str[]
	INIT(= N_("E1330: Invalid type used in variable declaration: %s"));
EXTERN ಅಕ್ಷರ e_ಬಹಿರಂಗ_must_be_followed_by_var_static_final_or_const[]
	INIT(= N_("E1331: public must be followed by \"var\" or \"static\" or \"final\" or \"const\""));
EXTERN ಅಕ್ಷರ e_ಬಹಿರಂಗ_variable_name_cannot_ಆರಂಭ_with_underscore_str[]
	INIT(= N_("E1332: public variable name cannot start with underscore: %s"));
EXTERN ಅಕ್ಷರ e_cannot_access_protected_variable_str[]
	INIT(= N_("E1333: Cannot access protected variable \"%s\" in class \"%s\""));
// E1334 unused
EXTERN ಅಕ್ಷರ e_variable_is_not_writable_str[]
	INIT(= N_("E1335: Variable \"%s\" in class \"%s\" is not writable"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಆಂತರಿಕ_error_shortmess_too_long[]
	INIT(= "E1336: Internal error: shortmess too long");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_class_variable_str_not_ಸಿಕ್ಕಿತು_ಒಳ_class_str[]
	INIT(= N_("E1337: Class variable \"%s\" not found in class \"%s\""));
#ಆದರೆ_ಅಂತ್ಯ
// E1338 and E1339 unused
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_ವಾದ_already_declared_ಒಳ_class_str[]
	INIT(= N_("E1340: Argument already declared in the class: %s"));
EXTERN ಅಕ್ಷರ e_variable_already_declared_ಒಳ_class_str[]
	INIT(= N_("E1341: Variable already declared in the class: %s"));
EXTERN ಅಕ್ಷರ e_interface_can_only_be_defined_ಒಳ_vim9_script[]
	INIT(= N_("E1342: Interface can only be defined in Vim9 script"));
EXTERN ಅಕ್ಷರ e_interface_name_must_ಆರಂಭ_with_uppercase_letter_str[]
	INIT(= N_("E1343: Interface name must start with an uppercase letter: %s"));
EXTERN ಅಕ್ಷರ e_cannot_initialize_variable_ಒಳ_interface[]
	INIT(= N_("E1344: Cannot initialize a variable in an interface"));
EXTERN ಅಕ್ಷರ e_not_valid_command_ಒಳ_interface_str[]
	INIT(= N_("E1345: Not a valid command in an interface: %s"));
EXTERN ಅಕ್ಷರ e_interface_name_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E1346: Interface name not found: %s"));
EXTERN ಅಕ್ಷರ e_not_valid_interface_str[]
	INIT(= N_("E1347: Not a valid interface: %s"));
EXTERN ಅಕ್ಷರ e_variable_str_ದ_interface_str_not_implemented[]
	INIT(= N_("E1348: Variable \"%s\" of interface \"%s\" is not implemented"));
EXTERN ಅಕ್ಷರ e_ವಿಧಾನ_str_ದ_interface_str_not_implemented[]
	INIT(= N_("E1349: Method \"%s\" of interface \"%s\" is not implemented"));
EXTERN ಅಕ್ಷರ e_duplicate_implements[]
	INIT(= N_("E1350: Duplicate \"implements\""));
EXTERN ಅಕ್ಷರ e_duplicate_interface_after_implements_str[]
	INIT(= N_("E1351: Duplicate interface after \"implements\": %s"));
EXTERN ಅಕ್ಷರ e_duplicate_extends[]
	INIT(= N_("E1352: Duplicate \"extends\""));
EXTERN ಅಕ್ಷರ e_class_name_not_ಸಿಕ್ಕಿತು_str[]
	INIT(= N_("E1353: Class name not found: %s"));
EXTERN ಅಕ್ಷರ e_cannot_extend_str[]
	INIT(= N_("E1354: Cannot extend %s"));
EXTERN ಅಕ್ಷರ e_duplicate_function_str[]
	INIT(= N_("E1355: Duplicate function: %s"));
EXTERN ಅಕ್ಷರ e_super_must_be_followed_by_dot[]
	INIT(= N_("E1356: \"super\" must be followed by a dot"));
EXTERN ಅಕ್ಷರ e_using_super_not_ಒಳ_class_ವಿಧಾನ[]
	INIT(= N_("E1357: Using \"super\" not in a class method"));
EXTERN ಅಕ್ಷರ e_using_super_not_ಒಳ_ಮಗು_class[]
	INIT(= N_("E1358: Using \"super\" not in a child class"));
EXTERN ಅಕ್ಷರ e_cannot_define_new_ವಿಧಾನ_ಒಳ_abstract_class[]
	INIT(= N_("E1359: Cannot define a \"new\" method in an abstract class"));
EXTERN ಅಕ್ಷರ e_using_null_object[]
	INIT(= N_("E1360: Using a null object"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_use_color_none_did_you_ಸರಾಸರಿ_none[]
	INIT(= N_("E1361: Cannot use color \"none\", did you mean \"NONE\"?"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_use_non_null_object[]
	INIT(= N_("E1362: Cannot use a non-null object"));
EXTERN ಅಕ್ಷರ e_incomplete_type[]
	INIT(= N_("E1363: Incomplete type"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_warning_ಸೂಚಕ_ಖಂಡ_corrupted[]
	INIT(= N_("E1364: Warning: Pointer block corrupted"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_use_a_return_type_with_new_ವಿಧಾನ[]
	INIT(= N_("E1365: Cannot use a return type with the \"new\" method"));
EXTERN ಅಕ್ಷರ e_cannot_access_protected_ವಿಧಾನ_str[]
	INIT(= N_("E1366: Cannot access protected method: %s"));
EXTERN ಅಕ್ಷರ e_variable_str_ದ_interface_str_has_different_access[]
	INIT(= N_("E1367: Access level of variable \"%s\" of interface \"%s\" is different"));
EXTERN ಅಕ್ಷರ e_static_must_be_followed_by_var_def_final_or_const[]
	INIT(= N_("E1368: Static must be followed by \"var\" or \"def\" or \"final\" or \"const\""));
EXTERN ಅಕ್ಷರ e_duplicate_variable_str[]
	INIT(= N_("E1369: Duplicate variable: %s"));
EXTERN ಅಕ್ಷರ e_cannot_define_new_ವಿಧಾನ_as_static[]
	INIT(= N_("E1370: Cannot define a \"new\" method as static"));
EXTERN ಅಕ್ಷರ e_abstract_must_be_followed_by_def[]
	INIT(= N_("E1371: Abstract must be followed by \"def\""));
EXTERN ಅಕ್ಷರ e_abstract_ವಿಧಾನ_ಒಳ_concrete_class[]
	INIT(= N_("E1372: Abstract method \"%s\" cannot be defined in a concrete class"));
EXTERN ಅಕ್ಷರ e_abstract_ವಿಧಾನ_str_not_implemented[]
	INIT(= N_("E1373: Abstract method \"%s\" is not implemented"));
EXTERN ಅಕ್ಷರ e_class_variable_str_accessible_only_inside_class_str[]
	INIT(= N_("E1374: Class variable \"%s\" accessible only inside class \"%s\""));
EXTERN ಅಕ್ಷರ e_class_variable_str_accessible_only_using_class_str[]
	INIT(= N_("E1375: Class variable \"%s\" accessible only using class \"%s\""));
EXTERN ಅಕ್ಷರ e_object_variable_str_accessible_only_using_object_str[]
	INIT(= N_("E1376: Object variable \"%s\" accessible only using class \"%s\" object"));
EXTERN ಅಕ್ಷರ e_ವಿಧಾನ_str_ದ_class_str_has_different_access[]
	INIT(= N_("E1377: Access level of method \"%s\" is different in class \"%s\""));
EXTERN ಅಕ್ಷರ e_static_member_not_supported_ಒಳ_interface[]
	INIT(= N_("E1378: Static member not supported in an interface"));
EXTERN ಅಕ್ಷರ e_protected_variable_not_supported_ಒಳ_interface[]
	INIT(= N_("E1379: Protected variable not supported in an interface"));
EXTERN ಅಕ್ಷರ e_protected_ವಿಧಾನ_not_supported_ಒಳ_interface[]
	INIT(= N_("E1380: Protected method not supported in an interface"));
EXTERN ಅಕ್ಷರ e_interface_cannot_use_implements[]
	INIT(= N_("E1381: Interface cannot use \"implements\""));
EXTERN ಅಕ್ಷರ e_variable_str_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1382: Variable \"%s\": type mismatch, expected %s but got %s"));
EXTERN ಅಕ್ಷರ e_ವಿಧಾನ_str_type_mismatch_expected_str_but_got_str[]
	INIT(= N_("E1383: Method \"%s\": type mismatch, expected %s but got %s"));
EXTERN ಅಕ್ಷರ e_class_ವಿಧಾನ_str_accessible_only_inside_class_str[]
	INIT(= N_("E1384: Class method \"%s\" accessible only inside class \"%s\""));
EXTERN ಅಕ್ಷರ e_class_ವಿಧಾನ_str_accessible_only_using_class_str[]
	INIT(= N_("E1385: Class method \"%s\" accessible only using class \"%s\""));
EXTERN ಅಕ್ಷರ e_object_ವಿಧಾನ_str_accessible_only_using_object_str[]
	INIT(= N_("E1386: Object method \"%s\" accessible only using class \"%s\" object"));
EXTERN ಅಕ್ಷರ e_ಬಹಿರಂಗ_variable_not_supported_ಒಳ_interface[]
	INIT(= N_("E1387: public variable not supported in an interface"));
EXTERN ಅಕ್ಷರ e_ಬಹಿರಂಗ_keyword_not_supported_for_ವಿಧಾನ[]
	INIT(= N_("E1388: public keyword not supported for a method"));
EXTERN ಅಕ್ಷರ e_missing_name_after_implements[]
	INIT(= N_("E1389: Missing name after implements"));
EXTERN ಅಕ್ಷರ e_cannot_use_an_object_variable_except_with_the_new_ವಿಧಾನ_str[]
	INIT(= N_("E1390: Cannot use an object variable \"this.%s\" except with the \"new\" method"));
EXTERN ಅಕ್ಷರ e_cannot_ಬೀಗ_object_variable_str[]
	INIT(= N_("E1391: Cannot (un)lock variable \"%s\" in class \"%s\""));
EXTERN ಅಕ್ಷರ e_cannot_ಬೀಗ_class_variable_str[]
	INIT(= N_("E1392: Cannot (un)lock class variable \"%s\" in class \"%s\""));
EXTERN ಅಕ್ಷರ e_type_can_only_be_defined_ಒಳ_vim9_script[]
	INIT(= N_("E1393: Type can only be defined in Vim9 script"));
EXTERN ಅಕ್ಷರ e_type_name_must_ಆರಂಭ_with_uppercase_letter_str[]
	INIT(= N_("E1394: Type name must start with an uppercase letter: %s"));
EXTERN ಅಕ್ಷರ e_using_null_class[]
	INIT(= N_("E1395: Using a null class"));
EXTERN ಅಕ್ಷರ e_typealias_already_exists_for_str[]
	INIT(= N_("E1396: Type alias \"%s\" already exists"));
EXTERN ಅಕ್ಷರ e_missing_typealias_name[]
	INIT(= N_("E1397: Missing type alias name"));
EXTERN ಅಕ್ಷರ e_missing_typealias_type[]
	INIT(= N_("E1398: Missing type alias type"));
EXTERN ಅಕ್ಷರ e_type_can_only_be_ಬಳಸಿದ_ಒಳ_script[]
	INIT(= N_("E1399: Type can only be used in a script"));
// E1400 unused
// E1401 unused
// E1402 unused
EXTERN ಅಕ್ಷರ e_using_typealias_as_ಮೌಲ್ಯ_str[]
	INIT(= N_("E1403: Type alias \"%s\" cannot be used as a value"));
EXTERN ಅಕ್ಷರ e_abstract_cannot_be_ಬಳಸಿದ_ಒಳ_interface[]
	INIT(= N_("E1404: Abstract cannot be used in an interface"));
EXTERN ಅಕ್ಷರ e_using_class_as_ಮೌಲ್ಯ_str[]
	INIT(= N_("E1405: Class \"%s\" cannot be used as a value"));
EXTERN ಅಕ್ಷರ e_ಬಹಿರಂಗ_and_protected_member_have_same_name_str_str[]
	INIT(= N_("E1406: Public and protected member have the same name: %s and _%s"));
EXTERN ಅಕ್ಷರ e_using_typealias_as_var_val[]
	INIT(= N_("E1407: Cannot use a Typealias as a variable or value"));
EXTERN ಅಕ್ಷರ e_final_variable_not_supported_ಒಳ_interface[]
	INIT(= N_("E1408: Final variable not supported in an interface"));
EXTERN ಅಕ್ಷರ e_cannot_change_readonly_variable_str_ಒಳ_class_str[]
	INIT(= N_("E1409: Cannot change read-only variable \"%s\" in class \"%s\""));
EXTERN ಅಕ್ಷರ e_const_variable_not_supported_ಒಳ_interface[]
	INIT(= N_("E1410: Const variable not supported in an interface"));
EXTERN ಅಕ್ಷರ e_missing_dot_after_object_str[]
	INIT(= N_("E1411: Missing dot after object \"%s\""));
EXTERN ಅಕ್ಷರ e_builtin_object_ವಿಧಾನ_str_not_supported[]
	INIT(= N_("E1412: Builtin object method \"%s\" not supported"));
EXTERN ಅಕ್ಷರ e_builtin_class_ವಿಧಾನ_not_supported[]
	INIT(= N_("E1413: Builtin class method not supported"));
EXTERN ಅಕ್ಷರ e_enum_can_only_be_defined_ಒಳ_vim9_script[]
       INIT(= N_("E1414: Enum can only be defined in Vim9 script"));
EXTERN ಅಕ್ಷರ e_enum_name_must_ಆರಂಭ_with_uppercase_letter_str[]
	INIT(= N_("E1415: Enum name must start with an uppercase letter: %s"));
EXTERN ಅಕ್ಷರ e_enum_cannot_extend_class[]
	INIT(= N_("E1416: Enum cannot extend a class or enum"));
EXTERN ಅಕ್ಷರ e_abstract_cannot_be_ಬಳಸಿದ_ಒಳ_enum[]
	INIT(= N_("E1417: Abstract cannot be used in an Enum"));
EXTERN ಅಕ್ಷರ e_invalid_enum_ಮೌಲ್ಯ_declaration_str[]
	INIT(= N_("E1418: Invalid enum value declaration: %s"));
EXTERN ಅಕ್ಷರ e_not_valid_command_ಒಳ_enum_str[]
	INIT(= N_("E1419: Not a valid command in an Enum: %s"));
EXTERN ಅಕ್ಷರ e_missing_endenum[]
	INIT(= N_("E1420: Missing :endenum"));
EXTERN ಅಕ್ಷರ e_using_enum_as_ಮೌಲ್ಯ_str[]
	INIT(= N_("E1421: Enum \"%s\" cannot be used as a value"));
EXTERN ಅಕ್ಷರ e_enum_ಮೌಲ್ಯ_str_not_ಸಿಕ್ಕಿತು_ಒಳ_enum_str[]
	INIT(= N_("E1422: Enum value \"%s\" not found in enum \"%s\""));
EXTERN ಅಕ್ಷರ e_enumvalue_str_cannot_be_modified[]
	INIT(= N_("E1423: Enum value \"%s.%s\" cannot be modified"));
EXTERN ಅಕ್ಷರ e_using_enum_str_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E1424: Using an Enum \"%s\" as a Number"));
EXTERN ಅಕ್ಷರ e_using_enum_str_as_string[]
	INIT(= N_("E1425: Using an Enum \"%s\" as a String"));
EXTERN ಅಕ್ಷರ e_enum_str_ordinal_cannot_be_modified[]
	INIT(= N_("E1426: Enum \"%s\" ordinal value cannot be modified"));
EXTERN ಅಕ್ಷರ e_enum_str_name_cannot_be_modified[]
	INIT(= N_("E1427: Enum \"%s\" name cannot be modified"));
EXTERN ಅಕ್ಷರ e_duplicate_enum_str[]
	INIT(= N_("E1428: Duplicate enum value: %s"));
EXTERN ಅಕ್ಷರ e_class_can_only_be_ಬಳಸಿದ_ಒಳ_script[]
	INIT(= N_("E1429: Class can only be used in a script"));
EXTERN ಅಕ್ಷರ e_uninitialized_object_var_reference[]
	INIT(= N_("E1430: Uninitialized object variable '%s' referenced"));
EXTERN ಅಕ್ಷರ e_abstract_ವಿಧಾನ_str_ನೇರ[]
	INIT(= N_("E1431: Abstract method \"%s\" in class \"%s\" cannot be accessed directly"));
EXTERN ಅಕ್ಷರ e_generic_ವಿಧಾನ_str_override_with_concrete_ವಿಧಾನ_ಒಳ_class_str[]
	INIT(= N_("E1432: Overriding generic method \"%s\" in class \"%s\" with a concrete method"));
EXTERN ಅಕ್ಷರ e_concrete_ವಿಧಾನ_str_override_with_generic_ವಿಧಾನ_ಒಳ_class_str[]
	INIT(= N_("E1433: Overriding concrete method \"%s\" in class \"%s\" with a generic method"));
EXTERN ಅಕ್ಷರ e_generic_ವಿಧಾನ_str_type_arguments_mismatch_ಒಳ_class_str[]
	INIT(= N_("E1434: Mismatched number of type variables for generic method  \"%s\" in class \"%s\""));
EXTERN ಅಕ್ಷರ e_enum_can_only_be_ಬಳಸಿದ_ಒಳ_script[]
	INIT(= N_("E1435: Enum can only be used in a script"));
EXTERN ಅಕ್ಷರ e_interface_can_only_be_ಬಳಸಿದ_ಒಳ_script[]
	INIT(= N_("E1436: Interface can only be used in a script"));
EXTERN ಅಕ್ಷರ e_can_only_compare_object_with_object[]
	INIT(= N_("E1437: Can only compare Object with Object"));
EXTERN ಅಕ್ಷರ e_type_var_ಎಣಿಕೆ_ದ_ವಿಧಾನ_str_differs_from_interface_str[]
	INIT(= N_("E1438: Number of type variables of method \"%s\" differs from interface \"%s\""));
EXTERN ಅಕ್ಷರ e_type_nested_too_deep[]
	INIT(= N_("E1439: Type nested too deep"));
#ಆದರೆ_ಅಂತ್ಯ
// E1440 - E1499 unused (reserved for Vim9 class support)
EXTERN ಅಕ್ಷರ e_cannot_mix_positional_and_non_positional_str[]
	INIT(= N_("E1500: Cannot mix positional and non-positional arguments: %s"));
EXTERN ಅಕ್ಷರ e_fmt_arg_nr_ಬಳಸದ_str[]
	INIT(= N_("E1501: format argument %d unused in $-style format: %s"));
EXTERN ಅಕ್ಷರ e_positional_ಅಂಕೆ_ಕ್ಷೇತ್ರ_spec_reused_str_str[]
	INIT(= N_("E1502: Positional argument %d used as field width reused as different type: %s/%s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_positional_nr_ಹೊರಹೋಗು_ದ_bounds_str[]
	INIT(= N_("E1503: Positional argument %d out of bounds: %s"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_positional_arg_ಅಂಕೆ_type_inconsistent_str_str[]
	INIT(= N_("E1504: Positional argument %d type used inconsistently: %s/%s"));
EXTERN ಅಕ್ಷರ e_invalid_format_specifier_str[]
	INIT(= N_("E1505: Invalid format specifier: %s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XATTR
EXTERN ಅಕ್ಷರ e_xattr_erange[]
	INIT(= N_("E1506: Buffer too small to copy xattr value or key"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_aptypes_is_null_nr_str[]
	INIT(= "E1507: Internal error: ap_types or ap_types[idx] is NULL: %d: %s");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XATTR
EXTERN ಅಕ್ಷರ e_xattr_e2big[]
	INIT(= N_("E1508: Size of the extended attribute value is larger than the maximum size allowed"));
EXTERN ಅಕ್ಷರ e_xattr_other[]
	INIT(= N_("E1509: Error occurred when reading or writing extended attribute"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_val_too_large[]
	INIT(= N_("E1510: Value too large: %s"));
EXTERN ಅಕ್ಷರ e_wrong_ಸಂಖ್ಯೆ_ದ_characters_for_ಕ್ಷೇತ್ರ_str[]
	INIT(= N_("E1511: Wrong number of characters for field \"%s\""));
EXTERN ಅಕ್ಷರ e_wrong_character_ಅಗಲ_for_ಕ್ಷೇತ್ರ_str[]
	INIT(= N_("E1512: Wrong character width for field \"%s\""));
EXTERN ಅಕ್ಷರ e_winfixbuf_cannot_go_ಗಮ್ಯ_ಬಫರ್[]
	INIT(= N_("E1513: Cannot switch buffer. 'winfixbuf' is enabled"));
EXTERN ಅಕ್ಷರ e_invalid_return_type_from_findfunc[]
	INIT(= N_("E1514: 'findfunc' did not return a List type"));
EXTERN ಅಕ್ಷರ e_str_encoding_from_failed[]
	INIT(= N_("E1515: Unable to convert from '%s' encoding"));
EXTERN ಅಕ್ಷರ e_str_encoding_ಗಮ್ಯ_failed[]
	INIT(= N_("E1516: Unable to convert to '%s' encoding"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_can_only_compare_tuple_with_tuple[]
	INIT(= N_("E1517: Can only compare Tuple with Tuple"));
EXTERN ಅಕ್ಷರ e_invalid_operation_for_tuple[]
	INIT(= N_("E1518: Invalid operation for Tuple"));
EXTERN ಅಕ್ಷರ e_tuple_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr[]
	INIT(= N_("E1519: Tuple index out of range: %ld"));
EXTERN ಅಕ್ಷರ e_using_tuple_as_ಸಂಖ್ಯೆ[]
	INIT(= N_("E1520: Using a Tuple as a Number"));
EXTERN ಅಕ್ಷರ e_using_tuple_as_float[]
	INIT(= N_("E1521: Using a Tuple as a Float"));
EXTERN ಅಕ್ಷರ e_using_tuple_as_string[]
	INIT(= N_("E1522: Using a Tuple as a String"));
EXTERN ಅಕ್ಷರ e_string_list_tuple_or_blob_required[]
	INIT(= N_("E1523: String, List, Tuple or Blob required"));
EXTERN ಅಕ್ಷರ e_cannot_use_tuple_with_function_str[]
	INIT(= N_("E1524: Cannot use a tuple with function %s"));
EXTERN ಅಕ್ಷರ e_ವಾದ_ದ_str_must_be_list_tuple_string_dictionary_or_blob[]
	INIT(= N_("E1525: Argument of %s must be a List, Tuple, String, Dictionary or Blob"));
EXTERN ಅಕ್ಷರ e_missing_ಕೊನೆ_ದ_tuple_rsp_str[]
	INIT(= N_("E1526: Missing end of Tuple ')': %s"));
EXTERN ಅಕ್ಷರ e_missing_comma_ಒಳ_tuple_str[]
	INIT(= N_("E1527: Missing comma in Tuple: %s"));
EXTERN ಅಕ್ಷರ e_list_or_tuple_or_blob_required_for_ವಾದ_nr[]
	INIT(= N_("E1528: List or Tuple or Blob required for argument %d"));
EXTERN ಅಕ್ಷರ e_list_or_tuple_required_for_ವಾದ_nr[]
	INIT(= N_("E1529: List or Tuple required for argument %d"));
EXTERN ಅಕ್ಷರ e_list_or_tuple_or_dict_required_for_ವಾದ_nr[]
	INIT(= N_("E1530: List or Tuple or Dictionary required for argument %d"));
EXTERN ಅಕ್ಷರ e_ವಾದ_ದ_str_must_be_list_tuple_dictionary_or_blob[]
	INIT(= N_("E1531: Argument of %s must be a List, Tuple, Dictionary or Blob"));
EXTERN ಅಕ್ಷರ e_tuple_is_immutable[]
	INIT(= N_("E1532: Cannot modify a tuple"));
EXTERN ಅಕ್ಷರ e_cannot_ತುಂಡು_tuple[]
	INIT(= N_("E1533: Cannot slice a tuple"));
EXTERN ಅಕ್ಷರ e_tuple_required_for_ವಾದ_nr[]
	INIT(= N_("E1534: Tuple required for argument %d"));
EXTERN ಅಕ್ಷರ e_list_or_tuple_required[]
	INIT(= N_("E1535: List or Tuple required"));
EXTERN ಅಕ್ಷರ e_tuple_required[]
	INIT(= N_("E1536: Tuple required"));
EXTERN ಅಕ್ಷರ e_less_targets_than_tuple_ವಸ್ತುಗಳು[]
	INIT(= N_("E1537: Less targets than Tuple items"));
EXTERN ಅಕ್ಷರ e_ಇನ್ನಷ್ಟು_targets_than_tuple_ವಸ್ತುಗಳು[]
	INIT(= N_("E1538: More targets than Tuple items"));
EXTERN ಅಕ್ಷರ e_variadic_tuple_must_ಕೊನೆ_with_list_type_str[]
	INIT(= N_("E1539: Variadic tuple must end with a list type: %s"));
EXTERN ಅಕ್ಷರ e_cannot_use_variadic_tuple_ಒಳ_concatenation[]
	INIT(= N_("E1540: Cannot use a variadic tuple in concatenation"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_unicode_val_too_large[]
	INIT(= N_("E1541: Value too large, max Unicode codepoint is U+10FFFF"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
EXTERN ಅಕ್ಷರ e_cannot_have_negative_or_zero_ಸಂಖ್ಯೆ_ದ_quickfix[]
	INIT(= N_("E1542: Cannot have a negative or zero number of quickfix/location lists"));
EXTERN ಅಕ್ಷರ e_cannot_have_ಇನ್ನಷ್ಟು_than_hundred_quickfix[]
	INIT(= N_("E1543: Cannot have more than a hundred quickfix/location lists"));
EXTERN ಅಕ್ಷರ e_failed_resizing_quickfix_ರಾಶಿ[]
	INIT(= N_("E1544: Failed resizing the quickfix/location list stack"));
EXTERN ಅಕ್ಷರ e_no_quickfix_ರಾಶಿ[]
	INIT(= N_("E1545: Quickfix list stack unavailable"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_cannot_switch_ಗಮ್ಯ_a_closing_ಬಫರ್[]
	INIT(= N_("E1546: Cannot switch to a closing buffer"));
EXTERN ಅಕ್ಷರ e_cannot_not_support_redrawtabpanel[]
	INIT(= N_("E1547: This version of Vim does not support :redrawtabpanel"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND
EXTERN ಅಕ್ಷರ e_wayland_connection_unavailable[]
	INIT(= N_("E1548: Wayland connection is unavailable"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_DIFF)
EXTERN ಅಕ್ಷರ e_cannot_have_ಇನ್ನಷ್ಟು_than_nr_diff_anchors[]
	INIT(= N_("E1549: Cannot have more than %d diff anchors"));
EXTERN ಅಕ್ಷರ e_failed_ಗಮ್ಯ_ಕಂಡುಹಿಡಿ_all_diff_anchors[]
	INIT(= N_("E1550: Failed to find all diff anchors"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_cannot_open_a_popup_ಕಿಟಕಿ_ಗಮ್ಯ_a_closing_ಬಫರ್[]
	INIT(= N_("E1551: Cannot open a popup window to a closing buffer"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_type_var_name_must_ಆರಂಭ_with_uppercase_letter_str[]
	INIT(= N_("E1552: Type variable name must start with an uppercase letter: %s"));
EXTERN ಅಕ್ಷರ e_missing_comma_ಒಳ_generic_function_str[]
	INIT(= N_("E1553: Missing comma after type in generic function: %s"));
EXTERN ಅಕ್ಷರ e_missing_closing_angle_bracket_ಒಳ_generic_function_str[]
	INIT(= N_("E1554: Missing '>' in generic function: %s"));
EXTERN ಅಕ್ಷರ e_ಖಾಲಿಗಳು_type_list_for_generic_function_str[]
	INIT(= N_("E1555: Empty type list specified for generic function '%s'"));
EXTERN ಅಕ್ಷರ e_too_many_types_for_generic_function_str[]
	INIT(= N_("E1556: Too many types specified for generic function '%s'"));
EXTERN ಅಕ್ಷರ e_not_enough_types_for_generic_function_str[]
	INIT(= N_("E1557: Not enough types specified for generic function '%s'"));
EXTERN ಅಕ್ಷರ e_unknown_generic_function_str[]
	INIT(= N_("E1558: Unknown generic function: %s"));
EXTERN ಅಕ್ಷರ e_generic_func_missing_type_ವಾದಗಳು_str[]
	INIT(= N_("E1559: Type arguments missing for generic function '%s'"));
EXTERN ಅಕ್ಷರ e_not_a_generic_function_str[]
	INIT(= N_("E1560: Not a generic function: %s"));
EXTERN ಅಕ್ಷರ e_duplicate_type_var_name_str[]
	INIT(= N_("E1561: Duplicate type variable name: %s"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_DIFF)
EXTERN ಅಕ್ಷರ e_diff_anchors_with_hidden_windows[]
	INIT(= N_("E1562: Diff anchors cannot be used with hidden diff windows"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
EXTERN ಅಕ್ಷರ e_socket_name_no_slashes[]
	INIT(= N_("E1564: Socket name '%s' cannot have slashes in it without being a path"));
EXTERN ಅಕ್ಷರ e_socket_server_not_online[]
	INIT(= N_("E1565: Socket server is not online, call remote_startserver() first"));
EXTERN ಅಕ್ಷರ e_socket_server_failed_connecting[]
	INIT(= N_("E1566: Failed connecting to socket '%s'"));
EXTERN ಅಕ್ಷರ e_socket_server_ಆವೃತ್ತಿ_mismatch[]
	INIT(= N_("E1567: Socket server protocol version mismatch, check what Vim version you are using"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_osc_response_timed_ಹೊರಹೋಗು[]
	INIT(= N_("E1568: OSC command response timed out: %.*s"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_ಕೂಡಿಸು_listener_ಒಳ_listener_callback[]
	INIT(= N_("E1569: Cannot use listener_add in a listener callback"));
EXTERN ಅಕ್ಷರ e_cannot_ಕೂಡಿಸು_redraw_listener_ಒಳ_listener_callback[]
	INIT(= N_("E1570: Cannot use redraw_listener_add in a redraw listener callback"));
EXTERN ಅಕ್ಷರ e_no_redraw_listener_callbacks_defined[]
	INIT(= N_("E1571: Must specify at least one callback for redraw_listener_add"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_leadtab_requires_tab[]
	INIT(= N_("E1572: 'listchars' field \"leadtab\" requires \"tab\" to be specified"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
EXTERN ಅಕ್ಷರ e_cannot_listen_on_ಪೋರ್ಟ್[]
	INIT(= N_("E1573: Cannot listen on port"));
EXTERN ಅಕ್ಷರ e_gethostbyname_ಒಳ_channel_listen[]
	INIT(= N_("E1574: gethostbyname(): cannot resolve hostname in channel_listen()"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_create_pipes[]
	INIT(= N_("E1575: Cannot create pipes"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_ಟ್ಯಾಗ್_file_ನಮೂದು_must_not_be_url[]
	INIT(= N_("E1576: Tag file entry must not be a URL"));
EXTERN ಅಕ್ಷರ e_invalid_format_string_single_percent_s[]
	INIT(= N_("E1577: Invalid format string, only one \"%s\" is allowed"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
EXTERN ಅಕ್ಷರ e_too_many_postponed_prefixes_spell[]
	INIT(= N_("E1578: Too many postponed prefixes and/or compound flags"));
#ಆದರೆ_ಅಂತ್ಯ
EXTERN ಅಕ್ಷರ e_completeopt_escape_cannot_be_ಬಳಸಿದ_with_nargs_underscore[]
	INIT(= N_("E1579: -completeopt=escape cannot be used with -nargs=_"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
EXTERN ಅಕ್ಷರ e_too_many_ಪಠ್ಯ_properties_on_a_single_ಸಾಲು[]
	INIT(= N_("E1580: Too many text properties on a single line"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
EXTERN ಅಕ್ಷರ e_cannot_extend_null_blob[]
	INIT(= N_("E1581: Cannot extend a null blob"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
EXTERN ಅಕ್ಷರ e_not_started_with_stdio_channel[]
	INIT(= N_("E1582: Not started with --stdio-channel"));
EXTERN ಅಕ್ಷರ e_cannot_open_stdio_channel[]
	INIT(= N_("E1583: Cannot open stdio channel"));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE
EXTERN ಅಕ್ಷರ e_invalid_image_dimensions[]
	INIT(= N_("E1584: Image dimensions are invalid"));
EXTERN ಅಕ್ಷರ e_no_image_backend_ಲಭ್ಯ[]
	INIT(= N_("E1585: No image backend available"));
EXTERN ಅಕ್ಷರ e_changing_image_backend_failed[]
	INIT(= N_("E1586: Failed changing image backends"));
EXTERN ಅಕ್ಷರ e_image_id_nr_does_not_exist[]
	INIT(= N_("E1587: Image ID %d does not exist"));
#ಆದರೆ_ಅಂತ್ಯ
