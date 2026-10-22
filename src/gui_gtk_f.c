/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * (C) 1998,1999 by Marcin Dalecki <martin@dalecki.de>
 *
 * Support for GTK+ 2 was added by:
 * (C) 2002,2003  Jason Hildebrand  <jason@peaceworks.ca>
 *		  Daniel Elstner  <daniel.elstner@gmx.net>
 *
 * This is a special purpose container widget, which manages arbitrary
 * children at arbitrary positions width arbitrary sizes.  This finally puts
 * an end on our resize problems with which we where struggling for such a
 * long time.
 *
 * Support for GTK+ 3 was added by:
 * 2016  Kazunobu Kuriyama  <kazunobu.kuriyama@gmail.com>
 */

#شامل "vim.h"
#شامل <gtk/gtk.h>	// without this it compiles, but gives errors at
			// runtime!
#شامل "gui_gtk_f.h"
#اگر !GTK_CHECK_نسخہ(3,0,0)
# include <gtk/gtksignal.h>
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
# include <gdk/gdkwin32.h>
#ورنہ
# include <gdk/gdkx.h>
#اگر_ختم

قسمتعریف ساخت Gtk_Form_بچہ Gtk_Form_بچہ_2;

ساخت Gtk_Form_بچہ
{
    GtkWidget *widget;
    Gdk_کھڑکی *کھڑکی_2;
    gint x;		// relative subwidget x position
    gint y;		// relative subwidget y position
    gint mapped;
};


ساکن خالی gui_gtk_form_class_init(GtkFormClass *klass);
#اگر GTK_CHECK_نسخہ(3,0,0)
ساکن خالی gui_gtk_form_init(GtkForm *form);
#ورنہ
ساکن خالی gui_gtk_form_init(GtkForm *form, خالی *g_class);
#اگر_ختم

ساکن خالی form_realize(GtkWidget *widget);
ساکن خالی form_unrealize(GtkWidget *widget);
ساکن خالی form_map(GtkWidget *widget);
ساکن خالی form_سائز_request(GtkWidget *widget, GtkRequisition *requisition);
#اگر GTK_CHECK_نسخہ(3,0,0)
ساکن خالی form_get_preferred_چوڑائی(GtkWidget *widget, gint *اقل_چوڑائی, gint *natural_چوڑائی);
ساکن خالی form_get_preferred_اونچائی(GtkWidget *widget, gint *اقل_اونچائی, gint *natural_اونچائی);
#اگر_ختم
ساکن خالی form_سائز_allocate(GtkWidget *widget, Gtk_تخصیص *تخصیص);
#اگر GTK_CHECK_نسخہ(3,0,0)
ساکن gboolean form_draw(GtkWidget *widget, cairo_t *cr);
#ورنہ
ساکن gint form_expose(GtkWidget *widget, GdkEventExpose *event);
#اگر_ختم

ساکن خالی form_remove(GtkContainer *container, GtkWidget *widget);
ساکن خالی form_forall(GtkContainer *container, gboolean include_internals, GtkCallback callback, gpointer callback_ڈیٹا);

ساکن خالی form_attach_بچہ_کھڑکی(GtkForm *form, Gtk_Form_بچہ_2 *بچہ);
ساکن خالی form_realize_بچہ(GtkForm *form, Gtk_Form_بچہ_2 *بچہ);
ساکن خالی form_position_بچہ(GtkForm *form, Gtk_Form_بچہ_2 *بچہ, gboolean force_allocate);
ساکن خالی form_position_children(GtkForm *form);

ساکن خالی form_send_configure(GtkForm *form);

ساکن خالی form_بچہ_map(GtkWidget *widget, gpointer user_ڈیٹا);
ساکن خالی form_بچہ_unmap(GtkWidget *widget, gpointer user_ڈیٹا);

#اگر !GTK_CHECK_نسخہ(3,0,0)
ساکن GtkWidgetClass *والد_class = تہی;
#اگر_ختم

// Public interface

    GtkWidget *
gui_gtk_form_new(خالی)
{
    GtkForm *form;

#اگر GTK_CHECK_نسخہ(3,0,0)
    form = g_object_new(GTK_TYPE_FORM, تہی);
#ورنہ
    form = gtk_type_new(gui_gtk_form_get_type());
#اگر_ختم

    واپس GTK_WIDGET(form);
}

    خالی
