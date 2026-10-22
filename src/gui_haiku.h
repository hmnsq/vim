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

ସଂରଚନା BMenu;
ସଂରଚନା BMenuItem;
ସଂରଚନା BPictureButton;

// Our own Vim-related classes

ସଂରଚନା VimApp;
ସଂରଚନା VimFormView;
ସଂରଚନା VimTextAreaView;
ସଂରଚନା VimWindow;
ସଂରଚନା VimScrollBar;

// Locking functions

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim_lock_screen();
ବାହ୍ୟ ଶୂନ୍ୟ vim_unlock_screen();

#ifndef __cplusplus

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା BMenu BMenu;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା BMenuItem BMenuItem;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା BPictureButton BPictureButton;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା VimWindow VimWindow;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା VimFormView VimFormView;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା VimTextAreaView VimTextAreaView;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା VimApp VimApp;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା VimScrollBar VimScrollBar;

#endif
