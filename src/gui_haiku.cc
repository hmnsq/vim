/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *    BeBox GUI support Copyright 1998 by Olaf Seibert.
 *		    All Rights Reserved.
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 * Based on "GUI support for the Buzzword Enhanced Operating System."
 *
 * Ported to R4 by Richard Offer <richard@whitequeen.com> Jul 99
 *
 * Haiku support by Siarzhuk Zharski <imker@gmx.li> Apr-Mai 2009
 *
 */

/*
 * Structure of the Haiku GUI code:
 *
 * There are 3 threads.
 * 1. The initial thread. In gui_mch_prepare() this gets to run the
 *    BApplication message loop. But before it starts doing that,
 *    it creates thread 2
 * 2. The main() thread. This thread is created in gui_mch_prepare()
 *    and its purpose in life is to call main(argc, argv) again.
 *    This thread is doing the bulk of the work.
 * 3. Sooner or later, a window is opened by the main() thread. This
 *    causes a second message loop to be created: the window thread.
 *
 * == alternatively ===
 *
 * #if RUN_BAPPLICATION_IN_NEW_THREAD...
 *
 * 1. The initial thread. In gui_mch_prepare() this gets to spawn
 *    thread 2. After doing that, it returns to main() to do the
 *    bulk of the work, being the main() thread.
 * 2. Runs the BApplication.
 * 3. The window thread, just like in the first case.
 *
 * This second alternative is cleaner from Vim's viewpoint. However,
 * the BeBook seems to assume everywhere that the BApplication *must*
 * run in the initial thread. So perhaps doing otherwise is very wrong.
 *
 * However, from a B_SINGLE_LAUNCH viewpoint, the first is better.
 * If Vim is marked "Single Launch" in its application resources,
 * and a file is dropped on the Vim icon, and another Vim is already
 * running, the file is passed on to the earlier Vim. This happens
 * in BApplication::Run(). So we want Vim to terminate if
 * BApplication::Run() terminates. (See the BeBook, on BApplication.
 * However, it seems that the second copy of Vim isn't even started
 * in this case... which is for the better since I wouldn't know how
 * to detect this case.)
 *
 * Communication between these threads occurs mostly by translating
 * BMessages that come in and posting an appropriate translation on
 * the VDCMP (Vim Direct Communication Message Port). Therefore the
 * actions required for keypresses and window resizes, etc, are mostly
 * performed in the main() thread.
 *
 * A notable exception to this is the Draw() event. The redrawing of
 * the window contents is performed asynchronously from the window
 * thread. To make this work correctly, a locking protocol is used when
 * any thread is accessing the essential variables that are used by
 * the window thread.
 *
 * This locking protocol consists of locking Vim's window. This is both
 * convenient and necessary.
 */

ബാഹ്യ "C" {

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <float.h>
#അന്തര്ഭാവ <syslog.h>

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#അന്തര്ഭാവ <Application.h>
#അന്തര്ഭാവ <Beep.h>
#അന്തര്ഭാവ <Bitmap.h>
#അന്തര്ഭാവ <Box.h>
#അന്തര്ഭാവ <Button.h>
#അന്തര്ഭാവ <Clipboard.h>
#അന്തര്ഭാവ <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#അന്തര്ഭാവ <File.h>
#അന്തര്ഭാവ <FilePanel.h>
#അന്തര്ഭാവ <FindDirectory.h>
//#include <Font.h>
#അന്തര്ഭാവ <IconUtils.h>
#അന്തര്ഭാവ <Input.h>
#അന്തര്ഭാവ <ListView.h>
#അന്തര്ഭാവ <MenuBar.h>
#അന്തര്ഭാവ <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#അന്തര്ഭാവ <Path.h>
#അന്തര്ഭാവ <PictureButton.h>
#അന്തര്ഭാവ <PopUpMenu.h>
//#include <Region.h>
#അന്തര്ഭാവ <Resources.h>
//#include <Roster.h>
#അന്തര്ഭാവ <Screen.h>
#അന്തര്ഭാവ <ScrollBar.h>
#അന്തര്ഭാവ <ScrollView.h>
#അന്തര്ഭാവ <String.h>
#അന്തര്ഭാവ <StringView.h>
//#include <SupportDefs.h>
#അന്തര്ഭാവ <TabView.h>
#അന്തര്ഭാവ <TextControl.h>
#അന്തര്ഭാവ <TextView.h>
#അന്തര്ഭാവ <TranslationUtils.h>
#അന്തര്ഭാവ <TranslatorFormats.h>
#അന്തര്ഭാവ <View.h>
#അന്തര്ഭാവ <Window.h>

വർഗ്ഗം VimApp;
വർഗ്ഗം VimFormView;
വർഗ്ഗം VimTextAreaView;
വർഗ്ഗം VimWindow;
വർഗ്ഗം VimToolbar;
വർഗ്ഗം Vim_Tab_വരി;

ബാഹ്യ താക്കോൽ_map *താക്കോൽ_Map;
ബാഹ്യ അക്ഷരം *താക്കോൽ_Map_Chars;

ബാഹ്യ പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv);

#ifndef B_MAX_PORT_എണ്ണം
#define B_MAX_PORT_എണ്ണം    255
#endif

