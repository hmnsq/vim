/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved    by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 * File MOTIF_REDEFINES.H originally delivers together with
 * OpenVMS Porting Library
 * defunct link: openvms.compaq.com/openvms/products/ips/porting.html
 *
 * This file has been modified for Vim development.
 * Original file contains just defines that GTK for OpenVMS uses,
 * but not all functions that DECW library has. Therefore it has been expanded
 * with necessary defines for Vim on OpenVMS with GTK GUI.
 *
 * Zoltan Arpadffy <arpadffy@polarhome.com>
 */

/*
 *************************************************************************
 *									 *
 * Copyright 2000 Compaq Computer Corporation				 *
 *									 *
 * COMPAQ Registered in U.S. Patent and Trademark Office.		 *
 *									 *
 *************************************************************************
 * IMPORTANT: Carefully read the License Terms below before		 *
 * proceeding.	By use of these materials you agree to these terms.	 *
 * If you do not agree to these terms, you may not use this software or  *
 * the accompanying documentation.					 *
 *************************************************************************
 * LICENSE TERMS							 *
 * 1. GRANT								 *
 * Compaq Computer Corporation ("COMPAQ") grants you the right to use,	 *
 * modify, and distribute the following source code (the "Software")	 *
 * on any number of computers. You may use the Software as part of	 *
 * creating a software program or product intended for commercial or	 *
 * non-commercial distribution in machine-readable source code, binary,  *
 * or executable formats. You may distribute the Software as		 *
 * machine-readable source code provided this license is not removed	 *
 * from the Software and any modifications are conspicuously indicated.  *
 * 2. COPYRIGHT								 *
 * The Software is owned by COMPAQ and its suppliers and is protected by *
 * copyright laws and international treaties.  Your use of the Software  *
 * and associated documentation is subject to the applicable copyright	 *
 * laws and the express rights and restrictions of these terms.		 *
 * 3. RESTRICTIONS							 *
 * You may not remove any copyright, trademark, or other proprietary	 *
 * notices from the Software or the associated	documentation.		 *
 * You are responsible for compliance with all applicable export or	 *
 * re-export control laws and regulations if you export the Software.	 *
 * This license is governed by and is to be construed under the laws	 *
 * of the State of Texas.						 *
 *									 *
 * DISCLAIMER OF WARRANTY AND LIABILITY					 *
 * Compaq shall not be liable for technical or editorial errors or	 *
 * omissions contained herein. The information contained herein is	 *
 * subject to change without notice.					 *
 *									 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.	 *
 * THE ENTIRE RISK ARISING OUT OF THE USE OF THIS SOFTWARE REMAINS WITH  *
 * RECIPIENT.  IN NO EVENT SHALL COMPAQ BE LIABLE FOR ANY DIRECT,	 *
 * CONSEQUENTIAL, INCIDENTAL, SPECIAL, PUNITIVE OR OTHER DAMAGES	 *
 * WHATSOEVER (INCLUDING WITHOUT LIMITATION DAMAGES FOR LOSS OF BUSINESS *
 * PROFITS, BUSINESS INTERRUPTION, OR LOSS OF BUSINESS INFORMATION),	 *
 * EVEN IF COMPAQ HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES	 *
 * AND WHETHER IN AN ACTION OF CONTRACT OR TORT INCLUDING NEGLIGENCE.	 *
 *									 *
 * If you have any questions concerning this license, please contact:	 *
 * Compaq Computer Corporation, Software Business Practices, ZKO1-2/D22, *
 * 110 Spit Brook Road, Nashua, NH. 03062-2698.				 *
 *									 *
 *************************************************************************
 */

// INTRINSIC.H omits proto if XtFree is defined
// VMS_BEGIN_C_PLUS_PLUS
బాహ్య శూన్య XtFree(అక్షరం*);
బాహ్య శూన్య XTFREE(అక్షరం*);
// VMS_END_C_PLUS_PLUS

#నిర్వచించు _XRegisterFilterByType _XREGISTERFILTERBYTYPE