gui_gtk_form_put(
	GtkForm	*form,
	GtkWidget	*بچہ_widget,
	gint	x,
	gint	y)
{
    Gtk_Form_بچہ_2 *بچہ;

    g_return_if_fail(GTK_IS_FORM(form));

    // LINTED: avoid warning: conversion to 'unsigned long'
    بچہ = g_new(Gtk_Form_بچہ_2, 1);
    اگر (بچہ == تہی)
	واپس;

    بچہ->widget = بچہ_widget;
    بچہ->کھڑکی_2 = تہی;
    بچہ->x = x;
    بچہ->y = y;
#اگر GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_سیٹ_سائز_request(بچہ->widget, -1, -1);
#ورنہ
    بچہ->widget->requisition.چوڑائی = 0;
    بچہ->widget->requisition.اونچائی = 0;
#اگر_ختم
    بچہ->mapped = FALSE;

    form->children = g_list_append(form->children, بچہ);

    // child->window must be created and attached to the widget _before_
    // it has been realized, or else things will break with GTK2.  Note
    // that gtk_widget_set_parent() realizes the widget if it's visible
    // and its parent is mapped.
    اگر (gtk_widget_get_realized(GTK_WIDGET(form)))
	form_attach_بچہ_کھڑکی(form, بچہ);

    gtk_widget_سیٹ_والد(بچہ_widget, GTK_WIDGET(form));

    اگر (gtk_widget_get_realized(GTK_WIDGET(form))
	    && !gtk_widget_get_realized(بچہ_widget))
	form_realize_بچہ(form, بچہ);

    form_position_بچہ(form, بچہ, TRUE);
}

    خالی
gui_gtk_form_move(
	GtkForm	*form,
	GtkWidget	*بچہ_widget,
	gint	x,
	gint	y)
{
    GList *tmp_list;
    Gtk_Form_بچہ_2 *بچہ;

    g_return_if_fail(GTK_IS_FORM(form));

    برائے (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
    {
	بچہ = tmp_list->ڈیٹا;
	اگر (بچہ->widget == بچہ_widget)
	{
	    بچہ->x = x;
	    بچہ->y = y;

	    form_position_بچہ(form, بچہ, TRUE);
	    واپس;
	}
    }
}

    خالی
gui_gtk_form_freeze(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ++form->freeze_گنتی;
}

    خالی
gui_gtk_form_thaw(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    اگر (!form->freeze_گنتی)
	واپس;

    اگر (!(--form->freeze_گنتی))
    {
	form_position_children(form);
	gtk_widget_قطار_draw(GTK_WIDGET(form));
    }
}

// Basic Object handling procedures

#اگر GTK_CHECK_نسخہ(3,0,0)
G_DEFINE_TYPE(GtkForm, gui_gtk_form, GTK_TYPE_CONTAINER)
#ورنہ
    GtkType
gui_gtk_form_get_type(خالی)
{
    ساکن GtkType form_type = 0;

    اگر (!form_type)
    {
	GtkTypeInfo form_info;

	CLEAR_میدان(form_info);
	form_info.type_name = "GtkForm";
	form_info.object_سائز = حجم(GtkForm);
	form_info.class_سائز = حجم(GtkFormClass);
	form_info.class_init_func = (GtkClassInitFunc)gui_gtk_form_class_init;
	form_info.object_init_func = (GtkObjectInitFunc)gui_gtk_form_init;

	form_type = gtk_type_unique(GTK_TYPE_CONTAINER, &form_info);
    }
    واپس form_type;
}
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)

    ساکن خالی
gui_gtk_form_class_init(GtkFormClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    widget_class = (GtkWidgetClass *) klass;
    container_class = (GtkContainerClass *) klass;

#اگر !GTK_CHECK_نسخہ(3,0,0)
    والد_class = gtk_type_class(gtk_container_get_type());
#اگر_ختم

    widget_class->realize = form_realize;
    widget_class->unrealize = form_unrealize;
    widget_class->map = form_map;
#اگر GTK_CHECK_نسخہ(3,0,0)
    widget_class->get_preferred_چوڑائی = form_get_preferred_چوڑائی;
    widget_class->get_preferred_اونچائی = form_get_preferred_اونچائی;
#ورنہ
    widget_class->سائز_request = form_سائز_request;
#اگر_ختم
    widget_class->سائز_allocate = form_سائز_allocate;
#اگر GTK_CHECK_نسخہ(3,0,0)
    widget_class->draw = form_draw;
#ورنہ
    widget_class->expose_event = form_expose;
#اگر_ختم

    container_class->remove = form_remove;
    container_class->forall = form_forall;
}

    ساکن خالی
