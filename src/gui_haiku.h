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

ರಚನೆ BMenu;
ರಚನೆ BMenu_ಅಂಶ;
ರಚನೆ BPictureButton;

// Our own Vim-related classes

ರಚನೆ VimApp;
ರಚನೆ VimFormView;
ರಚನೆ Vim_ಪಠ್ಯ_Area_View;
ರಚನೆ Vim_ಕಿಟಕಿ;
ರಚನೆ VimScrollBar;

// Locking functions

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ vim_ಬೀಗ_screen();
ಬಾಹ್ಯ ಶೂನ್ಯ vim_unlock_screen();

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __cplusplus

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ BMenu BMenu;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ BMenu_ಅಂಶ BMenu_ಅಂಶ;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ BPictureButton BPictureButton;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Vim_ಕಿಟಕಿ Vim_ಕಿಟಕಿ;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ VimFormView VimFormView;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Vim_ಪಠ್ಯ_Area_View Vim_ಪಠ್ಯ_Area_View;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ VimApp VimApp;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ VimScrollBar VimScrollBar;

#ಆದರೆ_ಅಂತ್ಯ
