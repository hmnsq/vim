/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

#اگر_تعریف_شدہ FEAT_TOOLBAR

#شامل <gtk/gtk.h>
#شامل "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
ساخت _VimToolbar
{
    GtkWidget والد;

    صحیح style; // TOOLBAR_* flags
    صحیح iconsize; // TBIS_* values

    GList *اشیاء;

    GtkWidget	*جڑ;
    GtkWidget	*strip;

    GtkWidget	*حد_سے_تجاوز_btn;
    GtkWidget	*حد_سے_تجاوز_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

ساکن خالی vim_toolbar_سائز_allocate(GtkWidget *widget, صحیح چوڑائی, صحیح اونچائی, صحیح baseline);
ساکن خالی vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, صحیح for_سائز, صحیح *minimum, صحیح *natural, صحیح *minimum_baseline, صحیح *natural_baseline);

    ساکن خالی
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->اشیاء, g_object_unref);
    g_clear_اشارہ_گر(&self->جڑ, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_والد_class)->dispose(object);
}

    ساکن خالی
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->سائز_allocate = vim_toolbar_سائز_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_سیٹ_css_name(widget_class, "toolbar");
}

    ساکن خالی
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->جڑ = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_سیٹ_والد(self->جڑ, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_سیٹ_hexpand(self->strip, TRUE);
    gtk_widget_سیٹ_halign(self->strip, GTK_ALIGN_آغاز);
    gtk_widget_سیٹ_حد_سے_تجاوز(self->strip, GTK_حد_سے_تجاوز_HIDDEN);
    gtk_box_append(GTK_BOX(self->جڑ), self->strip);

    self->حد_سے_تجاوز_btn = gtk_menu_button_new();
    gtk_widget_سیٹ_hexpand(self->حد_سے_تجاوز_btn, FALSE);
    gtk_widget_سیٹ_halign(self->حد_سے_تجاوز_btn, GTK_ALIGN_انتہا);
    gtk_widget_جوڑو_css_class(self->حد_سے_تجاوز_btn, "flat");
    gtk_box_append(GTK_BOX(self->جڑ), self->حد_سے_تجاوز_btn);
    g_object_سیٹ_ڈیٹا(G_OBJECT(self->حد_سے_تجاوز_btn),
	    "toolbar-width", GINT_مقصد_اشارہ_گر(-1));

    popover = gtk_popover_new();
    self->حد_سے_تجاوز_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_سیٹ_بچہ(GTK_POPOVER(popover), self->حد_سے_تجاوز_box);
    gtk_popover_سیٹ_has_arrow(GTK_POPOVER(popover), FALSE);
    // Make sure popover aligns to top right of button
    gtk_widget_سیٹ_halign(popover, GTK_ALIGN_انتہا);

    gtk_menu_button_سیٹ_popover(GTK_MENU_BUTTON(self->حد_سے_تجاوز_btn), popover);
}

    GtkWidget *
