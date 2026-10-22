/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR

#ਅਨ੍ਤਰ੍ਭਾਵ <gtk/gtk.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
ਬਣਤਰ _VimToolbar
{
    GtkWidget ਮਾਪਾ;

    ਪੂਰਨ style; // TOOLBAR_* flags
    ਪੂਰਨ iconsize; // TBIS_* values

    GList *ਚੀਜ਼ਾਂ;

    GtkWidget	*ਜੜ੍ਹ;
    GtkWidget	*strip;

    GtkWidget	*ਉਛਾਲ_btn;
    GtkWidget	*ਉਛਾਲ_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

ਸਥਿਰ ਖਾਲੀ vim_toolbar_ਮਾਪ_allocate(GtkWidget *widget, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ baseline);
ਸਥਿਰ ਖਾਲੀ vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, ਪੂਰਨ for_ਮਾਪ, ਪੂਰਨ *minimum, ਪੂਰਨ *natural, ਪੂਰਨ *minimum_baseline, ਪੂਰਨ *natural_baseline);

    ਸਥਿਰ ਖਾਲੀ
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->ਚੀਜ਼ਾਂ, g_object_unref);
    g_clear_ਸੂਚਕ(&self->ਜੜ੍ਹ, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_ਮਾਪਾ_class)->dispose(object);
}

    ਸਥਿਰ ਖਾਲੀ
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->ਮਾਪ_allocate = vim_toolbar_ਮਾਪ_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_ਸੈੱਟ_css_name(widget_class, "toolbar");
}

    ਸਥਿਰ ਖਾਲੀ
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->ਜੜ੍ਹ = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_ਸੈੱਟ_ਮਾਪਾ(self->ਜੜ੍ਹ, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_ਸੈੱਟ_hexpand(self->strip, TRUE);
    gtk_widget_ਸੈੱਟ_halign(self->strip, GTK_ALIGN_ਸ਼ੁਰੂ);
    gtk_widget_ਸੈੱਟ_ਉਛਾਲ(self->strip, GTK_ਉਛਾਲ_HIDDEN);
    gtk_box_append(GTK_BOX(self->ਜੜ੍ਹ), self->strip);

    self->ਉਛਾਲ_btn = gtk_menu_button_new();
    gtk_widget_ਸੈੱਟ_hexpand(self->ਉਛਾਲ_btn, FALSE);
    gtk_widget_ਸੈੱਟ_halign(self->ਉਛਾਲ_btn, GTK_ALIGN_ਸਿਰਾ);
    gtk_widget_ਜੋੜੋ_css_class(self->ਉਛਾਲ_btn, "flat");
    gtk_box_append(GTK_BOX(self->ਜੜ੍ਹ), self->ਉਛਾਲ_btn);
    g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(self->ਉਛਾਲ_btn),
	    "toolbar-width", GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(-1));

    popover = gtk_popover_new();
    self->ਉਛਾਲ_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_ਸੈੱਟ_ਬੱਚਾ(GTK_POPOVER(popover), self->ਉਛਾਲ_box);
    gtk_popover_ਸੈੱਟ_has_arrow(GTK_POPOVER(popover), FALSE);
    // Make sure popover aligns to top right of button
    gtk_widget_ਸੈੱਟ_halign(popover, GTK_ALIGN_ਸਿਰਾ);

    gtk_menu_button_ਸੈੱਟ_popover(GTK_MENU_BUTTON(self->ਉਛਾਲ_btn), popover);
}

    GtkWidget *