#నిర్వచించు XAllocClassHint XALLOCCLASSHINT
#నిర్వచించు XAllocColor XALLOCCOLOR
#నిర్వచించు XAllocColorCells XALLOCCOLORCELLS
#నిర్వచించు XAlloc_కొలత_Hints XALLOCSIZEHINTS
#నిర్వచించు XAllocWMHints XALLOCWMHINTS
#నిర్వచించు XAutoRepeatOff XAUTOREPEATOFF
#నిర్వచించు XAutoRepeatOn XAUTOREPEATON
#నిర్వచించు XBase_Font_Name_List_యొక్క_Font_సమితి XBASEFONTNAMELISTOFFONTSET
#నిర్వచించు XBell XBELL
#నిర్వచించు XBitmapPad XBITMAPPAD
#నిర్వచించు XChange_చురుకైన_సూచిక_Grab XCHANGEACTIVEPOINTERGRAB
#నిర్వచించు XChangeGC XCHANGEGC
#నిర్వచించు XChangeProperty XCHANGEPROPERTY
#నిర్వచించు XChange_కిటికీ_లక్షణాలు XCHANGEWINDOWATTRIBUTES
#నిర్వచించు XCheckIfEvent XCHECKIFEVENT
#నిర్వచించు XCheck_ముసుగు_Event XCHECKMASKEVENT
#నిర్వచించు XCheckTypedEvent XCHECKTYPEDEVENT
#నిర్వచించు XCheck_Typed_కిటికీ_Event XCHECKTYPEDWINDOWEVENT
#నిర్వచించు XCheck_కిటికీ_Event XCHECKWINDOWEVENT
#నిర్వచించు XClearArea XCLEARAREA
#నిర్వచించు XClear_కిటికీ XCLEARWINDOW
#నిర్వచించు XClipBox XCLIPBOX
#నిర్వచించు XCloseDisplay XCLOSEDISPLAY
#నిర్వచించు XCloseIM XCLOSEIM
#నిర్వచించు XConfigure_కిటికీ XCONFIGUREWINDOW
#నిర్వచించు XConvertSelection XCONVERTSELECTION
#నిర్వచించు XCopyArea XCOPYAREA
#నిర్వచించు XCopyGC XCOPYGC
#నిర్వచించు XCopyPlane XCOPYPLANE
#నిర్వచించు XCreate_Bitmap_From_దత్తాంశం XCREATEBITMAPFROMDATA
#నిర్వచించు XCreateColormap XCREATECOLORMAP
#నిర్వచించు XCreateFontCursor XCREATEFONTCURSOR
#నిర్వచించు XCreate_Font_సమితి XCREATEFONTSET
#నిర్వచించు XCreateGC XCREATEGC
#నిర్వచించు XCreateIC XCREATEIC
#నిర్వచించు XCreateImage XCREATEIMAGE
#నిర్వచించు XCreatePixmap XCREATEPIXMAP
#నిర్వచించు XCreatePixmapCursor XCREATEPIXMAPCURSOR
#నిర్వచించు XCreate_Pixmap_From_Bitmap_దత్తాంశం XCREATEPIXMAPFROMBITMAPDATA
#నిర్వచించు XCreateRegion XCREATEREGION
#నిర్వచించు XCreate_Simple_కిటికీ XCREATESIMPLEWINDOW
#నిర్వచించు XCreate_కిటికీ XCREATEWINDOW
#నిర్వచించు XDefault_మూలం_కిటికీ XDEFAULTROOTWINDOW
#నిర్వచించు XDefault_Screen_యొక్క_Display XDEFAULTSCREENOFDISPLAY
#నిర్వచించు XDefineCursor XDEFINECURSOR
#నిర్వచించు XDeleteProperty XDELETEPROPERTY
#నిర్వచించు XDestroyIC XDESTROYIC
#నిర్వచించు XDestroyRegion XDESTROYREGION
#నిర్వచించు XDestroy_కిటికీ XDESTROYWINDOW
#నిర్వచించు XDisplayName XDISPLAYNAME
#నిర్వచించు XDisplay_యొక్క_Screen XDISPLAYOFSCREEN
#నిర్వచించు XDisplayString XDISPLAYSTRING
#నిర్వచించు XDrawArc XDRAWARC
#నిర్వచించు XDrawImageString XDRAWIMAGESTRING
#నిర్వచించు XDrawImageString16 XDRAWIMAGESTRING16
#నిర్వచించు XDraw_పంక్తి XDRAWLINE
#నిర్వచించు XDraw_పంక్తులు XDRAWLINES
#నిర్వచించు XDraw_బిందువు XDRAWPOINT
#నిర్వచించు XDrawPoints XDRAWPOINTS
#నిర్వచించు XDrawRectangle XDRAWRECTANGLE
#నిర్వచించు XDrawSegments XDRAWSEGMENTS
#నిర్వచించు XDrawString XDRAWSTRING
#నిర్వచించు XDrawString16 XDRAWSTRING16
#నిర్వచించు XEmptyRegion XEMPTYREGION
#నిర్వచించు XEqualRegion XEQUALREGION
#నిర్వచించు XEventsQueued XEVENTSQUEUED
#నిర్వచించు XExtents_యొక్క_Font_సమితి XEXTENTSOFFONTSET
#నిర్వచించు XFetch_బఫరు XFETCHBUFFER
#నిర్వచించు XFillArc XFILLARC
#నిర్వచించు XFillPolygon XFILLPOLYGON
#నిర్వచించు XFillRectangle XFILLRECTANGLE
#నిర్వచించు XFillRectangles XFILLRECTANGLES
#నిర్వచించు XFilterEvent XFILTEREVENT
#నిర్వచించు XFlush XFLUSH
#నిర్వచించు XFonts_యొక్క_Font_సమితి XFONTSOFFONTSET
#నిర్వచించు XFree XFREE
#నిర్వచించు XFreeColormap XFREECOLORMAP
#నిర్వచించు XFreeColors XFREECOLORS
#నిర్వచించు XFreeCursor XFREECURSOR
#నిర్వచించు XFreeFont XFREEFONT
#నిర్వచించు XFreeFontInfo XFREEFONTINFO
#నిర్వచించు XFree_Font_పేర్లు XFREEFONTNAMES
#నిర్వచించు XFree_Font_సమితి XFREEFONTSET
#నిర్వచించు XFreeGC XFREEGC
#నిర్వచించు XFreeModifiermap XFREEMODIFIERMAP
#నిర్వచించు XFreePixmap XFREEPIXMAP
#నిర్వచించు XFreeStringList XFREESTRINGLIST
#నిర్వచించు XGetAtomName XGETATOMNAME
#నిర్వచించు XGetDefault XGETDEFAULT
#నిర్వచించు XGet_Error_Database_పాఠ్యం XGETERRORDATABASETEXT
#నిర్వచించు XGet_Error_పాఠ్యం XGETERRORTEXT
#నిర్వచించు XGetFontProperty XGETFONTPROPERTY
#నిర్వచించు XGetGCValues XGETGCVALUES
#నిర్వచించు XGetGeometry XGETGEOMETRY
#నిర్వచించు XGetIconSizes XGETICONSIZES
#నిర్వచించు XGetICValues XGETICVALUES
#నిర్వచించు XGetIMValues XGETIMVALUES
#నిర్వచించు XGetImage XGETIMAGE
#నిర్వచించు XGetKeyboardControl XGETKEYBOARDCONTROL
#నిర్వచించు XGetModifierMapping XGETMODIFIERMAPPING
#నిర్వచించు XGetMotionEvents XGETMOTIONEVENTS
#నిర్వచించు XGet_సాధారణ_Hints XGETNORMALHINTS
#నిర్వచించు XGetSelectionOwner XGETSELECTIONOWNER
#నిర్వచించు XGetSubImage XGETSUBIMAGE
#నిర్వచించు XGetVisualInfo XGETVISUALINFO
#నిర్వచించు XGetWMColormapWindows XGETWMCOLORMAPWINDOWS
#నిర్వచించు XGetWMIconName XGETWMICONNAME
#నిర్వచించు XGetWMProtocols XGETWMPROTOCOLS
#నిర్వచించు XGetWMHints XGETWMHINTS
#నిర్వచించు XGetWMName XGETWMNAME
#నిర్వచించు XGetWMNormalHints XGETWMNORMALHINTS
#నిర్వచించు XGet_కిటికీ_లక్షణాలు XGETWINDOWATTRIBUTES
#నిర్వచించు XGet_కిటికీ_Property XGETWINDOWPROPERTY
#నిర్వచించు XGrabKeyboard XGRABKEYBOARD
#నిర్వచించు XGrab_సూచిక XGRABPOINTER
#నిర్వచించు XGrabServer XGRABSERVER
#నిర్వచించు XHeight_యొక్క_Screen XHEIGHTOFSCREEN
#నిర్వచించు XIconify_కిటికీ XICONIFYWINDOW
#నిర్వచించు XIfEvent XIFEVENT
#నిర్వచించు XInternAtom XINTERNATOM
#నిర్వచించు XIntersectRegion XINTERSECTREGION
#నిర్వచించు XKeycode_గమ్యం_Keysym XKEYCODETOKEYSYM
#నిర్వచించు XKeysym_గమ్యం_Keycode XKEYSYMTOKEYCODE
#నిర్వచించు XKeysym_గమ్యం_String XKEYSYMTOSTRING
#నిర్వచించు XListFonts XLISTFONTS
#నిర్వచించు XListFontsWithInfo XLISTFONTSWITHINFO
#నిర్వచించు XListPixmapFormats XLISTPIXMAPFORMATS
#నిర్వచించు XListProperties XLISTPROPERTIES
#నిర్వచించు XLoadQueryFont XLOADQUERYFONT
#నిర్వచించు XLookupString XLOOKUPSTRING
#నిర్వచించు XLower_కిటికీ XLOWERWINDOW
#నిర్వచించు XMapRaised XMAPRAISED
#నిర్వచించు XMap_కిటికీ XMAPWINDOW
#నిర్వచించు XMatchVisualInfo XMATCHVISUALINFO
#నిర్వచించు XMove_Resize_కిటికీ XMOVERESIZEWINDOW
#నిర్వచించు XMove_కిటికీ XMOVEWINDOW
#నిర్వచించు XNextEvent XNEXTEVENT
#నిర్వచించు XOffsetRegion XOFFSETREGION
#నిర్వచించు XOpenDisplay XOPENDISPLAY
#నిర్వచించు XOpenIM XOPENIM
#నిర్వచించు XParseColor XPARSECOLOR
#నిర్వచించు XParseGeometry XPARSEGEOMETRY
#నిర్వచించు XPeekEvent XPEEKEVENT
#నిర్వచించు XPending XPENDING
#నిర్వచించు XPoint_లోపల_Region XPOINTINREGION
#నిర్వచించు XPolygonRegion XPOLYGONREGION
#నిర్వచించు XPut_వెనుక_Event XPUTBACKEVENT
#నిర్వచించు XPutImage XPUTIMAGE
#నిర్వచించు XRoot_కిటికీ XROOTWINDOW
#నిర్వచించు XQueryColor XQUERYCOLOR
#నిర్వచించు XQueryColors XQUERYCOLORS
#నిర్వచించు XQueryExtension XQUERYEXTENSION
#నిర్వచించు XQuery_సూచిక XQUERYPOINTER
#నిర్వచించు XQuery_వృక్షం XQUERYTREE
#నిర్వచించు XRaise_కిటికీ XRAISEWINDOW
#నిర్వచించు XReconfigureWMWindow XRECONFIGUREWMWINDOW
#నిర్వచించు XRect_లోపల_Region XRECTINREGION
#నిర్వచించు XRefreshKeyboardMapping XREFRESHKEYBOARDMAPPING
#నిర్వచించు XReparent_కిటికీ XREPARENTWINDOW
#నిర్వచించు XResize_కిటికీ XRESIZEWINDOW
#నిర్వచించు XRestackWindows XRESTACKWINDOWS
#నిర్వచించు XRoot_కిటికీ_యొక్క_Screen XROOTWINDOWOFSCREEN
#నిర్వచించు XScreen_సంఖ్య_యొక్క_Screen XSCREENNUMBEROFSCREEN
#నిర్వచించు XSelectAsyncEvent XSELECTASYNCEVENT
#నిర్వచించు XSelect_Async_ఆదానం XSELECTASYNCINPUT
#నిర్వచించు XSelect_ఆదానం XSELECTINPUT
#నిర్వచించు XSendEvent XSENDEVENT
#నిర్వచించు XServerVendor XSERVERVENDOR
#నిర్వచించు XSetBackground XSETBACKGROUND
#నిర్వచించు XSetClassHint XSETCLASSHINT
#నిర్వచించు XSet_Clip_ముసుగు XSETCLIPMASK
#నిర్వచించు XSetClipOrigin XSETCLIPORIGIN
#నిర్వచించు XSetClipRectangles XSETCLIPRECTANGLES
#నిర్వచించు XSet_Close_కిందికి_బహుళకం XSETCLOSEDOWNMODE
#నిర్వచించు XSetCommand XSETCOMMAND
#నిర్వచించు XSetDashes XSETDASHES
#నిర్వచించు XSetErrorHandler XSETERRORHANDLER
#నిర్వచించు XSetFillStyle XSETFILLSTYLE
#నిర్వచించు XSetFont XSETFONT
#నిర్వచించు XSetForeground XSETFOREGROUND
#నిర్వచించు XSetFunction XSETFUNCTION
#నిర్వచించు XSetGraphicsExposures XSETGRAPHICSEXPOSURES
#నిర్వచించు XSetICFocus XSETICFOCUS
#నిర్వచించు XSetICValues XSETICVALUES
#నిర్వచించు XSetIOErrorHandler XSETIOERRORHANDLER
#నిర్వచించు XSet_ఆదానం_Focus XSETINPUTFOCUS
#నిర్వచించు XSet_పంక్తి_లక్షణాలు XSETLINEATTRIBUTES
#నిర్వచించు XSetLocaleModifiers XSETLOCALEMODIFIERS
#నిర్వచించు XSet_సాధారణ_Hints XSETNORMALHINTS
#నిర్వచించు XSetRegion XSETREGION
#నిర్వచించు XSetSelectionOwner XSETSELECTIONOWNER
#నిర్వచించు XSetStipple XSETSTIPPLE
#నిర్వచించు XSet_Subwindow_బహుళకం XSETSUBWINDOWMODE
#నిర్వచించు XSetTSOrigin XSETTSORIGIN
#నిర్వచించు XSetTile XSETTILE
#నిర్వచించు XSetTransientForHint XSETTRANSIENTFORHINT
#నిర్వచించు XSetWMColormapWindows XSETWMCOLORMAPWINDOWS
#నిర్వచించు XSetWMHints XSETWMHINTS
#నిర్వచించు XSetWMIconName XSETWMICONNAME
#నిర్వచించు XSetWMName XSETWMNAME
#నిర్వచించు XSetWMNormalHints XSETWMNORMALHINTS
#నిర్వచించు XSetWMProperties XSETWMPROPERTIES
#నిర్వచించు XSetWMProtocols XSETWMPROTOCOLS
#నిర్వచించు XSet_కిటికీ_Background XSETWINDOWBACKGROUND
#నిర్వచించు XSet_కిటికీ_Background_Pixmap XSETWINDOWBACKGROUNDPIXMAP
#నిర్వచించు XSet_కిటికీ_Colormap XSETWINDOWCOLORMAP
#నిర్వచించు XShape_Combine_ముసుగు XSHAPECOMBINEMASK
#నిర్వచించు XShapeCombineRectangles XSHAPECOMBINERECTANGLES
#నిర్వచించు XShapeGetRectangles XSHAPEGETRECTANGLES
#నిర్వచించు XShrinkRegion XSHRINKREGION
#నిర్వచించు XStore_బఫరు XSTOREBUFFER
#నిర్వచించు XStoreColor XSTORECOLOR
#నిర్వచించు XStoreColors XSTORECOLORS
#నిర్వచించు XStoreName XSTORENAME
#నిర్వచించు XString_గమ్యం_Keysym XSTRINGTOKEYSYM
#నిర్వచించు XSubtractRegion XSUBTRACTREGION
#నిర్వచించు XSupportsLocale XSUPPORTSLOCALE
#నిర్వచించు XSync XSYNC
#నిర్వచించు XSynchronize XSYNCHRONIZE
#నిర్వచించు XTextExtents XTEXTEXTENTS
#నిర్వచించు XTextExtents16 XTEXTEXTENTS16
#నిర్వచించు XText_వెడల్పు XTEXTWIDTH
#నిర్వచించు XTextWidth16 XTEXTWIDTH16
#నిర్వచించు XTranslateCoordinates XTRANSLATECOORDINATES
#నిర్వచించు XUndefineCursor XUNDEFINECURSOR
#నిర్వచించు XUngrabKeyboard XUNGRABKEYBOARD
#నిర్వచించు XUngrab_సూచిక XUNGRABPOINTER
#నిర్వచించు XUngrabServer XUNGRABSERVER
#నిర్వచించు XUnionRectWithRegion XUNIONRECTWITHREGION
#నిర్వచించు XUnionRegion XUNIONREGION
#నిర్వచించు XUnmap_కిటికీ XUNMAPWINDOW
#నిర్వచించు _XUnregisterFilter _XUNREGISTERFILTER
#నిర్వచించు XUnsetICFocus XUNSETICFOCUS
#నిర్వచించు XVaCreateNestedList XVACREATENESTEDLIST
#నిర్వచించు XVisualIDFromVisual XVISUALIDFROMVISUAL
#నిర్వచించు XWarp_సూచిక XWARPPOINTER
#నిర్వచించు XWidth_యొక్క_Screen XWIDTHOFSCREEN
#నిర్వచించు XWindowEvent XWINDOWEVENT
#నిర్వచించు XWithdraw_కిటికీ XWITHDRAWWINDOW
#నిర్వచించు XXorRegion XXORREGION
#నిర్వచించు Xm_కూడు_Protocol_Callback XMADDPROTOCOLCALLBACK
#నిర్వచించు Xm_కూడు_Protocols XMADDPROTOCOLS
#నిర్వచించు XmChangeColor XMCHANGECOLOR
#నిర్వచించు XmClipboardCopy XMCLIPBOARDCOPY
#నిర్వచించు Xm_Clipboard_చివర_Copy XMCLIPBOARDENDCOPY
#నిర్వచించు Xm_Clipboard_Inquire_నిడివి XMCLIPBOARDINQUIRELENGTH
#నిర్వచించు Xm_Clipboard_తాళం XMCLIPBOARDLOCK
#నిర్వచించు XmClipboardRetrieve XMCLIPBOARDRETRIEVE
#నిర్వచించు Xm_Clipboard_ప్రారంభం_Copy XMCLIPBOARDSTARTCOPY
#నిర్వచించు XmClipboardUnlock XMCLIPBOARDUNLOCK
#నిర్వచించు XmCreateArrowButton XMCREATEARROWBUTTON
#నిర్వచించు XmCreateArrowButtonGadget XMCREATEARROWBUTTONGADGET
#నిర్వచించు XmCreateCascadeButton XMCREATECASCADEBUTTON
#నిర్వచించు XmCreateDialogShell XMCREATEDIALOGSHELL
#నిర్వచించు XmCreateDragIcon XMCREATEDRAGICON
#నిర్వచించు XmCreateDrawingArea XMCREATEDRAWINGAREA
#నిర్వచించు XmCreateDrawnButton XMCREATEDRAWNBUTTON
#నిర్వచించు XmCreateFileSelectionBox XMCREATEFILESELECTIONBOX
#నిర్వచించు XmCreateFileSelectionDialog XMCREATEFILESELECTIONDIALOG
#నిర్వచించు XmCreateForm XMCREATEFORM
#నిర్వచించు XmCreateFormDialog XMCREATEFORMDIALOG
#నిర్వచించు Xm_Create_చట్రం XMCREATEFRAME
#నిర్వచించు XmCreateInformationDialog XMCREATEINFORMATIONDIALOG
#నిర్వచించు XmCreateLabelGadget XMCREATELABELGADGET
#నిర్వచించు Xm_Create_Main_కిటికీ XMCREATEMAINWINDOW
#నిర్వచించు XmCreateMenuBar XMCREATEMENUBAR
#నిర్వచించు Xm_Create_సందేశం_Box XMCREATEMESSAGEBOX
#నిర్వచించు Xm_Create_సందేశం_Dialog XMCREATEMESSAGEDIALOG
#నిర్వచించు XmCreateOptionMenu XMCREATEOPTIONMENU
#నిర్వచించు Xm_Create_Paned_కిటికీ XMCREATEPANEDWINDOW
#నిర్వచించు XmCreatePopupMenu XMCREATEPOPUPMENU
#నిర్వచించు XmCreatePromptDialog XMCREATEPROMPTDIALOG
#నిర్వచించు XmCreatePulldownMenu XMCREATEPULLDOWNMENU
#నిర్వచించు Xm_Create_నెట్టు_Button XMCREATEPUSHBUTTON
#నిర్వచించు Xm_Create_నెట్టు_Button_Gadget XMCREATEPUSHBUTTONGADGET
#నిర్వచించు XmCreateQuestionDialog XMCREATEQUESTIONDIALOG
#నిర్వచించు XmCreateRadioBox XMCREATERADIOBOX
#నిర్వచించు Xm_Create_అడ్డువరుస_స్తంభం XMCREATEROWCOLUMN
#నిర్వచించు XmCreateScale XMCREATESCALE
#నిర్వచించు XmCreateScrollBar XMCREATESCROLLBAR
#నిర్వచించు XmCreateScrolledList XMCREATESCROLLEDLIST
#నిర్వచించు Xm_Create_Scrolled_పాఠ్యం XMCREATESCROLLEDTEXT
#నిర్వచించు Xm_Create_Scrolled_కిటికీ XMCREATESCROLLEDWINDOW
#నిర్వచించు XmCreateSelectionDialog XMCREATESELECTIONDIALOG
#నిర్వచించు XmCreateSeparator XMCREATESEPARATOR
#నిర్వచించు XmCreateSeparatorGadget XMCREATESEPARATORGADGET
#నిర్వచించు XmCreateTemplateDialog XMCREATETEMPLATEDIALOG
#నిర్వచించు Xm_Create_పాఠ్యం XMCREATETEXT
#నిర్వచించు Xm_Create_పాఠ్యం_క్షేత్రం XMCREATETEXTFIELD
#నిర్వచించు XmCreateToggleButton XMCREATETOGGLEBUTTON
#నిర్వచించు XmCreateToggleButtonGadget XMCREATETOGGLEBUTTONGADGET
#నిర్వచించు Xm_Drag_ప్రారంభం XMDRAGSTART
#నిర్వచించు Xm_Drop_Site_రిజిస్టరు XMDROPSITEREGISTER
#నిర్వచించు XmDropSiteUnregister XMDROPSITEUNREGISTER
#నిర్వచించు XmDropSiteUpdate XMDROPSITEUPDATE
#నిర్వచించు Xm_Drop_Transfer_ప్రారంభం XMDROPTRANSFERSTART
#నిర్వచించు Xm_File_Selection_Box_Get_బిడ్డ XMFILESELECTIONBOXGETCHILD
#నిర్వచించు Xm_File_Selection_Do_శోధన XMFILESELECTIONDOSEARCH
#నిర్వచించు Xm_Font_List_Append_నమోదు XMFONTLISTAPPENDENTRY
#నిర్వచించు XmFontListCopy XMFONTLISTCOPY
#నిర్వచించు XmFontListCreate XMFONTLISTCREATE
#నిర్వచించు Xm_Font_List_నమోదు_Create XMFONTLISTENTRYCREATE
#నిర్వచించు Xm_Font_List_నమోదు_Free XMFONTLISTENTRYFREE
#నిర్వచించు Xm_Font_List_నమోదు_Get_Font XMFONTLISTENTRYGETFONT
#నిర్వచించు Xm_Font_List_నమోదు_Get_ట్యాగ్ XMFONTLISTENTRYGETTAG
#నిర్వచించు Xm_Font_List_నమోదు_ఎక్కించు XMFONTLISTENTRYLOAD
#నిర్వచించు XmFontListFree XMFONTLISTFREE
#నిర్వచించు XmFontListFreeFontContext XMFONTLISTFREEFONTCONTEXT
#నిర్వచించు Xm_Font_List_Get_తదుపరి_Font XMFONTLISTGETNEXTFONT
#నిర్వచించు XmFontListInitFontContext XMFONTLISTINITFONTCONTEXT
#నిర్వచించు Xm_Font_List_తదుపరి_నమోదు XMFONTLISTNEXTENTRY
#నిర్వచించు XmGetColors XMGETCOLORS
#నిర్వచించు XmGetFocusWidget XMGETFOCUSWIDGET
#నిర్వచించు XmGetMenuCursor XMGETMENUCURSOR
#నిర్వచించు Xm_Get_Pixmap_By_లోతు XMGETPIXMAPBYDEPTH
#నిర్వచించు XmGetTearOffControl XMGETTEAROFFCONTROL
#నిర్వచించు XmGetXmDisplay XMGETXMDISPLAY
#నిర్వచించు XmImMbLookupString XMIMMBLOOKUPSTRING
#నిర్వచించు Xm_Im_రిజిస్టరు XMIMREGISTER
#నిర్వచించు Xm_Im_సమితి_Focus_విలువలు XMIMSETFOCUSVALUES
#నిర్వచించు Xm_Im_సమితి_విలువలు XMIMSETVALUES
#నిర్వచించు XmImUnregister XMIMUNREGISTER
#నిర్వచించు XmImUnsetFocus XMIMUNSETFOCUS
#నిర్వచించు XmInternAtom XMINTERNATOM
#నిర్వచించు XmIsMotifWMRunning XMISMOTIFWMRUNNING
#నిర్వచించు Xm_List_కూడు_అంశం XMLISTADDITEM
#నిర్వచించు Xm_List_కూడు_అంశం_Unselected XMLISTADDITEMUNSELECTED
#నిర్వచించు Xm_List_కూడు_వస్తువులు_Unselected XMLISTADDITEMSUNSELECTED
#నిర్వచించు Xm_List_Delete_All_వస్తువులు XMLISTDELETEALLITEMS
#నిర్వచించు Xm_List_Delete_వస్తువులు_Pos XMLISTDELETEITEMSPOS
#నిర్వచించు XmListDeletePos XMLISTDELETEPOS
#నిర్వచించు Xm_List_Deselect_All_వస్తువులు XMLISTDESELECTALLITEMS
#నిర్వచించు XmListDeselectPos XMLISTDESELECTPOS
#నిర్వచించు Xm_List_Get_Kbd_అంశం_Pos XMLISTGETKBDITEMPOS
#నిర్వచించు Xm_List_Get_సరిపోలిక_Pos XMLISTGETMATCHPOS
#నిర్వచించు XmListGetSelectedPos XMLISTGETSELECTEDPOS
#నిర్వచించు XmListPosSelected XMLISTPOSSELECTED
#నిర్వచించు Xm_List_Select_అంశం XMLISTSELECTITEM
#నిర్వచించు XmListSelectPos XMLISTSELECTPOS
#నిర్వచించు Xm_List_సమితి_Bottom_Pos XMLISTSETBOTTOMPOS
#నిర్వచించు Xm_List_సమితి_అంశం XMLISTSETITEM
#నిర్వచించు Xm_List_సమితి_Kbd_అంశం_Pos XMLISTSETKBDITEMPOS
#నిర్వచించు Xm_List_సమితి_Pos XMLISTSETPOS
#నిర్వచించు Xm_Main_కిటికీ_సమితి_Areas XMMAINWINDOWSETAREAS
#నిర్వచించు XmMenuPosition XMMENUPOSITION
#నిర్వచించు Xm_సందేశం_Box_Get_బిడ్డ XMMESSAGEBOXGETCHILD
#నిర్వచించు XmOptionButtonGadget XMOPTIONBUTTONGADGET
#నిర్వచించు XmOptionLabelGadget XMOPTIONLABELGADGET
#నిర్వచించు XmProcessTraversal XMPROCESSTRAVERSAL
#నిర్వచించు XmQmotif XMQMOTIF
#నిర్వచించు XmRemoveProtocolCallback XMREMOVEPROTOCOLCALLBACK
#నిర్వచించు XmRepTypeGetId XMREPTYPEGETID
#నిర్వచించు Xm_Rep_Type_Get_రికార్డు XMREPTYPEGETRECORD
#నిర్వచించు XmRepTypeInstallTearOffModelCon XMREPTYPEINSTALLTEAROFFMODELCON
#నిర్వచించు Xm_Rep_Type_రిజిస్టరు XMREPTYPEREGISTER
#నిర్వచించు Xm_Rep_Type_Valid_విలువ XMREPTYPEVALIDVALUE
#నిర్వచించు Xm_Scroll_Bar_సమితి_విలువలు XMSCROLLBARSETVALUES
#నిర్వచించు Xm_Scrolled_కిటికీ_సమితి_Areas XMSCROLLEDWINDOWSETAREAS
#నిర్వచించు Xm_Selection_Box_Get_బిడ్డ XMSELECTIONBOXGETCHILD
#నిర్వచించు XmStringByteCompare XMSTRINGBYTECOMPARE
#నిర్వచించు XmStringCompare XMSTRINGCOMPARE
#నిర్వచించు XmStringConcat XMSTRINGCONCAT
#నిర్వచించు XmStringCopy XMSTRINGCOPY
#నిర్వచించు XmStringCreate XMSTRINGCREATE
#నిర్వచించు XmStringCreateLocalized XMSTRINGCREATELOCALIZED
#నిర్వచించు XmStringCreateLtoR XMSTRINGCREATELTOR
#నిర్వచించు XmStringCreateSimple XMSTRINGCREATESIMPLE
#నిర్వచించు XmStringDraw XMSTRINGDRAW
#నిర్వచించు XmStringDrawUnderline XMSTRINGDRAWUNDERLINE
#నిర్వచించు XmStringExtent XMSTRINGEXTENT
#నిర్వచించు XmStringFree XMSTRINGFREE
#నిర్వచించు XmStringFreeContext XMSTRINGFREECONTEXT
#నిర్వచించు XmStringGetLtoR XMSTRINGGETLTOR
#నిర్వచించు Xm_String_Get_తదుపరి_Component XMSTRINGGETNEXTCOMPONENT
#నిర్వచించు Xm_String_Get_తదుపరి_Segment XMSTRINGGETNEXTSEGMENT
#నిర్వచించు XmStringInitContext XMSTRINGINITCONTEXT
#నిర్వచించు Xm_String_నిడివి XMSTRINGLENGTH
#నిర్వచించు XmStringLtoRCreate XMSTRINGLTORCREATE
#నిర్వచించు XmStringNConcat XMSTRINGNCONCAT
#నిర్వచించు XmStringSegmentCreate XMSTRINGSEGMENTCREATE
#నిర్వచించు Xm_String_వెడల్పు XMSTRINGWIDTH
#నిర్వచించు Xm_పాఠ్యం_Clear_Selection XMTEXTCLEARSELECTION
#నిర్వచించు Xm_పాఠ్యం_క్షేత్రం_Get_Editable XMTEXTFIELDGETEDITABLE
#నిర్వచించు Xm_పాఠ్యం_క్షేత్రం_Get_Insertion_Position XMTEXTFIELDGETINSERTIONPOSITION
#నిర్వచించు Xm_పాఠ్యం_క్షేత్రం_Get_చివరి_Position XMTEXTFIELDGETLASTPOSITION
#నిర్వచించు Xm_పాఠ్యం_క్షేత్రం_Get_Selection XMTEXTFIELDGETSELECTION
#నిర్వచించు Xm_పాఠ్యం_క్షేత్రం_Get_String XMTEXTFIELDGETSTRING
#నిర్వచించు Xm_పాఠ్యం_క్షేత్రం_చొప్పించు XMTEXTFIELDINSERT
#నిర్వచించు Xm_పాఠ్యం_క్షేత్రం_Remove XMTEXTFIELDREMOVE
#నిర్వచించు Xm_పాఠ్యం_క్షేత్రం_సమితి_Selection XMTEXTFIELDSETSELECTION
#నిర్వచించు Xm_పాఠ్యం_క్షేత్రం_సమితి_String XMTEXTFIELDSETSTRING
#నిర్వచించు Xm_పాఠ్యం_Get_Cursor_Position XMTEXTGETCURSORPOSITION
#నిర్వచించు Xm_పాఠ్యం_Get_Insertion_Position XMTEXTGETINSERTIONPOSITION
#నిర్వచించు Xm_పాఠ్యం_Get_చివరి_Position XMTEXTGETLASTPOSITION
#నిర్వచించు Xm_పాఠ్యం_Get_Max_నిడివి XMTEXTGETMAXLENGTH
#నిర్వచించు Xm_పాఠ్యం_Get_Selection XMTEXTGETSELECTION
#నిర్వచించు Xm_పాఠ్యం_Get_Selection_Position XMTEXTGETSELECTIONPOSITION
#నిర్వచించు Xm_పాఠ్యం_Get_String XMTEXTGETSTRING
#నిర్వచించు Xm_పాఠ్యం_చొప్పించు XMTEXTINSERT
#నిర్వచించు Xm_పాఠ్యం_Remove XMTEXTREMOVE
#నిర్వచించు Xm_పాఠ్యం_Replace XMTEXTREPLACE
#నిర్వచించు Xm_పాఠ్యం_సమితి_Cursor_Position XMTEXTSETCURSORPOSITION
#నిర్వచించు Xm_పాఠ్యం_సమితి_Highlight XMTEXTSETHIGHLIGHT
#నిర్వచించు Xm_పాఠ్యం_సమితి_Insertion_Position XMTEXTSETINSERTIONPOSITION
#నిర్వచించు Xm_పాఠ్యం_సమితి_Selection XMTEXTSETSELECTION
#నిర్వచించు Xm_పాఠ్యం_సమితి_String XMTEXTSETSTRING
#నిర్వచించు XmToggleButtonGadgetGetState XMTOGGLEBUTTONGADGETGETSTATE
#నిర్వచించు Xm_Toggle_Button_Gadget_సమితి_State XMTOGGLEBUTTONGADGETSETSTATE
#నిర్వచించు XmToggleButtonGetState XMTOGGLEBUTTONGETSTATE
#నిర్వచించు Xm_Toggle_Button_సమితి_State XMTOGGLEBUTTONSETSTATE
#నిర్వచించు XmUpdateDisplay XMUPDATEDISPLAY
#నిర్వచించు XmVaCreateSimpleRadioBox XMVACREATESIMPLERADIOBOX
#నిర్వచించు XmbDrawString XMBDRAWSTRING
#నిర్వచించు XmbLookupString XMBLOOKUPSTRING
#నిర్వచించు XmbResetIC XMBRESETIC
#నిర్వచించు Xmb_సమితి_WMProperties XMBSETWMPROPERTIES
#నిర్వచించు Xmb_పాఠ్యం_Escapement XMBTEXTESCAPEMENT
#నిర్వచించు Xmb_పాఠ్యం_Extents XMBTEXTEXTENTS
#నిర్వచించు Xmb_పాఠ్యం_List_గమ్యం_పాఠ్యం_Property XMBTEXTLISTTOTEXTPROPERTY
#నిర్వచించు Xmb_పాఠ్యం_Property_గమ్యం_పాఠ్యం_List XMBTEXTPROPERTYTOTEXTLIST
#నిర్వచించు Xmu_Client_కిటికీ XMUCLIENTWINDOW
#నిర్వచించు Xmu_Print_Default_Error_సందేశం XMUPRINTDEFAULTERRORMESSAGE
#నిర్వచించు XrmGetDatabase XRMGETDATABASE
#నిర్వచించు XrmGetResource XRMGETRESOURCE
#నిర్వచించు XrmPutStringResource XRMPUTSTRINGRESOURCE
#నిర్వచించు Xrm_Quark_గమ్యం_String XRMQUARKTOSTRING
#నిర్వచించు Xrm_String_గమ్యం_Quark XRMSTRINGTOQUARK
#నిర్వచించు Xt_కూడు_Callback XTADDCALLBACK
#నిర్వచించు Xt_కూడు_Callbacks XTADDCALLBACKS
#నిర్వచించు Xt_కూడు_Event_Handler XTADDEVENTHANDLER
#నిర్వచించు Xt_కూడు_Grab XTADDGRAB
#నిర్వచించు XtAllocateGC XTALLOCATEGC
#నిర్వచించు Xt_App_కూడు_Actions XTAPPADDACTIONS
#నిర్వచించు Xt_App_కూడు_ఆదానం XTAPPADDINPUT
#నిర్వచించు Xt_App_కూడు_సమయం_బహిర్గతం XTAPPADDTIMEOUT
#నిర్వచించు XtAppCreateShell XTAPPCREATESHELL
#నిర్వచించు XtAppInitialize XTAPPINITIALIZE
#నిర్వచించు Xt_App_తదుపరి_Event XTAPPNEXTEVENT
#నిర్వచించు XtAppPeekEvent XTAPPPEEKEVENT
#నిర్వచించు XtAppPending XTAPPPENDING
#నిర్వచించు XtAppProcessEvent XTAPPPROCESSEVENT
#నిర్వచించు Xt_App_సమితి_Error_Handler XTAPPSETERRORHANDLER
#నిర్వచించు Xt_App_సమితి_Fallback_Resources XTAPPSETFALLBACKRESOURCES
#నిర్వచించు Xt_App_సమితి_Warning_Handler XTAPPSETWARNINGHANDLER
#నిర్వచించు Xt_App_సమితి_Warning_సందేశం_Handler XTAPPSETWARNINGMSGHANDLER
#నిర్వచించు XtAppWarning XTAPPWARNING
#నిర్వచించు XtCallActionProc XTCALLACTIONPROC
#నిర్వచించు XtCallCallbackList XTCALLCALLBACKLIST
#నిర్వచించు XtCallCallbacks XTCALLCALLBACKS
#నిర్వచించు XtConfigureWidget XTCONFIGUREWIDGET
#నిర్వచించు XtConvertAndStore XTCONVERTANDSTORE
#నిర్వచించు XtCreateApplicationContext XTCREATEAPPLICATIONCONTEXT
#నిర్వచించు XtCreateManagedWidget XTCREATEMANAGEDWIDGET
#నిర్వచించు XtCreatePopupShell XTCREATEPOPUPSHELL
#నిర్వచించు XtCreateWidget XTCREATEWIDGET
#నిర్వచించు XtDatabase XTDATABASE
#నిర్వచించు XtDestroyWidget XTDESTROYWIDGET
#నిర్వచించు XtDisownSelection XTDISOWNSELECTION
#నిర్వచించు XtDispatchEvent XTDISPATCHEVENT
#నిర్వచించు Xt_Display_యొక్క_Object XTDISPLAYOFOBJECT
#నిర్వచించు XtDisplayStringConvWarning XTDISPLAYSTRINGCONVWARNING
#నిర్వచించు Xt_Display_గమ్యం_Application_Context XTDISPLAYTOAPPLICATIONCONTEXT
#నిర్వచించు XtFree XTFREE
#నిర్వచించు XtGetActionKeysym XTGETACTIONKEYSYM
#నిర్వచించు XtGetActionList XTGETACTIONLIST
#నిర్వచించు XtGetApplicationNameAndClass XTGETAPPLICATIONNAMEANDCLASS
#నిర్వచించు XtGetApplicationResources XTGETAPPLICATIONRESOURCES
#నిర్వచించు XtGetGC XTGETGC
#నిర్వచించు Xt_Get_Multi_Click_సమయం XTGETMULTICLICKTIME
#నిర్వచించు Xt_Get_Selection_విలువ XTGETSELECTIONVALUE
#నిర్వచించు Xt_Get_Selection_విలువలు XTGETSELECTIONVALUES
#నిర్వచించు XtGetSubresources XTGETSUBRESOURCES
#నిర్వచించు Xt_Get_విలువలు XTGETVALUES
#నిర్వచించు XtGrabKeyboard XTGRABKEYBOARD
#నిర్వచించు Xt_Grab_సూచిక XTGRABPOINTER
#నిర్వచించు XtHasCallbacks XTHASCALLBACKS
#నిర్వచించు XtInitializeWidgetClass XTINITIALIZEWIDGETCLASS
#నిర్వచించు Xt_చొప్పించు_Event_Handler XTINSERTEVENTHANDLER
#నిర్వచించు XtIsManaged XTISMANAGED
#నిర్వచించు XtIsObject XTISOBJECT
#నిర్వచించు XtIsSensitive XTISSENSITIVE
#నిర్వచించు XtIsSubclass XTISSUBCLASS
#నిర్వచించు Xt_చివరి_Timestamp_Processed XTLASTTIMESTAMPPROCESSED
#నిర్వచించు XtMakeGeometryRequest XTMAKEGEOMETRYREQUEST
#నిర్వచించు XtMakeResizeRequest XTMAKERESIZEREQUEST
#నిర్వచించు XtMalloc XTMALLOC
#నిర్వచించు Xt_Manage_బిడ్డ XTMANAGECHILD
#నిర్వచించు XtManageChildren XTMANAGECHILDREN
#నిర్వచించు Xt_విలీనం_Arg_Lists XTMERGEARGLISTS
#నిర్వచించు XtMoveWidget XTMOVEWIDGET
#నిర్వచించు XtName XTNAME
#నిర్వచించు Xt_Name_గమ్యం_Widget XTNAMETOWIDGET
#నిర్వచించు XtOpenDisplay XTOPENDISPLAY
#నిర్వచించు XtOverrideTranslations XTOVERRIDETRANSLATIONS
#నిర్వచించు XtOwnSelection XTOWNSELECTION
#నిర్వచించు Xt_మాతృ XTPARENT
#నిర్వచించు Xt_Parse_Translation_పట్టిక XTPARSETRANSLATIONTABLE
#నిర్వచించు XtPopdown XTPOPDOWN
#నిర్వచించు XtPopup XTPOPUP
#నిర్వచించు XtQueryGeometry XTQUERYGEOMETRY
#నిర్వచించు XtRealizeWidget XTREALIZEWIDGET
#నిర్వచించు XtRealloc XTREALLOC
#నిర్వచించు XtReleaseGC XTRELEASEGC
#నిర్వచించు XtRemoveAllCallbacks XTREMOVEALLCALLBACKS
#నిర్వచించు XtRemoveCallback XTREMOVECALLBACK
#నిర్వచించు XtRemoveEventHandler XTREMOVEEVENTHANDLER
#నిర్వచించు XtRemoveGrab XTREMOVEGRAB
#నిర్వచించు Xt_Remove_ఆదానం XTREMOVEINPUT
#నిర్వచించు Xt_Remove_సమయం_బహిర్గతం XTREMOVETIMEOUT
#నిర్వచించు XtResizeWidget XTRESIZEWIDGET
#నిర్వచించు XtResolvePathname XTRESOLVEPATHNAME
#నిర్వచించు Xt_సమితి_Keyboard_Focus XTSETKEYBOARDFOCUS
#నిర్వచించు Xt_సమితి_Mapped_When_Managed XTSETMAPPEDWHENMANAGED
#నిర్వచించు Xt_సమితి_Sensitive XTSETSENSITIVE
#నిర్వచించు Xt_సమితి_Type_Converter XTSETTYPECONVERTER
#నిర్వచించు Xt_సమితి_విలువలు XTSETVALUES
#నిర్వచించు Xt_Shell_అక్షరశ్రేణులు XTSHELLSTRINGS
#నిర్వచించు Xt_అక్షరశ్రేణులు XTSTRINGS
#నిర్వచించు XtToolkitInitialize XTTOOLKITINITIALIZE
#నిర్వచించు XtTranslateCoords XTTRANSLATECOORDS
#నిర్వచించు XtTranslateKeycode XTTRANSLATEKEYCODE
#నిర్వచించు XtUngrabKeyboard XTUNGRABKEYBOARD
#నిర్వచించు Xt_Ungrab_సూచిక XTUNGRABPOINTER
#నిర్వచించు Xt_Unmanage_బిడ్డ XTUNMANAGECHILD
#నిర్వచించు XtUnmanageChildren XTUNMANAGECHILDREN
#నిర్వచించు XtUnrealizeWidget XTUNREALIZEWIDGET
#నిర్వచించు XtVaAppCreateShell XTVAAPPCREATESHELL
#నిర్వచించు XtVaCreateManagedWidget XTVACREATEMANAGEDWIDGET
#నిర్వచించు XtVaCreatePopupShell XTVACREATEPOPUPSHELL
#నిర్వచించు XtVaCreateWidget XTVACREATEWIDGET
#నిర్వచించు Xt_Va_Get_విలువలు XTVAGETVALUES
#నిర్వచించు Xt_Va_సమితి_విలువలు XTVASETVALUES
#నిర్వచించు XtWarning XTWARNING
#నిర్వచించు Xt_Widget_గమ్యం_Application_Context XTWIDGETTOAPPLICATIONCONTEXT
#నిర్వచించు Xt_కిటికీ XTWINDOW
#నిర్వచించు Xt_కిటికీ_యొక్క_Object XTWINDOWOFOBJECT
#నిర్వచించు Xt_కిటికీ_గమ్యం_Widget XTWINDOWTOWIDGET
#నిర్వచించు XwcDrawString XWCDRAWSTRING
#నిర్వచించు XwcFreeStringList XWCFREESTRINGLIST
#నిర్వచించు Xwc_పాఠ్యం_Escapement XWCTEXTESCAPEMENT
#నిర్వచించు Xwc_పాఠ్యం_Extents XWCTEXTEXTENTS
#నిర్వచించు Xwc_పాఠ్యం_List_గమ్యం_పాఠ్యం_Property XWCTEXTLISTTOTEXTPROPERTY
#నిర్వచించు Xwc_పాఠ్యం_Property_గమ్యం_పాఠ్యం_List XWCTEXTPROPERTYTOTEXTLIST
#నిర్వచించు _XmBottomShadowColorDefault _XMBOTTOMSHADOWCOLORDEFAULT
#నిర్వచించు _XmClearBorder _XMCLEARBORDER
#నిర్వచించు _XmConfigureObject _XMCONFIGUREOBJECT
#నిర్వచించు Xm_Destroy_మాతృ_Callback _XMDESTROYPARENTCALLBACK
#నిర్వచించు _XmDrawArrow _XMDRAWARROW
#నిర్వచించు _XmDrawShadows _XMDRAWSHADOWS
#నిర్వచించు _XmFontListGetDefaultFont _XMFONTLISTGETDEFAULTFONT
#నిర్వచించు _XmFromHorizontalPixels _XMFROMHORIZONTALPIXELS
#నిర్వచించు _XmFromVerticalPixels _XMFROMVERTICALPIXELS
#నిర్వచించు _XmGetClassExtensionPtr _XMGETCLASSEXTENSIONPTR
#నిర్వచించు _XmGetDefaultFontList _XMGETDEFAULTFONTLIST
#నిర్వచించు _XmGetTextualDragIcon _XMGETTEXTUALDRAGICON
#నిర్వచించు Xm_Get_Widget_Ext_దత్తాంశం _XMGETWIDGETEXTDATA
#నిర్వచించు _XmGrabKeyboard _XMGRABKEYBOARD
#నిర్వచించు Xm_Grab_సూచిక _XMGRABPOINTER
#నిర్వచించు _XmInheritClass _XMINHERITCLASS
#నిర్వచించు Xm_ఆదానం_లోపల_Gadget _XMINPUTINGADGET
#నిర్వచించు _XmMakeGeometryRequest _XMMAKEGEOMETRYREQUEST
#నిర్వచించు Xm_Menu_తీయి_కిందికి _XMMENUPOPDOWN
#నిర్వచించు _XmMoveObject _XMMOVEOBJECT
#నిర్వచించు _XmNavigChangeManaged _XMNAVIGCHANGEMANAGED
#నిర్వచించు _XmOSBuildFileList _XMOSBUILDFILELIST
#నిర్వచించు _XmOSFileCompare _XMOSFILECOMPARE
#నిర్వచించు Xm_OSFind_నమూనా_Part _XMOSFINDPATTERNPART
#నిర్వచించు _XmOSQualifyFileSpec _XMOSQUALIFYFILESPEC
#నిర్వచించు _XmPostPopupMenu _XMPOSTPOPUPMENU
#నిర్వచించు _XmPrimitiveEnter _XMPRIMITIVEENTER
#నిర్వచించు _XmPrimitiveLeave _XMPRIMITIVELEAVE
#నిర్వచించు _XmRedisplayGadgets _XMREDISPLAYGADGETS
#నిర్వచించు _XmShellIsExclusive _XMSHELLISEXCLUSIVE
#నిర్వచించు _XmStringDraw _XMSTRINGDRAW
#నిర్వచించు Xm_String_Get_పాఠ్యం_Concat _XMSTRINGGETTEXTCONCAT
#నిర్వచించు Xm_అక్షరశ్రేణులు _XMSTRINGS
#నిర్వచించు Xm_గమ్యం_Horizontal_Pixels _XMTOHORIZONTALPIXELS
#నిర్వచించు Xm_గమ్యం_Vertical_Pixels _XMTOVERTICALPIXELS
#నిర్వచించు Xm_శిఖరం_Shadow_Color_Default _XMTOPSHADOWCOLORDEFAULT
#నిర్వచించు _Xm_fastPtr _XM_FASTPTR
#నిర్వచించు _XtCheckSubclassFlag _XTCHECKSUBCLASSFLAG
#నిర్వచించు _XtInherit _XTINHERIT
#నిర్వచించు _XtInheritTranslations _XTINHERITTRANSLATIONS
#నిర్వచించు applicationShellWidgetClass APPLICATIONSHELLWIDGETCLASS
#నిర్వచించు సంయుక్త_Widget_Class COMPOSITEWIDGETCLASS
#నిర్వచించు overrideShellWidgetClass OVERRIDESHELLWIDGETCLASS
#నిర్వచించు shellWidgetClass SHELLWIDGETCLASS
#నిర్వచించు శిఖరం_Level_Shell_Class_Rec TOPLEVELSHELLCLASSREC
#నిర్వచించు శిఖరం_Level_Shell_Widget_Class TOPLEVELSHELLWIDGETCLASS
#నిర్వచించు transientShellWidgetClass TRANSIENTSHELLWIDGETCLASS
#నిర్వచించు vendorShellClassRec VENDORSHELLCLASSREC
#నిర్వచించు vendorShellWidgetClass VENDORSHELLWIDGETCLASS
#నిర్వచించు wmShellWidgetClass WMSHELLWIDGETCLASS
#నిర్వచించు xmArrowButtonWidgetClass XMARROWBUTTONWIDGETCLASS
#నిర్వచించు xmCascadeButtonClassRec XMCASCADEBUTTONCLASSREC
#నిర్వచించు xmCascadeButtonGadgetClass XMCASCADEBUTTONGADGETCLASS
#నిర్వచించు xmCascadeButtonWidgetClass XMCASCADEBUTTONWIDGETCLASS
#నిర్వచించు xmDialogShellWidgetClass XMDIALOGSHELLWIDGETCLASS
#నిర్వచించు xmDrawingAreaWidgetClass XMDRAWINGAREAWIDGETCLASS
#నిర్వచించు xmDrawnButtonWidgetClass XMDRAWNBUTTONWIDGETCLASS
#నిర్వచించు xmFileSelectionBoxWidgetClass XMFILESELECTIONBOXWIDGETCLASS
#నిర్వచించు xmFormWidgetClass XMFORMWIDGETCLASS
#నిర్వచించు xm_చట్రం_Widget_Class XMFRAMEWIDGETCLASS
#నిర్వచించు xmGadgetClass XMGADGETCLASS
#నిర్వచించు xmLabelGadgetClass XMLABELGADGETCLASS
#నిర్వచించు xmLabelWidgetClass XMLABELWIDGETCLASS
#నిర్వచించు xmListWidgetClass XMLISTWIDGETCLASS
#నిర్వచించు xm_Main_కిటికీ_Widget_Class XMMAINWINDOWWIDGETCLASS
#నిర్వచించు xmManagerClassRec XMMANAGERCLASSREC
#నిర్వచించు xmManagerWidgetClass XMMANAGERWIDGETCLASS
#నిర్వచించు xmMenuShellWidgetClass XMMENUSHELLWIDGETCLASS
#నిర్వచించు xm_సందేశం_Box_Widget_Class XMMESSAGEBOXWIDGETCLASS
#నిర్వచించు xmPrimitiveClassRec XMPRIMITIVECLASSREC
#నిర్వచించు xmPrimitiveWidgetClass XMPRIMITIVEWIDGETCLASS
#నిర్వచించు xm_నెట్టు_Button_Class_Rec XMPUSHBUTTONCLASSREC
#నిర్వచించు xm_నెట్టు_Button_Gadget_Class XMPUSHBUTTONGADGETCLASS
#నిర్వచించు xm_నెట్టు_Button_Widget_Class XMPUSHBUTTONWIDGETCLASS
#నిర్వచించు xm_అడ్డువరుస_స్తంభం_Widget_Class XMROWCOLUMNWIDGETCLASS
#నిర్వచించు xmSashWidgetClass XMSASHWIDGETCLASS
#నిర్వచించు xmScrollBarWidgetClass XMSCROLLBARWIDGETCLASS
#నిర్వచించు xm_Scrolled_కిటికీ_Class_Rec XMSCROLLEDWINDOWCLASSREC
#నిర్వచించు xm_Scrolled_కిటికీ_Widget_Class XMSCROLLEDWINDOWWIDGETCLASS
#నిర్వచించు xmSeparatorGadgetClass XMSEPARATORGADGETCLASS
#నిర్వచించు xmSeparatorWidgetClass XMSEPARATORWIDGETCLASS
#నిర్వచించు xm_పాఠ్యం_క్షేత్రం_Widget_Class XMTEXTFIELDWIDGETCLASS
#నిర్వచించు xm_పాఠ్యం_Widget_Class XMTEXTWIDGETCLASS
#నిర్వచించు xmToggleButtonGadgetClass XMTOGGLEBUTTONGADGETCLASS
#నిర్వచించు xmToggleButtonWidgetClass XMTOGGLEBUTTONWIDGETCLASS

/*
** XtRegisterDrawable and XtUnregisterDrawable don't exist until R6.
** So we have to fake it for R5 and earlier. It would be real nice to
** include X11.h here and then test for R6 by checking
** XlibSpecificationRelease. But including X11.h defines the symbol "None"
** and there's Mozilla code in gfx/src/gtk that defines an enum element
** named None. So for now hard code this is R5....
*/
#నిర్వచితం_కాకపోతే VMS_X11R6
// R5 or earlier
#నిర్వచించు Xt_రిజిస్టరు_కిటికీ _XTREGISTERWINDOW
#నిర్వచించు Xt_Unregister_కిటికీ _XTUNREGISTERWINDOW
#లేకపోతే
// R6 or later
#నిర్వచించు Xt_రిజిస్టరు_Drawable XTREGISTERDRAWABLE
#నిర్వచించు XtUnregisterDrawable XTUNREGISTERDRAWABLE
#అయితే_ముగింపు
