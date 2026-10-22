/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU

#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>
#ಅನ್ತರ್ಭಾವ "gui_gtk4_menu.h"

// Note that this may return NULL for popup menus
#ವ್ಯಾಖ್ಯಾನಿಸು GET_MENU_BAR(m) VIM_MENU_BAR(gtk_widget_get_ancestor( \
	    GTK_WIDGET(m), VIM_TYPE_MENU_BAR))

/*
 * Similar as GtkButton but set CSS name to "item" to emulate GtkPopoverMenuBar
 * styling. Always has a submenu.
 */
ರಚನೆ Vim_Menu_Bar_ಅಂಶ
{
    GtkButton ಪೋಷಕ;

    GtkWidget *menu;
};

G_DEFINE_TYPE(Vim_Menu_Bar_ಅಂಶ_2, vim_menu_bar_ಅಂಶ, GTK_TYPE_BUTTON)

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_ಅಂಶ_dispose(GObject *object)
{
    Vim_Menu_Bar_ಅಂಶ_2 *self = VIM_MENU_BAR_ಅಂಶ(object);

    g_clear_ಸೂಚಕ((GtkWidget **)&self->menu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_ಅಂಶ_ಪೋಷಕ_class)->dispose(object);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_ಅಂಶ_class_init(Vim_Menu_Bar_ಅಂಶ_Class *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_ಅಂಶ_dispose;

    gtk_widget_class_ಗಣ_css_name(widget_class, "item");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_ಅಂಶ_init(Vim_Menu_Bar_ಅಂಶ_2 *self)
{
    // Enable mnemonics
    gtk_button_ಗಣ_use_underline(GTK_BUTTON(self), TRUE);
}

    GtkWidget *
vim_menu_bar_ಅಂಶ_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, VimMenu *menu)
{
    Vim_Menu_Bar_ಅಂಶ_2 *ಅಂಶ = g_object_new(VIM_TYPE_MENU_BAR_ಅಂಶ, ಖಾಲಿ);

    gtk_button_ಗಣ_label(GTK_BUTTON(ಅಂಶ), ಪಠ್ಯ);

    ಅಂಶ->menu = GTK_WIDGET(menu);
    gtk_popover_ಗಣ_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    // Make popover start at top left corner
    gtk_widget_ಗಣ_halign(GTK_WIDGET(menu), GTK_ALIGN_ಆರಂಭ);
    gtk_widget_ಗಣ_ಪೋಷಕ(GTK_WIDGET(menu), GTK_WIDGET(ಅಂಶ));

    ಹಿಂತಿರುಗಿ GTK_WIDGET(ಅಂಶ);
}

    ಶೂನ್ಯ
vim_menu_bar_ಅಂಶ_ಗಣ_ಪಠ್ಯ(Vim_Menu_Bar_ಅಂಶ_2 *self, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ)
{
    gtk_button_ಗಣ_label(GTK_BUTTON(self), ಪಠ್ಯ);
}

/*
 * Similar to GtkPopoverMenuBar
 */
ರಚನೆ _VimMenuBar
{
    GtkWidget ಪೋಷಕ;

    GList *ವಸ್ತುಗಳು;

    // Currently visible item that has submenu popped up, else NULL
    GtkWidget *ಸಕ್ರಿಯ_ಅಂಶ;
};

G_DEFINE_TYPE(VimMenuBar, vim_menu_bar, GTK_TYPE_WIDGET)

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_dispose(GObject *object)
{
    VimMenuBar *self = VIM_MENU_BAR(object);

    g_clear_list(&self->ವಸ್ತುಗಳು, (GDestroyNotify)gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_bar_ಪೋಷಕ_class)->dispose(object);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_class_init(VimMenuBarClass *class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_bar_dispose;

    gtk_widget_class_ಗಣ_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_ಗಣ_css_name(widget_class, "menubar");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_init(VimMenuBar *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_ಗಣ_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_ಗಣ_spacing(GTK_BOX_LAYOUT(lm), 0);
}

    GtkWidget *
vim_menu_bar_new(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ g_object_new(VIM_TYPE_MENU_BAR, ಖಾಲಿ);
}

/*
 * Create a VimMenu widget with the menus of the menu bar as its submenus. Note
 * that it is a deep copy.
 */
    GtkWidget *
vim_menu_bar_ಗಮ್ಯ_menu(VimMenuBar *self)
{
    GtkWidget	*menu = vim_menu_new();
    ಪೂರ್ಣಾಂಕ		i = 0;

    ಗಾಗಿ (GList *l = self->ವಸ್ತುಗಳು; l != ಖಾಲಿ; l = l->ಮುಂದಿನ, i++)
    {
	Vim_Menu_Bar_ಅಂಶ_2	*baritem = l->ದತ್ತಾಂಶ;
	GtkWidget	*ಅಂಶ;

	ಅಂಶ = vim_menu_ಅಂಶ_new(
		gtk_button_get_label(GTK_BUTTON(baritem)), ಖಾಲಿ, ಖಾಲಿ);

	vim_menu_ಅಂಶ_ಗಣ_submenu(VIM_MENU_ಅಂಶ(ಅಂಶ),
		VIM_MENU(vim_menu_copy(VIM_MENU(baritem->menu))));

	vim_menu_ಸೇರಿಸು_ಅಂಶ(VIM_MENU(menu), VIM_MENU_ಅಂಶ(ಅಂಶ), i);
    }
    ಹಿಂತಿರುಗಿ menu;
}

/*
 * Set the currently active menu of the menubar to "item". If NULL, then close
 * any submenus.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(
	VimMenuBar	*self,
	Vim_Menu_Bar_ಅಂಶ_2	*ಅಂಶ,
	gboolean	force)
{
    // Do nothing if currently active item is "item", or if there is not
    // currently active item. User must click a menu item first for menus to
    // automatically appear on hover. This is unless "force" is TRUE.
    //
    // Only make item selected if there is no active item (no submenu open), or
    // if the item was set as the active item..
    ಆದರೆ ((!force && self->ಸಕ್ರಿಯ_ಅಂಶ == ಖಾಲಿ)
	    || self->ಸಕ್ರಿಯ_ಅಂಶ == GTK_WIDGET(ಅಂಶ))
    {
	ಆದರೆ (self->ಸಕ್ರಿಯ_ಅಂಶ == ಖಾಲಿ)
	    gtk_widget_ಗಣ_state_flags(GTK_WIDGET(ಅಂಶ),
		    GTK_STATE_FLAG_SELECTED, FALSE);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (self->ಸಕ್ರಿಯ_ಅಂಶ != ಖಾಲಿ)
    {
	// Call this before popdown, since "closed" signal may be emitted
	// immediately.
	gtk_widget_unset_state_flags(self->ಸಕ್ರಿಯ_ಅಂಶ,
		GTK_STATE_FLAG_SELECTED);
	gtk_popover_popdown(GTK_POPOVER(
		    VIM_MENU_BAR_ಅಂಶ(self->ಸಕ್ರಿಯ_ಅಂಶ)->menu)
		);
    }

    self->ಸಕ್ರಿಯ_ಅಂಶ = GTK_WIDGET(ಅಂಶ);
    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
    {
	gtk_popover_popup(GTK_POPOVER(ಅಂಶ->menu));
	gtk_widget_ಗಣ_state_flags(GTK_WIDGET(ಅಂಶ),
		GTK_STATE_FLAG_SELECTED, FALSE);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_ಅಂಶ_enter_cb(
	GtkEventController  *controller,
	ದ್ವಿಗುಣ		    x ಬಳಸದ,
	ದ್ವಿಗುಣ		    y ಬಳಸದ,
	VimMenuBar	    *menubar)
{
    Vim_Menu_Bar_ಅಂಶ_2  *self;

    self = VIM_MENU_BAR_ಅಂಶ(gtk_event_controller_get_widget(controller));
    vim_menu_bar_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(menubar, self, FALSE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_ಅಂಶ_leave_cb(
	GtkEventController  *controller,
	VimMenuBar	    *menubar)
{
    Vim_Menu_Bar_ಅಂಶ_2 *self;

    self = VIM_MENU_BAR_ಅಂಶ(gtk_event_controller_get_widget(controller));

    // If the item is the currently active item, then don't deselect it.
    ಆದರೆ (menubar->ಸಕ್ರಿಯ_ಅಂಶ != GTK_WIDGET(self))
	gtk_widget_unset_state_flags(GTK_WIDGET(self),
		GTK_STATE_FLAG_SELECTED);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_ಅಂಶ_clicked_cb(Vim_Menu_Bar_ಅಂಶ_2 *self, VimMenuBar *menubar)
{
    vim_menu_bar_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(menubar, self, TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_bar_ಅಂಶ_menu_closed_cb(VimMenu *menu ಬಳಸದ, VimMenuBar *menubar)
{
    ಆದರೆ (menubar->ಸಕ್ರಿಯ_ಅಂಶ != ಖಾಲಿ)
	gtk_widget_unset_state_flags(GTK_WIDGET(menubar->ಸಕ್ರಿಯ_ಅಂಶ),
		GTK_STATE_FLAG_SELECTED);
    vim_menu_bar_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(menubar, ಖಾಲಿ, TRUE);
    // Make sure to focus drawarea
    gtk_widget_grab_focus(gui.drawarea);
}

/*
 * Insert the menu item at the given index in the menu bar.
 */
    ಶೂನ್ಯ
vim_menu_bar_ಸೇರಿಸು_ಅಂಶ(VimMenuBar *self, Vim_Menu_Bar_ಅಂಶ_2 *ಅಂಶ, ಪೂರ್ಣಾಂಕ idx)
{
    GtkEventController	*controller;
    GList		*ಮುಂದಿನ_sibling;

    ಮುಂದಿನ_sibling = g_list_nth(self->ವಸ್ತುಗಳು, idx);
    gtk_widget_ಸೇರಿಸು_before(GTK_WIDGET(ಅಂಶ), GTK_WIDGET(self),
	    ಮುಂದಿನ_sibling == ಖಾಲಿ ? ಖಾಲಿ : ಮುಂದಿನ_sibling->ದತ್ತಾಂಶ);

    self->ವಸ್ತುಗಳು = g_list_ಸೇರಿಸು(self->ವಸ್ತುಗಳು, ಅಂಶ, idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_bar_ಅಂಶ_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_bar_ಅಂಶ_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_ಕೂಡಿಸು_controller(GTK_WIDGET(ಅಂಶ), controller);

    g_signal_connect_object(ಅಂಶ, "clicked",
	    G_CALLBACK(vim_menu_bar_ಅಂಶ_clicked_cb), self, G_CONNECT_DEFAULT);

    g_signal_connect_object(ಅಂಶ->menu, "closed",
	    G_CALLBACK(vim_menu_bar_ಅಂಶ_menu_closed_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Remove the menu item or separator from the menu bar
 */
    ಶೂನ್ಯ
vim_menu_bar_remove(VimMenuBar *self, GtkWidget *ಅಂಶ)
{
    self->ವಸ್ತುಗಳು = g_list_remove(self->ವಸ್ತುಗಳು, ಅಂಶ);
    gtk_widget_unparent(ಅಂಶ);
}

/*
 * Show the given menu in the menubar. If "item" is NULL, then show first menu.
 */
    ಶೂನ್ಯ
vim_menu_bar_show(VimMenuBar *self, Vim_Menu_Bar_ಅಂಶ_2 *ಅಂಶ)
{
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಅಂಶ = g_list_nth_ದತ್ತಾಂಶ(self->ವಸ್ತುಗಳು, 0);

    vim_menu_bar_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(self, ಅಂಶ, TRUE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    ಸ್ಥಿರ GtkWidget *
vim_menu_bar_move_ಸಕ್ರಿಯ_ಅಂಶ(VimMenuBar *self, ಪೂರ್ಣಾಂಕ dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->ಸಕ್ರಿಯ_ಅಂಶ;

    ಆದರೆ (widget == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ gtk_widget_get_ಮೊದಲ_ಮಗು(GTK_WIDGET(self));

    ಆದರೆ (dir > 0)
    {
	func = gtk_widget_get_ಮುಂದಿನ_sibling;
	null_func = gtk_widget_get_ಮೊದಲ_ಮಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_ಕೊನೆಯ_ಮಗು;
    }

    ಆಗ (TRUE)
    {
	widget = func(widget);
	ಆದರೆ (widget == ಖಾಲಿ)
	{
	    ಆದರೆ (null_func == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    widget = null_func(GTK_WIDGET(self));
	    null_func = ಖಾಲಿ;
	    ಆದರೆ (widget == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	}
	ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ widget;
}

/*
 * Menu button that can be used to perform actions, or if there is a submenu,
 * toggle the state of the submenu popover. CSS name is "modelbutton" to make it
 * styled like GtkPopoverMenu
 */
ರಚನೆ Vim_Menu_ಅಂಶ_2
{
    GtkButton ಪೋಷಕ;

    GtkWidget *label;	    // Displays text for button.
    GtkWidget *aux_widget;  // Either an icon or a label showing the accelerator
			    // text.

    GtkWidget *submenu;	    // Submenu popover if any (VimMenu)

    // Callback called when clicked or selected, we store this so that copying a
    // menu item works properly.
    Vim_Menu_ಅಂಶ_Func	func;
    ಶೂನ್ಯ		*func_udata;
};

G_DEFINE_TYPE(Vim_Menu_ಅಂಶ, vim_menu_ಅಂಶ, GTK_TYPE_BUTTON)

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_ಅಂಶ_dispose(GObject *object)
{
    Vim_Menu_ಅಂಶ *self = VIM_MENU_ಅಂಶ(object);

    g_clear_ಸೂಚಕ(&self->label, gtk_widget_unparent);
    g_clear_ಸೂಚಕ(&self->aux_widget, gtk_widget_unparent);
    g_clear_ಸೂಚಕ((GtkWidget **)&self->submenu, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_ಅಂಶ_ಪೋಷಕ_class)->dispose(object);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_ಅಂಶ_class_init(Vim_Menu_ಅಂಶ_Class *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_ಅಂಶ_dispose;

    gtk_widget_class_ಗಣ_layout_manager_type(widget_class, GTK_TYPE_BOX_LAYOUT);
    gtk_widget_class_ಗಣ_css_name(widget_class, "modelbutton");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_ಅಂಶ_init(Vim_Menu_ಅಂಶ *self)
{
    GtkLayoutManager *lm = gtk_widget_get_layout_manager(GTK_WIDGET(self));

    gtk_orientable_ಗಣ_orientation(GTK_ORIENTABLE(lm),
	    GTK_ORIENTATION_HORIZONTAL);
    gtk_box_layout_ಗಣ_spacing(GTK_BOX_LAYOUT(lm), 0);
}

/*
 * Create a new menu item with the given text to display. "func" may be NULL if
 * not needed.
 */
    GtkWidget *
vim_menu_ಅಂಶ_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, Vim_Menu_ಅಂಶ_Func func, ಶೂನ್ಯ *udata)
{
    Vim_Menu_ಅಂಶ *ಅಂಶ = g_object_new(VIM_TYPE_MENU_ಅಂಶ, ಖಾಲಿ);

    ಅಂಶ->func = func;
    ಅಂಶ->func_udata = udata;
    ಅಂಶ->label = gtk_label_new_with_mnemonic(ಪಠ್ಯ);

    // Make sure label is on the right and pushes everything to the left
    gtk_widget_ಗಣ_halign(ಅಂಶ->label, GTK_ALIGN_ಆರಂಭ);
    gtk_widget_ಗಣ_hexpand(ಅಂಶ->label, TRUE);
    gtk_widget_ಗಣ_ಪೋಷಕ(ಅಂಶ->label, GTK_WIDGET(ಅಂಶ));

    ಹಿಂತಿರುಗಿ GTK_WIDGET(ಅಂಶ);
}

/*
 * Update displayed text for menu item
 */
    ಶೂನ್ಯ
vim_menu_ಅಂಶ_ಗಣ_ಪಠ್ಯ(Vim_Menu_ಅಂಶ *self, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ)
{
    gtk_label_ಗಣ_ಪಠ್ಯ_with_mnemonic(GTK_LABEL(self->label), ಪಠ್ಯ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_ಅಂಶ_ಗಣ_aux_widget(Vim_Menu_ಅಂಶ *self, GtkWidget *aux)
{
    self->aux_widget = aux;
    gtk_widget_ಗಣ_halign(self->aux_widget, GTK_ALIGN_ಕೊನೆ);
    gtk_widget_ಗಣ_hexpand(self->aux_widget, FALSE);
    gtk_widget_ಗಣ_margin_ಆರಂಭ(self->aux_widget, 50);
}

/*
 * Set the accelerator text for the menu item.
 */
    ಶೂನ್ಯ
vim_menu_ಅಂಶ_ಗಣ_accel(Vim_Menu_ಅಂಶ *self, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *accel_ಪಠ್ಯ)
{
    ಪ್ರತಿಜ್ಞಾ(self->aux_widget == ಖಾಲಿ);

    vim_menu_ಅಂಶ_ಗಣ_aux_widget(self, gtk_label_new(accel_ಪಠ್ಯ));
    gtk_widget_ಸೇರಿಸು_after(self->aux_widget, GTK_WIDGET(self), self->label);
}

/*
 * Set the submenu popover for the menu item
 */
    ಶೂನ್ಯ
vim_menu_ಅಂಶ_ಗಣ_submenu(Vim_Menu_ಅಂಶ *self, VimMenu *submenu)
{
    GtkWidget *icon;

    ಪ್ರತಿಜ್ಞಾ(self->submenu == ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(self->aux_widget == ಖಾಲಿ);

    // Add arrow icon pointing to right
    icon = gtk_image_new_from_icon_name("pan-end-symbolic");
    vim_menu_ಅಂಶ_ಗಣ_aux_widget(self, icon);
    gtk_widget_ಸೇರಿಸು_after(self->aux_widget, GTK_WIDGET(self), self->label);

    gtk_popover_ಗಣ_position(GTK_POPOVER(submenu), GTK_POS_ಬಲ);
    // Make top of popover be aligned with button.
    gtk_widget_ಗಣ_valign(GTK_WIDGET(submenu), GTK_ALIGN_ಆರಂಭ);

    self->submenu = GTK_WIDGET(submenu);
    gtk_widget_ಗಣ_ಪೋಷಕ(GTK_WIDGET(submenu), GTK_WIDGET(self));
}

/*
 * Create a deep copy of the menu item
 */
    ಸ್ಥಿರ GtkWidget *
vim_menu_ಅಂಶ_copy(Vim_Menu_ಅಂಶ *self)
{
    GtkWidget *copy;

    copy = vim_menu_ಅಂಶ_new(
	    gtk_label_get_ಪಠ್ಯ(GTK_LABEL(self->label)),
	    self->func, self->func_udata);

    ಆದರೆ (self->submenu != ಖಾಲಿ)
	vim_menu_ಅಂಶ_ಗಣ_submenu(VIM_MENU_ಅಂಶ(copy),
		VIM_MENU(vim_menu_copy(VIM_MENU(self->submenu))));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (self->aux_widget != ಖಾಲಿ)
	vim_menu_ಅಂಶ_ಗಣ_accel(VIM_MENU_ಅಂಶ(copy),
		gtk_label_get_ಪಠ್ಯ(GTK_LABEL(self->aux_widget)));
    ಹಿಂತಿರುಗಿ copy;
}

/*
 * Similar to GtkPopoverMenu, except uses GtkWidgets directly like GTK3, instead
 * of abstracting it into GMenuModel.
 */
ರಚನೆ _VimMenu
{
    GtkPopover ಪೋಷಕ;

    GtkWidget *box;
    GtkWidget *scr;

    GList *ವಸ್ತುಗಳು;

    // Currently active item showing submenu popover, or being hovered on, or
    // NULL. Note that item may have submenu but not have it open, when
    // navigating via keyboard.
    GtkWidget *ಸಕ್ರಿಯ_ಅಂಶ;

    // Used when mouse is hovering over an item and user is navigating with
    // keyboard. When the scrolled window scrolls down or up, this causes a
    // mouse enter event, causing the active item to go to the item that the
    // mouse is hovered on, instead of the next item (from keyboard navigation).
    gboolean	ignore_hover;
    ದ್ವಿಗುಣ	prev_x;
    ದ್ವಿಗುಣ	prev_y;
};

G_DEFINE_TYPE(VimMenu, vim_menu, GTK_TYPE_POPOVER)

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_dispose(GObject *object)
{
    VimMenu *self = VIM_MENU(object);

    g_clear_list(&self->ವಸ್ತುಗಳು, (GDestroyNotify)gtk_widget_unparent);
    g_clear_ಸೂಚಕ(&self->scr, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_menu_ಪೋಷಕ_class)->dispose(object);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_class_init(VimMenuClass *class)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(class);

    obj_class->dispose = vim_menu_dispose;
}

    ಸ್ಥಿರ gboolean
vim_menu_select_ಸಕ್ರಿಯ_ಅಂಶ(VimMenu *self, gboolean open)
{
    Vim_Menu_ಅಂಶ *ಅಂಶ;

    gtk_widget_ಗಣ_state_flags(self->ಸಕ್ರಿಯ_ಅಂಶ,
	    GTK_STATE_FLAG_SELECTED, FALSE);

    // Make sure to focus item, so that scrolled window knows what to do.
    gtk_widget_grab_focus(GTK_WIDGET(self->ಸಕ್ರಿಯ_ಅಂಶ));

    ಅಂಶ = VIM_MENU_ಅಂಶ(self->ಸಕ್ರಿಯ_ಅಂಶ);
    ಆದರೆ (ಅಂಶ->func != ಖಾಲಿ)
	ಅಂಶ->func(ಅಂಶ, VIM_MENU_ಅಂಶ_SELECTED, ಅಂಶ->func_udata);

    ಆದರೆ (open && VIM_MENU_ಅಂಶ(self->ಸಕ್ರಿಯ_ಅಂಶ)->submenu != ಖಾಲಿ)
    {
	GtkWidget *submenu = VIM_MENU_ಅಂಶ(self->ಸಕ್ರಿಯ_ಅಂಶ)->submenu;
	gtk_popover_popup(GTK_POPOVER(submenu));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Set the active item of the menu to "item". If "item" is NULL, then close any
 * submenus. If "open" is FALSE, then don't open the submenu if any.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(VimMenu *self, Vim_Menu_ಅಂಶ *ಅಂಶ, gboolean open)
{
    ಆದರೆ (self->ಸಕ್ರಿಯ_ಅಂಶ == GTK_WIDGET(ಅಂಶ))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (self->ಸಕ್ರಿಯ_ಅಂಶ != ಖಾಲಿ)
    {
	ಆದರೆ (VIM_MENU_ಅಂಶ(self->ಸಕ್ರಿಯ_ಅಂಶ)->submenu != ಖಾಲಿ)
	    gtk_popover_popdown(GTK_POPOVER(
			VIM_MENU_ಅಂಶ(self->ಸಕ್ರಿಯ_ಅಂಶ)->submenu
			));
	gtk_widget_unset_state_flags(GTK_WIDGET(self->ಸಕ್ರಿಯ_ಅಂಶ),
		GTK_STATE_FLAG_SELECTED);
    }

    self->ಸಕ್ರಿಯ_ಅಂಶ = GTK_WIDGET(ಅಂಶ);
    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
	(ಶೂನ್ಯ)vim_menu_select_ಸಕ್ರಿಯ_ಅಂಶ(self, open);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_closed_cb(VimMenu *self, ಶೂನ್ಯ *udata ಬಳಸದ)
{
    vim_menu_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(self, ಖಾಲಿ, FALSE);
}

/*
 * If "dir" is negative, then move to the item previous of currently the active
 * item. If "dir" is positive, then move to the next item. Return the resulting
 * item or NULL if there are no suitable ones.
 */
    ಸ್ಥಿರ GtkWidget *
vim_menu_move_ಸಕ್ರಿಯ_ಅಂಶ(VimMenu *self, ಪೂರ್ಣಾಂಕ dir)
{
    GtkWidget	*(*func)(GtkWidget *);
    GtkWidget	*(*null_func)(GtkWidget *);
    GtkWidget	*widget = self->ಸಕ್ರಿಯ_ಅಂಶ;

    // If there is no currently active item, then just use the first one
    ಆದರೆ (widget == ಖಾಲಿ)
    {
	widget = gtk_widget_get_ಮೊದಲ_ಮಗು(self->box);
	ಆಗ (widget != ಖಾಲಿ && !VIM_IS_MENU_ಅಂಶ(widget))
	    widget = gtk_widget_get_ಮುಂದಿನ_sibling(widget);
	ಹಿಂತಿರುಗಿ widget;
    }

    // Could also just use GList functions, but this seems simpler (no
    // difference anyways).
    ಆದರೆ (dir > 0)
    {
	func = gtk_widget_get_ಮುಂದಿನ_sibling;
	null_func = gtk_widget_get_ಮೊದಲ_ಮಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	func = gtk_widget_get_prev_sibling;
	null_func = gtk_widget_get_ಕೊನೆಯ_ಮಗು;
    }

    ಆಗ (TRUE)
    {
	widget = func(widget);
	ಆದರೆ (widget == ಖಾಲಿ)
	{
	    ಆದರೆ (null_func == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    widget = null_func(self->box);
	    null_func = ಖಾಲಿ;
	    ಆದರೆ (widget == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (VIM_IS_MENU_ಅಂಶ(widget))
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ widget;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_reset_ಪೋಷಕ_prelight(VimMenu *self)
{
    GtkWidget	*ಪೋಷಕ = gtk_widget_get_ಪೋಷಕ(GTK_WIDGET(self));
    VimMenu	*ಪೋಷಕ_menu;

    // gtk_widget_get_ancestor assumes the widget itself is also an ancestor, so
    // we must get parent of menu first.
    ಪೋಷಕ_menu = VIM_MENU(gtk_widget_get_ancestor(ಪೋಷಕ, VIM_TYPE_MENU));

    ಆದರೆ (ಪೋಷಕ_menu == ಖಾಲಿ)
	// TRUE for popup menus
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಪೋಷಕ_menu->ಸಕ್ರಿಯ_ಅಂಶ != ಖಾಲಿ)
	gtk_widget_unset_state_flags(GTK_WIDGET(ಪೋಷಕ_menu->ಸಕ್ರಿಯ_ಅಂಶ),
		GTK_STATE_FLAG_PRELIGHT);
}

/*
 * Close all submenus in the menubar given a menu widget
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_close_all(VimMenu *self)
{
    VimMenuBar *menubar = GET_MENU_BAR(self);

    // Must check if NULL, because popup menus don't have a parent.
    ಆದರೆ (menubar != ಖಾಲಿ)
	vim_menu_bar_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(menubar, ಖಾಲಿ, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
	gtk_popover_popdown(GTK_POPOVER(self));

    // Grab focus after popup menus without a menubar are closed
    gtk_widget_grab_focus(gui.drawarea);
}

    ಸ್ಥಿರ gboolean
vim_menu_ಕೀಲಿ_pressed_cb(
	GtkEventController  *controller ಬಳಸದ,
	guint		    keyval,
	guint		    keycode ಬಳಸದ,
	GdkModifierType	    state,
	VimMenu		    *self)
{
    GtkWidget	*widget;

    ಆಯ್ಕೆ (keyval)
    {
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_ಕೆಳಕ್ಕೆ:
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_KP_ಕೆಳಕ್ಕೆ:
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_ಮೇಲೆ:
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_KP_ಮೇಲೆ:
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_Tab:
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_KP_Tab:
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_ISO_ಎಡ_Tab:
	    // Go to the previous or next item if any
	    widget = vim_menu_move_ಸಕ್ರಿಯ_ಅಂಶ(self,
		    (state & GDK_ಸರಿಸು_ಮುಖವಾಡ)
		    || keyval == GDK_ಕೀಲಿ_ಮೇಲೆ ? -1 : 1);
	    vim_menu_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(self, VIM_MENU_ಅಂಶ(widget), FALSE);
	    self->ignore_hover = TRUE;
	    ಹಿಂತಿರುಗಿ TRUE;
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_ಎಡ:
	    // Pressing control switches menu bar item.
	    ಆದರೆ (state & GDK_CONTROL_ಮುಖವಾಡ)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		ಆದರೆ (menubar != ಖಾಲಿ)
		{
		    widget = vim_menu_bar_move_ಸಕ್ರಿಯ_ಅಂಶ(menubar, -1);
		    vim_menu_bar_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(menubar,
			    VIM_MENU_BAR_ಅಂಶ(widget), TRUE);
		}
		ಹಿಂತಿರುಗಿ TRUE;
	    }
	    // Go to parent menu (if any). We can do this by just closing the
	    // popover.
	    gtk_popover_popdown(GTK_POPOVER(self));
	    // For some reason when pointer is hovered over draw area, the
	    // active item in the parent menu will stay prelighted even when the
	    // active item is moved.
	    vim_menu_reset_ಪೋಷಕ_prelight(self);
	    ಹಿಂತಿರುಗಿ TRUE;
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_ಬಲ:
	    ಆದರೆ (state & GDK_CONTROL_ಮುಖವಾಡ)
	    {
		VimMenuBar *menubar = GET_MENU_BAR(self);

		ಆದರೆ (menubar != ಖಾಲಿ)
		{
		    widget = vim_menu_bar_move_ಸಕ್ರಿಯ_ಅಂಶ(menubar, 1);
		    vim_menu_bar_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(menubar,
			    VIM_MENU_BAR_ಅಂಶ(widget), TRUE);
		}
		ಹಿಂತಿರುಗಿ TRUE;
	    }
	    // Open submenu if active item has one
	    ಆದರೆ (self->ಸಕ್ರಿಯ_ಅಂಶ != ಖಾಲಿ
		    && vim_menu_select_ಸಕ್ರಿಯ_ಅಂಶ(self, TRUE))
	    {
		// Select first item in opened submenu
		VimMenu *submenu = VIM_MENU(
			VIM_MENU_ಅಂಶ(self->ಸಕ್ರಿಯ_ಅಂಶ)->submenu
			);

		vim_menu_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(submenu,
			VIM_MENU_ಅಂಶ(
			    gtk_widget_get_ಮೊದಲ_ಮಗು(submenu->box)),
			FALSE);
	    }
	    self->ignore_hover = TRUE;
	    ಹಿಂತಿರುಗಿ TRUE;
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_Escape:
	    // Close all popover menus
	    vim_menu_close_all(self);
	    ಹಿಂತಿರುಗಿ TRUE;
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_ISO_Enter:
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_3270_Enter:
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_KP_Enter:
	ಸ್ಥಿತಿ GDK_ಕೀಲಿ_Return:
	    ಆದರೆ (self->ಸಕ್ರಿಯ_ಅಂಶ != ಖಾಲಿ)
		g_signal_emit_by_name(self->ಸಕ್ರಿಯ_ಅಂಶ, "clicked");
	    ಹಿಂತಿರುಗಿ TRUE;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}


    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_motion_cb(
	GtkEventController  *controller ಬಳಸದ,
	ದ್ವಿಗುಣ		    x,
	ದ್ವಿಗುಣ		    y,
	VimMenu		    *self)
{
    ಆದರೆ (self->prev_x == -1 || self->prev_y == -1 ||
	    (fabs(self->prev_x - x) > 0.05 && fabs(self->prev_y - y) > 0.05))
	self->ignore_hover = FALSE;
    self->prev_x = x;
    self->prev_y = y;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_focus_cb(GtkEventController *controller ಬಳಸದ, VimMenu *self)
{
    gtk_popover_ಗಣ_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_notify_mnemonics_cb(
	VimMenu	    *self,
	GParamSpec  *pspec ಬಳಸದ,
	ಶೂನ್ಯ	    *udata ಬಳಸದ)
{
    gtk_popover_ಗಣ_mnemonics_visible(GTK_POPOVER(self), TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_init(VimMenu *self)
{
    GtkEventController	*controller;
    GtkWidget		*ರಾಶಿ;
    GtkWidget		*ಪೋಷಕ_box;
    GListModel		*controllers;

    gtk_popover_ಗಣ_has_arrow(GTK_POPOVER(self), FALSE);
    gtk_popover_ಗಣ_autohide(GTK_POPOVER(self), TRUE);

    // Do not make child popovers close parent popovers when they are closed.
    gtk_popover_ಗಣ_cascade_popdown(GTK_POPOVER(self), FALSE);

    ರಾಶಿ = gtk_ರಾಶಿ_new();

    // "stack" and "parent_box" have no use other than to make the css structure
    // of the popup menu be exactly like GtkPopoverMenu. This is so that GTK
    // themes style VimMenu exactly like GtkPopoverMenu.
    ಪೋಷಕ_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_ರಾಶಿ_ಕೂಡಿಸು_ಮಗು(GTK_ರಾಶಿ(ರಾಶಿ), ಪೋಷಕ_box);
    gtk_ರಾಶಿ_ಗಣ_visible_ಮಗು(GTK_ರಾಶಿ(ರಾಶಿ), ಪೋಷಕ_box);

    self->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_ಗಣ_hexpand(self->box, TRUE);
    gtk_widget_ಗಣ_vexpand(self->box, TRUE);
    gtk_box_append(GTK_BOX(ಪೋಷಕ_box), self->box);

    self->scr = gtk_scrolled_ಕಿಟಕಿ_new();
    gtk_scrolled_ಕಿಟಕಿ_ಗಣ_policy(GTK_SCROLLED_ಕಿಟಕಿ(self->scr),
	    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_ಕಿಟಕಿ_ಗಣ_propagate_natural_ಅಗಲ(
	    GTK_SCROLLED_ಕಿಟಕಿ(self->scr), TRUE);
    gtk_scrolled_ಕಿಟಕಿ_ಗಣ_propagate_natural_ಎತ್ತರ(
	    GTK_SCROLLED_ಕಿಟಕಿ(self->scr), TRUE);

    gtk_scrolled_ಕಿಟಕಿ_ಗಣ_ಮಗು(GTK_SCROLLED_ಕಿಟಕಿ(self->scr), ರಾಶಿ);
    gtk_popover_ಗಣ_ಮಗು(GTK_POPOVER(self), self->scr);

    gtk_widget_ಕೂಡಿಸು_css_class(GTK_WIDGET(self), "menu");

    // Add key controller for basic movement
    controller = gtk_event_controller_ಕೀಲಿ_new();
    // Make sure we get the key presses first and handle them if possible
    gtk_event_controller_ಗಣ_ಪ್ರಸರಣ_phase(controller,
	    GTK_PHASE_CAPTURE);
    g_signal_connect_object(controller, "key-pressed",
	    G_CALLBACK(vim_menu_ಕೀಲಿ_pressed_cb),
	    self, G_CONNECT_DEFAULT);
    gtk_widget_ಕೂಡಿಸು_controller(GTK_WIDGET(self), controller);

    // Show mnemonic underline always
    controller = gtk_event_controller_focus_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_focus_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_ಕೂಡಿಸು_controller(GTK_WIDGET(self), controller);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "motion",
	    G_CALLBACK(vim_menu_motion_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_ಕೂಡಿಸು_controller(GTK_WIDGET(self), controller);

    g_signal_connect(self, "closed", G_CALLBACK(vim_menu_closed_cb), ಖಾಲಿ);

    // Set all shortcut controllers in the window to not require a modifier for
    // mnemonics.
    controllers = gtk_widget_observe_controllers(GTK_WIDGET(self));
    ಗಾಗಿ (guint i = 0; i < g_list_model_get_n_ವಸ್ತುಗಳು(controllers); i++)
    {
	controller = g_list_model_get_ಅಂಶ(controllers, i);
	ಆದರೆ (GTK_IS_SHORTCUT_CONTROLLER(controller))
	    gtk_shortcut_controller_ಗಣ_mnemonics_modifiers(
		    GTK_SHORTCUT_CONTROLLER(controller), 0);
    }
    g_object_unref(controllers);

    // Force enable mnemonics underline
    g_signal_connect(self, "notify::mnemonics-visible",
	    G_CALLBACK(vim_menu_notify_mnemonics_cb), ಖಾಲಿ);

    self->prev_x = self->prev_y = -1;
}

    GtkWidget *
vim_menu_new(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ g_object_new(VIM_TYPE_MENU, ಖಾಲಿ);
}

    ಸ್ಥಿರ GtkWidget *
vim_menu_ಸೇರಿಸು(VimMenu *self, GtkWidget *ಅಂಶ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx > 0)
    {
	GList *prev = g_list_nth(self->ವಸ್ತುಗಳು, idx - 1);
	ಆದರೆ (prev == ಖಾಲಿ)
	    gtk_box_append(GTK_BOX(self->box), ಅಂಶ);
	ಇಲ್ಲದಿದ್ದರೆ
	    gtk_box_ಸೇರಿಸು_ಮಗು_after(GTK_BOX(self->box), ಅಂಶ,
		    prev->ದತ್ತಾಂಶ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == 0)
	gtk_box_prepend(GTK_BOX(self->box), ಅಂಶ);
    ಇಲ್ಲದಿದ್ದರೆ
	gtk_box_append(GTK_BOX(self->box), ಅಂಶ);

    self->ವಸ್ತುಗಳು = g_list_ಸೇರಿಸು(self->ವಸ್ತುಗಳು, ಅಂಶ, idx);
    ಹಿಂತಿರುಗಿ ಅಂಶ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_ಅಂಶ_clicked_cb(Vim_Menu_ಅಂಶ *self, VimMenu *menu)
{
    // Only close all menus if item is a regular button (no submenu). If item
    // has a submenu, then just toggle it on and off.
    ಆದರೆ (self->submenu != ಖಾಲಿ)
    {
	ಆದರೆ (gtk_widget_is_visible(self->submenu))
	    gtk_popover_popdown(GTK_POPOVER(self->submenu));
	ಇಲ್ಲದಿದ್ದರೆ
	    gtk_popover_popup(GTK_POPOVER(self->submenu));
	ಹಿಂತಿರುಗಿ;
    }

    // Since we set the "cascade-popdown" property to FALSE, we must popdown the
    // toplevel menu/popover, so that all submenus are closed.
    vim_menu_close_all(menu);
    ಆದರೆ (self->func != ಖಾಲಿ)
	self->func(self, VIM_MENU_ಅಂಶ_CLICKED, self->func_udata);
}

    ಸ್ಥಿರ gboolean
vim_menu_ಅಂಶ_mnemonic_activate_cb(
	Vim_Menu_ಅಂಶ *self,
	gboolean    ಗುಂಪು ಬಳಸದ,
	VimMenu	    *menu)
{
    vim_menu_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(menu, self, FALSE);
    // Return FALSE so that menu item emits "clicked" signal
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_ಅಂಶ_enter_cb(
	GtkEventController  *controller,
	ದ್ವಿಗುಣ		    x ಬಳಸದ,
	ದ್ವಿಗುಣ		    y ಬಳಸದ,
	VimMenu		    *menu)
{
    Vim_Menu_ಅಂಶ  *self;

    ಆದರೆ (menu->ignore_hover || !gtk_event_controller_motion_ಒಳಗೊಂಡಿದೆ_ಸೂಚಕ(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	ಹಿಂತಿರುಗಿ;

    self = VIM_MENU_ಅಂಶ(gtk_event_controller_get_widget(controller));
    vim_menu_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(menu, self, TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_menu_ಅಂಶ_leave_cb(GtkEventController *controller, VimMenu *menu)
{
    Vim_Menu_ಅಂಶ  *self;

    ಆದರೆ (gtk_event_controller_motion_ಒಳಗೊಂಡಿದೆ_ಸೂಚಕ(
		GTK_EVENT_CONTROLLER_MOTION(controller)))
	ಹಿಂತಿರುಗಿ;

    self = VIM_MENU_ಅಂಶ(gtk_event_controller_get_widget(controller));
    ಆದರೆ (menu->ಸಕ್ರಿಯ_ಅಂಶ == GTK_WIDGET(self))
	vim_menu_ಗಣ_ಸಕ್ರಿಯ_ಅಂಶ(menu, ಖಾಲಿ, FALSE);
}

/*
 * Insert the menu item at the given index in the menu. If "idx" is negative,
 * then append the menu item.
 */
    ಶೂನ್ಯ
vim_menu_ಸೇರಿಸು_ಅಂಶ(VimMenu *self, Vim_Menu_ಅಂಶ *ಅಂಶ, ಪೂರ್ಣಾಂಕ idx)
{
    GtkEventController *controller;

    vim_menu_ಸೇರಿಸು(self, GTK_WIDGET(ಅಂಶ), idx);

    controller = gtk_event_controller_motion_new();
    g_signal_connect_object(controller, "enter",
	    G_CALLBACK(vim_menu_ಅಂಶ_enter_cb), self, G_CONNECT_DEFAULT);
    g_signal_connect_object(controller, "leave",
	    G_CALLBACK(vim_menu_ಅಂಶ_leave_cb), self, G_CONNECT_DEFAULT);
    gtk_widget_ಕೂಡಿಸು_controller(GTK_WIDGET(ಅಂಶ), controller);

    g_signal_connect_object(ಅಂಶ, "clicked",
	    G_CALLBACK(vim_menu_ಅಂಶ_clicked_cb),
	    self, G_CONNECT_DEFAULT);
    g_signal_connect_object(ಅಂಶ, "mnemonic-activate",
	    G_CALLBACK(vim_menu_ಅಂಶ_mnemonic_activate_cb),
	    self, G_CONNECT_DEFAULT);
}

/*
 * Insert a separator at the given position and return it.
 */
    GtkWidget *
vim_menu_ಸೇರಿಸು_separator(VimMenu *self, ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ vim_menu_ಸೇರಿಸು(self,
	    gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), idx);
}

/*
 * Remove the menu item or separator from the menu
 */
    ಶೂನ್ಯ
vim_menu_remove(VimMenu *self, GtkWidget *ಅಂಶ)
{
    self->ವಸ್ತುಗಳು = g_list_remove(self->ವಸ್ತುಗಳು, ಅಂಶ);
    gtk_box_remove(GTK_BOX(self->box), ಅಂಶ);
}

/*
 * Create a deep copy of the menu
 */
    GtkWidget *
vim_menu_copy(VimMenu *self)
{
    GtkWidget	*copy = vim_menu_new();
    ಪೂರ್ಣಾಂಕ		i = 0;

    ಗಾಗಿ (GList *l = self->ವಸ್ತುಗಳು; l != ಖಾಲಿ; l = l->ಮುಂದಿನ, i++)
    {
	Vim_Menu_ಅಂಶ *ಅಂಶ;
	GtkWidget   *ಅಂಶ_copy;

	ಆದರೆ (!VIM_IS_MENU_ಅಂಶ(l->ದತ್ತಾಂಶ))
	{
	    ಪ್ರತಿಜ್ಞಾ(GTK_IS_SEPARATOR(l->ದತ್ತಾಂಶ));
	    vim_menu_ಸೇರಿಸು_separator(VIM_MENU(copy), i);
	    ಮುಂದುವರಿ;
	}

	ಅಂಶ = l->ದತ್ತಾಂಶ;
	ಅಂಶ_copy = vim_menu_ಅಂಶ_copy(ಅಂಶ);

	vim_menu_ಸೇರಿಸು_ಅಂಶ(VIM_MENU(copy), VIM_MENU_ಅಂಶ(ಅಂಶ_copy), i);
    }
    ಹಿಂತಿರುಗಿ copy;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU
