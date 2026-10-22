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

संरचना BMenu;
संरचना BMenuItem;
संरचना BPictureButton;

// Our own Vim-related classes

संरचना VimApp;
संरचना VimFormView;
संरचना VimTextAreaView;
संरचना VimWindow;
संरचना VimScrollBar;

// Locking functions

बाह्य पूर्णांक vim_lock_screen();
बाह्य शून्य vim_unlock_screen();

#ifndef __cplusplus

प्रकारव्याख्या संरचना BMenu BMenu;
प्रकारव्याख्या संरचना BMenuItem BMenuItem;
प्रकारव्याख्या संरचना BPictureButton BPictureButton;
प्रकारव्याख्या संरचना VimWindow VimWindow;
प्रकारव्याख्या संरचना VimFormView VimFormView;
प्रकारव्याख्या संरचना VimTextAreaView VimTextAreaView;
प्रकारव्याख्या संरचना VimApp VimApp;
प्रकारव्याख्या संरचना VimScrollBar VimScrollBar;

#endif