// VimApp seems comparable to the X "vimShell"
വർഗ്ഗം VimApp: പൊതു BApplication
{
    തരനിർവചനം BApplication Inherited;
    പൊതു:
    VimApp(സ്ഥിരം അക്ഷരം *appsig);
    ~VimApp();

    // callbacks:
#എങ്കിൽ 0
    മായാ ശൂന്യം DispatchMessage(BMessage *m, BHandler *h)
    {
	m->PrintToStream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    മായാ ശൂന്യം ReadyToRun();
    മായാ ശൂന്യം ArgvReceived(int32 argc, അക്ഷരം **argv);
    മായാ ശൂന്യം RefsReceived(BMessage *m);
    മായാ തർക്കം QuitRequested();
    മായാ ശൂന്യം MessageReceived(BMessage *m);

    സ്ഥിതം ശൂന്യം SendRefs(BMessage *m, തർക്കം changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    സ്വകാര്യ:
};

വർഗ്ഗം VimWindow: പൊതു BWindow
{
    തരനിർവചനം BWindow Inherited;
    പൊതു:
    VimWindow();
    ~VimWindow();

    മായാ ശൂന്യം DispatchMessage(BMessage *m, BHandler *h);
    മായാ ശൂന്യം WindowActivated(തർക്കം active);
    മായാ തർക്കം QuitRequested();

    VimFormView	    *formView;

    സ്വകാര്യ:
    ശൂന്യം init();

    തർക്കം is_fullscreen = അസത്യം;
    BRect saved_frame;
    window_look saved_look;
};

വർഗ്ഗം VimFormView: പൊതു BView
{
    തരനിർവചനം BView Inherited;
    പൊതു:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    മായാ ശൂന്യം AllAttached();
    മായാ ശൂന്യം FrameResized(ദശാംശം new_width, ദശാംശം new_height);

#define MENUBAR_MARGIN	1
    ദശാംശം MenuHeight() സ്ഥിരം
    { തിരികെ menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() സ്ഥിരം
    { തിരികെ menuBar; }

    സ്വകാര്യ:
    ശൂന്യം init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    പൊതു:
    ദശാംശം ToolbarHeight() സ്ഥിരം;
    VimToolbar *ToolBar() സ്ഥിരം
    { തിരികെ toolBar; }
    സ്വകാര്യ:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    പൊതു:
    Vim_Tab_വരി *Tab_വരി() സ്ഥിരം	{ തിരികെ tab_വരി; }
    തർക്കം Is_Showing_Tab_വരി() സ്ഥിരം { തിരികെ showing_Tab_വരി; }
    ശൂന്യം Set_Showing_Tab_വരി(തർക്കം showing) { showing_Tab_വരി = showing;	}
    ദശാംശം TablineHeight() സ്ഥിരം;
    സ്വകാര്യ:
    Vim_Tab_വരി	*tab_വരി;
    പൂർണ്ണസംഖ്യ	showing_Tab_വരി;
#endif
};

വർഗ്ഗം VimTextAreaView: പൊതു BView
{
    തരനിർവചനം BView Inherited;
    പൊതു:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    മായാ ശൂന്യം Draw(BRect updateRect);
    മായാ ശൂന്യം താക്കോൽ_Down(സ്ഥിരം അക്ഷരം *bytes, int32 അക്കം_Bytes);
    മായാ ശൂന്യം MouseDown(BPoint point);
    മായാ ശൂന്യം MouseUp(BPoint point);
    മായാ ശൂന്യം MouseMoved(BPoint point, uint32 transit, സ്ഥിരം BMessage *message);
    മായാ ശൂന്യം MessageReceived(BMessage *m);

    // own functions:
    പൂർണ്ണസംഖ്യ mchInitFont(char_u *name);
    ശൂന്യം mchDrawString(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, char_u *s, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ flags);
    ശൂന്യം mchClearBlock(പൂർണ്ണസംഖ്യ row1, പൂർണ്ണസംഖ്യ col1, പൂർണ്ണസംഖ്യ row2, പൂർണ്ണസംഖ്യ col2);
    ശൂന്യം mchClearAll();
    ശൂന്യം mch_Delete_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ അക്കം_വരികൾ);
    ശൂന്യം mch_തിരുകുക_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ അക്കം_വരികൾ);

    സ്ഥിതം ശൂന്യം guiSendMouseEvent(പൂർണ്ണസംഖ്യ button, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ repeated_click, int_u modifiers);
    സ്ഥിതം ശൂന്യം guiMouseMoved(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y);
    സ്ഥിതം ശൂന്യം guiBlankMouse(തർക്കം should_hide);
    സ്ഥിതം int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouse_Drag_Event_എണ്ണം;

#ifdef FEAT_MBYTE_IME
    ശൂന്യം DrawIMString(ശൂന്യം);
#endif

    സ്വകാര്യ:
    ശൂന്യം init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    ഘടന {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	പൂർണ്ണസംഖ്യ row;
	പൂർണ്ണസംഖ്യ col;
	പൂർണ്ണസംഖ്യ എണ്ണം;
    } IMData;
#endif
};

വർഗ്ഗം VimScrollBar: പൊതു BScrollBar
{
    തരനിർവചനം BScrollBar Inherited;
    പൊതു:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    മായാ ശൂന്യം മൂല്യം_Changed(ദശാംശം new_മൂല്യം);
    മായാ ശൂന്യം MouseUp(BPoint where);
    ശൂന്യം Set_മൂല്യം(ദശാംശം newval);
    scrollbar_T *getGsb()
    { തിരികെ gsb; }

    int32	scroll_Event_എണ്ണം;

    സ്വകാര്യ:
    scrollbar_T *gsb;
    ദശാംശം   ignore_മൂല്യം;
};


#ifdef FEAT_TOOLBAR

വർഗ്ഗം VimToolbar : പൊതു BBox
{
    സ്ഥിതം BBitmap *normalButtonsBitmap;
    സ്ഥിതം BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(സ്ഥിരം അക്ഷരം* fileName);
    തർക്കം GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, തർക്കം pressed);
    തർക്കം ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    ശൂന്യം InvalidateLayout();

    പൊതു:
    VimToolbar(BRect frame, സ്ഥിരം അക്ഷരം * name);
    ~VimToolbar();

    തർക്കം PrepareButtonBitmaps();

    തർക്കം AddButton(int32 index, vimmenu_T *menu);
    തർക്കം RemoveButton(vimmenu_T *menu);
    തർക്കം GrayButton(vimmenu_T *menu, പൂർണ്ണസംഖ്യ grey);

    ദശാംശം ToolbarHeight() സ്ഥിരം;
    മായാ ശൂന്യം AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = ശൂന്യ;
BBitmap *VimToolbar::grayedButtonsBitmap  = ശൂന്യ;

സ്ഥിരം ദശാംശം ToolbarMargin = 3.;
സ്ഥിരം ദശാംശം ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

വർഗ്ഗം Vim_Tab_വരി : പൊതു BTabView
{
    പൊതു:
	വർഗ്ഗം VimTab : പൊതു BTab {
	    പൊതു:
		VimTab() : BTab(പുതിയ BView(BRect(), "-Empty-", 0, 0)) {}

	    മായാ ശൂന്യം Select(BView* owner);
	};

	Vim_Tab_വരി(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_ഇടത് | B_FOLLOW_മുകൾ | B_FOLLOW_വലത്, B_WILL_DRAW | B_FRAME_EVENTS) {}

    ദശാംശം TablineHeight() സ്ഥിരം;
    മായാ ശൂന്യം MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
വർഗ്ഗം VimFont: പൊതു BFont
{
    തരനിർവചനം BFont Inherited;
    പൊതു:
    VimFont();
    VimFont(സ്ഥിരം VimFont *rhs);
    VimFont(സ്ഥിരം BFont *rhs);
    VimFont(സ്ഥിരം VimFont &rhs);
    ~VimFont();

    VimFont *next;
    പൂർണ്ണസംഖ്യ refcount;
    char_u *name;

    സ്വകാര്യ:
    ശൂന്യം init();
};

#എങ്കിൽ defined(FEAT_GUI_DIALOG)

വർഗ്ഗം VimDialog : പൊതു BWindow
{
    തരനിർവചനം BWindow Inherited;

    BButton* _CreateButton(int32 which, സ്ഥിരം അക്ഷരം* label);

    പൊതു:

    വർഗ്ഗം View : പൊതു BView {
	തരനിർവചനം BView Inherited;

	പൊതു:
	View(BRect frame);
	~View();

	മായാ ശൂന്യം Draw(BRect updateRect);
	ശൂന്യം InitIcon(int32 type);

	സ്വകാര്യ:
	BBitmap*    fIconBitmap;
    };

    VimDialog(പൂർണ്ണസംഖ്യ type, സ്ഥിരം അക്ഷരം *title, സ്ഥിരം അക്ഷരം *message,
	    സ്ഥിരം അക്ഷരം *buttons, പൂർണ്ണസംഖ്യ dfltbutton, സ്ഥിരം അക്ഷരം *textfield,
	    പൂർണ്ണസംഖ്യ ex_cmd);
    ~VimDialog();

    പൂർണ്ണസംഖ്യ Go();

    മായാ ശൂന്യം MessageReceived(BMessage *സന്ദേശം);

    സ്വകാര്യ:
    sem_id	    fDialogSem;
    പൂർണ്ണസംഖ്യ		    f_Dialog_മൂല്യം;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   f_നിവേശം_Control;
    സ്ഥിരം അക്ഷരം*	    f_നിവേശം_മൂല്യം;
};

വർഗ്ഗം VimSelectFontDialog : പൊതു BWindow
{
    തരനിർവചനം BWindow Inherited;

    ശൂന്യം _CleanList(BListView* list);
    ശൂന്യം _UpdateFontStyles();
    ശൂന്യം Update_Size_നിവേശം_Preview();
    ശൂന്യം _UpdateFontPreview();
    തർക്കം _UpdateFromListItem(BListView* list, അക്ഷരം* text, പൂർണ്ണസംഖ്യ textSize);
    പൊതു:

    VimSelectFontDialog(font_family* family, font_style* style, ദശാംശം* size);
    ~VimSelectFontDialog();

    തർക്കം Go();

    മായാ ശൂന്യം MessageReceived(BMessage *സന്ദേശം);

    സ്വകാര്യ:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    തർക്കം	    f_Dialog_മൂല്യം;
    font_family*    fFamily;
    font_style*	    fStyle;
    ദശാംശം*	    fSize;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    ദശാംശം	    fFontSize;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_നിവേശം;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

ഘടന MainArgs {
    പൂർണ്ണസംഖ്യ	     argc;
    അക്ഷരം    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	താക്കോൽ_സന്ദേശം_BUFSIZ	7
#എങ്കിൽ താക്കോൽ_സന്ദേശം_BUFSIZ < MAX_താക്കോൽ_CODE_LEN
#error Increase താക്കോൽ_സന്ദേശം_BUFSIZ!
#endif

ഘടന Vim_താക്കോൽ_സന്ദേശം {
    char_u  length;
    char_u  chars[താക്കോൽ_സന്ദേശം_BUFSIZ];  // contains Vim encoding
    തർക്കം    csi_escape;
};

ഘടന Vim_Resize_സന്ദേശം {
    പൂർണ്ണസംഖ്യ	    width;
    പൂർണ്ണസംഖ്യ	    height;
};

ഘടന Vim_Scroll_Bar_സന്ദേശം {
    VimScrollBar *sb;
    long    മൂല്യം;
    പൂർണ്ണസംഖ്യ	    stillDragging;
};

ഘടന Vim_Menu_സന്ദേശം {
    vimmenu_T	*guiMenu;
};

ഘടന Vim_Mouse_സന്ദേശം {
    പൂർണ്ണസംഖ്യ	    button;
    പൂർണ്ണസംഖ്യ	    x;
    പൂർണ്ണസംഖ്യ	    y;
    പൂർണ്ണസംഖ്യ	    repeated_click;
    int_u   modifiers;
};

ഘടന Vim_Mouse_Moved_സന്ദേശം {
    പൂർണ്ണസംഖ്യ	    x;
    പൂർണ്ണസംഖ്യ	    y;
};

ഘടന Vim_Focus_സന്ദേശം {
    തർക്കം    active;
};

ഘടന Vim_Refs_സന്ദേശം {
    BMessage   *message;
    തർക്കം    changedir;
};

ഘടന Vim_Tabline_സന്ദേശം {
    പൂർണ്ണസംഖ്യ	    index;
};

ഘടന Vim_Tabline_Menu_സന്ദേശം {
    പൂർണ്ണസംഖ്യ	    index;
    പൂർണ്ണസംഖ്യ	    event;
};

ഘടന Vim_സന്ദേശം {
    എണ്ണൽ Vim_സന്ദേശം_Type {
	താക്കോൽ_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    സംഘം {
	ഘടന Vim_താക്കോൽ_സന്ദേശം    താക്കോൽ_2;
	ഘടന Vim_Resize_സന്ദേശം NewSize;
	ഘടന Vim_Scroll_Bar_സന്ദേശം	Scroll;
	ഘടന Vim_Menu_സന്ദേശം   Menu;
	ഘടന Vim_Mouse_സന്ദേശം  Mouse;
	ഘടന Vim_Mouse_Moved_സന്ദേശം	MouseMoved;
	ഘടന Vim_Focus_സന്ദേശം  Focus;
	ഘടന Vim_Refs_സന്ദേശം   Refs;
	ഘടന Vim_Tabline_സന്ദേശം	Tabline;
	ഘടന Vim_Tabline_Menu_സന്ദേശം    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

സ്ഥിതം ഘടന specialkey
{
    uint16  Be_താക്കോലുകൾ;
#define താക്കോൽ_3(a,b)    ((a)<<8|(b))
#define K(a)	    താക്കോൽ_3(0,a)		// for ASCII codes
#define F(b)	    താക്കോൽ_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_താക്കോലുകൾ[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_ഇടത്_ARROW),	    'k', 'l'},
    {K(B_വലത്_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_തിരുകുക),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_താക്കോൽ  11
    {F(B_F1_താക്കോൽ),	'k', '1'},
    {F(B_F2_താക്കോൽ),	'k', '2'},
    {F(B_F3_താക്കോൽ),	'k', '3'},
    {F(B_F4_താക്കോൽ),	'k', '4'},
    {F(B_F5_താക്കോൽ),	'k', '5'},
    {F(B_F6_താക്കോൽ),	'k', '6'},
    {F(B_F7_താക്കോൽ),	'k', '7'},
    {F(B_F8_താക്കോൽ),	'k', '8'},
    {F(B_F9_താക്കോൽ),	'k', '9'},
    {F(B_F10_താക്കോൽ),	'k', ';'},

    {F(B_F11_താക്കോൽ),	'F', '1'},
    {F(B_F12_താക്കോൽ),	'F', '2'},
    //	{XK_F13,	    'F', '3'},	// would be print screen
    // sysreq
    {F(0x0F),		'F', '4'},	// scroll lock
    {F(0x10),		'F', '5'},	// pause/break
    //	{XK_F16,	'F', '6'},
    //	{XK_F17,	'F', '7'},
    //	{XK_F18,	'F', '8'},
    //	{XK_F19,	'F', '9'},
    //	 {XK_F20,	'F', 'A'},
    //	{XK_F21,	'F', 'B'},
    //	{XK_F22,	'F', 'C'},
    //	{XK_F23,	'F', 'D'},
    //	{XK_F24,	'F', 'E'},
    //	{XK_F25,	'F', 'F'},
    //	{XK_F26,	'F', 'G'},
    //	{XK_F27,	'F', 'H'},
    //	{XK_F28,	'F', 'I'},
    //	{XK_F29,	'F', 'J'},
    //	{XK_F30,	'F', 'K'},
    //	{XK_F31,	'F', 'L'},
    //	{XK_F32,	'F', 'M'},
    //	{XK_F33,	'F', 'N'},
    //	{XK_F34,	'F', 'O'},
    //	{XK_F35,	'F', 'P'},	// keysymdef.h defines up to F35

    //	{XK_Help,	'%', '1'},	// XK_Help
    {F(B_PRINT_താക്കോൽ),	    '%', '9'},

#എങ്കിൽ 0
    // Keypad keys:
    {F(0x48),	    'k', 'l'},	    // XK_KP_Left
    {F(0x4A),	    'k', 'r'},	    // XK_KP_Right
    {F(0x38),	    'k', 'u'},	    // XK_KP_Up
    {F(0x59),	    'k', 'd'},	    // XK_KP_Down
    {F(0x64),	    'k', 'I'},	    // XK_KP_Insert
    {F(0x65),	    'k', 'D'},	    // XK_KP_Delete
    {F(0x37),	    'k', 'h'},	    // XK_KP_Home
    {F(0x58),	    '@', '7'},	    // XK_KP_End
    {F(0x39),	    'k', 'P'},	    // XK_KP_Prior
    {F(0x60),	    'k', 'N'},	    // XK_KP_Next
    {F(0x49),	    '&', '8'},	    // XK_Undo, keypad 5
#endif

    // End of list marker:
    {0,		    0, 0}
};

#define അക്കം_SPECIAL_താക്കോലുകൾ    ARRAY_LENGTH(special_താക്കോലുകൾ)

// ---------------- VimApp ----------------

    സ്ഥിതം ശൂന്യം
docd(BPath &path)
{
    mch_chdir((അക്ഷരം *)path.Path());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	സ്ഥിതം ശൂന്യം
drop_callback(ശൂന്യം *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	സ്ഥിതം ശൂന്യം
RefsReceived(BMessage *m, തർക്കം changedir)
{
    uint32 type;
    int32 എണ്ണം;

    m->PrintToStream();
    തിരഞ്ഞെടുപ്പ് (m->what) {
	അവസ്ഥ B_REFS_RECEIVED:
	അവസ്ഥ B_SIMPLE_DATA:
	    m->GetInfo("refs", &type, &എണ്ണം);
	    എങ്കിൽ (type != B_REF_TYPE)
		പോകുക bad;
	    നിർത്തുക;
	അവസ്ഥ B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &എണ്ണം);
	    എങ്കിൽ (type != B_STRING_TYPE)
		പോകുക bad;
	    എങ്കിൽ (changedir) {
		അക്ഷരം *dirname;
		എങ്കിൽ (m->FindString("cwd", (സ്ഥിരം അക്ഷരം **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    തിരികെ;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(എണ്ണം * വലുപ്പം(char_u *));
    പൂർണ്ണസംഖ്യ fname_index = 0;

    തിരഞ്ഞെടുപ്പ് (m->what) {
	അവസ്ഥ B_REFS_RECEIVED:
	അവസ്ഥ B_SIMPLE_DATA:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; ++i)
	    {
		entry_ref ref;
		എങ്കിൽ (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, അസത്യം);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    എങ്കിൽ (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    എങ്കിൽ (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			എങ്കിൽ (!changedir)
			    docd(path);
		    } അല്ലെങ്കിൽ {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			എങ്കിൽ (fname == ശൂന്യ)
			    fname = (char_u *)path.Path();
			fnames[fname_index++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = അസത്യം;
		}
	    }
	    നിർത്തുക;
	അവസ്ഥ B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < എണ്ണം; ++i)
	    {
		അക്ഷരം *fname;

		എങ്കിൽ (m->FindString("argv", i, (സ്ഥിരം അക്ഷരം **) &fname) == B_OK) {
		    fnames[fname_index++] = vim_strsave((char_u *)fname);
		}
	    }
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    // fprintf(stderr, "case default\n");
	    നിർത്തുക;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    എങ്കിൽ (fname_index > 0) {
	handle_drop(fname_index, fnames, FALSE, drop_callback, ശൂന്യ);

	setcursor();
	out_flush();
    } അല്ലെങ്കിൽ {
	vim_free(fnames);
    }
}

VimApp::VimApp(സ്ഥിരം അക്ഷരം *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(ശൂന്യ)
{
}

VimApp::~VimApp()
{
}

    ശൂന്യം
VimApp::ReadyToRun()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    ശൂന്യം
VimApp::ArgvReceived(int32 arg_argc, അക്ഷരം **arg_argv)
{
    എങ്കിൽ (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	എങ്കിൽ (gui.vimWindow)
	    gui.vimWindow->Minimize(അസത്യം);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, സത്യം);
    }
}

    ശൂന്യം
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    പൂർണ്ണസംഖ്യ limit = 15;
    ആകുമ്പോൾ (--limit >= 0 && (curbuf == ശൂന്യ || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    എങ്കിൽ (gui.vimWindow)
	gui.vimWindow->Minimize(അസത്യം);
    DetachCurrentMessage();
    SendRefs(m, സത്യം);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    ശൂന്യം
VimApp::SendRefs(BMessage *m, തർക്കം changedir)
{
    Vim_Refs_സന്ദേശം rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, Vim_സന്ദേശം::Refs, &rm, വലുപ്പം(rm));
    //	calls ::RefsReceived
}

    ശൂന്യം
VimApp::MessageReceived(BMessage *m)
{
    തിരഞ്ഞെടുപ്പ് (m->what) {
	അവസ്ഥ 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    നിർത്തുക;
	അവസ്ഥ 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    നിർത്തുക;
	അവസ്ഥ B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (ശൂന്യം**)&panel);
		എങ്കിൽ (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    Inherited::MessageReceived(m);
	    നിർത്തുക;
    }
}

    തർക്കം
VimApp::QuitRequested()
{
    (ശൂന്യം)Inherited::QuitRequested();
    തിരികെ അസത്യം;
}

// ---------------- VimWindow ----------------

VimWindow::VimWindow():
    BWindow(BRect(40, 40, 150, 150),
	    "Vim",
	    B_TITLED_WINDOW,
	    0,
	    B_CURRENT_WORKSPACE)

{
    init();
}

VimWindow::~VimWindow()
{
    എങ്കിൽ (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = ശൂന്യ;
}

    ശൂന്യം
VimWindow::init()
{
    // Attach the VimFormView
    formView = പുതിയ VimFormView(Bounds());
    എങ്കിൽ (formView != ശൂന്യ) {
	AddChild(formView);
    }
}

#എങ്കിൽ 0  //  disabled in zeta patch
    ശൂന്യം
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    തിരഞ്ഞെടുപ്പ് (m->what) {
	അവസ്ഥ B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		എങ്കിൽ (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} അല്ലെങ്കിൽ {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    ശൂന്യം
VimWindow::WindowActivated(തർക്കം active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    എങ്കിൽ (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(സത്യം);

    ഘടന Vim_Focus_സന്ദേശം fm;
    fm.active = active;

    write_port(gui.vdcmp, Vim_സന്ദേശം::Focus, &fm, വലുപ്പം(fm));
}

    തർക്കം
VimWindow::QuitRequested()
{
    ഘടന Vim_താക്കോൽ_സന്ദേശം km;
    km.length = 5;
    memcpy((അക്ഷരം *)km.chars, "\033:qa\r", km.length);
    km.csi_escape = അസത്യം;
    write_port(gui.vdcmp, Vim_സന്ദേശം::താക്കോൽ_2, &km, വലുപ്പം(km));
    തിരികെ അസത്യം;
}
    ശൂന്യം
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    തർക്കം should_propagate = സത്യം;

    തിരഞ്ഞെടുപ്പ് (m->what)
    {
	അവസ്ഥ B_താക്കോൽ_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		എങ്കിൽ (scancode == B_ENTER && (beModifiers & B_ഇടത്_COMMAND_താക്കോൽ))
		    {
			should_propagate = അസത്യം;
			എങ്കിൽ (ഇത്->is_fullscreen)
			    {
				ഇത്->is_fullscreen = അസത്യം;
				ResizeTo(ഇത്->saved_frame.Width(), ഇത്->saved_frame.Height());
				MoveTo(ഇത്->saved_frame.ഇടത്, ഇത്->saved_frame.മുകൾ);
				SetLook(ഇത്->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } അല്ലെങ്കിൽ {
				ഇത്->saved_frame = Frame();
				ഇത്->saved_look = Look();
				ഇത്->is_fullscreen = സത്യം;
				BScreen s(ഇത്);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().ഇടത്, s.Frame().മുകൾ);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    എങ്കിൽ (should_propagate)
	Inherited::DispatchMessage(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect frame):
    BView(frame, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_FRAME_EVENTS),
    menuBar(ശൂന്യ),
#ifdef FEAT_TOOLBAR
    toolBar(ശൂന്യ),
#endif
#ifdef FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_വരി(ശൂന്യ),
#endif
    textArea(ശൂന്യ)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    എങ്കിൽ (menuBar) {
	RemoveChild(menuBar);
#ifdef never
	//  deleting the menuBar leads to SEGV on exit
	//  richard@whitequeen.com Jul 99
	delete menuBar;
#endif
    }

#ifdef FEAT_TOOLBAR
    delete toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    delete tab_വരി;
#endif

    എങ്കിൽ (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = ശൂന്യ;
}

    ശൂന്യം
VimFormView::init(BRect frame)
{
    menuBar = പുതിയ BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = പുതിയ VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tab_വരി = പുതിയ Vim_Tab_വരി(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tab_വരി);
#endif

    BRect remaining = frame;
    textArea = പുതിയ VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = ഇത്;
}

#ifdef FEAT_TOOLBAR
    ദശാംശം
VimFormView::ToolbarHeight() സ്ഥിരം
{
    തിരികെ toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    ദശാംശം
VimFormView::TablineHeight() സ്ഥിരം
{
    തിരികെ (tab_വരി && Is_Showing_Tab_വരി()) ? tab_വരി->TablineHeight() : 0.;
}
#endif

    ശൂന്യം
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    എങ്കിൽ (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.മുകൾ += MenuHeight();
	menuBar->ResizeTo(remaining.വലത്, remaining.മുകൾ);
	gui.menu_height = (പൂർണ്ണസംഖ്യ) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.ഇടത്, remaining.മുകൾ);
	toolBar->ResizeTo(remaining.വലത്, ToolbarHeight());
	remaining.മുകൾ += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tab_വരി->MoveTo(remaining.ഇടത്, remaining.മുകൾ);
	tab_വരി->ResizeTo(remaining.വലത് + 1, TablineHeight());
	remaining.മുകൾ += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.ഇടത്, remaining.മുകൾ);
    }


    Inherited::AllAttached();
}

    ശൂന്യം
VimFormView::FrameResized(ദശാംശം new_width, ദശാംശം new_height)
{
    ഘടന Vim_Resize_സന്ദേശം sm;
    പൂർണ്ണസംഖ്യ adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (പൂർണ്ണസംഖ്യ) new_width;
    sm.height = (പൂർണ്ണസംഖ്യ) new_height;
    adjust_w = ((പൂർണ്ണസംഖ്യ)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((പൂർണ്ണസംഖ്യ)new_height - gui_get_base_height()) % gui.char_height;

    എങ്കിൽ (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, Vim_സന്ദേശം::Resize, &sm, വലുപ്പം(sm));
    //	calls gui_resize_shell(new_width, new_height);

    തിരികെ;

    /*
     * The area below the vertical scrollbar is erased to the colour
     * set with SetViewColor() automatically, because we had set
     * B_WILL_DRAW. Resizing the window tight around the vertical
     * scroll bar also helps to avoid debris.
     */
}

// ---------------- VimTextAreaView ----------------

VimTextAreaView::VimTextAreaView(BRect frame):
    BView(frame, "VimTextAreaView", B_FOLLOW_ALL_SIDES,
#ifdef FEAT_MBYTE_IME
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_നിവേശം_METHOD_AWARE
#അല്ലെങ്കിൽ
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouse_Drag_Event_എണ്ണം(0)
{
#ifdef FEAT_MBYTE_IME
    IMData.messenger = ശൂന്യ;
    IMData.message = ശൂന്യ;
#endif
    init(frame);
}

VimTextAreaView::~VimTextAreaView()
{
    gui.vimTextArea = ശൂന്യ;
}

    ശൂന്യം
VimTextAreaView::init(BRect frame)
{
    // set up global var for fast access
    gui.vimTextArea = ഇത്;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    SetViewColor(B_TRANSPARENT_32_BIT);
#define PEN_WIDTH   1
    SetPenSize(PEN_WIDTH);
#define W_WIDTH(curwin)   0
}

    ശൂന്യം
VimTextAreaView::Draw(BRect updateRect)
{
    /*
     * XXX Other ports call here:
     * out_flush();	 * make sure all output has been processed *
     * but we can't do that, since it involves too much information
     * that is owned by other threads...
     */

    /*
     *	No need to use gui.vimWindow->Lock(): we are locked already.
     *	However, it would not hurt.
     */
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);
    Set_താഴെയുള്ള_Color(rgb);
    FillRect(updateRect, B_SOLID_താഴെയുള്ള);
    gui_redraw((പൂർണ്ണസംഖ്യ) updateRect.ഇടത്, (പൂർണ്ണസംഖ്യ) updateRect.മുകൾ,
	    (പൂർണ്ണസംഖ്യ) (updateRect.Width() + PEN_WIDTH), (പൂർണ്ണസംഖ്യ) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    Set_താഴെയുള്ള_Color(rgb);

    എങ്കിൽ (updateRect.ഇടത് < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.ഇടത്, updateRect.മുകൾ,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_താഴെയുള്ള);
    എങ്കിൽ (updateRect.മുകൾ < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.ഇടത്, updateRect.മുകൾ,
		    updateRect.വലത്, FILL_Y(0)-PEN_WIDTH), B_SOLID_താഴെയുള്ള);
    എങ്കിൽ (updateRect.വലത് >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((പൂർണ്ണസംഖ്യ)Columns), updateRect.മുകൾ,
		    updateRect.വലത്, updateRect.bottom), B_SOLID_താഴെയുള്ള);
    എങ്കിൽ (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.ഇടത്, FILL_Y((പൂർണ്ണസംഖ്യ)Rows),
		    updateRect.വലത്, updateRect.bottom), B_SOLID_താഴെയുള്ള);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    ശൂന്യം
VimTextAreaView::താക്കോൽ_Down(സ്ഥിരം അക്ഷരം *bytes, int32 അക്കം_Bytes)
{
    ഘടന Vim_താക്കോൽ_സന്ദേശം km;
    char_u *dest = km.chars;

    തർക്കം canHaveVimModifiers = അസത്യം;

    BMessage *സന്ദേശം = Window()->CurrentMessage();
    പ്രതിജ്ഞാ(സന്ദേശം);
    // msg->PrintToStream();

    /*
     * Convert special keys to Vim codes.
     * I think it is better to do it in the window thread
     * so we use at least a little bit of the potential
     * of our 2 CPUs. Besides, due to the fantastic mapping
     * of special keys to UTF-8, we have quite some work to
     * do...
     * TODO: I'm not quite happy with detection of special
     * keys. Perhaps I should use scan codes after all...
     */
    എങ്കിൽ (അക്കം_Bytes > 1) {
	// This cannot be a special key
	എങ്കിൽ (അക്കം_Bytes > താക്കോൽ_സന്ദേശം_BUFSIZ)
	    അക്കം_Bytes = താക്കോൽ_സന്ദേശം_BUFSIZ;	    //	should never happen... ???
	km.length = അക്കം_Bytes;
	memcpy((അക്ഷരം *)dest, bytes, അക്കം_Bytes);
	km.csi_escape = സത്യം;
    } അല്ലെങ്കിൽ {
	int32 scancode = 0;
	സന്ദേശം->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	സന്ദേശം->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	പൂർണ്ണസംഖ്യ len = 0;
	km.length = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	പ്രതിജ്ഞാ(B_BACKSPACE <= 0x20);
	പ്രതിജ്ഞാ(B_DELETE == 0x7F);
	എങ്കിൽ (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		അക്കം_Bytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    എങ്കിൽ (beModifiers & B_CONTROL_താക്കോൽ) {
		പൂർണ്ണസംഖ്യ index = താക്കോൽ_Map->normal_map[scancode];
		പൂർണ്ണസംഖ്യ new_അക്കം_Bytes = താക്കോൽ_Map_Chars[index];
		char_u *newBytes = (char_u *)&താക്കോൽ_Map_Chars[index + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		എങ്കിൽ (new_അക്കം_Bytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    പോകുക notspecial;
		}
		bytes = (അക്ഷരം *)newBytes;
	    }
	    canHaveVimModifiers = സത്യം;

	    uint16 beoskey;
	    പൂർണ്ണസംഖ്യ first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    എങ്കിൽ (അക്കം_Bytes == 0 || bytes[0] == B_FUNCTION_താക്കോൽ) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_താക്കോൽ;
		last = അക്കം_SPECIAL_താക്കോലുകൾ;
	    } അല്ലെങ്കിൽ എങ്കിൽ (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } അല്ലെങ്കിൽ {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_താക്കോൽ;
	    }

	    വേണ്ടി (പൂർണ്ണസംഖ്യ i = first; i < last; i++) {
		എങ്കിൽ (special_താക്കോലുകൾ[i].Be_താക്കോലുകൾ == beoskey) {
		    string[0] = CSI;
		    string[1] = special_താക്കോലുകൾ[i].vim_code0;
		    string[2] = special_താക്കോലുകൾ[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	എങ്കിൽ (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#എങ്കിൽ 0
	എങ്കിൽ (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#അല്ലെങ്കിൽ
	    എങ്കിൽ (canHaveVimModifiers)
#endif
	    {
		പൂർണ്ണസംഖ്യ modifiers;
		modifiers = 0;
		എങ്കിൽ (beModifiers & B_SHIFT_താക്കോൽ)
		    modifiers |= MOD_MASK_SHIFT;
		എങ്കിൽ (beModifiers & B_CONTROL_താക്കോൽ)
		    modifiers |= MOD_MASK_CTRL;
		എങ്കിൽ (beModifiers & B_OPTION_താക്കോൽ)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		പൂർണ്ണസംഖ്യ താക്കോൽ;
		എങ്കിൽ (string[0] == CSI && len == 3)
		    താക്കോൽ = TO_SPECIAL(string[1], string[2]);
		അല്ലെങ്കിൽ
		    താക്കോൽ = string[0];
		താക്കോൽ = simplify_താക്കോൽ(താക്കോൽ, &modifiers);
		എങ്കിൽ (IS_SPECIAL(താക്കോൽ))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(താക്കോൽ);
		    string[2] = K_THIRD(താക്കോൽ);
		    len = 3;
		}
		അല്ലെങ്കിൽ
		{
		    string[0] = താക്കോൽ;
		    len = 1;
		}

		എങ്കിൽ (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.length = 3;
		}
	    }
	memcpy((അക്ഷരം *)dest, string, len);
	km.length += len;
	km.csi_escape = അസത്യം;
    }

    write_port(gui.vdcmp, Vim_സന്ദേശം::താക്കോൽ_2, &km, വലുപ്പം(km));

    /*
     * blank out the pointer if necessary
     */
    എങ്കിൽ (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(സത്യം);
	gui.pointer_hidden = TRUE;
    }
}
ശൂന്യം
VimTextAreaView::guiSendMouseEvent(
	പൂർണ്ണസംഖ്യ	button,
	പൂർണ്ണസംഖ്യ	x,
	പൂർണ്ണസംഖ്യ	y,
	പൂർണ്ണസംഖ്യ	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_സന്ദേശം mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, Vim_സന്ദേശം::Mouse, &mm, വലുപ്പം(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    എങ്കിൽ (gui.pointer_hidden)
    {
	guiBlankMouse(അസത്യം);
	gui.pointer_hidden = FALSE;
    }
}

ശൂന്യം
VimTextAreaView::guiMouseMoved(
	പൂർണ്ണസംഖ്യ	x,
	പൂർണ്ണസംഖ്യ	y)
{
    Vim_Mouse_Moved_സന്ദേശം mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, Vim_സന്ദേശം::MouseMoved, &mm, വലുപ്പം(mm));

    എങ്കിൽ (gui.pointer_hidden)
    {
	guiBlankMouse(അസത്യം);
	gui.pointer_hidden = FALSE;
    }
}

    ശൂന്യം
VimTextAreaView::guiBlankMouse(തർക്കം should_hide)
{
    എങ്കിൽ (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } അല്ലെങ്കിൽ {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    എങ്കിൽ (beModifiers & B_SHIFT_താക്കോൽ)
	vim_modifiers |= MOUSE_SHIFT;
    എങ്കിൽ (beModifiers & B_CONTROL_താക്കോൽ)
	vim_modifiers |= MOUSE_CTRL;
    എങ്കിൽ (beModifiers & B_OPTION_താക്കോൽ)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    തിരികെ vim_modifiers;
}

    ശൂന്യം
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    പ്രതിജ്ഞാ(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    പൂർണ്ണസംഖ്യ vimButton;

    എങ്കിൽ (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_ഇടത്;
    അല്ലെങ്കിൽ എങ്കിൽ (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_വലത്;
    അല്ലെങ്കിൽ എങ്കിൽ (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_നടുവ്;
    അല്ലെങ്കിൽ
	തിരികെ;		// Unknown button

    vimMouseButton = 1;	    // don't care which one

    // Handle multiple clicks
    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(vimButton, point.x, point.y,
	    clicks > 1 /* = repeated_click*/, vimMouseModifiers);
}

    ശൂന്യം
VimTextAreaView::MouseUp(BPoint point)
{
    vimMouseButton = 0;

    BMessage *m = Window()->CurrentMessage();
    പ്രതിജ്ഞാ(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, point.x, point.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::MouseUp(point);
}

    ശൂന്യം
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, സ്ഥിരം BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    എങ്കിൽ (gui.pointer_hidden)
    {
	guiBlankMouse(അസത്യം);
	gui.pointer_hidden = FALSE;
    }

    എങ്കിൽ (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	തിരികെ;
    }

    atomic_add(&mouse_Drag_Event_എണ്ണം, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    ശൂന്യം
VimTextAreaView::MessageReceived(BMessage *m)
{
    തിരഞ്ഞെടുപ്പ് (m->what) {
	അവസ്ഥ 'menu':
	    {
		Vim_Menu_സന്ദേശം mm;
		mm.guiMenu = ശൂന്യ;  // in case no pointer in msg
		m->FindPointer("VimMenu", (ശൂന്യം **)&mm.guiMenu);
		write_port(gui.vdcmp, Vim_സന്ദേശം::Menu, &mm, വലുപ്പം(mm));
	    }
	    നിർത്തുക;
	അവസ്ഥ B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		ദശാംശം small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->Set_മൂല്യം(scb->മൂല്യം_2()+small*dy*3);
		scb->മൂല്യം_Changed(scb->മൂല്യം_2());
#എങ്കിൽ 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->Set_മൂല്യം(scb->മൂല്യം_2()+small*dy);
		scb->മൂല്യം_Changed(scb->മൂല്യം_2());
#endif
	    }
	    നിർത്തുക;
#ifdef FEAT_MBYTE_IME
	അവസ്ഥ B_നിവേശം_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		തിരഞ്ഞെടുപ്പ്(opcode)
		{
		    അവസ്ഥ B_നിവേശം_METHOD_STARTED:
			എങ്കിൽ (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = പുതിയ BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			നിർത്തുക;
		    അവസ്ഥ B_നിവേശം_METHOD_CHANGED:
			{
			    BString str;
			    തർക്കം confirmed;
			    എങ്കിൽ (IMData.message) *(IMData.message) = *m;
			    അല്ലെങ്കിൽ	       IMData.message = പുതിയ BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    എങ്കിൽ (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				ഘടന Vim_താക്കോൽ_സന്ദേശം km;
				km.csi_escape = സത്യം;
				പൂർണ്ണസംഖ്യ clen;
				പൂർണ്ണസംഖ്യ i = 0;
				ആകുമ്പോൾ (i < str.Length())
				{
				    clen = utf_ptr2len(chars+i);
				    memcpy(km.chars, chars+i, clen);
				    km.length = clen;
				    write_port(gui.vdcmp, Vim_സന്ദേശം::താക്കോൽ_2, &km, വലുപ്പം(km));
				    i += clen;
				}
			    }
			}
			നിർത്തുക;
		    അവസ്ഥ B_നിവേശം_METHOD_LOCATION_REQUEST:
			{
			    BMessage സന്ദേശം(B_നിവേശം_METHOD_EVENT);
			    സന്ദേശം.AddInt32("be:opcode", B_നിവേശം_METHOD_LOCATION_REQUEST);
			    സന്ദേശം.AddPoint("be:location_reply", IMData.location);
			    സന്ദേശം.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&സന്ദേശം);
			}
			നിർത്തുക;
		    അവസ്ഥ B_നിവേശം_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = ശൂന്യ;
			IMData.message = ശൂന്യ;
			നിർത്തുക;
		}
	    }
	    // TODO: sz: break here???
#endif
	സ്ഥിരസ്ഥിതി:
	    എങ്കിൽ (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(അസത്യം);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_താക്കോൽ) != 0);
	    } അല്ലെങ്കിൽ {
		Inherited::MessageReceived(m);
	    }
	    നിർത്തുക;
    }
}

    പൂർണ്ണസംഖ്യ
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    എങ്കിൽ (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	എങ്കിൽ (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	തിരികെ OK;
    }
    തിരികെ FAIL;
}

    ശൂന്യം
VimTextAreaView::mchDrawString(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, char_u *s, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    എങ്കിൽ (!(flags & DRAW_TRANSP)) {
	പൂർണ്ണസംഖ്യ cells;
	cells = 0;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i=0; i<len; i++) {
	    പൂർണ്ണസംഖ്യ cn = utf_ptr2cells((char_u *)(s+i));
	    എങ്കിൽ (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_താഴെയുള്ള);
    }

    BFont font;
    ഇത്->GetFont(&font);
    എങ്കിൽ (!font.IsFixed())
    {
	അക്ഷരം* p = (അക്ഷരം*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	പൂർണ്ണസംഖ്യ cells;
	ആകുമ്പോൾ ((p - (അക്ഷരം*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    എങ്കിൽ (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    എങ്കിൽ (cells<4) lastpos += cells;
	    അല്ലെങ്കിൽ	lastpos++;
	    p += clen;
	}
    }
    അല്ലെങ്കിൽ
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((അക്ഷരം*)s, len, where);
	എങ്കിൽ (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((അക്ഷരം*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    എങ്കിൽ (flags & DRAW_UNDERL) {
	പൂർണ്ണസംഖ്യ cells;
	cells = 0;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i=0; i<len; i++) {
	    പൂർണ്ണസംഖ്യ cn = utf_ptr2cells((char_u *)(s+i));
	    എങ്കിൽ (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	Stroke_വരി(start, end);
    }
}

ശൂന്യം
VimTextAreaView::mchClearBlock(
	പൂർണ്ണസംഖ്യ	row1,
	പൂർണ്ണസംഖ്യ	col1,
	പൂർണ്ണസംഖ്യ	row2,
	പൂർണ്ണസംഖ്യ	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_താഴെയുള്ള);
}

    ശൂന്യം
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_താഴെയുള്ള);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    ശൂന്യം
VimTextAreaView::mch_Delete_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ അക്കം_വരികൾ)
{
    BRect source, dest;
    source.ഇടത് = FILL_X(gui.scroll_region_ഇടത്);
    source.മുകൾ = FILL_Y(row + അക്കം_വരികൾ);
    source.വലത് = FILL_X(gui.scroll_region_വലത് + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.ഇടത് = FILL_X(gui.scroll_region_ഇടത്);
    dest.മുകൾ = FILL_Y(row);
    dest.വലത് = FILL_X(gui.scroll_region_വലത് + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - അക്കം_വരികൾ + 1) - PEN_WIDTH;

    എങ്കിൽ (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - അക്കം_വരികൾ + 1,
		gui.scroll_region_ഇടത്,
		gui.scroll_region_bot, gui.scroll_region_വലത്);


	gui.vimWindow->Unlock();
	/*
	 * The Draw() callback will be called now if some of the source
	 * bits were not in the visible region.
	 */
    }
    // gui_x11_check_copy_area();
    //	}
}

/*
 * mchInsertLines() Lock()s the window by itself.
 */
    ശൂന്യം
VimTextAreaView::mch_തിരുകുക_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ അക്കം_വരികൾ)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.ഇടത് = FILL_X(gui.scroll_region_ഇടത്);
    source.മുകൾ = FILL_Y(row);
    source.വലത് = FILL_X(gui.scroll_region_വലത് + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - അക്കം_വരികൾ + 1) - PEN_WIDTH;

    dest.ഇടത് = FILL_X(gui.scroll_region_ഇടത്);
    dest.മുകൾ = FILL_Y(row + അക്കം_വരികൾ);
    dest.വലത് = FILL_X(gui.scroll_region_വലത് + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    എങ്കിൽ (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_ഇടത്,
		row + അക്കം_വരികൾ - 1, gui.scroll_region_വലത്);

	gui.vimWindow->Unlock();
	/*
	 * The Draw() callback will be called now if some of the source
	 * bits were not in the visible region.
	 * However, if we scroll too fast it can't keep up and the
	 * update region gets messed up. This seems to be because copying
	 * un-Draw()n bits does not generate Draw() calls for the copy...
	 * I moved the hack to before the CopyBits() to reduce the
	 * amount of additional waiting needed.
	 */

	// gui_x11_check_copy_area();

    }
}

#ifdef FEAT_MBYTE_IME
/*
 * DrawIMString draws string with IMData.message.
 */
ശൂന്യം VimTextAreaView::DrawIMString(ശൂന്യം)
{
    സ്ഥിതം സ്ഥിരം rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    സ്ഥിരം അക്ഷരം* s;
    പൂർണ്ണസംഖ്യ len;
    BMessage* സന്ദേശം = IMData.message;
    എങ്കിൽ (!സന്ദേശം)
	തിരികെ;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.എണ്ണം, W_WIDTH(curwin), GUI_MON_NOCLEAR);
    തർക്കം confirmed = അസത്യം;
    സന്ദേശം->FindBool("be:confirmed", &confirmed);
    എങ്കിൽ (confirmed)
	തിരികെ;
    rgb_color hcolor = മുകളിലെ_Color(), lcolor = താഴെയുള്ള_Color();
    സന്ദേശം->FindString("be:string", &str);
    s = str.String();
    len = str.Length();
    Set_മുകളിലെ_Color(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    സന്ദേശം->FindInt32("be:selection", 0, &sel_start);
    സന്ദേശം->FindInt32("be:selection", 1, &sel_end);
    പൂർണ്ണസംഖ്യ clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    വേണ്ടി (പൂർണ്ണസംഖ്യ i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	എങ്കിൽ (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	എങ്കിൽ (sel_start<=i && i<sel_end)
	{
	    Set_താഴെയുള്ള_Color(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	അല്ലെങ്കിൽ
	{
	    Set_താഴെയുള്ള_Color(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_താഴെയുള്ള);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.എണ്ണം = (പൂർണ്ണസംഖ്യ)pos.y;

    Set_മുകളിലെ_Color(hcolor);
    Set_താഴെയുള്ള_Color(lcolor);
}
#endif
// ---------------- VimScrollBar ----------------

/*
 * BUG: XXX
 * It seems that BScrollBar determine their direction not from
 * "posture" but from if they are "tall" or "wide" in shape...
 *
 * Also, place them out of sight, because Vim enables them before
 * they are positioned.
 */
VimScrollBar::VimScrollBar(scrollbar_T *g, orientation posture):
    BScrollBar(posture == B_HORIZONTAL ?  BRect(-100,-100,-10,-90) :
	    BRect(-100,-100,-90,-10),
	    "vim scrollbar", (BView *)ശൂന്യ,
	    0.0, 10.0, posture),
    ignore_മൂല്യം(-1),
    scroll_Event_എണ്ണം(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    ശൂന്യം
VimScrollBar::മൂല്യം_Changed(ദശാംശം new_മൂല്യം)
{
    എങ്കിൽ (ignore_മൂല്യം >= 0.0 && new_മൂല്യം == ignore_മൂല്യം) {
	ignore_മൂല്യം = -1;
	തിരികെ;
    }
    ignore_മൂല്യം = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_add(&scroll_Event_എണ്ണം, 1);

    ഘടന Vim_Scroll_Bar_സന്ദേശം sm;

    sm.sb = ഇത്;
    sm.മൂല്യം = (long) new_മൂല്യം;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, Vim_സന്ദേശം::ScrollBar, &sm, വലുപ്പം(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    ശൂന്യം
VimScrollBar::MouseUp(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_add(&scroll_Event_എണ്ണം, 1);

    ഘടന Vim_Scroll_Bar_സന്ദേശം sm;

    sm.sb = ഇത്;
    sm.മൂല്യം = (long) മൂല്യം_2();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, Vim_സന്ദേശം::ScrollBar, &sm, വലുപ്പം(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    ശൂന്യം
VimScrollBar::Set_മൂല്യം(ദശാംശം new_മൂല്യം)
{
    എങ്കിൽ (new_മൂല്യം == മൂല്യം_2())
	തിരികെ;

    ignore_മൂല്യം = new_മൂല്യം;
    Inherited::Set_മൂല്യം(new_മൂല്യം);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(സ്ഥിരം VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(സ്ഥിരം BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(സ്ഥിരം VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    ശൂന്യം
VimFont::init()
{
    next = ശൂന്യ;
    refcount = 1;
    name = ശൂന്യ;
}

// ---------------- VimDialog ----------------

#എങ്കിൽ defined(FEAT_GUI_DIALOG)

സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  k_Vim_Dialog_Button_സന്ദേശം = 'VMDB';
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  kVimDialogIconStripeWidth = 30;
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  kVimDialogButtonsSpacingX = 9;
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  kVimDialogButtonsSpacingY = 4;
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  kVimDialogSpacingX = 6;
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  kVimDialogSpacingY = 10;
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  kVimDialogMinimalWidth  = 310;
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  kVimDialogMinimalHeight = 75;
സ്ഥിരം BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(പൂർണ്ണസംഖ്യ type, സ്ഥിരം അക്ഷരം *title, സ്ഥിരം അക്ഷരം *message,
	സ്ഥിരം അക്ഷരം *buttons, പൂർണ്ണസംഖ്യ dfltbutton, സ്ഥിരം അക്ഷരം *textfield, പൂർണ്ണസംഖ്യ ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_മൂല്യം(dfltbutton)
    , fMessageView(ശൂന്യ)
    , f_നിവേശം_Control(ശൂന്യ)
    , f_നിവേശം_മൂല്യം(textfield)
{
    //	master view
    VimDialog::View* view = പുതിയ VimDialog::View(Bounds());
    എങ്കിൽ (view == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (title == ശൂന്യ)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    ദശാംശം maxButtonWidth  = 0;
    ദശാംശം maxButtonHeight = 0;
    ദശാംശം buttonsWidth	  = 0;
    ദശാംശം buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    ചെയ്യുക
    {
	int32 end = strButtons.FindFirst('\n');
	എങ്കിൽ (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	എങ്കിൽ (end == B_ERROR)
	    നിർത്തുക;

	strButtons.Remove(0, end + 1);
    } ആകുമ്പോൾ (സത്യം);

    int32 buttons_എണ്ണം = fButtonsList.എണ്ണം_ഇനങ്ങൾ();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttons_എണ്ണം - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttons_എണ്ണം - 1);
    ദശാംശം dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    ദശാംശം dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    തർക്കം vertical = (vim_strchr(p_go, GO_VERTICAL) != ശൂന്യ) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    എങ്കിൽ (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.ഇടത്  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.മുകൾ   += kVimDialogSpacingY;
    rect.വലത് -= kVimDialogSpacingX;
    rect.bottom = rect.മുകൾ;
    fMessageView = പുതിയ BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_ഇടത് | B_FOLLOW_മുകൾ, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(അസത്യം);
    fMessageView->MakeSelectable(അസത്യം);
    fMessageView->SetWordWrap(സത്യം);
    AddChild(fMessageView);

    ദശാംശം messageHeight = fMessageView->TextHeight(0, fMessageView->എണ്ണം_വരികൾ());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    എങ്കിൽ (f_നിവേശം_മൂല്യം != ശൂന്യ) {
	rect.മുകൾ     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	f_നിവേശം_Control = പുതിയ BTextControl(rect, "_iv_", ശൂന്യ, f_നിവേശം_മൂല്യം, ശൂന്യ,
		B_FOLLOW_ഇടത് | B_FOLLOW_മുകൾ, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_നിവേശം_Control->TextView()->SetText(f_നിവേശം_മൂല്യം);
	f_നിവേശം_Control->TextView()->SetWordWrap(അസത്യം);
	AddChild(f_നിവേശം_Control);

	ദശാംശം width = 0.f, height = 0.f;
	f_നിവേശം_Control->GetPreferredSize(&width, &height);
	f_നിവേശം_Control->MakeFocus(സത്യം);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    ദശാംശം buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    വേണ്ടി (int32 i = 0 ; i < buttons_എണ്ണം; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	എങ്കിൽ (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} അല്ലെങ്കിൽ
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	എങ്കിൽ (dfltbutton == i + 1) {
	    button->MakeDefault(സത്യം);
	    button->MakeFocus(f_നിവേശം_Control == ശൂന്യ);
	}
    }
}

VimDialog::~VimDialog()
{
    എങ്കിൽ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    പൂർണ്ണസംഖ്യ
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    എങ്കിൽ (fDialogSem < B_OK) {
	Quit();
	തിരികെ f_Dialog_മൂല്യം;
    }

    Show();

    ആകുമ്പോൾ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    പൂർണ്ണസംഖ്യ ret_മൂല്യം = f_Dialog_മൂല്യം;
    എങ്കിൽ (f_നിവേശം_മൂല്യം != ശൂന്യ)
	vim_strncpy((char_u*)f_നിവേശം_മൂല്യം, (char_u*)f_നിവേശം_Control->Text(), IOSIZE - 1);

    എങ്കിൽ (Lock())
	Quit();

    തിരികെ ret_മൂല്യം;
}

ശൂന്യം VimDialog::MessageReceived(BMessage *സന്ദേശം)
{
    int32 which = 0;
    എങ്കിൽ (സന്ദേശം->what != k_Vim_Dialog_Button_സന്ദേശം ||
	    സന്ദേശം->FindInt32("which", &which) != B_OK)
	തിരികെ BWindow::MessageReceived(സന്ദേശം);

    f_Dialog_മൂല്യം = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, സ്ഥിരം അക്ഷരം* label)
{
    BMessage *message = പുതിയ BMessage(k_Vim_Dialog_Button_സന്ദേശം);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = പുതിയ BButton(rect, name.String(), label, message,
	    B_FOLLOW_വലത് | B_FOLLOW_BOTTOM);

    ദശാംശം width = 0.f, height = 0.f;
    button->GetPreferredSize(&width, &height);
    button->ResizeTo(width, height);

    തിരികെ button;
}

VimDialog::View::View(BRect frame)
    :	BView(frame, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(ശൂന്യ)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    delete fIconBitmap;
}

ശൂന്യം VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.വലത് = kVimDialogIconStripeWidth;
    Set_മുകളിലെ_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    എങ്കിൽ (fIconBitmap == ശൂന്യ)
	തിരികെ;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

ശൂന്യം VimDialog::View::InitIcon(int32 type)
{
    എങ്കിൽ (type == VIM_GENERIC)
	തിരികെ;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    എങ്കിൽ (status != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(status));
	തിരികെ;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    എങ്കിൽ (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	തിരികെ;
    }

    BResources resources(&file);
    എങ്കിൽ (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	തിരികെ;
    }

    സ്ഥിരം അക്ഷരം *name = "";
    തിരഞ്ഞെടുപ്പ്(type) {
	അവസ്ഥ VIM_ERROR:	    name = "stop"; നിർത്തുക;
	അവസ്ഥ VIM_WARNING:   name = "warn"; നിർത്തുക;
	അവസ്ഥ VIM_INFO:	    name = "info"; നിർത്തുക;
	അവസ്ഥ VIM_QUESTION:  name = "idea"; നിർത്തുക;
	സ്ഥിരസ്ഥിതി: തിരികെ;
    }

    int32 iconSize = 32;
    fIconBitmap = പുതിയ BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), 0, B_RGBA32);
    എങ്കിൽ (fIconBitmap == ശൂന്യ || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == ശൂന്യ) ? "null" : strerror(fIconBitmap->InitCheck()));
	തിരികെ;
    }

    size_t size = 0;
    സ്ഥിരം uint8* iconData = ശൂന്യ;
    //	try vector icon first?
    iconData = (സ്ഥിരം uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &size);
    എങ്കിൽ (iconData != ശൂന്യ && BIconUtils::GetVectorIcon(iconData, size, fIconBitmap) == B_OK)
	തിരികെ;

    //	try bitmap icon now
    iconData = (സ്ഥിരം uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &size);
    എങ്കിൽ (iconData == ശൂന്യ) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = ശൂന്യ;
	തിരികെ;
    }

    എങ്കിൽ (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(iconData, iconSize, iconSize, iconSize, fIconBitmap);
}

സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  k_Vim_Dialog_OKButton_സന്ദേശം = 'FDOK';
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  k_Vim_Dialog_Cancel_Button_സന്ദേശം = 'FDCN';
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  k_Vim_Dialog_Size_നിവേശം_സന്ദേശം = 'SICH';
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  k_Vim_Dialog_Family_Select_സന്ദേശം = 'MSFM';
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  k_Vim_Dialog_Style_Select_സന്ദേശം = 'MSST';
സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ  k_Vim_Dialog_Size_Select_സന്ദേശം = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, ദശാംശം* size)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_മൂല്യം(അസത്യം)
    , fFamily(family)
    , fStyle(style)
    , fSize(size)
    , fFontSize(*size)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_നിവേശം(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_LENGTH);
    strncpy(fFontStyle, *style, B_FONT_STYLE_LENGTH);

    //	"client" area view
    BBox *clientBox = പുതിയ BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.ഇടത്_മുകൾ(), RC.ഇടത്_മുകൾ());

    //	at first create all controls
    fPreview = പുതിയ BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = പുതിയ BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = പുതിയ BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = പുതിയ BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *labelSize = പുതിയ BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(labelSize);
    labelSize->ResizeToPreferred();

    fFamiliesList = പുതിയ BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = പുതിയ BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_ഇടത്_വലത്, 0, അസത്യം, സത്യം);
    clientBox->AddChild(scrollFamilies);

    fStylesList= പുതിയ BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = പുതിയ BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_ഇടത്_വലത്, 0, അസത്യം, സത്യം);
    clientBox->AddChild(scrollStyles);

    f_Sizes_നിവേശം = പുതിയ BTextControl(rc, "inputSize", ശൂന്യ, "???",
	    പുതിയ BMessage(k_Vim_Dialog_Size_നിവേശം_സന്ദേശം));
    clientBox->AddChild(f_Sizes_നിവേശം);
    f_Sizes_നിവേശം->ResizeToPreferred();

    fSizesList = പുതിയ BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = പുതിയ BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_ഇടത്_വലത്, 0, അസത്യം, സത്യം);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = പുതിയ BButton(rc, "buttonOK", "OK",
			പുതിയ BMessage(k_Vim_Dialog_OKButton_സന്ദേശം));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = പുതിയ BButton(rc, "buttonCancel", "Cancel",
			പുതിയ BMessage(k_Vim_Dialog_Cancel_Button_സന്ദേശം));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    ദശാംശം വരി_Height = labelFamily->Bounds().Height();
    ദശാംശം previewHeight = വരി_Height * 3;
    ദശാംശം offsetYLabels = previewHeight + kVimDialogSpacingY;
    ദശാംശം offsetYLists = offsetYLabels + വരി_Height + kVimDialogSpacingY / 2;
    ദശാംശം offsetYSizes = offsetYLists + f_Sizes_നിവേശം->Bounds().Height() + kVimDialogSpacingY / 2;
    ദശാംശം listsHeight = വരി_Height * 9;
    ദശാംശം offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    ദശാംശം maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    ദശാംശം familiesWidth = labelFamily->Bounds().Width() * 5;
    ദശാംശം offsetXStyles = familiesWidth + kVimDialogSpacingX;
    ദശാംശം stylesWidth = labelStyle->Bounds().Width() * 4;
    ദശാംശം offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    ദശാംശം sizesWidth = labelSize->Bounds().Width() * 2;
    ദശാംശം maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.ഇടത് + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.മുകൾ + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    labelSize->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    ദശാംശം insetX = f_Sizes_നിവേശം->TextView()->Bounds().Width() - f_Sizes_നിവേശം->Bounds().Width();
    ദശാംശം insetY = f_Sizes_നിവേശം->TextView()->Bounds().Width() - f_Sizes_നിവേശം->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    f_Sizes_നിവേശം->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    f_Sizes_നിവേശം->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    f_Sizes_നിവേശം->ResizeTo(sizesWidth, f_Sizes_നിവേശം->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    പൂർണ്ണസംഖ്യ selIndex = -1;
    പൂർണ്ണസംഖ്യ എണ്ണം = എണ്ണം_font_families();
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം; i++) {
	font_family family;
	എങ്കിൽ (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(പുതിയ BStringItem((സ്ഥിരം അക്ഷരം*)family));
	    എങ്കിൽ (strncmp(family, fFontFamily, B_FONT_FAMILY_LENGTH) == 0)
		selIndex = i;
	}
    }

    എങ്കിൽ (selIndex >= 0) {
	fFamiliesList->Select(selIndex);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    selIndex = -1;
    വേണ്ടി (പൂർണ്ണസംഖ്യ size = 8, index = 0; size <= 18; size++, index++) {
	BString str;
	str << size;
	fSizesList->AddItem(പുതിയ BStringItem(str));
	എങ്കിൽ (size == fFontSize)
	    selIndex = index;

    }

    എങ്കിൽ (selIndex >= 0) {
	fSizesList->Select(selIndex);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(പുതിയ BMessage(k_Vim_Dialog_Family_Select_സന്ദേശം));
    fStylesList->SetSelectionMessage(പുതിയ BMessage(k_Vim_Dialog_Style_Select_സന്ദേശം));
    fSizesList->SetSelectionMessage(പുതിയ BMessage(k_Vim_Dialog_Size_Select_സന്ദേശം));
    f_Sizes_നിവേശം->SetModificationMessage(പുതിയ BMessage(k_Vim_Dialog_Size_നിവേശം_സന്ദേശം));

    Update_Size_നിവേശം_Preview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    എങ്കിൽ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    ശൂന്യം
VimSelectFontDialog::_CleanList(BListView* list)
{
    ആകുമ്പോൾ (0 < list->എണ്ണം_ഇനങ്ങൾ())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    തർക്കം
VimSelectFontDialog::Go()
{
    എങ്കിൽ (fStatus != B_OK) {
	Quit();
	തിരികെ NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    എങ്കിൽ (fDialogSem < B_OK) {
	Quit();
	തിരികെ f_Dialog_മൂല്യം;
    }

    Show();

    ആകുമ്പോൾ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    തർക്കം ret_മൂല്യം = f_Dialog_മൂല്യം;

    എങ്കിൽ (Lock())
	Quit();

    തിരികെ ret_മൂല്യം;
}


ശൂന്യം VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 selIndex = -1;
    int32 എണ്ണം = എണ്ണം_font_styles(fFontFamily);
    വേണ്ടി (int32 i = 0; i < എണ്ണം; i++) {
	font_style style;
	uint32 flags = 0;
	എങ്കിൽ (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(പുതിയ BStringItem((സ്ഥിരം അക്ഷരം*)style));
	    എങ്കിൽ (strncmp(style, fFontStyle, B_FONT_STYLE_LENGTH) == 0)
		selIndex = i;
	}
    }

    എങ്കിൽ (selIndex >= 0) {
	fStylesList->Select(selIndex);
	fStylesList->ScrollToSelection();
    } അല്ലെങ്കിൽ
	fStylesList->Select(0);
}


ശൂന്യം VimSelectFontDialog::Update_Size_നിവേശം_Preview()
{
    അക്ഷരം buf[10] = {0};
    vim_snprintf(buf, വലുപ്പം(buf), (അക്ഷരം*)"%.0f", fFontSize);
    f_Sizes_നിവേശം->SetText(buf);
}


ശൂന്യം VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_SIZE);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (പൂർണ്ണസംഖ്യ)fFontSize << " pt.";
    fPreview->SetText(str);
}


    തർക്കം
VimSelectFontDialog::_UpdateFromListItem(BListView* list, അക്ഷരം* text, പൂർണ്ണസംഖ്യ textSize)
{
    int32 index = list->CurrentSelection();
    എങ്കിൽ (index < 0)
	തിരികെ അസത്യം;
    BStringItem* item = (BStringItem*)list->ItemAt(index);
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ അസത്യം;
    strncpy(text, item->Text(), textSize);
    തിരികെ സത്യം;
}


ശൂന്യം VimSelectFontDialog::MessageReceived(BMessage *സന്ദേശം)
{
    തിരഞ്ഞെടുപ്പ് (സന്ദേശം->what) {
	അവസ്ഥ k_Vim_Dialog_OKButton_സന്ദേശം:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_LENGTH);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_LENGTH);
	    *fSize = fFontSize;
	    f_Dialog_മൂല്യം = സത്യം;
	അവസ്ഥ k_Vim_Dialog_Cancel_Button_സന്ദേശം:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    തിരികെ;
	അവസ്ഥ B_താക്കോൽ_UP:
	    {
		int32 താക്കോൽ = 0;
		എങ്കിൽ (സന്ദേശം->FindInt32("raw_char", &താക്കോൽ) == B_OK
			&& താക്കോൽ == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    നിർത്തുക;

	അവസ്ഥ k_Vim_Dialog_Family_Select_സന്ദേശം:
	    എങ്കിൽ (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_LENGTH)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    നിർത്തുക;
	അവസ്ഥ k_Vim_Dialog_Style_Select_സന്ദേശം:
	    എങ്കിൽ (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_LENGTH))
		_UpdateFontPreview();
	    നിർത്തുക;
	അവസ്ഥ k_Vim_Dialog_Size_Select_സന്ദേശം:
	    {
		അക്ഷരം buf[10] = {0};
		എങ്കിൽ (_UpdateFromListItem(fSizesList, buf, വലുപ്പം(buf))) {
		    ദശാംശം size = atof(buf);
		    എങ്കിൽ (size > 0.f) {
			fFontSize = size;
			Update_Size_നിവേശം_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    നിർത്തുക;
	അവസ്ഥ k_Vim_Dialog_Size_നിവേശം_സന്ദേശം:
	    {
		ദശാംശം size = atof(f_Sizes_നിവേശം->Text());
		എങ്കിൽ (size > 0.f) {
		    fFontSize = size;
		    _UpdateFontPreview();
		}
	    }
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
    തിരികെ BWindow::MessageReceived(സന്ദേശം);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
സ്ഥിതം BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, സ്ഥിരം അക്ഷരം *name) :
    BBox(frame, name, B_FOLLOW_ഇടത് | B_FOLLOW_മുകൾ, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 എണ്ണം = fButtonsList.എണ്ണം_ഇനങ്ങൾ();
    വേണ്ടി (int32 i = 0; i < എണ്ണം; i++)
	delete (BPictureButton*)fButtonsList.ItemAt(i);
    fButtonsList.MakeEmpty();

    delete normalButtonsBitmap;
    delete grayedButtonsBitmap;
    normalButtonsBitmap    = ശൂന്യ;
    grayedButtonsBitmap  = ശൂന്യ;
}

    ശൂന്യം
VimToolbar::AttachedToWindow()
{
    BBox::AttachedToWindow();

    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    ദശാംശം
VimToolbar::ToolbarHeight() സ്ഥിരം
{
    ദശാംശം size = ശൂന്യ == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    തിരികെ size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    തർക്കം
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    ദശാംശം height = bitmap->Bounds().Height();
    ദശാംശം width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->BitsLength() / 4;
    വേണ്ടി (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    തിരികെ സത്യം;
}

    തർക്കം
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    എങ്കിൽ (normalButtonsBitmap == ശൂന്യ)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    എങ്കിൽ (normalButtonsBitmap == ശൂന്യ)
	തിരികെ അസത്യം;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = പുതിയ BBitmap(&archive);
    എങ്കിൽ (grayedButtonsBitmap == ശൂന്യ)
	തിരികെ അസത്യം;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    തിരികെ സത്യം;
}

BBitmap *VimToolbar::LoadVimBitmap(സ്ഥിരം അക്ഷരം* fileName)
{
    BBitmap *bitmap = ശൂന്യ;

    പൂർണ്ണസംഖ്യ mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    എങ്കിൽ (runtimePath != ശൂന്യ && fileName != ശൂന്യ) {
	BString strPath((അക്ഷരം*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    എങ്കിൽ (mustfree)
	vim_free(runtimePath);

    തിരികെ bitmap;
}

    തർക്കം
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, തർക്കം pressed)
{
    ദശാംശം size = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, size, size), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.Set_മുകളിലെ_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, size - 1, size - 1);
    BRect destination(source);

    source.OffsetBy(size * index, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    എങ്കിൽ (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	size += ButtonMargin * 2 - 1;
	view.Begin_വരി_Array(4);
	view.Add_വരി(BPoint(0, 0),	 BPoint(size, 0),    shadowColor);
	view.Add_വരി(BPoint(size, 0),	 BPoint(size, size), shineColor);
	view.Add_വരി(BPoint(size, size), BPoint(0, size),    shineColor);
	view.Add_വരി(BPoint(0, size),	 BPoint(0, 0),	     shadowColor);
	view.End_വരി_Array();
    }

    view.EndPicture();
    RemoveChild(&view);

    തിരികെ സത്യം;
}

    തർക്കം
VimToolbar::AddButton(int32 index, vimmenu_T *menu)
{
    BPictureButton *button = ശൂന്യ;
    എങ്കിൽ (!menu_is_separator(menu->name)) {
	ദശാംശം size = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, size, size);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	എങ്കിൽ (menu->iconfile == ശൂന്യ && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, സത്യം);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, അസത്യം);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, അസത്യം);
	} അല്ലെങ്കിൽ {

	    char_u buffer[MAXPATHL] = {0};
	    BBitmap *bitmap = ശൂന്യ;

	    എങ്കിൽ (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, buffer, (അക്ഷരം*)"png");
		bitmap = BTranslationUtils::GetBitmap((അക്ഷരം*)buffer);
	    }

	    എങ്കിൽ (bitmap == ശൂന്യ && gui_find_bitmap(menu->name, buffer, (അക്ഷരം*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((അക്ഷരം*)buffer);

	    എങ്കിൽ (bitmap == ശൂന്യ)
		bitmap = പുതിയ BBitmap(BRect(0, 0, size, size), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, സത്യം);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, അസത്യം);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, അസത്യം);

	    delete bitmap;
	}

	button = പുതിയ BPictureButton(frame, (അക്ഷരം*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    തർക്കം result = fButtonsList.AddItem(button, index);
    InvalidateLayout();
    തിരികെ result;
}

    തർക്കം
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    എങ്കിൽ (menu->button) {
	എങ്കിൽ (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = ശൂന്യ;
	}
    }
    തിരികെ സത്യം;
}

    തർക്കം
VimToolbar::GrayButton(vimmenu_T *menu, പൂർണ്ണസംഖ്യ grey)
{
    എങ്കിൽ (menu->button) {
	int32 index = fButtonsList.IndexOf(menu->button);
	എങ്കിൽ (index >= 0)
	    menu->button->SetEnabled(grey ? അസത്യം : സത്യം);
    }
    തിരികെ സത്യം;
}

    ശൂന്യം
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 എണ്ണം = fButtonsList.എണ്ണം_ഇനങ്ങൾ();
    വേണ്ടി (int32 i = 0; i < എണ്ണം; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	എങ്കിൽ (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} അല്ലെങ്കിൽ
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#എങ്കിൽ defined(FEAT_GUI_TABLINE)

    ദശാംശം
Vim_Tab_വരി::TablineHeight() സ്ഥിരം
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    തിരികെ TabHeight(); //  + ToolbarMargin;
}

ശൂന്യം
Vim_Tab_വരി::MouseDown(BPoint point)
{
    എങ്കിൽ (!gui_mch_showing_tabline())
	തിരികെ;

    BMessage *m = Window()->CurrentMessage();
    പ്രതിജ്ഞാ(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    പൂർണ്ണസംഖ്യ index = 0; //  0 means here - no tab found
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < എണ്ണം_Tabs(); i++) {
	എങ്കിൽ (TabFrame(i).Contains(point)) {
	    index = i + 1; //  indexes are 1-based
	    നിർത്തുക;
	}
    }

    പൂർണ്ണസംഖ്യ event = -1;

    എങ്കിൽ ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    അല്ലെങ്കിൽ എങ്കിൽ (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = index > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    അല്ലെങ്കിൽ എങ്കിൽ (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* എടുക്കുക_Up_Menu = പുതിയ BPopUpMenu("tabLineContextMenu", അസത്യം, അസത്യം);
	എടുക്കുക_Up_Menu->AddItem(പുതിയ BMenuItem(_("Close tabi R"), പുതിയ BMessage(TABLINE_MENU_CLOSE)));
	എടുക്കുക_Up_Menu->AddItem(പുതിയ BMenuItem(_("New tab    T"), പുതിയ BMessage(TABLINE_MENU_NEW)));
	എടുക്കുക_Up_Menu->AddItem(പുതിയ BMenuItem(_("Open tab..."), പുതിയ BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = എടുക്കുക_Up_Menu->Go(point);
	എങ്കിൽ (item != ശൂന്യ) {
	    event = item->Command();
	}

	delete എടുക്കുക_Up_Menu;

    } അല്ലെങ്കിൽ {
	//  default processing
	BTabView::MouseDown(point);
	തിരികെ;
    }

    എങ്കിൽ (event < 0)
	തിരികെ;

    Vim_Tabline_Menu_സന്ദേശം tmm;
    tmm.index = index;
    tmm.event = event;
    write_port(gui.vdcmp, Vim_സന്ദേശം::TablineMenu, &tmm, വലുപ്പം(tmm));
}

ശൂന്യം
Vim_Tab_വരി::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_വരി *tab_വരി = gui.vimForm->Tab_വരി();
    എങ്കിൽ (tab_വരി != ശൂന്യ) {

	int32 i = 0;
	വേണ്ടി (; i < tab_വരി->എണ്ണം_Tabs(); i++)
	    എങ്കിൽ (ഇത് == tab_വരി->TabAt(i))
		നിർത്തുക;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	എങ്കിൽ (i < tab_വരി->എണ്ണം_Tabs()) {
	    Vim_Tabline_സന്ദേശം tm;
	    tm.index = i + 1;
	    write_port(gui.vdcmp, Vim_സന്ദേശം::Tabline, &tm, വലുപ്പം(tm));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
സ്ഥിതം അക്ഷരം appsig[] = "application/x-vnd.Haiku-Vim-8";
താക്കോൽ_map *താക്കോൽ_Map;
അക്ഷരം *താക്കോൽ_Map_Chars;
പൂർണ്ണസംഖ്യ main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    ഘടന Vim_സന്ദേശം vm;
    int32 what;
    ssize_t size;

    size = read_port_etc(gui.vdcmp, &what, &vm, വലുപ്പം(vm),
	    B_TIMEOUT, timeout);

    എങ്കിൽ (size >= 0) {
	തിരഞ്ഞെടുപ്പ് (what) {
	    അവസ്ഥ Vim_സന്ദേശം::താക്കോൽ_2:
		{
		    char_u *string = vm.u.താക്കോൽ_2.chars;
		    പൂർണ്ണസംഖ്യ len = vm.u.താക്കോൽ_2.length;
		    എങ്കിൽ (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_നിവേശം_buf();
			got_int = TRUE;
		    }

		    എങ്കിൽ (vm.u.താക്കോൽ_2.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			പൂർണ്ണസംഖ്യ	i;
			char_u	buf[2];

			വേണ്ടി (i = 0; i < len; ++i)
			{
			    add_to_നിവേശം_buf(string + i, 1);
			    എങ്കിൽ (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (പൂർണ്ണസംഖ്യ)KE_CSI;
				add_to_നിവേശം_buf(buf, 2);
			    }
			}
		    }
#അല്ലെങ്കിൽ
			add_to_നിവേശം_buf_csi(string, len);
#endif
		    അല്ലെങ്കിൽ
			add_to_നിവേശം_buf(string, len);
		}
		നിർത്തുക;
	    അവസ്ഥ Vim_സന്ദേശം::Resize:
		gui_resize_shell(vm.u.NewSize.width, vm.u.NewSize.height);
		നിർത്തുക;
	    അവസ്ഥ Vim_സന്ദേശം::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_എണ്ണം =
			atomic_add(&vm.u.Scroll.sb->scroll_Event_എണ്ണം, -1);
		    എങ്കിൽ (old_എണ്ണം <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.മൂല്യം, vm.u.Scroll.stillDragging);
		}
		നിർത്തുക;
#എങ്കിൽ defined(FEAT_MENU)
	    അവസ്ഥ Vim_സന്ദേശം::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		നിർത്തുക;
#endif
	    അവസ്ഥ Vim_സന്ദേശം::Mouse:
		{
		    int32 old_എണ്ണം;
		    എങ്കിൽ (vm.u.Mouse.button == MOUSE_DRAG)
			old_എണ്ണം =
			    atomic_add(&gui.vimTextArea->mouse_Drag_Event_എണ്ണം, -1);
		    അല്ലെങ്കിൽ
			old_എണ്ണം = 0;
		    എങ്കിൽ (old_എണ്ണം <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		നിർത്തുക;
	    അവസ്ഥ Vim_സന്ദേശം::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		നിർത്തുക;
	    അവസ്ഥ Vim_സന്ദേശം::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		എങ്കിൽ (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		നിർത്തുക;
	    അവസ്ഥ Vim_സന്ദേശം::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		നിർത്തുക;
	    അവസ്ഥ Vim_സന്ദേശം::Tabline:
		send_tabline_event(vm.u.Tabline.index);
		നിർത്തുക;
	    അവസ്ഥ Vim_സന്ദേശം::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.index, vm.u.TablineMenu.event);
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		//  unrecognised message, ignore it
		നിർത്തുക;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    തിരികെ size;
}

/*
 * Here are some functions to protect access to ScreenLines[] and
 * LineOffset[]. These are used from the window thread to respond
 * to a Draw() callback. When that occurs, the window is already
 * locked by the system.
 *
 * Other code that needs to lock is any code that changes these
 * variables. Other read-only access, or access merely to the
 * contents of the screen buffer, need not be locked.
 *
 * If there is no window, don't call Lock() but do succeed.
 */

    പൂർണ്ണസംഖ്യ
vim_lock_screen()
{
    തിരികെ !gui.vimWindow || gui.vimWindow->Lock();
}

    ശൂന്യം
vim_unlock_screen()
{
    എങ്കിൽ (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#എങ്കിൽ RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(ശൂന്യം *args)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    തിരികെ 0;
}

#അല്ലെങ്കിൽ

    int32
call_main(ശൂന്യം *args)
{
    ഘടന MainArgs *ma = (MainArgs *)args;

    തിരികെ പ്രധാന(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ശൂന്യം
gui_mch_prepare(
	പൂർണ്ണസംഖ്യ	*argc,
	അക്ഷരം	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    എങ്കിൽ (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(ശൂന്യ), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_എണ്ണം, "vim VDCMP");

#എങ്കിൽ RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, ശൂന്യ);
	എങ്കിൽ (tid >= B_OK) {
	    resume_thread(tid);
	} അല്ലെങ്കിൽ {
	    getout(1);
	}
#അല്ലെങ്കിൽ
	MainArgs ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	എങ്കിൽ (tid >= B_OK) {
	    VimApp app(appsig);

	    gui.vimApp = &app;
	    resume_thread(tid);
	    /*
	     * This is rather horrible.
	     * call_main will call main() again...
	     * There will be no infinite recursion since
	     * gui.vimApp is set now.
	     */
	    app.Run();		    // Run until Quit() called
	    // fprintf(stderr, "app.Run() returned...\n");
	    status_t dummy_exitcode;
	    (ശൂന്യം)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    exit(main_exitcode);
	}
#endif
    }
    // Don't fork() when starting the GUI. Spawned threads are not
    // duplicated with a fork(). The result is a mess.
    gui.dofork = FALSE;
    /*
     * XXX Try to determine whether we were started from
     * the Tracker or the terminal.
     * It would be nice to have this work, because the Tracker
     * follows symlinks, so even if you double-click on gvim,
     * when it is a link to vim it will still pass a command name
     * of vim...
     * We try here to see if stdin comes from /dev/null. If so,
     * (or if there is an error, which should never happen) start the GUI.
     * This does the wrong thing for vim - </dev/null, and we're
     * too early to see the command line parsing. Tough.
     * On the other hand, it starts the gui for vim file & which is nice.
     */
    എങ്കിൽ (!isatty(0)) {
	ഘടന stat stat_stdin, stat_dev_null;

	എങ്കിൽ (fstat(0, &stat_stdin) == -1 ||
		stat("/dev/null", &stat_dev_null) == -1 ||
		(stat_stdin.st_dev == stat_dev_null.st_dev &&
		 stat_stdin.st_ino == stat_dev_null.st_ino))
	    gui.starting = TRUE;
    }
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_init_check(ശൂന്യം)
{
    തിരികെ OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (പൂർണ്ണസംഖ്യ) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (പൂർണ്ണസംഖ്യ) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    എങ്കിൽ (gui.vdcmp < B_OK)
	തിരികെ FAIL;
    get_താക്കോൽ_map(&താക്കോൽ_Map, &താക്കോൽ_Map_Chars);

    gui.vimWindow = പുതിയ VimWindow();	// hidden and locked
    എങ്കിൽ (!gui.vimWindow)
	തിരികെ FAIL;

    gui.vimWindow->Run();	// Run() unlocks but does not show

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    set_normal_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();	    // re-init colors and fonts

    gui_mch_new_colors();	// window must exist for this

    തിരികെ OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    ശൂന്യം
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    പൂർണ്ണസംഖ്യ
gui_mch_open()
{
    എങ്കിൽ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	തിരികെ OK;
    }

    തിരികെ FAIL;
}

    ശൂന്യം
gui_mch_exit(പൂർണ്ണസംഖ്യ vim_exitcode)
{
    എങ്കിൽ (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#എങ്കിൽ !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    എങ്കിൽ (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(അസത്യം);

	main_exitcode = vim_exitcode;
#എങ്കിൽ RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#അല്ലെങ്കിൽ
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	// suicide
	exit_thread(vim_exitcode);
#endif
    }
    // If we are somehow still here, let mch_exit() handle things.
}

/*
 * Get the position of the top left corner of the window.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_get_winpos(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (പൂർണ്ണസംഖ്യ)r.ഇടത്;
	*y = (പൂർണ്ണസംഖ്യ)r.മുകൾ;
	തിരികെ OK;
    }
    അല്ലെങ്കിൽ
	തിരികെ FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ശൂന്യം
gui_mch_set_winpos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
ശൂന്യം
gui_mch_set_shellsize(
	പൂർണ്ണസംഖ്യ	width,
	പൂർണ്ണസംഖ്യ	height,
	പൂർണ്ണസംഖ്യ	min_width,
	പൂർണ്ണസംഖ്യ	min_height,
	പൂർണ്ണസംഖ്യ	base_width,
	പൂർണ്ണസംഖ്യ	base_height,
	പൂർണ്ണസംഖ്യ	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	ദശാംശം minWidth, maxWidth, minHeight, maxHeight;

	gui.vimWindow->GetSizeLimits(&minWidth, &maxWidth,
		&minHeight, &maxHeight);
	gui.vimWindow->SetSizeLimits(min_width, maxWidth,
		min_height, maxHeight);

	/*
	 * Set the resizing alignment depending on font size.
	 */
	gui.vimWindow->SetWindowAlignment(
		B_PIXEL_ALIGNMENT,	//  window_alignment mode,
		1,		//  int32 h,
		0,		//  int32 hOffset = 0,
		gui.char_width,	    //	int32 width = 0,
		base_width,	    //	int32 widthOffset = 0,
		1,		//  int32 v = 0,
		0,		//  int32 vOffset = 0,
		gui.char_height,	//  int32 height = 0,
		base_height	    //	int32 heightOffset = 0
		);

	gui.vimWindow->Unlock();
    }
}

ശൂന്യം
gui_mch_get_screen_dimensions(
	പൂർണ്ണസംഖ്യ	*screen_w,
	പൂർണ്ണസംഖ്യ	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	എങ്കിൽ (screen.IsValid()) {
	    frame = screen.Frame();
	} അല്ലെങ്കിൽ {
	    frame.വലത് = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (പൂർണ്ണസംഖ്യ) frame.വലത് - 2 * gui.scrollbar_width - 20;
    *screen_h = (പൂർണ്ണസംഖ്യ) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

ശൂന്യം
gui_mch_set_text_area_pos(
	പൂർണ്ണസംഖ്യ	x,
	പൂർണ്ണസംഖ്യ	y,
	പൂർണ്ണസംഖ്യ	w,
	പൂർണ്ണസംഖ്യ	h)
{
    എങ്കിൽ (!gui.vimTextArea)
	തിരികെ;

    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	എങ്കിൽ (gui.vimForm->Tab_വരി() != ശൂന്യ) {
	    gui.vimForm->Tab_വരി()->ResizeTo(w, gui.vimForm->TablineHeight());
	}
#endif // FEAT_GUI_TABLINE

	gui.vimWindow->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

ശൂന്യം
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	പൂർണ്ണസംഖ്യ	flag)
{
    VimScrollBar *vsb = sb->id;
    എങ്കിൽ (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	എങ്കിൽ (flag) {
	    എങ്കിൽ (vsb->IsHidden()) {
		vsb->Show();
	    }
	} അല്ലെങ്കിൽ {
	    എങ്കിൽ (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

ശൂന്യം
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	പൂർണ്ണസംഖ്യ	val,
	പൂർണ്ണസംഖ്യ	size,
	പൂർണ്ണസംഖ്യ	max)
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	എങ്കിൽ (max == 0) {
	    s->Set_മൂല്യം(0);
	    s->SetRange(0.0, 0.0);
	} അല്ലെങ്കിൽ {
	    s->SetProportion((ദശാംശം)size / (max + 1.0));
	    s->SetSteps(1.0, size > 5 ? size - 2 : size);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - size;
#endif
	    എങ്കിൽ (max < s->മൂല്യം_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->Set_മൂല്യം(val);
		s->SetRange(0.0, max);
	    } അല്ലെങ്കിൽ {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->Set_മൂല്യം(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

ശൂന്യം
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	പൂർണ്ണസംഖ്യ	x,
	പൂർണ്ണസംഖ്യ	y,
	പൂർണ്ണസംഖ്യ	w,
	പൂർണ്ണസംഖ്യ	h)
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	ദശാംശം vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	എങ്കിൽ (winb.വലത്-(x+w)<w) vsbx = winb.വലത് - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    പൂർണ്ണസംഖ്യ
gui_mch_get_scrollbar_xpadding(ശൂന്യം)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    തിരികെ 0;
}

    പൂർണ്ണസംഖ്യ
gui_mch_get_scrollbar_ypadding(ശൂന്യം)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    തിരികെ 0;
}

ശൂന്യം
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	പൂർണ്ണസംഖ്യ	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = പുതിയ VimScrollBar(sb, posture);
    എങ്കിൽ (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#എങ്കിൽ defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
ശൂന്യം
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    പൂർണ്ണസംഖ്യ
gui_mch_is_blink_off(ശൂന്യം)
{
    തിരികെ FALSE;
}

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON blinking, cursor is shown
 */

#define BLINK_NONE  0
#define BLINK_OFF   1
#define BLINK_ON    2

സ്ഥിതം പൂർണ്ണസംഖ്യ	blink_state = BLINK_NONE;
സ്ഥിതം long_u	    blink_waittime = 700;
സ്ഥിതം long_u	    blink_ontime = 400;
സ്ഥിതം long_u	    blink_offtime = 250;
സ്ഥിതം പൂർണ്ണസംഖ്യ  blink_timer = 0;

ശൂന്യം
gui_mch_set_blinking(
	long	waittime,
	long	on,
	long	off)
{
    // TODO
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ശൂന്യം
gui_mch_stop_blink(പൂർണ്ണസംഖ്യ may_call_gui_update_cursor)
{
    // TODO
    എങ്കിൽ (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    എങ്കിൽ (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ശൂന്യം
gui_mch_start_blink()
{
    // TODO
    എങ്കിൽ (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    എങ്കിൽ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = 1; // XtAppAddTimeOut(app_context, blink_waittime,
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

/*
 * Initialise vim to use the font with the given name.	Return FAIL if the font
 * could not be loaded, OK otherwise.
 */
പൂർണ്ണസംഖ്യ
gui_mch_init_font(
	char_u	    *font_name,
	പൂർണ്ണസംഖ്യ	    fontset)
{
    എങ്കിൽ (gui.vimWindow->Lock())
    {
	പൂർണ്ണസംഖ്യ rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	തിരികെ rc;
    }

    തിരികെ FAIL;
}


    പൂർണ്ണസംഖ്യ
gui_mch_adjust_charsize()
{
    തിരികെ FAIL;
}


    പൂർണ്ണസംഖ്യ
gui_mch_font_dialog(font_family* family, font_style* style, ദശാംശം* size)
{
#എങ്കിൽ defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = പുതിയ VimSelectFontDialog(family, style, size);
    തർക്കം ret = dialog->Go();
	delete dialog;
	തിരികെ ret;
#അല്ലെങ്കിൽ
    തിരികെ NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	പൂർണ്ണസംഖ്യ	    giveErrorIfMissing)
{
    സ്ഥിതം VimFont *fontList = ശൂന്യ;

    എങ്കിൽ (!gui.in_use)	//  can't do this when GUI not running
	തിരികെ NOFONT;

    //	storage for locally modified name;
    സ്ഥിരം പൂർണ്ണസംഖ്യ buff_size = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 20;
    സ്ഥിതം അക്ഷരം font_name[buff_size] = {0};
    font_family family = {0};
    font_style	style  = {0};
    ദശാംശം size = 0.f;

    എങ്കിൽ (name == 0 && be_fixed_font == 0) {
	എങ്കിൽ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	തിരികെ NOFONT;
    }

    തർക്കം useSelectGUI = അസത്യം;
    എങ്കിൽ (name != ശൂന്യ)
	എങ്കിൽ (STRCMP(name, "*") == 0) {
	    useSelectGUI = സത്യം;
	    STRNCPY(font_name, hl_get_font_name(), buff_size);
	} അല്ലെങ്കിൽ
	    STRNCPY(font_name, name, buff_size);

    എങ്കിൽ (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	size = be_fixed_font->Size();
	vim_snprintf(font_name, buff_size,
	    (അക്ഷരം*)"%s/%s/%.0f", family, style, size);
    }

    //	replace underscores with spaces
    അക്ഷരം* end = 0;
    ആകുമ്പോൾ (end = strchr((അക്ഷരം *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    സ്ഥിതം അക്ഷരം buff[buff_size] = {0};
    STRNCPY(buff, font_name, buff_size);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_LENGTH);
    അക്ഷരം* style_s = strtok(0, "/\0");
    എങ്കിൽ (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_LENGTH);
    size = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    എങ്കിൽ (useSelectGUI) {
	എങ്കിൽ (gui_mch_font_dialog(&family, &style, &size) == NOFONT)
	    തിരികെ FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_size,
		(അക്ഷരം*)"%s/%s/%.0f", family, style, size);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	എങ്കിൽ (new_p_guifont != ശൂന്യ) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    വേണ്ടി ( ; *new_p_guifont; ++new_p_guifont)
		എങ്കിൽ (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    വേണ്ടി (flp = fontList; flp; flp = flp->next) {
	എങ്കിൽ (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    തിരികെ (GuiFont)flp;
	}
    }

    VimFont *font = പുതിയ VimFont();
    font->name = vim_strsave((char_u*)font_name);

    എങ്കിൽ (എണ്ണം_font_styles(family) <= 0) {
	എങ്കിൽ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	തിരികെ NOFONT;
    }

    //	Remember font in the static list for later use
    font->next = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    എങ്കിൽ (size > 0.f)
	font->SetSize(size);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    തിരികെ (GuiFont)font;
}

/*
 * Set the current text font.
 */
ശൂന്യം
gui_mch_set_font(
	GuiFont	font)
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (പൂർണ്ണസംഖ്യ) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (പൂർണ്ണസംഖ്യ)(fh.ascent + 0.9999)
	    + (പൂർണ്ണസംഖ്യ)(fh.descent + 0.9999) + (പൂർണ്ണസംഖ്യ)(fh.leading + 0.9999);
	gui.char_ascent = (പൂർണ്ണസംഖ്യ)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
ശൂന്യം
gui_mch_free_font(
	GuiFont	font)
{
    എങ്കിൽ (font == NOFONT)
	തിരികെ;
    VimFont *f = (VimFont *)font;
    എങ്കിൽ (--f->refcount <= 0) {
	എങ്കിൽ (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    പൂർണ്ണസംഖ്യ
gui_mch_adjust_charheight()
{

    // TODO: linespace support?

// #ifdef FEAT_XFONTSET
//  if (gui.fontset != NOFONTSET)
//  {
//  gui.char_height = fontset_height((XFontSet)gui.fontset) + p_linespace;
//  gui.char_ascent = fontset_ascent((XFontSet)gui.fontset)
//  + p_linespace / 2;
//  }
//  else
// #endif
    {
	VimFont *font = (VimFont *)gui.norm_font;
	font_height fh = {0};
	font->GetHeight(&fh);
	gui.char_height = (പൂർണ്ണസംഖ്യ)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (പൂർണ്ണസംഖ്യ)(fh.ascent + 0.5) + p_linespace / 2;
    }
    തിരികെ OK;
}

    ശൂന്യം
gui_mch_getmouse(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    fprintf(stderr, "gui_mch_getmouse");

    /*int	rootx, rooty, winx, winy;
      Window	root, child;
      unsigned int mask;

      if (gui.wid && XQueryPointer(gui.dpy, gui.wid, &root, &child,
      &rootx, &rooty, &winx, &winy, &mask)) {
     *x = winx;
     *y = winy;
     } else*/ {
	 *x = -1;
	 *y = -1;
     }
}

    ശൂന്യം
gui_mch_mousehide(പൂർണ്ണസംഖ്യ hide)
{
    fprintf(stderr, "gui_mch_getmouse");
    //	TODO
}

/*
 * This function has been lifted from gui_w32.c and extended a bit.
 *
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
guicolor_T
gui_mch_get_color(
	char_u	*name)
{
    തിരികെ gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
ശൂന്യം
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_മുകളിലെ_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text background color.
 */
ശൂന്യം
gui_mch_set_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_താഴെയുള്ള_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    ശൂന്യം
gui_mch_set_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

ശൂന്യം
gui_mch_draw_string(
	പൂർണ്ണസംഖ്യ	row,
	പൂർണ്ണസംഖ്യ	col,
	char_u	*s,
	പൂർണ്ണസംഖ്യ	len,
	പൂർണ്ണസംഖ്യ	flags)
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(പൂർണ്ണസംഖ്യ r, പൂർണ്ണസംഖ്യ g, പൂർണ്ണസംഖ്യ b)
{
    തിരികെ gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
പൂർണ്ണസംഖ്യ
gui_mch_haskey(
	char_u	*name)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; special_താക്കോലുകൾ[i].Be_താക്കോലുകൾ != 0; i++)
	എങ്കിൽ (name[0] == special_താക്കോലുകൾ[i].vim_code0 &&
		name[1] == special_താക്കോലുകൾ[i].vim_code1)
	    തിരികെ OK;
    തിരികെ FAIL;
}

    ശൂന്യം
gui_mch_beep()
{
    ::beep();
}

    ശൂന്യം
gui_mch_flash(പൂർണ്ണസംഖ്യ msec)
{
    // Do a visual beep by reversing the foreground and background colors

    എങ്കിൽ (gui.vimWindow->Lock()) {
	BRect rect = gui.vimTextArea->Bounds();

	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->Sync();
	snooze(msec * 1000);	 // wait for a few msec
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimTextArea->Flush();
	gui.vimWindow->Unlock();
    }
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
ശൂന്യം
gui_mch_invert_rectangle(
	പൂർണ്ണസംഖ്യ	r,
	പൂർണ്ണസംഖ്യ	c,
	പൂർണ്ണസംഖ്യ	nr,
	പൂർണ്ണസംഖ്യ	nc)
{
    BRect rect;
    rect.ഇടത് = FILL_X(c);
    rect.മുകൾ = FILL_Y(r);
    rect.വലത് = rect.ഇടത് + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.മുകൾ + nr * gui.char_height - PEN_WIDTH;

    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimWindow->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    ശൂന്യം
gui_mch_iconify()
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(സത്യം);
	gui.vimWindow->Unlock();
    }
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ശൂന്യം
gui_mch_set_foreground(ശൂന്യം)
{
    // TODO
}
#endif

/*
 * Set the window title
 */
ശൂന്യം
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((അക്ഷരം *)title);
	gui.vimWindow->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    ശൂന്യം
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.ഇടത് = FILL_X(gui.col);
    r.മുകൾ = FILL_Y(gui.row);
    പൂർണ്ണസംഖ്യ cells = utf_off2cells(വരി_Offset[gui.row] + gui.col, 100); // TODO-TODO
    എങ്കിൽ (cells>=4) cells = 1;
    r.വലത് = r.ഇടത് + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.മുകൾ + gui.char_height - PEN_WIDTH;

    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimTextArea->StrokeRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
ശൂന്യം
gui_mch_draw_part_cursor(
	പൂർണ്ണസംഖ്യ	w,
	പൂർണ്ണസംഖ്യ	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.ഇടത് =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_വലത് ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.വലത് = r.ഇടത് + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.മുകൾ = r.bottom - h + PEN_WIDTH;

    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimTextArea->FillRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    ശൂന്യം
gui_mch_update()
{
    gui_mch_flush();
    ആകുമ്പോൾ (port_എണ്ണം(gui.vdcmp) > 0 &&
	    !vim_is_നിവേശം_buf_full() &&
	    gui_haiku_process_event(0) >= B_OK)
	/* nothing */ ;
}

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1	    Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
പൂർണ്ണസംഖ്യ
gui_mch_wait_for_chars(
	long	wtime)
{
    പൂർണ്ണസംഖ്യ		focus;
    bigtime_t	until, timeout;
    status_t	st;

    എങ്കിൽ (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    അല്ലെങ്കിൽ
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    വേണ്ടി (;;)
    {
	// Stop or start blinking when focus changes
	എങ്കിൽ (gui.in_focus != focus)
	{
	    എങ്കിൽ (gui.in_focus)
		gui_mch_start_blink();
	    അല്ലെങ്കിൽ
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	gui_mch_flush();

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	എങ്കിൽ (did_add_timer)
	    // Need to recompute the waiting time.
	    നിർത്തുക;
# endif
# ifdef FEAT_JOB_CHANNEL
	എങ്കിൽ (has_any_channel())
	{
	    എങ്കിൽ (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (wtime < 0)
	    timeout = B_INFINITE_TIMEOUT;
# endif
#endif

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use gui_haiku_process_event() to hang until
	 * an event arrives.  No need to check for input_buf_full because we
	 * are returning as soon as it contains a single char.
	 */
	st = gui_haiku_process_event(timeout);

	എങ്കിൽ (നിവേശം_available())
	    തിരികെ OK;
	എങ്കിൽ (st < B_OK)		// includes B_TIMED_OUT
	    തിരികെ FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	എങ്കിൽ (wtime >= 0)
	{
	    timeout = until - system_time();
	    എങ്കിൽ (timeout < 0)
		നിർത്തുക;
	}
    }
    തിരികെ FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    ശൂന്യം
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    തിരികെ;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
ശൂന്യം
gui_mch_clear_block(
	പൂർണ്ണസംഖ്യ	row1,
	പൂർണ്ണസംഖ്യ	col1,
	പൂർണ്ണസംഖ്യ	row2,
	പൂർണ്ണസംഖ്യ	col2)
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    ശൂന്യം
gui_mch_clear_all()
{
    എങ്കിൽ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
ശൂന്യം
gui_mch_delete_വരികൾ(
	പൂർണ്ണസംഖ്യ	row,
	പൂർണ്ണസംഖ്യ	അക്കം_വരികൾ)
{
    gui.vimTextArea->mch_Delete_വരികൾ(row, അക്കം_വരികൾ);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
ശൂന്യം
gui_mch_തിരുകുക_വരികൾ(
	പൂർണ്ണസംഖ്യ	row,
	പൂർണ്ണസംഖ്യ	അക്കം_വരികൾ)
{
    gui.vimTextArea->mch_തിരുകുക_വരികൾ(row, അക്കം_വരികൾ);
}

#എങ്കിൽ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ശൂന്യം
gui_mch_enable_menu(
	പൂർണ്ണസംഖ്യ	flag)
{
    എങ്കിൽ (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

ശൂന്യം
gui_mch_set_menu_pos(
	പൂർണ്ണസംഖ്യ	x,
	പൂർണ്ണസംഖ്യ	y,
	പൂർണ്ണസംഖ്യ	w,
	പൂർണ്ണസംഖ്യ	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
ശൂന്യം
gui_mch_add_menu(
	vimmenu_T   *menu,
	പൂർണ്ണസംഖ്യ	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    എങ്കിൽ (menu_is_popup(menu->name) && parent == ശൂന്യ) {
	BPopUpMenu* എടുക്കുക_Up_Menu = പുതിയ BPopUpMenu((സ്ഥിരം അക്ഷരം*)menu->name, അസത്യം, അസത്യം);
	menu->submenu_id = എടുക്കുക_Up_Menu;
	menu->id = ശൂന്യ;
	തിരികെ;
    }

    എങ്കിൽ (!menu_is_menubar(menu->name)
	    || (parent != ശൂന്യ && parent->submenu_id == ശൂന്യ))
	തിരികെ;

    എങ്കിൽ (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	എങ്കിൽ ( parent )
	    tmp = parent->submenu_id;
	അല്ലെങ്കിൽ
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	എങ്കിൽ ( ! tmp->FindItem((സ്ഥിരം അക്ഷരം *) menu->dname)) {

	    BMenu *bmenu = പുതിയ BMenu((അക്ഷരം *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((സ്ഥിരം അക്ഷരം *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    ശൂന്യം
gui_mch_toggle_tearoffs(പൂർണ്ണസംഖ്യ enable)
{
    // no tearoff menus
}

    സ്ഥിതം BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = പുതിയ BMessage('menu');
    m->AddPointer("VimMenu", (ശൂന്യം *)menu);

    തിരികെ m;
}

/*
 * Add a menu item to a menu
 */
ശൂന്യം
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	പൂർണ്ണസംഖ്യ	idx)
{
    പൂർണ്ണസംഖ്യ	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    എങ്കിൽ (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	എങ്കിൽ (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    എങ്കിൽ (toolbar != ശൂന്യ) {
		toolbar->AddButton(idx, menu);
	    }
	} അല്ലെങ്കിൽ
#endif

	എങ്കിൽ (parent->submenu_id != ശൂന്യ || menu_is_popup(parent->name)) {
	    എങ്കിൽ (menu_is_separator(menu->name)) {
		BSeparatorItem *item = പുതിയ BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ {
		BMenuItem *item = പുതിയ BMenuItem((അക്ഷരം *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((അക്ഷരം) menu->mnemonic);
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = ശൂന്യ;
	    }
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
ശൂന്യം
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    എങ്കിൽ (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	എങ്കിൽ (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    എങ്കിൽ (toolbar != ശൂന്യ) {
		toolbar->RemoveButton(menu);
	    }
	} അല്ലെങ്കിൽ
#endif
	{
	    പ്രതിജ്ഞാ(menu->submenu_id == ശൂന്യ || menu->submenu_id->എണ്ണം_ഇനങ്ങൾ() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    എങ്കിൽ (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		എങ്കിൽ (bmenu == gui.vimForm->MenuBar() && bmenu->എണ്ണം_ഇനങ്ങൾ() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = ശൂന്യ;
	    menu->submenu_id = ശൂന്യ;

	    gui.menu_height = (പൂർണ്ണസംഖ്യ) gui.vimForm->MenuHeight();
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
ശൂന്യം
gui_mch_menu_grey(
	vimmenu_T   *menu,
	പൂർണ്ണസംഖ്യ	grey)
{
#ifdef FEAT_TOOLBAR
    എങ്കിൽ (menu->parent && menu_is_toolbar(menu->parent->name)) {
	എങ്കിൽ (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    എങ്കിൽ (toolbar != ശൂന്യ) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (menu->id != ശൂന്യ)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
ശൂന്യം
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	പൂർണ്ണസംഖ്യ	hidden)
{
    എങ്കിൽ (menu->id != ശൂന്യ)
	menu->id->SetEnabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ശൂന്യം
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    ശൂന്യം
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    എങ്കിൽ (!menu_is_popup(menu->name) || menu->submenu_id == ശൂന്യ)
	തിരികെ;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    എങ്കിൽ (popupMenu == ശൂന്യ)
	തിരികെ;

    BPoint point;
    എങ്കിൽ (gui.vimWindow->Lock()) {
	uint32 buttons = 0;
	gui.vimTextArea->GetMouse(&point, &buttons);
	gui.vimTextArea->ConvertToScreen(&point);
	gui.vimWindow->Unlock();
    }
    popupMenu->Go(point, സത്യം);
}

#endif // FEAT_MENU

// Mouse stuff

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
അക്ഷരം textplain[] = "text/plain";
അക്ഷരം vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    ശൂന്യം
clip_mch_request_selection(Clipboard_T *cbd)
{
    എങ്കിൽ (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->Data();
	// m->PrintToStream();

	char_u *string = ശൂന്യ;
	ssize_t stringlen = -1;

	എങ്കിൽ (m->FindData(textplain, B_MIME_TYPE,
		    (സ്ഥിരം ശൂന്യം **)&string, &stringlen) == B_OK
		|| m->FindString("text", (സ്ഥിരം അക്ഷരം **)&string) == B_OK)
	{
	    എങ്കിൽ (stringlen == -1)
		stringlen = STRLEN(string);

	    പൂർണ്ണസംഖ്യ type;
	    അക്ഷരം *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    എങ്കിൽ (m->FindData(vimselectiontype, B_MIME_TYPE,
			(സ്ഥിരം ശൂന്യം **)&seltype, &seltypelen) == B_OK)
	    {
		തിരഞ്ഞെടുപ്പ് (*seltype)
		{
		    സ്ഥിരസ്ഥിതി:
		    അവസ്ഥ 'L':	type = MLINE;	നിർത്തുക;
		    അവസ്ഥ 'C':	type = MCHAR;	നിർത്തുക;
#ifdef FEAT_VISUAL
		    അവസ്ഥ 'B':	type = MBLOCK;	നിർത്തുക;
#endif
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		// Otherwise use heuristic as documented
		type = memchr(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (long)stringlen, cbd);
	}
	be_clipboard->Unlock();
    }
}
/*
 * Make vim the owner of the current selection.
 */
    ശൂന്യം
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    പൂർണ്ണസംഖ്യ
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    തിരികെ FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    ശൂന്യം
clip_mch_set_selection(Clipboard_T *cbd)
{
    എങ്കിൽ (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->Data();
	പ്രതിജ്ഞാ(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = ശൂന്യ;
	long_u	എണ്ണം;
	പൂർണ്ണസംഖ്യ type;

	type = clip_convert_selection(&str, &എണ്ണം, cbd);

	എങ്കിൽ (type < 0)
	    തിരികെ;

	m->AddData(textplain, B_MIME_TYPE, (ശൂന്യം *)str, എണ്ണം);

	// Add type of selection
	അക്ഷരം	vtype;
	തിരഞ്ഞെടുപ്പ് (type)
	{
	    സ്ഥിരസ്ഥിതി:
	    അവസ്ഥ MLINE:    vtype = 'L';    നിർത്തുക;
	    അവസ്ഥ MCHAR:    vtype = 'C';    നിർത്തുക;
#ifdef FEAT_VISUAL
	    അവസ്ഥ MBLOCK:   vtype = 'B';    നിർത്തുക;
#endif
	}
	m->AddData(vimselectiontype, B_MIME_TYPE, (ശൂന്യം *)&vtype, 1);

	vim_free(str);

	be_clipboard->Commit();
	be_clipboard->Unlock();
    }
}

#endif	// FEAT_CLIPBOARD

#ifdef FEAT_BROWSE
/*
 * Pop open a file browser and return the file selected, in allocated memory,
 * or NULL if Cancel is hit.
 *  saving  - TRUE if the file will be saved to, FALSE if it will be opened.
 *  title   - Title message for the file browser dialog.
 *  dflt    - Default name of file.
 *  ext     - Default extension to be added to files without extensions.
 *  initdir - directory in which to open the browser (NULL = current dir)
 *  filter  - Filter for matched files to choose from.
 *  Has a format like this:
 *  "C Files (*.c)\0*.c\0"
 *  "All Files\0*.*\0\0"
 *  If these two strings were concatenated, then a choice of two file
 *  filters will be selectable to the user.  Then only matching files will
 *  be shown in the browser.  If NULL, the default allows all files.
 *
 *  *NOTE* - the filter string must be terminated with TWO nulls.
 */
char_u *
gui_mch_browse(
	പൂർണ്ണസംഖ്യ saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = പുതിയ BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    ശൂന്യ, ശൂന്യ, 0, അസത്യം,
	    പുതിയ BMessage((saving == TRUE) ? 'save' : 'open'), ശൂന്യ, സത്യം);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((അക്ഷരം*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((സ്ഥിരം അക്ഷരം*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    ആകുമ്പോൾ (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = ശൂന്യ;
    status_t result = gui.vimApp->fBrowsedPath.InitCheck();
    എങ്കിൽ (result == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } അല്ലെങ്കിൽ
	എങ്കിൽ (result != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    result, strerror(result));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = ശൂന്യ;

    തിരികെ fileName;
}
#endif // FEAT_BROWSE


#എങ്കിൽ defined(FEAT_GUI_DIALOG)

/*
 * Create a dialog dynamically from the parameter strings.
 * type	    = type of dialog (question, alert, etc.)
 * title    = dialog title. may be NULL for default title.
 * message  = text to display. Dialog sizes to accommodate it.
 * buttons  = '\n' separated list of button captions, default first.
 * dfltbutton	= number of default button.
 *
 * This routine returns 1 if the first button is pressed,
 *	    2 for the second, etc.
 *
 *	    0 indicates Esc was pressed.
 *	    -1 for unexpected error
 *
 * If stubbing out this fn, return 1.
 */

പൂർണ്ണസംഖ്യ
gui_mch_dialog(
	പൂർണ്ണസംഖ്യ	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	പൂർണ്ണസംഖ്യ	 dfltbutton,
	char_u	*textfield,
	പൂർണ്ണസംഖ്യ ex_cmd)
{
    VimDialog *dialog = പുതിയ VimDialog(type, (അക്ഷരം*)title, (അക്ഷരം*)message,
	    (അക്ഷരം*)buttons, dfltbutton, (അക്ഷരം*)textfield, ex_cmd);
    തർക്കം ret = dialog->Go();
    delete dialog;
	തിരികെ ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    തിരികെ ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    ശൂന്യം
gui_mch_setmouse(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    ശൂന്യം
im_set_position(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    എങ്കിൽ (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    തിരികെ;
}
#endif

    ശൂന്യം
gui_mch_show_toolbar(പൂർണ്ണസംഖ്യ showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    ശൂന്യം
gui_mch_set_toolbar_pos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    എങ്കിൽ (toolbar != ശൂന്യ) {
	എങ്കിൽ (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#എങ്കിൽ defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    ശൂന്യം
gui_mch_show_tabline(പൂർണ്ണസംഖ്യ showit)
{
    Vim_Tab_വരി *tab_വരി = gui.vimForm->Tab_വരി();

    എങ്കിൽ (tab_വരി == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (!showit != !gui.vimForm->Is_Showing_Tab_വരി()) {
	gui.vimForm->Set_Showing_Tab_വരി(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    ശൂന്യം
gui_mch_set_tabline_pos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h)
{
    Vim_Tab_വരി *tab_വരി = gui.vimForm->Tab_വരി();
    എങ്കിൽ (tab_വരി != ശൂന്യ) {
	എങ്കിൽ (gui.vimWindow->Lock()) {
	    tab_വരി->MoveTo(x, y);
	    tab_വരി->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_showing_tabline()
{
    Vim_Tab_വരി *tab_വരി = gui.vimForm->Tab_വരി();
    തിരികെ tab_വരി != ശൂന്യ && gui.vimForm->Is_Showing_Tab_വരി();
}

/*
 * Update the labels of the tabline.
 */
    ശൂന്യം
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    പൂർണ്ണസംഖ്യ	    nr = 0;
    പൂർണ്ണസംഖ്യ	    curtabidx = 0;

    Vim_Tab_വരി *tab_വരി = gui.vimForm->Tab_വരി();

    എങ്കിൽ (tab_വരി == ശൂന്യ)
	തിരികെ;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    വേണ്ടി (tp = first_tabpage; tp != ശൂന്യ; tp = tp->tp_next, ++nr) {
	എങ്കിൽ (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_വരി->TabAt(nr);

	എങ്കിൽ (tab == ശൂന്യ) {
	    tab = പുതിയ Vim_Tab_വരി::VimTab();
	    tab_വരി->AddTab(ശൂന്യ, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((സ്ഥിരം അക്ഷരം*)NameBuff);
	tab_വരി->Invalidate();
    }

    // Remove any old labels.
    ആകുമ്പോൾ (nr < tab_വരി->എണ്ണം_Tabs())
	tab_വരി->RemoveTab(nr);

    എങ്കിൽ (tab_വരി->Selection() != curtabidx)
	tab_വരി->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ശൂന്യം
gui_mch_set_curtab(പൂർണ്ണസംഖ്യ nr)
{
    Vim_Tab_വരി *tab_വരി = gui.vimForm->Tab_വരി();
    എങ്കിൽ (tab_വരി == ശൂന്യ)
	തിരികെ;

    gui.vimWindow->Lock();

    എങ്കിൽ (tab_വരി->Selection() != nr -1)
	tab_വരി->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
