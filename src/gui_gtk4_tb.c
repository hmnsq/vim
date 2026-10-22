/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR

#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>
#ಅನ್ತರ್ಭಾವ "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
ರಚನೆ _VimToolbar
{
    GtkWidget ಪೋಷಕ;

    ಪೂರ್ಣಾಂಕ style; // TOOLBAR_* flags
    ಪೂರ್ಣಾಂಕ iconsize; // TBIS_* values

    GList *ವಸ್ತುಗಳು;

    GtkWidget	*ಬೇರು;
    GtkWidget	*strip;

    GtkWidget	*ಅತಿಪ್ರವಾಹ_btn;
    GtkWidget	*ಅತಿಪ್ರವಾಹ_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

ಸ್ಥಿರ ಶೂನ್ಯ vim_toolbar_ಗಾತ್ರ_allocate(GtkWidget *widget, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಪೂರ್ಣಾಂಕ baseline);
ಸ್ಥಿರ ಶೂನ್ಯ vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, ಪೂರ್ಣಾಂಕ for_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ *minimum, ಪೂರ್ಣಾಂಕ *natural, ಪೂರ್ಣಾಂಕ *minimum_baseline, ಪೂರ್ಣಾಂಕ *natural_baseline);

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->ವಸ್ತುಗಳು, g_object_unref);
    g_clear_ಸೂಚಕ(&self->ಬೇರು, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_ಪೋಷಕ_class)->dispose(object);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->ಗಾತ್ರ_allocate = vim_toolbar_ಗಾತ್ರ_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_ಗಣ_css_name(widget_class, "toolbar");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->ಬೇರು = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_ಗಣ_ಪೋಷಕ(self->ಬೇರು, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_ಗಣ_hexpand(self->strip, TRUE);
    gtk_widget_ಗಣ_halign(self->strip, GTK_ALIGN_ಆರಂಭ);
    gtk_widget_ಗಣ_ಅತಿಪ್ರವಾಹ(self->strip, GTK_ಅತಿಪ್ರವಾಹ_HIDDEN);
    gtk_box_append(GTK_BOX(self->ಬೇರು), self->strip);

    self->ಅತಿಪ್ರವಾಹ_btn = gtk_menu_button_new();
    gtk_widget_ಗಣ_hexpand(self->ಅತಿಪ್ರವಾಹ_btn, FALSE);
    gtk_widget_ಗಣ_halign(self->ಅತಿಪ್ರವಾಹ_btn, GTK_ALIGN_ಕೊನೆ);
    gtk_widget_ಕೂಡಿಸು_css_class(self->ಅತಿಪ್ರವಾಹ_btn, "flat");
    gtk_box_append(GTK_BOX(self->ಬೇರು), self->ಅತಿಪ್ರವಾಹ_btn);
    g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(self->ಅತಿಪ್ರವಾಹ_btn),
	    "toolbar-width", GINT_ಗಮ್ಯ_ಸೂಚಕ(-1));

    popover = gtk_popover_new();
    self->ಅತಿಪ್ರವಾಹ_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_ಗಣ_ಮಗು(GTK_POPOVER(popover), self->ಅತಿಪ್ರವಾಹ_box);
    gtk_popover_ಗಣ_has_arrow(GTK_POPOVER(popover), FALSE);
    // Make sure popover aligns to top right of button
    gtk_widget_ಗಣ_halign(popover, GTK_ALIGN_ಕೊನೆ);

    gtk_menu_button_ಗಣ_popover(GTK_MENU_BUTTON(self->ಅತಿಪ್ರವಾಹ_btn), popover);
}

    GtkWidget *
