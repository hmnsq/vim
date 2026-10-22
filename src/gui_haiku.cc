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

خارجي "C" {

#ضمن <assert.h>
#ضمن <float.h>
#ضمن <syslog.h>

#ضمن "vim.h"
#ضمن "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#ضمن <Application.h>
#ضمن <Beep.h>
#ضمن <Bitmap.h>
#ضمن <Box.h>
#ضمن <Button.h>
#ضمن <Clipboard.h>
#ضمن <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#ضمن <File.h>
#ضمن <FilePanel.h>
#ضمن <FindDirectory.h>
//#include <Font.h>
#ضمن <IconUtils.h>
#ضمن <Input.h>
#ضمن <ListView.h>
#ضمن <MenuBar.h>
#ضمن <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#ضمن <Path.h>
#ضمن <PictureButton.h>
#ضمن <PopUpMenu.h>
//#include <Region.h>
#ضمن <Resources.h>
//#include <Roster.h>
#ضمن <Screen.h>
#ضمن <ScrollBar.h>
#ضمن <ScrollView.h>
#ضمن <String.h>
#ضمن <StringView.h>
//#include <SupportDefs.h>
#ضمن <TabView.h>
#ضمن <TextControl.h>
#ضمن <TextView.h>
#ضمن <TranslationUtils.h>
#ضمن <TranslatorFormats.h>
#ضمن <View.h>
#ضمن <Window.h>

صنف VimApp;
صنف VimFormView;
صنف VimTextAreaView;
صنف VimWindow;
صنف VimToolbar;
صنف Vim_Tab_لي;

خارجي مبودي_map *مبودي_Map;
خارجي حرف *مبودي_Map_Chars;

خارجي صحيح main(صحيح argc, حرف **argv);

#ifndef B_MAX_PORT_كدي
#define B_MAX_PORT_كدي    255
#endif

// VimApp seems comparable to the X "vimShell"
صنف VimApp: عام BApplication
{
    تعريفنوع BApplication Inherited;
    عام:
    VimApp(ثابت حرف *appsig);
    ~VimApp();

    // callbacks:
#إذا 0
    virtual فراغ DispatchMessage(BMessage *m, BHandler *h)
    {
	m->بغ_To_Stream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    virtual فراغ ReadyToRun();
    virtual فراغ ArgvReceived(int32 argc, حرف **argv);
    virtual فراغ RefsReceived(BMessage *m);
    virtual منطقي QuitRequested();
    virtual فراغ MessageReceived(BMessage *m);

    ساكن فراغ SendRefs(BMessage *m, منطقي changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    خاص:
};

صنف VimWindow: عام BWindow
{
    تعريفنوع BWindow Inherited;
    عام:
    VimWindow();
    ~VimWindow();

    virtual فراغ DispatchMessage(BMessage *m, BHandler *h);
    virtual فراغ WindowActivated(منطقي active);
    virtual منطقي QuitRequested();

    VimFormView	    *formView;

    خاص:
    فراغ init();

    منطقي is_fullscreen = خطأ;
    BRect saved_frame;
    window_look saved_look;
};

صنف VimFormView: عام BView
{
    تعريفنوع BView Inherited;
    عام:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    virtual فراغ AllAttached();
    virtual فراغ FrameResized(عائم new_width, عائم new_height);

#define MENUBAR_MARGIN	1
    عائم MenuHeight() ثابت
    { أرجع menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ثابت
    { أرجع menuBar; }

    خاص:
    فراغ init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    عام:
    عائم ToolbarHeight() ثابت;
    VimToolbar *ToolBar() ثابت
    { أرجع toolBar; }
    خاص:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    عام:
    Vim_Tab_لي *Tab_لي() ثابت	{ أرجع tab_لي; }
    منطقي Is_Showing_Tab_لي() ثابت { أرجع showing_Tab_لي; }
    فراغ Set_Showing_Tab_لي(منطقي showing) { showing_Tab_لي = showing;	}
    عائم TablineHeight() ثابت;
    خاص:
    Vim_Tab_لي	*tab_لي;
    صحيح	showing_Tab_لي;
#endif
};

صنف VimTextAreaView: عام BView
{
    تعريفنوع BView Inherited;
    عام:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    virtual فراغ Draw(BRect updateRect);
    virtual فراغ مبودي_Down(ثابت حرف *bytes, int32 لمب_Bytes);
    virtual فراغ MouseDown(BPoint point);
    virtual فراغ MouseUp(BPoint point);
    virtual فراغ MouseMoved(BPoint point, uint32 transit, ثابت BMessage *message);
    virtual فراغ MessageReceived(BMessage *m);

    // own functions:
    صحيح mchInitFont(char_u *name);
    فراغ mchDrawString(صحيح row, صحيح col, char_u *s, صحيح len, صحيح flags);
    فراغ mchClearBlock(صحيح row1, صحيح col1, صحيح row2, صحيح col2);
    فراغ mchClearAll();
    فراغ mch_Delete_ليوك(صحيح row, صحيح لمب_ليوك);
    فراغ mch_سك_ليوك(صحيح row, صحيح لمب_ليوك);

    ساكن فراغ guiSendMouseEvent(صحيح button, صحيح x, صحيح y, صحيح repeated_click, int_u modifiers);
    ساكن فراغ guiMouseMoved(صحيح x, صحيح y);
    ساكن فراغ guiBlankMouse(منطقي should_hide);
    ساكن int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouse_Drag_Event_كدي;

#ifdef FEAT_MBYTE_IME
    فراغ DrawIMString(فراغ);
#endif

    خاص:
    فراغ init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    بنية {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	صحيح row;
	صحيح col;
	صحيح كدي;
    } IMData;
#endif
};

صنف VimScrollBar: عام BScrollBar
{
    تعريفنوع BScrollBar Inherited;
    عام:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    virtual فراغ درج_Changed(عائم new_درج);
    virtual فراغ MouseUp(BPoint where);
    فراغ Set_درج(عائم newval);
    scrollbar_T *getGsb()
    { أرجع gsb; }

    int32	scroll_Event_كدي;

    خاص:
    scrollbar_T *gsb;
    عائم   ignore_درج;
};


#ifdef FEAT_TOOLBAR

صنف VimToolbar : عام BBox
{
    ساكن BBitmap *normalButtonsBitmap;
    ساكن BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(ثابت حرف* fileName);
    منطقي GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, منطقي pressed);
    منطقي ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    فراغ InvalidateLayout();

    عام:
    VimToolbar(BRect frame, ثابت حرف * name);
    ~VimToolbar();

    منطقي PrepareButtonBitmaps();

    منطقي AddButton(int32 index, vimmenu_T *menu);
    منطقي RemoveButton(vimmenu_T *menu);
    منطقي GrayButton(vimmenu_T *menu, صحيح grey);

    عائم ToolbarHeight() ثابت;
    virtual فراغ AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = عدم;
BBitmap *VimToolbar::grayedButtonsBitmap  = عدم;

ثابت عائم ToolbarMargin = 3.;
ثابت عائم ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

صنف Vim_Tab_لي : عام BTabView
{
    عام:
	صنف VimTab : عام BTab {
	    عام:
		VimTab() : BTab(جديد BView(BRect(), "-Empty-", 0, 0)) {}

	    virtual فراغ Select(BView* owner);
	};

	Vim_Tab_لي(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_هغو | B_FOLLOW_كولي | B_FOLLOW_دام, B_WILL_DRAW | B_FRAME_EVENTS) {}

    عائم TablineHeight() ثابت;
    virtual فراغ MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
صنف VimFont: عام BFont
{
    تعريفنوع BFont Inherited;
    عام:
    VimFont();
    VimFont(ثابت VimFont *rhs);
    VimFont(ثابت BFont *rhs);
    VimFont(ثابت VimFont &rhs);
    ~VimFont();

    VimFont *next;
    صحيح refcount;
    char_u *name;

    خاص:
    فراغ init();
};

#إذا defined(FEAT_GUI_DIALOG)

صنف VimDialog : عام BWindow
{
    تعريفنوع BWindow Inherited;

    BButton* _CreateButton(int32 which, ثابت حرف* label);

    عام:

    صنف View : عام BView {
	تعريفنوع BView Inherited;

	عام:
	View(BRect frame);
	~View();

	virtual فراغ Draw(BRect updateRect);
	فراغ InitIcon(int32 type);

	خاص:
	BBitmap*    fIconBitmap;
    };

    VimDialog(صحيح type, ثابت حرف *title, ثابت حرف *message,
	    ثابت حرف *buttons, صحيح dfltbutton, ثابت حرف *textfield,
	    صحيح ex_cmd);
    ~VimDialog();

    صحيح Go();

    virtual فراغ MessageReceived(BMessage *سكو);

    خاص:
    sem_id	    fDialogSem;
    صحيح		    f_Dialog_درج;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   f_شغرو_Control;
    ثابت حرف*	    f_شغرو_درج;
};

صنف VimSelectFontDialog : عام BWindow
{
    تعريفنوع BWindow Inherited;

    فراغ _CleanList(BListView* list);
    فراغ _UpdateFontStyles();
    فراغ Update_Size_شغرو_Preview();
    فراغ _UpdateFontPreview();
    منطقي _UpdateFromListItem(BListView* list, حرف* text, صحيح textSize);
    عام:

    VimSelectFontDialog(font_family* family, font_style* style, عائم* size);
    ~VimSelectFontDialog();

    منطقي Go();

    virtual فراغ MessageReceived(BMessage *سكو);

    خاص:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    منطقي	    f_Dialog_درج;
    font_family*    fFamily;
    font_style*	    fStyle;
    عائم*	    fSize;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    عائم	    fFontSize;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_شغرو;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

بنية Main_حجوجي {
    صحيح	     argc;
    حرف    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	مبودي_سكو_BUFSIZ	7
#إذا مبودي_سكو_BUFSIZ < MAX_مبودي_CODE_LEN
#error Increase مبودي_سكو_BUFSIZ!
#endif

بنية Vim_مبودي_سكو {
    char_u  length;
    char_u  chars[مبودي_سكو_BUFSIZ];  // contains Vim encoding
    منطقي    csi_escape;
};

بنية Vim_Resize_سكو {
    صحيح	    width;
    صحيح	    height;
};

بنية Vim_Scroll_Bar_سكو {
    VimScrollBar *sb;
    long    درج;
    صحيح	    stillDragging;
};

بنية Vim_Menu_سكو {
    vimmenu_T	*guiMenu;
};

بنية Vim_Mouse_سكو {
    صحيح	    button;
    صحيح	    x;
    صحيح	    y;
    صحيح	    repeated_click;
    int_u   modifiers;
};

بنية Vim_Mouse_Moved_سكو {
    صحيح	    x;
    صحيح	    y;
};

بنية Vim_Focus_سكو {
    منطقي    active;
};

بنية Vim_Refs_سكو {
    BMessage   *message;
    منطقي    changedir;
};

بنية Vim_Tabline_سكو {
    صحيح	    index;
};

بنية Vim_Tabline_Menu_سكو {
    صحيح	    index;
    صحيح	    event;
};

بنية Vim_سكو {
    تعداد Vim_سكو_Type {
	مبودي_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    اتحاد {
	بنية Vim_مبودي_سكو    مبودي_2;
	بنية Vim_Resize_سكو NewSize;
	بنية Vim_Scroll_Bar_سكو	Scroll;
	بنية Vim_Menu_سكو   Menu;
	بنية Vim_Mouse_سكو  Mouse;
	بنية Vim_Mouse_Moved_سكو	MouseMoved;
	بنية Vim_Focus_سكو  Focus;
	بنية Vim_Refs_سكو   Refs;
	بنية Vim_Tabline_سكو	Tabline;
	بنية Vim_Tabline_Menu_سكو    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ساكن بنية specialkey
{
    uint16  Be_مبوداي;
#define مبودي_3(a,b)    ((a)<<8|(b))
#define K(a)	    مبودي_3(0,a)		// for ASCII codes
#define F(b)	    مبودي_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_مبوداي[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_هغو_ARROW),	    'k', 'l'},
    {K(B_دام_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_سك),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_مبودي  11
    {F(B_F1_مبودي),	'k', '1'},
    {F(B_F2_مبودي),	'k', '2'},
    {F(B_F3_مبودي),	'k', '3'},
    {F(B_F4_مبودي),	'k', '4'},
    {F(B_F5_مبودي),	'k', '5'},
    {F(B_F6_مبودي),	'k', '6'},
    {F(B_F7_مبودي),	'k', '7'},
    {F(B_F8_مبودي),	'k', '8'},
    {F(B_F9_مبودي),	'k', '9'},
    {F(B_F10_مبودي),	'k', ';'},

    {F(B_F11_مبودي),	'F', '1'},
    {F(B_F12_مبودي),	'F', '2'},
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
    {F(B_بغ_مبودي),	    '%', '9'},

#إذا 0
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

#define لمب_SPECIAL_مبوداي    ARRAY_LENGTH(special_مبوداي)

// ---------------- VimApp ----------------

    ساكن فراغ
docd(BPath &path)
{
    mch_chdir((حرف *)path.Path());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	ساكن فراغ
drop_callback(فراغ *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	ساكن فراغ
RefsReceived(BMessage *m, منطقي changedir)
{
    uint32 type;
    int32 كدي;

    m->بغ_To_Stream();
    بدل (m->what) {
	حالة B_REFS_RECEIVED:
	حالة B_SIMPLE_DATA:
	    m->GetInfo("refs", &type, &كدي);
	    إذا (type != B_REF_TYPE)
		اذهب bad;
	    اكسر;
	حالة B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &كدي);
	    إذا (type != B_STRING_TYPE)
		اذهب bad;
	    إذا (changedir) {
		حرف *dirname;
		إذا (m->FindString("cwd", (ثابت حرف **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    اكسر;
	افتراضي:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    أرجع;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(كدي * حجم(char_u *));
    صحيح fname_index = 0;

    بدل (m->what) {
	حالة B_REFS_RECEIVED:
	حالة B_SIMPLE_DATA:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    لكل (صحيح i = 0; i < كدي; ++i)
	    {
		entry_ref ref;
		إذا (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, خطأ);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    إذا (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    إذا (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			إذا (!changedir)
			    docd(path);
		    } وإلا {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			إذا (fname == عدم)
			    fname = (char_u *)path.Path();
			fnames[fname_index++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = خطأ;
		}
	    }
	    اكسر;
	حالة B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    لكل (صحيح i = 1; i < كدي; ++i)
	    {
		حرف *fname;

		إذا (m->FindString("argv", i, (ثابت حرف **) &fname) == B_OK) {
		    fnames[fname_index++] = vim_strsave((char_u *)fname);
		}
	    }
	    اكسر;
	افتراضي:
	    // fprintf(stderr, "case default\n");
	    اكسر;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    إذا (fname_index > 0) {
	handle_drop(fname_index, fnames, FALSE, drop_callback, عدم);

	setcursor();
	out_flush();
    } وإلا {
	vim_free(fnames);
    }
}

VimApp::VimApp(ثابت حرف *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(عدم)
{
}

VimApp::~VimApp()
{
}

    فراغ
VimApp::ReadyToRun()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    فراغ
VimApp::ArgvReceived(int32 arg_argc, حرف **arg_argv)
{
    إذا (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	إذا (gui.vimWindow)
	    gui.vimWindow->Minimize(خطأ);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, true);
    }
}

    فراغ
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    صحيح limit = 15;
    طالما (--limit >= 0 && (curbuf == عدم || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    إذا (gui.vimWindow)
	gui.vimWindow->Minimize(خطأ);
    DetachCurrentMessage();
    SendRefs(m, true);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    فراغ
VimApp::SendRefs(BMessage *m, منطقي changedir)
{
    Vim_Refs_سكو rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, Vim_سكو::Refs, &rm, حجم(rm));
    //	calls ::RefsReceived
}

    فراغ
VimApp::MessageReceived(BMessage *m)
{
    بدل (m->what) {
	حالة 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    اكسر;
	حالة 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    اكسر;
	حالة B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (فراغ**)&panel);
		إذا (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    اكسر;
	افتراضي:
	    Inherited::MessageReceived(m);
	    اكسر;
    }
}

    منطقي
VimApp::QuitRequested()
{
    (فراغ)Inherited::QuitRequested();
    أرجع خطأ;
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
    إذا (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = عدم;
}

    فراغ
VimWindow::init()
{
    // Attach the VimFormView
    formView = جديد VimFormView(Bounds());
    إذا (formView != عدم) {
	AddChild(formView);
    }
}

#إذا 0  //  disabled in zeta patch
    فراغ
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    بدل (m->what) {
	حالة B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		إذا (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} وإلا {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    اكسر;
	افتراضي:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    فراغ
VimWindow::WindowActivated(منطقي active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    إذا (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(true);

    بنية Vim_Focus_سكو fm;
    fm.active = active;

    write_port(gui.vdcmp, Vim_سكو::Focus, &fm, حجم(fm));
}

    منطقي
VimWindow::QuitRequested()
{
    بنية Vim_مبودي_سكو km;
    km.length = 5;
    memcpy((حرف *)km.chars, "\033:qa\r", km.length);
    km.csi_escape = خطأ;
    write_port(gui.vdcmp, Vim_سكو::مبودي_2, &km, حجم(km));
    أرجع خطأ;
}
    فراغ
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    منطقي should_propagate = true;

    بدل (m->what)
    {
	حالة B_مبودي_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		إذا (scancode == B_ENTER && (beModifiers & B_هغو_COMMAND_مبودي))
		    {
			should_propagate = خطأ;
			إذا (هذا->is_fullscreen)
			    {
				هذا->is_fullscreen = خطأ;
				ResizeTo(هذا->saved_frame.Width(), هذا->saved_frame.Height());
				MoveTo(هذا->saved_frame.هغو, هذا->saved_frame.كولي);
				SetLook(هذا->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } وإلا {
				هذا->saved_frame = Frame();
				هذا->saved_look = Look();
				هذا->is_fullscreen = true;
				BScreen s(هذا);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().هغو, s.Frame().كولي);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    إذا (should_propagate)
	Inherited::DispatchMessage(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect frame):
    BView(frame, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_FRAME_EVENTS),
    menuBar(عدم),
#ifdef FEAT_TOOLBAR
    toolBar(عدم),
#endif
#ifdef FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_لي(عدم),
#endif
    textArea(عدم)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    إذا (menuBar) {
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
    delete tab_لي;
#endif

    إذا (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = عدم;
}

    فراغ
VimFormView::init(BRect frame)
{
    menuBar = جديد BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = جديد VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tab_لي = جديد Vim_Tab_لي(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tab_لي);
#endif

    BRect remaining = frame;
    textArea = جديد VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = هذا;
}

#ifdef FEAT_TOOLBAR
    عائم
VimFormView::ToolbarHeight() ثابت
{
    أرجع toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    عائم
VimFormView::TablineHeight() ثابت
{
    أرجع (tab_لي && Is_Showing_Tab_لي()) ? tab_لي->TablineHeight() : 0.;
}
#endif

    فراغ
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    إذا (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.كولي += MenuHeight();
	menuBar->ResizeTo(remaining.دام, remaining.كولي);
	gui.menu_height = (صحيح) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.هغو, remaining.كولي);
	toolBar->ResizeTo(remaining.دام, ToolbarHeight());
	remaining.كولي += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tab_لي->MoveTo(remaining.هغو, remaining.كولي);
	tab_لي->ResizeTo(remaining.دام + 1, TablineHeight());
	remaining.كولي += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.هغو, remaining.كولي);
    }


    Inherited::AllAttached();
}

    فراغ
VimFormView::FrameResized(عائم new_width, عائم new_height)
{
    بنية Vim_Resize_سكو sm;
    صحيح adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (صحيح) new_width;
    sm.height = (صحيح) new_height;
    adjust_w = ((صحيح)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((صحيح)new_height - gui_get_base_height()) % gui.char_height;

    إذا (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, Vim_سكو::Resize, &sm, حجم(sm));
    //	calls gui_resize_shell(new_width, new_height);

    أرجع;

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
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_شغرو_METHOD_AWARE
#وإلا
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouse_Drag_Event_كدي(0)
{
#ifdef FEAT_MBYTE_IME
    IMData.messenger = عدم;
    IMData.message = عدم;
#endif
    init(frame);
}

VimTextAreaView::~VimTextAreaView()
{
    gui.vimTextArea = عدم;
}

    فراغ
VimTextAreaView::init(BRect frame)
{
    // set up global var for fast access
    gui.vimTextArea = هذا;

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

    فراغ
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
    Set_كس_Color(rgb);
    FillRect(updateRect, B_SOLID_كس);
    gui_redraw((صحيح) updateRect.هغو, (صحيح) updateRect.كولي,
	    (صحيح) (updateRect.Width() + PEN_WIDTH), (صحيح) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    Set_كس_Color(rgb);

    إذا (updateRect.هغو < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.هغو, updateRect.كولي,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_كس);
    إذا (updateRect.كولي < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.هغو, updateRect.كولي,
		    updateRect.دام, FILL_Y(0)-PEN_WIDTH), B_SOLID_كس);
    إذا (updateRect.دام >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((صحيح)Columns), updateRect.كولي,
		    updateRect.دام, updateRect.bottom), B_SOLID_كس);
    إذا (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.هغو, FILL_Y((صحيح)Rows),
		    updateRect.دام, updateRect.bottom), B_SOLID_كس);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    فراغ
VimTextAreaView::مبودي_Down(ثابت حرف *bytes, int32 لمب_Bytes)
{
    بنية Vim_مبودي_سكو km;
    char_u *dest = km.chars;

    منطقي canHaveVimModifiers = خطأ;

    BMessage *سكو = Window()->CurrentMessage();
    أكد(سكو);
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
    إذا (لمب_Bytes > 1) {
	// This cannot be a special key
	إذا (لمب_Bytes > مبودي_سكو_BUFSIZ)
	    لمب_Bytes = مبودي_سكو_BUFSIZ;	    //	should never happen... ???
	km.length = لمب_Bytes;
	memcpy((حرف *)dest, bytes, لمب_Bytes);
	km.csi_escape = true;
    } وإلا {
	int32 scancode = 0;
	سكو->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	سكو->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	صحيح len = 0;
	km.length = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	أكد(B_BACKSPACE <= 0x20);
	أكد(B_DELETE == 0x7F);
	إذا (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		لمب_Bytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    إذا (beModifiers & B_CONTROL_مبودي) {
		صحيح index = مبودي_Map->normal_map[scancode];
		صحيح new_لمب_Bytes = مبودي_Map_Chars[index];
		char_u *newBytes = (char_u *)&مبودي_Map_Chars[index + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		إذا (new_لمب_Bytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    اذهب notspecial;
		}
		bytes = (حرف *)newBytes;
	    }
	    canHaveVimModifiers = true;

	    uint16 beoskey;
	    صحيح first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    إذا (لمب_Bytes == 0 || bytes[0] == B_FUNCTION_مبودي) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_مبودي;
		last = لمب_SPECIAL_مبوداي;
	    } وإلا إذا (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } وإلا {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_مبودي;
	    }

	    لكل (صحيح i = first; i < last; i++) {
		إذا (special_مبوداي[i].Be_مبوداي == beoskey) {
		    string[0] = CSI;
		    string[1] = special_مبوداي[i].vim_code0;
		    string[2] = special_مبوداي[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	إذا (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#إذا 0
	إذا (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#وإلا
	    إذا (canHaveVimModifiers)
#endif
	    {
		صحيح modifiers;
		modifiers = 0;
		إذا (beModifiers & B_SHIFT_مبودي)
		    modifiers |= MOD_MASK_SHIFT;
		إذا (beModifiers & B_CONTROL_مبودي)
		    modifiers |= MOD_MASK_CTRL;
		إذا (beModifiers & B_OPTION_مبودي)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		صحيح مبودي;
		إذا (string[0] == CSI && len == 3)
		    مبودي = TO_SPECIAL(string[1], string[2]);
		وإلا
		    مبودي = string[0];
		مبودي = simplify_مبودي(مبودي, &modifiers);
		إذا (IS_SPECIAL(مبودي))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(مبودي);
		    string[2] = K_THIRD(مبودي);
		    len = 3;
		}
		وإلا
		{
		    string[0] = مبودي;
		    len = 1;
		}

		إذا (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.length = 3;
		}
	    }
	memcpy((حرف *)dest, string, len);
	km.length += len;
	km.csi_escape = خطأ;
    }

    write_port(gui.vdcmp, Vim_سكو::مبودي_2, &km, حجم(km));

    /*
     * blank out the pointer if necessary
     */
    إذا (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(true);
	gui.pointer_hidden = TRUE;
    }
}
فراغ
VimTextAreaView::guiSendMouseEvent(
	صحيح	button,
	صحيح	x,
	صحيح	y,
	صحيح	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_سكو mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, Vim_سكو::Mouse, &mm, حجم(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    إذا (gui.pointer_hidden)
    {
	guiBlankMouse(خطأ);
	gui.pointer_hidden = FALSE;
    }
}

فراغ
VimTextAreaView::guiMouseMoved(
	صحيح	x,
	صحيح	y)
{
    Vim_Mouse_Moved_سكو mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, Vim_سكو::MouseMoved, &mm, حجم(mm));

    إذا (gui.pointer_hidden)
    {
	guiBlankMouse(خطأ);
	gui.pointer_hidden = FALSE;
    }
}

    فراغ
VimTextAreaView::guiBlankMouse(منطقي should_hide)
{
    إذا (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } وإلا {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    إذا (beModifiers & B_SHIFT_مبودي)
	vim_modifiers |= MOUSE_SHIFT;
    إذا (beModifiers & B_CONTROL_مبودي)
	vim_modifiers |= MOUSE_CTRL;
    إذا (beModifiers & B_OPTION_مبودي)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    أرجع vim_modifiers;
}

    فراغ
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    أكد(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    صحيح vimButton;

    إذا (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_هغو;
    وإلا إذا (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_دام;
    وإلا إذا (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_ڟكيا;
    وإلا
	أرجع;		// Unknown button

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

    فراغ
VimTextAreaView::MouseUp(BPoint point)
{
    vimMouseButton = 0;

    BMessage *m = Window()->CurrentMessage();
    أكد(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, point.x, point.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::MouseUp(point);
}

    فراغ
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, ثابت BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    إذا (gui.pointer_hidden)
    {
	guiBlankMouse(خطأ);
	gui.pointer_hidden = FALSE;
    }

    إذا (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	أرجع;
    }

    atomic_add(&mouse_Drag_Event_كدي, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    فراغ
VimTextAreaView::MessageReceived(BMessage *m)
{
    بدل (m->what) {
	حالة 'menu':
	    {
		Vim_Menu_سكو mm;
		mm.guiMenu = عدم;  // in case no pointer in msg
		m->FindPointer("VimMenu", (فراغ **)&mm.guiMenu);
		write_port(gui.vdcmp, Vim_سكو::Menu, &mm, حجم(mm));
	    }
	    اكسر;
	حالة B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		عائم small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->Set_درج(scb->درج_2()+small*dy*3);
		scb->درج_Changed(scb->درج_2());
#إذا 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->Set_درج(scb->درج_2()+small*dy);
		scb->درج_Changed(scb->درج_2());
#endif
	    }
	    اكسر;
#ifdef FEAT_MBYTE_IME
	حالة B_شغرو_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		بدل(opcode)
		{
		    حالة B_شغرو_METHOD_STARTED:
			إذا (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = جديد BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			اكسر;
		    حالة B_شغرو_METHOD_CHANGED:
			{
			    BString str;
			    منطقي confirmed;
			    إذا (IMData.message) *(IMData.message) = *m;
			    وإلا	       IMData.message = جديد BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    إذا (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				بنية Vim_مبودي_سكو km;
				km.csi_escape = true;
				صحيح clen;
				صحيح i = 0;
				طالما (i < str.Length())
				{
				    clen = utf_ptr2len(chars+i);
				    memcpy(km.chars, chars+i, clen);
				    km.length = clen;
				    write_port(gui.vdcmp, Vim_سكو::مبودي_2, &km, حجم(km));
				    i += clen;
				}
			    }
			}
			اكسر;
		    حالة B_شغرو_METHOD_LOCATION_REQUEST:
			{
			    BMessage سكو(B_شغرو_METHOD_EVENT);
			    سكو.AddInt32("be:opcode", B_شغرو_METHOD_LOCATION_REQUEST);
			    سكو.AddPoint("be:location_reply", IMData.location);
			    سكو.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&سكو);
			}
			اكسر;
		    حالة B_شغرو_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = عدم;
			IMData.message = عدم;
			اكسر;
		}
	    }
	    // TODO: sz: break here???
#endif
	افتراضي:
	    إذا (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(خطأ);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_مبودي) != 0);
	    } وإلا {
		Inherited::MessageReceived(m);
	    }
	    اكسر;
    }
}

    صحيح
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    إذا (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	إذا (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	أرجع OK;
    }
    أرجع FAIL;
}

    فراغ
VimTextAreaView::mchDrawString(صحيح row, صحيح col, char_u *s, صحيح len, صحيح flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    إذا (!(flags & DRAW_TRANSP)) {
	صحيح cells;
	cells = 0;
	لكل (صحيح i=0; i<len; i++) {
	    صحيح cn = utf_ptr2cells((char_u *)(s+i));
	    إذا (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_كس);
    }

    BFont font;
    هذا->GetFont(&font);
    إذا (!font.IsFixed())
    {
	حرف* p = (حرف*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	صحيح cells;
	طالما ((p - (حرف*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    إذا (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    إذا (cells<4) lastpos += cells;
	    وإلا	lastpos++;
	    p += clen;
	}
    }
    وإلا
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((حرف*)s, len, where);
	إذا (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((حرف*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    إذا (flags & DRAW_UNDERL) {
	صحيح cells;
	cells = 0;
	لكل (صحيح i=0; i<len; i++) {
	    صحيح cn = utf_ptr2cells((char_u *)(s+i));
	    إذا (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	Stroke_لي(start, end);
    }
}

فراغ
VimTextAreaView::mchClearBlock(
	صحيح	row1,
	صحيح	col1,
	صحيح	row2,
	صحيح	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_كس);
}

    فراغ
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_كس);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    فراغ
VimTextAreaView::mch_Delete_ليوك(صحيح row, صحيح لمب_ليوك)
{
    BRect source, dest;
    source.هغو = FILL_X(gui.scroll_region_هغو);
    source.كولي = FILL_Y(row + لمب_ليوك);
    source.دام = FILL_X(gui.scroll_region_دام + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.هغو = FILL_X(gui.scroll_region_هغو);
    dest.كولي = FILL_Y(row);
    dest.دام = FILL_X(gui.scroll_region_دام + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - لمب_ليوك + 1) - PEN_WIDTH;

    إذا (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - لمب_ليوك + 1,
		gui.scroll_region_هغو,
		gui.scroll_region_bot, gui.scroll_region_دام);


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
    فراغ
VimTextAreaView::mch_سك_ليوك(صحيح row, صحيح لمب_ليوك)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.هغو = FILL_X(gui.scroll_region_هغو);
    source.كولي = FILL_Y(row);
    source.دام = FILL_X(gui.scroll_region_دام + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - لمب_ليوك + 1) - PEN_WIDTH;

    dest.هغو = FILL_X(gui.scroll_region_هغو);
    dest.كولي = FILL_Y(row + لمب_ليوك);
    dest.دام = FILL_X(gui.scroll_region_دام + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    إذا (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_هغو,
		row + لمب_ليوك - 1, gui.scroll_region_دام);

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
فراغ VimTextAreaView::DrawIMString(فراغ)
{
    ساكن ثابت rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ثابت حرف* s;
    صحيح len;
    BMessage* سكو = IMData.message;
    إذا (!سكو)
	أرجع;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.كدي, W_WIDTH(curwin), GUI_MON_NOCLEAR);
    منطقي confirmed = خطأ;
    سكو->FindBool("be:confirmed", &confirmed);
    إذا (confirmed)
	أرجع;
    rgb_color hcolor = سم_Color(), lcolor = كس_Color();
    سكو->FindString("be:string", &str);
    s = str.String();
    len = str.Length();
    Set_سم_Color(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    سكو->FindInt32("be:selection", 0, &sel_start);
    سكو->FindInt32("be:selection", 1, &sel_end);
    صحيح clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    لكل (صحيح i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	إذا (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	إذا (sel_start<=i && i<sel_end)
	{
	    Set_كس_Color(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	وإلا
	{
	    Set_كس_Color(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_كس);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.كدي = (صحيح)pos.y;

    Set_سم_Color(hcolor);
    Set_كس_Color(lcolor);
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
	    "vim scrollbar", (BView *)عدم,
	    0.0, 10.0, posture),
    ignore_درج(-1),
    scroll_Event_كدي(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    فراغ
VimScrollBar::درج_Changed(عائم new_درج)
{
    إذا (ignore_درج >= 0.0 && new_درج == ignore_درج) {
	ignore_درج = -1;
	أرجع;
    }
    ignore_درج = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_add(&scroll_Event_كدي, 1);

    بنية Vim_Scroll_Bar_سكو sm;

    sm.sb = هذا;
    sm.درج = (long) new_درج;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, Vim_سكو::ScrollBar, &sm, حجم(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    فراغ
VimScrollBar::MouseUp(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_add(&scroll_Event_كدي, 1);

    بنية Vim_Scroll_Bar_سكو sm;

    sm.sb = هذا;
    sm.درج = (long) درج_2();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, Vim_سكو::ScrollBar, &sm, حجم(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    فراغ
VimScrollBar::Set_درج(عائم new_درج)
{
    إذا (new_درج == درج_2())
	أرجع;

    ignore_درج = new_درج;
    Inherited::Set_درج(new_درج);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(ثابت VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ثابت BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ثابت VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    فراغ
VimFont::init()
{
    next = عدم;
    refcount = 1;
    name = عدم;
}

// ---------------- VimDialog ----------------

#إذا defined(FEAT_GUI_DIALOG)

ثابت unsigned صحيح  k_Vim_Dialog_Button_سكو = 'VMDB';
ثابت unsigned صحيح  kVimDialogIconStripeWidth = 30;
ثابت unsigned صحيح  kVimDialogButtonsSpacingX = 9;
ثابت unsigned صحيح  kVimDialogButtonsSpacingY = 4;
ثابت unsigned صحيح  kVimDialogSpacingX = 6;
ثابت unsigned صحيح  kVimDialogSpacingY = 10;
ثابت unsigned صحيح  kVimDialogMinimalWidth  = 310;
ثابت unsigned صحيح  kVimDialogMinimalHeight = 75;
ثابت BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(صحيح type, ثابت حرف *title, ثابت حرف *message,
	ثابت حرف *buttons, صحيح dfltbutton, ثابت حرف *textfield, صحيح ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_درج(dfltbutton)
    , fMessageView(عدم)
    , f_شغرو_Control(عدم)
    , f_شغرو_درج(textfield)
{
    //	master view
    VimDialog::View* view = جديد VimDialog::View(Bounds());
    إذا (view == عدم)
	أرجع;

    إذا (title == عدم)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    عائم maxButtonWidth  = 0;
    عائم maxButtonHeight = 0;
    عائم buttonsWidth	  = 0;
    عائم buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    افعل
    {
	int32 end = strButtons.FindFirst('\n');
	إذا (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	إذا (end == B_ERROR)
	    اكسر;

	strButtons.Remove(0, end + 1);
    } طالما (true);

    int32 buttons_كدي = fButtonsList.كدي_ابوبو();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttons_كدي - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttons_كدي - 1);
    عائم dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    عائم dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    منطقي vertical = (vim_strchr(p_go, GO_VERTICAL) != عدم) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    إذا (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.هغو  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.كولي   += kVimDialogSpacingY;
    rect.دام -= kVimDialogSpacingX;
    rect.bottom = rect.كولي;
    fMessageView = جديد BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_هغو | B_FOLLOW_كولي, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(خطأ);
    fMessageView->MakeSelectable(خطأ);
    fMessageView->SetWordWrap(true);
    AddChild(fMessageView);

    عائم messageHeight = fMessageView->TextHeight(0, fMessageView->كدي_ليوك());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    إذا (f_شغرو_درج != عدم) {
	rect.كولي     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	f_شغرو_Control = جديد BTextControl(rect, "_iv_", عدم, f_شغرو_درج, عدم,
		B_FOLLOW_هغو | B_FOLLOW_كولي, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_شغرو_Control->TextView()->SetText(f_شغرو_درج);
	f_شغرو_Control->TextView()->SetWordWrap(خطأ);
	AddChild(f_شغرو_Control);

	عائم width = 0.f, height = 0.f;
	f_شغرو_Control->GetPreferredSize(&width, &height);
	f_شغرو_Control->MakeFocus(true);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    عائم buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    لكل (int32 i = 0 ; i < buttons_كدي; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	إذا (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} وإلا
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	إذا (dfltbutton == i + 1) {
	    button->MakeDefault(true);
	    button->MakeFocus(f_شغرو_Control == عدم);
	}
    }
}

VimDialog::~VimDialog()
{
    إذا (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    صحيح
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    إذا (fDialogSem < B_OK) {
	Quit();
	أرجع f_Dialog_درج;
    }

    Show();

    طالما (acquire_sem(fDialogSem) == B_INTERRUPTED);

    صحيح ret_درج = f_Dialog_درج;
    إذا (f_شغرو_درج != عدم)
	vim_strncpy((char_u*)f_شغرو_درج, (char_u*)f_شغرو_Control->Text(), IOSIZE - 1);

    إذا (Lock())
	Quit();

    أرجع ret_درج;
}

فراغ VimDialog::MessageReceived(BMessage *سكو)
{
    int32 which = 0;
    إذا (سكو->what != k_Vim_Dialog_Button_سكو ||
	    سكو->FindInt32("which", &which) != B_OK)
	أرجع BWindow::MessageReceived(سكو);

    f_Dialog_درج = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ثابت حرف* label)
{
    BMessage *message = جديد BMessage(k_Vim_Dialog_Button_سكو);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = جديد BButton(rect, name.String(), label, message,
	    B_FOLLOW_دام | B_FOLLOW_BOTTOM);

    عائم width = 0.f, height = 0.f;
    button->GetPreferredSize(&width, &height);
    button->ResizeTo(width, height);

    أرجع button;
}

VimDialog::View::View(BRect frame)
    :	BView(frame, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(عدم)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    delete fIconBitmap;
}

فراغ VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.دام = kVimDialogIconStripeWidth;
    Set_سم_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    إذا (fIconBitmap == عدم)
	أرجع;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

فراغ VimDialog::View::InitIcon(int32 type)
{
    إذا (type == VIM_GENERIC)
	أرجع;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    إذا (status != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(status));
	أرجع;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    إذا (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	أرجع;
    }

    BResources resources(&file);
    إذا (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	أرجع;
    }

    ثابت حرف *name = "";
    بدل(type) {
	حالة VIM_ERROR:	    name = "stop"; اكسر;
	حالة VIM_WARNING:   name = "warn"; اكسر;
	حالة VIM_INFO:	    name = "info"; اكسر;
	حالة VIM_QUESTION:  name = "idea"; اكسر;
	افتراضي: أرجع;
    }

    int32 iconSize = 32;
    fIconBitmap = جديد BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), 0, B_RGBA32);
    إذا (fIconBitmap == عدم || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == عدم) ? "null" : strerror(fIconBitmap->InitCheck()));
	أرجع;
    }

    size_t size = 0;
    ثابت uint8* iconData = عدم;
    //	try vector icon first?
    iconData = (ثابت uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &size);
    إذا (iconData != عدم && BIconUtils::GetVectorIcon(iconData, size, fIconBitmap) == B_OK)
	أرجع;

    //	try bitmap icon now
    iconData = (ثابت uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &size);
    إذا (iconData == عدم) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = عدم;
	أرجع;
    }

    إذا (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(iconData, iconSize, iconSize, iconSize, fIconBitmap);
}

ثابت unsigned صحيح  k_Vim_Dialog_OKButton_سكو = 'FDOK';
ثابت unsigned صحيح  k_Vim_Dialog_Cancel_Button_سكو = 'FDCN';
ثابت unsigned صحيح  k_Vim_Dialog_Size_شغرو_سكو = 'SICH';
ثابت unsigned صحيح  k_Vim_Dialog_Family_Select_سكو = 'MSFM';
ثابت unsigned صحيح  k_Vim_Dialog_Style_Select_سكو = 'MSST';
ثابت unsigned صحيح  k_Vim_Dialog_Size_Select_سكو = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, عائم* size)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_درج(خطأ)
    , fFamily(family)
    , fStyle(style)
    , fSize(size)
    , fFontSize(*size)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_شغرو(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_LENGTH);
    strncpy(fFontStyle, *style, B_FONT_STYLE_LENGTH);

    //	"client" area view
    BBox *clientBox = جديد BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.هغو_كولي(), RC.هغو_كولي());

    //	at first create all controls
    fPreview = جديد BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = جديد BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = جديد BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = جديد BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *labelSize = جديد BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(labelSize);
    labelSize->ResizeToPreferred();

    fFamiliesList = جديد BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = جديد BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_هغو_دام, 0, خطأ, true);
    clientBox->AddChild(scrollFamilies);

    fStylesList= جديد BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = جديد BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_هغو_دام, 0, خطأ, true);
    clientBox->AddChild(scrollStyles);

    f_Sizes_شغرو = جديد BTextControl(rc, "inputSize", عدم, "???",
	    جديد BMessage(k_Vim_Dialog_Size_شغرو_سكو));
    clientBox->AddChild(f_Sizes_شغرو);
    f_Sizes_شغرو->ResizeToPreferred();

    fSizesList = جديد BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = جديد BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_هغو_دام, 0, خطأ, true);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = جديد BButton(rc, "buttonOK", "OK",
			جديد BMessage(k_Vim_Dialog_OKButton_سكو));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = جديد BButton(rc, "buttonCancel", "Cancel",
			جديد BMessage(k_Vim_Dialog_Cancel_Button_سكو));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    عائم لي_Height = labelFamily->Bounds().Height();
    عائم previewHeight = لي_Height * 3;
    عائم offsetYLabels = previewHeight + kVimDialogSpacingY;
    عائم offsetYLists = offsetYLabels + لي_Height + kVimDialogSpacingY / 2;
    عائم offsetYSizes = offsetYLists + f_Sizes_شغرو->Bounds().Height() + kVimDialogSpacingY / 2;
    عائم listsHeight = لي_Height * 9;
    عائم offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    عائم maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    عائم familiesWidth = labelFamily->Bounds().Width() * 5;
    عائم offsetXStyles = familiesWidth + kVimDialogSpacingX;
    عائم stylesWidth = labelStyle->Bounds().Width() * 4;
    عائم offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    عائم sizesWidth = labelSize->Bounds().Width() * 2;
    عائم maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.هغو + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.كولي + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    labelSize->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    عائم insetX = f_Sizes_شغرو->TextView()->Bounds().Width() - f_Sizes_شغرو->Bounds().Width();
    عائم insetY = f_Sizes_شغرو->TextView()->Bounds().Width() - f_Sizes_شغرو->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    f_Sizes_شغرو->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    f_Sizes_شغرو->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    f_Sizes_شغرو->ResizeTo(sizesWidth, f_Sizes_شغرو->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    صحيح selIndex = -1;
    صحيح كدي = كدي_font_families();
    لكل (صحيح i = 0; i < كدي; i++) {
	font_family family;
	إذا (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(جديد BStringItem((ثابت حرف*)family));
	    إذا (strncmp(family, fFontFamily, B_FONT_FAMILY_LENGTH) == 0)
		selIndex = i;
	}
    }

    إذا (selIndex >= 0) {
	fFamiliesList->Select(selIndex);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    selIndex = -1;
    لكل (صحيح size = 8, index = 0; size <= 18; size++, index++) {
	BString str;
	str << size;
	fSizesList->AddItem(جديد BStringItem(str));
	إذا (size == fFontSize)
	    selIndex = index;

    }

    إذا (selIndex >= 0) {
	fSizesList->Select(selIndex);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(جديد BMessage(k_Vim_Dialog_Family_Select_سكو));
    fStylesList->SetSelectionMessage(جديد BMessage(k_Vim_Dialog_Style_Select_سكو));
    fSizesList->SetSelectionMessage(جديد BMessage(k_Vim_Dialog_Size_Select_سكو));
    f_Sizes_شغرو->SetModificationMessage(جديد BMessage(k_Vim_Dialog_Size_شغرو_سكو));

    Update_Size_شغرو_Preview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    إذا (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    فراغ
VimSelectFontDialog::_CleanList(BListView* list)
{
    طالما (0 < list->كدي_ابوبو())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    منطقي
VimSelectFontDialog::Go()
{
    إذا (fStatus != B_OK) {
	Quit();
	أرجع NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    إذا (fDialogSem < B_OK) {
	Quit();
	أرجع f_Dialog_درج;
    }

    Show();

    طالما (acquire_sem(fDialogSem) == B_INTERRUPTED);

    منطقي ret_درج = f_Dialog_درج;

    إذا (Lock())
	Quit();

    أرجع ret_درج;
}


فراغ VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 selIndex = -1;
    int32 كدي = كدي_font_styles(fFontFamily);
    لكل (int32 i = 0; i < كدي; i++) {
	font_style style;
	uint32 flags = 0;
	إذا (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(جديد BStringItem((ثابت حرف*)style));
	    إذا (strncmp(style, fFontStyle, B_FONT_STYLE_LENGTH) == 0)
		selIndex = i;
	}
    }

    إذا (selIndex >= 0) {
	fStylesList->Select(selIndex);
	fStylesList->ScrollToSelection();
    } وإلا
	fStylesList->Select(0);
}


فراغ VimSelectFontDialog::Update_Size_شغرو_Preview()
{
    حرف buf[10] = {0};
    vim_snprintf(buf, حجم(buf), (حرف*)"%.0f", fFontSize);
    f_Sizes_شغرو->SetText(buf);
}


فراغ VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_SIZE);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (صحيح)fFontSize << " pt.";
    fPreview->SetText(str);
}


    منطقي
VimSelectFontDialog::_UpdateFromListItem(BListView* list, حرف* text, صحيح textSize)
{
    int32 index = list->CurrentSelection();
    إذا (index < 0)
	أرجع خطأ;
    BStringItem* item = (BStringItem*)list->ItemAt(index);
    إذا (item == عدم)
	أرجع خطأ;
    strncpy(text, item->Text(), textSize);
    أرجع true;
}


فراغ VimSelectFontDialog::MessageReceived(BMessage *سكو)
{
    بدل (سكو->what) {
	حالة k_Vim_Dialog_OKButton_سكو:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_LENGTH);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_LENGTH);
	    *fSize = fFontSize;
	    f_Dialog_درج = true;
	حالة k_Vim_Dialog_Cancel_Button_سكو:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    أرجع;
	حالة B_مبودي_UP:
	    {
		int32 مبودي = 0;
		إذا (سكو->FindInt32("raw_char", &مبودي) == B_OK
			&& مبودي == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    اكسر;

	حالة k_Vim_Dialog_Family_Select_سكو:
	    إذا (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_LENGTH)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    اكسر;
	حالة k_Vim_Dialog_Style_Select_سكو:
	    إذا (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_LENGTH))
		_UpdateFontPreview();
	    اكسر;
	حالة k_Vim_Dialog_Size_Select_سكو:
	    {
		حرف buf[10] = {0};
		إذا (_UpdateFromListItem(fSizesList, buf, حجم(buf))) {
		    عائم size = atof(buf);
		    إذا (size > 0.f) {
			fFontSize = size;
			Update_Size_شغرو_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    اكسر;
	حالة k_Vim_Dialog_Size_شغرو_سكو:
	    {
		عائم size = atof(f_Sizes_شغرو->Text());
		إذا (size > 0.f) {
		    fFontSize = size;
		    _UpdateFontPreview();
		}
	    }
	    اكسر;
	افتراضي:
	    اكسر;
    }
    أرجع BWindow::MessageReceived(سكو);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ساكن BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, ثابت حرف *name) :
    BBox(frame, name, B_FOLLOW_هغو | B_FOLLOW_كولي, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 كدي = fButtonsList.كدي_ابوبو();
    لكل (int32 i = 0; i < كدي; i++)
	delete (BPictureButton*)fButtonsList.ItemAt(i);
    fButtonsList.MakeEmpty();

    delete normalButtonsBitmap;
    delete grayedButtonsBitmap;
    normalButtonsBitmap    = عدم;
    grayedButtonsBitmap  = عدم;
}

    فراغ
VimToolbar::AttachedToWindow()
{
    BBox::AttachedToWindow();

    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    عائم
VimToolbar::ToolbarHeight() ثابت
{
    عائم size = عدم == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    أرجع size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    منطقي
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    عائم height = bitmap->Bounds().Height();
    عائم width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->BitsLength() / 4;
    لكل (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    أرجع true;
}

    منطقي
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    إذا (normalButtonsBitmap == عدم)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    إذا (normalButtonsBitmap == عدم)
	أرجع خطأ;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = جديد BBitmap(&archive);
    إذا (grayedButtonsBitmap == عدم)
	أرجع خطأ;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    أرجع true;
}

BBitmap *VimToolbar::LoadVimBitmap(ثابت حرف* fileName)
{
    BBitmap *bitmap = عدم;

    صحيح mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    إذا (runtimePath != عدم && fileName != عدم) {
	BString strPath((حرف*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    إذا (mustfree)
	vim_free(runtimePath);

    أرجع bitmap;
}

    منطقي
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, منطقي pressed)
{
    عائم size = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, size, size), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.Set_سم_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, size - 1, size - 1);
    BRect destination(source);

    source.OffsetBy(size * index, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    إذا (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	size += ButtonMargin * 2 - 1;
	view.Begin_لي_Array(4);
	view.Add_لي(BPoint(0, 0),	 BPoint(size, 0),    shadowColor);
	view.Add_لي(BPoint(size, 0),	 BPoint(size, size), shineColor);
	view.Add_لي(BPoint(size, size), BPoint(0, size),    shineColor);
	view.Add_لي(BPoint(0, size),	 BPoint(0, 0),	     shadowColor);
	view.End_لي_Array();
    }

    view.EndPicture();
    RemoveChild(&view);

    أرجع true;
}

    منطقي
VimToolbar::AddButton(int32 index, vimmenu_T *menu)
{
    BPictureButton *button = عدم;
    إذا (!menu_is_separator(menu->name)) {
	عائم size = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, size, size);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	إذا (menu->iconfile == عدم && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, true);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, خطأ);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, خطأ);
	} وإلا {

	    char_u buffer[MAXPATHL] = {0};
	    BBitmap *bitmap = عدم;

	    إذا (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, buffer, (حرف*)"png");
		bitmap = BTranslationUtils::GetBitmap((حرف*)buffer);
	    }

	    إذا (bitmap == عدم && gui_find_bitmap(menu->name, buffer, (حرف*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((حرف*)buffer);

	    إذا (bitmap == عدم)
		bitmap = جديد BBitmap(BRect(0, 0, size, size), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, true);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, خطأ);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, خطأ);

	    delete bitmap;
	}

	button = جديد BPictureButton(frame, (حرف*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    منطقي result = fButtonsList.AddItem(button, index);
    InvalidateLayout();
    أرجع result;
}

    منطقي
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    إذا (menu->button) {
	إذا (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = عدم;
	}
    }
    أرجع true;
}

    منطقي
VimToolbar::GrayButton(vimmenu_T *menu, صحيح grey)
{
    إذا (menu->button) {
	int32 index = fButtonsList.IndexOf(menu->button);
	إذا (index >= 0)
	    menu->button->SetEnabled(grey ? خطأ : true);
    }
    أرجع true;
}

    فراغ
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 كدي = fButtonsList.كدي_ابوبو();
    لكل (int32 i = 0; i < كدي; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	إذا (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} وإلا
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#إذا defined(FEAT_GUI_TABLINE)

    عائم
Vim_Tab_لي::TablineHeight() ثابت
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    أرجع TabHeight(); //  + ToolbarMargin;
}

فراغ
Vim_Tab_لي::MouseDown(BPoint point)
{
    إذا (!gui_mch_showing_tabline())
	أرجع;

    BMessage *m = Window()->CurrentMessage();
    أكد(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    صحيح index = 0; //  0 means here - no tab found
    لكل (صحيح i = 0; i < كدي_Tabs(); i++) {
	إذا (TabFrame(i).Contains(point)) {
	    index = i + 1; //  indexes are 1-based
	    اكسر;
	}
    }

    صحيح event = -1;

    إذا ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    وإلا إذا (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = index > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    وإلا إذا (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* چرو_Up_Menu = جديد BPopUpMenu("tabLineContextMenu", خطأ, خطأ);
	چرو_Up_Menu->AddItem(جديد BMenuItem(_("Close tabi R"), جديد BMessage(TABLINE_MENU_CLOSE)));
	چرو_Up_Menu->AddItem(جديد BMenuItem(_("New tab    T"), جديد BMessage(TABLINE_MENU_NEW)));
	چرو_Up_Menu->AddItem(جديد BMenuItem(_("Open tab..."), جديد BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = چرو_Up_Menu->Go(point);
	إذا (item != عدم) {
	    event = item->Command();
	}

	delete چرو_Up_Menu;

    } وإلا {
	//  default processing
	BTabView::MouseDown(point);
	أرجع;
    }

    إذا (event < 0)
	أرجع;

    Vim_Tabline_Menu_سكو tmm;
    tmm.index = index;
    tmm.event = event;
    write_port(gui.vdcmp, Vim_سكو::TablineMenu, &tmm, حجم(tmm));
}

فراغ
Vim_Tab_لي::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_لي *tab_لي = gui.vimForm->Tab_لي();
    إذا (tab_لي != عدم) {

	int32 i = 0;
	لكل (; i < tab_لي->كدي_Tabs(); i++)
	    إذا (هذا == tab_لي->TabAt(i))
		اكسر;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	إذا (i < tab_لي->كدي_Tabs()) {
	    Vim_Tabline_سكو tm;
	    tm.index = i + 1;
	    write_port(gui.vdcmp, Vim_سكو::Tabline, &tm, حجم(tm));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ساكن حرف appsig[] = "application/x-vnd.Haiku-Vim-8";
مبودي_map *مبودي_Map;
حرف *مبودي_Map_Chars;
صحيح main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    بنية Vim_سكو vm;
    int32 what;
    ssize_t size;

    size = read_port_etc(gui.vdcmp, &what, &vm, حجم(vm),
	    B_TIMEOUT, timeout);

    إذا (size >= 0) {
	بدل (what) {
	    حالة Vim_سكو::مبودي_2:
		{
		    char_u *string = vm.u.مبودي_2.chars;
		    صحيح len = vm.u.مبودي_2.length;
		    إذا (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_شغرو_buf();
			got_int = TRUE;
		    }

		    إذا (vm.u.مبودي_2.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			صحيح	i;
			char_u	buf[2];

			لكل (i = 0; i < len; ++i)
			{
			    add_to_شغرو_buf(string + i, 1);
			    إذا (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (صحيح)KE_CSI;
				add_to_شغرو_buf(buf, 2);
			    }
			}
		    }
#وإلا
			add_to_شغرو_buf_csi(string, len);
#endif
		    وإلا
			add_to_شغرو_buf(string, len);
		}
		اكسر;
	    حالة Vim_سكو::Resize:
		gui_resize_shell(vm.u.NewSize.width, vm.u.NewSize.height);
		اكسر;
	    حالة Vim_سكو::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_كدي =
			atomic_add(&vm.u.Scroll.sb->scroll_Event_كدي, -1);
		    إذا (old_كدي <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.درج, vm.u.Scroll.stillDragging);
		}
		اكسر;
#إذا defined(FEAT_MENU)
	    حالة Vim_سكو::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		اكسر;
#endif
	    حالة Vim_سكو::Mouse:
		{
		    int32 old_كدي;
		    إذا (vm.u.Mouse.button == MOUSE_DRAG)
			old_كدي =
			    atomic_add(&gui.vimTextArea->mouse_Drag_Event_كدي, -1);
		    وإلا
			old_كدي = 0;
		    إذا (old_كدي <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		اكسر;
	    حالة Vim_سكو::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		اكسر;
	    حالة Vim_سكو::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		إذا (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		اكسر;
	    حالة Vim_سكو::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		اكسر;
	    حالة Vim_سكو::Tabline:
		send_tabline_event(vm.u.Tabline.index);
		اكسر;
	    حالة Vim_سكو::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.index, vm.u.TablineMenu.event);
		اكسر;
	    افتراضي:
		//  unrecognised message, ignore it
		اكسر;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    أرجع size;
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

    صحيح
vim_lock_screen()
{
    أرجع !gui.vimWindow || gui.vimWindow->Lock();
}

    فراغ
vim_unlock_screen()
{
    إذا (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#إذا RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(فراغ *حجوجي)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    أرجع 0;
}

#وإلا

    int32
call_main(فراغ *حجوجي)
{
    بنية Main_حجوجي *ma = (Main_حجوجي *)حجوجي;

    أرجع main(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    فراغ
gui_mch_prepare(
	صحيح	*argc,
	حرف	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    إذا (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(عدم), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_كدي, "vim VDCMP");

#إذا RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, عدم);
	إذا (tid >= B_OK) {
	    resume_thread(tid);
	} وإلا {
	    getout(1);
	}
#وإلا
	Main_حجوجي ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	إذا (tid >= B_OK) {
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
	    (فراغ)wait_for_thread(tid, &dummy_exitcode);

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
    إذا (!isatty(0)) {
	بنية stat stat_stdin, stat_dev_null;

	إذا (fstat(0, &stat_stdin) == -1 ||
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
    صحيح
gui_mch_init_check(فراغ)
{
    أرجع OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    صحيح
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (صحيح) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (صحيح) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    إذا (gui.vdcmp < B_OK)
	أرجع FAIL;
    get_مبودي_map(&مبودي_Map, &مبودي_Map_Chars);

    gui.vimWindow = جديد VimWindow();	// hidden and locked
    إذا (!gui.vimWindow)
	أرجع FAIL;

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

    أرجع OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    فراغ
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    إذا (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open()
{
    إذا (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    إذا (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	أرجع OK;
    }

    أرجع FAIL;
}

    فراغ
gui_mch_exit(صحيح vim_exitcode)
{
    إذا (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#إذا !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    إذا (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(خطأ);

	main_exitcode = vim_exitcode;
#إذا RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#وإلا
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
    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    إذا (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (صحيح)r.هغو;
	*y = (صحيح)r.كولي;
	أرجع OK;
    }
    وإلا
	أرجع FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    فراغ
gui_mch_set_winpos(صحيح x, صحيح y)
{
    إذا (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
فراغ
gui_mch_set_shellsize(
	صحيح	width,
	صحيح	height,
	صحيح	min_width,
	صحيح	min_height,
	صحيح	base_width,
	صحيح	base_height,
	صحيح	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    إذا (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	عائم minWidth, maxWidth, minHeight, maxHeight;

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

فراغ
gui_mch_get_screen_dimensions(
	صحيح	*screen_w,
	صحيح	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	إذا (screen.IsValid()) {
	    frame = screen.Frame();
	} وإلا {
	    frame.دام = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (صحيح) frame.دام - 2 * gui.scrollbar_width - 20;
    *screen_h = (صحيح) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

فراغ
gui_mch_set_text_area_pos(
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    إذا (!gui.vimTextArea)
	أرجع;

    إذا (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	إذا (gui.vimForm->Tab_لي() != عدم) {
	    gui.vimForm->Tab_لي()->ResizeTo(w, gui.vimForm->TablineHeight());
	}
#endif // FEAT_GUI_TABLINE

	gui.vimWindow->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

فراغ
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	صحيح	flag)
{
    VimScrollBar *vsb = sb->id;
    إذا (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	إذا (flag) {
	    إذا (vsb->IsHidden()) {
		vsb->Show();
	    }
	} وإلا {
	    إذا (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

فراغ
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	صحيح	val,
	صحيح	size,
	صحيح	max)
{
    إذا (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	إذا (max == 0) {
	    s->Set_درج(0);
	    s->SetRange(0.0, 0.0);
	} وإلا {
	    s->SetProportion((عائم)size / (max + 1.0));
	    s->SetSteps(1.0, size > 5 ? size - 2 : size);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - size;
#endif
	    إذا (max < s->درج_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->Set_درج(val);
		s->SetRange(0.0, max);
	    } وإلا {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->Set_درج(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

فراغ
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    إذا (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	عائم vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	إذا (winb.دام-(x+w)<w) vsbx = winb.دام - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    أرجع 0;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    أرجع 0;
}

فراغ
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	صحيح	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = جديد VimScrollBar(sb, posture);
    إذا (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#إذا defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
فراغ
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    إذا (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    صحيح
gui_mch_is_blink_off(فراغ)
{
    أرجع FALSE;
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

ساكن صحيح	blink_state = BLINK_NONE;
ساكن long_u	    blink_waittime = 700;
ساكن long_u	    blink_ontime = 400;
ساكن long_u	    blink_offtime = 250;
ساكن صحيح  blink_timer = 0;

فراغ
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
    فراغ
gui_mch_stop_blink(صحيح may_call_gui_update_cursor)
{
    // TODO
    إذا (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    إذا (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    فراغ
gui_mch_start_blink()
{
    // TODO
    إذا (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
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
صحيح
gui_mch_init_font(
	char_u	    *font_name,
	صحيح	    fontset)
{
    إذا (gui.vimWindow->Lock())
    {
	صحيح rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	أرجع rc;
    }

    أرجع FAIL;
}


    صحيح
gui_mch_adjust_charsize()
{
    أرجع FAIL;
}


    صحيح
gui_mch_font_dialog(font_family* family, font_style* style, عائم* size)
{
#إذا defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = جديد VimSelectFontDialog(family, style, size);
    منطقي ret = dialog->Go();
	delete dialog;
	أرجع ret;
#وإلا
    أرجع NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	صحيح	    giveErrorIfMissing)
{
    ساكن VimFont *fontList = عدم;

    إذا (!gui.in_use)	//  can't do this when GUI not running
	أرجع NOFONT;

    //	storage for locally modified name;
    ثابت صحيح buff_size = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 20;
    ساكن حرف font_name[buff_size] = {0};
    font_family family = {0};
    font_style	style  = {0};
    عائم size = 0.f;

    إذا (name == 0 && be_fixed_font == 0) {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	أرجع NOFONT;
    }

    منطقي useSelectGUI = خطأ;
    إذا (name != عدم)
	إذا (STRCMP(name, "*") == 0) {
	    useSelectGUI = true;
	    STRNCPY(font_name, hl_get_font_name(), buff_size);
	} وإلا
	    STRNCPY(font_name, name, buff_size);

    إذا (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	size = be_fixed_font->Size();
	vim_snprintf(font_name, buff_size,
	    (حرف*)"%s/%s/%.0f", family, style, size);
    }

    //	replace underscores with spaces
    حرف* end = 0;
    طالما (end = strchr((حرف *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ساكن حرف buff[buff_size] = {0};
    STRNCPY(buff, font_name, buff_size);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_LENGTH);
    حرف* style_s = strtok(0, "/\0");
    إذا (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_LENGTH);
    size = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    إذا (useSelectGUI) {
	إذا (gui_mch_font_dialog(&family, &style, &size) == NOFONT)
	    أرجع FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_size,
		(حرف*)"%s/%s/%.0f", family, style, size);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	إذا (new_p_guifont != عدم) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    لكل ( ; *new_p_guifont; ++new_p_guifont)
		إذا (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    لكل (flp = fontList; flp; flp = flp->next) {
	إذا (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    أرجع (GuiFont)flp;
	}
    }

    VimFont *font = جديد VimFont();
    font->name = vim_strsave((char_u*)font_name);

    إذا (كدي_font_styles(family) <= 0) {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	أرجع NOFONT;
    }

    //	Remember font in the static list for later use
    font->next = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    إذا (size > 0.f)
	font->SetSize(size);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    أرجع (GuiFont)font;
}

/*
 * Set the current text font.
 */
فراغ
gui_mch_set_font(
	GuiFont	font)
{
    إذا (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (صحيح) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (صحيح)(fh.ascent + 0.9999)
	    + (صحيح)(fh.descent + 0.9999) + (صحيح)(fh.leading + 0.9999);
	gui.char_ascent = (صحيح)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
فراغ
gui_mch_free_font(
	GuiFont	font)
{
    إذا (font == NOFONT)
	أرجع;
    VimFont *f = (VimFont *)font;
    إذا (--f->refcount <= 0) {
	إذا (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    إذا (name == عدم)
	أرجع عدم;
    أرجع vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحيح
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
	gui.char_height = (صحيح)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (صحيح)(fh.ascent + 0.5) + p_linespace / 2;
    }
    أرجع OK;
}

    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
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

    فراغ
gui_mch_mousehide(صحيح hide)
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
    أرجع gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
فراغ
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    إذا (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_سم_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text background color.
 */
فراغ
gui_mch_set_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    إذا (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_كس_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    فراغ
gui_mch_set_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

فراغ
gui_mch_draw_string(
	صحيح	row,
	صحيح	col,
	char_u	*s,
	صحيح	len,
	صحيح	flags)
{
    إذا (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(صحيح r, صحيح g, صحيح b)
{
    أرجع gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
صحيح
gui_mch_haskey(
	char_u	*name)
{
    صحيح i;

    لكل (i = 0; special_مبوداي[i].Be_مبوداي != 0; i++)
	إذا (name[0] == special_مبوداي[i].vim_code0 &&
		name[1] == special_مبوداي[i].vim_code1)
	    أرجع OK;
    أرجع FAIL;
}

    فراغ
gui_mch_beep()
{
    ::beep();
}

    فراغ
gui_mch_flash(صحيح msec)
{
    // Do a visual beep by reversing the foreground and background colors

    إذا (gui.vimWindow->Lock()) {
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
فراغ
gui_mch_invert_rectangle(
	صحيح	r,
	صحيح	c,
	صحيح	nr,
	صحيح	nc)
{
    BRect rect;
    rect.هغو = FILL_X(c);
    rect.كولي = FILL_Y(r);
    rect.دام = rect.هغو + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.كولي + nr * gui.char_height - PEN_WIDTH;

    إذا (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimWindow->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify()
{
    إذا (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(true);
	gui.vimWindow->Unlock();
    }
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_set_foreground(فراغ)
{
    // TODO
}
#endif

/*
 * Set the window title
 */
فراغ
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    إذا (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((حرف *)title);
	gui.vimWindow->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.هغو = FILL_X(gui.col);
    r.كولي = FILL_Y(gui.row);
    صحيح cells = utf_off2cells(لي_Offset[gui.row] + gui.col, 100); // TODO-TODO
    إذا (cells>=4) cells = 1;
    r.دام = r.هغو + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.كولي + gui.char_height - PEN_WIDTH;

    إذا (gui.vimWindow->Lock()) {
	gui.vimTextArea->StrokeRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
فراغ
gui_mch_draw_part_cursor(
	صحيح	w,
	صحيح	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.هغو =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_دام ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.دام = r.هغو + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.كولي = r.bottom - h + PEN_WIDTH;

    إذا (gui.vimWindow->Lock()) {
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
    فراغ
gui_mch_update()
{
    gui_mch_flush();
    طالما (port_كدي(gui.vdcmp) > 0 &&
	    !vim_is_شغرو_buf_full() &&
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
صحيح
gui_mch_wait_for_chars(
	long	wtime)
{
    صحيح		focus;
    bigtime_t	until, timeout;
    status_t	st;

    إذا (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    وإلا
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    لكل (;;)
    {
	// Stop or start blinking when focus changes
	إذا (gui.in_focus != focus)
	{
	    إذا (gui.in_focus)
		gui_mch_start_blink();
	    وإلا
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
	إذا (did_add_timer)
	    // Need to recompute the waiting time.
	    اكسر;
# endif
# ifdef FEAT_JOB_CHANNEL
	إذا (has_any_channel())
	{
	    إذا (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	وإلا إذا (wtime < 0)
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

	إذا (شغرو_available())
	    أرجع OK;
	إذا (st < B_OK)		// includes B_TIMED_OUT
	    أرجع FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	إذا (wtime >= 0)
	{
	    timeout = until - system_time();
	    إذا (timeout < 0)
		اكسر;
	}
    }
    أرجع FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    فراغ
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    إذا (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    أرجع;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
فراغ
gui_mch_clear_block(
	صحيح	row1,
	صحيح	col1,
	صحيح	row2,
	صحيح	col2)
{
    إذا (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    فراغ
gui_mch_clear_all()
{
    إذا (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
فراغ
gui_mch_delete_ليوك(
	صحيح	row,
	صحيح	لمب_ليوك)
{
    gui.vimTextArea->mch_Delete_ليوك(row, لمب_ليوك);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
فراغ
gui_mch_سك_ليوك(
	صحيح	row,
	صحيح	لمب_ليوك)
{
    gui.vimTextArea->mch_سك_ليوك(row, لمب_ليوك);
}

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

فراغ
gui_mch_enable_menu(
	صحيح	flag)
{
    إذا (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

فراغ
gui_mch_set_menu_pos(
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
فراغ
gui_mch_add_menu(
	vimmenu_T   *menu,
	صحيح	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    إذا (menu_is_popup(menu->name) && parent == عدم) {
	BPopUpMenu* چرو_Up_Menu = جديد BPopUpMenu((ثابت حرف*)menu->name, خطأ, خطأ);
	menu->submenu_id = چرو_Up_Menu;
	menu->id = عدم;
	أرجع;
    }

    إذا (!menu_is_menubar(menu->name)
	    || (parent != عدم && parent->submenu_id == عدم))
	أرجع;

    إذا (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	إذا ( parent )
	    tmp = parent->submenu_id;
	وإلا
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	إذا ( ! tmp->FindItem((ثابت حرف *) menu->dname)) {

	    BMenu *bmenu = جديد BMenu((حرف *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((ثابت حرف *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    // no tearoff menus
}

    ساكن BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = جديد BMessage('menu');
    m->AddPointer("VimMenu", (فراغ *)menu);

    أرجع m;
}

/*
 * Add a menu item to a menu
 */
فراغ
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	صحيح	idx)
{
    صحيح	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    إذا (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	إذا (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->AddButton(idx, menu);
	    }
	} وإلا
#endif

	إذا (parent->submenu_id != عدم || menu_is_popup(parent->name)) {
	    إذا (menu_is_separator(menu->name)) {
		BSeparatorItem *item = جديد BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = عدم;
	    }
	    وإلا {
		BMenuItem *item = جديد BMenuItem((حرف *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((حرف) menu->mnemonic);
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = عدم;
	    }
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
فراغ
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    إذا (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	إذا (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->RemoveButton(menu);
	    }
	} وإلا
#endif
	{
	    أكد(menu->submenu_id == عدم || menu->submenu_id->كدي_ابوبو() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    إذا (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		إذا (bmenu == gui.vimForm->MenuBar() && bmenu->كدي_ابوبو() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = عدم;
	    menu->submenu_id = عدم;

	    gui.menu_height = (صحيح) gui.vimForm->MenuHeight();
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
فراغ
gui_mch_menu_grey(
	vimmenu_T   *menu,
	صحيح	grey)
{
#ifdef FEAT_TOOLBAR
    إذا (menu->parent && menu_is_toolbar(menu->parent->name)) {
	إذا (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } وإلا
#endif
    إذا (menu->id != عدم)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
فراغ
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	صحيح	hidden)
{
    إذا (menu->id != عدم)
	menu->id->SetEnabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    فراغ
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    إذا (!menu_is_popup(menu->name) || menu->submenu_id == عدم)
	أرجع;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    إذا (popupMenu == عدم)
	أرجع;

    BPoint point;
    إذا (gui.vimWindow->Lock()) {
	uint32 buttons = 0;
	gui.vimTextArea->GetMouse(&point, &buttons);
	gui.vimTextArea->ConvertToScreen(&point);
	gui.vimWindow->Unlock();
    }
    popupMenu->Go(point, true);
}

#endif // FEAT_MENU

// Mouse stuff

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
حرف textplain[] = "text/plain";
حرف vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    فراغ
clip_mch_request_selection(Clipboard_T *cbd)
{
    إذا (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->Data();
	// m->PrintToStream();

	char_u *string = عدم;
	ssize_t stringlen = -1;

	إذا (m->FindData(textplain, B_MIME_TYPE,
		    (ثابت فراغ **)&string, &stringlen) == B_OK
		|| m->FindString("text", (ثابت حرف **)&string) == B_OK)
	{
	    إذا (stringlen == -1)
		stringlen = STRLEN(string);

	    صحيح type;
	    حرف *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    إذا (m->FindData(vimselectiontype, B_MIME_TYPE,
			(ثابت فراغ **)&seltype, &seltypelen) == B_OK)
	    {
		بدل (*seltype)
		{
		    افتراضي:
		    حالة 'L':	type = MLINE;	اكسر;
		    حالة 'C':	type = MCHAR;	اكسر;
#ifdef FEAT_VISUAL
		    حالة 'B':	type = MBLOCK;	اكسر;
#endif
		}
	    }
	    وإلا
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
    فراغ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    صحيح
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    أرجع FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    فراغ
clip_mch_set_selection(Clipboard_T *cbd)
{
    إذا (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->Data();
	أكد(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = عدم;
	long_u	كدي;
	صحيح type;

	type = clip_convert_selection(&str, &كدي, cbd);

	إذا (type < 0)
	    أرجع;

	m->AddData(textplain, B_MIME_TYPE, (فراغ *)str, كدي);

	// Add type of selection
	حرف	vtype;
	بدل (type)
	{
	    افتراضي:
	    حالة MLINE:    vtype = 'L';    اكسر;
	    حالة MCHAR:    vtype = 'C';    اكسر;
#ifdef FEAT_VISUAL
	    حالة MBLOCK:   vtype = 'B';    اكسر;
#endif
	}
	m->AddData(vimselectiontype, B_MIME_TYPE, (فراغ *)&vtype, 1);

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
	صحيح saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = جديد BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    عدم, عدم, 0, خطأ,
	    جديد BMessage((saving == TRUE) ? 'save' : 'open'), عدم, true);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((حرف*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((ثابت حرف*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    طالما (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = عدم;
    status_t result = gui.vimApp->fBrowsedPath.InitCheck();
    إذا (result == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } وإلا
	إذا (result != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    result, strerror(result));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = عدم;

    أرجع fileName;
}
#endif // FEAT_BROWSE


#إذا defined(FEAT_GUI_DIALOG)

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

صحيح
gui_mch_dialog(
	صحيح	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	صحيح	 dfltbutton,
	char_u	*textfield,
	صحيح ex_cmd)
{
    VimDialog *dialog = جديد VimDialog(type, (حرف*)title, (حرف*)message,
	    (حرف*)buttons, dfltbutton, (حرف*)textfield, ex_cmd);
    منطقي ret = dialog->Go();
    delete dialog;
	أرجع ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    أرجع ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    فراغ
im_set_position(صحيح row, صحيح col)
{
    إذا (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    أرجع;
}
#endif

    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    فراغ
gui_mch_set_toolbar_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    إذا (toolbar != عدم) {
	إذا (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#إذا defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    فراغ
gui_mch_show_tabline(صحيح showit)
{
    Vim_Tab_لي *tab_لي = gui.vimForm->Tab_لي();

    إذا (tab_لي == عدم)
	أرجع;

    إذا (!showit != !gui.vimForm->Is_Showing_Tab_لي()) {
	gui.vimForm->Set_Showing_Tab_لي(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    فراغ
gui_mch_set_tabline_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    Vim_Tab_لي *tab_لي = gui.vimForm->Tab_لي();
    إذا (tab_لي != عدم) {
	إذا (gui.vimWindow->Lock()) {
	    tab_لي->MoveTo(x, y);
	    tab_لي->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحيح
gui_mch_showing_tabline()
{
    Vim_Tab_لي *tab_لي = gui.vimForm->Tab_لي();
    أرجع tab_لي != عدم && gui.vimForm->Is_Showing_Tab_لي();
}

/*
 * Update the labels of the tabline.
 */
    فراغ
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    صحيح	    nr = 0;
    صحيح	    curtabidx = 0;

    Vim_Tab_لي *tab_لي = gui.vimForm->Tab_لي();

    إذا (tab_لي == عدم)
	أرجع;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = first_tabpage; tp != عدم; tp = tp->tp_next, ++nr) {
	إذا (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_لي->TabAt(nr);

	إذا (tab == عدم) {
	    tab = جديد Vim_Tab_لي::VimTab();
	    tab_لي->AddTab(عدم, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((ثابت حرف*)NameBuff);
	tab_لي->Invalidate();
    }

    // Remove any old labels.
    طالما (nr < tab_لي->كدي_Tabs())
	tab_لي->RemoveTab(nr);

    إذا (tab_لي->Selection() != curtabidx)
	tab_لي->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_set_curtab(صحيح nr)
{
    Vim_Tab_لي *tab_لي = gui.vimForm->Tab_لي();
    إذا (tab_لي == عدم)
	أرجع;

    gui.vimWindow->Lock();

    إذا (tab_لي->Selection() != nr -1)
	tab_لي->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
