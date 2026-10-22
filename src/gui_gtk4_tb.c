/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"

#ifdef FEAT_TOOLBAR

#अन्तर्भाव <gtk/gtk.h>
#अन्तर्भाव "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
संरचना _VimToolbar
{
    GtkWidget parent;

    पूर्णाङ्क style; // TOOLBAR_* flags
    पूर्णाङ्क iconsize; // TBIS_* values

    GList *वस्तुएँ;

    GtkWidget	*मूल;
    GtkWidget	*strip;

    GtkWidget	*overflow_btn;
    GtkWidget	*overflow_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

स्थिर शून्य vim_toolbar_size_allocate(GtkWidget *widget, पूर्णाङ्क width, पूर्णाङ्क height, पूर्णाङ्क baseline);
स्थिर शून्य vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, पूर्णाङ्क for_size, पूर्णाङ्क *minimum, पूर्णाङ्क *natural, पूर्णाङ्क *minimum_baseline, पूर्णाङ्क *natural_baseline);

    स्थिर शून्य
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->वस्तुएँ, g_object_unref);
    g_clear_pointer(&self->मूल, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_parent_class)->dispose(object);
}

    स्थिर शून्य
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->size_allocate = vim_toolbar_size_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_set_css_name(widget_class, "toolbar");
}

    स्थिर शून्य
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->मूल = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_parent(self->मूल, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(self->strip, TRUE);
    gtk_widget_set_halign(self->strip, GTK_ALIGN_START);
    gtk_widget_set_overflow(self->strip, GTK_OVERFLOW_HIDDEN);
    gtk_box_append(GTK_BOX(self->मूल), self->strip);

    self->overflow_btn = gtk_menu_button_new();
    gtk_widget_set_hexpand(self->overflow_btn, FALSE);
    gtk_widget_set_halign(self->overflow_btn, GTK_ALIGN_END);
    gtk_widget_add_css_class(self->overflow_btn, "flat");
    gtk_box_append(GTK_BOX(self->मूल), self->overflow_btn);
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
vim_toolbar_new(शून्य)
{
    प्रत्यागम g_object_new(VIM_TYPE_TOOLBAR, रिक्त);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    स्थिर शून्य
vim_toolbar_जोड़ो(VimToolbar *self, GtkWidget *widget, पूर्णाङ्क idx)
{
    GtkWidget	*cur_child = gtk_widget_get_first_child(self->strip);
    पूर्णाङ्क		i = 0;

    यावत् (cur_child != रिक्त && i != idx - 1)
    {
	cur_child = gtk_widget_get_next_sibling(cur_child);
	i++;
    }

    gtk_box_जोड़ो_child_after(GTK_BOX(self->strip), widget, cur_child);
    self->वस्तुएँ = g_list_जोड़ो(self->वस्तुएँ, g_object_ref(widget), idx);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    स्थिर शून्य
set_button_style(GtkWidget *btn, पूर्णाङ्क style, पूर्णाङ्क iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    यदि (GTK_IS_SEPARATOR(btn))
	प्रत्यागम;

    box = gtk_button_get_child(GTK_BUTTON(btn));
    यदि (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(box), 8);
    }
    अन्यथा
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_set_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_set_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_data(G_OBJECT(btn), "icon");
    यदि (icon != रिक्त)
    {
	यदि (style & TOOLBAR_ICONS)
	{
	    GtkIconSize size;

	    विकल्प (iconsize)
	    {
		पक्ष TBIS_TINY:
		पक्ष TBIS_SMALL:
		पक्ष TBIS_MEDIUM:
		    size = GTK_ICON_SIZE_NORMAL;
		    भङ्ग;
		पक्ष TBIS_LARGE:
		पक्ष TBIS_HUGE:
		पक्ष TBIS_GIANT:
		    size = GTK_ICON_SIZE_LARGE;
		    भङ्ग;
		स्वभाव:
		    size = GTK_ICON_SIZE_NORMAL;
		    भङ्ग;
	    }
	    gtk_image_set_icon_size(GTK_IMAGE(icon), size);
	    gtk_widget_set_visible(icon, TRUE);
	}
	अन्यथा
	    gtk_widget_set_visible(icon, FALSE);
    }

    label = g_object_get_data(G_OBJECT(btn), "label");
    यदि (label != रिक्त)
	gtk_widget_set_visible(label, style & TOOLBAR_TEXT);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    यदि (invalidate)
	g_object_set_data(G_OBJECT(btn), "toolbar-width", GINT_TO_POINTER(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    स्थिर शून्य
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
vim_toolbar_जोड़ो_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ध्रुव वर्ण  *text,
	पूर्णाङ्क	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_add_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_set_child(GTK_BUTTON(btn), box);

    यदि (icon != रिक्त)
    {
	gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_set_data(G_OBJECT(btn), "icon", icon);
    }
    यदि (text != रिक्त)
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

    vim_toolbar_जोड़ो(self, btn, idx);
    प्रत्यागम btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_जोड़ो_separator(VimToolbar *self, पूर्णाङ्क idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_set_data(G_OBJECT(sep), "toolbar-width", GINT_TO_POINTER(-1));
    vim_toolbar_जोड़ो(self, sep, idx);
    प्रत्यागम sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    शून्य
vim_toolbar_set_style(VimToolbar *self, पूर्णाङ्क style, पूर्णाङ्क iconsize)
{
    GtkWidget *cur_child = gtk_widget_get_first_child(self->strip);

    यदि (style == self->style && self->iconsize == iconsize)
	प्रत्यागम;

    self->style = style;
    self->iconsize = iconsize;

    यावत् (cur_child != रिक्त)
    {
	यदि (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }

    cur_child = gtk_widget_get_first_child(self->overflow_box);
    यावत् (cur_child != रिक्त)
    {
	यदि (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }
    // Sizes may have changed
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    शून्य
vim_toolbar_remove(VimToolbar *self, GtkWidget *item)
{
    यदि (gtk_widget_get_parent(item) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), item);
    अन्यथा
	gtk_box_remove(GTK_BOX(self->overflow_box), item);

    self->वस्तुएँ = g_list_remove(self->वस्तुएँ, item);
    g_object_unref(item);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    स्थिर शून्य
vim_toolbar_move_item_to(
	VimToolbar  *self,
	GtkWidget   *item,
	gboolean    overflow)
{
    GtkBox *from, *to;

    to = overflow ? GTK_BOX(self->overflow_box) : GTK_BOX(self->strip);
    यदि (gtk_widget_get_parent(item) == GTK_WIDGET(to))
	प्रत्यागम;

    from = overflow ? GTK_BOX(self->strip) : GTK_BOX(self->overflow_box);

    g_object_ref(item);
    gtk_box_remove(from, item);
    gtk_box_append(to, item);
    g_object_unref(item);

    यदि (GTK_IS_SEPARATOR(item))
	gtk_widget_set_visible(item, !overflow);
    अन्यथा
    {
	यदि (overflow)
	{
	    पूर्णाङ्क style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_TEXT;
	    style |= TOOLBAR_ICONS;
	    set_button_style(item, style, self->iconsize, FALSE);
	}
	अन्यथा
	    set_button_style(item, self->style, self->iconsize, FALSE);
    }
}

    स्थिर पूर्णाङ्क
get_item_width(GtkWidget *item)
{
    पूर्णाङ्क toolbar_width = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_width = GPOINTER_TO_INT(
	    g_object_get_data(G_OBJECT(item), "toolbar-width"));

    यदि (toolbar_width == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_size(item, &min_req, &nat_req);
	toolbar_width = MAX(min_req.width, nat_req.width);

	// Save width for later
	g_object_set_data(G_OBJECT(item),
		"toolbar-width", GINT_TO_POINTER(toolbar_width));
    }
    प्रत्यागम toolbar_width;
}

    स्थिर शून्य
vim_toolbar_size_allocate(
	GtkWidget   *widget,
	पूर्णाङ्क	    width,
	पूर्णाङ्क	    height,
	पूर्णाङ्क	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    GtkAllocation   alloc;
    पूर्णाङ्क		    used = 0;
    पूर्णाङ्क		    avail;
    GList	    *overflow_ele = रिक्त;
    GtkWidget	    *child;

    avail = width - get_item_width(self->overflow_btn);

    प्रति (GList *ele = self->वस्तुएँ; ele != रिक्त; ele = ele->next)
    {
	GtkWidget   *item = ele->data;
	पूर्णाङ्क	    toolbar_width = get_item_width(item);

	यदि (used + toolbar_width > avail)
	{
	    overflow_ele = ele;
	    भङ्ग;
	}
	अन्यथा
	    vim_toolbar_move_item_to(self, item, FALSE);
	used += toolbar_width;
    }

    यदि (overflow_ele != रिक्त)
    {
	पूर्णाङ्क	    remaining = width - used;
	gboolean    need_overflow_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	यावत् ((child = gtk_widget_get_first_child(self->overflow_box))
		!= रिक्त)
	    vim_toolbar_move_item_to(self, child, FALSE);

	प्रति (GList *ele = overflow_ele; ele != रिक्त; ele = ele->next)
	{
	    remaining -= get_item_width(ele->data);
	    यदि (remaining < 0)
	    {
		need_overflow_btn = TRUE;
		भङ्ग;
	    }
	}

	यदि (need_overflow_btn)
	    प्रति (; overflow_ele != रिक्त; overflow_ele = overflow_ele->next)
		vim_toolbar_move_item_to(self, overflow_ele->data, TRUE);
	gtk_widget_set_visible(self->overflow_btn, need_overflow_btn);
    }
    अन्यथा
	gtk_widget_set_visible(self->overflow_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.width = width;
    alloc.height = height;

    gtk_widget_size_allocate(self->मूल, &alloc, baseline);
}

    स्थिर शून्य
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	पूर्णाङ्क		for_size,
	पूर्णाङ्क		*min,
	पूर्णाङ्क		*nat,
	पूर्णाङ्क		*min_baseline,
	पूर्णाङ्क		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    पूर्णाङ्क		strip_min = 0, strip_nat = 0;
    पूर्णाङ्क		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_size,
	    &strip_min, &strip_nat, रिक्त, रिक्त);
    gtk_widget_measure(self->overflow_btn, orientation, for_size,
	    &obtn_min, &obtn_nat, रिक्त, रिक्त);

    यदि (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	यदि (min != रिक्त)
	    *min = obtn_min;
	यदि (nat != रिक्त)
	    *nat = strip_nat + obtn_nat;
    }
    अन्यथा
    {
	यदि (min != रिक्त)
	    *min = MAX(strip_min, obtn_min);
	यदि (nat != रिक्त)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    यदि (min_baseline != रिक्त)
	*min_baseline = -1;
    यदि (nat_baseline != रिक्त)
	*nat_baseline = -1;
}

#endif // FEAT_TOOLBAR
