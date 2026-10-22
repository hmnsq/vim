/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI support by Olaf "Rhialto" Seibert
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 * Haiku GUI.
 *
 * Based on "GUI support for the Buzzword Enhanced Operating System for PPC."
 *
 */

/*
 * This file must be acceptable both as C and C++.
 * The BeOS API is defined in terms of C++, but some classes
 * should be somewhat known in the common C code.
 */

// System classes

સંરચના BMenu;
સંરચના BMenuItem;
સંરચના BPictureButton;

// Our own Vim-related classes

સંરચના VimApp;
સંરચના VimFormView;
સંરચના VimTextAreaView;
સંરચના VimWindow;
સંરચના VimScrollBar;

// Locking functions

બાહ્ય પૂર્ણાંક vim_lock_screen();
બાહ્ય શૂન્ય vim_unlock_screen();

#ifndef __cplusplus

પ્રકારવ્યાખ્યા સંરચના BMenu BMenu;
પ્રકારવ્યાખ્યા સંરચના BMenuItem BMenuItem;
પ્રકારવ્યાખ્યા સંરચના BPictureButton BPictureButton;
પ્રકારવ્યાખ્યા સંરચના VimWindow VimWindow;
પ્રકારવ્યાખ્યા સંરચના VimFormView VimFormView;
પ્રકારવ્યાખ્યા સંરચના VimTextAreaView VimTextAreaView;
પ્રકારવ્યાખ્યા સંરચના VimApp VimApp;
પ્રકારવ્યાખ્યા સંરચના VimScrollBar VimScrollBar;

#endif