gui_gtk_form_init(GtkForm *form
#اگر !GTK_CHECK_نسخہ(3,0,0)
	, خالی *g_class غیر_مستعمل
#اگر_ختم
	)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_سیٹ_has_کھڑکی(GTK_WIDGET(form), TRUE);
#اگر_ختم
    form->children = تہی;
    form->bin_کھڑکی = تہی;
    form->freeze_گنتی = 0;
}

/*
 * Widget methods
 */

    ساکن خالی
form_realize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gdk_کھڑکی_Attr اوصاف;
    gint اوصاف_نقاب;
    Gtk_تخصیص تخصیص;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);
    gtk_widget_سیٹ_realized(widget, TRUE);

    gtk_widget_get_تخصیص(widget, &تخصیص);
    اوصاف.کھڑکی_type = GDK_کھڑکی_بچہ;
    اوصاف.x = تخصیص.x;
    اوصاف.y = تخصیص.y;
    اوصاف.چوڑائی = تخصیص.چوڑائی;
    اوصاف.اونچائی = تخصیص.اونچائی;
    اوصاف.wclass = GDK_داخلہ_OUTPUT;
    اوصاف.visual = gtk_widget_get_visual(widget);
#اگر GTK_CHECK_نسخہ(3,0,0)
    اوصاف.event_نقاب = GDK_EXPOSURE_نقاب;
#ورنہ
    اوصاف.colormap = gtk_widget_get_colormap(widget);
    اوصاف.event_نقاب = GDK_VISIBILITY_NOTIFY_نقاب;
#اگر_ختم

#اگر GTK_CHECK_نسخہ(3,0,0)
    اوصاف_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#ورنہ
    اوصاف_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#اگر_ختم

    gtk_widget_سیٹ_کھڑکی(widget,
			  gdk_کھڑکی_new(gtk_widget_get_والد_کھڑکی(widget),
					 &اوصاف, اوصاف_نقاب));
    gdk_کھڑکی_سیٹ_user_ڈیٹا(gtk_widget_get_کھڑکی(widget), widget);

    اوصاف.x = 0;
    اوصاف.y = 0;
    اوصاف.event_نقاب = gtk_widget_get_events(widget);

    form->bin_کھڑکی = gdk_کھڑکی_new(gtk_widget_get_کھڑکی(widget),
				      &اوصاف, اوصاف_نقاب);
    gdk_کھڑکی_سیٹ_user_ڈیٹا(form->bin_کھڑکی, widget);

#اگر GTK_CHECK_نسخہ(3,0,0)
    {
	GtkStyleContext * ثابت sctx = gtk_widget_get_style_context(widget);

	gtk_style_context_جوڑو_class(sctx, "gtk-form");
	gtk_style_context_سیٹ_state(sctx, GTK_STATE_FLAG_معمول);
# اگر !GTK_CHECK_نسخہ(3,18,0)
	gtk_style_context_سیٹ_background(sctx, gtk_widget_get_کھڑکی(widget));
	gtk_style_context_سیٹ_background(sctx, form->bin_کھڑکی);
# endif
    }
#ورنہ
    widget->style = gtk_style_attach(widget->style, widget->کھڑکی_2);
    gtk_style_سیٹ_background(widget->style, widget->کھڑکی_2, GTK_STATE_معمول);
    gtk_style_سیٹ_background(widget->style, form->bin_کھڑکی, GTK_STATE_معمول);
#اگر_ختم

    برائے (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
    {
	Gtk_Form_بچہ_2 *بچہ = tmp_list->ڈیٹا;

	form_attach_بچہ_کھڑکی(form, بچہ);

	اگر (gtk_widget_get_visible(بچہ->widget))
	    form_realize_بچہ(form, بچہ);
    }
}


// After reading the documentation at
// https://www.manpagez.com/html/gtk2/gtk2-2.24.29/gtk-changes-2-0.php
// I think it should be possible to remove this function when compiling
// against gtk-2.0.  It doesn't seem to cause problems, though.
//
// Well, I reckon at least the gdk_window_show(form->bin_window)
// is necessary.  GtkForm is anything but a usual container widget.
    ساکن خالی
