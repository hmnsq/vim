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
वर्ग Vim_Tab_पङ्क्तिः;

बाह्य कुञ्चिका_map *कुञ्चिका_Map;
बाह्य वर्ण *कुञ्चिका_Map_Chars;

बाह्य पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण **argv);

#ifndef B_MAX_PORT_गणनम्
#define B_MAX_PORT_गणनम्    255
#endif

// VimApp seems comparable to the X "vimShell"
वर्ग VimApp: सार्वजनिक BApplication
{
    प्रकारनिर्देश BApplication Inherited;
    सार्वजनिक:
    VimApp(ध्रुव वर्ण *appsig);
    ~VimApp();

    // callbacks:
#जइ 0
    आभास शून्य DispatchMessage(BMessage *m, BHandler *h)
    {
	m->PrintToStream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    आभास शून्य ReadyToRun();
    आभास शून्य ArgvReceived(int32 argc, वर्ण **argv);
    आभास शून्य RefsReceived(BMessage *m);
    आभास तार्किक QuitRequested();
    आभास शून्य MessageReceived(BMessage *m);

    स्थिर शून्य SendRefs(BMessage *m, तार्किक changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    गुप्त:
};

वर्ग VimWindow: सार्वजनिक BWindow
{
    प्रकारनिर्देश BWindow Inherited;
    सार्वजनिक:
    VimWindow();
    ~VimWindow();

    आभास शून्य DispatchMessage(BMessage *m, BHandler *h);
    आभास शून्य WindowActivated(तार्किक active);
    आभास तार्किक QuitRequested();

    VimFormView	    *formView;

    गुप्त:
    शून्य init();

    तार्किक is_fullscreen = मिच्छ;
    BRect saved_frame;
    window_look saved_look;
};

वर्ग VimFormView: सार्वजनिक BView
{
    प्रकारनिर्देश BView Inherited;
    सार्वजनिक:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    आभास शून्य AllAttached();
    आभास शून्य FrameResized(भिन्न new_width, भिन्न new_height);

#define MENUBAR_MARGIN	1
    भिन्न MenuHeight() ध्रुव
    { प्रत्यागम menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() ध्रुव
    { प्रत्यागम menuBar; }

    गुप्त:
    शून्य init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    सार्वजनिक:
    भिन्न ToolbarHeight() ध्रुव;
    VimToolbar *ToolBar() ध्रुव
    { प्रत्यागम toolBar; }
    गुप्त:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    सार्वजनिक:
    Vim_Tab_पङ्क्तिः *Tab_पङ्क्तिः() ध्रुव	{ प्रत्यागम tab_पङ्क्तिः; }
    तार्किक Is_Showing_Tab_पङ्क्तिः() ध्रुव { प्रत्यागम showing_Tab_पङ्क्तिः; }
    शून्य Set_Showing_Tab_पङ्क्तिः(तार्किक showing) { showing_Tab_पङ्क्तिः = showing;	}
    भिन्न TablineHeight() ध्रुव;
    गुप्त:
    Vim_Tab_पङ्क्तिः	*tab_पङ्क्तिः;
    पूर्णाङ्क	showing_Tab_पङ्क्तिः;
#endif
};

वर्ग VimTextAreaView: सार्वजनिक BView
{
    प्रकारनिर्देश BView Inherited;
    सार्वजनिक:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    आभास शून्य Draw(BRect updateRect);
    आभास शून्य कुञ्चिका_Down(ध्रुव वर्ण *bytes, int32 अङ्कः_Bytes);
    आभास शून्य MouseDown(BPoint point);
    आभास शून्य MouseUp(BPoint point);
    आभास शून्य MouseMoved(BPoint point, uint32 transit, ध्रुव BMessage *message);
    आभास शून्य MessageReceived(BMessage *m);

    // own functions:
    पूर्णाङ्क mchInitFont(char_u *name);
    शून्य mchDrawString(पूर्णाङ्क row, पूर्णाङ्क col, char_u *s, पूर्णाङ्क len, पूर्णाङ्क flags);
    शून्य mchClearBlock(पूर्णाङ्क row1, पूर्णाङ्क col1, पूर्णाङ्क row2, पूर्णाङ्क col2);
    शून्य mchClearAll();
    शून्य mch_Delete_पङ्क्तयः(पूर्णाङ्क row, पूर्णाङ्क अङ्कः_पङ्क्तयः);
    शून्य mch_निवेशय_पङ्क्तयः(पूर्णाङ्क row, पूर्णाङ्क अङ्कः_पङ्क्तयः);

    स्थिर शून्य guiSendMouseEvent(पूर्णाङ्क button, पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क repeated_click, int_u modifiers);
    स्थिर शून्य guiMouseMoved(पूर्णाङ्क x, पूर्णाङ्क y);
    स्थिर शून्य guiBlankMouse(तार्किक should_hide);
    स्थिर int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouse_Drag_Event_गणनम्;

#ifdef FEAT_MBYTE_IME
    शून्य DrawIMString(शून्य);
#endif

    गुप्त:
    शून्य init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    संरचना {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	पूर्णाङ्क row;
	पूर्णाङ्क col;
	पूर्णाङ्क गणनम्;
    } IMData;
#endif
};

वर्ग VimScrollBar: सार्वजनिक BScrollBar
{
    प्रकारनिर्देश BScrollBar Inherited;
    सार्वजनिक:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    आभास शून्य मानम्_Changed(भिन्न new_मानम्);
    आभास शून्य MouseUp(BPoint where);
    शून्य Set_मानम्(भिन्न newval);
    scrollbar_T *getGsb()
    { प्रत्यागम gsb; }

    int32	scroll_Event_गणनम्;

    गुप्त:
    scrollbar_T *gsb;
    भिन्न   ignore_मानम्;
};


#ifdef FEAT_TOOLBAR

वर्ग VimToolbar : सार्वजनिक BBox
{
    स्थिर BBitmap *normalButtonsBitmap;
    स्थिर BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(ध्रुव वर्ण* fileName);
    तार्किक GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, तार्किक pressed);
    तार्किक ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    शून्य InvalidateLayout();

    सार्वजनिक:
    VimToolbar(BRect frame, ध्रुव वर्ण * name);
    ~VimToolbar();

    तार्किक PrepareButtonBitmaps();

    तार्किक AddButton(int32 index, vimmenu_T *menu);
    तार्किक RemoveButton(vimmenu_T *menu);
    तार्किक GrayButton(vimmenu_T *menu, पूर्णाङ्क grey);

    भिन्न ToolbarHeight() ध्रुव;
    आभास शून्य AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = रिक्त;
BBitmap *VimToolbar::grayedButtonsBitmap  = रिक्त;

ध्रुव भिन्न ToolbarMargin = 3.;
ध्रुव भिन्न ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

वर्ग Vim_Tab_पङ्क्तिः : सार्वजनिक BTabView
{
    सार्वजनिक:
	वर्ग VimTab : सार्वजनिक BTab {
	    सार्वजनिक:
		VimTab() : BTab(नूतन BView(BRect(), "-Empty-", 0, 0)) {}

	    आभास शून्य Select(BView* owner);
	};

	Vim_Tab_पङ्क्तिः(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_वामः | B_FOLLOW_शिखरम् | B_FOLLOW_दक्षिणः, B_WILL_DRAW | B_FRAME_EVENTS) {}

    भिन्न TablineHeight() ध्रुव;
    आभास शून्य MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
वर्ग VimFont: सार्वजनिक BFont
{
    प्रकारनिर्देश BFont Inherited;
    सार्वजनिक:
    VimFont();
    VimFont(ध्रुव VimFont *rhs);
    VimFont(ध्रुव BFont *rhs);
    VimFont(ध्रुव VimFont &rhs);
    ~VimFont();

    VimFont *next;
    पूर्णाङ्क refcount;
    char_u *name;

    गुप्त:
    शून्य init();
};

#जइ defined(FEAT_GUI_DIALOG)

वर्ग VimDialog : सार्वजनिक BWindow
{
    प्रकारनिर्देश BWindow Inherited;

    BButton* _CreateButton(int32 which, ध्रुव वर्ण* label);

    सार्वजनिक:

    वर्ग View : सार्वजनिक BView {
	प्रकारनिर्देश BView Inherited;

	सार्वजनिक:
	View(BRect frame);
	~View();

	आभास शून्य Draw(BRect updateRect);
	शून्य InitIcon(int32 type);

	गुप्त:
	BBitmap*    fIconBitmap;
    };

    VimDialog(पूर्णाङ्क type, ध्रुव वर्ण *title, ध्रुव वर्ण *message,
	    ध्रुव वर्ण *buttons, पूर्णाङ्क dfltbutton, ध्रुव वर्ण *textfield,
	    पूर्णाङ्क ex_cmd);
    ~VimDialog();

    पूर्णाङ्क Go();

    आभास शून्य MessageReceived(BMessage *सन्देशः);

    गुप्त:
    sem_id	    fDialogSem;
    पूर्णाङ्क		    f_Dialog_मानम्;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   f_निवेशः_Control;
    ध्रुव वर्ण*	    f_निवेशः_मानम्;
};

वर्ग VimSelectFontDialog : सार्वजनिक BWindow
{
    प्रकारनिर्देश BWindow Inherited;

    शून्य _CleanList(BListView* list);
    शून्य _UpdateFontStyles();
    शून्य Update_Size_निवेशः_Preview();
    शून्य _UpdateFontPreview();
    तार्किक _UpdateFromListItem(BListView* list, वर्ण* text, पूर्णाङ्क textSize);
    सार्वजनिक:

    VimSelectFontDialog(font_family* family, font_style* style, भिन्न* size);
    ~VimSelectFontDialog();

    तार्किक Go();

    आभास शून्य MessageReceived(BMessage *सन्देशः);

    गुप्त:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    तार्किक	    f_Dialog_मानम्;
    font_family*    fFamily;
    font_style*	    fStyle;
    भिन्न*	    fSize;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    भिन्न	    fFontSize;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_निवेशः;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

संरचना MainArgs {
    पूर्णाङ्क	     argc;
    वर्ण    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	कुञ्चिका_सन्देशः_BUFSIZ	7
#जइ कुञ्चिका_सन्देशः_BUFSIZ < MAX_कुञ्चिका_CODE_LEN
#error Increase कुञ्चिका_सन्देशः_BUFSIZ!
#endif

संरचना Vim_कुञ्चिका_सन्देशः {
    char_u  length;
    char_u  chars[कुञ्चिका_सन्देशः_BUFSIZ];  // contains Vim encoding
    तार्किक    csi_escape;
};

संरचना Vim_Resize_सन्देशः {
    पूर्णाङ्क	    width;
    पूर्णाङ्क	    height;
};

संरचना Vim_Scroll_Bar_सन्देशः {
    VimScrollBar *sb;
    long    मानम्;
    पूर्णाङ्क	    stillDragging;
};

संरचना Vim_Menu_सन्देशः {
    vimmenu_T	*guiMenu;
};

संरचना Vim_Mouse_सन्देशः {
    पूर्णाङ्क	    button;
    पूर्णाङ्क	    x;
    पूर्णाङ्क	    y;
    पूर्णाङ्क	    repeated_click;
    int_u   modifiers;
};

संरचना Vim_Mouse_Moved_सन्देशः {
    पूर्णाङ्क	    x;
    पूर्णाङ्क	    y;
};

संरचना Vim_Focus_सन्देशः {
    तार्किक    active;
};

संरचना Vim_Refs_सन्देशः {
    BMessage   *message;
    तार्किक    changedir;
};

संरचना Vim_Tabline_सन्देशः {
    पूर्णाङ्क	    index;
};

संरचना Vim_Tabline_Menu_सन्देशः {
    पूर्णाङ्क	    index;
    पूर्णाङ्क	    event;
};

संरचना Vim_सन्देशः {
    गणना Vim_सन्देशः_Type {
	कुञ्चिका_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    सङ्घ {
	संरचना Vim_कुञ्चिका_सन्देशः    कुञ्चिका_2;
	संरचना Vim_Resize_सन्देशः NewSize;
	संरचना Vim_Scroll_Bar_सन्देशः	Scroll;
	संरचना Vim_Menu_सन्देशः   Menu;
	संरचना Vim_Mouse_सन्देशः  Mouse;
	संरचना Vim_Mouse_Moved_सन्देशः	MouseMoved;
	संरचना Vim_Focus_सन्देशः  Focus;
	संरचना Vim_Refs_सन्देशः   Refs;
	संरचना Vim_Tabline_सन्देशः	Tabline;
	संरचना Vim_Tabline_Menu_सन्देशः    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

स्थिर संरचना specialkey
{
    uint16  Be_कुञ्चिकाः;
#define कुञ्चिका_3(a,b)    ((a)<<8|(b))
#define K(a)	    कुञ्चिका_3(0,a)		// for ASCII codes
#define F(b)	    कुञ्चिका_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_कुञ्चिकाः[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_वामः_ARROW),	    'k', 'l'},
    {K(B_दक्षिणः_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_निवेशय),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_कुञ्चिका  11
    {F(B_F1_कुञ्चिका),	'k', '1'},
    {F(B_F2_कुञ्चिका),	'k', '2'},
    {F(B_F3_कुञ्चिका),	'k', '3'},
    {F(B_F4_कुञ्चिका),	'k', '4'},
    {F(B_F5_कुञ्चिका),	'k', '5'},
    {F(B_F6_कुञ्चिका),	'k', '6'},
    {F(B_F7_कुञ्चिका),	'k', '7'},
    {F(B_F8_कुञ्चिका),	'k', '8'},
    {F(B_F9_कुञ्चिका),	'k', '9'},
    {F(B_F10_कुञ्चिका),	'k', ';'},

    {F(B_F11_कुञ्चिका),	'F', '1'},
    {F(B_F12_कुञ्चिका),	'F', '2'},
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
    {F(B_PRINT_कुञ्चिका),	    '%', '9'},

#जइ 0
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

#define अङ्कः_SPECIAL_कुञ्चिकाः    ARRAY_LENGTH(special_कुञ्चिकाः)

// ---------------- VimApp ----------------

    स्थिर शून्य
docd(BPath &path)
{
    mch_chdir((वर्ण *)path.Path());
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
    int32 गणनम्;

    m->PrintToStream();
    विकल्प (m->what) {
	पक्ष B_REFS_RECEIVED:
	पक्ष B_SIMPLE_DATA:
	    m->GetInfo("refs", &type, &गणनम्);
	    जइ (type != B_REF_TYPE)
		गच्छ bad;
	    भङ्ग;
	पक्ष B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &गणनम्);
	    जइ (type != B_STRING_TYPE)
		गच्छ bad;
	    जइ (changedir) {
		वर्ण *dirname;
		जइ (m->FindString("cwd", (ध्रुव वर्ण **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    भङ्ग;
	स्वभाव:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    प्रत्यागम;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(गणनम् * परिमाण(char_u *));
    पूर्णाङ्क fname_index = 0;

    विकल्प (m->what) {
	पक्ष B_REFS_RECEIVED:
	पक्ष B_SIMPLE_DATA:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    प्रति (पूर्णाङ्क i = 0; i < गणनम्; ++i)
	    {
		entry_ref ref;
		जइ (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, मिच्छ);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    जइ (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    जइ (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			जइ (!changedir)
			    docd(path);
		    } अण्णहा {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			जइ (fname == रिक्त)
			    fname = (char_u *)path.Path();
			fnames[fname_index++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = मिच्छ;
		}
	    }
	    भङ्ग;
	पक्ष B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    प्रति (पूर्णाङ्क i = 1; i < गणनम्; ++i)
	    {
		वर्ण *fname;

		जइ (m->FindString("argv", i, (ध्रुव वर्ण **) &fname) == B_OK) {
		    fnames[fname_index++] = vim_strsave((char_u *)fname);
		}
	    }
	    भङ्ग;
	स्वभाव:
	    // fprintf(stderr, "case default\n");
	    भङ्ग;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    जइ (fname_index > 0) {
	handle_drop(fname_index, fnames, FALSE, drop_callback, रिक्त);

	setcursor();
	out_flush();
    } अण्णहा {
	vim_free(fnames);
    }
}

VimApp::VimApp(ध्रुव वर्ण *appsig):
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
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    शून्य
VimApp::ArgvReceived(int32 arg_argc, वर्ण **arg_argv)
{
    जइ (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	जइ (gui.vimWindow)
	    gui.vimWindow->Minimize(मिच्छ);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, सच्च);
    }
}

    शून्य
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    पूर्णाङ्क limit = 15;
    जाव (--limit >= 0 && (curbuf == रिक्त || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    जइ (gui.vimWindow)
	gui.vimWindow->Minimize(मिच्छ);
    DetachCurrentMessage();
    SendRefs(m, सच्च);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    शून्य
VimApp::SendRefs(BMessage *m, तार्किक changedir)
{
    Vim_Refs_सन्देशः rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, Vim_सन्देशः::Refs, &rm, परिमाण(rm));
    //	calls ::RefsReceived
}

    शून्य
VimApp::MessageReceived(BMessage *m)
{
    विकल्प (m->what) {
	पक्ष 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    भङ्ग;
	पक्ष 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    भङ्ग;
	पक्ष B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (शून्य**)&panel);
		जइ (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    भङ्ग;
	स्वभाव:
	    Inherited::MessageReceived(m);
	    भङ्ग;
    }
}

    तार्किक
VimApp::QuitRequested()
{
    (शून्य)Inherited::QuitRequested();
    प्रत्यागम मिच्छ;
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
    जइ (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = रिक्त;
}

    शून्य
VimWindow::init()
{
    // Attach the VimFormView
    formView = नूतन VimFormView(Bounds());
    जइ (formView != रिक्त) {
	AddChild(formView);
    }
}

#जइ 0  //  disabled in zeta patch
    शून्य
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    विकल्प (m->what) {
	पक्ष B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		जइ (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} अण्णहा {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    भङ्ग;
	स्वभाव:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    शून्य
VimWindow::WindowActivated(तार्किक active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    जइ (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(सच्च);

    संरचना Vim_Focus_सन्देशः fm;
    fm.active = active;

    write_port(gui.vdcmp, Vim_सन्देशः::Focus, &fm, परिमाण(fm));
}

    तार्किक
VimWindow::QuitRequested()
{
    संरचना Vim_कुञ्चिका_सन्देशः km;
    km.length = 5;
    memcpy((वर्ण *)km.chars, "\033:qa\r", km.length);
    km.csi_escape = मिच्छ;
    write_port(gui.vdcmp, Vim_सन्देशः::कुञ्चिका_2, &km, परिमाण(km));
    प्रत्यागम मिच्छ;
}
    शून्य
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    तार्किक should_propagate = सच्च;

    विकल्प (m->what)
    {
	पक्ष B_कुञ्चिका_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		जइ (scancode == B_ENTER && (beModifiers & B_वामः_COMMAND_कुञ्चिका))
		    {
			should_propagate = मिच्छ;
			जइ (एतत्->is_fullscreen)
			    {
				एतत्->is_fullscreen = मिच्छ;
				ResizeTo(एतत्->saved_frame.Width(), एतत्->saved_frame.Height());
				MoveTo(एतत्->saved_frame.वामः, एतत्->saved_frame.शिखरम्);
				SetLook(एतत्->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } अण्णहा {
				एतत्->saved_frame = Frame();
				एतत्->saved_look = Look();
				एतत्->is_fullscreen = सच्च;
				BScreen s(एतत्);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().वामः, s.Frame().शिखरम्);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    जइ (should_propagate)
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
    tab_पङ्क्तिः(रिक्त),
#endif
    textArea(रिक्त)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    जइ (menuBar) {
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
    delete tab_पङ्क्तिः;
#endif

    जइ (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = रिक्त;
}

    शून्य
VimFormView::init(BRect frame)
{
    menuBar = नूतन BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = नूतन VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tab_पङ्क्तिः = नूतन Vim_Tab_पङ्क्तिः(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tab_पङ्क्तिः);
#endif

    BRect remaining = frame;
    textArea = नूतन VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = एतत्;
}

#ifdef FEAT_TOOLBAR
    भिन्न
VimFormView::ToolbarHeight() ध्रुव
{
    प्रत्यागम toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    भिन्न
VimFormView::TablineHeight() ध्रुव
{
    प्रत्यागम (tab_पङ्क्तिः && Is_Showing_Tab_पङ्क्तिः()) ? tab_पङ्क्तिः->TablineHeight() : 0.;
}
#endif

    शून्य
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    जइ (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.शिखरम् += MenuHeight();
	menuBar->ResizeTo(remaining.दक्षिणः, remaining.शिखरम्);
	gui.menu_height = (पूर्णाङ्क) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.वामः, remaining.शिखरम्);
	toolBar->ResizeTo(remaining.दक्षिणः, ToolbarHeight());
	remaining.शिखरम् += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tab_पङ्क्तिः->MoveTo(remaining.वामः, remaining.शिखरम्);
	tab_पङ्क्तिः->ResizeTo(remaining.दक्षिणः + 1, TablineHeight());
	remaining.शिखरम् += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.वामः, remaining.शिखरम्);
    }


    Inherited::AllAttached();
}

    शून्य
VimFormView::FrameResized(भिन्न new_width, भिन्न new_height)
{
    संरचना Vim_Resize_सन्देशः sm;
    पूर्णाङ्क adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (पूर्णाङ्क) new_width;
    sm.height = (पूर्णाङ्क) new_height;
    adjust_w = ((पूर्णाङ्क)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((पूर्णाङ्क)new_height - gui_get_base_height()) % gui.char_height;

    जइ (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, Vim_सन्देशः::Resize, &sm, परिमाण(sm));
    //	calls gui_resize_shell(new_width, new_height);

    प्रत्यागम;

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
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_निवेशः_METHOD_AWARE
#अण्णहा
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouse_Drag_Event_गणनम्(0)
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
    gui.vimTextArea = एतत्;

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
    Set_नीचः_Color(rgb);
    FillRect(updateRect, B_SOLID_नीचः);
    gui_redraw((पूर्णाङ्क) updateRect.वामः, (पूर्णाङ्क) updateRect.शिखरम्,
	    (पूर्णाङ्क) (updateRect.Width() + PEN_WIDTH), (पूर्णाङ्क) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    Set_नीचः_Color(rgb);

    जइ (updateRect.वामः < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.वामः, updateRect.शिखरम्,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_नीचः);
    जइ (updateRect.शिखरम् < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.वामः, updateRect.शिखरम्,
		    updateRect.दक्षिणः, FILL_Y(0)-PEN_WIDTH), B_SOLID_नीचः);
    जइ (updateRect.दक्षिणः >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((पूर्णाङ्क)Columns), updateRect.शिखरम्,
		    updateRect.दक्षिणः, updateRect.bottom), B_SOLID_नीचः);
    जइ (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.वामः, FILL_Y((पूर्णाङ्क)Rows),
		    updateRect.दक्षिणः, updateRect.bottom), B_SOLID_नीचः);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    शून्य
VimTextAreaView::कुञ्चिका_Down(ध्रुव वर्ण *bytes, int32 अङ्कः_Bytes)
{
    संरचना Vim_कुञ्चिका_सन्देशः km;
    char_u *dest = km.chars;

    तार्किक canHaveVimModifiers = मिच्छ;

    BMessage *सन्देशः = Window()->CurrentMessage();
    प्रतिज्ञा(सन्देशः);
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
    जइ (अङ्कः_Bytes > 1) {
	// This cannot be a special key
	जइ (अङ्कः_Bytes > कुञ्चिका_सन्देशः_BUFSIZ)
	    अङ्कः_Bytes = कुञ्चिका_सन्देशः_BUFSIZ;	    //	should never happen... ???
	km.length = अङ्कः_Bytes;
	memcpy((वर्ण *)dest, bytes, अङ्कः_Bytes);
	km.csi_escape = सच्च;
    } अण्णहा {
	int32 scancode = 0;
	सन्देशः->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	सन्देशः->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	पूर्णाङ्क len = 0;
	km.length = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	प्रतिज्ञा(B_BACKSPACE <= 0x20);
	प्रतिज्ञा(B_DELETE == 0x7F);
	जइ (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		अङ्कः_Bytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    जइ (beModifiers & B_CONTROL_कुञ्चिका) {
		पूर्णाङ्क index = कुञ्चिका_Map->normal_map[scancode];
		पूर्णाङ्क new_अङ्कः_Bytes = कुञ्चिका_Map_Chars[index];
		char_u *newBytes = (char_u *)&कुञ्चिका_Map_Chars[index + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		जइ (new_अङ्कः_Bytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    गच्छ notspecial;
		}
		bytes = (वर्ण *)newBytes;
	    }
	    canHaveVimModifiers = सच्च;

	    uint16 beoskey;
	    पूर्णाङ्क first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    जइ (अङ्कः_Bytes == 0 || bytes[0] == B_FUNCTION_कुञ्चिका) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_कुञ्चिका;
		last = अङ्कः_SPECIAL_कुञ्चिकाः;
	    } अण्णहा जइ (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } अण्णहा {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_कुञ्चिका;
	    }

	    प्रति (पूर्णाङ्क i = first; i < last; i++) {
		जइ (special_कुञ्चिकाः[i].Be_कुञ्चिकाः == beoskey) {
		    string[0] = CSI;
		    string[1] = special_कुञ्चिकाः[i].vim_code0;
		    string[2] = special_कुञ्चिकाः[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	जइ (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#जइ 0
	जइ (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#अण्णहा
	    जइ (canHaveVimModifiers)
#endif
	    {
		पूर्णाङ्क modifiers;
		modifiers = 0;
		जइ (beModifiers & B_SHIFT_कुञ्चिका)
		    modifiers |= MOD_MASK_SHIFT;
		जइ (beModifiers & B_CONTROL_कुञ्चिका)
		    modifiers |= MOD_MASK_CTRL;
		जइ (beModifiers & B_OPTION_कुञ्चिका)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		पूर्णाङ्क कुञ्चिका;
		जइ (string[0] == CSI && len == 3)
		    कुञ्चिका = TO_SPECIAL(string[1], string[2]);
		अण्णहा
		    कुञ्चिका = string[0];
		कुञ्चिका = simplify_कुञ्चिका(कुञ्चिका, &modifiers);
		जइ (IS_SPECIAL(कुञ्चिका))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(कुञ्चिका);
		    string[2] = K_THIRD(कुञ्चिका);
		    len = 3;
		}
		अण्णहा
		{
		    string[0] = कुञ्चिका;
		    len = 1;
		}

		जइ (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.length = 3;
		}
	    }
	memcpy((वर्ण *)dest, string, len);
	km.length += len;
	km.csi_escape = मिच्छ;
    }

    write_port(gui.vdcmp, Vim_सन्देशः::कुञ्चिका_2, &km, परिमाण(km));

    /*
     * blank out the pointer if necessary
     */
    जइ (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(सच्च);
	gui.pointer_hidden = TRUE;
    }
}
शून्य
VimTextAreaView::guiSendMouseEvent(
	पूर्णाङ्क	button,
	पूर्णाङ्क	x,
	पूर्णाङ्क	y,
	पूर्णाङ्क	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_सन्देशः mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, Vim_सन्देशः::Mouse, &mm, परिमाण(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    जइ (gui.pointer_hidden)
    {
	guiBlankMouse(मिच्छ);
	gui.pointer_hidden = FALSE;
    }
}

शून्य
VimTextAreaView::guiMouseMoved(
	पूर्णाङ्क	x,
	पूर्णाङ्क	y)
{
    Vim_Mouse_Moved_सन्देशः mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, Vim_सन्देशः::MouseMoved, &mm, परिमाण(mm));

    जइ (gui.pointer_hidden)
    {
	guiBlankMouse(मिच्छ);
	gui.pointer_hidden = FALSE;
    }
}

    शून्य
VimTextAreaView::guiBlankMouse(तार्किक should_hide)
{
    जइ (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } अण्णहा {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    जइ (beModifiers & B_SHIFT_कुञ्चिका)
	vim_modifiers |= MOUSE_SHIFT;
    जइ (beModifiers & B_CONTROL_कुञ्चिका)
	vim_modifiers |= MOUSE_CTRL;
    जइ (beModifiers & B_OPTION_कुञ्चिका)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    प्रत्यागम vim_modifiers;
}

    शून्य
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    प्रतिज्ञा(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    पूर्णाङ्क vimButton;

    जइ (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_वामः;
    अण्णहा जइ (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_दक्षिणः;
    अण्णहा जइ (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_मध्यः;
    अण्णहा
	प्रत्यागम;		// Unknown button

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
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, ध्रुव BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    जइ (gui.pointer_hidden)
    {
	guiBlankMouse(मिच्छ);
	gui.pointer_hidden = FALSE;
    }

    जइ (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	प्रत्यागम;
    }

    atomic_add(&mouse_Drag_Event_गणनम्, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    शून्य
VimTextAreaView::MessageReceived(BMessage *m)
{
    विकल्प (m->what) {
	पक्ष 'menu':
	    {
		Vim_Menu_सन्देशः mm;
		mm.guiMenu = रिक्त;  // in case no pointer in msg
		m->FindPointer("VimMenu", (शून्य **)&mm.guiMenu);
		write_port(gui.vdcmp, Vim_सन्देशः::Menu, &mm, परिमाण(mm));
	    }
	    भङ्ग;
	पक्ष B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		भिन्न small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->Set_मानम्(scb->मानम्_2()+small*dy*3);
		scb->मानम्_Changed(scb->मानम्_2());
#जइ 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->Set_मानम्(scb->मानम्_2()+small*dy);
		scb->मानम्_Changed(scb->मानम्_2());
#endif
	    }
	    भङ्ग;
#ifdef FEAT_MBYTE_IME
	पक्ष B_निवेशः_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		विकल्प(opcode)
		{
		    पक्ष B_निवेशः_METHOD_STARTED:
			जइ (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = नूतन BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			भङ्ग;
		    पक्ष B_निवेशः_METHOD_CHANGED:
			{
			    BString str;
			    तार्किक confirmed;
			    जइ (IMData.message) *(IMData.message) = *m;
			    अण्णहा	       IMData.message = नूतन BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    जइ (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				संरचना Vim_कुञ्चिका_सन्देशः km;
				km.csi_escape = सच्च;
				पूर्णाङ्क clen;
				पूर्णाङ्क i = 0;
				जाव (i < str.Length())
				{
				    clen = utf_ptr2len(chars+i);
				    memcpy(km.chars, chars+i, clen);
				    km.length = clen;
				    write_port(gui.vdcmp, Vim_सन्देशः::कुञ्चिका_2, &km, परिमाण(km));
				    i += clen;
				}
			    }
			}
			भङ्ग;
		    पक्ष B_निवेशः_METHOD_LOCATION_REQUEST:
			{
			    BMessage सन्देशः(B_निवेशः_METHOD_EVENT);
			    सन्देशः.AddInt32("be:opcode", B_निवेशः_METHOD_LOCATION_REQUEST);
			    सन्देशः.AddPoint("be:location_reply", IMData.location);
			    सन्देशः.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&सन्देशः);
			}
			भङ्ग;
		    पक्ष B_निवेशः_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = रिक्त;
			IMData.message = रिक्त;
			भङ्ग;
		}
	    }
	    // TODO: sz: break here???
#endif
	स्वभाव:
	    जइ (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(मिच्छ);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_कुञ्चिका) != 0);
	    } अण्णहा {
		Inherited::MessageReceived(m);
	    }
	    भङ्ग;
    }
}

    पूर्णाङ्क
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    जइ (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	जइ (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	प्रत्यागम OK;
    }
    प्रत्यागम FAIL;
}

    शून्य
VimTextAreaView::mchDrawString(पूर्णाङ्क row, पूर्णाङ्क col, char_u *s, पूर्णाङ्क len, पूर्णाङ्क flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    जइ (!(flags & DRAW_TRANSP)) {
	पूर्णाङ्क cells;
	cells = 0;
	प्रति (पूर्णाङ्क i=0; i<len; i++) {
	    पूर्णाङ्क cn = utf_ptr2cells((char_u *)(s+i));
	    जइ (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_नीचः);
    }

    BFont font;
    एतत्->GetFont(&font);
    जइ (!font.IsFixed())
    {
	वर्ण* p = (वर्ण*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	पूर्णाङ्क cells;
	जाव ((p - (वर्ण*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    जइ (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    जइ (cells<4) lastpos += cells;
	    अण्णहा	lastpos++;
	    p += clen;
	}
    }
    अण्णहा
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((वर्ण*)s, len, where);
	जइ (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((वर्ण*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    जइ (flags & DRAW_UNDERL) {
	पूर्णाङ्क cells;
	cells = 0;
	प्रति (पूर्णाङ्क i=0; i<len; i++) {
	    पूर्णाङ्क cn = utf_ptr2cells((char_u *)(s+i));
	    जइ (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	Stroke_पङ्क्तिः(start, end);
    }
}

शून्य
VimTextAreaView::mchClearBlock(
	पूर्णाङ्क	row1,
	पूर्णाङ्क	col1,
	पूर्णाङ्क	row2,
	पूर्णाङ्क	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_नीचः);
}

    शून्य
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_नीचः);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    शून्य
VimTextAreaView::mch_Delete_पङ्क्तयः(पूर्णाङ्क row, पूर्णाङ्क अङ्कः_पङ्क्तयः)
{
    BRect source, dest;
    source.वामः = FILL_X(gui.scroll_region_वामः);
    source.शिखरम् = FILL_Y(row + अङ्कः_पङ्क्तयः);
    source.दक्षिणः = FILL_X(gui.scroll_region_दक्षिणः + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.वामः = FILL_X(gui.scroll_region_वामः);
    dest.शिखरम् = FILL_Y(row);
    dest.दक्षिणः = FILL_X(gui.scroll_region_दक्षिणः + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - अङ्कः_पङ्क्तयः + 1) - PEN_WIDTH;

    जइ (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - अङ्कः_पङ्क्तयः + 1,
		gui.scroll_region_वामः,
		gui.scroll_region_bot, gui.scroll_region_दक्षिणः);


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
VimTextAreaView::mch_निवेशय_पङ्क्तयः(पूर्णाङ्क row, पूर्णाङ्क अङ्कः_पङ्क्तयः)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.वामः = FILL_X(gui.scroll_region_वामः);
    source.शिखरम् = FILL_Y(row);
    source.दक्षिणः = FILL_X(gui.scroll_region_दक्षिणः + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - अङ्कः_पङ्क्तयः + 1) - PEN_WIDTH;

    dest.वामः = FILL_X(gui.scroll_region_वामः);
    dest.शिखरम् = FILL_Y(row + अङ्कः_पङ्क्तयः);
    dest.दक्षिणः = FILL_X(gui.scroll_region_दक्षिणः + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    जइ (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_वामः,
		row + अङ्कः_पङ्क्तयः - 1, gui.scroll_region_दक्षिणः);

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
    स्थिर ध्रुव rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    ध्रुव वर्ण* s;
    पूर्णाङ्क len;
    BMessage* सन्देशः = IMData.message;
    जइ (!सन्देशः)
	प्रत्यागम;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.गणनम्, W_WIDTH(curwin), GUI_MON_NOCLEAR);
    तार्किक confirmed = मिच्छ;
    सन्देशः->FindBool("be:confirmed", &confirmed);
    जइ (confirmed)
	प्रत्यागम;
    rgb_color hcolor = उच्चः_Color(), lcolor = नीचः_Color();
    सन्देशः->FindString("be:string", &str);
    s = str.String();
    len = str.Length();
    Set_उच्चः_Color(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    सन्देशः->FindInt32("be:selection", 0, &sel_start);
    सन्देशः->FindInt32("be:selection", 1, &sel_end);
    पूर्णाङ्क clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    प्रति (पूर्णाङ्क i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	जइ (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	जइ (sel_start<=i && i<sel_end)
	{
	    Set_नीचः_Color(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	अण्णहा
	{
	    Set_नीचः_Color(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_नीचः);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.गणनम् = (पूर्णाङ्क)pos.y;

    Set_उच्चः_Color(hcolor);
    Set_नीचः_Color(lcolor);
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
    ignore_मानम्(-1),
    scroll_Event_गणनम्(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    शून्य
VimScrollBar::मानम्_Changed(भिन्न new_मानम्)
{
    जइ (ignore_मानम् >= 0.0 && new_मानम् == ignore_मानम्) {
	ignore_मानम् = -1;
	प्रत्यागम;
    }
    ignore_मानम् = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_add(&scroll_Event_गणनम्, 1);

    संरचना Vim_Scroll_Bar_सन्देशः sm;

    sm.sb = एतत्;
    sm.मानम् = (long) new_मानम्;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, Vim_सन्देशः::ScrollBar, &sm, परिमाण(sm));

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

    atomic_add(&scroll_Event_गणनम्, 1);

    संरचना Vim_Scroll_Bar_सन्देशः sm;

    sm.sb = एतत्;
    sm.मानम् = (long) मानम्_2();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, Vim_सन्देशः::ScrollBar, &sm, परिमाण(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    शून्य
VimScrollBar::Set_मानम्(भिन्न new_मानम्)
{
    जइ (new_मानम् == मानम्_2())
	प्रत्यागम;

    ignore_मानम् = new_मानम्;
    Inherited::Set_मानम्(new_मानम्);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(ध्रुव VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ध्रुव BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(ध्रुव VimFont &rhs): BFont(rhs)
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

#जइ defined(FEAT_GUI_DIALOG)

ध्रुव unsigned पूर्णाङ्क  k_Vim_Dialog_Button_सन्देशः = 'VMDB';
ध्रुव unsigned पूर्णाङ्क  kVimDialogIconStripeWidth = 30;
ध्रुव unsigned पूर्णाङ्क  kVimDialogButtonsSpacingX = 9;
ध्रुव unsigned पूर्णाङ्क  kVimDialogButtonsSpacingY = 4;
ध्रुव unsigned पूर्णाङ्क  kVimDialogSpacingX = 6;
ध्रुव unsigned पूर्णाङ्क  kVimDialogSpacingY = 10;
ध्रुव unsigned पूर्णाङ्क  kVimDialogMinimalWidth  = 310;
ध्रुव unsigned पूर्णाङ्क  kVimDialogMinimalHeight = 75;
ध्रुव BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(पूर्णाङ्क type, ध्रुव वर्ण *title, ध्रुव वर्ण *message,
	ध्रुव वर्ण *buttons, पूर्णाङ्क dfltbutton, ध्रुव वर्ण *textfield, पूर्णाङ्क ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_मानम्(dfltbutton)
    , fMessageView(रिक्त)
    , f_निवेशः_Control(रिक्त)
    , f_निवेशः_मानम्(textfield)
{
    //	master view
    VimDialog::View* view = नूतन VimDialog::View(Bounds());
    जइ (view == रिक्त)
	प्रत्यागम;

    जइ (title == रिक्त)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    भिन्न maxButtonWidth  = 0;
    भिन्न maxButtonHeight = 0;
    भिन्न buttonsWidth	  = 0;
    भिन्न buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    करेहि
    {
	int32 end = strButtons.FindFirst('\n');
	जइ (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	जइ (end == B_ERROR)
	    भङ्ग;

	strButtons.Remove(0, end + 1);
    } जाव (सच्च);

    int32 buttons_गणनम् = fButtonsList.गणनम्_वस्तूनि();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttons_गणनम् - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttons_गणनम् - 1);
    भिन्न dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    भिन्न dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    तार्किक vertical = (vim_strchr(p_go, GO_VERTICAL) != रिक्त) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    जइ (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.वामः  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.शिखरम्   += kVimDialogSpacingY;
    rect.दक्षिणः -= kVimDialogSpacingX;
    rect.bottom = rect.शिखरम्;
    fMessageView = नूतन BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_वामः | B_FOLLOW_शिखरम्, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(मिच्छ);
    fMessageView->MakeSelectable(मिच्छ);
    fMessageView->SetWordWrap(सच्च);
    AddChild(fMessageView);

    भिन्न messageHeight = fMessageView->TextHeight(0, fMessageView->गणनम्_पङ्क्तयः());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    जइ (f_निवेशः_मानम् != रिक्त) {
	rect.शिखरम्     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	f_निवेशः_Control = नूतन BTextControl(rect, "_iv_", रिक्त, f_निवेशः_मानम्, रिक्त,
		B_FOLLOW_वामः | B_FOLLOW_शिखरम्, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_निवेशः_Control->TextView()->SetText(f_निवेशः_मानम्);
	f_निवेशः_Control->TextView()->SetWordWrap(मिच्छ);
	AddChild(f_निवेशः_Control);

	भिन्न width = 0.f, height = 0.f;
	f_निवेशः_Control->GetPreferredSize(&width, &height);
	f_निवेशः_Control->MakeFocus(सच्च);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    भिन्न buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    प्रति (int32 i = 0 ; i < buttons_गणनम्; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	जइ (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} अण्णहा
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	जइ (dfltbutton == i + 1) {
	    button->MakeDefault(सच्च);
	    button->MakeFocus(f_निवेशः_Control == रिक्त);
	}
    }
}

VimDialog::~VimDialog()
{
    जइ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    पूर्णाङ्क
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    जइ (fDialogSem < B_OK) {
	Quit();
	प्रत्यागम f_Dialog_मानम्;
    }

    Show();

    जाव (acquire_sem(fDialogSem) == B_INTERRUPTED);

    पूर्णाङ्क ret_मानम् = f_Dialog_मानम्;
    जइ (f_निवेशः_मानम् != रिक्त)
	vim_strncpy((char_u*)f_निवेशः_मानम्, (char_u*)f_निवेशः_Control->Text(), IOSIZE - 1);

    जइ (Lock())
	Quit();

    प्रत्यागम ret_मानम्;
}

शून्य VimDialog::MessageReceived(BMessage *सन्देशः)
{
    int32 which = 0;
    जइ (सन्देशः->what != k_Vim_Dialog_Button_सन्देशः ||
	    सन्देशः->FindInt32("which", &which) != B_OK)
	प्रत्यागम BWindow::MessageReceived(सन्देशः);

    f_Dialog_मानम् = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, ध्रुव वर्ण* label)
{
    BMessage *message = नूतन BMessage(k_Vim_Dialog_Button_सन्देशः);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = नूतन BButton(rect, name.String(), label, message,
	    B_FOLLOW_दक्षिणः | B_FOLLOW_BOTTOM);

    भिन्न width = 0.f, height = 0.f;
    button->GetPreferredSize(&width, &height);
    button->ResizeTo(width, height);

    प्रत्यागम button;
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
    stripeRect.दक्षिणः = kVimDialogIconStripeWidth;
    Set_उच्चः_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    जइ (fIconBitmap == रिक्त)
	प्रत्यागम;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

शून्य VimDialog::View::InitIcon(int32 type)
{
    जइ (type == VIM_GENERIC)
	प्रत्यागम;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    जइ (status != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(status));
	प्रत्यागम;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    जइ (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	प्रत्यागम;
    }

    BResources resources(&file);
    जइ (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	प्रत्यागम;
    }

    ध्रुव वर्ण *name = "";
    विकल्प(type) {
	पक्ष VIM_ERROR:	    name = "stop"; भङ्ग;
	पक्ष VIM_WARNING:   name = "warn"; भङ्ग;
	पक्ष VIM_INFO:	    name = "info"; भङ्ग;
	पक्ष VIM_QUESTION:  name = "idea"; भङ्ग;
	स्वभाव: प्रत्यागम;
    }

    int32 iconSize = 32;
    fIconBitmap = नूतन BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), 0, B_RGBA32);
    जइ (fIconBitmap == रिक्त || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == रिक्त) ? "null" : strerror(fIconBitmap->InitCheck()));
	प्रत्यागम;
    }

    size_t size = 0;
    ध्रुव uint8* iconData = रिक्त;
    //	try vector icon first?
    iconData = (ध्रुव uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &size);
    जइ (iconData != रिक्त && BIconUtils::GetVectorIcon(iconData, size, fIconBitmap) == B_OK)
	प्रत्यागम;

    //	try bitmap icon now
    iconData = (ध्रुव uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &size);
    जइ (iconData == रिक्त) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = रिक्त;
	प्रत्यागम;
    }

    जइ (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(iconData, iconSize, iconSize, iconSize, fIconBitmap);
}

ध्रुव unsigned पूर्णाङ्क  k_Vim_Dialog_OKButton_सन्देशः = 'FDOK';
ध्रुव unsigned पूर्णाङ्क  k_Vim_Dialog_Cancel_Button_सन्देशः = 'FDCN';
ध्रुव unsigned पूर्णाङ्क  k_Vim_Dialog_Size_निवेशः_सन्देशः = 'SICH';
ध्रुव unsigned पूर्णाङ्क  k_Vim_Dialog_Family_Select_सन्देशः = 'MSFM';
ध्रुव unsigned पूर्णाङ्क  k_Vim_Dialog_Style_Select_सन्देशः = 'MSST';
ध्रुव unsigned पूर्णाङ्क  k_Vim_Dialog_Size_Select_सन्देशः = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, भिन्न* size)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_मानम्(मिच्छ)
    , fFamily(family)
    , fStyle(style)
    , fSize(size)
    , fFontSize(*size)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_निवेशः(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_LENGTH);
    strncpy(fFontStyle, *style, B_FONT_STYLE_LENGTH);

    //	"client" area view
    BBox *clientBox = नूतन BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.वामः_शिखरम्(), RC.वामः_शिखरम्());

    //	at first create all controls
    fPreview = नूतन BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = नूतन BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = नूतन BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = नूतन BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *labelSize = नूतन BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(labelSize);
    labelSize->ResizeToPreferred();

    fFamiliesList = नूतन BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = नूतन BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_वामः_दक्षिणः, 0, मिच्छ, सच्च);
    clientBox->AddChild(scrollFamilies);

    fStylesList= नूतन BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = नूतन BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_वामः_दक्षिणः, 0, मिच्छ, सच्च);
    clientBox->AddChild(scrollStyles);

    f_Sizes_निवेशः = नूतन BTextControl(rc, "inputSize", रिक्त, "???",
	    नूतन BMessage(k_Vim_Dialog_Size_निवेशः_सन्देशः));
    clientBox->AddChild(f_Sizes_निवेशः);
    f_Sizes_निवेशः->ResizeToPreferred();

    fSizesList = नूतन BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = नूतन BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_वामः_दक्षिणः, 0, मिच्छ, सच्च);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = नूतन BButton(rc, "buttonOK", "OK",
			नूतन BMessage(k_Vim_Dialog_OKButton_सन्देशः));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = नूतन BButton(rc, "buttonCancel", "Cancel",
			नूतन BMessage(k_Vim_Dialog_Cancel_Button_सन्देशः));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    भिन्न पङ्क्तिः_Height = labelFamily->Bounds().Height();
    भिन्न previewHeight = पङ्क्तिः_Height * 3;
    भिन्न offsetYLabels = previewHeight + kVimDialogSpacingY;
    भिन्न offsetYLists = offsetYLabels + पङ्क्तिः_Height + kVimDialogSpacingY / 2;
    भिन्न offsetYSizes = offsetYLists + f_Sizes_निवेशः->Bounds().Height() + kVimDialogSpacingY / 2;
    भिन्न listsHeight = पङ्क्तिः_Height * 9;
    भिन्न offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    भिन्न maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    भिन्न familiesWidth = labelFamily->Bounds().Width() * 5;
    भिन्न offsetXStyles = familiesWidth + kVimDialogSpacingX;
    भिन्न stylesWidth = labelStyle->Bounds().Width() * 4;
    भिन्न offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    भिन्न sizesWidth = labelSize->Bounds().Width() * 2;
    भिन्न maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.वामः + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.शिखरम् + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    labelSize->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    भिन्न insetX = f_Sizes_निवेशः->TextView()->Bounds().Width() - f_Sizes_निवेशः->Bounds().Width();
    भिन्न insetY = f_Sizes_निवेशः->TextView()->Bounds().Width() - f_Sizes_निवेशः->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    f_Sizes_निवेशः->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    f_Sizes_निवेशः->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    f_Sizes_निवेशः->ResizeTo(sizesWidth, f_Sizes_निवेशः->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    पूर्णाङ्क selIndex = -1;
    पूर्णाङ्क गणनम् = गणनम्_font_families();
    प्रति (पूर्णाङ्क i = 0; i < गणनम्; i++) {
	font_family family;
	जइ (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(नूतन BStringItem((ध्रुव वर्ण*)family));
	    जइ (strncmp(family, fFontFamily, B_FONT_FAMILY_LENGTH) == 0)
		selIndex = i;
	}
    }

    जइ (selIndex >= 0) {
	fFamiliesList->Select(selIndex);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    selIndex = -1;
    प्रति (पूर्णाङ्क size = 8, index = 0; size <= 18; size++, index++) {
	BString str;
	str << size;
	fSizesList->AddItem(नूतन BStringItem(str));
	जइ (size == fFontSize)
	    selIndex = index;

    }

    जइ (selIndex >= 0) {
	fSizesList->Select(selIndex);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(नूतन BMessage(k_Vim_Dialog_Family_Select_सन्देशः));
    fStylesList->SetSelectionMessage(नूतन BMessage(k_Vim_Dialog_Style_Select_सन्देशः));
    fSizesList->SetSelectionMessage(नूतन BMessage(k_Vim_Dialog_Size_Select_सन्देशः));
    f_Sizes_निवेशः->SetModificationMessage(नूतन BMessage(k_Vim_Dialog_Size_निवेशः_सन्देशः));

    Update_Size_निवेशः_Preview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    जइ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    शून्य
VimSelectFontDialog::_CleanList(BListView* list)
{
    जाव (0 < list->गणनम्_वस्तूनि())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    तार्किक
VimSelectFontDialog::Go()
{
    जइ (fStatus != B_OK) {
	Quit();
	प्रत्यागम NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    जइ (fDialogSem < B_OK) {
	Quit();
	प्रत्यागम f_Dialog_मानम्;
    }

    Show();

    जाव (acquire_sem(fDialogSem) == B_INTERRUPTED);

    तार्किक ret_मानम् = f_Dialog_मानम्;

    जइ (Lock())
	Quit();

    प्रत्यागम ret_मानम्;
}


शून्य VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 selIndex = -1;
    int32 गणनम् = गणनम्_font_styles(fFontFamily);
    प्रति (int32 i = 0; i < गणनम्; i++) {
	font_style style;
	uint32 flags = 0;
	जइ (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(नूतन BStringItem((ध्रुव वर्ण*)style));
	    जइ (strncmp(style, fFontStyle, B_FONT_STYLE_LENGTH) == 0)
		selIndex = i;
	}
    }

    जइ (selIndex >= 0) {
	fStylesList->Select(selIndex);
	fStylesList->ScrollToSelection();
    } अण्णहा
	fStylesList->Select(0);
}


शून्य VimSelectFontDialog::Update_Size_निवेशः_Preview()
{
    वर्ण buf[10] = {0};
    vim_snprintf(buf, परिमाण(buf), (वर्ण*)"%.0f", fFontSize);
    f_Sizes_निवेशः->SetText(buf);
}


शून्य VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_SIZE);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (पूर्णाङ्क)fFontSize << " pt.";
    fPreview->SetText(str);
}


    तार्किक
VimSelectFontDialog::_UpdateFromListItem(BListView* list, वर्ण* text, पूर्णाङ्क textSize)
{
    int32 index = list->CurrentSelection();
    जइ (index < 0)
	प्रत्यागम मिच्छ;
    BStringItem* item = (BStringItem*)list->ItemAt(index);
    जइ (item == रिक्त)
	प्रत्यागम मिच्छ;
    strncpy(text, item->Text(), textSize);
    प्रत्यागम सच्च;
}


शून्य VimSelectFontDialog::MessageReceived(BMessage *सन्देशः)
{
    विकल्प (सन्देशः->what) {
	पक्ष k_Vim_Dialog_OKButton_सन्देशः:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_LENGTH);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_LENGTH);
	    *fSize = fFontSize;
	    f_Dialog_मानम् = सच्च;
	पक्ष k_Vim_Dialog_Cancel_Button_सन्देशः:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    प्रत्यागम;
	पक्ष B_कुञ्चिका_UP:
	    {
		int32 कुञ्चिका = 0;
		जइ (सन्देशः->FindInt32("raw_char", &कुञ्चिका) == B_OK
			&& कुञ्चिका == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    भङ्ग;

	पक्ष k_Vim_Dialog_Family_Select_सन्देशः:
	    जइ (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_LENGTH)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    भङ्ग;
	पक्ष k_Vim_Dialog_Style_Select_सन्देशः:
	    जइ (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_LENGTH))
		_UpdateFontPreview();
	    भङ्ग;
	पक्ष k_Vim_Dialog_Size_Select_सन्देशः:
	    {
		वर्ण buf[10] = {0};
		जइ (_UpdateFromListItem(fSizesList, buf, परिमाण(buf))) {
		    भिन्न size = atof(buf);
		    जइ (size > 0.f) {
			fFontSize = size;
			Update_Size_निवेशः_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    भङ्ग;
	पक्ष k_Vim_Dialog_Size_निवेशः_सन्देशः:
	    {
		भिन्न size = atof(f_Sizes_निवेशः->Text());
		जइ (size > 0.f) {
		    fFontSize = size;
		    _UpdateFontPreview();
		}
	    }
	    भङ्ग;
	स्वभाव:
	    भङ्ग;
    }
    प्रत्यागम BWindow::MessageReceived(सन्देशः);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
स्थिर BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, ध्रुव वर्ण *name) :
    BBox(frame, name, B_FOLLOW_वामः | B_FOLLOW_शिखरम्, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 गणनम् = fButtonsList.गणनम्_वस्तूनि();
    प्रति (int32 i = 0; i < गणनम्; i++)
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

    भिन्न
VimToolbar::ToolbarHeight() ध्रुव
{
    भिन्न size = रिक्त == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    प्रत्यागम size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    तार्किक
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    भिन्न height = bitmap->Bounds().Height();
    भिन्न width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->BitsLength() / 4;
    प्रति (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    प्रत्यागम सच्च;
}

    तार्किक
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    जइ (normalButtonsBitmap == रिक्त)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    जइ (normalButtonsBitmap == रिक्त)
	प्रत्यागम मिच्छ;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = नूतन BBitmap(&archive);
    जइ (grayedButtonsBitmap == रिक्त)
	प्रत्यागम मिच्छ;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    प्रत्यागम सच्च;
}

BBitmap *VimToolbar::LoadVimBitmap(ध्रुव वर्ण* fileName)
{
    BBitmap *bitmap = रिक्त;

    पूर्णाङ्क mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    जइ (runtimePath != रिक्त && fileName != रिक्त) {
	BString strPath((वर्ण*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    जइ (mustfree)
	vim_free(runtimePath);

    प्रत्यागम bitmap;
}

    तार्किक
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, तार्किक pressed)
{
    भिन्न size = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, size, size), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.Set_उच्चः_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, size - 1, size - 1);
    BRect destination(source);

    source.OffsetBy(size * index, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    जइ (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	size += ButtonMargin * 2 - 1;
	view.Begin_पङ्क्तिः_Array(4);
	view.Add_पङ्क्तिः(BPoint(0, 0),	 BPoint(size, 0),    shadowColor);
	view.Add_पङ्क्तिः(BPoint(size, 0),	 BPoint(size, size), shineColor);
	view.Add_पङ्क्तिः(BPoint(size, size), BPoint(0, size),    shineColor);
	view.Add_पङ्क्तिः(BPoint(0, size),	 BPoint(0, 0),	     shadowColor);
	view.End_पङ्क्तिः_Array();
    }

    view.EndPicture();
    RemoveChild(&view);

    प्रत्यागम सच्च;
}

    तार्किक
VimToolbar::AddButton(int32 index, vimmenu_T *menu)
{
    BPictureButton *button = रिक्त;
    जइ (!menu_is_separator(menu->name)) {
	भिन्न size = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, size, size);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	जइ (menu->iconfile == रिक्त && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, सच्च);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, मिच्छ);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, मिच्छ);
	} अण्णहा {

	    char_u buffer[MAXPATHL] = {0};
	    BBitmap *bitmap = रिक्त;

	    जइ (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, buffer, (वर्ण*)"png");
		bitmap = BTranslationUtils::GetBitmap((वर्ण*)buffer);
	    }

	    जइ (bitmap == रिक्त && gui_find_bitmap(menu->name, buffer, (वर्ण*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((वर्ण*)buffer);

	    जइ (bitmap == रिक्त)
		bitmap = नूतन BBitmap(BRect(0, 0, size, size), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, सच्च);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, मिच्छ);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, मिच्छ);

	    delete bitmap;
	}

	button = नूतन BPictureButton(frame, (वर्ण*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    तार्किक result = fButtonsList.AddItem(button, index);
    InvalidateLayout();
    प्रत्यागम result;
}

    तार्किक
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    जइ (menu->button) {
	जइ (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = रिक्त;
	}
    }
    प्रत्यागम सच्च;
}

    तार्किक
VimToolbar::GrayButton(vimmenu_T *menu, पूर्णाङ्क grey)
{
    जइ (menu->button) {
	int32 index = fButtonsList.IndexOf(menu->button);
	जइ (index >= 0)
	    menu->button->SetEnabled(grey ? मिच्छ : सच्च);
    }
    प्रत्यागम सच्च;
}

    शून्य
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 गणनम् = fButtonsList.गणनम्_वस्तूनि();
    प्रति (int32 i = 0; i < गणनम्; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	जइ (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} अण्णहा
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#जइ defined(FEAT_GUI_TABLINE)

    भिन्न
Vim_Tab_पङ्क्तिः::TablineHeight() ध्रुव
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    प्रत्यागम TabHeight(); //  + ToolbarMargin;
}

शून्य
Vim_Tab_पङ्क्तिः::MouseDown(BPoint point)
{
    जइ (!gui_mch_showing_tabline())
	प्रत्यागम;

    BMessage *m = Window()->CurrentMessage();
    प्रतिज्ञा(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    पूर्णाङ्क index = 0; //  0 means here - no tab found
    प्रति (पूर्णाङ्क i = 0; i < गणनम्_Tabs(); i++) {
	जइ (TabFrame(i).Contains(point)) {
	    index = i + 1; //  indexes are 1-based
	    भङ्ग;
	}
    }

    पूर्णाङ्क event = -1;

    जइ ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    अण्णहा जइ (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = index > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    अण्णहा जइ (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* निष्कासय_Up_Menu = नूतन BPopUpMenu("tabLineContextMenu", मिच्छ, मिच्छ);
	निष्कासय_Up_Menu->AddItem(नूतन BMenuItem(_("Close tabi R"), नूतन BMessage(TABLINE_MENU_CLOSE)));
	निष्कासय_Up_Menu->AddItem(नूतन BMenuItem(_("New tab    T"), नूतन BMessage(TABLINE_MENU_NEW)));
	निष्कासय_Up_Menu->AddItem(नूतन BMenuItem(_("Open tab..."), नूतन BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = निष्कासय_Up_Menu->Go(point);
	जइ (item != रिक्त) {
	    event = item->Command();
	}

	delete निष्कासय_Up_Menu;

    } अण्णहा {
	//  default processing
	BTabView::MouseDown(point);
	प्रत्यागम;
    }

    जइ (event < 0)
	प्रत्यागम;

    Vim_Tabline_Menu_सन्देशः tmm;
    tmm.index = index;
    tmm.event = event;
    write_port(gui.vdcmp, Vim_सन्देशः::TablineMenu, &tmm, परिमाण(tmm));
}

शून्य
Vim_Tab_पङ्क्तिः::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_पङ्क्तिः *tab_पङ्क्तिः = gui.vimForm->Tab_पङ्क्तिः();
    जइ (tab_पङ्क्तिः != रिक्त) {

	int32 i = 0;
	प्रति (; i < tab_पङ्क्तिः->गणनम्_Tabs(); i++)
	    जइ (एतत् == tab_पङ्क्तिः->TabAt(i))
		भङ्ग;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	जइ (i < tab_पङ्क्तिः->गणनम्_Tabs()) {
	    Vim_Tabline_सन्देशः tm;
	    tm.index = i + 1;
	    write_port(gui.vdcmp, Vim_सन्देशः::Tabline, &tm, परिमाण(tm));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
स्थिर वर्ण appsig[] = "application/x-vnd.Haiku-Vim-8";
कुञ्चिका_map *कुञ्चिका_Map;
वर्ण *कुञ्चिका_Map_Chars;
पूर्णाङ्क main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    संरचना Vim_सन्देशः vm;
    int32 what;
    ssize_t size;

    size = read_port_etc(gui.vdcmp, &what, &vm, परिमाण(vm),
	    B_TIMEOUT, timeout);

    जइ (size >= 0) {
	विकल्प (what) {
	    पक्ष Vim_सन्देशः::कुञ्चिका_2:
		{
		    char_u *string = vm.u.कुञ्चिका_2.chars;
		    पूर्णाङ्क len = vm.u.कुञ्चिका_2.length;
		    जइ (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_निवेशः_buf();
			got_int = TRUE;
		    }

		    जइ (vm.u.कुञ्चिका_2.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			पूर्णाङ्क	i;
			char_u	buf[2];

			प्रति (i = 0; i < len; ++i)
			{
			    add_to_निवेशः_buf(string + i, 1);
			    जइ (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (पूर्णाङ्क)KE_CSI;
				add_to_निवेशः_buf(buf, 2);
			    }
			}
		    }
#अण्णहा
			add_to_निवेशः_buf_csi(string, len);
#endif
		    अण्णहा
			add_to_निवेशः_buf(string, len);
		}
		भङ्ग;
	    पक्ष Vim_सन्देशः::Resize:
		gui_resize_shell(vm.u.NewSize.width, vm.u.NewSize.height);
		भङ्ग;
	    पक्ष Vim_सन्देशः::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_गणनम् =
			atomic_add(&vm.u.Scroll.sb->scroll_Event_गणनम्, -1);
		    जइ (old_गणनम् <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.मानम्, vm.u.Scroll.stillDragging);
		}
		भङ्ग;
#जइ defined(FEAT_MENU)
	    पक्ष Vim_सन्देशः::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		भङ्ग;
#endif
	    पक्ष Vim_सन्देशः::Mouse:
		{
		    int32 old_गणनम्;
		    जइ (vm.u.Mouse.button == MOUSE_DRAG)
			old_गणनम् =
			    atomic_add(&gui.vimTextArea->mouse_Drag_Event_गणनम्, -1);
		    अण्णहा
			old_गणनम् = 0;
		    जइ (old_गणनम् <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		भङ्ग;
	    पक्ष Vim_सन्देशः::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		भङ्ग;
	    पक्ष Vim_सन्देशः::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		जइ (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		भङ्ग;
	    पक्ष Vim_सन्देशः::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		भङ्ग;
	    पक्ष Vim_सन्देशः::Tabline:
		send_tabline_event(vm.u.Tabline.index);
		भङ्ग;
	    पक्ष Vim_सन्देशः::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.index, vm.u.TablineMenu.event);
		भङ्ग;
	    स्वभाव:
		//  unrecognised message, ignore it
		भङ्ग;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    प्रत्यागम size;
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

    पूर्णाङ्क
vim_lock_screen()
{
    प्रत्यागम !gui.vimWindow || gui.vimWindow->Lock();
}

    शून्य
vim_unlock_screen()
{
    जइ (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#जइ RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(शून्य *args)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    प्रत्यागम 0;
}

#अण्णहा

    int32
call_main(शून्य *args)
{
    संरचना MainArgs *ma = (MainArgs *)args;

    प्रत्यागम main(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    शून्य
gui_mch_prepare(
	पूर्णाङ्क	*argc,
	वर्ण	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    जइ (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(रिक्त), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_गणनम्, "vim VDCMP");

#जइ RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, रिक्त);
	जइ (tid >= B_OK) {
	    resume_thread(tid);
	} अण्णहा {
	    getout(1);
	}
#अण्णहा
	MainArgs ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	जइ (tid >= B_OK) {
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
    जइ (!isatty(0)) {
	संरचना stat stat_stdin, stat_dev_null;

	जइ (fstat(0, &stat_stdin) == -1 ||
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
    पूर्णाङ्क
gui_mch_init_check(शून्य)
{
    प्रत्यागम OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    पूर्णाङ्क
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (पूर्णाङ्क) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (पूर्णाङ्क) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    जइ (gui.vdcmp < B_OK)
	प्रत्यागम FAIL;
    get_कुञ्चिका_map(&कुञ्चिका_Map, &कुञ्चिका_Map_Chars);

    gui.vimWindow = नूतन VimWindow();	// hidden and locked
    जइ (!gui.vimWindow)
	प्रत्यागम FAIL;

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

    प्रत्यागम OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    शून्य
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    जइ (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    पूर्णाङ्क
gui_mch_open()
{
    जइ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    जइ (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	प्रत्यागम OK;
    }

    प्रत्यागम FAIL;
}

    शून्य
gui_mch_exit(पूर्णाङ्क vim_exitcode)
{
    जइ (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#जइ !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    जइ (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(मिच्छ);

	main_exitcode = vim_exitcode;
#जइ RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#अण्णहा
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
    पूर्णाङ्क
gui_mch_get_winpos(पूर्णाङ्क *x, पूर्णाङ्क *y)
{
    जइ (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (पूर्णाङ्क)r.वामः;
	*y = (पूर्णाङ्क)r.शिखरम्;
	प्रत्यागम OK;
    }
    अण्णहा
	प्रत्यागम FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    शून्य
gui_mch_set_winpos(पूर्णाङ्क x, पूर्णाङ्क y)
{
    जइ (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
शून्य
gui_mch_set_shellsize(
	पूर्णाङ्क	width,
	पूर्णाङ्क	height,
	पूर्णाङ्क	min_width,
	पूर्णाङ्क	min_height,
	पूर्णाङ्क	base_width,
	पूर्णाङ्क	base_height,
	पूर्णाङ्क	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    जइ (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	भिन्न minWidth, maxWidth, minHeight, maxHeight;

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
	पूर्णाङ्क	*screen_w,
	पूर्णाङ्क	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	जइ (screen.IsValid()) {
	    frame = screen.Frame();
	} अण्णहा {
	    frame.दक्षिणः = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (पूर्णाङ्क) frame.दक्षिणः - 2 * gui.scrollbar_width - 20;
    *screen_h = (पूर्णाङ्क) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

शून्य
gui_mch_set_text_area_pos(
	पूर्णाङ्क	x,
	पूर्णाङ्क	y,
	पूर्णाङ्क	w,
	पूर्णाङ्क	h)
{
    जइ (!gui.vimTextArea)
	प्रत्यागम;

    जइ (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	जइ (gui.vimForm->Tab_पङ्क्तिः() != रिक्त) {
	    gui.vimForm->Tab_पङ्क्तिः()->ResizeTo(w, gui.vimForm->TablineHeight());
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
	पूर्णाङ्क	flag)
{
    VimScrollBar *vsb = sb->id;
    जइ (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	जइ (flag) {
	    जइ (vsb->IsHidden()) {
		vsb->Show();
	    }
	} अण्णहा {
	    जइ (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

शून्य
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	पूर्णाङ्क	val,
	पूर्णाङ्क	size,
	पूर्णाङ्क	max)
{
    जइ (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	जइ (max == 0) {
	    s->Set_मानम्(0);
	    s->SetRange(0.0, 0.0);
	} अण्णहा {
	    s->SetProportion((भिन्न)size / (max + 1.0));
	    s->SetSteps(1.0, size > 5 ? size - 2 : size);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - size;
#endif
	    जइ (max < s->मानम्_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->Set_मानम्(val);
		s->SetRange(0.0, max);
	    } अण्णहा {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->Set_मानम्(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

शून्य
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	पूर्णाङ्क	x,
	पूर्णाङ्क	y,
	पूर्णाङ्क	w,
	पूर्णाङ्क	h)
{
    जइ (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	भिन्न vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	जइ (winb.दक्षिणः-(x+w)<w) vsbx = winb.दक्षिणः - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    पूर्णाङ्क
gui_mch_get_scrollbar_xpadding(शून्य)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    प्रत्यागम 0;
}

    पूर्णाङ्क
gui_mch_get_scrollbar_ypadding(शून्य)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    प्रत्यागम 0;
}

शून्य
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	पूर्णाङ्क	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = नूतन VimScrollBar(sb, posture);
    जइ (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#जइ defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
शून्य
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    जइ (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    पूर्णाङ्क
gui_mch_is_blink_off(शून्य)
{
    प्रत्यागम FALSE;
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

स्थिर पूर्णाङ्क	blink_state = BLINK_NONE;
स्थिर long_u	    blink_waittime = 700;
स्थिर long_u	    blink_ontime = 400;
स्थिर long_u	    blink_offtime = 250;
स्थिर पूर्णाङ्क  blink_timer = 0;

शून्य
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
    शून्य
gui_mch_stop_blink(पूर्णाङ्क may_call_gui_update_cursor)
{
    // TODO
    जइ (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    जइ (blink_state == BLINK_OFF)
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
    जइ (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    जइ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
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
पूर्णाङ्क
gui_mch_init_font(
	char_u	    *font_name,
	पूर्णाङ्क	    fontset)
{
    जइ (gui.vimWindow->Lock())
    {
	पूर्णाङ्क rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	प्रत्यागम rc;
    }

    प्रत्यागम FAIL;
}


    पूर्णाङ्क
gui_mch_adjust_charsize()
{
    प्रत्यागम FAIL;
}


    पूर्णाङ्क
gui_mch_font_dialog(font_family* family, font_style* style, भिन्न* size)
{
#जइ defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = नूतन VimSelectFontDialog(family, style, size);
    तार्किक ret = dialog->Go();
	delete dialog;
	प्रत्यागम ret;
#अण्णहा
    प्रत्यागम NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	पूर्णाङ्क	    giveErrorIfMissing)
{
    स्थिर VimFont *fontList = रिक्त;

    जइ (!gui.in_use)	//  can't do this when GUI not running
	प्रत्यागम NOFONT;

    //	storage for locally modified name;
    ध्रुव पूर्णाङ्क buff_size = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 20;
    स्थिर वर्ण font_name[buff_size] = {0};
    font_family family = {0};
    font_style	style  = {0};
    भिन्न size = 0.f;

    जइ (name == 0 && be_fixed_font == 0) {
	जइ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	प्रत्यागम NOFONT;
    }

    तार्किक useSelectGUI = मिच्छ;
    जइ (name != रिक्त)
	जइ (STRCMP(name, "*") == 0) {
	    useSelectGUI = सच्च;
	    STRNCPY(font_name, hl_get_font_name(), buff_size);
	} अण्णहा
	    STRNCPY(font_name, name, buff_size);

    जइ (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	size = be_fixed_font->Size();
	vim_snprintf(font_name, buff_size,
	    (वर्ण*)"%s/%s/%.0f", family, style, size);
    }

    //	replace underscores with spaces
    वर्ण* end = 0;
    जाव (end = strchr((वर्ण *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    स्थिर वर्ण buff[buff_size] = {0};
    STRNCPY(buff, font_name, buff_size);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_LENGTH);
    वर्ण* style_s = strtok(0, "/\0");
    जइ (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_LENGTH);
    size = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    जइ (useSelectGUI) {
	जइ (gui_mch_font_dialog(&family, &style, &size) == NOFONT)
	    प्रत्यागम FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_size,
		(वर्ण*)"%s/%s/%.0f", family, style, size);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	जइ (new_p_guifont != रिक्त) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    प्रति ( ; *new_p_guifont; ++new_p_guifont)
		जइ (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    प्रति (flp = fontList; flp; flp = flp->next) {
	जइ (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    प्रत्यागम (GuiFont)flp;
	}
    }

    VimFont *font = नूतन VimFont();
    font->name = vim_strsave((char_u*)font_name);

    जइ (गणनम्_font_styles(family) <= 0) {
	जइ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	प्रत्यागम NOFONT;
    }

    //	Remember font in the static list for later use
    font->next = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    जइ (size > 0.f)
	font->SetSize(size);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    प्रत्यागम (GuiFont)font;
}

/*
 * Set the current text font.
 */
शून्य
gui_mch_set_font(
	GuiFont	font)
{
    जइ (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (पूर्णाङ्क) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (पूर्णाङ्क)(fh.ascent + 0.9999)
	    + (पूर्णाङ्क)(fh.descent + 0.9999) + (पूर्णाङ्क)(fh.leading + 0.9999);
	gui.char_ascent = (पूर्णाङ्क)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
शून्य
gui_mch_free_font(
	GuiFont	font)
{
    जइ (font == NOFONT)
	प्रत्यागम;
    VimFont *f = (VimFont *)font;
    जइ (--f->refcount <= 0) {
	जइ (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    जइ (name == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    पूर्णाङ्क
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
	gui.char_height = (पूर्णाङ्क)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (पूर्णाङ्क)(fh.ascent + 0.5) + p_linespace / 2;
    }
    प्रत्यागम OK;
}

    शून्य
gui_mch_getmouse(पूर्णाङ्क *x, पूर्णाङ्क *y)
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

    शून्य
gui_mch_mousehide(पूर्णाङ्क hide)
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
    प्रत्यागम gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
शून्य
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    जइ (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_उच्चः_Color(rgb);
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
    जइ (gui.vimWindow->Lock()) {
	gui.vimTextArea->Set_नीचः_Color(rgb);
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
	पूर्णाङ्क	row,
	पूर्णाङ्क	col,
	char_u	*s,
	पूर्णाङ्क	len,
	पूर्णाङ्क	flags)
{
    जइ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(पूर्णाङ्क r, पूर्णाङ्क g, पूर्णाङ्क b)
{
    प्रत्यागम gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
पूर्णाङ्क
gui_mch_haskey(
	char_u	*name)
{
    पूर्णाङ्क i;

    प्रति (i = 0; special_कुञ्चिकाः[i].Be_कुञ्चिकाः != 0; i++)
	जइ (name[0] == special_कुञ्चिकाः[i].vim_code0 &&
		name[1] == special_कुञ्चिकाः[i].vim_code1)
	    प्रत्यागम OK;
    प्रत्यागम FAIL;
}

    शून्य
gui_mch_beep()
{
    ::beep();
}

    शून्य
gui_mch_flash(पूर्णाङ्क msec)
{
    // Do a visual beep by reversing the foreground and background colors

    जइ (gui.vimWindow->Lock()) {
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
	पूर्णाङ्क	r,
	पूर्णाङ्क	c,
	पूर्णाङ्क	nr,
	पूर्णाङ्क	nc)
{
    BRect rect;
    rect.वामः = FILL_X(c);
    rect.शिखरम् = FILL_Y(r);
    rect.दक्षिणः = rect.वामः + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.शिखरम् + nr * gui.char_height - PEN_WIDTH;

    जइ (gui.vimWindow->Lock()) {
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
    जइ (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(सच्च);
	gui.vimWindow->Unlock();
    }
}

#जइ defined(FEAT_EVAL)
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
    जइ (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((वर्ण *)title);
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
    r.वामः = FILL_X(gui.col);
    r.शिखरम् = FILL_Y(gui.row);
    पूर्णाङ्क cells = utf_off2cells(पङ्क्तिः_Offset[gui.row] + gui.col, 100); // TODO-TODO
    जइ (cells>=4) cells = 1;
    r.दक्षिणः = r.वामः + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.शिखरम् + gui.char_height - PEN_WIDTH;

    जइ (gui.vimWindow->Lock()) {
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
	पूर्णाङ्क	w,
	पूर्णाङ्क	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.वामः =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_दक्षिणः ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.दक्षिणः = r.वामः + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.शिखरम् = r.bottom - h + PEN_WIDTH;

    जइ (gui.vimWindow->Lock()) {
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
    जाव (port_गणनम्(gui.vdcmp) > 0 &&
	    !vim_is_निवेशः_buf_full() &&
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
पूर्णाङ्क
gui_mch_wait_for_chars(
	long	wtime)
{
    पूर्णाङ्क		focus;
    bigtime_t	until, timeout;
    status_t	st;

    जइ (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    अण्णहा
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    प्रति (;;)
    {
	// Stop or start blinking when focus changes
	जइ (gui.in_focus != focus)
	{
	    जइ (gui.in_focus)
		gui_mch_start_blink();
	    अण्णहा
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
	जइ (did_add_timer)
	    // Need to recompute the waiting time.
	    भङ्ग;
# endif
# ifdef FEAT_JOB_CHANNEL
	जइ (has_any_channel())
	{
	    जइ (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	अण्णहा जइ (wtime < 0)
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

	जइ (निवेशः_available())
	    प्रत्यागम OK;
	जइ (st < B_OK)		// includes B_TIMED_OUT
	    प्रत्यागम FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	जइ (wtime >= 0)
	{
	    timeout = until - system_time();
	    जइ (timeout < 0)
		भङ्ग;
	}
    }
    प्रत्यागम FAIL;

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
    जइ (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    प्रत्यागम;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
शून्य
gui_mch_clear_block(
	पूर्णाङ्क	row1,
	पूर्णाङ्क	col1,
	पूर्णाङ्क	row2,
	पूर्णाङ्क	col2)
{
    जइ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    शून्य
gui_mch_clear_all()
{
    जइ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
शून्य
gui_mch_delete_पङ्क्तयः(
	पूर्णाङ्क	row,
	पूर्णाङ्क	अङ्कः_पङ्क्तयः)
{
    gui.vimTextArea->mch_Delete_पङ्क्तयः(row, अङ्कः_पङ्क्तयः);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
शून्य
gui_mch_निवेशय_पङ्क्तयः(
	पूर्णाङ्क	row,
	पूर्णाङ्क	अङ्कः_पङ्क्तयः)
{
    gui.vimTextArea->mch_निवेशय_पङ्क्तयः(row, अङ्कः_पङ्क्तयः);
}

#जइ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

शून्य
gui_mch_enable_menu(
	पूर्णाङ्क	flag)
{
    जइ (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

शून्य
gui_mch_set_menu_pos(
	पूर्णाङ्क	x,
	पूर्णाङ्क	y,
	पूर्णाङ्क	w,
	पूर्णाङ्क	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
शून्य
gui_mch_add_menu(
	vimmenu_T   *menu,
	पूर्णाङ्क	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    जइ (menu_is_popup(menu->name) && parent == रिक्त) {
	BPopUpMenu* निष्कासय_Up_Menu = नूतन BPopUpMenu((ध्रुव वर्ण*)menu->name, मिच्छ, मिच्छ);
	menu->submenu_id = निष्कासय_Up_Menu;
	menu->id = रिक्त;
	प्रत्यागम;
    }

    जइ (!menu_is_menubar(menu->name)
	    || (parent != रिक्त && parent->submenu_id == रिक्त))
	प्रत्यागम;

    जइ (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	जइ ( parent )
	    tmp = parent->submenu_id;
	अण्णहा
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	जइ ( ! tmp->FindItem((ध्रुव वर्ण *) menu->dname)) {

	    BMenu *bmenu = नूतन BMenu((वर्ण *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((ध्रुव वर्ण *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    शून्य
gui_mch_toggle_tearoffs(पूर्णाङ्क enable)
{
    // no tearoff menus
}

    स्थिर BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = नूतन BMessage('menu');
    m->AddPointer("VimMenu", (शून्य *)menu);

    प्रत्यागम m;
}

/*
 * Add a menu item to a menu
 */
शून्य
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	पूर्णाङ्क	idx)
{
    पूर्णाङ्क	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    जइ (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	जइ (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    जइ (toolbar != रिक्त) {
		toolbar->AddButton(idx, menu);
	    }
	} अण्णहा
#endif

	जइ (parent->submenu_id != रिक्त || menu_is_popup(parent->name)) {
	    जइ (menu_is_separator(menu->name)) {
		BSeparatorItem *item = नूतन BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = रिक्त;
	    }
	    अण्णहा {
		BMenuItem *item = नूतन BMenuItem((वर्ण *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((वर्ण) menu->mnemonic);
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
    जइ (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	जइ (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    जइ (toolbar != रिक्त) {
		toolbar->RemoveButton(menu);
	    }
	} अण्णहा
#endif
	{
	    प्रतिज्ञा(menu->submenu_id == रिक्त || menu->submenu_id->गणनम्_वस्तूनि() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    जइ (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		जइ (bmenu == gui.vimForm->MenuBar() && bmenu->गणनम्_वस्तूनि() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = रिक्त;
	    menu->submenu_id = रिक्त;

	    gui.menu_height = (पूर्णाङ्क) gui.vimForm->MenuHeight();
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
	पूर्णाङ्क	grey)
{
#ifdef FEAT_TOOLBAR
    जइ (menu->parent && menu_is_toolbar(menu->parent->name)) {
	जइ (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    जइ (toolbar != रिक्त) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } अण्णहा
#endif
    जइ (menu->id != रिक्त)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
शून्य
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	पूर्णाङ्क	hidden)
{
    जइ (menu->id != रिक्त)
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
    जइ (!menu_is_popup(menu->name) || menu->submenu_id == रिक्त)
	प्रत्यागम;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    जइ (popupMenu == रिक्त)
	प्रत्यागम;

    BPoint point;
    जइ (gui.vimWindow->Lock()) {
	uint32 buttons = 0;
	gui.vimTextArea->GetMouse(&point, &buttons);
	gui.vimTextArea->ConvertToScreen(&point);
	gui.vimWindow->Unlock();
    }
    popupMenu->Go(point, सच्च);
}

#endif // FEAT_MENU

// Mouse stuff

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
वर्ण textplain[] = "text/plain";
वर्ण vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    शून्य
clip_mch_request_selection(Clipboard_T *cbd)
{
    जइ (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->Data();
	// m->PrintToStream();

	char_u *string = रिक्त;
	ssize_t stringlen = -1;

	जइ (m->FindData(textplain, B_MIME_TYPE,
		    (ध्रुव शून्य **)&string, &stringlen) == B_OK
		|| m->FindString("text", (ध्रुव वर्ण **)&string) == B_OK)
	{
	    जइ (stringlen == -1)
		stringlen = STRLEN(string);

	    पूर्णाङ्क type;
	    वर्ण *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    जइ (m->FindData(vimselectiontype, B_MIME_TYPE,
			(ध्रुव शून्य **)&seltype, &seltypelen) == B_OK)
	    {
		विकल्प (*seltype)
		{
		    स्वभाव:
		    पक्ष 'L':	type = MLINE;	भङ्ग;
		    पक्ष 'C':	type = MCHAR;	भङ्ग;
#ifdef FEAT_VISUAL
		    पक्ष 'B':	type = MBLOCK;	भङ्ग;
#endif
		}
	    }
	    अण्णहा
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
    शून्य
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    पूर्णाङ्क
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    प्रत्यागम FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    शून्य
clip_mch_set_selection(Clipboard_T *cbd)
{
    जइ (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->Data();
	प्रतिज्ञा(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = रिक्त;
	long_u	गणनम्;
	पूर्णाङ्क type;

	type = clip_convert_selection(&str, &गणनम्, cbd);

	जइ (type < 0)
	    प्रत्यागम;

	m->AddData(textplain, B_MIME_TYPE, (शून्य *)str, गणनम्);

	// Add type of selection
	वर्ण	vtype;
	विकल्प (type)
	{
	    स्वभाव:
	    पक्ष MLINE:    vtype = 'L';    भङ्ग;
	    पक्ष MCHAR:    vtype = 'C';    भङ्ग;
#ifdef FEAT_VISUAL
	    पक्ष MBLOCK:   vtype = 'B';    भङ्ग;
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
	पूर्णाङ्क saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = नूतन BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    रिक्त, रिक्त, 0, मिच्छ,
	    नूतन BMessage((saving == TRUE) ? 'save' : 'open'), रिक्त, सच्च);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((वर्ण*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((ध्रुव वर्ण*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    जाव (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = रिक्त;
    status_t result = gui.vimApp->fBrowsedPath.InitCheck();
    जइ (result == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } अण्णहा
	जइ (result != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    result, strerror(result));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = रिक्त;

    प्रत्यागम fileName;
}
#endif // FEAT_BROWSE


#जइ defined(FEAT_GUI_DIALOG)

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

पूर्णाङ्क
gui_mch_dialog(
	पूर्णाङ्क	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	पूर्णाङ्क	 dfltbutton,
	char_u	*textfield,
	पूर्णाङ्क ex_cmd)
{
    VimDialog *dialog = नूतन VimDialog(type, (वर्ण*)title, (वर्ण*)message,
	    (वर्ण*)buttons, dfltbutton, (वर्ण*)textfield, ex_cmd);
    तार्किक ret = dialog->Go();
    delete dialog;
	प्रत्यागम ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    प्रत्यागम ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    शून्य
gui_mch_setmouse(पूर्णाङ्क x, पूर्णाङ्क y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    शून्य
im_set_position(पूर्णाङ्क row, पूर्णाङ्क col)
{
    जइ (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    प्रत्यागम;
}
#endif

    शून्य
gui_mch_show_toolbar(पूर्णाङ्क showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    शून्य
gui_mch_set_toolbar_pos(पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    जइ (toolbar != रिक्त) {
	जइ (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#जइ defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    शून्य
gui_mch_show_tabline(पूर्णाङ्क showit)
{
    Vim_Tab_पङ्क्तिः *tab_पङ्क्तिः = gui.vimForm->Tab_पङ्क्तिः();

    जइ (tab_पङ्क्तिः == रिक्त)
	प्रत्यागम;

    जइ (!showit != !gui.vimForm->Is_Showing_Tab_पङ्क्तिः()) {
	gui.vimForm->Set_Showing_Tab_पङ्क्तिः(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    शून्य
gui_mch_set_tabline_pos(पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h)
{
    Vim_Tab_पङ्क्तिः *tab_पङ्क्तिः = gui.vimForm->Tab_पङ्क्तिः();
    जइ (tab_पङ्क्तिः != रिक्त) {
	जइ (gui.vimWindow->Lock()) {
	    tab_पङ्क्तिः->MoveTo(x, y);
	    tab_पङ्क्तिः->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    पूर्णाङ्क
gui_mch_showing_tabline()
{
    Vim_Tab_पङ्क्तिः *tab_पङ्क्तिः = gui.vimForm->Tab_पङ्क्तिः();
    प्रत्यागम tab_पङ्क्तिः != रिक्त && gui.vimForm->Is_Showing_Tab_पङ्क्तिः();
}

/*
 * Update the labels of the tabline.
 */
    शून्य
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    पूर्णाङ्क	    nr = 0;
    पूर्णाङ्क	    curtabidx = 0;

    Vim_Tab_पङ्क्तिः *tab_पङ्क्तिः = gui.vimForm->Tab_पङ्क्तिः();

    जइ (tab_पङ्क्तिः == रिक्त)
	प्रत्यागम;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    प्रति (tp = first_tabpage; tp != रिक्त; tp = tp->tp_next, ++nr) {
	जइ (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_पङ्क्तिः->TabAt(nr);

	जइ (tab == रिक्त) {
	    tab = नूतन Vim_Tab_पङ्क्तिः::VimTab();
	    tab_पङ्क्तिः->AddTab(रिक्त, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((ध्रुव वर्ण*)NameBuff);
	tab_पङ्क्तिः->Invalidate();
    }

    // Remove any old labels.
    जाव (nr < tab_पङ्क्तिः->गणनम्_Tabs())
	tab_पङ्क्तिः->RemoveTab(nr);

    जइ (tab_पङ्क्तिः->Selection() != curtabidx)
	tab_पङ्क्तिः->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    शून्य
gui_mch_set_curtab(पूर्णाङ्क nr)
{
    Vim_Tab_पङ्क्तिः *tab_पङ्क्तिः = gui.vimForm->Tab_पङ्क्तिः();
    जइ (tab_पङ्क्तिः == रिक्त)
	प्रत्यागम;

    gui.vimWindow->Lock();

    जइ (tab_पङ्क्तिः->Selection() != nr -1)
	tab_पङ्क्तिः->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
