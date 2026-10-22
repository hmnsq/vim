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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <gtk/gtk.h>	// without this it compiles, but gives errors at
			// runtime!
#ਅਨ੍ਤਰ੍ਭਾਵ "gui_gtk_f.h"
#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
# include <gtk/gtksignal.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# include <gdk/gdkwin32.h>
#ਨਹੀਂਤਾਂ
# include <gdk/gdkx.h>
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Gtk_Form_ਬੱਚਾ Gtk_Form_ਬੱਚਾ_2;

ਬਣਤਰ Gtk_Form_ਬੱਚਾ
{
    GtkWidget *widget;
    Gdk_ਖਿੜਕੀ *ਖਿੜਕੀ_2;
    gint x;		// relative subwidget x position
    gint y;		// relative subwidget y position
    gint mapped;
};


ਸਥਿਰ ਖਾਲੀ gui_gtk_form_class_init(GtkFormClass *klass);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
ਸਥਿਰ ਖਾਲੀ gui_gtk_form_init(GtkForm *form);
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ gui_gtk_form_init(GtkForm *form, ਖਾਲੀ *g_class);
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ form_realize(GtkWidget *widget);
ਸਥਿਰ ਖਾਲੀ form_unrealize(GtkWidget *widget);
ਸਥਿਰ ਖਾਲੀ form_map(GtkWidget *widget);
ਸਥਿਰ ਖਾਲੀ form_ਮਾਪ_request(GtkWidget *widget, GtkRequisition *requisition);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
ਸਥਿਰ ਖਾਲੀ form_get_preferred_ਚੌੜਾਈ(GtkWidget *widget, gint *ਨਿਊਨਤਮ_ਚੌੜਾਈ, gint *natural_ਚੌੜਾਈ);
ਸਥਿਰ ਖਾਲੀ form_get_preferred_ਉਚਾਈ(GtkWidget *widget, gint *ਨਿਊਨਤਮ_ਉਚਾਈ, gint *natural_ਉਚਾਈ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ form_ਮਾਪ_allocate(GtkWidget *widget, Gtk_ਵੰਡ *ਵੰਡ);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
ਸਥਿਰ gboolean form_draw(GtkWidget *widget, cairo_t *cr);
#ਨਹੀਂਤਾਂ
ਸਥਿਰ gint form_expose(GtkWidget *widget, GdkEventExpose *event);
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ form_remove(GtkContainer *container, GtkWidget *widget);
ਸਥਿਰ ਖਾਲੀ form_forall(GtkContainer *container, gboolean include_internals, GtkCallback callback, gpointer callback_ਅੰਕੜੇ);

ਸਥਿਰ ਖਾਲੀ form_attach_ਬੱਚਾ_ਖਿੜਕੀ(GtkForm *form, Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ);
ਸਥਿਰ ਖਾਲੀ form_realize_ਬੱਚਾ(GtkForm *form, Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ);
ਸਥਿਰ ਖਾਲੀ form_position_ਬੱਚਾ(GtkForm *form, Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ, gboolean force_allocate);
ਸਥਿਰ ਖਾਲੀ form_position_children(GtkForm *form);

ਸਥਿਰ ਖਾਲੀ form_send_configure(GtkForm *form);

ਸਥਿਰ ਖਾਲੀ form_ਬੱਚਾ_map(GtkWidget *widget, gpointer user_ਅੰਕੜੇ);
ਸਥਿਰ ਖਾਲੀ form_ਬੱਚਾ_unmap(GtkWidget *widget, gpointer user_ਅੰਕੜੇ);

#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
ਸਥਿਰ GtkWidgetClass *ਮਾਪਾ_class = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

// Public interface

    GtkWidget *
gui_gtk_form_new(ਖਾਲੀ)
{
    GtkForm *form;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    form = g_object_new(GTK_TYPE_FORM, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
    form = gtk_type_new(gui_gtk_form_get_type());
#ਜੇ_ਅੰਤ

    ਵਾਪਸ GTK_WIDGET(form);
}

    ਖਾਲੀ
gui_gtk_form_put(
	GtkForm	*form,
	GtkWidget	*ਬੱਚਾ_widget,
	gint	x,
	gint	y)
{
    Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ;

    g_return_if_fail(GTK_IS_FORM(form));

    // LINTED: avoid warning: conversion to 'unsigned long'
    ਬੱਚਾ = g_new(Gtk_Form_ਬੱਚਾ_2, 1);
    ਜੇ (ਬੱਚਾ == ਨਿਰਾ)
	ਵਾਪਸ;

    ਬੱਚਾ->widget = ਬੱਚਾ_widget;
    ਬੱਚਾ->ਖਿੜਕੀ_2 = ਨਿਰਾ;
    ਬੱਚਾ->x = x;
    ਬੱਚਾ->y = y;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_widget_ਸੈੱਟ_ਮਾਪ_request(ਬੱਚਾ->widget, -1, -1);
#ਨਹੀਂਤਾਂ
    ਬੱਚਾ->widget->requisition.ਚੌੜਾਈ = 0;
    ਬੱਚਾ->widget->requisition.ਉਚਾਈ = 0;
#ਜੇ_ਅੰਤ
    ਬੱਚਾ->mapped = FALSE;

    form->children = g_list_append(form->children, ਬੱਚਾ);

    // child->window must be created and attached to the widget _before_
    // it has been realized, or else things will break with GTK2.  Note
    // that gtk_widget_set_parent() realizes the widget if it's visible
    // and its parent is mapped.
    ਜੇ (gtk_widget_get_realized(GTK_WIDGET(form)))
	form_attach_ਬੱਚਾ_ਖਿੜਕੀ(form, ਬੱਚਾ);

    gtk_widget_ਸੈੱਟ_ਮਾਪਾ(ਬੱਚਾ_widget, GTK_WIDGET(form));

    ਜੇ (gtk_widget_get_realized(GTK_WIDGET(form))
	    && !gtk_widget_get_realized(ਬੱਚਾ_widget))
	form_realize_ਬੱਚਾ(form, ਬੱਚਾ);

    form_position_ਬੱਚਾ(form, ਬੱਚਾ, TRUE);
}

    ਖਾਲੀ
gui_gtk_form_move(
	GtkForm	*form,
	GtkWidget	*ਬੱਚਾ_widget,
	gint	x,
	gint	y)
{
    GList *tmp_list;
    Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ;

    g_return_if_fail(GTK_IS_FORM(form));

    ਲਈ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ਅਗਲਾ)
    {
	ਬੱਚਾ = tmp_list->ਅੰਕੜੇ;
	ਜੇ (ਬੱਚਾ->widget == ਬੱਚਾ_widget)
	{
	    ਬੱਚਾ->x = x;
	    ਬੱਚਾ->y = y;

	    form_position_ਬੱਚਾ(form, ਬੱਚਾ, TRUE);
	    ਵਾਪਸ;
	}
    }
}

    ਖਾਲੀ
gui_gtk_form_freeze(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ++form->freeze_ਗਿਣਤੀ;
}

    ਖਾਲੀ
gui_gtk_form_thaw(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ਜੇ (!form->freeze_ਗਿਣਤੀ)
	ਵਾਪਸ;

    ਜੇ (!(--form->freeze_ਗਿਣਤੀ))
    {
	form_position_children(form);
	gtk_widget_ਕਤਾਰ_draw(GTK_WIDGET(form));
    }
}

// Basic Object handling procedures

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
G_DEFINE_TYPE(GtkForm, gui_gtk_form, GTK_TYPE_CONTAINER)
#ਨਹੀਂਤਾਂ
    GtkType
gui_gtk_form_get_type(ਖਾਲੀ)
{
    ਸਥਿਰ GtkType form_type = 0;

    ਜੇ (!form_type)
    {
	GtkTypeInfo form_info;

	CLEAR_ਖੇਤਰ(form_info);
	form_info.type_name = "GtkForm";
	form_info.object_ਮਾਪ = ਆਕਾਰ(GtkForm);
	form_info.class_ਮਾਪ = ਆਕਾਰ(GtkFormClass);
	form_info.class_init_func = (GtkClassInitFunc)gui_gtk_form_class_init;
	form_info.object_init_func = (GtkObjectInitFunc)gui_gtk_form_init;

	form_type = gtk_type_unique(GTK_TYPE_CONTAINER, &form_info);
    }
    ਵਾਪਸ form_type;
}
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)

    ਸਥਿਰ ਖਾਲੀ
