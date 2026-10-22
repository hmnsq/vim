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

బాహ్య "C" {

#అన్తర్భావ <assert.h>
#అన్తర్భావ <float.h>
#అన్తర్భావ <syslog.h>

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

}   // extern "C"

// ---------------- start of header part ----------------

//#include <Alert.h>
#అన్తర్భావ <Application.h>
#అన్తర్భావ <Beep.h>
#అన్తర్భావ <Bitmap.h>
#అన్తర్భావ <Box.h>
#అన్తర్భావ <Button.h>
#అన్తర్భావ <Clipboard.h>
#అన్తర్భావ <Debug.h>
//#include <Directory.h>
//#include <Entry.h>
#అన్తర్భావ <File.h>
#అన్తర్భావ <FilePanel.h>
#అన్తర్భావ <FindDirectory.h>
//#include <Font.h>
#అన్తర్భావ <IconUtils.h>
#అన్తర్భావ <Input.h>
#అన్తర్భావ <ListView.h>
#అన్తర్భావ <MenuBar.h>
#అన్తర్భావ <MenuItem.h>
//#include <MessageQueue.h>
//#include <OS.h>
#అన్తర్భావ <Path.h>
#అన్తర్భావ <PictureButton.h>
#అన్తర్భావ <PopUpMenu.h>
//#include <Region.h>
#అన్తర్భావ <Resources.h>
//#include <Roster.h>
#అన్తర్భావ <Screen.h>
#అన్తర్భావ <ScrollBar.h>
#అన్తర్భావ <ScrollView.h>
#అన్తర్భావ <String.h>
#అన్తర్భావ <StringView.h>
//#include <SupportDefs.h>
#అన్తర్భావ <TabView.h>
#అన్తర్భావ <TextControl.h>
#అన్తర్భావ <TextView.h>
#అన్తర్భావ <TranslationUtils.h>
#అన్తర్భావ <TranslatorFormats.h>
#అన్తర్భావ <View.h>
#అన్తర్భావ <Window.h>

తరగతి VimApp;
తరగతి VimFormView;
తరగతి Vim_పాఠ్యం_Area_View;
తరగతి Vim_కిటికీ;
తరగతి VimToolbar;
తరగతి Vim_Tab_పంక్తి;

బాహ్య కీలకం_map *కీలకం_Map;
బాహ్య అక్షరం *కీలకం_Map_Chars;

బాహ్య పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం **argv);

#నిర్వచితం_కాకపోతే B_MAX_పోర్టు_లెక్క
#నిర్వచించు B_MAX_పోర్టు_లెక్క    255
#అయితే_ముగింపు

// VimApp seems comparable to the X "vimShell"
తరగతి VimApp: బహిరంగ BApplication
{
    రకనిర్వచనం BApplication Inherited;
    బహిరంగ:
    VimApp(స్థిరరాశి అక్షరం *appsig);
    ~VimApp();

    // callbacks:
#ఒకవేళ 0
    మాయా శూన్య Dispatch_సందేశం(BMessage *m, BHandler *h)
    {
	m->Print_గమ్యం_ప్రవాహం();
	Inherited::Dispatch_సందేశం(m, h);
    }
#అయితే_ముగింపు
    మాయా శూన్య Ready_గమ్యం_నిరంతరం();
    మాయా శూన్య ArgvReceived(int32 argc, అక్షరం **argv);
    మాయా శూన్య RefsReceived(BMessage *m);
    మాయా తార్కిక QuitRequested();
    మాయా శూన్య సందేశం_Received(BMessage *m);

    స్థిర శూన్య SendRefs(BMessage *m, తార్కిక changedir);

    sem_id	fFilePanelSem;
    BFilePanel*	fFilePanel;
    BPath	f_Browsed_మార్గం;
    అంతరంగ:
};

తరగతి Vim_కిటికీ: బహిరంగ BWindow
{
    రకనిర్వచనం BWindow Inherited;
    బహిరంగ:
    Vim_కిటికీ();
    ~Vim_కిటికీ();

    మాయా శూన్య Dispatch_సందేశం(BMessage *m, BHandler *h);
    మాయా శూన్య కిటికీ_Activated(తార్కిక చురుకైన);
    మాయా తార్కిక QuitRequested();

    VimFormView	    *formView;

    అంతరంగ:
    శూన్య init();

    తార్కిక is_fullscreen = అబద్ధం;
    BRect saved_చట్రం;
    కిటికీ_look saved_look;
};

తరగతి VimFormView: బహిరంగ BView
{
    రకనిర్వచనం BView Inherited;
    బహిరంగ:
    VimFormView(BRect చట్రం);
    ~VimFormView();

    // callbacks:
    మాయా శూన్య AllAttached();
    మాయా శూన్య చట్రం_Resized(దశాంశం new_వెడల్పు, దశాంశం new_ఎత్తు);

#నిర్వచించు MENUBAR_MARGIN	1
    దశాంశం Menu_ఎత్తు() స్థిరరాశి
    { తిరిగి menuBar ? menuBar->చట్రం_2().ఎత్తు_2() + MENUBAR_MARGIN: 0; }
    BMenuBar *MenuBar() స్థిరరాశి
    { తిరిగి menuBar; }

    అంతరంగ:
    శూన్య init(BRect);

    BMenuBar	    *menuBar;
    Vim_పాఠ్యం_Area_View *పాఠ్యం_Area;

#నిర్వచితమైతే FEAT_TOOLBAR
    బహిరంగ:
    దశాంశం Toolbar_ఎత్తు() స్థిరరాశి;
    VimToolbar *ToolBar() స్థిరరాశి
    { తిరిగి toolBar; }
    అంతరంగ:
    VimToolbar	    *toolBar;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_TABLINE
    బహిరంగ:
    Vim_Tab_పంక్తి *Tab_పంక్తి() స్థిరరాశి	{ తిరిగి tab_పంక్తి; }
    తార్కిక Is_Showing_Tab_పంక్తి() స్థిరరాశి { తిరిగి showing_Tab_పంక్తి; }
    శూన్య సమితి_Showing_Tab_పంక్తి(తార్కిక showing) { showing_Tab_పంక్తి = showing;	}
    దశాంశం Tabline_ఎత్తు() స్థిరరాశి;
    అంతరంగ:
    Vim_Tab_పంక్తి	*tab_పంక్తి;
    పూర్ణాంకం	showing_Tab_పంక్తి;
#అయితే_ముగింపు
};

తరగతి Vim_పాఠ్యం_Area_View: బహిరంగ BView
{
    రకనిర్వచనం BView Inherited;
    బహిరంగ:
    Vim_పాఠ్యం_Area_View(BRect చట్రం);
    ~Vim_పాఠ్యం_Area_View();

    // callbacks:
    మాయా శూన్య Draw(BRect updateRect);
    మాయా శూన్య కీలకం_కిందికి(స్థిరరాశి అక్షరం *బైట్లు, int32 అంకె_బైట్లు_2);
    మాయా శూన్య Mouse_కిందికి(BPoint బిందువు);
    మాయా శూన్య Mouse_పైకి(BPoint బిందువు);
    మాయా శూన్య MouseMoved(BPoint బిందువు, uint32 transit, స్థిరరాశి BMessage *సందేశం_2);
    మాయా శూన్య సందేశం_Received(BMessage *m);

    // own functions:
    పూర్ణాంకం mchInitFont(char_u *name);
    శూన్య mchDrawString(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, char_u *s, పూర్ణాంకం len, పూర్ణాంకం flags);
    శూన్య mch_Clear_ఖండం(పూర్ణాంకం row1, పూర్ణాంకం col1, పూర్ణాంకం row2, పూర్ణాంకం col2);
    శూన్య mchClearAll();
    శూన్య mch_Delete_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు);
    శూన్య mch_చొప్పించు_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు);

    స్థిర శూన్య guiSendMouseEvent(పూర్ణాంకం button, పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం repeated_click, int_u modifiers);
    స్థిర శూన్య guiMouseMoved(పూర్ణాంకం x, పూర్ణాంకం y);
    స్థిర శూన్య guiBlankMouse(తార్కిక should_hide);
    స్థిర int_u mouse_Modifiers_గమ్యం_Vim(int32 beModifiers);

    int32 mouse_Drag_Event_లెక్క;

#నిర్వచితమైతే FEAT_MBYTE_IME
    శూన్య DrawIMString(శూన్య);
#అయితే_ముగింపు

    అంతరంగ:
    శూన్య init(BRect);

    int_u	vimMouseButton;
    int_u	vimMouseModifiers;

#నిర్వచితమైతే FEAT_MBYTE_IME
    నిర్మాణం {
	BMessenger* messenger;
	BMessage* సందేశం_2;
	BPoint location;
	పూర్ణాంకం అడ్డువరుస;
	పూర్ణాంకం నిలువు;
	పూర్ణాంకం లెక్క;
    } IMData;
#అయితే_ముగింపు
};

తరగతి VimScrollBar: బహిరంగ BScrollBar
{
    రకనిర్వచనం BScrollBar Inherited;
    బహిరంగ:
    VimScrollBar(scrollbar_T *gsb, orientation posture);
    ~VimScrollBar();

    మాయా శూన్య విలువ_మారింది_2(దశాంశం new_విలువ);
    మాయా శూన్య Mouse_పైకి(BPoint where);
    శూన్య సమితి_విలువ(దశాంశం newval);
    scrollbar_T *getGsb()
    { తిరిగి gsb; }

    int32	scroll_Event_లెక్క;

    అంతరంగ:
    scrollbar_T *gsb;
    దశాంశం   ignore_విలువ;
};


#నిర్వచితమైతే FEAT_TOOLBAR

తరగతి VimToolbar : బహిరంగ BBox
{
    స్థిర BBitmap *సాధారణ_Buttons_Bitmap;
    స్థిర BBitmap *grayedButtonsBitmap;

    BBitmap *ఎక్కించు_Vim_Bitmap(స్థిరరాశి అక్షరం* fileName);
    తార్కిక GetPictureFromBitmap(BPicture *picture_గమ్యం, int32 సూచీ, BBitmap *bitmapFrom, తార్కిక pressed);
    తార్కిక Modify_Bitmap_గమ్యం_Grayed(BBitmap *bitmap);

    BList fButtonsList;
    శూన్య InvalidateLayout();

    బహిరంగ:
    VimToolbar(BRect చట్రం, స్థిరరాశి అక్షరం * name);
    ~VimToolbar();

    తార్కిక PrepareButtonBitmaps();

    తార్కిక కూడు_Button(int32 సూచీ, vimmenu_T *menu);
    తార్కిక RemoveButton(vimmenu_T *menu);
    తార్కిక బూడిద_Button(vimmenu_T *menu, పూర్ణాంకం grey);

    దశాంశం Toolbar_ఎత్తు() స్థిరరాశి;
    మాయా శూన్య Attached_గమ్యం_కిటికీ();
};

BBitmap *VimToolbar::సాధారణ_Buttons_Bitmap  = ఖాళీ;
BBitmap *VimToolbar::grayedButtonsBitmap  = ఖాళీ;

స్థిరరాశి దశాంశం ToolbarMargin = 3.;
స్థిరరాశి దశాంశం ButtonMargin  = 3.;

#అయితే_ముగింపు //FEAT_TOOLBAR

#నిర్వచితమైతే FEAT_GUI_TABLINE

తరగతి Vim_Tab_పంక్తి : బహిరంగ BTabView
{
    బహిరంగ:
	తరగతి VimTab : బహిరంగ BTab {
	    బహిరంగ:
		VimTab() : BTab(కొత్త BView(BRect(), "-Empty-", 0, 0)) {}

	    మాయా శూన్య Select(BView* owner);
	};

	Vim_Tab_పంక్తి(BRect r) : BTabView(r, "vimTabLine", B_వెడల్పు_FROM_LABEL,
	       B_FOLLOW_ఎడమ | B_FOLLOW_శిఖరం | B_FOLLOW_కుడి, B_WILL_DRAW | B_చట్రం_EVENTS) {}

    దశాంశం Tabline_ఎత్తు() స్థిరరాశి;
    మాయా శూన్య Mouse_కిందికి(BPoint బిందువు);
};

#అయితే_ముగింపు //FEAT_GUI_TABLINE


// For caching the fonts that are used;
// Vim seems rather sloppy in this regard.
తరగతి VimFont: బహిరంగ BFont
{
    రకనిర్వచనం BFont Inherited;
    బహిరంగ:
    VimFont();
    VimFont(స్థిరరాశి VimFont *rhs);
    VimFont(స్థిరరాశి BFont *rhs);
    VimFont(స్థిరరాశి VimFont &rhs);
    ~VimFont();

    VimFont *తదుపరి;
    పూర్ణాంకం refcount;
    char_u *name;

    అంతరంగ:
    శూన్య init();
};

#ఒకవేళ defined(FEAT_GUI_DIALOG)

తరగతి VimDialog : బహిరంగ BWindow
{
    రకనిర్వచనం BWindow Inherited;

    BButton* _CreateButton(int32 which, స్థిరరాశి అక్షరం* label);

    బహిరంగ:

    తరగతి View : బహిరంగ BView {
	రకనిర్వచనం BView Inherited;

	బహిరంగ:
	View(BRect చట్రం);
	~View();

	మాయా శూన్య Draw(BRect updateRect);
	శూన్య InitIcon(int32 type);

	అంతరంగ:
	BBitmap*    fIconBitmap;
    };

    VimDialog(పూర్ణాంకం type, స్థిరరాశి అక్షరం *title, స్థిరరాశి అక్షరం *సందేశం_2,
	    స్థిరరాశి అక్షరం *buttons, పూర్ణాంకం dfltbutton, స్థిరరాశి అక్షరం *textfield,
	    పూర్ణాంకం ex_cmd);
    ~VimDialog();

    పూర్ణాంకం Go();

    మాయా శూన్య సందేశం_Received(BMessage *సందేశం);

    అంతరంగ:
    sem_id	    fDialogSem;
    పూర్ణాంకం		    f_Dialog_విలువ;
    BList	    fButtonsList;
    BTextView*	    f_సందేశం_View;
    BTextControl*   f_ఆదానం_Control;
    స్థిరరాశి అక్షరం*	    f_ఆదానం_విలువ;
};

తరగతి VimSelectFontDialog : బహిరంగ BWindow
{
    రకనిర్వచనం BWindow Inherited;

    శూన్య _CleanList(BListView* list);
    శూన్య _UpdateFontStyles();
    శూన్య Update_కొలత_ఆదానం_Preview();
    శూన్య _UpdateFontPreview();
    తార్కిక Update_From_List_అంశం(BListView* list, అక్షరం* పాఠ్యం, పూర్ణాంకం పాఠ్యం_కొలత);
    బహిరంగ:

    VimSelectFontDialog(font_family* family, font_style* style, దశాంశం* కొలత);
    ~VimSelectFontDialog();

    తార్కిక Go();

    మాయా శూన్య సందేశం_Received(BMessage *సందేశం);

    అంతరంగ:
    స్థితి_t	    f_స్థితి;
    sem_id	    fDialogSem;
    తార్కిక	    f_Dialog_విలువ;
    font_family*    fFamily;
    font_style*	    fStyle;
    దశాంశం*	    f_కొలత;
    font_family	    fFontFamily;
    font_style	    fFontStyle;
    దశాంశం	    f_Font_కొలత;
    BStringView*    fPreview;
    BListView*	    fFamiliesList;
    BListView*	    fStylesList;
    BListView*	    fSizesList;
    BTextControl*   f_Sizes_ఆదానం;
};

#అయితే_ముగింపు // FEAT_GUI_DIALOG

// ---------------- end of GUI classes ----------------

నిర్మాణం Main_వాదనలు {
    పూర్ణాంకం	     argc;
    అక్షరం    **argv;
};

// These messages are copied through the VDCMP.
// Therefore they ought not to have anything fancy.
// They must be of POD type (Plain Old Data)
// as the C++ standard calls them.

#నిర్వచించు	కీలకం_సందేశం_BUFSIZ	7
#ఒకవేళ కీలకం_సందేశం_BUFSIZ < MAX_కీలకం_సంకేతం_LEN
#దోషం Increase కీలకం_సందేశం_BUFSIZ!
#అయితే_ముగింపు

నిర్మాణం Vim_కీలకం_సందేశం {
    char_u  నిడివి;
    char_u  chars[కీలకం_సందేశం_BUFSIZ];  // contains Vim encoding
    తార్కిక    csi_escape;
};

నిర్మాణం Vim_Resize_సందేశం {
    పూర్ణాంకం	    వెడల్పు;
    పూర్ణాంకం	    ఎత్తు;
};

నిర్మాణం Vim_Scroll_Bar_సందేశం {
    VimScrollBar *sb;
    దీర్ఘ    విలువ;
    పూర్ణాంకం	    stillDragging;
};

నిర్మాణం Vim_Menu_సందేశం {
    vimmenu_T	*guiMenu;
};

నిర్మాణం Vim_Mouse_సందేశం {
    పూర్ణాంకం	    button;
    పూర్ణాంకం	    x;
    పూర్ణాంకం	    y;
    పూర్ణాంకం	    repeated_click;
    int_u   modifiers;
};

నిర్మాణం Vim_Mouse_Moved_సందేశం {
    పూర్ణాంకం	    x;
    పూర్ణాంకం	    y;
};

నిర్మాణం Vim_Focus_సందేశం {
    తార్కిక    చురుకైన;
};

నిర్మాణం Vim_Refs_సందేశం {
    BMessage   *సందేశం_2;
    తార్కిక    changedir;
};

నిర్మాణం Vim_Tabline_సందేశం {
    పూర్ణాంకం	    సూచీ;
};

నిర్మాణం Vim_Tabline_Menu_సందేశం {
    పూర్ణాంకం	    సూచీ;
    పూర్ణాంకం	    event;
};

నిర్మాణం Vim_సందేశం {
    గణన Vim_సందేశం_Type {
	కీలకం_2, Resize, ScrollBar, Menu, Mouse, MouseMoved, Focus, Refs, Tabline, TablineMenu
    };

    సంఘం {
	నిర్మాణం Vim_కీలకం_సందేశం    కీలకం_2;
	నిర్మాణం Vim_Resize_సందేశం New_కొలత;
	నిర్మాణం Vim_Scroll_Bar_సందేశం	Scroll;
	నిర్మాణం Vim_Menu_సందేశం   Menu;
	నిర్మాణం Vim_Mouse_సందేశం  Mouse;
	నిర్మాణం Vim_Mouse_Moved_సందేశం	MouseMoved;
	నిర్మాణం Vim_Focus_సందేశం  Focus;
	నిర్మాణం Vim_Refs_సందేశం   Refs;
	నిర్మాణం Vim_Tabline_సందేశం	Tabline;
	నిర్మాణం Vim_Tabline_Menu_సందేశం    TablineMenu;
    } u;
};

#నిర్వచించు RGB(r, g, b)	((char_u)(r) << 16 | (char_u)(g) << 8 | (char_u)(b) << 0)
#నిర్వచించు GUI_గమ్యం_RGB(g)	{ (char_u)((g) >> 16), (char_u)((g) >> 8), (char_u)((g) >> 0), 255 }

// ---------------- end of header part ----------------

స్థిర నిర్మాణం specialkey
{
    uint16  Be_కీలకాలు;
#నిర్వచించు కీలకం_3(a,b)    ((a)<<8|(b))
#నిర్వచించు K(a)	    కీలకం_3(0,a)		// for ASCII codes
#నిర్వచించు F(b)	    కీలకం_3(1,b)		// for scancodes
    char_u  vim_code0;
    char_u  vim_code1;
} special_కీలకాలు[] =
{
    {K(B_పైకి_ARROW),	'k', 'u'},
    {K(B_కిందికి_ARROW),	    'k', 'd'},
    {K(B_ఎడమ_ARROW),	    'k', 'l'},
    {K(B_కుడి_ARROW),	    'k', 'r'},
    {K(B_BACKSPACE),	    'k', 'b'},
    {K(B_చొప్పించు),	'k', 'I'},
    {K(B_DELETE),	'k', 'D'},
    {K(B_HOME),		'k', 'h'},
    {K(B_చివర),		'@', '7'},
    {K(B_పుట_పైకి),	'k', 'P'},	// XK_Prior
    {K(B_పుట_కిందికి),	    'k', 'N'},	    // XK_Next,

#నిర్వచించు మొదటి_FUNCTION_కీలకం  11
    {F(B_F1_కీలకం),	'k', '1'},
    {F(B_F2_కీలకం),	'k', '2'},
    {F(B_F3_కీలకం),	'k', '3'},
    {F(B_F4_కీలకం),	'k', '4'},
    {F(B_F5_కీలకం),	'k', '5'},
    {F(B_F6_కీలకం),	'k', '6'},
    {F(B_F7_కీలకం),	'k', '7'},
    {F(B_F8_కీలకం),	'k', '8'},
    {F(B_F9_కీలకం),	'k', '9'},
    {F(B_F10_కీలకం),	'k', ';'},

    {F(B_F11_కీలకం),	'F', '1'},
    {F(B_F12_కీలకం),	'F', '2'},
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
    {F(B_PRINT_కీలకం),	    '%', '9'},

#ఒకవేళ 0
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
#అయితే_ముగింపు

    // End of list marker:
    {0,		    0, 0}
};

#నిర్వచించు అంకె_SPECIAL_కీలకాలు    ARRAY_నిడివి(special_కీలకాలు)

// ---------------- VimApp ----------------

    స్థిర శూన్య
docd(BPath &మార్గం)
{
    mch_chdir((అక్షరం *)మార్గం.మార్గం_2());
    // Do this to get the side effects of a :cd command
    do_cmdline_cmd((char_u *)"cd .");
}

	స్థిర శూన్య
drop_callback(శూన్య *cookie)
{
    // TODO here we could handle going to a specific position in the dropped
    // file (see src/gui_mac.c, deleted in 8.2.1422)
    // Update the screen display
    update_screen(UPD_NOT_VALID);
}

    // Really handle dropped files and folders.
	స్థిర శూన్య