form_map(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    gtk_widget_سیٹ_mapped(widget, TRUE);

    gdk_کھڑکی_show(gtk_widget_get_کھڑکی(widget));
    gdk_کھڑکی_show(form->bin_کھڑکی);

    برائے (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
    {
	Gtk_Form_بچہ_2 *بچہ = tmp_list->ڈیٹا;

	اگر (gtk_widget_get_visible(بچہ->widget)
		&& !gtk_widget_get_mapped(بچہ->widget))
	    gtk_widget_map(بچہ->widget);
    }
}

    ساکن خالی
form_unrealize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    tmp_list = form->children;

    gdk_کھڑکی_سیٹ_user_ڈیٹا(form->bin_کھڑکی, تہی);
    gdk_کھڑکی_destroy(form->bin_کھڑکی);
    form->bin_کھڑکی = تہی;

    جب (tmp_list)
    {
	Gtk_Form_بچہ_2 *بچہ = tmp_list->ڈیٹا;

	اگر (بچہ->کھڑکی_2 != تہی)
	{
	    g_signal_handlers_disconnect_by_func(G_OBJECT(بچہ->widget),
		    FUNC2GENERIC(form_بچہ_map),
		    بچہ);
	    g_signal_handlers_disconnect_by_func(G_OBJECT(بچہ->widget),
		    FUNC2GENERIC(form_بچہ_unmap),
		    بچہ);

	    gdk_کھڑکی_سیٹ_user_ڈیٹا(بچہ->کھڑکی_2, تہی);
	    gdk_کھڑکی_destroy(بچہ->کھڑکی_2);

	    بچہ->کھڑکی_2 = تہی;
	}

	tmp_list = tmp_list->اگلا;
    }

#اگر GTK_CHECK_نسخہ(3,0,0)
    اگر (GTK_WIDGET_CLASS (gui_gtk_form_والد_class)->unrealize)
	 (* GTK_WIDGET_CLASS (gui_gtk_form_والد_class)->unrealize) (widget);
#ورنہ
    اگر (GTK_WIDGET_CLASS (والد_class)->unrealize)
	 (* GTK_WIDGET_CLASS (والد_class)->unrealize) (widget);
#اگر_ختم
}

    ساکن خالی
form_سائز_request(GtkWidget *widget, GtkRequisition *requisition)
{
    g_return_if_fail(GTK_IS_FORM(widget));
    g_return_if_fail(requisition != تہی);

    requisition->چوڑائی = 1;
    requisition->اونچائی = 1;
}

#اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن خالی
form_get_preferred_چوڑائی(GtkWidget *widget,
			     gint      *اقل_چوڑائی,
			     gint      *natural_چوڑائی)
{
    GtkRequisition requisition;

    form_سائز_request(widget, &requisition);

    *اقل_چوڑائی = requisition.چوڑائی;
    *natural_چوڑائی = requisition.چوڑائی;
}

    ساکن خالی
form_get_preferred_اونچائی(GtkWidget *widget,
			      gint	*اقل_اونچائی,
			      gint	*natural_اونچائی)
{
    GtkRequisition requisition;

    form_سائز_request(widget, &requisition);

    *اقل_اونچائی = requisition.اونچائی;
    *natural_اونچائی = requisition.اونچائی;
}
#اگر_ختم // GTK_CHECK_VERSION(3,0,0)

    ساکن خالی
form_سائز_allocate(GtkWidget *widget, Gtk_تخصیص *تخصیص)
{
    GList *tmp_list;
    GtkForm *form;
    gboolean need_reposition;
    Gtk_تخصیص cur_alloc;

    g_return_if_fail(GTK_IS_FORM(widget));

    gtk_widget_get_تخصیص(widget, &cur_alloc);

    اگر (cur_alloc.x == تخصیص->x
	    && cur_alloc.y == تخصیص->y
	    && cur_alloc.چوڑائی == تخصیص->چوڑائی
	    && cur_alloc.اونچائی == تخصیص->اونچائی)
	واپس;

    need_reposition = cur_alloc.چوڑائی != تخصیص->چوڑائی
		   || cur_alloc.اونچائی != تخصیص->اونچائی;
    form = GTK_FORM(widget);

    اگر (need_reposition)
    {
	tmp_list = form->children;

	جب (tmp_list)
	{
	    Gtk_Form_بچہ_2 *بچہ = tmp_list->ڈیٹا;
	    form_position_بچہ(form, بچہ, TRUE);

	    tmp_list = tmp_list->اگلا;
	}
    }

    اگر (gtk_widget_get_realized(widget))
    {
	gdk_کھڑکی_move_resize(gtk_widget_get_کھڑکی(widget),
			       تخصیص->x, تخصیص->y,
			       تخصیص->چوڑائی, تخصیص->اونچائی);
	gdk_کھڑکی_move_resize(GTK_FORM(widget)->bin_کھڑکی,
			       0, 0,
			       تخصیص->چوڑائی, تخصیص->اونچائی);
    }
    gtk_widget_سیٹ_تخصیص(widget, تخصیص);
    اگر (need_reposition)
	form_send_configure(form);
}

#اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن خالی
gtk_form_render_background(GtkWidget *widget, cairo_t *cr)
{
    gtk_render_background(gtk_widget_get_style_context(widget), cr,
			  0, 0,
			  gtk_widget_get_allocated_چوڑائی(widget),
			  gtk_widget_get_allocated_اونچائی(widget));
}

    ساکن gboolean
form_draw(GtkWidget *widget, cairo_t *cr)
{
    GList   *tmp_list = تہی;
    GtkForm *form     = تہی;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    gtk_form_render_background(widget, cr);

    form = GTK_FORM(widget);
    برائے (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
    {
	Gtk_Form_بچہ_2 * ثابت formchild = tmp_list->ڈیٹا;

	اگر (!gtk_widget_get_has_کھڑکی(formchild->widget) &&
		gtk_cairo_should_draw_کھڑکی(cr, formchild->کھڑکی_2))
	{
	    // To get gtk_widget_draw() to work, it is required to call
	    // gtk_widget_size_allocate() in advance with a well-posed
	    // allocation for a given child widget in order to set a
	    // certain private GtkWidget variable, called
	    // widget->priv->alloc_need, to the proper value; otherwise,
	    // gtk_widget_draw() fails and the relevant scrollbar won't
	    // appear on the screen.
	    //
	    // Calling form_position_child() like this is one of ways
	    // to make sure of that.
	    form_position_بچہ(form, formchild, TRUE);

	    gtk_form_render_background(formchild->widget, cr);
	}
    }

    واپس GTK_WIDGET_CLASS(gui_gtk_form_والد_class)->draw(widget, cr);
}
#ورنہ // !GTK_CHECK_VERSION(3,0,0)
    ساکن gint
form_expose(GtkWidget *widget, GdkEventExpose *event)
{
    GList   *tmp_list;
    GtkForm *form;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    form = GTK_FORM(widget);

    اگر (event->کھڑکی_2 == form->bin_کھڑکی)
	واپس FALSE;

    برائے (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
	gtk_container_propagate_expose(GTK_CONTAINER(widget),
		GTK_WIDGET(((Gtk_Form_بچہ_2 *)tmp_list->ڈیٹا)->widget),
		event);

    واپس FALSE;
}
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)

// Container method
    ساکن خالی
form_remove(GtkContainer *container, GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gtk_Form_بچہ_2 *بچہ = تہی;	    // init for gcc

    g_return_if_fail(GTK_IS_FORM(container));

    form = GTK_FORM(container);

    tmp_list = form->children;
    جب (tmp_list)
    {
	بچہ = tmp_list->ڈیٹا;
	اگر (بچہ->widget == widget)
	    توڑو;
	tmp_list = tmp_list->اگلا;
    }

    اگر (tmp_list == تہی)
	واپس;

#اگر GTK_CHECK_نسخہ(3,0,0)
    ثابت gboolean was_visible = gtk_widget_get_visible(widget);
#اگر_ختم
    اگر (بچہ->کھڑکی_2)
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(بچہ->widget),
		FUNC2GENERIC(&form_بچہ_map), بچہ);
	g_signal_handlers_disconnect_by_func(G_OBJECT(بچہ->widget),
		FUNC2GENERIC(&form_بچہ_unmap), بچہ);

	// FIXME: This will cause problems for reparenting NO_WINDOW
	// widgets out of a GtkForm
	gdk_کھڑکی_سیٹ_user_ڈیٹا(بچہ->کھڑکی_2, تہی);
	gdk_کھڑکی_destroy(بچہ->کھڑکی_2);
    }
    gtk_widget_unparent(widget);
