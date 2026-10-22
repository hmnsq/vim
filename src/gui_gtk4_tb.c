/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"

#ifdef FEAT_TOOLBAR

#include <gtk/gtk.h>
#include "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
sa_nracanaa _VimToolbar
{
    GtkWidget parent;

    ຈຳນວນເຕັມ style; // TOOLBAR_* flags
    ຈຳນວນເຕັມ iconsize; // TBIS_* values

    GList *items;

    GtkWidget	*root;
    GtkWidget	*strip;

    GtkWidget	*overflow_btn;
    GtkWidget	*overflow_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

static void vim_toolbar_size_allocate(GtkWidget *widget, ຈຳນວນເຕັມ width, ຈຳນວນເຕັມ height, ຈຳນວນເຕັມ baseline);
static void vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, ຈຳນວນເຕັມ for_size, ຈຳນວນເຕັມ *minimum, ຈຳນວນເຕັມ *natural, ຈຳນວນເຕັມ *minimum_baseline, ຈຳນວນເຕັມ *natural_baseline);

    static void
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->items, g_object_unref);
    g_clear_pointer(&self->root, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_parent_class)->dispose(object);
}

    static void
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->size_allocate = vim_toolbar_size_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_set_css_name(widget_class, "toolbar");
}

    static void
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->root = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_set_parent(self->root, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(self->strip, TRUE);
    gtk_widget_set_halign(self->strip, GTK_ALIGN_START);
    gtk_widget_set_overflow(self->strip, GTK_OVERFLOW_HIDDEN);
    gtk_box_append(GTK_BOX(self->root), self->strip);

    self->overflow_btn = gtk_menu_button_new();
    gtk_widget_set_hexpand(self->overflow_btn, FALSE);
    gtk_widget_set_halign(self->overflow_btn, GTK_ALIGN_END);
    gtk_widget_add_css_class(self->overflow_btn, "flat");
    gtk_box_append(GTK_BOX(self->root), self->overflow_btn);
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
vim_toolbar_new(void)
{
    ສົ່ງຄືນ g_object_new(VIM_TYPE_TOOLBAR, NULL);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    static void
vim_toolbar_insert(VimToolbar *self, GtkWidget *widget, ຈຳນວນເຕັມ idx)
{
    GtkWidget	*cur_child = gtk_widget_get_first_child(self->strip);
    ຈຳນວນເຕັມ		i = 0;

    ໃນຂະນະທີ່ (cur_child != NULL && i != idx - 1)
    {
	cur_child = gtk_widget_get_next_sibling(cur_child);
	i++;
    }

    gtk_box_insert_child_after(GTK_BOX(self->strip), widget, cur_child);
    self->items = g_list_insert(self->items, g_object_ref(widget), idx);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    static void
set_button_style(GtkWidget *btn, ຈຳນວນເຕັມ style, ຈຳນວນເຕັມ iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    ຖ້າ (GTK_IS_SEPARATOR(btn))
	ສົ່ງຄືນ;

    box = gtk_button_get_child(GTK_BUTTON(btn));
    ຖ້າ (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_set_spacing(GTK_BOX(box), 8);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	gtk_orientable_set_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_set_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_set_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_data(G_OBJECT(btn), "icon");
    ຖ້າ (icon != NULL)
    {
	ຖ້າ (style & TOOLBAR_ICONS)
	{
	    GtkIconSize size;

	    switch (iconsize)
	    {
		pak_sa TBIS_TINY:
		pak_sa TBIS_SMALL:
		pak_sa TBIS_MEDIUM:
		    size = GTK_ICON_SIZE_NORMAL;
		    bha_nga;
		pak_sa TBIS_LARGE:
		pak_sa TBIS_HUGE:
		pak_sa TBIS_GIANT:
		    size = GTK_ICON_SIZE_LARGE;
		    bha_nga;
		default:
		    size = GTK_ICON_SIZE_NORMAL;
		    bha_nga;
	    }
	    gtk_image_set_icon_size(GTK_IMAGE(icon), size);
	    gtk_widget_set_visible(icon, TRUE);
	}
	ບໍ່ດັ່ງນັ້ນ
	    gtk_widget_set_visible(icon, FALSE);
    }

    label = g_object_get_data(G_OBJECT(btn), "label");
    ຖ້າ (label != NULL)
	gtk_widget_set_visible(label, style & TOOLBAR_TEXT);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    ຖ້າ (invalidate)
	g_object_set_data(G_OBJECT(btn), "toolbar-width", GINT_TO_POINTER(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    static void
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
vim_toolbar_insert_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	const var_na  *text,
	ຈຳນວນເຕັມ	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_add_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_set_child(GTK_BUTTON(btn), box);

    ຖ້າ (icon != NULL)
    {
	gtk_widget_set_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_set_data(G_OBJECT(btn), "icon", icon);
    }
    ຖ້າ (text != NULL)
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

    vim_toolbar_insert(self, btn, idx);
    ສົ່ງຄືນ btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_insert_separator(VimToolbar *self, ຈຳນວນເຕັມ idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_set_data(G_OBJECT(sep), "toolbar-width", GINT_TO_POINTER(-1));
    vim_toolbar_insert(self, sep, idx);
    ສົ່ງຄືນ sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    void
vim_toolbar_set_style(VimToolbar *self, ຈຳນວນເຕັມ style, ຈຳນວນເຕັມ iconsize)
{
    GtkWidget *cur_child = gtk_widget_get_first_child(self->strip);

    ຖ້າ (style == self->style && self->iconsize == iconsize)
	ສົ່ງຄືນ;

    self->style = style;
    self->iconsize = iconsize;

    ໃນຂະນະທີ່ (cur_child != NULL)
    {
	ຖ້າ (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }

    cur_child = gtk_widget_get_first_child(self->overflow_box);
    ໃນຂະນະທີ່ (cur_child != NULL)
    {
	ຖ້າ (!GTK_IS_SEPARATOR(cur_child))
	    set_button_style(cur_child, style, iconsize, TRUE);
	cur_child = gtk_widget_get_next_sibling(cur_child);
    }
    // Sizes may have changed
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    void
vim_toolbar_remove(VimToolbar *self, GtkWidget *item)
{
    ຖ້າ (gtk_widget_get_parent(item) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), item);
    ບໍ່ດັ່ງນັ້ນ
	gtk_box_remove(GTK_BOX(self->overflow_box), item);

    self->items = g_list_remove(self->items, item);
    g_object_unref(item);
    gtk_widget_queue_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    static void
vim_toolbar_move_item_to(
	VimToolbar  *self,
	GtkWidget   *item,
	gboolean    overflow)
{
    GtkBox *from, *to;

    to = overflow ? GTK_BOX(self->overflow_box) : GTK_BOX(self->strip);
    ຖ້າ (gtk_widget_get_parent(item) == GTK_WIDGET(to))
	ສົ່ງຄືນ;

    from = overflow ? GTK_BOX(self->strip) : GTK_BOX(self->overflow_box);

    g_object_ref(item);
    gtk_box_remove(from, item);
    gtk_box_append(to, item);
    g_object_unref(item);

    ຖ້າ (GTK_IS_SEPARATOR(item))
	gtk_widget_set_visible(item, !overflow);
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (overflow)
	{
	    ຈຳນວນເຕັມ style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_TEXT;
	    style |= TOOLBAR_ICONS;
	    set_button_style(item, style, self->iconsize, FALSE);
	}
	ບໍ່ດັ່ງນັ້ນ
	    set_button_style(item, self->style, self->iconsize, FALSE);
    }
}

    static ຈຳນວນເຕັມ
get_item_width(GtkWidget *item)
{
    ຈຳນວນເຕັມ toolbar_width = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_width = GPOINTER_TO_INT(
	    g_object_get_data(G_OBJECT(item), "toolbar-width"));

    ຖ້າ (toolbar_width == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_size(item, &min_req, &nat_req);
	toolbar_width = MAX(min_req.width, nat_req.width);

	// Save width for later
	g_object_set_data(G_OBJECT(item),
		"toolbar-width", GINT_TO_POINTER(toolbar_width));
    }
    ສົ່ງຄືນ toolbar_width;
}

    static void
vim_toolbar_size_allocate(
	GtkWidget   *widget,
	ຈຳນວນເຕັມ	    width,
	ຈຳນວນເຕັມ	    height,
	ຈຳນວນເຕັມ	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    GtkAllocation   alloc;
    ຈຳນວນເຕັມ		    used = 0;
    ຈຳນວນເຕັມ		    avail;
    GList	    *overflow_ele = NULL;
    GtkWidget	    *child;

    avail = width - get_item_width(self->overflow_btn);

    ສຳລັບ (GList *ele = self->items; ele != NULL; ele = ele->next)
    {
	GtkWidget   *item = ele->data;
	ຈຳນວນເຕັມ	    toolbar_width = get_item_width(item);

	ຖ້າ (used + toolbar_width > avail)
	{
	    overflow_ele = ele;
	    bha_nga;
	}
	ບໍ່ດັ່ງນັ້ນ
	    vim_toolbar_move_item_to(self, item, FALSE);
	used += toolbar_width;
    }

    ຖ້າ (overflow_ele != NULL)
    {
	ຈຳນວນເຕັມ	    remaining = width - used;
	gboolean    need_overflow_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	ໃນຂະນະທີ່ ((child = gtk_widget_get_first_child(self->overflow_box))
		!= NULL)
	    vim_toolbar_move_item_to(self, child, FALSE);

	ສຳລັບ (GList *ele = overflow_ele; ele != NULL; ele = ele->next)
	{
	    remaining -= get_item_width(ele->data);
	    ຖ້າ (remaining < 0)
	    {
		need_overflow_btn = TRUE;
		bha_nga;
	    }
	}

	ຖ້າ (need_overflow_btn)
	    ສຳລັບ (; overflow_ele != NULL; overflow_ele = overflow_ele->next)
		vim_toolbar_move_item_to(self, overflow_ele->data, TRUE);
	gtk_widget_set_visible(self->overflow_btn, need_overflow_btn);
    }
    ບໍ່ດັ່ງນັ້ນ
	gtk_widget_set_visible(self->overflow_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.width = width;
    alloc.height = height;

    gtk_widget_size_allocate(self->root, &alloc, baseline);
}

    static void
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	ຈຳນວນເຕັມ		for_size,
	ຈຳນວນເຕັມ		*min,
	ຈຳນວນເຕັມ		*nat,
	ຈຳນວນເຕັມ		*min_baseline,
	ຈຳນວນເຕັມ		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    ຈຳນວນເຕັມ		strip_min = 0, strip_nat = 0;
    ຈຳນວນເຕັມ		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_size,
	    &strip_min, &strip_nat, NULL, NULL);
    gtk_widget_measure(self->overflow_btn, orientation, for_size,
	    &obtn_min, &obtn_nat, NULL, NULL);

    ຖ້າ (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	ຖ້າ (min != NULL)
	    *min = obtn_min;
	ຖ້າ (nat != NULL)
	    *nat = strip_nat + obtn_nat;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	ຖ້າ (min != NULL)
	    *min = MAX(strip_min, obtn_min);
	ຖ້າ (nat != NULL)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    ຖ້າ (min_baseline != NULL)
	*min_baseline = -1;
    ຖ້າ (nat_baseline != NULL)
	*nat_baseline = -1;
}

#endif // FEAT_TOOLBAR