vim_toolbar_new(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ g_object_new(VIM_TYPE_TOOLBAR, ಖಾಲಿ);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_ಸೇರಿಸು(VimToolbar *self, GtkWidget *widget, ಪೂರ್ಣಾಂಕ idx)
{
    GtkWidget	*cur_ಮಗು = gtk_widget_get_ಮೊದಲ_ಮಗು(self->strip);
    ಪೂರ್ಣಾಂಕ		i = 0;

    ಆಗ (cur_ಮಗು != ಖಾಲಿ && i != idx - 1)
    {
	cur_ಮಗು = gtk_widget_get_ಮುಂದಿನ_sibling(cur_ಮಗು);
	i++;
    }

    gtk_box_ಸೇರಿಸು_ಮಗು_after(GTK_BOX(self->strip), widget, cur_ಮಗು);
    self->ವಸ್ತುಗಳು = g_list_ಸೇರಿಸು(self->ವಸ್ತುಗಳು, g_object_ref(widget), idx);
    gtk_widget_ಸರತಿ_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_button_style(GtkWidget *btn, ಪೂರ್ಣಾಂಕ style, ಪೂರ್ಣಾಂಕ iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    ಆದರೆ (GTK_IS_SEPARATOR(btn))
	ಹಿಂತಿರುಗಿ;

    box = gtk_button_get_ಮಗು(GTK_BUTTON(btn));
    ಆದರೆ (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_ಗಣ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_ಗಣ_spacing(GTK_BOX(box), 8);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gtk_orientable_ಗಣ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_ಗಣ_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_ಗಣ_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_ದತ್ತಾಂಶ(G_OBJECT(btn), "icon");
    ಆದರೆ (icon != ಖಾಲಿ)
    {
	ಆದರೆ (style & TOOLBAR_ICONS)
	{
	    Gtk_Icon_ಗಾತ್ರ ಗಾತ್ರ_2;

	    ಆಯ್ಕೆ (iconsize)
	    {
		ಸ್ಥಿತಿ TBIS_TINY:
		ಸ್ಥಿತಿ TBIS_SMALL:
		ಸ್ಥಿತಿ TBIS_MEDIUM:
		    ಗಾತ್ರ_2 = GTK_ICON_ಗಾತ್ರ_ಸಾಮಾನ್ಯ;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ TBIS_LARGE:
		ಸ್ಥಿತಿ TBIS_HUGE:
		ಸ್ಥಿತಿ TBIS_GIANT:
		    ಗಾತ್ರ_2 = GTK_ICON_ಗಾತ್ರ_LARGE;
		    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    ಗಾತ್ರ_2 = GTK_ICON_ಗಾತ್ರ_ಸಾಮಾನ್ಯ;
		    ನಿಲ್ಲಿಸು;
	    }
	    gtk_image_ಗಣ_icon_ಗಾತ್ರ(GTK_IMAGE(icon), ಗಾತ್ರ_2);
	    gtk_widget_ಗಣ_visible(icon, TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    gtk_widget_ಗಣ_visible(icon, FALSE);
    }

    label = g_object_get_ದತ್ತಾಂಶ(G_OBJECT(btn), "label");
    ಆದರೆ (label != ಖಾಲಿ)
	gtk_widget_ಗಣ_visible(label, style & TOOLBAR_ಪಠ್ಯ);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    ಆದರೆ (invalidate)
	g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(btn), "toolbar-width", GINT_ಗಮ್ಯ_ಸೂಚಕ(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
button_clicked(GtkWidget *widget ಬಳಸದ, VimToolbar *self)
{
    gtk_menu_button_popdown(GTK_MENU_BUTTON(self->ಅತಿಪ್ರವಾಹ_btn));
    // Bring focus to drawarea, popping overflow menu down does not change
    // focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_mch_flush();
}

/*
 * Add a new toolbar button at the given index and return the widget. "icon" and
 * "text" may be NULL..
 */
    GtkWidget *
vim_toolbar_ಸೇರಿಸು_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *ಪಠ್ಯ,
	ಪೂರ್ಣಾಂಕ	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_ಕೂಡಿಸು_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_ಗಣ_ಮಗು(GTK_BUTTON(btn), box);

    ಆದರೆ (icon != ಖಾಲಿ)
    {
	gtk_widget_ಗಣ_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_ಗಣ_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(btn), "icon", icon);
    }
    ಆದರೆ (ಪಠ್ಯ != ಖಾಲಿ)
    {
	GtkWidget *label = gtk_label_new(ಪಠ್ಯ);

	gtk_widget_ಗಣ_valign (label, GTK_ALIGN_CENTER);
	gtk_widget_ಗಣ_vexpand (label, TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(btn), "label", label);
    }

    ಗಣ_button_style(btn, self->style, self->iconsize, TRUE);

    g_signal_connect_object(btn, "clicked",
	    G_CALLBACK(button_clicked), self, G_CONNECT_DEFAULT);

    vim_toolbar_ಸೇರಿಸು(self, btn, idx);
    ಹಿಂತಿರುಗಿ btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_ಸೇರಿಸು_separator(VimToolbar *self, ಪೂರ್ಣಾಂಕ idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(sep), "toolbar-width", GINT_ಗಮ್ಯ_ಸೂಚಕ(-1));
    vim_toolbar_ಸೇರಿಸು(self, sep, idx);
    ಹಿಂತಿರುಗಿ sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    ಶೂನ್ಯ
vim_toolbar_ಗಣ_style(VimToolbar *self, ಪೂರ್ಣಾಂಕ style, ಪೂರ್ಣಾಂಕ iconsize)
{
    GtkWidget *cur_ಮಗು = gtk_widget_get_ಮೊದಲ_ಮಗು(self->strip);

    ಆದರೆ (style == self->style && self->iconsize == iconsize)
	ಹಿಂತಿರುಗಿ;

    self->style = style;
    self->iconsize = iconsize;

    ಆಗ (cur_ಮಗು != ಖಾಲಿ)
    {
	ಆದರೆ (!GTK_IS_SEPARATOR(cur_ಮಗು))
	    ಗಣ_button_style(cur_ಮಗು, style, iconsize, TRUE);
	cur_ಮಗು = gtk_widget_get_ಮುಂದಿನ_sibling(cur_ಮಗು);
    }

    cur_ಮಗು = gtk_widget_get_ಮೊದಲ_ಮಗು(self->ಅತಿಪ್ರವಾಹ_box);
    ಆಗ (cur_ಮಗು != ಖಾಲಿ)
    {
	ಆದರೆ (!GTK_IS_SEPARATOR(cur_ಮಗು))
	    ಗಣ_button_style(cur_ಮಗು, style, iconsize, TRUE);
	cur_ಮಗು = gtk_widget_get_ಮುಂದಿನ_sibling(cur_ಮಗು);
    }
    // Sizes may have changed
    gtk_widget_ಸರತಿ_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    ಶೂನ್ಯ
vim_toolbar_remove(VimToolbar *self, GtkWidget *ಅಂಶ)
{
    ಆದರೆ (gtk_widget_get_ಪೋಷಕ(ಅಂಶ) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), ಅಂಶ);
    ಇಲ್ಲದಿದ್ದರೆ
	gtk_box_remove(GTK_BOX(self->ಅತಿಪ್ರವಾಹ_box), ಅಂಶ);

    self->ವಸ್ತುಗಳು = g_list_remove(self->ವಸ್ತುಗಳು, ಅಂಶ);
    g_object_unref(ಅಂಶ);
    gtk_widget_ಸರತಿ_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_move_ಅಂಶ_ಗಮ್ಯ(
	VimToolbar  *self,
	GtkWidget   *ಅಂಶ,
	gboolean    ಅತಿಪ್ರವಾಹ)
{
    GtkBox *from, *ಗಮ್ಯ;

    ಗಮ್ಯ = ಅತಿಪ್ರವಾಹ ? GTK_BOX(self->ಅತಿಪ್ರವಾಹ_box) : GTK_BOX(self->strip);
    ಆದರೆ (gtk_widget_get_ಪೋಷಕ(ಅಂಶ) == GTK_WIDGET(ಗಮ್ಯ))
	ಹಿಂತಿರುಗಿ;

    from = ಅತಿಪ್ರವಾಹ ? GTK_BOX(self->strip) : GTK_BOX(self->ಅತಿಪ್ರವಾಹ_box);

    g_object_ref(ಅಂಶ);
    gtk_box_remove(from, ಅಂಶ);
    gtk_box_append(ಗಮ್ಯ, ಅಂಶ);
    g_object_unref(ಅಂಶ);

    ಆದರೆ (GTK_IS_SEPARATOR(ಅಂಶ))
	gtk_widget_ಗಣ_visible(ಅಂಶ, !ಅತಿಪ್ರವಾಹ);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಅತಿಪ್ರವಾಹ)
	{
	    ಪೂರ್ಣಾಂಕ style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_ಪಠ್ಯ;
	    style |= TOOLBAR_ICONS;
	    ಗಣ_button_style(ಅಂಶ, style, self->iconsize, FALSE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಣ_button_style(ಅಂಶ, self->style, self->iconsize, FALSE);
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಅಂಶ_ಅಗಲ(GtkWidget *ಅಂಶ)
{
    ಪೂರ್ಣಾಂಕ toolbar_ಅಗಲ = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_ಅಗಲ = GPOINTER_ಗಮ್ಯ_INT(
	    g_object_get_ದತ್ತಾಂಶ(G_OBJECT(ಅಂಶ), "toolbar-width"));

    ಆದರೆ (toolbar_ಅಗಲ == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_ಗಾತ್ರ(ಅಂಶ, &min_req, &nat_req);
	toolbar_ಅಗಲ = MAX(min_req.ಅಗಲ, nat_req.ಅಗಲ);

	// Save width for later
	g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(ಅಂಶ),
		"toolbar-width", GINT_ಗಮ್ಯ_ಸೂಚಕ(toolbar_ಅಗಲ));
    }
    ಹಿಂತಿರುಗಿ toolbar_ಅಗಲ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_ಗಾತ್ರ_allocate(
	GtkWidget   *widget,
	ಪೂರ್ಣಾಂಕ	    ಅಗಲ,
	ಪೂರ್ಣಾಂಕ	    ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    Gtk_ಹಂಚಿಕೆ   alloc;
    ಪೂರ್ಣಾಂಕ		    ಬಳಸಿದ = 0;
    ಪೂರ್ಣಾಂಕ		    avail;
    GList	    *ಅತಿಪ್ರವಾಹ_ele = ಖಾಲಿ;
    GtkWidget	    *ಮಗು;

    avail = ಅಗಲ - get_ಅಂಶ_ಅಗಲ(self->ಅತಿಪ್ರವಾಹ_btn);

    ಗಾಗಿ (GList *ele = self->ವಸ್ತುಗಳು; ele != ಖಾಲಿ; ele = ele->ಮುಂದಿನ)
    {
	GtkWidget   *ಅಂಶ = ele->ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ	    toolbar_ಅಗಲ = get_ಅಂಶ_ಅಗಲ(ಅಂಶ);

	ಆದರೆ (ಬಳಸಿದ + toolbar_ಅಗಲ > avail)
	{
	    ಅತಿಪ್ರವಾಹ_ele = ele;
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_toolbar_move_ಅಂಶ_ಗಮ್ಯ(self, ಅಂಶ, FALSE);
	ಬಳಸಿದ += toolbar_ಅಗಲ;
    }

    ಆದರೆ (ಅತಿಪ್ರವಾಹ_ele != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	    ಉಳಿದ = ಅಗಲ - ಬಳಸಿದ;
	gboolean    need_ಅತಿಪ್ರವಾಹ_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	ಆಗ ((ಮಗು = gtk_widget_get_ಮೊದಲ_ಮಗು(self->ಅತಿಪ್ರವಾಹ_box))
		!= ಖಾಲಿ)
	    vim_toolbar_move_ಅಂಶ_ಗಮ್ಯ(self, ಮಗು, FALSE);

	ಗಾಗಿ (GList *ele = ಅತಿಪ್ರವಾಹ_ele; ele != ಖಾಲಿ; ele = ele->ಮುಂದಿನ)
	{
	    ಉಳಿದ -= get_ಅಂಶ_ಅಗಲ(ele->ದತ್ತಾಂಶ);
	    ಆದರೆ (ಉಳಿದ < 0)
	    {
		need_ಅತಿಪ್ರವಾಹ_btn = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}

	ಆದರೆ (need_ಅತಿಪ್ರವಾಹ_btn)
	    ಗಾಗಿ (; ಅತಿಪ್ರವಾಹ_ele != ಖಾಲಿ; ಅತಿಪ್ರವಾಹ_ele = ಅತಿಪ್ರವಾಹ_ele->ಮುಂದಿನ)
		vim_toolbar_move_ಅಂಶ_ಗಮ್ಯ(self, ಅತಿಪ್ರವಾಹ_ele->ದತ್ತಾಂಶ, TRUE);
	gtk_widget_ಗಣ_visible(self->ಅತಿಪ್ರವಾಹ_btn, need_ಅತಿಪ್ರವಾಹ_btn);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	gtk_widget_ಗಣ_visible(self->ಅತಿಪ್ರವಾಹ_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.ಅಗಲ = ಅಗಲ;
    alloc.ಎತ್ತರ = ಎತ್ತರ;

    gtk_widget_ಗಾತ್ರ_allocate(self->ಬೇರು, &alloc, baseline);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	ಪೂರ್ಣಾಂಕ		for_ಗಾತ್ರ,
	ಪೂರ್ಣಾಂಕ		*min,
	ಪೂರ್ಣಾಂಕ		*nat,
	ಪೂರ್ಣಾಂಕ		*min_baseline,
	ಪೂರ್ಣಾಂಕ		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    ಪೂರ್ಣಾಂಕ		strip_min = 0, strip_nat = 0;
    ಪೂರ್ಣಾಂಕ		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_ಗಾತ್ರ,
	    &strip_min, &strip_nat, ಖಾಲಿ, ಖಾಲಿ);
    gtk_widget_measure(self->ಅತಿಪ್ರವಾಹ_btn, orientation, for_ಗಾತ್ರ,
	    &obtn_min, &obtn_nat, ಖಾಲಿ, ಖಾಲಿ);

    ಆದರೆ (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	ಆದರೆ (min != ಖಾಲಿ)
	    *min = obtn_min;
	ಆದರೆ (nat != ಖಾಲಿ)
	    *nat = strip_nat + obtn_nat;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (min != ಖಾಲಿ)
	    *min = MAX(strip_min, obtn_min);
	ಆದರೆ (nat != ಖಾಲಿ)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    ಆದರೆ (min_baseline != ಖಾಲಿ)
	*min_baseline = -1;
    ಆದರೆ (nat_baseline != ಖಾಲಿ)
	*nat_baseline = -1;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_TOOLBAR
