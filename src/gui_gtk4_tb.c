/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_TOOLBAR

#అన్తర్భావ <gtk/gtk.h>
#అన్తర్భావ "gui_gtk4_tb.h"

/*
 * GTK4 removed GtkToolbar, so this is our version of it.
 */
నిర్మాణం _VimToolbar
{
    GtkWidget మాతృ;

    పూర్ణాంకం style; // TOOLBAR_* flags
    పూర్ణాంకం iconsize; // TBIS_* values

    GList *వస్తువులు;

    GtkWidget	*మూలం;
    GtkWidget	*strip;

    GtkWidget	*అధికప్రవాహం_btn;
    GtkWidget	*అధికప్రవాహం_box;
};

G_DEFINE_TYPE(VimToolbar, vim_toolbar, GTK_TYPE_WIDGET)

స్థిర శూన్య vim_toolbar_కొలత_allocate(GtkWidget *widget, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు, పూర్ణాంకం baseline);
స్థిర శూన్య vim_toolbar_measure(GtkWidget *widget, GtkOrientation orientation, పూర్ణాంకం for_కొలత, పూర్ణాంకం *minimum, పూర్ణాంకం *natural, పూర్ణాంకం *minimum_baseline, పూర్ణాంకం *natural_baseline);

    స్థిర శూన్య
vim_toolbar_dispose(GObject *object)
{
    VimToolbar *self = VIM_TOOLBAR(object);

    g_clear_list(&self->వస్తువులు, g_object_unref);
    g_clear_సూచిక(&self->మూలం, gtk_widget_unparent);

    G_OBJECT_CLASS(vim_toolbar_మాతృ_class)->dispose(object);
}

    స్థిర శూన్య
vim_toolbar_class_init(VimToolbarClass *class)
{
    GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(class);
    GObjectClass    *obj_class = G_OBJECT_CLASS(class);

    widget_class->కొలత_allocate = vim_toolbar_కొలత_allocate;
    widget_class->measure = vim_toolbar_measure;

    obj_class->dispose = vim_toolbar_dispose;

    gtk_widget_class_సమితి_css_name(widget_class, "toolbar");
}

    స్థిర శూన్య
vim_toolbar_init(VimToolbar *self)
{
    GtkWidget *popover;

    self->style = TOOLBAR_ICONS | TOOLBAR_TOOLTIPS;
    self->iconsize = TBIS_MEDIUM;

    self->మూలం = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_widget_సమితి_మాతృ(self->మూలం, GTK_WIDGET(self));

    self->strip = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_సమితి_hexpand(self->strip, TRUE);
    gtk_widget_సమితి_halign(self->strip, GTK_ALIGN_ప్రారంభం);
    gtk_widget_సమితి_అధికప్రవాహం(self->strip, GTK_అధికప్రవాహం_HIDDEN);
    gtk_box_append(GTK_BOX(self->మూలం), self->strip);

    self->అధికప్రవాహం_btn = gtk_menu_button_new();
    gtk_widget_సమితి_hexpand(self->అధికప్రవాహం_btn, FALSE);
    gtk_widget_సమితి_halign(self->అధికప్రవాహం_btn, GTK_ALIGN_చివర);
    gtk_widget_కూడు_css_class(self->అధికప్రవాహం_btn, "flat");
    gtk_box_append(GTK_BOX(self->మూలం), self->అధికప్రవాహం_btn);
    g_object_సమితి_దత్తాంశం(G_OBJECT(self->అధికప్రవాహం_btn),
	    "toolbar-width", GINT_గమ్యం_సూచిక(-1));

    popover = gtk_popover_new();
    self->అధికప్రవాహం_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_సమితి_బిడ్డ(GTK_POPOVER(popover), self->అధికప్రవాహం_box);
    gtk_popover_సమితి_has_arrow(GTK_POPOVER(popover), FALSE);
    // Make sure popover aligns to top right of button
    gtk_widget_సమితి_halign(popover, GTK_ALIGN_చివర);

    gtk_menu_button_సమితి_popover(GTK_MENU_BUTTON(self->అధికప్రవాహం_btn), popover);
}

    GtkWidget *