RefsReceived(BMessage *m, తార్కిక changedir)
{
    uint32 type;
    int32 లెక్క;

    m->Print_గమ్యం_ప్రవాహం();
    ఎంపిక (m->what) {
	స్థితి B_REFS_RECEIVED:
	స్థితి B_SIMPLE_దత్తాంశం:
	    m->GetInfo("refs", &type, &లెక్క);
	    ఒకవేళ (type != B_REF_TYPE)
		వెళ్ళు bad;
	    ఆపు;
	స్థితి B_ARGV_RECEIVED:
	    m->GetInfo("argv", &type, &లెక్క);
	    ఒకవేళ (type != B_STRING_TYPE)
		వెళ్ళు bad;
	    ఒకవేళ (changedir) {
		అక్షరం *dirname;
		ఒకవేళ (m->కనుగొను_String("cwd", (స్థిరరాశి అక్షరం **) &dirname) == B_OK) {
		    chdir(dirname);
		    do_cmdline_cmd((char_u *)"cd .");
		}
	    }
	    ఆపు;
	అప్రమేయ:
bad:
	    /*fprintf(stderr, "bad!\n"); */
	    తొలగించు m;
	    తిరిగి;
    }

#నిర్వచితమైతే FEAT_VISUAL
    reset_VIsual();
#అయితే_ముగింపు

    char_u  **fnames;
    fnames = (char_u **) alloc(లెక్క * పరిమాణం(char_u *));
    పూర్ణాంకం fname_సూచీ = 0;

    ఎంపిక (m->what) {
	స్థితి B_REFS_RECEIVED:
	స్థితి B_SIMPLE_దత్తాంశం:
	    // fprintf(stderr, "case B_REFS_RECEIVED\n");
	    కొరకు (పూర్ణాంకం i = 0; i < లెక్క; ++i)
	    {
		నమోదు_ref ref;
		ఒకవేళ (m->కనుగొను_Ref("refs", i, &ref) == B_OK) {
		    BEntry నమోదు_2(&ref, అబద్ధం);
		    BPath మార్గం;
		    నమోదు_2.Get_మార్గం(&మార్గం);

		    // Change to parent directory?
		    ఒకవేళ (changedir) {
			BPath parentpath;
			మార్గం.Get_మాతృ(&parentpath);
			docd(parentpath);
		    }

		    // Is it a directory? If so, cd into it.
		    BDirectory bdir(&ref);
		    ఒకవేళ (bdir.InitCheck() == B_OK) {
			// don't cd if we already did it
			ఒకవేళ (!changedir)
			    docd(మార్గం);
		    } లేకపోతే {
			mch_dirname(IObuff, IOSIZE);
			char_u *fname = shorten_fname((char_u *)మార్గం.మార్గం_2(), IObuff);
			ఒకవేళ (fname == ఖాళీ)
			    fname = (char_u *)మార్గం.మార్గం_2();
			fnames[fname_సూచీ++] = vim_strsave(fname);
			// fprintf(stderr, "%s\n", fname);
		    }

		    // Only do it for the first file/dir
		    changedir = అబద్ధం;
		}
	    }
	    ఆపు;
	స్థితి B_ARGV_RECEIVED:
	    // fprintf(stderr, "case B_ARGV_RECEIVED\n");
	    కొరకు (పూర్ణాంకం i = 1; i < లెక్క; ++i)
	    {
		అక్షరం *fname;

		ఒకవేళ (m->కనుగొను_String("argv", i, (స్థిరరాశి అక్షరం **) &fname) == B_OK) {
		    fnames[fname_సూచీ++] = vim_strsave((char_u *)fname);
		}
	    }
	    ఆపు;
	అప్రమేయ:
	    // fprintf(stderr, "case default\n");
	    ఆపు;
    }

    తొలగించు m;

    // Handle the drop, :edit to get to the file
    ఒకవేళ (fname_సూచీ > 0) {
	handle_drop(fname_సూచీ, fnames, FALSE, drop_callback, ఖాళీ);

	setcursor();
	బహిర్గతం_flush();
    } లేకపోతే {
	vim_free(fnames);
    }
}

VimApp::VimApp(స్థిరరాశి అక్షరం *appsig):
    BApplication(appsig),
    fFilePanelSem(-1),
    fFilePanel(ఖాళీ)
{
}

VimApp::~VimApp()
{
}

    శూన్య
VimApp::Ready_గమ్యం_నిరంతరం()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);
}

    శూన్య
VimApp::ArgvReceived(int32 arg_argc, అక్షరం **arg_argv)
{
    ఒకవేళ (!IsLaunching()) {
	/*
	 * This can happen if we are set to Single or Exclusive
	 * Launch. Be nice and open the file(s).
	 */
	ఒకవేళ (gui.vim_కిటికీ)
	    gui.vim_కిటికీ->Minimize(అబద్ధం);
	BMessage *m = Current_సందేశం();
	Detach_Current_సందేశం();
	SendRefs(m, నిజం);
    }
}

    శూన్య
VimApp::RefsReceived(BMessage *m)
{
    // Horrible hack!!! XXX XXX XXX
    // The real problem is that b_start_ffc is set too late for
    // the initial empty buffer. As a result the window will be
    // split instead of abandoned.
    పూర్ణాంకం హద్దు = 15;
    అయితే (--హద్దు >= 0 && (curbuf == ఖాళీ || curbuf->b_ప్రారంభం_ffc == 0))
	snooze(100000);    //  0.1 s
    ఒకవేళ (gui.vim_కిటికీ)
	gui.vim_కిటికీ->Minimize(అబద్ధం);
    Detach_Current_సందేశం();
    SendRefs(m, నిజం);
}

/*
 * Pass a BMessage on to the main() thread.
 * Caller must have detached the message.
 */
    శూన్య
VimApp::SendRefs(BMessage *m, తార్కిక changedir)
{
    Vim_Refs_సందేశం rm;
    rm.సందేశం_2 = m;
    rm.changedir = changedir;

    write_పోర్టు(gui.vdcmp, Vim_సందేశం::Refs, &rm, పరిమాణం(rm));
    //	calls ::RefsReceived
}

    శూన్య
VimApp::సందేశం_Received(BMessage *m)
{
    ఎంపిక (m->what) {
	స్థితి 'save':
	    {
		నమోదు_ref refDirectory;
		m->కనుగొను_Ref("directory", &refDirectory);
		f_Browsed_మార్గం.సమితి_గమ్యం(&refDirectory);
		BString strName;
		m->కనుగొను_String("name", &strName);
		f_Browsed_మార్గం.Append(strName.String());
	    }
	    ఆపు;
	స్థితి 'open':
	    {
		నమోదు_ref ref;
		m->కనుగొను_Ref("refs", &ref);
		f_Browsed_మార్గం.సమితి_గమ్యం(&ref);
	    }
	    ఆపు;
	స్థితి B_CANCEL:
	    {
		BFilePanel *panel;
		m->కనుగొను_సూచిక("source", (శూన్య**)&panel);
		ఒకవేళ (fFilePanelSem != -1 && panel == fFilePanel)
		{
		    delete_sem(fFilePanelSem);
		    fFilePanelSem = -1;
		}

	    }
	    ఆపు;
	అప్రమేయ:
	    Inherited::సందేశం_Received(m);
	    ఆపు;
    }
}

    తార్కిక
VimApp::QuitRequested()
{
    (శూన్య)Inherited::QuitRequested();
    తిరిగి అబద్ధం;
}

// ---------------- VimWindow ----------------

Vim_కిటికీ::Vim_కిటికీ():
    BWindow(BRect(40, 40, 150, 150),
	    "Vim",
	    B_TITLED_కిటికీ,
	    0,
	    B_CURRENT_WORKSPACE)

{
    init();
}

Vim_కిటికీ::~Vim_కిటికీ()
{
    ఒకవేళ (formView) {
	Remove_బిడ్డ(formView);
	తొలగించు formView;
    }
    gui.vim_కిటికీ = ఖాళీ;
}

    శూన్య
Vim_కిటికీ::init()
{
    // Attach the VimFormView
    formView = కొత్త VimFormView(Bounds());
    ఒకవేళ (formView != ఖాళీ) {
	కూడు_బిడ్డ(formView);
    }
}

#ఒకవేళ 0  //  disabled in zeta patch
    శూన్య
Vim_కిటికీ::Dispatch_సందేశం(BMessage *m, BHandler *h)
{
    /*
     * Route B_MOUSE_UP messages to MouseUp(), in
     * a manner that should be compatible with the
     * intended future system behaviour.
     */
    ఎంపిక (m->what) {
	స్థితి B_MOUSE_పైకి:
	    //	if (!h) h = PreferredHandler();
	    //	gcc isn't happy without this extra set of braces, complains about
	    //	jump to case label crosses init of 'class BView * v'
	    //	richard@whitequeen.com jul 99
	    {
		BView *v = dynamic_cast<BView *>(h);
		ఒకవేళ (v) {
		    // m->PrintToStream();
		    BPoint where;
		    m->కనుగొను_బిందువు("where", &where);
		    v->Mouse_పైకి(where);
		} లేకపోతే {
		    Inherited::Dispatch_సందేశం(m, h);
		}
	    }
	    ఆపు;
	అప్రమేయ:
	    Inherited::Dispatch_సందేశం(m, h);
    }
}
#అయితే_ముగింపు

    శూన్య
Vim_కిటికీ::కిటికీ_Activated(తార్కిక చురుకైన)
{
    Inherited::కిటికీ_Activated(చురుకైన);
    // the textArea gets the keyboard action
    ఒకవేళ (చురుకైన && gui.vim_పాఠ్యం_Area)
	gui.vim_పాఠ్యం_Area->MakeFocus(నిజం);

    నిర్మాణం Vim_Focus_సందేశం fm;
    fm.చురుకైన = చురుకైన;

    write_పోర్టు(gui.vdcmp, Vim_సందేశం::Focus, &fm, పరిమాణం(fm));
}

    తార్కిక
Vim_కిటికీ::QuitRequested()
{
    నిర్మాణం Vim_కీలకం_సందేశం km;
    km.నిడివి = 5;
    జ్ఞాపకం_నకలు((అక్షరం *)km.chars, "\033:qa\r", km.నిడివి);
    km.csi_escape = అబద్ధం;
    write_పోర్టు(gui.vdcmp, Vim_సందేశం::కీలకం_2, &km, పరిమాణం(km));
    తిరిగి అబద్ధం;
}
    శూన్య
