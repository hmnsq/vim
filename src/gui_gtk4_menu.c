/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_MENU

#అన్తర్భావ <gtk/gtk.h>
#అన్తర్భావ "gui_gtk4_menu.h"

// Note that this may return NULL for popup menus
#నిర్వచించు GET_MENU_BAR(m) VIM_MENU_BAR(gtk_widget_get_ancestor( \
	    GTK_WIDGET(m), VIM_TYPE_MENU_BAR))

/*
 * Similar as GtkButton but set CSS name to "item" to emulate GtkPopoverMenuBar
 * styling. Always has a submenu.
 */
నిర్మాణం Vim_Menu_Bar_అంశం
{
    GtkButton మాతృ;

    GtkWidget *menu;
};

G_DEFINE_TYPE(Vim_Menu_Bar_అంశం_2, vim_menu_bar_అంశం, GTK_TYPE_BUTTON)

    స్థిర శూన్య
vim_menu_bar_అంశం_dispose(GObject *object)
{
    Vim_Menu_Bar_అంశం_2 *self = VIM_MENU_BAR_అంశం(object);

    g_clear_సూచిక((GtkWidget **)&self->menu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_అంశం_మాతృ_class)->dispose(object);
}

    స్థిర శూన్య
vim_menu_bar_అంశం_class_init(Vim_Menu_Bar_అంశం_Class *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_అంశం_dispose;

    gtk_widget_class_సమితి_css_name(widget_class, "item");
}

    స్థిర శూన్య
vim_menu_bar_అంశం_init(Vim_Menu_Bar_అంశం_2 *self)
{
    // Enable mnemonics
    gtk_button_సమితి_use_underline(GTK_BUTTON(self), TRUE);
}

    GtkWidget *
vim_menu_bar_అంశం_new(స్థిరరాశి అక్షరం *పాఠ్యం, VimMenu *menu)
{
    Vim_Menu_Bar_అంశం_2 *అంశం = g_object_new(VIM_TYPE_MENU_BAR_అంశం, ఖాళీ);

    gtk_button_సమితి_label(GTK_BUTTON(అంశం), పాఠ్యం);

    అంశం->menu = GTK_WIDGET(menu);
    gtk_popover_సమితి_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    // Make popover start at top left corner
    gtk_widget_సమితి_halign(GTK_WIDGET(menu), GTK_ALIGN_ప్రారంభం);
    gtk_widget_సమితి_మాతృ(GTK_WIDGET(menu), GTK_WIDGET(అంశం));

    తిరిగి GTK_WIDGET(అంశం);
}

    శూన్య
vim_menu_bar_అంశం_సమితి_పాఠ్యం(Vim_Menu_Bar_అంశం_2 *self, స్థిరరాశి అక్షరం *పాఠ్యం)
{
    gtk_button_సమితి_label(GTK_BUTTON(self), పాఠ్యం);
}

/*
 * Similar to GtkPopoverMenuBar
 */
నిర్మాణం _VimMenuBar
{
    GtkWidget మాతృ;

    GList *వస్తువులు;

    // Currently visible item that has submenu popped up, else NULL
    GtkWidget *చురుకైన_అంశం;
};

G_DEFINE_TYPE(VimMenuBar, vim_menu_bar, GTK_TYPE_WIDGET)

    స్థిర శూన్య
vim_menu_bar_dispose(GObject *object)
{
    VimMenuBar *self = VIM_MENU_BAR(object);

    g_clear_list(&self->వస్తువులు, (GDestroyNotify)gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_మాతృ_class)->dispose(object);
}

    స్థిర శూన్య
vim_menu_bar_class_init(VimMenuBarClass *class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_dispose;

    gtk_widget_class_సమితి_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_సమితి_css_name(widget_class, "menubar");
}

    స్థిర శూన్య
vim_menu_bar_init(VimMenuBar *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_సమితి_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_సమితి_spacing(GTK_BOX_LAYOUT(lm), 0);
}

    GtkWidget *
vim_menu_bar_new(శూన్య)
{
    తిరిగి g_object_new(VIM_TYPE_MENU_BAR, ఖాళీ);
}

/*
 * Create a VimMenu widget with the menus of the menu bar as its submenus. Note
 * that it is a deep copy.
 */
    GtkWidget *
vim_menu_bar_గమ్యం_menu(VimMenuBar *self)
{
    GtkWidget	*menu = vim_menu_new();
    పూర్ణాంకం		i = 0;

    కొరకు (GList *l = self->వస్తువులు; l != ఖాళీ; l = l->తదుపరి, i++)
    {
	Vim_Menu_Bar_అంశం_2	*baritem = l->దత్తాంశం;
	GtkWidget	*అంశం;

	అంశం = vim_menu_అంశం_new(
		gtk_button_get_label(GTK_BUTTON(baritem)), ఖాళీ, ఖాళీ);

	vim_menu_అంశం_సమితి_submenu(VIM_MENU_అంశం(అంశం),
		VIM_MENU(vim_menu_copy(VIM_MENU(baritem->menu))));

	vim_menu_చొప్పించు_అంశం(VIM_MENU(menu), VIM_MENU_అంశం(అంశం), i);
    }
    తిరిగి menu;
}

