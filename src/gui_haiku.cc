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

بیرونی "C" {

#شامل <assert.h>
#شامل <float.h>
#شامل <syslog.h>

#شامل "vim.h"
#شامل "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#شامل <Application.h>
#شامل <Beep.h>
#شامل <Bitmap.h>
#شامل <Box.h>
#شامل <Button.h>
#شامل <Clipboard.h>
#شامل <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#شامل <File.h>
#شامل <FilePanel.h>
#شامل <FindDirectory.h>
//#include <Font.h>
#شامل <IconUtils.h>
#شامل <Input.h>
#شامل <ListView.h>
#شامل <MenuBar.h>
#شامل <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#شامل <Path.h>
#شامل <PictureButton.h>
#شامل <PopUpMenu.h>
//#include <Region.h>
#شامل <Resources.h>
//#include <Roster.h>
#شامل <Screen.h>
#شامل <ScrollBar.h>
#شامل <ScrollView.h>
#شامل <String.h>
#شامل <StringView.h>
//#include <SupportDefs.h>
#شامل <TabView.h>
#شامل <TextControl.h>
#شامل <TextView.h>
#شامل <TranslationUtils.h>
#شامل <TranslatorFormats.h>
#شامل <View.h>
#شامل <Window.h>

رده VimApp;
رده VimFormView;
رده VimTextAreaView;
رده VimWindow;
رده VimToolbar;
رده Vim_Tab_سطر;

بیرونی کلید_map *کلید_Map;
بیرونی نویسه *کلید_Map_Chars;

بیرونی صحیح اصلی(صحیح argc, نویسه **argv);

#ifndef B_MAX_PORT_شمار
#define B_MAX_PORT_شمار    255
#endif

// VimApp seems comparable to the X "vimShell"
رده VimApp: عمومی BApplication
{
    تعریفنوع BApplication Inherited;
    عمومی:
    VimApp(ثابت نویسه *appsig);
    ~VimApp();

    // callbacks:
#اگر 0
    مجازی تهی DispatchMessage(BMessage *m, BHandler *h)
    {
	m->PrintToStream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    مجازی تهی ReadyToRun();
    مجازی تهی ArgvReceived(int32 argc, نویسه **argv);
    مجازی تهی RefsReceived(BMessage *m);
    مجازی منطقی QuitRequested();
    مجازی تهی MessageReceived(BMessage *m);

    ایستا تهی SendRefs(BMessage *m, منطقی changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    خصوصی:
};

رده VimWindow: عمومی BWindow
{
    تعریفنوع BWindow Inherited;
    عمومی:
    VimWindow();
    ~VimWindow();

    مجازی تهی DispatchMessage(BMessage *m, BHandler *h);
    مجازی تهی WindowActivated(منطقی active);
    مجازی منطقی QuitRequested();

    VimFormView	    *formView;

    خصوصی:
    تهی init();

    منطقی is_fullscreen = نادرست;
    BRect saved_frame;
    window_look saved_look;
};

رده VimFormView: عمومی BView
{
    تعریفنوع BView Inherited;
    عمومی:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    مجازی تهی AllAttached();
    مجازی تهی FrameResized(اعشاری new_width, اعشاری new_height);

#define MENUBAR_MARGIN	1
    اعشاری MenuHeight() ثابت
    { بازگشت menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ثابت
    { بازگشت menuBar; }

    خصوصی:
    تهی init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    عمومی:
    اعشاری ToolbarHeight() ثابت;
    VimToolbar *ToolBar() ثابت
    { بازگشت toolBar; }
    خصوصی:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    عمومی:
    Vim_Tab_سطر *Tab_سطر() ثابت	{ بازگشت tab_سطر; }
    منطقی Is_Showing_Tab_سطر() ثابت { بازگشت showing_Tab_سطر; }
    تهی Set_Showing_Tab_سطر(منطقی showing) { showing_Tab_سطر = showing;	}
    اعشاری TablineHeight() ثابت;
    خصوصی:
    Vim_Tab_سطر	*tab_سطر;
    صحیح	showing_Tab_سطر;
#endif
};

رده VimTextAreaView: عمومی BView
{
    تعریفنوع BView Inherited;
    عمومی:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    مجازی تهی Draw(BRect updateRect);
    مجازی تهی کلید_Down(ثابت نویسه *bytes, int32 رقم_Bytes);
    مجازی تهی MouseDown(BPoint point);
    مجازی تهی MouseUp(BPoint point);
    مجازی تهی MouseMoved(BPoint point, uint32 transit, ثابت BMessage *message);
    مجازی تهی MessageReceived(BMessage *m);

    // own functions:
    صحیح mchInitFont(char_u *name);
    تهی mchDrawString(صحیح row, صحیح col, char_u *s, صحیح len, صحیح flags);
    تهی mchClearBlock(صحیح row1, صحیح col1, صحیح row2, صحیح col2);
    تهی mchClearAll();
    تهی mch_Delete_سطرها(صحیح row, صحیح رقم_سطرها);
    تهی mch_درج_سطرها(صحیح row, صحیح رقم_سطرها);

    ایستا تهی guiSendMouseEvent(صحیح button, صحیح x, صحیح y, صحیح repeated_click, int_u modifiers);
    ایستا تهی guiMouseMoved(صحیح x, صحیح y);
    ایستا تهی guiBlankMouse(منطقی should_hide);
    ایستا int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouse_Drag_Event_شمار;

#ifdef FEAT_MBYTE_IME
    تهی DrawIMString(تهی);
#endif

    خصوصی:
    تهی init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    ساختار {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	صحیح row;
	صحیح col;
	صحیح شمار;
    } IMData;
#endif
};

رده VimScrollBar: عمومی BScrollBar
{
    تعریفنوع BScrollBar Inherited;
    عمومی:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    مجازی تهی مقدار_Changed(اعشاری new_مقدار);
    مجازی تهی MouseUp(BPoint where);
    تهی Set_مقدار(اعشاری newval);
    scrollbar_T *getGsb()
    { بازگشت gsb; }

    int32	scroll_Event_شمار;

    خصوصی:
    scrollbar_T *gsb;
    اعشاری   ignore_مقدار;
};


#ifdef FEAT_TOOLBAR

رده VimToolbar : عمومی BBox
{
    ایستا BBitmap *normalButtonsBitmap;
    ایستا BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(ثابت نویسه* fileName);
    منطقی GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, منطقی pressed);
    منطقی ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    تهی InvalidateLayout();

    عمومی:
    VimToolbar(BRect frame, ثابت نویسه * name);
    ~VimToolbar();

    منطقی PrepareButtonBitmaps();

    منطقی AddButton(int32 index, vimmenu_T *menu);
    منطقی RemoveButton(vimmenu_T *menu);
    منطقی GrayButton(vimmenu_T *menu, صحیح grey);

    اعشاری ToolbarHeight() ثابت;
    مجازی تهی AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = پوچ;
BBitmap *VimToolbar::grayedButtonsBitmap  = پوچ;

ثابت اعشاری ToolbarMargin = 3.;
ثابت اعشاری ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

رده Vim_Tab_سطر : عمومی BTabView
{
    عمومی:
	رده VimTab : عمومی BTab {
	    عمومی:
		VimTab() : BTab(جدید BView(BRect(), "-Empty-", 0, 0)) {}

	    مجازی تهی Select(BView* owner);
	};

	Vim_Tab_سطر(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_چپ | B_FOLLOW_بالاترین | B_FOLLOW_راست, B_WILL_DRAW | B_FRAME_EVENTS) {}

    اعشاری TablineHeight() ثابت;
    مجازی تهی MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
رده VimFont: عمومی BFont
{
    تعریفنوع BFont Inherited;
    عمومی:
    VimFont();
    VimFont(ثابت VimFont *rhs);
    VimFont(ثابت BFont *rhs);
    VimFont(ثابت VimFont &rhs);
    ~VimFont();

    VimFont *next;
    صحیح refcount;
    char_u *name;

    خصوصی:
    تهی init();
};

#اگر defined(FEAT_GUI_DIALOG)

رده VimDialog : عمومی BWindow
{
    تعریفنوع BWindow Inherited;

    BButton* _CreateButton(int32 which, ثابت نویسه* label);

    عمومی:

    رده View : عمومی BView {
	تعریفنوع BView Inherited;

	عمومی:
	View(BRect frame);
	~View();

	مجازی تهی Draw(BRect updateRect);
	تهی InitIcon(int32 type);

	خصوصی:
	BBitmap*    fIconBitmap;
    };

    VimDialog(صحیح type, ثابت نویسه *title, ثابت نویسه *message,
	    ثابت نویسه *buttons, صحیح dfltbutton, ثابت نویسه *textfield,
	    صحیح ex_cmd);
    ~VimDialog();

    صحیح Go();

    مجازی تهی MessageReceived(BMessage *پیام);

    خصوصی:
    sem_id	    fDialogSem;
    صحیح		    f_Dialog_مقدار;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   f_درون_داد_Control;
    ثابت نویسه*	    f_درون_داد_مقدار;
};

رده VimSelectFontDialog : عمومی BWindow
{
    تعریفنوع BWindow Inherited;

    تهی _CleanList(BListView* list);
    تهی _UpdateFontStyles();
    تهی Update_Size_درون_داد_Preview();
    تهی _UpdateFontPreview();
    منطقی _UpdateFromListItem(BListView* list, نویسه* text, صحیح textSize);
    عمومی:

    VimSelectFontDialog(font_family* family, font_style* style, اعشاری* size);
    ~VimSelectFontDialog();

    منطقی Go();

    مجازی تهی MessageReceived(BMessage *پیام);

    خصوصی:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    منطقی	    f_Dialog_مقدار;
    font_family*    fFamily;
    font_style*	    fStyle;
    اعشاری*	    fSize;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    اعشاری	    fFontSize;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_درون_داد;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

ساختار MainArgs {
    صحیح	     argc;
    نویسه    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	کلید_پیام_BUFSIZ	7
#اگر کلید_پیام_BUFSIZ < MAX_کلید_CODE_LEN
#error Increase کلید_پیام_BUFSIZ!
#endif

ساختار Vim_کلید_پیام {
    char_u  length;
    char_u  chars[کلید_پیام_BUFSIZ];  // contains Vim encoding
    منطقی    csi_escape;
};

ساختار Vim_Resize_پیام {
    صحیح	    width;
    صحیح	    height;
};

ساختار Vim_Scroll_Bar_پیام {
    VimScrollBar *sb;
    long    مقدار;
    صحیح	    stillDragging;
};

ساختار Vim_Menu_پیام {
    vimmenu_T	*guiMenu;
};

ساختار Vim_Mouse_پیام {
    صحیح	    button;
    صحیح	    x;
    صحیح	    y;
    صحیح	    repeated_click;
    int_u   modifiers;
};

ساختار Vim_Mouse_Moved_پیام {
    صحیح	    x;
    صحیح	    y;
};

ساختار Vim_Focus_پیام {
    منطقی    active;
};

ساختار Vim_Refs_پیام {
    BMessage   *message;
    منطقی    changedir;
};

ساختار Vim_Tabline_پیام {
    صحیح	    index;
};

ساختار Vim_Tabline_Menu_پیام {
    صحیح	    index;
    صحیح	    event;
};

ساختار Vim_پیام {
    شمارش Vim_پیام_Type {
	کلید_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    اجتماع {
	ساختار Vim_کلید_پیام    کلید_2;
	ساختار Vim_Resize_پیام NewSize;
	ساختار Vim_Scroll_Bar_پیام	Scroll;
	ساختار Vim_Menu_پیام   Menu;
	ساختار Vim_Mouse_پیام  Mouse;
	ساختار Vim_Mouse_Moved_پیام	MouseMoved;
	ساختار Vim_Focus_پیام  Focus;
	ساختار Vim_Refs_پیام   Refs;
	ساختار Vim_Tabline_پیام	Tabline;
	ساختار Vim_Tabline_Menu_پیام    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ایستا ساختار specialkey
{
    uint16  Be_کلیدها;
#define کلید_3(a,b)    ((a)<<8|(b))
#define K(a)	    کلید_3(0,a)		// for ASCII codes
#define F(b)	    کلید_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیدها[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_چپ_ARROW),	    'k', 'l'},
    {K(B_راست_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_درج),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_کلید  11
    {F(B_F1_کلید),	'k', '1'},
    {F(B_F2_کلید),	'k', '2'},
    {F(B_F3_کلید),	'k', '3'},
    {F(B_F4_کلید),	'k', '4'},
    {F(B_F5_کلید),	'k', '5'},
    {F(B_F6_کلید),	'k', '6'},
    {F(B_F7_کلید),	'k', '7'},
    {F(B_F8_کلید),	'k', '8'},
    {F(B_F9_کلید),	'k', '9'},
    {F(B_F10_کلید),	'k', ';'},

    {F(B_F11_کلید),	'F', '1'},
    {F(B_F12_کلید),	'F', '2'},
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
    {F(B_PRINT_کلید),	    '%', '9'},

#اگر 0
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

#define رقم_SPECIAL_کلیدها    ARRAY_LENGTH(special_کلیدها)

// ---------------- VimApp ----------------

    ایستا تهی
docd(BPath &path)
{
    mch_chdir((نویسه *)path.Path());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	ایستا تهی
drop_callback(تهی *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	ایستا تهی
RefsReceived(BMessage *m, منطقی changedir)
{
    uint32 type;
    int32 شمار;

    m->PrintToStream();
    انتخاب (m->what) {
	حالت B_REFS_RECEIVED:
	حالت B_SIMPLE_DATA:
	    m->GetInfo("refs", &type, &شمار);
	    اگر (type != B_REF_TYPE)
		برو bad;
	    بشکن;
	حالت B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &شمار);
	    اگر (type != B_STRING_TYPE)
		برو bad;
	    اگر (changedir) {
		نویسه *dirname;
		اگر (m->FindString("cwd", (ثابت نویسه **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    بشکن;
	پیشفرض:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    بازگشت;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(شمار * اندازه(char_u *));
    صحیح fname_index = 0;

    انتخاب (m->what) {
	حالت B_REFS_RECEIVED:
	حالت B_SIMPLE_DATA:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    برای (صحیح i = 0; i < شمار; ++i)
	    {
		entry_ref ref;
		اگر (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, نادرست);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    اگر (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    اگر (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			اگر (!changedir)
			    docd(path);
		    } وگرنه {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			اگر (fname == پوچ)
			    fname = (char_u *)path.Path();
			fnames[fname_index++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = نادرست;
		}
	    }
	    بشکن;
	حالت B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    برای (صحیح i = 1; i < شمار; ++i)
	    {
		نویسه *fname;

		اگر (m->FindString("argv", i, (ثابت نویسه **) &fname) == B_OK) {
		    fnames[fname_index++] = vim_strsave((char_u *)fname);
		}
	    }
	    بشکن;
	پیشفرض:
	    // fprintf(stderr, "case default\n");
	    بشکن;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    اگر (fname_index > 0) {
	handle_drop(fname_index, fnames, FALSE, drop_callback, پوچ);

	setcursor();
	out_flush();
    } وگرنه {
	vim_free(fnames);
    }
}

VimApp::VimApp(ثابت نویسه *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(پوچ)
{
}

VimApp::~VimApp()
{
}

    تهی
VimApp::ReadyToRun()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    تهی
VimApp::ArgvReceived(int32 arg_argc, نویسه **arg_argv)
{
    اگر (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	اگر (gui.vimWindow)
	    gui.vimWindow->Minimize(نادرست);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, درست);
    }
}

    تهی
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    صحیح limit = 15;
    وقتی (--limit >= 0 && (curbuf == پوچ || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    اگر (gui.vimWindow)
	gui.vimWindow->Minimize(نادرست);
    DetachCurrentMessage();
    SendRefs(m, درست);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    تهی
VimApp::SendRefs(BMessage *m, منطقی changedir)
{
    Vim_Refs_پیام rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, Vim_پیام::Refs, &rm, اندازه(rm));
    //	calls ::RefsReceived
}

    تهی
VimApp::MessageReceived(BMessage *m)
{
    انتخاب (m->what) {
	حالت 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    بشکن;
	حالت 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    بشکن;
	حالت B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (تهی**)&panel);
		اگر (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    بشکن;
	پیشفرض:
	    Inherited::MessageReceived(m);
	    بشکن;
    }
}

    منطقی
VimApp::QuitRequested()
{
    (تهی)Inherited::QuitRequested();
    بازگشت نادرست;
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
    اگر (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = پوچ;
}

    تهی
VimWindow::init()
{
    // Attach the VimFormView
    formView = جدید VimFormView(Bounds());
    اگر (formView != پوچ) {
	AddChild(formView);
    }
}

#اگر 0  //  disabled in zeta patch
    تهی
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    انتخاب (m->what) {
	حالت B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		اگر (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} وگرنه {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    بشکن;
	پیشفرض:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    تهی
VimWindow::WindowActivated(منطقی active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    اگر (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(درست);

    ساختار Vim_Focus_پیام fm;
    fm.active = active;

    write_port(gui.vdcmp, Vim_پیام::Focus, &fm, اندازه(fm));
}

    منطقی
VimWindow::QuitRequested()
{
    ساختار Vim_کلید_پیام km;
    km.length = 5;
    memcpy((نویسه *)km.chars, "\033:qa\r", km.length);
    km.csi_escape = نادرست;
    write_port(gui.vdcmp, Vim_پیام::کلید_2, &km, اندازه(km));
    بازگشت نادرست;
}
    تهی
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    منطقی should_propagate = درست;

    انتخاب (m->what)
    {
	حالت B_کلید_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		اگر (scancode == B_ENTER && (beModifiers & B_چپ_COMMAND_کلید))
		    {
			should_propagate = نادرست;
			اگر (این->is_fullscreen)
			    {
				این->is_fullscreen = نادرست;
				ResizeTo(این->saved_frame.Width(), این->saved_frame.Height());
				MoveTo(این->saved_frame.چپ, این->saved_frame.بالاترین);
				SetLook(این->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } وگرنه {
				این->saved_frame = Frame();
				این->saved_look = Look();
				این->is_fullscreen = درست;
				BScreen s(این);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().چپ, s.Frame().بالاترین);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    اگر (should_propagate)
	Inherited::DispatchMessage(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect frame):
    BView(frame, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_FRAME_EVENTS),
    menuBar(پوچ),
#ifdef FEAT_TOOLBAR
    toolBar(پوچ),
#endif
#ifdef FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_سطر(پوچ),
#endif
    textArea(پوچ)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    اگر (menuBar) {
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
    delete tab_سطر;
#endif

    اگر (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = پوچ;
}

    تهی
VimFormView::init(BRect frame)
{
    menuBar = جدید BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = جدید VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tab_سطر = جدید Vim_Tab_سطر(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tab_سطر);
#endif

    BRect remaining = frame;
    textArea = جدید VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = این;
}

#ifdef FEAT_TOOLBAR
    اعشاری
VimFormView::ToolbarHeight() ثابت
{
    بازگشت toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    اعشاری
VimFormView::TablineHeight() ثابت
{
    بازگشت (tab_سطر && Is_Showing_Tab_سطر()) ? tab_سطر->TablineHeight() : 0.;
}
#endif

    تهی
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    اگر (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.بالاترین += MenuHeight();
	menuBar->ResizeTo(remaining.راست, remaining.بالاترین);
	gui.menu_height = (صحیح) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.چپ, remaining.بالاترین);
	toolBar->ResizeTo(remaining.راست, ToolbarHeight());
	remaining.بالاترین += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tab_سطر->MoveTo(remaining.چپ, remaining.بالاترین);
	tab_سطر->ResizeTo(remaining.راست + 1, TablineHeight());
	remaining.بالاترین += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.چپ, remaining.بالاترین);
    }


    Inherited::AllAttached();
}

    تهی
VimFormView::FrameResized(اعشاری new_width, اعشاری new_height)
{
    ساختار Vim_Resize_پیام sm;
    صحیح adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (صحیح) new_width;
    sm.height = (صحیح) new_height;
    adjust_w = ((صحیح)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((صحیح)new_height - gui_get_base_height()) % gui.char_height;

    اگر (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, Vim_پیام::Resize, &sm, اندازه(sm));
    //	calls gui_resize_shell(new_width, new_height);

    بازگشت;

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
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_درون_داد_METHOD_AWARE
#وگرنه
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouse_Drag_Event_شمار(0)
{
#ifdef FEAT_MBYTE_IME
    IMData.messenger = پوچ;
    IMData.message = پوچ;
#endif
    init(frame);
}

VimTextAreaView::~VimTextAreaView()
{
    gui.vimTextArea = پوچ;
}

    تهی
VimTextAreaView::init(BRect frame)
{
    // set up global var for fast access
    gui.vimTextArea = این;

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

    تهی
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
    Set_کم_Color(rgb);
    FillRect(updateRect, B_SOLID_کم);
    gui_redraw((صحیح) updateRect.چپ, (صحیح) updateRect.بالاترین,
	    (صحیح) (updateRect.Width() + PEN_WIDTH), (صحیح) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    Set_کم_Color(rgb);

    اگر (updateRect.چپ < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.چپ, updateRect.بالاترین,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_کم);
    اگر (updateRect.بالاترین < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.چپ, updateRect.بالاترین,
		    updateRect.راست, FILL_Y(0)-PEN_WIDTH), B_SOLID_کم);
    اگر (updateRect.راست >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((صحیح)Columns), updateRect.بالاترین,
		    updateRect.راست, updateRect.bottom), B_SOLID_کم);
    اگر (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.چپ, FILL_Y((صحیح)Rows),
		    updateRect.راست, updateRect.bottom), B_SOLID_کم);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    تهی
VimTextAreaView::کلید_Down(ثابت نویسه *bytes, int32 رقم_Bytes)
{
    ساختار Vim_کلید_پیام km;
    char_u *dest = km.chars;

    منطقی canHaveVimModifiers = نادرست;

    BMessage *پیام = Window()->CurrentMessage();
    أكد(پیام);
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
    اگر (رقم_Bytes > 1) {
	// This cannot be a special key
	اگر (رقم_Bytes > کلید_پیام_BUFSIZ)
	    رقم_Bytes = کلید_پیام_BUFSIZ;	    //	should never happen... ???
	km.length = رقم_Bytes;
	memcpy((نویسه *)dest, bytes, رقم_Bytes);
	km.csi_escape = درست;
    } وگرنه {
	int32 scancode = 0;
	پیام->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	پیام->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	صحیح len = 0;
	km.length = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	أكد(B_BACKSPACE <= 0x20);
	أكد(B_DELETE == 0x7F);
	اگر (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		رقم_Bytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    اگر (beModifiers & B_CONTROL_کلید) {
		صحیح index = کلید_Map->normal_map[scancode];
		صحیح new_رقم_Bytes = کلید_Map_Chars[index];
		char_u *newBytes = (char_u *)&کلید_Map_Chars[index + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		اگر (new_رقم_Bytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    برو notspecial;
		}
		bytes = (نویسه *)newBytes;
	    }
	    canHaveVimModifiers = درست;

	    uint16 beoskey;
	    صحیح first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    اگر (رقم_Bytes == 0 || bytes[0] == B_FUNCTION_کلید) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_کلید;
		last = رقم_SPECIAL_کلیدها;
	    } وگرنه اگر (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } وگرنه {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_کلید;
	    }

	    برای (صحیح i = first; i < last; i++) {
		اگر (special_کلیدها[i].Be_کلیدها == beoskey) {
		    string[0] = CSI;
		    string[1] = special_کلیدها[i].vim_code0;
		    string[2] = special_کلیدها[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	اگر (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#اگر 0
	اگر (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#وگرنه
	    اگر (canHaveVimModifiers)
#endif
	    {
		صحیح modifiers;
		modifiers = 0;
		اگر (beModifiers & B_SHIFT_کلید)
		    modifiers |= MOD_MASK_SHIFT;
		اگر (beModifiers & B_CONTROL_کلید)
		    modifiers |= MOD_MASK_CTRL;
		اگر (beModifiers & B_OPTION_کلید)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		صحیح کلید;
		اگر (string[0] == CSI && len == 3)
		    کلید = TO_SPECIAL(string[1], string[2]);
		وگرنه
		    کلید = string[0];
		کلید = simplify_کلید(کلید, &modifiers);
		اگر (IS_SPECIAL(کلید))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(کلید);
		    string[2] = K_THIRD(کلید);
		    len = 3;
		}
		وگرنه
		{
		    string[0] = کلید;
		    len = 1;
		}

		اگر (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.length = 3;
		}
	    }
	memcpy((نویسه *)dest, string, len);
	km.length += len;
	km.csi_escape = نادرست;
    }

    write_port(gui.vdcmp, Vim_پیام::کلید_2, &km, اندازه(km));

    /*
     * blank out the pointer if necessary
     */
    اگر (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(درست);
	gui.pointer_hidden = TRUE;
    }
}
تهی
VimTextAreaView::guiSendMouseEvent(
	صحیح	button,
	صحیح	x,
	صحیح	y,
	صحیح	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_پیام mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, Vim_پیام::Mouse, &mm, اندازه(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    اگر (gui.pointer_hidden)
    {
	guiBlankMouse(نادرست);
	gui.pointer_hidden = FALSE;
    }
}

تهی
VimTextAreaView::guiMouseMoved(
	صحیح	x,
	صحیح	y)
{
    Vim_Mouse_Moved_پیام mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, Vim_پیام::MouseMoved, &mm, اندازه(mm));

    اگر (gui.pointer_hidden)
    {
	guiBlankMouse(نادرست);
	gui.pointer_hidden = FALSE;
    }
}

    تهی
VimTextAreaView::guiBlankMouse(منطقی should_hide)
{
    اگر (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } وگرنه {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    اگر (beModifiers & B_SHIFT_کلید)
	vim_modifiers |= MOUSE_SHIFT;
    اگر (beModifiers & B_CONTROL_کلید)
	vim_modifiers |= MOUSE_CTRL;
    اگر (beModifiers & B_OPTION_کلید)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    بازگشت vim_modifiers;
}

    تهی
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    أكد(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    صحیح vimButton;

    اگر (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_چپ;
    وگرنه اگر (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_راست;
    وگرنه اگر (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_میانه;
    وگرنه
	بازگشت;		// Unknown button

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

    تهی
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

    تهی
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, ثابت BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    اگر (gui.pointer_hidden)
    {
	guiBlankMouse(نادرست);
	gui.pointer_hidden = FALSE;
    }

    اگر (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	بازگشت;
    }

    atomic_add(&mouse_Drag_Event_شمار, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    تهی
VimTextAreaView::MessageReceived(BMessage *m)
{
    انتخاب (m->what) {
	حالت 'menu':
	    {
		Vim_Menu_پیام mm;
		mm.guiMenu = پوچ;  // in case no pointer in msg
		m->FindPointer("VimMenu", (تهی **)&mm.guiMenu);
		write_port(gui.vdcmp, Vim_پیام::Menu, &mm, اندازه(mm));
	    }
	    بشکن;
	حالت B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		اعشاری small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->Set_مقدار(scb->مقدار_2()+small*dy*3);
		scb->مقدار_Changed(scb->مقدار_2());
#اگر 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->Set_مقدار(scb->مقدار_2()+small*dy);
		scb->مقدار_Changed(scb->مقدار_2());
#endif
	    }
	    بشکن;
#ifdef FEAT_MBYTE_IME
	حالت B_درون_داد_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		انتخاب(opcode)
		{
		    حالت B_درون_داد_METHOD_STARTED:
			اگر (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = جدید BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			بشکن;
		    حالت B_درون_داد_METHOD_CHANGED:
			{
			    BString str;
			    منطقی confirmed;
			    اگر (IMData.message) *(IMData.message) = *m;
			    وگرنه	       IMData.message = جدید BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    اگر (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				ساختار Vim_کلید_پیام km;
				km.csi_escape = درست;
				صحیح clen;
				صحیح i = 0;
				وقتی (i < str.Length())
				{
				    clen = utf_ptr2len(chars+i);
				    memcpy(km.chars, chars+i, clen);
				    km.length = clen;
				    write_port(gui.vdcmp, Vim_پیام::کلید_2, &km, اندازه(km));
				    i += clen;
				}
			    }
			}
			بشکن;
		    حالت B_درون_داد_METHOD_LOCATION_REQUEST:
			{
			    BMessage پیام(B_درون_داد_METHOD_EVENT);
			    پیام.AddInt32("be:opcode", B_درون_داد_METHOD_LOCATION_REQUEST);
			    پیام.AddPoint("be:location_reply", IMData.location);
			    پیام.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&پیام);
			}
			بشکن;
		    حالت B_درون_داد_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = پوچ;
			IMData.message = پوچ;
			بشکن;
		}
	    }
	    // TODO: sz: break here???
#endif
	پیشفرض:
	    اگر (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(نادرست);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_کلید) != 0);
	    } وگرنه {
		Inherited::MessageReceived(m);
	    }
	    بشکن;
    }
}

    صحیح
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    اگر (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	اگر (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	بازگشت OK;
    }
    بازگشت FAIL;
}

    تهی
VimTextAreaView::mchDrawString(صحیح row, صحیح col, char_u *s, صحیح len, صحیح flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    اگر (!(flags & DRAW_TRANSP)) {
	صحیح cells;
	cells = 0;
	برای (صحیح i=0; i<len; i++) {
	    صحیح cn = utf_ptr2cells((char_u *)(s+i));
	    اگر (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_کم);
    }

    BFont font;
    این->GetFont(&font);
    اگر (!font.IsFixed())
    {
	نویسه* p = (نویسه*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	صحیح cells;
	وقتی ((p - (نویسه*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    اگر (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    اگر (cells<4) lastpos += cells;
	    وگرنه	lastpos++;
	    p += clen;
	}
    }
    وگرنه
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((نویسه*)s, len, where);
	اگر (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((نویسه*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    اگر (flags & DRAW_UNDERL) {
	صحیح cells;
	cells = 0;
	برای (صحیح i=0; i<len; i++) {
	    صحیح cn = utf_ptr2cells((char_u *)(s+i));
	    اگر (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	Stroke_سطر(start, end);
    }
}

تهی
VimTextAreaView::mchClearBlock(
	صحیح	row1,
	صحیح	col1,
	صحیح	row2,
	صحیح	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_کم);
}

    تهی
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_کم);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    تهی
VimTextAreaView::mch_Delete_سطرها(صحیح row, صحیح رقم_سطرها)
{
    BRect source, dest;
    source.چپ = FILL_X(gui.scroll_region_چپ);
    source.بالاترین = FILL_Y(row + رقم_سطرها);
    source.راست = FILL_X(gui.scroll_region_راست + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.چپ = FILL_X(gui.scroll_region_چپ);
    dest.بالاترین = FILL_Y(row);
    dest.راست = FILL_X(gui.scroll_region_راست + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - رقم_سطرها + 1) - PEN_WIDTH;

    اگر (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - رقم_سطرها + 1,
		gui.scroll_region_چپ,
		gui.scroll_region_bot, gui.scroll_region_راست);


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
    تهی
VimTextAreaView::mch_درج_سطرها(صحیح row, صحیح رقم_سطرها)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.چپ = FILL_X(gui.scroll_region_چپ);
    source.بالاترین = FILL_Y(row);
    source.راست = FILL_X(gui.scroll_region_راست + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - رقم_سطرها + 1) - PEN_WIDTH;

    dest.چپ = FILL_X(gui.scroll_region_چپ);
    dest.بالاترین = FILL_Y(row + رقم_سطرها);
    dest.راست = FILL_X(gui.scroll_region_راست + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    اگر (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_چپ,
		row + رقم_سطرها - 1, gui.scroll_region_راست);

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
تهی VimTextAreaView::DrawIMString(تهی)
{
    ایستا ثابت rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ثابت نویسه* s;
    صحیح len;
    BMessage* پیام = IMData.message;
    اگر (!پیام)
	بازگشت;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.شمار, W_WIDTH(curwin), GUI_MON_NOCLEAR);
    منطقی confirmed = نادرست;
    پیام->FindBool("be:confirmed", &confirmed);
    اگر (confirmed)
	بازگشت;
    rgb_color hcolor = بالا_Color(), lcolor = کم_Color();
    پیام->FindString("be:string", &str);
    s = str.String();
    len = str.Length();
    Set_بالا_Color(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    پیام->FindInt32("be:selection", 0, &sel_start);
    پیام->FindInt32("be:selection", 1, &sel_end);
    صحیح clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    برای (صحیح i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	اگر (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	اگر (sel_start<=i && i<sel_end)
	{
	    Set_کم_Color(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	وگرنه
	{
	    Set_کم_Color(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_کم);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.شمار = (صحیح)pos.y;

    Set_بالا_Color(hcolor);
    Set_کم_Color(lcolor);
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
	    "vim scrollbar", (BView *)پوچ,
	    0.0, 10.0, posture),
    ignore_مقدار(-1),
    scroll_Event_شمار(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    تهی
VimScrollBar::مقدار_Changed(اعشاری new_مقدار)
{
    اگر (ignore_مقدار >= 0.0 && new_مقدار == ignore_مقدار) {
	ignore_مقدار = -1;
	بازگشت;
    }
    ignore_مقدار = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_add(&scroll_Event_شمار, 1);

    ساختار Vim_Scroll_Bar_پیام sm;

    sm.sb = این;
    sm.مقدار = (long) new_مقدار;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, Vim_پیام::ScrollBar, &sm, اندازه(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    تهی
VimScrollBar::MouseUp(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_add(&scroll_Event_شمار, 1);

    ساختار Vim_Scroll_Bar_پیام sm;

    sm.sb = این;
    sm.مقدار = (long) مقدار_2();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, Vim_پیام::ScrollBar, &sm, اندازه(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    تهی
VimScrollBar::Set_مقدار(اعشاری new_مقدار)
{
    اگر (new_مقدار == مقدار_2())
	بازگشت;

    ignore_مقدار = new_مقدار;
    Inherited::Set_مقدار(new_مقدار);
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

    تهی
VimFont::init()
{
    next = پوچ;
    refcount = 1;
    name = پوچ;
}

// ---------------- VimDialog ----------------

#اگر defined(FEAT_GUI_DIALOG)

ثابت unsigned صحیح  k_Vim_Dialog_Button_پیام = 'VMDB';
ثابت unsigned صحیح  kVimDialogIconStripeWidth = 30;
ثابت unsigned صحیح  kVimDialogButtonsSpacingX = 9;
ثابت unsigned صحیح  kVimDialogButtonsSpacingY = 4;
ثابت unsigned صحیح  kVimDialogSpacingX = 6;
ثابت unsigned صحیح  kVimDialogSpacingY = 10;
ثابت unsigned صحیح  kVimDialogMinimalWidth  = 310;
ثابت unsigned صحیح  kVimDialogMinimalHeight = 75;
ثابت BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(صحیح type, ثابت نویسه *title, ثابت نویسه *message,
	ثابت نویسه *buttons, صحیح dfltbutton, ثابت نویسه *textfield, صحیح ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_مقدار(dfltbutton)
    , fMessageView(پوچ)
    , f_درون_داد_Control(پوچ)
    , f_درون_داد_مقدار(textfield)
{
    //	master view
    VimDialog::View* view = جدید VimDialog::View(Bounds());
    اگر (view == پوچ)
	بازگشت;

    اگر (title == پوچ)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    اعشاری maxButtonWidth  = 0;
    اعشاری maxButtonHeight = 0;
    اعشاری buttonsWidth	  = 0;
    اعشاری buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    انجامبده
    {
	int32 end = strButtons.FindFirst('\n');
	اگر (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	اگر (end == B_ERROR)
	    بشکن;

	strButtons.Remove(0, end + 1);
    } وقتی (درست);

    int32 buttons_شمار = fButtonsList.شمار_اقلام();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttons_شمار - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttons_شمار - 1);
    اعشاری dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    اعشاری dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    منطقی vertical = (vim_strchr(p_go, GO_VERTICAL) != پوچ) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    اگر (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.چپ  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.بالاترین   += kVimDialogSpacingY;
    rect.راست -= kVimDialogSpacingX;
    rect.bottom = rect.بالاترین;
    fMessageView = جدید BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_چپ | B_FOLLOW_بالاترین, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(نادرست);
    fMessageView->MakeSelectable(نادرست);
    fMessageView->SetWordWrap(درست);
    AddChild(fMessageView);

    اعشاری messageHeight = fMessageView->TextHeight(0, fMessageView->شمار_سطرها());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    اگر (f_درون_داد_مقدار != پوچ) {
	rect.بالاترین     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	f_درون_داد_Control = جدید BTextControl(rect, "_iv_", پوچ, f_درون_داد_مقدار, پوچ,
		B_FOLLOW_چپ | B_FOLLOW_بالاترین, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_درون_داد_Control->TextView()->SetText(f_درون_داد_مقدار);
	f_درون_داد_Control->TextView()->SetWordWrap(نادرست);
	AddChild(f_درون_داد_Control);

	اعشاری width = 0.f, height = 0.f;
	f_درون_داد_Control->GetPreferredSize(&width, &height);
	f_درون_داد_Control->MakeFocus(درست);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    اعشاری buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    برای (int32 i = 0 ; i < buttons_شمار; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	اگر (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} وگرنه
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	اگر (dfltbutton == i + 1) {
	    button->MakeDefault(درست);
	    button->MakeFocus(f_درون_داد_Control == پوچ);
	}
    }
}

VimDialog::~VimDialog()
{
    اگر (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    صحیح
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    اگر (fDialogSem < B_OK) {
	Quit();
	بازگشت f_Dialog_مقدار;
    }

    Show();

    وقتی (acquire_sem(fDialogSem) == B_INTERRUPTED);

    صحیح ret_مقدار = f_Dialog_مقدار;
    اگر (f_درون_داد_مقدار != پوچ)
	vim_strncpy((char_u*)f_درون_داد_مقدار, (char_u*)f_درون_داد_Control->Text(), IOSIZE - 1);

    اگر (Lock())
	Quit();

    بازگشت ret_مقدار;
}

تهی VimDialog::MessageReceived(BMessage *پیام)
{
    int32 which = 0;
    اگر (پیام->what != k_Vim_Dialog_Button_پیام ||
	    پیام->FindInt32("which", &which) != B_OK)
	بازگشت BWindow::MessageReceived(پیام);

    f_Dialog_مقدار = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ثابت نویسه* label)
{
    BMessage *message = جدید BMessage(k_Vim_Dialog_Button_پیام);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = جدید BButton(rect, name.String(), label, message,
	    B_FOLLOW_راست | B_FOLLOW_BOTTOM);

    اعشاری width = 0.f, height = 0.f;
    button->GetPreferredSize(&width, &height);
    button->ResizeTo(width, height);

    بازگشت button;
}

VimDialog::View::View(BRect frame)
    :	BView(frame, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(پوچ)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    delete fIconBitmap;
}

تهی VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.راست = kVimDialogIconStripeWidth;
    Set_بالا_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    اگر (fIconBitmap == پوچ)
	بازگشت;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

تهی VimDialog::View::InitIcon(int32 type)
{
    اگر (type == VIM_GENERIC)
	بازگشت;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    اگر (status != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(status));
	بازگشت;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    اگر (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	بازگشت;
    }

    BResources resources(&file);
    اگر (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	بازگشت;
    }

    ثابت نویسه *name = "";
    انتخاب(type) {
	حالت VIM_ERROR:	    name = "stop"; بشکن;
	حالت VIM_WARNING:   name = "warn"; بشکن;
	حالت VIM_INFO:	    name = "info"; بشکن;
	حالت VIM_QUESTION:  name = "idea"; بشکن;
	پیشفرض: بازگشت;
    }

    int32 iconSize = 32;
    fIconBitmap = جدید BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), 0, B_RGBA32);
    اگر (fIconBitmap == پوچ || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == پوچ) ? "null" : strerror(fIconBitmap->InitCheck()));
	بازگشت;
    }

    size_t size = 0;
    ثابت uint8* iconData = پوچ;
    //	try vector icon first?
    iconData = (ثابت uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &size);
    اگر (iconData != پوچ && BIconUtils::GetVectorIcon(iconData, size, fIconBitmap) == B_OK)
	بازگشت;

    //	try bitmap icon now
    iconData = (ثابت uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &size);
    اگر (iconData == پوچ) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = پوچ;
	بازگشت;
    }

    اگر (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(iconData, iconSize, iconSize, iconSize, fIconBitmap);
}

ثابت unsigned صحیح  k_Vim_Dialog_OKButton_پیام = 'FDOK';
ثابت unsigned صحیح  k_Vim_Dialog_Cancel_Button_پیام = 'FDCN';
ثابت unsigned صحیح  k_Vim_Dialog_Size_درون_داد_پیام = 'SICH';
ثابت unsigned صحیح  k_Vim_Dialog_Family_Select_پیام = 'MSFM';
ثابت unsigned صحیح  k_Vim_Dialog_Style_Select_پیام = 'MSST';
ثابت unsigned صحیح  k_Vim_Dialog_Size_Select_پیام = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, اعشاری* size)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_مقدار(نادرست)
    , fFamily(family)
    , fStyle(style)
    , fSize(size)
    , fFontSize(*size)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_درون_داد(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_LENGTH);
    strncpy(fFontStyle, *style, B_FONT_STYLE_LENGTH);

    //	"client" area view
    BBox *clientBox = جدید BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.چپ_بالاترین(), RC.چپ_بالاترین());

    //	at first create all controls
    fPreview = جدید BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = جدید BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = جدید BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = جدید BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *labelSize = جدید BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(labelSize);
    labelSize->ResizeToPreferred();

    fFamiliesList = جدید BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = جدید BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_چپ_راست, 0, نادرست, درست);
    clientBox->AddChild(scrollFamilies);

    fStylesList= جدید BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = جدید BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_چپ_راست, 0, نادرست, درست);
    clientBox->AddChild(scrollStyles);

    f_Sizes_درون_داد = جدید BTextControl(rc, "inputSize", پوچ, "???",
	    جدید BMessage(k_Vim_Dialog_Size_درون_داد_پیام));
    clientBox->AddChild(f_Sizes_درون_داد);
    f_Sizes_درون_داد->ResizeToPreferred();

    fSizesList = جدید BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = جدید BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_چپ_راست, 0, نادرست, درست);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = جدید BButton(rc, "buttonOK", "OK",
			جدید BMessage(k_Vim_Dialog_OKButton_پیام));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = جدید BButton(rc, "buttonCancel", "Cancel",
			جدید BMessage(k_Vim_Dialog_Cancel_Button_پیام));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    اعشاری سطر_Height = labelFamily->Bounds().Height();
    اعشاری previewHeight = سطر_Height * 3;
    اعشاری offsetYLabels = previewHeight + kVimDialogSpacingY;
    اعشاری offsetYLists = offsetYLabels + سطر_Height + kVimDialogSpacingY / 2;
    اعشاری offsetYSizes = offsetYLists + f_Sizes_درون_داد->Bounds().Height() + kVimDialogSpacingY / 2;
    اعشاری listsHeight = سطر_Height * 9;
    اعشاری offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    اعشاری maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    اعشاری familiesWidth = labelFamily->Bounds().Width() * 5;
    اعشاری offsetXStyles = familiesWidth + kVimDialogSpacingX;
    اعشاری stylesWidth = labelStyle->Bounds().Width() * 4;
    اعشاری offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    اعشاری sizesWidth = labelSize->Bounds().Width() * 2;
    اعشاری maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.چپ + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.بالاترین + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    labelSize->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    اعشاری insetX = f_Sizes_درون_داد->TextView()->Bounds().Width() - f_Sizes_درون_داد->Bounds().Width();
    اعشاری insetY = f_Sizes_درون_داد->TextView()->Bounds().Width() - f_Sizes_درون_داد->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    f_Sizes_درون_داد->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    f_Sizes_درون_داد->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    f_Sizes_درون_داد->ResizeTo(sizesWidth, f_Sizes_درون_داد->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    صحیح selIndex = -1;
    صحیح شمار = شمار_font_families();
    برای (صحیح i = 0; i < شمار; i++) {
	font_family family;
	اگر (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(جدید BStringItem((ثابت نویسه*)family));
	    اگر (strncmp(family, fFontFamily, B_FONT_FAMILY_LENGTH) == 0)
		selIndex = i;
	}
    }

    اگر (selIndex >= 0) {
	fFamiliesList->Select(selIndex);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    selIndex = -1;
    برای (صحیح size = 8, index = 0; size <= 18; size++, index++) {
	BString str;
	str << size;
	fSizesList->AddItem(جدید BStringItem(str));
	اگر (size == fFontSize)
	    selIndex = index;

    }

    اگر (selIndex >= 0) {
	fSizesList->Select(selIndex);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(جدید BMessage(k_Vim_Dialog_Family_Select_پیام));
    fStylesList->SetSelectionMessage(جدید BMessage(k_Vim_Dialog_Style_Select_پیام));
    fSizesList->SetSelectionMessage(جدید BMessage(k_Vim_Dialog_Size_Select_پیام));
    f_Sizes_درون_داد->SetModificationMessage(جدید BMessage(k_Vim_Dialog_Size_درون_داد_پیام));

    Update_Size_درون_داد_Preview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    اگر (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    تهی
VimSelectFontDialog::_CleanList(BListView* list)
{
    وقتی (0 < list->شمار_اقلام())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    منطقی
VimSelectFontDialog::Go()
{
    اگر (fStatus != B_OK) {
	Quit();
	بازگشت NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    اگر (fDialogSem < B_OK) {
	Quit();
	بازگشت f_Dialog_مقدار;
    }

    Show();

    وقتی (acquire_sem(fDialogSem) == B_INTERRUPTED);

    منطقی ret_مقدار = f_Dialog_مقدار;

    اگر (Lock())
	Quit();

    بازگشت ret_مقدار;
}


تهی VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 selIndex = -1;
    int32 شمار = شمار_font_styles(fFontFamily);
    برای (int32 i = 0; i < شمار; i++) {
	font_style style;
	uint32 flags = 0;
	اگر (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(جدید BStringItem((ثابت نویسه*)style));
	    اگر (strncmp(style, fFontStyle, B_FONT_STYLE_LENGTH) == 0)
		selIndex = i;
	}
    }

    اگر (selIndex >= 0) {
	fStylesList->Select(selIndex);
	fStylesList->ScrollToSelection();
    } وگرنه
	fStylesList->Select(0);
}


تهی VimSelectFontDialog::Update_Size_درون_داد_Preview()
{
    نویسه buf[10] = {0};
    vim_snprintf(buf, اندازه(buf), (نویسه*)"%.0f", fFontSize);
    f_Sizes_درون_داد->SetText(buf);
}


تهی VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_SIZE);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (صحیح)fFontSize << " pt.";
    fPreview->SetText(str);
}


    منطقی
VimSelectFontDialog::_UpdateFromListItem(BListView* list, نویسه* text, صحیح textSize)
{
    int32 index = list->CurrentSelection();
    اگر (index < 0)
	بازگشت نادرست;
    BStringItem* item = (BStringItem*)list->ItemAt(index);
    اگر (item == پوچ)
	بازگشت نادرست;
    strncpy(text, item->Text(), textSize);
    بازگشت درست;
}


تهی VimSelectFontDialog::MessageReceived(BMessage *پیام)
{
    انتخاب (پیام->what) {
	حالت k_Vim_Dialog_OKButton_پیام:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_LENGTH);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_LENGTH);
	    *fSize = fFontSize;
	    f_Dialog_مقدار = درست;
	حالت k_Vim_Dialog_Cancel_Button_پیام:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    بازگشت;
	حالت B_کلید_UP:
	    {
		int32 کلید = 0;
		اگر (پیام->FindInt32("raw_char", &کلید) == B_OK
			&& کلید == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    بشکن;

	حالت k_Vim_Dialog_Family_Select_پیام:
	    اگر (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_LENGTH)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    بشکن;
	حالت k_Vim_Dialog_Style_Select_پیام:
	    اگر (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_LENGTH))
		_UpdateFontPreview();
	    بشکن;
	حالت k_Vim_Dialog_Size_Select_پیام:
	    {
		نویسه buf[10] = {0};
		اگر (_UpdateFromListItem(fSizesList, buf, اندازه(buf))) {
		    اعشاری size = atof(buf);
		    اگر (size > 0.f) {
			fFontSize = size;
			Update_Size_درون_داد_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    بشکن;
	حالت k_Vim_Dialog_Size_درون_داد_پیام:
	    {
		اعشاری size = atof(f_Sizes_درون_داد->Text());
		اگر (size > 0.f) {
		    fFontSize = size;
		    _UpdateFontPreview();
		}
	    }
	    بشکن;
	پیشفرض:
	    بشکن;
    }
    بازگشت BWindow::MessageReceived(پیام);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ایستا BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, ثابت نویسه *name) :
    BBox(frame, name, B_FOLLOW_چپ | B_FOLLOW_بالاترین, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 شمار = fButtonsList.شمار_اقلام();
    برای (int32 i = 0; i < شمار; i++)
	delete (BPictureButton*)fButtonsList.ItemAt(i);
    fButtonsList.MakeEmpty();

    delete normalButtonsBitmap;
    delete grayedButtonsBitmap;
    normalButtonsBitmap    = پوچ;
    grayedButtonsBitmap  = پوچ;
}

    تهی
VimToolbar::AttachedToWindow()
{
    BBox::AttachedToWindow();

    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    اعشاری
VimToolbar::ToolbarHeight() ثابت
{
    اعشاری size = پوچ == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    بازگشت size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    منطقی
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    اعشاری height = bitmap->Bounds().Height();
    اعشاری width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->BitsLength() / 4;
    برای (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    بازگشت درست;
}

    منطقی
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    اگر (normalButtonsBitmap == پوچ)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    اگر (normalButtonsBitmap == پوچ)
	بازگشت نادرست;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = جدید BBitmap(&archive);
    اگر (grayedButtonsBitmap == پوچ)
	بازگشت نادرست;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    بازگشت درست;
}

BBitmap *VimToolbar::LoadVimBitmap(ثابت نویسه* fileName)
{
    BBitmap *bitmap = پوچ;

    صحیح mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    اگر (runtimePath != پوچ && fileName != پوچ) {
	BString strPath((نویسه*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    اگر (mustfree)
	vim_free(runtimePath);

    بازگشت bitmap;
}

    منطقی
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, منطقی pressed)
{
    اعشاری size = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, size, size), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.Set_بالا_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, size - 1, size - 1);
    BRect destination(source);

    source.OffsetBy(size * index, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    اگر (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	size += ButtonMargin * 2 - 1;
	view.Begin_سطر_Array(4);
	view.Add_سطر(BPoint(0, 0),	 BPoint(size, 0),    shadowColor);
	view.Add_سطر(BPoint(size, 0),	 BPoint(size, size), shineColor);
	view.Add_سطر(BPoint(size, size), BPoint(0, size),    shineColor);
	view.Add_سطر(BPoint(0, size),	 BPoint(0, 0),	     shadowColor);
	view.End_سطر_Array();
    }

    view.EndPicture();
    RemoveChild(&view);

    بازگشت درست;
}

    منطقی
VimToolbar::AddButton(int32 index, vimmenu_T *menu)
{
    BPictureButton *button = پوچ;
    اگر (!menu_is_separator(menu->name)) {
	اعشاری size = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, size, size);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	اگر (menu->iconfile == پوچ && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, درست);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, نادرست);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, نادرست);
	} وگرنه {

	    char_u buffer[MAXPATHL] = {0};
	    BBitmap *bitmap = پوچ;

	    اگر (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, buffer, (نویسه*)"png");
		bitmap = BTranslationUtils::GetBitmap((نویسه*)buffer);
	    }

	    اگر (bitmap == پوچ && gui_find_bitmap(menu->name, buffer, (نویسه*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((نویسه*)buffer);

	    اگر (bitmap == پوچ)
		bitmap = جدید BBitmap(BRect(0, 0, size, size), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, درست);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, نادرست);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, نادرست);

	    delete bitmap;
	}

	button = جدید BPictureButton(frame, (نویسه*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    منطقی result = fButtonsList.AddItem(button, index);
    InvalidateLayout();
    بازگشت result;
}

    منطقی
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    اگر (menu->button) {
	اگر (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = پوچ;
	}
    }
    بازگشت درست;
}

    منطقی
VimToolbar::GrayButton(vimmenu_T *menu, صحیح grey)
{
    اگر (menu->button) {
	int32 index = fButtonsList.IndexOf(menu->button);
	اگر (index >= 0)
	    menu->button->SetEnabled(grey ? نادرست : درست);
    }
    بازگشت درست;
}

    تهی
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 شمار = fButtonsList.شمار_اقلام();
    برای (int32 i = 0; i < شمار; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	اگر (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} وگرنه
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#اگر defined(FEAT_GUI_TABLINE)

    اعشاری
Vim_Tab_سطر::TablineHeight() ثابت
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    بازگشت TabHeight(); //  + ToolbarMargin;
}

تهی
Vim_Tab_سطر::MouseDown(BPoint point)
{
    اگر (!gui_mch_showing_tabline())
	بازگشت;

    BMessage *m = Window()->CurrentMessage();
    أكد(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    صحیح index = 0; //  0 means here - no tab found
    برای (صحیح i = 0; i < شمار_Tabs(); i++) {
	اگر (TabFrame(i).Contains(point)) {
	    index = i + 1; //  indexes are 1-based
	    بشکن;
	}
    }

    صحیح event = -1;

    اگر ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    وگرنه اگر (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = index > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    وگرنه اگر (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* برداشت_Up_Menu = جدید BPopUpMenu("tabLineContextMenu", نادرست, نادرست);
	برداشت_Up_Menu->AddItem(جدید BMenuItem(_("Close tabi R"), جدید BMessage(TABLINE_MENU_CLOSE)));
	برداشت_Up_Menu->AddItem(جدید BMenuItem(_("New tab    T"), جدید BMessage(TABLINE_MENU_NEW)));
	برداشت_Up_Menu->AddItem(جدید BMenuItem(_("Open tab..."), جدید BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = برداشت_Up_Menu->Go(point);
	اگر (item != پوچ) {
	    event = item->Command();
	}

	delete برداشت_Up_Menu;

    } وگرنه {
	//  default processing
	BTabView::MouseDown(point);
	بازگشت;
    }

    اگر (event < 0)
	بازگشت;

    Vim_Tabline_Menu_پیام tmm;
    tmm.index = index;
    tmm.event = event;
    write_port(gui.vdcmp, Vim_پیام::TablineMenu, &tmm, اندازه(tmm));
}

تهی
Vim_Tab_سطر::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    اگر (tab_سطر != پوچ) {

	int32 i = 0;
	برای (; i < tab_سطر->شمار_Tabs(); i++)
	    اگر (این == tab_سطر->TabAt(i))
		بشکن;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	اگر (i < tab_سطر->شمار_Tabs()) {
	    Vim_Tabline_پیام tm;
	    tm.index = i + 1;
	    write_port(gui.vdcmp, Vim_پیام::Tabline, &tm, اندازه(tm));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ایستا نویسه appsig[] = "application/x-vnd.Haiku-Vim-8";
کلید_map *کلید_Map;
نویسه *کلید_Map_Chars;
صحیح main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    ساختار Vim_پیام vm;
    int32 what;
    ssize_t size;

    size = read_port_etc(gui.vdcmp, &what, &vm, اندازه(vm),
	    B_TIMEOUT, timeout);

    اگر (size >= 0) {
	انتخاب (what) {
	    حالت Vim_پیام::کلید_2:
		{
		    char_u *string = vm.u.کلید_2.chars;
		    صحیح len = vm.u.کلید_2.length;
		    اگر (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_درون_داد_buf();
			got_int = TRUE;
		    }

		    اگر (vm.u.کلید_2.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			صحیح	i;
			char_u	buf[2];

			برای (i = 0; i < len; ++i)
			{
			    add_to_درون_داد_buf(string + i, 1);
			    اگر (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (صحیح)KE_CSI;
				add_to_درون_داد_buf(buf, 2);
			    }
			}
		    }
#وگرنه
			add_to_درون_داد_buf_csi(string, len);
#endif
		    وگرنه
			add_to_درون_داد_buf(string, len);
		}
		بشکن;
	    حالت Vim_پیام::Resize:
		gui_resize_shell(vm.u.NewSize.width, vm.u.NewSize.height);
		بشکن;
	    حالت Vim_پیام::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_شمار =
			atomic_add(&vm.u.Scroll.sb->scroll_Event_شمار, -1);
		    اگر (old_شمار <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.مقدار, vm.u.Scroll.stillDragging);
		}
		بشکن;
#اگر defined(FEAT_MENU)
	    حالت Vim_پیام::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		بشکن;
#endif
	    حالت Vim_پیام::Mouse:
		{
		    int32 old_شمار;
		    اگر (vm.u.Mouse.button == MOUSE_DRAG)
			old_شمار =
			    atomic_add(&gui.vimTextArea->mouse_Drag_Event_شمار, -1);
		    وگرنه
			old_شمار = 0;
		    اگر (old_شمار <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		بشکن;
	    حالت Vim_پیام::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		بشکن;
	    حالت Vim_پیام::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		اگر (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		بشکن;
	    حالت Vim_پیام::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		بشکن;
	    حالت Vim_پیام::Tabline:
		send_tabline_event(vm.u.Tabline.index);
		بشکن;
	    حالت Vim_پیام::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.index, vm.u.TablineMenu.event);
		بشکن;
	    پیشفرض:
		//  unrecognised message, ignore it
		بشکن;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    بازگشت size;
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

    صحیح
vim_lock_screen()
{
    بازگشت !gui.vimWindow || gui.vimWindow->Lock();
}

    تهی
vim_unlock_screen()
{
    اگر (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#اگر RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(تهی *args)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    بازگشت 0;
}

#وگرنه

    int32
call_main(تهی *args)
{
    ساختار MainArgs *ma = (MainArgs *)args;

    بازگشت اصلی(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    تهی
gui_mch_prepare(
	صحیح	*argc,
	نویسه	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    اگر (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(پوچ), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_شمار, "vim VDCMP");

#اگر RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, پوچ);
	اگر (tid >= B_OK) {
	    resume_thread(tid);
	} وگرنه {
	    getout(1);
	}
#وگرنه
	MainArgs ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	اگر (tid >= B_OK) {
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
	    (تهی)wait_for_thread(tid, &dummy_exitcode);

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
    اگر (!isatty(0)) {
	ساختار stat stat_stdin, stat_dev_null;

	اگر (fstat(0, &stat_stdin) == -1 ||
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
    صحیح
gui_mch_init_check(تهی)
{
    بازگشت OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    صحیح
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (صحیح) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (صحیح) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    اگر (gui.vdcmp < B_OK)
	بازگشت FAIL;
    get_کلید_map(&کلید_Map, &کلید_Map_Chars);

    gui.vimWindow = جدید VimWindow();	// hidden and locked
    اگر (!gui.vimWindow)
	بازگشت FAIL;

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

    بازگشت OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    تهی
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    اگر (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحیح
gui_mch_open()
{
    اگر (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    اگر (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	بازگشت OK;
    }

    بازگشت FAIL;
}

    تهی
gui_mch_exit(صحیح vim_exitcode)
{
    اگر (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#اگر !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    اگر (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(نادرست);

	main_exitcode = vim_exitcode;
#اگر RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#وگرنه
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
    صحیح
gui_mch_get_winpos(صحیح *x, صحیح *y)
{
    اگر (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (صحیح)r.چپ;
	*y = (صحیح)r.بالاترین;
	بازگشت OK;
    }
    وگرنه
	بازگشت FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    تهی
gui_mch_set_winpos(صحیح x, صحیح y)
{
    اگر (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
تهی
gui_mch_set_shellsize(
	صحیح	width,
	صحیح	height,
	صحیح	min_width,
	صحیح	min_height,
	صحیح	base_width,
	صحیح	base_height,
	صحیح	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    اگر (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	اعشاری minWidth, maxWidth, minHeight, maxHeight;

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

تهی
gui_mch_get_screen_dimensions(
	صحیح	*screen_w,
	صحیح	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	اگر (screen.IsValid()) {
	    frame = screen.Frame();
	} وگرنه {
	    frame.راست = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (صحیح) frame.راست - 2 * gui.scrollbar_width - 20;
    *screen_h = (صحیح) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

تهی
gui_mch_set_text_area_pos(
	صحیح	x,
	صحیح	y,
	صحیح	w,
	صحیح	h)
{
    اگر (!gui.vimTextArea)
	بازگشت;

    اگر (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	اگر (gui.vimForm->Tab_سطر() != پوچ) {
	    gui.vimForm->Tab_سطر()->ResizeTo(w, gui.vimForm->TablineHeight());
	}
#endif // FEAT_GUI_TABLINE

	gui.vimWindow->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

تهی
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	صحیح	flag)
{
    VimScrollBar *vsb = sb->id;
    اگر (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	اگر (flag) {
	    اگر (vsb->IsHidden()) {
		vsb->Show();
	    }
	} وگرنه {
	    اگر (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

تهی
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	صحیح	val,
	صحیح	size,
	صحیح	max)
{
    اگر (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	اگر (max == 0) {
	    s->Set_مقدار(0);
	    s->SetRange(0.0, 0.0);
	} وگرنه {
	    s->SetProportion((اعشاری)size / (max + 1.0));
	    s->SetSteps(1.0, size > 5 ? size - 2 : size);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - size;
#endif
	    اگر (max < s->مقدار_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->Set_مقدار(val);
		s->SetRange(0.0, max);
	    } وگرنه {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->Set_مقدار(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

تهی
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	صحیح	x,
	صحیح	y,
	صحیح	w,
	صحیح	h)
{
    اگر (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	اعشاری vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	اگر (winb.راست-(x+w)<w) vsbx = winb.راست - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    صحیح
gui_mch_get_scrollbar_xpadding(تهی)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    بازگشت 0;
}

    صحیح
gui_mch_get_scrollbar_ypadding(تهی)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    بازگشت 0;
}

تهی
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	صحیح	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = جدید VimScrollBar(sb, posture);
    اگر (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#اگر defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
تهی
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    اگر (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    صحیح
gui_mch_is_blink_off(تهی)
{
    بازگشت FALSE;
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

ایستا صحیح	blink_state = BLINK_NONE;
ایستا long_u	    blink_waittime = 700;
ایستا long_u	    blink_ontime = 400;
ایستا long_u	    blink_offtime = 250;
ایستا صحیح  blink_timer = 0;

تهی
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
    تهی
gui_mch_stop_blink(صحیح may_call_gui_update_cursor)
{
    // TODO
    اگر (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    اگر (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    تهی
gui_mch_start_blink()
{
    // TODO
    اگر (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
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
صحیح
gui_mch_init_font(
	char_u	    *font_name,
	صحیح	    fontset)
{
    اگر (gui.vimWindow->Lock())
    {
	صحیح rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	بازگشت rc;
    }

    بازگشت FAIL;
}


    صحیح
gui_mch_adjust_charsize()
{
    بازگشت FAIL;
}


    صحیح
gui_mch_font_dialog(font_family* family, font_style* style, اعشاری* size)
{
#اگر defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = جدید VimSelectFontDialog(family, style, size);
    منطقی ret = dialog->Go();
	delete dialog;
	بازگشت ret;
#وگرنه
    بازگشت NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	صحیح	    giveErrorIfMissing)
{
    ایستا VimFont *fontList = پوچ;

    اگر (!gui.in_use)	//  can't do this when GUI not running
	بازگشت NOFONT;

    //	storage for locally modified name;
    ثابت صحیح buff_size = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 20;
    ایستا نویسه font_name[buff_size] = {0};
    font_family family = {0};
    font_style	style  = {0};
    اعشاری size = 0.f;

    اگر (name == 0 && be_fixed_font == 0) {
	اگر (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	بازگشت NOFONT;
    }

    منطقی useSelectGUI = نادرست;
    اگر (name != پوچ)
	اگر (STRCMP(name, "*") == 0) {
	    useSelectGUI = درست;
	    STRNCPY(font_name, hl_get_font_name(), buff_size);
	} وگرنه
	    STRNCPY(font_name, name, buff_size);

    اگر (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	size = be_fixed_font->Size();
	vim_snprintf(font_name, buff_size,
	    (نویسه*)"%s/%s/%.0f", family, style, size);
    }

    //	replace underscores with spaces
    نویسه* end = 0;
    وقتی (end = strchr((نویسه *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ایستا نویسه buff[buff_size] = {0};
    STRNCPY(buff, font_name, buff_size);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_LENGTH);
    نویسه* style_s = strtok(0, "/\0");
    اگر (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_LENGTH);
    size = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    اگر (useSelectGUI) {
	اگر (gui_mch_font_dialog(&family, &style, &size) == NOFONT)
	    بازگشت FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_size,
		(نویسه*)"%s/%s/%.0f", family, style, size);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	اگر (new_p_guifont != پوچ) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    برای ( ; *new_p_guifont; ++new_p_guifont)
		اگر (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    برای (flp = fontList; flp; flp = flp->next) {
	اگر (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    بازگشت (GuiFont)flp;
	}
    }

    VimFont *font = جدید VimFont();
    font->name = vim_strsave((char_u*)font_name);

    اگر (شمار_font_styles(family) <= 0) {
	اگر (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	بازگشت NOFONT;
    }

    //	Remember font in the static list for later use
    font->next = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    اگر (size > 0.f)
	font->SetSize(size);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    بازگشت (GuiFont)font;
}

/*
 * Set the current text font.
 */
تهی
gui_mch_set_font(
	GuiFont	font)
{
    اگر (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (صحیح) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (صحیح)(fh.ascent + 0.9999)
	    + (صحیح)(fh.descent + 0.9999) + (صحیح)(fh.leading + 0.9999);
	gui.char_ascent = (صحیح)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
تهی
gui_mch_free_font(
	GuiFont	font)
{
    اگر (font == NOFONT)
	بازگشت;
    VimFont *f = (VimFont *)font;
    اگر (--f->refcount <= 0) {
	اگر (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    اگر (name == پوچ)
	بازگشت پوچ;
    بازگشت vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحیح
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
	gui.char_height = (صحیح)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (صحیح)(fh.ascent + 0.5) + p_linespace / 2;
    }
    بازگشت OK;
}

    تهی
gui_mch_getmouse(صحیح *x, صحیح *y)
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

    تهی
gui_mch_mousehide(صحیح hide)
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
    بازگشت gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
تهی
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    اگر (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_بالا_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text background color.
 */
تهی
gui_mch_set_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    اگر (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_کم_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    تهی
gui_mch_set_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

تهی
gui_mch_draw_string(
	صحیح	row,
	صحیح	col,
	char_u	*s,
	صحیح	len,
	صحیح	flags)
{
    اگر (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
    بازگشت gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
صحیح
gui_mch_haskey(
	char_u	*name)
{
    صحیح i;

    برای (i = 0; special_کلیدها[i].Be_کلیدها != 0; i++)
	اگر (name[0] == special_کلیدها[i].vim_code0 &&
		name[1] == special_کلیدها[i].vim_code1)
	    بازگشت OK;
    بازگشت FAIL;
}

    تهی
gui_mch_beep()
{
    ::beep();
}

    تهی
gui_mch_flash(صحیح msec)
{
    // Do a visual beep by reversing the foreground and background colors

    اگر (gui.vimWindow->Lock()) {
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
تهی
gui_mch_invert_rectangle(
	صحیح	r,
	صحیح	c,
	صحیح	nr,
	صحیح	nc)
{
    BRect rect;
    rect.چپ = FILL_X(c);
    rect.بالاترین = FILL_Y(r);
    rect.راست = rect.چپ + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.بالاترین + nr * gui.char_height - PEN_WIDTH;

    اگر (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimWindow->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    تهی
gui_mch_iconify()
{
    اگر (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(درست);
	gui.vimWindow->Unlock();
    }
}

#اگر defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    تهی
gui_mch_set_foreground(تهی)
{
    // TODO
}
#endif

/*
 * Set the window title
 */
تهی
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    اگر (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((نویسه *)title);
	gui.vimWindow->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    تهی
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.چپ = FILL_X(gui.col);
    r.بالاترین = FILL_Y(gui.row);
    صحیح cells = utf_off2cells(سطر_Offset[gui.row] + gui.col, 100); // TODO-TODO
    اگر (cells>=4) cells = 1;
    r.راست = r.چپ + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.بالاترین + gui.char_height - PEN_WIDTH;

    اگر (gui.vimWindow->Lock()) {
	gui.vimTextArea->StrokeRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
تهی
gui_mch_draw_part_cursor(
	صحیح	w,
	صحیح	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.چپ =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_راست ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.راست = r.چپ + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.بالاترین = r.bottom - h + PEN_WIDTH;

    اگر (gui.vimWindow->Lock()) {
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
    تهی
gui_mch_update()
{
    gui_mch_flush();
    وقتی (port_شمار(gui.vdcmp) > 0 &&
	    !vim_is_درون_داد_buf_full() &&
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
صحیح
gui_mch_wait_for_chars(
	long	wtime)
{
    صحیح		focus;
    bigtime_t	until, timeout;
    status_t	st;

    اگر (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    وگرنه
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    برای (;;)
    {
	// Stop or start blinking when focus changes
	اگر (gui.in_focus != focus)
	{
	    اگر (gui.in_focus)
		gui_mch_start_blink();
	    وگرنه
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
	اگر (did_add_timer)
	    // Need to recompute the waiting time.
	    بشکن;
# endif
# ifdef FEAT_JOB_CHANNEL
	اگر (has_any_channel())
	{
	    اگر (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	وگرنه اگر (wtime < 0)
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

	اگر (درون_داد_available())
	    بازگشت OK;
	اگر (st < B_OK)		// includes B_TIMED_OUT
	    بازگشت FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	اگر (wtime >= 0)
	{
	    timeout = until - system_time();
	    اگر (timeout < 0)
		بشکن;
	}
    }
    بازگشت FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    تهی
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    اگر (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    بازگشت;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
تهی
gui_mch_clear_block(
	صحیح	row1,
	صحیح	col1,
	صحیح	row2,
	صحیح	col2)
{
    اگر (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    تهی
gui_mch_clear_all()
{
    اگر (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
تهی
gui_mch_delete_سطرها(
	صحیح	row,
	صحیح	رقم_سطرها)
{
    gui.vimTextArea->mch_Delete_سطرها(row, رقم_سطرها);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
تهی
gui_mch_درج_سطرها(
	صحیح	row,
	صحیح	رقم_سطرها)
{
    gui.vimTextArea->mch_درج_سطرها(row, رقم_سطرها);
}

#اگر defined(FEAT_MENU)
/*
 * Menu stuff.
 */

تهی
gui_mch_enable_menu(
	صحیح	flag)
{
    اگر (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

تهی
gui_mch_set_menu_pos(
	صحیح	x,
	صحیح	y,
	صحیح	w,
	صحیح	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
تهی
gui_mch_add_menu(
	vimmenu_T   *menu,
	صحیح	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    اگر (menu_is_popup(menu->name) && parent == پوچ) {
	BPopUpMenu* برداشت_Up_Menu = جدید BPopUpMenu((ثابت نویسه*)menu->name, نادرست, نادرست);
	menu->submenu_id = برداشت_Up_Menu;
	menu->id = پوچ;
	بازگشت;
    }

    اگر (!menu_is_menubar(menu->name)
	    || (parent != پوچ && parent->submenu_id == پوچ))
	بازگشت;

    اگر (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	اگر ( parent )
	    tmp = parent->submenu_id;
	وگرنه
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	اگر ( ! tmp->FindItem((ثابت نویسه *) menu->dname)) {

	    BMenu *bmenu = جدید BMenu((نویسه *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((ثابت نویسه *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    تهی
gui_mch_toggle_tearoffs(صحیح enable)
{
    // no tearoff menus
}

    ایستا BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = جدید BMessage('menu');
    m->AddPointer("VimMenu", (تهی *)menu);

    بازگشت m;
}

/*
 * Add a menu item to a menu
 */
تهی
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	صحیح	idx)
{
    صحیح	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    اگر (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	اگر (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    اگر (toolbar != پوچ) {
		toolbar->AddButton(idx, menu);
	    }
	} وگرنه
#endif

	اگر (parent->submenu_id != پوچ || menu_is_popup(parent->name)) {
	    اگر (menu_is_separator(menu->name)) {
		BSeparatorItem *item = جدید BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = پوچ;
	    }
	    وگرنه {
		BMenuItem *item = جدید BMenuItem((نویسه *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((نویسه) menu->mnemonic);
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = پوچ;
	    }
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
تهی
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    اگر (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	اگر (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    اگر (toolbar != پوچ) {
		toolbar->RemoveButton(menu);
	    }
	} وگرنه
#endif
	{
	    أكد(menu->submenu_id == پوچ || menu->submenu_id->شمار_اقلام() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    اگر (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		اگر (bmenu == gui.vimForm->MenuBar() && bmenu->شمار_اقلام() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = پوچ;
	    menu->submenu_id = پوچ;

	    gui.menu_height = (صحیح) gui.vimForm->MenuHeight();
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
تهی
gui_mch_menu_grey(
	vimmenu_T   *menu,
	صحیح	grey)
{
#ifdef FEAT_TOOLBAR
    اگر (menu->parent && menu_is_toolbar(menu->parent->name)) {
	اگر (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    اگر (toolbar != پوچ) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } وگرنه
#endif
    اگر (menu->id != پوچ)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
تهی
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	صحیح	hidden)
{
    اگر (menu->id != پوچ)
	menu->id->SetEnabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    تهی
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    تهی
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    اگر (!menu_is_popup(menu->name) || menu->submenu_id == پوچ)
	بازگشت;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    اگر (popupMenu == پوچ)
	بازگشت;

    BPoint point;
    اگر (gui.vimWindow->Lock()) {
	uint32 buttons = 0;
	gui.vimTextArea->GetMouse(&point, &buttons);
	gui.vimTextArea->ConvertToScreen(&point);
	gui.vimWindow->Unlock();
    }
    popupMenu->Go(point, درست);
}

#endif // FEAT_MENU

// Mouse stuff

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
نویسه textplain[] = "text/plain";
نویسه vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    تهی
clip_mch_request_selection(Clipboard_T *cbd)
{
    اگر (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->Data();
	// m->PrintToStream();

	char_u *string = پوچ;
	ssize_t stringlen = -1;

	اگر (m->FindData(textplain, B_MIME_TYPE,
		    (ثابت تهی **)&string, &stringlen) == B_OK
		|| m->FindString("text", (ثابت نویسه **)&string) == B_OK)
	{
	    اگر (stringlen == -1)
		stringlen = STRLEN(string);

	    صحیح type;
	    نویسه *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    اگر (m->FindData(vimselectiontype, B_MIME_TYPE,
			(ثابت تهی **)&seltype, &seltypelen) == B_OK)
	    {
		انتخاب (*seltype)
		{
		    پیشفرض:
		    حالت 'L':	type = MLINE;	بشکن;
		    حالت 'C':	type = MCHAR;	بشکن;
#ifdef FEAT_VISUAL
		    حالت 'B':	type = MBLOCK;	بشکن;
#endif
		}
	    }
	    وگرنه
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
    تهی
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    صحیح
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    بازگشت FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    تهی
clip_mch_set_selection(Clipboard_T *cbd)
{
    اگر (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->Data();
	أكد(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = پوچ;
	long_u	شمار;
	صحیح type;

	type = clip_convert_selection(&str, &شمار, cbd);

	اگر (type < 0)
	    بازگشت;

	m->AddData(textplain, B_MIME_TYPE, (تهی *)str, شمار);

	// Add type of selection
	نویسه	vtype;
	انتخاب (type)
	{
	    پیشفرض:
	    حالت MLINE:    vtype = 'L';    بشکن;
	    حالت MCHAR:    vtype = 'C';    بشکن;
#ifdef FEAT_VISUAL
	    حالت MBLOCK:   vtype = 'B';    بشکن;
#endif
	}
	m->AddData(vimselectiontype, B_MIME_TYPE, (تهی *)&vtype, 1);

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
	صحیح saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = جدید BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    پوچ, پوچ, 0, نادرست,
	    جدید BMessage((saving == TRUE) ? 'save' : 'open'), پوچ, درست);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((نویسه*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((ثابت نویسه*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    وقتی (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = پوچ;
    status_t result = gui.vimApp->fBrowsedPath.InitCheck();
    اگر (result == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } وگرنه
	اگر (result != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    result, strerror(result));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = پوچ;

    بازگشت fileName;
}
#endif // FEAT_BROWSE


#اگر defined(FEAT_GUI_DIALOG)

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

صحیح
gui_mch_dialog(
	صحیح	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	صحیح	 dfltbutton,
	char_u	*textfield,
	صحیح ex_cmd)
{
    VimDialog *dialog = جدید VimDialog(type, (نویسه*)title, (نویسه*)message,
	    (نویسه*)buttons, dfltbutton, (نویسه*)textfield, ex_cmd);
    منطقی ret = dialog->Go();
    delete dialog;
	بازگشت ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    بازگشت ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    تهی
gui_mch_setmouse(صحیح x, صحیح y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    تهی
im_set_position(صحیح row, صحیح col)
{
    اگر (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    بازگشت;
}
#endif

    تهی
gui_mch_show_toolbar(صحیح showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    تهی
gui_mch_set_toolbar_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    اگر (toolbar != پوچ) {
	اگر (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#اگر defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    تهی
gui_mch_show_tabline(صحیح showit)
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();

    اگر (tab_سطر == پوچ)
	بازگشت;

    اگر (!showit != !gui.vimForm->Is_Showing_Tab_سطر()) {
	gui.vimForm->Set_Showing_Tab_سطر(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    تهی
gui_mch_set_tabline_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    اگر (tab_سطر != پوچ) {
	اگر (gui.vimWindow->Lock()) {
	    tab_سطر->MoveTo(x, y);
	    tab_سطر->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحیح
gui_mch_showing_tabline()
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    بازگشت tab_سطر != پوچ && gui.vimForm->Is_Showing_Tab_سطر();
}

/*
 * Update the labels of the tabline.
 */
    تهی
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    صحیح	    nr = 0;
    صحیح	    curtabidx = 0;

    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();

    اگر (tab_سطر == پوچ)
	بازگشت;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    برای (tp = first_tabpage; tp != پوچ; tp = tp->tp_next, ++nr) {
	اگر (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_سطر->TabAt(nr);

	اگر (tab == پوچ) {
	    tab = جدید Vim_Tab_سطر::VimTab();
	    tab_سطر->AddTab(پوچ, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((ثابت نویسه*)NameBuff);
	tab_سطر->Invalidate();
    }

    // Remove any old labels.
    وقتی (nr < tab_سطر->شمار_Tabs())
	tab_سطر->RemoveTab(nr);

    اگر (tab_سطر->Selection() != curtabidx)
	tab_سطر->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    تهی
gui_mch_set_curtab(صحیح nr)
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    اگر (tab_سطر == پوچ)
	بازگشت;

    gui.vimWindow->Lock();

    اگر (tab_سطر->Selection() != nr -1)
	tab_سطر->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