Vim_కిటికీ::Dispatch_సందేశం(BMessage *m, BHandler *h)
{
    తార్కిక should_propagate = నిజం;

    ఎంపిక (m->what)
    {
	స్థితి B_కీలకం_కిందికి:
	    {
		int32 scancode = 0;
		int32 beModifiers = 0;
		m->కనుగొను_Int32("raw_char", &scancode);
		m->కనుగొను_Int32("modifiers", &beModifiers);

		ఒకవేళ (scancode == B_ENTER && (beModifiers & B_ఎడమ_COMMAND_కీలకం))
		    {
			should_propagate = అబద్ధం;
			ఒకవేళ (ఇది->is_fullscreen)
			    {
				ఇది->is_fullscreen = అబద్ధం;
				Resize_గమ్యం(ఇది->saved_చట్రం.వెడల్పు_2(), ఇది->saved_చట్రం.ఎత్తు_2());
				Move_గమ్యం(ఇది->saved_చట్రం.ఎడమ, ఇది->saved_చట్రం.శిఖరం);
				సమితి_Look(ఇది->saved_look);
				సమితి_Flags(Flags() & ~(B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    } లేకపోతే {
				ఇది->saved_చట్రం = చట్రం_2();
				ఇది->saved_look = Look();
				ఇది->is_fullscreen = నిజం;
				BScreen s(ఇది);
				సమితి_Look(B_NO_BORDER_కిటికీ_LOOK);
				Resize_గమ్యం(s.చట్రం_2().వెడల్పు_2() + 1, s.చట్రం_2().ఎత్తు_2() + 1);
				Move_గమ్యం(s.చట్రం_2().ఎడమ, s.చట్రం_2().శిఖరం);
				సమితి_Flags(Flags() | (B_NOT_RESIZABLE | B_NOT_MOVABLE));
			    }
		    }
	    }
    }

    ఒకవేళ (should_propagate)
	Inherited::Dispatch_సందేశం(m, h);
}


// ---------------- VimFormView ----------------

VimFormView::VimFormView(BRect చట్రం):
    BView(చట్రం, "VimFormView", B_FOLLOW_ALL_SIDES,
	    B_WILL_DRAW | B_చట్రం_EVENTS),
    menuBar(ఖాళీ),
#నిర్వచితమైతే FEAT_TOOLBAR
    toolBar(ఖాళీ),
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_TABLINE
//  showingTabLine(false),
    tab_పంక్తి(ఖాళీ),
#అయితే_ముగింపు
    పాఠ్యం_Area(ఖాళీ)
{
    init(చట్రం);
}

VimFormView::~VimFormView()
{
    ఒకవేళ (menuBar) {
	Remove_బిడ్డ(menuBar);
#నిర్వచితమైతే never
	//  deleting the menuBar leads to SEGV on exit
	//  richard@whitequeen.com Jul 99
	తొలగించు menuBar;
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_TOOLBAR
    తొలగించు toolBar;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_TABLINE
    తొలగించు tab_పంక్తి;
#అయితే_ముగింపు

    ఒకవేళ (పాఠ్యం_Area) {
	Remove_బిడ్డ(పాఠ్యం_Area);
	తొలగించు పాఠ్యం_Area;
    }
    gui.vimForm = ఖాళీ;
}

    శూన్య
VimFormView::init(BRect చట్రం)
{
    menuBar = కొత్త BMenuBar(BRect(0,0,-MENUBAR_MARGIN,-MENUBAR_MARGIN),
	    "VimMenuBar");

    కూడు_బిడ్డ(menuBar);

#నిర్వచితమైతే FEAT_TOOLBAR
    toolBar = కొత్త VimToolbar(BRect(0,0,0,0), "VimToolBar");
    toolBar->PrepareButtonBitmaps();
    కూడు_బిడ్డ(toolBar);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_TABLINE
    tab_పంక్తి = కొత్త Vim_Tab_పంక్తి(BRect(0,0,0,0));
//  tabLine->PrepareButtonBitmaps();
    కూడు_బిడ్డ(tab_పంక్తి);
#అయితే_ముగింపు

    BRect మిగిలిన = చట్రం;
    పాఠ్యం_Area = కొత్త Vim_పాఠ్యం_Area_View(మిగిలిన);
    కూడు_బిడ్డ(పాఠ్యం_Area);
    // The textArea will be resized later when menus are added

    gui.vimForm = ఇది;
}

#నిర్వచితమైతే FEAT_TOOLBAR
    దశాంశం
VimFormView::Toolbar_ఎత్తు() స్థిరరాశి
{
    తిరిగి toolBar ? toolBar->Toolbar_ఎత్తు() : 0.;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_TABLINE
    దశాంశం
VimFormView::Tabline_ఎత్తు() స్థిరరాశి
{
    తిరిగి (tab_పంక్తి && Is_Showing_Tab_పంక్తి()) ? tab_పంక్తి->Tabline_ఎత్తు() : 0.;
}
#అయితే_ముగింపు

    శూన్య
VimFormView::AllAttached()
{
    /*
     * Apparently signals are inherited by the created thread -
     * disable the most annoying ones.
     */
    mch_signal(SIGINT, SIG_IGN);
    mch_signal(SIGQUIT, SIG_IGN);

    ఒకవేళ (menuBar && పాఠ్యం_Area) {
	/*
	 * Resize the textArea to fill the space left over by the menu.
	 * This is somewhat futile since it will be done again once
	 * menus are added to the menu bar.
	 */
	BRect మిగిలిన = Bounds();

#నిర్వచితమైతే FEAT_MENU
	మిగిలిన.శిఖరం += Menu_ఎత్తు();
	menuBar->Resize_గమ్యం(మిగిలిన.కుడి, మిగిలిన.శిఖరం);
	gui.menu_ఎత్తు = (పూర్ణాంకం) Menu_ఎత్తు();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TOOLBAR
	toolBar->Move_గమ్యం(మిగిలిన.ఎడమ, మిగిలిన.శిఖరం);
	toolBar->Resize_గమ్యం(మిగిలిన.కుడి, Toolbar_ఎత్తు());
	మిగిలిన.శిఖరం += Toolbar_ఎత్తు();
	gui.toolbar_ఎత్తు = Toolbar_ఎత్తు();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_TABLINE
	tab_పంక్తి->Move_గమ్యం(మిగిలిన.ఎడమ, మిగిలిన.శిఖరం);
	tab_పంక్తి->Resize_గమ్యం(మిగిలిన.కుడి + 1, Tabline_ఎత్తు());
	మిగిలిన.శిఖరం += Tabline_ఎత్తు();
	gui.tabline_ఎత్తు = Tabline_ఎత్తు();
#అయితే_ముగింపు

	పాఠ్యం_Area->Resize_గమ్యం(మిగిలిన.వెడల్పు_2(), మిగిలిన.ఎత్తు_2());
	పాఠ్యం_Area->Move_గమ్యం(మిగిలిన.ఎడమ, మిగిలిన.శిఖరం);
    }


    Inherited::AllAttached();
}

    శూన్య
VimFormView::చట్రం_Resized(దశాంశం new_వెడల్పు, దశాంశం new_ఎత్తు)
{
    నిర్మాణం Vim_Resize_సందేశం sm;
    పూర్ణాంకం adjust_h, adjust_w;

    new_వెడల్పు += 1;	//  adjust from width to number of pixels occupied
    new_ఎత్తు += 1;

    sm.వెడల్పు = (పూర్ణాంకం) new_వెడల్పు;
    sm.ఎత్తు = (పూర్ణాంకం) new_ఎత్తు;
    adjust_w = ((పూర్ణాంకం)new_వెడల్పు - gui_get_పునాది_వెడల్పు()) % gui.char_వెడల్పు;
    adjust_h = ((పూర్ణాంకం)new_ఎత్తు - gui_get_పునాది_ఎత్తు()) % gui.char_ఎత్తు;

    ఒకవేళ (adjust_w > 0 || adjust_h > 0) {
	sm.వెడల్పు  -= adjust_w;
	sm.ఎత్తు -= adjust_h;
    }

    write_పోర్టు(gui.vdcmp, Vim_సందేశం::Resize, &sm, పరిమాణం(sm));
    //	calls gui_resize_shell(new_width, new_height);

    తిరిగి;

    /*
     * The area below the vertical scrollbar is erased to the colour
     * set with SetViewColor() automatically, because we had set
     * B_WILL_DRAW. Resizing the window tight around the vertical
     * scroll bar also helps to avoid debris.
     */
}

// ---------------- VimTextAreaView ----------------

Vim_పాఠ్యం_Area_View::Vim_పాఠ్యం_Area_View(BRect చట్రం):
    BView(చట్రం, "VimTextAreaView", B_FOLLOW_ALL_SIDES,
#నిర్వచితమైతే FEAT_MBYTE_IME
	B_WILL_DRAW | B_నిండిన_UPDATE_ON_RESIZE | B_ఆదానం_పద్ధతి_AWARE
#లేకపోతే
	B_WILL_DRAW | B_నిండిన_UPDATE_ON_RESIZE
#అయితే_ముగింపు
	),
    mouse_Drag_Event_లెక్క(0)
{
#నిర్వచితమైతే FEAT_MBYTE_IME
    IMData.messenger = ఖాళీ;
    IMData.సందేశం_2 = ఖాళీ;
#అయితే_ముగింపు
    init(చట్రం);
}

Vim_పాఠ్యం_Area_View::~Vim_పాఠ్యం_Area_View()
{
    gui.vim_పాఠ్యం_Area = ఖాళీ;
}

    శూన్య
Vim_పాఠ్యం_Area_View::init(BRect చట్రం)
{
    // set up global var for fast access
    gui.vim_పాఠ్యం_Area = ఇది;

    /*
     * Tell the app server not to erase the view: we will
     * fill it in completely by ourselves.
     * (Does this really work? Even if not, it won't harm either.)
     */
    సమితి_View_Color(B_TRANSPARENT_32_బిట్టు);
#నిర్వచించు PEN_వెడల్పు   1
    సమితి_Pen_కొలత(PEN_వెడల్పు);
#నిర్వచించు W_వెడల్పు(curwin)   0
}

    శూన్య
Vim_పాఠ్యం_Area_View::Draw(BRect updateRect)
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
    rgb_color rgb = GUI_గమ్యం_RGB(gui.వెనుక_pixel);
    సమితి_దిగువ_Color(rgb);
    FillRect(updateRect, B_SOLID_దిగువ);
    gui_redraw((పూర్ణాంకం) updateRect.ఎడమ, (పూర్ణాంకం) updateRect.శిఖరం,
	    (పూర్ణాంకం) (updateRect.వెడల్పు_2() + PEN_వెడల్పు), (పూర్ణాంకం) (updateRect.ఎత్తు_2() + PEN_వెడల్పు));

    // Clear the border areas if needed
    సమితి_దిగువ_Color(rgb);

    ఒకవేళ (updateRect.ఎడమ < FILL_X(0))	//  left border
	FillRect(BRect(updateRect.ఎడమ, updateRect.శిఖరం,
		    FILL_X(0)-PEN_వెడల్పు, updateRect.bottom), B_SOLID_దిగువ);
    ఒకవేళ (updateRect.శిఖరం < FILL_Y(0)) //	top border
	FillRect(BRect(updateRect.ఎడమ, updateRect.శిఖరం,
		    updateRect.కుడి, FILL_Y(0)-PEN_వెడల్పు), B_SOLID_దిగువ);
    ఒకవేళ (updateRect.కుడి >= FILL_X(Columns)) //  right border
	FillRect(BRect(FILL_X((పూర్ణాంకం)Columns), updateRect.శిఖరం,
		    updateRect.కుడి, updateRect.bottom), B_SOLID_దిగువ);
    ఒకవేళ (updateRect.bottom >= FILL_Y(అడ్డువరుసలు))   //  bottom border
	FillRect(BRect(updateRect.ఎడమ, FILL_Y((పూర్ణాంకం)అడ్డువరుసలు),
		    updateRect.కుడి, updateRect.bottom), B_SOLID_దిగువ);

#నిర్వచితమైతే FEAT_MBYTE_IME
    DrawIMString();
#అయితే_ముగింపు
}

    శూన్య
Vim_పాఠ్యం_Area_View::కీలకం_కిందికి(స్థిరరాశి అక్షరం *బైట్లు, int32 అంకె_బైట్లు_2)
{
    నిర్మాణం Vim_కీలకం_సందేశం km;
    char_u *dest = km.chars;

    తార్కిక canHaveVimModifiers = అబద్ధం;

    BMessage *సందేశం = కిటికీ()->Current_సందేశం();
    ప్రతిజ్ఞా(సందేశం);
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
    ఒకవేళ (అంకె_బైట్లు_2 > 1) {
	// This cannot be a special key
	ఒకవేళ (అంకె_బైట్లు_2 > కీలకం_సందేశం_BUFSIZ)
	    అంకె_బైట్లు_2 = కీలకం_సందేశం_BUFSIZ;	    //	should never happen... ???
	km.నిడివి = అంకె_బైట్లు_2;
	జ్ఞాపకం_నకలు((అక్షరం *)dest, బైట్లు, అంకె_బైట్లు_2);
	km.csi_escape = నిజం;
    } లేకపోతే {
	int32 scancode = 0;
	సందేశం->కనుగొను_Int32("key", &scancode);

	int32 beModifiers = 0;
	సందేశం->కనుగొను_Int32("modifiers", &beModifiers);

	char_u string[3];
	పూర్ణాంకం len = 0;
	km.నిడివి = 0;

	/*
	 * For normal, printable ASCII characters, don't look them up
	 * to check if they might be a special key. They aren't.
	 */
	ప్రతిజ్ఞా(B_BACKSPACE <= 0x20);
	ప్రతిజ్ఞా(B_DELETE == 0x7F);
	ఒకవేళ (((char_u)బైట్లు[0] <= 0x20 || (char_u)బైట్లు[0] == 0x7F) &&
		అంకె_బైట్లు_2 == 1) {
	    /*
	     * Due to the great nature of Be's mapping of special keys,
	     * viz. into the range of the control characters,
	     * we can only be sure it is *really* a special key if
	     * if it is special without using ctrl. So, only if ctrl is
	     * used, we need to check it unmodified.
	     */
	    ఒకవేళ (beModifiers & B_CONTROL_కీలకం) {
		పూర్ణాంకం సూచీ = కీలకం_Map->సాధారణ_map[scancode];
		పూర్ణాంకం new_అంకె_బైట్లు = కీలకం_Map_Chars[సూచీ];
		char_u *new_బైట్లు = (char_u *)&కీలకం_Map_Chars[సూచీ + 1];

		/*
		 * Check if still special without the control key.
		 * This is needed for BACKSPACE: that key does produce
		 * different values with modifiers (DEL).
		 * Otherwise we could simply have checked for equality.
		 */
		ఒకవేళ (new_అంకె_బైట్లు != 1 || (*new_బైట్లు > 0x20 &&
			    *new_బైట్లు != 0x7F )) {
		    వెళ్ళు notspecial;
		}
		బైట్లు = (అక్షరం *)new_బైట్లు;
	    }
	    canHaveVimModifiers = నిజం;

	    uint16 beoskey;
	    పూర్ణాంకం మొదటి, చివరి;

	    /*
	     * If numBytes == 0 that probably always indicates a special key.
	     * (does not happen yet)
	     */
	    ఒకవేళ (అంకె_బైట్లు_2 == 0 || బైట్లు[0] == B_FUNCTION_కీలకం) {
		beoskey = F(scancode);
		మొదటి = మొదటి_FUNCTION_కీలకం;
		చివరి = అంకె_SPECIAL_కీలకాలు;
	    } లేకపోతే ఒకవేళ (*బైట్లు == '\n' && scancode == 0x47) {
		// remap the (non-keypad) ENTER key from \n to \r.
		string[0] = '\r';
		len = 1;
		మొదటి = చివరి = 0;
	    } లేకపోతే {
		beoskey = K(బైట్లు[0]);
		మొదటి = 0;
		చివరి = మొదటి_FUNCTION_కీలకం;
	    }

	    కొరకు (పూర్ణాంకం i = మొదటి; i < చివరి; i++) {
		ఒకవేళ (special_కీలకాలు[i].Be_కీలకాలు == beoskey) {
		    string[0] = CSI;
		    string[1] = special_కీలకాలు[i].vim_code0;
		    string[2] = special_కీలకాలు[i].vim_code1;
		    len = 3;
		}
	    }
	}
notspecial:
	ఒకవేళ (len == 0) {
	    string[0] = బైట్లు[0];
	    len = 1;
	}

	// Special keys (and a few others) may have modifiers
#ఒకవేళ 0
	ఒకవేళ (len == 3 ||
		బైట్లు[0] == B_SPACE || బైట్లు[0] == B_TAB ||
		బైట్లు[0] == B_RETURN || బైట్లు[0] == '\r' ||
		బైట్లు[0] == B_ESCAPE)
#లేకపోతే
	    ఒకవేళ (canHaveVimModifiers)
#అయితే_ముగింపు
	    {
		పూర్ణాంకం modifiers;
		modifiers = 0;
		ఒకవేళ (beModifiers & B_జరుపు_కీలకం)
		    modifiers |= MOD_ముసుగు_జరుపు;
		ఒకవేళ (beModifiers & B_CONTROL_కీలకం)
		    modifiers |= MOD_ముసుగు_CTRL;
		ఒకవేళ (beModifiers & B_OPTION_కీలకం)
		    modifiers |= MOD_ముసుగు_ALT;

		/*
		 * For some keys a shift modifier is translated into another key
		 * code.  Do we need to handle the case where len != 1 and
		 * string[0] != CSI? (Not for BeOS, since len == 3 implies
		 * string[0] == CSI...)
		 */
		పూర్ణాంకం కీలకం;
		ఒకవేళ (string[0] == CSI && len == 3)
		    కీలకం = గమ్యం_SPECIAL(string[1], string[2]);
		లేకపోతే
		    కీలకం = string[0];
		కీలకం = simplify_కీలకం(కీలకం, &modifiers);
		ఒకవేళ (IS_SPECIAL(కీలకం))
		{
		    string[0] = CSI;
		    string[1] = K_రెండవ(కీలకం);
		    string[2] = K_మూడవ(కీలకం);
		    len = 3;
		}
		లేకపోతే
		{
		    string[0] = కీలకం;
		    len = 1;
		}

		ఒకవేళ (modifiers)
		{
		    *dest++ = CSI;
		    *dest++ = KS_MODIFIER;
		    *dest++ = modifiers;
		    km.నిడివి = 3;
		}
	    }
	జ్ఞాపకం_నకలు((అక్షరం *)dest, string, len);
	km.నిడివి += len;
	km.csi_escape = అబద్ధం;
    }

    write_పోర్టు(gui.vdcmp, Vim_సందేశం::కీలకం_2, &km, పరిమాణం(km));

    /*
     * blank out the pointer if necessary
     */
    ఒకవేళ (p_mh && !gui.సూచిక_hidden)
    {
	guiBlankMouse(నిజం);
	gui.సూచిక_hidden = TRUE;
    }
}
శూన్య
Vim_పాఠ్యం_Area_View::guiSendMouseEvent(
	పూర్ణాంకం	button,
	పూర్ణాంకం	x,
	పూర్ణాంకం	y,
	పూర్ణాంకం	repeated_click,
	int_u	modifiers)
{
    Vim_Mouse_సందేశం mm;

    mm.button = button;
    mm.x = x;
    mm.y = y;
    mm.repeated_click = repeated_click;
    mm.modifiers = modifiers;

    write_పోర్టు(gui.vdcmp, Vim_సందేశం::Mouse, &mm, పరిమాణం(mm));
    //	calls gui_send_mouse_event()

    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ఒకవేళ (gui.సూచిక_hidden)
    {
	guiBlankMouse(అబద్ధం);
	gui.సూచిక_hidden = FALSE;
    }
}

శూన్య
Vim_పాఠ్యం_Area_View::guiMouseMoved(
	పూర్ణాంకం	x,
	పూర్ణాంకం	y)
{
    Vim_Mouse_Moved_సందేశం mm;

    mm.x = x;
    mm.y = y;

    write_పోర్టు(gui.vdcmp, Vim_సందేశం::MouseMoved, &mm, పరిమాణం(mm));

    ఒకవేళ (gui.సూచిక_hidden)
    {
	guiBlankMouse(అబద్ధం);
	gui.సూచిక_hidden = FALSE;
    }
}

    శూన్య
Vim_పాఠ్యం_Area_View::guiBlankMouse(తార్కిక should_hide)
{
    ఒకవేళ (should_hide) {
	// gui.vimApp->HideCursor();
	gui.vimApp->ObscureCursor();
	/*
	 * ObscureCursor() would even be easier, but then
	 * Vim's idea of mouse visibility does not necessarily
	 * correspond to reality.
	 */
    } లేకపోతే {
	// gui.vimApp->ShowCursor();
    }
}

    int_u
Vim_పాఠ్యం_Area_View::mouse_Modifiers_గమ్యం_Vim(int32 beModifiers)
{
    int_u vim_modifiers = 0x0;

    ఒకవేళ (beModifiers & B_జరుపు_కీలకం)
	vim_modifiers |= MOUSE_జరుపు;
    ఒకవేళ (beModifiers & B_CONTROL_కీలకం)
	vim_modifiers |= MOUSE_CTRL;
    ఒకవేళ (beModifiers & B_OPTION_కీలకం)	// Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    తిరిగి vim_modifiers;
}

    శూన్య
Vim_పాఠ్యం_Area_View::Mouse_కిందికి(BPoint బిందువు)
{
    BMessage *m = కిటికీ()->Current_సందేశం();
    ప్రతిజ్ఞా(m);

    int32 buttons = 0;
    m->కనుగొను_Int32("buttons", &buttons);

    పూర్ణాంకం vimButton;

    ఒకవేళ (buttons & B_PRIMARY_MOUSE_BUTTON)
	vimButton = MOUSE_ఎడమ;
    లేకపోతే ఒకవేళ (buttons & B_SECONDARY_MOUSE_BUTTON)
	vimButton = MOUSE_కుడి;
    లేకపోతే ఒకవేళ (buttons & B_TERTIARY_MOUSE_BUTTON)
	vimButton = MOUSE_మధ్య;
    లేకపోతే
	తిరిగి;		// Unknown button

    vimMouseButton = 1;	    // don't care which one

    // Handle multiple clicks
    int32 clicks = 0;
    m->కనుగొను_Int32("clicks", &clicks);

    int32 modifiers = 0;
    m->కనుగొను_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_గమ్యం_Vim(modifiers);

    guiSendMouseEvent(vimButton, బిందువు.x, బిందువు.y,
	    clicks > 1 /* = repeated_click*/, vimMouseModifiers);
}

    శూన్య
Vim_పాఠ్యం_Area_View::Mouse_పైకి(BPoint బిందువు)
{
    vimMouseButton = 0;

    BMessage *m = కిటికీ()->Current_సందేశం();
    ప్రతిజ్ఞా(m);
    // m->PrintToStream();

    int32 modifiers = 0;
    m->కనుగొను_Int32("modifiers", &modifiers);

    vimMouseModifiers = mouse_Modifiers_గమ్యం_Vim(modifiers);

    guiSendMouseEvent(MOUSE_RELEASE, బిందువు.x, బిందువు.y,
	    0 /* = repeated_click*/, vimMouseModifiers);

    Inherited::Mouse_పైకి(బిందువు);
}

    శూన్య
Vim_పాఠ్యం_Area_View::MouseMoved(BPoint బిందువు, uint32 transit, స్థిరరాశి BMessage *సందేశం_2)
{
    /*
     * if our pointer is currently hidden, then we should show it.
     */
    ఒకవేళ (gui.సూచిక_hidden)
    {
	guiBlankMouse(అబద్ధం);
	gui.సూచిక_hidden = FALSE;
    }

    ఒకవేళ (!vimMouseButton) {    // could also check m->"buttons"
	guiMouseMoved(బిందువు.x, బిందువు.y);
	తిరిగి;
    }

    atomic_కూడు(&mouse_Drag_Event_లెక్క, 1);

    // Don't care much about "transit"
    guiSendMouseEvent(MOUSE_DRAG, బిందువు.x, బిందువు.y, 0, vimMouseModifiers);
}

    శూన్య
Vim_పాఠ్యం_Area_View::సందేశం_Received(BMessage *m)
{
    ఎంపిక (m->what) {
	స్థితి 'menu':
	    {
		Vim_Menu_సందేశం mm;
		mm.guiMenu = ఖాళీ;  // in case no pointer in msg
		m->కనుగొను_సూచిక("VimMenu", (శూన్య **)&mm.guiMenu);
		write_పోర్టు(gui.vdcmp, Vim_సందేశం::Menu, &mm, పరిమాణం(mm));
	    }
	    ఆపు;
	స్థితి B_MOUSE_WHEEL_మారింది:
	    {
		VimScrollBar* scb = curwin->w_scrollbars[1].id;
		దశాంశం small=0, big=0, dy=0;
		m->కనుగొను_Float("be:wheel_delta_y", &dy);
		scb->Get_అడుగులు(&small, &big);
		scb->సమితి_విలువ(scb->విలువ_2()+small*dy*3);
		scb->విలువ_మారింది_2(scb->విలువ_2());
#ఒకవేళ 0
		scb = curwin->w_scrollbars[0].id;
		scb->Get_అడుగులు(&small, &big);
		scb->సమితి_విలువ(scb->విలువ_2()+small*dy);
		scb->విలువ_మారింది_2(scb->విలువ_2());
#అయితే_ముగింపు
	    }
	    ఆపు;
#నిర్వచితమైతే FEAT_MBYTE_IME
	స్థితి B_ఆదానం_పద్ధతి_EVENT:
	    {
		int32 opcode;
		m->కనుగొను_Int32("be:opcode", &opcode);
		ఎంపిక(opcode)
		{
		    స్థితి B_ఆదానం_పద్ధతి_STARTED:
			ఒకవేళ (!IMData.messenger) తొలగించు IMData.messenger;
			IMData.messenger = కొత్త BMessenger();
			m->కనుగొను_Messenger("be:reply_to", IMData.messenger);
			ఆపు;
		    స్థితి B_ఆదానం_పద్ధతి_మారింది:
			{
			    BString str;
			    తార్కిక confirmed;
			    ఒకవేళ (IMData.సందేశం_2) *(IMData.సందేశం_2) = *m;
			    లేకపోతే	       IMData.సందేశం_2 = కొత్త BMessage(*m);
			    DrawIMString();
			    m->కనుగొను_Bool("be:confirmed", &confirmed);
			    ఒకవేళ (confirmed)
			    {
				m->కనుగొను_String("be:string", &str);
				char_u *chars = (char_u*)str.String();
				నిర్మాణం Vim_కీలకం_సందేశం km;
				km.csi_escape = నిజం;
				పూర్ణాంకం clen;
				పూర్ణాంకం i = 0;
				అయితే (i < str.నిడివి_2())
				{
				    clen = utf_ptr2len(chars+i);
				    జ్ఞాపకం_నకలు(km.chars, chars+i, clen);
				    km.నిడివి = clen;
				    write_పోర్టు(gui.vdcmp, Vim_సందేశం::కీలకం_2, &km, పరిమాణం(km));
				    i += clen;
				}
			    }
			}
			ఆపు;
		    స్థితి B_ఆదానం_పద్ధతి_LOCATION_REQUEST:
			{
			    BMessage సందేశం(B_ఆదానం_పద్ధతి_EVENT);
			    సందేశం.కూడు_Int32("be:opcode", B_ఆదానం_పద్ధతి_LOCATION_REQUEST);
			    సందేశం.కూడు_బిందువు("be:location_reply", IMData.location);
			    సందేశం.కూడు_Float("be:height_reply", FILL_Y(1));
			    IMData.messenger->Send_సందేశం(&సందేశం);
			}
			ఆపు;
		    స్థితి B_ఆదానం_పద్ధతి_STOPPED:
			తొలగించు IMData.messenger;
			తొలగించు IMData.సందేశం_2;
			IMData.messenger = ఖాళీ;
			IMData.సందేశం_2 = ఖాళీ;
			ఆపు;
		}
	    }
	    // TODO: sz: break here???
#అయితే_ముగింపు
	అప్రమేయ:
	    ఒకవేళ (m->WasDropped()) {
		BWindow *w = కిటికీ();
		w->Detach_Current_సందేశం();
		w->Minimize(అబద్ధం);
		VimApp::SendRefs(m, (modifiers() & B_జరుపు_కీలకం) != 0);
	    } లేకపోతే {
		Inherited::సందేశం_Received(m);
	    }
	    ఆపు;
    }
}

    పూర్ణాంకం
Vim_పాఠ్యం_Area_View::mchInitFont(char_u *name)
{
    VimFont *newFont = (VimFont *)gui_mch_get_font(name, 1);
    ఒకవేళ (newFont != NOFONT) {
	gui.norm_font = (GuiFont)newFont;
	gui_mch_సమితి_font((GuiFont)newFont);
	ఒకవేళ (name && STRCMP(name, "*") != 0)
	    hl_సమితి_font_name(name);

	సమితి_Drawing_బహుళకం(B_OP_COPY);

	/*
	 * Try to load other fonts for bold, italic, and bold-italic.
	 * We should also try to work out what font to use for these when they are
	 * not specified by X resources, but we don't yet.
	 */
	తిరిగి OK;
    }
    తిరిగి FAIL;
}

    శూన్య
Vim_పాఠ్యం_Area_View::mchDrawString(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, char_u *s, పూర్ణాంకం len, పూర్ణాంకం flags)
{
    /*
     * First we must erase the area, because DrawString won't do
     * that for us. XXX Most of the time this is a waste of effort
     * since the bachground has been erased already... DRAW_TRANSP
     * should be set when appropriate!!!
     * (Rectangles include the bottom and right edge)
     */
    ఒకవేళ (!(flags & DRAW_TRANSP)) {
	పూర్ణాంకం cells;
	cells = 0;
	కొరకు (పూర్ణాంకం i=0; i<len; i++) {
	    పూర్ణాంకం cn = utf_ptr2cells((char_u *)(s+i));
	    ఒకవేళ (cn<4) cells += cn;
	}

	BRect r(FILL_X(నిలువు), FILL_Y(అడ్డువరుస),
		FILL_X(నిలువు + cells) - PEN_వెడల్పు, FILL_Y(అడ్డువరుస + 1) - PEN_వెడల్పు);
	FillRect(r, B_SOLID_దిగువ);
    }

    BFont font;
    ఇది->GetFont(&font);
    ఒకవేళ (!font.IsFixed())
    {
	అక్షరం* p = (అక్షరం*)s;
	int32 clen, lastpos = 0;
	BPoint where;
	పూర్ణాంకం cells;
	అయితే ((p - (అక్షరం*)s) < len) {
	    clen = utf_ptr2len((u_char*)p);
	    where.సమితి(పాఠ్యం_X(నిలువు+lastpos), పాఠ్యం_Y(అడ్డువరుస));
	    DrawString(p, clen, where);
	    ఒకవేళ (flags & DRAW_BOLD) {
		where.x += 1.0;
		సమితి_Drawing_బహుళకం(B_OP_BLEND);
		DrawString(p, clen, where);
		సమితి_Drawing_బహుళకం(B_OP_COPY);
	    }
	    cells = utf_ptr2cells((char_u *)p);
	    ఒకవేళ (cells<4) lastpos += cells;
	    లేకపోతే	lastpos++;
	    p += clen;
	}
    }
    లేకపోతే
    {
	BPoint where(పాఠ్యం_X(నిలువు), పాఠ్యం_Y(అడ్డువరుస));
	DrawString((అక్షరం*)s, len, where);
	ఒకవేళ (flags & DRAW_BOLD) {
	    where.x += 1.0;
	    సమితి_Drawing_బహుళకం(B_OP_BLEND);
	    DrawString((అక్షరం*)s, len, where);
	    సమితి_Drawing_బహుళకం(B_OP_COPY);
	}
    }

    ఒకవేళ (flags & DRAW_UNDERL) {
	పూర్ణాంకం cells;
	cells = 0;
	కొరకు (పూర్ణాంకం i=0; i<len; i++) {
	    పూర్ణాంకం cn = utf_ptr2cells((char_u *)(s+i));
	    ఒకవేళ (cn<4) cells += cn;
	}

	BPoint ప్రారంభం(FILL_X(నిలువు), FILL_Y(అడ్డువరుస + 1) - PEN_వెడల్పు);
	BPoint చివర(FILL_X(నిలువు + cells) - PEN_వెడల్పు, ప్రారంభం.y);

	Stroke_పంక్తి(ప్రారంభం, చివర);
    }
}

శూన్య
Vim_పాఠ్యం_Area_View::mch_Clear_ఖండం(
	పూర్ణాంకం	row1,
	పూర్ణాంకం	col1,
	పూర్ణాంకం	row2,
	పూర్ణాంకం	col2)
{
    BRect r(FILL_X(col1), FILL_Y(row1),
	    FILL_X(col2 + 1) - PEN_వెడల్పు, FILL_Y(row2 + 1) - PEN_వెడల్పు);
    gui_mch_సమితి_bg_color(gui.వెనుక_pixel);
    FillRect(r, B_SOLID_దిగువ);
}

    శూన్య
Vim_పాఠ్యం_Area_View::mchClearAll()
{
    gui_mch_సమితి_bg_color(gui.వెనుక_pixel);
    FillRect(Bounds(), B_SOLID_దిగువ);
}

/*
 * mchDeleteLines() Lock()s the window by itself.
 */
    శూన్య
Vim_పాఠ్యం_Area_View::mch_Delete_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు)
{
    BRect ఆధారం, dest;
    ఆధారం.ఎడమ = FILL_X(gui.scroll_region_ఎడమ);
    ఆధారం.శిఖరం = FILL_Y(అడ్డువరుస + అంకె_పంక్తులు);
    ఆధారం.కుడి = FILL_X(gui.scroll_region_కుడి + 1) - PEN_వెడల్పు;
    ఆధారం.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_వెడల్పు;

    dest.ఎడమ = FILL_X(gui.scroll_region_ఎడమ);
    dest.శిఖరం = FILL_Y(అడ్డువరుస);
    dest.కుడి = FILL_X(gui.scroll_region_కుడి + 1) - PEN_వెడల్పు;
    dest.bottom = FILL_Y(gui.scroll_region_bot - అంకె_పంక్తులు + 1) - PEN_వెడల్పు;

    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	// Clear one column more for when bold has spilled over
	Copy_బిట్లు(ఆధారం, dest);
	gui_clear_ఖండం(gui.scroll_region_bot - అంకె_పంక్తులు + 1,
		gui.scroll_region_ఎడమ,
		gui.scroll_region_bot, gui.scroll_region_కుడి);


	gui.vim_కిటికీ->Unlock();
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
    శూన్య
Vim_పాఠ్యం_Area_View::mch_చొప్పించు_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు)
{
    BRect ఆధారం, dest;

    // XXX Attempt at a hack:
    gui.vim_కిటికీ->UpdateIfNeeded();
    ఆధారం.ఎడమ = FILL_X(gui.scroll_region_ఎడమ);
    ఆధారం.శిఖరం = FILL_Y(అడ్డువరుస);
    ఆధారం.కుడి = FILL_X(gui.scroll_region_కుడి + 1) - PEN_వెడల్పు;
    ఆధారం.bottom = FILL_Y(gui.scroll_region_bot - అంకె_పంక్తులు + 1) - PEN_వెడల్పు;

    dest.ఎడమ = FILL_X(gui.scroll_region_ఎడమ);
    dest.శిఖరం = FILL_Y(అడ్డువరుస + అంకె_పంక్తులు);
    dest.కుడి = FILL_X(gui.scroll_region_కుడి + 1) - PEN_వెడల్పు;
    dest.bottom = FILL_Y(gui.scroll_region_bot + 1) - PEN_వెడల్పు;

    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	// Clear one column more for when bold has spilled over
	Copy_బిట్లు(ఆధారం, dest);
	gui_clear_ఖండం(అడ్డువరుస, gui.scroll_region_ఎడమ,
		అడ్డువరుస + అంకె_పంక్తులు - 1, gui.scroll_region_కుడి);

	gui.vim_కిటికీ->Unlock();
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

#నిర్వచితమైతే FEAT_MBYTE_IME
/*
 * DrawIMString draws string with IMData.message.
 */
శూన్య Vim_పాఠ్యం_Area_View::DrawIMString(శూన్య)
{
    స్థిర స్థిరరాశి rgb_color r_highlight = {255, 152, 152, 255},
		 b_highlight = {152, 203, 255, 255};
    BString str;
    స్థిరరాశి అక్షరం* s;
    పూర్ణాంకం len;
    BMessage* సందేశం = IMData.సందేశం_2;
    ఒకవేళ (!సందేశం)
	తిరిగి;
    gui_redraw_ఖండం(IMData.అడ్డువరుస, 0,
	    IMData.అడ్డువరుస + IMData.లెక్క, W_వెడల్పు(curwin), GUI_MON_NOCLEAR);
    తార్కిక confirmed = అబద్ధం;
    సందేశం->కనుగొను_Bool("be:confirmed", &confirmed);
    ఒకవేళ (confirmed)
	తిరిగి;
    rgb_color hcolor = ఎగువ_Color(), lcolor = దిగువ_Color();
    సందేశం->కనుగొను_String("be:string", &str);
    s = str.String();
    len = str.నిడివి_2();
    సమితి_ఎగువ_Color(0, 0, 0);
    IMData.అడ్డువరుస = gui.అడ్డువరుస;
    IMData.నిలువు = gui.నిలువు;
    int32 sel_ప్రారంభం = 0, sel_చివర = 0;
    సందేశం->కనుగొను_Int32("be:selection", 0, &sel_ప్రారంభం);
    సందేశం->కనుగొను_Int32("be:selection", 1, &sel_చివర);
    పూర్ణాంకం clen, cn;
    BPoint pos(IMData.నిలువు, 0);
    BRect r;
    BPoint where;
    IMData.location = Convert_గమ్యం_Screen(
	    BPoint(FILL_X(pos.x), FILL_Y(IMData.అడ్డువరుస + pos.y)));
    కొరకు (పూర్ణాంకం i=0; i<len; i+=clen)
    {
	cn = utf_ptr2cells((char_u *)(s+i));
	clen = utf_ptr2len((char_u *)(s+i));
	ఒకవేళ (pos.x + cn > W_వెడల్పు(curwin))
	{
	    pos.y++;
	    pos.x = 0;
	}
	ఒకవేళ (sel_ప్రారంభం<=i && i<sel_చివర)
	{
	    సమితి_దిగువ_Color(r_highlight);
	    IMData.location = Convert_గమ్యం_Screen(
		    BPoint(FILL_X(pos.x), FILL_Y(IMData.అడ్డువరుస + pos.y)));
	}
	లేకపోతే
	{
	    సమితి_దిగువ_Color(b_highlight);
	}
	r.సమితి(FILL_X(pos.x), FILL_Y(IMData.అడ్డువరుస + pos.y),
		FILL_X(pos.x + cn) - PEN_వెడల్పు,
		FILL_Y(IMData.అడ్డువరుస + pos.y + 1) - PEN_వెడల్పు);
	FillRect(r, B_SOLID_దిగువ);
	where.సమితి(పాఠ్యం_X(pos.x), పాఠ్యం_Y(IMData.అడ్డువరుస + pos.y));
	DrawString((s+i), clen, where);
	pos.x += cn;
    }
    IMData.లెక్క = (పూర్ణాంకం)pos.y;

    సమితి_ఎగువ_Color(hcolor);
    సమితి_దిగువ_Color(lcolor);
}
#అయితే_ముగింపు
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
	    "vim scrollbar", (BView *)ఖాళీ,
	    0.0, 10.0, posture),
    ignore_విలువ(-1),
    scroll_Event_లెక్క(0)
{
    gsb = g;
    సమితి_Resizing_బహుళకం(B_FOLLOW_NONE);
}

VimScrollBar::~VimScrollBar()
{
}

    శూన్య
VimScrollBar::విలువ_మారింది_2(దశాంశం new_విలువ)
{
    ఒకవేళ (ignore_విలువ >= 0.0 && new_విలువ == ignore_విలువ) {
	ignore_విలువ = -1;
	తిరిగి;
    }
    ignore_విలువ = -1;
    /*
     * We want to throttle the amount of scroll messages generated.
     * Normally I presume you won't get a new message before we've
     * handled the previous one, but because we're passing them on this
     * happens very quickly. So instead we keep a counter of how many
     * scroll events there are (or will be) in the VDCMP, and the
     * throttling happens at the receiving end.
     */
    atomic_కూడు(&scroll_Event_లెక్క, 1);

    నిర్మాణం Vim_Scroll_Bar_సందేశం sm;

    sm.sb = ఇది;
    sm.విలువ = (దీర్ఘ) new_విలువ;
    sm.stillDragging = TRUE;

    write_పోర్టు(gui.vdcmp, Vim_సందేశం::ScrollBar, &sm, పరిమాణం(sm));

    //	calls gui_drag_scrollbar(sb, newValue, TRUE);
}

/*
 * When the mouse goes up, report that scrolling has stopped.
 * MouseUp() is NOT called when the mouse-up occurs outside
 * the window, even though the thumb does move while the mouse
 * is outside... This has some funny effects... XXX
 * So we do special processing when the window de/activates.
 */
    శూన్య
VimScrollBar::Mouse_పైకి(BPoint where)
{
    // BMessage *m = Window()->CurrentMessage();
    // m->PrintToStream();

    atomic_కూడు(&scroll_Event_లెక్క, 1);

    నిర్మాణం Vim_Scroll_Bar_సందేశం sm;

    sm.sb = ఇది;
    sm.విలువ = (దీర్ఘ) విలువ_2();
    sm.stillDragging = FALSE;

    write_పోర్టు(gui.vdcmp, Vim_సందేశం::ScrollBar, &sm, పరిమాణం(sm));

    //	calls gui_drag_scrollbar(sb, newValue, FALSE);

    Inherited::Mouse_పైకి(where);
}

    శూన్య
VimScrollBar::సమితి_విలువ(దశాంశం new_విలువ)
{
    ఒకవేళ (new_విలువ == విలువ_2())
	తిరిగి;

    ignore_విలువ = new_విలువ;
    Inherited::సమితి_విలువ(new_విలువ);
}

// ---------------- VimFont ----------------

VimFont::VimFont(): BFont()
{
    init();
}

VimFont::VimFont(స్థిరరాశి VimFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(స్థిరరాశి BFont *rhs): BFont(rhs)
{
    init();
}

VimFont::VimFont(స్థిరరాశి VimFont &rhs): BFont(rhs)
{
    init();
}

VimFont::~VimFont()
{
}

    శూన్య
VimFont::init()
{
    తదుపరి = ఖాళీ;
    refcount = 1;
    name = ఖాళీ;
}

// ---------------- VimDialog ----------------

#ఒకవేళ defined(FEAT_GUI_DIALOG)

స్థిరరాశి గుర్తులేని పూర్ణాంకం  k_Vim_Dialog_Button_సందేశం = 'VMDB';
స్థిరరాశి గుర్తులేని పూర్ణాంకం  k_Vim_Dialog_Icon_Stripe_వెడల్పు = 30;
స్థిరరాశి గుర్తులేని పూర్ణాంకం  kVimDialogButtonsSpacingX = 9;
స్థిరరాశి గుర్తులేని పూర్ణాంకం  kVimDialogButtonsSpacingY = 4;
స్థిరరాశి గుర్తులేని పూర్ణాంకం  kVimDialogSpacingX = 6;
స్థిరరాశి గుర్తులేని పూర్ణాంకం  kVimDialogSpacingY = 10;
స్థిరరాశి గుర్తులేని పూర్ణాంకం  k_Vim_Dialog_కనీస_వెడల్పు  = 310;
స్థిరరాశి గుర్తులేని పూర్ణాంకం  k_Vim_Dialog_కనీస_ఎత్తు = 75;
స్థిరరాశి BRect	    kDefaultRect =
BRect(0, 0, k_Vim_Dialog_కనీస_వెడల్పు, k_Vim_Dialog_కనీస_ఎత్తు);

VimDialog::VimDialog(పూర్ణాంకం type, స్థిరరాశి అక్షరం *title, స్థిరరాశి అక్షరం *సందేశం_2,
	స్థిరరాశి అక్షరం *buttons, పూర్ణాంకం dfltbutton, స్థిరరాశి అక్షరం *textfield, పూర్ణాంకం ex_cmd)
: BWindow(kDefaultRect, title, B_TITLED_కిటికీ_LOOK, B_MODAL_APP_కిటికీ_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , fDialogSem(-1)
    , f_Dialog_విలువ(dfltbutton)
    , f_సందేశం_View(ఖాళీ)
    , f_ఆదానం_Control(ఖాళీ)
    , f_ఆదానం_విలువ(textfield)
{
    //	master view
    VimDialog::View* view = కొత్త VimDialog::View(Bounds());
    ఒకవేళ (view == ఖాళీ)
	తిరిగి;

    ఒకవేళ (title == ఖాళీ)
	సమితి_Title("Vim " VIM_సంస్కరణ_MEDIUM);

    కూడు_బిడ్డ(view);

    //	icon
    view->InitIcon(type);

    //	buttons
    int32 which = 1;
    దశాంశం max_Button_వెడల్పు  = 0;
    దశాంశం max_Button_ఎత్తు = 0;
    దశాంశం buttons_వెడల్పు	  = 0;
    దశాంశం buttons_ఎత్తు   = 0;
    BString strButtons(buttons);
    strButtons.RemoveAll("&");
    చేయి
    {
	int32 చివర = strButtons.కనుగొను_మొదటి('\n');
	ఒకవేళ (చివర != B_ERROR)
	    strButtons.సమితి_Byte_At(చివర, '\0');

	BButton *button = _CreateButton(which++, strButtons.String());
	view->కూడు_బిడ్డ(button);
	fButtonsList.కూడు_అంశం(button);

	max_Button_వెడల్పు	= max_c(max_Button_వెడల్పు,  button->Bounds().వెడల్పు_2());
	max_Button_ఎత్తు = max_c(max_Button_ఎత్తు, button->Bounds().ఎత్తు_2());
	buttons_వెడల్పు   += button->Bounds().వెడల్పు_2();
	buttons_ఎత్తు  += button->Bounds().ఎత్తు_2();

	ఒకవేళ (చివర == B_ERROR)
	    ఆపు;

	strButtons.Remove(0, చివర + 1);
    } అయితే (నిజం);

    int32 buttons_లెక్క = fButtonsList.లెక్క_వస్తువులు();
    buttons_వెడల్పు      += kVimDialogButtonsSpacingX * (buttons_లెక్క - 1);
    buttons_ఎత్తు     += kVimDialogButtonsSpacingY * (buttons_లెక్క - 1);
    దశాంశం dialog_వెడల్పు  = buttons_వెడల్పు + k_Vim_Dialog_Icon_Stripe_వెడల్పు +
	kVimDialogSpacingX * 2;
    దశాంశం dialog_ఎత్తు = max_Button_ఎత్తు + kVimDialogSpacingY * 3;

    // Check 'v' flag in 'guioptions': vertical button placement.
    తార్కిక vertical = (vim_strchr(p_go, GO_VERTICAL) != ఖాళీ) ||
	dialog_వెడల్పు >= gui.vim_కిటికీ->Bounds().వెడల్పు_2();
    ఒకవేళ (vertical) {
	dialog_వెడల్పు  -= buttons_వెడల్పు;
	dialog_వెడల్పు  += max_Button_వెడల్పు;
	dialog_ఎత్తు -= max_Button_ఎత్తు;
	dialog_ఎత్తు += buttons_ఎత్తు;
    }

    dialog_వెడల్పు  = max_c(dialog_వెడల్పు,  k_Vim_Dialog_కనీస_వెడల్పు);

    //	message view
    BRect rect(0, 0, dialog_వెడల్పు, 0);
    rect.ఎడమ  += k_Vim_Dialog_Icon_Stripe_వెడల్పు + 16 + kVimDialogSpacingX;
    rect.శిఖరం   += kVimDialogSpacingY;
    rect.కుడి -= kVimDialogSpacingX;
    rect.bottom = rect.శిఖరం;
    f_సందేశం_View = కొత్త BTextView(rect, "_tv_", rect.స్థానభ్రంశం_By_Copy(B_ORIGIN),
	    B_FOLLOW_ఎడమ | B_FOLLOW_శిఖరం, B_WILL_DRAW);

    f_సందేశం_View->సమితి_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    rgb_color పాఠ్యం_Color = ui_color(B_PANEL_పాఠ్యం_COLOR);
    f_సందేశం_View->సమితి_Font_And_Color(be_plain_font, B_FONT_ALL, &పాఠ్యం_Color);
    f_సందేశం_View->సమితి_పాఠ్యం(సందేశం_2);
    f_సందేశం_View->MakeEditable(అబద్ధం);
    f_సందేశం_View->MakeSelectable(అబద్ధం);
    f_సందేశం_View->సమితి_మాట_Wrap(నిజం);
    కూడు_బిడ్డ(f_సందేశం_View);

    దశాంశం సందేశం_ఎత్తు = f_సందేశం_View->పాఠ్యం_ఎత్తు(0, f_సందేశం_View->లెక్క_పంక్తులు());
    f_సందేశం_View->ResizeBy(0, సందేశం_ఎత్తు);
    f_సందేశం_View->సమితి_పాఠ్యం_Rect(BRect(0, 0, rect.వెడల్పు_2(), సందేశం_ఎత్తు));

    dialog_ఎత్తు += సందేశం_ఎత్తు;

    //	input view
    ఒకవేళ (f_ఆదానం_విలువ != ఖాళీ) {
	rect.శిఖరం     =
	    rect.bottom += సందేశం_ఎత్తు + kVimDialogSpacingY;
	f_ఆదానం_Control = కొత్త BTextControl(rect, "_iv_", ఖాళీ, f_ఆదానం_విలువ, ఖాళీ,
		B_FOLLOW_ఎడమ | B_FOLLOW_శిఖరం, B_WILL_DRAW | B_NAVIGABLE |  B_PULSE_NEEDED);
	f_ఆదానం_Control->పాఠ్యం_View()->సమితి_పాఠ్యం(f_ఆదానం_విలువ);
	f_ఆదానం_Control->పాఠ్యం_View()->సమితి_మాట_Wrap(అబద్ధం);
	కూడు_బిడ్డ(f_ఆదానం_Control);

	దశాంశం వెడల్పు = 0.f, ఎత్తు = 0.f;
	f_ఆదానం_Control->Get_Preferred_కొలత(&వెడల్పు, &ఎత్తు);
	f_ఆదానం_Control->MakeFocus(నిజం);

	dialog_ఎత్తు += ఎత్తు + kVimDialogSpacingY * 1.5;
    }

    dialog_ఎత్తు = max_c(dialog_ఎత్తు, k_Vim_Dialog_కనీస_ఎత్తు);

    Resize_గమ్యం(dialog_వెడల్పు, dialog_ఎత్తు);
    Move_గమ్యం((gui.vim_కిటికీ->Bounds().వెడల్పు_2() - dialog_వెడల్పు) / 2,
	    (gui.vim_కిటికీ->Bounds().ఎత్తు_2() - dialog_ఎత్తు) / 2);

    //	adjust layout of buttons
    దశాంశం button_వెడల్పు = max_c(max_Button_వెడల్పు, rect.వెడల్పు_2() * 0.66);
    BPoint origin(dialog_వెడల్పు, dialog_ఎత్తు);
    origin.x -= kVimDialogSpacingX + (vertical ? button_వెడల్పు : buttons_వెడల్పు);
    origin.y -= kVimDialogSpacingY + (vertical ? buttons_ఎత్తు	: max_Button_ఎత్తు);

    కొరకు (int32 i = 0 ; i < buttons_లెక్క; i++) {
	BButton *button = (BButton*)fButtonsList.అంశం_At(i);
	button->Move_గమ్యం(origin);
	ఒకవేళ (vertical) {
	    origin.y += button->చట్రం_2().ఎత్తు_2() + kVimDialogButtonsSpacingY;
	    button->Resize_గమ్యం(button_వెడల్పు, button->చట్రం_2().ఎత్తు_2());
	} లేకపోతే
	    origin.x += button->చట్రం_2().వెడల్పు_2() + kVimDialogButtonsSpacingX;

	ఒకవేళ (dfltbutton == i + 1) {
	    button->MakeDefault(నిజం);
	    button->MakeFocus(f_ఆదానం_Control == ఖాళీ);
	}
    }
}

VimDialog::~VimDialog()
{
    ఒకవేళ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    పూర్ణాంకం
VimDialog::Go()
{
    fDialogSem = create_sem(0, "VimDialogSem");
    ఒకవేళ (fDialogSem < B_OK) {
	Quit();
	తిరిగి f_Dialog_విలువ;
    }

    Show();

    అయితే (acquire_sem(fDialogSem) == B_INTERRUPTED);

    పూర్ణాంకం ret_విలువ = f_Dialog_విలువ;
    ఒకవేళ (f_ఆదానం_విలువ != ఖాళీ)
	vim_strncpy((char_u*)f_ఆదానం_విలువ, (char_u*)f_ఆదానం_Control->పాఠ్యం_2(), IOSIZE - 1);

    ఒకవేళ (తాళం_2())
	Quit();

    తిరిగి ret_విలువ;
}

శూన్య VimDialog::సందేశం_Received(BMessage *సందేశం)
{
    int32 which = 0;
    ఒకవేళ (సందేశం->what != k_Vim_Dialog_Button_సందేశం ||
	    సందేశం->కనుగొను_Int32("which", &which) != B_OK)
	తిరిగి BWindow::సందేశం_Received(సందేశం);

    f_Dialog_విలువ = which;
    delete_sem(fDialogSem);
    fDialogSem = -1;
}

BButton* VimDialog::_CreateButton(int32 which, స్థిరరాశి అక్షరం* label)
{
    BMessage *సందేశం_2 = కొత్త BMessage(k_Vim_Dialog_Button_సందేశం);
    సందేశం_2->కూడు_Int32("which", which);

    BRect rect(0, 0, 0, 0);
    BString name;
    name << "_b" << which << "_";

    BButton* button = కొత్త BButton(rect, name.String(), label, సందేశం_2,
	    B_FOLLOW_కుడి | B_FOLLOW_BOTTOM);

    దశాంశం వెడల్పు = 0.f, ఎత్తు = 0.f;
    button->Get_Preferred_కొలత(&వెడల్పు, &ఎత్తు);
    button->Resize_గమ్యం(వెడల్పు, ఎత్తు);

    తిరిగి button;
}

VimDialog::View::View(BRect చట్రం)
    :	BView(చట్రం, "VimDialogView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
    fIconBitmap(ఖాళీ)
{
    సమితి_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

VimDialog::View::~View()
{
    తొలగించు fIconBitmap;
}

శూన్య VimDialog::View::Draw(BRect updateRect)
{
    BRect stripeRect = Bounds();
    stripeRect.కుడి = k_Vim_Dialog_Icon_Stripe_వెడల్పు;
    సమితి_ఎగువ_Color(tint_color(ViewColor(), B_DARKEN_1_TINT));
    FillRect(stripeRect);

    ఒకవేళ (fIconBitmap == ఖాళీ)
	తిరిగి;

    సమితి_Drawing_బహుళకం(B_OP_ALPHA);
    సమితి_Blending_బహుళకం(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    DrawBitmapAsync(fIconBitmap, BPoint(18, 6));
}

శూన్య VimDialog::View::InitIcon(int32 type)
{
    ఒకవేళ (type == VIM_GENERIC)
	తిరిగి;

    BPath మార్గం;
    స్థితి_t స్థితి_2 = కనుగొను_directory(B_BEOS_SERVERS_DIRECTORY, &మార్గం);
    ఒకవేళ (స్థితి_2 != B_OK) {
	fprintf(stderr, "Cannot retrieve app info:%s\n", strerror(స్థితి_2));
	తిరిగి;
    }

    మార్గం.Append("app_server");

    BFile file(మార్గం.మార్గం_2(), O_RDONLY);
    ఒకవేళ (file.InitCheck() != B_OK) {
	fprintf(stderr, "App file assignment failed:%s\n",
		strerror(file.InitCheck()));
	తిరిగి;
    }

    BResources resources(&file);
    ఒకవేళ (resources.InitCheck() != B_OK) {
	fprintf(stderr, "App server resources assignment failed:%s\n",
		strerror(resources.InitCheck()));
	తిరిగి;
    }

    స్థిరరాశి అక్షరం *name = "";
    ఎంపిక(type) {
	స్థితి VIM_ERROR:	    name = "stop"; ఆపు;
	స్థితి VIM_WARNING:   name = "warn"; ఆపు;
	స్థితి VIM_INFO:	    name = "info"; ఆపు;
	స్థితి VIM_QUESTION:  name = "idea"; ఆపు;
	అప్రమేయ: తిరిగి;
    }

    int32 icon_కొలత_2 = 32;
    fIconBitmap = కొత్త BBitmap(BRect(0, 0, icon_కొలత_2 - 1, icon_కొలత_2 - 1), 0, B_RGBA32);
    ఒకవేళ (fIconBitmap == ఖాళీ || fIconBitmap->InitCheck() != B_OK) {
	fprintf(stderr, "Icon bitmap allocation failed:%s\n",
		(fIconBitmap == ఖాళీ) ? "null" : strerror(fIconBitmap->InitCheck()));
	తిరిగి;
    }

    size_t కొలత = 0;
    స్థిరరాశి uint8* icon_దత్తాంశం = ఖాళీ;
    //	try vector icon first?
    icon_దత్తాంశం = (స్థిరరాశి uint8*)resources.ఎక్కించు_Resource(B_సదిశ_ICON_TYPE, name, &కొలత);
    ఒకవేళ (icon_దత్తాంశం != ఖాళీ && BIconUtils::Get_సదిశ_Icon(icon_దత్తాంశం, కొలత, fIconBitmap) == B_OK)
	తిరిగి;

    //	try bitmap icon now
    icon_దత్తాంశం = (స్థిరరాశి uint8*)resources.ఎక్కించు_Resource(B_LARGE_ICON_TYPE, name, &కొలత);
    ఒకవేళ (icon_దత్తాంశం == ఖాళీ) {
	fprintf(stderr, "Bitmap icon resource not found\n");
	తొలగించు fIconBitmap;
	fIconBitmap = ఖాళీ;
	తిరిగి;
    }

    ఒకవేళ (fIconBitmap->ColorSpace() != B_CMAP8)
	BIconUtils::ConvertFromCMAP8(icon_దత్తాంశం, icon_కొలత_2, icon_కొలత_2, icon_కొలత_2, fIconBitmap);
}

స్థిరరాశి గుర్తులేని పూర్ణాంకం  k_Vim_Dialog_OKButton_సందేశం = 'FDOK';
స్థిరరాశి గుర్తులేని పూర్ణాంకం  k_Vim_Dialog_Cancel_Button_సందేశం = 'FDCN';
స్థిరరాశి గుర్తులేని పూర్ణాంకం  k_Vim_Dialog_కొలత_ఆదానం_సందేశం = 'SICH';
స్థిరరాశి గుర్తులేని పూర్ణాంకం  k_Vim_Dialog_Family_Select_సందేశం = 'MSFM';
స్థిరరాశి గుర్తులేని పూర్ణాంకం  k_Vim_Dialog_Style_Select_సందేశం = 'MSST';
స్థిరరాశి గుర్తులేని పూర్ణాంకం  k_Vim_Dialog_కొలత_Select_సందేశం = 'MSSZ';

VimSelectFontDialog::VimSelectFontDialog(font_family* family, font_style* style, దశాంశం* కొలత)
: BWindow(kDefaultRect, "Font Selection", B_TITLED_కిటికీ_LOOK, B_MODAL_APP_కిటికీ_FEEL,
	B_NOT_CLOSABLE | B_NOT_RESIZABLE |
	B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_ASYNCHRONOUS_CONTROLS)
    , f_స్థితి(B_NO_INIT)
    , fDialogSem(-1)
    , f_Dialog_విలువ(అబద్ధం)
    , fFamily(family)
    , fStyle(style)
    , f_కొలత(కొలత)
    , f_Font_కొలత(*కొలత)
    , fPreview(0)
    , fFamiliesList(0)
    , fStylesList(0)
    , fSizesList(0)
    , f_Sizes_ఆదానం(0)
{
    strncpy(fFontFamily, *family, B_FONT_FAMILY_నిడివి);
    strncpy(fFontStyle, *style, B_FONT_STYLE_నిడివి);

    //	"client" area view
    BBox *clientBox = కొత్త BBox(Bounds(), B_ఖాళీలు_STRING, B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_చట్రం_EVENTS | B_NAVIGABLE_JUMP | B_PULSE_NEEDED,
		    B_PLAIN_BORDER);
    కూడు_బిడ్డ(clientBox);

    //	client view
    BRect RC = clientBox->Bounds();
    RC.InsetBy(kVimDialogSpacingX, kVimDialogSpacingY);
    BRect rc(RC.ఎడమ_శిఖరం(), RC.ఎడమ_శిఖరం());

    //	at first create all controls
    fPreview = కొత్త BStringView(rc, "preview", "DejaVu Sans Mono");
    clientBox->కూడు_బిడ్డ(fPreview);

    BBox* boxDivider = కొత్త BBox(rc, B_ఖాళీలు_STRING,
	    B_FOLLOW_NONE, B_WILL_DRAW, B_FANCY_BORDER);
    clientBox->కూడు_బిడ్డ(boxDivider);

    BStringView *labelFamily = కొత్త BStringView(rc, "labelFamily", "Family:");
    clientBox->కూడు_బిడ్డ(labelFamily);
    labelFamily->Resize_గమ్యం_Preferred();

    BStringView *labelStyle = కొత్త BStringView(rc, "labelStyle", "Style:");
    clientBox->కూడు_బిడ్డ(labelStyle);
    labelStyle->Resize_గమ్యం_Preferred();

    BStringView *label_కొలత = కొత్త BStringView(rc, "labelSize", "Size:");
    clientBox->కూడు_బిడ్డ(label_కొలత);
    label_కొలత->Resize_గమ్యం_Preferred();

    fFamiliesList = కొత్త BListView(rc, "listFamily",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollFamilies = కొత్త BScrollView("scrollFamily",
	    fFamiliesList, B_FOLLOW_ఎడమ_కుడి, 0, అబద్ధం, నిజం);
    clientBox->కూడు_బిడ్డ(scrollFamilies);

    fStylesList= కొత్త BListView(rc, "listStyles",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollStyles = కొత్త BScrollView("scrollStyle",
	    fStylesList, B_FOLLOW_ఎడమ_కుడి, 0, అబద్ధం, నిజం);
    clientBox->కూడు_బిడ్డ(scrollStyles);

    f_Sizes_ఆదానం = కొత్త BTextControl(rc, "inputSize", ఖాళీ, "???",
	    కొత్త BMessage(k_Vim_Dialog_కొలత_ఆదానం_సందేశం));
    clientBox->కూడు_బిడ్డ(f_Sizes_ఆదానం);
    f_Sizes_ఆదానం->Resize_గమ్యం_Preferred();

    fSizesList = కొత్త BListView(rc, "listSizes",
	    B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
    BScrollView *scrollSizes = కొత్త BScrollView("scrollSize",
	    fSizesList, B_FOLLOW_ఎడమ_కుడి, 0, అబద్ధం, నిజం);
    clientBox->కూడు_బిడ్డ(scrollSizes);

    BButton *buttonOK = కొత్త BButton(rc, "buttonOK", "OK",
			కొత్త BMessage(k_Vim_Dialog_OKButton_సందేశం));
    clientBox->కూడు_బిడ్డ(buttonOK);
    buttonOK->Resize_గమ్యం_Preferred();

    BButton *buttonCancel = కొత్త BButton(rc, "buttonCancel", "Cancel",
			కొత్త BMessage(k_Vim_Dialog_Cancel_Button_సందేశం));
    clientBox->కూడు_బిడ్డ(buttonCancel);
    buttonCancel->Resize_గమ్యం_Preferred();

    //	layout controls
    దశాంశం పంక్తి_ఎత్తు = labelFamily->Bounds().ఎత్తు_2();
    దశాంశం preview_ఎత్తు = పంక్తి_ఎత్తు * 3;
    దశాంశం స్థానభ్రంశం_YLabels = preview_ఎత్తు + kVimDialogSpacingY;
    దశాంశం స్థానభ్రంశం_YLists = స్థానభ్రంశం_YLabels + పంక్తి_ఎత్తు + kVimDialogSpacingY / 2;
    దశాంశం స్థానభ్రంశం_YSizes = స్థానభ్రంశం_YLists + f_Sizes_ఆదానం->Bounds().ఎత్తు_2() + kVimDialogSpacingY / 2;
    దశాంశం lists_ఎత్తు = పంక్తి_ఎత్తు * 9;
    దశాంశం స్థానభ్రంశం_YButtons = స్థానభ్రంశం_YLists + lists_ఎత్తు +  kVimDialogSpacingY;
    దశాంశం max_Controls_ఎత్తు = స్థానభ్రంశం_YButtons + buttonOK->Bounds().ఎత్తు_2();
    దశాంశం families_వెడల్పు = labelFamily->Bounds().వెడల్పు_2() * 5;
    దశాంశం స్థానభ్రంశం_XStyles = families_వెడల్పు + kVimDialogSpacingX;
    దశాంశం styles_వెడల్పు = labelStyle->Bounds().వెడల్పు_2() * 4;
    దశాంశం స్థానభ్రంశం_XSizes = స్థానభ్రంశం_XStyles + styles_వెడల్పు + kVimDialogSpacingX;
    దశాంశం sizes_వెడల్పు = label_కొలత->Bounds().వెడల్పు_2() * 2;
    దశాంశం max_Controls_వెడల్పు = స్థానభ్రంశం_XSizes + sizes_వెడల్పు;

    Resize_గమ్యం(max_Controls_వెడల్పు + kVimDialogSpacingX * 2,
	max_Controls_ఎత్తు + kVimDialogSpacingY * 2);

    BRect rcVim = gui.vim_కిటికీ->చట్రం_2();
    Move_గమ్యం(rcVim.ఎడమ + (rcVim.వెడల్పు_2() - చట్రం_2().వెడల్పు_2()) / 2,
	    rcVim.శిఖరం + (rcVim.ఎత్తు_2() - చట్రం_2().ఎత్తు_2()) / 2);

    fPreview->Resize_గమ్యం(max_Controls_వెడల్పు, preview_ఎత్తు);
    fPreview->సమితి_Alignment(B_ALIGN_CENTER);

    boxDivider->MoveBy(0.f, preview_ఎత్తు + kVimDialogSpacingY / 2);
    boxDivider->Resize_గమ్యం(max_Controls_వెడల్పు, 1.f);

    labelFamily->MoveBy(0.f, స్థానభ్రంశం_YLabels);
    labelStyle->MoveBy(స్థానభ్రంశం_XStyles, స్థానభ్రంశం_YLabels);
    label_కొలత->MoveBy(స్థానభ్రంశం_XSizes, స్థానభ్రంశం_YLabels);

    //	text control alignment issues
    దశాంశం insetX = f_Sizes_ఆదానం->పాఠ్యం_View()->Bounds().వెడల్పు_2() - f_Sizes_ఆదానం->Bounds().వెడల్పు_2();
    దశాంశం insetY = f_Sizes_ఆదానం->పాఠ్యం_View()->Bounds().వెడల్పు_2() - f_Sizes_ఆదానం->Bounds().వెడల్పు_2();

    scrollFamilies->MoveBy(0.f, స్థానభ్రంశం_YLists);
    scrollStyles->MoveBy(స్థానభ్రంశం_XStyles, స్థానభ్రంశం_YLists);
    f_Sizes_ఆదానం->MoveBy(స్థానభ్రంశం_XSizes + insetX / 2, స్థానభ్రంశం_YLists + insetY / 2);
    scrollSizes->MoveBy(స్థానభ్రంశం_XSizes, స్థానభ్రంశం_YSizes);

    f_Sizes_ఆదానం->సమితి_Alignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

    scrollFamilies->Resize_గమ్యం(families_వెడల్పు, lists_ఎత్తు);
    scrollStyles->Resize_గమ్యం(styles_వెడల్పు, lists_ఎత్తు);
    f_Sizes_ఆదానం->Resize_గమ్యం(sizes_వెడల్పు, f_Sizes_ఆదానం->Bounds().ఎత్తు_2());
    scrollSizes->Resize_గమ్యం(sizes_వెడల్పు,
	    lists_ఎత్తు - (స్థానభ్రంశం_YSizes - స్థానభ్రంశం_YLists));

    buttonOK->MoveBy(max_Controls_వెడల్పు - buttonOK->Bounds().వెడల్పు_2(), స్థానభ్రంశం_YButtons);
    buttonCancel->MoveBy(max_Controls_వెడల్పు - buttonOK->Bounds().వెడల్పు_2()
	    - buttonCancel->Bounds().వెడల్పు_2() - kVimDialogSpacingX, స్థానభ్రంశం_YButtons);

    //	fill lists
    పూర్ణాంకం sel_సూచీ = -1;
    పూర్ణాంకం లెక్క = లెక్క_font_families();
    కొరకు (పూర్ణాంకం i = 0; i < లెక్క; i++) {
	font_family family;
	ఒకవేళ (get_font_family(i, &family ) == B_OK) {
	    fFamiliesList->కూడు_అంశం(కొత్త BString_అంశం((స్థిరరాశి అక్షరం*)family));
	    ఒకవేళ (అక్షరశ్రేణి_పోలిక(family, fFontFamily, B_FONT_FAMILY_నిడివి) == 0)
		sel_సూచీ = i;
	}
    }

    ఒకవేళ (sel_సూచీ >= 0) {
	fFamiliesList->Select(sel_సూచీ);
	fFamiliesList->Scroll_గమ్యం_Selection();
    }

    _UpdateFontStyles();

    sel_సూచీ = -1;
    కొరకు (పూర్ణాంకం కొలత = 8, సూచీ = 0; కొలత <= 18; కొలత++, సూచీ++) {
	BString str;
	str << కొలత;
	fSizesList->కూడు_అంశం(కొత్త BString_అంశం(str));
	ఒకవేళ (కొలత == f_Font_కొలత)
	    sel_సూచీ = సూచీ;

    }

    ఒకవేళ (sel_సూచీ >= 0) {
	fSizesList->Select(sel_సూచీ);
	fSizesList->Scroll_గమ్యం_Selection();
    }

    fFamiliesList->సమితి_Selection_సందేశం(కొత్త BMessage(k_Vim_Dialog_Family_Select_సందేశం));
    fStylesList->సమితి_Selection_సందేశం(కొత్త BMessage(k_Vim_Dialog_Style_Select_సందేశం));
    fSizesList->సమితి_Selection_సందేశం(కొత్త BMessage(k_Vim_Dialog_కొలత_Select_సందేశం));
    f_Sizes_ఆదానం->సమితి_Modification_సందేశం(కొత్త BMessage(k_Vim_Dialog_కొలత_ఆదానం_సందేశం));

    Update_కొలత_ఆదానం_Preview();
    _UpdateFontPreview();

    f_స్థితి = B_OK;
}

VimSelectFontDialog::~VimSelectFontDialog()
{
    _CleanList(fFamiliesList);
    _CleanList(fStylesList);
    _CleanList(fSizesList);

    ఒకవేళ (fDialogSem > B_OK)
	delete_sem(fDialogSem);
}

    శూన్య
VimSelectFontDialog::_CleanList(BListView* list)
{
    అయితే (0 < list->లెక్క_వస్తువులు())
	తొలగించు (dynamic_cast<BString_అంశం*>(list->Remove_అంశం((int32)0)));
}

    తార్కిక
VimSelectFontDialog::Go()
{
    ఒకవేళ (f_స్థితి != B_OK) {
	Quit();
	తిరిగి NOFONT;
    }

    fDialogSem = create_sem(0, "VimFontSelectDialogSem");
    ఒకవేళ (fDialogSem < B_OK) {
	Quit();
	తిరిగి f_Dialog_విలువ;
    }

    Show();

    అయితే (acquire_sem(fDialogSem) == B_INTERRUPTED);

    తార్కిక ret_విలువ = f_Dialog_విలువ;

    ఒకవేళ (తాళం_2())
	Quit();

    తిరిగి ret_విలువ;
}


శూన్య VimSelectFontDialog::_UpdateFontStyles()
{
    _CleanList(fStylesList);

    int32 sel_సూచీ = -1;
    int32 లెక్క = లెక్క_font_styles(fFontFamily);
    కొరకు (int32 i = 0; i < లెక్క; i++) {
	font_style style;
	uint32 flags = 0;
	ఒకవేళ (get_font_style(fFontFamily, i, &style, &flags) == B_OK) {
	    fStylesList->కూడు_అంశం(కొత్త BString_అంశం((స్థిరరాశి అక్షరం*)style));
	    ఒకవేళ (అక్షరశ్రేణి_పోలిక(style, fFontStyle, B_FONT_STYLE_నిడివి) == 0)
		sel_సూచీ = i;
	}
    }

    ఒకవేళ (sel_సూచీ >= 0) {
	fStylesList->Select(sel_సూచీ);
	fStylesList->Scroll_గమ్యం_Selection();
    } లేకపోతే
	fStylesList->Select(0);
}


శూన్య VimSelectFontDialog::Update_కొలత_ఆదానం_Preview()
{
    అక్షరం buf[10] = {0};
    vim_snprintf(buf, పరిమాణం(buf), (అక్షరం*)"%.0f", f_Font_కొలత);
    f_Sizes_ఆదానం->సమితి_పాఠ్యం(buf);
}


శూన్య VimSelectFontDialog::_UpdateFontPreview()
{
    BFont font;
    fPreview->GetFont(&font);
    font.సమితి_కొలత(f_Font_కొలత);
    font.సమితి_Family_And_Style(fFontFamily, fFontStyle);
    fPreview->సమితి_Font(&font, B_FONT_FAMILY_AND_STYLE | B_FONT_కొలత);

    BString str;
    str << fFontFamily << " " << fFontStyle << ", " << (పూర్ణాంకం)f_Font_కొలత << " pt.";
    fPreview->సమితి_పాఠ్యం(str);
}


    తార్కిక
VimSelectFontDialog::Update_From_List_అంశం(BListView* list, అక్షరం* పాఠ్యం, పూర్ణాంకం పాఠ్యం_కొలత)
{
    int32 సూచీ = list->CurrentSelection();
    ఒకవేళ (సూచీ < 0)
	తిరిగి అబద్ధం;
    BString_అంశం* అంశం = (BString_అంశం*)list->అంశం_At(సూచీ);
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి అబద్ధం;
    strncpy(పాఠ్యం, అంశం->పాఠ్యం_2(), పాఠ్యం_కొలత);
    తిరిగి నిజం;
}


శూన్య VimSelectFontDialog::సందేశం_Received(BMessage *సందేశం)
{
    ఎంపిక (సందేశం->what) {
	స్థితి k_Vim_Dialog_OKButton_సందేశం:
	    strncpy(*fFamily, fFontFamily, B_FONT_FAMILY_నిడివి);
	    strncpy(*fStyle, fFontStyle, B_FONT_STYLE_నిడివి);
	    *f_కొలత = f_Font_కొలత;
	    f_Dialog_విలువ = నిజం;
	స్థితి k_Vim_Dialog_Cancel_Button_సందేశం:
	    delete_sem(fDialogSem);
	    fDialogSem = -1;
	    తిరిగి;
	స్థితి B_కీలకం_పైకి:
	    {
		int32 కీలకం = 0;
		ఒకవేళ (సందేశం->కనుగొను_Int32("raw_char", &కీలకం) == B_OK
			&& కీలకం == B_ESCAPE) {
		    delete_sem(fDialogSem);
		    fDialogSem = -1;
		}
	    }
	    ఆపు;

	స్థితి k_Vim_Dialog_Family_Select_సందేశం:
	    ఒకవేళ (Update_From_List_అంశం(fFamiliesList,
		    fFontFamily, B_FONT_FAMILY_నిడివి)) {
		_UpdateFontStyles();
		_UpdateFontPreview();
	    }
	    ఆపు;
	స్థితి k_Vim_Dialog_Style_Select_సందేశం:
	    ఒకవేళ (Update_From_List_అంశం(fStylesList,
		    fFontStyle, B_FONT_STYLE_నిడివి))
		_UpdateFontPreview();
	    ఆపు;
	స్థితి k_Vim_Dialog_కొలత_Select_సందేశం:
	    {
		అక్షరం buf[10] = {0};
		ఒకవేళ (Update_From_List_అంశం(fSizesList, buf, పరిమాణం(buf))) {
		    దశాంశం కొలత = atof(buf);
		    ఒకవేళ (కొలత > 0.f) {
			f_Font_కొలత = కొలత;
			Update_కొలత_ఆదానం_Preview();
			_UpdateFontPreview();
		    }
		}
	    }
	    ఆపు;
	స్థితి k_Vim_Dialog_కొలత_ఆదానం_సందేశం:
	    {
		దశాంశం కొలత = atof(f_Sizes_ఆదానం->పాఠ్యం_2());
		ఒకవేళ (కొలత > 0.f) {
		    f_Font_కొలత = కొలత;
		    _UpdateFontPreview();
		}
	    }
	    ఆపు;
	అప్రమేయ:
	    ఆపు;
    }
    తిరిగి BWindow::సందేశం_Received(సందేశం);
}

#అయితే_ముగింపు // FEAT_GUI_DIALOG

#నిర్వచితమైతే FEAT_TOOLBAR

//  some forward declaration required by toolbar functions...
స్థిర BMessage * Menu_సందేశం(vimmenu_T *menu);

VimToolbar::VimToolbar(BRect చట్రం, స్థిరరాశి అక్షరం *name) :
    BBox(చట్రం, name, B_FOLLOW_ఎడమ | B_FOLLOW_శిఖరం, B_WILL_DRAW | B_చట్రం_EVENTS, B_PLAIN_BORDER)
{
}

VimToolbar::~VimToolbar()
{
    int32 లెక్క = fButtonsList.లెక్క_వస్తువులు();
    కొరకు (int32 i = 0; i < లెక్క; i++)
	తొలగించు (BPictureButton*)fButtonsList.అంశం_At(i);
    fButtonsList.Make_ఖాళీలు();

    తొలగించు సాధారణ_Buttons_Bitmap;
    తొలగించు grayedButtonsBitmap;
    సాధారణ_Buttons_Bitmap    = ఖాళీ;
    grayedButtonsBitmap  = ఖాళీ;
}

    శూన్య
VimToolbar::Attached_గమ్యం_కిటికీ()
{
    BBox::Attached_గమ్యం_కిటికీ();

    సమితి_View_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
}

    దశాంశం
VimToolbar::Toolbar_ఎత్తు() స్థిరరాశి
{
    దశాంశం కొలత = ఖాళీ == సాధారణ_Buttons_Bitmap ? 18. : సాధారణ_Buttons_Bitmap->Bounds().ఎత్తు_2();
    తిరిగి కొలత + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
}

    తార్కిక
VimToolbar::Modify_Bitmap_గమ్యం_Grayed(BBitmap *bitmap)
{
    దశాంశం ఎత్తు = bitmap->Bounds().ఎత్తు_2();
    దశాంశం వెడల్పు  = bitmap->Bounds().వెడల్పు_2();

    rgb_color *బిట్లు = (rgb_color*)bitmap->బిట్లు_2();
    int32 pixels = bitmap->బిట్లు_నిడివి() / 4;
    కొరకు (int32 i = 0; i < pixels; i++) {
	బిట్లు[i].red = బిట్లు[i].green =
	బిట్లు[i].blue = ((uint32)బిట్లు[i].red + బిట్లు[i].green + బిట్లు[i].blue) / 3;
	బిట్లు[i].alpha /= 4;
    }

    తిరిగి నిజం;
}

    తార్కిక
VimToolbar::PrepareButtonBitmaps()
{
    //	first try to load potentially customized $VIRUNTIME/bitmaps/builtin-tools.png
    సాధారణ_Buttons_Bitmap = ఎక్కించు_Vim_Bitmap("builtin-tools.png");
    ఒకవేళ (సాధారణ_Buttons_Bitmap == ఖాళీ)
	//  customized not found? dig application resources for "builtin-tools" one
	సాధారణ_Buttons_Bitmap = BTranslationUtils::GetBitmap(B_PNG_FORMAT, "builtin-tools");

    ఒకవేళ (సాధారణ_Buttons_Bitmap == ఖాళీ)
	తిరిగి అబద్ధం;

    BMessage archive;
    సాధారణ_Buttons_Bitmap->Archive(&archive);

    grayedButtonsBitmap = కొత్త BBitmap(&archive);
    ఒకవేళ (grayedButtonsBitmap == ఖాళీ)
	తిరిగి అబద్ధం;

    //	modify grayed bitmap
    Modify_Bitmap_గమ్యం_Grayed(grayedButtonsBitmap);

    తిరిగి నిజం;
}

BBitmap *VimToolbar::ఎక్కించు_Vim_Bitmap(స్థిరరాశి అక్షరం* fileName)
{
    BBitmap *bitmap = ఖాళీ;

    పూర్ణాంకం mustfree = 0;
    char_u* runtime_మార్గం = vim_getenv((char_u*)"VIMRUNTIME", &mustfree);
    ఒకవేళ (runtime_మార్గం != ఖాళీ && fileName != ఖాళీ) {
	BString str_మార్గం((అక్షరం*)runtime_మార్గం);
	str_మార్గం << "/bitmaps/" << fileName;
	bitmap = BTranslationUtils::GetBitmap(str_మార్గం.String());
    }

    ఒకవేళ (mustfree)
	vim_free(runtime_మార్గం);

    తిరిగి bitmap;
}

    తార్కిక
VimToolbar::GetPictureFromBitmap(BPicture *picture_గమ్యం, int32 సూచీ, BBitmap *bitmapFrom, తార్కిక pressed)
{
    దశాంశం కొలత = bitmapFrom->Bounds().ఎత్తు_2() + 1.;

    BView view(BRect(0, 0, కొలత, కొలత), "", 0, 0);

    కూడు_బిడ్డ(&view);
    view.BeginPicture(picture_గమ్యం);

    view.సమితి_ఎగువ_Color(ui_color(B_PANEL_BACKGROUND_COLOR));
    view.FillRect(view.Bounds());
    view.సమితి_Drawing_బహుళకం(B_OP_OVER);

    BRect ఆధారం(0, 0, కొలత - 1, కొలత - 1);
    BRect destination(ఆధారం);

    ఆధారం.స్థానభ్రంశం_By(కొలత * సూచీ, 0);
    destination.స్థానభ్రంశం_By(ButtonMargin, ButtonMargin);

    view.DrawBitmap(bitmapFrom, ఆధారం, destination);

    ఒకవేళ (pressed)	{
	rgb_color shineColor  = ui_color(B_SHINE_COLOR);
	rgb_color shadowColor = ui_color(B_SHADOW_COLOR);
	కొలత += ButtonMargin * 2 - 1;
	view.Begin_పంక్తి_Array(4);
	view.కూడు_పంక్తి(BPoint(0, 0),	 BPoint(కొలత, 0),    shadowColor);
	view.కూడు_పంక్తి(BPoint(కొలత, 0),	 BPoint(కొలత, కొలత), shineColor);
	view.కూడు_పంక్తి(BPoint(కొలత, కొలత), BPoint(0, కొలత),    shineColor);
	view.కూడు_పంక్తి(BPoint(0, కొలత),	 BPoint(0, 0),	     shadowColor);
	view.చివర_పంక్తి_Array();
    }

    view.చివర_Picture();
    Remove_బిడ్డ(&view);

    తిరిగి నిజం;
}

    తార్కిక
VimToolbar::కూడు_Button(int32 సూచీ, vimmenu_T *menu)
{
    BPictureButton *button = ఖాళీ;
    ఒకవేళ (!menu_is_separator(menu->name)) {
	దశాంశం కొలత = సాధారణ_Buttons_Bitmap ?
	    సాధారణ_Buttons_Bitmap->Bounds().ఎత్తు_2() + 1. + ButtonMargin * 2 : 18.;
	BRect చట్రం(0, 0, కొలత, కొలత);
	BPicture pictureOn;
	BPicture pictureOff;
	BPicture picture_బూడిద;

	ఒకవేళ (menu->iconfile == ఖాళీ && menu->iconidx >= 0 && సాధారణ_Buttons_Bitmap) {
	    GetPictureFromBitmap(&pictureOn,  menu->iconidx, సాధారణ_Buttons_Bitmap, నిజం);
	    GetPictureFromBitmap(&pictureOff, menu->iconidx, సాధారణ_Buttons_Bitmap, అబద్ధం);
	    GetPictureFromBitmap(&picture_బూడిద, menu->iconidx, grayedButtonsBitmap, అబద్ధం);
	} లేకపోతే {

	    char_u బఫరు[MAXPATHL] = {0};
	    BBitmap *bitmap = ఖాళీ;

	    ఒకవేళ (menu->iconfile) {
		gui_కనుగొను_iconfile(menu->iconfile, బఫరు, (అక్షరం*)"png");
		bitmap = BTranslationUtils::GetBitmap((అక్షరం*)బఫరు);
	    }

	    ఒకవేళ (bitmap == ఖాళీ && gui_కనుగొను_bitmap(menu->name, బఫరు, (అక్షరం*)"png") == OK)
		bitmap = BTranslationUtils::GetBitmap((అక్షరం*)బఫరు);

	    ఒకవేళ (bitmap == ఖాళీ)
		bitmap = కొత్త BBitmap(BRect(0, 0, కొలత, కొలత), B_RGB32);

	    GetPictureFromBitmap(&pictureOn,   0, bitmap, నిజం);
	    GetPictureFromBitmap(&pictureOff,  0, bitmap, అబద్ధం);
	    Modify_Bitmap_గమ్యం_Grayed(bitmap);
	    GetPictureFromBitmap(&picture_బూడిద, 0, bitmap, అబద్ధం);

	    తొలగించు bitmap;
	}

	button = కొత్త BPictureButton(చట్రం, (అక్షరం*)menu->name,
		    &pictureOff, &pictureOn, Menu_సందేశం(menu));

	button->సమితి_Disabled_On(&picture_బూడిద);
	button->సమితి_Disabled_Off(&picture_బూడిద);

	button->సమితి_లక్ష్యం(gui.vim_పాఠ్యం_Area);

	కూడు_బిడ్డ(button);

	menu->button = button;
    }

    తార్కిక ఫలితం = fButtonsList.కూడు_అంశం(button, సూచీ);
    InvalidateLayout();
    తిరిగి ఫలితం;
}

    తార్కిక
VimToolbar::RemoveButton(vimmenu_T *menu)
{
    ఒకవేళ (menu->button) {
	ఒకవేళ (fButtonsList.Remove_అంశం(menu->button)) {
	    తొలగించు menu->button;
	    menu->button = ఖాళీ;
	}
    }
    తిరిగి నిజం;
}

    తార్కిక
VimToolbar::బూడిద_Button(vimmenu_T *menu, పూర్ణాంకం grey)
{
    ఒకవేళ (menu->button) {
	int32 సూచీ = fButtonsList.సూచీ_యొక్క(menu->button);
	ఒకవేళ (సూచీ >= 0)
	    menu->button->సమితి_Enabled(grey ? అబద్ధం : నిజం);
    }
    తిరిగి నిజం;
}

    శూన్య
VimToolbar::InvalidateLayout()
{
    int32 స్థానభ్రంశం = ToolbarMargin;
    int32 లెక్క = fButtonsList.లెక్క_వస్తువులు();
    కొరకు (int32 i = 0; i < లెక్క; i++) {
	BPictureButton *button = (BPictureButton *)fButtonsList.అంశం_At(i);
	ఒకవేళ (button) {
	    button->Move_గమ్యం(స్థానభ్రంశం, ToolbarMargin);
	    స్థానభ్రంశం += button->Bounds().వెడల్పు_2() + ToolbarMargin;
	} లేకపోతే
	    స్థానభ్రంశం += ToolbarMargin * 3;
    }
}

#అయితే_ముగింపు /*FEAT_TOOLBAR*/

#ఒకవేళ defined(FEAT_GUI_TABLINE)

    దశాంశం
Vim_Tab_పంక్తి::Tabline_ఎత్తు() స్థిరరాశి
{
//  float size = NULL == normalButtonsBitmap ? 18. : normalButtonsBitmap->Bounds().Height();
//  return size + ToolbarMargin * 2 + ButtonMargin * 2 + 1;
    తిరిగి Tab_ఎత్తు(); //  + ToolbarMargin;
}

శూన్య
Vim_Tab_పంక్తి::Mouse_కిందికి(BPoint బిందువు)
{
    ఒకవేళ (!gui_mch_showing_tabline())
	తిరిగి;

    BMessage *m = కిటికీ()->Current_సందేశం();
    ప్రతిజ్ఞా(m);

    int32 buttons = 0;
    m->కనుగొను_Int32("buttons", &buttons);

    int32 clicks = 0;
    m->కనుగొను_Int32("clicks", &clicks);

    పూర్ణాంకం సూచీ = 0; //  0 means here - no tab found
    కొరకు (పూర్ణాంకం i = 0; i < లెక్క_Tabs(); i++) {
	ఒకవేళ (Tab_చట్రం(i).కలిగియుంది_2(బిందువు)) {
	    సూచీ = i + 1; //  indexes are 1-based
	    ఆపు;
	}
    }

    పూర్ణాంకం event = -1;

    ఒకవేళ ((buttons & B_PRIMARY_MOUSE_BUTTON) && clicks > 1)
	//  left button double click on - create new tab
	event = TABLINE_MENU_NEW;

    లేకపోతే ఒకవేళ (buttons & B_TERTIARY_MOUSE_BUTTON)
	//  middle button click - close the pointed tab
	//  or create new one in case empty space
	event = సూచీ > 0 ? TABLINE_MENU_CLOSE : TABLINE_MENU_NEW;

    లేకపోతే ఒకవేళ (buttons & B_SECONDARY_MOUSE_BUTTON) {
	//  right button click - show context menu
	BPop_పైకి_Menu* తీయి_పైకి_Menu = కొత్త BPop_పైకి_Menu("tabLineContextMenu", అబద్ధం, అబద్ధం);
	తీయి_పైకి_Menu->కూడు_అంశం(కొత్త BMenu_అంశం(_("Close tabi R"), కొత్త BMessage(TABLINE_MENU_CLOSE)));
	తీయి_పైకి_Menu->కూడు_అంశం(కొత్త BMenu_అంశం(_("New tab    T"), కొత్త BMessage(TABLINE_MENU_NEW)));
	తీయి_పైకి_Menu->కూడు_అంశం(కొత్త BMenu_అంశం(_("Open tab..."), కొత్త BMessage(TABLINE_MENU_OPEN)));

	Convert_గమ్యం_Screen(&బిందువు);
	BMenu_అంశం* అంశం = తీయి_పైకి_Menu->Go(బిందువు);
	ఒకవేళ (అంశం != ఖాళీ) {
	    event = అంశం->Command();
	}

	తొలగించు తీయి_పైకి_Menu;

    } లేకపోతే {
	//  default processing
	BTabView::Mouse_కిందికి(బిందువు);
	తిరిగి;
    }

    ఒకవేళ (event < 0)
	తిరిగి;

    Vim_Tabline_Menu_సందేశం tmm;
    tmm.సూచీ = సూచీ;
    tmm.event = event;
    write_పోర్టు(gui.vdcmp, Vim_సందేశం::TablineMenu, &tmm, పరిమాణం(tmm));
}

శూన్య
Vim_Tab_పంక్తి::VimTab::Select(BView* owner)
{
    BTab::Select(owner);

    Vim_Tab_పంక్తి *tab_పంక్తి = gui.vimForm->Tab_పంక్తి();
    ఒకవేళ (tab_పంక్తి != ఖాళీ) {

	int32 i = 0;
	కొరకు (; i < tab_పంక్తి->లెక్క_Tabs(); i++)
	    ఒకవేళ (ఇది == tab_పంక్తి->TabAt(i))
		ఆపు;

//	printf("%d:%d:%s\n", i, tabLine->CountTabs(), tabLine->TabAt(i)->Label());
	ఒకవేళ (i < tab_పంక్తి->లెక్క_Tabs()) {
	    Vim_Tabline_సందేశం tm;
	    tm.సూచీ = i + 1;
	    write_పోర్టు(gui.vdcmp, Vim_సందేశం::Tabline, &tm, పరిమాణం(tm));
	}
    }
}

#అయితే_ముగింపు //  defined(FEAT_GUI_TABLINE)

// ---------------- ----------------

//  some global variables
స్థిర అక్షరం appsig[] = "application/x-vnd.Haiku-Vim-8";
కీలకం_map *కీలకం_Map;
అక్షరం *కీలకం_Map_Chars;
పూర్ణాంకం main_exitcode = 127;

    స్థితి_t
gui_haiku_process_event(bigtime_t timeout)
{
    నిర్మాణం Vim_సందేశం vm;
    int32 what;
    ssize_t కొలత;

    కొలత = read_పోర్టు_etc(gui.vdcmp, &what, &vm, పరిమాణం(vm),
	    B_TIMEOUT, timeout);

    ఒకవేళ (కొలత >= 0) {
	ఎంపిక (what) {
	    స్థితి Vim_సందేశం::కీలకం_2:
		{
		    char_u *string = vm.u.కీలకం_2.chars;
		    పూర్ణాంకం len = vm.u.కీలకం_2.నిడివి;
		    ఒకవేళ (len == 1 && string[0] == Ctrl_chr('C')) {
			trash_ఆదానం_buf();
			got_int = TRUE;
		    }

		    ఒకవేళ (vm.u.కీలకం_2.csi_escape)
#నిర్వచితం_కాకపోతే FEAT_MBYTE_IME
		    {
			పూర్ణాంకం	i;
			char_u	buf[2];

			కొరకు (i = 0; i < len; ++i)
			{
			    కూడు_గమ్యం_ఆదానం_buf(string + i, 1);
			    ఒకవేళ (string[i] == CSI)
			    {
				// Turn CSI into K_CSI.
				buf[0] = KS_EXTRA;
				buf[1] = (పూర్ణాంకం)KE_CSI;
				కూడు_గమ్యం_ఆదానం_buf(buf, 2);
			    }
			}
		    }
#లేకపోతే
			కూడు_గమ్యం_ఆదానం_buf_csi(string, len);
#అయితే_ముగింపు
		    లేకపోతే
			కూడు_గమ్యం_ఆదానం_buf(string, len);
		}
		ఆపు;
	    స్థితి Vim_సందేశం::Resize:
		gui_resize_shell(vm.u.New_కొలత.వెడల్పు, vm.u.New_కొలత.ఎత్తు);
		ఆపు;
	    స్థితి Vim_సందేశం::ScrollBar:
		{
		    /*
		     * If loads of scroll messages queue up, use only the last
		     * one. Always report when the scrollbar stops dragging.
		     * This is not perfect yet anyway: these events are queued
		     * yet again, this time in the keyboard input buffer.
		     */
		    int32 old_లెక్క =
			atomic_కూడు(&vm.u.Scroll.sb->scroll_Event_లెక్క, -1);
		    ఒకవేళ (old_లెక్క <= 1 || !vm.u.Scroll.stillDragging)
			gui_drag_scrollbar(vm.u.Scroll.sb->getGsb(),
				vm.u.Scroll.విలువ, vm.u.Scroll.stillDragging);
		}
		ఆపు;
#ఒకవేళ defined(FEAT_MENU)
	    స్థితి Vim_సందేశం::Menu:
		gui_menu_cb(vm.u.Menu.guiMenu);
		ఆపు;
#అయితే_ముగింపు
	    స్థితి Vim_సందేశం::Mouse:
		{
		    int32 old_లెక్క;
		    ఒకవేళ (vm.u.Mouse.button == MOUSE_DRAG)
			old_లెక్క =
			    atomic_కూడు(&gui.vim_పాఠ్యం_Area->mouse_Drag_Event_లెక్క, -1);
		    లేకపోతే
			old_లెక్క = 0;
		    ఒకవేళ (old_లెక్క <= 1)
			gui_send_mouse_event(vm.u.Mouse.button, vm.u.Mouse.x,
				vm.u.Mouse.y, vm.u.Mouse.repeated_click,
				vm.u.Mouse.modifiers);
		}
		ఆపు;
	    స్థితి Vim_సందేశం::MouseMoved:
		{
		    gui_mouse_moved(vm.u.MouseMoved.x, vm.u.MouseMoved.y);
		}
		ఆపు;
	    స్థితి Vim_సందేశం::Focus:
		gui.లోపల_focus = vm.u.Focus.చురుకైన;
		// XXX Signal that scrollbar dragging has stopped?
		// This is needed because we don't get a MouseUp if
		// that happens while outside the window... :-(
		ఒకవేళ (gui.dragged_sb) {
		    gui.dragged_sb = SBAR_NONE;
		}
		//  gui_update_cursor(TRUE, FALSE);
		ఆపు;
	    స్థితి Vim_సందేశం::Refs:
		::RefsReceived(vm.u.Refs.సందేశం_2, vm.u.Refs.changedir);
		ఆపు;
	    స్థితి Vim_సందేశం::Tabline:
		send_tabline_event(vm.u.Tabline.సూచీ);
		ఆపు;
	    స్థితి Vim_సందేశం::TablineMenu:
		send_tabline_menu_event(vm.u.TablineMenu.సూచీ, vm.u.TablineMenu.event);
		ఆపు;
	    అప్రమేయ:
		//  unrecognised message, ignore it
		ఆపు;
	}
    }

    /*
     * If size < B_OK, it is an error code.
     */
    తిరిగి కొలత;
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

    పూర్ణాంకం
vim_తాళం_screen()
{
    తిరిగి !gui.vim_కిటికీ || gui.vim_కిటికీ->తాళం_2();
}

    శూన్య
vim_unlock_screen()
{
    ఒకవేళ (gui.vim_కిటికీ)
	gui.vim_కిటికీ->Unlock();
}

#నిర్వచించు నిరంతరం_BAPPLICATION_లోపల_NEW_THREAD	0

#ఒకవేళ నిరంతరం_BAPPLICATION_లోపల_NEW_THREAD

    int32
నిరంతరం_vimapp(శూన్య *వాదనలు)
{
    VimApp app(appsig);

    gui.vimApp = &app;
    app.నిరంతరం_2();		    // Run until Quit() called

    తిరిగి 0;
}

#లేకపోతే

    int32
call_main(శూన్య *వాదనలు)
{
    నిర్మాణం Main_వాదనలు *ma = (Main_వాదనలు *)వాదనలు;

    తిరిగి ప్రధాన(ma->argc, ma->argv);
}
#అయితే_ముగింపు

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    శూన్య
gui_mch_prepare(
	పూర్ణాంకం	*argc,
	అక్షరం	**argv)
{
    /*
     * We don't have any command line arguments for the BeOS GUI yet,
     * but this is an excellent place to create our Application object.
     */
    ఒకవేళ (!gui.vimApp) {
	thread_info tinfo;
	get_thread_info(కనుగొను_thread(ఖాళీ), &tinfo);

	// May need the port very early on to process RefsReceived()
	gui.vdcmp = create_పోర్టు(B_MAX_పోర్టు_లెక్క, "vim VDCMP");

#ఒకవేళ నిరంతరం_BAPPLICATION_లోపల_NEW_THREAD
	thread_id tid = spawn_thread(నిరంతరం_vimapp, "vim VimApp",
		tinfo.ప్రాధాన్యత, ఖాళీ);
	ఒకవేళ (tid >= B_OK) {
	    resume_thread(tid);
	} లేకపోతే {
	    getout(1);
	}
#లేకపోతే
	Main_వాదనలు ma = { *argc, argv };
	thread_id tid = spawn_thread(call_main, "vim main()",
		tinfo.ప్రాధాన్యత, &ma);
	ఒకవేళ (tid >= B_OK) {
	    VimApp app(appsig);

	    gui.vimApp = &app;
	    resume_thread(tid);
	    /*
	     * This is rather horrible.
	     * call_main will call main() again...
	     * There will be no infinite recursion since
	     * gui.vimApp is set now.
	     */
	    app.నిరంతరం_2();		    // Run until Quit() called
	    // fprintf(stderr, "app.Run() returned...\n");
	    స్థితి_t dummy_exitcode;
	    (శూన్య)wait_for_thread(tid, &dummy_exitcode);

	    /*
	     * This path should be the normal one taken to exit Vim.
	     * The main() thread calls mch_exit() which calls
	     * gui_mch_exit() which terminates its thread.
	     */
	    నిష్క్రమించు(main_exitcode);
	}
#అయితే_ముగింపు
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
    ఒకవేళ (!isatty(0)) {
	నిర్మాణం stat stat_stdin, stat_dev_null;

	ఒకవేళ (fstat(0, &stat_stdin) == -1 ||
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
    పూర్ణాంకం
gui_mch_init_check(శూన్య)
{
    తిరిగి OK;	    // TODO: GUI can always be started?
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    పూర్ణాంకం
gui_mch_init()
{
    display_errors();
    gui.def_norm_pixel = RGB(0x00, 0x00, 0x00);	//  black
    gui.def_వెనుక_pixel = RGB(0xFF, 0xFF, 0xFF);	//  white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.వెనుక_pixel = gui.def_వెనుక_pixel;

    gui.scrollbar_వెడల్పు = (పూర్ణాంకం) B_V_SCROLL_BAR_వెడల్పు;
    gui.scrollbar_ఎత్తు = (పూర్ణాంకం) B_H_SCROLL_BAR_ఎత్తు;
#నిర్వచితమైతే FEAT_MENU
    gui.menu_ఎత్తు = 19;   //	initial guess -
    //	correct for my default settings
#అయితే_ముగింపు
    gui.border_స్థానభ్రంశం = 3;  //	coordinates are inside window borders

    ఒకవేళ (gui.vdcmp < B_OK)
	తిరిగి FAIL;
    get_కీలకం_map(&కీలకం_Map, &కీలకం_Map_Chars);

    gui.vim_కిటికీ = కొత్త Vim_కిటికీ();	// hidden and locked
    ఒకవేళ (!gui.vim_కిటికీ)
	తిరిగి FAIL;

    gui.vim_కిటికీ->నిరంతరం_2();	// Run() unlocks but does not show

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    సమితి_సాధారణ_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();	    // re-init colors and fonts

    gui_mch_new_colors();	// window must exist for this

    తిరిగి OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    శూన్య
gui_mch_new_colors()
{
    rgb_color rgb = GUI_గమ్యం_RGB(gui.వెనుక_pixel);

    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vimForm->సమితి_View_Color(rgb);
	//  Does this not have too much effect for those small rectangles?
	gui.vimForm->Invalidate();
	gui.vim_కిటికీ->Unlock();
    }
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    పూర్ణాంకం
gui_mch_open()
{
    ఒకవేళ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_సమితి_winpos(gui_win_x, gui_win_y);

    // Actually open the window
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_కిటికీ->Show();
	gui.vim_కిటికీ->Unlock();
	తిరిగి OK;
    }

    తిరిగి FAIL;
}

    శూన్య
gui_mch_exit(పూర్ణాంకం vim_exitcode)
{
    ఒకవేళ (gui.vim_కిటికీ) {
	thread_id tid = gui.vim_కిటికీ->Thread();
	gui.vim_కిటికీ->తాళం_2();
	gui.vim_కిటికీ->Quit();
	// Wait until it is truly gone
	int32 exitcode;
	wait_for_thread(tid, &exitcode);
    }
    delete_పోర్టు(gui.vdcmp);
#ఒకవేళ !నిరంతరం_BAPPLICATION_లోపల_NEW_THREAD
    /*
     * We are in the main() thread - quit the App thread and
     * quit ourselves (passing on the exitcode). Use a global since the
     * value from exit_thread() is only used if wait_for_thread() is
     * called in time (race condition).
     */
#అయితే_ముగింపు
    ఒకవేళ (gui.vimApp) {
	Vim_పాఠ్యం_Area_View::guiBlankMouse(అబద్ధం);

	main_exitcode = vim_exitcode;
#ఒకవేళ నిరంతరం_BAPPLICATION_లోపల_NEW_THREAD
	thread_id tid = gui.vimApp->Thread();
	int32 exitcode;
	gui.vimApp->తాళం_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	wait_for_thread(tid, &exitcode);
#లేకపోతే
	gui.vimApp->తాళం_2();
	gui.vimApp->Quit();
	gui.vimApp->Unlock();
	// suicide
	exit_thread(vim_exitcode);
#అయితే_ముగింపు
    }
    // If we are somehow still here, let mch_exit() handle things.
}

/*
 * Get the position of the top left corner of the window.
 */
    పూర్ణాంకం
gui_mch_get_winpos(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	BRect r;
	r = gui.vim_కిటికీ->చట్రం_2();
	gui.vim_కిటికీ->Unlock();
	*x = (పూర్ణాంకం)r.ఎడమ;
	*y = (పూర్ణాంకం)r.శిఖరం;
	తిరిగి OK;
    }
    లేకపోతే
	తిరిగి FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    శూన్య
gui_mch_సమితి_winpos(పూర్ణాంకం x, పూర్ణాంకం y)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_కిటికీ->Move_గమ్యం(x, y);
	gui.vim_కిటికీ->Unlock();
    }
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
శూన్య
gui_mch_సమితి_shellsize(
	పూర్ణాంకం	వెడల్పు,
	పూర్ణాంకం	ఎత్తు,
	పూర్ణాంకం	min_వెడల్పు,
	పూర్ణాంకం	min_ఎత్తు,
	పూర్ణాంకం	పునాది_వెడల్పు,
	పూర్ణాంకం	పునాది_ఎత్తు,
	పూర్ణాంకం	direction) // TODO: utilize?
{
    /*
     * We are basically given the size of the VimForm, if I understand
     * correctly. Since it fills the window completely, this will also
     * be the size of the window.
     */
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_కిటికీ->Resize_గమ్యం(వెడల్పు - PEN_వెడల్పు, ఎత్తు - PEN_వెడల్పు);

	// set size limits
	దశాంశం min_వెడల్పు_2, max_వెడల్పు, min_ఎత్తు_2, max_ఎత్తు;

	gui.vim_కిటికీ->Get_కొలత_Limits(&min_వెడల్పు_2, &max_వెడల్పు,
		&min_ఎత్తు_2, &max_ఎత్తు);
	gui.vim_కిటికీ->సమితి_కొలత_Limits(min_వెడల్పు, max_వెడల్పు,
		min_ఎత్తు, max_ఎత్తు);

	/*
	 * Set the resizing alignment depending on font size.
	 */
	gui.vim_కిటికీ->సమితి_కిటికీ_Alignment(
		B_PIXEL_ALIGNMENT,	//  window_alignment mode,
		1,		//  int32 h,
		0,		//  int32 hOffset = 0,
		gui.char_వెడల్పు,	    //	int32 width = 0,
		పునాది_వెడల్పు,	    //	int32 widthOffset = 0,
		1,		//  int32 v = 0,
		0,		//  int32 vOffset = 0,
		gui.char_ఎత్తు,	//  int32 height = 0,
		పునాది_ఎత్తు	    //	int32 heightOffset = 0
		);

	gui.vim_కిటికీ->Unlock();
    }
}

శూన్య
gui_mch_get_screen_dimensions(
	పూర్ణాంకం	*screen_w,
	పూర్ణాంకం	*screen_h)
{
    BRect చట్రం;

    {
	BScreen screen(gui.vim_కిటికీ);

	ఒకవేళ (screen.IsValid()) {
	    చట్రం = screen.చట్రం_2();
	} లేకపోతే {
	    చట్రం.కుడి = 640;
	    చట్రం.bottom = 480;
	}
    }

    // XXX approximations...
    *screen_w = (పూర్ణాంకం) చట్రం.కుడి - 2 * gui.scrollbar_వెడల్పు - 20;
    *screen_h = (పూర్ణాంకం) చట్రం.bottom - gui.scrollbar_ఎత్తు
#నిర్వచితమైతే FEAT_MENU
	- gui.menu_ఎత్తు
#అయితే_ముగింపు
	- 30;
}

శూన్య
gui_mch_సమితి_పాఠ్యం_area_pos(
	పూర్ణాంకం	x,
	పూర్ణాంకం	y,
	పూర్ణాంకం	w,
	పూర్ణాంకం	h)
{
    ఒకవేళ (!gui.vim_పాఠ్యం_Area)
	తిరిగి;

    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_పాఠ్యం_Area->Move_గమ్యం(x, y);
	gui.vim_పాఠ్యం_Area->Resize_గమ్యం(w - PEN_వెడల్పు, h - PEN_వెడల్పు);

#నిర్వచితమైతే FEAT_GUI_TABLINE
	ఒకవేళ (gui.vimForm->Tab_పంక్తి() != ఖాళీ) {
	    gui.vimForm->Tab_పంక్తి()->Resize_గమ్యం(w, gui.vimForm->Tabline_ఎత్తు());
	}
#అయితే_ముగింపు // FEAT_GUI_TABLINE

	gui.vim_కిటికీ->Unlock();
    }
}


/*
 * Scrollbar stuff:
 */

శూన్య
gui_mch_enable_scrollbar(
	scrollbar_T *sb,
	పూర్ణాంకం	flag)
{
    VimScrollBar *vsb = sb->id;
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	/*
	 * This function is supposed to be idempotent, but Show()/Hide()
	 * is not. Therefore we test if they are needed.
	 */
	ఒకవేళ (flag) {
	    ఒకవేళ (vsb->IsHidden()) {
		vsb->Show();
	    }
	} లేకపోతే {
	    ఒకవేళ (!vsb->IsHidden()) {
		vsb->Hide();
	    }
	}
	gui.vim_కిటికీ->Unlock();
    }
}

శూన్య
gui_mch_సమితి_scrollbar_thumb(
	scrollbar_T *sb,
	పూర్ణాంకం	val,
	పూర్ణాంకం	కొలత,
	పూర్ణాంకం	max)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	VimScrollBar *s = sb->id;
	ఒకవేళ (max == 0) {
	    s->సమితి_విలువ(0);
	    s->సమితి_Range(0.0, 0.0);
	} లేకపోతే {
	    s->సమితి_Proportion((దశాంశం)కొలత / (max + 1.0));
	    s->సమితి_అడుగులు(1.0, కొలత > 5 ? కొలత - 2 : కొలత);
#నిర్వచితం_కాకపోతే SCROLL_PAST_చివర	    //	really only defined in gui.c...
	    max = max + 1 - కొలత;
#అయితే_ముగింపు
	    ఒకవేళ (max < s->విలువ_2()) {
		/*
		 * If the new maximum is lower than the current value,
		 * setting it would cause the value to be clipped and
		 * therefore a ValueChanged() call.
		 * We avoid this by setting the value first, because
		 * it presumably is <= max.
		 */
		s->సమితి_విలువ(val);
		s->సమితి_Range(0.0, max);
	    } లేకపోతే {
		/*
		 * In the other case, set the range first, since the
		 * new value might be higher than the current max.
		 */
		s->సమితి_Range(0.0, max);
		s->సమితి_విలువ(val);
	    }
	}
	gui.vim_కిటికీ->Unlock();
    }
}

శూన్య
gui_mch_సమితి_scrollbar_pos(
	scrollbar_T *sb,
	పూర్ణాంకం	x,
	పూర్ణాంకం	y,
	పూర్ణాంకం	w,
	పూర్ణాంకం	h)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	BRect winb = gui.vim_కిటికీ->Bounds();
	దశాంశం vsbx = x, vsby = y;
	VimScrollBar *vsb = sb->id;
	vsb->Resize_గమ్యం(w - PEN_వెడల్పు, h - PEN_వెడల్పు);
	ఒకవేళ (winb.కుడి-(x+w)<w) vsbx = winb.కుడి - (w - PEN_వెడల్పు);
	vsb->Move_గమ్యం(vsbx, vsby);
	gui.vim_కిటికీ->Unlock();
    }
}

    పూర్ణాంకం
gui_mch_get_scrollbar_xpadding(శూన్య)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    తిరిగి 0;
}

    పూర్ణాంకం
gui_mch_get_scrollbar_ypadding(శూన్య)
{
    // TODO: Calculate the padding for adjust scrollbar position when the
    // Window is maximized.
    తిరిగి 0;
}

శూన్య
gui_mch_create_scrollbar(
	scrollbar_T *sb,
	పూర్ణాంకం	orient)	    // SBAR_VERT or SBAR_HORIZ
{
    orientation posture =
	(orient == SBAR_HORIZ) ? B_HORIZONTAL : B_VERTICAL;

    VimScrollBar *vsb = sb->id = కొత్త VimScrollBar(sb, posture);
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	vsb->సమితి_లక్ష్యం(gui.vim_పాఠ్యం_Area);
	vsb->Hide();
	gui.vimForm->కూడు_బిడ్డ(vsb);
	gui.vim_కిటికీ->Unlock();
    }
}

#ఒకవేళ defined(FEAT_WINDOWS) || defined(FEAT_GUI_HAIKU)
శూన్య
gui_mch_destroy_scrollbar(
	scrollbar_T *sb)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	sb->id->RemoveSelf();
	తొలగించు sb->id;
	gui.vim_కిటికీ->Unlock();
    }
}
#అయితే_ముగింపు

/*
 * Cursor does not flash
 */
    పూర్ణాంకం
gui_mch_is_blink_off(శూన్య)
{
    తిరిగి FALSE;
}

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON blinking, cursor is shown
 */

#నిర్వచించు BLINK_NONE  0
#నిర్వచించు BLINK_OFF   1
#నిర్వచించు BLINK_ON    2

స్థిర పూర్ణాంకం	blink_state = BLINK_NONE;
స్థిర long_u	    blink_waittime = 700;
స్థిర long_u	    blink_ontime = 400;
స్థిర long_u	    blink_offtime = 250;
స్థిర పూర్ణాంకం  blink_timer = 0;

శూన్య
gui_mch_సమితి_blinking(
	దీర్ఘ	waittime,
	దీర్ఘ	on,
	దీర్ఘ	off)
{
    // TODO
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    శూన్య
gui_mch_stop_blink(పూర్ణాంకం may_call_gui_update_cursor)
{
    // TODO
    ఒకవేళ (blink_timer != 0)
    {
	// XtRemoveTimeOut(blink_timer);
	blink_timer = 0;
    }
    ఒకవేళ (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    శూన్య
gui_mch_ప్రారంభం_blink()
{
    // TODO
    ఒకవేళ (blink_timer != 0)
	;// XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    ఒకవేళ (blink_waittime && blink_ontime && blink_offtime && gui.లోపల_focus)
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
పూర్ణాంకం
gui_mch_init_font(
	char_u	    *font_name,
	పూర్ణాంకం	    fontset)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2())
    {
	పూర్ణాంకం rc = gui.vim_పాఠ్యం_Area->mchInitFont(font_name);
	gui.vim_కిటికీ->Unlock();

	తిరిగి rc;
    }

    తిరిగి FAIL;
}


    పూర్ణాంకం
gui_mch_adjust_charsize()
{
    తిరిగి FAIL;
}


    పూర్ణాంకం
gui_mch_font_dialog(font_family* family, font_style* style, దశాంశం* కొలత)
{
#ఒకవేళ defined(FEAT_GUI_DIALOG)
	// gui.vimWindow->Unlock();
    VimSelectFontDialog *dialog = కొత్త VimSelectFontDialog(family, style, కొలత);
    తార్కిక ret = dialog->Go();
	తొలగించు dialog;
	తిరిగి ret;
#లేకపోతే
    తిరిగి NOFONT;
#అయితే_ముగింపు // FEAT_GUI_DIALOG
}


GuiFont
gui_mch_get_font(
	char_u	    *name,
	పూర్ణాంకం	    giveErrorIfMissing)
{
    స్థిర VimFont *fontList = ఖాళీ;

    ఒకవేళ (!gui.లోపల_use)	//  can't do this when GUI not running
	తిరిగి NOFONT;

    //	storage for locally modified name;
    స్థిరరాశి పూర్ణాంకం buff_కొలత = B_FONT_FAMILY_నిడివి + B_FONT_STYLE_నిడివి + 20;
    స్థిర అక్షరం font_name[buff_కొలత] = {0};
    font_family family = {0};
    font_style	style  = {0};
    దశాంశం కొలత = 0.f;

    ఒకవేళ (name == 0 && be_fixed_font == 0) {
	ఒకవేళ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	తిరిగి NOFONT;
    }

    తార్కిక useSelectGUI = అబద్ధం;
    ఒకవేళ (name != ఖాళీ)
	ఒకవేళ (STRCMP(name, "*") == 0) {
	    useSelectGUI = నిజం;
	    STRNCPY(font_name, hl_get_font_name(), buff_కొలత);
	} లేకపోతే
	    STRNCPY(font_name, name, buff_కొలత);

    ఒకవేళ (font_name[0] == 0) {
	be_fixed_font->GetFamilyAndStyle(&family, &style);
	కొలత = be_fixed_font->కొలత_2();
	vim_snprintf(font_name, buff_కొలత,
	    (అక్షరం*)"%s/%s/%.0f", family, style, కొలత);
    }

    //	replace underscores with spaces
    అక్షరం* చివర = 0;
    అయితే (చివర = strchr((అక్షరం *)font_name, '_'))
	*చివర = ' ';

    //	store the name before strtok corrupt the buffer ;-)
    స్థిర అక్షరం buff[buff_కొలత] = {0};
    STRNCPY(buff, font_name, buff_కొలత);
    STRNCPY(family, strtok(buff, "/\0"), B_FONT_FAMILY_నిడివి);
    అక్షరం* style_s = strtok(0, "/\0");
    ఒకవేళ (style_s != 0)
	STRNCPY(style, style_s, B_FONT_STYLE_నిడివి);
    కొలత = atof((style_s != 0) ? strtok(0, "/\0") : "0");

    ఒకవేళ (useSelectGUI) {
	ఒకవేళ (gui_mch_font_dialog(&family, &style, &కొలత) == NOFONT)
	    తిరిగి FAIL;
	//  compose for further processing
	vim_snprintf(font_name, buff_కొలత,
		(అక్షరం*)"%s/%s/%.0f", family, style, కొలత);
	hl_సమితి_font_name((char_u*)font_name);

	//  Set guifont to the name of the selected font.
	char_u* new_p_guifont = (char_u*)alloc(STRLEN(font_name) + 1);
	ఒకవేళ (new_p_guifont != ఖాళీ) {
	    STRCPY(new_p_guifont, font_name);
	    vim_free(p_guifont);
	    p_guifont = new_p_guifont;
	    //	Replace spaces in the font name with underscores.
	    కొరకు ( ; *new_p_guifont; ++new_p_guifont)
		ఒకవేళ (*new_p_guifont == ' ')
		    *new_p_guifont = '_';
	}
    }

    VimFont *flp;
    కొరకు (flp = fontList; flp; flp = flp->తదుపరి) {
	ఒకవేళ (STRCMP(font_name, flp->name) == 0) {
	    flp->refcount++;
	    తిరిగి (GuiFont)flp;
	}
    }

    VimFont *font = కొత్త VimFont();
    font->name = vim_strsave((char_u*)font_name);

    ఒకవేళ (లెక్క_font_styles(family) <= 0) {
	ఒకవేళ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), font->name);
	తొలగించు font;
	తిరిగి NOFONT;
    }

    //	Remember font in the static list for later use
    font->తదుపరి = fontList;
    fontList = font;

    font->సమితి_Family_And_Style(family, style);
    ఒకవేళ (కొలత > 0.f)
	font->సమితి_కొలత(కొలత);

    font->సమితి_Spacing(B_FIXED_SPACING);
    font->సమితి_Encoding(B_UNICODE_UTF8);

    తిరిగి (GuiFont)font;
}

/*
 * Set the current text font.
 */
శూన్య
gui_mch_సమితి_font(
	GuiFont	font)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	VimFont *vf = (VimFont *)font;

	gui.vim_పాఠ్యం_Area->సమితి_Font(vf);

	gui.char_వెడల్పు = (పూర్ణాంకం) vf->String_వెడల్పు("n");
	font_ఎత్తు fh;
	vf->Get_ఎత్తు(&fh);
	gui.char_ఎత్తు = (పూర్ణాంకం)(fh.ascent + 0.9999)
	    + (పూర్ణాంకం)(fh.descent + 0.9999) + (పూర్ణాంకం)(fh.leading + 0.9999);
	gui.char_ascent = (పూర్ణాంకం)(fh.ascent + 0.9999);

	gui.vim_కిటికీ->Unlock();
    }
}

// XXX TODO This is apparently never called...
శూన్య
gui_mch_free_font(
	GuiFont	font)
{
    ఒకవేళ (font == NOFONT)
	తిరిగి;
    VimFont *f = (VimFont *)font;
    ఒకవేళ (--f->refcount <= 0) {
	ఒకవేళ (f->refcount < 0)
	    fprintf(stderr, "VimFont: refcount < 0\n");
	తొలగించు f;
    }
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    ఒకవేళ (name == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి vim_strsave(name);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    పూర్ణాంకం
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
	font_ఎత్తు fh = {0};
	font->Get_ఎత్తు(&fh);
	gui.char_ఎత్తు = (పూర్ణాంకం)(fh.ascent + fh.descent + 0.5) + p_linespace;
	gui.char_ascent = (పూర్ణాంకం)(fh.ascent + 0.5) + p_linespace / 2;
    }
    తిరిగి OK;
}

    శూన్య
gui_mch_getmouse(పూర్ణాంకం *x, పూర్ణాంకం *y)
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

    శూన్య
gui_mch_mousehide(పూర్ణాంకం hide)
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
    తిరిగి gui_get_color_cmn(name);
}

/*
 * Set the current text foreground color.
 */
శూన్య
gui_mch_సమితి_fg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_గమ్యం_RGB(color);
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_పాఠ్యం_Area->సమితి_ఎగువ_Color(rgb);
	gui.vim_కిటికీ->Unlock();
    }
}