/*
 * Set the currently active menu of the menubar to "item". If NULL, then close
 * any submenus.
 */
    స్థిర శూన్య
vim_menu_bar_సమితి_చురుకైన_అంశం(
	VimMenuBar	*self,
	Vim_Menu_Bar_అంశం_2	*అంశం,
	gboolean	force)
{
    // Do nothing if currently active item is "item", or if there is not
    // currently active item. User must click a menu item first for menus to
    // automatically appear on hover. This is unless "force" is TRUE.
    //
    // Only make item selected if there is no active item (no submenu open), or
    // if the item was set as the active item..
    ఒకవేళ ((!force && self->చురుకైన_అంశం == ఖాళీ)
	    || self->చురుకైన_అంశం == GTK_WIDGET(అంశం))
    {
	ఒకవేళ (self->చురుకైన_అంశం == ఖాళీ)
	    gtk_widget_సమితి_state_flags(GTK_WIDGET(అంశం),
		    GTK_STATE_FLAG_SELECTED, FALSE);
	తిరిగి;
    }

    ఒకవేళ (self->చురుకైన_అంశం != ఖాళీ)
    {
	// Call this before popdown, since "closed" signal may be emitted
	// immediately.
	gtk_widget_unset_state_flags(self->చురుకైన_అంశం,
		GTK_STATE_FLAG_SELECTED);
	gtk_popover_popdown(GTK_POPOVER(
		    VIM_MENU_BAR_అంశం(self->చురుకైన_అంశం)->menu)
		);
    }

    self->చురుకైన_అంశం = GTK_WIDGET(అంశం);
    ఒకవేళ (అంశం != ఖాళీ)
    {
	gtk_popover_popup(GTK_POPOVER(అంశం->menu));
	gtk_widget_సమితి_state_flags(GTK_WIDGET(అంశం),
		GTK_STATE_FLAG_SELECTED, FALSE);
    }
}

    స్థిర శూన్య
vim_menu_bar_అంశం_enter_cb(
	GtkEventController  *controller,
	ద్విగుణ		    x వాడని,
	ద్విగుణ		    y వాడని,
	VimMenuBar	    *menubar)
{
    Vim_Menu_Bar_అంశం_2  *self;

    self = VIM_MENU_BAR_అంశం(gtk_event_controller_get_widget(controller));
    vim_menu_bar_సమితి_చురుకైన_అంశం(menubar, self, FALSE);
}

    స్థిర శూన్య
vim_menu_bar_అంశం_leave_cb(
	GtkEventController  *controller,
	VimMenuBar	    *menubar)
{
    Vim_Menu_Bar_అంశం_2 *self;

    self = VIM_MENU_BAR_అంశం(gtk_event_controller_get_widget(controller));

    // If the item is the currently active item, then don't deselect it.
    ఒకవేళ (menubar->చురుకైన_అంశం != GTK_WIDGET(self))
	gtk_widget_unset_state_flags(GTK_WIDGET(self),
		GTK_STATE_FLAG_SELECTED);
}

    స్థిర శూన్య
vim_menu_bar_అంశం_clicked_cb(Vim_Menu_Bar_అంశం_2 *self, VimMenuBar *menubar)
{
    vim_menu_bar_సమితి_చురుకైన_అంశం(menubar, self, TRUE);
}

    స్థిర శూన్య
vim_menu_bar_అంశం_menu_closed_cb(VimMenu *menu వాడని, VimMenuBar *menubar)
{
    ఒకవేళ (menubar->చురుకైన_అంశం != ఖాళీ)
	gtk_widget_unset_state_flags(GTK_WIDGET(menubar->చురుకైన_అంశం),
		GTK_STATE_FLAG_SELECTED);
    vim_menu_bar_సమితి_చురుకైన_అంశం(menubar, ఖాళీ, TRUE);
    // Make sure to focus drawarea
    gtk_widget_grab_focus(gui.drawarea);
}

/*
 * Insert the menu item at the given index in the menu bar.
 */
    శూన్య
