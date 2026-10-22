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

கட்டமைப்பு BMenu;
கட்டமைப்பு BMenuItem;
கட்டமைப்பு BPictureButton;

// Our own Vim-related classes

கட்டமைப்பு VimApp;
கட்டமைப்பு VimFormView;
கட்டமைப்பு VimTextAreaView;
கட்டமைப்பு VimWindow;
கட்டமைப்பு VimScrollBar;

// Locking functions

வெளி முழு vim_lock_screen();
வெளி வெறுமை vim_unlock_screen();

#ifndef __cplusplus

வகைவரை கட்டமைப்பு BMenu BMenu;
வகைவரை கட்டமைப்பு BMenuItem BMenuItem;
வகைவரை கட்டமைப்பு BPictureButton BPictureButton;
வகைவரை கட்டமைப்பு VimWindow VimWindow;
வகைவரை கட்டமைப்பு VimFormView VimFormView;
வகைவரை கட்டமைப்பு VimTextAreaView VimTextAreaView;
வகைவரை கட்டமைப்பு VimApp VimApp;
வகைவரை கட்டமைப்பு VimScrollBar VimScrollBar;

#endif