#اگر GTK_CHECK_نسخہ(3,0,0)
    اگر (was_visible)
	gtk_widget_قطار_resize(GTK_WIDGET(container));
#اگر_ختم
    form->children = g_list_remove_link(form->children, tmp_list);
    g_list_free_1(tmp_list);
    g_free(بچہ);
}

    ساکن خالی
form_forall(GtkContainer	*container,
		gboolean	include_internals غیر_مستعمل,
		GtkCallback	callback,
		gpointer	callback_ڈیٹا)
{
    GtkForm *form;
    Gtk_Form_بچہ_2 *بچہ;
    GList *tmp_list;

    g_return_if_fail(GTK_IS_FORM(container));
    g_return_if_fail(callback != تہی);

    form = GTK_FORM(container);

    tmp_list = form->children;
    جب (tmp_list)
    {
	بچہ = tmp_list->ڈیٹا;
	tmp_list = tmp_list->اگلا;

	(*callback) (بچہ->widget, callback_ڈیٹا);
    }
}

// Operations on children

    ساکن خالی
form_attach_بچہ_کھڑکی(GtkForm *form, Gtk_Form_بچہ_2 *بچہ)
{
    اگر (بچہ->کھڑکی_2 != تہی)
	واپس; // been there, done that

    اگر (!gtk_widget_get_has_کھڑکی(بچہ->widget))
    {
	GtkWidget	*widget;
	Gdk_کھڑکی_Attr	اوصاف;
	gint		اوصاف_نقاب;
	GtkRequisition	requisition;

	widget = GTK_WIDGET(form);

#اگر GTK_CHECK_نسخہ(3,0,0)
	gtk_widget_get_preferred_سائز(بچہ->widget, &requisition, تہی);
#ورنہ
	requisition = بچہ->widget->requisition;
#اگر_ختم
	اوصاف.کھڑکی_type = GDK_کھڑکی_بچہ;
	اوصاف.x = بچہ->x;
	اوصاف.y = بچہ->y;
	اوصاف.چوڑائی = requisition.چوڑائی;
	اوصاف.اونچائی = requisition.اونچائی;
	اوصاف.wclass = GDK_داخلہ_OUTPUT;
	اوصاف.visual = gtk_widget_get_visual(widget);
#اگر !GTK_CHECK_نسخہ(3,0,0)
	اوصاف.colormap = gtk_widget_get_colormap(widget);
#اگر_ختم
	اوصاف.event_نقاب = GDK_EXPOSURE_نقاب;

#اگر GTK_CHECK_نسخہ(3,0,0)
	اوصاف_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#ورنہ
	اوصاف_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#اگر_ختم
	بچہ->کھڑکی_2 = gdk_کھڑکی_new(form->bin_کھڑکی,
				       &اوصاف, اوصاف_نقاب);
	gdk_کھڑکی_سیٹ_user_ڈیٹا(بچہ->کھڑکی_2, widget);

#اگر GTK_CHECK_نسخہ(3,0,0)
	{
	    GtkStyleContext * ثابت sctx = gtk_widget_get_style_context(widget);

	    gtk_style_context_سیٹ_state(sctx, GTK_STATE_FLAG_معمول);
# اگر !GTK_CHECK_نسخہ(3,18,0)
	    gtk_style_context_سیٹ_background(sctx, بچہ->کھڑکی_2);
# endif
	}
#ورنہ
	gtk_style_سیٹ_background(widget->style,
				 بچہ->کھڑکی_2,
				 GTK_STATE_معمول);
#اگر_ختم

	gtk_widget_سیٹ_والد_کھڑکی(بچہ->widget, بچہ->کھڑکی_2);
	/*
	 * Install signal handlers to map/unmap child->window
	 * alongside with the actual widget.
	 */
	g_signal_connect(G_OBJECT(بچہ->widget), "map",
			 G_CALLBACK(&form_بچہ_map), بچہ);
	g_signal_connect(G_OBJECT(بچہ->widget), "unmap",
			 G_CALLBACK(&form_بچہ_unmap), بچہ);
    }
    ورنہ اگر (!gtk_widget_get_realized(بچہ->widget))
    {
	gtk_widget_سیٹ_والد_کھڑکی(بچہ->widget, form->bin_کھڑکی);
    }
}

    ساکن خالی
