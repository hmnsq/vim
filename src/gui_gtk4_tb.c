/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#અન્તર્ભાવ "vim.h"

#ifdef FEAT_TOOLBAR

#અન્તર્ભાવ <gtk/gtk.h>
#અન્તર્ભાવ "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
સંરચના _VimToolbar
{
    GtkWidget parent;

    પૂર્ણાંક style; // TOOLBAR_* flags
    પૂર્ણાંક iconsize; // TBIS_* values

    GList *વસ્તુઓ;

    GtkWidget	*મૂળ;
    GtkWidget	*strip;

    GtkWidget	*overflow_btn;
    GtkWidget	*overflow_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

સ્થિર શૂન્ય vim_toolbar_size_allocate(GtkWidget *widget, પૂર્ણાંક width, પૂર્ણાંક height, પૂર્ણાંક baseline);
સ્થિર શૂન્ય vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, પૂર્ણાંક for_size, પૂર્ણાંક *minimum, પૂર્ણાંક *natural, પૂર્ણાંક *minimum_baseline, પૂર્ણાંક *natural_baseline);

    સ્થિર શૂન્ય
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->વસ્તુઓ, g_object_unref);
    g_clear_pointer(&self->મૂળ, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_parent_class)->dispose(object);
}

    સ્થિર શૂન્ય
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->size_allocate = vim_toolbar_size_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_set_css_name(widget_class, "toolbar");
}

    સ્થિર શૂન્ય
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->મૂળ = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_parent(self->મૂળ, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(self->strip, TRUE);
    gtk_widget_set_halign(self->strip, GTK_ALIGN_START);
    gtk_widget_set_overflow(self->strip, GTK_OVERFLOW_HIDDEN);
    gtk_box_append(GTK_BOX(self->મૂળ), self->strip);

    self->overflow_btn = gtk_menu_button_new();
    gtk_widget_set_hexpand(self->overflow_btn, FALSE);
    gtk_widget_set_halign(self->overflow_btn, GTK_ALIGN_END);
    gtk_widget_add_css_class(self->overflow_btn, "flat");
    gtk_box_append(GTK_BOX(self->મૂળ), self->overflow_btn);
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
vim_toolbar_new(શૂન્ય)
{
    પાછા g_object_new(VIM_TYPE_TOOLBAR, ખાલી);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    સ્થિર શૂન્ય
vim_toolbar_ઉમેરો(VimToolbar *self, GtkWidget *widget, પૂર્ણાંક idx)
{
    GtkWidget	*cur_child = gtk_widget_get_first_child(self->strip);
    પૂર્ણાંક		i = 0;

    જ્યારે (cur_child != ખાલી && i != idx - 1)
    {
	cur_child = gtk_widget_get_next_sibling(cur_child);
	i++;
    }

    gtk_box_ઉમેરો_child_after(GTK_BOX(self->strip), widget, cur_child);
    self->વસ્તુઓ = g_list_ઉમેરો(self->વસ્તુઓ, g_object_ref(widget), idx);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    સ્થિર શૂન્ય
set_button_style(GtkWidget *btn, પૂર્ણાંક style, પૂર્ણાંક iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    જો (GTK_IS_SEPARATOR(btn))
	પાછા;

    box = gtk_button_get_child(GTK_BUTTON(btn));
    જો (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(box), 8);
    }
    નહીંતર
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_set_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_set_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_data(G_OBJECT(btn), "icon");
    જો (icon != ખાલી)
    {
	જો (style & TOOLBAR_ICONS)
	{
	    GtkIconSize size;

	    પસંદગી (iconsize)
	    {
		સ્થિતિ TBIS_TINY:
		સ્થિતિ TBIS_SMALL:
		સ્થિતિ TBIS_MEDIUM:
		    size = GTK_ICON_SIZE_NORMAL;
		    તોડો;
		સ્થિતિ TBIS_LARGE:
		સ્થિતિ TBIS_HUGE:
		સ્થિતિ TBIS_GIANT:
		    size = GTK_ICON_SIZE_LARGE;
		    તોડો;
		મૂળભૂત:
		    size = GTK_ICON_SIZE_NORMAL;
		    તોડો;
	    }
	    gtk_image_set_icon_size(GTK_IMAGE(icon), size);
	    gtk_widget_set_visible(icon, TRUE);
	}
	નહીંતર
	    gtk_widget_set_visible(icon, FALSE);
    }

    label = g_object_get_data(G_OBJECT(btn), "label");
    જો (label != ખાલી)
	gtk_widget_set_visible(label, style & TOOLBAR_TEXT);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    જો (invalidate)
	g_object_set_data(G_OBJECT(btn), "toolbar-width", GINT_TO_POINTER(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    સ્થિર શૂન્ય
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
vim_toolbar_ઉમેરો_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	અચલ અક્ષર  *text,
	પૂર્ણાંક	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_add_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_set_child(GTK_BUTTON(btn), box);

    જો (icon != ખાલી)
    {
	gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_set_data(G_OBJECT(btn), "icon", icon);
    }
    જો (text != ખાલી)
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

    vim_toolbar_ઉમેરો(self, btn, idx);
    પાછા btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_ઉમેરો_separator(VimToolbar *self, પૂર્ણાંક idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_set_data(G_OBJECT(sep), "toolbar-width", GINT_TO_POINTER(-1));
    vim_toolbar_ઉમેરો(self, sep, idx);
    પાછા sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    શૂન્ય
vim_toolbar_set_style(VimToolbar *self, પૂર્ણાંક style, પૂર્ણાંક iconsize)
{
    GtkWidget *cur_child = gtk_widget_get_first_child(self->strip);

    જો (style == self->style && self->iconsize == iconsize)
	પાછા;

    self->style = style;
    self->iconsize = iconsize;

    જ્યારે (cur_child != ખાલી)
    {
	જો (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }

    cur_child = gtk_widget_get_first_child(self->overflow_box);
    જ્યારે (cur_child != ખાલી)
    {
	જો (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }
    // Sizes may have changed
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    શૂન્ય
vim_toolbar_remove(VimToolbar *self, GtkWidget *item)
{
    જો (gtk_widget_get_parent(item) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), item);
    નહીંતર
	gtk_box_remove(GTK_BOX(self->overflow_box), item);

    self->વસ્તુઓ = g_list_remove(self->વસ્તુઓ, item);
    g_object_unref(item);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    સ્થિર શૂન્ય
vim_toolbar_move_item_to(
	VimToolbar  *self,
	GtkWidget   *item,
	gboolean    overflow)
{
    GtkBox *from, *to;

    to = overflow ? GTK_BOX(self->overflow_box) : GTK_BOX(self->strip);
    જો (gtk_widget_get_parent(item) == GTK_WIDGET(to))
	પાછા;

    from = overflow ? GTK_BOX(self->strip) : GTK_BOX(self->overflow_box);

    g_object_ref(item);
    gtk_box_remove(from, item);
    gtk_box_append(to, item);
    g_object_unref(item);

    જો (GTK_IS_SEPARATOR(item))
	gtk_widget_set_visible(item, !overflow);
    નહીંતર
    {
	જો (overflow)
	{
	    પૂર્ણાંક style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_TEXT;
	    style |= TOOLBAR_ICONS;
	    set_button_style(item, style, self->iconsize, FALSE);
	}
	નહીંતર
	    set_button_style(item, self->style, self->iconsize, FALSE);
    }
}

    સ્થિર પૂર્ણાંક
get_item_width(GtkWidget *item)
{
    પૂર્ણાંક toolbar_width = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_width = GPOINTER_TO_INT(
	    g_object_get_data(G_OBJECT(item), "toolbar-width"));

    જો (toolbar_width == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_size(item, &min_req, &nat_req);
	toolbar_width = MAX(min_req.width, nat_req.width);

	// Save width for later
	g_object_set_data(G_OBJECT(item),
		"toolbar-width", GINT_TO_POINTER(toolbar_width));
    }
    પાછા toolbar_width;
}

    સ્થિર શૂન્ય
vim_toolbar_size_allocate(
	GtkWidget   *widget,
	પૂર્ણાંક	    width,
	પૂર્ણાંક	    height,
	પૂર્ણાંક	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    GtkAllocation   alloc;
    પૂર્ણાંક		    used = 0;
    પૂર્ણાંક		    avail;
    GList	    *overflow_ele = ખાલી;
    GtkWidget	    *child;

    avail = width - get_item_width(self->overflow_btn);

    માટે (GList *ele = self->વસ્તુઓ; ele != ખાલી; ele = ele->next)
    {
	GtkWidget   *item = ele->data;
	પૂર્ણાંક	    toolbar_width = get_item_width(item);

	જો (used + toolbar_width > avail)
	{
	    overflow_ele = ele;
	    તોડો;
	}
	નહીંતર
	    vim_toolbar_move_item_to(self, item, FALSE);
	used += toolbar_width;
    }

    જો (overflow_ele != ખાલી)
    {
	પૂર્ણાંક	    remaining = width - used;
	gboolean    need_overflow_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	જ્યારે ((child = gtk_widget_get_first_child(self->overflow_box))
		!= ખાલી)
	    vim_toolbar_move_item_to(self, child, FALSE);

	માટે (GList *ele = overflow_ele; ele != ખાલી; ele = ele->next)
	{
	    remaining -= get_item_width(ele->data);
	    જો (remaining < 0)
	    {
		need_overflow_btn = TRUE;
		તોડો;
	    }
	}

	જો (need_overflow_btn)
	    માટે (; overflow_ele != ખાલી; overflow_ele = overflow_ele->next)
		vim_toolbar_move_item_to(self, overflow_ele->data, TRUE);
	gtk_widget_set_visible(self->overflow_btn, need_overflow_btn);
    }
    નહીંતર
	gtk_widget_set_visible(self->overflow_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.width = width;
    alloc.height = height;

    gtk_widget_size_allocate(self->મૂળ, &alloc, baseline);
}

    સ્થિર શૂન્ય
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	પૂર્ણાંક		for_size,
	પૂર્ણાંક		*min,
	પૂર્ણાંક		*nat,
	પૂર્ણાંક		*min_baseline,
	પૂર્ણાંક		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    પૂર્ણાંક		strip_min = 0, strip_nat = 0;
    પૂર્ણાંક		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_size,
	    &strip_min, &strip_nat, ખાલી, ખાલી);
    gtk_widget_measure(self->overflow_btn, orientation, for_size,
	    &obtn_min, &obtn_nat, ખાલી, ખાલી);

    જો (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	જો (min != ખાલી)
	    *min = obtn_min;
	જો (nat != ખાલી)
	    *nat = strip_nat + obtn_nat;
    }
    નહીંતર
    {
	જો (min != ખાલી)
	    *min = MAX(strip_min, obtn_min);
	જો (nat != ખાલી)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    જો (min_baseline != ખાલી)
	*min_baseline = -1;
    જો (nat_baseline != ખાલી)
	*nat_baseline = -1;
}

#endif // FEAT_TOOLBAR