vim_toolbar_new(శూన్య)
{
    తిరిగి g_object_new(VIM_TYPE_TOOLBAR, ఖాళీ);
}

/*
 * Insert the widget at the given index, then queue a size allocate to check for
 * overflowing items.
 */
    స్థిర శూన్య
vim_toolbar_చొప్పించు(VimToolbar *self, GtkWidget *widget, పూర్ణాంకం idx)
{
    GtkWidget	*cur_బిడ్డ = gtk_widget_get_మొదటి_బిడ్డ(self->strip);
    పూర్ణాంకం		i = 0;

    అయితే (cur_బిడ్డ != ఖాళీ && i != idx - 1)
    {
	cur_బిడ్డ = gtk_widget_get_తదుపరి_sibling(cur_బిడ్డ);
	i++;
    }

    gtk_box_చొప్పించు_బిడ్డ_after(GTK_BOX(self->strip), widget, cur_బిడ్డ);
    self->వస్తువులు = g_list_చొప్పించు(self->వస్తువులు, g_object_ref(widget), idx);
    gtk_widget_వరుస_allocate(GTK_WIDGET(self));
}

/*
 * Update "btn" according to "style" and "iconsize".
 */
    స్థిర శూన్య
సమితి_button_style(GtkWidget *btn, పూర్ణాంకం style, పూర్ణాంకం iconsize, gboolean invalidate)
{
    GtkWidget *box;
    GtkWidget *icon;
    GtkWidget *label;

    ఒకవేళ (GTK_IS_SEPARATOR(btn))
	తిరిగి;

    box = gtk_button_get_బిడ్డ(GTK_BUTTON(btn));
    ఒకవేళ (style & TOOLBAR_HORIZ)
    {
	gtk_orientable_సమితి_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_HORIZONTAL);
	gtk_box_సమితి_spacing(GTK_BOX(box), 8);
    }
    లేకపోతే
    {
	gtk_orientable_సమితి_orientation(GTK_ORIENTABLE(box),
		GTK_ORIENTATION_VERTICAL);
	gtk_box_సమితి_spacing(GTK_BOX(box), 4);
    }

    gtk_widget_సమితి_has_tooltip(btn, style & TOOLBAR_TOOLTIPS);

    icon = g_object_get_దత్తాంశం(G_OBJECT(btn), "icon");
    ఒకవేళ (icon != ఖాళీ)
    {
	ఒకవేళ (style & TOOLBAR_ICONS)
	{
	    Gtk_Icon_కొలత కొలత;

	    ఎంపిక (iconsize)
	    {
		స్థితి TBIS_TINY:
		స్థితి TBIS_SMALL:
		స్థితి TBIS_MEDIUM:
		    కొలత = GTK_ICON_కొలత_సాధారణ;
		    ఆపు;
		స్థితి TBIS_LARGE:
		స్థితి TBIS_HUGE:
		స్థితి TBIS_GIANT:
		    కొలత = GTK_ICON_కొలత_LARGE;
		    ఆపు;
		అప్రమేయ:
		    కొలత = GTK_ICON_కొలత_సాధారణ;
		    ఆపు;
	    }
	    gtk_image_సమితి_icon_కొలత(GTK_IMAGE(icon), కొలత);
	    gtk_widget_సమితి_visible(icon, TRUE);
	}
	లేకపోతే
	    gtk_widget_సమితి_visible(icon, FALSE);
    }

    label = g_object_get_దత్తాంశం(G_OBJECT(btn), "label");
    ఒకవేళ (label != ఖాళీ)
	gtk_widget_సమితి_visible(label, style & TOOLBAR_పాఠ్యం);

    // Used to cache width in toolbar ("self->strip"). -1 means not calculated
    // yet.
    ఒకవేళ (invalidate)
	g_object_సమితి_దత్తాంశం(G_OBJECT(btn), "toolbar-width", GINT_గమ్యం_సూచిక(-1));
}

/*
 * Always close overflow popover when button is clicked.
 */
    స్థిర శూన్య
