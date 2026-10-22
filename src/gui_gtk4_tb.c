/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR

#ضمن <gtk/gtk.h>
#ضمن "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
بنية _VimToolbar
{
    GtkWidget والد;

    صحيح style; // TOOLBAR_* flags
    صحيح iconsize; // TBIS_* values

    GList *شيون;

    GtkWidget	*پاڙ;
    GtkWidget	*strip;

    GtkWidget	*حد_کان_مٿي_btn;
    GtkWidget	*حد_کان_مٿي_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

ساكن فراغ vim_toolbar_سائيز_allocate(GtkWidget *widget, صحيح ويڪر, صحيح اوچائي, صحيح baseline);
ساكن فراغ vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, صحيح for_سائيز, صحيح *minimum, صحيح *natural, صحيح *minimum_baseline, صحيح *natural_baseline);

    ساكن فراغ
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->شيون, g_object_unref);
    g_clear_اشارو(&self->پاڙ, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_والد_class)->dispose(object);
}

    ساكن فراغ
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->سائيز_allocate = vim_toolbar_سائيز_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_سيٽ_css_name(widget_class, "toolbar");
}

    ساكن فراغ
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->پاڙ = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_سيٽ_والد(self->پاڙ, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_سيٽ_hexpand(self->strip, TRUE);
    gtk_widget_سيٽ_halign(self->strip, GTK_ALIGN_شروعات);
    gtk_widget_سيٽ_حد_کان_مٿي(self->strip, GTK_حد_کان_مٿي_HIDDEN);
    gtk_box_append(GTK_BOX(self->پاڙ), self->strip);

    self->حد_کان_مٿي_btn = gtk_menu_button_new();
    gtk_widget_سيٽ_hexpand(self->حد_کان_مٿي_btn, FALSE);
    gtk_widget_سيٽ_halign(self->حد_کان_مٿي_btn, GTK_ALIGN_پڇاڙي);
    gtk_widget_جوڙيو_css_class(self->حد_کان_مٿي_btn, "flat");
    gtk_box_append(GTK_BOX(self->پاڙ), self->حد_کان_مٿي_btn);
    g_object_سيٽ_معلومات(G_OBJECT(self->حد_کان_مٿي_btn),
	    "toolbar-width", GINT_منزل_اشارو(-1));

    popover = gtk_popover_new();
    self->حد_کان_مٿي_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_سيٽ_ٻار(GTK_POPOVER(popover), self->حد_کان_مٿي_box);
    gtk_popover_سيٽ_has_arrow(GTK_POPOVER(popover), FALSE);
    // Make sure popover aligns to top right of button
    gtk_widget_سيٽ_halign(popover, GTK_ALIGN_پڇاڙي);

    gtk_menu_button_سيٽ_popover(GTK_MENU_BUTTON(self->حد_کان_مٿي_btn), popover);
}

    GtkWidget *