vim_menu_bar_చొప్పించు_అంశం(VimMenuBar *self, Vim_Menu_Bar_అంశం_2 *అంశం, పూర్ణాంకం idx)
{
    GtkEventController	*controller;
    GList		*తదుపరి_sibling;

    తదుపరి_sibling = g_list_nth(self->వస్తువులు, idx);
    gtk_widget_చొప్పించు_before(GTK_WIDGET(అంశం), GTK_WIDGET(self),
	    తదుపరి_sibling == ఖాళీ ? ఖాళీ : తదుపరి_sibling->దత్తాంశం);

    self->వస్తువులు = g_list_చొప్పించు(self->వస్తువులు, అంశం, idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_bar_అంశం_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_bar_అంశం_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_కూడు_controller(GTK_WIDGET(అంశం), controller);

    g_signal_connect_object(అంశం, "clicked",
	    G_CALLBACK(vim_menu_bar_అంశం_clicked_cb), self, G_CONNECT_DEFAULT);

    g_signal_connect_object(అంశం->menu, "closed",
	    G_CALLBACK(vim_menu_bar_అంశం_menu_closed_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Remove the menu item or separator from the menu bar
 */
    శూన్య
vim_menu_bar_remove(VimMenuBar *self, GtkWidget *అంశం)
{
    self->వస్తువులు = g_list_remove(self->వస్తువులు, అంశం);
    gtk_widget_unparent(అంశం);
}

/*
 * Show the given menu in the menubar. If "item" is NULL, then show first menu.
 */
    శూన్య
vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_అంశం_2 *అంశం)
{
    ఒకవేళ (అంశం == ఖాళీ)
	అంశం = g_list_nth_దత్తాంశం(self->వస్తువులు, 0);

    vim_menu_bar_సమితి_చురుకైన_అంశం(self, అంశం, TRUE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    స్థిర GtkWidget *
vim_menu_bar_move_చురుకైన_అంశం(VimMenuBar *self, పూర్ణాంకం dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->చురుకైన_అంశం;

    ఒకవేళ (widget == ఖాళీ)
	తిరిగి gtk_widget_get_మొదటి_బిడ్డ(GTK_WIDGET(self));

    ఒకవేళ (dir > 0)
    {
	func = gtk_widget_get_తదుపరి_sibling;
	null_func = gtk_widget_get_మొదటి_బిడ్డ;
    }
    లేకపోతే
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_చివరి_బిడ్డ;
    }

    అయితే (TRUE)
    {
	widget = func(widget);
	ఒకవేళ (widget == ఖాళీ)
	{
	    ఒకవేళ (null_func == ఖాళీ)
		ఆపు;
	    widget = null_func(GTK_WIDGET(self));
	    null_func = ఖాళీ;
	    ఒకవేళ (widget == ఖాళీ)
		ఆపు;
	}
	ఆపు;
    }
    తిరిగి widget;
}

/*
 * Menu button that can be used to perform actions, or if there is a submenu,
 * toggle the state of the submenu popover. CSS name is "modelbutton" to make it
 * styled like GtkPopoverMenu
 */
నిర్మాణం Vim_Menu_అంశం_2
{
    GtkButton మాతృ;

    GtkWidget *label;	    // Displays text for button.
    GtkWidget *aux_widget;  // Either an icon or a label showing the accelerator
			    // text.

    GtkWidget *submenu;	    // Submenu popover if any (VimMenu)

    // Callback called when clicked or selected, we store this so that copying a
    // menu item works properly.
    Vim_Menu_అంశం_Func	func;
    శూన్య		*func_udata;
};

G_DEFINE_TYPE(Vim_Menu_అంశం, vim_menu_అంశం, GTK_TYPE_BUTTON)

    స్థిర శూన్య
vim_menu_అంశం_dispose(GObject *object)
{
    Vim_Menu_అంశం *self = VIM_MENU_అంశం(object);

    g_clear_సూచిక(&self->label, gtk_widget_unparent);
    g_clear_సూచిక(&self->aux_widget, gtk_widget_unparent);
    g_clear_సూచిక((GtkWidget **)&self->submenu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_అంశం_మాతృ_class)->dispose(object);
}

    స్థిర శూన్య
vim_menu_అంశం_class_init(Vim_Menu_అంశం_Class *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_అంశం_dispose;

    gtk_widget_class_సమితి_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_సమితి_css_name(widget_class, "modelbutton");
}

    స్థిర శూన్య
vim_menu_అంశం_init(Vim_Menu_అంశం *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_సమితి_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_సమితి_spacing(GTK_BOX_LAYOUT(lm), 0);
}

/*
 * Create a new menu item with the given text to display. "func" may be NULL if
 * not needed.
 */
    GtkWidget *
vim_menu_అంశం_new(స్థిరరాశి అక్షరం *పాఠ్యం, Vim_Menu_అంశం_Func func, శూన్య *udata)
{
    Vim_Menu_అంశం *అంశం = g_object_new(VIM_TYPE_MENU_అంశం, ఖాళీ);

    అంశం->func = func;
    అంశం->func_udata = udata;
    అంశం->label = gtk_label_new_with_mnemonic(పాఠ్యం);

    // Make sure label is on the right and pushes everything to the left
    gtk_widget_సమితి_halign(అంశం->label, GTK_ALIGN_ప్రారంభం);
    gtk_widget_సమితి_hexpand(అంశం->label, TRUE);
    gtk_widget_సమితి_మాతృ(అంశం->label, GTK_WIDGET(అంశం));

    తిరిగి GTK_WIDGET(అంశం);
}

/*
 * Update displayed text for menu item
 */
    శూన్య
vim_menu_అంశం_సమితి_పాఠ్యం(Vim_Menu_అంశం *self, స్థిరరాశి అక్షరం *పాఠ్యం)
{
    gtk_label_సమితి_పాఠ్యం_with_mnemonic(GTK_LABEL(self->label), పాఠ్యం);
}

    స్థిర శూన్య
vim_menu_అంశం_సమితి_aux_widget(Vim_Menu_అంశం *self, GtkWidget *aux)
{
    self->aux_widget = aux;
    gtk_widget_సమితి_halign(self->aux_widget, GTK_ALIGN_చివర);
    gtk_widget_సమితి_hexpand(self->aux_widget, FALSE);
    gtk_widget_సమితి_margin_ప్రారంభం(self->aux_widget, 50);
}

/*
 * Set the accelerator text for the menu item.
 */
    శూన్య
vim_menu_అంశం_సమితి_accel(Vim_Menu_అంశం *self, స్థిరరాశి అక్షరం *accel_పాఠ్యం)
{
    ప్రతిజ్ఞా(self->aux_widget == ఖాళీ);

    vim_menu_అంశం_సమితి_aux_widget(self, gtk_label_new(accel_పాఠ్యం));
    gtk_widget_చొప్పించు_after(self->aux_widget, GTK_WIDGET(self), self->label);
}

/*
 * Set the submenu popover for the menu item
 */
    శూన్య
vim_menu_అంశం_సమితి_submenu(Vim_Menu_అంశం *self, VimMenu *submenu)
{
    GtkWidget *icon;

    ప్రతిజ్ఞా(self->submenu == ఖాళీ);
    ప్రతిజ్ఞా(self->aux_widget == ఖాళీ);

    // Add arrow icon pointing to right
    icon = gtk_image_new_from_icon_name("pan-end-symbolic");
    vim_menu_అంశం_సమితి_aux_widget(self, icon);
    gtk_widget_చొప్పించు_after(self->aux_widget, GTK_WIDGET(self), self->label);

    gtk_popover_సమితి_position(GTK_POPOVER(submenu), GTK_POS_కుడి);
    // Make top of popover be aligned with button.
    gtk_widget_సమితి_valign(GTK_WIDGET(submenu), GTK_ALIGN_ప్రారంభం);

    self->submenu = GTK_WIDGET(submenu);
    gtk_widget_సమితి_మాతృ(GTK_WIDGET(submenu), GTK_WIDGET(self));
}

/*
 * Create a deep copy of the menu item
 */
    స్థిర GtkWidget *
vim_menu_అంశం_copy(Vim_Menu_అంశం *self)
{
    GtkWidget *copy;

    copy = vim_menu_అంశం_new(
	    gtk_label_get_పాఠ్యం(GTK_LABEL(self->label)),
	    self->func, self->func_udata);

    ఒకవేళ (self->submenu != ఖాళీ)
	vim_menu_అంశం_సమితి_submenu(VIM_MENU_అంశం(copy),
		VIM_MENU(vim_menu_copy(VIM_MENU(self->submenu))));
    లేకపోతే ఒకవేళ (self->aux_widget != ఖాళీ)
	vim_menu_అంశం_సమితి_accel(VIM_MENU_అంశం(copy),
		gtk_label_get_పాఠ్యం(GTK_LABEL(self->aux_widget)));
    తిరిగి copy;
}

/*
 * Similar to GtkPopoverMenu, except uses GtkWidgets directly like GTK3, instead
 * of abstracting it into GMenuModel.
 */
నిర్మాణం _VimMenu
{
    GtkPopover మాతృ;

    GtkWidget *box;
    GtkWidget *scr;

    GList *వస్తువులు;

    // Currently active item showing submenu popover, or being hovered on, or
    // NULL. Note that item may have submenu but not have it open, when
    // navigating via keyboard.
    GtkWidget *చురుకైన_అంశం;

    // Used when mouse is hovering over an item and user is navigating with
    // keyboard. When the scrolled window scrolls down or up, this causes a
    // mouse enter event, causing the active item to go to the item that the
    // mouse is hovered on, instead of the next item (from keyboard navigation).
    gboolean	ignore_hover;
    ద్విగుణ	prev_x;
    ద్విగుణ	prev_y;
};

G_DEFINE_TYPE(VimMenu, vim_menu, GTK_TYPE_POPOVER)

    స్థిర శూన్య
vim_menu_dispose(GObject *object)
{
    VimMenu *self = VIM_MENU(object);

    g_clear_list(&self->వస్తువులు, (GDestroyNotify)gtk_widget_unparent);
    g_clear_సూచిక(&self->scr, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_మాతృ_class)->dispose(object);
}

    స్థిర శూన్య
vim_menu_class_init(VimMenuClass *class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_dispose;
}

    స్థిర gboolean
vim_menu_select_చురుకైన_అంశం(VimMenu *self, gboolean open)
{
    Vim_Menu_అంశం *అంశం;

    gtk_widget_సమితి_state_flags(self->చురుకైన_అంశం,
	    GTK_STATE_FLAG_SELECTED, FALSE);

    // Make sure to focus item, so that scrolled window knows what to do.
    gtk_widget_grab_focus(GTK_WIDGET(self->చురుకైన_అంశం));

    అంశం = VIM_MENU_అంశం(self->చురుకైన_అంశం);
    ఒకవేళ (అంశం->func != ఖాళీ)
	అంశం->func(అంశం, VIM_MENU_అంశం_SELECTED, అంశం->func_udata);

    ఒకవేళ (open && VIM_MENU_అంశం(self->చురుకైన_అంశం)->submenu != ఖాళీ)
    {
	GtkWidget *submenu = VIM_MENU_అంశం(self->చురుకైన_అంశం)->submenu;
	gtk_popover_popup(GTK_POPOVER(submenu));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Set the active item of the menu to "item". If "item" is NULL, then close any
 * submenus. If "open" is FALSE, then don't open the submenu if any.
 */
    స్థిర శూన్య
vim_menu_సమితి_చురుకైన_అంశం(VimMenu *self, Vim_Menu_అంశం *అంశం, gboolean open)
{
    ఒకవేళ (self->చురుకైన_అంశం == GTK_WIDGET(అంశం))
	తిరిగి;

    ఒకవేళ (self->చురుకైన_అంశం != ఖాళీ)
    {
	ఒకవేళ (VIM_MENU_అంశం(self->చురుకైన_అంశం)->submenu != ఖాళీ)
	    gtk_popover_popdown(GTK_POPOVER(
			VIM_MENU_అంశం(self->చురుకైన_అంశం)->submenu
			));
	gtk_widget_unset_state_flags(GTK_WIDGET(self->చురుకైన_అంశం),
		GTK_STATE_FLAG_SELECTED);
    }

    self->చురుకైన_అంశం = GTK_WIDGET(అంశం);
    ఒకవేళ (అంశం != ఖాళీ)
	(శూన్య)vim_menu_select_చురుకైన_అంశం(self, open);
}

    స్థిర శూన్య
vim_menu_closed_cb(VimMenu *self, శూన్య *udata వాడని)
{
    vim_menu_సమితి_చురుకైన_అంశం(self, ఖాళీ, FALSE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    స్థిర GtkWidget *
vim_menu_move_చురుకైన_అంశం(VimMenu *self, పూర్ణాంకం dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->చురుకైన_అంశం;

    // If there is no currently active item, then just use the first one
    ఒకవేళ (widget == ఖాళీ)
    {
	widget = gtk_widget_get_మొదటి_బిడ్డ(self->box);
	అయితే (widget != ఖాళీ && !VIM_IS_MENU_అంశం(widget))
	    widget = gtk_widget_get_తదుపరి_sibling(widget);
	తిరిగి widget;
    }

    // Could also just use GList functions, but this seems simpler (no
    // difference anyways).
    ఒకవేళ (dir > 0)
    {
	func = gtk_widget_get_తదుపరి_sibling;
	null_func = gtk_widget_get_మొదటి_బిడ్డ;
    }
    లేకపోతే
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_చివరి_బిడ్డ;
    }

    అయితే (TRUE)
    {
	widget = func(widget);
	ఒకవేళ (widget == ఖాళీ)
	{
	    ఒకవేళ (null_func == ఖాళీ)
		ఆపు;
	    widget = null_func(self->box);
	    null_func = ఖాళీ;
	    ఒకవేళ (widget == ఖాళీ)
		ఆపు;
	}
	ఒకవేళ (VIM_IS_MENU_అంశం(widget))
	    ఆపు;
    }
    తిరిగి widget;
}

    స్థిర శూన్య
vim_menu_reset_మాతృ_prelight(VimMenu *self)
{
    GtkWidget	*మాతృ = gtk_widget_get_మాతృ(GTK_WIDGET(self));
    VimMenu	*మాతృ_menu;

    // gtk_widget_get_ancestor assumes the widget itself is also an ancestor, so
    // we must get parent of menu first.
    మాతృ_menu = VIM_MENU(gtk_widget_get_ancestor(మాతృ, VIM_TYPE_MENU));

    ఒకవేళ (మాతృ_menu == ఖాళీ)
	// TRUE for popup menus
	తిరిగి;

    ఒకవేళ (మాతృ_menu->చురుకైన_అంశం != ఖాళీ)
	gtk_widget_unset_state_flags(GTK_WIDGET(మాతృ_menu->చురుకైన_అంశం),
		GTK_STATE_FLAG_PRELIGHT);
}

/*
 * Close all submenus in the menubar given a menu widget
 */
    స్థిర శూన్య
vim_menu_close_all(VimMenu *self)
{
    VimMenuBar *menubar = GET_MENU_BAR(self);

    // Must check if NULL, because popup menus don't have a parent.
    ఒకవేళ (menubar != ఖాళీ)
	vim_menu_bar_సమితి_చురుకైన_అంశం(menubar, ఖాళీ, TRUE);
    లేకపోతే
	gtk_popover_popdown(GTK_POPOVER(self));

    // Grab focus after popup menus without a menubar are closed
    gtk_widget_grab_focus(gui.drawarea);
}

    స్థిర gboolean
vim_menu_కీలకం_pressed_cb(
	GtkEventController  *controller వాడని,
	guint		    keyval,
	guint		    keycode వాడని,
	GdkModifierType	    state,
	VimMenu		    *self)
{
    GtkWidget	*widget;

    ఎంపిక (keyval)
    {
	స్థితి GDK_కీలకం_కిందికి:
	స్థితి GDK_కీలకం_KP_కిందికి:
	స్థితి GDK_కీలకం_పైకి:
	స్థితి GDK_కీలకం_KP_పైకి:
	స్థితి GDK_కీలకం_Tab:
	స్థితి GDK_కీలకం_KP_Tab:
	స్థితి GDK_కీలకం_ISO_ఎడమ_Tab:
	    // Go to the previous or next item if any
	    widget = vim_menu_move_చురుకైన_అంశం(self,
		    (state & GDK_జరుపు_ముసుగు)
		    || keyval == GDK_కీలకం_పైకి ? -1 : 1);
	    vim_menu_సమితి_చురుకైన_అంశం(self, VIM_MENU_అంశం(widget), FALSE);
	    self->ignore_hover = TRUE;
	    తిరిగి TRUE;
	స్థితి GDK_కీలకం_ఎడమ:
	    // Pressing control switches menu bar item.
	    ఒకవేళ (state & GDK_CONTROL_ముసుగు)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		ఒకవేళ (menubar != ఖాళీ)
		{
		    widget = vim_menu_bar_move_చురుకైన_అంశం(menubar, -1);
		    vim_menu_bar_సమితి_చురుకైన_అంశం(menubar,
			    VIM_MENU_BAR_అంశం(widget), TRUE);
		}
		తిరిగి TRUE;
	    }
	    // Go to parent menu (if any). We can do this by just closing the
	    // popover.
	    gtk_popover_popdown(GTK_POPOVER(self));
	    // For some reason when pointer is hovered over draw area, the
	    // active item in the parent menu will stay prelighted even when the
	    // active item is moved.
	    vim_menu_reset_మాతృ_prelight(self);
	    తిరిగి TRUE;
	స్థితి GDK_కీలకం_కుడి:
	    ఒకవేళ (state & GDK_CONTROL_ముసుగు)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		ఒకవేళ (menubar != ఖాళీ)
		{
		    widget = vim_menu_bar_move_చురుకైన_అంశం(menubar, 1);
		    vim_menu_bar_సమితి_చురుకైన_అంశం(menubar,
			    VIM_MENU_BAR_అంశం(widget), TRUE);
		}
		తిరిగి TRUE;
	    }
	    // Open submenu if active item has one
	    ఒకవేళ (self->చురుకైన_అంశం != ఖాళీ
		    && vim_menu_select_చురుకైన_అంశం(self, TRUE))
	    {
		// Select first item in opened submenu
		VimMenu *submenu = VIM_MENU(
			VIM_MENU_అంశం(self->చురుకైన_అంశం)->submenu
			);

		vim_menu_సమితి_చురుకైన_అంశం(submenu,
			VIM_MENU_అంశం(
			    gtk_widget_get_మొదటి_బిడ్డ(submenu->box)),
			FALSE);
	    }
	    self->ignore_hover = TRUE;
	    తిరిగి TRUE;
	స్థితి GDK_కీలకం_Escape:
	    // Close all popover menus
	    vim_menu_close_all(self);
	    తిరిగి TRUE;
	స్థితి GDK_కీలకం_ISO_Enter:
	స్థితి GDK_కీలకం_3270_Enter:
	స్థితి GDK_కీలకం_KP_Enter:
	స్థితి GDK_కీలకం_Return:
	    ఒకవేళ (self->చురుకైన_అంశం != ఖాళీ)
		g_signal_emit_by_name(self->చురుకైన_అంశం, "clicked");
	    తిరిగి TRUE;
	అప్రమేయ:
	    ఆపు;
    }
    తిరిగి FALSE;
}


    స్థిర శూన్య
vim_menu_motion_cb(
	GtkEventController  *controller వాడని,
	ద్విగుణ		    x,
	ద్విగుణ		    y,
	VimMenu		    *self)
{
    ఒకవేళ (self->prev_x == -1 || self->prev_y == -1 ||
	    (fabs(self->prev_x - x) > 0.05 && fabs(self->prev_y - y) > 0.05))
	self->ignore_hover = FALSE;
    self->prev_x = x;
    self->prev_y = y;
}

    స్థిర శూన్య
vim_menu_focus_cb(GtkEventController *controller వాడని, VimMenu *self)
{
    gtk_popover_సమితి_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    స్థిర శూన్య
vim_menu_notify_mnemonics_cb(
	VimMenu	    *self,
	GParamSpec  *pspec వాడని,
	శూన్య	    *udata వాడని)
{
    gtk_popover_సమితి_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    స్థిర శూన్య
vim_menu_init(VimMenu *self)
{
    GtkEventController	*controller;
    GtkWidget		*దొంతర;
    GtkWidget		*మాతృ_box;
    GListModel		*controllers;

    gtk_popover_సమితి_has_arrow(GTK_POPOVER(self), FALSE);
    gtk_popover_సమితి_autohide(GTK_POPOVER(self), TRUE);

    // Do not make child popovers close parent popovers when they are closed.
    gtk_popover_సమితి_cascade_popdown(GTK_POPOVER(self), FALSE);

    దొంతర = gtk_దొంతర_new();

    // "stack" and "parent_box" have no use other than to make the css structure
    // of the popup menu be exactly like GtkPopoverMenu. This is so that GTK
    // themes style VimMenu exactly like GtkPopoverMenu.
    మాతృ_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_దొంతర_కూడు_బిడ్డ(GTK_దొంతర(దొంతర), మాతృ_box);
    gtk_దొంతర_సమితి_visible_బిడ్డ(GTK_దొంతర(దొంతర), మాతృ_box);

    self->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_సమితి_hexpand(self->box, TRUE);
    gtk_widget_సమితి_vexpand(self->box, TRUE);
    gtk_box_append(GTK_BOX(మాతృ_box), self->box);

    self->scr = gtk_scrolled_కిటికీ_new();
    gtk_scrolled_కిటికీ_సమితి_policy(GTK_SCROLLED_కిటికీ(self->scr),
	    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_కిటికీ_సమితి_propagate_natural_వెడల్పు(
	    GTK_SCROLLED_కిటికీ(self->scr), TRUE);
    gtk_scrolled_కిటికీ_సమితి_propagate_natural_ఎత్తు(
	    GTK_SCROLLED_కిటికీ(self->scr), TRUE);

    gtk_scrolled_కిటికీ_సమితి_బిడ్డ(GTK_SCROLLED_కిటికీ(self->scr), దొంతర);
    gtk_popover_సమితి_బిడ్డ(GTK_POPOVER(self), self->scr);

    gtk_widget_కూడు_css_class(GTK_WIDGET(self), "menu");

    // Add key controller for basic movement
    controller = gtk_event_controller_కీలకం_new();
    // Make sure we get the key presses first and handle them if possible
    gtk_event_controller_సమితి_ప్రసరణ_phase(controller,
	    GTK_PHASE_CAPTURE);
    g_signal_connect_object(controller, "key-pressed",
	    G_CALLBACK(vim_menu_కీలకం_pressed_cb),
	    self, G_CONNECT_DEFAULT);
    gtk_widget_కూడు_controller(GTK_WIDGET(self), controller);

    // Show mnemonic underline always
    controller = gtk_event_controller_focus_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_focus_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_కూడు_controller(GTK_WIDGET(self), controller);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "motion",
	    G_CALLBACK(vim_menu_motion_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_కూడు_controller(GTK_WIDGET(self), controller);

    g_signal_connect(self, "closed", G_CALLBACK(vim_menu_closed_cb), ఖాళీ);

    // Set all shortcut controllers in the window to not require a modifier for
    // mnemonics.
    controllers = gtk_widget_observe_controllers(GTK_WIDGET(self));
    కొరకు (guint i = 0; i < g_list_model_get_n_వస్తువులు(controllers); i++)
    {
	controller = g_list_model_get_అంశం(controllers, i);
	ఒకవేళ (GTK_IS_SHORTCUT_CONTROLLER(controller))
	    gtk_shortcut_controller_సమితి_mnemonics_modifiers(
		    GTK_SHORTCUT_CONTROLLER(controller), 0);
    }
    g_object_unref(controllers);

    // Force enable mnemonics underline
    g_signal_connect(self, "notify::mnemonics-visible",
	    G_CALLBACK(vim_menu_notify_mnemonics_cb), ఖాళీ);

    self->prev_x = self->prev_y = -1;
}

    GtkWidget *
vim_menu_new(శూన్య)
{
    తిరిగి g_object_new(VIM_TYPE_MENU, ఖాళీ);
}

    స్థిర GtkWidget *
vim_menu_చొప్పించు(VimMenu *self, GtkWidget *అంశం, పూర్ణాంకం idx)
{
    ఒకవేళ (idx > 0)
    {
	GList *prev = g_list_nth(self->వస్తువులు, idx - 1);
	ఒకవేళ (prev == ఖాళీ)
	    gtk_box_append(GTK_BOX(self->box), అంశం);
	లేకపోతే
	    gtk_box_చొప్పించు_బిడ్డ_after(GTK_BOX(self->box), అంశం,
		    prev->దత్తాంశం);
    }
    లేకపోతే ఒకవేళ (idx == 0)
	gtk_box_prepend(GTK_BOX(self->box), అంశం);
    లేకపోతే
	gtk_box_append(GTK_BOX(self->box), అంశం);

    self->వస్తువులు = g_list_చొప్పించు(self->వస్తువులు, అంశం, idx);
    తిరిగి అంశం;
}

    స్థిర శూన్య
vim_menu_అంశం_clicked_cb(Vim_Menu_అంశం *self, VimMenu *menu)
{
    // Only close all menus if item is a regular button (no submenu). If item
    // has a submenu, then just toggle it on and off.
    ఒకవేళ (self->submenu != ఖాళీ)
    {
	ఒకవేళ (gtk_widget_is_visible(self->submenu))
	    gtk_popover_popdown(GTK_POPOVER(self->submenu));
	లేకపోతే
	    gtk_popover_popup(GTK_POPOVER(self->submenu));
	తిరిగి;
    }

    // Since we set the "cascade-popdown" property to FALSE, we must popdown the
    // toplevel menu/popover, so that all submenus are closed.
    vim_menu_close_all(menu);
    ఒకవేళ (self->func != ఖాళీ)
	self->func(self, VIM_MENU_అంశం_CLICKED, self->func_udata);
}

    స్థిర gboolean
vim_menu_అంశం_mnemonic_activate_cb(
	Vim_Menu_అంశం *self,
	gboolean    సమూహం వాడని,
	VimMenu	    *menu)
{
    vim_menu_సమితి_చురుకైన_అంశం(menu, self, FALSE);
    // Return FALSE so that menu item emits "clicked" signal
    తిరిగి FALSE;
}

    స్థిర శూన్య
vim_menu_అంశం_enter_cb(
	GtkEventController  *controller,
	ద్విగుణ		    x వాడని,
	ద్విగుణ		    y వాడని,
	VimMenu		    *menu)
{
    Vim_Menu_అంశం  *self;

    ఒకవేళ (menu->ignore_hover || !gtk_event_controller_motion_కలిగియుంది_సూచిక(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	తిరిగి;

    self = VIM_MENU_అంశం(gtk_event_controller_get_widget(controller));
    vim_menu_సమితి_చురుకైన_అంశం(menu, self, TRUE);
}

    స్థిర శూన్య
vim_menu_అంశం_leave_cb(GtkEventController *controller, VimMenu *menu)
{
    Vim_Menu_అంశం  *self;

    ఒకవేళ (gtk_event_controller_motion_కలిగియుంది_సూచిక(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	తిరిగి;

    self = VIM_MENU_అంశం(gtk_event_controller_get_widget(controller));
    ఒకవేళ (menu->చురుకైన_అంశం == GTK_WIDGET(self))
	vim_menu_సమితి_చురుకైన_అంశం(menu, ఖాళీ, FALSE);
}

/*
 * Insert the menu item at the given index in the menu. If "idx" is negative,
 * then append the menu item.
 */
    శూన్య
vim_menu_చొప్పించు_అంశం(VimMenu *self, Vim_Menu_అంశం *అంశం, పూర్ణాంకం idx)
{
    GtkEventController *controller;

    vim_menu_చొప్పించు(self, GTK_WIDGET(అంశం), idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_అంశం_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_అంశం_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_కూడు_controller(GTK_WIDGET(అంశం), controller);

    g_signal_connect_object(అంశం, "clicked",
	    G_CALLBACK(vim_menu_అంశం_clicked_cb),
	    self, G_CONNECT_DEFAULT);
    g_signal_connect_object(అంశం, "mnemonic-activate",
	    G_CALLBACK(vim_menu_అంశం_mnemonic_activate_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Insert a separator at the given position and return it.
 */
    GtkWidget *
vim_menu_చొప్పించు_separator(VimMenu *self, పూర్ణాంకం idx)
{
    తిరిగి vim_menu_చొప్పించు(self,
	    gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), idx);
}

/*
 * Remove the menu item or separator from the menu
 */
    శూన్య
vim_menu_remove(VimMenu *self, GtkWidget *అంశం)
{
    self->వస్తువులు = g_list_remove(self->వస్తువులు, అంశం);
    gtk_box_remove(GTK_BOX(self->box), అంశం);
}

/*
 * Create a deep copy of the menu
 */
    GtkWidget *
vim_menu_copy(VimMenu *self)
{
    GtkWidget	*copy = vim_menu_new();
    పూర్ణాంకం		i = 0;

    కొరకు (GList *l = self->వస్తువులు; l != ఖాళీ; l = l->తదుపరి, i++)
    {
	Vim_Menu_అంశం *అంశం;
	GtkWidget   *అంశం_copy;

	ఒకవేళ (!VIM_IS_MENU_అంశం(l->దత్తాంశం))
	{
	    ప్రతిజ్ఞా(GTK_IS_SEPARATOR(l->దత్తాంశం));
	    vim_menu_చొప్పించు_separator(VIM_MENU(copy), i);
	    కొనసాగు;
	}

	అంశం = l->దత్తాంశం;
	అంశం_copy = vim_menu_అంశం_copy(అంశం);

	vim_menu_చొప్పించు_అంశం(VIM_MENU(copy), VIM_MENU_అంశం(అంశం_copy), i);
    }
    తిరిగి copy;
}

#అయితే_ముగింపు // FEAT_MENU
