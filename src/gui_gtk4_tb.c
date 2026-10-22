/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef FEAT_TOOLBAR

#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>
#ಅನ್ತರ್ಭಾವ "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
ಸಂರಚನಾ _VimToolbar
{
    GtkWidget parent;

    ಪೂರ್ಣಾಙ್ಕ style; // TOOLBAR_* flags
    ಪೂರ್ಣಾಙ್ಕ iconsize; // TBIS_* values

    GList *ವಸ್ತುಗಳು;

    GtkWidget	*ಬೇರು;
    GtkWidget	*strip;

    GtkWidget	*overflow_btn;
    GtkWidget	*overflow_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

ಸ್ಥಿರ ಶೂನ್ಯ vim_toolbar_size_allocate(GtkWidget *widget, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height, ಪೂರ್ಣಾಙ್ಕ baseline);
ಸ್ಥಿರ ಶೂನ್ಯ vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, ಪೂರ್ಣಾಙ್ಕ for_size, ಪೂರ್ಣಾಙ್ಕ *minimum, ಪೂರ್ಣಾಙ್ಕ *natural, ಪೂರ್ಣಾಙ್ಕ *minimum_baseline, ಪೂರ್ಣಾಙ್ಕ *natural_baseline);

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->ವಸ್ತುಗಳು, g_object_unref);
    g_clear_pointer(&self->ಬೇರು, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_parent_class)->dispose(object);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->size_allocate = vim_toolbar_size_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_set_css_name(widget_class, "toolbar");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->ಬೇರು = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_parent(self->ಬೇರು, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(self->strip, TRUE);
    gtk_widget_set_halign(self->strip, GTK_ALIGN_START);
    gtk_widget_set_overflow(self->strip, GTK_OVERFLOW_HIDDEN);
    gtk_box_append(GTK_BOX(self->ಬೇರು), self->strip);

    self->overflow_btn = gtk_menu_button_new();
    gtk_widget_set_hexpand(self->overflow_btn, FALSE);
    gtk_widget_set_halign(self->overflow_btn, GTK_ALIGN_END);
    gtk_widget_add_css_class(self->overflow_btn, "flat");
    gtk_box_append(GTK_BOX(self->ಬೇರು), self->overflow_btn);
    g_object_set_data(G_OBJECT(self->overflow_btn),
	    "toolbar-width", GINT_TO_POINTER(-1));

    popover = gtk_popover_new();
    self->overflow_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_set_child(GTK_POPOVER(popover), self->overflow_box);
    gtk_popover_set_has_arrow(GTK_POPOVER(popover), FALSE);
    // Make sure popover aligns to top right of button
    gtk_widget_set_halign(popover, GTK_ALIGN_END);

    gtk_menu_button_set_popover(GTK_MENU_BUTTON(self->overflow_btn), popover);
}

    GtkWidget *
