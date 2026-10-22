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
صنف Vim_متن_Area_View;
صنف Vim_دري;
صنف VimToolbar;
صنف Vim_Tab_سٽ;

خارجي ڪنجي_map *ڪنجي_Map;
خارجي حرف *ڪنجي_Map_Chars;

خارجي صحيح مکيه(صحيح argc, حرف **argv);

#جيڪڏهن_وصف_نه_ٿيل B_MAX_پورٽ_ڳڻپ
#وصف_ڪريو B_MAX_پورٽ_ڳڻپ    255
#جيڪڏهن_پورو

// VimApp seems comparable to the X "vimShell"
صنف VimApp: عام BApplication
{
    تعريفنوع BApplication Inherited;
    عام:
    VimApp(ثابت حرف *appsig);
    ~VimApp();

    // callbacks:
#إذا 0
    مجازي فراغ Dispatch_پيغام(BMessage *m, BHandler *h)
    {
	m->Print_منزل_وهڪرو();
	Inherited::Dispatch_پيغام(m, h);
    }
#جيڪڏهن_پورو
    مجازي فراغ Ready_منزل_لاڳيتو();
    مجازي فراغ ArgvReceived(int32 argc, حرف **argv);
    مجازي فراغ RefsReceived(BMessage *m);
    مجازي منطقي QuitRequested();
    مجازي فراغ پيغام_Received(BMessage *m);

    ساكن فراغ SendRefs(BMessage *m, منطقي changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	f_Browsed_رستو;
    خاص:
};

صنف Vim_دري: عام BWindow
{
    تعريفنوع BWindow Inherited;
    عام:
    Vim_دري();
    ~Vim_دري();

    مجازي فراغ Dispatch_پيغام(BMessage *m, BHandler *h);
    مجازي فراغ دري_Activated(منطقي سرگرم);
    مجازي منطقي QuitRequested();

    VimFormView	    *formView;

    خاص:
    فراغ init();

    منطقي is_fullscreen = خطأ;
    BRect saved_چوکٺو;
    دري_look saved_look;
};

صنف VimFormView: عام BView
{
    تعريفنوع BView Inherited;
    عام:
    VimFormView(BRect چوکٺو);
    ~VimFormView();

    // callbacks:
    مجازي فراغ AllAttached();
    مجازي فراغ چوکٺو_Resized(عائم new_ويڪر, عائم new_اوچائي);

#وصف_ڪريو MENUBAR_MARGIN	1
    عائم Menu_اوچائي() ثابت
    { أرجع menuBar ? menuBar->چوکٺو_2().اوچائي_2() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ثابت
    { أرجع menuBar; }

    خاص:
    فراغ init(BRect);

    BMenuBar	    *menuBar;
    Vim_متن_Area_View *متن_Area;

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    عام:
    عائم Toolbar_اوچائي() ثابت;
    VimToolbar *ToolBar() ثابت
    { أرجع toolBar; }
    خاص:
    VimToolbar	    *toolBar;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
    عام:
    Vim_Tab_سٽ *Tab_سٽ() ثابت	{ أرجع tab_سٽ; }
    منطقي Is_Showing_Tab_سٽ() ثابت { أرجع showing_Tab_سٽ; }
    فراغ سيٽ_Showing_Tab_سٽ(منطقي showing) { showing_Tab_سٽ = showing;	}
    عائم Tabline_اوچائي() ثابت;
    خاص:
    Vim_Tab_سٽ	*tab_سٽ;
    صحيح	showing_Tab_سٽ;
#جيڪڏهن_پورو
};

صنف Vim_متن_Area_View: عام BView
{
    تعريفنوع BView Inherited;
    عام:
    Vim_متن_Area_View(BRect چوکٺو);
    ~Vim_متن_Area_View();

    // callbacks:
    مجازي فراغ Draw(BRect updateRect);
    مجازي فراغ ڪنجي_هيٺ_ڏانهن(ثابت حرف *بائيٽون, int32 انگ_بائيٽون_2);
    مجازي فراغ Mouse_هيٺ_ڏانهن(BPoint نقطو);
    مجازي فراغ Mouse_مٿي(BPoint نقطو);
    مجازي فراغ MouseMoved(BPoint نقطو, uint32 transit, ثابت BMessage *پيغام);
    مجازي فراغ پيغام_Received(BMessage *m);

    // own functions:
    صحيح mchInitFont(char_u *name);
    فراغ mchDrawString(صحيح صف, صحيح کانو, char_u *s, صحيح len, صحيح flags);
    فراغ mch_Clear_بلاڪ(صحيح row1, صحيح col1, صحيح row2, صحيح col2);
    فراغ mchClearAll();
    فراغ mch_Delete_سٽون(صحيح صف, صحيح انگ_سٽون);
    فراغ mch_وجهو_سٽون(صحيح صف, صحيح انگ_سٽون);

    ساكن فراغ guiSendMouseEvent(صحيح button, صحيح x, صحيح y, صحيح repeated_click, int_u modifiers);
    ساكن فراغ guiMouseMoved(صحيح x, صحيح y);
    ساكن فراغ guiBlankMouse(منطقي should_hide);
    ساكن int_u mouse_Modifiers_منزل_Vim(int32 beModifiers);

    int32 mouse_Drag_Event_ڳڻپ;

#جيڪڏهن_وصف_ٿيل FEAT_MBYTE_IME
    فراغ DrawIMString(فراغ);
#جيڪڏهن_پورو

    خاص:
    فراغ init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#جيڪڏهن_وصف_ٿيل FEAT_MBYTE_IME
    بنية {
	BMessenger* messenger;
	BMessage* پيغام;
	BPoint location;
	صحيح صف;
	صحيح کانو;
	صحيح ڳڻپ;
    } IMData;
#جيڪڏهن_پورو
};

صنف VimScrollBar: عام BScrollBar
{
    تعريفنوع BScrollBar Inherited;
    عام:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    مجازي فراغ قدر_بدليو_2(عائم new_قدر);
    مجازي فراغ Mouse_مٿي(BPoint where);
    فراغ سيٽ_قدر(عائم newval);
    scrollbar_T *getGsb()
    { أرجع gsb; }

    int32	scroll_Event_ڳڻپ;

    خاص:
    scrollbar_T *gsb;
    عائم   ignore_قدر;
};


#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR

صنف VimToolbar : عام BBox
{
    ساكن BBitmap *عام_Buttons_Bitmap;
    ساكن BBitmap *grayedButtonsBitmap;

    BBitmap *ڀريو_Vim_Bitmap(ثابت حرف* fileName);
    منطقي GetPictureFromBitmap(BPicture *picture_منزل, int32 اشاريو, BBitmap *bitmapFrom, منطقي pressed);
    منطقي Modify_Bitmap_منزل_Grayed(BBitmap *bitmap);

    BList fButtonsList;
    فراغ InvalidateLayout();

    عام:
    VimToolbar(BRect چوکٺو, ثابت حرف * name);
    ~VimToolbar();

    منطقي PrepareButtonBitmaps();

    منطقي جوڙيو_Button(int32 اشاريو, vimmenu_T *menu);
    منطقي RemoveButton(vimmenu_T *menu);
    منطقي ناسي_Button(vimmenu_T *menu, صحيح grey);

    عائم Toolbar_اوچائي() ثابت;
    مجازي فراغ Attached_منزل_دري();
};

BBitmap *VimToolbar::عام_Buttons_Bitmap  = عدم;
BBitmap *VimToolbar::grayedButtonsBitmap  = عدم;

ثابت عائم ToolbarMargin = 3.;
ثابت عائم ButtonMargin  = 3.;

#جيڪڏهن_پورو //FEAT_TOOLBAR

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE

صنف Vim_Tab_سٽ : عام BTabView
{
    عام:
	صنف VimTab : عام BTab {
	    عام:
		VimTab() : BTab(جديد BView(BRect(), "-Empty-", 0, 0)) {}

	    مجازي فراغ Select(BView* owner);
	};

	Vim_Tab_سٽ(BRect r) : BTabView(r, "vimTabLine", B_ويڪر_FROM_LABEL,
	       B_FOLLOW_کاٻو | B_FOLLOW_چوٽي | B_FOLLOW_ساڄو, B_WILL_DRAW | B_چوکٺو_EVENTS) {}

    عائم Tabline_اوچائي() ثابت;
    مجازي فراغ Mouse_هيٺ_ڏانهن(BPoint نقطو);
};

#جيڪڏهن_پورو //FEAT_GUI_TABLINE


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

    VimFont *ايندڙ;
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
	View(BRect چوکٺو);
	~View();

	مجازي فراغ Draw(BRect updateRect);
	فراغ InitIcon(int32 type);

	خاص:
	BBitmap*    fIconBitmap;
    };

    VimDialog(صحيح type, ثابت حرف *title, ثابت حرف *پيغام,
	    ثابت حرف *buttons, صحيح dfltbutton, ثابت حرف *textfield,
	    صحيح ex_cmd);
    ~VimDialog();

    صحيح Go();

    مجازي فراغ پيغام_Received(BMessage *نياپو);

    خاص:
    sem_id	    fDialogSem;
    صحيح		    f_Dialog_قدر;
    BList	    fButtonsList;
    BTextView*	    f_پيغام_View;
    BTextControl*   f_داخلا_Control;
    ثابت حرف*	    f_داخلا_قدر;
};

صنف VimSelectFontDialog : عام BWindow
{
    تعريفنوع BWindow Inherited;

    فراغ _CleanList(BListView* list);
    فراغ _UpdateFontStyles();
    فراغ Update_سائيز_داخلا_Preview();
    فراغ _UpdateFontPreview();
    منطقي Update_From_List_شي(BListView* list, حرف* متن, صحيح متن_سائيز);
    عام:

    VimSelectFontDialog(font_family* family, font_style* style, عائم* سائيز);
    ~VimSelectFontDialog();

    منطقي Go();

    مجازي فراغ پيغام_Received(BMessage *نياپو);

    خاص:
    حالت_t	    f_حالت;
    sem_id	    fDialogSem;
    منطقي	    f_Dialog_قدر;
    font_family*    fFamily;
    font_style*	    fStyle;
    عائم*	    f_سائيز;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    عائم	    f_Font_سائيز;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_داخلا;
};

#جيڪڏهن_پورو // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

