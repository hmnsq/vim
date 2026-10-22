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
صنف Vim_کھڑکی;
صنف VimToolbar;
صنف Vim_Tab_سطر;

خارجي کلید_map *کلید_Map;
خارجي حرف *کلید_Map_Chars;

خارجي صحيح مکھ(صحيح argc, حرف **argv);

#جے_بنا_تعریف B_MAX_پورٹ_گنتی
#تعریف_کرو B_MAX_پورٹ_گنتی    255
#جے_انت

// VimApp seems comparable to the X "vimShell"
صنف VimApp: عام BApplication
{
    تعريفنوع BApplication Inherited;
    عام:
    VimApp(ثابت حرف *appsig);
    ~VimApp();

    // callbacks:
#إذا 0
    مجازی فراغ Dispatch_سنیہا(BMessage *m, BHandler *h)
    {
	m->Print_منزل_دھارا();
	Inherited::Dispatch_سنیہا(m, h);
    }
#جے_انت
    مجازی فراغ Ready_منزل_لگاتار();
    مجازی فراغ ArgvReceived(int32 argc, حرف **argv);
    مجازی فراغ RefsReceived(BMessage *m);
    مجازی منطقي QuitRequested();
    مجازی فراغ سنیہا_Received(BMessage *m);

    ساكن فراغ SendRefs(BMessage *m, منطقي changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	f_Browsed_رستہ;
    خاص:
};

صنف Vim_کھڑکی: عام BWindow
{
    تعريفنوع BWindow Inherited;
    عام:
    Vim_کھڑکی();
    ~Vim_کھڑکی();

    مجازی فراغ Dispatch_سنیہا(BMessage *m, BHandler *h);
    مجازی فراغ کھڑکی_Activated(منطقي سرگرم);
    مجازی منطقي QuitRequested();

    VimFormView	    *formView;

    خاص:
    فراغ init();

    منطقي is_fullscreen = خطأ;
    BRect saved_چوکھٹا;
    کھڑکی_look saved_look;
};

صنف VimFormView: عام BView
{
    تعريفنوع BView Inherited;
    عام:
    VimFormView(BRect چوکھٹا);
    ~VimFormView();

    // callbacks:
    مجازی فراغ AllAttached();
    مجازی فراغ چوکھٹا_Resized(عائم new_چوڑائی, عائم new_اچائی);

#تعریف_کرو MENUBAR_MARGIN	1
    عائم Menu_اچائی() ثابت
    { أرجع menuBar ? menuBar->چوکھٹا_2().اچائی_2() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ثابت
    { أرجع menuBar; }

    خاص:
    فراغ init(BRect);

    BMenuBar	    *menuBar;
    Vim_متن_Area_View *متن_Area;

#جے_تعریف_شدہ FEAT_TOOLBAR
    عام:
    عائم Toolbar_اچائی() ثابت;
    VimToolbar *ToolBar() ثابت
    { أرجع toolBar; }
    خاص:
    VimToolbar	    *toolBar;
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    عام:
    Vim_Tab_سطر *Tab_سطر() ثابت	{ أرجع tab_سطر; }
    منطقي Is_Showing_Tab_سطر() ثابت { أرجع showing_Tab_سطر; }
    فراغ سیٹ_Showing_Tab_سطر(منطقي showing) { showing_Tab_سطر = showing;	}
    عائم Tabline_اچائی() ثابت;
    خاص:
    Vim_Tab_سطر	*tab_سطر;
    صحيح	showing_Tab_سطر;
#جے_انت
};

صنف Vim_متن_Area_View: عام BView
{
    تعريفنوع BView Inherited;
    عام:
    Vim_متن_Area_View(BRect چوکھٹا);
    ~Vim_متن_Area_View();

    // callbacks:
    مجازی فراغ Draw(BRect updateRect);
    مجازی فراغ کلید_تھلے(ثابت حرف *بائٹاں, int32 عدد_بائٹاں_2);
    مجازی فراغ Mouse_تھلے(BPoint نقطہ);
    مجازی فراغ Mouse_اتے(BPoint نقطہ);
    مجازی فراغ MouseMoved(BPoint نقطہ, uint32 transit, ثابت BMessage *سنیہا);
    مجازی فراغ سنیہا_Received(BMessage *m);

    // own functions:
    صحيح mchInitFont(char_u *name);
    فراغ mchDrawString(صحيح صف, صحيح خانہ, char_u *s, صحيح len, صحيح flags);
    فراغ mch_Clear_بلاک(صحيح row1, صحيح col1, صحيح row2, صحيح col2);
    فراغ mchClearAll();
    فراغ mch_Delete_سطریں(صحيح صف, صحيح عدد_سطریں);
    فراغ mch_داخل_کرو_سطریں(صحيح صف, صحيح عدد_سطریں);

    ساكن فراغ guiSendMouseEvent(صحيح button, صحيح x, صحيح y, صحيح repeated_click, int_u modifiers);
    ساكن فراغ guiMouseMoved(صحيح x, صحيح y);
    ساكن فراغ guiBlankMouse(منطقي should_hide);
    ساكن int_u mouse_Modifiers_منزل_Vim(int32 beModifiers);

    int32 mouse_Drag_Event_گنتی;

#جے_تعریف_شدہ FEAT_MBYTE_IME
    فراغ DrawIMString(فراغ);
#جے_انت

    خاص:
    فراغ init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#جے_تعریف_شدہ FEAT_MBYTE_IME
    بنية {
	BMessenger* messenger;
	BMessage* سنیہا;
	BPoint location;
	صحيح صف;
	صحيح خانہ;
	صحيح گنتی;
    } IMData;
#جے_انت
};

صنف VimScrollBar: عام BScrollBar
{
    تعريفنوع BScrollBar Inherited;
    عام:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    مجازی فراغ قدر_بدلیا_2(عائم new_قدر);
    مجازی فراغ Mouse_اتے(BPoint where);
    فراغ سیٹ_قدر(عائم newval);
    scrollbar_T *getGsb()
    { أرجع gsb; }

    int32	scroll_Event_گنتی;

    خاص:
    scrollbar_T *gsb;
    عائم   ignore_قدر;
};


#جے_تعریف_شدہ FEAT_TOOLBAR

صنف VimToolbar : عام BBox
{
    ساكن BBitmap *عام_Buttons_Bitmap;
    ساكن BBitmap *grayedButtonsBitmap;

    BBitmap *لدو_Vim_Bitmap(ثابت حرف* fileName);
    منطقي GetPictureFromBitmap(BPicture *picture_منزل, int32 اشاریہ, BBitmap *bitmapFrom, منطقي pressed);
    منطقي Modify_Bitmap_منزل_Grayed(BBitmap *bitmap);

    BList fButtonsList;
    فراغ InvalidateLayout();

    عام:
    VimToolbar(BRect چوکھٹا, ثابت حرف * name);
    ~VimToolbar();

    منطقي PrepareButtonBitmaps();

    منطقي جوڑو_Button(int32 اشاریہ, vimmenu_T *menu);
    منطقي RemoveButton(vimmenu_T *menu);
    منطقي سلیٹی_Button(vimmenu_T *menu, صحيح grey);

    عائم Toolbar_اچائی() ثابت;
    مجازی فراغ Attached_منزل_کھڑکی();
};

BBitmap *VimToolbar::عام_Buttons_Bitmap  = عدم;
BBitmap *VimToolbar::grayedButtonsBitmap  = عدم;

ثابت عائم ToolbarMargin = 3.;
ثابت عائم ButtonMargin  = 3.;

#جے_انت //FEAT_TOOLBAR

#جے_تعریف_شدہ FEAT_GUI_TABLINE

صنف Vim_Tab_سطر : عام BTabView
{
    عام:
	صنف VimTab : عام BTab {
	    عام:
		VimTab() : BTab(جديد BView(BRect(), "-Empty-", 0, 0)) {}

	    مجازی فراغ Select(BView* owner);
	};

	Vim_Tab_سطر(BRect r) : BTabView(r, "vimTabLine", B_چوڑائی_FROM_LABEL,
	       B_FOLLOW_بایاں | B_FOLLOW_چوٹی | B_FOLLOW_دایاں, B_WILL_DRAW | B_چوکھٹا_EVENTS) {}

    عائم Tabline_اچائی() ثابت;
    مجازی فراغ Mouse_تھلے(BPoint نقطہ);
};

#جے_انت //FEAT_GUI_TABLINE


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

    VimFont *اگلا;
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
	View(BRect چوکھٹا);
	~View();

	مجازی فراغ Draw(BRect updateRect);
	فراغ InitIcon(int32 type);

	خاص:
	BBitmap*    fIconBitmap;
    };

    VimDialog(صحيح type, ثابت حرف *title, ثابت حرف *سنیہا,
	    ثابت حرف *buttons, صحيح dfltbutton, ثابت حرف *textfield,
	    صحيح ex_cmd);
    ~VimDialog();

    صحيح Go();

    مجازی فراغ سنیہا_Received(BMessage *پیغام);

    خاص:
    sem_id	    fDialogSem;
    صحيح		    f_Dialog_قدر;
    BList	    fButtonsList;
    BTextView*	    f_سنیہا_View;
    BTextControl*   f_داخلہ_Control;
    ثابت حرف*	    f_داخلہ_قدر;
};

صنف VimSelectFontDialog : عام BWindow
{
    تعريفنوع BWindow Inherited;

    فراغ _CleanList(BListView* list);
    فراغ _UpdateFontStyles();
    فراغ Update_ناپ_داخلہ_Preview();
    فراغ _UpdateFontPreview();
    منطقي Update_From_List_شے(BListView* list, حرف* متن, صحيح متن_ناپ);
    عام:

    VimSelectFontDialog(font_family* family, font_style* style, عائم* ناپ);
    ~VimSelectFontDialog();

    منطقي Go();

    مجازی فراغ سنیہا_Received(BMessage *پیغام);

    خاص:
    حالت_t	    f_حالت;
    sem_id	    fDialogSem;
    منطقي	    f_Dialog_قدر;
    font_family*    fFamily;
    font_style*	    fStyle;
    عائم*	    f_ناپ;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    عائم	    f_Font_ناپ;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_داخلہ;
};

#جے_انت // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