/*
 * Set the current text background color.
 */
శూన్య
gui_mch_సమితి_bg_color(
	guicolor_T  color)
{
    rgb_color rgb = GUI_గమ్యం_RGB(color);
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_పాఠ్యం_Area->సమితి_దిగువ_Color(rgb);
	gui.vim_కిటికీ->Unlock();
    }
}

/*
 * Set the current text special color.
 */
    శూన్య
gui_mch_సమితి_sp_color(guicolor_T	color)
{
    // prev_sp_color = color;
}

శూన్య
gui_mch_draw_string(
	పూర్ణాంకం	అడ్డువరుస,
	పూర్ణాంకం	నిలువు,
	char_u	*s,
	పూర్ణాంకం	len,
	పూర్ణాంకం	flags)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_పాఠ్యం_Area->mchDrawString(అడ్డువరుస, నిలువు, s, len, flags);
	gui.vim_కిటికీ->Unlock();
    }
}

	guicolor_T
gui_mch_get_rgb_color(పూర్ణాంకం r, పూర్ణాంకం g, పూర్ణాంకం b)
{
    తిరిగి gui_get_rgb_color_cmn(r, g, b);
}


// Return OK if the key with the termcap name "name" is supported.
పూర్ణాంకం
gui_mch_haskey(
	char_u	*name)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; special_కీలకాలు[i].Be_కీలకాలు != 0; i++)
	ఒకవేళ (name[0] == special_కీలకాలు[i].vim_code0 &&
		name[1] == special_కీలకాలు[i].vim_code1)
	    తిరిగి OK;
    తిరిగి FAIL;
}

    శూన్య