بنية Main_دليلون {
    صحيح	     argc;
    حرف    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#وصف_ڪريو	ڪنجي_نياپو_BUFSIZ	7
#إذا ڪنجي_نياپو_BUFSIZ < MAX_ڪنجي_ڪوڊ_LEN
#غلطي Increase ڪنجي_نياپو_BUFSIZ!
#جيڪڏهن_پورو

بنية Vim_ڪنجي_نياپو {
    char_u  ڊيگهائي;
    char_u  chars[ڪنجي_نياپو_BUFSIZ];  // contains Vim encoding
    منطقي    csi_escape;
};

بنية Vim_Resize_نياپو {
    صحيح	    ويڪر;
    صحيح	    اوچائي;
};

بنية Vim_Scroll_Bar_نياپو {
    VimScrollBar *sb;
    ڊگهو    قدر;
    صحيح	    stillDragging;
};

بنية Vim_Menu_نياپو {
    vimmenu_T	*guiMenu;
};

بنية Vim_Mouse_نياپو {
    صحيح	    button;
    صحيح	    x;
    صحيح	    y;
    صحيح	    repeated_click;
    int_u   modifiers;
};

بنية Vim_Mouse_Moved_نياپو {
    صحيح	    x;
    صحيح	    y;
};

بنية Vim_Focus_نياپو {
    منطقي    سرگرم;
};

بنية Vim_Refs_نياپو {
    BMessage   *پيغام;
    منطقي    changedir;
};

بنية Vim_Tabline_نياپو {
    صحيح	    اشاريو;
};

بنية Vim_Tabline_Menu_نياپو {
    صحيح	    اشاريو;
    صحيح	    event;
};

بنية Vim_نياپو {
    تعداد Vim_نياپو_Type {
	ڪنجي_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    اتحاد {
	بنية Vim_ڪنجي_نياپو    ڪنجي_2;
	بنية Vim_Resize_نياپو New_سائيز;
	بنية Vim_Scroll_Bar_نياپو	Scroll;
	بنية Vim_Menu_نياپو   Menu;
	بنية Vim_Mouse_نياپو  Mouse;
	بنية Vim_Mouse_Moved_نياپو	MouseMoved;
	بنية Vim_Focus_نياپو  Focus;
	بنية Vim_Refs_نياپو   Refs;
	بنية Vim_Tabline_نياپو	Tabline;
	بنية Vim_Tabline_Menu_نياپو    TablineMenu;
    } u;
};

#وصف_ڪريو RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#وصف_ڪريو GUI_منزل_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ساكن بنية specialkey
{
    uint16  Be_ڪنجيون;
#وصف_ڪريو ڪنجي_3(a,b)    ((a)<<8|(b))
#وصف_ڪريو K(a)	    ڪنجي_3(0,a)		// for ASCII codes
#وصف_ڪريو F(b)	    ڪنجي_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_ڪنجيون[] =
{
    {K(B_مٿي_ARROW),	'k', 'u'},
    {K(B_هيٺ_ڏانهن_ARROW),	    'k', 'd'},
    {K(B_کاٻو_ARROW),	    'k', 'l'},
    {K(B_ساڄو_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_وجهو),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_پڇاڙي),		'@', '7'},
    {K(B_صفحو_مٿي),	'k', 'P'},	// XK_Prior
    {K(B_صفحو_هيٺ_ڏانهن),	    'k', 'N'},	    // XK_Next,

#وصف_ڪريو پهريون_FUNCTION_ڪنجي  11
    {F(B_F1_ڪنجي),	'k', '1'},
    {F(B_F2_ڪنجي),	'k', '2'},
    {F(B_F3_ڪنجي),	'k', '3'},
    {F(B_F4_ڪنجي),	'k', '4'},
    {F(B_F5_ڪنجي),	'k', '5'},
    {F(B_F6_ڪنجي),	'k', '6'},
    {F(B_F7_ڪنجي),	'k', '7'},
    {F(B_F8_ڪنجي),	'k', '8'},
    {F(B_F9_ڪنجي),	'k', '9'},
    {F(B_F10_ڪنجي),	'k', ';'},

    {F(B_F11_ڪنجي),	'F', '1'},
    {F(B_F12_ڪنجي),	'F', '2'},
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
    {F(B_PRINT_ڪنجي),	    '%', '9'},

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
#جيڪڏهن_پورو

    // End of list marker:
    {0,		    0, 0}
};

#وصف_ڪريو انگ_SPECIAL_ڪنجيون    ARRAY_ڊيگهائي(special_ڪنجيون)

// ---------------- VimApp ----------------

    ساكن فراغ
docd(BPath &رستو)
{
    mch_chdir((حرف *)رستو.رستو_2());
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
    int32 ڳڻپ;

    m->Print_منزل_وهڪرو();
    بدل (m->what) {
	حالة B_REFS_RECEIVED:
	حالة B_SIMPLE_معلومات:
	    m->GetInfo("refs", &type, &ڳڻپ);
	    إذا (type != B_REF_TYPE)
		اذهب bad;
	    اكسر;
	حالة B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &ڳڻپ);
	    إذا (type != B_STRING_TYPE)
		اذهب bad;
	    إذا (changedir) {
		حرف *dirname;
		إذا (m->ڳوليو_String("cwd", (ثابت حرف **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    اكسر;
	افتراضي:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    ميٽيو m;
	    أرجع;
    }

#جيڪڏهن_وصف_ٿيل FEAT_VISUAL
    reset_VIsual();
#جيڪڏهن_پورو

    char_u  **fnames;
    fnames = (char_u **) alloc(ڳڻپ * حجم(char_u *));
    صحيح fname_اشاريو = 0;

    بدل (m->what) {
	حالة B_REFS_RECEIVED:
	حالة B_SIMPLE_معلومات:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    لكل (صحيح i = 0; i < ڳڻپ; ++i)
	    {
		داخلا_ref ref;
		إذا (m->ڳوليو_Ref("refs", i, &ref) == B_OK) {
		    BEntry داخلا(&ref, خطأ);
		    BPath رستو;
		    داخلا.Get_رستو(&رستو);

		    // Change to parent directory?
		    إذا (changedir) {
			BPath parentpath;
			رستو.Get_والد(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    إذا (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			إذا (!changedir)
			    docd(رستو);
		    } وإلا {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)رستو.رستو_2(), IObuff);
			إذا (fname == عدم)
			    fname = (char_u *)رستو.رستو_2();
			fnames[fname_اشاريو++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = خطأ;
		}
	    }
	    اكسر;
	حالة B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    لكل (صحيح i = 1; i < ڳڻپ; ++i)
	    {
		حرف *fname;

		إذا (m->ڳوليو_String("argv", i, (ثابت حرف **) &fname) == B_OK) {
		    fnames[fname_اشاريو++] = vim_strsave((char_u *)fname);
		}
	    }
	    اكسر;
	افتراضي:
	    // fprintf(stderr, "case default\n");
	    اكسر;
    }

    ميٽيو m;

    // Handle the drop, :edit to get to the file
    إذا (fname_اشاريو > 0) {
	handle_drop(fname_اشاريو, fnames, FALSE, drop_callback, عدم);

	setcursor();
	ٻاهريون_flush();
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
VimApp::Ready_منزل_لاڳيتو()
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
	إذا (gui.vim_دري)
	    gui.vim_دري->Minimize(خطأ);
	BMessage *m = Current_پيغام();
	Detach_Current_پيغام();
	SendRefs(m, سچ);
    }
}

    فراغ
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    صحيح حد_بندي = 15;
    طالما (--حد_بندي >= 0 && (curbuf == عدم || curbuf->b_شروعات_ffc == 0))
	snooze(100000);    //  0.1 s
    إذا (gui.vim_دري)
	gui.vim_دري->Minimize(خطأ);
    Detach_Current_پيغام();
    SendRefs(m, سچ);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    فراغ
VimApp::SendRefs(BMessage *m, منطقي changedir)
{
    Vim_Refs_نياپو rm;
    rm.پيغام = m;
    rm.changedir = changedir;

    write_پورٽ(gui.vdcmp, Vim_نياپو::Refs, &rm, حجم(rm));
    //	calls ::RefsReceived
}

    فراغ
VimApp::پيغام_Received(BMessage *m)
{
    بدل (m->what) {
	حالة 'save':
	    {
		داخلا_ref refDirectory;
		m->ڳوليو_Ref("directory", &refDirectory);
		f_Browsed_رستو.سيٽ_منزل(&refDirectory);
		BString strName;
		m->ڳوليو_String("name", &strName);
		f_Browsed_رستو.Append(strName.String());
	    }
	    اكسر;
	حالة 'open':
	    {
		داخلا_ref ref;
		m->ڳوليو_Ref("refs", &ref);
		f_Browsed_رستو.سيٽ_منزل(&ref);
	    }
	    اكسر;
	حالة B_CANCEL:
	    {
		BFilePanel *panel;
		m->ڳوليو_اشارو("source", (فراغ**)&panel);
		إذا (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    اكسر;
	افتراضي:
	    Inherited::پيغام_Received(m);
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

Vim_دري::Vim_دري():
    BWindow(BRect(40, 40, 150, 150),
	    "Vim",
	    B_TITLED_دري,
	    0,
	    B_CURRENT_WORKSPACE)

{
    init();
}

Vim_دري::~Vim_دري()
{
    إذا (formView) {
	Remove_ٻار(formView);
	ميٽيو formView;
    }
    gui.vim_دري = عدم;
}

    فراغ
Vim_دري::init()
{
    // Attach the VimFormView
    formView = جديد VimFormView(Bounds());
    إذا (formView != عدم) {
	جوڙيو_ٻار(formView);
    }
}

#إذا 0  //  disabled in zeta patch
    فراغ
Vim_دري::Dispatch_پيغام(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    بدل (m->what) {
	حالة B_MOUSE_مٿي:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		إذا (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->ڳوليو_نقطو("where", &where);
		    v->Mouse_مٿي(where);
		} وإلا {
		    Inherited::Dispatch_پيغام(m, h);
		}
	    }
	    اكسر;
	افتراضي:
	    Inherited::Dispatch_پيغام(m, h);
    }
}
#جيڪڏهن_پورو

    فراغ
Vim_دري::دري_Activated(منطقي سرگرم)
{
    Inherited::دري_Activated(سرگرم);
    // the textArea gets the keyboard action
    إذا (سرگرم && gui.vim_متن_Area)
	gui.vim_متن_Area->MakeFocus(سچ);

    بنية Vim_Focus_نياپو fm;
    fm.سرگرم = سرگرم;

    write_پورٽ(gui.vdcmp, Vim_نياپو::Focus, &fm, حجم(fm));
}

    منطقي
Vim_دري::QuitRequested()
{
    بنية Vim_ڪنجي_نياپو km;
    km.ڊيگهائي = 5;
    يادگيري_نقل((حرف *)km.chars, "\033:qa\r", km.ڊيگهائي);
    km.csi_escape = خطأ;
    write_پورٽ(gui.vdcmp, Vim_نياپو::ڪنجي_2, &km, حجم(km));
    أرجع خطأ;
}
    فراغ
Vim_دري::Dispatch_پيغام(BMessage *m, BHandler *h)
{
    منطقي should_propagate = سچ;

    بدل (m->what)
    {
	حالة B_ڪنجي_هيٺ_ڏانهن:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->ڳوليو_Int32("raw_char", &scancode);
		m->ڳوليو_Int32("modifiers", &beModifiers);

		إذا (scancode == B_ENTER && (beModifiers & B_کاٻو_COMMAND_ڪنجي))
		    {
			should_propagate = خطأ;
			إذا (هذا->is_fullscreen)
			    {
				هذا->is_fullscreen = خطأ;
				Resize_منزل(هذا->saved_چوکٺو.ويڪر_2(), هذا->saved_چوکٺو.اوچائي_2());
				Move_منزل(هذا->saved_چوکٺو.کاٻي, هذا->saved_چوکٺو.چوٽي);
				سيٽ_Look(هذا->saved_look);
				سيٽ_Flags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } وإلا {
				هذا->saved_چوکٺو = چوکٺو_2();
				هذا->saved_look = Look();
				هذا->is_fullscreen = سچ;
				BScreen s(هذا);
				سيٽ_Look(B_NO_BORDER_دري_LOOK);
				Resize_منزل(s.چوکٺو_2().ويڪر_2() + 1, s.چوکٺو_2().اوچائي_2() + 1);
				Move_منزل(s.چوکٺو_2().کاٻي, s.چوکٺو_2().چوٽي);
				سيٽ_Flags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    إذا (should_propagate)
	Inherited::Dispatch_پيغام(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect چوکٺو):
    BView(چوکٺو, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_چوکٺو_EVENTS),
    menuBar(عدم),
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    toolBar(عدم),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_سٽ(عدم),
#جيڪڏهن_پورو
    متن_Area(عدم)
{
    init(چوکٺو);
}

VimFormView::~VimFormView()
{
    إذا (menuBar) {
	Remove_ٻار(menuBar);
#جيڪڏهن_وصف_ٿيل never
	//  deleting the menuBar leads to SEGV on exit
	//  richard@whitequeen.com Jul 99
	ميٽيو menuBar;
#جيڪڏهن_پورو
    }

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    ميٽيو toolBar;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
    ميٽيو tab_سٽ;
#جيڪڏهن_پورو

    إذا (متن_Area) {
	Remove_ٻار(متن_Area);
	ميٽيو متن_Area;
    }
    gui.vimForm = عدم;
}

    فراغ
VimFormView::init(BRect چوکٺو)
{
    menuBar = جديد BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    جوڙيو_ٻار(menuBar);

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    toolBar = جديد VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    جوڙيو_ٻار(toolBar);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
    tab_سٽ = جديد Vim_Tab_سٽ(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    جوڙيو_ٻار(tab_سٽ);
#جيڪڏهن_پورو

    BRect بچيل = چوکٺو;
    متن_Area = جديد Vim_متن_Area_View(بچيل);
    جوڙيو_ٻار(متن_Area);
    // The textArea will be resized later when menus are added

    gui.vimForm = هذا;
}

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    عائم
VimFormView::Toolbar_اوچائي() ثابت
{
    أرجع toolBar ? toolBar->Toolbar_اوچائي() : 0.;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
    عائم
VimFormView::Tabline_اوچائي() ثابت
{
    أرجع (tab_سٽ && Is_Showing_Tab_سٽ()) ? tab_سٽ->Tabline_اوچائي() : 0.;
}
#جيڪڏهن_پورو

    فراغ
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    إذا (menuBar && متن_Area) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect بچيل = Bounds();

#جيڪڏهن_وصف_ٿيل FEAT_MENU
	بچيل.چوٽي += Menu_اوچائي();
	menuBar->Resize_منزل(بچيل.ساڄي, بچيل.چوٽي);
	gui.menu_اوچائي = (صحيح) Menu_اوچائي();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
	toolBar->Move_منزل(بچيل.کاٻي, بچيل.چوٽي);
	toolBar->Resize_منزل(بچيل.ساڄي, Toolbar_اوچائي());
	بچيل.چوٽي += Toolbar_اوچائي();
	gui.toolbar_اوچائي = Toolbar_اوچائي();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
	tab_سٽ->Move_منزل(بچيل.کاٻي, بچيل.چوٽي);
	tab_سٽ->Resize_منزل(بچيل.ساڄي + 1, Tabline_اوچائي());
	بچيل.چوٽي += Tabline_اوچائي();
	gui.tabline_اوچائي = Tabline_اوچائي();
#جيڪڏهن_پورو

	متن_Area->Resize_منزل(بچيل.ويڪر_2(), بچيل.اوچائي_2());
	متن_Area->Move_منزل(بچيل.کاٻي, بچيل.چوٽي);
    }


    Inherited::AllAttached();
}

    فراغ
VimFormView::چوکٺو_Resized(عائم new_ويڪر, عائم new_اوچائي)
{
    بنية Vim_Resize_نياپو sm;
    صحيح adjust_h, adjust_w;

    new_ويڪر += 1;	//  adjust from width to number of pixels occupied
    new_اوچائي += 1;

    sm.ويڪر = (صحيح) new_ويڪر;
    sm.اوچائي = (صحيح) new_اوچائي;
    adjust_w = ((صحيح)new_ويڪر - gui_get_بنياد_ويڪر()) % gui.char_ويڪر;
    adjust_h = ((صحيح)new_اوچائي - gui_get_بنياد_اوچائي()) % gui.char_اوچائي;

    إذا (adjust_w > 0 || adjust_h > 0) {
	sm.ويڪر  -= adjust_w;
	sm.اوچائي -= adjust_h;
    }

    write_پورٽ(gui.vdcmp, Vim_نياپو::Resize, &sm, حجم(sm));
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

Vim_متن_Area_View::Vim_متن_Area_View(BRect چوکٺو):
    BView(چوکٺو, "VimTextAreaView", B_FOLLOW_ALL_SIDES,
#جيڪڏهن_وصف_ٿيل FEAT_MBYTE_IME
	B_WILL_DRAW | B_ڀريل_UPDATE_ON_RESIZE | B_داخلا_طريقو_AWARE
#وإلا
	B_WILL_DRAW | B_ڀريل_UPDATE_ON_RESIZE
#جيڪڏهن_پورو
	),
    mouse_Drag_Event_ڳڻپ(0)
{
#جيڪڏهن_وصف_ٿيل FEAT_MBYTE_IME
    IMData.messenger = عدم;
    IMData.پيغام = عدم;
#جيڪڏهن_پورو
    init(چوکٺو);
}

Vim_متن_Area_View::~Vim_متن_Area_View()
{
    gui.vim_متن_Area = عدم;
}

    فراغ
Vim_متن_Area_View::init(BRect چوکٺو)
{
    // set up global var for fast access
    gui.vim_متن_Area = هذا;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    سيٽ_View_Color(B_TRANSPARENT_32_ٻٽو_انگ);
#وصف_ڪريو PEN_ويڪر   1
    سيٽ_Pen_سائيز(PEN_ويڪر);
#وصف_ڪريو W_ويڪر(curwin)   0
}

    فراغ
Vim_متن_Area_View::Draw(BRect updateRect)
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
    rgb_color rgb = GUI_منزل_RGB(gui.پٺ_pixel);
    سيٽ_هيٺيون_Color(rgb);
    FillRect(updateRect, B_SOLID_هيٺيون);
    gui_redraw((صحيح) updateRect.کاٻي, (صحيح) updateRect.چوٽي,
	    (صحيح) (updateRect.ويڪر_2() + PEN_ويڪر), (صحيح) (updateRect.اوچائي_2() + PEN_ويڪر));

    // Clear the border areas if needed
    سيٽ_هيٺيون_Color(rgb);

    إذا (updateRect.کاٻي < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.کاٻي, updateRect.چوٽي,
		    FILL_X(0)-PEN_ويڪر, updateRect.bottom), B_SOLID_هيٺيون);
    إذا (updateRect.چوٽي < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.کاٻي, updateRect.چوٽي,
		    updateRect.ساڄي, FILL_Y(0)-PEN_ويڪر), B_SOLID_هيٺيون);
    إذا (updateRect.ساڄي >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((صحيح)Columns), updateRect.چوٽي,
		    updateRect.ساڄي, updateRect.bottom), B_SOLID_هيٺيون);
    إذا (updateRect.bottom >= FILL_Y(صفون))   //  bottom border
	FillRect(BRect(updateRect.کاٻي, FILL_Y((صحيح)صفون),
		    updateRect.ساڄي, updateRect.bottom), B_SOLID_هيٺيون);

#جيڪڏهن_وصف_ٿيل FEAT_MBYTE_IME
    DrawIMString();
#جيڪڏهن_پورو
}

    فراغ
Vim_متن_Area_View::ڪنجي_هيٺ_ڏانهن(ثابت حرف *بائيٽون, int32 انگ_بائيٽون_2)
{
    بنية Vim_ڪنجي_نياپو km;
    char_u *dest = km.chars;

    منطقي canHaveVimModifiers = خطأ;

    BMessage *نياپو = دري()->Current_پيغام();
    أكد(نياپو);
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
    إذا (انگ_بائيٽون_2 > 1) {
	// This cannot be a special key
	إذا (انگ_بائيٽون_2 > ڪنجي_نياپو_BUFSIZ)
	    انگ_بائيٽون_2 = ڪنجي_نياپو_BUFSIZ;	    //	should never happen... ???
	km.ڊيگهائي = انگ_بائيٽون_2;
	يادگيري_نقل((حرف *)dest, بائيٽون, انگ_بائيٽون_2);
	km.csi_escape = سچ;
    } وإلا {
	int32 scancode = 0;
	نياپو->ڳوليو_Int32("key", &scancode);

	int32 beModifiers = 0;
	نياپو->ڳوليو_Int32("modifiers", &beModifiers);

	char_u string[3];
	صحيح len = 0;
	km.ڊيگهائي = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	أكد(B_BACKSPACE <= 0x20);
	أكد(B_DELETE == 0x7F);
	إذا (((char_u)بائيٽون[0] <= 0x20 || (char_u)بائيٽون[0] == 0x7F) &&
		انگ_بائيٽون_2 == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    إذا (beModifiers & B_CONTROL_ڪنجي) {
		صحيح اشاريو = ڪنجي_Map->عام_map[scancode];
		صحيح new_انگ_بائيٽون = ڪنجي_Map_Chars[اشاريو];
		char_u *new_بائيٽون = (char_u *)&ڪنجي_Map_Chars[اشاريو + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		إذا (new_انگ_بائيٽون != 1 || (*new_بائيٽون > 0x20 &&
			    *new_بائيٽون != 0x7F )) {
		    اذهب notspecial;
		}
		بائيٽون = (حرف *)new_بائيٽون;
	    }
	    canHaveVimModifiers = سچ;

	    uint16 beoskey;
	    صحيح پهريون, آخري;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    إذا (انگ_بائيٽون_2 == 0 || بائيٽون[0] == B_FUNCTION_ڪنجي) {
		beoskey = F(scancode);
		پهريون = پهريون_FUNCTION_ڪنجي;
		آخري = انگ_SPECIAL_ڪنجيون;
	    } وإلا إذا (*بائيٽون == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		پهريون = آخري = 0;
	    } وإلا {
		beoskey = K(بائيٽون[0]);
		پهريون = 0;
		آخري = پهريون_FUNCTION_ڪنجي;
	    }

	    لكل (صحيح i = پهريون; i < آخري; i++) {
		إذا (special_ڪنجيون[i].Be_ڪنجيون == beoskey) {
		    string[0] = CSI;
		    string[1] = special_ڪنجيون[i].vim_code0;
		    string[2] = special_ڪنجيون[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	إذا (len == 0) {
	    string[0] = بائيٽون[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#إذا 0
	إذا (len == 3 ||
		بائيٽون[0] == B_SPACE || بائيٽون[0] == B_TAB ||
		بائيٽون[0] == B_RETURN || بائيٽون[0] == '\r' ||
		بائيٽون[0] == B_ESCAPE)
#وإلا
	    إذا (canHaveVimModifiers)
#جيڪڏهن_پورو
	    {
		صحيح modifiers;
		modifiers = 0;
		إذا (beModifiers & B_سرڪايو_ڪنجي)
		    modifiers |= MOD_نقاب_سرڪايو;
		إذا (beModifiers & B_CONTROL_ڪنجي)
		    modifiers |= MOD_نقاب_CTRL;
		إذا (beModifiers & B_OPTION_ڪنجي)
		    modifiers |= MOD_نقاب_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		صحيح ڪنجي;
		إذا (string[0] == CSI && len == 3)
		    ڪنجي = منزل_SPECIAL(string[1], string[2]);
		وإلا
		    ڪنجي = string[0];
		ڪنجي = simplify_ڪنجي(ڪنجي, &modifiers);
		إذا (IS_SPECIAL(ڪنجي))
		{
		    string[0] = CSI;
		    string[1] = K_ٻيو(ڪنجي);
		    string[2] = K_ٽيون(ڪنجي);
		    len = 3;
		}
		وإلا
		{
		    string[0] = ڪنجي;
		    len = 1;
		}

		إذا (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.ڊيگهائي = 3;
		}
	    }
	يادگيري_نقل((حرف *)dest, string, len);
	km.ڊيگهائي += len;
	km.csi_escape = خطأ;
    }

    write_پورٽ(gui.vdcmp, Vim_نياپو::ڪنجي_2, &km, حجم(km));

    /*
     * blank out the pointer if necessary
     */
    إذا (p_mh && !gui.اشارو_hidden)
    {
	guiBlankMouse(سچ);
	gui.اشارو_hidden = TRUE;
    }
}
فراغ
Vim_متن_Area_View::guiSendMouseEvent(
	صحيح	button,
	صحيح	x,
	صحيح	y,
	صحيح	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_نياپو mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_پورٽ(gui.vdcmp, Vim_نياپو::Mouse, &mm, حجم(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    إذا (gui.اشارو_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشارو_hidden = FALSE;
    }
}

فراغ
Vim_متن_Area_View::guiMouseMoved(
	صحيح	x,
	صحيح	y)
{
    Vim_Mouse_Moved_نياپو mm;

    mm.x = x;
    mm.y = y;

    write_پورٽ(gui.vdcmp, Vim_نياپو::MouseMoved, &mm, حجم(mm));

    إذا (gui.اشارو_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشارو_hidden = FALSE;
    }
}

    فراغ
Vim_متن_Area_View::guiBlankMouse(منطقي should_hide)
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
Vim_متن_Area_View::mouse_Modifiers_منزل_Vim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    إذا (beModifiers & B_سرڪايو_ڪنجي)
	vim_modifiers |= MOUSE_سرڪايو;
    إذا (beModifiers & B_CONTROL_ڪنجي)
	vim_modifiers |= MOUSE_CTRL;
    إذا (beModifiers & B_OPTION_ڪنجي)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    أرجع vim_modifiers;
}

    فراغ
Vim_متن_Area_View::Mouse_هيٺ_ڏانهن(BPoint نقطو)
{
    BMessage *m = دري()->Current_پيغام();
    أكد(m);

    int32 buttons = 0;
    m->ڳوليو_Int32("buttons", &buttons);

    صحيح vimButton;

    إذا (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_کاٻو;
    وإلا إذا (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_ساڄو;
    وإلا إذا (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_وچ;
    وإلا
	أرجع;		// Unknown button

    vimMouseButton = 1;	    // don't care which one

    // Handle multiple clicks
    int32 clicks = 0;
    m->ڳوليو_Int32("clicks", &clicks);

    int32 modifiers = 0;
    m->ڳوليو_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_منزل_Vim(modifiers);

    guiSendMouseEvent(vimButton, نقطو.x, نقطو.y,
	    clicks > 1 /* = repeated_click*/, vimMouseModifiers);
}

    فراغ
Vim_متن_Area_View::Mouse_مٿي(BPoint نقطو)
{
    vimMouseButton = 0;

    BMessage *m = دري()->Current_پيغام();
    أكد(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->ڳوليو_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_منزل_Vim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, نقطو.x, نقطو.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::Mouse_مٿي(نقطو);
}

    فراغ
Vim_متن_Area_View::MouseMoved(BPoint نقطو, uint32 transit, ثابت BMessage *پيغام)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    إذا (gui.اشارو_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشارو_hidden = FALSE;
    }

    إذا (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(نقطو.x, نقطو.y);
	أرجع;
    }

    atomic_جوڙيو(&mouse_Drag_Event_ڳڻپ, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, نقطو.x, نقطو.y, 0, vimMouseModifiers);
}

    فراغ
Vim_متن_Area_View::پيغام_Received(BMessage *m)
{
    بدل (m->what) {
	حالة 'menu':
	    {
		Vim_Menu_نياپو mm;
		mm.guiMenu = عدم;  // in case no pointer in msg
		m->ڳوليو_اشارو("VimMenu", (فراغ **)&mm.guiMenu);
		write_پورٽ(gui.vdcmp, Vim_نياپو::Menu, &mm, حجم(mm));
	    }
	    اكسر;
	حالة B_MOUSE_WHEEL_بدليو:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		عائم small=0, big=0, dy=0;
		m->ڳوليو_Float("be:wheel_delta_y", &dy);
		scb->Get_قدمن(&small, &big);
		scb->سيٽ_قدر(scb->قدر_2()+small*dy*3);
		scb->قدر_بدليو_2(scb->قدر_2());
#إذا 0
		scb = curwin->w_scrollbars[0].id;
		scb->Get_قدمن(&small, &big);
		scb->سيٽ_قدر(scb->قدر_2()+small*dy);
		scb->قدر_بدليو_2(scb->قدر_2());
#جيڪڏهن_پورو
	    }
	    اكسر;
#جيڪڏهن_وصف_ٿيل FEAT_MBYTE_IME
	حالة B_داخلا_طريقو_EVENT:
	    {
		int32 opcode;
		m->ڳوليو_Int32("be:opcode", &opcode);
		بدل(opcode)
		{
		    حالة B_داخلا_طريقو_STARTED:
			إذا (!IMData.messenger) ميٽيو IMData.messenger;
			IMData.messenger = جديد BMessenger();
			m->ڳوليو_Messenger("be:reply_to", IMData.messenger);
			اكسر;
		    حالة B_داخلا_طريقو_بدليو:
			{
			    BString str;
			    منطقي confirmed;
			    إذا (IMData.پيغام) *(IMData.پيغام) = *m;
			    وإلا	       IMData.پيغام = جديد BMessage(*m);
			    DrawIMString();
			    m->ڳوليو_Bool("be:confirmed", &confirmed);
			    إذا (confirmed)
			    {
				m->ڳوليو_String("be:string", &str);
				char_u *chars = (char_u*)str.String();
				بنية Vim_ڪنجي_نياپو km;
				km.csi_escape = سچ;
				صحيح clen;
				صحيح i = 0;
				طالما (i < str.ڊيگهائي_2())
				{
				    clen = utf_ptr2len(chars+i);
				    يادگيري_نقل(km.chars, chars+i, clen);
				    km.ڊيگهائي = clen;
				    write_پورٽ(gui.vdcmp, Vim_نياپو::ڪنجي_2, &km, حجم(km));
				    i += clen;
				}
			    }
			}
			اكسر;
		    حالة B_داخلا_طريقو_LOCATION_REQUEST:
			{
			    BMessage نياپو(B_داخلا_طريقو_EVENT);
			    نياپو.جوڙيو_Int32("be:opcode", B_داخلا_طريقو_LOCATION_REQUEST);
			    نياپو.جوڙيو_نقطو("be:location_reply", IMData.location);
			    نياپو.جوڙيو_Float("be:height_reply", FILL_Y(1));
			    IMData.messenger->Send_پيغام(&نياپو);
			}
			اكسر;
		    حالة B_داخلا_طريقو_STOPPED:
			ميٽيو IMData.messenger;
			ميٽيو IMData.پيغام;
			IMData.messenger = عدم;
			IMData.پيغام = عدم;
			اكسر;
		}
	    }
	    // TODO: sz: break here???
#جيڪڏهن_پورو
	افتراضي:
	    إذا (m->WasDropped()) {
		BWindow *w = دري();
		w->Detach_Current_پيغام();
		w->Minimize(خطأ);
		VimApp::SendRefs(m, (modifiers() & B_سرڪايو_ڪنجي) != 0);
	    } وإلا {
		Inherited::پيغام_Received(m);
	    }
	    اكسر;
    }
}

    صحيح
Vim_متن_Area_View::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    إذا (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_سيٽ_font((GuiFont)newFont);
	إذا (name && STRCMP(name, "*") != 0)
	    hl_سيٽ_font_name(name);

	سيٽ_Drawing_گهڻائي(B_OP_COPY);

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
Vim_متن_Area_View::mchDrawString(صحيح صف, صحيح کانو, char_u *s, صحيح len, صحيح flags)
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

	BRect r(FILL_X(کانو), FILL_Y(صف),
		FILL_X(کانو + cells) - PEN_ويڪر, FILL_Y(صف + 1) - PEN_ويڪر);
	FillRect(r, B_SOLID_هيٺيون);
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
	    where.سيٽ(متن_X(کانو+lastpos), متن_Y(صف));
	    DrawString(p, clen, where);
	    إذا (flags & DRAW_BOLD) {
		where.x += 1.0;
		سيٽ_Drawing_گهڻائي(B_OP_BLEND);
		DrawString(p, clen, where);
		سيٽ_Drawing_گهڻائي(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    إذا (cells<4) lastpos += cells;
	    وإلا	lastpos++;
	    p += clen;
	}
    }
    وإلا
    {
	BPoint where(متن_X(کانو), متن_Y(صف));
	DrawString((حرف*)s, len, where);
	إذا (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    سيٽ_Drawing_گهڻائي(B_OP_BLEND);
	    DrawString((حرف*)s, len, where);
	    سيٽ_Drawing_گهڻائي(B_OP_COPY);
	}
    }

    إذا (flags & DRAW_UNDERL) {
	صحيح cells;
	cells = 0;
	لكل (صحيح i=0; i<len; i++) {
	    صحيح cn = utf_ptr2cells((char_u *)(s+i));
	    إذا (cn<4) cells += cn;
	}

	BPoint شروعات(FILL_X(کانو), FILL_Y(صف + 1) - PEN_ويڪر);
	BPoint پڇاڙي(FILL_X(کانو + cells) - PEN_ويڪر, شروعات.y);

	Stroke_سٽ(شروعات, پڇاڙي);
    }
}

فراغ
Vim_متن_Area_View::mch_Clear_بلاڪ(
	صحيح	row1,
	صحيح	col1,
	صحيح	row2,
	صحيح	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_ويڪر, FILL_Y(row2 + 1) - PEN_ويڪر);
    gui_mch_سيٽ_bg_color(gui.پٺ_pixel);
    FillRect(r, B_SOLID_هيٺيون);
}

    فراغ
Vim_متن_Area_View::mchClearAll()
{
    gui_mch_سيٽ_bg_color(gui.پٺ_pixel);
    FillRect(Bounds(), B_SOLID_هيٺيون);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    فراغ
Vim_متن_Area_View::mch_Delete_سٽون(صحيح صف, صحيح انگ_سٽون)
{
    BRect ذريعو, dest;
    ذريعو.کاٻي = FILL_X(gui.scroll_region_کاٻو);
    ذريعو.چوٽي = FILL_Y(صف + انگ_سٽون);
    ذريعو.ساڄي = FILL_X(gui.scroll_region_ساڄو + 1) - PEN_ويڪر;
    ذريعو.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_ويڪر;

    dest.کاٻي = FILL_X(gui.scroll_region_کاٻو);
    dest.چوٽي = FILL_Y(صف);
    dest.ساڄي = FILL_X(gui.scroll_region_ساڄو + 1) - PEN_ويڪر;
    dest.bottom = FILL_Y(gui.scroll_region_bot - انگ_سٽون + 1) - PEN_ويڪر;

    إذا (gui.vim_دري->تالو_2()) {
	// Clear one column more for when bold has spilled over
	Copy_بٽ(ذريعو, dest);
	gui_clear_بلاڪ(gui.scroll_region_bot - انگ_سٽون + 1,
		gui.scroll_region_کاٻو,
		gui.scroll_region_bot, gui.scroll_region_ساڄو);


	gui.vim_دري->Unlock();
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
Vim_متن_Area_View::mch_وجهو_سٽون(صحيح صف, صحيح انگ_سٽون)
{
    BRect ذريعو, dest;

    // XXX Attempt at a hack:
    gui.vim_دري->UpdateIfNeeded();
    ذريعو.کاٻي = FILL_X(gui.scroll_region_کاٻو);
    ذريعو.چوٽي = FILL_Y(صف);
    ذريعو.ساڄي = FILL_X(gui.scroll_region_ساڄو + 1) - PEN_ويڪر;
    ذريعو.bottom = FILL_Y(gui.scroll_region_bot - انگ_سٽون + 1) - PEN_ويڪر;

    dest.کاٻي = FILL_X(gui.scroll_region_کاٻو);
    dest.چوٽي = FILL_Y(صف + انگ_سٽون);
    dest.ساڄي = FILL_X(gui.scroll_region_ساڄو + 1) - PEN_ويڪر;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_ويڪر;

    إذا (gui.vim_دري->تالو_2()) {
	// Clear one column more for when bold has spilled over
	Copy_بٽ(ذريعو, dest);
	gui_clear_بلاڪ(صف, gui.scroll_region_کاٻو,
		صف + انگ_سٽون - 1, gui.scroll_region_ساڄو);

	gui.vim_دري->Unlock();
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

#جيڪڏهن_وصف_ٿيل FEAT_MBYTE_IME
/*
 * DrawIMString draws string with IMData.message.
 */
فراغ Vim_متن_Area_View::DrawIMString(فراغ)
{
    ساكن ثابت rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ثابت حرف* s;
    صحيح len;
    BMessage* نياپو = IMData.پيغام;
    إذا (!نياپو)
	أرجع;
    gui_redraw_بلاڪ(IMData.صف, 0,
	    IMData.صف + IMData.ڳڻپ, W_ويڪر(curwin), GUI_MON_NOCLEAR);
    منطقي confirmed = خطأ;
    نياپو->ڳوليو_Bool("be:confirmed", &confirmed);
    إذا (confirmed)
	أرجع;
    rgb_color hcolor = مٿاهون_Color(), lcolor = هيٺيون_Color();
    نياپو->ڳوليو_String("be:string", &str);
    s = str.String();
    len = str.ڊيگهائي_2();
    سيٽ_مٿاهون_Color(0, 0, 0);
    IMData.صف = gui.صف;
    IMData.کانو = gui.کانو;
    int32 sel_شروعات = 0, sel_پڇاڙي = 0;
    نياپو->ڳوليو_Int32("be:selection", 0, &sel_شروعات);
    نياپو->ڳوليو_Int32("be:selection", 1, &sel_پڇاڙي);
    صحيح clen, cn;
    BPoint pos(IMData.کانو, 0);
    BRect r;
    BPoint where;
    IMData.location = Convert_منزل_Screen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.صف + pos.y)));
    لكل (صحيح i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	إذا (pos.x + cn > W_ويڪر(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	إذا (sel_شروعات<=i && i<sel_پڇاڙي)
	{
	    سيٽ_هيٺيون_Color(r_highlight);
	    IMData.location = Convert_منزل_Screen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.صف + pos.y)));
	}
	وإلا
	{
	    سيٽ_هيٺيون_Color(b_highlight);
	}
	r.سيٽ(FILL_X(pos.x), FILL_Y(IMData.صف + pos.y),
		FILL_X(pos.x + cn) - PEN_ويڪر,
		FILL_Y(IMData.صف + pos.y + 1) - PEN_ويڪر);
	FillRect(r, B_SOLID_هيٺيون);
	where.سيٽ(متن_X(pos.x), متن_Y(IMData.صف + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.ڳڻپ = (صحيح)pos.y;

    سيٽ_مٿاهون_Color(hcolor);
    سيٽ_هيٺيون_Color(lcolor);
}
#جيڪڏهن_پورو
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
    ignore_قدر(-1),
    scroll_Event_ڳڻپ(0)
{
    gsb = g;
    سيٽ_Resizing_گهڻائي(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    فراغ
VimScrollBar::قدر_بدليو_2(عائم new_قدر)
{
    إذا (ignore_قدر >= 0.0 && new_قدر == ignore_قدر) {
	ignore_قدر = -1;
	أرجع;
    }
    ignore_قدر = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_جوڙيو(&scroll_Event_ڳڻپ, 1);

    بنية Vim_Scroll_Bar_نياپو sm;

    sm.sb = هذا;
    sm.قدر = (ڊگهو) new_قدر;
    sm.stillDragging = TRUE;

    write_پورٽ(gui.vdcmp, Vim_نياپو::ScrollBar, &sm, حجم(sm));

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
VimScrollBar::Mouse_مٿي(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_جوڙيو(&scroll_Event_ڳڻپ, 1);

    بنية Vim_Scroll_Bar_نياپو sm;

    sm.sb = هذا;
    sm.قدر = (ڊگهو) قدر_2();
    sm.stillDragging = FALSE;

    write_پورٽ(gui.vdcmp, Vim_نياپو::ScrollBar, &sm, حجم(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::Mouse_مٿي(where);
}

    فراغ
VimScrollBar::سيٽ_قدر(عائم new_قدر)
{
    إذا (new_قدر == قدر_2())
	أرجع;

    ignore_قدر = new_قدر;
    Inherited::سيٽ_قدر(new_قدر);
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
    ايندڙ = عدم;
    refcount = 1;
    name = عدم;
}

// ---------------- VimDialog ----------------

#إذا defined(FEAT_GUI_DIALOG)

ثابت بنانشان صحيح  k_Vim_Dialog_Button_نياپو = 'VMDB';
ثابت بنانشان صحيح  k_Vim_Dialog_Icon_Stripe_ويڪر = 30;
ثابت بنانشان صحيح  kVimDialogButtonsSpacingX = 9;
ثابت بنانشان صحيح  kVimDialogButtonsSpacingY = 4;
ثابت بنانشان صحيح  kVimDialogSpacingX = 6;
ثابت بنانشان صحيح  kVimDialogSpacingY = 10;
ثابت بنانشان صحيح  k_Vim_Dialog_گهٽتر_ويڪر  = 310;
ثابت بنانشان صحيح  k_Vim_Dialog_گهٽتر_اوچائي = 75;
ثابت BRect	    kDefaultRect =
BRect(0, 0, k_Vim_Dialog_گهٽتر_ويڪر, k_Vim_Dialog_گهٽتر_اوچائي);

VimDialog::VimDialog(صحيح type, ثابت حرف *title, ثابت حرف *پيغام,
	ثابت حرف *buttons, صحيح dfltbutton, ثابت حرف *textfield, صحيح ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_دري_LOOK, B_MODAL_APP_دري_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_قدر(dfltbutton)
    , f_پيغام_View(عدم)
    , f_داخلا_Control(عدم)
    , f_داخلا_قدر(textfield)
{
    //	master view
    VimDialog::View* view = جديد VimDialog::View(Bounds());
    إذا (view == عدم)
	أرجع;

    إذا (title == عدم)
	سيٽ_Title("Vim " VIM_نسخو_MEDIUM);

    جوڙيو_ٻار(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    عائم max_Button_ويڪر  = 0;
    عائم max_Button_اوچائي = 0;
    عائم buttons_ويڪر	  = 0;
    عائم buttons_اوچائي   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    افعل
    {
	int32 پڇاڙي = strButtons.ڳوليو_پهريون('\n');
	إذا (پڇاڙي != B_ERROR)
	    strButtons.سيٽ_Byte_At(پڇاڙي, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->جوڙيو_ٻار(button);
	fButtonsList.جوڙيو_شي(button);

	max_Button_ويڪر	= max_c(max_Button_ويڪر,  button->Bounds().ويڪر_2());
	max_Button_اوچائي = max_c(max_Button_اوچائي, button->Bounds().اوچائي_2());
	buttons_ويڪر   += button->Bounds().ويڪر_2();
	buttons_اوچائي  += button->Bounds().اوچائي_2();

	إذا (پڇاڙي == B_ERROR)
	    اكسر;

	strButtons.Remove(0, پڇاڙي + 1);
    } طالما (سچ);

    int32 buttons_ڳڻپ = fButtonsList.ڳڻپ_شيون();
    buttons_ويڪر      += kVimDialogButtonsSpacingX * (buttons_ڳڻپ - 1);
    buttons_اوچائي     += kVimDialogButtonsSpacingY * (buttons_ڳڻپ - 1);
    عائم dialog_ويڪر  = buttons_ويڪر + k_Vim_Dialog_Icon_Stripe_ويڪر +
	kVimDialogSpacingX * 2;
    عائم dialog_اوچائي = max_Button_اوچائي + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    منطقي vertical = (vim_strchr(p_go, GO_VERTICAL) != عدم) ||
	dialog_ويڪر >= gui.vim_دري->Bounds().ويڪر_2();
    إذا (vertical) {
	dialog_ويڪر  -= buttons_ويڪر;
	dialog_ويڪر  += max_Button_ويڪر;
	dialog_اوچائي -= max_Button_اوچائي;
	dialog_اوچائي += buttons_اوچائي;
    }

    dialog_ويڪر  = max_c(dialog_ويڪر,  k_Vim_Dialog_گهٽتر_ويڪر);

    //	message view
    BRect rect(0, 0, dialog_ويڪر, 0);
    rect.کاٻي  += k_Vim_Dialog_Icon_Stripe_ويڪر + 16 + kVimDialogSpacingX;
    rect.چوٽي   += kVimDialogSpacingY;
    rect.ساڄي -= kVimDialogSpacingX;
    rect.bottom = rect.چوٽي;
    f_پيغام_View = جديد BTextView(rect, "_tv_", rect.وٿي_By_Copy(B_ORIGIN),
	    B_FOLLOW_کاٻو | B_FOLLOW_چوٽي, B_WILL_DRAW);

    f_پيغام_View->سيٽ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color متن_Color = ui_color(B_PANEL_متن_COLOR);
    f_پيغام_View->سيٽ_Font_And_Color(be_plain_font, B_FONT_ALL, &متن_Color);
    f_پيغام_View->سيٽ_متن(پيغام);
    f_پيغام_View->MakeEditable(خطأ);
    f_پيغام_View->MakeSelectable(خطأ);
    f_پيغام_View->سيٽ_لفظ_Wrap(سچ);
    جوڙيو_ٻار(f_پيغام_View);

    عائم پيغام_اوچائي = f_پيغام_View->متن_اوچائي(0, f_پيغام_View->ڳڻپ_سٽون());
    f_پيغام_View->ResizeBy(0, پيغام_اوچائي);
    f_پيغام_View->سيٽ_متن_Rect(BRect(0, 0, rect.ويڪر_2(), پيغام_اوچائي));

    dialog_اوچائي += پيغام_اوچائي;

    //	input view
    إذا (f_داخلا_قدر != عدم) {
	rect.چوٽي     =
	    rect.bottom += پيغام_اوچائي + kVimDialogSpacingY;
	f_داخلا_Control = جديد BTextControl(rect, "_iv_", عدم, f_داخلا_قدر, عدم,
		B_FOLLOW_کاٻو | B_FOLLOW_چوٽي, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_داخلا_Control->متن_View()->سيٽ_متن(f_داخلا_قدر);
	f_داخلا_Control->متن_View()->سيٽ_لفظ_Wrap(خطأ);
	جوڙيو_ٻار(f_داخلا_Control);

	عائم ويڪر = 0.f, اوچائي = 0.f;
	f_داخلا_Control->Get_Preferred_سائيز(&ويڪر, &اوچائي);
	f_داخلا_Control->MakeFocus(سچ);

	dialog_اوچائي += اوچائي + kVimDialogSpacingY * 1.5;
    }

    dialog_اوچائي = max_c(dialog_اوچائي, k_Vim_Dialog_گهٽتر_اوچائي);

    Resize_منزل(dialog_ويڪر, dialog_اوچائي);
    Move_منزل((gui.vim_دري->Bounds().ويڪر_2() - dialog_ويڪر) / 2,
	    (gui.vim_دري->Bounds().اوچائي_2() - dialog_اوچائي) / 2);

    //	adjust layout of buttons
    عائم button_ويڪر = max_c(max_Button_ويڪر, rect.ويڪر_2() * 0.66);
    BPoint origin(dialog_ويڪر, dialog_اوچائي);
    origin.x -= kVimDialogSpacingX + (vertical ? button_ويڪر : buttons_ويڪر);
    origin.y -= kVimDialogSpacingY + (vertical ? buttons_اوچائي	: max_Button_اوچائي);

    لكل (int32 i = 0 ; i < buttons_ڳڻپ; i++) {
	BButton *button = (BButton*)fButtonsList.شي_At(i);
	button->Move_منزل(origin);
	إذا (vertical) {
	    origin.y += button->چوکٺو_2().اوچائي_2() + kVimDialogButtonsSpacingY;
	    button->Resize_منزل(button_ويڪر, button->چوکٺو_2().اوچائي_2());
	} وإلا
	    origin.x += button->چوکٺو_2().ويڪر_2() + kVimDialogButtonsSpacingX;

	إذا (dfltbutton == i + 1) {
	    button->MakeDefault(سچ);
	    button->MakeFocus(f_داخلا_Control == عدم);
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
	أرجع f_Dialog_قدر;
    }

    Show();

    طالما (acquire_sem(fDialogSem) == B_INTERRUPTED);

    صحيح ret_قدر = f_Dialog_قدر;
    إذا (f_داخلا_قدر != عدم)
	vim_strncpy((char_u*)f_داخلا_قدر, (char_u*)f_داخلا_Control->متن_2(), IOSIZE - 1);

    إذا (تالو_2())
	Quit();

    أرجع ret_قدر;
}

فراغ VimDialog::پيغام_Received(BMessage *نياپو)
{
    int32 which = 0;
    إذا (نياپو->what != k_Vim_Dialog_Button_نياپو ||
	    نياپو->ڳوليو_Int32("which", &which) != B_OK)
	أرجع BWindow::پيغام_Received(نياپو);

    f_Dialog_قدر = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ثابت حرف* label)
{
    BMessage *پيغام = جديد BMessage(k_Vim_Dialog_Button_نياپو);
    پيغام->جوڙيو_Int32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = جديد BButton(rect, name.String(), label, پيغام,
	    B_FOLLOW_ساڄو | B_FOLLOW_BOTTOM);

    عائم ويڪر = 0.f, اوچائي = 0.f;
    button->Get_Preferred_سائيز(&ويڪر, &اوچائي);
    button->Resize_منزل(ويڪر, اوچائي);

    أرجع button;
}

VimDialog::View::View(BRect چوکٺو)
    :	BView(چوکٺو, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(عدم)
{
    سيٽ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    ميٽيو fIconBitmap;
}

فراغ VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.ساڄي = k_Vim_Dialog_Icon_Stripe_ويڪر;
    سيٽ_مٿاهون_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    إذا (fIconBitmap == عدم)
	أرجع;

    سيٽ_Drawing_گهڻائي(B_OP_ALPHA);
    سيٽ_Blending_گهڻائي(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

فراغ VimDialog::View::InitIcon(int32 type)
{
    إذا (type == VIM_GENERIC)
	أرجع;

    BPath رستو;
    حالت_t حالت = ڳوليو_directory(B_BEOS_SERVERS_DIRECTORY, &رستو);
    إذا (حالت != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(حالت));
	أرجع;
    }

    رستو.Append("app_server");

    BFile file(رستو.رستو_2(), O_RDONLY);
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

    int32 icon_سائيز_2 = 32;
    fIconBitmap = جديد BBitmap(BRect(0, 0, icon_سائيز_2 - 1, icon_سائيز_2 - 1), 0, B_RGBA32);
    إذا (fIconBitmap == عدم || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == عدم) ? "null" : strerror(fIconBitmap->InitCheck()));
	أرجع;
    }

    size_t سائيز = 0;
    ثابت uint8* icon_معلومات = عدم;
    //	try vector icon first?
    icon_معلومات = (ثابت uint8*)resources.ڀريو_Resource(B_ويڪٽر_ICON_TYPE, name, &سائيز);
    إذا (icon_معلومات != عدم && BIconUtils::Get_ويڪٽر_Icon(icon_معلومات, سائيز, fIconBitmap) == B_OK)
	أرجع;

    //	try bitmap icon now
    icon_معلومات = (ثابت uint8*)resources.ڀريو_Resource(B_LARGE_ICON_TYPE, name, &سائيز);
    إذا (icon_معلومات == عدم) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	ميٽيو fIconBitmap;
	fIconBitmap = عدم;
	أرجع;
    }

    إذا (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(icon_معلومات, icon_سائيز_2, icon_سائيز_2, icon_سائيز_2, fIconBitmap);
}

ثابت بنانشان صحيح  k_Vim_Dialog_OKButton_نياپو = 'FDOK';
ثابت بنانشان صحيح  k_Vim_Dialog_Cancel_Button_نياپو = 'FDCN';
ثابت بنانشان صحيح  k_Vim_Dialog_سائيز_داخلا_نياپو = 'SICH';
ثابت بنانشان صحيح  k_Vim_Dialog_Family_Select_نياپو = 'MSFM';
ثابت بنانشان صحيح  k_Vim_Dialog_Style_Select_نياپو = 'MSST';
ثابت بنانشان صحيح  k_Vim_Dialog_سائيز_Select_نياپو = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, عائم* سائيز)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_دري_LOOK, B_MODAL_APP_دري_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , f_حالت(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_قدر(خطأ)
    , fFamily(family)
    , fStyle(style)
    , f_سائيز(سائيز)
    , f_Font_سائيز(*سائيز)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_داخلا(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_ڊيگهائي);
    strncpy(fFontStyle, *style, B_FONT_STYLE_ڊيگهائي);

    //	"client" area view
    BBox *clientBox = جديد BBox(Bounds(), B_خالي_جايون_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_چوکٺو_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    جوڙيو_ٻار(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.کاٻو_چوٽي(), RC.کاٻو_چوٽي());

    //	at first create all controls
    fPreview = جديد BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->جوڙيو_ٻار(fPreview);

    BBox* boxDivider = جديد BBox(rc, B_خالي_جايون_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->جوڙيو_ٻار(boxDivider);

    BStringView *labelFamily = جديد BStringView(rc, "labelFamily", "Family:");
    clientBox->جوڙيو_ٻار(labelFamily);
    labelFamily->Resize_منزل_Preferred();

    BStringView *labelStyle = جديد BStringView(rc, "labelStyle", "Style:");
    clientBox->جوڙيو_ٻار(labelStyle);
    labelStyle->Resize_منزل_Preferred();

    BStringView *label_سائيز = جديد BStringView(rc, "labelSize", "Size:");
    clientBox->جوڙيو_ٻار(label_سائيز);
    label_سائيز->Resize_منزل_Preferred();

    fFamiliesList = جديد BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = جديد BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_کاٻو_ساڄو, 0, خطأ, سچ);
    clientBox->جوڙيو_ٻار(scrollFamilies);

    fStylesList= جديد BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = جديد BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_کاٻو_ساڄو, 0, خطأ, سچ);
    clientBox->جوڙيو_ٻار(scrollStyles);

    f_Sizes_داخلا = جديد BTextControl(rc, "inputSize", عدم, "???",
	    جديد BMessage(k_Vim_Dialog_سائيز_داخلا_نياپو));
    clientBox->جوڙيو_ٻار(f_Sizes_داخلا);
    f_Sizes_داخلا->Resize_منزل_Preferred();

    fSizesList = جديد BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = جديد BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_کاٻو_ساڄو, 0, خطأ, سچ);
    clientBox->جوڙيو_ٻار(scrollSizes);

    BButton *buttonOK = جديد BButton(rc, "buttonOK", "OK",
			جديد BMessage(k_Vim_Dialog_OKButton_نياپو));
    clientBox->جوڙيو_ٻار(buttonOK);
    buttonOK->Resize_منزل_Preferred();

    BButton *buttonCancel = جديد BButton(rc, "buttonCancel", "Cancel",
			جديد BMessage(k_Vim_Dialog_Cancel_Button_نياپو));
    clientBox->جوڙيو_ٻار(buttonCancel);
    buttonCancel->Resize_منزل_Preferred();

    //	layout controls
    عائم سٽ_اوچائي = labelFamily->Bounds().اوچائي_2();
    عائم preview_اوچائي = سٽ_اوچائي * 3;
    عائم وٿي_YLabels = preview_اوچائي + kVimDialogSpacingY;
    عائم وٿي_YLists = وٿي_YLabels + سٽ_اوچائي + kVimDialogSpacingY / 2;
    عائم وٿي_YSizes = وٿي_YLists + f_Sizes_داخلا->Bounds().اوچائي_2() + kVimDialogSpacingY / 2;
    عائم lists_اوچائي = سٽ_اوچائي * 9;
    عائم وٿي_YButtons = وٿي_YLists + lists_اوچائي +  kVimDialogSpacingY;
    عائم max_Controls_اوچائي = وٿي_YButtons + buttonOK->Bounds().اوچائي_2();
    عائم families_ويڪر = labelFamily->Bounds().ويڪر_2() * 5;
    عائم وٿي_XStyles = families_ويڪر + kVimDialogSpacingX;
    عائم styles_ويڪر = labelStyle->Bounds().ويڪر_2() * 4;
    عائم وٿي_XSizes = وٿي_XStyles + styles_ويڪر + kVimDialogSpacingX;
    عائم sizes_ويڪر = label_سائيز->Bounds().ويڪر_2() * 2;
    عائم max_Controls_ويڪر = وٿي_XSizes + sizes_ويڪر;

    Resize_منزل(max_Controls_ويڪر + kVimDialogSpacingX * 2,
	max_Controls_اوچائي + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vim_دري->چوکٺو_2();
    Move_منزل(rcVim.کاٻي + (rcVim.ويڪر_2() - چوکٺو_2().ويڪر_2()) / 2,
	    rcVim.چوٽي + (rcVim.اوچائي_2() - چوکٺو_2().اوچائي_2()) / 2);

    fPreview->Resize_منزل(max_Controls_ويڪر, preview_اوچائي);
    fPreview->سيٽ_Alignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, preview_اوچائي + kVimDialogSpacingY / 2);
    boxDivider->Resize_منزل(max_Controls_ويڪر, 1.f);

    labelFamily->MoveBy(0.f, وٿي_YLabels);
    labelStyle->MoveBy(وٿي_XStyles, وٿي_YLabels);
    label_سائيز->MoveBy(وٿي_XSizes, وٿي_YLabels);

    //	text control alignment issues
    عائم insetX = f_Sizes_داخلا->متن_View()->Bounds().ويڪر_2() - f_Sizes_داخلا->Bounds().ويڪر_2();
    عائم insetY = f_Sizes_داخلا->متن_View()->Bounds().ويڪر_2() - f_Sizes_داخلا->Bounds().ويڪر_2();

    scrollFamilies->MoveBy(0.f, وٿي_YLists);
    scrollStyles->MoveBy(وٿي_XStyles, وٿي_YLists);
    f_Sizes_داخلا->MoveBy(وٿي_XSizes + insetX / 2, وٿي_YLists + insetY / 2);
    scrollSizes->MoveBy(وٿي_XSizes, وٿي_YSizes);

    f_Sizes_داخلا->سيٽ_Alignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->Resize_منزل(families_ويڪر, lists_اوچائي);
    scrollStyles->Resize_منزل(styles_ويڪر, lists_اوچائي);
    f_Sizes_داخلا->Resize_منزل(sizes_ويڪر, f_Sizes_داخلا->Bounds().اوچائي_2());
    scrollSizes->Resize_منزل(sizes_ويڪر,
	    lists_اوچائي - (وٿي_YSizes - وٿي_YLists));

    buttonOK->MoveBy(max_Controls_ويڪر - buttonOK->Bounds().ويڪر_2(), وٿي_YButtons);
    buttonCancel->MoveBy(max_Controls_ويڪر - buttonOK->Bounds().ويڪر_2()
	    - buttonCancel->Bounds().ويڪر_2() - kVimDialogSpacingX, وٿي_YButtons);

    //	fill lists
    صحيح sel_اشاريو = -1;
    صحيح ڳڻپ = ڳڻپ_font_families();
    لكل (صحيح i = 0; i < ڳڻپ; i++) {
	font_family family;
	إذا (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->جوڙيو_شي(جديد BString_شي((ثابت حرف*)family));
	    إذا (لڙي_ڀيٽ(family, fFontFamily, B_FONT_FAMILY_ڊيگهائي) == 0)
		sel_اشاريو = i;
	}
    }

    إذا (sel_اشاريو >= 0) {
	fFamiliesList->Select(sel_اشاريو);
	fFamiliesList->Scroll_منزل_Selection();
    }

    _UpdateFontStyles();

    sel_اشاريو = -1;
    لكل (صحيح سائيز = 8, اشاريو = 0; سائيز <= 18; سائيز++, اشاريو++) {
	BString str;
	str << سائيز;
	fSizesList->جوڙيو_شي(جديد BString_شي(str));
	إذا (سائيز == f_Font_سائيز)
	    sel_اشاريو = اشاريو;

    }

    إذا (sel_اشاريو >= 0) {
	fSizesList->Select(sel_اشاريو);
	fSizesList->Scroll_منزل_Selection();
    }

    fFamiliesList->سيٽ_Selection_پيغام(جديد BMessage(k_Vim_Dialog_Family_Select_نياپو));
    fStylesList->سيٽ_Selection_پيغام(جديد BMessage(k_Vim_Dialog_Style_Select_نياپو));
    fSizesList->سيٽ_Selection_پيغام(جديد BMessage(k_Vim_Dialog_سائيز_Select_نياپو));
    f_Sizes_داخلا->سيٽ_Modification_پيغام(جديد BMessage(k_Vim_Dialog_سائيز_داخلا_نياپو));

    Update_سائيز_داخلا_Preview();
    _UpdateFontPreview();

    f_حالت = B_OK;
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
    طالما (0 < list->ڳڻپ_شيون())
	ميٽيو (dynamic_cast<BString_شي*>(list->Remove_شي((int32)0)));
}

    منطقي
VimSelectFontDialog::Go()
{
    إذا (f_حالت != B_OK) {
	Quit();
	أرجع NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    إذا (fDialogSem < B_OK) {
	Quit();
	أرجع f_Dialog_قدر;
    }

    Show();

    طالما (acquire_sem(fDialogSem) == B_INTERRUPTED);

    منطقي ret_قدر = f_Dialog_قدر;

    إذا (تالو_2())
	Quit();

    أرجع ret_قدر;
}


فراغ VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 sel_اشاريو = -1;
    int32 ڳڻپ = ڳڻپ_font_styles(fFontFamily);
    لكل (int32 i = 0; i < ڳڻپ; i++) {
	font_style style;
	uint32 flags = 0;
	إذا (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->جوڙيو_شي(جديد BString_شي((ثابت حرف*)style));
	    إذا (لڙي_ڀيٽ(style, fFontStyle, B_FONT_STYLE_ڊيگهائي) == 0)
		sel_اشاريو = i;
	}
    }

    إذا (sel_اشاريو >= 0) {
	fStylesList->Select(sel_اشاريو);
	fStylesList->Scroll_منزل_Selection();
    } وإلا
	fStylesList->Select(0);
}


فراغ VimSelectFontDialog::Update_سائيز_داخلا_Preview()
{
    حرف buf[10] = {0};
    vim_snprintf(buf, حجم(buf), (حرف*)"%.0f", f_Font_سائيز);
    f_Sizes_داخلا->سيٽ_متن(buf);
}


فراغ VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.سيٽ_سائيز(f_Font_سائيز);
    font.سيٽ_Family_And_Style(fFontFamily, fFontStyle);
    fPreview->سيٽ_Font(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_سائيز);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (صحيح)f_Font_سائيز << " pt.";
    fPreview->سيٽ_متن(str);
}


    منطقي
VimSelectFontDialog::Update_From_List_شي(BListView* list, حرف* متن, صحيح متن_سائيز)
{
    int32 اشاريو = list->CurrentSelection();
    إذا (اشاريو < 0)
	أرجع خطأ;
    BString_شي* شي = (BString_شي*)list->شي_At(اشاريو);
    إذا (شي == عدم)
	أرجع خطأ;
    strncpy(متن, شي->متن_2(), متن_سائيز);
    أرجع سچ;
}


فراغ VimSelectFontDialog::پيغام_Received(BMessage *نياپو)
{
    بدل (نياپو->what) {
	حالة k_Vim_Dialog_OKButton_نياپو:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_ڊيگهائي);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_ڊيگهائي);
	    *f_سائيز = f_Font_سائيز;
	    f_Dialog_قدر = سچ;
	حالة k_Vim_Dialog_Cancel_Button_نياپو:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    أرجع;
	حالة B_ڪنجي_مٿي:
	    {
		int32 ڪنجي = 0;
		إذا (نياپو->ڳوليو_Int32("raw_char", &ڪنجي) == B_OK
			&& ڪنجي == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    اكسر;

	حالة k_Vim_Dialog_Family_Select_نياپو:
	    إذا (Update_From_List_شي(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_ڊيگهائي)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    اكسر;
	حالة k_Vim_Dialog_Style_Select_نياپو:
	    إذا (Update_From_List_شي(fStylesList,
		    fFontStyle, B_FONT_STYLE_ڊيگهائي))
		_UpdateFontPreview();
	    اكسر;
	حالة k_Vim_Dialog_سائيز_Select_نياپو:
	    {
		حرف buf[10] = {0};
		إذا (Update_From_List_شي(fSizesList, buf, حجم(buf))) {
		    عائم سائيز = atof(buf);
		    إذا (سائيز > 0.f) {
			f_Font_سائيز = سائيز;
			Update_سائيز_داخلا_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    اكسر;
	حالة k_Vim_Dialog_سائيز_داخلا_نياپو:
	    {
		عائم سائيز = atof(f_Sizes_داخلا->متن_2());
		إذا (سائيز > 0.f) {
		    f_Font_سائيز = سائيز;
		    _UpdateFontPreview();
		}
	    }
	    اكسر;
	افتراضي:
	    اكسر;
    }
    أرجع BWindow::پيغام_Received(نياپو);
}

#جيڪڏهن_پورو // FEAT_GUI_DIALOG

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ساكن BMessage * Menu_پيغام(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect چوکٺو, ثابت حرف *name) :
    BBox(چوکٺو, name, B_FOLLOW_کاٻو | B_FOLLOW_چوٽي, B_WILL_DRAW | B_چوکٺو_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 ڳڻپ = fButtonsList.ڳڻپ_شيون();
    لكل (int32 i = 0; i < ڳڻپ; i++)
	ميٽيو (BPictureButton*)fButtonsList.شي_At(i);
    fButtonsList.Make_خالي_جايون();

    ميٽيو عام_Buttons_Bitmap;
    ميٽيو grayedButtonsBitmap;
    عام_Buttons_Bitmap    = عدم;
    grayedButtonsBitmap  = عدم;
}

    فراغ
VimToolbar::Attached_منزل_دري()
{
    BBox::Attached_منزل_دري();

    سيٽ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    عائم
VimToolbar::Toolbar_اوچائي() ثابت
{
    عائم سائيز = عدم == عام_Buttons_Bitmap ? 18. : عام_Buttons_Bitmap->Bounds().اوچائي_2();
    أرجع سائيز + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    منطقي
VimToolbar::Modify_Bitmap_منزل_Grayed(BBitmap *bitmap)
{
    عائم اوچائي = bitmap->Bounds().اوچائي_2();
    عائم ويڪر  = bitmap->Bounds().ويڪر_2();

    rgb_color *بٽ = (rgb_color*)bitmap->بٽ_2();
    int32 pixels = bitmap->بٽ_ڊيگهائي() / 4;
    لكل (int32 i = 0; i < pixels; i++) {
	بٽ[i].red = بٽ[i].green =
	بٽ[i].blue = ((uint32)بٽ[i].red + بٽ[i].green + بٽ[i].blue) / 3;
	بٽ[i].alpha /= 4;
    }

    أرجع سچ;
}

    منطقي
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    عام_Buttons_Bitmap = ڀريو_Vim_Bitmap("builtin-tools.png");
    إذا (عام_Buttons_Bitmap == عدم)
	//  customized not found? dig application resources for "builtin-tools" one
	عام_Buttons_Bitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    إذا (عام_Buttons_Bitmap == عدم)
	أرجع خطأ;

    BMessage archive;
    عام_Buttons_Bitmap->Archive(&archive);

    grayedButtonsBitmap = جديد BBitmap(&archive);
    إذا (grayedButtonsBitmap == عدم)
	أرجع خطأ;

    //	modify grayed bitmap
    Modify_Bitmap_منزل_Grayed(grayedButtonsBitmap);

    أرجع سچ;
}

BBitmap *VimToolbar::ڀريو_Vim_Bitmap(ثابت حرف* fileName)
{
    BBitmap *bitmap = عدم;

    صحيح mustfree = 0;
    char_u* runtime_رستو = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    إذا (runtime_رستو != عدم && fileName != عدم) {
	BString str_رستو((حرف*)runtime_رستو);
	str_رستو << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(str_رستو.String());
    }

    إذا (mustfree)
	vim_free(runtime_رستو);

    أرجع bitmap;
}

    منطقي
VimToolbar::GetPictureFromBitmap(BPicture *picture_منزل, int32 اشاريو, BBitmap *bitmapFrom, منطقي pressed)
{
    عائم سائيز = bitmapFrom->Bounds().اوچائي_2() + 1.;

    BView view(BRect(0, 0, سائيز, سائيز), "", 0, 0);

    جوڙيو_ٻار(&view);
    view.BeginPicture(picture_منزل);

    view.سيٽ_مٿاهون_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.سيٽ_Drawing_گهڻائي(B_OP_OVER);

    BRect ذريعو(0, 0, سائيز - 1, سائيز - 1);
    BRect destination(ذريعو);

    ذريعو.وٿي_By(سائيز * اشاريو, 0);
    destination.وٿي_By(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, ذريعو, destination);

    إذا (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	سائيز += ButtonMargin * 2 - 1;
	view.Begin_سٽ_Array(4);
	view.جوڙيو_سٽ(BPoint(0, 0),	 BPoint(سائيز, 0),    shadowColor);
	view.جوڙيو_سٽ(BPoint(سائيز, 0),	 BPoint(سائيز, سائيز), shineColor);
	view.جوڙيو_سٽ(BPoint(سائيز, سائيز), BPoint(0, سائيز),    shineColor);
	view.جوڙيو_سٽ(BPoint(0, سائيز),	 BPoint(0, 0),	     shadowColor);
	view.پڇاڙي_سٽ_Array();
    }

    view.پڇاڙي_Picture();
    Remove_ٻار(&view);

    أرجع سچ;
}

    منطقي
VimToolbar::جوڙيو_Button(int32 اشاريو, vimmenu_T *menu)
{
    BPictureButton *button = عدم;
    إذا (!menu_is_separator(menu->name)) {
	عائم سائيز = عام_Buttons_Bitmap ?
	    عام_Buttons_Bitmap->Bounds().اوچائي_2() + 1. + ButtonMargin * 2 : 18.;
	BRect چوکٺو(0, 0, سائيز, سائيز);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture picture_ناسي;

	إذا (menu->iconfile == عدم && menu->iconidx >= 0 && عام_Buttons_Bitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, عام_Buttons_Bitmap, سچ);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, عام_Buttons_Bitmap, خطأ);
	    GetPictureFromBitmap(&picture_ناسي, menu->iconidx, grayedButtonsBitmap, خطأ);
	} وإلا {

	    char_u بفر[MAXPATHL] = {0};
	    BBitmap *bitmap = عدم;

	    إذا (menu->iconfile) {
		gui_ڳوليو_iconfile(menu->iconfile, بفر, (حرف*)"png");
		bitmap = BTranslationUtils::GetBitmap((حرف*)بفر);
	    }

	    إذا (bitmap == عدم && gui_ڳوليو_bitmap(menu->name, بفر, (حرف*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((حرف*)بفر);

	    إذا (bitmap == عدم)
		bitmap = جديد BBitmap(BRect(0, 0, سائيز, سائيز), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, سچ);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, خطأ);
	    Modify_Bitmap_منزل_Grayed(bitmap);
	    GetPictureFromBitmap(&picture_ناسي, 0, bitmap, خطأ);

	    ميٽيو bitmap;
	}

	button = جديد BPictureButton(چوکٺو, (حرف*)menu->name,
		    &pictureOff, &pictureOn, Menu_پيغام(menu));

	button->سيٽ_Disabled_On(&picture_ناسي);
	button->سيٽ_Disabled_Off(&picture_ناسي);

	button->سيٽ_هدف(gui.vim_متن_Area);

	جوڙيو_ٻار(button);

	menu->button = button;
    }

    منطقي نتيجو = fButtonsList.جوڙيو_شي(button, اشاريو);
    InvalidateLayout();
    أرجع نتيجو;
}

    منطقي
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    إذا (menu->button) {
	إذا (fButtonsList.Remove_شي(menu->button)) {
	    ميٽيو menu->button;
	    menu->button = عدم;
	}
    }
    أرجع سچ;
}

    منطقي
VimToolbar::ناسي_Button(vimmenu_T *menu, صحيح grey)
{
    إذا (menu->button) {
	int32 اشاريو = fButtonsList.اشاريو_جو(menu->button);
	إذا (اشاريو >= 0)
	    menu->button->سيٽ_Enabled(grey ? خطأ : سچ);
    }
    أرجع سچ;
}

    فراغ
VimToolbar::InvalidateLayout()
{
    int32 وٿي = ToolbarMargin;
    int32 ڳڻپ = fButtonsList.ڳڻپ_شيون();
    لكل (int32 i = 0; i < ڳڻپ; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.شي_At(i);
	إذا (button) {
	    button->Move_منزل(وٿي, ToolbarMargin);
	    وٿي += button->Bounds().ويڪر_2() + ToolbarMargin;
	} وإلا
	    وٿي += ToolbarMargin * 3;
    }
}

#جيڪڏهن_پورو /*FEAT_TOOLBAR*/

#إذا defined(FEAT_GUI_TABLINE)

    عائم
Vim_Tab_سٽ::Tabline_اوچائي() ثابت
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    أرجع Tab_اوچائي(); //  + ToolbarMargin;
}

فراغ
Vim_Tab_سٽ::Mouse_هيٺ_ڏانهن(BPoint نقطو)
{
    إذا (!gui_mch_showing_tabline())
	أرجع;

    BMessage *m = دري()->Current_پيغام();
    أكد(m);

    int32 buttons = 0;
    m->ڳوليو_Int32("buttons", &buttons);

    int32 clicks = 0;
    m->ڳوليو_Int32("clicks", &clicks);

    صحيح اشاريو = 0; //  0 means here - no tab found
    لكل (صحيح i = 0; i < ڳڻپ_Tabs(); i++) {
	إذا (Tab_چوکٺو(i).شامل_آهي_2(نقطو)) {
	    اشاريو = i + 1; //  indexes are 1-based
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
	event = اشاريو > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    وإلا إذا (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPop_مٿي_Menu* ڪڍو_مٿي_Menu = جديد BPop_مٿي_Menu("tabLineContextMenu", خطأ, خطأ);
	ڪڍو_مٿي_Menu->جوڙيو_شي(جديد BMenu_شي(_("Close tabi R"), جديد BMessage(TABLINE_MENU_CLOSE)));
	ڪڍو_مٿي_Menu->جوڙيو_شي(جديد BMenu_شي(_("New tab    T"), جديد BMessage(TABLINE_MENU_NEW)));
	ڪڍو_مٿي_Menu->جوڙيو_شي(جديد BMenu_شي(_("Open tab..."), جديد BMessage(TABLINE_MENU_OPEN)));

	Convert_منزل_Screen(&نقطو);
	BMenu_شي* شي = ڪڍو_مٿي_Menu->Go(نقطو);
	إذا (شي != عدم) {
	    event = شي->Command();
	}

	ميٽيو ڪڍو_مٿي_Menu;

    } وإلا {
	//  default processing
	BTabView::Mouse_هيٺ_ڏانهن(نقطو);
	أرجع;
    }

    إذا (event < 0)
	أرجع;

    Vim_Tabline_Menu_نياپو tmm;
    tmm.اشاريو = اشاريو;
    tmm.event = event;
    write_پورٽ(gui.vdcmp, Vim_نياپو::TablineMenu, &tmm, حجم(tmm));
}

فراغ
Vim_Tab_سٽ::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_سٽ *tab_سٽ = gui.vimForm->Tab_سٽ();
    إذا (tab_سٽ != عدم) {

	int32 i = 0;
	لكل (; i < tab_سٽ->ڳڻپ_Tabs(); i++)
	    إذا (هذا == tab_سٽ->TabAt(i))
		اكسر;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	إذا (i < tab_سٽ->ڳڻپ_Tabs()) {
	    Vim_Tabline_نياپو tm;
	    tm.اشاريو = i + 1;
	    write_پورٽ(gui.vdcmp, Vim_نياپو::Tabline, &tm, حجم(tm));
	}
    }
}

#جيڪڏهن_پورو //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ساكن حرف appsig[] = "application/x-vnd.Haiku-Vim-8";
ڪنجي_map *ڪنجي_Map;
حرف *ڪنجي_Map_Chars;
صحيح main_exitcode = 127;

    حالت_t
gui_haiku_process_event(bigtime_t timeout)
{
    بنية Vim_نياپو vm;
    int32 what;
    ssize_t سائيز;

    سائيز = read_پورٽ_etc(gui.vdcmp, &what, &vm, حجم(vm),
	    B_TIMEOUT, timeout);

    إذا (سائيز >= 0) {
	بدل (what) {
	    حالة Vim_نياپو::ڪنجي_2:
		{
		    char_u *string = vm.u.ڪنجي_2.chars;
		    صحيح len = vm.u.ڪنجي_2.ڊيگهائي;
		    إذا (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_داخلا_buf();
			got_int = TRUE;
		    }

		    إذا (vm.u.ڪنجي_2.csi_escape)
#جيڪڏهن_وصف_نه_ٿيل FEAT_MBYTE_IME
		    {
			صحيح	i;
			char_u	buf[2];

			لكل (i = 0; i < len; ++i)
			{
			    جوڙيو_منزل_داخلا_buf(string + i, 1);
			    إذا (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (صحيح)KE_CSI;
				جوڙيو_منزل_داخلا_buf(buf, 2);
			    }
			}
		    }
#وإلا
			جوڙيو_منزل_داخلا_buf_csi(string, len);
#جيڪڏهن_پورو
		    وإلا
			جوڙيو_منزل_داخلا_buf(string, len);
		}
		اكسر;
	    حالة Vim_نياپو::Resize:
		gui_resize_shell(vm.u.New_سائيز.ويڪر, vm.u.New_سائيز.اوچائي);
		اكسر;
	    حالة Vim_نياپو::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_ڳڻپ =
			atomic_جوڙيو(&vm.u.Scroll.sb->scroll_Event_ڳڻپ, -1);
		    إذا (old_ڳڻپ <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.قدر, vm.u.Scroll.stillDragging);
		}
		اكسر;
#إذا defined(FEAT_MENU)
	    حالة Vim_نياپو::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		اكسر;
#جيڪڏهن_پورو
	    حالة Vim_نياپو::Mouse:
		{
		    int32 old_ڳڻپ;
		    إذا (vm.u.Mouse.button == MOUSE_DRAG)
			old_ڳڻپ =
			    atomic_جوڙيو(&gui.vim_متن_Area->mouse_Drag_Event_ڳڻپ, -1);
		    وإلا
			old_ڳڻپ = 0;
		    إذا (old_ڳڻپ <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		اكسر;
	    حالة Vim_نياپو::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		اكسر;
	    حالة Vim_نياپو::Focus:
		gui.اندريون_focus = vm.u.Focus.سرگرم;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		إذا (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		اكسر;
	    حالة Vim_نياپو::Refs:
		::RefsReceived(vm.u.Refs.پيغام, vm.u.Refs.changedir);
		اكسر;
	    حالة Vim_نياپو::Tabline:
		send_tabline_event(vm.u.Tabline.اشاريو);
		اكسر;
	    حالة Vim_نياپو::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.اشاريو, vm.u.TablineMenu.event);
		اكسر;
	    افتراضي:
		//  unrecognised message, ignore it
		اكسر;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    أرجع سائيز;
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
vim_تالو_screen()
{
    أرجع !gui.vim_دري || gui.vim_دري->تالو_2();
}

    فراغ
vim_unlock_screen()
{
    إذا (gui.vim_دري)
	gui.vim_دري->Unlock();
}

#وصف_ڪريو لاڳيتو_BAPPLICATION_اندريون_NEW_THREAD	0

#إذا لاڳيتو_BAPPLICATION_اندريون_NEW_THREAD

    int32
لاڳيتو_vimapp(فراغ *دليلون)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.لاڳيتو_2();		    // Run until Quit() called

    أرجع 0;
}

#وإلا

    int32
call_main(فراغ *دليلون)
{
    بنية Main_دليلون *ma = (Main_دليلون *)دليلون;

    أرجع مکيه(ma->argc, ma->argv);
}
#جيڪڏهن_پورو

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
	get_thread_info(ڳوليو_thread(عدم), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_پورٽ(B_MAX_پورٽ_ڳڻپ, "vim VDCMP");

#إذا لاڳيتو_BAPPLICATION_اندريون_NEW_THREAD
	thread_id tid = spawn_thread(لاڳيتو_vimapp, "vim VimApp",
		tinfo.ترجيح, عدم);
	إذا (tid >= B_OK) {
	    resume_thread(tid);
	} وإلا {
	    getout(1);
	}
#وإلا
	Main_دليلون ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.ترجيح, &ma);
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
	    app.لاڳيتو_2();		    // Run until Quit() called
	    // fprintf(stderr, "app.Run() returned...\n");
	    حالت_t dummy_exitcode;
	    (فراغ)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    ٻاهر_نڪرو(main_exitcode);
	}
#جيڪڏهن_پورو
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
    gui.def_پٺ_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.پٺ_pixel = gui.def_پٺ_pixel;

    gui.scrollbar_ويڪر = (صحيح) B_V_SCROLL_BAR_ويڪر;
    gui.scrollbar_اوچائي = (صحيح) B_H_SCROLL_BAR_اوچائي;
#جيڪڏهن_وصف_ٿيل FEAT_MENU
    gui.menu_اوچائي = 19;   //	initial guess -
    //	correct for my default settings
#جيڪڏهن_پورو
    gui.border_وٿي = 3;  //	coordinates are inside window borders

    إذا (gui.vdcmp < B_OK)
	أرجع FAIL;
    get_ڪنجي_map(&ڪنجي_Map, &ڪنجي_Map_Chars);

    gui.vim_دري = جديد Vim_دري();	// hidden and locked
    إذا (!gui.vim_دري)
	أرجع FAIL;

    gui.vim_دري->لاڳيتو_2();	// Run() unlocks but does not show

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    سيٽ_عام_colors();

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
    rgb_color rgb = GUI_منزل_RGB(gui.پٺ_pixel);

    إذا (gui.vim_دري->تالو_2()) {
	gui.vimForm->سيٽ_View_Color(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vim_دري->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open()
{
    إذا (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_سيٽ_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_دري->Show();
	gui.vim_دري->Unlock();
	أرجع OK;
    }

    أرجع FAIL;
}

    فراغ
gui_mch_exit(صحيح vim_exitcode)
{
    إذا (gui.vim_دري) {
	thread_id tid = gui.vim_دري->Thread();
	gui.vim_دري->تالو_2();
	gui.vim_دري->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_پورٽ(gui.vdcmp);
#إذا !لاڳيتو_BAPPLICATION_اندريون_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#جيڪڏهن_پورو
    إذا (gui.vimApp) {
	Vim_متن_Area_View::guiBlankMouse(خطأ);

	main_exitcode = vim_exitcode;
#إذا لاڳيتو_BAPPLICATION_اندريون_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->تالو_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#وإلا
	gui.vimApp->تالو_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	// suicide
	exit_thread(vim_exitcode);
#جيڪڏهن_پورو
    }
    // If we are somehow still here, let mch_exit() handle things.
}

/*
 * Get the position of the top left corner of the window.
 */
    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    إذا (gui.vim_دري->تالو_2()) {
	BRect r;
	r = gui.vim_دري->چوکٺو_2();
	gui.vim_دري->Unlock();
	*x = (صحيح)r.کاٻي;
	*y = (صحيح)r.چوٽي;
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
gui_mch_سيٽ_winpos(صحيح x, صحيح y)
{
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_دري->Move_منزل(x, y);
	gui.vim_دري->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
فراغ
gui_mch_سيٽ_shellsize(
	صحيح	ويڪر,
	صحيح	اوچائي,
	صحيح	min_ويڪر,
	صحيح	min_اوچائي,
	صحيح	بنياد_ويڪر,
	صحيح	بنياد_اوچائي,
	صحيح	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_دري->Resize_منزل(ويڪر - PEN_ويڪر, اوچائي - PEN_ويڪر);

	// set size limits
	عائم min_ويڪر_2, max_ويڪر, min_اوچائي_2, max_اوچائي;

	gui.vim_دري->Get_سائيز_Limits(&min_ويڪر_2, &max_ويڪر,
		&min_اوچائي_2, &max_اوچائي);
	gui.vim_دري->سيٽ_سائيز_Limits(min_ويڪر, max_ويڪر,
		min_اوچائي, max_اوچائي);

	/*
	 * Set the resizing alignment depending on font size.
	 */
	gui.vim_دري->سيٽ_دري_Alignment(
		B_PIXEL_ALIGNMENT,	//  window_alignment mode,
		1,		//  int32 h,
		0,		//  int32 hOffset = 0,
		gui.char_ويڪر,	    //	int32 width = 0,
		بنياد_ويڪر,	    //	int32 widthOffset = 0,
		1,		//  int32 v = 0,
		0,		//  int32 vOffset = 0,
		gui.char_اوچائي,	//  int32 height = 0,
		بنياد_اوچائي	    //	int32 heightOffset = 0
		);

	gui.vim_دري->Unlock();
    }
}

فراغ
gui_mch_get_screen_dimensions(
	صحيح	*screen_w,
	صحيح	*screen_h)
{
    BRect چوکٺو;

    {
	BScreen screen(gui.vim_دري);

	إذا (screen.IsValid()) {
	    چوکٺو = screen.چوکٺو_2();
	} وإلا {
	    چوکٺو.ساڄي = 640;
	    چوکٺو.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (صحيح) چوکٺو.ساڄي - 2 * gui.scrollbar_ويڪر - 20;
    *screen_h = (صحيح) چوکٺو.bottom - gui.scrollbar_اوچائي
#جيڪڏهن_وصف_ٿيل FEAT_MENU
	- gui.menu_اوچائي
#جيڪڏهن_پورو
	- 30;
}

فراغ
gui_mch_سيٽ_متن_area_pos(
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    إذا (!gui.vim_متن_Area)
	أرجع;

    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_متن_Area->Move_منزل(x, y);
	gui.vim_متن_Area->Resize_منزل(w - PEN_ويڪر, h - PEN_ويڪر);

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
	إذا (gui.vimForm->Tab_سٽ() != عدم) {
	    gui.vimForm->Tab_سٽ()->Resize_منزل(w, gui.vimForm->Tabline_اوچائي());
	}
#جيڪڏهن_پورو // FEAT_GUI_TABLINE

	gui.vim_دري->Unlock();
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
    إذا (gui.vim_دري->تالو_2()) {
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
	gui.vim_دري->Unlock();
    }
}

فراغ
gui_mch_سيٽ_scrollbar_thumb(
	scrollbar_T *sb,
	صحيح	val,
	صحيح	سائيز,
	صحيح	max)
{
    إذا (gui.vim_دري->تالو_2()) {
	VimScrollBar *s = sb->id;
	إذا (max == 0) {
	    s->سيٽ_قدر(0);
	    s->سيٽ_Range(0.0, 0.0);
	} وإلا {
	    s->سيٽ_Proportion((عائم)سائيز / (max + 1.0));
	    s->سيٽ_قدمن(1.0, سائيز > 5 ? سائيز - 2 : سائيز);
#جيڪڏهن_وصف_نه_ٿيل SCROLL_PAST_پڇاڙي	    //	really only defined in gui.c...
	    max = max + 1 - سائيز;
#جيڪڏهن_پورو
	    إذا (max < s->قدر_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->سيٽ_قدر(val);
		s->سيٽ_Range(0.0, max);
	    } وإلا {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->سيٽ_Range(0.0, max);
		s->سيٽ_قدر(val);
	    }
	}
	gui.vim_دري->Unlock();
    }
}

فراغ
gui_mch_سيٽ_scrollbar_pos(
	scrollbar_T *sb,
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    إذا (gui.vim_دري->تالو_2()) {
	BRect winb = gui.vim_دري->Bounds();
	عائم vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->Resize_منزل(w - PEN_ويڪر, h - PEN_ويڪر);
	إذا (winb.ساڄي-(x+w)<w) vsbx = winb.ساڄي - (w - PEN_ويڪر);
	vsb->Move_منزل(vsbx, vsby);
	gui.vim_دري->Unlock();
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
    إذا (gui.vim_دري->تالو_2()) {
	vsb->سيٽ_هدف(gui.vim_متن_Area);
	vsb->Hide();
	gui.vimForm->جوڙيو_ٻار(vsb);
	gui.vim_دري->Unlock();
    }
}

#إذا defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
فراغ
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    إذا (gui.vim_دري->تالو_2()) {
	sb->id->RemoveSelf();
	ميٽيو sb->id;
	gui.vim_دري->Unlock();
    }
}
#جيڪڏهن_پورو

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

#وصف_ڪريو BLINK_NONE  0
#وصف_ڪريو BLINK_OFF   1
#وصف_ڪريو BLINK_ON    2

ساكن صحيح	blink_state = BLINK_NONE;
ساكن long_u	    blink_waittime = 700;
ساكن long_u	    blink_ontime = 400;
ساكن long_u	    blink_offtime = 250;
ساكن صحيح  blink_timer = 0;

فراغ
gui_mch_سيٽ_blinking(
	ڊگهو	waittime,
	ڊگهو	on,
	ڊگهو	off)
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
gui_mch_شروعات_blink()
{
    // TODO
    إذا (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.اندريون_focus)
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
    إذا (gui.vim_دري->تالو_2())
    {
	صحيح rc = gui.vim_متن_Area->mchInitFont(font_name);
	gui.vim_دري->Unlock();

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
gui_mch_font_dialog(font_family* family, font_style* style, عائم* سائيز)
{
#إذا defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = جديد VimSelectFontDialog(family, style, سائيز);
    منطقي ret = dialog->Go();
	ميٽيو dialog;
	أرجع ret;
#وإلا
    أرجع NOFONT;
#جيڪڏهن_پورو // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	صحيح	    giveErrorIfMissing)
{
    ساكن VimFont *fontList = عدم;

    إذا (!gui.اندريون_use)	//  can't do this when GUI not running
	أرجع NOFONT;

    //	storage for locally modified name;
    ثابت صحيح buff_سائيز = B_FONT_FAMILY_ڊيگهائي + B_FONT_STYLE_ڊيگهائي + 20;
    ساكن حرف font_name[buff_سائيز] = {0};
    font_family family = {0};
    font_style	style  = {0};
    عائم سائيز = 0.f;

    إذا (name == 0 && be_fixed_font == 0) {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	أرجع NOFONT;
    }

    منطقي useSelectGUI = خطأ;
    إذا (name != عدم)
	إذا (STRCMP(name, "*") == 0) {
	    useSelectGUI = سچ;
	    STRNCPY(font_name, hl_get_font_name(), buff_سائيز);
	} وإلا
	    STRNCPY(font_name, name, buff_سائيز);

    إذا (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	سائيز = be_fixed_font->سائيز_2();
	vim_snprintf(font_name, buff_سائيز,
	    (حرف*)"%s/%s/%.0f", family, style, سائيز);
    }

    //	replace underscores with spaces
    حرف* پڇاڙي = 0;
    طالما (پڇاڙي = strchr((حرف *)font_name, '_'))
	*پڇاڙي = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ساكن حرف buff[buff_سائيز] = {0};
    STRNCPY(buff, font_name, buff_سائيز);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_ڊيگهائي);
    حرف* style_s = strtok(0, "/\0");
    إذا (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_ڊيگهائي);
    سائيز = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    إذا (useSelectGUI) {
	إذا (gui_mch_font_dialog(&family, &style, &سائيز) == NOFONT)
	    أرجع FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_سائيز,
		(حرف*)"%s/%s/%.0f", family, style, سائيز);
	hl_سيٽ_font_name((char_u*)font_name);

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
    لكل (flp = fontList; flp; flp = flp->ايندڙ) {
	إذا (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    أرجع (GuiFont)flp;
	}
    }

    VimFont *font = جديد VimFont();
    font->name = vim_strsave((char_u*)font_name);

    إذا (ڳڻپ_font_styles(family) <= 0) {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	ميٽيو font;
	أرجع NOFONT;
    }

    //	Remember font in the static list for later use
    font->ايندڙ = fontList;
    fontList = font;

    font->سيٽ_Family_And_Style(family, style);
    إذا (سائيز > 0.f)
	font->سيٽ_سائيز(سائيز);

    font->سيٽ_Spacing(B_FIXED_SPACING);
    font->سيٽ_Encoding(B_UNICODE_UTF8);

    أرجع (GuiFont)font;
}

/*
 * Set the current text font.
 */
فراغ
gui_mch_سيٽ_font(
	GuiFont	font)
{
    إذا (gui.vim_دري->تالو_2()) {
	VimFont *vf = (VimFont *)font;

	gui.vim_متن_Area->سيٽ_Font(vf);

	gui.char_ويڪر = (صحيح) vf->String_ويڪر("n");
	font_اوچائي fh;
	vf->Get_اوچائي(&fh);
	gui.char_اوچائي = (صحيح)(fh.ascent + 0.9999)
	    + (صحيح)(fh.descent + 0.9999) + (صحيح)(fh.leading + 0.9999);
	gui.char_ascent = (صحيح)(fh.ascent + 0.9999);

	gui.vim_دري->Unlock();
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
	ميٽيو f;
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
	font_اوچائي fh = {0};
	font->Get_اوچائي(&fh);
	gui.char_اوچائي = (صحيح)(fh.ascent + fh.descent + 0.5) + p_linespace;
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
gui_mch_سيٽ_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_منزل_RGB(color);
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_متن_Area->سيٽ_مٿاهون_Color(rgb);
	gui.vim_دري->Unlock();
    }
}

/*
 * Set the current text background color.
 */
فراغ
gui_mch_سيٽ_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_منزل_RGB(color);
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_متن_Area->سيٽ_هيٺيون_Color(rgb);
	gui.vim_دري->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    فراغ
gui_mch_سيٽ_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

فراغ
gui_mch_draw_string(
	صحيح	صف,
	صحيح	کانو,
	char_u	*s,
	صحيح	len,
	صحيح	flags)
{
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_متن_Area->mchDrawString(صف, کانو, s, len, flags);
	gui.vim_دري->Unlock();
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

    لكل (i = 0; special_ڪنجيون[i].Be_ڪنجيون != 0; i++)
	إذا (name[0] == special_ڪنجيون[i].vim_code0 &&
		name[1] == special_ڪنجيون[i].vim_code1)
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

    إذا (gui.vim_دري->تالو_2()) {
	BRect rect = gui.vim_متن_Area->Bounds();

	gui.vim_متن_Area->سيٽ_Drawing_گهڻائي(B_OP_INVERT);
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->Sync();
	snooze(msec * 1000);	 // wait for a few msec
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->سيٽ_Drawing_گهڻائي(B_OP_COPY);
	gui.vim_متن_Area->Flush();
	gui.vim_دري->Unlock();
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
    rect.کاٻي = FILL_X(c);
    rect.چوٽي = FILL_Y(r);
    rect.ساڄي = rect.کاٻي + nc * gui.char_ويڪر - PEN_ويڪر;
    rect.bottom = rect.چوٽي + nr * gui.char_اوچائي - PEN_ويڪر;

    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_متن_Area->سيٽ_Drawing_گهڻائي(B_OP_INVERT);
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->سيٽ_Drawing_گهڻائي(B_OP_COPY);
	gui.vim_دري->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify()
{
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_دري->Minimize(سچ);
	gui.vim_دري->Unlock();
    }
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_سيٽ_foreground(فراغ)
{
    // TODO
}
#جيڪڏهن_پورو

/*
 * Set the window title
 */
فراغ
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_دري->سيٽ_Title((حرف *)title);
	gui.vim_دري->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_سيٽ_fg_color(color);

    BRect r;
    r.کاٻي = FILL_X(gui.کانو);
    r.چوٽي = FILL_Y(gui.صف);
    صحيح cells = utf_off2cells(سٽ_وٿي[gui.صف] + gui.کانو, 100); // TODO-TODO
    إذا (cells>=4) cells = 1;
    r.ساڄي = r.کاٻي + cells*gui.char_ويڪر - PEN_ويڪر;
    r.bottom = r.چوٽي + gui.char_اوچائي - PEN_ويڪر;

    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_متن_Area->StrokeRect(r);
	gui.vim_دري->Unlock();
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
    gui_mch_سيٽ_fg_color(color);

    BRect r;
    r.کاٻي =
#جيڪڏهن_وصف_ٿيل FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_ساڄو ? FILL_X(gui.کانو + 1) - w :
#جيڪڏهن_پورو
	FILL_X(gui.کانو);
    r.ساڄي = r.کاٻي + w - PEN_ويڪر;
    r.bottom = FILL_Y(gui.صف + 1) - PEN_ويڪر;
    r.چوٽي = r.bottom - h + PEN_ويڪر;

    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_متن_Area->FillRect(r);
	gui.vim_دري->Unlock();
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
    طالما (پورٽ_ڳڻپ(gui.vdcmp) > 0 &&
	    !vim_is_داخلا_buf_ڀريل() &&
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
	ڊگهو	wtime)
{
    صحيح		focus;
    bigtime_t	until, timeout;
    حالت_t	st;

    إذا (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_وقت() + timeout;
    }
    وإلا
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.اندريون_focus;
    لكل (;;)
    {
	// Stop or start blinking when focus changes
	إذا (gui.اندريون_focus != focus)
	{
	    إذا (gui.اندريون_focus)
		gui_mch_شروعات_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.اندريون_focus;
	}

	gui_mch_flush();

#جيڪڏهن_وصف_ٿيل پيغام_قطار
# ifdef FEAT_TIMERS
	did_جوڙيو_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_جوڙيو_timer)
	    // Need to recompute the waiting time.
	    اكسر;
# endif
# ifdef FEAT_ڪم_CHANNEL
	إذا (has_any_channel())
	{
	    إذا (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	وإلا إذا (wtime < 0)
	    timeout = B_INFINITE_TIMEOUT;
# endif
#جيڪڏهن_پورو

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use gui_haiku_process_event() to hang until
	 * an event arrives.  No need to check for input_buf_full because we
	 * are returning as soon as it contains a single char.
	 */
	st = gui_haiku_process_event(timeout);

	إذا (داخلا_موجود())
	    أرجع OK;
	إذا (st < B_OK)		// includes B_TIMED_OUT
	    أرجع FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	إذا (wtime >= 0)
	{
	    timeout = until - system_وقت();
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
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_دري->Flush();
	gui.vim_دري->Unlock();
    }
    أرجع;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
فراغ
gui_mch_clear_بلاڪ(
	صحيح	row1,
	صحيح	col1,
	صحيح	row2,
	صحيح	col2)
{
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_متن_Area->mch_Clear_بلاڪ(row1, col1, row2, col2);
	gui.vim_دري->Unlock();
    }
}

    فراغ
gui_mch_clear_all()
{
    إذا (gui.vim_دري->تالو_2()) {
	gui.vim_متن_Area->mchClearAll();
	gui.vim_دري->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
فراغ
gui_mch_delete_سٽون(
	صحيح	صف,
	صحيح	انگ_سٽون)
{
    gui.vim_متن_Area->mch_Delete_سٽون(صف, انگ_سٽون);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
فراغ
gui_mch_وجهو_سٽون(
	صحيح	صف,
	صحيح	انگ_سٽون)
{
    gui.vim_متن_Area->mch_وجهو_سٽون(صف, انگ_سٽون);
}

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

فراغ
gui_mch_enable_menu(
	صحيح	flag)
{
    إذا (gui.vim_دري->تالو_2())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->سيٽ_Enabled(flag);
	gui.vim_دري->Unlock();
    }
}

فراغ
gui_mch_سيٽ_menu_pos(
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
gui_mch_جوڙيو_menu(
	vimmenu_T   *menu,
	صحيح	idx)
{
    vimmenu_T	*والد = menu->والد;

    //	popup menu - just create it unattached
    إذا (menu_is_popup(menu->name) && والد == عدم) {
	BPop_مٿي_Menu* ڪڍو_مٿي_Menu = جديد BPop_مٿي_Menu((ثابت حرف*)menu->name, خطأ, خطأ);
	menu->submenu_id = ڪڍو_مٿي_Menu;
	menu->id = عدم;
	أرجع;
    }

    إذا (!menu_is_menubar(menu->name)
	    || (والد != عدم && والد->submenu_id == عدم))
	أرجع;

    إذا (gui.vim_دري->تالو_2())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	إذا ( والد )
	    tmp = والد->submenu_id;
	وإلا
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	إذا ( ! tmp->ڳوليو_شي((ثابت حرف *) menu->dname)) {

	    BMenu *bmenu = جديد BMenu((حرف *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->جوڙيو_شي(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->ڳوليو_شي((ثابت حرف *) menu->dname);

	}
	gui.vim_دري->Unlock();
    }
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    // no tearoff menus
}

    ساكن BMessage *
Menu_پيغام(vimmenu_T *menu)
{
    BMessage *m = جديد BMessage('menu');
    m->جوڙيو_اشارو("VimMenu", (فراغ *)menu);

    أرجع m;
}

/*
 * Add a menu item to a menu
 */
فراغ
gui_mch_جوڙيو_menu_شي(
	vimmenu_T   *menu,
	صحيح	idx)
{
    صحيح	    mnemonic = 0;
    vimmenu_T	*والد = menu->والد;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    إذا (gui.vim_دري->تالو_2())
    {
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
	إذا (menu_is_toolbar(والد->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->جوڙيو_Button(idx, menu);
	    }
	} وإلا
#جيڪڏهن_پورو

	إذا (والد->submenu_id != عدم || menu_is_popup(والد->name)) {
	    إذا (menu_is_separator(menu->name)) {
		BSeparator_شي *شي = جديد BSeparator_شي();
		والد->submenu_id->جوڙيو_شي(شي);
		menu->id = شي;
		menu->submenu_id = عدم;
	    }
	    وإلا {
		BMenu_شي *شي = جديد BMenu_شي((حرف *)menu->dname,
			Menu_پيغام(menu));
		شي->سيٽ_هدف(gui.vim_متن_Area);
		شي->سيٽ_Trigger((حرف) menu->mnemonic);
		والد->submenu_id->جوڙيو_شي(شي);
		menu->id = شي;
		menu->submenu_id = عدم;
	    }
	}
	gui.vim_دري->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
فراغ
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    إذا (gui.vim_دري->تالو_2())
    {
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
	إذا (menu->والد && menu_is_toolbar(menu->والد->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->RemoveButton(menu);
	    }
	} وإلا
#جيڪڏهن_پورو
	{
	    أكد(menu->submenu_id == عدم || menu->submenu_id->ڳڻپ_شيون() == 0);
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
		bmenu->Remove_شي(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		إذا (bmenu == gui.vimForm->MenuBar() && bmenu->ڳڻپ_شيون() == 0)
		{
		    bmenu->Resize_منزل(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    ميٽيو menu->id;
	    menu->id = عدم;
	    menu->submenu_id = عدم;

	    gui.menu_اوچائي = (صحيح) gui.vimForm->Menu_اوچائي();
	}
	gui.vim_دري->Unlock();
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
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    إذا (menu->والد && menu_is_toolbar(menu->والد->name)) {
	إذا (gui.vim_دري->تالو_2()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->ناسي_Button(menu, grey);
	    }
	    gui.vim_دري->Unlock();
	}
    } وإلا
#جيڪڏهن_پورو
    إذا (menu->id != عدم)
	menu->id->سيٽ_Enabled(!grey);
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
	menu->id->سيٽ_Enabled(!hidden);
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

    BPop_مٿي_Menu* popupMenu = dynamic_cast<BPop_مٿي_Menu*>(menu->submenu_id);
    إذا (popupMenu == عدم)
	أرجع;

    BPoint نقطو;
    إذا (gui.vim_دري->تالو_2()) {
	uint32 buttons = 0;
	gui.vim_متن_Area->GetMouse(&نقطو, &buttons);
	gui.vim_متن_Area->Convert_منزل_Screen(&نقطو);
	gui.vim_دري->Unlock();
    }
    popupMenu->Go(نقطو, سچ);
}

#جيڪڏهن_پورو // FEAT_MENU

// Mouse stuff

#جيڪڏهن_وصف_ٿيل FEAT_CLIPBOARD
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
    إذا (be_clipboard->تالو_2())
    {
	BMessage *m = be_clipboard->معلومات_2();
	// m->PrintToStream();

	char_u *string = عدم;
	ssize_t stringlen = -1;

	إذا (m->ڳوليو_معلومات(textplain, B_MIME_TYPE,
		    (ثابت فراغ **)&string, &stringlen) == B_OK
		|| m->ڳوليو_String("text", (ثابت حرف **)&string) == B_OK)
	{
	    إذا (stringlen == -1)
		stringlen = STRLEN(string);

	    صحيح type;
	    حرف *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    إذا (m->ڳوليو_معلومات(vimselectiontype, B_MIME_TYPE,
			(ثابت فراغ **)&seltype, &seltypelen) == B_OK)
	    {
		بدل (*seltype)
		{
		    افتراضي:
		    حالة 'L':	type = MLINE;	اكسر;
		    حالة 'C':	type = MCHAR;	اكسر;
#جيڪڏهن_وصف_ٿيل FEAT_VISUAL
		    حالة 'B':	type = MBLOCK;	اكسر;
#جيڪڏهن_پورو
		}
	    }
	    وإلا
	    {
		// Otherwise use heuristic as documented
		type = memchr(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (ڊگهو)stringlen, cbd);
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
clip_mch_سيٽ_selection(Clipboard_T *cbd)
{
    إذا (be_clipboard->تالو_2())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->معلومات_2();
	أكد(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = عدم;
	long_u	ڳڻپ;
	صحيح type;

	type = clip_convert_selection(&str, &ڳڻپ, cbd);

	إذا (type < 0)
	    أرجع;

	m->جوڙيو_معلومات(textplain, B_MIME_TYPE, (فراغ *)str, ڳڻپ);

	// Add type of selection
	حرف	vtype;
	بدل (type)
	{
	    افتراضي:
	    حالة MLINE:    vtype = 'L';    اكسر;
	    حالة MCHAR:    vtype = 'C';    اكسر;
#جيڪڏهن_وصف_ٿيل FEAT_VISUAL
	    حالة MBLOCK:   vtype = 'B';    اكسر;
#جيڪڏهن_پورو
	}
	m->جوڙيو_معلومات(vimselectiontype, B_MIME_TYPE, (فراغ *)&vtype, 1);

	vim_free(str);

	be_clipboard->Commit();
	be_clipboard->Unlock();
    }
}

#جيڪڏهن_پورو	// FEAT_CLIPBOARD

#جيڪڏهن_وصف_ٿيل FEAT_BROWSE
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
	    جديد BMessage((saving == TRUE) ? 'save' : 'open'), عدم, سچ);

    gui.vimApp->f_Browsed_رستو.Unset();

    gui.vimApp->fFilePanel->دري()->سيٽ_Title((حرف*)title);
    gui.vimApp->fFilePanel->سيٽ_Panel_Directory((ثابت حرف*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    طالما (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = عدم;
    حالت_t نتيجو = gui.vimApp->f_Browsed_رستو.InitCheck();
    إذا (نتيجو == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->f_Browsed_رستو.رستو_2());
    } وإلا
	إذا (نتيجو != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    نتيجو, strerror(نتيجو));
	}

    ميٽيو gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = عدم;

    أرجع fileName;
}
#جيڪڏهن_پورو // FEAT_BROWSE


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
	char_u	*پيغام,
	char_u	*buttons,
	صحيح	 dfltbutton,
	char_u	*textfield,
	صحيح ex_cmd)
{
    VimDialog *dialog = جديد VimDialog(type, (حرف*)title, (حرف*)پيغام,
	    (حرف*)buttons, dfltbutton, (حرف*)textfield, ex_cmd);
    منطقي ret = dialog->Go();
    ميٽيو dialog;
	أرجع ret;
}

#جيڪڏهن_پورو // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_منزل_RGB(pixel);

    أرجع ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    TRACE();
    // TODO
}

#جيڪڏهن_وصف_ٿيل FEAT_MBYTE_IME
    فراغ
im_سيٽ_position(صحيح صف, صحيح کانو)
{
    إذا (gui.vim_دري->تالو_2())
    {
	gui.vim_متن_Area->DrawIMString();
	gui.vim_دري->Unlock();
    }
    أرجع;
}
#جيڪڏهن_پورو

    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_اوچائي = (toolbar && showit) ? toolbar->Toolbar_اوچائي() : 0.;
}

    فراغ
gui_mch_سيٽ_toolbar_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    إذا (toolbar != عدم) {
	إذا (gui.vim_دري->تالو_2()) {
	    toolbar->Move_منزل(x, y);
	    toolbar->Resize_منزل(w - 1, h - 1);
	    gui.vim_دري->Unlock();
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
    Vim_Tab_سٽ *tab_سٽ = gui.vimForm->Tab_سٽ();

    إذا (tab_سٽ == عدم)
	أرجع;

    إذا (!showit != !gui.vimForm->Is_Showing_Tab_سٽ()) {
	gui.vimForm->سيٽ_Showing_Tab_سٽ(showit != 0);
	gui.tabline_اوچائي = gui.vimForm->Tabline_اوچائي();
    }
}

    فراغ
gui_mch_سيٽ_tabline_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    Vim_Tab_سٽ *tab_سٽ = gui.vimForm->Tab_سٽ();
    إذا (tab_سٽ != عدم) {
	إذا (gui.vim_دري->تالو_2()) {
	    tab_سٽ->Move_منزل(x, y);
	    tab_سٽ->Resize_منزل(w - 1, h - 1);
	    gui.vim_دري->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحيح
gui_mch_showing_tabline()
{
    Vim_Tab_سٽ *tab_سٽ = gui.vimForm->Tab_سٽ();
    أرجع tab_سٽ != عدم && gui.vimForm->Is_Showing_Tab_سٽ();
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

    Vim_Tab_سٽ *tab_سٽ = gui.vimForm->Tab_سٽ();

    إذا (tab_سٽ == عدم)
	أرجع;

    gui.vim_دري->تالو_2();

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = پهريون_tabpage; tp != عدم; tp = tp->tp_ايندڙ, ++nr) {
	إذا (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_سٽ->TabAt(nr);

	إذا (tab == عدم) {
	    tab = جديد Vim_Tab_سٽ::VimTab();
	    tab_سٽ->جوڙيو_Tab(عدم, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->سيٽ_Label((ثابت حرف*)NameBuff);
	tab_سٽ->Invalidate();
    }

    // Remove any old labels.
    طالما (nr < tab_سٽ->ڳڻپ_Tabs())
	tab_سٽ->RemoveTab(nr);

    إذا (tab_سٽ->Selection() != curtabidx)
	tab_سٽ->Select(curtabidx);

    gui.vim_دري->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_سيٽ_curtab(صحيح nr)
{
    Vim_Tab_سٽ *tab_سٽ = gui.vimForm->Tab_سٽ();
    إذا (tab_سٽ == عدم)
	أرجع;

    gui.vim_دري->تالو_2();

    إذا (tab_سٽ->Selection() != nr -1)
	tab_سٽ->Select(nr -1);

    gui.vim_دري->Unlock();
}

#جيڪڏهن_پورو // FEAT_GUI_TABLINE