vim_toolbar_new(خالی)
{
    واپس g_object_new(VIM_TYPE_TOOLBAR, تہی);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    ساکن خالی
vim_toolbar_داخل_کرو(VimToolbar *self, GtkWidget *widget, صحیح idx)
{
    GtkWidget	*cur_بچہ = gtk_widget_get_پہلا_بچہ(self->strip);
    صحیح		i = 0;

    جب (cur_بچہ != تہی && i != idx - 1)
    {
	cur_بچہ = gtk_widget_get_اگلا_sibling(cur_بچہ);
	i++;
    }

    gtk_box_داخل_کرو_بچہ_after(GTK_BOX(self->strip), widget, cur_بچہ);
    self->اشیاء = g_list_داخل_کرو(self->اشیاء, g_object_ref(widget), idx);
    gtk_widget_قطار_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    ساکن خالی
سیٹ_button_style(GtkWidget *btn, صحیح style, صحیح iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    اگر (GTK_IS_SEPARATOR(btn))
	واپس;

    box = gtk_button_get_بچہ(GTK_BUTTON(btn));
    اگر (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_سیٹ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_سیٹ_spacing(GTK_BOX(box), 8);
    }
    ورنہ
    {
	gtk_orientable_سیٹ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_سیٹ_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_سیٹ_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_ڈیٹا(G_OBJECT(btn), "icon");
    اگر (icon != تہی)
    {
	اگر (style & TOOLBAR_ICONS)
	{
	    Gtk_Icon_سائز سائز;

	    انتخاب (iconsize)
	    {
		حالت TBIS_TINY:
		حالت TBIS_SMALL:
		حالت TBIS_MEDIUM:
		    سائز = GTK_ICON_سائز_معمول;
		    توڑو;
		حالت TBIS_LARGE:
		حالت TBIS_HUGE:
		حالت TBIS_GIANT:
		    سائز = GTK_ICON_سائز_LARGE;
		    توڑو;
		طےشدہ:
		    سائز = GTK_ICON_سائز_معمول;
		    توڑو;
	    }
	    gtk_image_سیٹ_icon_سائز(GTK_IMAGE(icon), سائز);
	    gtk_widget_سیٹ_visible(icon, TRUE);
	}
	ورنہ
	    gtk_widget_سیٹ_visible(icon, FALSE);
    }

    label = g_object_get_ڈیٹا(G_OBJECT(btn), "label");
    اگر (label != تہی)
	gtk_widget_سیٹ_visible(label, style & TOOLBAR_متن);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    اگر (invalidate)
	g_object_سیٹ_ڈیٹا(G_OBJECT(btn), "toolbar-width", GINT_مقصد_اشارہ_گر(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    ساکن خالی
button_clicked(GtkWidget *widget غیر_مستعمل, VimToolbar *self)
{
    gtk_menu_button_popdown(GTK_MENU_BUTTON(self->حد_سے_تجاوز_btn));
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
vim_toolbar_داخل_کرو_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ثابت حرف  *متن,
	صحیح	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_جوڑو_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_سیٹ_بچہ(GTK_BUTTON(btn), box);

    اگر (icon != تہی)
    {
	gtk_widget_سیٹ_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_سیٹ_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_سیٹ_ڈیٹا(G_OBJECT(btn), "icon", icon);
    }
    اگر (متن != تہی)
    {
	GtkWidget *label = gtk_label_new(متن);

	gtk_widget_سیٹ_valign (label, GTK_ALIGN_CENTER);
	gtk_widget_سیٹ_vexpand (label, TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_object_سیٹ_ڈیٹا(G_OBJECT(btn), "label", label);
    }

    سیٹ_button_style(btn, self->style, self->iconsize, TRUE);

    g_signal_connect_object(btn, "clicked",
	    G_CALLBACK(button_clicked), self, G_CONNECT_DEFAULT);

    vim_toolbar_داخل_کرو(self, btn, idx);
    واپس btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_داخل_کرو_separator(VimToolbar *self, صحیح idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_سیٹ_ڈیٹا(G_OBJECT(sep), "toolbar-width", GINT_مقصد_اشارہ_گر(-1));
    vim_toolbar_داخل_کرو(self, sep, idx);
    واپس sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    خالی
vim_toolbar_سیٹ_style(VimToolbar *self, صحیح style, صحیح iconsize)
{
    GtkWidget *cur_بچہ = gtk_widget_get_پہلا_بچہ(self->strip);

    اگر (style == self->style && self->iconsize == iconsize)
	واپس;

    self->style = style;
    self->iconsize = iconsize;

    جب (cur_بچہ != تہی)
    {
	اگر (!GTK_IS_SEPARATOR(cur_بچہ))
	    سیٹ_button_style(cur_بچہ, style, iconsize, TRUE);
	cur_بچہ = gtk_widget_get_اگلا_sibling(cur_بچہ);
    }

    cur_بچہ = gtk_widget_get_پہلا_بچہ(self->حد_سے_تجاوز_box);
    جب (cur_بچہ != تہی)
    {
	اگر (!GTK_IS_SEPARATOR(cur_بچہ))
	    سیٹ_button_style(cur_بچہ, style, iconsize, TRUE);
	cur_بچہ = gtk_widget_get_اگلا_sibling(cur_بچہ);
    }
    // Sizes may have changed
    gtk_widget_قطار_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    خالی
vim_toolbar_remove(VimToolbar *self, GtkWidget *شے)
{
    اگر (gtk_widget_get_والد(شے) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), شے);
    ورنہ
	gtk_box_remove(GTK_BOX(self->حد_سے_تجاوز_box), شے);

    self->اشیاء = g_list_remove(self->اشیاء, شے);
    g_object_unref(شے);
    gtk_widget_قطار_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    ساکن خالی
vim_toolbar_move_شے_مقصد(
	VimToolbar  *self,
	GtkWidget   *شے,
	gboolean    حد_سے_تجاوز)
{
    GtkBox *from, *مقصد;

    مقصد = حد_سے_تجاوز ? GTK_BOX(self->حد_سے_تجاوز_box) : GTK_BOX(self->strip);
    اگر (gtk_widget_get_والد(شے) == GTK_WIDGET(مقصد))
	واپس;

    from = حد_سے_تجاوز ? GTK_BOX(self->strip) : GTK_BOX(self->حد_سے_تجاوز_box);

    g_object_ref(شے);
    gtk_box_remove(from, شے);
    gtk_box_append(مقصد, شے);
    g_object_unref(شے);

    اگر (GTK_IS_SEPARATOR(شے))
	gtk_widget_سیٹ_visible(شے, !حد_سے_تجاوز);
    ورنہ
    {
	اگر (حد_سے_تجاوز)
	{
	    صحیح style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_متن;
	    style |= TOOLBAR_ICONS;
	    سیٹ_button_style(شے, style, self->iconsize, FALSE);
	}
	ورنہ
	    سیٹ_button_style(شے, self->style, self->iconsize, FALSE);
    }
}

    ساکن صحیح
get_شے_چوڑائی(GtkWidget *شے)
{
    صحیح toolbar_چوڑائی = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_چوڑائی = GPOINTER_مقصد_INT(
	    g_object_get_ڈیٹا(G_OBJECT(شے), "toolbar-width"));

    اگر (toolbar_چوڑائی == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_سائز(شے, &min_req, &nat_req);
	toolbar_چوڑائی = MAX(min_req.چوڑائی, nat_req.چوڑائی);

	// Save width for later
	g_object_سیٹ_ڈیٹا(G_OBJECT(شے),
		"toolbar-width", GINT_مقصد_اشارہ_گر(toolbar_چوڑائی));
    }
    واپس toolbar_چوڑائی;
}

    ساکن خالی
vim_toolbar_سائز_allocate(
	GtkWidget   *widget,
	صحیح	    چوڑائی,
	صحیح	    اونچائی,
	صحیح	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    Gtk_تخصیص   alloc;
    صحیح		    مستعمل = 0;
    صحیح		    avail;
    GList	    *حد_سے_تجاوز_ele = تہی;
    GtkWidget	    *بچہ;

    avail = چوڑائی - get_شے_چوڑائی(self->حد_سے_تجاوز_btn);

    برائے (GList *ele = self->اشیاء; ele != تہی; ele = ele->اگلا)
    {
	GtkWidget   *شے = ele->ڈیٹا;
	صحیح	    toolbar_چوڑائی = get_شے_چوڑائی(شے);

	اگر (مستعمل + toolbar_چوڑائی > avail)
	{
	    حد_سے_تجاوز_ele = ele;
	    توڑو;
	}
	ورنہ
	    vim_toolbar_move_شے_مقصد(self, شے, FALSE);
	مستعمل += toolbar_چوڑائی;
    }

    اگر (حد_سے_تجاوز_ele != تہی)
    {
	صحیح	    بقایا = چوڑائی - مستعمل;
	gboolean    need_حد_سے_تجاوز_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	جب ((بچہ = gtk_widget_get_پہلا_بچہ(self->حد_سے_تجاوز_box))
		!= تہی)
	    vim_toolbar_move_شے_مقصد(self, بچہ, FALSE);

	برائے (GList *ele = حد_سے_تجاوز_ele; ele != تہی; ele = ele->اگلا)
	{
	    بقایا -= get_شے_چوڑائی(ele->ڈیٹا);
	    اگر (بقایا < 0)
	    {
		need_حد_سے_تجاوز_btn = TRUE;
		توڑو;
	    }
	}

	اگر (need_حد_سے_تجاوز_btn)
	    برائے (; حد_سے_تجاوز_ele != تہی; حد_سے_تجاوز_ele = حد_سے_تجاوز_ele->اگلا)
		vim_toolbar_move_شے_مقصد(self, حد_سے_تجاوز_ele->ڈیٹا, TRUE);
	gtk_widget_سیٹ_visible(self->حد_سے_تجاوز_btn, need_حد_سے_تجاوز_btn);
    }
    ورنہ
	gtk_widget_سیٹ_visible(self->حد_سے_تجاوز_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.چوڑائی = چوڑائی;
    alloc.اونچائی = اونچائی;

    gtk_widget_سائز_allocate(self->جڑ, &alloc, baseline);
}

    ساکن خالی
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	صحیح		for_سائز,
	صحیح		*min,
	صحیح		*nat,
	صحیح		*min_baseline,
	صحیح		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    صحیح		strip_min = 0, strip_nat = 0;
    صحیح		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_سائز,
	    &strip_min, &strip_nat, تہی, تہی);
    gtk_widget_measure(self->حد_سے_تجاوز_btn, orientation, for_سائز,
	    &obtn_min, &obtn_nat, تہی, تہی);

    اگر (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	اگر (min != تہی)
	    *min = obtn_min;
	اگر (nat != تہی)
	    *nat = strip_nat + obtn_nat;
    }
    ورنہ
    {
	اگر (min != تہی)
	    *min = MAX(strip_min, obtn_min);
	اگر (nat != تہی)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    اگر (min_baseline != تہی)
	*min_baseline = -1;
    اگر (nat_baseline != تہی)
	*nat_baseline = -1;
}

#اگر_ختم // FEAT_TOOLBAR