gui_mch_beep()
{
    ::beep();
}

    శూన్య
gui_mch_flash(పూర్ణాంకం msec)
{
    // Do a visual beep by reversing the foreground and background colors

    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	BRect rect = gui.vim_పాఠ్యం_Area->Bounds();

	gui.vim_పాఠ్యం_Area->సమితి_Drawing_బహుళకం(B_OP_INVERT);
	gui.vim_పాఠ్యం_Area->FillRect(rect);
	gui.vim_పాఠ్యం_Area->Sync();
	snooze(msec * 1000);	 // wait for a few msec
	gui.vim_పాఠ్యం_Area->FillRect(rect);
	gui.vim_పాఠ్యం_Area->సమితి_Drawing_బహుళకం(B_OP_COPY);
	gui.vim_పాఠ్యం_Area->Flush();
	gui.vim_కిటికీ->Unlock();
    }
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
శూన్య
gui_mch_invert_rectangle(
	పూర్ణాంకం	r,
	పూర్ణాంకం	c,
	పూర్ణాంకం	nr,
	పూర్ణాంకం	nc)
{
    BRect rect;
    rect.ఎడమ = FILL_X(c);
    rect.శిఖరం = FILL_Y(r);
    rect.కుడి = rect.ఎడమ + nc * gui.char_వెడల్పు - PEN_వెడల్పు;
    rect.bottom = rect.శిఖరం + nr * gui.char_ఎత్తు - PEN_వెడల్పు;

    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_పాఠ్యం_Area->సమితి_Drawing_బహుళకం(B_OP_INVERT);
	gui.vim_పాఠ్యం_Area->FillRect(rect);
	gui.vim_పాఠ్యం_Area->సమితి_Drawing_బహుళకం(B_OP_COPY);
	gui.vim_కిటికీ->Unlock();
    }
}