gui_gtk_form_class_init(GtkFormClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    widget_class = (GtkWidgetClass *) klass;
    container_class = (GtkContainerClass *) klass;

#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਮਾਪਾ_class = gtk_type_class(gtk_container_get_type());
#ਜੇ_ਅੰਤ

    widget_class->realize = form_realize;
    widget_class->unrealize = form_unrealize;
    widget_class->map = form_map;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    widget_class->get_preferred_ਚੌੜਾਈ = form_get_preferred_ਚੌੜਾਈ;
    widget_class->get_preferred_ਉਚਾਈ = form_get_preferred_ਉਚਾਈ;
#ਨਹੀਂਤਾਂ
    widget_class->ਮਾਪ_request = form_ਮਾਪ_request;
#ਜੇ_ਅੰਤ
    widget_class->ਮਾਪ_allocate = form_ਮਾਪ_allocate;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    widget_class->draw = form_draw;
#ਨਹੀਂਤਾਂ
    widget_class->expose_event = form_expose;
#ਜੇ_ਅੰਤ

    container_class->remove = form_remove;
    container_class->forall = form_forall;
}

    ਸਥਿਰ ਖਾਲੀ
gui_gtk_form_init(GtkForm *form
#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	, ਖਾਲੀ *g_class ਅਣਵਰਤਿਆ
#ਜੇ_ਅੰਤ
	)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_widget_ਸੈੱਟ_has_ਖਿੜਕੀ(GTK_WIDGET(form), TRUE);