vim_toolbar_new(ਖਾਲੀ)
{
    ਵਾਪਸ g_object_new(VIM_TYPE_TOOLBAR, ਨਿਰਾ);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    ਸਥਿਰ ਖਾਲੀ
vim_toolbar_ਪਾਓ(VimToolbar *self, GtkWidget *widget, ਪੂਰਨ idx)
{
    GtkWidget	*cur_ਬੱਚਾ = gtk_widget_get_ਪਹਿਲਾ_ਬੱਚਾ(self->strip);
    ਪੂਰਨ		i = 0;

    ਜਦੋਂ (cur_ਬੱਚਾ != ਨਿਰਾ && i != idx - 1)
    {
	cur_ਬੱਚਾ = gtk_widget_get_ਅਗਲਾ_sibling(cur_ਬੱਚਾ);
	i++;
    }

    gtk_box_ਪਾਓ_ਬੱਚਾ_after(GTK_BOX(self->strip), widget, cur_ਬੱਚਾ);
    self->ਚੀਜ਼ਾਂ = g_list_ਪਾਓ(self->ਚੀਜ਼ਾਂ, g_object_ref(widget), idx);
    gtk_widget_ਕਤਾਰ_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_button_style(GtkWidget *btn, ਪੂਰਨ style, ਪੂਰਨ iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    ਜੇ (GTK_IS_SEPARATOR(btn))
	ਵਾਪਸ;

    box = gtk_button_get_ਬੱਚਾ(GTK_BUTTON(btn));
    ਜੇ (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_ਸੈੱਟ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_ਸੈੱਟ_spacing(GTK_BOX(box), 8);
    }
    ਨਹੀਂਤਾਂ
    {
	gtk_orientable_ਸੈੱਟ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_ਸੈੱਟ_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_ਸੈੱਟ_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_ਅੰਕੜੇ(G_OBJECT(btn), "icon");
    ਜੇ (icon != ਨਿਰਾ)
    {
	ਜੇ (style & TOOLBAR_ICONS)
	{
	    Gtk_Icon_ਮਾਪ ਮਾਪ;

	    ਚੋਣ (iconsize)
	    {
		ਹਾਲਤ TBIS_TINY:
		ਹਾਲਤ TBIS_SMALL:
		ਹਾਲਤ TBIS_MEDIUM:
		    ਮਾਪ = GTK_ICON_ਮਾਪ_ਸਾਧਾਰਨ;
		    ਰੋਕੋ;
		ਹਾਲਤ TBIS_LARGE:
		ਹਾਲਤ TBIS_HUGE:
		ਹਾਲਤ TBIS_GIANT:
		    ਮਾਪ = GTK_ICON_ਮਾਪ_LARGE;
		    ਰੋਕੋ;
		ਮੂਲ:
		    ਮਾਪ = GTK_ICON_ਮਾਪ_ਸਾਧਾਰਨ;
		    ਰੋਕੋ;
	    }
	    gtk_image_ਸੈੱਟ_icon_ਮਾਪ(GTK_IMAGE(icon), ਮਾਪ);
	    gtk_widget_ਸੈੱਟ_visible(icon, TRUE);
	}
	ਨਹੀਂਤਾਂ
	    gtk_widget_ਸੈੱਟ_visible(icon, FALSE);
    }

    label = g_object_get_ਅੰਕੜੇ(G_OBJECT(btn), "label");
    ਜੇ (label != ਨਿਰਾ)
	gtk_widget_ਸੈੱਟ_visible(label, style & TOOLBAR_ਪਾਠ);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    ਜੇ (invalidate)
	g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(btn), "toolbar-width", GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    ਸਥਿਰ ਖਾਲੀ
button_clicked(GtkWidget *widget ਅਣਵਰਤਿਆ, VimToolbar *self)
{
    gtk_menu_button_popdown(GTK_MENU_BUTTON(self->ਉਛਾਲ_btn));
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
vim_toolbar_ਪਾਓ_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ਅਚਲ ਅੱਖਰ  *ਪਾਠ,
	ਪੂਰਨ	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_ਜੋੜੋ_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_ਸੈੱਟ_ਬੱਚਾ(GTK_BUTTON(btn), box);

    ਜੇ (icon != ਨਿਰਾ)
    {
	gtk_widget_ਸੈੱਟ_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_ਸੈੱਟ_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(btn), "icon", icon);
    }
    ਜੇ (ਪਾਠ != ਨਿਰਾ)
    {
	GtkWidget *label = gtk_label_new(ਪਾਠ);

	gtk_widget_ਸੈੱਟ_valign (label, GTK_ALIGN_CENTER);
	gtk_widget_ਸੈੱਟ_vexpand (label, TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(btn), "label", label);
    }

    ਸੈੱਟ_button_style(btn, self->style, self->iconsize, TRUE);

    g_signal_connect_object(btn, "clicked",
	    G_CALLBACK(button_clicked), self, G_CONNECT_DEFAULT);

    vim_toolbar_ਪਾਓ(self, btn, idx);
    ਵਾਪਸ btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_ਪਾਓ_separator(VimToolbar *self, ਪੂਰਨ idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(sep), "toolbar-width", GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(-1));
    vim_toolbar_ਪਾਓ(self, sep, idx);
    ਵਾਪਸ sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    ਖਾਲੀ
vim_toolbar_ਸੈੱਟ_style(VimToolbar *self, ਪੂਰਨ style, ਪੂਰਨ iconsize)
{
    GtkWidget *cur_ਬੱਚਾ = gtk_widget_get_ਪਹਿਲਾ_ਬੱਚਾ(self->strip);

    ਜੇ (style == self->style && self->iconsize == iconsize)
	ਵਾਪਸ;

    self->style = style;
    self->iconsize = iconsize;

    ਜਦੋਂ (cur_ਬੱਚਾ != ਨਿਰਾ)
    {
	ਜੇ (!GTK_IS_SEPARATOR(cur_ਬੱਚਾ))
	    ਸੈੱਟ_button_style(cur_ਬੱਚਾ, style, iconsize, TRUE);
	cur_ਬੱਚਾ = gtk_widget_get_ਅਗਲਾ_sibling(cur_ਬੱਚਾ);
    }

    cur_ਬੱਚਾ = gtk_widget_get_ਪਹਿਲਾ_ਬੱਚਾ(self->ਉਛਾਲ_box);
    ਜਦੋਂ (cur_ਬੱਚਾ != ਨਿਰਾ)
    {
	ਜੇ (!GTK_IS_SEPARATOR(cur_ਬੱਚਾ))
	    ਸੈੱਟ_button_style(cur_ਬੱਚਾ, style, iconsize, TRUE);
	cur_ਬੱਚਾ = gtk_widget_get_ਅਗਲਾ_sibling(cur_ਬੱਚਾ);
    }
    // Sizes may have changed
    gtk_widget_ਕਤਾਰ_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    ਖਾਲੀ
vim_toolbar_remove(VimToolbar *self, GtkWidget *ਵਸਤੂ)
{
    ਜੇ (gtk_widget_get_ਮਾਪਾ(ਵਸਤੂ) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), ਵਸਤੂ);
    ਨਹੀਂਤਾਂ
	gtk_box_remove(GTK_BOX(self->ਉਛਾਲ_box), ਵਸਤੂ);

    self->ਚੀਜ਼ਾਂ = g_list_remove(self->ਚੀਜ਼ਾਂ, ਵਸਤੂ);
    g_object_unref(ਵਸਤੂ);
    gtk_widget_ਕਤਾਰ_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    ਸਥਿਰ ਖਾਲੀ
vim_toolbar_move_ਵਸਤੂ_ਮੰਜ਼ਿਲ(
	VimToolbar  *self,
	GtkWidget   *ਵਸਤੂ,
	gboolean    ਉਛਾਲ)
{
    GtkBox *from, *ਮੰਜ਼ਿਲ;

    ਮੰਜ਼ਿਲ = ਉਛਾਲ ? GTK_BOX(self->ਉਛਾਲ_box) : GTK_BOX(self->strip);
    ਜੇ (gtk_widget_get_ਮਾਪਾ(ਵਸਤੂ) == GTK_WIDGET(ਮੰਜ਼ਿਲ))
	ਵਾਪਸ;

    from = ਉਛਾਲ ? GTK_BOX(self->strip) : GTK_BOX(self->ਉਛਾਲ_box);

    g_object_ref(ਵਸਤੂ);
    gtk_box_remove(from, ਵਸਤੂ);
    gtk_box_append(ਮੰਜ਼ਿਲ, ਵਸਤੂ);
    g_object_unref(ਵਸਤੂ);

    ਜੇ (GTK_IS_SEPARATOR(ਵਸਤੂ))
	gtk_widget_ਸੈੱਟ_visible(ਵਸਤੂ, !ਉਛਾਲ);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਉਛਾਲ)
	{
	    ਪੂਰਨ style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_ਪਾਠ;
	    style |= TOOLBAR_ICONS;
	    ਸੈੱਟ_button_style(ਵਸਤੂ, style, self->iconsize, FALSE);
	}
	ਨਹੀਂਤਾਂ
	    ਸੈੱਟ_button_style(ਵਸਤੂ, self->style, self->iconsize, FALSE);
    }
}

    ਸਥਿਰ ਪੂਰਨ
get_ਵਸਤੂ_ਚੌੜਾਈ(GtkWidget *ਵਸਤੂ)
{
    ਪੂਰਨ toolbar_ਚੌੜਾਈ = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_ਚੌੜਾਈ = GPOINTER_ਮੰਜ਼ਿਲ_INT(
	    g_object_get_ਅੰਕੜੇ(G_OBJECT(ਵਸਤੂ), "toolbar-width"));

    ਜੇ (toolbar_ਚੌੜਾਈ == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_ਮਾਪ(ਵਸਤੂ, &min_req, &nat_req);
	toolbar_ਚੌੜਾਈ = MAX(min_req.ਚੌੜਾਈ, nat_req.ਚੌੜਾਈ);

	// Save width for later
	g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(ਵਸਤੂ),
		"toolbar-width", GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(toolbar_ਚੌੜਾਈ));
    }
    ਵਾਪਸ toolbar_ਚੌੜਾਈ;
}

    ਸਥਿਰ ਖਾਲੀ
vim_toolbar_ਮਾਪ_allocate(
	GtkWidget   *widget,
	ਪੂਰਨ	    ਚੌੜਾਈ,
	ਪੂਰਨ	    ਉਚਾਈ,
	ਪੂਰਨ	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    Gtk_ਵੰਡ   alloc;
    ਪੂਰਨ		    ਵਰਤਿਆ = 0;
    ਪੂਰਨ		    avail;
    GList	    *ਉਛਾਲ_ele = ਨਿਰਾ;
    GtkWidget	    *ਬੱਚਾ;

    avail = ਚੌੜਾਈ - get_ਵਸਤੂ_ਚੌੜਾਈ(self->ਉਛਾਲ_btn);

    ਲਈ (GList *ele = self->ਚੀਜ਼ਾਂ; ele != ਨਿਰਾ; ele = ele->ਅਗਲਾ)
    {
	GtkWidget   *ਵਸਤੂ = ele->ਅੰਕੜੇ;
	ਪੂਰਨ	    toolbar_ਚੌੜਾਈ = get_ਵਸਤੂ_ਚੌੜਾਈ(ਵਸਤੂ);

	ਜੇ (ਵਰਤਿਆ + toolbar_ਚੌੜਾਈ > avail)
	{
	    ਉਛਾਲ_ele = ele;
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	    vim_toolbar_move_ਵਸਤੂ_ਮੰਜ਼ਿਲ(self, ਵਸਤੂ, FALSE);
	ਵਰਤਿਆ += toolbar_ਚੌੜਾਈ;
    }

    ਜੇ (ਉਛਾਲ_ele != ਨਿਰਾ)
    {
	ਪੂਰਨ	    ਬਚਿਆ = ਚੌੜਾਈ - ਵਰਤਿਆ;
	gboolean    need_ਉਛਾਲ_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	ਜਦੋਂ ((ਬੱਚਾ = gtk_widget_get_ਪਹਿਲਾ_ਬੱਚਾ(self->ਉਛਾਲ_box))
		!= ਨਿਰਾ)
	    vim_toolbar_move_ਵਸਤੂ_ਮੰਜ਼ਿਲ(self, ਬੱਚਾ, FALSE);

	ਲਈ (GList *ele = ਉਛਾਲ_ele; ele != ਨਿਰਾ; ele = ele->ਅਗਲਾ)
	{
	    ਬਚਿਆ -= get_ਵਸਤੂ_ਚੌੜਾਈ(ele->ਅੰਕੜੇ);
	    ਜੇ (ਬਚਿਆ < 0)
	    {
		need_ਉਛਾਲ_btn = TRUE;
		ਰੋਕੋ;
	    }
	}

	ਜੇ (need_ਉਛਾਲ_btn)
	    ਲਈ (; ਉਛਾਲ_ele != ਨਿਰਾ; ਉਛਾਲ_ele = ਉਛਾਲ_ele->ਅਗਲਾ)
		vim_toolbar_move_ਵਸਤੂ_ਮੰਜ਼ਿਲ(self, ਉਛਾਲ_ele->ਅੰਕੜੇ, TRUE);
	gtk_widget_ਸੈੱਟ_visible(self->ਉਛਾਲ_btn, need_ਉਛਾਲ_btn);
    }
    ਨਹੀਂਤਾਂ
	gtk_widget_ਸੈੱਟ_visible(self->ਉਛਾਲ_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.ਚੌੜਾਈ = ਚੌੜਾਈ;
    alloc.ਉਚਾਈ = ਉਚਾਈ;

    gtk_widget_ਮਾਪ_allocate(self->ਜੜ੍ਹ, &alloc, baseline);
}

    ਸਥਿਰ ਖਾਲੀ
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	ਪੂਰਨ		for_ਮਾਪ,
	ਪੂਰਨ		*min,
	ਪੂਰਨ		*nat,
	ਪੂਰਨ		*min_baseline,
	ਪੂਰਨ		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    ਪੂਰਨ		strip_min = 0, strip_nat = 0;
    ਪੂਰਨ		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_ਮਾਪ,
	    &strip_min, &strip_nat, ਨਿਰਾ, ਨਿਰਾ);
    gtk_widget_measure(self->ਉਛਾਲ_btn, orientation, for_ਮਾਪ,
	    &obtn_min, &obtn_nat, ਨਿਰਾ, ਨਿਰਾ);

    ਜੇ (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	ਜੇ (min != ਨਿਰਾ)
	    *min = obtn_min;
	ਜੇ (nat != ਨਿਰਾ)
	    *nat = strip_nat + obtn_nat;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (min != ਨਿਰਾ)
	    *min = MAX(strip_min, obtn_min);
	ਜੇ (nat != ਨਿਰਾ)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    ਜੇ (min_baseline != ਨਿਰਾ)
	*min_baseline = -1;
    ਜੇ (nat_baseline != ਨਿਰਾ)
	*nat_baseline = -1;
}

#ਜੇ_ਅੰਤ // FEAT_TOOLBAR