/*
 * Iconify the GUI window.
 */
    శూన్య
gui_mch_iconify()
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_కిటికీ->Minimize(నిజం);
	gui.vim_కిటికీ->Unlock();
    }
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    శూన్య
gui_mch_సమితి_foreground(శూన్య)
{
    // TODO
}
#అయితే_ముగింపు

/*
 * Set the window title
 */
శూన్య
gui_mch_settitle(
	char_u	*title,
	char_u	*icon)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_కిటికీ->సమితి_Title((అక్షరం *)title);
	gui.vim_కిటికీ->Unlock();
    }
}

/*
 * Draw a cursor without focus.
 */
    శూన్య
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    gui_mch_సమితి_fg_color(color);

    BRect r;
    r.ఎడమ = FILL_X(gui.నిలువు);
    r.శిఖరం = FILL_Y(gui.అడ్డువరుస);
    పూర్ణాంకం cells = utf_off2cells(పంక్తి_స్థానభ్రంశం[gui.అడ్డువరుస] + gui.నిలువు, 100); // TODO-TODO
    ఒకవేళ (cells>=4) cells = 1;
    r.కుడి = r.ఎడమ + cells*gui.char_వెడల్పు - PEN_వెడల్పు;
    r.bottom = r.శిఖరం + gui.char_ఎత్తు - PEN_వెడల్పు;

    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_పాఠ్యం_Area->StrokeRect(r);
	gui.vim_కిటికీ->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Draw part of a cursor, only w pixels wide, and h pixels high.
 */