#ਜੇ_ਅੰਤ
    form->children = ਨਿਰਾ;
    form->bin_ਖਿੜਕੀ = ਨਿਰਾ;
    form->freeze_ਗਿਣਤੀ = 0;
}

/*
 * Widget methods
 */

    ਸਥਿਰ ਖਾਲੀ
form_realize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gdk_ਖਿੜਕੀ_Attr ਗੁਣ;
    gint ਗੁਣ_ਨਕਾਬ;
    Gtk_ਵੰਡ ਵੰਡ;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);
    gtk_widget_ਸੈੱਟ_realized(widget, TRUE);

    gtk_widget_get_ਵੰਡ(widget, &ਵੰਡ);
    ਗੁਣ.ਖਿੜਕੀ_type = GDK_ਖਿੜਕੀ_ਬੱਚਾ;
    ਗੁਣ.x = ਵੰਡ.x;
    ਗੁਣ.y = ਵੰਡ.y;
    ਗੁਣ.ਚੌੜਾਈ = ਵੰਡ.ਚੌੜਾਈ;
    ਗੁਣ.ਉਚਾਈ = ਵੰਡ.ਉਚਾਈ;
    ਗੁਣ.wclass = GDK_ਨਿਵੇਸ਼_OUTPUT;
    ਗੁਣ.visual = gtk_widget_get_visual(widget);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਗੁਣ.event_ਨਕਾਬ = GDK_EXPOSURE_ਨਕਾਬ;
#ਨਹੀਂਤਾਂ
    ਗੁਣ.colormap = gtk_widget_get_colormap(widget);
    ਗੁਣ.event_ਨਕਾਬ = GDK_VISIBILITY_NOTIFY_ਨਕਾਬ;
#ਜੇ_ਅੰਤ

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਗੁਣ_ਨਕਾਬ = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#ਨਹੀਂਤਾਂ
    ਗੁਣ_ਨਕਾਬ = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#ਜੇ_ਅੰਤ

    gtk_widget_ਸੈੱਟ_ਖਿੜਕੀ(widget,
			  gdk_ਖਿੜਕੀ_new(gtk_widget_get_ਮਾਪਾ_ਖਿੜਕੀ(widget),
					 &ਗੁਣ, ਗੁਣ_ਨਕਾਬ));
    gdk_ਖਿੜਕੀ_ਸੈੱਟ_user_ਅੰਕੜੇ(gtk_widget_get_ਖਿੜਕੀ(widget), widget);

    ਗੁਣ.x = 0;
    ਗੁਣ.y = 0;
    ਗੁਣ.event_ਨਕਾਬ = gtk_widget_get_events(widget);

    form->bin_ਖਿੜਕੀ = gdk_ਖਿੜਕੀ_new(gtk_widget_get_ਖਿੜਕੀ(widget),
				      &ਗੁਣ, ਗੁਣ_ਨਕਾਬ);
    gdk_ਖਿੜਕੀ_ਸੈੱਟ_user_ਅੰਕੜੇ(form->bin_ਖਿੜਕੀ, widget);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    {
	GtkStyleContext * ਅਚਲ sctx = gtk_widget_get_style_context(widget);

	gtk_style_context_ਜੋੜੋ_class(sctx, "gtk-form");
	gtk_style_context_ਸੈੱਟ_state(sctx, GTK_STATE_FLAG_ਸਾਧਾਰਨ);
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,18,0)
	gtk_style_context_ਸੈੱਟ_background(sctx, gtk_widget_get_ਖਿੜਕੀ(widget));
	gtk_style_context_ਸੈੱਟ_background(sctx, form->bin_ਖਿੜਕੀ);
# endif
    }
