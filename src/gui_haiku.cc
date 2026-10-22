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

extern "C" {

#include <assert.h>
#include <float.h>
#include <syslog.h>

#include "vim.h"
#include "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#include <Application.h>
#include <Beep.h>
#include <Bitmap.h>
#include <Box.h>
#include <Button.h>
#include <Clipboard.h>
#include <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#include <File.h>
#include <FilePanel.h>
#include <FindDirectory.h>
//#include <Font.h>
#include <IconUtils.h>
#include <Input.h>
#include <ListView.h>
#include <MenuBar.h>
#include <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#include <Path.h>
#include <PictureButton.h>
#include <PopUpMenu.h>
//#include <Region.h>
#include <Resources.h>
//#include <Roster.h>
#include <Screen.h>
#include <ScrollBar.h>
#include <ScrollView.h>
#include <String.h>
#include <StringView.h>
//#include <SupportDefs.h>
#include <TabView.h>
#include <TextControl.h>
#include <TextView.h>
#include <TranslationUtils.h>
#include <TranslatorFormats.h>
#include <View.h>
#include <Window.h>

class VimApp;
class VimFormView;
class VimTextAreaView;
class VimWindow;
class VimToolbar;
class VimTabLine;

extern key_map *keyMap;
extern var_na *keyMapChars;

extern ຈຳນວນເຕັມ main(ຈຳນວນເຕັມ argc, var_na **argv);

#ifndef B_MAX_PORT_COUNT
#define B_MAX_PORT_COUNT    255
#endif

// VimApp seems comparable to the X "vimShell"
class VimApp: public BApplication
{
    typedef BApplication Inherited;
    public:
    VimApp(const var_na *appsig);
    ~VimApp();

    // callbacks:
#ຖ້າ 0
    virtual void DispatchMessage(BMessage *m, BHandler *h)
    {
	m->PrintToStream();
	Inherited::DispatchMessage(m, h);
    }
#endif
    virtual void ReadyToRun();
    virtual void ArgvReceived(int32 argc, var_na **argv);
    virtual void RefsReceived(BMessage *m);
    virtual bool QuitRequested();
    virtual void MessageReceived(BMessage *m);

    static void SendRefs(BMessage *m, bool changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	fBrowsedPath;
    private:
};

class VimWindow: public BWindow
{
    typedef BWindow Inherited;
    public:
    VimWindow();
    ~VimWindow();

    virtual void DispatchMessage(BMessage *m, BHandler *h);
    virtual void WindowActivated(bool active);
    virtual bool QuitRequested();

    VimFormView	    *formView;

    private:
    void init();

    bool is_fullscreen = false;
    BRect saved_frame;
    window_look saved_look;
};

class VimFormView: public BView
{
    typedef BView Inherited;
    public:
    VimFormView(BRect frame);
    ~VimFormView();

    // callbacks:
    virtual void AllAttached();
    virtual void FrameResized(float new_width, float new_height);

#define MENUBAR_MARGIN	1
    float MenuHeight() const
    { ສົ່ງຄືນ menuBar ? menuBar->Frame().Height() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() const
    { ສົ່ງຄືນ menuBar; }

    private:
    void init(BRect);

    BMenuBar	    *menuBar;
    VimTextAreaView *textArea;

#ifdef FEAT_TOOLBAR
    public:
    float ToolbarHeight() const;
    VimToolbar *ToolBar() const
    { ສົ່ງຄືນ toolBar; }
    private:
    VimToolbar	    *toolBar;
#endif

#ifdef FEAT_GUI_TABLINE
    public:
    VimTabLine *TabLine() const	{ ສົ່ງຄືນ tabLine; }
    bool IsShowingTabLine() const { ສົ່ງຄືນ showingTabLine; }
    void SetShowingTabLine(bool showing) { showingTabLine = showing;	}
    float TablineHeight() const;
    private:
    VimTabLine	*tabLine;
    ຈຳນວນເຕັມ	showingTabLine;
#endif
};

class VimTextAreaView: public BView
{
    typedef BView Inherited;
    public:
    VimTextAreaView(BRect frame);
    ~VimTextAreaView();

    // callbacks:
    virtual void Draw(BRect updateRect);
    virtual void KeyDown(const var_na *bytes, int32 numBytes);
    virtual void MouseDown(BPoint point);
    virtual void MouseUp(BPoint point);
    virtual void MouseMoved(BPoint point, uint32 transit, const BMessage *message);
    virtual void MessageReceived(BMessage *m);

    // own functions:
    ຈຳນວນເຕັມ mchInitFont(char_u *name);
    void mchDrawString(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ col, char_u *s, ຈຳນວນເຕັມ len, ຈຳນວນເຕັມ flags);
    void mchClearBlock(ຈຳນວນເຕັມ row1, ຈຳນວນເຕັມ col1, ຈຳນວນເຕັມ row2, ຈຳນວນເຕັມ col2);
    void mchClearAll();
    void mchDeleteLines(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ num_lines);
    void mchInsertLines(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ num_lines);

    static void guiSendMouseEvent(ຈຳນວນເຕັມ button, ຈຳນວນເຕັມ x, ຈຳນວນເຕັມ y, ຈຳນວນເຕັມ repeated_click, int_u modifiers);
    static void guiMouseMoved(ຈຳນວນເຕັມ x, ຈຳນວນເຕັມ y);
    static void guiBlankMouse(bool should_hide);
    static int_u mouseModifiersToVim(int32 beModifiers);

    int32 mouseDragEventCount;

#ifdef FEAT_MBYTE_IME
    void DrawIMString(void);
#endif

    private:
    void init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#ifdef FEAT_MBYTE_IME
    sa_nracanaa {
	BMessenger* messenger;
	BMessage* message;
	BPoint location;
	ຈຳນວນເຕັມ row;
	ຈຳນວນເຕັມ col;
	ຈຳນວນເຕັມ count;
    } IMData;
#endif
};

class VimScrollBar: public BScrollBar
{
    typedef BScrollBar Inherited;
    public:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    virtual void ValueChanged(float newValue);
    virtual void MouseUp(BPoint where);
    void SetValue(float newval);
    scrollbar_T *getGsb()
    { ສົ່ງຄືນ gsb; }

    int32	scrollEventCount;

    private:
    scrollbar_T *gsb;
    float   ignoreValue;
};


#ifdef FEAT_TOOLBAR

class VimToolbar : public BBox
{
    static BBitmap *normalButtonsBitmap;
    static BBitmap *grayedButtonsBitmap;

    BBitmap *LoadVimBitmap(const var_na* fileName);
    bool GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, bool pressed);
    bool ModifyBitmapToGrayed(BBitmap *bitmap);

    BList fButtonsList;
    void InvalidateLayout();

    public:
    VimToolbar(BRect frame, const var_na * name);
    ~VimToolbar();

    bool PrepareButtonBitmaps();

    bool AddButton(int32 index, vimmenu_T *menu);
    bool RemoveButton(vimmenu_T *menu);
    bool GrayButton(vimmenu_T *menu, ຈຳນວນເຕັມ grey);

    float ToolbarHeight() const;
    virtual void AttachedToWindow();
};

BBitmap *VimToolbar::normalButtonsBitmap  = NULL;
BBitmap *VimToolbar::grayedButtonsBitmap  = NULL;

const float ToolbarMargin = 3.;
const float ButtonMargin  = 3.;

#endif //FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE

class VimTabLine : public BTabView
{
    public:
	class VimTab : public BTab {
	    public:
		VimTab() : BTab(new BView(BRect(), "-Empty-", 0, 0)) {}

	    virtual void Select(BView* owner);
	};

	VimTabLine(BRect r) : BTabView(r, "vimTabLine", B_WIDTH_FROM_LABEL,
	       B_FOLLOW_LEFT | B_FOLLOW_TOP | B_FOLLOW_RIGHT, B_WILL_DRAW | B_FRAME_EVENTS) {}

    float TablineHeight() const;
    virtual void MouseDown(BPoint point);
};

#endif //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
class VimFont: public BFont
{
    typedef BFont Inherited;
    public:
    VimFont();
    VimFont(const VimFont *rhs);
    VimFont(const BFont *rhs);
    VimFont(const VimFont &rhs);
    ~VimFont();

    VimFont *next;
    ຈຳນວນເຕັມ refcount;
    char_u *name;

    private:
    void init();
};

#ຖ້າ defined(FEAT_GUI_DIALOG)

class VimDialog : public BWindow
{
    typedef BWindow Inherited;

    BButton* _CreateButton(int32 which, const var_na* label);

    public:

    class View : public BView {
	typedef BView Inherited;

	public:
	View(BRect frame);
	~View();

	virtual void Draw(BRect updateRect);
	void InitIcon(int32 type);

	private:
	BBitmap*    fIconBitmap;
    };

    VimDialog(ຈຳນວນເຕັມ type, const var_na *title, const var_na *message,
	    const var_na *buttons, ຈຳນວນເຕັມ dfltbutton, const var_na *textfield,
	    ຈຳນວນເຕັມ ex_cmd);
    ~VimDialog();

    ຈຳນວນເຕັມ Go();

    virtual void MessageReceived(BMessage *msg);

    private:
    sem_id	    fDialogSem;
    ຈຳນວນເຕັມ		    fDialogValue;
    BList	    fButtonsList;
    BTextView*	    fMessageView;
    BTextControl*   fInputControl;
    const var_na*	    fInputValue;
};

class VimSelectFontDialog : public BWindow
{
    typedef BWindow Inherited;

    void _CleanList(BListView* list);
    void _UpdateFontStyles();
    void _UpdateSizeInputPreview();
    void _UpdateFontPreview();
    bool _UpdateFromListItem(BListView* list, var_na* text, ຈຳນວນເຕັມ textSize);
    public:

    VimSelectFontDialog(font_family* family, font_style* style, float* size);
    ~VimSelectFontDialog();

    bool Go();

    virtual void MessageReceived(BMessage *msg);

    private:
    status_t	    fStatus;
    sem_id	    fDialogSem;
    bool	    fDialogValue;
    font_family*    fFamily;
    font_style*	    fStyle;
    float*	    fSize;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    float	    fFontSize;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   fSizesInput;
};

#endif // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

sa_nracanaa MainArgs {
    ຈຳນວນເຕັມ	     argc;
    var_na    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#define	KEY_MSG_BUFSIZ	7
#ຖ້າ KEY_MSG_BUFSIZ < MAX_KEY_CODE_LEN
#error Increase KEY_MSG_BUFSIZ!
#endif

sa_nracanaa VimKeyMsg {
    char_u  length;
    char_u  chars[KEY_MSG_BUFSIZ];  // contains Vim encoding
    bool    csi_escape;
};

sa_nracanaa VimResizeMsg {
    ຈຳນວນເຕັມ	    width;
    ຈຳນວນເຕັມ	    height;
};

sa_nracanaa VimScrollBarMsg {
    VimScrollBar *sb;
    long    value;
    ຈຳນວນເຕັມ	    stillDragging;
};

sa_nracanaa VimMenuMsg {
    vimmenu_T	*guiMenu;
};

sa_nracanaa VimMouseMsg {
    ຈຳນວນເຕັມ	    button;
    ຈຳນວນເຕັມ	    x;
    ຈຳນວນເຕັມ	    y;
    ຈຳນວນເຕັມ	    repeated_click;
    int_u   modifiers;
};

sa_nracanaa VimMouseMovedMsg {
    ຈຳນວນເຕັມ	    x;
    ຈຳນວນເຕັມ	    y;
};

sa_nracanaa VimFocusMsg {
    bool    active;
};

sa_nracanaa VimRefsMsg {
    BMessage   *message;
    bool    changedir;
};

sa_nracanaa VimTablineMsg {
    ຈຳນວນເຕັມ	    index;
};

sa_nracanaa VimTablineMenuMsg {
    ຈຳນວນເຕັມ	    index;
    ຈຳນວນເຕັມ	    event;
};

sa_nracanaa VimMsg {
    ga_nanaa VimMsgType {
	Key, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    sa_ngha {
	sa_nracanaa VimKeyMsg    Key;
	sa_nracanaa VimResizeMsg NewSize;
	sa_nracanaa VimScrollBarMsg	Scroll;
	sa_nracanaa VimMenuMsg   Menu;
	sa_nracanaa VimMouseMsg  Mouse;
	sa_nracanaa VimMouseMovedMsg	MouseMoved;
	sa_nracanaa VimFocusMsg  Focus;
	sa_nracanaa VimRefsMsg   Refs;
	sa_nracanaa VimTablineMsg	Tabline;
	sa_nracanaa VimTablineMenuMsg    TablineMenu;
    } u;
};

#define RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#define GUI_TO_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

static sa_nracanaa specialkey
{
    uint16  BeKeys;
#define KEY(a,b)    ((a)<<8|(b))
#define K(a)	    KEY(0,a)		// for ASCII codes
#define F(b)	    KEY(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_keys[] =
{
    {K(B_UP_ARROW),	'k', 'u'},
    {K(B_DOWN_ARROW),	    'k', 'd'},
    {K(B_LEFT_ARROW),	    'k', 'l'},
    {K(B_RIGHT_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_INSERT),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_END),		'@', '7'},
    {K(B_PAGE_UP),	'k', 'P'},	// XK_Prior
    {K(B_PAGE_DOWN),	    'k', 'N'},	    // XK_Next,

#define FIRST_FUNCTION_KEY  11
    {F(B_F1_KEY),	'k', '1'},
    {F(B_F2_KEY),	'k', '2'},
    {F(B_F3_KEY),	'k', '3'},
    {F(B_F4_KEY),	'k', '4'},
    {F(B_F5_KEY),	'k', '5'},
    {F(B_F6_KEY),	'k', '6'},
    {F(B_F7_KEY),	'k', '7'},
    {F(B_F8_KEY),	'k', '8'},
    {F(B_F9_KEY),	'k', '9'},
    {F(B_F10_KEY),	'k', ';'},

    {F(B_F11_KEY),	'F', '1'},
    {F(B_F12_KEY),	'F', '2'},
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
    {F(B_PRINT_KEY),	    '%', '9'},

#ຖ້າ 0
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

#define NUM_SPECIAL_KEYS    ARRAY_LENGTH(special_keys)

// ---------------- VimApp ----------------

    static void
docd(BPath &path)
{
    mch_chdir((var_na *)path.Path());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	static void
drop_callback(void *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	static void
RefsReceived(BMessage *m, bool changedir)
{
    uint32 type;
    int32 count;

    m->PrintToStream();
    switch (m->what) {
	pak_sa B_REFS_RECEIVED:
	pak_sa B_SIMPLE_DATA:
	    m->GetInfo("refs", &type, &count);
	    ຖ້າ (type != B_REF_TYPE)
		goto bad;
	    bha_nga;
	pak_sa B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &count);
	    ຖ້າ (type != B_STRING_TYPE)
		goto bad;
	    ຖ້າ (changedir) {
		var_na *dirname;
		ຖ້າ (m->FindString("cwd", (const var_na **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    bha_nga;
	default:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    delete m;
	    ສົ່ງຄືນ;
    }

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    char_u  **fnames;
    fnames = (char_u **) alloc(count * parimaa_na(char_u *));
    ຈຳນວນເຕັມ fname_index = 0;

    switch (m->what) {
	pak_sa B_REFS_RECEIVED:
	pak_sa B_SIMPLE_DATA:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < count; ++i)
	    {
		entry_ref ref;
		ຖ້າ (m->FindRef("refs", i, &ref) == B_OK) {
		    BEntry entry(&ref, false);
		    BPath path;
		    entry.GetPath(&path);

		    // Change to parent directory?
		    ຖ້າ (changedir) {
			BPath parentpath;
			path.GetParent(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    ຖ້າ (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			ຖ້າ (!changedir)
			    docd(path);
		    } ບໍ່ດັ່ງນັ້ນ {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)path.Path(), IObuff);
			ຖ້າ (fname == NULL)
			    fname = (char_u *)path.Path();
			fnames[fname_index++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = false;
		}
	    }
	    bha_nga;
	pak_sa B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    ສຳລັບ (ຈຳນວນເຕັມ i = 1; i < count; ++i)
	    {
		var_na *fname;

		ຖ້າ (m->FindString("argv", i, (const var_na **) &fname) == B_OK) {
		    fnames[fname_index++] = vim_strsave((char_u *)fname);
		}
	    }
	    bha_nga;
	default:
	    // fprintf(stderr, "case default\n");
	    bha_nga;
    }

    delete m;

    // Handle the drop, :edit to get to the file
    ຖ້າ (fname_index > 0) {
	handle_drop(fname_index, fnames, FALSE, drop_callback, NULL);

	setcursor();
	out_flush();
    } ບໍ່ດັ່ງນັ້ນ {
	vim_free(fnames);
    }
}

VimApp::VimApp(const var_na *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(NULL)
{
}

VimApp::~VimApp()
{
}

    void
VimApp::ReadyToRun()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    void
VimApp::ArgvReceived(int32 arg_argc, var_na **arg_argv)
{
    ຖ້າ (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	ຖ້າ (gui.vimWindow)
	    gui.vimWindow->Minimize(false);
	BMessage *m = CurrentMessage();
	DetachCurrentMessage();
	SendRefs(m, true);
    }
}

    void
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    ຈຳນວນເຕັມ limit = 15;
    ໃນຂະນະທີ່ (--limit >= 0 && (curbuf == NULL || curbuf->b_start_ffc == 0))
	snooze(100000);    //  0.1 s
    ຖ້າ (gui.vimWindow)
	gui.vimWindow->Minimize(false);
    DetachCurrentMessage();
    SendRefs(m, true);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    void
VimApp::SendRefs(BMessage *m, bool changedir)
{
    VimRefsMsg rm;
    rm.message = m;
    rm.changedir = changedir;

    write_port(gui.vdcmp, VimMsg::Refs, &rm, parimaa_na(rm));
    //	calls ::RefsReceived
}

    void
VimApp::MessageReceived(BMessage *m)
{
    switch (m->what) {
	pak_sa 'save':
	    {
		entry_ref refDirectory;
		m->FindRef("directory", &refDirectory);
		fBrowsedPath.SetTo(&refDirectory);
		BString strName;
		m->FindString("name", &strName);
		fBrowsedPath.Append(strName.String());
	    }
	    bha_nga;
	pak_sa 'open':
	    {
		entry_ref ref;
		m->FindRef("refs", &ref);
		fBrowsedPath.SetTo(&ref);
	    }
	    bha_nga;
	pak_sa B_CANCEL:
	    {
		BFilePanel *panel;
		m->FindPointer("source", (void**)&panel);
		ຖ້າ (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    bha_nga;
	default:
	    Inherited::MessageReceived(m);
	    bha_nga;
    }
}

    bool
VimApp::QuitRequested()
{
    (void)Inherited::QuitRequested();
    ສົ່ງຄືນ false;
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
    ຖ້າ (formView) {
	RemoveChild(formView);
	delete formView;
    }
    gui.vimWindow = NULL;
}

    void
VimWindow::init()
{
    // Attach the VimFormView
    formView = new VimFormView(Bounds());
    ຖ້າ (formView != NULL) {
	AddChild(formView);
    }
}

#ຖ້າ 0  //  disabled in zeta patch
    void
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    switch (m->what) {
	pak_sa B_MOUSE_UP:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		ຖ້າ (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->FindPoint("where", &where);
		    v->MouseUp(where);
		} ບໍ່ດັ່ງນັ້ນ {
		    Inherited::DispatchMessage(m, h);
		}
	    }
	    bha_nga;
	default:
	    Inherited::DispatchMessage(m, h);
    }
}
#endif

    void
VimWindow::WindowActivated(bool active)
{
    Inherited::WindowActivated(active);
    // the textArea gets the keyboard action
    ຖ້າ (active && gui.vimTextArea)
	gui.vimTextArea->MakeFocus(true);

    sa_nracanaa VimFocusMsg fm;
    fm.active = active;

    write_port(gui.vdcmp, VimMsg::Focus, &fm, parimaa_na(fm));
}

    bool
VimWindow::QuitRequested()
{
    sa_nracanaa VimKeyMsg km;
    km.length = 5;
    memcpy((var_na *)km.chars, "\033:qa\r", km.length);
    km.csi_escape = false;
    write_port(gui.vdcmp, VimMsg::Key, &km, parimaa_na(km));
    ສົ່ງຄືນ false;
}
    void
VimWindow::DispatchMessage(BMessage *m, BHandler *h)
{
    bool should_propagate = true;

    switch (m->what)
    {
	pak_sa B_KEY_DOWN:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->FindInt32("raw_char", &scancode);
		m->FindInt32("modifiers", &beModifiers);

		ຖ້າ (scancode == B_ENTER && (beModifiers & B_LEFT_COMMAND_KEY))
		    {
			should_propagate = false;
			ຖ້າ (this->is_fullscreen)
			    {
				this->is_fullscreen = false;
				ResizeTo(this->saved_frame.Width(), this->saved_frame.Height());
				MoveTo(this->saved_frame.left, this->saved_frame.top);
				SetLook(this->saved_look);
				SetFlags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } ບໍ່ດັ່ງນັ້ນ {
				this->saved_frame = Frame();
				this->saved_look = Look();
				this->is_fullscreen = true;
				BScreen s(this);
				SetLook(B_NO_BORDER_WINDOW_LOOK);
				ResizeTo(s.Frame().Width() + 1, s.Frame().Height() + 1);
				MoveTo(s.Frame().left, s.Frame().top);
				SetFlags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    ຖ້າ (should_propagate)
	Inherited::DispatchMessage(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect frame):
    BView(frame, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_FRAME_EVENTS),
    menuBar(NULL),
#ifdef FEAT_TOOLBAR
    toolBar(NULL),
#endif
#ifdef FEAT_GUI_TABLINE
//  showingTabLine(false),
    tabLine(NULL),
#endif
    textArea(NULL)
{
    init(frame);
}

VimFormView::~VimFormView()
{
    ຖ້າ (menuBar) {
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
    delete tabLine;
#endif

    ຖ້າ (textArea) {
	RemoveChild(textArea);
	delete textArea;
    }
    gui.vimForm = NULL;
}

    void
VimFormView::init(BRect frame)
{
    menuBar = new BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    AddChild(menuBar);

#ifdef FEAT_TOOLBAR
    toolBar = new VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    AddChild(toolBar);
#endif

#ifdef FEAT_GUI_TABLINE
    tabLine = new VimTabLine(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    AddChild(tabLine);
#endif

    BRect remaining = frame;
    textArea = new VimTextAreaView(remaining);
    AddChild(textArea);
    // The textArea will be resized later when menus are added

    gui.vimForm = this;
}

#ifdef FEAT_TOOLBAR
    float
VimFormView::ToolbarHeight() const
{
    ສົ່ງຄືນ toolBar ? toolBar->ToolbarHeight() : 0.;
}
#endif

#ifdef FEAT_GUI_TABLINE
    float
VimFormView::TablineHeight() const
{
    ສົ່ງຄືນ (tabLine && IsShowingTabLine()) ? tabLine->TablineHeight() : 0.;
}
#endif

    void
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    ຖ້າ (menuBar && textArea) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect remaining = Bounds();

#ifdef FEAT_MENU
	remaining.top += MenuHeight();
	menuBar->ResizeTo(remaining.right, remaining.top);
	gui.menu_height = (ຈຳນວນເຕັມ) MenuHeight();
#endif

#ifdef FEAT_TOOLBAR
	toolBar->MoveTo(remaining.left, remaining.top);
	toolBar->ResizeTo(remaining.right, ToolbarHeight());
	remaining.top += ToolbarHeight();
	gui.toolbar_height = ToolbarHeight();
#endif

#ifdef FEAT_GUI_TABLINE
	tabLine->MoveTo(remaining.left, remaining.top);
	tabLine->ResizeTo(remaining.right + 1, TablineHeight());
	remaining.top += TablineHeight();
	gui.tabline_height = TablineHeight();
#endif

	textArea->ResizeTo(remaining.Width(), remaining.Height());
	textArea->MoveTo(remaining.left, remaining.top);
    }


    Inherited::AllAttached();
}

    void
VimFormView::FrameResized(float new_width, float new_height)
{
    sa_nracanaa VimResizeMsg sm;
    ຈຳນວນເຕັມ adjust_h, adjust_w;

    new_width += 1;	//  adjust from width to number of pixels occupied
    new_height += 1;

    sm.width = (ຈຳນວນເຕັມ) new_width;
    sm.height = (ຈຳນວນເຕັມ) new_height;
    adjust_w = ((ຈຳນວນເຕັມ)new_width - gui_get_base_width()) % gui.char_width;
    adjust_h = ((ຈຳນວນເຕັມ)new_height - gui_get_base_height()) % gui.char_height;

    ຖ້າ (adjust_w > 0 || adjust_h > 0) {
	sm.width  -= adjust_w;
	sm.height -= adjust_h;
    }

    write_port(gui.vdcmp, VimMsg::Resize, &sm, parimaa_na(sm));
    //	calls gui_resize_shell(new_width, new_height);

    ສົ່ງຄືນ;

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
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_INPUT_METHOD_AWARE
#ບໍ່ດັ່ງນັ້ນ
	B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
#endif
	),
    mouseDragEventCount(0)
{
#ifdef FEAT_MBYTE_IME
    IMData.messenger = NULL;
    IMData.message = NULL;
#endif
    init(frame);
}

VimTextAreaView::~VimTextAreaView()
{
    gui.vimTextArea = NULL;
}

    void
VimTextAreaView::init(BRect frame)
{
    // set up global var for fast access
    gui.vimTextArea = this;

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

    void
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
    SetLowColor(rgb);
    FillRect(updateRect, B_SOLID_LOW);
    gui_redraw((ຈຳນວນເຕັມ) updateRect.left, (ຈຳນວນເຕັມ) updateRect.top,
	    (ຈຳນວນເຕັມ) (updateRect.Width() + PEN_WIDTH), (ຈຳນວນເຕັມ) (updateRect.Height() + PEN_WIDTH));

    // Clear the border areas if needed
    SetLowColor(rgb);

    ຖ້າ (updateRect.left < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.left, updateRect.top,
		    FILL_X(0)-PEN_WIDTH, updateRect.bottom), B_SOLID_LOW);
    ຖ້າ (updateRect.top < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.left, updateRect.top,
		    updateRect.right, FILL_Y(0)-PEN_WIDTH), B_SOLID_LOW);
    ຖ້າ (updateRect.right >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((ຈຳນວນເຕັມ)Columns), updateRect.top,
		    updateRect.right, updateRect.bottom), B_SOLID_LOW);
    ຖ້າ (updateRect.bottom >= FILL_Y(Rows))   //  bottom border
	FillRect(BRect(updateRect.left, FILL_Y((ຈຳນວນເຕັມ)Rows),
		    updateRect.right, updateRect.bottom), B_SOLID_LOW);

#ifdef FEAT_MBYTE_IME
    DrawIMString();
#endif
}

    void
VimTextAreaView::KeyDown(const var_na *bytes, int32 numBytes)
{
    sa_nracanaa VimKeyMsg km;
    char_u *dest = km.chars;

    bool canHaveVimModifiers = false;

    BMessage *msg = Window()->CurrentMessage();
    assert(msg);
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
    ຖ້າ (numBytes > 1) {
	// This cannot be a special key
	ຖ້າ (numBytes > KEY_MSG_BUFSIZ)
	    numBytes = KEY_MSG_BUFSIZ;	    //	should never happen... ???
	km.length = numBytes;
	memcpy((var_na *)dest, bytes, numBytes);
	km.csi_escape = true;
    } ບໍ່ດັ່ງນັ້ນ {
	int32 scancode = 0;
	msg->FindInt32("key", &scancode);

	int32 beModifiers = 0;
	msg->FindInt32("modifiers", &beModifiers);

	char_u string[3];
	ຈຳນວນເຕັມ len = 0;
	km.length = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	assert(B_BACKSPACE <= 0x20);
	assert(B_DELETE == 0x7F);
	ຖ້າ (((char_u)bytes[0] <= 0x20 || (char_u)bytes[0] == 0x7F) &&
		numBytes == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    ຖ້າ (beModifiers & B_CONTROL_KEY) {
		ຈຳນວນເຕັມ index = keyMap->normal_map[scancode];
		ຈຳນວນເຕັມ newNumBytes = keyMapChars[index];
		char_u *newBytes = (char_u *)&keyMapChars[index + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		ຖ້າ (newNumBytes != 1 || (*newBytes > 0x20 &&
			    *newBytes != 0x7F )) {
		    goto notspecial;
		}
		bytes = (var_na *)newBytes;
	    }
	    canHaveVimModifiers = true;

	    uint16 beoskey;
	    ຈຳນວນເຕັມ first, last;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    ຖ້າ (numBytes == 0 || bytes[0] == B_FUNCTION_KEY) {
		beoskey = F(scancode);
		first = FIRST_FUNCTION_KEY;
		last = NUM_SPECIAL_KEYS;
	    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (*bytes == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		first = last = 0;
	    } ບໍ່ດັ່ງນັ້ນ {
		beoskey = K(bytes[0]);
		first = 0;
		last = FIRST_FUNCTION_KEY;
	    }

	    ສຳລັບ (ຈຳນວນເຕັມ i = first; i < last; i++) {
		ຖ້າ (special_keys[i].BeKeys == beoskey) {
		    string[0] = CSI;
		    string[1] = special_keys[i].vim_code0;
		    string[2] = special_keys[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	ຖ້າ (len == 0) {
	    string[0] = bytes[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#ຖ້າ 0
	ຖ້າ (len == 3 ||
		bytes[0] == B_SPACE || bytes[0] == B_TAB ||
		bytes[0] == B_RETURN || bytes[0] == '\r' ||
		bytes[0] == B_ESCAPE)
#ບໍ່ດັ່ງນັ້ນ
	    ຖ້າ (canHaveVimModifiers)
#endif
	    {
		ຈຳນວນເຕັມ modifiers;
		modifiers = 0;
		ຖ້າ (beModifiers & B_SHIFT_KEY)
		    modifiers |= MOD_MASK_SHIFT;
		ຖ້າ (beModifiers & B_CONTROL_KEY)
		    modifiers |= MOD_MASK_CTRL;
		ຖ້າ (beModifiers & B_OPTION_KEY)
		    modifiers |= MOD_MASK_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		ຈຳນວນເຕັມ key;
		ຖ້າ (string[0] == CSI && len == 3)
		    key = TO_SPECIAL(string[1], string[2]);
		ບໍ່ດັ່ງນັ້ນ
		    key = string[0];
		key = simplify_key(key, &modifiers);
		ຖ້າ (IS_SPECIAL(key))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(key);
		    string[2] = K_THIRD(key);
		    len = 3;
		}
		ບໍ່ດັ່ງນັ້ນ
		{
		    string[0] = key;
		    len = 1;
		}

		ຖ້າ (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.length = 3;
		}
	    }
	memcpy((var_na *)dest, string, len);
	km.length += len;
	km.csi_escape = false;
    }

    write_port(gui.vdcmp, VimMsg::Key, &km, parimaa_na(km));

    /*
     * blank out the pointer if necessary
     */
    ຖ້າ (p_mh && !gui.pointer_hidden)
    {
	guiBlankMouse(true);
	gui.pointer_hidden = TRUE;
    }
}
void
VimTextAreaView::guiSendMouseEvent(
	ຈຳນວນເຕັມ	button,
	ຈຳນວນເຕັມ	x,
	ຈຳນວນເຕັມ	y,
	ຈຳນວນເຕັມ	repeated_click,
	int_u	modifiers)
{
    VimMouseMsg mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_port(gui.vdcmp, VimMsg::Mouse, &mm, parimaa_na(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ຖ້າ (gui.pointer_hidden)
    {
	guiBlankMouse(false);
	gui.pointer_hidden = FALSE;
    }
}

void
VimTextAreaView::guiMouseMoved(
	ຈຳນວນເຕັມ	x,
	ຈຳນວນເຕັມ	y)
{
    VimMouseMovedMsg mm;

    mm.x = x;
    mm.y = y;

    write_port(gui.vdcmp, VimMsg::MouseMoved, &mm, parimaa_na(mm));

    ຖ້າ (gui.pointer_hidden)
    {
	guiBlankMouse(false);
	gui.pointer_hidden = FALSE;
    }
}

    void
VimTextAreaView::guiBlankMouse(bool should_hide)
{
    ຖ້າ (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } ບໍ່ດັ່ງນັ້ນ {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
VimTextAreaView::mouseModifiersToVim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    ຖ້າ (beModifiers & B_SHIFT_KEY)
	vim_modifiers |= MOUSE_SHIFT;
    ຖ້າ (beModifiers & B_CONTROL_KEY)
	vim_modifiers |= MOUSE_CTRL;
    ຖ້າ (beModifiers & B_OPTION_KEY)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    ສົ່ງຄືນ vim_modifiers;
}

    void
VimTextAreaView::MouseDown(BPoint point)
{
    BMessage *m = Window()->CurrentMessage();
    assert(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    ຈຳນວນເຕັມ vimButton;

    ຖ້າ (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_LEFT;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_RIGHT;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_MIDDLE;
    ບໍ່ດັ່ງນັ້ນ
	ສົ່ງຄືນ;		// Unknown button

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

    void
VimTextAreaView::MouseUp(BPoint point)
{
    vimMouseButton = 0;

    BMessage *m = Window()->CurrentMessage();
    assert(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->FindInt32("modifiers", &modifiers);

    vimMouseModifiers = mouseModifiersToVim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, point.x, point.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::MouseUp(point);
}

    void
VimTextAreaView::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ຖ້າ (gui.pointer_hidden)
    {
	guiBlankMouse(false);
	gui.pointer_hidden = FALSE;
    }

    ຖ້າ (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(point.x, point.y);
	ສົ່ງຄືນ;
    }

    atomic_add(&mouseDragEventCount, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, point.x, point.y, 0, vimMouseModifiers);
}

    void
VimTextAreaView::MessageReceived(BMessage *m)
{
    switch (m->what) {
	pak_sa 'menu':
	    {
		VimMenuMsg mm;
		mm.guiMenu = NULL;  // in case no pointer in msg
		m->FindPointer("VimMenu", (void **)&mm.guiMenu);
		write_port(gui.vdcmp, VimMsg::Menu, &mm, parimaa_na(mm));
	    }
	    bha_nga;
	pak_sa B_MOUSE_WHEEL_CHANGED:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		float small=0, big=0, dy=0;
		m->FindFloat("be:wheel_delta_y", &dy);
		scb->GetSteps(&small, &big);
		scb->SetValue(scb->Value()+small*dy*3);
		scb->ValueChanged(scb->Value());
#ຖ້າ 0
		scb = curwin->w_scrollbars[0].id;
		scb->GetSteps(&small, &big);
		scb->SetValue(scb->Value()+small*dy);
		scb->ValueChanged(scb->Value());
#endif
	    }
	    bha_nga;
#ifdef FEAT_MBYTE_IME
	pak_sa B_INPUT_METHOD_EVENT:
	    {
		int32 opcode;
		m->FindInt32("be:opcode", &opcode);
		switch(opcode)
		{
		    pak_sa B_INPUT_METHOD_STARTED:
			ຖ້າ (!IMData.messenger) delete IMData.messenger;
			IMData.messenger = new BMessenger();
			m->FindMessenger("be:reply_to", IMData.messenger);
			bha_nga;
		    pak_sa B_INPUT_METHOD_CHANGED:
			{
			    BString str;
			    bool confirmed;
			    ຖ້າ (IMData.message) *(IMData.message) = *m;
			    ບໍ່ດັ່ງນັ້ນ	       IMData.message = new BMessage(*m);
			    DrawIMString();
			    m->FindBool("be:confirmed", &confirmed);
			    ຖ້າ (confirmed)
			    {
				m->FindString("be:string", &str);
				char_u *chars = (char_u*)str.String();
				sa_nracanaa VimKeyMsg km;
				km.csi_escape = true;
				ຈຳນວນເຕັມ clen;
				ຈຳນວນເຕັມ i = 0;
				ໃນຂະນະທີ່ (i < str.Length())
				{
				    clen = utf_ptr2len(chars+i);
				    memcpy(km.chars, chars+i, clen);
				    km.length = clen;
				    write_port(gui.vdcmp, VimMsg::Key, &km, parimaa_na(km));
				    i += clen;
				}
			    }
			}
			bha_nga;
		    pak_sa B_INPUT_METHOD_LOCATION_REQUEST:
			{
			    BMessage msg(B_INPUT_METHOD_EVENT);
			    msg.AddInt32("be:opcode", B_INPUT_METHOD_LOCATION_REQUEST);
			    msg.AddPoint("be:location_reply", IMData.location);
			    msg.AddFloat("be:height_reply", FILL_Y(1));
			    IMData.messenger->SendMessage(&msg);
			}
			bha_nga;
		    pak_sa B_INPUT_METHOD_STOPPED:
			delete IMData.messenger;
			delete IMData.message;
			IMData.messenger = NULL;
			IMData.message = NULL;
			bha_nga;
		}
	    }
	    // TODO: sz: break here???
#endif
	default:
	    ຖ້າ (m->WasDropped()) {
		BWindow *w = Window();
		w->DetachCurrentMessage();
		w->Minimize(false);
		VimApp::SendRefs(m, (modifiers() & B_SHIFT_KEY) != 0);
	    } ບໍ່ດັ່ງນັ້ນ {
		Inherited::MessageReceived(m);
	    }
	    bha_nga;
    }
}

    ຈຳນວນເຕັມ
VimTextAreaView::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    ຖ້າ (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_set_font((GuiFont)newFont);
	ຖ້າ (name && STRCMP(name, "*") != 0)
	    hl_set_font_name(name);

	SetDrawingMode(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	ສົ່ງຄືນ OK;
    }
    ສົ່ງຄືນ FAIL;
}

    void
VimTextAreaView::mchDrawString(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ col, char_u *s, ຈຳນວນເຕັມ len, ຈຳນວນເຕັມ flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    ຖ້າ (!(flags & DRAW_TRANSP)) {
	ຈຳນວນເຕັມ cells;
	cells = 0;
	ສຳລັບ (ຈຳນວນເຕັມ i=0; i<len; i++) {
	    ຈຳນວນເຕັມ cn = utf_ptr2cells((char_u *)(s+i));
	    ຖ້າ (cn<4) cells += cn;
	}

	BRect r(FILL_X(col), FILL_Y(row),
		FILL_X(col + cells) - PEN_WIDTH, FILL_Y(row + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_LOW);
    }

    BFont font;
    this->GetFont(&font);
    ຖ້າ (!font.IsFixed())
    {
	var_na* p = (var_na*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	ຈຳນວນເຕັມ cells;
	ໃນຂະນະທີ່ ((p - (var_na*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.Set(TEXT_X(col+lastpos), TEXT_Y(row));
	    DrawString(p, clen, where);
	    ຖ້າ (flags & DRAW_BOLD) {
		where.x += 1.0;
		SetDrawingMode(B_OP_BLEND);
		DrawString(p, clen, where);
		SetDrawingMode(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    ຖ້າ (cells<4) lastpos += cells;
	    ບໍ່ດັ່ງນັ້ນ	lastpos++;
	    p += clen;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	BPoint where(TEXT_X(col), TEXT_Y(row));
	DrawString((var_na*)s, len, where);
	ຖ້າ (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    SetDrawingMode(B_OP_BLEND);
	    DrawString((var_na*)s, len, where);
	    SetDrawingMode(B_OP_COPY);
	}
    }

    ຖ້າ (flags & DRAW_UNDERL) {
	ຈຳນວນເຕັມ cells;
	cells = 0;
	ສຳລັບ (ຈຳນວນເຕັມ i=0; i<len; i++) {
	    ຈຳນວນເຕັມ cn = utf_ptr2cells((char_u *)(s+i));
	    ຖ້າ (cn<4) cells += cn;
	}

	BPoint start(FILL_X(col), FILL_Y(row + 1) - PEN_WIDTH);
	BPoint end(FILL_X(col + cells) - PEN_WIDTH, start.y);

	StrokeLine(start, end);
    }
}

void
VimTextAreaView::mchClearBlock(
	ຈຳນວນເຕັມ	row1,
	ຈຳນວນເຕັມ	col1,
	ຈຳນວນເຕັມ	row2,
	ຈຳນວນເຕັມ	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_WIDTH, FILL_Y(row2 + 1) - PEN_WIDTH);
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(r, B_SOLID_LOW);
}

    void
VimTextAreaView::mchClearAll()
{
    gui_mch_set_bg_color(gui.back_pixel);
    FillRect(Bounds(), B_SOLID_LOW);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    void
VimTextAreaView::mchDeleteLines(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ num_lines)
{
    BRect source, dest;
    source.left = FILL_X(gui.scroll_region_left);
    source.top = FILL_Y(row + num_lines);
    source.right = FILL_X(gui.scroll_region_right + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    dest.left = FILL_X(gui.scroll_region_left);
    dest.top = FILL_Y(row);
    dest.right = FILL_X(gui.scroll_region_right + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot - num_lines + 1) - PEN_WIDTH;

    ຖ້າ (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(gui.scroll_region_bot - num_lines + 1,
		gui.scroll_region_left,
		gui.scroll_region_bot, gui.scroll_region_right);


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
    void
VimTextAreaView::mchInsertLines(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ num_lines)
{
    BRect source, dest;

    // XXX Attempt at a hack:
    gui.vimWindow->UpdateIfNeeded();
    source.left = FILL_X(gui.scroll_region_left);
    source.top = FILL_Y(row);
    source.right = FILL_X(gui.scroll_region_right + 1) - PEN_WIDTH;
    source.bottom = FILL_Y(gui.scroll_region_bot - num_lines + 1) - PEN_WIDTH;

    dest.left = FILL_X(gui.scroll_region_left);
    dest.top = FILL_Y(row + num_lines);
    dest.right = FILL_X(gui.scroll_region_right + 1) - PEN_WIDTH;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_WIDTH;

    ຖ້າ (gui.vimWindow->Lock()) {
	// Clear one column more for when bold has spilled over
	CopyBits(source, dest);
	gui_clear_block(row, gui.scroll_region_left,
		row + num_lines - 1, gui.scroll_region_right);

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
void VimTextAreaView::DrawIMString(void)
{
    static const rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    const var_na* s;
    ຈຳນວນເຕັມ len;
    BMessage* msg = IMData.message;
    ຖ້າ (!msg)
	ສົ່ງຄືນ;
    gui_redraw_block(IMData.row, 0,
	    IMData.row + IMData.count, W_WIDTH(curwin), GUI_MON_NOCLEAR);
    bool confirmed = false;
    msg->FindBool("be:confirmed", &confirmed);
    ຖ້າ (confirmed)
	ສົ່ງຄືນ;
    rgb_color hcolor = HighColor(), lcolor = LowColor();
    msg->FindString("be:string", &str);
    s = str.String();
    len = str.Length();
    SetHighColor(0, 0, 0);
    IMData.row = gui.row;
    IMData.col = gui.col;
    int32 sel_start = 0, sel_end = 0;
    msg->FindInt32("be:selection", 0, &sel_start);
    msg->FindInt32("be:selection", 1, &sel_end);
    ຈຳນວນເຕັມ clen, cn;
    BPoint pos(IMData.col, 0);
    BRect r;
    BPoint where;
    IMData.location = ConvertToScreen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
    ສຳລັບ (ຈຳນວນເຕັມ i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	ຖ້າ (pos.x + cn > W_WIDTH(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	ຖ້າ (sel_start<=i && i<sel_end)
	{
	    SetLowColor(r_highlight);
	    IMData.location = ConvertToScreen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.row + pos.y)));
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    SetLowColor(b_highlight);
	}
	r.Set(FILL_X(pos.x), FILL_Y(IMData.row + pos.y),
		FILL_X(pos.x + cn) - PEN_WIDTH,
		FILL_Y(IMData.row + pos.y + 1) - PEN_WIDTH);
	FillRect(r, B_SOLID_LOW);
	where.Set(TEXT_X(pos.x), TEXT_Y(IMData.row + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.count = (ຈຳນວນເຕັມ)pos.y;

    SetHighColor(hcolor);
    SetLowColor(lcolor);
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
	    "vim scrollbar", (BView *)NULL,
	    0.0, 10.0, posture),
    ignoreValue(-1),
    scrollEventCount(0)
{
    gsb = g;
    SetResizingMode(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    void
VimScrollBar::ValueChanged(float newValue)
{
    ຖ້າ (ignoreValue >= 0.0 && newValue == ignoreValue) {
	ignoreValue = -1;
	ສົ່ງຄືນ;
    }
    ignoreValue = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_add(&scrollEventCount, 1);

    sa_nracanaa VimScrollBarMsg sm;

    sm.sb = this;
    sm.value = (long) newValue;
    sm.stillDragging = TRUE;

    write_port(gui.vdcmp, VimMsg::ScrollBar, &sm, parimaa_na(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    void
VimScrollBar::MouseUp(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_add(&scrollEventCount, 1);

    sa_nracanaa VimScrollBarMsg sm;

    sm.sb = this;
    sm.value = (long) Value();
    sm.stillDragging = FALSE;

    write_port(gui.vdcmp, VimMsg::ScrollBar, &sm, parimaa_na(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::MouseUp(where);
}

    void
VimScrollBar::SetValue(float newValue)
{
    ຖ້າ (newValue == Value())
	ສົ່ງຄືນ;

    ignoreValue = newValue;
    Inherited::SetValue(newValue);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(const VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(const BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(const VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    void
VimFont::init()
{
    next = NULL;
    refcount = 1;
    name = NULL;
}

// ---------------- VimDialog ----------------

#ຖ້າ defined(FEAT_GUI_DIALOG)

const unsigned ຈຳນວນເຕັມ  kVimDialogButtonMsg = 'VMDB';
const unsigned ຈຳນວນເຕັມ  kVimDialogIconStripeWidth = 30;
const unsigned ຈຳນວນເຕັມ  kVimDialogButtonsSpacingX = 9;
const unsigned ຈຳນວນເຕັມ  kVimDialogButtonsSpacingY = 4;
const unsigned ຈຳນວນເຕັມ  kVimDialogSpacingX = 6;
const unsigned ຈຳນວນເຕັມ  kVimDialogSpacingY = 10;
const unsigned ຈຳນວນເຕັມ  kVimDialogMinimalWidth  = 310;
const unsigned ຈຳນວນເຕັມ  kVimDialogMinimalHeight = 75;
const BRect	    kDefaultRect =
BRect(0, 0, kVimDialogMinimalWidth, kVimDialogMinimalHeight);

VimDialog::VimDialog(ຈຳນວນເຕັມ type, const var_na *title, const var_na *message,
	const var_na *buttons, ຈຳນວນເຕັມ dfltbutton, const var_na *textfield, ຈຳນວນເຕັມ ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , fDialogValue(dfltbutton)
    , fMessageView(NULL)
    , fInputControl(NULL)
    , fInputValue(textfield)
{
    //	master view
    VimDialog::View* view = new VimDialog::View(Bounds());
    ຖ້າ (view == NULL)
	ສົ່ງຄືນ;

    ຖ້າ (title == NULL)
	SetTitle("Vim " VIM_VERSION_MEDIUM);

    AddChild(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    float maxButtonWidth  = 0;
    float maxButtonHeight = 0;
    float buttonsWidth	  = 0;
    float buttonsHeight   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    do
    {
	int32 end = strButtons.FindFirst('\n');
	ຖ້າ (end != B_ERROR)
	    strButtons.SetByteAt(end, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->AddChild(button);
	fButtonsList.AddItem(button);

	maxButtonWidth	= max_c(maxButtonWidth,  button->Bounds().Width());
	maxButtonHeight = max_c(maxButtonHeight, button->Bounds().Height());
	buttonsWidth   += button->Bounds().Width();
	buttonsHeight  += button->Bounds().Height();

	ຖ້າ (end == B_ERROR)
	    bha_nga;

	strButtons.Remove(0, end + 1);
    } ໃນຂະນະທີ່ (true);

    int32 buttonsCount = fButtonsList.CountItems();
    buttonsWidth      += kVimDialogButtonsSpacingX * (buttonsCount - 1);
    buttonsHeight     += kVimDialogButtonsSpacingY * (buttonsCount - 1);
    float dialogWidth  = buttonsWidth + kVimDialogIconStripeWidth +
	kVimDialogSpacingX * 2;
    float dialogHeight = maxButtonHeight + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    bool vertical = (vim_strchr(p_go, GO_VERTICAL) != NULL) ||
	dialogWidth >= gui.vimWindow->Bounds().Width();
    ຖ້າ (vertical) {
	dialogWidth  -= buttonsWidth;
	dialogWidth  += maxButtonWidth;
	dialogHeight -= maxButtonHeight;
	dialogHeight += buttonsHeight;
    }

    dialogWidth  = max_c(dialogWidth,  kVimDialogMinimalWidth);

    //	message view
    BRect rect(0, 0, dialogWidth, 0);
    rect.left  += kVimDialogIconStripeWidth + 16 + kVimDialogSpacingX;
    rect.top   += kVimDialogSpacingY;
    rect.right -= kVimDialogSpacingX;
    rect.bottom = rect.top;
    fMessageView = new BTextView(rect, "_tv_", rect.OffsetByCopy(B_ORIGIN),
	    B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);

    fMessageView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color textColor = ui_color(B_PANEL_TEXT_COLOR);
    fMessageView->SetFontAndColor(be_plain_font, B_FONT_ALL, &textColor);
    fMessageView->SetText(message);
    fMessageView->MakeEditable(false);
    fMessageView->MakeSelectable(false);
    fMessageView->SetWordWrap(true);
    AddChild(fMessageView);

    float messageHeight = fMessageView->TextHeight(0, fMessageView->CountLines());
    fMessageView->ResizeBy(0, messageHeight);
    fMessageView->SetTextRect(BRect(0, 0, rect.Width(), messageHeight));

    dialogHeight += messageHeight;

    //	input view
    ຖ້າ (fInputValue != NULL) {
	rect.top     =
	    rect.bottom += messageHeight + kVimDialogSpacingY;
	fInputControl = new BTextControl(rect, "_iv_", NULL, fInputValue, NULL,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	fInputControl->TextView()->SetText(fInputValue);
	fInputControl->TextView()->SetWordWrap(false);
	AddChild(fInputControl);

	float width = 0.f, height = 0.f;
	fInputControl->GetPreferredSize(&width, &height);
	fInputControl->MakeFocus(true);

	dialogHeight += height + kVimDialogSpacingY * 1.5;
    }

    dialogHeight = max_c(dialogHeight, kVimDialogMinimalHeight);

    ResizeTo(dialogWidth, dialogHeight);
    MoveTo((gui.vimWindow->Bounds().Width() - dialogWidth) / 2,
	    (gui.vimWindow->Bounds().Height() - dialogHeight) / 2);

    //	adjust layout of buttons
    float buttonWidth = max_c(maxButtonWidth, rect.Width() * 0.66);
    BPoint origin(dialogWidth, dialogHeight);
    origin.x -= kVimDialogSpacingX + (vertical ? buttonWidth : buttonsWidth);
    origin.y -= kVimDialogSpacingY + (vertical ? buttonsHeight	: maxButtonHeight);

    ສຳລັບ (int32 i = 0 ; i < buttonsCount; i++) {
	BButton *button = (BButton*)fButtonsList.ItemAt(i);
	button->MoveTo(origin);
	ຖ້າ (vertical) {
	    origin.y += button->Frame().Height() + kVimDialogButtonsSpacingY;
	    button->ResizeTo(buttonWidth, button->Frame().Height());
	} ບໍ່ດັ່ງນັ້ນ
	    origin.x += button->Frame().Width() + kVimDialogButtonsSpacingX;

	ຖ້າ (dfltbutton == i + 1) {
	    button->MakeDefault(true);
	    button->MakeFocus(fInputControl == NULL);
	}
    }
}

VimDialog::~VimDialog()
{
    ຖ້າ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    ຈຳນວນເຕັມ
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    ຖ້າ (fDialogSem < B_OK) {
	Quit();
	ສົ່ງຄືນ fDialogValue;
    }

    Show();

    ໃນຂະນະທີ່ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    ຈຳນວນເຕັມ retValue = fDialogValue;
    ຖ້າ (fInputValue != NULL)
	vim_strncpy((char_u*)fInputValue, (char_u*)fInputControl->Text(), IOSIZE - 1);

    ຖ້າ (Lock())
	Quit();

    ສົ່ງຄືນ retValue;
}

void VimDialog::MessageReceived(BMessage *msg)
{
    int32 which = 0;
    ຖ້າ (msg->what != kVimDialogButtonMsg ||
	    msg->FindInt32("which", &which) != B_OK)
	ສົ່ງຄືນ BWindow::MessageReceived(msg);

    fDialogValue = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, const var_na* label)
{
    BMessage *message = new BMessage(kVimDialogButtonMsg);
    message->AddInt32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = new BButton(rect, name.String(), label, message,
	    B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);

    float width = 0.f, height = 0.f;
    button->GetPreferredSize(&width, &height);
    button->ResizeTo(width, height);

    ສົ່ງຄືນ button;
}

VimDialog::View::View(BRect frame)
    :	BView(frame, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(NULL)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    delete fIconBitmap;
}

void VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.right = kVimDialogIconStripeWidth;
    SetHighColor(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    ຖ້າ (fIconBitmap == NULL)
	ສົ່ງຄືນ;

    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

void VimDialog::View::InitIcon(int32 type)
{
    ຖ້າ (type == VIM_GENERIC)
	ສົ່ງຄືນ;

    BPath path;
    status_t status = find_directory(B_BEOS_SERVERS_DIRECTORY, &path);
    ຖ້າ (status != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(status));
	ສົ່ງຄືນ;
    }

    path.Append("app_server");

    BFile file(path.Path(), O_RDONLY);
    ຖ້າ (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	ສົ່ງຄືນ;
    }

    BResources resources(&file);
    ຖ້າ (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	ສົ່ງຄືນ;
    }

    const var_na *name = "";
    switch(type) {
	pak_sa VIM_ERROR:	    name = "stop"; bha_nga;
	pak_sa VIM_WARNING:   name = "warn"; bha_nga;
	pak_sa VIM_INFO:	    name = "info"; bha_nga;
	pak_sa VIM_QUESTION:  name = "idea"; bha_nga;
	default: ສົ່ງຄືນ;
    }

    int32 iconSize = 32;
    fIconBitmap = new BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), 0, B_RGBA32);
    ຖ້າ (fIconBitmap == NULL || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == NULL) ? "null" : strerror(fIconBitmap->InitCheck()));
	ສົ່ງຄືນ;
    }

    size_t size = 0;
    const uint8* iconData = NULL;
    //	try vector icon first?
    iconData = (const uint8*)resources.LoadResource(B_VECTOR_ICON_TYPE, name, &size);
    ຖ້າ (iconData != NULL && BIconUtils::GetVectorIcon(iconData, size, fIconBitmap) == B_OK)
	ສົ່ງຄືນ;

    //	try bitmap icon now
    iconData = (const uint8*)resources.LoadResource(B_LARGE_ICON_TYPE, name, &size);
    ຖ້າ (iconData == NULL) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	delete fIconBitmap;
	fIconBitmap = NULL;
	ສົ່ງຄືນ;
    }

    ຖ້າ (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(iconData, iconSize, iconSize, iconSize, fIconBitmap);
}

const unsigned ຈຳນວນເຕັມ  kVimDialogOKButtonMsg = 'FDOK';
const unsigned ຈຳນວນເຕັມ  kVimDialogCancelButtonMsg = 'FDCN';
const unsigned ຈຳນວນເຕັມ  kVimDialogSizeInputMsg = 'SICH';
const unsigned ຈຳນວນເຕັມ  kVimDialogFamilySelectMsg = 'MSFM';
const unsigned ຈຳນວນເຕັມ  kVimDialogStyleSelectMsg = 'MSST';
const unsigned ຈຳນວນເຕັມ  kVimDialogSizeSelectMsg = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, float* size)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fStatus(B_NO_INIT)
    , fDialogSem(-1)
    , fDialogValue(false)
    , fFamily(family)
    , fStyle(style)
    , fSize(size)
    , fFontSize(*size)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , fSizesInput(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_LENGTH);
    strncpy(fFontStyle, *style, B_FONT_STYLE_LENGTH);

    //	"client" area view
    BBox *clientBox = new BBox(Bounds(), B_EMPTY_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    AddChild(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.LeftTop(), RC.LeftTop());

    //	at first create all controls
    fPreview = new BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->AddChild(fPreview);

    BBox* boxDivider = new BBox(rc, B_EMPTY_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->AddChild(boxDivider);

    BStringView *labelFamily = new BStringView(rc, "labelFamily", "Family:");
    clientBox->AddChild(labelFamily);
    labelFamily->ResizeToPreferred();

    BStringView *labelStyle = new BStringView(rc, "labelStyle", "Style:");
    clientBox->AddChild(labelStyle);
    labelStyle->ResizeToPreferred();

    BStringView *labelSize = new BStringView(rc, "labelSize", "Size:");
    clientBox->AddChild(labelSize);
    labelSize->ResizeToPreferred();

    fFamiliesList = new BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = new BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_LEFT_RIGHT, 0, false, true);
    clientBox->AddChild(scrollFamilies);

    fStylesList= new BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = new BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_LEFT_RIGHT, 0, false, true);
    clientBox->AddChild(scrollStyles);

    fSizesInput = new BTextControl(rc, "inputSize", NULL, "???",
	    new BMessage(kVimDialogSizeInputMsg));
    clientBox->AddChild(fSizesInput);
    fSizesInput->ResizeToPreferred();

    fSizesList = new BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = new BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_LEFT_RIGHT, 0, false, true);
    clientBox->AddChild(scrollSizes);

    BButton *buttonOK = new BButton(rc, "buttonOK", "OK",
			new BMessage(kVimDialogOKButtonMsg));
    clientBox->AddChild(buttonOK);
    buttonOK->ResizeToPreferred();

    BButton *buttonCancel = new BButton(rc, "buttonCancel", "Cancel",
			new BMessage(kVimDialogCancelButtonMsg));
    clientBox->AddChild(buttonCancel);
    buttonCancel->ResizeToPreferred();

    //	layout controls
    float lineHeight = labelFamily->Bounds().Height();
    float previewHeight = lineHeight * 3;
    float offsetYLabels = previewHeight + kVimDialogSpacingY;
    float offsetYLists = offsetYLabels + lineHeight + kVimDialogSpacingY / 2;
    float offsetYSizes = offsetYLists + fSizesInput->Bounds().Height() + kVimDialogSpacingY / 2;
    float listsHeight = lineHeight * 9;
    float offsetYButtons = offsetYLists + listsHeight +  kVimDialogSpacingY;
    float maxControlsHeight = offsetYButtons + buttonOK->Bounds().Height();
    float familiesWidth = labelFamily->Bounds().Width() * 5;
    float offsetXStyles = familiesWidth + kVimDialogSpacingX;
    float stylesWidth = labelStyle->Bounds().Width() * 4;
    float offsetXSizes = offsetXStyles + stylesWidth + kVimDialogSpacingX;
    float sizesWidth = labelSize->Bounds().Width() * 2;
    float maxControlsWidth = offsetXSizes + sizesWidth;

    ResizeTo(maxControlsWidth + kVimDialogSpacingX * 2,
	maxControlsHeight + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vimWindow->Frame();
    MoveTo(rcVim.left + (rcVim.Width() - Frame().Width()) / 2,
	    rcVim.top + (rcVim.Height() - Frame().Height()) / 2);

    fPreview->ResizeTo(maxControlsWidth, previewHeight);
    fPreview->SetAlignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, previewHeight + kVimDialogSpacingY / 2);
    boxDivider->ResizeTo(maxControlsWidth, 1.f);

    labelFamily->MoveBy(0.f, offsetYLabels);
    labelStyle->MoveBy(offsetXStyles, offsetYLabels);
    labelSize->MoveBy(offsetXSizes, offsetYLabels);

    //	text control alignment issues
    float insetX = fSizesInput->TextView()->Bounds().Width() - fSizesInput->Bounds().Width();
    float insetY = fSizesInput->TextView()->Bounds().Width() - fSizesInput->Bounds().Width();

    scrollFamilies->MoveBy(0.f, offsetYLists);
    scrollStyles->MoveBy(offsetXStyles, offsetYLists);
    fSizesInput->MoveBy(offsetXSizes + insetX / 2, offsetYLists + insetY / 2);
    scrollSizes->MoveBy(offsetXSizes, offsetYSizes);

    fSizesInput->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->ResizeTo(familiesWidth, listsHeight);
    scrollStyles->ResizeTo(stylesWidth, listsHeight);
    fSizesInput->ResizeTo(sizesWidth, fSizesInput->Bounds().Height());
    scrollSizes->ResizeTo(sizesWidth,
	    listsHeight - (offsetYSizes - offsetYLists));

    buttonOK->MoveBy(maxControlsWidth - buttonOK->Bounds().Width(), offsetYButtons);
    buttonCancel->MoveBy(maxControlsWidth - buttonOK->Bounds().Width()
	    - buttonCancel->Bounds().Width() - kVimDialogSpacingX, offsetYButtons);

    //	fill lists
    ຈຳນວນເຕັມ selIndex = -1;
    ຈຳນວນເຕັມ count = count_font_families();
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < count; i++) {
	font_family family;
	ຖ້າ (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->AddItem(new BStringItem((const var_na*)family));
	    ຖ້າ (strncmp(family, fFontFamily, B_FONT_FAMILY_LENGTH) == 0)
		selIndex = i;
	}
    }

    ຖ້າ (selIndex >= 0) {
	fFamiliesList->Select(selIndex);
	fFamiliesList->ScrollToSelection();
    }

    _UpdateFontStyles();

    selIndex = -1;
    ສຳລັບ (ຈຳນວນເຕັມ size = 8, index = 0; size <= 18; size++, index++) {
	BString str;
	str << size;
	fSizesList->AddItem(new BStringItem(str));
	ຖ້າ (size == fFontSize)
	    selIndex = index;

    }

    ຖ້າ (selIndex >= 0) {
	fSizesList->Select(selIndex);
	fSizesList->ScrollToSelection();
    }

    fFamiliesList->SetSelectionMessage(new BMessage(kVimDialogFamilySelectMsg));
    fStylesList->SetSelectionMessage(new BMessage(kVimDialogStyleSelectMsg));
    fSizesList->SetSelectionMessage(new BMessage(kVimDialogSizeSelectMsg));
    fSizesInput->SetModificationMessage(new BMessage(kVimDialogSizeInputMsg));

    _UpdateSizeInputPreview();
    _UpdateFontPreview();

    fStatus = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    ຖ້າ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    void
VimSelectFontDialog::_CleanList(BListView* list)
{
    ໃນຂະນະທີ່ (0 < list->CountItems())
	delete (dynamic_cast<BStringItem*>(list->RemoveItem((int32)0)));
}

    bool
VimSelectFontDialog::Go()
{
    ຖ້າ (fStatus != B_OK) {
	Quit();
	ສົ່ງຄືນ NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    ຖ້າ (fDialogSem < B_OK) {
	Quit();
	ສົ່ງຄືນ fDialogValue;
    }

    Show();

    ໃນຂະນະທີ່ (acquire_sem(fDialogSem) == B_INTERRUPTED);

    bool retValue = fDialogValue;

    ຖ້າ (Lock())
	Quit();

    ສົ່ງຄືນ retValue;
}


void VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 selIndex = -1;
    int32 count = count_font_styles(fFontFamily);
    ສຳລັບ (int32 i = 0; i < count; i++) {
	font_style style;
	uint32 flags = 0;
	ຖ້າ (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->AddItem(new BStringItem((const var_na*)style));
	    ຖ້າ (strncmp(style, fFontStyle, B_FONT_STYLE_LENGTH) == 0)
		selIndex = i;
	}
    }

    ຖ້າ (selIndex >= 0) {
	fStylesList->Select(selIndex);
	fStylesList->ScrollToSelection();
    } ບໍ່ດັ່ງນັ້ນ
	fStylesList->Select(0);
}


void VimSelectFontDialog::_UpdateSizeInputPreview()
{
    var_na buf[10] = {0};
    vim_snprintf(buf, parimaa_na(buf), (var_na*)"%.0f", fFontSize);
    fSizesInput->SetText(buf);
}


void VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFamilyAndStyle(fFontFamily, fFontStyle);
    fPreview->SetFont(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_SIZE);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (ຈຳນວນເຕັມ)fFontSize << " pt.";
    fPreview->SetText(str);
}


    bool
VimSelectFontDialog::_UpdateFromListItem(BListView* list, var_na* text, ຈຳນວນເຕັມ textSize)
{
    int32 index = list->CurrentSelection();
    ຖ້າ (index < 0)
	ສົ່ງຄືນ false;
    BStringItem* item = (BStringItem*)list->ItemAt(index);
    ຖ້າ (item == NULL)
	ສົ່ງຄືນ false;
    strncpy(text, item->Text(), textSize);
    ສົ່ງຄືນ true;
}


void VimSelectFontDialog::MessageReceived(BMessage *msg)
{
    switch (msg->what) {
	pak_sa kVimDialogOKButtonMsg:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_LENGTH);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_LENGTH);
	    *fSize = fFontSize;
	    fDialogValue = true;
	pak_sa kVimDialogCancelButtonMsg:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    ສົ່ງຄືນ;
	pak_sa B_KEY_UP:
	    {
		int32 key = 0;
		ຖ້າ (msg->FindInt32("raw_char", &key) == B_OK
			&& key == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    bha_nga;

	pak_sa kVimDialogFamilySelectMsg:
	    ຖ້າ (_UpdateFromListItem(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_LENGTH)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    bha_nga;
	pak_sa kVimDialogStyleSelectMsg:
	    ຖ້າ (_UpdateFromListItem(fStylesList,
		    fFontStyle, B_FONT_STYLE_LENGTH))
		_UpdateFontPreview();
	    bha_nga;
	pak_sa kVimDialogSizeSelectMsg:
	    {
		var_na buf[10] = {0};
		ຖ້າ (_UpdateFromListItem(fSizesList, buf, parimaa_na(buf))) {
		    float size = atof(buf);
		    ຖ້າ (size > 0.f) {
			fFontSize = size;
			_UpdateSizeInputPreview();
			_UpdateFontPreview();
		    }
		}
	    }
	    bha_nga;
	pak_sa kVimDialogSizeInputMsg:
	    {
		float size = atof(fSizesInput->Text());
		ຖ້າ (size > 0.f) {
		    fFontSize = size;
		    _UpdateFontPreview();
		}
	    }
	    bha_nga;
	default:
	    bha_nga;
    }
    ສົ່ງຄືນ BWindow::MessageReceived(msg);
}

#endif // FEAT_GUI_DIALOG

#ifdef FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
static BMessage * MenuMessage(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect frame, const var_na *name) :
    BBox(frame, name, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_FRAME_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 count = fButtonsList.CountItems();
    ສຳລັບ (int32 i = 0; i < count; i++)
	delete (BPictureButton*)fButtonsList.ItemAt(i);
    fButtonsList.MakeEmpty();

    delete normalButtonsBitmap;
    delete grayedButtonsBitmap;
    normalButtonsBitmap    = NULL;
    grayedButtonsBitmap  = NULL;
}

    void
VimToolbar::AttachedToWindow()
{
    BBox::AttachedToWindow();

    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    float
VimToolbar::ToolbarHeight() const
{
    float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
    ສົ່ງຄືນ size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    bool
VimToolbar::ModifyBitmapToGrayed(BBitmap *bitmap)
{
    float height = bitmap->Bounds().Height();
    float width  = bitmap->Bounds().Width();

    rgb_color *bits = (rgb_color*)bitmap->Bits();
    int32 pixels = bitmap->BitsLength() / 4;
    ສຳລັບ (int32 i = 0; i < pixels; i++) {
	bits[i].red = bits[i].green =
	bits[i].blue = ((uint32)bits[i].red + bits[i].green + bits[i].blue) / 3;
	bits[i].alpha /= 4;
    }

    ສົ່ງຄືນ true;
}

    bool
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    normalButtonsBitmap = LoadVimBitmap("builtin-tools.png");
    ຖ້າ (normalButtonsBitmap == NULL)
	//  customized not found? dig application resources for "builtin-tools" one
	normalButtonsBitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    ຖ້າ (normalButtonsBitmap == NULL)
	ສົ່ງຄືນ false;

    BMessage archive;
    normalButtonsBitmap->Archive(&archive);

    grayedButtonsBitmap = new BBitmap(&archive);
    ຖ້າ (grayedButtonsBitmap == NULL)
	ສົ່ງຄືນ false;

    //	modify grayed bitmap
    ModifyBitmapToGrayed(grayedButtonsBitmap);

    ສົ່ງຄືນ true;
}

BBitmap *VimToolbar::LoadVimBitmap(const var_na* fileName)
{
    BBitmap *bitmap = NULL;

    ຈຳນວນເຕັມ mustfree = 0;
    char_u* runtimePath = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    ຖ້າ (runtimePath != NULL && fileName != NULL) {
	BString strPath((var_na*)runtimePath);
	strPath << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(strPath.String());
    }

    ຖ້າ (mustfree)
	vim_free(runtimePath);

    ສົ່ງຄືນ bitmap;
}

    bool
VimToolbar::GetPictureFromBitmap(BPicture *pictureTo, int32 index, BBitmap *bitmapFrom, bool pressed)
{
    float size = bitmapFrom->Bounds().Height() + 1.;

    BView view(BRect(0, 0, size, size), "", 0, 0);

    AddChild(&view);
    view.BeginPicture(pictureTo);

    view.SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.SetDrawingMode(B_OP_OVER);

    BRect source(0, 0, size - 1, size - 1);
    BRect destination(source);

    source.OffsetBy(size * index, 0);
    destination.OffsetBy(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, source, destination);

    ຖ້າ (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	size += ButtonMargin * 2 - 1;
	view.BeginLineArray(4);
	view.AddLine(BPoint(0, 0),	 BPoint(size, 0),    shadowColor);
	view.AddLine(BPoint(size, 0),	 BPoint(size, size), shineColor);
	view.AddLine(BPoint(size, size), BPoint(0, size),    shineColor);
	view.AddLine(BPoint(0, size),	 BPoint(0, 0),	     shadowColor);
	view.EndLineArray();
    }

    view.EndPicture();
    RemoveChild(&view);

    ສົ່ງຄືນ true;
}

    bool
VimToolbar::AddButton(int32 index, vimmenu_T *menu)
{
    BPictureButton *button = NULL;
    ຖ້າ (!menu_is_separator(menu->name)) {
	float size = normalButtonsBitmap ?
	    normalButtonsBitmap->Bounds().Height() + 1. + ButtonMargin * 2 : 18.;
	BRect frame(0, 0, size, size);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture pictureGray;

	ຖ້າ (menu->iconfile == NULL && menu->iconidx >= 0 && normalButtonsBitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, normalButtonsBitmap, true);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, normalButtonsBitmap, false);
	    GetPictureFromBitmap(&pictureGray, menu->iconidx, grayedButtonsBitmap, false);
	} ບໍ່ດັ່ງນັ້ນ {

	    char_u buffer[MAXPATHL] = {0};
	    BBitmap *bitmap = NULL;

	    ຖ້າ (menu->iconfile) {
		gui_find_iconfile(menu->iconfile, buffer, (var_na*)"png");
		bitmap = BTranslationUtils::GetBitmap((var_na*)buffer);
	    }

	    ຖ້າ (bitmap == NULL && gui_find_bitmap(menu->name, buffer, (var_na*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((var_na*)buffer);

	    ຖ້າ (bitmap == NULL)
		bitmap = new BBitmap(BRect(0, 0, size, size), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, true);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, false);
	    ModifyBitmapToGrayed(bitmap);
	    GetPictureFromBitmap(&pictureGray, 0, bitmap, false);

	    delete bitmap;
	}

	button = new BPictureButton(frame, (var_na*)menu->name,
		    &pictureOff, &pictureOn, MenuMessage(menu));

	button->SetDisabledOn(&pictureGray);
	button->SetDisabledOff(&pictureGray);

	button->SetTarget(gui.vimTextArea);

	AddChild(button);

	menu->button = button;
    }

    bool result = fButtonsList.AddItem(button, index);
    InvalidateLayout();
    ສົ່ງຄືນ result;
}

    bool
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    ຖ້າ (menu->button) {
	ຖ້າ (fButtonsList.RemoveItem(menu->button)) {
	    delete menu->button;
	    menu->button = NULL;
	}
    }
    ສົ່ງຄືນ true;
}

    bool
VimToolbar::GrayButton(vimmenu_T *menu, ຈຳນວນເຕັມ grey)
{
    ຖ້າ (menu->button) {
	int32 index = fButtonsList.IndexOf(menu->button);
	ຖ້າ (index >= 0)
	    menu->button->SetEnabled(grey ? false : true);
    }
    ສົ່ງຄືນ true;
}

    void
VimToolbar::InvalidateLayout()
{
    int32 offset = ToolbarMargin;
    int32 count = fButtonsList.CountItems();
    ສຳລັບ (int32 i = 0; i < count; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.ItemAt(i);
	ຖ້າ (button) {
	    button->MoveTo(offset, ToolbarMargin);
	    offset += button->Bounds().Width() + ToolbarMargin;
	} ບໍ່ດັ່ງນັ້ນ
	    offset += ToolbarMargin * 3;
    }
}

#endif /*FEAT_TOOLBAR*/

#ຖ້າ defined(FEAT_GUI_TABLINE)

    float
VimTabLine::TablineHeight() const
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    ສົ່ງຄືນ TabHeight(); //  + ToolbarMargin;
}

void
VimTabLine::MouseDown(BPoint point)
{
    ຖ້າ (!gui_mch_showing_tabline())
	ສົ່ງຄືນ;

    BMessage *m = Window()->CurrentMessage();
    assert(m);

    int32 buttons = 0;
    m->FindInt32("buttons", &buttons);

    int32 clicks = 0;
    m->FindInt32("clicks", &clicks);

    ຈຳນວນເຕັມ index = 0; //  0 means here - no tab found
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < CountTabs(); i++) {
	ຖ້າ (TabFrame(i).Contains(point)) {
	    index = i + 1; //  indexes are 1-based
	    bha_nga;
	}
    }

    ຈຳນວນເຕັມ event = -1;

    ຖ້າ ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    ບໍ່ດັ່ງນັ້ນ ຖ້າ (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = index > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    ບໍ່ດັ່ງນັ້ນ ຖ້າ (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPopUpMenu* popUpMenu = new BPopUpMenu("tabLineContextMenu", false, false);
	popUpMenu->AddItem(new BMenuItem(_("Close tabi R"), new BMessage(TABLINE_MENU_CLOSE)));
	popUpMenu->AddItem(new BMenuItem(_("New tab    T"), new BMessage(TABLINE_MENU_NEW)));
	popUpMenu->AddItem(new BMenuItem(_("Open tab..."), new BMessage(TABLINE_MENU_OPEN)));

	ConvertToScreen(&point);
	BMenuItem* item = popUpMenu->Go(point);
	ຖ້າ (item != NULL) {
	    event = item->Command();
	}

	delete popUpMenu;

    } ບໍ່ດັ່ງນັ້ນ {
	//  default processing
	BTabView::MouseDown(point);
	ສົ່ງຄືນ;
    }

    ຖ້າ (event < 0)
	ສົ່ງຄືນ;

    VimTablineMenuMsg tmm;
    tmm.index = index;
    tmm.event = event;
    write_port(gui.vdcmp, VimMsg::TablineMenu, &tmm, parimaa_na(tmm));
}

void
VimTabLine::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    VimTabLine *tabLine = gui.vimForm->TabLine();
    ຖ້າ (tabLine != NULL) {

	int32 i = 0;
	ສຳລັບ (; i < tabLine->CountTabs(); i++)
	    ຖ້າ (this == tabLine->TabAt(i))
		bha_nga;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	ຖ້າ (i < tabLine->CountTabs()) {
	    VimTablineMsg tm;
	    tm.index = i + 1;
	    write_port(gui.vdcmp, VimMsg::Tabline, &tm, parimaa_na(tm));
	}
    }
}

#endif //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
static var_na appsig[] = "application/x-vnd.Haiku-Vim-8";
key_map *keyMap;
var_na *keyMapChars;
ຈຳນວນເຕັມ main_exitcode = 127;

    status_t
gui_haiku_process_event(bigtime_t timeout)
{
    sa_nracanaa VimMsg vm;
    int32 what;
    ssize_t size;

    size = read_port_etc(gui.vdcmp, &what, &vm, parimaa_na(vm),
	    B_TIMEOUT, timeout);

    ຖ້າ (size >= 0) {
	switch (what) {
	    pak_sa VimMsg::Key:
		{
		    char_u *string = vm.u.Key.chars;
		    ຈຳນວນເຕັມ len = vm.u.Key.length;
		    ຖ້າ (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_input_buf();
			got_int = TRUE;
		    }

		    ຖ້າ (vm.u.Key.csi_escape)
#ifndef FEAT_MBYTE_IME
		    {
			ຈຳນວນເຕັມ	i;
			char_u	buf[2];

			ສຳລັບ (i = 0; i < len; ++i)
			{
			    add_to_input_buf(string + i, 1);
			    ຖ້າ (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (ຈຳນວນເຕັມ)KE_CSI;
				add_to_input_buf(buf, 2);
			    }
			}
		    }
#ບໍ່ດັ່ງນັ້ນ
			add_to_input_buf_csi(string, len);
#endif
		    ບໍ່ດັ່ງນັ້ນ
			add_to_input_buf(string, len);
		}
		bha_nga;
	    pak_sa VimMsg::Resize:
		gui_resize_shell(vm.u.NewSize.width, vm.u.NewSize.height);
		bha_nga;
	    pak_sa VimMsg::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 oldCount =
			atomic_add(&vm.u.Scroll.sb->scrollEventCount, -1);
		    ຖ້າ (oldCount <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.value, vm.u.Scroll.stillDragging);
		}
		bha_nga;
#ຖ້າ defined(FEAT_MENU)
	    pak_sa VimMsg::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		bha_nga;
#endif
	    pak_sa VimMsg::Mouse:
		{
		    int32 oldCount;
		    ຖ້າ (vm.u.Mouse.button == MOUSE_DRAG)
			oldCount =
			    atomic_add(&gui.vimTextArea->mouseDragEventCount, -1);
		    ບໍ່ດັ່ງນັ້ນ
			oldCount = 0;
		    ຖ້າ (oldCount <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		bha_nga;
	    pak_sa VimMsg::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		bha_nga;
	    pak_sa VimMsg::Focus:
		gui.in_focus = vm.u.Focus.active;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		ຖ້າ (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		bha_nga;
	    pak_sa VimMsg::Refs:
		::RefsReceived(vm.u.Refs.message, vm.u.Refs.changedir);
		bha_nga;
	    pak_sa VimMsg::Tabline:
		send_tabline_event(vm.u.Tabline.index);
		bha_nga;
	    pak_sa VimMsg::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.index, vm.u.TablineMenu.event);
		bha_nga;
	    default:
		//  unrecognised message, ignore it
		bha_nga;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    ສົ່ງຄືນ size;
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

    ຈຳນວນເຕັມ
vim_lock_screen()
{
    ສົ່ງຄືນ !gui.vimWindow || gui.vimWindow->Lock();
}

    void
vim_unlock_screen()
{
    ຖ້າ (gui.vimWindow)
	gui.vimWindow->Unlock();
}

#define RUN_BAPPLICATION_IN_NEW_THREAD	0

#ຖ້າ RUN_BAPPLICATION_IN_NEW_THREAD

    int32
run_vimapp(void *args)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.Run();		    // Run until Quit() called

    ສົ່ງຄືນ 0;
}

#ບໍ່ດັ່ງນັ້ນ

    int32
call_main(void *args)
{
    sa_nracanaa MainArgs *ma = (MainArgs *)args;

    ສົ່ງຄືນ main(ma->argc, ma->argv);
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    void
gui_mch_prepare(
	ຈຳນວນເຕັມ	*argc,
	var_na	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    ຖ້າ (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(find_thread(NULL), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_port(B_MAX_PORT_COUNT, "vim VDCMP");

#ຖ້າ RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = spawn_thread(run_vimapp, "vim VimApp",
		tinfo.priority, NULL);
	ຖ້າ (tid >= B_OK) {
	    resume_thread(tid);
	} ບໍ່ດັ່ງນັ້ນ {
	    getout(1);
	}
#ບໍ່ດັ່ງນັ້ນ
	MainArgs ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.priority, &ma);
	ຖ້າ (tid >= B_OK) {
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
	    (void)wait_for_thread(tid, &dummy_exitcode);

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
    ຖ້າ (!isatty(0)) {
	sa_nracanaa stat stat_stdin, stat_dev_null;

	ຖ້າ (fstat(0, &stat_stdin) == -1 ||
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
    ຈຳນວນເຕັມ
gui_mch_init_check(void)
{
    ສົ່ງຄືນ OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    ຈຳນວນເຕັມ
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_back_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    gui.scrollbar_width = (ຈຳນວນເຕັມ) B_V_SCROLL_BAR_WIDTH;
    gui.scrollbar_height = (ຈຳນວນເຕັມ) B_H_SCROLL_BAR_HEIGHT;
#ifdef FEAT_MENU
    gui.menu_height = 19;   //	initial guess -
    //	correct for my default settings
#endif
    gui.border_offset = 3;  //	coordinates are inside window borders

    ຖ້າ (gui.vdcmp < B_OK)
	ສົ່ງຄືນ FAIL;
    get_key_map(&keyMap, &keyMapChars);

    gui.vimWindow = new VimWindow();	// hidden and locked
    ຖ້າ (!gui.vimWindow)
	ສົ່ງຄືນ FAIL;

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

    ສົ່ງຄືນ OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    void
gui_mch_new_colors()
{
    rgb_color rgb = GUI_TO_RGB(gui.back_pixel);

    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimForm->SetViewColor(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vimWindow->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ຈຳນວນເຕັມ
gui_mch_open()
{
    ຖ້າ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimWindow->Show();
	gui.vimWindow->Unlock();
	ສົ່ງຄືນ OK;
    }

    ສົ່ງຄືນ FAIL;
}

    void
gui_mch_exit(ຈຳນວນເຕັມ vim_exitcode)
{
    ຖ້າ (gui.vimWindow) {
	thread_id tid = gui.vimWindow->Thread();
	gui.vimWindow->Lock();
	gui.vimWindow->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_port(gui.vdcmp);
#ຖ້າ !RUN_BAPPLICATION_IN_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#endif
    ຖ້າ (gui.vimApp) {
	VimTextAreaView::guiBlankMouse(false);

	main_exitcode = vim_exitcode;
#ຖ້າ RUN_BAPPLICATION_IN_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->Lock();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#ບໍ່ດັ່ງນັ້ນ
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
    ຈຳນວນເຕັມ
gui_mch_get_winpos(ຈຳນວນເຕັມ *x, ຈຳນວນເຕັມ *y)
{
    ຖ້າ (gui.vimWindow->Lock()) {
	BRect r;
	r = gui.vimWindow->Frame();
	gui.vimWindow->Unlock();
	*x = (ຈຳນວນເຕັມ)r.left;
	*y = (ຈຳນວນເຕັມ)r.top;
	ສົ່ງຄືນ OK;
    }
    ບໍ່ດັ່ງນັ້ນ
	ສົ່ງຄືນ FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    void
gui_mch_set_winpos(ຈຳນວນເຕັມ x, ຈຳນວນເຕັມ y)
{
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimWindow->MoveTo(x, y);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
void
gui_mch_set_shellsize(
	ຈຳນວນເຕັມ	width,
	ຈຳນວນເຕັມ	height,
	ຈຳນວນເຕັມ	min_width,
	ຈຳນວນເຕັມ	min_height,
	ຈຳນວນເຕັມ	base_width,
	ຈຳນວນເຕັມ	base_height,
	ຈຳນວນເຕັມ	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimWindow->ResizeTo(width - PEN_WIDTH, height - PEN_WIDTH);

	// set size limits
	float minWidth, maxWidth, minHeight, maxHeight;

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

void
gui_mch_get_screen_dimensions(
	ຈຳນວນເຕັມ	*screen_w,
	ຈຳນວນເຕັມ	*screen_h)
{
    BRect frame;

    {
	BScreen screen(gui.vimWindow);

	ຖ້າ (screen.IsValid()) {
	    frame = screen.Frame();
	} ບໍ່ດັ່ງນັ້ນ {
	    frame.right = 640;
	    frame.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (ຈຳນວນເຕັມ) frame.right - 2 * gui.scrollbar_width - 20;
    *screen_h = (ຈຳນວນເຕັມ) frame.bottom - gui.scrollbar_height
#ifdef FEAT_MENU
	- gui.menu_height
#endif
	- 30;
}

void
gui_mch_set_text_area_pos(
	ຈຳນວນເຕັມ	x,
	ຈຳນວນເຕັມ	y,
	ຈຳນວນເຕັມ	w,
	ຈຳນວນເຕັມ	h)
{
    ຖ້າ (!gui.vimTextArea)
	ສົ່ງຄືນ;

    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimTextArea->MoveTo(x, y);
	gui.vimTextArea->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);

#ifdef FEAT_GUI_TABLINE
	ຖ້າ (gui.vimForm->TabLine() != NULL) {
	    gui.vimForm->TabLine()->ResizeTo(w, gui.vimForm->TablineHeight());
	}
#endif // FEAT_GUI_TABLINE

	gui.vimWindow->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

void
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	ຈຳນວນເຕັມ	flag)
{
    VimScrollBar *vsb = sb->id;
    ຖ້າ (gui.vimWindow->Lock()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	ຖ້າ (flag) {
	    ຖ້າ (vsb->IsHidden()) {
		vsb->Show();
	    }
	} ບໍ່ດັ່ງນັ້ນ {
	    ຖ້າ (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vimWindow->Unlock();
    }
}

void
gui_mch_set_scrollbar_thumb(
	scrollbar_T *sb,
	ຈຳນວນເຕັມ	val,
	ຈຳນວນເຕັມ	size,
	ຈຳນວນເຕັມ	max)
{
    ຖ້າ (gui.vimWindow->Lock()) {
	VimScrollBar *s = sb->id;
	ຖ້າ (max == 0) {
	    s->SetValue(0);
	    s->SetRange(0.0, 0.0);
	} ບໍ່ດັ່ງນັ້ນ {
	    s->SetProportion((float)size / (max + 1.0));
	    s->SetSteps(1.0, size > 5 ? size - 2 : size);
#ifndef SCROLL_PAST_END	    //	really only defined in gui.c...
	    max = max + 1 - size;
#endif
	    ຖ້າ (max < s->Value()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->SetValue(val);
		s->SetRange(0.0, max);
	    } ບໍ່ດັ່ງນັ້ນ {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->SetRange(0.0, max);
		s->SetValue(val);
	    }
	}
	gui.vimWindow->Unlock();
    }
}

void
gui_mch_set_scrollbar_pos(
	scrollbar_T *sb,
	ຈຳນວນເຕັມ	x,
	ຈຳນວນເຕັມ	y,
	ຈຳນວນເຕັມ	w,
	ຈຳນວນເຕັມ	h)
{
    ຖ້າ (gui.vimWindow->Lock()) {
	BRect winb = gui.vimWindow->Bounds();
	float vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->ResizeTo(w - PEN_WIDTH, h - PEN_WIDTH);
	ຖ້າ (winb.right-(x+w)<w) vsbx = winb.right - (w - PEN_WIDTH);
	vsb->MoveTo(vsbx, vsby);
	gui.vimWindow->Unlock();
    }
}

    ຈຳນວນເຕັມ
gui_mch_get_scrollbar_xpadding(void)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ສົ່ງຄືນ 0;
}

    ຈຳນວນເຕັມ
gui_mch_get_scrollbar_ypadding(void)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    ສົ່ງຄືນ 0;
}

void
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	ຈຳນວນເຕັມ	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = new VimScrollBar(sb, posture);
    ຖ້າ (gui.vimWindow->Lock()) {
	vsb->SetTarget(gui.vimTextArea);
	vsb->Hide();
	gui.vimForm->AddChild(vsb);
	gui.vimWindow->Unlock();
    }
}

#ຖ້າ defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
void
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    ຖ້າ (gui.vimWindow->Lock()) {
	sb->id->RemoveSelf();
	delete sb->id;
	gui.vimWindow->Unlock();
    }
}
#endif

/*
 * Cursor does not flash
 */
    ຈຳນວນເຕັມ
gui_mch_is_blink_off(void)
{
    ສົ່ງຄືນ FALSE;
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

static ຈຳນວນເຕັມ	blink_state = BLINK_NONE;
static long_u	    blink_waittime = 700;
static long_u	    blink_ontime = 400;
static long_u	    blink_offtime = 250;
static ຈຳນວນເຕັມ  blink_timer = 0;

void
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
    void
gui_mch_stop_blink(ຈຳນວນເຕັມ may_call_gui_update_cursor)
{
    // TODO
    ຖ້າ (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    ຖ້າ (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    void
gui_mch_start_blink()
{
    // TODO
    ຖ້າ (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    ຖ້າ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
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
ຈຳນວນເຕັມ
gui_mch_init_font(
	char_u	    *font_name,
	ຈຳນວນເຕັມ	    fontset)
{
    ຖ້າ (gui.vimWindow->Lock())
    {
	ຈຳນວນເຕັມ rc = gui.vimTextArea->mchInitFont(font_name);
	gui.vimWindow->Unlock();

	ສົ່ງຄືນ rc;
    }

    ສົ່ງຄືນ FAIL;
}


    ຈຳນວນເຕັມ
gui_mch_adjust_charsize()
{
    ສົ່ງຄືນ FAIL;
}


    ຈຳນວນເຕັມ
gui_mch_font_dialog(font_family* family, font_style* style, float* size)
{
#ຖ້າ defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = new VimSelectFontDialog(family, style, size);
    bool ret = dialog->Go();
	delete dialog;
	ສົ່ງຄືນ ret;
#ບໍ່ດັ່ງນັ້ນ
    ສົ່ງຄືນ NOFONT;
#endif // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	ຈຳນວນເຕັມ	    giveErrorIfMissing)
{
    static VimFont *fontList = NULL;

    ຖ້າ (!gui.in_use)	//  can't do this when GUI not running
	ສົ່ງຄືນ NOFONT;

    //	storage for locally modified name;
    const ຈຳນວນເຕັມ buff_size = B_FONT_FAMILY_LENGTH + B_FONT_STYLE_LENGTH + 20;
    static var_na font_name[buff_size] = {0};
    font_family family = {0};
    font_style	style  = {0};
    float size = 0.f;

    ຖ້າ (name == 0 && be_fixed_font == 0) {
	ຖ້າ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	ສົ່ງຄືນ NOFONT;
    }

    bool useSelectGUI = false;
    ຖ້າ (name != NULL)
	ຖ້າ (STRCMP(name, "*") == 0) {
	    useSelectGUI = true;
	    STRNCPY(font_name, hl_get_font_name(), buff_size);
	} ບໍ່ດັ່ງນັ້ນ
	    STRNCPY(font_name, name, buff_size);

    ຖ້າ (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	size = be_fixed_font->Size();
	vim_snprintf(font_name, buff_size,
	    (var_na*)"%s/%s/%.0f", family, style, size);
    }

    //	replace underscores with spaces
    var_na* end = 0;
    ໃນຂະນະທີ່ (end = strchr((var_na *)font_name, '_'))
	*end = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    static var_na buff[buff_size] = {0};
    STRNCPY(buff, font_name, buff_size);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_LENGTH);
    var_na* style_s = strtok(0, "/\0");
    ຖ້າ (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_LENGTH);
    size = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    ຖ້າ (useSelectGUI) {
	ຖ້າ (gui_mch_font_dialog(&family, &style, &size) == NOFONT)
	    ສົ່ງຄືນ FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_size,
		(var_na*)"%s/%s/%.0f", family, style, size);
	hl_set_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	ຖ້າ (new_p_guifont != NULL) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    ສຳລັບ ( ; *new_p_guifont; ++new_p_guifont)
		ຖ້າ (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    ສຳລັບ (flp = fontList; flp; flp = flp->next) {
	ຖ້າ (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    ສົ່ງຄືນ (GuiFont)flp;
	}
    }

    VimFont *font = new VimFont();
    font->name = vim_strsave((char_u*)font_name);

    ຖ້າ (count_font_styles(family) <= 0) {
	ຖ້າ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	delete font;
	ສົ່ງຄືນ NOFONT;
    }

    //	Remember font in the static list for later use
    font->next = fontList;
    fontList = font;

    font->SetFamilyAndStyle(family, style);
    ຖ້າ (size > 0.f)
	font->SetSize(size);

    font->SetSpacing(B_FIXED_SPACING);
    font->SetEncoding(B_UNICODE_UTF8);

    ສົ່ງຄືນ (GuiFont)font;
}

/*
 * Set the current text font.
 */
void
gui_mch_set_font(
	GuiFont	font)
{
    ຖ້າ (gui.vimWindow->Lock()) {
	VimFont *vf = (VimFont *)font;

	gui.vimTextArea->SetFont(vf);

	gui.char_width = (ຈຳນວນເຕັມ) vf->StringWidth("n");
	font_height fh;
	vf->GetHeight(&fh);
	gui.char_height = (ຈຳນວນເຕັມ)(fh.ascent + 0.9999)
	    + (ຈຳນວນເຕັມ)(fh.descent + 0.9999) + (ຈຳນວນເຕັມ)(fh.leading + 0.9999);
	gui.char_ascent = (ຈຳນວນເຕັມ)(fh.ascent + 0.9999);

	gui.vimWindow->Unlock();
    }
}

// XXX TODO This is apparently never called...
void
gui_mch_free_font(
	GuiFont	font)
{
    ຖ້າ (font == NOFONT)
	ສົ່ງຄືນ;
    VimFont *f = (VimFont *)font;
    ຖ້າ (--f->refcount <= 0) {
	ຖ້າ (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	delete f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ຖ້າ (name == NULL)
	ສົ່ງຄືນ NULL;
    ສົ່ງຄືນ vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ຈຳນວນເຕັມ
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
	gui.char_height = (ຈຳນວນເຕັມ)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (ຈຳນວນເຕັມ)(fh.ascent + 0.5) + p_linespace / 2;
    }
    ສົ່ງຄືນ OK;
}

    void
gui_mch_getmouse(ຈຳນວນເຕັມ *x, ຈຳນວນເຕັມ *y)
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

    void
gui_mch_mousehide(ຈຳນວນເຕັມ hide)
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
    ສົ່ງຄືນ gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
void
gui_mch_set_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetHighColor(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text background color.
 */
void
gui_mch_set_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_TO_RGB(color);
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetLowColor(rgb);
	gui.vimWindow->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    void
gui_mch_set_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

void
gui_mch_draw_string(
	ຈຳນວນເຕັມ	row,
	ຈຳນວນເຕັມ	col,
	char_u	*s,
	ຈຳນວນເຕັມ	len,
	ຈຳນວນເຕັມ	flags)
{
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchDrawString(row, col, s, len, flags);
	gui.vimWindow->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(ຈຳນວນເຕັມ r, ຈຳນວນເຕັມ g, ຈຳນວນເຕັມ b)
{
    ສົ່ງຄືນ gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
ຈຳນວນເຕັມ
gui_mch_haskey(
	char_u	*name)
{
    ຈຳນວນເຕັມ i;

    ສຳລັບ (i = 0; special_keys[i].BeKeys != 0; i++)
	ຖ້າ (name[0] == special_keys[i].vim_code0 &&
		name[1] == special_keys[i].vim_code1)
	    ສົ່ງຄືນ OK;
    ສົ່ງຄືນ FAIL;
}

    void
gui_mch_beep()
{
    ::beep();
}

    void
gui_mch_flash(ຈຳນວນເຕັມ msec)
{
    // Do a visual beep by reversing the foreground and background colors

    ຖ້າ (gui.vimWindow->Lock()) {
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
void
gui_mch_invert_rectangle(
	ຈຳນວນເຕັມ	r,
	ຈຳນວນເຕັມ	c,
	ຈຳນວນເຕັມ	nr,
	ຈຳນວນເຕັມ	nc)
{
    BRect rect;
    rect.left = FILL_X(c);
    rect.top = FILL_Y(r);
    rect.right = rect.left + nc * gui.char_width - PEN_WIDTH;
    rect.bottom = rect.top + nr * gui.char_height - PEN_WIDTH;

    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimTextArea->SetDrawingMode(B_OP_INVERT);
	gui.vimTextArea->FillRect(rect);
	gui.vimTextArea->SetDrawingMode(B_OP_COPY);
	gui.vimWindow->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    void
gui_mch_iconify()
{
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimWindow->Minimize(true);
	gui.vimWindow->Unlock();
    }
}

#ຖ້າ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    void
gui_mch_set_foreground(void)
{
    // TODO
}
#endif

/*
 * Set the window title
 */
void
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimWindow->SetTitle((var_na *)title);
	gui.vimWindow->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    void
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.left = FILL_X(gui.col);
    r.top = FILL_Y(gui.row);
    ຈຳນວນເຕັມ cells = utf_off2cells(LineOffset[gui.row] + gui.col, 100); // TODO-TODO
    ຖ້າ (cells>=4) cells = 1;
    r.right = r.left + cells*gui.char_width - PEN_WIDTH;
    r.bottom = r.top + gui.char_height - PEN_WIDTH;

    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimTextArea->StrokeRect(r);
	gui.vimWindow->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
void
gui_mch_draw_part_cursor(
	ຈຳນວນເຕັມ	w,
	ຈຳນວນເຕັມ	h,
	guicolor_T  color)
{
    gui_mch_set_fg_color(color);

    BRect r;
    r.left =
#ifdef FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_RIGHT ? FILL_X(gui.col + 1) - w :
#endif
	FILL_X(gui.col);
    r.right = r.left + w - PEN_WIDTH;
    r.bottom = FILL_Y(gui.row + 1) - PEN_WIDTH;
    r.top = r.bottom - h + PEN_WIDTH;

    ຖ້າ (gui.vimWindow->Lock()) {
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
    void
gui_mch_update()
{
    gui_mch_flush();
    ໃນຂະນະທີ່ (port_count(gui.vdcmp) > 0 &&
	    !vim_is_input_buf_full() &&
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
ຈຳນວນເຕັມ
gui_mch_wait_for_chars(
	long	wtime)
{
    ຈຳນວນເຕັມ		focus;
    bigtime_t	until, timeout;
    status_t	st;

    ຖ້າ (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_time() + timeout;
    }
    ບໍ່ດັ່ງນັ້ນ
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.in_focus;
    ສຳລັບ (;;)
    {
	// Stop or start blinking when focus changes
	ຖ້າ (gui.in_focus != focus)
	{
	    ຖ້າ (gui.in_focus)
		gui_mch_start_blink();
	    ບໍ່ດັ່ງນັ້ນ
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
	ຖ້າ (did_add_timer)
	    // Need to recompute the waiting time.
	    bha_nga;
# endif
# ifdef FEAT_JOB_CHANNEL
	ຖ້າ (has_any_channel())
	{
	    ຖ້າ (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (wtime < 0)
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

	ຖ້າ (input_available())
	    ສົ່ງຄືນ OK;
	ຖ້າ (st < B_OK)		// includes B_TIMED_OUT
	    ສົ່ງຄືນ FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	ຖ້າ (wtime >= 0)
	{
	    timeout = until - system_time();
	    ຖ້າ (timeout < 0)
		bha_nga;
	}
    }
    ສົ່ງຄືນ FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    void
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimWindow->Flush();
	gui.vimWindow->Unlock();
    }
    ສົ່ງຄືນ;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
void
gui_mch_clear_block(
	ຈຳນວນເຕັມ	row1,
	ຈຳນວນເຕັມ	col1,
	ຈຳນວນເຕັມ	row2,
	ຈຳນວນເຕັມ	col2)
{
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearBlock(row1, col1, row2, col2);
	gui.vimWindow->Unlock();
    }
}

    void
gui_mch_clear_all()
{
    ຖ້າ (gui.vimWindow->Lock()) {
	gui.vimTextArea->mchClearAll();
	gui.vimWindow->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
void
gui_mch_delete_lines(
	ຈຳນວນເຕັມ	row,
	ຈຳນວນເຕັມ	num_lines)
{
    gui.vimTextArea->mchDeleteLines(row, num_lines);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
void
gui_mch_insert_lines(
	ຈຳນວນເຕັມ	row,
	ຈຳນວນເຕັມ	num_lines)
{
    gui.vimTextArea->mchInsertLines(row, num_lines);
}

#ຖ້າ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

void
gui_mch_enable_menu(
	ຈຳນວນເຕັມ	flag)
{
    ຖ້າ (gui.vimWindow->Lock())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->SetEnabled(flag);
	gui.vimWindow->Unlock();
    }
}

void
gui_mch_set_menu_pos(
	ຈຳນວນເຕັມ	x,
	ຈຳນວນເຕັມ	y,
	ຈຳນວນເຕັມ	w,
	ຈຳນວນເຕັມ	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
void
gui_mch_add_menu(
	vimmenu_T   *menu,
	ຈຳນວນເຕັມ	idx)
{
    vimmenu_T	*parent = menu->parent;

    //	popup menu - just create it unattached
    ຖ້າ (menu_is_popup(menu->name) && parent == NULL) {
	BPopUpMenu* popUpMenu = new BPopUpMenu((const var_na*)menu->name, false, false);
	menu->submenu_id = popUpMenu;
	menu->id = NULL;
	ສົ່ງຄືນ;
    }

    ຖ້າ (!menu_is_menubar(menu->name)
	    || (parent != NULL && parent->submenu_id == NULL))
	ສົ່ງຄືນ;

    ຖ້າ (gui.vimWindow->Lock())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	ຖ້າ ( parent )
	    tmp = parent->submenu_id;
	ບໍ່ດັ່ງນັ້ນ
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	ຖ້າ ( ! tmp->FindItem((const var_na *) menu->dname)) {

	    BMenu *bmenu = new BMenu((var_na *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->AddItem(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->FindItem((const var_na *) menu->dname);

	}
	gui.vimWindow->Unlock();
    }
}

    void
gui_mch_toggle_tearoffs(ຈຳນວນເຕັມ enable)
{
    // no tearoff menus
}

    static BMessage *
MenuMessage(vimmenu_T *menu)
{
    BMessage *m = new BMessage('menu');
    m->AddPointer("VimMenu", (void *)menu);

    ສົ່ງຄືນ m;
}

/*
 * Add a menu item to a menu
 */
void
gui_mch_add_menu_item(
	vimmenu_T   *menu,
	ຈຳນວນເຕັມ	idx)
{
    ຈຳນວນເຕັມ	    mnemonic = 0;
    vimmenu_T	*parent = menu->parent;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    ຖ້າ (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	ຖ້າ (menu_is_toolbar(parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ຖ້າ (toolbar != NULL) {
		toolbar->AddButton(idx, menu);
	    }
	} ບໍ່ດັ່ງນັ້ນ
#endif

	ຖ້າ (parent->submenu_id != NULL || menu_is_popup(parent->name)) {
	    ຖ້າ (menu_is_separator(menu->name)) {
		BSeparatorItem *item = new BSeparatorItem();
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = NULL;
	    }
	    ບໍ່ດັ່ງນັ້ນ {
		BMenuItem *item = new BMenuItem((var_na *)menu->dname,
			MenuMessage(menu));
		item->SetTarget(gui.vimTextArea);
		item->SetTrigger((var_na) menu->mnemonic);
		parent->submenu_id->AddItem(item);
		menu->id = item;
		menu->submenu_id = NULL;
	    }
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
void
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    ຖ້າ (gui.vimWindow->Lock())
    {
#ifdef FEAT_TOOLBAR
	ຖ້າ (menu->parent && menu_is_toolbar(menu->parent->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ຖ້າ (toolbar != NULL) {
		toolbar->RemoveButton(menu);
	    }
	} ບໍ່ດັ່ງນັ້ນ
#endif
	{
	    assert(menu->submenu_id == NULL || menu->submenu_id->CountItems() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    ຖ້າ (bmenu)
	    {
		bmenu->RemoveItem(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		ຖ້າ (bmenu == gui.vimForm->MenuBar() && bmenu->CountItems() == 0)
		{
		    bmenu->ResizeTo(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    delete menu->id;
	    menu->id = NULL;
	    menu->submenu_id = NULL;

	    gui.menu_height = (ຈຳນວນເຕັມ) gui.vimForm->MenuHeight();
	}
	gui.vimWindow->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
void
gui_mch_menu_grey(
	vimmenu_T   *menu,
	ຈຳນວນເຕັມ	grey)
{
#ifdef FEAT_TOOLBAR
    ຖ້າ (menu->parent && menu_is_toolbar(menu->parent->name)) {
	ຖ້າ (gui.vimWindow->Lock()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ຖ້າ (toolbar != NULL) {
		toolbar->GrayButton(menu, grey);
	    }
	    gui.vimWindow->Unlock();
	}
    } ບໍ່ດັ່ງນັ້ນ
#endif
    ຖ້າ (menu->id != NULL)
	menu->id->SetEnabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
void
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	ຈຳນວນເຕັມ	hidden)
{
    ຖ້າ (menu->id != NULL)
	menu->id->SetEnabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    void
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    void
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ຖ້າ (!menu_is_popup(menu->name) || menu->submenu_id == NULL)
	ສົ່ງຄືນ;

    BPopUpMenu* popupMenu = dynamic_cast<BPopUpMenu*>(menu->submenu_id);
    ຖ້າ (popupMenu == NULL)
	ສົ່ງຄືນ;

    BPoint point;
    ຖ້າ (gui.vimWindow->Lock()) {
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
var_na textplain[] = "text/plain";
var_na vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    void
clip_mch_request_selection(Clipboard_T *cbd)
{
    ຖ້າ (be_clipboard->Lock())
    {
	BMessage *m = be_clipboard->Data();
	// m->PrintToStream();

	char_u *string = NULL;
	ssize_t stringlen = -1;

	ຖ້າ (m->FindData(textplain, B_MIME_TYPE,
		    (const void **)&string, &stringlen) == B_OK
		|| m->FindString("text", (const var_na **)&string) == B_OK)
	{
	    ຖ້າ (stringlen == -1)
		stringlen = STRLEN(string);

	    ຈຳນວນເຕັມ type;
	    var_na *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    ຖ້າ (m->FindData(vimselectiontype, B_MIME_TYPE,
			(const void **)&seltype, &seltypelen) == B_OK)
	    {
		switch (*seltype)
		{
		    default:
		    pak_sa 'L':	type = MLINE;	bha_nga;
		    pak_sa 'C':	type = MCHAR;	bha_nga;
#ifdef FEAT_VISUAL
		    pak_sa 'B':	type = MBLOCK;	bha_nga;
#endif
		}
	    }
	    ບໍ່ດັ່ງນັ້ນ
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
    void
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    ຈຳນວນເຕັມ
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    ສົ່ງຄືນ FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    void
clip_mch_set_selection(Clipboard_T *cbd)
{
    ຖ້າ (be_clipboard->Lock())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->Data();
	assert(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = NULL;
	long_u	count;
	ຈຳນວນເຕັມ type;

	type = clip_convert_selection(&str, &count, cbd);

	ຖ້າ (type < 0)
	    ສົ່ງຄືນ;

	m->AddData(textplain, B_MIME_TYPE, (void *)str, count);

	// Add type of selection
	var_na	vtype;
	switch (type)
	{
	    default:
	    pak_sa MLINE:    vtype = 'L';    bha_nga;
	    pak_sa MCHAR:    vtype = 'C';    bha_nga;
#ifdef FEAT_VISUAL
	    pak_sa MBLOCK:   vtype = 'B';    bha_nga;
#endif
	}
	m->AddData(vimselectiontype, B_MIME_TYPE, (void *)&vtype, 1);

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
	ຈຳນວນເຕັມ saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = new BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    NULL, NULL, 0, false,
	    new BMessage((saving == TRUE) ? 'save' : 'open'), NULL, true);

    gui.vimApp->fBrowsedPath.Unset();

    gui.vimApp->fFilePanel->Window()->SetTitle((var_na*)title);
    gui.vimApp->fFilePanel->SetPanelDirectory((const var_na*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    ໃນຂະນະທີ່ (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = NULL;
    status_t result = gui.vimApp->fBrowsedPath.InitCheck();
    ຖ້າ (result == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->fBrowsedPath.Path());
    } ບໍ່ດັ່ງນັ້ນ
	ຖ້າ (result != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    result, strerror(result));
	}

    delete gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = NULL;

    ສົ່ງຄືນ fileName;
}
#endif // FEAT_BROWSE


#ຖ້າ defined(FEAT_GUI_DIALOG)

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

ຈຳນວນເຕັມ
gui_mch_dialog(
	ຈຳນວນເຕັມ	 type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	ຈຳນວນເຕັມ	 dfltbutton,
	char_u	*textfield,
	ຈຳນວນເຕັມ ex_cmd)
{
    VimDialog *dialog = new VimDialog(type, (var_na*)title, (var_na*)message,
	    (var_na*)buttons, dfltbutton, (var_na*)textfield, ex_cmd);
    bool ret = dialog->Go();
    delete dialog;
	ສົ່ງຄືນ ret;
}

#endif // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_TO_RGB(pixel);

    ສົ່ງຄືນ ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    void
gui_mch_setmouse(ຈຳນວນເຕັມ x, ຈຳນວນເຕັມ y)
{
    TRACE();
    // TODO
}

#ifdef FEAT_MBYTE_IME
    void
im_set_position(ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ col)
{
    ຖ້າ (gui.vimWindow->Lock())
    {
	gui.vimTextArea->DrawIMString();
	gui.vimWindow->Unlock();
    }
    ສົ່ງຄືນ;
}
#endif

    void
gui_mch_show_toolbar(ຈຳນວນເຕັມ showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_height = (toolbar && showit) ? toolbar->ToolbarHeight() : 0.;
}

    void
gui_mch_set_toolbar_pos(ຈຳນວນເຕັມ x, ຈຳນວນເຕັມ y, ຈຳນວນເຕັມ w, ຈຳນວນເຕັມ h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    ຖ້າ (toolbar != NULL) {
	ຖ້າ (gui.vimWindow->Lock()) {
	    toolbar->MoveTo(x, y);
	    toolbar->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

#ຖ້າ defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    void
gui_mch_show_tabline(ຈຳນວນເຕັມ showit)
{
    VimTabLine *tabLine = gui.vimForm->TabLine();

    ຖ້າ (tabLine == NULL)
	ສົ່ງຄືນ;

    ຖ້າ (!showit != !gui.vimForm->IsShowingTabLine()) {
	gui.vimForm->SetShowingTabLine(showit != 0);
	gui.tabline_height = gui.vimForm->TablineHeight();
    }
}

    void
gui_mch_set_tabline_pos(ຈຳນວນເຕັມ x, ຈຳນວນເຕັມ y, ຈຳນວນເຕັມ w, ຈຳນວນເຕັມ h)
{
    VimTabLine *tabLine = gui.vimForm->TabLine();
    ຖ້າ (tabLine != NULL) {
	ຖ້າ (gui.vimWindow->Lock()) {
	    tabLine->MoveTo(x, y);
	    tabLine->ResizeTo(w - 1, h - 1);
	    gui.vimWindow->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ຈຳນວນເຕັມ
gui_mch_showing_tabline()
{
    VimTabLine *tabLine = gui.vimForm->TabLine();
    ສົ່ງຄືນ tabLine != NULL && gui.vimForm->IsShowingTabLine();
}

/*
 * Update the labels of the tabline.
 */
    void
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    ຈຳນວນເຕັມ	    nr = 0;
    ຈຳນວນເຕັມ	    curtabidx = 0;

    VimTabLine *tabLine = gui.vimForm->TabLine();

    ຖ້າ (tabLine == NULL)
	ສົ່ງຄືນ;

    gui.vimWindow->Lock();

    // Add a label for each tab page.  They all contain the same text area.
    ສຳລັບ (tp = first_tabpage; tp != NULL; tp = tp->tp_next, ++nr) {
	ຖ້າ (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tabLine->TabAt(nr);

	ຖ້າ (tab == NULL) {
	    tab = new VimTabLine::VimTab();
	    tabLine->AddTab(NULL, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->SetLabel((const var_na*)NameBuff);
	tabLine->Invalidate();
    }

    // Remove any old labels.
    ໃນຂະນະທີ່ (nr < tabLine->CountTabs())
	tabLine->RemoveTab(nr);

    ຖ້າ (tabLine->Selection() != curtabidx)
	tabLine->Select(curtabidx);

    gui.vimWindow->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    void
gui_mch_set_curtab(ຈຳນວນເຕັມ nr)
{
    VimTabLine *tabLine = gui.vimForm->TabLine();
    ຖ້າ (tabLine == NULL)
	ສົ່ງຄືນ;

    gui.vimWindow->Lock();

    ຖ້າ (tabLine->Selection() != nr -1)
	tabLine->Select(nr -1);

    gui.vimWindow->Unlock();
}

#endif // FEAT_GUI_TABLINE
