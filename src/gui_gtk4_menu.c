/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU

#ਅਨ੍ਤਰ੍ਭਾਵ <gtk/gtk.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "gui_gtk4_menu.h"

// Note that this may return NULL for popup menus
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_MENU_BAR(m) VIM_MENU_BAR(gtk_widget_get_ancestor( \
	    GTK_WIDGET(m), VIM_TYPE_MENU_BAR))

/*
 * Similar as GtkButton but set CSS name to "item" to emulate GtkPopoverMenuBar
 * styling. Always has a submenu.
 */
ਬਣਤਰ Vim_Menu_Bar_ਵਸਤੂ
{
    GtkButton ਮਾਪਾ;

    GtkWidget *menu;
};

G_DEFINE_TYPE(Vim_Menu_Bar_ਵਸਤੂ_2, vim_menu_bar_ਵਸਤੂ, GTK_TYPE_BUTTON)

    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_ਵਸਤੂ_dispose(GObject *object)
{
    Vim_Menu_Bar_ਵਸਤੂ_2 *self = VIM_MENU_BAR_ਵਸਤੂ(object);

    g_clear_ਸੂਚਕ((GtkWidget **)&self->menu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_ਵਸਤੂ_ਮਾਪਾ_class)->dispose(object);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_ਵਸਤੂ_class_init(Vim_Menu_Bar_ਵਸਤੂ_Class *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_ਵਸਤੂ_dispose;

    gtk_widget_class_ਸੈੱਟ_css_name(widget_class, "item");
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_ਵਸਤੂ_init(Vim_Menu_Bar_ਵਸਤੂ_2 *self)
{
    // Enable mnemonics
    gtk_button_ਸੈੱਟ_use_underline(GTK_BUTTON(self), TRUE);
}

    GtkWidget *
vim_menu_bar_ਵਸਤੂ_new(ਅਚਲ ਅੱਖਰ *ਪਾਠ, VimMenu *menu)
{
    Vim_Menu_Bar_ਵਸਤੂ_2 *ਵਸਤੂ = g_object_new(VIM_TYPE_MENU_BAR_ਵਸਤੂ, ਨਿਰਾ);

    gtk_button_ਸੈੱਟ_label(GTK_BUTTON(ਵਸਤੂ), ਪਾਠ);

    ਵਸਤੂ->menu = GTK_WIDGET(menu);
    gtk_popover_ਸੈੱਟ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    // Make popover start at top left corner
    gtk_widget_ਸੈੱਟ_halign(GTK_WIDGET(menu), GTK_ALIGN_ਸ਼ੁਰੂ);
    gtk_widget_ਸੈੱਟ_ਮਾਪਾ(GTK_WIDGET(menu), GTK_WIDGET(ਵਸਤੂ));

    ਵਾਪਸ GTK_WIDGET(ਵਸਤੂ);
}

    ਖਾਲੀ
vim_menu_bar_ਵਸਤੂ_ਸੈੱਟ_ਪਾਠ(Vim_Menu_Bar_ਵਸਤੂ_2 *self, ਅਚਲ ਅੱਖਰ *ਪਾਠ)
{
    gtk_button_ਸੈੱਟ_label(GTK_BUTTON(self), ਪਾਠ);
}

/*
 * Similar to GtkPopoverMenuBar
 */
ਬਣਤਰ _VimMenuBar
{
    GtkWidget ਮਾਪਾ;

    GList *ਚੀਜ਼ਾਂ;

    // Currently visible item that has submenu popped up, else NULL
    GtkWidget *ਸਰਗਰਮ_ਵਸਤੂ;
};

G_DEFINE_TYPE(VimMenuBar, vim_menu_bar, GTK_TYPE_WIDGET)

    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_dispose(GObject *object)
{
    VimMenuBar *self = VIM_MENU_BAR(object);

    g_clear_list(&self->ਚੀਜ਼ਾਂ, (GDestroyNotify)gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_ਮਾਪਾ_class)->dispose(object);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_class_init(VimMenuBarClass *class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_dispose;

    gtk_widget_class_ਸੈੱਟ_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_ਸੈੱਟ_css_name(widget_class, "menubar");
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_init(VimMenuBar *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_ਸੈੱਟ_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_ਸੈੱਟ_spacing(GTK_BOX_LAYOUT(lm), 0);
}

    GtkWidget *
vim_menu_bar_new(ਖਾਲੀ)
{
    ਵਾਪਸ g_object_new(VIM_TYPE_MENU_BAR, ਨਿਰਾ);
}

/*
 * Create a VimMenu widget with the menus of the menu bar as its submenus. Note
 * that it is a deep copy.
 */
    GtkWidget *
vim_menu_bar_ਮੰਜ਼ਿਲ_menu(VimMenuBar *self)
{
    GtkWidget	*menu = vim_menu_new();
    ਪੂਰਨ		i = 0;

    ਲਈ (GList *l = self->ਚੀਜ਼ਾਂ; l != ਨਿਰਾ; l = l->ਅਗਲਾ, i++)
    {
	Vim_Menu_Bar_ਵਸਤੂ_2	*baritem = l->ਅੰਕੜੇ;
	GtkWidget	*ਵਸਤੂ;

	ਵਸਤੂ = vim_menu_ਵਸਤੂ_new(
		gtk_button_get_label(GTK_BUTTON(baritem)), ਨਿਰਾ, ਨਿਰਾ);

	vim_menu_ਵਸਤੂ_ਸੈੱਟ_submenu(VIM_MENU_ਵਸਤੂ(ਵਸਤੂ),
		VIM_MENU(vim_menu_copy(VIM_MENU(baritem->menu))));

	vim_menu_ਪਾਓ_ਵਸਤੂ(VIM_MENU(menu), VIM_MENU_ਵਸਤੂ(ਵਸਤੂ), i);
    }
    ਵਾਪਸ menu;
}

/*
 * Set the currently active menu of the menubar to "item". If NULL, then close
 * any submenus.
 */
    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(
	VimMenuBar	*self,
	Vim_Menu_Bar_ਵਸਤੂ_2	*ਵਸਤੂ,
	gboolean	force)
{
    // Do nothing if currently active item is "item", or if there is not
    // currently active item. User must click a menu item first for menus to
    // automatically appear on hover. This is unless "force" is TRUE.
    //
    // Only make item selected if there is no active item (no submenu open), or
    // if the item was set as the active item..
    ਜੇ ((!force && self->ਸਰਗਰਮ_ਵਸਤੂ == ਨਿਰਾ)
	    || self->ਸਰਗਰਮ_ਵਸਤੂ == GTK_WIDGET(ਵਸਤੂ))
    {
	ਜੇ (self->ਸਰਗਰਮ_ਵਸਤੂ == ਨਿਰਾ)
	    gtk_widget_ਸੈੱਟ_state_flags(GTK_WIDGET(ਵਸਤੂ),
		    GTK_STATE_FLAG_SELECTED, FALSE);
	ਵਾਪਸ;
    }

    ਜੇ (self->ਸਰਗਰਮ_ਵਸਤੂ != ਨਿਰਾ)
    {
	// Call this before popdown, since "closed" signal may be emitted
	// immediately.
	gtk_widget_unset_state_flags(self->ਸਰਗਰਮ_ਵਸਤੂ,
		GTK_STATE_FLAG_SELECTED);
	gtk_popover_popdown(GTK_POPOVER(
		    VIM_MENU_BAR_ਵਸਤੂ(self->ਸਰਗਰਮ_ਵਸਤੂ)->menu)
		);
    }

    self->ਸਰਗਰਮ_ਵਸਤੂ = GTK_WIDGET(ਵਸਤੂ);
    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
    {
	gtk_popover_popup(GTK_POPOVER(ਵਸਤੂ->menu));
	gtk_widget_ਸੈੱਟ_state_flags(GTK_WIDGET(ਵਸਤੂ),
		GTK_STATE_FLAG_SELECTED, FALSE);
    }
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_ਵਸਤੂ_enter_cb(
	GtkEventController  *controller,
	ਦੁੱਗਣਾ		    x ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ		    y ਅਣਵਰਤਿਆ,
	VimMenuBar	    *menubar)
{
    Vim_Menu_Bar_ਵਸਤੂ_2  *self;

    self = VIM_MENU_BAR_ਵਸਤੂ(gtk_event_controller_get_widget(controller));
    vim_menu_bar_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(menubar, self, FALSE);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_ਵਸਤੂ_leave_cb(
	GtkEventController  *controller,
	VimMenuBar	    *menubar)
{
    Vim_Menu_Bar_ਵਸਤੂ_2 *self;

    self = VIM_MENU_BAR_ਵਸਤੂ(gtk_event_controller_get_widget(controller));

    // If the item is the currently active item, then don't deselect it.
    ਜੇ (menubar->ਸਰਗਰਮ_ਵਸਤੂ != GTK_WIDGET(self))
	gtk_widget_unset_state_flags(GTK_WIDGET(self),
		GTK_STATE_FLAG_SELECTED);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_ਵਸਤੂ_clicked_cb(Vim_Menu_Bar_ਵਸਤੂ_2 *self, VimMenuBar *menubar)
{
    vim_menu_bar_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(menubar, self, TRUE);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_bar_ਵਸਤੂ_menu_closed_cb(VimMenu *menu ਅਣਵਰਤਿਆ, VimMenuBar *menubar)
{
    ਜੇ (menubar->ਸਰਗਰਮ_ਵਸਤੂ != ਨਿਰਾ)
	gtk_widget_unset_state_flags(GTK_WIDGET(menubar->ਸਰਗਰਮ_ਵਸਤੂ),
		GTK_STATE_FLAG_SELECTED);
    vim_menu_bar_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(menubar, ਨਿਰਾ, TRUE);
    // Make sure to focus drawarea
    gtk_widget_grab_focus(gui.drawarea);
}

/*
 * Insert the menu item at the given index in the menu bar.
 */
    ਖਾਲੀ
vim_menu_bar_ਪਾਓ_ਵਸਤੂ(VimMenuBar *self, Vim_Menu_Bar_ਵਸਤੂ_2 *ਵਸਤੂ, ਪੂਰਨ idx)
{
    GtkEventController	*controller;
    GList		*ਅਗਲਾ_sibling;

    ਅਗਲਾ_sibling = g_list_nth(self->ਚੀਜ਼ਾਂ, idx);
    gtk_widget_ਪਾਓ_before(GTK_WIDGET(ਵਸਤੂ), GTK_WIDGET(self),
	    ਅਗਲਾ_sibling == ਨਿਰਾ ? ਨਿਰਾ : ਅਗਲਾ_sibling->ਅੰਕੜੇ);

    self->ਚੀਜ਼ਾਂ = g_list_ਪਾਓ(self->ਚੀਜ਼ਾਂ, ਵਸਤੂ, idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_bar_ਵਸਤੂ_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_bar_ਵਸਤੂ_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_ਜੋੜੋ_controller(GTK_WIDGET(ਵਸਤੂ), controller);

    g_signal_connect_object(ਵਸਤੂ, "clicked",
	    G_CALLBACK(vim_menu_bar_ਵਸਤੂ_clicked_cb), self, G_CONNECT_DEFAULT);

    g_signal_connect_object(ਵਸਤੂ->menu, "closed",
	    G_CALLBACK(vim_menu_bar_ਵਸਤੂ_menu_closed_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Remove the menu item or separator from the menu bar
 */
    ਖਾਲੀ
vim_menu_bar_remove(VimMenuBar *self, GtkWidget *ਵਸਤੂ)
{
    self->ਚੀਜ਼ਾਂ = g_list_remove(self->ਚੀਜ਼ਾਂ, ਵਸਤੂ);
    gtk_widget_unparent(ਵਸਤੂ);
}

/*
 * Show the given menu in the menubar. If "item" is NULL, then show first menu.
 */
    ਖਾਲੀ
vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_ਵਸਤੂ_2 *ਵਸਤੂ)
{
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਸਤੂ = g_list_nth_ਅੰਕੜੇ(self->ਚੀਜ਼ਾਂ, 0);

    vim_menu_bar_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(self, ਵਸਤੂ, TRUE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    ਸਥਿਰ GtkWidget *
vim_menu_bar_move_ਸਰਗਰਮ_ਵਸਤੂ(VimMenuBar *self, ਪੂਰਨ dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->ਸਰਗਰਮ_ਵਸਤੂ;

    ਜੇ (widget == ਨਿਰਾ)
	ਵਾਪਸ gtk_widget_get_ਪਹਿਲਾ_ਬੱਚਾ(GTK_WIDGET(self));

    ਜੇ (dir > 0)
    {
	func = gtk_widget_get_ਅਗਲਾ_sibling;
	null_func = gtk_widget_get_ਪਹਿਲਾ_ਬੱਚਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_ਆਖਰੀ_ਬੱਚਾ;
    }

    ਜਦੋਂ (TRUE)
    {
	widget = func(widget);
	ਜੇ (widget == ਨਿਰਾ)
	{
	    ਜੇ (null_func == ਨਿਰਾ)
		ਰੋਕੋ;
	    widget = null_func(GTK_WIDGET(self));
	    null_func = ਨਿਰਾ;
	    ਜੇ (widget == ਨਿਰਾ)
		ਰੋਕੋ;
	}
	ਰੋਕੋ;
    }
    ਵਾਪਸ widget;
}

/*
 * Menu button that can be used to perform actions, or if there is a submenu,
 * toggle the state of the submenu popover. CSS name is "modelbutton" to make it
 * styled like GtkPopoverMenu
 */
ਬਣਤਰ Vim_Menu_ਵਸਤੂ_2
{
    GtkButton ਮਾਪਾ;

    GtkWidget *label;	    // Displays text for button.
    GtkWidget *aux_widget;  // Either an icon or a label showing the accelerator
			    // text.

    GtkWidget *submenu;	    // Submenu popover if any (VimMenu)

    // Callback called when clicked or selected, we store this so that copying a
    // menu item works properly.
    Vim_Menu_ਵਸਤੂ_Func	func;
    ਖਾਲੀ		*func_udata;
};

G_DEFINE_TYPE(Vim_Menu_ਵਸਤੂ, vim_menu_ਵਸਤੂ, GTK_TYPE_BUTTON)

    ਸਥਿਰ ਖਾਲੀ
vim_menu_ਵਸਤੂ_dispose(GObject *object)
{
    Vim_Menu_ਵਸਤੂ *self = VIM_MENU_ਵਸਤੂ(object);

    g_clear_ਸੂਚਕ(&self->label, gtk_widget_unparent);
    g_clear_ਸੂਚਕ(&self->aux_widget, gtk_widget_unparent);
    g_clear_ਸੂਚਕ((GtkWidget **)&self->submenu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_ਵਸਤੂ_ਮਾਪਾ_class)->dispose(object);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_ਵਸਤੂ_class_init(Vim_Menu_ਵਸਤੂ_Class *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_ਵਸਤੂ_dispose;

    gtk_widget_class_ਸੈੱਟ_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_ਸੈੱਟ_css_name(widget_class, "modelbutton");
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_ਵਸਤੂ_init(Vim_Menu_ਵਸਤੂ *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_ਸੈੱਟ_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_ਸੈੱਟ_spacing(GTK_BOX_LAYOUT(lm), 0);
}

/*
 * Create a new menu item with the given text to display. "func" may be NULL if
 * not needed.
 */
    GtkWidget *
vim_menu_ਵਸਤੂ_new(ਅਚਲ ਅੱਖਰ *ਪਾਠ, Vim_Menu_ਵਸਤੂ_Func func, ਖਾਲੀ *udata)
{
    Vim_Menu_ਵਸਤੂ *ਵਸਤੂ = g_object_new(VIM_TYPE_MENU_ਵਸਤੂ, ਨਿਰਾ);

    ਵਸਤੂ->func = func;
    ਵਸਤੂ->func_udata = udata;
    ਵਸਤੂ->label = gtk_label_new_with_mnemonic(ਪਾਠ);

    // Make sure label is on the right and pushes everything to the left
    gtk_widget_ਸੈੱਟ_halign(ਵਸਤੂ->label, GTK_ALIGN_ਸ਼ੁਰੂ);
    gtk_widget_ਸੈੱਟ_hexpand(ਵਸਤੂ->label, TRUE);
    gtk_widget_ਸੈੱਟ_ਮਾਪਾ(ਵਸਤੂ->label, GTK_WIDGET(ਵਸਤੂ));

    ਵਾਪਸ GTK_WIDGET(ਵਸਤੂ);
}

/*
 * Update displayed text for menu item
 */
    ਖਾਲੀ
vim_menu_ਵਸਤੂ_ਸੈੱਟ_ਪਾਠ(Vim_Menu_ਵਸਤੂ *self, ਅਚਲ ਅੱਖਰ *ਪਾਠ)
{
    gtk_label_ਸੈੱਟ_ਪਾਠ_with_mnemonic(GTK_LABEL(self->label), ਪਾਠ);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_ਵਸਤੂ_ਸੈੱਟ_aux_widget(Vim_Menu_ਵਸਤੂ *self, GtkWidget *aux)
{
    self->aux_widget = aux;
    gtk_widget_ਸੈੱਟ_halign(self->aux_widget, GTK_ALIGN_ਸਿਰਾ);
    gtk_widget_ਸੈੱਟ_hexpand(self->aux_widget, FALSE);
    gtk_widget_ਸੈੱਟ_margin_ਸ਼ੁਰੂ(self->aux_widget, 50);
}

/*
 * Set the accelerator text for the menu item.
 */
    ਖਾਲੀ
vim_menu_ਵਸਤੂ_ਸੈੱਟ_accel(Vim_Menu_ਵਸਤੂ *self, ਅਚਲ ਅੱਖਰ *accel_ਪਾਠ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(self->aux_widget == ਨਿਰਾ);

    vim_menu_ਵਸਤੂ_ਸੈੱਟ_aux_widget(self, gtk_label_new(accel_ਪਾਠ));
    gtk_widget_ਪਾਓ_after(self->aux_widget, GTK_WIDGET(self), self->label);
}

/*
 * Set the submenu popover for the menu item
 */
    ਖਾਲੀ
vim_menu_ਵਸਤੂ_ਸੈੱਟ_submenu(Vim_Menu_ਵਸਤੂ *self, VimMenu *submenu)
{
    GtkWidget *icon;

    ਪ੍ਰਤਿਜ੍ਞਾ(self->submenu == ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(self->aux_widget == ਨਿਰਾ);

    // Add arrow icon pointing to right
    icon = gtk_image_new_from_icon_name("pan-end-symbolic");
    vim_menu_ਵਸਤੂ_ਸੈੱਟ_aux_widget(self, icon);
    gtk_widget_ਪਾਓ_after(self->aux_widget, GTK_WIDGET(self), self->label);

    gtk_popover_ਸੈੱਟ_position(GTK_POPOVER(submenu), GTK_POS_ਸੱਜਾ);
    // Make top of popover be aligned with button.
    gtk_widget_ਸੈੱਟ_valign(GTK_WIDGET(submenu), GTK_ALIGN_ਸ਼ੁਰੂ);

    self->submenu = GTK_WIDGET(submenu);
    gtk_widget_ਸੈੱਟ_ਮਾਪਾ(GTK_WIDGET(submenu), GTK_WIDGET(self));
}

/*
 * Create a deep copy of the menu item
 */
    ਸਥਿਰ GtkWidget *
vim_menu_ਵਸਤੂ_copy(Vim_Menu_ਵਸਤੂ *self)
{
    GtkWidget *copy;

    copy = vim_menu_ਵਸਤੂ_new(
	    gtk_label_get_ਪਾਠ(GTK_LABEL(self->label)),
	    self->func, self->func_udata);

    ਜੇ (self->submenu != ਨਿਰਾ)
	vim_menu_ਵਸਤੂ_ਸੈੱਟ_submenu(VIM_MENU_ਵਸਤੂ(copy),
		VIM_MENU(vim_menu_copy(VIM_MENU(self->submenu))));
    ਨਹੀਂਤਾਂ ਜੇ (self->aux_widget != ਨਿਰਾ)
	vim_menu_ਵਸਤੂ_ਸੈੱਟ_accel(VIM_MENU_ਵਸਤੂ(copy),
		gtk_label_get_ਪਾਠ(GTK_LABEL(self->aux_widget)));
    ਵਾਪਸ copy;
}

/*
 * Similar to GtkPopoverMenu, except uses GtkWidgets directly like GTK3, instead
 * of abstracting it into GMenuModel.
 */
ਬਣਤਰ _VimMenu
{
    GtkPopover ਮਾਪਾ;

    GtkWidget *box;
    GtkWidget *scr;

    GList *ਚੀਜ਼ਾਂ;

    // Currently active item showing submenu popover, or being hovered on, or
    // NULL. Note that item may have submenu but not have it open, when
    // navigating via keyboard.
    GtkWidget *ਸਰਗਰਮ_ਵਸਤੂ;

    // Used when mouse is hovering over an item and user is navigating with
    // keyboard. When the scrolled window scrolls down or up, this causes a
    // mouse enter event, causing the active item to go to the item that the
    // mouse is hovered on, instead of the next item (from keyboard navigation).
    gboolean	ignore_hover;
    ਦੁੱਗਣਾ	prev_x;
    ਦੁੱਗਣਾ	prev_y;
};

G_DEFINE_TYPE(VimMenu, vim_menu, GTK_TYPE_POPOVER)

    ਸਥਿਰ ਖਾਲੀ
vim_menu_dispose(GObject *object)
{
    VimMenu *self = VIM_MENU(object);

    g_clear_list(&self->ਚੀਜ਼ਾਂ, (GDestroyNotify)gtk_widget_unparent);
    g_clear_ਸੂਚਕ(&self->scr, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_ਮਾਪਾ_class)->dispose(object);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_class_init(VimMenuClass *class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_dispose;
}

    ਸਥਿਰ gboolean
vim_menu_select_ਸਰਗਰਮ_ਵਸਤੂ(VimMenu *self, gboolean open)
{
    Vim_Menu_ਵਸਤੂ *ਵਸਤੂ;

    gtk_widget_ਸੈੱਟ_state_flags(self->ਸਰਗਰਮ_ਵਸਤੂ,
	    GTK_STATE_FLAG_SELECTED, FALSE);

    // Make sure to focus item, so that scrolled window knows what to do.
    gtk_widget_grab_focus(GTK_WIDGET(self->ਸਰਗਰਮ_ਵਸਤੂ));

    ਵਸਤੂ = VIM_MENU_ਵਸਤੂ(self->ਸਰਗਰਮ_ਵਸਤੂ);
    ਜੇ (ਵਸਤੂ->func != ਨਿਰਾ)
	ਵਸਤੂ->func(ਵਸਤੂ, VIM_MENU_ਵਸਤੂ_SELECTED, ਵਸਤੂ->func_udata);

    ਜੇ (open && VIM_MENU_ਵਸਤੂ(self->ਸਰਗਰਮ_ਵਸਤੂ)->submenu != ਨਿਰਾ)
    {
	GtkWidget *submenu = VIM_MENU_ਵਸਤੂ(self->ਸਰਗਰਮ_ਵਸਤੂ)->submenu;
	gtk_popover_popup(GTK_POPOVER(submenu));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Set the active item of the menu to "item". If "item" is NULL, then close any
 * submenus. If "open" is FALSE, then don't open the submenu if any.
 */
    ਸਥਿਰ ਖਾਲੀ
vim_menu_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(VimMenu *self, Vim_Menu_ਵਸਤੂ *ਵਸਤੂ, gboolean open)
{
    ਜੇ (self->ਸਰਗਰਮ_ਵਸਤੂ == GTK_WIDGET(ਵਸਤੂ))
	ਵਾਪਸ;

    ਜੇ (self->ਸਰਗਰਮ_ਵਸਤੂ != ਨਿਰਾ)
    {
	ਜੇ (VIM_MENU_ਵਸਤੂ(self->ਸਰਗਰਮ_ਵਸਤੂ)->submenu != ਨਿਰਾ)
	    gtk_popover_popdown(GTK_POPOVER(
			VIM_MENU_ਵਸਤੂ(self->ਸਰਗਰਮ_ਵਸਤੂ)->submenu
			));
	gtk_widget_unset_state_flags(GTK_WIDGET(self->ਸਰਗਰਮ_ਵਸਤੂ),
		GTK_STATE_FLAG_SELECTED);
    }

    self->ਸਰਗਰਮ_ਵਸਤੂ = GTK_WIDGET(ਵਸਤੂ);
    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
	(ਖਾਲੀ)vim_menu_select_ਸਰਗਰਮ_ਵਸਤੂ(self, open);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_closed_cb(VimMenu *self, ਖਾਲੀ *udata ਅਣਵਰਤਿਆ)
{
    vim_menu_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(self, ਨਿਰਾ, FALSE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    ਸਥਿਰ GtkWidget *
vim_menu_move_ਸਰਗਰਮ_ਵਸਤੂ(VimMenu *self, ਪੂਰਨ dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->ਸਰਗਰਮ_ਵਸਤੂ;

    // If there is no currently active item, then just use the first one
    ਜੇ (widget == ਨਿਰਾ)
    {
	widget = gtk_widget_get_ਪਹਿਲਾ_ਬੱਚਾ(self->box);
	ਜਦੋਂ (widget != ਨਿਰਾ && !VIM_IS_MENU_ਵਸਤੂ(widget))
	    widget = gtk_widget_get_ਅਗਲਾ_sibling(widget);
	ਵਾਪਸ widget;
    }

    // Could also just use GList functions, but this seems simpler (no
    // difference anyways).
    ਜੇ (dir > 0)
    {
	func = gtk_widget_get_ਅਗਲਾ_sibling;
	null_func = gtk_widget_get_ਪਹਿਲਾ_ਬੱਚਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_ਆਖਰੀ_ਬੱਚਾ;
    }

    ਜਦੋਂ (TRUE)
    {
	widget = func(widget);
	ਜੇ (widget == ਨਿਰਾ)
	{
	    ਜੇ (null_func == ਨਿਰਾ)
		ਰੋਕੋ;
	    widget = null_func(self->box);
	    null_func = ਨਿਰਾ;
	    ਜੇ (widget == ਨਿਰਾ)
		ਰੋਕੋ;
	}
	ਜੇ (VIM_IS_MENU_ਵਸਤੂ(widget))
	    ਰੋਕੋ;
    }
    ਵਾਪਸ widget;
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_reset_ਮਾਪਾ_prelight(VimMenu *self)
{
    GtkWidget	*ਮਾਪਾ = gtk_widget_get_ਮਾਪਾ(GTK_WIDGET(self));
    VimMenu	*ਮਾਪਾ_menu;

    // gtk_widget_get_ancestor assumes the widget itself is also an ancestor, so
    // we must get parent of menu first.
    ਮਾਪਾ_menu = VIM_MENU(gtk_widget_get_ancestor(ਮਾਪਾ, VIM_TYPE_MENU));

    ਜੇ (ਮਾਪਾ_menu == ਨਿਰਾ)
	// TRUE for popup menus
	ਵਾਪਸ;

    ਜੇ (ਮਾਪਾ_menu->ਸਰਗਰਮ_ਵਸਤੂ != ਨਿਰਾ)
	gtk_widget_unset_state_flags(GTK_WIDGET(ਮਾਪਾ_menu->ਸਰਗਰਮ_ਵਸਤੂ),
		GTK_STATE_FLAG_PRELIGHT);
}

/*
 * Close all submenus in the menubar given a menu widget
 */
    ਸਥਿਰ ਖਾਲੀ
vim_menu_close_all(VimMenu *self)
{
    VimMenuBar *menubar = GET_MENU_BAR(self);

    // Must check if NULL, because popup menus don't have a parent.
    ਜੇ (menubar != ਨਿਰਾ)
	vim_menu_bar_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(menubar, ਨਿਰਾ, TRUE);
    ਨਹੀਂਤਾਂ
	gtk_popover_popdown(GTK_POPOVER(self));

    // Grab focus after popup menus without a menubar are closed
    gtk_widget_grab_focus(gui.drawarea);
}

    ਸਥਿਰ gboolean
vim_menu_ਕੁੰਜੀ_pressed_cb(
	GtkEventController  *controller ਅਣਵਰਤਿਆ,
	guint		    keyval,
	guint		    keycode ਅਣਵਰਤਿਆ,
	GdkModifierType	    state,
	VimMenu		    *self)
{
    GtkWidget	*widget;

    ਚੋਣ (keyval)
    {
	ਹਾਲਤ GDK_ਕੁੰਜੀ_ਥੱਲੇ:
	ਹਾਲਤ GDK_ਕੁੰਜੀ_KP_ਥੱਲੇ:
	ਹਾਲਤ GDK_ਕੁੰਜੀ_ਉੱਪਰ:
	ਹਾਲਤ GDK_ਕੁੰਜੀ_KP_ਉੱਪਰ:
	ਹਾਲਤ GDK_ਕੁੰਜੀ_Tab:
	ਹਾਲਤ GDK_ਕੁੰਜੀ_KP_Tab:
	ਹਾਲਤ GDK_ਕੁੰਜੀ_ISO_ਖੱਬਾ_Tab:
	    // Go to the previous or next item if any
	    widget = vim_menu_move_ਸਰਗਰਮ_ਵਸਤੂ(self,
		    (state & GDK_ਖਿਸਕਾਓ_ਨਕਾਬ)
		    || keyval == GDK_ਕੁੰਜੀ_ਉੱਪਰ ? -1 : 1);
	    vim_menu_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(self, VIM_MENU_ਵਸਤੂ(widget), FALSE);
	    self->ignore_hover = TRUE;
	    ਵਾਪਸ TRUE;
	ਹਾਲਤ GDK_ਕੁੰਜੀ_ਖੱਬਾ:
	    // Pressing control switches menu bar item.
	    ਜੇ (state & GDK_CONTROL_ਨਕਾਬ)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		ਜੇ (menubar != ਨਿਰਾ)
		{
		    widget = vim_menu_bar_move_ਸਰਗਰਮ_ਵਸਤੂ(menubar, -1);
		    vim_menu_bar_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(menubar,
			    VIM_MENU_BAR_ਵਸਤੂ(widget), TRUE);
		}
		ਵਾਪਸ TRUE;
	    }
	    // Go to parent menu (if any). We can do this by just closing the
	    // popover.
	    gtk_popover_popdown(GTK_POPOVER(self));
	    // For some reason when pointer is hovered over draw area, the
	    // active item in the parent menu will stay prelighted even when the
	    // active item is moved.
	    vim_menu_reset_ਮਾਪਾ_prelight(self);
	    ਵਾਪਸ TRUE;
	ਹਾਲਤ GDK_ਕੁੰਜੀ_ਸੱਜਾ:
	    ਜੇ (state & GDK_CONTROL_ਨਕਾਬ)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		ਜੇ (menubar != ਨਿਰਾ)
		{
		    widget = vim_menu_bar_move_ਸਰਗਰਮ_ਵਸਤੂ(menubar, 1);
		    vim_menu_bar_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(menubar,
			    VIM_MENU_BAR_ਵਸਤੂ(widget), TRUE);
		}
		ਵਾਪਸ TRUE;
	    }
	    // Open submenu if active item has one
	    ਜੇ (self->ਸਰਗਰਮ_ਵਸਤੂ != ਨਿਰਾ
		    && vim_menu_select_ਸਰਗਰਮ_ਵਸਤੂ(self, TRUE))
	    {
		// Select first item in opened submenu
		VimMenu *submenu = VIM_MENU(
			VIM_MENU_ਵਸਤੂ(self->ਸਰਗਰਮ_ਵਸਤੂ)->submenu
			);

		vim_menu_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(submenu,
			VIM_MENU_ਵਸਤੂ(
			    gtk_widget_get_ਪਹਿਲਾ_ਬੱਚਾ(submenu->box)),
			FALSE);
	    }
	    self->ignore_hover = TRUE;
	    ਵਾਪਸ TRUE;
	ਹਾਲਤ GDK_ਕੁੰਜੀ_Escape:
	    // Close all popover menus
	    vim_menu_close_all(self);
	    ਵਾਪਸ TRUE;
	ਹਾਲਤ GDK_ਕੁੰਜੀ_ISO_Enter:
	ਹਾਲਤ GDK_ਕੁੰਜੀ_3270_Enter:
	ਹਾਲਤ GDK_ਕੁੰਜੀ_KP_Enter:
	ਹਾਲਤ GDK_ਕੁੰਜੀ_Return:
	    ਜੇ (self->ਸਰਗਰਮ_ਵਸਤੂ != ਨਿਰਾ)
		g_signal_emit_by_name(self->ਸਰਗਰਮ_ਵਸਤੂ, "clicked");
	    ਵਾਪਸ TRUE;
	ਮੂਲ:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ FALSE;
}


    ਸਥਿਰ ਖਾਲੀ
vim_menu_motion_cb(
	GtkEventController  *controller ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ		    x,
	ਦੁੱਗਣਾ		    y,
	VimMenu		    *self)
{
    ਜੇ (self->prev_x == -1 || self->prev_y == -1 ||
	    (fabs(self->prev_x - x) > 0.05 && fabs(self->prev_y - y) > 0.05))
	self->ignore_hover = FALSE;
    self->prev_x = x;
    self->prev_y = y;
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_focus_cb(GtkEventController *controller ਅਣਵਰਤਿਆ, VimMenu *self)
{
    gtk_popover_ਸੈੱਟ_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_notify_mnemonics_cb(
	VimMenu	    *self,
	GParamSpec  *pspec ਅਣਵਰਤਿਆ,
	ਖਾਲੀ	    *udata ਅਣਵਰਤਿਆ)
{
    gtk_popover_ਸੈੱਟ_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_init(VimMenu *self)
{
    GtkEventController	*controller;
    GtkWidget		*ਢੇਰ;
    GtkWidget		*ਮਾਪਾ_box;
    GListModel		*controllers;

    gtk_popover_ਸੈੱਟ_has_arrow(GTK_POPOVER(self), FALSE);
    gtk_popover_ਸੈੱਟ_autohide(GTK_POPOVER(self), TRUE);

    // Do not make child popovers close parent popovers when they are closed.
    gtk_popover_ਸੈੱਟ_cascade_popdown(GTK_POPOVER(self), FALSE);

    ਢੇਰ = gtk_ਢੇਰ_new();

    // "stack" and "parent_box" have no use other than to make the css structure
    // of the popup menu be exactly like GtkPopoverMenu. This is so that GTK
    // themes style VimMenu exactly like GtkPopoverMenu.
    ਮਾਪਾ_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_ਢੇਰ_ਜੋੜੋ_ਬੱਚਾ(GTK_ਢੇਰ(ਢੇਰ), ਮਾਪਾ_box);
    gtk_ਢੇਰ_ਸੈੱਟ_visible_ਬੱਚਾ(GTK_ਢੇਰ(ਢੇਰ), ਮਾਪਾ_box);

    self->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_ਸੈੱਟ_hexpand(self->box, TRUE);
    gtk_widget_ਸੈੱਟ_vexpand(self->box, TRUE);
    gtk_box_append(GTK_BOX(ਮਾਪਾ_box), self->box);

    self->scr = gtk_scrolled_ਖਿੜਕੀ_new();
    gtk_scrolled_ਖਿੜਕੀ_ਸੈੱਟ_policy(GTK_SCROLLED_ਖਿੜਕੀ(self->scr),
	    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_ਖਿੜਕੀ_ਸੈੱਟ_propagate_natural_ਚੌੜਾਈ(
	    GTK_SCROLLED_ਖਿੜਕੀ(self->scr), TRUE);
    gtk_scrolled_ਖਿੜਕੀ_ਸੈੱਟ_propagate_natural_ਉਚਾਈ(
	    GTK_SCROLLED_ਖਿੜਕੀ(self->scr), TRUE);

    gtk_scrolled_ਖਿੜਕੀ_ਸੈੱਟ_ਬੱਚਾ(GTK_SCROLLED_ਖਿੜਕੀ(self->scr), ਢੇਰ);
    gtk_popover_ਸੈੱਟ_ਬੱਚਾ(GTK_POPOVER(self), self->scr);

    gtk_widget_ਜੋੜੋ_css_class(GTK_WIDGET(self), "menu");

    // Add key controller for basic movement
    controller = gtk_event_controller_ਕੁੰਜੀ_new();
    // Make sure we get the key presses first and handle them if possible
    gtk_event_controller_ਸੈੱਟ_ਪ੍ਰਸਾਰ_phase(controller,
	    GTK_PHASE_CAPTURE);
    g_signal_connect_object(controller, "key-pressed",
	    G_CALLBACK(vim_menu_ਕੁੰਜੀ_pressed_cb),
	    self, G_CONNECT_DEFAULT);
    gtk_widget_ਜੋੜੋ_controller(GTK_WIDGET(self), controller);

    // Show mnemonic underline always
    controller = gtk_event_controller_focus_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_focus_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_ਜੋੜੋ_controller(GTK_WIDGET(self), controller);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "motion",
	    G_CALLBACK(vim_menu_motion_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_ਜੋੜੋ_controller(GTK_WIDGET(self), controller);

    g_signal_connect(self, "closed", G_CALLBACK(vim_menu_closed_cb), ਨਿਰਾ);

    // Set all shortcut controllers in the window to not require a modifier for
    // mnemonics.
    controllers = gtk_widget_observe_controllers(GTK_WIDGET(self));
    ਲਈ (guint i = 0; i < g_list_model_get_n_ਚੀਜ਼ਾਂ(controllers); i++)
    {
	controller = g_list_model_get_ਵਸਤੂ(controllers, i);
	ਜੇ (GTK_IS_SHORTCUT_CONTROLLER(controller))
	    gtk_shortcut_controller_ਸੈੱਟ_mnemonics_modifiers(
		    GTK_SHORTCUT_CONTROLLER(controller), 0);
    }
    g_object_unref(controllers);

    // Force enable mnemonics underline
    g_signal_connect(self, "notify::mnemonics-visible",
	    G_CALLBACK(vim_menu_notify_mnemonics_cb), ਨਿਰਾ);

    self->prev_x = self->prev_y = -1;
}

    GtkWidget *
vim_menu_new(ਖਾਲੀ)
{
    ਵਾਪਸ g_object_new(VIM_TYPE_MENU, ਨਿਰਾ);
}

    ਸਥਿਰ GtkWidget *
vim_menu_ਪਾਓ(VimMenu *self, GtkWidget *ਵਸਤੂ, ਪੂਰਨ idx)
{
    ਜੇ (idx > 0)
    {
	GList *prev = g_list_nth(self->ਚੀਜ਼ਾਂ, idx - 1);
	ਜੇ (prev == ਨਿਰਾ)
	    gtk_box_append(GTK_BOX(self->box), ਵਸਤੂ);
	ਨਹੀਂਤਾਂ
	    gtk_box_ਪਾਓ_ਬੱਚਾ_after(GTK_BOX(self->box), ਵਸਤੂ,
		    prev->ਅੰਕੜੇ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (idx == 0)
	gtk_box_prepend(GTK_BOX(self->box), ਵਸਤੂ);
    ਨਹੀਂਤਾਂ
	gtk_box_append(GTK_BOX(self->box), ਵਸਤੂ);

    self->ਚੀਜ਼ਾਂ = g_list_ਪਾਓ(self->ਚੀਜ਼ਾਂ, ਵਸਤੂ, idx);
    ਵਾਪਸ ਵਸਤੂ;
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_ਵਸਤੂ_clicked_cb(Vim_Menu_ਵਸਤੂ *self, VimMenu *menu)
{
    // Only close all menus if item is a regular button (no submenu). If item
    // has a submenu, then just toggle it on and off.
    ਜੇ (self->submenu != ਨਿਰਾ)
    {
	ਜੇ (gtk_widget_is_visible(self->submenu))
	    gtk_popover_popdown(GTK_POPOVER(self->submenu));
	ਨਹੀਂਤਾਂ
	    gtk_popover_popup(GTK_POPOVER(self->submenu));
	ਵਾਪਸ;
    }

    // Since we set the "cascade-popdown" property to FALSE, we must popdown the
    // toplevel menu/popover, so that all submenus are closed.
    vim_menu_close_all(menu);
    ਜੇ (self->func != ਨਿਰਾ)
	self->func(self, VIM_MENU_ਵਸਤੂ_CLICKED, self->func_udata);
}

    ਸਥਿਰ gboolean
vim_menu_ਵਸਤੂ_mnemonic_activate_cb(
	Vim_Menu_ਵਸਤੂ *self,
	gboolean    ਸਮੂਹ ਅਣਵਰਤਿਆ,
	VimMenu	    *menu)
{
    vim_menu_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(menu, self, FALSE);
    // Return FALSE so that menu item emits "clicked" signal
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_ਵਸਤੂ_enter_cb(
	GtkEventController  *controller,
	ਦੁੱਗਣਾ		    x ਅਣਵਰਤਿਆ,
	ਦੁੱਗਣਾ		    y ਅਣਵਰਤਿਆ,
	VimMenu		    *menu)
{
    Vim_Menu_ਵਸਤੂ  *self;

    ਜੇ (menu->ignore_hover || !gtk_event_controller_motion_ਸ਼ਾਮਲ_ਹੈ_ਸੂਚਕ(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	ਵਾਪਸ;

    self = VIM_MENU_ਵਸਤੂ(gtk_event_controller_get_widget(controller));
    vim_menu_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(menu, self, TRUE);
}

    ਸਥਿਰ ਖਾਲੀ
vim_menu_ਵਸਤੂ_leave_cb(GtkEventController *controller, VimMenu *menu)
{
    Vim_Menu_ਵਸਤੂ  *self;

    ਜੇ (gtk_event_controller_motion_ਸ਼ਾਮਲ_ਹੈ_ਸੂਚਕ(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	ਵਾਪਸ;

    self = VIM_MENU_ਵਸਤੂ(gtk_event_controller_get_widget(controller));
    ਜੇ (menu->ਸਰਗਰਮ_ਵਸਤੂ == GTK_WIDGET(self))
	vim_menu_ਸੈੱਟ_ਸਰਗਰਮ_ਵਸਤੂ(menu, ਨਿਰਾ, FALSE);
}

/*
 * Insert the menu item at the given index in the menu. If "idx" is negative,
 * then append the menu item.
 */
    ਖਾਲੀ
vim_menu_ਪਾਓ_ਵਸਤੂ(VimMenu *self, Vim_Menu_ਵਸਤੂ *ਵਸਤੂ, ਪੂਰਨ idx)
{
    GtkEventController *controller;

    vim_menu_ਪਾਓ(self, GTK_WIDGET(ਵਸਤੂ), idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_ਵਸਤੂ_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_ਵਸਤੂ_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_ਜੋੜੋ_controller(GTK_WIDGET(ਵਸਤੂ), controller);

    g_signal_connect_object(ਵਸਤੂ, "clicked",
	    G_CALLBACK(vim_menu_ਵਸਤੂ_clicked_cb),
	    self, G_CONNECT_DEFAULT);
    g_signal_connect_object(ਵਸਤੂ, "mnemonic-activate",
	    G_CALLBACK(vim_menu_ਵਸਤੂ_mnemonic_activate_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Insert a separator at the given position and return it.
 */
    GtkWidget *
vim_menu_ਪਾਓ_separator(VimMenu *self, ਪੂਰਨ idx)
{
    ਵਾਪਸ vim_menu_ਪਾਓ(self,
	    gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), idx);
}

/*
 * Remove the menu item or separator from the menu
 */
    ਖਾਲੀ
vim_menu_remove(VimMenu *self, GtkWidget *ਵਸਤੂ)
{
    self->ਚੀਜ਼ਾਂ = g_list_remove(self->ਚੀਜ਼ਾਂ, ਵਸਤੂ);
    gtk_box_remove(GTK_BOX(self->box), ਵਸਤੂ);
}

/*
 * Create a deep copy of the menu
 */
    GtkWidget *
vim_menu_copy(VimMenu *self)
{
    GtkWidget	*copy = vim_menu_new();
    ਪੂਰਨ		i = 0;

    ਲਈ (GList *l = self->ਚੀਜ਼ਾਂ; l != ਨਿਰਾ; l = l->ਅਗਲਾ, i++)
    {
	Vim_Menu_ਵਸਤੂ *ਵਸਤੂ;
	GtkWidget   *ਵਸਤੂ_copy;

	ਜੇ (!VIM_IS_MENU_ਵਸਤੂ(l->ਅੰਕੜੇ))
	{
	    ਪ੍ਰਤਿਜ੍ਞਾ(GTK_IS_SEPARATOR(l->ਅੰਕੜੇ));
	    vim_menu_ਪਾਓ_separator(VIM_MENU(copy), i);
	    ਜਾਰੀ;
	}

	ਵਸਤੂ = l->ਅੰਕੜੇ;
	ਵਸਤੂ_copy = vim_menu_ਵਸਤੂ_copy(ਵਸਤੂ);

	vim_menu_ਪਾਓ_ਵਸਤੂ(VIM_MENU(copy), VIM_MENU_ਵਸਤੂ(ਵਸਤੂ_copy), i);
    }
    ਵਾਪਸ copy;
}

#ਜੇ_ਅੰਤ // FEAT_MENU