#ਨਹੀਂਤਾਂ
    widget->style = gtk_style_attach(widget->style, widget->ਖਿੜਕੀ_2);
    gtk_style_ਸੈੱਟ_background(widget->style, widget->ਖਿੜਕੀ_2, GTK_STATE_ਸਾਧਾਰਨ);
    gtk_style_ਸੈੱਟ_background(widget->style, form->bin_ਖਿੜਕੀ, GTK_STATE_ਸਾਧਾਰਨ);
#ਜੇ_ਅੰਤ

    ਲਈ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ਅਗਲਾ)
    {
	Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ = tmp_list->ਅੰਕੜੇ;

	form_attach_ਬੱਚਾ_ਖਿੜਕੀ(form, ਬੱਚਾ);

	ਜੇ (gtk_widget_get_visible(ਬੱਚਾ->widget))
	    form_realize_ਬੱਚਾ(form, ਬੱਚਾ);
    }
}


// After reading the documentation at
// https://www.manpagez.com/html/gtk2/gtk2-2.24.29/gtk-changes-2-0.php
// I think it should be possible to remove this function when compiling
// against gtk-2.0.  It doesn't seem to cause problems, though.
//
// Well, I reckon at least the gdk_window_show(form->bin_window)
// is necessary.  GtkForm is anything but a usual container widget.
    ਸਥਿਰ ਖਾਲੀ
form_map(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    gtk_widget_ਸੈੱਟ_mapped(widget, TRUE);

    gdk_ਖਿੜਕੀ_show(gtk_widget_get_ਖਿੜਕੀ(widget));
    gdk_ਖਿੜਕੀ_show(form->bin_ਖਿੜਕੀ);

    ਲਈ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ਅਗਲਾ)
    {
	Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ = tmp_list->ਅੰਕੜੇ;

	ਜੇ (gtk_widget_get_visible(ਬੱਚਾ->widget)
		&& !gtk_widget_get_mapped(ਬੱਚਾ->widget))
	    gtk_widget_map(ਬੱਚਾ->widget);
    }
}

    ਸਥਿਰ ਖਾਲੀ
form_unrealize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    tmp_list = form->children;

    gdk_ਖਿੜਕੀ_ਸੈੱਟ_user_ਅੰਕੜੇ(form->bin_ਖਿੜਕੀ, ਨਿਰਾ);
    gdk_ਖਿੜਕੀ_destroy(form->bin_ਖਿੜਕੀ);
    form->bin_ਖਿੜਕੀ = ਨਿਰਾ;

    ਜਦੋਂ (tmp_list)
    {
	Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ = tmp_list->ਅੰਕੜੇ;

	ਜੇ (ਬੱਚਾ->ਖਿੜਕੀ_2 != ਨਿਰਾ)
	{
	    g_signal_handlers_disconnect_by_func(G_OBJECT(ਬੱਚਾ->widget),
		    FUNC2GENERIC(form_ਬੱਚਾ_map),
		    ਬੱਚਾ);
	    g_signal_handlers_disconnect_by_func(G_OBJECT(ਬੱਚਾ->widget),
		    FUNC2GENERIC(form_ਬੱਚਾ_unmap),
		    ਬੱਚਾ);

	    gdk_ਖਿੜਕੀ_ਸੈੱਟ_user_ਅੰਕੜੇ(ਬੱਚਾ->ਖਿੜਕੀ_2, ਨਿਰਾ);
	    gdk_ਖਿੜਕੀ_destroy(ਬੱਚਾ->ਖਿੜਕੀ_2);

	    ਬੱਚਾ->ਖਿੜਕੀ_2 = ਨਿਰਾ;
	}

	tmp_list = tmp_list->ਅਗਲਾ;
    }

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਜੇ (GTK_WIDGET_CLASS (gui_gtk_form_ਮਾਪਾ_class)->unrealize)
	 (* GTK_WIDGET_CLASS (gui_gtk_form_ਮਾਪਾ_class)->unrealize) (widget);