శూన్య
gui_mch_draw_part_cursor(
	పూర్ణాంకం	w,
	పూర్ణాంకం	h,
	guicolor_T  color)
{
    gui_mch_సమితి_fg_color(color);

    BRect r;
    r.ఎడమ =
#నిర్వచితమైతే FEAT_RIGHTLEFT
	// vertical line should be on the right of current point
	CURSOR_BAR_కుడి ? FILL_X(gui.నిలువు + 1) - w :
#అయితే_ముగింపు
	FILL_X(gui.నిలువు);
    r.కుడి = r.ఎడమ + w - PEN_వెడల్పు;
    r.bottom = FILL_Y(gui.అడ్డువరుస + 1) - PEN_వెడల్పు;
    r.శిఖరం = r.bottom - h + PEN_వెడల్పు;

    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_పాఠ్యం_Area->FillRect(r);
	gui.vim_కిటికీ->Unlock();
	// gui_mch_flush();
    }
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    శూన్య
gui_mch_update()
{
    gui_mch_flush();
    అయితే (పోర్టు_లెక్క(gui.vdcmp) > 0 &&
	    !vim_is_ఆదానం_buf_నిండిన() &&
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
పూర్ణాంకం
gui_mch_wait_for_chars(
	దీర్ఘ	wtime)
{
    పూర్ణాంకం		focus;
    bigtime_t	until, timeout;
    స్థితి_t	st;

    ఒకవేళ (wtime >= 0)
    {
	timeout = wtime * 1000;
	until = system_సమయం() + timeout;
    }
    లేకపోతే
	timeout = B_INFINITE_TIMEOUT;

    focus = gui.లోపల_focus;
    కొరకు (;;)
    {
	// Stop or start blinking when focus changes
	ఒకవేళ (gui.లోపల_focus != focus)
	{
	    ఒకవేళ (gui.లోపల_focus)
		gui_mch_ప్రారంభం_blink();
	    లేకపోతే
		gui_mch_stop_blink(TRUE);
	    focus = gui.లోపల_focus;
	}

	gui_mch_flush();

#నిర్వచితమైతే సందేశం_వరుస
# ifdef FEAT_TIMERS
	did_కూడు_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ఒకవేళ (did_కూడు_timer)
	    // Need to recompute the waiting time.
	    ఆపు;
# endif
# ifdef FEAT_పని_CHANNEL
	ఒకవేళ (has_any_channel())
	{
	    ఒకవేళ (wtime < 0 || timeout > 20000)
		timeout = 20000;
	}
	లేకపోతే ఒకవేళ (wtime < 0)
	    timeout = B_INFINITE_TIMEOUT;
# endif
#అయితే_ముగింపు

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use gui_haiku_process_event() to hang until
	 * an event arrives.  No need to check for input_buf_full because we
	 * are returning as soon as it contains a single char.
	 */
	st = gui_haiku_process_event(timeout);

	ఒకవేళ (ఆదానం_అందుబాటు())
	    తిరిగి OK;
	ఒకవేళ (st < B_OK)		// includes B_TIMED_OUT
	    తిరిగి FAIL;

	/*
	 * Calculate how much longer we're willing to wait for the
	 * next event.
	 */
	ఒకవేళ (wtime >= 0)
	{
	    timeout = until - system_సమయం();
	    ఒకవేళ (timeout < 0)
		ఆపు;
	}
    }
    తిరిగి FAIL;

}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen. This is typically called before
 * the app goes to sleep.
 */
    శూన్య
gui_mch_flush()
{
    //	does this need to lock the window? Apparently not but be safe.
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_కిటికీ->Flush();
	gui.vim_కిటికీ->Unlock();
    }
    తిరిగి;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
శూన్య
gui_mch_clear_ఖండం(
	పూర్ణాంకం	row1,
	పూర్ణాంకం	col1,
	పూర్ణాంకం	row2,
	పూర్ణాంకం	col2)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_పాఠ్యం_Area->mch_Clear_ఖండం(row1, col1, row2, col2);
	gui.vim_కిటికీ->Unlock();
    }
}

    శూన్య
