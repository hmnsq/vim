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

बाह्य "C" {

#अन्तर्भाव <assert.h>
#अन्तर्भाव <float.h>
#अन्तर्भाव <syslog.h>

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#अन्तर्भाव <Application.h>
#अन्तर्भाव <Beep.h>
#अन्तर्भाव <Bitmap.h>
#अन्तर्भाव <Box.h>
#अन्तर्भाव <Button.h>
#अन्तर्भाव <Clipboard.h>
#अन्तर्भाव <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#अन्तर्भाव <File.h>
#अन्तर्भाव <FilePanel.h>
#अन्तर्भाव <FindDirectory.h>
//#include <Font.h>
#अन्तर्भाव <IconUtils.h>
#अन्तर्भाव <Input.h>
#अन्तर्भाव <ListView.h>
#अन्तर्भाव <MenuBar.h>
#अन्तर्भाव <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#अन्तर्भाव <Path.h>
#अन्तर्भाव <PictureButton.h>
#अन्तर्भाव <PopUpMenu.h>
//#include <Region.h>
#अन्तर्भाव <Resources.h>
//#include <Roster.h>
#अन्तर्भाव <Screen.h>
#अन्तर्भाव <ScrollBar.h>
#अन्तर्भाव <ScrollView.h>
#अन्तर्भाव <String.h>
#अन्तर्भाव <StringView.h>
//#include <SupportDefs.h>
#अन्तर्भाव <TabView.h>
#अन्तर्भाव <TextControl.h>
#अन्तर्भाव <TextView.h>
#अन्तर्भाव <TranslationUtils.h>
#अन्तर्भाव <TranslatorFormats.h>
#अन्तर्भाव <View.h>
#अन्तर्भाव <Window.h>

वर्ग VimApp;
वर्ग VimFormView;
वर्ग VimTextAreaView;
वर्ग VimWindow;
वर्ग VimToolbar;
वर्ग Vim_Tab_ओळ;

बाह्य किल्ली_map *किल्ली_Map;
बाह्य अक्षर *किल्ली_Map_Chars;

बाह्य पूर्णांक main(पूर्णांक argc, अक्षर **argv);

#ifndef B_MAX_PORT_मोजणी
#define B_MAX_PORT_मोजणी    255
#endif

// VimApp seems comparable to the X "vimShell"
वर्ग VimApp: सार्वजनिक BApplication
{
    प्रकारव्याख्या BApplication Inherited;
    सार्वजनिक:
    VimApp(स्थिरांक अक्षर *appsig);
    ~VimApp();

    // callbacks:
#जर 0
    आभासी शून्य DispatchMessage(BMessage *m, BHandler *h)
    {
	m->PrintToStream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    आभासी शून्य ReadyToRun();
    आभासी शून्य ArgvReceived(int32 argc, अक्षर **argv);
    आभासी शून्य RefsReceived(BMessage *m);
    आभासी तार्किक QuitRequested();
    आभासी शून्य MessageReceived(BMessage *m);

    स्थिर शून्य SendRefs(BMessage *m, तार्किक changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    खाजगी:
};

वर्ग VimWindow: सार्वजनिक BWindow
{
    प्रकारव्याख्या BWindow Inherited;
    सार्वजनिक:
    VimWindow();
    ~VimWindow();

    आभासी शून्य DispatchMessage(BMessage *m, BHandler *h);
    आभासी शून्य WindowActivated(तार्किक active);
    आभासी तार्किक QuitRequested();

    VimFormView	    *formView;

    खाजगी:
    शून्य init();

    तार्किक is_fullscreen = असत्य;
    BRect saved_frame;
    window_look saved_look;
};

वर्ग VimFormView: सार्वजनिक BView
{
    प्रकारव्याख्या BView Inherited;
    सार्वजनिक:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    आभासी शून्य AllAttached();
    आभासी शून्य FrameResized(दशांश new_width, दशांश new_height);

#define MENUBAR_MARGIN	1
    दशांश MenuHeight() स्थिरांक
    { परत menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() स्थिरांक
    { परत menuBar; }

    खाजगी:
    शून्य init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    सार्वजनिक:
    दशांश ToolbarHeight() स्थिरांक;
    VimToolbar *ToolBar() स्थिरांक
    { परत toolBar; }
    खाजगी:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    सार्वजनिक:
    Vim_Tab_ओळ *Tab_ओळ() स्थिरांक	{ परत tab_ओळ; }
    तार्किक Is_Showing_Tab_ओळ() स्थिरांक { परत showing_Tab_ओळ; }
    शून्य Set_Showing_Tab_ओळ(तार्किक showing) { showing_Tab_ओळ = showing;	}
    दशांश TablineHeight() स्थिरांक;
    खाजगी:
    Vim_Tab_ओळ	*tab_ओळ;
    पूर्णांक	showing_Tab_ओळ;
#endif
};

वर्ग VimTextAreaView: सार्वजनिक BView
{
    प्रकारव्याख्या BView Inherited;
    सार्वजनिक:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    आभासी शून्य Draw(BRect updateRect);
    आभासी शून्य किल्ली_Down(स्थिरांक अक्षर *bytes, int32 अंक_Bytes);
    आभासी शून्य MouseDown(BPoint point);
    आभासी शून्य MouseUp(BPoint point);
    आभासी शून्य MouseMoved(BPoint point, uint32 transit, स्थिरांक BMessage *message);
    आभासी शून्य MessageReceived(BMessage *m);

    // own functions:
    पूर्णांक mchInitFont(char_u *name);
    शून्य mchDrawString(पूर्णांक row, पूर्णांक col, char_u *s, पूर्णांक len, पूर्णांक flags);
    शून्य mchClearBlock(पूर्णांक row1, पूर्णांक col1, पूर्णांक row2, पूर्णांक col2);
    शून्य mchClearAll();
    शून्य mch_Delete_ओळी(पूर्णांक row, पूर्णांक अंक_ओळी);
    शून्य mch_घाला_ओळी(पूर्णांक row, पूर्णांक अंक_ओळी);

    स्थिर शून्य guiSendMouseEvent(पूर्णांक button, पूर्णांक x, पूर्णांक y, पूर्णांक repeated_click, int_u modifiers);
    स्थिर शून्य guiMouseMoved(पूर्णांक x, पूर्णांक y);
    स्थिर शून्य guiBlankMouse(तार्किक should_hide);
    स्थिर int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouse_Drag_Event_मोजणी;

#ifdef FEAT_MBYTE_IME
    शून्य DrawIMString(शून्य);
#endif

    खाजगी:
    शून्य init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    संरचना {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	पूर्णांक row;
	पूर्णांक col;
	पूर्णांक मोजणी;
    } IMData;
#endif
};

वर्ग VimScrollBar: सार्वजनिक BScrollBar
{
    प्रकारव्याख्या BScrollBar Inherited;
    सार्वजनिक:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    आभासी शून्य मूल्य_Changed(दशांश new_मूल्य);
    आभासी शून्य MouseUp(BPoint where);
    शून्य Set_मूल्य(दशांश newval);
    scrollbar_T *getGsb()
    { परत gsb; }

    int32	scroll_Event_मोजणी;

    खाजगी:
    scrollbar_T *gsb;
    दशांश   ignore_मूल्य;
};


#ifdef FEAT_TOOLBAR

वर्ग VimToolbar : सार्वजनिक BBox
{
    स्थिर BBitmap *normalButtonsBitmap;
    स्थिर BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(स्थिरांक अक्षर* fileName);
    तार्किक GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, तार्किक pressed);
    तार्किक ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    शून्य InvalidateLayout();

    सार्वजनिक:
    VimToolbar(BRect frame, स्थिरांक अक्षर * name);
    ~VimToolbar();

    तार्किक PrepareButtonBitmaps();

    तार्किक AddButton(int32 index, vimmenu_T *menu);
    तार्किक RemoveButton(vimmenu_T *menu);
    तार्किक GrayButton(vimmenu_T *menu, पूर्णांक grey);

    दशांश ToolbarHeight() स्थिरांक;
    आभासी शून्य AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = रिक्त;
BBitmap *VimToolbar::grayedButtonsBitmap  = रिक्त;

स्थिरांक दशांश ToolbarMargin = 3.;
स्थिरांक दशांश ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

वर्ग Vim_Tab_ओळ : सार्वजनिक BTabView
{
    सार्वजनिक:
	वर्ग VimTab : सार्वजनिक BTab {
	    सार्वजनिक:
		VimTab() : BTab(नवीन BView(BRect(), "-Empty-", 0, 0)) {}

	    आभासी शून्य Select(BView* owner);
	};

	Vim_Tab_ओळ(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_डावा | B_FOLLOW_शीर्ष | B_FOLLOW_उजवा, B_WILL_DRAW | B_FRAME_EVENTS) {}

    दशांश TablineHeight() स्थिरांक;
    आभासी शून्य MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
वर्ग VimFont: सार्वजनिक BFont
{
    प्रकारव्याख्या BFont Inherited;
    सार्वजनिक:
    VimFont();
    VimFont(स्थिरांक VimFont *rhs);
    VimFont(स्थिरांक BFont *rhs);
    VimFont(स्थिरांक VimFont &rhs);
    ~VimFont();

    VimFont *next;
    पूर्णांक refcount;
    char_u *name;

    खाजगी:
    शून्य init();
};

#जर defined(FEAT_GUI_DIALOG)

वर्ग VimDialog : सार्वजनिक BWindow
{
    प्रकारव्याख्या BWindow Inherited;

    BButton* _CreateButton(int32 which, स्थिरांक अक्षर* label);

    सार्वजनिक:

    वर्ग View : सार्वजनिक BView {
	प्रकारव्याख्या BView Inherited;

	सार्वजनिक:
	View(BRect frame);
	~View();

	आभासी शून्य Draw(BRect updateRect);
	शून्य InitIcon(int32 type);

	खाजगी:
	BBitmap*    fIconBitmap;
    };

    VimDialog(पूर्णांक type, स्थिरांक अक्षर *title, स्थिरांक अक्षर *message,
	    स्थिरांक अक्षर *buttons, पूर्णांक dfltbutton, स्थिरांक अक्षर *textfield,
	    पूर्णांक ex_cmd);
    ~VimDialog();

    पूर्णांक Go();

    आभासी शून्य MessageReceived(BMessage *संदेश);

    खाजगी:
    sem_id	    fDialogSem;
    पूर्णांक		    f_Dialog_मूल्य;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   f_आदान_Control;
    स्थिरांक अक्षर*	    f_आदान_मूल्य;
};

वर्ग VimSelectFontDialog : सार्वजनिक BWindow
{
    प्रकारव्याख्या BWindow Inherited;

    शून्य _CleanList(BListView* list);
    शून्य _UpdateFontStyles();
    शून्य Update_Size_आदान_Preview();
    शून्य _UpdateFontPreview();
    तार्किक _UpdateFromListItem(BListView* list, अक्षर* text, पूर्णांक textSize);
    सार्वजनिक:

    VimSelectFontDialog(font_family* family, font_style* style, दशांश* size);
    ~VimSelectFontDialog();

    तार्किक Go();

    आभासी शून्य MessageReceived(BMessage *संदेश);

    खाजगी:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    तार्किक	    f_Dialog_मूल्य;
    font_family*    fFamily;
    font_style*	    fStyle;
    दशांश*	    fSize;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    दशांश	    fFontSize;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_आदान;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

संरचना MainArgs {
    पूर्णांक	     argc;
    अक्षर    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	किल्ली_संदेश_BUFSIZ	7
#जर किल्ली_संदेश_BUFSIZ < MAX_किल्ली_CODE_LEN
#error Increase किल्ली_संदेश_BUFSIZ!
#endif

संरचना Vim_किल्ली_संदेश {
    char_u  length;
    char_u  chars[किल्ली_संदेश_BUFSIZ];  // contains Vim encoding
    तार्किक    csi_escape;
};

संरचना Vim_Resize_संदेश {
    पूर्णांक	    width;
    पूर्णांक	    height;
};

संरचना Vim_Scroll_Bar_संदेश {
    VimScrollBar *sb;
    दीर्घ    मूल्य;
    पूर्णांक	    stillDragging;
};

संरचना Vim_Menu_संदेश {
    vimmenu_T	*guiMenu;
};

संरचना Vim_Mouse_संदेश {
    पूर्णांक	    button;
    पूर्णांक	    x;
    पूर्णांक	    y;
    पूर्णांक	    repeated_click;
    int_u   modifiers;
};

संरचना Vim_Mouse_Moved_संदेश {
    पूर्णांक	    x;
    पूर्णांक	    y;
};

संरचना Vim_Focus_संदेश {
    तार्किक    active;
};

संरचना Vim_Refs_संदेश {
    BMessage   *message;
    तार्किक    changedir;
};

संरचना Vim_Tabline_संदेश {
    पूर्णांक	    index;
};

संरचना Vim_Tabline_Menu_संदेश {
    पूर्णांक	    index;
    पूर्णांक	    event;
};

संरचना Vim_संदेश {
    गणना Vim_संदेश_Type {
	किल्ली_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    संघ {
	संरचना Vim_किल्ली_संदेश    किल्ली_2;
	संरचना Vim_Resize_संदेश NewSize;
	संरचना Vim_Scroll_Bar_संदेश	Scroll;
	संरचना Vim_Menu_संदेश   Menu;
	संरचना Vim_Mouse_संदेश  Mouse;
	संरचना Vim_Mouse_Moved_संदेश	MouseMoved;
	संरचना Vim_Focus_संदेश  Focus;
	संरचना Vim_Refs_संदेश   Refs;
	संरचना Vim_Tabline_संदेश	Tabline;
	संरचना Vim_Tabline_Menu_संदेश    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

स्थिर संरचना specialkey
{
    uint16  Be_किल्ल्या;
#define किल्ली_3(a,b)    ((a)<<8|(b))
#define K(a)	    किल्ली_3(0,a)		// for ASCII codes
#define F(b)	    किल्ली_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_किल्ल्या[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_डावा_ARROW),	    'k', 'l'},
    {K(B_उजवा_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_घाला),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_किल्ली  11
    {F(B_F1_किल्ली),	'k', '1'},
    {F(B_F2_किल्ली),	'k', '2'},
    {F(B_F3_किल्ली),	'k', '3'},
    {F(B_F4_किल्ली),	'k', '4'},
    {F(B_F5_किल्ली),	'k', '5'},
    {F(B_F6_किल्ली),	'k', '6'},
    {F(B_F7_किल्ली),	'k', '7'},
    {F(B_F8_किल्ली),	'k', '8'},
    {F(B_F9_किल्ली),	'k', '9'},
    {F(B_F10_किल्ली),	'k', ';'},

    {F(B_F11_किल्ली),	'F', '1'},
    {F(B_F12_किल्ली),	'F', '2'},
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
    {F(B_PRINT_किल्ली),	    '%', '9'},

#जर 0
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

#define अंक_SPECIAL_किल्ल्या    ARRAY_LENGTH(special_किल्ल्या)

// ---------------- VimApp ----------------

    स्थिर शून्य
docd(BPath &path)
{
    mch_chdir((अक्षर *)path.Path());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	स्थिर शून्य
drop_callback(शून्य *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	स्थिर शून्य
RefsReceived(BMessage *m, तार्किक changedir)
{
    uint32 type;
    int32 मोजणी;

    m->PrintToStream();
    निवड (m->what) {
	स्थिती B_REFS_RECEIVED:
	स्थिती B_SIMPLE_DATA:
	    m->GetInfo("refs", &type, &मोजणी);
	    जर (type != B_REF_TYPE)
		जा bad;
	    थांबा;
	स्थिती B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &मोजणी);
	    जर (type != B_STRING_TYPE)
		जा bad;
	    जर (changedir) {
		अक्षर *स_नाम;
		जर (m->FindString("cwd", (स्थिरांक अक्षर **) &स_नाम) == B_OK) {
		    स_बदलो(स_नाम);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    थांबा;
	पूर्वनिर्धारित:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    परत;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(मोजणी * आकार(char_u *));
    पूर्णांक fname_index = 0;

    निवड (m->what) {
	स्थिती B_REFS_RECEIVED:
	स्थिती B_SIMPLE_DATA:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    साठी (पूर्णांक i = 0; i < मोजणी; ++i)
	    {
		entry_ref ref;
		जर (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, असत्य);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    जर (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    जर (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			जर (!changedir)
			    docd(path);
		    } अन्यथा {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			जर (fname == रिक्त)
			    fname = (char_u *)path.Path();
			fnames[fname_index++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = असत्य;
		}
	    }
	    थांबा;
	स्थिती B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    साठी (पूर्णांक i = 1; i < मोजणी; ++i)
	    {
		अक्षर *fname;

		जर (m->FindString("argv", i, (स्थिरांक अक्षर **) &fname) == B_OK) {
		    fnames[fname_index++] = vim_strsave((char_u *)fname);
		}
	    }
	    थांबा;
	पूर्वनिर्धारित:
	    // fprintf(stderr, "case default\n");
	    थांबा;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    जर (fname_index > 0) {
	handle_drop(fname_index, fnames, FALSE, drop_callback, रिक्त);

	setcursor();
	out_flush();
    } अन्यथा {
	vim_free(fnames);
    }
}

VimApp::VimApp(स्थिरांक अक्षर *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(रिक्त)
{
}

VimApp::~VimApp()
{
}

    शून्य
VimApp::ReadyToRun()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(संक_विघ्न, संक_छोड़ो);
    mch_signal(SIGQUIT, संक_छोड़ो);
}

    शून्य
VimApp::ArgvReceived(int32 arg_argc, अक्षर **arg_argv)
{
    जर (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	जर (gui.vimWindow)
	    gui.vimWindow->Minimize(असत्य);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, सत्य);
    }
}

    शून्य
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    पूर्णांक limit = 15;
    पर्यंत (--limit >= 0 && (curbuf == रिक्त || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    जर (gui.vimWindow)
	gui.vimWindow->Minimize(असत्य);
    DetachCurrentMessage();
    SendRefs(m, सत्य);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    शून्य
VimApp::SendRefs(BMessage *m, तार्किक changedir)
{
    Vim_Refs_संदेश rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, Vim_संदेश::Refs, &rm, आकार(rm));
    //	calls ::RefsReceived
}

    शून्य
VimApp::MessageReceived(BMessage *m)
{
    निवड (m->what) {
	स्थिती 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    थांबा;
	स्थिती 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    थांबा;
	स्थिती B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (शून्य**)&panel);
		जर (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    थांबा;
	पूर्वनिर्धारित:
	    Inherited::MessageReceived(m);
	    थांबा;
    }
}

    तार्किक
VimApp::QuitRequested()
{
    (शून्य)Inherited::QuitRequested();
    परत असत्य;
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
    जर (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = रिक्त;
}

    शून्य
VimWindow::init()
{
    // Attach the VimFormView
    formView = नवीन VimFormView(Bounds());
    जर (formView != रिक्त) {
	AddChild(formView);
    }
}

#जर 0  //  disabled in zeta patch
    शून्य
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    निवड (m->what) {
	स्थिती B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		जर (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} अन्यथा {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    थांबा;
	पूर्वनिर्धारित:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    शून्य
VimWindow::WindowActivated(तार्किक active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    जर (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(सत्य);

    संरचना Vim_Focus_संदेश fm;
    fm.active = active;

    write_port(gui.vdcmp, Vim_संदेश::Focus, &fm, आकार(fm));
}

    तार्किक
VimWindow::QuitRequested()
{
    संरचना Vim_किल्ली_संदेश km;
    km.length = 5;
    स_नकल((अक्षर *)km.chars, "\033:qa\r", km.length);
    km.csi_escape = असत्य;
    write_port(gui.vdcmp, Vim_संदेश::किल्ली_2, &km, आकार(km));
    परत असत्य;
}
    शून्य
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    तार्किक should_propagate = सत्य;

    निवड (m->what)
    {
	स्थिती B_किल्ली_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		जर (scancode == B_ENTER && (beModifiers & B_डावा_COMMAND_किल्ली))
		    {
			should_propagate = असत्य;
			जर (हे->is_fullscreen)
			    {
				हे->is_fullscreen = असत्य;
				ResizeTo(हे->saved_frame.Width(), हे->saved_frame.Height());
				MoveTo(हे->saved_frame.डावा, हे->saved_frame.शीर्ष);
				SetLook(हे->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } अन्यथा {
				हे->saved_frame = Frame();
				हे->saved_look = Look();
				हे->is_fullscreen = सत्य;
				BScreen s(हे);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().डावा, s.Frame().शीर्ष);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    जर (should_propagate)
	Inherited::DispatchMessage(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect frame):
    BView(frame, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_FRAME_EVENTS),
    menuBar(रिक्त),
#ifdef FEAT_TOOLBAR
    toolBar(रिक्त),
#endif
#ifdef FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_ओळ(रिक्त),
#endif
    textArea(रिक्त)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    जर (menuBar) {
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
    delete tab_ओळ;
#endif

    जर (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = रिक्त;
}

    शून्य
VimFormView::init(BRect frame)
{
    menuBar = नवीन BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = नवीन VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tab_ओळ = नवीन Vim_Tab_ओळ(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tab_ओळ);
#endif

    BRect remaining = frame;
    textArea = नवीन VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = हे;
}

#ifdef FEAT_TOOLBAR
    दशांश
VimFormView::ToolbarHeight() स्थिरांक
{
    परत toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    दशांश
VimFormView::TablineHeight() स्थिरांक
{
    परत (tab_ओळ && Is_Showing_Tab_ओळ()) ? tab_ओळ->TablineHeight() : 0.;
}
#endif

    शून्य
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(संक_विघ्न, संक_छोड़ो);
    mch_signal(SIGQUIT, संक_छोड़ो);

    जर (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.शीर्ष += MenuHeight();
	menuBar->ResizeTo(remaining.उजवा, remaining.शीर्ष);
	gui.menu_height = (पूर्णांक) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.डावा, remaining.शीर्ष);
	toolBar->ResizeTo(remaining.उजवा, ToolbarHeight());
	remaining.शीर्ष += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tab_ओळ->MoveTo(remaining.डावा, remaining.शीर्ष);
	tab_ओळ->ResizeTo(remaining.उजवा + 1, TablineHeight());
	remaining.शीर्ष += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.डावा, remaining.शीर्ष);
    }


    Inherited::AllAttached();
}

    शून्य
VimFormView::FrameResized(दशांश new_width, दशांश new_height)
{
    संरचना Vim_Resize_संदेश sm;
    पूर्णांक adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (पूर्णांक) new_width;
    sm.height = (पूर्णांक) new_height;
    adjust_w = ((पूर्णांक)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((पूर्णांक)new_height - gui_get_base_height()) % gui.char_height;

    जर (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, Vim_संदेश::Resize, &sm, आकार(sm));
    //	calls gui_resize_shell(new_width, new_height);

    परत;

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
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_आदान_METHOD_AWARE
#अन्यथा
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouse_Drag_Event_मोजणी(0)
{
#ifdef FEAT_MBYTE_IME
    IMData.messenger = रिक्त;
    IMData.message = रिक्त;
#endif
    init(frame);
}

VimTextAreaView::~VimTextAreaView()
{
    gui.vimTextArea = रिक्त;
}

    शून्य
VimTextAreaView::init(BRect frame)
{
    // set up global var for fast access
    gui.vimTextArea = हे;

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

    शून्य
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
    Set_खालचा_Color(rgb);
    FillRect(updateRect, B_SOLID_खालचा);
    gui_redraw((पूर्णांक) updateRect.डावा, (पूर्णांक) updateRect.शीर्ष,
	    (पूर्णांक) (updateRect.Width() + PEN_WIDTH), (पूर्णांक) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    Set_खालचा_Color(rgb);

    जर (updateRect.डावा < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.डावा, updateRect.शीर्ष,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_खालचा);
    जर (updateRect.शीर्ष < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.डावा, updateRect.शीर्ष,
		    updateRect.उजवा, FILL_Y(0)-PEN_WIDTH), B_SOLID_खालचा);
    जर (updateRect.उजवा >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((पूर्णांक)Columns), updateRect.शीर्ष,
		    updateRect.उजवा, updateRect.bottom), B_SOLID_खालचा);
    जर (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.डावा, FILL_Y((पूर्णांक)Rows),
		    updateRect.उजवा, updateRect.bottom), B_SOLID_खालचा);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    शून्य
VimTextAreaView::किल्ली_Down(स्थिरांक अक्षर *bytes, int32 अंक_Bytes)
{
    संरचना Vim_किल्ली_संदेश km;
    char_u *dest = km.chars;

    तार्किक canHaveVimModifiers = असत्य;

    BMessage *संदेश = Window()->CurrentMessage();
    प्रतिज्ञा(संदेश);
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
    जर (अंक_Bytes > 1) {
	// This cannot be a special key
	जर (अंक_Bytes > किल्ली_संदेश_BUFSIZ)
	    अंक_Bytes = किल्ली_संदेश_BUFSIZ;	    //	should never happen... ???
	km.length = अंक_Bytes;
	स_नकल((अक्षर *)dest, bytes, अंक_Bytes);
	km.csi_escape = सत्य;
    } अन्यथा {
	int32 scancode = 0;
	संदेश->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	संदेश->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	पूर्णांक len = 0;
	km.length = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	प्रतिज्ञा(B_BACKSPACE <= 0x20);
	प्रतिज्ञा(B_DELETE == 0x7F);
	जर (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		अंक_Bytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    जर (beModifiers & B_CONTROL_किल्ली) {
		पूर्णांक index = किल्ली_Map->normal_map[scancode];
		पूर्णांक new_अंक_Bytes = किल्ली_Map_Chars[index];
		char_u *newBytes = (char_u *)&किल्ली_Map_Chars[index + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		जर (new_अंक_Bytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    जा notspecial;
		}
		bytes = (अक्षर *)newBytes;
	    }
	    canHaveVimModifiers = सत्य;

	    uint16 beoskey;
	    पूर्णांक first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    जर (अंक_Bytes == 0 || bytes[0] == B_FUNCTION_किल्ली) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_किल्ली;
		last = अंक_SPECIAL_किल्ल्या;
	    } अन्यथा जर (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } अन्यथा {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_किल्ली;
	    }

	    साठी (पूर्णांक i = first; i < last; i++) {
		जर (special_किल्ल्या[i].Be_किल्ल्या == beoskey) {
		    string[0] = CSI;
		    string[1] = special_किल्ल्या[i].vim_code0;
		    string[2] = special_किल्ल्या[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	जर (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#जर 0
	जर (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#अन्यथा
	    जर (canHaveVimModifiers)
#endif
	    {
		पूर्णांक modifiers;
		modifiers = 0;
		जर (beModifiers & B_SHIFT_किल्ली)
		    modifiers |= MOD_MASK_SHIFT;
		जर (beModifiers & B_CONTROL_किल्ली)
		    modifiers |= MOD_MASK_CTRL;
		जर (beModifiers & B_OPTION_किल्ली)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		पूर्णांक किल्ली;
		जर (string[0] == CSI && len == 3)
		    किल्ली = TO_SPECIAL(string[1], string[2]);
		अन्यथा
		    किल्ली = string[0];
		किल्ली = simplify_किल्ली(किल्ली, &modifiers);
		जर (IS_SPECIAL(किल्ली))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(किल्ली);
		    string[2] = K_THIRD(किल्ली);
		    len = 3;
		}
		अन्यथा
		{
		    string[0] = किल्ली;
		    len = 1;
		}

		जर (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.length = 3;
		}
	    }
	स_नकल((अक्षर *)dest, string, len);
	km.length += len;
	km.csi_escape = असत्य;
    }

    write_port(gui.vdcmp, Vim_संदेश::किल्ली_2, &km, आकार(km));

    /*
     * blank out the pointer if necessary
     */
    जर (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(सत्य);
	gui.pointer_hidden = TRUE;
    }
}
शून्य
VimTextAreaView::guiSendMouseEvent(
	पूर्णांक	button,
	पूर्णांक	x,
	पूर्णांक	y,
	पूर्णांक	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_संदेश mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, Vim_संदेश::Mouse, &mm, आकार(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    जर (gui.pointer_hidden)
    {
	guiBlankMouse(असत्य);
	gui.pointer_hidden = FALSE;
    }
}

शून्य
VimTextAreaView::guiMouseMoved(
	पूर्णांक	x,
	पूर्णांक	y)
{
    Vim_Mouse_Moved_संदेश mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, Vim_संदेश::MouseMoved, &mm, आकार(mm));

    जर (gui.pointer_hidden)
    {
	guiBlankMouse(असत्य);
	gui.pointer_hidden = FALSE;
    }
}

    शून्य
VimTextAreaView::guiBlankMouse(तार्किक should_hide)
{
    जर (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } अन्यथा {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    जर (beModifiers & B_SHIFT_किल्ली)
	vim_modifiers |= MOUSE_SHIFT;
    जर (beModifiers & B_CONTROL_किल्ली)
	vim_modifiers |= MOUSE_CTRL;
    जर (beModifiers & B_OPTION_किल्ली)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    परत vim_modifiers;
}

    शून्य
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    प्रतिज्ञा(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    पूर्णांक vimButton;

    जर (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_डावा;
    अन्यथा जर (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_उजवा;
    अन्यथा जर (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_मधला;
    अन्यथा
	परत;		// Unknown button

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

    शून्य
VimTextAreaView::MouseUp(BPoint point)
{
    vimMouseButton = 0;

    BMessage *m = Window()->CurrentMessage();
    प्रतिज्ञा(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, point.x, point.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::MouseUp(point);
}

    शून्य
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, स्थिरांक BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    जर (gui.pointer_hidden)
    {
	guiBlankMouse(असत्य);
	gui.pointer_hidden = FALSE;
    }

    जर (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	परत;
    }

    atomic_add(&mouse_Drag_Event_मोजणी, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    शून्य
VimTextAreaView::MessageReceived(BMessage *m)
{
    निवड (m->what) {
	स्थिती 'menu':
	    {
		Vim_Menu_संदेश mm;
		mm.guiMenu = रिक्त;  // in case no pointer in msg
		m->FindPointer("VimMenu", (शून्य **)&mm.guiMenu);
		write_port(gui.vdcmp, Vim_संदेश::Menu, &mm, आकार(mm));
	    }
	    थांबा;
	स्थिती B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		दशांश small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->Set_मूल्य(scb->मूल्य_2()+small*dy*3);
		scb->मूल्य_Changed(scb->मूल्य_2());
#जर 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->Set_मूल्य(scb->मूल्य_2()+small*dy);
		scb->मूल्य_Changed(scb->मूल्य_2());
#endif
	    }
	    थांबा;
#ifdef FEAT_MBYTE_IME
	स्थिती B_आदान_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		निवड(opcode)
		{
		    स्थिती B_आदान_METHOD_STARTED:
			जर (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = नवीन BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			थांबा;
		    स्थिती B_आदान_METHOD_CHANGED:
			{
			    BString str;
			    तार्किक confirmed;
			    जर (IMData.message) *(IMData.message) = *m;
			    अन्यथा	       IMData.message = नवीन BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    जर (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				संरचना Vim_किल्ली_संदेश km;
				km.csi_escape = सत्य;
				पूर्णांक clen;
				पूर्णांक i = 0;
				पर्यंत (i < str.Length())
				{
				    clen = utf_ptr2len(chars+i);
				    स_नकल(km.chars, chars+i, clen);
				    km.length = clen;
				    write_port(gui.vdcmp, Vim_संदेश::किल्ली_2, &km, आकार(km));
				    i += clen;
				}
			    }
			}
			थांबा;
		    स्थिती B_आदान_METHOD_LOCATION_REQUEST:
			{
			    BMessage संदेश(B_आदान_METHOD_EVENT);
			    संदेश.AddInt32("be:opcode", B_आदान_METHOD_LOCATION_REQUEST);
			    संदेश.AddPoint("be:location_reply", IMData.location);
			    संदेश.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&संदेश);
			}
			थांबा;
		    स्थिती B_आदान_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = रिक्त;
			IMData.message = रिक्त;
			थांबा;
		}
	    }
	    // TODO: sz: break here???
#endif
	पूर्वनिर्धारित:
	    जर (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(असत्य);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_किल्ली) != 0);
	    } अन्यथा {
		Inherited::MessageReceived(m);
	    }
	    थांबा;
    }
}

    पूर्णांक
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    जर (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	जर (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	परत OK;
    }
    परत FAIL;
}

    शून्य
VimTextAreaView::mchDrawString(पूर्णांक row, पूर्णांक col, char_u *s, पूर्णांक len, पूर्णांक flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    जर (!(flags & DRAW_TRANSP)) {
	पूर्णांक cells;
	cells = 0;
	साठी (पूर्णांक i=0; i<len; i++) {
	    पूर्णांक cn = utf_ptr2cells((char_u *)(s+i));
	    जर (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_खालचा);
    }

    BFont font;
    हे->GetFont(&font);
    जर (!font.IsFixed())
    {
	अक्षर* p = (अक्षर*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	पूर्णांक cells;
	पर्यंत ((p - (अक्षर*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    जर (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    जर (cells<4) lastpos += cells;
	    अन्यथा	lastpos++;
	    p += clen;
	}
    }
    अन्यथा
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((अक्षर*)s, len, where);
	जर (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((अक्षर*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    जर (flags & DRAW_UNDERL) {
	पूर्णांक cells;
	cells = 0;
	साठी (पूर्णांक i=0; i<len; i++) {
	    पूर्णांक cn = utf_ptr2cells((char_u *)(s+i));
	    जर (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	Stroke_ओळ(start, end);
    }
}

शून्य
VimTextAreaView::mchClearBlock(
	पूर्णांक	row1,
	पूर्णांक	col1,
	पूर्णांक	row2,
	पूर्णांक	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_खालचा);
}

    शून्य
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_खालचा);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    शून्य
VimTextAreaView::mch_Delete_ओळी(पूर्णांक row, पूर्णांक अंक_ओळी)
{
    BRect source, dest;
    source.डावा = FILL_X(gui.scroll_region_डावा);
    source.शीर्ष = FILL_Y(row + अंक_ओळी);
    source.उजवा = FILL_X(gui.scroll_region_उजवा + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.डावा = FILL_X(gui.scroll_region_डावा);
    dest.शीर्ष = FILL_Y(row);
    dest.उजवा = FILL_X(gui.scroll_region_उजवा + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - अंक_ओळी + 1) - PEN_WIDTH;

    जर (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - अंक_ओळी + 1,
		gui.scroll_region_डावा,
		gui.scroll_region_bot, gui.scroll_region_उजवा);


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
    शून्य
VimTextAreaView::mch_घाला_ओळी(पूर्णांक row, पूर्णांक अंक_ओळी)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.डावा = FILL_X(gui.scroll_region_डावा);
    source.शीर्ष = FILL_Y(row);
    source.उजवा = FILL_X(gui.scroll_region_उजवा + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - अंक_ओळी + 1) - PEN_WIDTH;

    dest.डावा = FILL_X(gui.scroll_region_डावा);
    dest.शीर्ष = FILL_Y(row + अंक_ओळी);
    dest.उजवा = FILL_X(gui.scroll_region_उजवा + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    जर (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_डावा,
		row + अंक_ओळी - 1, gui.scroll_region_उजवा);

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
शून्य VimTextAreaView::DrawIMString(शून्य)
{
    स्थिर स्थिरांक rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    स्थिरांक अक्षर* s;
    पूर्णांक len;
    BMessage* संदेश = IMData.message;
    जर (!संदेश)
	परत;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.मोजणी, W_WIDTH(curwin), GUI_MON_NOCLEAR);
    तार्किक confirmed = असत्य;
    संदेश->FindBool("be:confirmed", &confirmed);
    जर (confirmed)
	परत;
    rgb_color hcolor = वरचा_Color(), lcolor = खालचा_Color();
    संदेश->FindString("be:string", &str);
    s = str.String();
    len = str.Length();
    Set_वरचा_Color(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    संदेश->FindInt32("be:selection", 0, &sel_start);
    संदेश->FindInt32("be:selection", 1, &sel_end);
    पूर्णांक clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    साठी (पूर्णांक i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	जर (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	जर (sel_start<=i && i<sel_end)
	{
	    Set_खालचा_Color(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	अन्यथा
	{
	    Set_खालचा_Color(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_खालचा);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.मोजणी = (पूर्णांक)pos.y;

    Set_वरचा_Color(hcolor);
    Set_खालचा_Color(lcolor);
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
	    "vim scrollbar", (BView *)रिक्त,
	    0.0, 10.0, posture),
    ignore_मूल्य(-1),
    scroll_Event_मोजणी(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    शून्य
VimScrollBar::मूल्य_Changed(दशांश new_मूल्य)
{
    जर (ignore_मूल्य >= 0.0 && new_मूल्य == ignore_मूल्य) {
	ignore_मूल्य = -1;
	परत;
    }
    ignore_मूल्य = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_add(&scroll_Event_मोजणी, 1);

    संरचना Vim_Scroll_Bar_संदेश sm;

    sm.sb = हे;
    sm.मूल्य = (दीर्घ) new_मूल्य;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, Vim_संदेश::ScrollBar, &sm, आकार(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    शून्य
VimScrollBar::MouseUp(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_add(&scroll_Event_मोजणी, 1);

    संरचना Vim_Scroll_Bar_संदेश sm;

    sm.sb = हे;
    sm.मूल्य = (दीर्घ) मूल्य_2();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, Vim_संदेश::ScrollBar, &sm, आकार(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    शून्य
VimScrollBar::Set_मूल्य(दशांश new_मूल्य)
{
    जर (new_मूल्य == मूल्य_2())
	परत;

    ignore_मूल्य = new_मूल्य;
    Inherited::Set_मूल्य(new_मूल्य);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(स्थिरांक VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(स्थिरांक BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(स्थिरांक VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    शून्य
VimFont::init()
{
    next = रिक्त;
    refcount = 1;
    name = रिक्त;
}

// ---------------- VimDialog ----------------

#जर defined(FEAT_GUI_DIALOG)

स्थिरांक अचिन्हित पूर्णांक  k_Vim_Dialog_Button_संदेश = 'VMDB';
स्थिरांक अचिन्हित पूर्णांक  kVimDialogIconStripeWidth = 30;
स्थिरांक अचिन्हित पूर्णांक  kVimDialogButtonsSpacingX = 9;
स्थिरांक अचिन्हित पूर्णांक  kVimDialogButtonsSpacingY = 4;
स्थिरांक अचिन्हित पूर्णांक  kVimDialogSpacingX = 6;
स्थिरांक अचिन्हित पूर्णांक  kVimDialogSpacingY = 10;
स्थिरांक अचिन्हित पूर्णांक  kVimDialogMinimalWidth  = 310;
स्थिरांक अचिन्हित पूर्णांक  kVimDialogMinimalHeight = 75;
स्थिरांक BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(पूर्णांक type, स्थिरांक अक्षर *title, स्थिरांक अक्षर *message,
	स्थिरांक अक्षर *buttons, पूर्णांक dfltbutton, स्थिरांक अक्षर *textfield, पूर्णांक ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_मूल्य(dfltbutton)
    , fMessageView(रिक्त)
    , f_आदान_Control(रिक्त)
    , f_आदान_मूल्य(textfield)
{
    //	master view
    VimDialog::View* view = नवीन VimDialog::View(Bounds());
    जर (view == रिक्त)
	परत;

    जर (title == रिक्त)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    दशांश maxButtonWidth  = 0;
    दशांश maxButtonHeight = 0;
    दशांश buttonsWidth	  = 0;
    दशांश buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    करा
    {
	int32 end = strButtons.FindFirst('\n');
	जर (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	जर (end == B_ERROR)
	    थांबा;

	strButtons.Remove(0, end + 1);
    } पर्यंत (सत्य);

    int32 buttons_मोजणी = fButtonsList.मोजणी_वस्तुसंच();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttons_मोजणी - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttons_मोजणी - 1);
    दशांश dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    दशांश dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    तार्किक vertical = (vim_strchr(p_go, GO_VERTICAL) != रिक्त) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    जर (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.डावा  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.शीर्ष   += kVimDialogSpacingY;
    rect.उजवा -= kVimDialogSpacingX;
    rect.bottom = rect.शीर्ष;
    fMessageView = नवीन BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_डावा | B_FOLLOW_शीर्ष, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(असत्य);
    fMessageView->MakeSelectable(असत्य);
    fMessageView->SetWordWrap(सत्य);
    AddChild(fMessageView);

    दशांश messageHeight = fMessageView->TextHeight(0, fMessageView->मोजणी_ओळी());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    जर (f_आदान_मूल्य != रिक्त) {
	rect.शीर्ष     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	f_आदान_Control = नवीन BTextControl(rect, "_iv_", रिक्त, f_आदान_मूल्य, रिक्त,
		B_FOLLOW_डावा | B_FOLLOW_शीर्ष, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_आदान_Control->TextView()->SetText(f_आदान_मूल्य);
	f_आदान_Control->TextView()->SetWordWrap(असत्य);
	AddChild(f_आदान_Control);

	दशांश width = 0.f, height = 0.f;
	f_आदान_Control->GetPreferredSize(&width, &height);
	f_आदान_Control->MakeFocus(सत्य);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    दशांश buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    साठी (int32 i = 0 ; i < buttons_मोजणी; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	जर (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} अन्यथा
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	जर (dfltbutton == i + 1) {
	    button->MakeDefault(सत्य);
	    button->MakeFocus(f_आदान_Control == रिक्त);
	}
    }
}

VimDialog::~VimDialog()
{
    जर (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    पूर्णांक
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    जर (fDialogSem < B_OK) {
	Quit();
	परत f_Dialog_मूल्य;
    }

    Show();

    पर्यंत (acquire_sem(fDialogSem) == B_INTERRUPTED);

    पूर्णांक ret_मूल्य = f_Dialog_मूल्य;
    जर (f_आदान_मूल्य != रिक्त)
	vim_strncpy((char_u*)f_आदान_मूल्य, (char_u*)f_आदान_Control->Text(), IOSIZE - 1);

    जर (Lock())
	Quit();

    परत ret_मूल्य;
}

शून्य VimDialog::MessageReceived(BMessage *संदेश)
{
    int32 which = 0;
    जर (संदेश->what != k_Vim_Dialog_Button_संदेश ||
	    संदेश->FindInt32("which", &which) != B_OK)
	परत BWindow::MessageReceived(संदेश);

    f_Dialog_मूल्य = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, स्थिरांक अक्षर* label)
{
    BMessage *message = नवीन BMessage(k_Vim_Dialog_Button_संदेश);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = नवीन BButton(rect, name.String(), label, message,
	    B_FOLLOW_उजवा | B_FOLLOW_BOTTOM);

    दशांश width = 0.f, height = 0.f;
    button->GetPreferredSize(&width, &height);
    button->ResizeTo(width, height);

    परत button;
}

VimDialog::View::View(BRect frame)
    :	BView(frame, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(रिक्त)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    delete fIconBitmap;
}

शून्य VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.उजवा = kVimDialogIconStripeWidth;
    Set_वरचा_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    जर (fIconBitmap == रिक्त)
	परत;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

शून्य VimDialog::View::InitIcon(int32 type)
{
    जर (type == VIM_GENERIC)
	परत;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    जर (status != B_OK) {
	ख_लिखो(मानक_त्रुटि, "Cannot retrieve app info:%s\n", म_त्रुटि(status));
	परत;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    जर (file.InitCheck() != B_OK) {
	ख_लिखो(मानक_त्रुटि, "App file assignment failed:%s\n",
		म_त्रुटि(file.InitCheck()));
	परत;
    }

    BResources resources(&file);
    जर (resources.InitCheck() != B_OK) {
	ख_लिखो(मानक_त्रुटि, "App server resources assignment failed:%s\n",
		म_त्रुटि(resources.InitCheck()));
	परत;
    }

    स्थिरांक अक्षर *name = "";
    निवड(type) {
	स्थिती VIM_ERROR:	    name = "stop"; थांबा;
	स्थिती VIM_WARNING:   name = "warn"; थांबा;
	स्थिती VIM_INFO:	    name = "info"; थांबा;
	स्थिती VIM_QUESTION:  name = "idea"; थांबा;
	पूर्वनिर्धारित: परत;
    }

    int32 iconSize = 32;
    fIconBitmap = नवीन BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), 0, B_RGBA32);
    जर (fIconBitmap == रिक्त || fIconBitmap->InitCheck() != B_OK) {
	ख_लिखो(मानक_त्रुटि, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == रिक्त) ? "null" : म_त्रुटि(fIconBitmap->InitCheck()));
	परत;
    }

    माप_प्रकार size = 0;
    स्थिरांक uint8* iconData = रिक्त;
    //	try vector icon first?
    iconData = (स्थिरांक uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &size);
    जर (iconData != रिक्त && BIconUtils::GetVectorIcon(iconData, size, fIconBitmap) == B_OK)
	परत;

    //	try bitmap icon now
    iconData = (स्थिरांक uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &size);
    जर (iconData == रिक्त) {
	ख_लिखो(मानक_त्रुटि, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = रिक्त;
	परत;
    }

    जर (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(iconData, iconSize, iconSize, iconSize, fIconBitmap);
}

स्थिरांक अचिन्हित पूर्णांक  k_Vim_Dialog_OKButton_संदेश = 'FDOK';
स्थिरांक अचिन्हित पूर्णांक  k_Vim_Dialog_Cancel_Button_संदेश = 'FDCN';
स्थिरांक अचिन्हित पूर्णांक  k_Vim_Dialog_Size_आदान_संदेश = 'SICH';
स्थिरांक अचिन्हित पूर्णांक  k_Vim_Dialog_Family_Select_संदेश = 'MSFM';
स्थिरांक अचिन्हित पूर्णांक  k_Vim_Dialog_Style_Select_संदेश = 'MSST';
स्थिरांक अचिन्हित पूर्णांक  k_Vim_Dialog_Size_Select_संदेश = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, दशांश* size)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_मूल्य(असत्य)
    , fFamily(family)
    , fStyle(style)
    , fSize(size)
    , fFontSize(*size)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_आदान(0)
{
    म_नकलन(fFontFamily, *family, B_FONT_FAMILY_LENGTH);
    म_नकलन(fFontStyle, *style, B_FONT_STYLE_LENGTH);

    //	"client" area view
    BBox *clientBox = नवीन BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.डावा_शीर्ष(), RC.डावा_शीर्ष());

    //	at first create all controls
    fPreview = नवीन BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = नवीन BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = नवीन BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = नवीन BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *labelSize = नवीन BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(labelSize);
    labelSize->ResizeToPreferred();

    fFamiliesList = नवीन BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = नवीन BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_डावा_उजवा, 0, असत्य, सत्य);
    clientBox->AddChild(scrollFamilies);

    fStylesList= नवीन BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = नवीन BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_डावा_उजवा, 0, असत्य, सत्य);
    clientBox->AddChild(scrollStyles);

    f_Sizes_आदान = नवीन BTextControl(rc, "inputSize", रिक्त, "???",
	    नवीन BMessage(k_Vim_Dialog_Size_आदान_संदेश));
    clientBox->AddChild(f_Sizes_आदान);
    f_Sizes_आदान->ResizeToPreferred();

    fSizesList = नवीन BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = नवीन BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_डावा_उजवा, 0, असत्य, सत्य);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = नवीन BButton(rc, "buttonOK", "OK",
			नवीन BMessage(k_Vim_Dialog_OKButton_संदेश));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = नवीन BButton(rc, "buttonCancel", "Cancel",
			नवीन BMessage(k_Vim_Dialog_Cancel_Button_संदेश));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    दशांश ओळ_Height = labelFamily->Bounds().Height();
    दशांश previewHeight = ओळ_Height * 3;
    दशांश offsetYLabels = previewHeight + kVimDialogSpacingY;
    दशांश offsetYLists = offsetYLabels + ओळ_Height + kVimDialogSpacingY / 2;
    दशांश offsetYSizes = offsetYLists + f_Sizes_आदान->Bounds().Height() + kVimDialogSpacingY / 2;
    दशांश listsHeight = ओळ_Height * 9;
    दशांश offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    दशांश maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    दशांश familiesWidth = labelFamily->Bounds().Width() * 5;
    दशांश offsetXStyles = familiesWidth + kVimDialogSpacingX;
    दशांश stylesWidth = labelStyle->Bounds().Width() * 4;
    दशांश offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    दशांश sizesWidth = labelSize->Bounds().Width() * 2;
    दशांश maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.डावा + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.शीर्ष + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    labelSize->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    दशांश insetX = f_Sizes_आदान->TextView()->Bounds().Width() - f_Sizes_आदान->Bounds().Width();
    दशांश insetY = f_Sizes_आदान->TextView()->Bounds().Width() - f_Sizes_आदान->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    f_Sizes_आदान->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    f_Sizes_आदान->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    f_Sizes_आदान->ResizeTo(sizesWidth, f_Sizes_आदान->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    पूर्णांक selIndex = -1;
    पूर्णांक मोजणी = मोजणी_font_families();
    साठी (पूर्णांक i = 0; i < मोजणी; i++) {
	font_family family;
	जर (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(नवीन BStringItem((स्थिरांक अक्षर*)family));
	    जर (म_भेदन(family, fFontFamily, B_FONT_FAMILY_LENGTH) == 0)
		selIndex = i;
	}
    }

    जर (selIndex >= 0) {
	fFamiliesList->Select(selIndex);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    selIndex = -1;
    साठी (पूर्णांक size = 8, index = 0; size <= 18; size++, index++) {
	BString str;
	str << size;
	fSizesList->AddItem(नवीन BStringItem(str));
	जर (size == fFontSize)
	    selIndex = index;

    }

    जर (selIndex >= 0) {
	fSizesList->Select(selIndex);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(नवीन BMessage(k_Vim_Dialog_Family_Select_संदेश));
    fStylesList->SetSelectionMessage(नवीन BMessage(k_Vim_Dialog_Style_Select_संदेश));
    fSizesList->SetSelectionMessage(नवीन BMessage(k_Vim_Dialog_Size_Select_संदेश));
    f_Sizes_आदान->SetModificationMessage(नवीन BMessage(k_Vim_Dialog_Size_आदान_संदेश));

    Update_Size_आदान_Preview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    जर (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    शून्य
VimSelectFontDialog::_CleanList(BListView* list)
{
    पर्यंत (0 < list->मोजणी_वस्तुसंच())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    तार्किक
VimSelectFontDialog::Go()
{
    जर (fStatus != B_OK) {
	Quit();
	परत NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    जर (fDialogSem < B_OK) {
	Quit();
	परत f_Dialog_मूल्य;
    }

    Show();

    पर्यंत (acquire_sem(fDialogSem) == B_INTERRUPTED);

    तार्किक ret_मूल्य = f_Dialog_मूल्य;

    जर (Lock())
	Quit();

    परत ret_मूल्य;
}


शून्य VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 selIndex = -1;
    int32 मोजणी = मोजणी_font_styles(fFontFamily);
    साठी (int32 i = 0; i < मोजणी; i++) {
	font_style style;
	uint32 flags = 0;
	जर (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(नवीन BStringItem((स्थिरांक अक्षर*)style));
	    जर (म_भेदन(style, fFontStyle, B_FONT_STYLE_LENGTH) == 0)
		selIndex = i;
	}
    }

    जर (selIndex >= 0) {
	fStylesList->Select(selIndex);
	fStylesList->ScrollToSelection();
    } अन्यथा
	fStylesList->Select(0);
}


शून्य VimSelectFontDialog::Update_Size_आदान_Preview()
{
    अक्षर buf[10] = {0};
    vim_snprintf(buf, आकार(buf), (अक्षर*)"%.0f", fFontSize);
    f_Sizes_आदान->SetText(buf);
}


शून्य VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_SIZE);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (पूर्णांक)fFontSize << " pt.";
    fPreview->SetText(str);
}


    तार्किक
VimSelectFontDialog::_UpdateFromListItem(BListView* list, अक्षर* text, पूर्णांक textSize)
{
    int32 index = list->CurrentSelection();
    जर (index < 0)
	परत असत्य;
    BStringItem* item = (BStringItem*)list->ItemAt(index);
    जर (item == रिक्त)
	परत असत्य;
    म_नकलन(text, item->Text(), textSize);
    परत सत्य;
}


शून्य VimSelectFontDialog::MessageReceived(BMessage *संदेश)
{
    निवड (संदेश->what) {
	स्थिती k_Vim_Dialog_OKButton_संदेश:
	    म_नकलन(*fFamily, fFontFamily, B_FONT_FAMILY_LENGTH);
	    म_नकलन(*fStyle, fFontStyle, B_FONT_STYLE_LENGTH);
	    *fSize = fFontSize;
	    f_Dialog_मूल्य = सत्य;
	स्थिती k_Vim_Dialog_Cancel_Button_संदेश:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    परत;
	स्थिती B_किल्ली_UP:
	    {
		int32 किल्ली = 0;
		जर (संदेश->FindInt32("raw_char", &किल्ली) == B_OK
			&& किल्ली == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    थांबा;

	स्थिती k_Vim_Dialog_Family_Select_संदेश:
	    जर (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_LENGTH)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    थांबा;
	स्थिती k_Vim_Dialog_Style_Select_संदेश:
	    जर (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_LENGTH))
		_UpdateFontPreview();
	    थांबा;
	स्थिती k_Vim_Dialog_Size_Select_संदेश:
	    {
		अक्षर buf[10] = {0};
		जर (_UpdateFromListItem(fSizesList, buf, आकार(buf))) {
		    दशांश size = म_से_भ(buf);
		    जर (size > 0.f) {
			fFontSize = size;
			Update_Size_आदान_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    थांबा;
	स्थिती k_Vim_Dialog_Size_आदान_संदेश:
	    {
		दशांश size = म_से_भ(f_Sizes_आदान->Text());
		जर (size > 0.f) {
		    fFontSize = size;
		    _UpdateFontPreview();
		}
	    }
	    थांबा;
	पूर्वनिर्धारित:
	    थांबा;
    }
    परत BWindow::MessageReceived(संदेश);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
स्थिर BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, स्थिरांक अक्षर *name) :
    BBox(frame, name, B_FOLLOW_डावा | B_FOLLOW_शीर्ष, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 मोजणी = fButtonsList.मोजणी_वस्तुसंच();
    साठी (int32 i = 0; i < मोजणी; i++)
	delete (BPictureButton*)fButtonsList.ItemAt(i);
    fButtonsList.MakeEmpty();

    delete normalButtonsBitmap;
    delete grayedButtonsBitmap;
    normalButtonsBitmap    = रिक्त;
    grayedButtonsBitmap  = रिक्त;
}

    शून्य
VimToolbar::AttachedToWindow()
{
    BBox::AttachedToWindow();

    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    दशांश
VimToolbar::ToolbarHeight() स्थिरांक
{
    दशांश size = रिक्त == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    परत size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    तार्किक
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    दशांश height = bitmap->Bounds().Height();
    दशांश width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->BitsLength() / 4;
    साठी (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    परत सत्य;
}

    तार्किक
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    जर (normalButtonsBitmap == रिक्त)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    जर (normalButtonsBitmap == रिक्त)
	परत असत्य;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = नवीन BBitmap(&archive);
    जर (grayedButtonsBitmap == रिक्त)
	परत असत्य;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    परत सत्य;
}

BBitmap *VimToolbar::LoadVimBitmap(स्थिरांक अक्षर* fileName)
{
    BBitmap *bitmap = रिक्त;

    पूर्णांक mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    जर (runtimePath != रिक्त && fileName != रिक्त) {
	BString strPath((अक्षर*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    जर (mustfree)
	vim_free(runtimePath);

    परत bitmap;
}

    तार्किक
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, तार्किक pressed)
{
    दशांश size = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, size, size), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.Set_वरचा_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, size - 1, size - 1);
    BRect destination(source);

    source.OffsetBy(size * index, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    जर (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	size += ButtonMargin * 2 - 1;
	view.Begin_ओळ_Array(4);
	view.Add_ओळ(BPoint(0, 0),	 BPoint(size, 0),    shadowColor);
	view.Add_ओळ(BPoint(size, 0),	 BPoint(size, size), shineColor);
	view.Add_ओळ(BPoint(size, size), BPoint(0, size),    shineColor);
	view.Add_ओळ(BPoint(0, size),	 BPoint(0, 0),	     shadowColor);
	view.End_ओळ_Array();
    }

    view.EndPicture();
    RemoveChild(&view);

    परत सत्य;
}

    तार्किक
VimToolbar::AddButton(int32 index, vimmenu_T *menu)
{
    BPictureButton *button = रिक्त;
    जर (!menu_is_separator(menu->name)) {
	दशांश size = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, size, size);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	जर (menu->iconfile == रिक्त && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, सत्य);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, असत्य);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, असत्य);
	} अन्यथा {

	    char_u buffer[MAXPATHL] = {0};
	    BBitmap *bitmap = रिक्त;

	    जर (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, buffer, (अक्षर*)"png");
		bitmap = BTranslationUtils::GetBitmap((अक्षर*)buffer);
	    }

	    जर (bitmap == रिक्त && gui_find_bitmap(menu->name, buffer, (अक्षर*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((अक्षर*)buffer);

	    जर (bitmap == रिक्त)
		bitmap = नवीन BBitmap(BRect(0, 0, size, size), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, सत्य);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, असत्य);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, असत्य);

	    delete bitmap;
	}

	button = नवीन BPictureButton(frame, (अक्षर*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    तार्किक result = fButtonsList.AddItem(button, index);
    InvalidateLayout();
    परत result;
}

    तार्किक
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    जर (menu->button) {
	जर (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = रिक्त;
	}
    }
    परत सत्य;
}

    तार्किक
VimToolbar::GrayButton(vimmenu_T *menu, पूर्णांक grey)
{
    जर (menu->button) {
	int32 index = fButtonsList.IndexOf(menu->button);
	जर (index >= 0)
	    menu->button->SetEnabled(grey ? असत्य : सत्य);
    }
    परत सत्य;
}

    शून्य
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 मोजणी = fButtonsList.मोजणी_वस्तुसंच();
    साठी (int32 i = 0; i < मोजणी; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	जर (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} अन्यथा
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#जर defined(FEAT_GUI_TABLINE)

    दशांश
Vim_Tab_ओळ::TablineHeight() स्थिरांक
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    परत TabHeight(); //  + ToolbarMargin;
}

शून्य
Vim_Tab_ओळ::MouseDown(BPoint point)
{
    जर (!gui_mch_showing_tabline())
	परत;

    BMessage *m = Window()->CurrentMessage();
    प्रतिज्ञा(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    पूर्णांक index = 0; //  0 means here - no tab found
    साठी (पूर्णांक i = 0; i < मोजणी_Tabs(); i++) {
	जर (TabFrame(i).Contains(point)) {
	    index = i + 1; //  indexes are 1-based
	    थांबा;
	}
    }

    पूर्णांक event = -1;

    जर ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    अन्यथा जर (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = index > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    अन्यथा जर (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* काढा_Up_Menu = नवीन BPopUpMenu("tabLineContextMenu", असत्य, असत्य);
	काढा_Up_Menu->AddItem(नवीन BMenuItem(_("Close tabi R"), नवीन BMessage(TABLINE_MENU_CLOSE)));
	काढा_Up_Menu->AddItem(नवीन BMenuItem(_("New tab    T"), नवीन BMessage(TABLINE_MENU_NEW)));
	काढा_Up_Menu->AddItem(नवीन BMenuItem(_("Open tab..."), नवीन BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = काढा_Up_Menu->Go(point);
	जर (item != रिक्त) {
	    event = item->Command();
	}

	delete काढा_Up_Menu;

    } अन्यथा {
	//  default processing
	BTabView::MouseDown(point);
	परत;
    }

    जर (event < 0)
	परत;

    Vim_Tabline_Menu_संदेश tmm;
    tmm.index = index;
    tmm.event = event;
    write_port(gui.vdcmp, Vim_संदेश::TablineMenu, &tmm, आकार(tmm));
}

शून्य
Vim_Tab_ओळ::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_ओळ *tab_ओळ = gui.vimForm->Tab_ओळ();
    जर (tab_ओळ != रिक्त) {

	int32 i = 0;
	साठी (; i < tab_ओळ->मोजणी_Tabs(); i++)
	    जर (हे == tab_ओळ->TabAt(i))
		थांबा;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	जर (i < tab_ओळ->मोजणी_Tabs()) {
	    Vim_Tabline_संदेश पंचांग;
	    पंचांग.index = i + 1;
	    write_port(gui.vdcmp, Vim_संदेश::Tabline, &पंचांग, आकार(पंचांग));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
स्थिर अक्षर appsig[] = "application/x-vnd.Haiku-Vim-8";
किल्ली_map *किल्ली_Map;
अक्षर *किल्ली_Map_Chars;
पूर्णांक main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    संरचना Vim_संदेश vm;
    int32 what;
    ssize_t size;

    size = read_port_etc(gui.vdcmp, &what, &vm, आकार(vm),
	    B_TIMEOUT, timeout);

    जर (size >= 0) {
	निवड (what) {
	    स्थिती Vim_संदेश::किल्ली_2:
		{
		    char_u *string = vm.u.किल्ली_2.chars;
		    पूर्णांक len = vm.u.किल्ली_2.length;
		    जर (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_आदान_buf();
			got_int = TRUE;
		    }

		    जर (vm.u.किल्ली_2.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			पूर्णांक	i;
			char_u	buf[2];

			साठी (i = 0; i < len; ++i)
			{
			    add_to_आदान_buf(string + i, 1);
			    जर (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (पूर्णांक)KE_CSI;
				add_to_आदान_buf(buf, 2);
			    }
			}
		    }
#अन्यथा
			add_to_आदान_buf_csi(string, len);
#endif
		    अन्यथा
			add_to_आदान_buf(string, len);
		}
		थांबा;
	    स्थिती Vim_संदेश::Resize:
		gui_resize_shell(vm.u.NewSize.width, vm.u.NewSize.height);
		थांबा;
	    स्थिती Vim_संदेश::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_मोजणी =
			atomic_add(&vm.u.Scroll.sb->scroll_Event_मोजणी, -1);
		    जर (old_मोजणी <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.मूल्य, vm.u.Scroll.stillDragging);
		}
		थांबा;
#जर defined(FEAT_MENU)
	    स्थिती Vim_संदेश::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		थांबा;
#endif
	    स्थिती Vim_संदेश::Mouse:
		{
		    int32 old_मोजणी;
		    जर (vm.u.Mouse.button == MOUSE_DRAG)
			old_मोजणी =
			    atomic_add(&gui.vimTextArea->mouse_Drag_Event_मोजणी, -1);
		    अन्यथा
			old_मोजणी = 0;
		    जर (old_मोजणी <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		थांबा;
	    स्थिती Vim_संदेश::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		थांबा;
	    स्थिती Vim_संदेश::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		जर (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		थांबा;
	    स्थिती Vim_संदेश::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		थांबा;
	    स्थिती Vim_संदेश::Tabline:
		send_tabline_event(vm.u.Tabline.index);
		थांबा;
	    स्थिती Vim_संदेश::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.index, vm.u.TablineMenu.event);
		थांबा;
	    पूर्वनिर्धारित:
		//  unrecognised message, ignore it
		थांबा;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    परत size;
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

    पूर्णांक
vim_lock_screen()
{
    परत !gui.vimWindow || gui.vimWindow->Lock();
}

    शून्य
vim_unlock_screen()
{
    जर (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#जर RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(शून्य *args)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    परत 0;
}

#अन्यथा

    int32
call_main(शून्य *args)
{
    संरचना MainArgs *ma = (MainArgs *)args;

    परत main(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    शून्य
gui_mch_prepare(
	पूर्णांक	*argc,
	अक्षर	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    जर (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(रिक्त), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_मोजणी, "vim VDCMP");

#जर RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, रिक्त);
	जर (tid >= B_OK) {
	    resume_thread(tid);
	} अन्यथा {
	    getout(1);
	}
#अन्यथा
	MainArgs ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	जर (tid >= B_OK) {
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
	    (शून्य)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    निकास(main_exitcode);
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
    जर (!isatty(0)) {
	संरचना stat stat_stdin, stat_dev_null;

	जर (ख_स्थिति(0, &stat_stdin) == -1 ||
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
    पूर्णांक
gui_mch_init_check(शून्य)
{
    परत OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    पूर्णांक
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (पूर्णांक) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (पूर्णांक) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    जर (gui.vdcmp < B_OK)
	परत FAIL;
    get_किल्ली_map(&किल्ली_Map, &किल्ली_Map_Chars);

    gui.vimWindow = नवीन VimWindow();	// hidden and locked
    जर (!gui.vimWindow)
	परत FAIL;

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

    परत OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    शून्य
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    जर (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    पूर्णांक
gui_mch_open()
{
    जर (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    जर (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	परत OK;
    }

    परत FAIL;
}

    शून्य
gui_mch_exit(पूर्णांक vim_exitcode)
{
    जर (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#जर !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    जर (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(असत्य);

	main_exitcode = vim_exitcode;
#जर RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#अन्यथा
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
    पूर्णांक
gui_mch_get_winpos(पूर्णांक *x, पूर्णांक *y)
{
    जर (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (पूर्णांक)r.डावा;
	*y = (पूर्णांक)r.शीर्ष;
	परत OK;
    }
    अन्यथा
	परत FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    शून्य
gui_mch_set_winpos(पूर्णांक x, पूर्णांक y)
{
    जर (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
शून्य
gui_mch_set_shellsize(
	पूर्णांक	width,
	पूर्णांक	height,
	पूर्णांक	min_width,
	पूर्णांक	min_height,
	पूर्णांक	base_width,
	पूर्णांक	base_height,
	पूर्णांक	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    जर (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	दशांश minWidth, maxWidth, minHeight, maxHeight;

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

शून्य
gui_mch_get_screen_dimensions(
	पूर्णांक	*screen_w,
	पूर्णांक	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	जर (screen.IsValid()) {
	    frame = screen.Frame();
	} अन्यथा {
	    frame.उजवा = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (पूर्णांक) frame.उजवा - 2 * gui.scrollbar_width - 20;
    *screen_h = (पूर्णांक) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

शून्य
gui_mch_set_text_area_pos(
	पूर्णांक	x,
	पूर्णांक	y,
	पूर्णांक	w,
	पूर्णांक	h)
{
    जर (!gui.vimTextArea)
	परत;

    जर (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	जर (gui.vimForm->Tab_ओळ() != रिक्त) {
	    gui.vimForm->Tab_ओळ()->ResizeTo(w, gui.vimForm->TablineHeight());
	}
#endif // FEAT_GUI_TABLINE

	gui.vimWindow->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

शून्य
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	पूर्णांक	flag)
{
    VimScrollBar *vsb = sb->id;
    जर (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	जर (flag) {
	    जर (vsb->IsHidden()) {
		vsb->Show();
	    }
	} अन्यथा {
	    जर (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

शून्य
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	पूर्णांक	val,
	पूर्णांक	size,
	पूर्णांक	max)
{
    जर (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	जर (max == 0) {
	    s->Set_मूल्य(0);
	    s->SetRange(0.0, 0.0);
	} अन्यथा {
	    s->SetProportion((दशांश)size / (max + 1.0));
	    s->SetSteps(1.0, size > 5 ? size - 2 : size);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - size;
#endif
	    जर (max < s->मूल्य_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->Set_मूल्य(val);
		s->SetRange(0.0, max);
	    } अन्यथा {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->Set_मूल्य(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

शून्य
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	पूर्णांक	x,
	पूर्णांक	y,
	पूर्णांक	w,
	पूर्णांक	h)
{
    जर (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	दशांश vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	जर (winb.उजवा-(x+w)<w) vsbx = winb.उजवा - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    पूर्णांक
gui_mch_get_scrollbar_xpadding(शून्य)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    परत 0;
}

    पूर्णांक
gui_mch_get_scrollbar_ypadding(शून्य)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    परत 0;
}

शून्य
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	पूर्णांक	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = नवीन VimScrollBar(sb, posture);
    जर (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#जर defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
शून्य
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    जर (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    पूर्णांक
gui_mch_is_blink_off(शून्य)
{
    परत FALSE;
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

स्थिर पूर्णांक	blink_state = BLINK_NONE;
स्थिर long_u	    blink_waittime = 700;
स्थिर long_u	    blink_ontime = 400;
स्थिर long_u	    blink_offtime = 250;
स्थिर पूर्णांक  blink_timer = 0;

शून्य
gui_mch_set_blinking(
	दीर्घ	waittime,
	दीर्घ	on,
	दीर्घ	off)
{
    // TODO
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    शून्य
gui_mch_stop_blink(पूर्णांक may_call_gui_update_cursor)
{
    // TODO
    जर (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    जर (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    शून्य
gui_mch_start_blink()
{
    // TODO
    जर (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    जर (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
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
पूर्णांक
gui_mch_init_font(
	char_u	    *font_name,
	पूर्णांक	    fontset)
{
    जर (gui.vimWindow->Lock())
    {
	पूर्णांक rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	परत rc;
    }

    परत FAIL;
}


    पूर्णांक
gui_mch_adjust_charsize()
{
    परत FAIL;
}


    पूर्णांक
gui_mch_font_dialog(font_family* family, font_style* style, दशांश* size)
{
#जर defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = नवीन VimSelectFontDialog(family, style, size);
    तार्किक ret = dialog->Go();
	delete dialog;
	परत ret;
#अन्यथा
    परत NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	पूर्णांक	    giveErrorIfMissing)
{
    स्थिर VimFont *fontList = रिक्त;

    जर (!gui.in_use)	//  can't do this when GUI not running
	परत NOFONT;

    //	storage for locally modified name;
    स्थिरांक पूर्णांक buff_size = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 20;
    स्थिर अक्षर font_name[buff_size] = {0};
    font_family family = {0};
    font_style	style  = {0};
    दशांश size = 0.f;

    जर (name == 0 && be_fixed_font == 0) {
	जर (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	परत NOFONT;
    }

    तार्किक useSelectGUI = असत्य;
    जर (name != रिक्त)
	जर (STRCMP(name, "*") == 0) {
	    useSelectGUI = सत्य;
	    STRNCPY(font_name, hl_get_font_name(), buff_size);
	} अन्यथा
	    STRNCPY(font_name, name, buff_size);

    जर (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	size = be_fixed_font->Size();
	vim_snprintf(font_name, buff_size,
	    (अक्षर*)"%s/%s/%.0f", family, style, size);
    }

    //	replace underscores with spaces
    अक्षर* end = 0;
    पर्यंत (end = म_अक्षर((अक्षर *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    स्थिर अक्षर buff[buff_size] = {0};
    STRNCPY(buff, font_name, buff_size);
    STRNCPY(family, म_मोहर(buff, "/\0"), B_FONT_FAMILY_LENGTH);
    अक्षर* style_s = म_मोहर(0, "/\0");
    जर (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_LENGTH);
    size = म_से_भ((style_s != 0) ? म_मोहर(0, "/\0") : "0");

    जर (useSelectGUI) {
	जर (gui_mch_font_dialog(&family, &style, &size) == NOFONT)
	    परत FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_size,
		(अक्षर*)"%s/%s/%.0f", family, style, size);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	जर (new_p_guifont != रिक्त) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    साठी ( ; *new_p_guifont; ++new_p_guifont)
		जर (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    साठी (flp = fontList; flp; flp = flp->next) {
	जर (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    परत (GuiFont)flp;
	}
    }

    VimFont *font = नवीन VimFont();
    font->name = vim_strsave((char_u*)font_name);

    जर (मोजणी_font_styles(family) <= 0) {
	जर (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	परत NOFONT;
    }

    //	Remember font in the static list for later use
    font->next = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    जर (size > 0.f)
	font->SetSize(size);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    परत (GuiFont)font;
}

/*
 * Set the current text font.
 */
शून्य
gui_mch_set_font(
	GuiFont	font)
{
    जर (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (पूर्णांक) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (पूर्णांक)(fh.ascent + 0.9999)
	    + (पूर्णांक)(fh.descent + 0.9999) + (पूर्णांक)(fh.leading + 0.9999);
	gui.char_ascent = (पूर्णांक)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
शून्य
gui_mch_free_font(
	GuiFont	font)
{
    जर (font == NOFONT)
	परत;
    VimFont *f = (VimFont *)font;
    जर (--f->refcount <= 0) {
	जर (f->refcount < 0)
	    ख_लिखो(मानक_त्रुटि, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    जर (name == रिक्त)
	परत रिक्त;
    परत vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    पूर्णांक
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
	gui.char_height = (पूर्णांक)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (पूर्णांक)(fh.ascent + 0.5) + p_linespace / 2;
    }
    परत OK;
}

    शून्य
gui_mch_getmouse(पूर्णांक *x, पूर्णांक *y)
{
    ख_लिखो(मानक_त्रुटि, "gui_mch_getmouse");

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

    शून्य
gui_mch_mousehide(पूर्णांक hide)
{
    ख_लिखो(मानक_त्रुटि, "gui_mch_getmouse");
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
    परत gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
शून्य
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    जर (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_वरचा_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text background color.
 */
शून्य
gui_mch_set_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    जर (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_खालचा_Color(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    शून्य
gui_mch_set_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

शून्य
gui_mch_draw_string(
	पूर्णांक	row,
	पूर्णांक	col,
	char_u	*s,
	पूर्णांक	len,
	पूर्णांक	flags)
{
    जर (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(पूर्णांक r, पूर्णांक g, पूर्णांक b)
{
    परत gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
पूर्णांक
gui_mch_haskey(
	char_u	*name)
{
    पूर्णांक i;

    साठी (i = 0; special_किल्ल्या[i].Be_किल्ल्या != 0; i++)
	जर (name[0] == special_किल्ल्या[i].vim_code0 &&
		name[1] == special_किल्ल्या[i].vim_code1)
	    परत OK;
    परत FAIL;
}

    शून्य
gui_mch_beep()
{
    ::beep();
}

    शून्य
gui_mch_flash(पूर्णांक msec)
{
    // Do a visual beep by reversing the foreground and background colors

    जर (gui.vimWindow->Lock()) {
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
शून्य
gui_mch_invert_rectangle(
	पूर्णांक	r,
	पूर्णांक	c,
	पूर्णांक	nr,
	पूर्णांक	nc)
{
    BRect rect;
    rect.डावा = FILL_X(c);
    rect.शीर्ष = FILL_Y(r);
    rect.उजवा = rect.डावा + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.शीर्ष + nr * gui.char_height - PEN_WIDTH;

    जर (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimWindow->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    शून्य
gui_mch_iconify()
{
    जर (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(सत्य);
	gui.vimWindow->Unlock();
    }
}

#जर defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    शून्य
gui_mch_set_foreground(शून्य)
{
    // TODO
}
#endif

/*
 * Set the window title
 */
शून्य
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    जर (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((अक्षर *)title);
	gui.vimWindow->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    शून्य
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.डावा = FILL_X(gui.col);
    r.शीर्ष = FILL_Y(gui.row);
    पूर्णांक cells = utf_off2cells(ओळ_Offset[gui.row] + gui.col, 100); // TODO-TODO
    जर (cells>=4) cells = 1;
    r.उजवा = r.डावा + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.शीर्ष + gui.char_height - PEN_WIDTH;

    जर (gui.vimWindow->Lock()) {
	gui.vimTextArea->StrokeRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
शून्य
gui_mch_draw_part_cursor(
	पूर्णांक	w,
	पूर्णांक	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.डावा =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_उजवा ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.उजवा = r.डावा + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.शीर्ष = r.bottom - h + PEN_WIDTH;

    जर (gui.vimWindow->Lock()) {
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
    शून्य
gui_mch_update()
{
    gui_mch_flush();
    पर्यंत (port_मोजणी(gui.vdcmp) > 0 &&
	    !vim_is_आदान_buf_full() &&
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
पूर्णांक
gui_mch_wait_for_chars(
	दीर्घ	wtime)
{
    पूर्णांक		focus;
    bigtime_t	until, timeout;
    status_t	st;

    जर (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    अन्यथा
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    साठी (;;)
    {
	// Stop or start blinking when focus changes
	जर (gui.in_focus != focus)
	{
	    जर (gui.in_focus)
		gui_mch_start_blink();
	    अन्यथा
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
	जर (did_add_timer)
	    // Need to recompute the waiting time.
	    थांबा;
# endif
# ifdef FEAT_JOB_CHANNEL
	जर (has_any_channel())
	{
	    जर (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	अन्यथा जर (wtime < 0)
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

	जर (आदान_available())
	    परत OK;
	जर (st < B_OK)		// includes B_TIMED_OUT
	    परत FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	जर (wtime >= 0)
	{
	    timeout = until - system_time();
	    जर (timeout < 0)
		थांबा;
	}
    }
    परत FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    शून्य
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    जर (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    परत;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
शून्य
gui_mch_clear_block(
	पूर्णांक	row1,
	पूर्णांक	col1,
	पूर्णांक	row2,
	पूर्णांक	col2)
{
    जर (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    शून्य
gui_mch_clear_all()
{
    जर (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
शून्य
gui_mch_delete_ओळी(
	पूर्णांक	row,
	पूर्णांक	अंक_ओळी)
{
    gui.vimTextArea->mch_Delete_ओळी(row, अंक_ओळी);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
शून्य
gui_mch_घाला_ओळी(
	पूर्णांक	row,
	पूर्णांक	अंक_ओळी)
{
    gui.vimTextArea->mch_घाला_ओळी(row, अंक_ओळी);
}

#जर defined(FEAT_MENU)
/*
 * Menu stuff.
 */

शून्य
gui_mch_enable_menu(
	पूर्णांक	flag)
{
    जर (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

शून्य
gui_mch_set_menu_pos(
	पूर्णांक	x,
	पूर्णांक	y,
	पूर्णांक	w,
	पूर्णांक	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
शून्य
gui_mch_add_menu(
	vimmenu_T   *menu,
	पूर्णांक	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    जर (menu_is_popup(menu->name) && parent == रिक्त) {
	BPopUpMenu* काढा_Up_Menu = नवीन BPopUpMenu((स्थिरांक अक्षर*)menu->name, असत्य, असत्य);
	menu->submenu_id = काढा_Up_Menu;
	menu->id = रिक्त;
	परत;
    }

    जर (!menu_is_menubar(menu->name)
	    || (parent != रिक्त && parent->submenu_id == रिक्त))
	परत;

    जर (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	जर ( parent )
	    tmp = parent->submenu_id;
	अन्यथा
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	जर ( ! tmp->FindItem((स्थिरांक अक्षर *) menu->dname)) {

	    BMenu *bmenu = नवीन BMenu((अक्षर *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((स्थिरांक अक्षर *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    शून्य
gui_mch_toggle_tearoffs(पूर्णांक enable)
{
    // no tearoff menus
}

    स्थिर BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = नवीन BMessage('menu');
    m->AddPointer("VimMenu", (शून्य *)menu);

    परत m;
}

/*
 * Add a menu item to a menu
 */
शून्य
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	पूर्णांक	idx)
{
    पूर्णांक	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    जर (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	जर (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    जर (toolbar != रिक्त) {
		toolbar->AddButton(idx, menu);
	    }
	} अन्यथा
#endif

	जर (parent->submenu_id != रिक्त || menu_is_popup(parent->name)) {
	    जर (menu_is_separator(menu->name)) {
		BSeparatorItem *item = नवीन BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = रिक्त;
	    }
	    अन्यथा {
		BMenuItem *item = नवीन BMenuItem((अक्षर *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((अक्षर) menu->mnemonic);
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = रिक्त;
	    }
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
शून्य
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    जर (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	जर (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    जर (toolbar != रिक्त) {
		toolbar->RemoveButton(menu);
	    }
	} अन्यथा
#endif
	{
	    प्रतिज्ञा(menu->submenu_id == रिक्त || menu->submenu_id->मोजणी_वस्तुसंच() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    जर (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		जर (bmenu == gui.vimForm->MenuBar() && bmenu->मोजणी_वस्तुसंच() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = रिक्त;
	    menu->submenu_id = रिक्त;

	    gui.menu_height = (पूर्णांक) gui.vimForm->MenuHeight();
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
शून्य
gui_mch_menu_grey(
	vimmenu_T   *menu,
	पूर्णांक	grey)
{
#ifdef FEAT_TOOLBAR
    जर (menu->parent && menu_is_toolbar(menu->parent->name)) {
	जर (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    जर (toolbar != रिक्त) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } अन्यथा
#endif
    जर (menu->id != रिक्त)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
शून्य
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	पूर्णांक	hidden)
{
    जर (menu->id != रिक्त)
	menu->id->SetEnabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    शून्य
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    शून्य
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    जर (!menu_is_popup(menu->name) || menu->submenu_id == रिक्त)
	परत;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    जर (popupMenu == रिक्त)
	परत;

    BPoint point;
    जर (gui.vimWindow->Lock()) {
	uint32 buttons = 0;
	gui.vimTextArea->GetMouse(&point, &buttons);
	gui.vimTextArea->ConvertToScreen(&point);
	gui.vimWindow->Unlock();
    }
    popupMenu->Go(point, सत्य);
}

#endif // FEAT_MENU

// Mouse stuff

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
अक्षर textplain[] = "text/plain";
अक्षर vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    शून्य
clip_mch_request_selection(Clipboard_T *cbd)
{
    जर (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->Data();
	// m->PrintToStream();

	char_u *string = रिक्त;
	ssize_t stringlen = -1;

	जर (m->FindData(textplain, B_MIME_TYPE,
		    (स्थिरांक शून्य **)&string, &stringlen) == B_OK
		|| m->FindString("text", (स्थिरांक अक्षर **)&string) == B_OK)
	{
	    जर (stringlen == -1)
		stringlen = STRLEN(string);

	    पूर्णांक type;
	    अक्षर *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    जर (m->FindData(vimselectiontype, B_MIME_TYPE,
			(स्थिरांक शून्य **)&seltype, &seltypelen) == B_OK)
	    {
		निवड (*seltype)
		{
		    पूर्वनिर्धारित:
		    स्थिती 'L':	type = MLINE;	थांबा;
		    स्थिती 'C':	type = MCHAR;	थांबा;
#ifdef FEAT_VISUAL
		    स्थिती 'B':	type = MBLOCK;	थांबा;
#endif
		}
	    }
	    अन्यथा
	    {
		// Otherwise use heuristic as documented
		type = स_प्रथम(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (दीर्घ)stringlen, cbd);
	}
	be_clipboard->Unlock();
    }
}
/*
 * Make vim the owner of the current selection.
 */
    शून्य
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    पूर्णांक
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    परत FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    शून्य
clip_mch_set_selection(Clipboard_T *cbd)
{
    जर (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->Data();
	प्रतिज्ञा(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = रिक्त;
	long_u	मोजणी;
	पूर्णांक type;

	type = clip_convert_selection(&str, &मोजणी, cbd);

	जर (type < 0)
	    परत;

	m->AddData(textplain, B_MIME_TYPE, (शून्य *)str, मोजणी);

	// Add type of selection
	अक्षर	vtype;
	निवड (type)
	{
	    पूर्वनिर्धारित:
	    स्थिती MLINE:    vtype = 'L';    थांबा;
	    स्थिती MCHAR:    vtype = 'C';    थांबा;
#ifdef FEAT_VISUAL
	    स्थिती MBLOCK:   vtype = 'B';    थांबा;
#endif
	}
	m->AddData(vimselectiontype, B_MIME_TYPE, (शून्य *)&vtype, 1);

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
	पूर्णांक saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = नवीन BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    रिक्त, रिक्त, 0, असत्य,
	    नवीन BMessage((saving == TRUE) ? 'save' : 'open'), रिक्त, सत्य);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((अक्षर*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((स्थिरांक अक्षर*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    पर्यंत (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = रिक्त;
    status_t result = gui.vimApp->fBrowsedPath.InitCheck();
    जर (result == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } अन्यथा
	जर (result != B_NO_INIT) {
	    ख_लिखो(मानक_त्रुटि, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    result, म_त्रुटि(result));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = रिक्त;

    परत fileName;
}
#endif // FEAT_BROWSE


#जर defined(FEAT_GUI_DIALOG)

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

पूर्णांक
gui_mch_dialog(
	पूर्णांक	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	पूर्णांक	 dfltbutton,
	char_u	*textfield,
	पूर्णांक ex_cmd)
{
    VimDialog *dialog = नवीन VimDialog(type, (अक्षर*)title, (अक्षर*)message,
	    (अक्षर*)buttons, dfltbutton, (अक्षर*)textfield, ex_cmd);
    तार्किक ret = dialog->Go();
    delete dialog;
	परत ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    परत ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    शून्य
gui_mch_setmouse(पूर्णांक x, पूर्णांक y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    शून्य
im_set_position(पूर्णांक row, पूर्णांक col)
{
    जर (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    परत;
}
#endif

    शून्य
gui_mch_show_toolbar(पूर्णांक showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    शून्य
gui_mch_set_toolbar_pos(पूर्णांक x, पूर्णांक y, पूर्णांक w, पूर्णांक h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    जर (toolbar != रिक्त) {
	जर (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#जर defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    शून्य
gui_mch_show_tabline(पूर्णांक showit)
{
    Vim_Tab_ओळ *tab_ओळ = gui.vimForm->Tab_ओळ();

    जर (tab_ओळ == रिक्त)
	परत;

    जर (!showit != !gui.vimForm->Is_Showing_Tab_ओळ()) {
	gui.vimForm->Set_Showing_Tab_ओळ(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    शून्य
gui_mch_set_tabline_pos(पूर्णांक x, पूर्णांक y, पूर्णांक w, पूर्णांक h)
{
    Vim_Tab_ओळ *tab_ओळ = gui.vimForm->Tab_ओळ();
    जर (tab_ओळ != रिक्त) {
	जर (gui.vimWindow->Lock()) {
	    tab_ओळ->MoveTo(x, y);
	    tab_ओळ->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    पूर्णांक
gui_mch_showing_tabline()
{
    Vim_Tab_ओळ *tab_ओळ = gui.vimForm->Tab_ओळ();
    परत tab_ओळ != रिक्त && gui.vimForm->Is_Showing_Tab_ओळ();
}

/*
 * Update the labels of the tabline.
 */
    शून्य
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    पूर्णांक	    nr = 0;
    पूर्णांक	    curtabidx = 0;

    Vim_Tab_ओळ *tab_ओळ = gui.vimForm->Tab_ओळ();

    जर (tab_ओळ == रिक्त)
	परत;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    साठी (tp = first_tabpage; tp != रिक्त; tp = tp->tp_next, ++nr) {
	जर (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_ओळ->TabAt(nr);

	जर (tab == रिक्त) {
	    tab = नवीन Vim_Tab_ओळ::VimTab();
	    tab_ओळ->AddTab(रिक्त, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((स्थिरांक अक्षर*)NameBuff);
	tab_ओळ->Invalidate();
    }

    // Remove any old labels.
    पर्यंत (nr < tab_ओळ->मोजणी_Tabs())
	tab_ओळ->RemoveTab(nr);

    जर (tab_ओळ->Selection() != curtabidx)
	tab_ओळ->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    शून्य
gui_mch_set_curtab(पूर्णांक nr)
{
    Vim_Tab_ओळ *tab_ओळ = gui.vimForm->Tab_ओळ();
    जर (tab_ओळ == रिक्त)
	परत;

    gui.vimWindow->Lock();

    जर (tab_ओळ->Selection() != nr -1)
	tab_ओळ->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
