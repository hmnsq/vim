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

నిర్మాణం BMenu;
నిర్మాణం BMenu_అంశం;
నిర్మాణం BPictureButton;

// Our own Vim-related classes

నిర్మాణం VimApp;
నిర్మాణం VimFormView;
నిర్మాణం Vim_పాఠ్యం_Area_View;
నిర్మాణం Vim_కిటికీ;
నిర్మాణం VimScrollBar;

// Locking functions

బాహ్య పూర్ణాంకం vim_తాళం_screen();
బాహ్య శూన్య vim_unlock_screen();

#నిర్వచితం_కాకపోతే __cplusplus

రకనిర్వచనం నిర్మాణం BMenu BMenu;
రకనిర్వచనం నిర్మాణం BMenu_అంశం BMenu_అంశం;
రకనిర్వచనం నిర్మాణం BPictureButton BPictureButton;
రకనిర్వచనం నిర్మాణం Vim_కిటికీ Vim_కిటికీ;
రకనిర్వచనం నిర్మాణం VimFormView VimFormView;
రకనిర్వచనం నిర్మాణం Vim_పాఠ్యం_Area_View Vim_పాఠ్యం_Area_View;
రకనిర్వచనం నిర్మాణం VimApp VimApp;
రకనిర్వచనం నిర్మాణం VimScrollBar VimScrollBar;

#అయితే_ముగింపు