#ਨਹੀਂਤਾਂ
    ਜੇ (GTK_WIDGET_CLASS (ਮਾਪਾ_class)->unrealize)
	 (* GTK_WIDGET_CLASS (ਮਾਪਾ_class)->unrealize) (widget);
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
form_ਮਾਪ_request(GtkWidget *widget, GtkRequisition *requisition)
{
    g_return_if_fail(GTK_IS_FORM(widget));
    g_return_if_fail(requisition != ਨਿਰਾ);

    requisition->ਚੌੜਾਈ = 1;
    requisition->ਉਚਾਈ = 1;
}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ ਖਾਲੀ
form_get_preferred_ਚੌੜਾਈ(GtkWidget *widget,
			     gint      *ਨਿਊਨਤਮ_ਚੌੜਾਈ,
			     gint      *natural_ਚੌੜਾਈ)
{
    GtkRequisition requisition;

    form_ਮਾਪ_request(widget, &requisition);

    *ਨਿਊਨਤਮ_ਚੌੜਾਈ = requisition.ਚੌੜਾਈ;
    *natural_ਚੌੜਾਈ = requisition.ਚੌੜਾਈ;
}

    ਸਥਿਰ ਖਾਲੀ
form_get_preferred_ਉਚਾਈ(GtkWidget *widget,
			      gint	*ਨਿਊਨਤਮ_ਉਚਾਈ,
			      gint	*natural_ਉਚਾਈ)
{
    GtkRequisition requisition;

    form_ਮਾਪ_request(widget, &requisition);

    *ਨਿਊਨਤਮ_ਉਚਾਈ = requisition.ਉਚਾਈ;
    *natural_ਉਚਾਈ = requisition.ਉਚਾਈ;
}
#ਜੇ_ਅੰਤ // GTK_CHECK_VERSION(3,0,0)

    ਸਥਿਰ ਖਾਲੀ
form_ਮਾਪ_allocate(GtkWidget *widget, Gtk_ਵੰਡ *ਵੰਡ)
{
    GList *tmp_list;
    GtkForm *form;
    gboolean need_reposition;
    Gtk_ਵੰਡ cur_alloc;

    g_return_if_fail(GTK_IS_FORM(widget));

    gtk_widget_get_ਵੰਡ(widget, &cur_alloc);

    ਜੇ (cur_alloc.x == ਵੰਡ->x
	    && cur_alloc.y == ਵੰਡ->y
	    && cur_alloc.ਚੌੜਾਈ == ਵੰਡ->ਚੌੜਾਈ
	    && cur_alloc.ਉਚਾਈ == ਵੰਡ->ਉਚਾਈ)
	ਵਾਪਸ;

    need_reposition = cur_alloc.ਚੌੜਾਈ != ਵੰਡ->ਚੌੜਾਈ
		   || cur_alloc.ਉਚਾਈ != ਵੰਡ->ਉਚਾਈ;
    form = GTK_FORM(widget);

    ਜੇ (need_reposition)
    {
	tmp_list = form->children;

	ਜਦੋਂ (tmp_list)
	{
	    Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ = tmp_list->ਅੰਕੜੇ;
	    form_position_ਬੱਚਾ(form, ਬੱਚਾ, TRUE);

	    tmp_list = tmp_list->ਅਗਲਾ;
	}
    }

    ਜੇ (gtk_widget_get_realized(widget))
    {
	gdk_ਖਿੜਕੀ_move_resize(gtk_widget_get_ਖਿੜਕੀ(widget),
			       ਵੰਡ->x, ਵੰਡ->y,
			       ਵੰਡ->ਚੌੜਾਈ, ਵੰਡ->ਉਚਾਈ);
	gdk_ਖਿੜਕੀ_move_resize(GTK_FORM(widget)->bin_ਖਿੜਕੀ,
			       0, 0,
			       ਵੰਡ->ਚੌੜਾਈ, ਵੰਡ->ਉਚਾਈ);
    }
    gtk_widget_ਸੈੱਟ_ਵੰਡ(widget, ਵੰਡ);
    ਜੇ (need_reposition)
	form_send_configure(form);
}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ ਖਾਲੀ
gtk_form_render_background(GtkWidget *widget, cairo_t *cr)
{
    gtk_render_background(gtk_widget_get_style_context(widget), cr,
			  0, 0,
			  gtk_widget_get_allocated_ਚੌੜਾਈ(widget),
			  gtk_widget_get_allocated_ਉਚਾਈ(widget));
}

    ਸਥਿਰ gboolean
form_draw(GtkWidget *widget, cairo_t *cr)
{
    GList   *tmp_list = ਨਿਰਾ;
    GtkForm *form     = ਨਿਰਾ;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    gtk_form_render_background(widget, cr);

    form = GTK_FORM(widget);
    ਲਈ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ਅਗਲਾ)
    {
	Gtk_Form_ਬੱਚਾ_2 * ਅਚਲ formchild = tmp_list->ਅੰਕੜੇ;

	ਜੇ (!gtk_widget_get_has_ਖਿੜਕੀ(formchild->widget) &&
		gtk_cairo_should_draw_ਖਿੜਕੀ(cr, formchild->ਖਿੜਕੀ_2))
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
	    form_position_ਬੱਚਾ(form, formchild, TRUE);

	    gtk_form_render_background(formchild->widget, cr);
	}
    }

    ਵਾਪਸ GTK_WIDGET_CLASS(gui_gtk_form_ਮਾਪਾ_class)->draw(widget, cr);
}
#ਨਹੀਂਤਾਂ // !GTK_CHECK_VERSION(3,0,0)
    ਸਥਿਰ gint