بنية Main_دلیلاں {
    صحيح	     argc;
    حرف    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#تعریف_کرو	کلید_پیغام_BUFSIZ	7
#إذا کلید_پیغام_BUFSIZ < MAX_کلید_کوڈ_LEN
#غلطی Increase کلید_پیغام_BUFSIZ!
#جے_انت

بنية Vim_کلید_پیغام {
    char_u  لمائی;
    char_u  chars[کلید_پیغام_BUFSIZ];  // contains Vim encoding
    منطقي    csi_escape;
};

بنية Vim_Resize_پیغام {
    صحيح	    چوڑائی;
    صحيح	    اچائی;
};

بنية Vim_Scroll_Bar_پیغام {
    VimScrollBar *sb;
    لما    قدر;
    صحيح	    stillDragging;
};

بنية Vim_Menu_پیغام {
    vimmenu_T	*guiMenu;
};

بنية Vim_Mouse_پیغام {
    صحيح	    button;
    صحيح	    x;
    صحيح	    y;
    صحيح	    repeated_click;
    int_u   modifiers;
};

بنية Vim_Mouse_Moved_پیغام {
    صحيح	    x;
    صحيح	    y;
};

بنية Vim_Focus_پیغام {
    منطقي    سرگرم;
};

بنية Vim_Refs_پیغام {
    BMessage   *سنیہا;
    منطقي    changedir;
};

بنية Vim_Tabline_پیغام {
    صحيح	    اشاریہ;
};

بنية Vim_Tabline_Menu_پیغام {
    صحيح	    اشاریہ;
    صحيح	    event;
};

بنية Vim_پیغام {
    تعداد Vim_پیغام_Type {
	کلید_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    اتحاد {
	بنية Vim_کلید_پیغام    کلید_2;
	بنية Vim_Resize_پیغام New_ناپ;
	بنية Vim_Scroll_Bar_پیغام	Scroll;
	بنية Vim_Menu_پیغام   Menu;
	بنية Vim_Mouse_پیغام  Mouse;
	بنية Vim_Mouse_Moved_پیغام	MouseMoved;
	بنية Vim_Focus_پیغام  Focus;
	بنية Vim_Refs_پیغام   Refs;
	بنية Vim_Tabline_پیغام	Tabline;
	بنية Vim_Tabline_Menu_پیغام    TablineMenu;
    } u;
};

#تعریف_کرو RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#تعریف_کرو GUI_منزل_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

ساكن بنية specialkey
{
    uint16  Be_کلیدیں;
#تعریف_کرو کلید_3(a,b)    ((a)<<8|(b))
#تعریف_کرو K(a)	    کلید_3(0,a)		// for ASCII codes
#تعریف_کرو F(b)	    کلید_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیدیں[] =
{
    {K(B_اتے_ARROW),	'k', 'u'},
    {K(B_تھلے_ARROW),	    'k', 'd'},
    {K(B_بایاں_ARROW),	    'k', 'l'},
    {K(B_دایاں_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_داخل_کرو),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_سرا),		'@', '7'},
    {K(B_ورقہ_اتے),	'k', 'P'},	// XK_Prior
    {K(B_ورقہ_تھلے),	    'k', 'N'},	    // XK_Next,

#تعریف_کرو پہلا_FUNCTION_کلید  11
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
#جے_انت

    // End of list marker:
    {0,		    0, 0}
};

#تعریف_کرو عدد_SPECIAL_کلیدیں    ARRAY_لمائی(special_کلیدیں)

// ---------------- VimApp ----------------

    ساكن فراغ
docd(BPath &رستہ)
{
    mch_chdir((حرف *)رستہ.رستہ_2());
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
    int32 گنتی;

    m->Print_منزل_دھارا();
    بدل (m->what) {
	حالة B_REFS_RECEIVED:
	حالة B_SIMPLE_انکڑے:
	    m->GetInfo("refs", &type, &گنتی);
	    إذا (type != B_REF_TYPE)
		اذهب bad;
	    اكسر;
	حالة B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &گنتی);
	    إذا (type != B_STRING_TYPE)
		اذهب bad;
	    إذا (changedir) {
		حرف *dirname;
		إذا (m->لبھو_String("cwd", (ثابت حرف **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    اكسر;
	افتراضي:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    مٹاؤ m;
	    أرجع;
    }

#جے_تعریف_شدہ FEAT_VISUAL
    reset_VIsual();
#جے_انت

    char_u  **fnames;
    fnames = (char_u **) alloc(گنتی * حجم(char_u *));
    صحيح fname_اشاریہ = 0;

    بدل (m->what) {
	حالة B_REFS_RECEIVED:
	حالة B_SIMPLE_انکڑے:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    لكل (صحيح i = 0; i < گنتی; ++i)
	    {
		اندراج_ref ref;
		إذا (m->لبھو_Ref("refs", i, &ref) == B_OK) {
		    BEntry اندراج(&ref, خطأ);
		    BPath رستہ;
		    اندراج.Get_رستہ(&رستہ);

		    // Change to parent directory?
		    إذا (changedir) {
			BPath parentpath;
			رستہ.Get_ماپا(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    إذا (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			إذا (!changedir)
			    docd(رستہ);
		    } وإلا {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)رستہ.رستہ_2(), IObuff);
			إذا (fname == عدم)
			    fname = (char_u *)رستہ.رستہ_2();
			fnames[fname_اشاریہ++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = خطأ;
		}
	    }
	    اكسر;
	حالة B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    لكل (صحيح i = 1; i < گنتی; ++i)
	    {
		حرف *fname;

		إذا (m->لبھو_String("argv", i, (ثابت حرف **) &fname) == B_OK) {
		    fnames[fname_اشاریہ++] = vim_strsave((char_u *)fname);
		}
	    }
	    اكسر;
	افتراضي:
	    // fprintf(stderr, "case default\n");
	    اكسر;
    }

    مٹاؤ m;

    // Handle the drop, :edit to get to the file
    إذا (fname_اشاریہ > 0) {
	handle_drop(fname_اشاریہ, fnames, FALSE, drop_callback, عدم);

	setcursor();
	باہرلا_flush();
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
VimApp::Ready_منزل_لگاتار()
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
	إذا (gui.vim_کھڑکی)
	    gui.vim_کھڑکی->Minimize(خطأ);
	BMessage *m = Current_سنیہا();
	Detach_Current_سنیہا();
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
    صحيح حد_بندی = 15;
    طالما (--حد_بندی >= 0 && (curbuf == عدم || curbuf->b_شروع_ffc == 0))
	snooze(100000);    //  0.1 s
    إذا (gui.vim_کھڑکی)
	gui.vim_کھڑکی->Minimize(خطأ);
    Detach_Current_سنیہا();
    SendRefs(m, سچ);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    فراغ
VimApp::SendRefs(BMessage *m, منطقي changedir)
{
    Vim_Refs_پیغام rm;
    rm.سنیہا = m;
    rm.changedir = changedir;

    write_پورٹ(gui.vdcmp, Vim_پیغام::Refs, &rm, حجم(rm));
    //	calls ::RefsReceived
}

    فراغ
VimApp::سنیہا_Received(BMessage *m)
{
    بدل (m->what) {
	حالة 'save':
	    {
		اندراج_ref refDirectory;
		m->لبھو_Ref("directory", &refDirectory);
		f_Browsed_رستہ.سیٹ_منزل(&refDirectory);
		BString strName;
		m->لبھو_String("name", &strName);
		f_Browsed_رستہ.Append(strName.String());
	    }
	    اكسر;
	حالة 'open':
	    {
		اندراج_ref ref;
		m->لبھو_Ref("refs", &ref);
		f_Browsed_رستہ.سیٹ_منزل(&ref);
	    }
	    اكسر;
	حالة B_CANCEL:
	    {
		BFilePanel *panel;
		m->لبھو_اشارہ_گر("source", (فراغ**)&panel);
		إذا (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    اكسر;
	افتراضي:
	    Inherited::سنیہا_Received(m);
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

Vim_کھڑکی::Vim_کھڑکی():
    BWindow(BRect(40, 40, 150, 150),
	    "Vim",
	    B_TITLED_کھڑکی,
	    0,
	    B_CURRENT_WORKSPACE)

{
    init();
}

Vim_کھڑکی::~Vim_کھڑکی()
{
    إذا (formView) {
	Remove_بچہ(formView);
	مٹاؤ formView;
    }
    gui.vim_کھڑکی = عدم;
}

    فراغ
Vim_کھڑکی::init()
{
    // Attach the VimFormView
    formView = جديد VimFormView(Bounds());
    إذا (formView != عدم) {
	جوڑو_بچہ(formView);
    }
}

#إذا 0  //  disabled in zeta patch
    فراغ
Vim_کھڑکی::Dispatch_سنیہا(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    بدل (m->what) {
	حالة B_MOUSE_اتے:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		إذا (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->لبھو_نقطہ("where", &where);
		    v->Mouse_اتے(where);
		} وإلا {
		    Inherited::Dispatch_سنیہا(m, h);
		}
	    }
	    اكسر;
	افتراضي:
	    Inherited::Dispatch_سنیہا(m, h);
    }
}
#جے_انت

    فراغ
Vim_کھڑکی::کھڑکی_Activated(منطقي سرگرم)
{
    Inherited::کھڑکی_Activated(سرگرم);
    // the textArea gets the keyboard action
    إذا (سرگرم && gui.vim_متن_Area)
	gui.vim_متن_Area->MakeFocus(سچ);

    بنية Vim_Focus_پیغام fm;
    fm.سرگرم = سرگرم;

    write_پورٹ(gui.vdcmp, Vim_پیغام::Focus, &fm, حجم(fm));
}

    منطقي
Vim_کھڑکی::QuitRequested()
{
    بنية Vim_کلید_پیغام km;
    km.لمائی = 5;
    حافظہ_نقل((حرف *)km.chars, "\033:qa\r", km.لمائی);
    km.csi_escape = خطأ;
    write_پورٹ(gui.vdcmp, Vim_پیغام::کلید_2, &km, حجم(km));
    أرجع خطأ;
}
    فراغ
Vim_کھڑکی::Dispatch_سنیہا(BMessage *m, BHandler *h)
{
    منطقي should_propagate = سچ;

    بدل (m->what)
    {
	حالة B_کلید_تھلے:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->لبھو_Int32("raw_char", &scancode);
		m->لبھو_Int32("modifiers", &beModifiers);

		إذا (scancode == B_ENTER && (beModifiers & B_بایاں_COMMAND_کلید))
		    {
			should_propagate = خطأ;
			إذا (هذا->is_fullscreen)
			    {
				هذا->is_fullscreen = خطأ;
				Resize_منزل(هذا->saved_چوکھٹا.چوڑائی_2(), هذا->saved_چوکھٹا.اچائی_2());
				Move_منزل(هذا->saved_چوکھٹا.کھبے, هذا->saved_چوکھٹا.چوٹی);
				سیٹ_Look(هذا->saved_look);
				سیٹ_Flags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } وإلا {
				هذا->saved_چوکھٹا = چوکھٹا_2();
				هذا->saved_look = Look();
				هذا->is_fullscreen = سچ;
				BScreen s(هذا);
				سیٹ_Look(B_NO_BORDER_کھڑکی_LOOK);
				Resize_منزل(s.چوکھٹا_2().چوڑائی_2() + 1, s.چوکھٹا_2().اچائی_2() + 1);
				Move_منزل(s.چوکھٹا_2().کھبے, s.چوکھٹا_2().چوٹی);
				سیٹ_Flags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    إذا (should_propagate)
	Inherited::Dispatch_سنیہا(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect چوکھٹا):
    BView(چوکھٹا, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_چوکھٹا_EVENTS),
    menuBar(عدم),
#جے_تعریف_شدہ FEAT_TOOLBAR
    toolBar(عدم),
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_سطر(عدم),
#جے_انت
    متن_Area(عدم)
{
    init(چوکھٹا);
}

VimFormView::~VimFormView()
{
    إذا (menuBar) {
	Remove_بچہ(menuBar);
#جے_تعریف_شدہ never
	//  deleting the menuBar leads to SEGV on exit
	//  richard@whitequeen.com Jul 99
	مٹاؤ menuBar;
#جے_انت
    }

#جے_تعریف_شدہ FEAT_TOOLBAR
    مٹاؤ toolBar;
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    مٹاؤ tab_سطر;
#جے_انت

    إذا (متن_Area) {
	Remove_بچہ(متن_Area);
	مٹاؤ متن_Area;
    }
    gui.vimForm = عدم;
}

    فراغ
VimFormView::init(BRect چوکھٹا)
{
    menuBar = جديد BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    جوڑو_بچہ(menuBar);

#جے_تعریف_شدہ FEAT_TOOLBAR
    toolBar = جديد VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    جوڑو_بچہ(toolBar);
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    tab_سطر = جديد Vim_Tab_سطر(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    جوڑو_بچہ(tab_سطر);
#جے_انت

    BRect بچیا = چوکھٹا;
    متن_Area = جديد Vim_متن_Area_View(بچیا);
    جوڑو_بچہ(متن_Area);
    // The textArea will be resized later when menus are added

    gui.vimForm = هذا;
}

#جے_تعریف_شدہ FEAT_TOOLBAR
    عائم
VimFormView::Toolbar_اچائی() ثابت
{
    أرجع toolBar ? toolBar->Toolbar_اچائی() : 0.;
}
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    عائم
VimFormView::Tabline_اچائی() ثابت
{
    أرجع (tab_سطر && Is_Showing_Tab_سطر()) ? tab_سطر->Tabline_اچائی() : 0.;
}
#جے_انت

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
	BRect بچیا = Bounds();

#جے_تعریف_شدہ FEAT_MENU
	بچیا.چوٹی += Menu_اچائی();
	menuBar->Resize_منزل(بچیا.سجے, بچیا.چوٹی);
	gui.menu_اچائی = (صحيح) Menu_اچائی();
#جے_انت

#جے_تعریف_شدہ FEAT_TOOLBAR
	toolBar->Move_منزل(بچیا.کھبے, بچیا.چوٹی);
	toolBar->Resize_منزل(بچیا.سجے, Toolbar_اچائی());
	بچیا.چوٹی += Toolbar_اچائی();
	gui.toolbar_اچائی = Toolbar_اچائی();
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_TABLINE
	tab_سطر->Move_منزل(بچیا.کھبے, بچیا.چوٹی);
	tab_سطر->Resize_منزل(بچیا.سجے + 1, Tabline_اچائی());
	بچیا.چوٹی += Tabline_اچائی();
	gui.tabline_اچائی = Tabline_اچائی();
#جے_انت

	متن_Area->Resize_منزل(بچیا.چوڑائی_2(), بچیا.اچائی_2());
	متن_Area->Move_منزل(بچیا.کھبے, بچیا.چوٹی);
    }


    Inherited::AllAttached();
}

    فراغ
VimFormView::چوکھٹا_Resized(عائم new_چوڑائی, عائم new_اچائی)
{
    بنية Vim_Resize_پیغام sm;
    صحيح adjust_h, adjust_w;

    new_چوڑائی += 1;	//  adjust from width to number of pixels occupied
    new_اچائی += 1;

    sm.چوڑائی = (صحيح) new_چوڑائی;
    sm.اچائی = (صحيح) new_اچائی;
    adjust_w = ((صحيح)new_چوڑائی - gui_get_ادھار_چوڑائی()) % gui.char_چوڑائی;
    adjust_h = ((صحيح)new_اچائی - gui_get_ادھار_اچائی()) % gui.char_اچائی;

    إذا (adjust_w > 0 || adjust_h > 0) {
	sm.چوڑائی  -= adjust_w;
	sm.اچائی -= adjust_h;
    }

    write_پورٹ(gui.vdcmp, Vim_پیغام::Resize, &sm, حجم(sm));
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

Vim_متن_Area_View::Vim_متن_Area_View(BRect چوکھٹا):
    BView(چوکھٹا, "VimTextAreaView", B_FOLLOW_ALL_SIDES,
#جے_تعریف_شدہ FEAT_MBYTE_IME
	B_WILL_DRAW | B_بھریا_UPDATE_ON_RESIZE | B_داخلہ_طریقہ_AWARE
#وإلا
	B_WILL_DRAW | B_بھریا_UPDATE_ON_RESIZE
#جے_انت
	),
    mouse_Drag_Event_گنتی(0)
{
#جے_تعریف_شدہ FEAT_MBYTE_IME
    IMData.messenger = عدم;
    IMData.سنیہا = عدم;
#جے_انت
    init(چوکھٹا);
}

Vim_متن_Area_View::~Vim_متن_Area_View()
{
    gui.vim_متن_Area = عدم;
}

    فراغ
Vim_متن_Area_View::init(BRect چوکھٹا)
{
    // set up global var for fast access
    gui.vim_متن_Area = هذا;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    سیٹ_View_Color(B_TRANSPARENT_32_ثنائی_ہندسہ);
#تعریف_کرو PEN_چوڑائی   1
    سیٹ_Pen_ناپ(PEN_چوڑائی);
#تعریف_کرو W_چوڑائی(curwin)   0
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
    rgb_color rgb = GUI_منزل_RGB(gui.پٹھ_pixel);
    سیٹ_کم_Color(rgb);
    FillRect(updateRect, B_SOLID_کم);
    gui_redraw((صحيح) updateRect.کھبے, (صحيح) updateRect.چوٹی,
	    (صحيح) (updateRect.چوڑائی_2() + PEN_چوڑائی), (صحيح) (updateRect.اچائی_2() + PEN_چوڑائی));

    // Clear the border areas if needed
    سیٹ_کم_Color(rgb);

    إذا (updateRect.کھبے < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.کھبے, updateRect.چوٹی,
		    FILL_X(0)-PEN_چوڑائی, updateRect.bottom), B_SOLID_کم);
    إذا (updateRect.چوٹی < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.کھبے, updateRect.چوٹی,
		    updateRect.سجے, FILL_Y(0)-PEN_چوڑائی), B_SOLID_کم);
    إذا (updateRect.سجے >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((صحيح)Columns), updateRect.چوٹی,
		    updateRect.سجے, updateRect.bottom), B_SOLID_کم);
    إذا (updateRect.bottom >= FILL_Y(صفاں))   //  bottom border
	FillRect(BRect(updateRect.کھبے, FILL_Y((صحيح)صفاں),
		    updateRect.سجے, updateRect.bottom), B_SOLID_کم);

#جے_تعریف_شدہ FEAT_MBYTE_IME
    DrawIMString();
#جے_انت
}

    فراغ
Vim_متن_Area_View::کلید_تھلے(ثابت حرف *بائٹاں, int32 عدد_بائٹاں_2)
{
    بنية Vim_کلید_پیغام km;
    char_u *dest = km.chars;

    منطقي canHaveVimModifiers = خطأ;

    BMessage *پیغام = کھڑکی()->Current_سنیہا();
    أكد(پیغام);
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
    إذا (عدد_بائٹاں_2 > 1) {
	// This cannot be a special key
	إذا (عدد_بائٹاں_2 > کلید_پیغام_BUFSIZ)
	    عدد_بائٹاں_2 = کلید_پیغام_BUFSIZ;	    //	should never happen... ???
	km.لمائی = عدد_بائٹاں_2;
	حافظہ_نقل((حرف *)dest, بائٹاں, عدد_بائٹاں_2);
	km.csi_escape = سچ;
    } وإلا {
	int32 scancode = 0;
	پیغام->لبھو_Int32("key", &scancode);

	int32 beModifiers = 0;
	پیغام->لبھو_Int32("modifiers", &beModifiers);

	char_u string[3];
	صحيح len = 0;
	km.لمائی = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	أكد(B_BACKSPACE <= 0x20);
	أكد(B_DELETE == 0x7F);
	إذا (((char_u)بائٹاں[0] <= 0x20 || (char_u)بائٹاں[0] == 0x7F) &&
		عدد_بائٹاں_2 == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    إذا (beModifiers & B_CONTROL_کلید) {
		صحيح اشاریہ = کلید_Map->عام_map[scancode];
		صحيح new_عدد_بائٹاں = کلید_Map_Chars[اشاریہ];
		char_u *new_بائٹاں = (char_u *)&کلید_Map_Chars[اشاریہ + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		إذا (new_عدد_بائٹاں != 1 || (*new_بائٹاں > 0x20 &&
			    *new_بائٹاں != 0x7F )) {
		    اذهب notspecial;
		}
		بائٹاں = (حرف *)new_بائٹاں;
	    }
	    canHaveVimModifiers = سچ;

	    uint16 beoskey;
	    صحيح پہلا, آخری;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    إذا (عدد_بائٹاں_2 == 0 || بائٹاں[0] == B_FUNCTION_کلید) {
		beoskey = F(scancode);
		پہلا = پہلا_FUNCTION_کلید;
		آخری = عدد_SPECIAL_کلیدیں;
	    } وإلا إذا (*بائٹاں == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		پہلا = آخری = 0;
	    } وإلا {
		beoskey = K(بائٹاں[0]);
		پہلا = 0;
		آخری = پہلا_FUNCTION_کلید;
	    }

	    لكل (صحيح i = پہلا; i < آخری; i++) {
		إذا (special_کلیدیں[i].Be_کلیدیں == beoskey) {
		    string[0] = CSI;
		    string[1] = special_کلیدیں[i].vim_code0;
		    string[2] = special_کلیدیں[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	إذا (len == 0) {
	    string[0] = بائٹاں[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#إذا 0
	إذا (len == 3 ||
		بائٹاں[0] == B_SPACE || بائٹاں[0] == B_TAB ||
		بائٹاں[0] == B_RETURN || بائٹاں[0] == '\r' ||
		بائٹاں[0] == B_ESCAPE)
#وإلا
	    إذا (canHaveVimModifiers)
#جے_انت
	    {
		صحيح modifiers;
		modifiers = 0;
		إذا (beModifiers & B_کھسکاؤ_کلید)
		    modifiers |= MOD_نقاب_کھسکاؤ;
		إذا (beModifiers & B_CONTROL_کلید)
		    modifiers |= MOD_نقاب_CTRL;
		إذا (beModifiers & B_OPTION_کلید)
		    modifiers |= MOD_نقاب_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		صحيح کلید;
		إذا (string[0] == CSI && len == 3)
		    کلید = منزل_SPECIAL(string[1], string[2]);
		وإلا
		    کلید = string[0];
		کلید = simplify_کلید(کلید, &modifiers);
		إذا (IS_SPECIAL(کلید))
		{
		    string[0] = CSI;
		    string[1] = K_دوجا(کلید);
		    string[2] = K_تیجا(کلید);
		    len = 3;
		}
		وإلا
		{
		    string[0] = کلید;
		    len = 1;
		}

		إذا (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.لمائی = 3;
		}
	    }
	حافظہ_نقل((حرف *)dest, string, len);
	km.لمائی += len;
	km.csi_escape = خطأ;
    }

    write_پورٹ(gui.vdcmp, Vim_پیغام::کلید_2, &km, حجم(km));

    /*
     * blank out the pointer if necessary
     */
    إذا (p_mh && !gui.اشارہ_گر_hidden)
    {
	guiBlankMouse(سچ);
	gui.اشارہ_گر_hidden = TRUE;
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
    Vim_Mouse_پیغام mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_پورٹ(gui.vdcmp, Vim_پیغام::Mouse, &mm, حجم(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    إذا (gui.اشارہ_گر_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشارہ_گر_hidden = FALSE;
    }
}

فراغ
Vim_متن_Area_View::guiMouseMoved(
	صحيح	x,
	صحيح	y)
{
    Vim_Mouse_Moved_پیغام mm;

    mm.x = x;
    mm.y = y;

    write_پورٹ(gui.vdcmp, Vim_پیغام::MouseMoved, &mm, حجم(mm));

    إذا (gui.اشارہ_گر_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشارہ_گر_hidden = FALSE;
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

    إذا (beModifiers & B_کھسکاؤ_کلید)
	vim_modifiers |= MOUSE_کھسکاؤ;
    إذا (beModifiers & B_CONTROL_کلید)
	vim_modifiers |= MOUSE_CTRL;
    إذا (beModifiers & B_OPTION_کلید)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    أرجع vim_modifiers;
}

    فراغ
Vim_متن_Area_View::Mouse_تھلے(BPoint نقطہ)
{
    BMessage *m = کھڑکی()->Current_سنیہا();
    أكد(m);

    int32 buttons = 0;
    m->لبھو_Int32("buttons", &buttons);

    صحيح vimButton;

    إذا (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_بایاں;
    وإلا إذا (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_دایاں;
    وإلا إذا (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_درمیان;
    وإلا
	أرجع;		// Unknown button

    vimMouseButton = 1;	    // don't care which one

    // Handle multiple clicks
    int32 clicks = 0;
    m->لبھو_Int32("clicks", &clicks);

    int32 modifiers = 0;
    m->لبھو_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_منزل_Vim(modifiers);

    guiSendMouseEvent(vimButton, نقطہ.x, نقطہ.y,
	    clicks > 1 /* = repeated_click*/, vimMouseModifiers);
}

    فراغ
Vim_متن_Area_View::Mouse_اتے(BPoint نقطہ)
{
    vimMouseButton = 0;

    BMessage *m = کھڑکی()->Current_سنیہا();
    أكد(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->لبھو_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_منزل_Vim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, نقطہ.x, نقطہ.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::Mouse_اتے(نقطہ);
}

    فراغ
Vim_متن_Area_View::MouseMoved(BPoint نقطہ, uint32 transit, ثابت BMessage *سنیہا)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    إذا (gui.اشارہ_گر_hidden)
    {
	guiBlankMouse(خطأ);
	gui.اشارہ_گر_hidden = FALSE;
    }

    إذا (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(نقطہ.x, نقطہ.y);
	أرجع;
    }

    atomic_جوڑو(&mouse_Drag_Event_گنتی, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, نقطہ.x, نقطہ.y, 0, vimMouseModifiers);
}

    فراغ
Vim_متن_Area_View::سنیہا_Received(BMessage *m)
{
    بدل (m->what) {
	حالة 'menu':
	    {
		Vim_Menu_پیغام mm;
		mm.guiMenu = عدم;  // in case no pointer in msg
		m->لبھو_اشارہ_گر("VimMenu", (فراغ **)&mm.guiMenu);
		write_پورٹ(gui.vdcmp, Vim_پیغام::Menu, &mm, حجم(mm));
	    }
	    اكسر;
	حالة B_MOUSE_WHEEL_بدلیا:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		عائم small=0, big=0, dy=0;
		m->لبھو_Float("be:wheel_delta_y", &dy);
		scb->Get_قدماں(&small, &big);
		scb->سیٹ_قدر(scb->قدر_2()+small*dy*3);
		scb->قدر_بدلیا_2(scb->قدر_2());
#إذا 0
		scb = curwin->w_scrollbars[0].id;
		scb->Get_قدماں(&small, &big);
		scb->سیٹ_قدر(scb->قدر_2()+small*dy);
		scb->قدر_بدلیا_2(scb->قدر_2());
#جے_انت
	    }
	    اكسر;
#جے_تعریف_شدہ FEAT_MBYTE_IME
	حالة B_داخلہ_طریقہ_EVENT:
	    {
		int32 opcode;
		m->لبھو_Int32("be:opcode", &opcode);
		بدل(opcode)
		{
		    حالة B_داخلہ_طریقہ_STARTED:
			إذا (!IMData.messenger) مٹاؤ IMData.messenger;
			IMData.messenger = جديد BMessenger();
			m->لبھو_Messenger("be:reply_to", IMData.messenger);
			اكسر;
		    حالة B_داخلہ_طریقہ_بدلیا:
			{
			    BString str;
			    منطقي confirmed;
			    إذا (IMData.سنیہا) *(IMData.سنیہا) = *m;
			    وإلا	       IMData.سنیہا = جديد BMessage(*m);
			    DrawIMString();
			    m->لبھو_Bool("be:confirmed", &confirmed);
			    إذا (confirmed)
			    {
				m->لبھو_String("be:string", &str);
				char_u *chars = (char_u*)str.String();
				بنية Vim_کلید_پیغام km;
				km.csi_escape = سچ;
				صحيح clen;
				صحيح i = 0;
				طالما (i < str.لمائی_2())
				{
				    clen = utf_ptr2len(chars+i);
				    حافظہ_نقل(km.chars, chars+i, clen);
				    km.لمائی = clen;
				    write_پورٹ(gui.vdcmp, Vim_پیغام::کلید_2, &km, حجم(km));
				    i += clen;
				}
			    }
			}
			اكسر;
		    حالة B_داخلہ_طریقہ_LOCATION_REQUEST:
			{
			    BMessage پیغام(B_داخلہ_طریقہ_EVENT);
			    پیغام.جوڑو_Int32("be:opcode", B_داخلہ_طریقہ_LOCATION_REQUEST);
			    پیغام.جوڑو_نقطہ("be:location_reply", IMData.location);
			    پیغام.جوڑو_Float("be:height_reply", FILL_Y(1));
			    IMData.messenger->Send_سنیہا(&پیغام);
			}
			اكسر;
		    حالة B_داخلہ_طریقہ_STOPPED:
			مٹاؤ IMData.messenger;
			مٹاؤ IMData.سنیہا;
			IMData.messenger = عدم;
			IMData.سنیہا = عدم;
			اكسر;
		}
	    }
	    // TODO: sz: break here???
#جے_انت
	افتراضي:
	    إذا (m->WasDropped()) {
		BWindow *w = کھڑکی();
		w->Detach_Current_سنیہا();
		w->Minimize(خطأ);
		VimApp::SendRefs(m, (modifiers() & B_کھسکاؤ_کلید) != 0);
	    } وإلا {
		Inherited::سنیہا_Received(m);
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
	gui_mch_سیٹ_font((GuiFont)newFont);
	إذا (name && STRCMP(name, "*") != 0)
	    hl_سیٹ_font_name(name);

	سیٹ_Drawing_منوال(B_OP_COPY);

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
Vim_متن_Area_View::mchDrawString(صحيح صف, صحيح خانہ, char_u *s, صحيح len, صحيح flags)
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

	BRect r(FILL_X(خانہ), FILL_Y(صف),
		FILL_X(خانہ + cells) - PEN_چوڑائی, FILL_Y(صف + 1) - PEN_چوڑائی);
	FillRect(r, B_SOLID_کم);
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
	    where.سیٹ(متن_X(خانہ+lastpos), متن_Y(صف));
	    DrawString(p, clen, where);
	    إذا (flags & DRAW_BOLD) {
		where.x += 1.0;
		سیٹ_Drawing_منوال(B_OP_BLEND);
		DrawString(p, clen, where);
		سیٹ_Drawing_منوال(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    إذا (cells<4) lastpos += cells;
	    وإلا	lastpos++;
	    p += clen;
	}
    }
    وإلا
    {
	BPoint where(متن_X(خانہ), متن_Y(صف));
	DrawString((حرف*)s, len, where);
	إذا (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    سیٹ_Drawing_منوال(B_OP_BLEND);
	    DrawString((حرف*)s, len, where);
	    سیٹ_Drawing_منوال(B_OP_COPY);
	}
    }

    إذا (flags & DRAW_UNDERL) {
	صحيح cells;
	cells = 0;
	لكل (صحيح i=0; i<len; i++) {
	    صحيح cn = utf_ptr2cells((char_u *)(s+i));
	    إذا (cn<4) cells += cn;
	}

	BPoint شروع(FILL_X(خانہ), FILL_Y(صف + 1) - PEN_چوڑائی);
	BPoint سرا(FILL_X(خانہ + cells) - PEN_چوڑائی, شروع.y);

	Stroke_سطر(شروع, سرا);
    }
}

فراغ
Vim_متن_Area_View::mch_Clear_بلاک(
	صحيح	row1,
	صحيح	col1,
	صحيح	row2,
	صحيح	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_چوڑائی, FILL_Y(row2 + 1) - PEN_چوڑائی);
    gui_mch_سیٹ_bg_color(gui.پٹھ_pixel);
    FillRect(r, B_SOLID_کم);
}

    فراغ
Vim_متن_Area_View::mchClearAll()
{
    gui_mch_سیٹ_bg_color(gui.پٹھ_pixel);
    FillRect(Bounds(), B_SOLID_کم);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    فراغ
Vim_متن_Area_View::mch_Delete_سطریں(صحيح صف, صحيح عدد_سطریں)
{
    BRect سروت, dest;
    سروت.کھبے = FILL_X(gui.scroll_region_بایاں);
    سروت.چوٹی = FILL_Y(صف + عدد_سطریں);
    سروت.سجے = FILL_X(gui.scroll_region_دایاں + 1) - PEN_چوڑائی;
    سروت.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_چوڑائی;

    dest.کھبے = FILL_X(gui.scroll_region_بایاں);
    dest.چوٹی = FILL_Y(صف);
    dest.سجے = FILL_X(gui.scroll_region_دایاں + 1) - PEN_چوڑائی;
    dest.bottom = FILL_Y(gui.scroll_region_bot - عدد_سطریں + 1) - PEN_چوڑائی;

    إذا (gui.vim_کھڑکی->جندرا_2()) {
	// Clear one column more for when bold has spilled over
	Copy_بٹ(سروت, dest);
	gui_clear_بلاک(gui.scroll_region_bot - عدد_سطریں + 1,
		gui.scroll_region_بایاں,
		gui.scroll_region_bot, gui.scroll_region_دایاں);


	gui.vim_کھڑکی->Unlock();
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
Vim_متن_Area_View::mch_داخل_کرو_سطریں(صحيح صف, صحيح عدد_سطریں)
{
    BRect سروت, dest;

    // XXX Attempt at a hack:
    gui.vim_کھڑکی->UpdateIfNeeded();
    سروت.کھبے = FILL_X(gui.scroll_region_بایاں);
    سروت.چوٹی = FILL_Y(صف);
    سروت.سجے = FILL_X(gui.scroll_region_دایاں + 1) - PEN_چوڑائی;
    سروت.bottom = FILL_Y(gui.scroll_region_bot - عدد_سطریں + 1) - PEN_چوڑائی;

    dest.کھبے = FILL_X(gui.scroll_region_بایاں);
    dest.چوٹی = FILL_Y(صف + عدد_سطریں);
    dest.سجے = FILL_X(gui.scroll_region_دایاں + 1) - PEN_چوڑائی;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_چوڑائی;

    إذا (gui.vim_کھڑکی->جندرا_2()) {
	// Clear one column more for when bold has spilled over
	Copy_بٹ(سروت, dest);
	gui_clear_بلاک(صف, gui.scroll_region_بایاں,
		صف + عدد_سطریں - 1, gui.scroll_region_دایاں);

	gui.vim_کھڑکی->Unlock();
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

#جے_تعریف_شدہ FEAT_MBYTE_IME
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
    BMessage* پیغام = IMData.سنیہا;
    إذا (!پیغام)
	أرجع;
    gui_redraw_بلاک(IMData.صف, 0,
	    IMData.صف + IMData.گنتی, W_چوڑائی(curwin), GUI_MON_NOCLEAR);
    منطقي confirmed = خطأ;
    پیغام->لبھو_Bool("be:confirmed", &confirmed);
    إذا (confirmed)
	أرجع;
    rgb_color hcolor = بلند_Color(), lcolor = کم_Color();
    پیغام->لبھو_String("be:string", &str);
    s = str.String();
    len = str.لمائی_2();
    سیٹ_بلند_Color(0, 0, 0);
    IMData.صف = gui.صف;
    IMData.خانہ = gui.خانہ;
    int32 sel_شروع = 0, sel_سرا = 0;
    پیغام->لبھو_Int32("be:selection", 0, &sel_شروع);
    پیغام->لبھو_Int32("be:selection", 1, &sel_سرا);
    صحيح clen, cn;
    BPoint pos(IMData.خانہ, 0);
    BRect r;
    BPoint where;
    IMData.location = Convert_منزل_Screen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.صف + pos.y)));
    لكل (صحيح i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	إذا (pos.x + cn > W_چوڑائی(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	إذا (sel_شروع<=i && i<sel_سرا)
	{
	    سیٹ_کم_Color(r_highlight);
	    IMData.location = Convert_منزل_Screen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.صف + pos.y)));
	}
	وإلا
	{
	    سیٹ_کم_Color(b_highlight);
	}
	r.سیٹ(FILL_X(pos.x), FILL_Y(IMData.صف + pos.y),
		FILL_X(pos.x + cn) - PEN_چوڑائی,
		FILL_Y(IMData.صف + pos.y + 1) - PEN_چوڑائی);
	FillRect(r, B_SOLID_کم);
	where.سیٹ(متن_X(pos.x), متن_Y(IMData.صف + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.گنتی = (صحيح)pos.y;

    سیٹ_بلند_Color(hcolor);
    سیٹ_کم_Color(lcolor);
}
#جے_انت
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
    scroll_Event_گنتی(0)
{
    gsb = g;
    سیٹ_Resizing_منوال(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    فراغ
VimScrollBar::قدر_بدلیا_2(عائم new_قدر)
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
    atomic_جوڑو(&scroll_Event_گنتی, 1);

    بنية Vim_Scroll_Bar_پیغام sm;

    sm.sb = هذا;
    sm.قدر = (لما) new_قدر;
    sm.stillDragging = TRUE;

    write_پورٹ(gui.vdcmp, Vim_پیغام::ScrollBar, &sm, حجم(sm));

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
VimScrollBar::Mouse_اتے(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_جوڑو(&scroll_Event_گنتی, 1);

    بنية Vim_Scroll_Bar_پیغام sm;

    sm.sb = هذا;
    sm.قدر = (لما) قدر_2();
    sm.stillDragging = FALSE;

    write_پورٹ(gui.vdcmp, Vim_پیغام::ScrollBar, &sm, حجم(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::Mouse_اتے(where);
}

    فراغ
VimScrollBar::سیٹ_قدر(عائم new_قدر)
{
    إذا (new_قدر == قدر_2())
	أرجع;

    ignore_قدر = new_قدر;
    Inherited::سیٹ_قدر(new_قدر);
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
    اگلا = عدم;
    refcount = 1;
    name = عدم;
}

// ---------------- VimDialog ----------------

#إذا defined(FEAT_GUI_DIALOG)

ثابت بنانشانی صحيح  k_Vim_Dialog_Button_پیغام = 'VMDB';
ثابت بنانشانی صحيح  k_Vim_Dialog_Icon_Stripe_چوڑائی = 30;
ثابت بنانشانی صحيح  kVimDialogButtonsSpacingX = 9;
ثابت بنانشانی صحيح  kVimDialogButtonsSpacingY = 4;
ثابت بنانشانی صحيح  kVimDialogSpacingX = 6;
ثابت بنانشانی صحيح  kVimDialogSpacingY = 10;
ثابت بنانشانی صحيح  k_Vim_Dialog_اقل_چوڑائی  = 310;
ثابت بنانشانی صحيح  k_Vim_Dialog_اقل_اچائی = 75;
ثابت BRect	    kDefaultRect =
BRect(0, 0, k_Vim_Dialog_اقل_چوڑائی, k_Vim_Dialog_اقل_اچائی);

VimDialog::VimDialog(صحيح type, ثابت حرف *title, ثابت حرف *سنیہا,
	ثابت حرف *buttons, صحيح dfltbutton, ثابت حرف *textfield, صحيح ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_کھڑکی_LOOK, B_MODAL_APP_کھڑکی_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_قدر(dfltbutton)
    , f_سنیہا_View(عدم)
    , f_داخلہ_Control(عدم)
    , f_داخلہ_قدر(textfield)
{
    //	master view
    VimDialog::View* view = جديد VimDialog::View(Bounds());
    إذا (view == عدم)
	أرجع;

    إذا (title == عدم)
	سیٹ_Title("Vim " VIM_نسخہ_MEDIUM);

    جوڑو_بچہ(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    عائم max_Button_چوڑائی  = 0;
    عائم max_Button_اچائی = 0;
    عائم buttons_چوڑائی	  = 0;
    عائم buttons_اچائی   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    افعل
    {
	int32 سرا = strButtons.لبھو_پہلا('\n');
	إذا (سرا != B_ERROR)
	    strButtons.سیٹ_Byte_At(سرا, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->جوڑو_بچہ(button);
	fButtonsList.جوڑو_شے(button);

	max_Button_چوڑائی	= max_c(max_Button_چوڑائی,  button->Bounds().چوڑائی_2());
	max_Button_اچائی = max_c(max_Button_اچائی, button->Bounds().اچائی_2());
	buttons_چوڑائی   += button->Bounds().چوڑائی_2();
	buttons_اچائی  += button->Bounds().اچائی_2();

	إذا (سرا == B_ERROR)
	    اكسر;

	strButtons.Remove(0, سرا + 1);
    } طالما (سچ);

    int32 buttons_گنتی = fButtonsList.گنتی_اشیاء();
    buttons_چوڑائی      += kVimDialogButtonsSpacingX * (buttons_گنتی - 1);
    buttons_اچائی     += kVimDialogButtonsSpacingY * (buttons_گنتی - 1);
    عائم dialog_چوڑائی  = buttons_چوڑائی + k_Vim_Dialog_Icon_Stripe_چوڑائی +
	kVimDialogSpacingX * 2;
    عائم dialog_اچائی = max_Button_اچائی + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    منطقي vertical = (vim_strchr(p_go, GO_VERTICAL) != عدم) ||
	dialog_چوڑائی >= gui.vim_کھڑکی->Bounds().چوڑائی_2();
    إذا (vertical) {
	dialog_چوڑائی  -= buttons_چوڑائی;
	dialog_چوڑائی  += max_Button_چوڑائی;
	dialog_اچائی -= max_Button_اچائی;
	dialog_اچائی += buttons_اچائی;
    }

    dialog_چوڑائی  = max_c(dialog_چوڑائی,  k_Vim_Dialog_اقل_چوڑائی);

    //	message view
    BRect rect(0, 0, dialog_چوڑائی, 0);
    rect.کھبے  += k_Vim_Dialog_Icon_Stripe_چوڑائی + 16 + kVimDialogSpacingX;
    rect.چوٹی   += kVimDialogSpacingY;
    rect.سجے -= kVimDialogSpacingX;
    rect.bottom = rect.چوٹی;
    f_سنیہا_View = جديد BTextView(rect, "_tv_", rect.وتھ_By_Copy(B_ORIGIN),
	    B_FOLLOW_بایاں | B_FOLLOW_چوٹی, B_WILL_DRAW);

    f_سنیہا_View->سیٹ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color متن_Color = ui_color(B_PANEL_متن_COLOR);
    f_سنیہا_View->سیٹ_Font_And_Color(be_plain_font, B_FONT_ALL, &متن_Color);
    f_سنیہا_View->سیٹ_متن(سنیہا);
    f_سنیہا_View->MakeEditable(خطأ);
    f_سنیہا_View->MakeSelectable(خطأ);
    f_سنیہا_View->سیٹ_لفظ_Wrap(سچ);
    جوڑو_بچہ(f_سنیہا_View);

    عائم سنیہا_اچائی = f_سنیہا_View->متن_اچائی(0, f_سنیہا_View->گنتی_سطریں());
    f_سنیہا_View->ResizeBy(0, سنیہا_اچائی);
    f_سنیہا_View->سیٹ_متن_Rect(BRect(0, 0, rect.چوڑائی_2(), سنیہا_اچائی));

    dialog_اچائی += سنیہا_اچائی;

    //	input view
    إذا (f_داخلہ_قدر != عدم) {
	rect.چوٹی     =
	    rect.bottom += سنیہا_اچائی + kVimDialogSpacingY;
	f_داخلہ_Control = جديد BTextControl(rect, "_iv_", عدم, f_داخلہ_قدر, عدم,
		B_FOLLOW_بایاں | B_FOLLOW_چوٹی, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_داخلہ_Control->متن_View()->سیٹ_متن(f_داخلہ_قدر);
	f_داخلہ_Control->متن_View()->سیٹ_لفظ_Wrap(خطأ);
	جوڑو_بچہ(f_داخلہ_Control);

	عائم چوڑائی = 0.f, اچائی = 0.f;
	f_داخلہ_Control->Get_Preferred_ناپ(&چوڑائی, &اچائی);
	f_داخلہ_Control->MakeFocus(سچ);

	dialog_اچائی += اچائی + kVimDialogSpacingY * 1.5;
    }

    dialog_اچائی = max_c(dialog_اچائی, k_Vim_Dialog_اقل_اچائی);

    Resize_منزل(dialog_چوڑائی, dialog_اچائی);
    Move_منزل((gui.vim_کھڑکی->Bounds().چوڑائی_2() - dialog_چوڑائی) / 2,
	    (gui.vim_کھڑکی->Bounds().اچائی_2() - dialog_اچائی) / 2);

    //	adjust layout of buttons
    عائم button_چوڑائی = max_c(max_Button_چوڑائی, rect.چوڑائی_2() * 0.66);
    BPoint origin(dialog_چوڑائی, dialog_اچائی);
    origin.x -= kVimDialogSpacingX + (vertical ? button_چوڑائی : buttons_چوڑائی);
    origin.y -= kVimDialogSpacingY + (vertical ? buttons_اچائی	: max_Button_اچائی);

    لكل (int32 i = 0 ; i < buttons_گنتی; i++) {
	BButton *button = (BButton*)fButtonsList.شے_At(i);
	button->Move_منزل(origin);
	إذا (vertical) {
	    origin.y += button->چوکھٹا_2().اچائی_2() + kVimDialogButtonsSpacingY;
	    button->Resize_منزل(button_چوڑائی, button->چوکھٹا_2().اچائی_2());
	} وإلا
	    origin.x += button->چوکھٹا_2().چوڑائی_2() + kVimDialogButtonsSpacingX;

	إذا (dfltbutton == i + 1) {
	    button->MakeDefault(سچ);
	    button->MakeFocus(f_داخلہ_Control == عدم);
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
    إذا (f_داخلہ_قدر != عدم)
	vim_strncpy((char_u*)f_داخلہ_قدر, (char_u*)f_داخلہ_Control->متن_2(), IOSIZE - 1);

    إذا (جندرا_2())
	Quit();

    أرجع ret_قدر;
}

فراغ VimDialog::سنیہا_Received(BMessage *پیغام)
{
    int32 which = 0;
    إذا (پیغام->what != k_Vim_Dialog_Button_پیغام ||
	    پیغام->لبھو_Int32("which", &which) != B_OK)
	أرجع BWindow::سنیہا_Received(پیغام);

    f_Dialog_قدر = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ثابت حرف* label)
{
    BMessage *سنیہا = جديد BMessage(k_Vim_Dialog_Button_پیغام);
    سنیہا->جوڑو_Int32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = جديد BButton(rect, name.String(), label, سنیہا,
	    B_FOLLOW_دایاں | B_FOLLOW_BOTTOM);

    عائم چوڑائی = 0.f, اچائی = 0.f;
    button->Get_Preferred_ناپ(&چوڑائی, &اچائی);
    button->Resize_منزل(چوڑائی, اچائی);

    أرجع button;
}

VimDialog::View::View(BRect چوکھٹا)
    :	BView(چوکھٹا, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(عدم)
{
    سیٹ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    مٹاؤ fIconBitmap;
}

فراغ VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.سجے = k_Vim_Dialog_Icon_Stripe_چوڑائی;
    سیٹ_بلند_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    إذا (fIconBitmap == عدم)
	أرجع;

    سیٹ_Drawing_منوال(B_OP_ALPHA);
    سیٹ_Blending_منوال(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

فراغ VimDialog::View::InitIcon(int32 type)
{
    إذا (type == VIM_GENERIC)
	أرجع;

    BPath رستہ;
    حالت_t حالت = لبھو_directory(B_BEOS_SERVERS_DIRECTORY, &رستہ);
    إذا (حالت != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(حالت));
	أرجع;
    }

    رستہ.Append("app_server");

    BFile file(رستہ.رستہ_2(), O_RDONLY);
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

    int32 icon_ناپ_2 = 32;
    fIconBitmap = جديد BBitmap(BRect(0, 0, icon_ناپ_2 - 1, icon_ناپ_2 - 1), 0, B_RGBA32);
    إذا (fIconBitmap == عدم || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == عدم) ? "null" : strerror(fIconBitmap->InitCheck()));
	أرجع;
    }

    size_t ناپ = 0;
    ثابت uint8* icon_انکڑے = عدم;
    //	try vector icon first?
    icon_انکڑے = (ثابت uint8*)resources.لدو_Resource(B_سمتیہ_ICON_TYPE, name, &ناپ);
    إذا (icon_انکڑے != عدم && BIconUtils::Get_سمتیہ_Icon(icon_انکڑے, ناپ, fIconBitmap) == B_OK)
	أرجع;

    //	try bitmap icon now
    icon_انکڑے = (ثابت uint8*)resources.لدو_Resource(B_LARGE_ICON_TYPE, name, &ناپ);
    إذا (icon_انکڑے == عدم) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	مٹاؤ fIconBitmap;
	fIconBitmap = عدم;
	أرجع;
    }

    إذا (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(icon_انکڑے, icon_ناپ_2, icon_ناپ_2, icon_ناپ_2, fIconBitmap);
}

ثابت بنانشانی صحيح  k_Vim_Dialog_OKButton_پیغام = 'FDOK';
ثابت بنانشانی صحيح  k_Vim_Dialog_Cancel_Button_پیغام = 'FDCN';
ثابت بنانشانی صحيح  k_Vim_Dialog_ناپ_داخلہ_پیغام = 'SICH';
ثابت بنانشانی صحيح  k_Vim_Dialog_Family_Select_پیغام = 'MSFM';
ثابت بنانشانی صحيح  k_Vim_Dialog_Style_Select_پیغام = 'MSST';
ثابت بنانشانی صحيح  k_Vim_Dialog_ناپ_Select_پیغام = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, عائم* ناپ)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_کھڑکی_LOOK, B_MODAL_APP_کھڑکی_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , f_حالت(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_قدر(خطأ)
    , fFamily(family)
    , fStyle(style)
    , f_ناپ(ناپ)
    , f_Font_ناپ(*ناپ)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_داخلہ(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_لمائی);
    strncpy(fFontStyle, *style, B_FONT_STYLE_لمائی);

    //	"client" area view
    BBox *clientBox = جديد BBox(Bounds(), B_خالی_تھاواں_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_چوکھٹا_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    جوڑو_بچہ(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.بایاں_چوٹی(), RC.بایاں_چوٹی());

    //	at first create all controls
    fPreview = جديد BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->جوڑو_بچہ(fPreview);

    BBox* boxDivider = جديد BBox(rc, B_خالی_تھاواں_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->جوڑو_بچہ(boxDivider);

    BStringView *labelFamily = جديد BStringView(rc, "labelFamily", "Family:");
    clientBox->جوڑو_بچہ(labelFamily);
    labelFamily->Resize_منزل_Preferred();

    BStringView *labelStyle = جديد BStringView(rc, "labelStyle", "Style:");
    clientBox->جوڑو_بچہ(labelStyle);
    labelStyle->Resize_منزل_Preferred();

    BStringView *label_ناپ = جديد BStringView(rc, "labelSize", "Size:");
    clientBox->جوڑو_بچہ(label_ناپ);
    label_ناپ->Resize_منزل_Preferred();

    fFamiliesList = جديد BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = جديد BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_بایاں_دایاں, 0, خطأ, سچ);
    clientBox->جوڑو_بچہ(scrollFamilies);

    fStylesList= جديد BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = جديد BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_بایاں_دایاں, 0, خطأ, سچ);
    clientBox->جوڑو_بچہ(scrollStyles);

    f_Sizes_داخلہ = جديد BTextControl(rc, "inputSize", عدم, "???",
	    جديد BMessage(k_Vim_Dialog_ناپ_داخلہ_پیغام));
    clientBox->جوڑو_بچہ(f_Sizes_داخلہ);
    f_Sizes_داخلہ->Resize_منزل_Preferred();

    fSizesList = جديد BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = جديد BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_بایاں_دایاں, 0, خطأ, سچ);
    clientBox->جوڑو_بچہ(scrollSizes);

    BButton *buttonOK = جديد BButton(rc, "buttonOK", "OK",
			جديد BMessage(k_Vim_Dialog_OKButton_پیغام));
    clientBox->جوڑو_بچہ(buttonOK);
    buttonOK->Resize_منزل_Preferred();

    BButton *buttonCancel = جديد BButton(rc, "buttonCancel", "Cancel",
			جديد BMessage(k_Vim_Dialog_Cancel_Button_پیغام));
    clientBox->جوڑو_بچہ(buttonCancel);
    buttonCancel->Resize_منزل_Preferred();

    //	layout controls
    عائم سطر_اچائی = labelFamily->Bounds().اچائی_2();
    عائم preview_اچائی = سطر_اچائی * 3;
    عائم وتھ_YLabels = preview_اچائی + kVimDialogSpacingY;
    عائم وتھ_YLists = وتھ_YLabels + سطر_اچائی + kVimDialogSpacingY / 2;
    عائم وتھ_YSizes = وتھ_YLists + f_Sizes_داخلہ->Bounds().اچائی_2() + kVimDialogSpacingY / 2;
    عائم lists_اچائی = سطر_اچائی * 9;
    عائم وتھ_YButtons = وتھ_YLists + lists_اچائی +  kVimDialogSpacingY;
    عائم max_Controls_اچائی = وتھ_YButtons + buttonOK->Bounds().اچائی_2();
    عائم families_چوڑائی = labelFamily->Bounds().چوڑائی_2() * 5;
    عائم وتھ_XStyles = families_چوڑائی + kVimDialogSpacingX;
    عائم styles_چوڑائی = labelStyle->Bounds().چوڑائی_2() * 4;
    عائم وتھ_XSizes = وتھ_XStyles + styles_چوڑائی + kVimDialogSpacingX;
    عائم sizes_چوڑائی = label_ناپ->Bounds().چوڑائی_2() * 2;
    عائم max_Controls_چوڑائی = وتھ_XSizes + sizes_چوڑائی;

    Resize_منزل(max_Controls_چوڑائی + kVimDialogSpacingX * 2,
	max_Controls_اچائی + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vim_کھڑکی->چوکھٹا_2();
    Move_منزل(rcVim.کھبے + (rcVim.چوڑائی_2() - چوکھٹا_2().چوڑائی_2()) / 2,
	    rcVim.چوٹی + (rcVim.اچائی_2() - چوکھٹا_2().اچائی_2()) / 2);

    fPreview->Resize_منزل(max_Controls_چوڑائی, preview_اچائی);
    fPreview->سیٹ_Alignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, preview_اچائی + kVimDialogSpacingY / 2);
    boxDivider->Resize_منزل(max_Controls_چوڑائی, 1.f);

    labelFamily->MoveBy(0.f, وتھ_YLabels);
    labelStyle->MoveBy(وتھ_XStyles, وتھ_YLabels);
    label_ناپ->MoveBy(وتھ_XSizes, وتھ_YLabels);

    //	text control alignment issues
    عائم insetX = f_Sizes_داخلہ->متن_View()->Bounds().چوڑائی_2() - f_Sizes_داخلہ->Bounds().چوڑائی_2();
    عائم insetY = f_Sizes_داخلہ->متن_View()->Bounds().چوڑائی_2() - f_Sizes_داخلہ->Bounds().چوڑائی_2();

    scrollFamilies->MoveBy(0.f, وتھ_YLists);
    scrollStyles->MoveBy(وتھ_XStyles, وتھ_YLists);
    f_Sizes_داخلہ->MoveBy(وتھ_XSizes + insetX / 2, وتھ_YLists + insetY / 2);
    scrollSizes->MoveBy(وتھ_XSizes, وتھ_YSizes);

    f_Sizes_داخلہ->سیٹ_Alignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->Resize_منزل(families_چوڑائی, lists_اچائی);
    scrollStyles->Resize_منزل(styles_چوڑائی, lists_اچائی);
    f_Sizes_داخلہ->Resize_منزل(sizes_چوڑائی, f_Sizes_داخلہ->Bounds().اچائی_2());
    scrollSizes->Resize_منزل(sizes_چوڑائی,
	    lists_اچائی - (وتھ_YSizes - وتھ_YLists));

    buttonOK->MoveBy(max_Controls_چوڑائی - buttonOK->Bounds().چوڑائی_2(), وتھ_YButtons);
    buttonCancel->MoveBy(max_Controls_چوڑائی - buttonOK->Bounds().چوڑائی_2()
	    - buttonCancel->Bounds().چوڑائی_2() - kVimDialogSpacingX, وتھ_YButtons);

    //	fill lists
    صحيح sel_اشاریہ = -1;
    صحيح گنتی = گنتی_font_families();
    لكل (صحيح i = 0; i < گنتی; i++) {
	font_family family;
	إذا (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->جوڑو_شے(جديد BString_شے((ثابت حرف*)family));
	    إذا (لڑی_مقابلہ(family, fFontFamily, B_FONT_FAMILY_لمائی) == 0)
		sel_اشاریہ = i;
	}
    }

    إذا (sel_اشاریہ >= 0) {
	fFamiliesList->Select(sel_اشاریہ);
	fFamiliesList->Scroll_منزل_Selection();
    }

    _UpdateFontStyles();

    sel_اشاریہ = -1;
    لكل (صحيح ناپ = 8, اشاریہ = 0; ناپ <= 18; ناپ++, اشاریہ++) {
	BString str;
	str << ناپ;
	fSizesList->جوڑو_شے(جديد BString_شے(str));
	إذا (ناپ == f_Font_ناپ)
	    sel_اشاریہ = اشاریہ;

    }

    إذا (sel_اشاریہ >= 0) {
	fSizesList->Select(sel_اشاریہ);
	fSizesList->Scroll_منزل_Selection();
    }

    fFamiliesList->سیٹ_Selection_سنیہا(جديد BMessage(k_Vim_Dialog_Family_Select_پیغام));
    fStylesList->سیٹ_Selection_سنیہا(جديد BMessage(k_Vim_Dialog_Style_Select_پیغام));
    fSizesList->سیٹ_Selection_سنیہا(جديد BMessage(k_Vim_Dialog_ناپ_Select_پیغام));
    f_Sizes_داخلہ->سیٹ_Modification_سنیہا(جديد BMessage(k_Vim_Dialog_ناپ_داخلہ_پیغام));

    Update_ناپ_داخلہ_Preview();
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
    طالما (0 < list->گنتی_اشیاء())
	مٹاؤ (dynamic_cast<BString_شے*>(list->Remove_شے((int32)0)));
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

    إذا (جندرا_2())
	Quit();

    أرجع ret_قدر;
}


فراغ VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 sel_اشاریہ = -1;
    int32 گنتی = گنتی_font_styles(fFontFamily);
    لكل (int32 i = 0; i < گنتی; i++) {
	font_style style;
	uint32 flags = 0;
	إذا (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->جوڑو_شے(جديد BString_شے((ثابت حرف*)style));
	    إذا (لڑی_مقابلہ(style, fFontStyle, B_FONT_STYLE_لمائی) == 0)
		sel_اشاریہ = i;
	}
    }

    إذا (sel_اشاریہ >= 0) {
	fStylesList->Select(sel_اشاریہ);
	fStylesList->Scroll_منزل_Selection();
    } وإلا
	fStylesList->Select(0);
}


فراغ VimSelectFontDialog::Update_ناپ_داخلہ_Preview()
{
    حرف buf[10] = {0};
    vim_snprintf(buf, حجم(buf), (حرف*)"%.0f", f_Font_ناپ);
    f_Sizes_داخلہ->سیٹ_متن(buf);
}


فراغ VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.سیٹ_ناپ(f_Font_ناپ);
    font.سیٹ_Family_And_Style(fFontFamily, fFontStyle);
    fPreview->سیٹ_Font(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_ناپ);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (صحيح)f_Font_ناپ << " pt.";
    fPreview->سیٹ_متن(str);
}


    منطقي
VimSelectFontDialog::Update_From_List_شے(BListView* list, حرف* متن, صحيح متن_ناپ)
{
    int32 اشاریہ = list->CurrentSelection();
    إذا (اشاریہ < 0)
	أرجع خطأ;
    BString_شے* شے = (BString_شے*)list->شے_At(اشاریہ);
    إذا (شے == عدم)
	أرجع خطأ;
    strncpy(متن, شے->متن_2(), متن_ناپ);
    أرجع سچ;
}


فراغ VimSelectFontDialog::سنیہا_Received(BMessage *پیغام)
{
    بدل (پیغام->what) {
	حالة k_Vim_Dialog_OKButton_پیغام:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_لمائی);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_لمائی);
	    *f_ناپ = f_Font_ناپ;
	    f_Dialog_قدر = سچ;
	حالة k_Vim_Dialog_Cancel_Button_پیغام:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    أرجع;
	حالة B_کلید_اتے:
	    {
		int32 کلید = 0;
		إذا (پیغام->لبھو_Int32("raw_char", &کلید) == B_OK
			&& کلید == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    اكسر;

	حالة k_Vim_Dialog_Family_Select_پیغام:
	    إذا (Update_From_List_شے(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_لمائی)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    اكسر;
	حالة k_Vim_Dialog_Style_Select_پیغام:
	    إذا (Update_From_List_شے(fStylesList,
		    fFontStyle, B_FONT_STYLE_لمائی))
		_UpdateFontPreview();
	    اكسر;
	حالة k_Vim_Dialog_ناپ_Select_پیغام:
	    {
		حرف buf[10] = {0};
		إذا (Update_From_List_شے(fSizesList, buf, حجم(buf))) {
		    عائم ناپ = atof(buf);
		    إذا (ناپ > 0.f) {
			f_Font_ناپ = ناپ;
			Update_ناپ_داخلہ_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    اكسر;
	حالة k_Vim_Dialog_ناپ_داخلہ_پیغام:
	    {
		عائم ناپ = atof(f_Sizes_داخلہ->متن_2());
		إذا (ناپ > 0.f) {
		    f_Font_ناپ = ناپ;
		    _UpdateFontPreview();
		}
	    }
	    اكسر;
	افتراضي:
	    اكسر;
    }
    أرجع BWindow::سنیہا_Received(پیغام);
}

#جے_انت // FEAT_GUI_DIALOG

#جے_تعریف_شدہ FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
ساكن BMessage * Menu_سنیہا(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect چوکھٹا, ثابت حرف *name) :
    BBox(چوکھٹا, name, B_FOLLOW_بایاں | B_FOLLOW_چوٹی, B_WILL_DRAW | B_چوکھٹا_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 گنتی = fButtonsList.گنتی_اشیاء();
    لكل (int32 i = 0; i < گنتی; i++)
	مٹاؤ (BPictureButton*)fButtonsList.شے_At(i);
    fButtonsList.Make_خالی_تھاواں();

    مٹاؤ عام_Buttons_Bitmap;
    مٹاؤ grayedButtonsBitmap;
    عام_Buttons_Bitmap    = عدم;
    grayedButtonsBitmap  = عدم;
}

    فراغ
VimToolbar::Attached_منزل_کھڑکی()
{
    BBox::Attached_منزل_کھڑکی();

    سیٹ_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    عائم
VimToolbar::Toolbar_اچائی() ثابت
{
    عائم ناپ = عدم == عام_Buttons_Bitmap ? 18. : عام_Buttons_Bitmap->Bounds().اچائی_2();
    أرجع ناپ + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    منطقي
VimToolbar::Modify_Bitmap_منزل_Grayed(BBitmap *bitmap)
{
    عائم اچائی = bitmap->Bounds().اچائی_2();
    عائم چوڑائی  = bitmap->Bounds().چوڑائی_2();

    rgb_color *بٹ = (rgb_color*)bitmap->بٹ_2();
    int32 pixels = bitmap->بٹ_لمائی() / 4;
    لكل (int32 i = 0; i < pixels; i++) {
	بٹ[i].red = بٹ[i].green =
	بٹ[i].blue = ((uint32)بٹ[i].red + بٹ[i].green + بٹ[i].blue) / 3;
	بٹ[i].alpha /= 4;
    }

    أرجع سچ;
}

    منطقي
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    عام_Buttons_Bitmap = لدو_Vim_Bitmap("builtin-tools.png");
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

BBitmap *VimToolbar::لدو_Vim_Bitmap(ثابت حرف* fileName)
{
    BBitmap *bitmap = عدم;

    صحيح mustfree = 0;
    char_u* runtime_رستہ = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    إذا (runtime_رستہ != عدم && fileName != عدم) {
	BString str_رستہ((حرف*)runtime_رستہ);
	str_رستہ << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(str_رستہ.String());
    }

    إذا (mustfree)
	vim_free(runtime_رستہ);

    أرجع bitmap;
}

    منطقي
VimToolbar::GetPictureFromBitmap(BPicture *picture_منزل, int32 اشاریہ, BBitmap *bitmapFrom, منطقي pressed)
{
    عائم ناپ = bitmapFrom->Bounds().اچائی_2() + 1.;

    BView view(BRect(0, 0, ناپ, ناپ), "", 0, 0);

    جوڑو_بچہ(&view);
    view.BeginPicture(picture_منزل);

    view.سیٹ_بلند_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.سیٹ_Drawing_منوال(B_OP_OVER);

    BRect سروت(0, 0, ناپ - 1, ناپ - 1);
    BRect destination(سروت);

    سروت.وتھ_By(ناپ * اشاریہ, 0);
    destination.وتھ_By(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, سروت, destination);

    إذا (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	ناپ += ButtonMargin * 2 - 1;
	view.Begin_سطر_Array(4);
	view.جوڑو_سطر(BPoint(0, 0),	 BPoint(ناپ, 0),    shadowColor);
	view.جوڑو_سطر(BPoint(ناپ, 0),	 BPoint(ناپ, ناپ), shineColor);
	view.جوڑو_سطر(BPoint(ناپ, ناپ), BPoint(0, ناپ),    shineColor);
	view.جوڑو_سطر(BPoint(0, ناپ),	 BPoint(0, 0),	     shadowColor);
	view.سرا_سطر_Array();
    }

    view.سرا_Picture();
    Remove_بچہ(&view);

    أرجع سچ;
}

    منطقي
VimToolbar::جوڑو_Button(int32 اشاریہ, vimmenu_T *menu)
{
    BPictureButton *button = عدم;
    إذا (!menu_is_separator(menu->name)) {
	عائم ناپ = عام_Buttons_Bitmap ?
	    عام_Buttons_Bitmap->Bounds().اچائی_2() + 1. + ButtonMargin * 2 : 18.;
	BRect چوکھٹا(0, 0, ناپ, ناپ);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture picture_سلیٹی;

	إذا (menu->iconfile == عدم && menu->iconidx >= 0 && عام_Buttons_Bitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, عام_Buttons_Bitmap, سچ);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, عام_Buttons_Bitmap, خطأ);
	    GetPictureFromBitmap(&picture_سلیٹی, menu->iconidx, grayedButtonsBitmap, خطأ);
	} وإلا {

	    char_u بفر[MAXPATHL] = {0};
	    BBitmap *bitmap = عدم;

	    إذا (menu->iconfile) {
		gui_لبھو_iconfile(menu->iconfile, بفر, (حرف*)"png");
		bitmap = BTranslationUtils::GetBitmap((حرف*)بفر);
	    }

	    إذا (bitmap == عدم && gui_لبھو_bitmap(menu->name, بفر, (حرف*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((حرف*)بفر);

	    إذا (bitmap == عدم)
		bitmap = جديد BBitmap(BRect(0, 0, ناپ, ناپ), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, سچ);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, خطأ);
	    Modify_Bitmap_منزل_Grayed(bitmap);
	    GetPictureFromBitmap(&picture_سلیٹی, 0, bitmap, خطأ);

	    مٹاؤ bitmap;
	}

	button = جديد BPictureButton(چوکھٹا, (حرف*)menu->name,
		    &pictureOff, &pictureOn, Menu_سنیہا(menu));

	button->سیٹ_Disabled_On(&picture_سلیٹی);
	button->سیٹ_Disabled_Off(&picture_سلیٹی);

	button->سیٹ_نشانہ(gui.vim_متن_Area);

	جوڑو_بچہ(button);

	menu->button = button;
    }

    منطقي نتیجہ = fButtonsList.جوڑو_شے(button, اشاریہ);
    InvalidateLayout();
    أرجع نتیجہ;
}

    منطقي
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    إذا (menu->button) {
	إذا (fButtonsList.Remove_شے(menu->button)) {
	    مٹاؤ menu->button;
	    menu->button = عدم;
	}
    }
    أرجع سچ;
}

    منطقي
VimToolbar::سلیٹی_Button(vimmenu_T *menu, صحيح grey)
{
    إذا (menu->button) {
	int32 اشاریہ = fButtonsList.اشاریہ_دا(menu->button);
	إذا (اشاریہ >= 0)
	    menu->button->سیٹ_Enabled(grey ? خطأ : سچ);
    }
    أرجع سچ;
}

    فراغ
VimToolbar::InvalidateLayout()
{
    int32 وتھ = ToolbarMargin;
    int32 گنتی = fButtonsList.گنتی_اشیاء();
    لكل (int32 i = 0; i < گنتی; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.شے_At(i);
	إذا (button) {
	    button->Move_منزل(وتھ, ToolbarMargin);
	    وتھ += button->Bounds().چوڑائی_2() + ToolbarMargin;
	} وإلا
	    وتھ += ToolbarMargin * 3;
    }
}

#جے_انت /*FEAT_TOOLBAR*/

#إذا defined(FEAT_GUI_TABLINE)

    عائم
Vim_Tab_سطر::Tabline_اچائی() ثابت
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    أرجع Tab_اچائی(); //  + ToolbarMargin;
}

فراغ
Vim_Tab_سطر::Mouse_تھلے(BPoint نقطہ)
{
    إذا (!gui_mch_showing_tabline())
	أرجع;

    BMessage *m = کھڑکی()->Current_سنیہا();
    أكد(m);

    int32 buttons = 0;
    m->لبھو_Int32("buttons", &buttons);

    int32 clicks = 0;
    m->لبھو_Int32("clicks", &clicks);

    صحيح اشاریہ = 0; //  0 means here - no tab found
    لكل (صحيح i = 0; i < گنتی_Tabs(); i++) {
	إذا (Tab_چوکھٹا(i).شامل_اے_2(نقطہ)) {
	    اشاریہ = i + 1; //  indexes are 1-based
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
	event = اشاریہ > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    وإلا إذا (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPop_اتے_Menu* نکالو_اتے_Menu = جديد BPop_اتے_Menu("tabLineContextMenu", خطأ, خطأ);
	نکالو_اتے_Menu->جوڑو_شے(جديد BMenu_شے(_("Close tabi R"), جديد BMessage(TABLINE_MENU_CLOSE)));
	نکالو_اتے_Menu->جوڑو_شے(جديد BMenu_شے(_("New tab    T"), جديد BMessage(TABLINE_MENU_NEW)));
	نکالو_اتے_Menu->جوڑو_شے(جديد BMenu_شے(_("Open tab..."), جديد BMessage(TABLINE_MENU_OPEN)));

	Convert_منزل_Screen(&نقطہ);
	BMenu_شے* شے = نکالو_اتے_Menu->Go(نقطہ);
	إذا (شے != عدم) {
	    event = شے->Command();
	}

	مٹاؤ نکالو_اتے_Menu;

    } وإلا {
	//  default processing
	BTabView::Mouse_تھلے(نقطہ);
	أرجع;
    }

    إذا (event < 0)
	أرجع;

    Vim_Tabline_Menu_پیغام tmm;
    tmm.اشاریہ = اشاریہ;
    tmm.event = event;
    write_پورٹ(gui.vdcmp, Vim_پیغام::TablineMenu, &tmm, حجم(tmm));
}

فراغ
Vim_Tab_سطر::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    إذا (tab_سطر != عدم) {

	int32 i = 0;
	لكل (; i < tab_سطر->گنتی_Tabs(); i++)
	    إذا (هذا == tab_سطر->TabAt(i))
		اكسر;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	إذا (i < tab_سطر->گنتی_Tabs()) {
	    Vim_Tabline_پیغام tm;
	    tm.اشاریہ = i + 1;
	    write_پورٹ(gui.vdcmp, Vim_پیغام::Tabline, &tm, حجم(tm));
	}
    }
}

#جے_انت //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
ساكن حرف appsig[] = "application/x-vnd.Haiku-Vim-8";
کلید_map *کلید_Map;
حرف *کلید_Map_Chars;
صحيح main_exitcode = 127;

    حالت_t
gui_haiku_process_event(bigtime_t timeout)
{
    بنية Vim_پیغام vm;
    int32 what;
    ssize_t ناپ;

    ناپ = read_پورٹ_etc(gui.vdcmp, &what, &vm, حجم(vm),
	    B_TIMEOUT, timeout);

    إذا (ناپ >= 0) {
	بدل (what) {
	    حالة Vim_پیغام::کلید_2:
		{
		    char_u *string = vm.u.کلید_2.chars;
		    صحيح len = vm.u.کلید_2.لمائی;
		    إذا (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_داخلہ_buf();
			got_int = TRUE;
		    }

		    إذا (vm.u.کلید_2.csi_escape)
#جے_بنا_تعریف FEAT_MBYTE_IME
		    {
			صحيح	i;
			char_u	buf[2];

			لكل (i = 0; i < len; ++i)
			{
			    جوڑو_منزل_داخلہ_buf(string + i, 1);
			    إذا (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (صحيح)KE_CSI;
				جوڑو_منزل_داخلہ_buf(buf, 2);
			    }
			}
		    }
#وإلا
			جوڑو_منزل_داخلہ_buf_csi(string, len);
#جے_انت
		    وإلا
			جوڑو_منزل_داخلہ_buf(string, len);
		}
		اكسر;
	    حالة Vim_پیغام::Resize:
		gui_resize_shell(vm.u.New_ناپ.چوڑائی, vm.u.New_ناپ.اچائی);
		اكسر;
	    حالة Vim_پیغام::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_گنتی =
			atomic_جوڑو(&vm.u.Scroll.sb->scroll_Event_گنتی, -1);
		    إذا (old_گنتی <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.قدر, vm.u.Scroll.stillDragging);
		}
		اكسر;
#إذا defined(FEAT_MENU)
	    حالة Vim_پیغام::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		اكسر;
#جے_انت
	    حالة Vim_پیغام::Mouse:
		{
		    int32 old_گنتی;
		    إذا (vm.u.Mouse.button == MOUSE_DRAG)
			old_گنتی =
			    atomic_جوڑو(&gui.vim_متن_Area->mouse_Drag_Event_گنتی, -1);
		    وإلا
			old_گنتی = 0;
		    إذا (old_گنتی <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		اكسر;
	    حالة Vim_پیغام::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		اكسر;
	    حالة Vim_پیغام::Focus:
		gui.وچ_focus = vm.u.Focus.سرگرم;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		إذا (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		اكسر;
	    حالة Vim_پیغام::Refs:
		::RefsReceived(vm.u.Refs.سنیہا, vm.u.Refs.changedir);
		اكسر;
	    حالة Vim_پیغام::Tabline:
		send_tabline_event(vm.u.Tabline.اشاریہ);
		اكسر;
	    حالة Vim_پیغام::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.اشاریہ, vm.u.TablineMenu.event);
		اكسر;
	    افتراضي:
		//  unrecognised message, ignore it
		اكسر;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    أرجع ناپ;
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
vim_جندرا_screen()
{
    أرجع !gui.vim_کھڑکی || gui.vim_کھڑکی->جندرا_2();
}

    فراغ
vim_unlock_screen()
{
    إذا (gui.vim_کھڑکی)
	gui.vim_کھڑکی->Unlock();
}

#تعریف_کرو لگاتار_BAPPLICATION_وچ_NEW_THREAD	0

#إذا لگاتار_BAPPLICATION_وچ_NEW_THREAD

    int32
لگاتار_vimapp(فراغ *دلیلاں)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.لگاتار_2();		    // Run until Quit() called

    أرجع 0;
}

#وإلا

    int32
call_main(فراغ *دلیلاں)
{
    بنية Main_دلیلاں *ma = (Main_دلیلاں *)دلیلاں;

    أرجع مکھ(ma->argc, ma->argv);
}
#جے_انت

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
	get_thread_info(لبھو_thread(عدم), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_پورٹ(B_MAX_پورٹ_گنتی, "vim VDCMP");

#إذا لگاتار_BAPPLICATION_وچ_NEW_THREAD
	thread_id tid = spawn_thread(لگاتار_vimapp, "vim VimApp",
		tinfo.ترجیح, عدم);
	إذا (tid >= B_OK) {
	    resume_thread(tid);
	} وإلا {
	    getout(1);
	}
#وإلا
	Main_دلیلاں ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.ترجیح, &ma);
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
	    app.لگاتار_2();		    // Run until Quit() called
	    // fprintf(stderr, "app.Run() returned...\n");
	    حالت_t dummy_exitcode;
	    (فراغ)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    باہر_نکلو(main_exitcode);
	}
#جے_انت
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
    gui.def_پٹھ_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.پٹھ_pixel = gui.def_پٹھ_pixel;

    gui.scrollbar_چوڑائی = (صحيح) B_V_SCROLL_BAR_چوڑائی;
    gui.scrollbar_اچائی = (صحيح) B_H_SCROLL_BAR_اچائی;
#جے_تعریف_شدہ FEAT_MENU
    gui.menu_اچائی = 19;   //	initial guess -
    //	correct for my default settings
#جے_انت
    gui.border_وتھ = 3;  //	coordinates are inside window borders

    إذا (gui.vdcmp < B_OK)
	أرجع FAIL;
    get_کلید_map(&کلید_Map, &کلید_Map_Chars);

    gui.vim_کھڑکی = جديد Vim_کھڑکی();	// hidden and locked
    إذا (!gui.vim_کھڑکی)
	أرجع FAIL;

    gui.vim_کھڑکی->لگاتار_2();	// Run() unlocks but does not show

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    سیٹ_عام_colors();

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
    rgb_color rgb = GUI_منزل_RGB(gui.پٹھ_pixel);

    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vimForm->سیٹ_View_Color(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open()
{
    إذا (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_سیٹ_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_کھڑکی->Show();
	gui.vim_کھڑکی->Unlock();
	أرجع OK;
    }

    أرجع FAIL;
}

    فراغ
gui_mch_exit(صحيح vim_exitcode)
{
    إذا (gui.vim_کھڑکی) {
	thread_id tid = gui.vim_کھڑکی->Thread();
	gui.vim_کھڑکی->جندرا_2();
	gui.vim_کھڑکی->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_پورٹ(gui.vdcmp);
#إذا !لگاتار_BAPPLICATION_وچ_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#جے_انت
    إذا (gui.vimApp) {
	Vim_متن_Area_View::guiBlankMouse(خطأ);

	main_exitcode = vim_exitcode;
#إذا لگاتار_BAPPLICATION_وچ_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->جندرا_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#وإلا
	gui.vimApp->جندرا_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	// suicide
	exit_thread(vim_exitcode);
#جے_انت
    }
    // If we are somehow still here, let mch_exit() handle things.
}

/*
 * Get the position of the top left corner of the window.
 */
    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	BRect r;
	r = gui.vim_کھڑکی->چوکھٹا_2();
	gui.vim_کھڑکی->Unlock();
	*x = (صحيح)r.کھبے;
	*y = (صحيح)r.چوٹی;
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
gui_mch_سیٹ_winpos(صحيح x, صحيح y)
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_کھڑکی->Move_منزل(x, y);
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
فراغ
gui_mch_سیٹ_shellsize(
	صحيح	چوڑائی,
	صحيح	اچائی,
	صحيح	min_چوڑائی,
	صحيح	min_اچائی,
	صحيح	ادھار_چوڑائی,
	صحيح	ادھار_اچائی,
	صحيح	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_کھڑکی->Resize_منزل(چوڑائی - PEN_چوڑائی, اچائی - PEN_چوڑائی);

	// set size limits
	عائم min_چوڑائی_2, max_چوڑائی, min_اچائی_2, max_اچائی;

	gui.vim_کھڑکی->Get_ناپ_Limits(&min_چوڑائی_2, &max_چوڑائی,
		&min_اچائی_2, &max_اچائی);
	gui.vim_کھڑکی->سیٹ_ناپ_Limits(min_چوڑائی, max_چوڑائی,
		min_اچائی, max_اچائی);

	/*
	 * Set the resizing alignment depending on font size.
	 */
	gui.vim_کھڑکی->سیٹ_کھڑکی_Alignment(
		B_PIXEL_ALIGNMENT,	//  window_alignment mode,
		1,		//  int32 h,
		0,		//  int32 hOffset = 0,
		gui.char_چوڑائی,	    //	int32 width = 0,
		ادھار_چوڑائی,	    //	int32 widthOffset = 0,
		1,		//  int32 v = 0,
		0,		//  int32 vOffset = 0,
		gui.char_اچائی,	//  int32 height = 0,
		ادھار_اچائی	    //	int32 heightOffset = 0
		);

	gui.vim_کھڑکی->Unlock();
    }
}

فراغ
gui_mch_get_screen_dimensions(
	صحيح	*screen_w,
	صحيح	*screen_h)
{
    BRect چوکھٹا;

    {
	BScreen screen(gui.vim_کھڑکی);

	إذا (screen.IsValid()) {
	    چوکھٹا = screen.چوکھٹا_2();
	} وإلا {
	    چوکھٹا.سجے = 640;
	    چوکھٹا.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (صحيح) چوکھٹا.سجے - 2 * gui.scrollbar_چوڑائی - 20;
    *screen_h = (صحيح) چوکھٹا.bottom - gui.scrollbar_اچائی
#جے_تعریف_شدہ FEAT_MENU
	- gui.menu_اچائی
#جے_انت
	- 30;
}

فراغ
gui_mch_سیٹ_متن_area_pos(
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    إذا (!gui.vim_متن_Area)
	أرجع;

    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_متن_Area->Move_منزل(x, y);
	gui.vim_متن_Area->Resize_منزل(w - PEN_چوڑائی, h - PEN_چوڑائی);

#جے_تعریف_شدہ FEAT_GUI_TABLINE
	إذا (gui.vimForm->Tab_سطر() != عدم) {
	    gui.vimForm->Tab_سطر()->Resize_منزل(w, gui.vimForm->Tabline_اچائی());
	}
#جے_انت // FEAT_GUI_TABLINE

	gui.vim_کھڑکی->Unlock();
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
    إذا (gui.vim_کھڑکی->جندرا_2()) {
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
	gui.vim_کھڑکی->Unlock();
    }
}

فراغ
gui_mch_سیٹ_scrollbar_thumb(
	scrollbar_T *sb,
	صحيح	val,
	صحيح	ناپ,
	صحيح	max)
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	VimScrollBar *s = sb->id;
	إذا (max == 0) {
	    s->سیٹ_قدر(0);
	    s->سیٹ_Range(0.0, 0.0);
	} وإلا {
	    s->سیٹ_Proportion((عائم)ناپ / (max + 1.0));
	    s->سیٹ_قدماں(1.0, ناپ > 5 ? ناپ - 2 : ناپ);
#جے_بنا_تعریف SCROLL_PAST_سرا	    //	really only defined in gui.c...
	    max = max + 1 - ناپ;
#جے_انت
	    إذا (max < s->قدر_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->سیٹ_قدر(val);
		s->سیٹ_Range(0.0, max);
	    } وإلا {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->سیٹ_Range(0.0, max);
		s->سیٹ_قدر(val);
	    }
	}
	gui.vim_کھڑکی->Unlock();
    }
}

فراغ
gui_mch_سیٹ_scrollbar_pos(
	scrollbar_T *sb,
	صحيح	x,
	صحيح	y,
	صحيح	w,
	صحيح	h)
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	BRect winb = gui.vim_کھڑکی->Bounds();
	عائم vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->Resize_منزل(w - PEN_چوڑائی, h - PEN_چوڑائی);
	إذا (winb.سجے-(x+w)<w) vsbx = winb.سجے - (w - PEN_چوڑائی);
	vsb->Move_منزل(vsbx, vsby);
	gui.vim_کھڑکی->Unlock();
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
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	vsb->سیٹ_نشانہ(gui.vim_متن_Area);
	vsb->Hide();
	gui.vimForm->جوڑو_بچہ(vsb);
	gui.vim_کھڑکی->Unlock();
    }
}

#إذا defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
فراغ
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	sb->id->RemoveSelf();
	مٹاؤ sb->id;
	gui.vim_کھڑکی->Unlock();
    }
}
#جے_انت

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

#تعریف_کرو BLINK_NONE  0
#تعریف_کرو BLINK_OFF   1
#تعریف_کرو BLINK_ON    2

ساكن صحيح	blink_state = BLINK_NONE;
ساكن long_u	    blink_waittime = 700;
ساكن long_u	    blink_ontime = 400;
ساكن long_u	    blink_offtime = 250;
ساكن صحيح  blink_timer = 0;

فراغ
gui_mch_سیٹ_blinking(
	لما	waittime,
	لما	on,
	لما	off)
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
gui_mch_شروع_blink()
{
    // TODO
    إذا (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.وچ_focus)
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
    إذا (gui.vim_کھڑکی->جندرا_2())
    {
	صحيح rc = gui.vim_متن_Area->mchInitFont(font_name);
	gui.vim_کھڑکی->Unlock();

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
gui_mch_font_dialog(font_family* family, font_style* style, عائم* ناپ)
{
#إذا defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = جديد VimSelectFontDialog(family, style, ناپ);
    منطقي ret = dialog->Go();
	مٹاؤ dialog;
	أرجع ret;
#وإلا
    أرجع NOFONT;
#جے_انت // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	صحيح	    giveErrorIfMissing)
{
    ساكن VimFont *fontList = عدم;

    إذا (!gui.وچ_use)	//  can't do this when GUI not running
	أرجع NOFONT;

    //	storage for locally modified name;
    ثابت صحيح buff_ناپ = B_FONT_FAMILY_لمائی + B_FONT_STYLE_لمائی + 20;
    ساكن حرف font_name[buff_ناپ] = {0};
    font_family family = {0};
    font_style	style  = {0};
    عائم ناپ = 0.f;

    إذا (name == 0 && be_fixed_font == 0) {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	أرجع NOFONT;
    }

    منطقي useSelectGUI = خطأ;
    إذا (name != عدم)
	إذا (STRCMP(name, "*") == 0) {
	    useSelectGUI = سچ;
	    STRNCPY(font_name, hl_get_font_name(), buff_ناپ);
	} وإلا
	    STRNCPY(font_name, name, buff_ناپ);

    إذا (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	ناپ = be_fixed_font->ناپ_2();
	vim_snprintf(font_name, buff_ناپ,
	    (حرف*)"%s/%s/%.0f", family, style, ناپ);
    }

    //	replace underscores with spaces
    حرف* سرا = 0;
    طالما (سرا = strchr((حرف *)font_name, '_'))
	*سرا = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    ساكن حرف buff[buff_ناپ] = {0};
    STRNCPY(buff, font_name, buff_ناپ);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_لمائی);
    حرف* style_s = strtok(0, "/\0");
    إذا (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_لمائی);
    ناپ = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    إذا (useSelectGUI) {
	إذا (gui_mch_font_dialog(&family, &style, &ناپ) == NOFONT)
	    أرجع FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_ناپ,
		(حرف*)"%s/%s/%.0f", family, style, ناپ);
	hl_سیٹ_font_name((char_u*)font_name);

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
    لكل (flp = fontList; flp; flp = flp->اگلا) {
	إذا (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    أرجع (GuiFont)flp;
	}
    }

    VimFont *font = جديد VimFont();
    font->name = vim_strsave((char_u*)font_name);

    إذا (گنتی_font_styles(family) <= 0) {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	مٹاؤ font;
	أرجع NOFONT;
    }

    //	Remember font in the static list for later use
    font->اگلا = fontList;
    fontList = font;

    font->سیٹ_Family_And_Style(family, style);
    إذا (ناپ > 0.f)
	font->سیٹ_ناپ(ناپ);

    font->سیٹ_Spacing(B_FIXED_SPACING);
    font->سیٹ_Encoding(B_UNICODE_UTF8);

    أرجع (GuiFont)font;
}

/*
 * Set the current text font.
 */
فراغ
gui_mch_سیٹ_font(
	GuiFont	font)
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	VimFont *vf = (VimFont *)font;

	gui.vim_متن_Area->سیٹ_Font(vf);

	gui.char_چوڑائی = (صحيح) vf->String_چوڑائی("n");
	font_اچائی fh;
	vf->Get_اچائی(&fh);
	gui.char_اچائی = (صحيح)(fh.ascent + 0.9999)
	    + (صحيح)(fh.descent + 0.9999) + (صحيح)(fh.leading + 0.9999);
	gui.char_ascent = (صحيح)(fh.ascent + 0.9999);

	gui.vim_کھڑکی->Unlock();
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
	مٹاؤ f;
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
	font_اچائی fh = {0};
	font->Get_اچائی(&fh);
	gui.char_اچائی = (صحيح)(fh.ascent + fh.descent + 0.5) + p_linespace;
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
gui_mch_سیٹ_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_منزل_RGB(color);
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_متن_Area->سیٹ_بلند_Color(rgb);
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Set the current text background color.
 */
فراغ
gui_mch_سیٹ_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_منزل_RGB(color);
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_متن_Area->سیٹ_کم_Color(rgb);
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    فراغ
gui_mch_سیٹ_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

فراغ
gui_mch_draw_string(
	صحيح	صف,
	صحيح	خانہ,
	char_u	*s,
	صحيح	len,
	صحيح	flags)
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_متن_Area->mchDrawString(صف, خانہ, s, len, flags);
	gui.vim_کھڑکی->Unlock();
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

    لكل (i = 0; special_کلیدیں[i].Be_کلیدیں != 0; i++)
	إذا (name[0] == special_کلیدیں[i].vim_code0 &&
		name[1] == special_کلیدیں[i].vim_code1)
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

    إذا (gui.vim_کھڑکی->جندرا_2()) {
	BRect rect = gui.vim_متن_Area->Bounds();

	gui.vim_متن_Area->سیٹ_Drawing_منوال(B_OP_INVERT);
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->Sync();
	snooze(msec * 1000);	 // wait for a few msec
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->سیٹ_Drawing_منوال(B_OP_COPY);
	gui.vim_متن_Area->Flush();
	gui.vim_کھڑکی->Unlock();
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
    rect.کھبے = FILL_X(c);
    rect.چوٹی = FILL_Y(r);
    rect.سجے = rect.کھبے + nc * gui.char_چوڑائی - PEN_چوڑائی;
    rect.bottom = rect.چوٹی + nr * gui.char_اچائی - PEN_چوڑائی;

    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_متن_Area->سیٹ_Drawing_منوال(B_OP_INVERT);
	gui.vim_متن_Area->FillRect(rect);
	gui.vim_متن_Area->سیٹ_Drawing_منوال(B_OP_COPY);
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify()
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_کھڑکی->Minimize(سچ);
	gui.vim_کھڑکی->Unlock();
    }
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_سیٹ_foreground(فراغ)
{
    // TODO
}
#جے_انت

/*
 * Set the window title
 */
فراغ
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_کھڑکی->سیٹ_Title((حرف *)title);
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_سیٹ_fg_color(color);

    BRect r;
    r.کھبے = FILL_X(gui.خانہ);
    r.چوٹی = FILL_Y(gui.صف);
    صحيح cells = utf_off2cells(سطر_وتھ[gui.صف] + gui.خانہ, 100); // TODO-TODO
    إذا (cells>=4) cells = 1;
    r.سجے = r.کھبے + cells*gui.char_چوڑائی - PEN_چوڑائی;
    r.bottom = r.چوٹی + gui.char_اچائی - PEN_چوڑائی;

    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_متن_Area->StrokeRect(r);
	gui.vim_کھڑکی->Unlock();
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
    gui_mch_سیٹ_fg_color(color);

    BRect r;
    r.کھبے =
#جے_تعریف_شدہ FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_دایاں ? FILL_X(gui.خانہ + 1) - w :
#جے_انت
	FILL_X(gui.خانہ);
    r.سجے = r.کھبے + w - PEN_چوڑائی;
    r.bottom = FILL_Y(gui.صف + 1) - PEN_چوڑائی;
    r.چوٹی = r.bottom - h + PEN_چوڑائی;

    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_متن_Area->FillRect(r);
	gui.vim_کھڑکی->Unlock();
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
    طالما (پورٹ_گنتی(gui.vdcmp) > 0 &&
	    !vim_is_داخلہ_buf_بھریا() &&
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
	لما	wtime)
{
    صحيح		focus;
    bigtime_t	until, timeout;
    حالت_t	st;

    إذا (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_ویلا() + timeout;
    }
    وإلا
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.وچ_focus;
    لكل (;;)
    {
	// Stop or start blinking when focus changes
	إذا (gui.وچ_focus != focus)
	{
	    إذا (gui.وچ_focus)
		gui_mch_شروع_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.وچ_focus;
	}

	gui_mch_flush();

#جے_تعریف_شدہ سنیہا_قطار
# ifdef FEAT_TIMERS
	did_جوڑو_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_جوڑو_timer)
	    // Need to recompute the waiting time.
	    اكسر;
# endif
# ifdef FEAT_کم_CHANNEL
	إذا (has_any_channel())
	{
	    إذا (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	وإلا إذا (wtime < 0)
	    timeout = B_INFINITE_TIMEOUT;
# endif
#جے_انت

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use gui_haiku_process_event() to hang until
	 * an event arrives.  No need to check for input_buf_full because we
	 * are returning as soon as it contains a single char.
	 */
	st = gui_haiku_process_event(timeout);

	إذا (داخلہ_دستیاب())
	    أرجع OK;
	إذا (st < B_OK)		// includes B_TIMED_OUT
	    أرجع FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	إذا (wtime >= 0)
	{
	    timeout = until - system_ویلا();
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
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_کھڑکی->Flush();
	gui.vim_کھڑکی->Unlock();
    }
    أرجع;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
فراغ
gui_mch_clear_بلاک(
	صحيح	row1,
	صحيح	col1,
	صحيح	row2,
	صحيح	col2)
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_متن_Area->mch_Clear_بلاک(row1, col1, row2, col2);
	gui.vim_کھڑکی->Unlock();
    }
}

    فراغ
gui_mch_clear_all()
{
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	gui.vim_متن_Area->mchClearAll();
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
فراغ
gui_mch_delete_سطریں(
	صحيح	صف,
	صحيح	عدد_سطریں)
{
    gui.vim_متن_Area->mch_Delete_سطریں(صف, عدد_سطریں);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
فراغ
gui_mch_داخل_کرو_سطریں(
	صحيح	صف,
	صحيح	عدد_سطریں)
{
    gui.vim_متن_Area->mch_داخل_کرو_سطریں(صف, عدد_سطریں);
}

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

فراغ
gui_mch_enable_menu(
	صحيح	flag)
{
    إذا (gui.vim_کھڑکی->جندرا_2())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->سیٹ_Enabled(flag);
	gui.vim_کھڑکی->Unlock();
    }
}

فراغ
gui_mch_سیٹ_menu_pos(
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
gui_mch_جوڑو_menu(
	vimmenu_T   *menu,
	صحيح	idx)
{
    vimmenu_T	*ماپا = menu->ماپا;

    //	popup menu - just create it unattached
    إذا (menu_is_popup(menu->name) && ماپا == عدم) {
	BPop_اتے_Menu* نکالو_اتے_Menu = جديد BPop_اتے_Menu((ثابت حرف*)menu->name, خطأ, خطأ);
	menu->submenu_id = نکالو_اتے_Menu;
	menu->id = عدم;
	أرجع;
    }

    إذا (!menu_is_menubar(menu->name)
	    || (ماپا != عدم && ماپا->submenu_id == عدم))
	أرجع;

    إذا (gui.vim_کھڑکی->جندرا_2())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	إذا ( ماپا )
	    tmp = ماپا->submenu_id;
	وإلا
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	إذا ( ! tmp->لبھو_شے((ثابت حرف *) menu->dname)) {

	    BMenu *bmenu = جديد BMenu((حرف *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->جوڑو_شے(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->لبھو_شے((ثابت حرف *) menu->dname);

	}
	gui.vim_کھڑکی->Unlock();
    }
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
    // no tearoff menus
}

    ساكن BMessage *
Menu_سنیہا(vimmenu_T *menu)
{
    BMessage *m = جديد BMessage('menu');
    m->جوڑو_اشارہ_گر("VimMenu", (فراغ *)menu);

    أرجع m;
}

/*
 * Add a menu item to a menu
 */
فراغ
gui_mch_جوڑو_menu_شے(
	vimmenu_T   *menu,
	صحيح	idx)
{
    صحيح	    mnemonic = 0;
    vimmenu_T	*ماپا = menu->ماپا;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    إذا (gui.vim_کھڑکی->جندرا_2())
    {
#جے_تعریف_شدہ FEAT_TOOLBAR
	إذا (menu_is_toolbar(ماپا->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->جوڑو_Button(idx, menu);
	    }
	} وإلا
#جے_انت

	إذا (ماپا->submenu_id != عدم || menu_is_popup(ماپا->name)) {
	    إذا (menu_is_separator(menu->name)) {
		BSeparator_شے *شے = جديد BSeparator_شے();
		ماپا->submenu_id->جوڑو_شے(شے);
		menu->id = شے;
		menu->submenu_id = عدم;
	    }
	    وإلا {
		BMenu_شے *شے = جديد BMenu_شے((حرف *)menu->dname,
			Menu_سنیہا(menu));
		شے->سیٹ_نشانہ(gui.vim_متن_Area);
		شے->سیٹ_Trigger((حرف) menu->mnemonic);
		ماپا->submenu_id->جوڑو_شے(شے);
		menu->id = شے;
		menu->submenu_id = عدم;
	    }
	}
	gui.vim_کھڑکی->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
فراغ
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    إذا (gui.vim_کھڑکی->جندرا_2())
    {
#جے_تعریف_شدہ FEAT_TOOLBAR
	إذا (menu->ماپا && menu_is_toolbar(menu->ماپا->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->RemoveButton(menu);
	    }
	} وإلا
#جے_انت
	{
	    أكد(menu->submenu_id == عدم || menu->submenu_id->گنتی_اشیاء() == 0);
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
		bmenu->Remove_شے(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		إذا (bmenu == gui.vimForm->MenuBar() && bmenu->گنتی_اشیاء() == 0)
		{
		    bmenu->Resize_منزل(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    مٹاؤ menu->id;
	    menu->id = عدم;
	    menu->submenu_id = عدم;

	    gui.menu_اچائی = (صحيح) gui.vimForm->Menu_اچائی();
	}
	gui.vim_کھڑکی->Unlock();
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
#جے_تعریف_شدہ FEAT_TOOLBAR
    إذا (menu->ماپا && menu_is_toolbar(menu->ماپا->name)) {
	إذا (gui.vim_کھڑکی->جندرا_2()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    إذا (toolbar != عدم) {
		toolbar->سلیٹی_Button(menu, grey);
	    }
	    gui.vim_کھڑکی->Unlock();
	}
    } وإلا
#جے_انت
    إذا (menu->id != عدم)
	menu->id->سیٹ_Enabled(!grey);
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
	menu->id->سیٹ_Enabled(!hidden);
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

    BPop_اتے_Menu* popupMenu = dynamic_cast<BPop_اتے_Menu*>(menu->submenu_id);
    إذا (popupMenu == عدم)
	أرجع;

    BPoint نقطہ;
    إذا (gui.vim_کھڑکی->جندرا_2()) {
	uint32 buttons = 0;
	gui.vim_متن_Area->GetMouse(&نقطہ, &buttons);
	gui.vim_متن_Area->Convert_منزل_Screen(&نقطہ);
	gui.vim_کھڑکی->Unlock();
    }
    popupMenu->Go(نقطہ, سچ);
}

#جے_انت // FEAT_MENU

// Mouse stuff

#جے_تعریف_شدہ FEAT_CLIPBOARD
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
    إذا (be_clipboard->جندرا_2())
    {
	BMessage *m = be_clipboard->انکڑے_2();
	// m->PrintToStream();

	char_u *string = عدم;
	ssize_t stringlen = -1;

	إذا (m->لبھو_انکڑے(textplain, B_MIME_TYPE,
		    (ثابت فراغ **)&string, &stringlen) == B_OK
		|| m->لبھو_String("text", (ثابت حرف **)&string) == B_OK)
	{
	    إذا (stringlen == -1)
		stringlen = STRLEN(string);

	    صحيح type;
	    حرف *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    إذا (m->لبھو_انکڑے(vimselectiontype, B_MIME_TYPE,
			(ثابت فراغ **)&seltype, &seltypelen) == B_OK)
	    {
		بدل (*seltype)
		{
		    افتراضي:
		    حالة 'L':	type = MLINE;	اكسر;
		    حالة 'C':	type = MCHAR;	اكسر;
#جے_تعریف_شدہ FEAT_VISUAL
		    حالة 'B':	type = MBLOCK;	اكسر;
#جے_انت
		}
	    }
	    وإلا
	    {
		// Otherwise use heuristic as documented
		type = memchr(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (لما)stringlen, cbd);
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
clip_mch_سیٹ_selection(Clipboard_T *cbd)
{
    إذا (be_clipboard->جندرا_2())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->انکڑے_2();
	أكد(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = عدم;
	long_u	گنتی;
	صحيح type;

	type = clip_convert_selection(&str, &گنتی, cbd);

	إذا (type < 0)
	    أرجع;

	m->جوڑو_انکڑے(textplain, B_MIME_TYPE, (فراغ *)str, گنتی);

	// Add type of selection
	حرف	vtype;
	بدل (type)
	{
	    افتراضي:
	    حالة MLINE:    vtype = 'L';    اكسر;
	    حالة MCHAR:    vtype = 'C';    اكسر;
#جے_تعریف_شدہ FEAT_VISUAL
	    حالة MBLOCK:   vtype = 'B';    اكسر;
#جے_انت
	}
	m->جوڑو_انکڑے(vimselectiontype, B_MIME_TYPE, (فراغ *)&vtype, 1);

	vim_free(str);

	be_clipboard->Commit();
	be_clipboard->Unlock();
    }
}

#جے_انت	// FEAT_CLIPBOARD

#جے_تعریف_شدہ FEAT_BROWSE
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

    gui.vimApp->f_Browsed_رستہ.Unset();

    gui.vimApp->fFilePanel->کھڑکی()->سیٹ_Title((حرف*)title);
    gui.vimApp->fFilePanel->سیٹ_Panel_Directory((ثابت حرف*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    طالما (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = عدم;
    حالت_t نتیجہ = gui.vimApp->f_Browsed_رستہ.InitCheck();
    إذا (نتیجہ == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->f_Browsed_رستہ.رستہ_2());
    } وإلا
	إذا (نتیجہ != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    نتیجہ, strerror(نتیجہ));
	}

    مٹاؤ gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = عدم;

    أرجع fileName;
}
#جے_انت // FEAT_BROWSE


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
	char_u	*سنیہا,
	char_u	*buttons,
	صحيح	 dfltbutton,
	char_u	*textfield,
	صحيح ex_cmd)
{
    VimDialog *dialog = جديد VimDialog(type, (حرف*)title, (حرف*)سنیہا,
	    (حرف*)buttons, dfltbutton, (حرف*)textfield, ex_cmd);
    منطقي ret = dialog->Go();
    مٹاؤ dialog;
	أرجع ret;
}

#جے_انت // FEAT_GUI_DIALOG


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

#جے_تعریف_شدہ FEAT_MBYTE_IME
    فراغ
im_سیٹ_position(صحيح صف, صحيح خانہ)
{
    إذا (gui.vim_کھڑکی->جندرا_2())
    {
	gui.vim_متن_Area->DrawIMString();
	gui.vim_کھڑکی->Unlock();
    }
    أرجع;
}
#جے_انت

    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_اچائی = (toolbar && showit) ? toolbar->Toolbar_اچائی() : 0.;
}

    فراغ
gui_mch_سیٹ_toolbar_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    إذا (toolbar != عدم) {
	إذا (gui.vim_کھڑکی->جندرا_2()) {
	    toolbar->Move_منزل(x, y);
	    toolbar->Resize_منزل(w - 1, h - 1);
	    gui.vim_کھڑکی->Unlock();
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
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();

    إذا (tab_سطر == عدم)
	أرجع;

    إذا (!showit != !gui.vimForm->Is_Showing_Tab_سطر()) {
	gui.vimForm->سیٹ_Showing_Tab_سطر(showit != 0);
	gui.tabline_اچائی = gui.vimForm->Tabline_اچائی();
    }
}

    فراغ
gui_mch_سیٹ_tabline_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    إذا (tab_سطر != عدم) {
	إذا (gui.vim_کھڑکی->جندرا_2()) {
	    tab_سطر->Move_منزل(x, y);
	    tab_سطر->Resize_منزل(w - 1, h - 1);
	    gui.vim_کھڑکی->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحيح
gui_mch_showing_tabline()
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    أرجع tab_سطر != عدم && gui.vimForm->Is_Showing_Tab_سطر();
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

    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();

    إذا (tab_سطر == عدم)
	أرجع;

    gui.vim_کھڑکی->جندرا_2();

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = پہلا_tabpage; tp != عدم; tp = tp->tp_اگلا, ++nr) {
	إذا (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_سطر->TabAt(nr);

	إذا (tab == عدم) {
	    tab = جديد Vim_Tab_سطر::VimTab();
	    tab_سطر->جوڑو_Tab(عدم, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->سیٹ_Label((ثابت حرف*)NameBuff);
	tab_سطر->Invalidate();
    }

    // Remove any old labels.
    طالما (nr < tab_سطر->گنتی_Tabs())
	tab_سطر->RemoveTab(nr);

    إذا (tab_سطر->Selection() != curtabidx)
	tab_سطر->Select(curtabidx);

    gui.vim_کھڑکی->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_سیٹ_curtab(صحيح nr)
{
    Vim_Tab_سطر *tab_سطر = gui.vimForm->Tab_سطر();
    إذا (tab_سطر == عدم)
	أرجع;

    gui.vim_کھڑکی->جندرا_2();

    إذا (tab_سطر->Selection() != nr -1)
	tab_سطر->Select(nr -1);

    gui.vim_کھڑکی->Unlock();
}

#جے_انت // FEAT_GUI_TABLINE