vim_toolbar_new(فراغ)
{
    أرجع g_object_new(VIM_TYPE_TOOLBAR, عدم);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    ساكن فراغ
vim_toolbar_وجهو(VimToolbar *self, GtkWidget *widget, صحيح idx)
{
    GtkWidget	*cur_ٻار = gtk_widget_get_پهريون_ٻار(self->strip);
    صحيح		i = 0;

    طالما (cur_ٻار != عدم && i != idx - 1)
    {
	cur_ٻار = gtk_widget_get_ايندڙ_sibling(cur_ٻار);
	i++;
    }

    gtk_box_وجهو_ٻار_after(GTK_BOX(self->strip), widget, cur_ٻار);
    self->شيون = g_list_وجهو(self->شيون, g_object_ref(widget), idx);
    gtk_widget_قطار_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    ساكن فراغ
سيٽ_button_style(GtkWidget *btn, صحيح style, صحيح iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    إذا (GTK_IS_SEPARATOR(btn))
	أرجع;

    box = gtk_button_get_ٻار(GTK_BUTTON(btn));
    إذا (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_سيٽ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_سيٽ_spacing(GTK_BOX(box), 8);
    }
    وإلا
    {
	gtk_orientable_سيٽ_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_سيٽ_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_سيٽ_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_معلومات(G_OBJECT(btn), "icon");
    إذا (icon != عدم)
    {
	إذا (style & TOOLBAR_ICONS)
	{
	    Gtk_Icon_سائيز سائيز;

	    بدل (iconsize)
	    {
		حالة TBIS_TINY:
		حالة TBIS_SMALL:
		حالة TBIS_MEDIUM:
		    سائيز = GTK_ICON_سائيز_عام;
		    اكسر;
		حالة TBIS_LARGE:
		حالة TBIS_HUGE:
		حالة TBIS_GIANT:
		    سائيز = GTK_ICON_سائيز_LARGE;
		    اكسر;
		افتراضي:
		    سائيز = GTK_ICON_سائيز_عام;
		    اكسر;
	    }
	    gtk_image_سيٽ_icon_سائيز(GTK_IMAGE(icon), سائيز);
	    gtk_widget_سيٽ_visible(icon, TRUE);
	}
	وإلا
	    gtk_widget_سيٽ_visible(icon, FALSE);
    }

    label = g_object_get_معلومات(G_OBJECT(btn), "label");
    إذا (label != عدم)
	gtk_widget_سيٽ_visible(label, style & TOOLBAR_متن);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    إذا (invalidate)
	g_object_سيٽ_معلومات(G_OBJECT(btn), "toolbar-width", GINT_منزل_اشارو(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    ساكن فراغ
button_clicked(GtkWidget *widget اڻ_استعمال_ٿيل, VimToolbar *self)
{
    gtk_menu_button_popdown(GTK_MENU_BUTTON(self->حد_کان_مٿي_btn));
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
vim_toolbar_وجهو_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	ثابت حرف  *متن,
	صحيح	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_جوڙيو_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_سيٽ_ٻار(GTK_BUTTON(btn), box);

    إذا (icon != عدم)
    {
	gtk_widget_سيٽ_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_سيٽ_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_سيٽ_معلومات(G_OBJECT(btn), "icon", icon);
    }
    إذا (متن != عدم)
    {
	GtkWidget *label = gtk_label_new(متن);

	gtk_widget_سيٽ_valign (label, GTK_ALIGN_CENTER);
	gtk_widget_سيٽ_vexpand (label, TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_object_سيٽ_معلومات(G_OBJECT(btn), "label", label);
    }

    سيٽ_button_style(btn, self->style, self->iconsize, TRUE);

    g_signal_connect_object(btn, "clicked",
	    G_CALLBACK(button_clicked), self, G_CONNECT_DEFAULT);

    vim_toolbar_وجهو(self, btn, idx);
    أرجع btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_وجهو_separator(VimToolbar *self, صحيح idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_سيٽ_معلومات(G_OBJECT(sep), "toolbar-width", GINT_منزل_اشارو(-1));
    vim_toolbar_وجهو(self, sep, idx);
    أرجع sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    فراغ
vim_toolbar_سيٽ_style(VimToolbar *self, صحيح style, صحيح iconsize)
{
    GtkWidget *cur_ٻار = gtk_widget_get_پهريون_ٻار(self->strip);

    إذا (style == self->style && self->iconsize == iconsize)
	أرجع;

    self->style = style;
    self->iconsize = iconsize;

    طالما (cur_ٻار != عدم)
    {
	إذا (!GTK_IS_SEPARATOR(cur_ٻار))
	    سيٽ_button_style(cur_ٻار, style, iconsize, TRUE);
	cur_ٻار = gtk_widget_get_ايندڙ_sibling(cur_ٻار);
    }

    cur_ٻار = gtk_widget_get_پهريون_ٻار(self->حد_کان_مٿي_box);
    طالما (cur_ٻار != عدم)
    {
	إذا (!GTK_IS_SEPARATOR(cur_ٻار))
	    سيٽ_button_style(cur_ٻار, style, iconsize, TRUE);
	cur_ٻار = gtk_widget_get_ايندڙ_sibling(cur_ٻار);
    }
    // Sizes may have changed
    gtk_widget_قطار_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    فراغ
vim_toolbar_remove(VimToolbar *self, GtkWidget *شي)
{
    إذا (gtk_widget_get_والد(شي) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), شي);
    وإلا
	gtk_box_remove(GTK_BOX(self->حد_کان_مٿي_box), شي);

    self->شيون = g_list_remove(self->شيون, شي);
    g_object_unref(شي);
    gtk_widget_قطار_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    ساكن فراغ
vim_toolbar_move_شي_منزل(
	VimToolbar  *self,
	GtkWidget   *شي,
	gboolean    حد_کان_مٿي)
{
    GtkBox *from, *منزل;

    منزل = حد_کان_مٿي ? GTK_BOX(self->حد_کان_مٿي_box) : GTK_BOX(self->strip);
    إذا (gtk_widget_get_والد(شي) == GTK_WIDGET(منزل))
	أرجع;

    from = حد_کان_مٿي ? GTK_BOX(self->strip) : GTK_BOX(self->حد_کان_مٿي_box);

    g_object_ref(شي);
    gtk_box_remove(from, شي);
    gtk_box_append(منزل, شي);
    g_object_unref(شي);

    إذا (GTK_IS_SEPARATOR(شي))
	gtk_widget_سيٽ_visible(شي, !حد_کان_مٿي);
    وإلا
    {
	إذا (حد_کان_مٿي)
	{
	    صحيح style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_متن;
	    style |= TOOLBAR_ICONS;
	    سيٽ_button_style(شي, style, self->iconsize, FALSE);
	}
	وإلا
	    سيٽ_button_style(شي, self->style, self->iconsize, FALSE);
    }
}

    ساكن صحيح
get_شي_ويڪر(GtkWidget *شي)
{
    صحيح toolbar_ويڪر = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_ويڪر = GPOINTER_منزل_INT(
	    g_object_get_معلومات(G_OBJECT(شي), "toolbar-width"));

    إذا (toolbar_ويڪر == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_سائيز(شي, &min_req, &nat_req);
	toolbar_ويڪر = MAX(min_req.ويڪر, nat_req.ويڪر);

	// Save width for later
	g_object_سيٽ_معلومات(G_OBJECT(شي),
		"toolbar-width", GINT_منزل_اشارو(toolbar_ويڪر));
    }
    أرجع toolbar_ويڪر;
}

    ساكن فراغ
vim_toolbar_سائيز_allocate(
	GtkWidget   *widget,
	صحيح	    ويڪر,
	صحيح	    اوچائي,
	صحيح	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    Gtk_ورهاست   alloc;
    صحيح		    استعمال_ٿيل = 0;
    صحيح		    avail;
    GList	    *حد_کان_مٿي_ele = عدم;
    GtkWidget	    *ٻار;

    avail = ويڪر - get_شي_ويڪر(self->حد_کان_مٿي_btn);

    لكل (GList *ele = self->شيون; ele != عدم; ele = ele->ايندڙ)
    {
	GtkWidget   *شي = ele->معلومات;
	صحيح	    toolbar_ويڪر = get_شي_ويڪر(شي);

	إذا (استعمال_ٿيل + toolbar_ويڪر > avail)
	{
	    حد_کان_مٿي_ele = ele;
	    اكسر;
	}
	وإلا
	    vim_toolbar_move_شي_منزل(self, شي, FALSE);
	استعمال_ٿيل += toolbar_ويڪر;
    }

    إذا (حد_کان_مٿي_ele != عدم)
    {
	صحيح	    بچيل = ويڪر - استعمال_ٿيل;
	gboolean    need_حد_کان_مٿي_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	طالما ((ٻار = gtk_widget_get_پهريون_ٻار(self->حد_کان_مٿي_box))
		!= عدم)
	    vim_toolbar_move_شي_منزل(self, ٻار, FALSE);

	لكل (GList *ele = حد_کان_مٿي_ele; ele != عدم; ele = ele->ايندڙ)
	{
	    بچيل -= get_شي_ويڪر(ele->معلومات);
	    إذا (بچيل < 0)
	    {
		need_حد_کان_مٿي_btn = TRUE;
		اكسر;
	    }
	}

	إذا (need_حد_کان_مٿي_btn)
	    لكل (; حد_کان_مٿي_ele != عدم; حد_کان_مٿي_ele = حد_کان_مٿي_ele->ايندڙ)
		vim_toolbar_move_شي_منزل(self, حد_کان_مٿي_ele->معلومات, TRUE);
	gtk_widget_سيٽ_visible(self->حد_کان_مٿي_btn, need_حد_کان_مٿي_btn);
    }
    وإلا
	gtk_widget_سيٽ_visible(self->حد_کان_مٿي_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.ويڪر = ويڪر;
    alloc.اوچائي = اوچائي;

    gtk_widget_سائيز_allocate(self->پاڙ, &alloc, baseline);
}

    ساكن فراغ
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	صحيح		for_سائيز,
	صحيح		*min,
	صحيح		*nat,
	صحيح		*min_baseline,
	صحيح		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    صحيح		strip_min = 0, strip_nat = 0;
    صحيح		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_سائيز,
	    &strip_min, &strip_nat, عدم, عدم);
    gtk_widget_measure(self->حد_کان_مٿي_btn, orientation, for_سائيز,
	    &obtn_min, &obtn_nat, عدم, عدم);

    إذا (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	إذا (min != عدم)
	    *min = obtn_min;
	إذا (nat != عدم)
	    *nat = strip_nat + obtn_nat;
    }
    وإلا
    {
	إذا (min != عدم)
	    *min = MAX(strip_min, obtn_min);
	إذا (nat != عدم)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    إذا (min_baseline != عدم)
	*min_baseline = -1;
    إذا (nat_baseline != عدم)
	*nat_baseline = -1;
}

#جيڪڏهن_پورو // FEAT_TOOLBAR