form_expose(GtkWidget *widget, GdkEventExpose *event)
{
    GList   *tmp_list;
    GtkForm *form;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    form = GTK_FORM(widget);

    ਜੇ (event->ਖਿੜਕੀ_2 == form->bin_ਖਿੜਕੀ)
	ਵਾਪਸ FALSE;

    ਲਈ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ਅਗਲਾ)
	gtk_container_propagate_expose(GTK_CONTAINER(widget),
		GTK_WIDGET(((Gtk_Form_ਬੱਚਾ_2 *)tmp_list->ਅੰਕੜੇ)->widget),
		event);

    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)

// Container method
    ਸਥਿਰ ਖਾਲੀ
form_remove(GtkContainer *container, GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ = ਨਿਰਾ;	    // init for gcc

    g_return_if_fail(GTK_IS_FORM(container));

    form = GTK_FORM(container);

    tmp_list = form->children;
    ਜਦੋਂ (tmp_list)
    {
	ਬੱਚਾ = tmp_list->ਅੰਕੜੇ;
	ਜੇ (ਬੱਚਾ->widget == widget)
	    ਰੋਕੋ;
	tmp_list = tmp_list->ਅਗਲਾ;
    }

    ਜੇ (tmp_list == ਨਿਰਾ)
	ਵਾਪਸ;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਅਚਲ gboolean was_visible = gtk_widget_get_visible(widget);
#ਜੇ_ਅੰਤ
    ਜੇ (ਬੱਚਾ->ਖਿੜਕੀ_2)
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(ਬੱਚਾ->widget),
		FUNC2GENERIC(&form_ਬੱਚਾ_map), ਬੱਚਾ);
	g_signal_handlers_disconnect_by_func(G_OBJECT(ਬੱਚਾ->widget),
		FUNC2GENERIC(&form_ਬੱਚਾ_unmap), ਬੱਚਾ);

	// FIXME: This will cause problems for reparenting NO_WINDOW
	// widgets out of a GtkForm
	gdk_ਖਿੜਕੀ_ਸੈੱਟ_user_ਅੰਕੜੇ(ਬੱਚਾ->ਖਿੜਕੀ_2, ਨਿਰਾ);
	gdk_ਖਿੜਕੀ_destroy(ਬੱਚਾ->ਖਿੜਕੀ_2);
    }
    gtk_widget_unparent(widget);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਜੇ (was_visible)
	gtk_widget_ਕਤਾਰ_resize(GTK_WIDGET(container));
#ਜੇ_ਅੰਤ
    form->children = g_list_remove_link(form->children, tmp_list);
    g_list_free_1(tmp_list);
    g_free(ਬੱਚਾ);
}

    ਸਥਿਰ ਖਾਲੀ
form_forall(GtkContainer	*container,
		gboolean	include_internals ਅਣਵਰਤਿਆ,
		GtkCallback	callback,
		gpointer	callback_ਅੰਕੜੇ)
{
    GtkForm *form;
    Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ;
    GList *tmp_list;

    g_return_if_fail(GTK_IS_FORM(container));
    g_return_if_fail(callback != ਨਿਰਾ);

    form = GTK_FORM(container);

    tmp_list = form->children;
    ਜਦੋਂ (tmp_list)
    {
	ਬੱਚਾ = tmp_list->ਅੰਕੜੇ;
	tmp_list = tmp_list->ਅਗਲਾ;

	(*callback) (ਬੱਚਾ->widget, callback_ਅੰਕੜੇ);
    }
}

// Operations on children

    ਸਥਿਰ ਖਾਲੀ