gui_mch_clear_all()
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	gui.vim_పాఠ్యం_Area->mchClearAll();
	gui.vim_కిటికీ->Unlock();
    }
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
శూన్య
gui_mch_delete_పంక్తులు(
	పూర్ణాంకం	అడ్డువరుస,
	పూర్ణాంకం	అంకె_పంక్తులు)
{
    gui.vim_పాఠ్యం_Area->mch_Delete_పంక్తులు(అడ్డువరుస, అంకె_పంక్తులు);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
శూన్య
gui_mch_చొప్పించు_పంక్తులు(
	పూర్ణాంకం	అడ్డువరుస,
	పూర్ణాంకం	అంకె_పంక్తులు)
{
    gui.vim_పాఠ్యం_Area->mch_చొప్పించు_పంక్తులు(అడ్డువరుస, అంకె_పంక్తులు);
}

#ఒకవేళ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

శూన్య
gui_mch_enable_menu(
	పూర్ణాంకం	flag)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2())
    {
	BMenuBar *menubar = gui.vimForm->MenuBar();
	menubar->సమితి_Enabled(flag);
	gui.vim_కిటికీ->Unlock();
    }
}

శూన్య
gui_mch_సమితి_menu_pos(
	పూర్ణాంకం	x,
	పూర్ణాంకం	y,
	పూర్ణాంకం	w,
	పూర్ణాంకం	h)
{
    // It will be in the right place anyway
}

/*
 * Add a sub menu to the menu bar.
 */
శూన్య
gui_mch_కూడు_menu(
	vimmenu_T   *menu,
	పూర్ణాంకం	idx)
{
    vimmenu_T	*మాతృ = menu->మాతృ;

    //	popup menu - just create it unattached
    ఒకవేళ (menu_is_popup(menu->name) && మాతృ == ఖాళీ) {
	BPop_పైకి_Menu* తీయి_పైకి_Menu = కొత్త BPop_పైకి_Menu((స్థిరరాశి అక్షరం*)menu->name, అబద్ధం, అబద్ధం);
	menu->submenu_id = తీయి_పైకి_Menu;
	menu->id = ఖాళీ;
	తిరిగి;
    }

    ఒకవేళ (!menu_is_menubar(menu->name)
	    || (మాతృ != ఖాళీ && మాతృ->submenu_id == ఖాళీ))
	తిరిగి;

    ఒకవేళ (gui.vim_కిటికీ->తాళం_2())
    {
	// Major re-write of the menu code, it was failing with memory corruption when
	// we started loading multiple files (the Buffer menu)
	//
	// Note we don't use the preference values yet, all are inserted into the
	// menubar on a first come-first served basis...
	//
	// richard@whitequeen.com jul 99

	BMenu *tmp;

	ఒకవేళ ( మాతృ )
	    tmp = మాతృ->submenu_id;
	లేకపోతే
	    tmp = gui.vimForm->MenuBar();
	//  make sure we don't try and add the same menu twice. The Buffers menu tries to
	//  do this and Be starts to crash...

	ఒకవేళ ( ! tmp->కనుగొను_అంశం((స్థిరరాశి అక్షరం *) menu->dname)) {

	    BMenu *bmenu = కొత్త BMenu((అక్షరం *)menu->dname);

	    menu->submenu_id = bmenu;

	    //	when we add a BMenu to another Menu, it creates the interconnecting BMenuItem
	    tmp->కూడు_అంశం(bmenu);

	    //	Now it's safe to query the menu for the associated MenuItem...
	    menu->id = tmp->కనుగొను_అంశం((స్థిరరాశి అక్షరం *) menu->dname);

	}
	gui.vim_కిటికీ->Unlock();
    }
}

    శూన్య
gui_mch_toggle_tearoffs(పూర్ణాంకం enable)
{
    // no tearoff menus
}

    స్థిర BMessage *
Menu_సందేశం(vimmenu_T *menu)
{
    BMessage *m = కొత్త BMessage('menu');
    m->కూడు_సూచిక("VimMenu", (శూన్య *)menu);

    తిరిగి m;
}

/*
 * Add a menu item to a menu
 */
శూన్య
gui_mch_కూడు_menu_అంశం(
	vimmenu_T   *menu,
	పూర్ణాంకం	idx)
{
    పూర్ణాంకం	    mnemonic = 0;
    vimmenu_T	*మాతృ = menu->మాతృ;

    // TODO: use menu->actext
    // This is difficult, since on Be, an accelerator must be a single char
    // and a lot of Vim ones are the standard VI commands.
    //
    // Punt for Now...
    // richard@whiequeen.com jul 99
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2())
    {
#నిర్వచితమైతే FEAT_TOOLBAR
	ఒకవేళ (menu_is_toolbar(మాతృ->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ఒకవేళ (toolbar != ఖాళీ) {
		toolbar->కూడు_Button(idx, menu);
	    }
	} లేకపోతే
#అయితే_ముగింపు

	ఒకవేళ (మాతృ->submenu_id != ఖాళీ || menu_is_popup(మాతృ->name)) {
	    ఒకవేళ (menu_is_separator(menu->name)) {
		BSeparator_అంశం *అంశం = కొత్త BSeparator_అంశం();
		మాతృ->submenu_id->కూడు_అంశం(అంశం);
		menu->id = అంశం;
		menu->submenu_id = ఖాళీ;
	    }
	    లేకపోతే {
		BMenu_అంశం *అంశం = కొత్త BMenu_అంశం((అక్షరం *)menu->dname,
			Menu_సందేశం(menu));
		అంశం->సమితి_లక్ష్యం(gui.vim_పాఠ్యం_Area);
		అంశం->సమితి_Trigger((అక్షరం) menu->mnemonic);
		మాతృ->submenu_id->కూడు_అంశం(అంశం);
		menu->id = అంశం;
		menu->submenu_id = ఖాళీ;
	    }
	}
	gui.vim_కిటికీ->Unlock();
    }
}

/*
 * Destroy the machine specific menu widget.
 */
శూన్య
gui_mch_destroy_menu(
	vimmenu_T   *menu)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2())
    {
#నిర్వచితమైతే FEAT_TOOLBAR
	ఒకవేళ (menu->మాతృ && menu_is_toolbar(menu->మాతృ->name)) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ఒకవేళ (toolbar != ఖాళీ) {
		toolbar->RemoveButton(menu);
	    }
	} లేకపోతే
#అయితే_ముగింపు
	{
	    ప్రతిజ్ఞా(menu->submenu_id == ఖాళీ || menu->submenu_id->లెక్క_వస్తువులు() == 0);
	    /*
	     * Detach this menu from its parent, so that it is not deleted
	     * twice once we get to delete that parent.
	     * Deleting a BMenuItem also deletes the associated BMenu, if any
	     * (which does not have any items anymore since they were
	     * removed and deleted before).
	     */
	    BMenu *bmenu = menu->id->Menu();
	    ఒకవేళ (bmenu)
	    {
		bmenu->Remove_అంశం(menu->id);
		/*
		 * If we removed the last item from the menu bar,
		 * resize it out of sight.
		 */
		ఒకవేళ (bmenu == gui.vimForm->MenuBar() && bmenu->లెక్క_వస్తువులు() == 0)
		{
		    bmenu->Resize_గమ్యం(-MENUBAR_MARGIN, -MENUBAR_MARGIN);
		}
	    }
	    తొలగించు menu->id;
	    menu->id = ఖాళీ;
	    menu->submenu_id = ఖాళీ;

	    gui.menu_ఎత్తు = (పూర్ణాంకం) gui.vimForm->Menu_ఎత్తు();
	}
	gui.vim_కిటికీ->Unlock();
    }
}

/*
 * Make a menu either grey or not grey.
 */
శూన్య
gui_mch_menu_grey(
	vimmenu_T   *menu,
	పూర్ణాంకం	grey)
{
#నిర్వచితమైతే FEAT_TOOLBAR
    ఒకవేళ (menu->మాతృ && menu_is_toolbar(menu->మాతృ->name)) {
	ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	    VimToolbar *toolbar = gui.vimForm->ToolBar();
	    ఒకవేళ (toolbar != ఖాళీ) {
		toolbar->బూడిద_Button(menu, grey);
	    }
	    gui.vim_కిటికీ->Unlock();
	}
    } లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (menu->id != ఖాళీ)
	menu->id->సమితి_Enabled(!grey);
}

/*
 * Make menu item hidden or not hidden
 */
శూన్య
gui_mch_menu_hidden(
	vimmenu_T   *menu,
	పూర్ణాంకం	hidden)
{
    ఒకవేళ (menu->id != ఖాళీ)
	menu->id->సమితి_Enabled(!hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    శూన్య
gui_mch_draw_menubar()
{
    // Nothing to do in BeOS
}

    శూన్య
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ఒకవేళ (!menu_is_popup(menu->name) || menu->submenu_id == ఖాళీ)
	తిరిగి;

    BPop_పైకి_Menu* popupMenu = dynamic_cast<BPop_పైకి_Menu*>(menu->submenu_id);
    ఒకవేళ (popupMenu == ఖాళీ)
	తిరిగి;

    BPoint బిందువు;
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	uint32 buttons = 0;
	gui.vim_పాఠ్యం_Area->GetMouse(&బిందువు, &buttons);
	gui.vim_పాఠ్యం_Area->Convert_గమ్యం_Screen(&బిందువు);
	gui.vim_కిటికీ->Unlock();
    }
    popupMenu->Go(బిందువు, నిజం);
}

#అయితే_ముగింపు // FEAT_MENU

// Mouse stuff

#నిర్వచితమైతే FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */
అక్షరం textplain[] = "text/plain";
అక్షరం vimselectiontype[] = "application/x-vnd.Rhialto-Vim-selectiontype";

/*
 * Get the current selection and put it in the clipboard register.
 */
    శూన్య
clip_mch_request_selection(Clipboard_T *cbd)
{
    ఒకవేళ (be_clipboard->తాళం_2())
    {
	BMessage *m = be_clipboard->దత్తాంశం_2();
	// m->PrintToStream();

	char_u *string = ఖాళీ;
	ssize_t stringlen = -1;

	ఒకవేళ (m->కనుగొను_దత్తాంశం(textplain, B_MIME_TYPE,
		    (స్థిరరాశి శూన్య **)&string, &stringlen) == B_OK
		|| m->కనుగొను_String("text", (స్థిరరాశి అక్షరం **)&string) == B_OK)
	{
	    ఒకవేళ (stringlen == -1)
		stringlen = STRLEN(string);

	    పూర్ణాంకం type;
	    అక్షరం *seltype;
	    ssize_t seltypelen;

	    /*
	     * Try to get the special vim selection type first
	     */
	    ఒకవేళ (m->కనుగొను_దత్తాంశం(vimselectiontype, B_MIME_TYPE,
			(స్థిరరాశి శూన్య **)&seltype, &seltypelen) == B_OK)
	    {
		ఎంపిక (*seltype)
		{
		    అప్రమేయ:
		    స్థితి 'L':	type = MLINE;	ఆపు;
		    స్థితి 'C':	type = MCHAR;	ఆపు;
#నిర్వచితమైతే FEAT_VISUAL
		    స్థితి 'B':	type = MBLOCK;	ఆపు;
#అయితే_ముగింపు
		}
	    }
	    లేకపోతే
	    {
		// Otherwise use heuristic as documented
		type = memchr(string, stringlen, '\n') ? MLINE : MCHAR;
	    }
	    clip_yank_selection(type, string, (దీర్ఘ)stringlen, cbd);
	}
	be_clipboard->Unlock();
    }
}
/*
 * Make vim the owner of the current selection.
 */
    శూన్య
clip_mch_lose_selection(Clipboard_T *cbd)
{
    // Nothing needs to be done here
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    పూర్ణాంకం
clip_mch_own_selection(Clipboard_T *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    తిరిగి FAIL;
}

/*
 * Send the current selection to the clipboard.
 */
    శూన్య
clip_mch_సమితి_selection(Clipboard_T *cbd)
{
    ఒకవేళ (be_clipboard->తాళం_2())
    {
	be_clipboard->Clear();
	BMessage *m = be_clipboard->దత్తాంశం_2();
	ప్రతిజ్ఞా(m);

	// If the '*' register isn't already filled in, fill it in now
	cbd->owned = TRUE;
	clip_get_selection(cbd);
	cbd->owned = FALSE;

	char_u	*str = ఖాళీ;
	long_u	లెక్క;
	పూర్ణాంకం type;

	type = clip_convert_selection(&str, &లెక్క, cbd);

	ఒకవేళ (type < 0)
	    తిరిగి;

	m->కూడు_దత్తాంశం(textplain, B_MIME_TYPE, (శూన్య *)str, లెక్క);

	// Add type of selection
	అక్షరం	vtype;
	ఎంపిక (type)
	{
	    అప్రమేయ:
	    స్థితి MLINE:    vtype = 'L';    ఆపు;
	    స్థితి MCHAR:    vtype = 'C';    ఆపు;
#నిర్వచితమైతే FEAT_VISUAL
	    స్థితి MBLOCK:   vtype = 'B';    ఆపు;
#అయితే_ముగింపు
	}
	m->కూడు_దత్తాంశం(vimselectiontype, B_MIME_TYPE, (శూన్య *)&vtype, 1);

	vim_free(str);

	be_clipboard->Commit();
	be_clipboard->Unlock();
    }
}

#అయితే_ముగింపు	// FEAT_CLIPBOARD

#నిర్వచితమైతే FEAT_BROWSE
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
	పూర్ణాంకం saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    gui.vimApp->fFilePanel = కొత్త BFilePanel((saving == TRUE) ? B_SAVE_PANEL : B_OPEN_PANEL,
	    ఖాళీ, ఖాళీ, 0, అబద్ధం,
	    కొత్త BMessage((saving == TRUE) ? 'save' : 'open'), ఖాళీ, నిజం);

    gui.vimApp->f_Browsed_మార్గం.Unset();

    gui.vimApp->fFilePanel->కిటికీ()->సమితి_Title((అక్షరం*)title);
    gui.vimApp->fFilePanel->సమితి_Panel_Directory((స్థిరరాశి అక్షరం*)initdir);

    gui.vimApp->fFilePanel->Show();

    gui.vimApp->fFilePanelSem = create_sem(0, "FilePanelSem");

    అయితే (acquire_sem(gui.vimApp->fFilePanelSem) == B_INTERRUPTED);

    char_u *fileName = ఖాళీ;
    స్థితి_t ఫలితం = gui.vimApp->f_Browsed_మార్గం.InitCheck();
    ఒకవేళ (ఫలితం == B_OK) {
	fileName = vim_strsave((char_u*)gui.vimApp->f_Browsed_మార్గం.మార్గం_2());
    } లేకపోతే
	ఒకవేళ (ఫలితం != B_NO_INIT) {
	    fprintf(stderr, "gui_mch_browse: BPath error: %#08x (%s)\n",
		    ఫలితం, strerror(ఫలితం));
	}

    తొలగించు gui.vimApp->fFilePanel;
    gui.vimApp->fFilePanel = ఖాళీ;

    తిరిగి fileName;
}
#అయితే_ముగింపు // FEAT_BROWSE


#ఒకవేళ defined(FEAT_GUI_DIALOG)

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

పూర్ణాంకం
gui_mch_dialog(
	పూర్ణాంకం	 type,
	char_u	*title,
	char_u	*సందేశం_2,
	char_u	*buttons,
	పూర్ణాంకం	 dfltbutton,
	char_u	*textfield,
	పూర్ణాంకం ex_cmd)
{
    VimDialog *dialog = కొత్త VimDialog(type, (అక్షరం*)title, (అక్షరం*)సందేశం_2,
	    (అక్షరం*)buttons, dfltbutton, (అక్షరం*)textfield, ex_cmd);
    తార్కిక ret = dialog->Go();
    తొలగించు dialog;
	తిరిగి ret;
}

#అయితే_ముగింపు // FEAT_GUI_DIALOG


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    rgb_color rgb = GUI_గమ్యం_RGB(pixel);

    తిరిగి ((rgb.red & 0xff) << 16) + ((rgb.green & 0xff) << 8)
	+ (rgb.blue & 0xff);
}

    శూన్య
gui_mch_setmouse(పూర్ణాంకం x, పూర్ణాంకం y)
{
    TRACE();
    // TODO
}

#నిర్వచితమైతే FEAT_MBYTE_IME
    శూన్య
im_సమితి_position(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    ఒకవేళ (gui.vim_కిటికీ->తాళం_2())
    {
	gui.vim_పాఠ్యం_Area->DrawIMString();
	gui.vim_కిటికీ->Unlock();
    }
    తిరిగి;
}
#అయితే_ముగింపు

    శూన్య
gui_mch_show_toolbar(పూర్ణాంకం showit)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    gui.toolbar_ఎత్తు = (toolbar && showit) ? toolbar->Toolbar_ఎత్తు() : 0.;
}

    శూన్య
gui_mch_సమితి_toolbar_pos(పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    VimToolbar *toolbar = gui.vimForm->ToolBar();
    ఒకవేళ (toolbar != ఖాళీ) {
	ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	    toolbar->Move_గమ్యం(x, y);
	    toolbar->Resize_గమ్యం(w - 1, h - 1);
	    gui.vim_కిటికీ->Unlock();
	}
    }
}

#ఒకవేళ defined(FEAT_GUI_TABLINE)

/*
 * Show or hide the tabline.
 */
    శూన్య
gui_mch_show_tabline(పూర్ణాంకం showit)
{
    Vim_Tab_పంక్తి *tab_పంక్తి = gui.vimForm->Tab_పంక్తి();

    ఒకవేళ (tab_పంక్తి == ఖాళీ)
	తిరిగి;

    ఒకవేళ (!showit != !gui.vimForm->Is_Showing_Tab_పంక్తి()) {
	gui.vimForm->సమితి_Showing_Tab_పంక్తి(showit != 0);
	gui.tabline_ఎత్తు = gui.vimForm->Tabline_ఎత్తు();
    }
}

    శూన్య
gui_mch_సమితి_tabline_pos(పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    Vim_Tab_పంక్తి *tab_పంక్తి = gui.vimForm->Tab_పంక్తి();
    ఒకవేళ (tab_పంక్తి != ఖాళీ) {
	ఒకవేళ (gui.vim_కిటికీ->తాళం_2()) {
	    tab_పంక్తి->Move_గమ్యం(x, y);
	    tab_పంక్తి->Resize_గమ్యం(w - 1, h - 1);
	    gui.vim_కిటికీ->Unlock();
	}
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    పూర్ణాంకం
gui_mch_showing_tabline()
{
    Vim_Tab_పంక్తి *tab_పంక్తి = gui.vimForm->Tab_పంక్తి();
    తిరిగి tab_పంక్తి != ఖాళీ && gui.vimForm->Is_Showing_Tab_పంక్తి();
}

/*
 * Update the labels of the tabline.
 */
    శూన్య
gui_mch_update_tabline()
{
    tabpage_T	*tp;
    పూర్ణాంకం	    nr = 0;
    పూర్ణాంకం	    curtabidx = 0;

    Vim_Tab_పంక్తి *tab_పంక్తి = gui.vimForm->Tab_పంక్తి();

    ఒకవేళ (tab_పంక్తి == ఖాళీ)
	తిరిగి;

    gui.vim_కిటికీ->తాళం_2();

    // Add a label for each tab page.  They all contain the same text area.
    కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ; tp = tp->tp_తదుపరి, ++nr) {
	ఒకవేళ (tp == curtab)
	    curtabidx = nr;

	BTab* tab = tab_పంక్తి->TabAt(nr);

	ఒకవేళ (tab == ఖాళీ) {
	    tab = కొత్త Vim_Tab_పంక్తి::VimTab();
	    tab_పంక్తి->కూడు_Tab(ఖాళీ, tab);
	}

	get_tabline_label(tp, FALSE);
	tab->సమితి_Label((స్థిరరాశి అక్షరం*)NameBuff);
	tab_పంక్తి->Invalidate();
    }

    // Remove any old labels.
    అయితే (nr < tab_పంక్తి->లెక్క_Tabs())
	tab_పంక్తి->RemoveTab(nr);

    ఒకవేళ (tab_పంక్తి->Selection() != curtabidx)
	tab_పంక్తి->Select(curtabidx);

    gui.vim_కిటికీ->Unlock();
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    శూన్య
gui_mch_సమితి_curtab(పూర్ణాంకం nr)
{
    Vim_Tab_పంక్తి *tab_పంక్తి = gui.vimForm->Tab_పంక్తి();
    ఒకవేళ (tab_పంక్తి == ఖాళీ)
	తిరిగి;

    gui.vim_కిటికీ->తాళం_2();

    ఒకవేళ (tab_పంక్తి->Selection() != nr -1)
	tab_పంక్తి->Select(nr -1);

    gui.vim_కిటికీ->Unlock();
}

#అయితే_ముగింపు // FEAT_GUI_TABLINE