vim_toolbar_new(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ g_object_new(VIM_TYPE_TOOLBAR, ರಿಕ್ತ);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_ಸೇರಿಸು(VimToolbar *self, GtkWidget *widget, ಪೂರ್ಣಾಙ್ಕ idx)
{
    GtkWidget	*cur_child = gtk_widget_get_first_child(self->strip);
    ಪೂರ್ಣಾಙ್ಕ		i = 0;

    ಯಾವತ್ (cur_child != ರಿಕ್ತ && i != idx - 1)
    {
	cur_child = gtk_widget_get_next_sibling(cur_child);
	i++;
    }

    gtk_box_ಸೇರಿಸು_child_after(GTK_BOX(self->strip), widget, cur_child);
    self->ವಸ್ತುಗಳು = g_list_ಸೇರಿಸು(self->ವಸ್ತುಗಳು, g_object_ref(widget), idx);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_button_style(GtkWidget *btn, ಪೂರ್ಣಾಙ್ಕ style, ಪೂರ್ಣಾಙ್ಕ iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    ಯದಿ (GTK_IS_SEPARATOR(btn))
	ಪ್ರತ್ಯಾಗಮ;

    box = gtk_button_get_child(GTK_BUTTON(btn));
    ಯದಿ (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(box), 8);
    }
    ಅನ್ಯಥಾ
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_set_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_set_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_data(G_OBJECT(btn), "icon");
    ಯದಿ (icon != ರಿಕ್ತ)
    {
	ಯದಿ (style & TOOLBAR_ICONS)
	{
	    GtkIconSize size;

	    ವಿಕಲ್ಪ (iconsize)
	    {
		ಪಕ್ಷ TBIS_TINY:
		ಪಕ್ಷ TBIS_SMALL:
		ಪಕ್ಷ TBIS_MEDIUM:
		    size = GTK_ICON_SIZE_NORMAL;
		    ಭಙ್ಗ;
		ಪಕ್ಷ TBIS_LARGE:
		ಪಕ್ಷ TBIS_HUGE:
		ಪಕ್ಷ TBIS_GIANT:
		    size = GTK_ICON_SIZE_LARGE;
		    ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    size = GTK_ICON_SIZE_NORMAL;
		    ಭಙ್ಗ;
	    }
	    gtk_image_set_icon_size(GTK_IMAGE(icon), size);
	    gtk_widget_set_visible(icon, TRUE);
	}
	ಅನ್ಯಥಾ
	    gtk_widget_set_visible(icon, FALSE);
    }

    label = g_object_get_data(G_OBJECT(btn), "label");
    ಯದಿ (label != ರಿಕ್ತ)
	gtk_widget_set_visible(label, style & TOOLBAR_TEXT);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    ಯದಿ (invalidate)
	g_object_set_data(G_OBJECT(btn), "toolbar-width", GINT_TO_POINTER(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
button_clicked(GtkWidget *widget UNUSED, VimToolbar *self)
{
    gtk_menu_button_popdown(GTK_MENU_BUTTON(self->overflow_btn));
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
	ಧ್ರುವ ವರ್ಣ  *text,
	ಪೂರ್ಣಾಙ್ಕ	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_add_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_set_child(GTK_BUTTON(btn), box);

    ಯದಿ (icon != ರಿಕ್ತ)
    {
	gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_set_data(G_OBJECT(btn), "icon", icon);
    }
    ಯದಿ (text != ರಿಕ್ತ)
    {
	GtkWidget *label = gtk_label_new(text);

	gtk_widget_set_valign (label, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (label, TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_object_set_data(G_OBJECT(btn), "label", label);
    }

    set_button_style(btn, self->style, self->iconsize, TRUE);

    g_signal_connect_object(btn, "clicked",
	    G_CALLBACK(button_clicked), self, G_CONNECT_DEFAULT);

    vim_toolbar_ಸೇರಿಸು(self, btn, idx);
    ಪ್ರತ್ಯಾಗಮ btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_ಸೇರಿಸು_separator(VimToolbar *self, ಪೂರ್ಣಾಙ್ಕ idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_set_data(G_OBJECT(sep), "toolbar-width", GINT_TO_POINTER(-1));
    vim_toolbar_ಸೇರಿಸು(self, sep, idx);
    ಪ್ರತ್ಯಾಗಮ sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    ಶೂನ್ಯ
vim_toolbar_set_style(VimToolbar *self, ಪೂರ್ಣಾಙ್ಕ style, ಪೂರ್ಣಾಙ್ಕ iconsize)
{
    GtkWidget *cur_child = gtk_widget_get_first_child(self->strip);

    ಯದಿ (style == self->style && self->iconsize == iconsize)
	ಪ್ರತ್ಯಾಗಮ;

    self->style = style;
    self->iconsize = iconsize;

    ಯಾವತ್ (cur_child != ರಿಕ್ತ)
    {
	ಯದಿ (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }

    cur_child = gtk_widget_get_first_child(self->overflow_box);
    ಯಾವತ್ (cur_child != ರಿಕ್ತ)
    {
	ಯದಿ (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }
    // Sizes may have changed
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    ಶೂನ್ಯ
vim_toolbar_remove(VimToolbar *self, GtkWidget *item)
{
    ಯದಿ (gtk_widget_get_parent(item) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), item);
    ಅನ್ಯಥಾ
	gtk_box_remove(GTK_BOX(self->overflow_box), item);

    self->ವಸ್ತುಗಳು = g_list_remove(self->ವಸ್ತುಗಳು, item);
    g_object_unref(item);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_move_item_to(
	VimToolbar  *self,
	GtkWidget   *item,
	gboolean    overflow)
{
    GtkBox *from, *to;

    to = overflow ? GTK_BOX(self->overflow_box) : GTK_BOX(self->strip);
    ಯದಿ (gtk_widget_get_parent(item) == GTK_WIDGET(to))
	ಪ್ರತ್ಯಾಗಮ;

    from = overflow ? GTK_BOX(self->strip) : GTK_BOX(self->overflow_box);

    g_object_ref(item);
    gtk_box_remove(from, item);
    gtk_box_append(to, item);
    g_object_unref(item);

    ಯದಿ (GTK_IS_SEPARATOR(item))
	gtk_widget_set_visible(item, !overflow);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (overflow)
	{
	    ಪೂರ್ಣಾಙ್ಕ style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_TEXT;
	    style |= TOOLBAR_ICONS;
	    set_button_style(item, style, self->iconsize, FALSE);
	}
	ಅನ್ಯಥಾ
	    set_button_style(item, self->style, self->iconsize, FALSE);
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_item_width(GtkWidget *item)
{
    ಪೂರ್ಣಾಙ್ಕ toolbar_width = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_width = GPOINTER_TO_INT(
	    g_object_get_data(G_OBJECT(item), "toolbar-width"));

    ಯದಿ (toolbar_width == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_size(item, &min_req, &nat_req);
	toolbar_width = MAX(min_req.width, nat_req.width);

	// Save width for later
	g_object_set_data(G_OBJECT(item),
		"toolbar-width", GINT_TO_POINTER(toolbar_width));
    }
    ಪ್ರತ್ಯಾಗಮ toolbar_width;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_size_allocate(
	GtkWidget   *widget,
	ಪೂರ್ಣಾಙ್ಕ	    width,
	ಪೂರ್ಣಾಙ್ಕ	    height,
	ಪೂರ್ಣಾಙ್ಕ	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    GtkAllocation   alloc;
    ಪೂರ್ಣಾಙ್ಕ		    used = 0;
    ಪೂರ್ಣಾಙ್ಕ		    avail;
    GList	    *overflow_ele = ರಿಕ್ತ;
    GtkWidget	    *child;

    avail = width - get_item_width(self->overflow_btn);

    ಪ್ರತಿ (GList *ele = self->ವಸ್ತುಗಳು; ele != ರಿಕ್ತ; ele = ele->next)
    {
	GtkWidget   *item = ele->data;
	ಪೂರ್ಣಾಙ್ಕ	    toolbar_width = get_item_width(item);

	ಯದಿ (used + toolbar_width > avail)
	{
	    overflow_ele = ele;
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	    vim_toolbar_move_item_to(self, item, FALSE);
	used += toolbar_width;
    }

    ಯದಿ (overflow_ele != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	    remaining = width - used;
	gboolean    need_overflow_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	ಯಾವತ್ ((child = gtk_widget_get_first_child(self->overflow_box))
		!= ರಿಕ್ತ)
	    vim_toolbar_move_item_to(self, child, FALSE);

	ಪ್ರತಿ (GList *ele = overflow_ele; ele != ರಿಕ್ತ; ele = ele->next)
	{
	    remaining -= get_item_width(ele->data);
	    ಯದಿ (remaining < 0)
	    {
		need_overflow_btn = TRUE;
		ಭಙ್ಗ;
	    }
	}

	ಯದಿ (need_overflow_btn)
	    ಪ್ರತಿ (; overflow_ele != ರಿಕ್ತ; overflow_ele = overflow_ele->next)
		vim_toolbar_move_item_to(self, overflow_ele->data, TRUE);
	gtk_widget_set_visible(self->overflow_btn, need_overflow_btn);
    }
    ಅನ್ಯಥಾ
	gtk_widget_set_visible(self->overflow_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.width = width;
    alloc.height = height;

    gtk_widget_size_allocate(self->ಬೇರು, &alloc, baseline);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	ಪೂರ್ಣಾಙ್ಕ		for_size,
	ಪೂರ್ಣಾಙ್ಕ		*min,
	ಪೂರ್ಣಾಙ್ಕ		*nat,
	ಪೂರ್ಣಾಙ್ಕ		*min_baseline,
	ಪೂರ್ಣಾಙ್ಕ		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    ಪೂರ್ಣಾಙ್ಕ		strip_min = 0, strip_nat = 0;
    ಪೂರ್ಣಾಙ್ಕ		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_size,
	    &strip_min, &strip_nat, ರಿಕ್ತ, ರಿಕ್ತ);
    gtk_widget_measure(self->overflow_btn, orientation, for_size,
	    &obtn_min, &obtn_nat, ರಿಕ್ತ, ರಿಕ್ತ);

    ಯದಿ (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	ಯದಿ (min != ರಿಕ್ತ)
	    *min = obtn_min;
	ಯದಿ (nat != ರಿಕ್ತ)
	    *nat = strip_nat + obtn_nat;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (min != ರಿಕ್ತ)
	    *min = MAX(strip_min, obtn_min);
	ಯದಿ (nat != ರಿಕ್ತ)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    ಯದಿ (min_baseline != ರಿಕ್ತ)
	*min_baseline = -1;
    ಯದಿ (nat_baseline != ರಿಕ್ತ)
	*nat_baseline = -1;
}

#endif // FEAT_TOOLBAR