button_clicked(GtkWidget *widget వాడని, VimToolbar *self)
{
    gtk_menu_button_popdown(GTK_MENU_BUTTON(self->అధికప్రవాహం_btn));
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
vim_toolbar_చొప్పించు_button(
	VimToolbar  *self,
	GtkWidget   *icon,
	స్థిరరాశి అక్షరం  *పాఠ్యం,
	పూర్ణాంకం	    idx)
{
    GtkWidget *btn = gtk_button_new();
    GtkWidget *box;

    gtk_widget_కూడు_css_class(btn, "flat");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_button_సమితి_బిడ్డ(GTK_BUTTON(btn), box);

    ఒకవేళ (icon != ఖాళీ)
    {
	gtk_widget_సమితి_valign (icon, GTK_ALIGN_CENTER);
	gtk_widget_సమితి_vexpand (icon, TRUE);
	gtk_box_append(GTK_BOX(box), icon);
	g_object_సమితి_దత్తాంశం(G_OBJECT(btn), "icon", icon);
    }
    ఒకవేళ (పాఠ్యం != ఖాళీ)
    {
	GtkWidget *label = gtk_label_new(పాఠ్యం);

	gtk_widget_సమితి_valign (label, GTK_ALIGN_CENTER);
	gtk_widget_సమితి_vexpand (label, TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_object_సమితి_దత్తాంశం(G_OBJECT(btn), "label", label);
    }

    సమితి_button_style(btn, self->style, self->iconsize, TRUE);

    g_signal_connect_object(btn, "clicked",
	    G_CALLBACK(button_clicked), self, G_CONNECT_DEFAULT);

    vim_toolbar_చొప్పించు(self, btn, idx);
    తిరిగి btn;
}

/*
 * Insert a toolbar separator at the given index and return the widget.
 */
    GtkWidget *
vim_toolbar_చొప్పించు_separator(VimToolbar *self, పూర్ణాంకం idx)
{
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

    g_object_సమితి_దత్తాంశం(G_OBJECT(sep), "toolbar-width", GINT_గమ్యం_సూచిక(-1));
    vim_toolbar_చొప్పించు(self, sep, idx);
    తిరిగి sep;
}

/*
 * Update the style and iconsize of the toolbar
 */
    శూన్య
vim_toolbar_సమితి_style(VimToolbar *self, పూర్ణాంకం style, పూర్ణాంకం iconsize)
{
    GtkWidget *cur_బిడ్డ = gtk_widget_get_మొదటి_బిడ్డ(self->strip);

    ఒకవేళ (style == self->style && self->iconsize == iconsize)
	తిరిగి;

    self->style = style;
    self->iconsize = iconsize;

    అయితే (cur_బిడ్డ != ఖాళీ)
    {
	ఒకవేళ (!GTK_IS_SEPARATOR(cur_బిడ్డ))
	    సమితి_button_style(cur_బిడ్డ, style, iconsize, TRUE);
	cur_బిడ్డ = gtk_widget_get_తదుపరి_sibling(cur_బిడ్డ);
    }

    cur_బిడ్డ = gtk_widget_get_మొదటి_బిడ్డ(self->అధికప్రవాహం_box);
    అయితే (cur_బిడ్డ != ఖాళీ)
    {
	ఒకవేళ (!GTK_IS_SEPARATOR(cur_బిడ్డ))
	    సమితి_button_style(cur_బిడ్డ, style, iconsize, TRUE);
	cur_బిడ్డ = gtk_widget_get_తదుపరి_sibling(cur_బిడ్డ);
    }
    // Sizes may have changed
    gtk_widget_వరుస_allocate(GTK_WIDGET(self));
}

/*
 * Remove the item from the toolbar.
 */
    శూన్య
vim_toolbar_remove(VimToolbar *self, GtkWidget *అంశం)
{
    ఒకవేళ (gtk_widget_get_మాతృ(అంశం) == self->strip)
	gtk_box_remove(GTK_BOX(self->strip), అంశం);
    లేకపోతే
	gtk_box_remove(GTK_BOX(self->అధికప్రవాహం_box), అంశం);

    self->వస్తువులు = g_list_remove(self->వస్తువులు, అంశం);
    g_object_unref(అంశం);
    gtk_widget_వరుస_allocate(GTK_WIDGET(self));
}

/*
 * If "overflow" is TRUE, then move "item" from the toolbar to the overflow, and
 * vice versa.
 */
    స్థిర శూన్య
vim_toolbar_move_అంశం_గమ్యం(
	VimToolbar  *self,
	GtkWidget   *అంశం,
	gboolean    అధికప్రవాహం)
{
    GtkBox *from, *గమ్యం;

    గమ్యం = అధికప్రవాహం ? GTK_BOX(self->అధికప్రవాహం_box) : GTK_BOX(self->strip);
    ఒకవేళ (gtk_widget_get_మాతృ(అంశం) == GTK_WIDGET(గమ్యం))
	తిరిగి;

    from = అధికప్రవాహం ? GTK_BOX(self->strip) : GTK_BOX(self->అధికప్రవాహం_box);

    g_object_ref(అంశం);
    gtk_box_remove(from, అంశం);
    gtk_box_append(గమ్యం, అంశం);
    g_object_unref(అంశం);

    ఒకవేళ (GTK_IS_SEPARATOR(అంశం))
	gtk_widget_సమితి_visible(అంశం, !అధికప్రవాహం);
    లేకపోతే
    {
	ఒకవేళ (అధికప్రవాహం)
	{
	    పూర్ణాంకం style = self->style;

	    // Force set these flags for the overflow menu, to mimic what GTK3
	    // GtkToolbar does.
	    style |= TOOLBAR_HORIZ;
	    style |= TOOLBAR_పాఠ్యం;
	    style |= TOOLBAR_ICONS;
	    సమితి_button_style(అంశం, style, self->iconsize, FALSE);
	}
	లేకపోతే
	    సమితి_button_style(అంశం, self->style, self->iconsize, FALSE);
    }
}

    స్థిర పూర్ణాంకం
get_అంశం_వెడల్పు(GtkWidget *అంశం)
{
    పూర్ణాంకం toolbar_వెడల్పు = -1;

    // Always use cached width for item, because width in overflow menu is
    // different (GtkBox is in horizontal layout). Also use it for separators,
    // since if its not visible, then width is zero.
    toolbar_వెడల్పు = GPOINTER_గమ్యం_INT(
	    g_object_get_దత్తాంశం(G_OBJECT(అంశం), "toolbar-width"));

    ఒకవేళ (toolbar_వెడల్పు == -1)
    {
	GtkRequisition min_req, nat_req;
	gtk_widget_get_preferred_కొలత(అంశం, &min_req, &nat_req);
	toolbar_వెడల్పు = MAX(min_req.వెడల్పు, nat_req.వెడల్పు);

	// Save width for later
	g_object_సమితి_దత్తాంశం(G_OBJECT(అంశం),
		"toolbar-width", GINT_గమ్యం_సూచిక(toolbar_వెడల్పు));
    }
    తిరిగి toolbar_వెడల్పు;
}

    స్థిర శూన్య
vim_toolbar_కొలత_allocate(
	GtkWidget   *widget,
	పూర్ణాంకం	    వెడల్పు,
	పూర్ణాంకం	    ఎత్తు,
	పూర్ణాంకం	    baseline)
{
    VimToolbar	    *self = VIM_TOOLBAR(widget);
    Gtk_కేటాయింపు   alloc;
    పూర్ణాంకం		    వాడిన = 0;
    పూర్ణాంకం		    avail;
    GList	    *అధికప్రవాహం_ele = ఖాళీ;
    GtkWidget	    *బిడ్డ;

    avail = వెడల్పు - get_అంశం_వెడల్పు(self->అధికప్రవాహం_btn);

    కొరకు (GList *ele = self->వస్తువులు; ele != ఖాళీ; ele = ele->తదుపరి)
    {
	GtkWidget   *అంశం = ele->దత్తాంశం;
	పూర్ణాంకం	    toolbar_వెడల్పు = get_అంశం_వెడల్పు(అంశం);

	ఒకవేళ (వాడిన + toolbar_వెడల్పు > avail)
	{
	    అధికప్రవాహం_ele = ele;
	    ఆపు;
	}
	లేకపోతే
	    vim_toolbar_move_అంశం_గమ్యం(self, అంశం, FALSE);
	వాడిన += toolbar_వెడల్పు;
    }

    ఒకవేళ (అధికప్రవాహం_ele != ఖాళీ)
    {
	పూర్ణాంకం	    మిగిలిన = వెడల్పు - వాడిన;
	gboolean    need_అధికప్రవాహం_btn = FALSE;

	// Move all items in the overflow to the toolbar, so we can add them
	// in the correct order back again. Also check if the overflow button is
	// still needed (can fit all toolbar items by disabling it).
	అయితే ((బిడ్డ = gtk_widget_get_మొదటి_బిడ్డ(self->అధికప్రవాహం_box))
		!= ఖాళీ)
	    vim_toolbar_move_అంశం_గమ్యం(self, బిడ్డ, FALSE);

	కొరకు (GList *ele = అధికప్రవాహం_ele; ele != ఖాళీ; ele = ele->తదుపరి)
	{
	    మిగిలిన -= get_అంశం_వెడల్పు(ele->దత్తాంశం);
	    ఒకవేళ (మిగిలిన < 0)
	    {
		need_అధికప్రవాహం_btn = TRUE;
		ఆపు;
	    }
	}

	ఒకవేళ (need_అధికప్రవాహం_btn)
	    కొరకు (; అధికప్రవాహం_ele != ఖాళీ; అధికప్రవాహం_ele = అధికప్రవాహం_ele->తదుపరి)
		vim_toolbar_move_అంశం_గమ్యం(self, అధికప్రవాహం_ele->దత్తాంశం, TRUE);
	gtk_widget_సమితి_visible(self->అధికప్రవాహం_btn, need_అధికప్రవాహం_btn);
    }
    లేకపోతే
	gtk_widget_సమితి_visible(self->అధికప్రవాహం_btn, FALSE);

    alloc.x = alloc.y = 0;
    alloc.వెడల్పు = వెడల్పు;
    alloc.ఎత్తు = ఎత్తు;

    gtk_widget_కొలత_allocate(self->మూలం, &alloc, baseline);
}

    స్థిర శూన్య
vim_toolbar_measure(
	GtkWidget	*widget,
	GtkOrientation	orientation,
	పూర్ణాంకం		for_కొలత,
	పూర్ణాంకం		*min,
	పూర్ణాంకం		*nat,
	పూర్ణాంకం		*min_baseline,
	పూర్ణాంకం		*nat_baseline)
{
    VimToolbar	*self = VIM_TOOLBAR(widget);
    పూర్ణాంకం		strip_min = 0, strip_nat = 0;
    పూర్ణాంకం		obtn_min = 0, obtn_nat = 0;

    gtk_widget_measure(self->strip, orientation, for_కొలత,
	    &strip_min, &strip_nat, ఖాళీ, ఖాళీ);
    gtk_widget_measure(self->అధికప్రవాహం_btn, orientation, for_కొలత,
	    &obtn_min, &obtn_nat, ఖాళీ, ఖాళీ);

    ఒకవేళ (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
	ఒకవేళ (min != ఖాళీ)
	    *min = obtn_min;
	ఒకవేళ (nat != ఖాళీ)
	    *nat = strip_nat + obtn_nat;
    }
    లేకపోతే
    {
	ఒకవేళ (min != ఖాళీ)
	    *min = MAX(strip_min, obtn_min);
	ఒకవేళ (nat != ఖాళీ)
	    *nat = MAX(strip_nat, obtn_nat);
    }

    ఒకవేళ (min_baseline != ఖాళీ)
	*min_baseline = -1;
    ఒకవేళ (nat_baseline != ఖాళీ)
	*nat_baseline = -1;
}

#అయితే_ముగింపు // FEAT_TOOLBAR
