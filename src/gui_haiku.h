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

সংরচনা BMenu;
সংরচনা BMenuItem;
সংরচনা BPictureButton;

// Our own Vim-related classes

সংরচনা VimApp;
সংরচনা VimFormView;
সংরচনা VimTextAreaView;
সংরচনা VimWindow;
সংরচনা VimScrollBar;

// Locking functions

বাহ্য পূর্ণাঙ্ক vim_lock_screen();
বাহ্য শূন্য vim_unlock_screen();

#ifndef __cplusplus

প্রকারনির্দেশ সংরচনা BMenu BMenu;
প্রকারনির্দেশ সংরচনা BMenuItem BMenuItem;
প্রকারনির্দেশ সংরচনা BPictureButton BPictureButton;
প্রকারনির্দেশ সংরচনা VimWindow VimWindow;
প্রকারনির্দেশ সংরচনা VimFormView VimFormView;
প্রকারনির্দেশ সংরচনা VimTextAreaView VimTextAreaView;
প্রকারনির্দেশ সংরচনা VimApp VimApp;
প্রকারনির্দেশ সংরচনা VimScrollBar VimScrollBar;

#endif