form_attach_ਬੱਚਾ_ਖਿੜਕੀ(GtkForm *form, Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ)
{
    ਜੇ (ਬੱਚਾ->ਖਿੜਕੀ_2 != ਨਿਰਾ)
	ਵਾਪਸ; // been there, done that

    ਜੇ (!gtk_widget_get_has_ਖਿੜਕੀ(ਬੱਚਾ->widget))
    {
	GtkWidget	*widget;
	Gdk_ਖਿੜਕੀ_Attr	ਗੁਣ;
	gint		ਗੁਣ_ਨਕਾਬ;
	GtkRequisition	requisition;

	widget = GTK_WIDGET(form);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	gtk_widget_get_preferred_ਮਾਪ(ਬੱਚਾ->widget, &requisition, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
	requisition = ਬੱਚਾ->widget->requisition;
#ਜੇ_ਅੰਤ
	ਗੁਣ.ਖਿੜਕੀ_type = GDK_ਖਿੜਕੀ_ਬੱਚਾ;
	ਗੁਣ.x = ਬੱਚਾ->x;
	ਗੁਣ.y = ਬੱਚਾ->y;
	ਗੁਣ.ਚੌੜਾਈ = requisition.ਚੌੜਾਈ;
	ਗੁਣ.ਉਚਾਈ = requisition.ਉਚਾਈ;
	ਗੁਣ.wclass = GDK_ਨਿਵੇਸ਼_OUTPUT;
	ਗੁਣ.visual = gtk_widget_get_visual(widget);
#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	ਗੁਣ.colormap = gtk_widget_get_colormap(widget);
#ਜੇ_ਅੰਤ
	ਗੁਣ.event_ਨਕਾਬ = GDK_EXPOSURE_ਨਕਾਬ;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	ਗੁਣ_ਨਕਾਬ = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#ਨਹੀਂਤਾਂ
	ਗੁਣ_ਨਕਾਬ = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#ਜੇ_ਅੰਤ
	ਬੱਚਾ->ਖਿੜਕੀ_2 = gdk_ਖਿੜਕੀ_new(form->bin_ਖਿੜਕੀ,
				       &ਗੁਣ, ਗੁਣ_ਨਕਾਬ);
	gdk_ਖਿੜਕੀ_ਸੈੱਟ_user_ਅੰਕੜੇ(ਬੱਚਾ->ਖਿੜਕੀ_2, widget);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	{
	    GtkStyleContext * ਅਚਲ sctx = gtk_widget_get_style_context(widget);

	    gtk_style_context_ਸੈੱਟ_state(sctx, GTK_STATE_FLAG_ਸਾਧਾਰਨ);
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,18,0)
	    gtk_style_context_ਸੈੱਟ_background(sctx, ਬੱਚਾ->ਖਿੜਕੀ_2);
# endif
	}
#ਨਹੀਂਤਾਂ
	gtk_style_ਸੈੱਟ_background(widget->style,
				 ਬੱਚਾ->ਖਿੜਕੀ_2,
				 GTK_STATE_ਸਾਧਾਰਨ);
#ਜੇ_ਅੰਤ

	gtk_widget_ਸੈੱਟ_ਮਾਪਾ_ਖਿੜਕੀ(ਬੱਚਾ->widget, ਬੱਚਾ->ਖਿੜਕੀ_2);
	/*
	 * Install signal handlers to map/unmap child->window
	 * alongside with the actual widget.
	 */
	g_signal_connect(G_OBJECT(ਬੱਚਾ->widget), "map",
			 G_CALLBACK(&form_ਬੱਚਾ_map), ਬੱਚਾ);
	g_signal_connect(G_OBJECT(ਬੱਚਾ->widget), "unmap",
			 G_CALLBACK(&form_ਬੱਚਾ_unmap), ਬੱਚਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (!gtk_widget_get_realized(ਬੱਚਾ->widget))
    {
	gtk_widget_ਸੈੱਟ_ਮਾਪਾ_ਖਿੜਕੀ(ਬੱਚਾ->widget, form->bin_ਖਿੜਕੀ);
    }
}

    ਸਥਿਰ ਖਾਲੀ
form_realize_ਬੱਚਾ(GtkForm *form, Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ)
{
    form_attach_ਬੱਚਾ_ਖਿੜਕੀ(form, ਬੱਚਾ);
    gtk_widget_realize(ਬੱਚਾ->widget);
}

    ਸਥਿਰ ਖਾਲੀ
form_position_ਬੱਚਾ(
    GtkForm		*form,
    Gtk_Form_ਬੱਚਾ_2	*ਬੱਚਾ,
    gboolean		force_allocate)
{
    gint x;
    gint y;

    x = ਬੱਚਾ->x;
    y = ਬੱਚਾ->y;

    ਜੇ ((x >= G_MINSHORT) && (x <= G_MAXSHORT) &&
	(y >= G_MINSHORT) && (y <= G_MAXSHORT))
    {
	ਜੇ (!ਬੱਚਾ->mapped)
	{
	    ਜੇ (gtk_widget_get_mapped(GTK_WIDGET(form))
		    && gtk_widget_get_visible(ਬੱਚਾ->widget))
	    {
		ਜੇ (!gtk_widget_get_mapped(ਬੱਚਾ->widget))
		    gtk_widget_map(ਬੱਚਾ->widget);

		ਬੱਚਾ->mapped = TRUE;
		force_allocate = TRUE;
	    }
	}

	ਜੇ (force_allocate)
	{
	    Gtk_ਵੰਡ ਵੰਡ;
	    GtkRequisition requisition;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	    gtk_widget_get_preferred_ਮਾਪ(ਬੱਚਾ->widget, &requisition, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
	    requisition = ਬੱਚਾ->widget->requisition;
#ਜੇ_ਅੰਤ

	    ਜੇ (!gtk_widget_get_has_ਖਿੜਕੀ(ਬੱਚਾ->widget))
	    {
		ਜੇ (ਬੱਚਾ->ਖਿੜਕੀ_2)
		{
		    gdk_ਖਿੜਕੀ_move_resize(ਬੱਚਾ->ਖਿੜਕੀ_2,
			    x, y,
			    requisition.ਚੌੜਾਈ,
			    requisition.ਉਚਾਈ);
		}

		ਵੰਡ.x = 0;
		ਵੰਡ.y = 0;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਵੰਡ.x = x;
		ਵੰਡ.y = y;
	    }

	    ਵੰਡ.ਚੌੜਾਈ = requisition.ਚੌੜਾਈ;
	    ਵੰਡ.ਉਚਾਈ = requisition.ਉਚਾਈ;

	    gtk_widget_ਮਾਪ_allocate(ਬੱਚਾ->widget, &ਵੰਡ);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਬੱਚਾ->mapped)
	{
	    ਬੱਚਾ->mapped = FALSE;

	    ਜੇ (gtk_widget_get_mapped(ਬੱਚਾ->widget))
		gtk_widget_unmap(ਬੱਚਾ->widget);
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
form_position_children(GtkForm *form)
{
    GList *tmp_list;

    ਲਈ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ਅਗਲਾ)
	form_position_ਬੱਚਾ(form, tmp_list->ਅੰਕੜੇ, FALSE);
}

    ਖਾਲੀ
gui_gtk_form_move_resize(GtkForm *form, GtkWidget *widget,
		     gint x, gint y, gint w, gint h)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_widget_ਸੈੱਟ_ਮਾਪ_request(widget, w, h);
#ਨਹੀਂਤਾਂ
    widget->requisition.ਚੌੜਾਈ  = w;
    widget->requisition.ਉਚਾਈ = h;
#ਜੇ_ਅੰਤ

    gui_gtk_form_move(form, widget, x, y);
}

    ਸਥਿਰ ਖਾਲੀ
form_send_configure(GtkForm *form)
{
    GtkWidget *widget;
    GdkEventConfigure event;
    Gtk_ਵੰਡ ਵੰਡ;

    widget = GTK_WIDGET(form);

    gtk_widget_get_ਵੰਡ(widget, &ਵੰਡ);
    event.type = GDK_CONFIGURE;
    event.ਖਿੜਕੀ_2 = gtk_widget_get_ਖਿੜਕੀ(widget);
    event.x = ਵੰਡ.x;
    event.y = ਵੰਡ.y;
    event.ਚੌੜਾਈ = ਵੰਡ.ਚੌੜਾਈ;
    event.ਉਚਾਈ = ਵੰਡ.ਉਚਾਈ;

    gtk_main_do_event((GdkEvent*)&event);
}

    ਸਥਿਰ ਖਾਲੀ
form_ਬੱਚਾ_map(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer user_ਅੰਕੜੇ)
{
    Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ;

    ਬੱਚਾ = (Gtk_Form_ਬੱਚਾ_2 *)user_ਅੰਕੜੇ;

    ਬੱਚਾ->mapped = TRUE;
    gdk_ਖਿੜਕੀ_show(ਬੱਚਾ->ਖਿੜਕੀ_2);
}

    ਸਥਿਰ ਖਾਲੀ
form_ਬੱਚਾ_unmap(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer user_ਅੰਕੜੇ)
{
    Gtk_Form_ਬੱਚਾ_2 *ਬੱਚਾ;

    ਬੱਚਾ = (Gtk_Form_ਬੱਚਾ_2 *)user_ਅੰਕੜੇ;

    ਬੱਚਾ->mapped = FALSE;
    gdk_ਖਿੜਕੀ_hide(ਬੱਚਾ->ਖਿੜਕੀ_2);
}