form_realize_بچہ(GtkForm *form, Gtk_Form_بچہ_2 *بچہ)
{
    form_attach_بچہ_کھڑکی(form, بچہ);
    gtk_widget_realize(بچہ->widget);
}

    ساکن خالی
form_position_بچہ(
    GtkForm		*form,
    Gtk_Form_بچہ_2	*بچہ,
    gboolean		force_allocate)
{
    gint x;
    gint y;

    x = بچہ->x;
    y = بچہ->y;

    اگر ((x >= G_MINSHORT) && (x <= G_MAXSHORT) &&
	(y >= G_MINSHORT) && (y <= G_MAXSHORT))
    {
	اگر (!بچہ->mapped)
	{
	    اگر (gtk_widget_get_mapped(GTK_WIDGET(form))
		    && gtk_widget_get_visible(بچہ->widget))
	    {
		اگر (!gtk_widget_get_mapped(بچہ->widget))
		    gtk_widget_map(بچہ->widget);

		بچہ->mapped = TRUE;
		force_allocate = TRUE;
	    }
	}

	اگر (force_allocate)
	{
	    Gtk_تخصیص تخصیص;
	    GtkRequisition requisition;

#اگر GTK_CHECK_نسخہ(3,0,0)
	    gtk_widget_get_preferred_سائز(بچہ->widget, &requisition, تہی);
#ورنہ
	    requisition = بچہ->widget->requisition;
#اگر_ختم

	    اگر (!gtk_widget_get_has_کھڑکی(بچہ->widget))
	    {
		اگر (بچہ->کھڑکی_2)
		{
		    gdk_کھڑکی_move_resize(بچہ->کھڑکی_2,
			    x, y,
			    requisition.چوڑائی,
			    requisition.اونچائی);
		}

		تخصیص.x = 0;
		تخصیص.y = 0;
	    }
	    ورنہ
	    {
		تخصیص.x = x;
		تخصیص.y = y;
	    }

	    تخصیص.چوڑائی = requisition.چوڑائی;
	    تخصیص.اونچائی = requisition.اونچائی;

	    gtk_widget_سائز_allocate(بچہ->widget, &تخصیص);
	}
    }
    ورنہ
    {
	اگر (بچہ->mapped)
	{
	    بچہ->mapped = FALSE;

	    اگر (gtk_widget_get_mapped(بچہ->widget))
		gtk_widget_unmap(بچہ->widget);
	}
    }
}

    ساکن خالی
form_position_children(GtkForm *form)
{
    GList *tmp_list;

    برائے (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
	form_position_بچہ(form, tmp_list->ڈیٹا, FALSE);
}

    خالی
gui_gtk_form_move_resize(GtkForm *form, GtkWidget *widget,
		     gint x, gint y, gint w, gint h)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_سیٹ_سائز_request(widget, w, h);
#ورنہ
    widget->requisition.چوڑائی  = w;
    widget->requisition.اونچائی = h;
#اگر_ختم

    gui_gtk_form_move(form, widget, x, y);
}

    ساکن خالی
form_send_configure(GtkForm *form)
{
    GtkWidget *widget;
    GdkEventConfigure event;
    Gtk_تخصیص تخصیص;

    widget = GTK_WIDGET(form);

    gtk_widget_get_تخصیص(widget, &تخصیص);
    event.type = GDK_CONFIGURE;
    event.کھڑکی_2 = gtk_widget_get_کھڑکی(widget);
    event.x = تخصیص.x;
    event.y = تخصیص.y;
    event.چوڑائی = تخصیص.چوڑائی;
    event.اونچائی = تخصیص.اونچائی;

    gtk_main_do_event((GdkEvent*)&event);
}

    ساکن خالی
form_بچہ_map(GtkWidget *widget غیر_مستعمل, gpointer user_ڈیٹا)
{
    Gtk_Form_بچہ_2 *بچہ;

    بچہ = (Gtk_Form_بچہ_2 *)user_ڈیٹا;

    بچہ->mapped = TRUE;
    gdk_کھڑکی_show(بچہ->کھڑکی_2);
}

    ساکن خالی
form_بچہ_unmap(GtkWidget *widget غیر_مستعمل, gpointer user_ڈیٹا)
{
    Gtk_Form_بچہ_2 *بچہ;

    بچہ = (Gtk_Form_بچہ_2 *)user_ڈیٹا;

    بچہ->mapped = FALSE;
    gdk_کھڑکی_hide(بچہ->کھڑکی_2);
}
