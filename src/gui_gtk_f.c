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

#ضمن "vim.h"
#ضمن <gtk/gtk.h>	// without this it compiles, but gives errors at
			// runtime!
#ضمن "gui_gtk_f.h"
#إذا !GTK_CHECK_نسخو(3,0,0)
# include <gtk/gtksignal.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MSWIN
# include <gdk/gdkwin32.h>
#وإلا
# include <gdk/gdkx.h>
#جيڪڏهن_پورو

تعريفنوع بنية Gtk_Form_ٻار Gtk_Form_ٻار_2;

بنية Gtk_Form_ٻار
{
    GtkWidget *widget;
    Gdk_دري *دري_2;
    gint x;		// relative subwidget x position
    gint y;		// relative subwidget y position
    gint mapped;
};


ساكن فراغ gui_gtk_form_class_init(GtkFormClass *klass);
#إذا GTK_CHECK_نسخو(3,0,0)
ساكن فراغ gui_gtk_form_init(GtkForm *form);
#وإلا
ساكن فراغ gui_gtk_form_init(GtkForm *form, فراغ *g_class);
#جيڪڏهن_پورو

ساكن فراغ form_realize(GtkWidget *widget);
ساكن فراغ form_unrealize(GtkWidget *widget);
ساكن فراغ form_map(GtkWidget *widget);
ساكن فراغ form_سائيز_request(GtkWidget *widget, GtkRequisition *requisition);
#إذا GTK_CHECK_نسخو(3,0,0)
ساكن فراغ form_get_preferred_ويڪر(GtkWidget *widget, gint *گهٽتر_ويڪر, gint *natural_ويڪر);
ساكن فراغ form_get_preferred_اوچائي(GtkWidget *widget, gint *گهٽتر_اوچائي, gint *natural_اوچائي);
#جيڪڏهن_پورو
ساكن فراغ form_سائيز_allocate(GtkWidget *widget, Gtk_ورهاست *ورهاست);
#إذا GTK_CHECK_نسخو(3,0,0)
ساكن gboolean form_draw(GtkWidget *widget, cairo_t *cr);
#وإلا
ساكن gint form_expose(GtkWidget *widget, GdkEventExpose *event);
#جيڪڏهن_پورو

ساكن فراغ form_remove(GtkContainer *container, GtkWidget *widget);
ساكن فراغ form_forall(GtkContainer *container, gboolean include_internals, GtkCallback callback, gpointer callback_معلومات);

ساكن فراغ form_attach_ٻار_دري(GtkForm *form, Gtk_Form_ٻار_2 *ٻار);
ساكن فراغ form_realize_ٻار(GtkForm *form, Gtk_Form_ٻار_2 *ٻار);
ساكن فراغ form_position_ٻار(GtkForm *form, Gtk_Form_ٻار_2 *ٻار, gboolean force_allocate);
ساكن فراغ form_position_children(GtkForm *form);

ساكن فراغ form_send_configure(GtkForm *form);

ساكن فراغ form_ٻار_map(GtkWidget *widget, gpointer user_معلومات);
ساكن فراغ form_ٻار_unmap(GtkWidget *widget, gpointer user_معلومات);

#إذا !GTK_CHECK_نسخو(3,0,0)
ساكن GtkWidgetClass *والد_class = عدم;
#جيڪڏهن_پورو

// Public interface

    GtkWidget *
gui_gtk_form_new(فراغ)
{
    GtkForm *form;

#إذا GTK_CHECK_نسخو(3,0,0)
    form = g_object_new(GTK_TYPE_FORM, عدم);
#وإلا
    form = gtk_type_new(gui_gtk_form_get_type());
#جيڪڏهن_پورو

    أرجع GTK_WIDGET(form);
}

    فراغ
gui_gtk_form_put(
	GtkForm	*form,
	GtkWidget	*ٻار_widget,
	gint	x,
	gint	y)
{
    Gtk_Form_ٻار_2 *ٻار;

    g_return_if_fail(GTK_IS_FORM(form));

    // LINTED: avoid warning: conversion to 'unsigned long'
    ٻار = g_new(Gtk_Form_ٻار_2, 1);
    إذا (ٻار == عدم)
	أرجع;

    ٻار->widget = ٻار_widget;
    ٻار->دري_2 = عدم;
    ٻار->x = x;
    ٻار->y = y;
#إذا GTK_CHECK_نسخو(3,0,0)
    gtk_widget_سيٽ_سائيز_request(ٻار->widget, -1, -1);
#وإلا
    ٻار->widget->requisition.ويڪر = 0;
    ٻار->widget->requisition.اوچائي = 0;
#جيڪڏهن_پورو
    ٻار->mapped = FALSE;

    form->children = g_list_append(form->children, ٻار);

    // child->window must be created and attached to the widget _before_
    // it has been realized, or else things will break with GTK2.  Note
    // that gtk_widget_set_parent() realizes the widget if it's visible
    // and its parent is mapped.
    إذا (gtk_widget_get_realized(GTK_WIDGET(form)))
	form_attach_ٻار_دري(form, ٻار);

    gtk_widget_سيٽ_والد(ٻار_widget, GTK_WIDGET(form));

    إذا (gtk_widget_get_realized(GTK_WIDGET(form))
	    && !gtk_widget_get_realized(ٻار_widget))
	form_realize_ٻار(form, ٻار);

    form_position_ٻار(form, ٻار, TRUE);
}

    فراغ
gui_gtk_form_move(
	GtkForm	*form,
	GtkWidget	*ٻار_widget,
	gint	x,
	gint	y)
{
    GList *tmp_list;
    Gtk_Form_ٻار_2 *ٻار;

    g_return_if_fail(GTK_IS_FORM(form));

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ايندڙ)
    {
	ٻار = tmp_list->معلومات;
	إذا (ٻار->widget == ٻار_widget)
	{
	    ٻار->x = x;
	    ٻار->y = y;

	    form_position_ٻار(form, ٻار, TRUE);
	    أرجع;
	}
    }
}

    فراغ
gui_gtk_form_freeze(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ++form->freeze_ڳڻپ;
}

    فراغ
gui_gtk_form_thaw(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    إذا (!form->freeze_ڳڻپ)
	أرجع;

    إذا (!(--form->freeze_ڳڻپ))
    {
	form_position_children(form);
	gtk_widget_قطار_draw(GTK_WIDGET(form));
    }
}

// Basic Object handling procedures

#إذا GTK_CHECK_نسخو(3,0,0)
G_DEFINE_TYPE(GtkForm, gui_gtk_form, GTK_TYPE_CONTAINER)
#وإلا
    GtkType
gui_gtk_form_get_type(فراغ)
{
    ساكن GtkType form_type = 0;

    إذا (!form_type)
    {
	GtkTypeInfo form_info;

	CLEAR_ميدان(form_info);
	form_info.type_name = "GtkForm";
	form_info.object_سائيز = حجم(GtkForm);
	form_info.class_سائيز = حجم(GtkFormClass);
	form_info.class_init_func = (GtkClassInitFunc)gui_gtk_form_class_init;
	form_info.object_init_func = (GtkObjectInitFunc)gui_gtk_form_init;

	form_type = gtk_type_unique(GTK_TYPE_CONTAINER, &form_info);
    }
    أرجع form_type;
}
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)

    ساكن فراغ
gui_gtk_form_class_init(GtkFormClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    widget_class = (GtkWidgetClass *) klass;
    container_class = (GtkContainerClass *) klass;

#إذا !GTK_CHECK_نسخو(3,0,0)
    والد_class = gtk_type_class(gtk_container_get_type());
#جيڪڏهن_پورو

    widget_class->realize = form_realize;
    widget_class->unrealize = form_unrealize;
    widget_class->map = form_map;
#إذا GTK_CHECK_نسخو(3,0,0)
    widget_class->get_preferred_ويڪر = form_get_preferred_ويڪر;
    widget_class->get_preferred_اوچائي = form_get_preferred_اوچائي;
#وإلا
    widget_class->سائيز_request = form_سائيز_request;
#جيڪڏهن_پورو
    widget_class->سائيز_allocate = form_سائيز_allocate;
#إذا GTK_CHECK_نسخو(3,0,0)
    widget_class->draw = form_draw;
#وإلا
    widget_class->expose_event = form_expose;
#جيڪڏهن_پورو

    container_class->remove = form_remove;
    container_class->forall = form_forall;
}

    ساكن فراغ
gui_gtk_form_init(GtkForm *form
#إذا !GTK_CHECK_نسخو(3,0,0)
	, فراغ *g_class اڻ_استعمال_ٿيل
#جيڪڏهن_پورو
	)
{
#إذا GTK_CHECK_نسخو(3,0,0)
    gtk_widget_سيٽ_has_دري(GTK_WIDGET(form), TRUE);
#جيڪڏهن_پورو
    form->children = عدم;
    form->bin_دري = عدم;
    form->freeze_ڳڻپ = 0;
}

/*
 * Widget methods
 */

    ساكن فراغ
form_realize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gdk_دري_Attr خاصيتون;
    gint خاصيتون_نقاب;
    Gtk_ورهاست ورهاست;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);
    gtk_widget_سيٽ_realized(widget, TRUE);

    gtk_widget_get_ورهاست(widget, &ورهاست);
    خاصيتون.دري_type = GDK_دري_ٻار;
    خاصيتون.x = ورهاست.x;
    خاصيتون.y = ورهاست.y;
    خاصيتون.ويڪر = ورهاست.ويڪر;
    خاصيتون.اوچائي = ورهاست.اوچائي;
    خاصيتون.wclass = GDK_داخلا_OUTPUT;
    خاصيتون.visual = gtk_widget_get_visual(widget);
#إذا GTK_CHECK_نسخو(3,0,0)
    خاصيتون.event_نقاب = GDK_EXPOSURE_نقاب;
#وإلا
    خاصيتون.colormap = gtk_widget_get_colormap(widget);
    خاصيتون.event_نقاب = GDK_VISIBILITY_NOTIFY_نقاب;
#جيڪڏهن_پورو

#إذا GTK_CHECK_نسخو(3,0,0)
    خاصيتون_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#وإلا
    خاصيتون_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#جيڪڏهن_پورو

    gtk_widget_سيٽ_دري(widget,
			  gdk_دري_new(gtk_widget_get_والد_دري(widget),
					 &خاصيتون, خاصيتون_نقاب));
    gdk_دري_سيٽ_user_معلومات(gtk_widget_get_دري(widget), widget);

    خاصيتون.x = 0;
    خاصيتون.y = 0;
    خاصيتون.event_نقاب = gtk_widget_get_events(widget);

    form->bin_دري = gdk_دري_new(gtk_widget_get_دري(widget),
				      &خاصيتون, خاصيتون_نقاب);
    gdk_دري_سيٽ_user_معلومات(form->bin_دري, widget);

#إذا GTK_CHECK_نسخو(3,0,0)
    {
	GtkStyleContext * ثابت sctx = gtk_widget_get_style_context(widget);

	gtk_style_context_جوڙيو_class(sctx, "gtk-form");
	gtk_style_context_سيٽ_state(sctx, GTK_STATE_FLAG_عام);
# إذا !GTK_CHECK_نسخو(3,18,0)
	gtk_style_context_سيٽ_background(sctx, gtk_widget_get_دري(widget));
	gtk_style_context_سيٽ_background(sctx, form->bin_دري);
# endif
    }
#وإلا
    widget->style = gtk_style_attach(widget->style, widget->دري_2);
    gtk_style_سيٽ_background(widget->style, widget->دري_2, GTK_STATE_عام);
    gtk_style_سيٽ_background(widget->style, form->bin_دري, GTK_STATE_عام);
#جيڪڏهن_پورو

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ايندڙ)
    {
	Gtk_Form_ٻار_2 *ٻار = tmp_list->معلومات;

	form_attach_ٻار_دري(form, ٻار);

	إذا (gtk_widget_get_visible(ٻار->widget))
	    form_realize_ٻار(form, ٻار);
    }
}


// After reading the documentation at
// https://www.manpagez.com/html/gtk2/gtk2-2.24.29/gtk-changes-2-0.php
// I think it should be possible to remove this function when compiling
// against gtk-2.0.  It doesn't seem to cause problems, though.
//
// Well, I reckon at least the gdk_window_show(form->bin_window)
// is necessary.  GtkForm is anything but a usual container widget.
    ساكن فراغ
form_map(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    gtk_widget_سيٽ_mapped(widget, TRUE);

    gdk_دري_show(gtk_widget_get_دري(widget));
    gdk_دري_show(form->bin_دري);

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ايندڙ)
    {
	Gtk_Form_ٻار_2 *ٻار = tmp_list->معلومات;

	إذا (gtk_widget_get_visible(ٻار->widget)
		&& !gtk_widget_get_mapped(ٻار->widget))
	    gtk_widget_map(ٻار->widget);
    }
}

    ساكن فراغ
form_unrealize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    tmp_list = form->children;

    gdk_دري_سيٽ_user_معلومات(form->bin_دري, عدم);
    gdk_دري_destroy(form->bin_دري);
    form->bin_دري = عدم;

    طالما (tmp_list)
    {
	Gtk_Form_ٻار_2 *ٻار = tmp_list->معلومات;

	إذا (ٻار->دري_2 != عدم)
	{
	    g_signal_handlers_disconnect_by_func(G_OBJECT(ٻار->widget),
		    FUNC2GENERIC(form_ٻار_map),
		    ٻار);
	    g_signal_handlers_disconnect_by_func(G_OBJECT(ٻار->widget),
		    FUNC2GENERIC(form_ٻار_unmap),
		    ٻار);

	    gdk_دري_سيٽ_user_معلومات(ٻار->دري_2, عدم);
	    gdk_دري_destroy(ٻار->دري_2);

	    ٻار->دري_2 = عدم;
	}

	tmp_list = tmp_list->ايندڙ;
    }

#إذا GTK_CHECK_نسخو(3,0,0)
    إذا (GTK_WIDGET_CLASS (gui_gtk_form_والد_class)->unrealize)
	 (* GTK_WIDGET_CLASS (gui_gtk_form_والد_class)->unrealize) (widget);
#وإلا
    إذا (GTK_WIDGET_CLASS (والد_class)->unrealize)
	 (* GTK_WIDGET_CLASS (والد_class)->unrealize) (widget);
#جيڪڏهن_پورو
}

    ساكن فراغ
form_سائيز_request(GtkWidget *widget, GtkRequisition *requisition)
{
    g_return_if_fail(GTK_IS_FORM(widget));
    g_return_if_fail(requisition != عدم);

    requisition->ويڪر = 1;
    requisition->اوچائي = 1;
}

#إذا GTK_CHECK_نسخو(3,0,0)
    ساكن فراغ
form_get_preferred_ويڪر(GtkWidget *widget,
			     gint      *گهٽتر_ويڪر,
			     gint      *natural_ويڪر)
{
    GtkRequisition requisition;

    form_سائيز_request(widget, &requisition);

    *گهٽتر_ويڪر = requisition.ويڪر;
    *natural_ويڪر = requisition.ويڪر;
}

    ساكن فراغ
form_get_preferred_اوچائي(GtkWidget *widget,
			      gint	*گهٽتر_اوچائي,
			      gint	*natural_اوچائي)
{
    GtkRequisition requisition;

    form_سائيز_request(widget, &requisition);

    *گهٽتر_اوچائي = requisition.اوچائي;
    *natural_اوچائي = requisition.اوچائي;
}
#جيڪڏهن_پورو // GTK_CHECK_VERSION(3,0,0)

    ساكن فراغ
form_سائيز_allocate(GtkWidget *widget, Gtk_ورهاست *ورهاست)
{
    GList *tmp_list;
    GtkForm *form;
    gboolean need_reposition;
    Gtk_ورهاست cur_alloc;

    g_return_if_fail(GTK_IS_FORM(widget));

    gtk_widget_get_ورهاست(widget, &cur_alloc);

    إذا (cur_alloc.x == ورهاست->x
	    && cur_alloc.y == ورهاست->y
	    && cur_alloc.ويڪر == ورهاست->ويڪر
	    && cur_alloc.اوچائي == ورهاست->اوچائي)
	أرجع;

    need_reposition = cur_alloc.ويڪر != ورهاست->ويڪر
		   || cur_alloc.اوچائي != ورهاست->اوچائي;
    form = GTK_FORM(widget);

    إذا (need_reposition)
    {
	tmp_list = form->children;

	طالما (tmp_list)
	{
	    Gtk_Form_ٻار_2 *ٻار = tmp_list->معلومات;
	    form_position_ٻار(form, ٻار, TRUE);

	    tmp_list = tmp_list->ايندڙ;
	}
    }

    إذا (gtk_widget_get_realized(widget))
    {
	gdk_دري_move_resize(gtk_widget_get_دري(widget),
			       ورهاست->x, ورهاست->y,
			       ورهاست->ويڪر, ورهاست->اوچائي);
	gdk_دري_move_resize(GTK_FORM(widget)->bin_دري,
			       0, 0,
			       ورهاست->ويڪر, ورهاست->اوچائي);
    }
    gtk_widget_سيٽ_ورهاست(widget, ورهاست);
    إذا (need_reposition)
	form_send_configure(form);
}

#إذا GTK_CHECK_نسخو(3,0,0)
    ساكن فراغ
gtk_form_render_background(GtkWidget *widget, cairo_t *cr)
{
    gtk_render_background(gtk_widget_get_style_context(widget), cr,
			  0, 0,
			  gtk_widget_get_allocated_ويڪر(widget),
			  gtk_widget_get_allocated_اوچائي(widget));
}

    ساكن gboolean
form_draw(GtkWidget *widget, cairo_t *cr)
{
    GList   *tmp_list = عدم;
    GtkForm *form     = عدم;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    gtk_form_render_background(widget, cr);

    form = GTK_FORM(widget);
    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ايندڙ)
    {
	Gtk_Form_ٻار_2 * ثابت formchild = tmp_list->معلومات;

	إذا (!gtk_widget_get_has_دري(formchild->widget) &&
		gtk_cairo_should_draw_دري(cr, formchild->دري_2))
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
	    form_position_ٻار(form, formchild, TRUE);

	    gtk_form_render_background(formchild->widget, cr);
	}
    }

    أرجع GTK_WIDGET_CLASS(gui_gtk_form_والد_class)->draw(widget, cr);
}
#وإلا // !GTK_CHECK_VERSION(3,0,0)
    ساكن gint
form_expose(GtkWidget *widget, GdkEventExpose *event)
{
    GList   *tmp_list;
    GtkForm *form;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    form = GTK_FORM(widget);

    إذا (event->دري_2 == form->bin_دري)
	أرجع FALSE;

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ايندڙ)
	gtk_container_propagate_expose(GTK_CONTAINER(widget),
		GTK_WIDGET(((Gtk_Form_ٻار_2 *)tmp_list->معلومات)->widget),
		event);

    أرجع FALSE;
}
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)

// Container method
    ساكن فراغ
form_remove(GtkContainer *container, GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gtk_Form_ٻار_2 *ٻار = عدم;	    // init for gcc

    g_return_if_fail(GTK_IS_FORM(container));

    form = GTK_FORM(container);

    tmp_list = form->children;
    طالما (tmp_list)
    {
	ٻار = tmp_list->معلومات;
	إذا (ٻار->widget == widget)
	    اكسر;
	tmp_list = tmp_list->ايندڙ;
    }

    إذا (tmp_list == عدم)
	أرجع;

#إذا GTK_CHECK_نسخو(3,0,0)
    ثابت gboolean was_visible = gtk_widget_get_visible(widget);
#جيڪڏهن_پورو
    إذا (ٻار->دري_2)
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(ٻار->widget),
		FUNC2GENERIC(&form_ٻار_map), ٻار);
	g_signal_handlers_disconnect_by_func(G_OBJECT(ٻار->widget),
		FUNC2GENERIC(&form_ٻار_unmap), ٻار);

	// FIXME: This will cause problems for reparenting NO_WINDOW
	// widgets out of a GtkForm
	gdk_دري_سيٽ_user_معلومات(ٻار->دري_2, عدم);
	gdk_دري_destroy(ٻار->دري_2);
    }
    gtk_widget_unparent(widget);
#إذا GTK_CHECK_نسخو(3,0,0)
    إذا (was_visible)
	gtk_widget_قطار_resize(GTK_WIDGET(container));
#جيڪڏهن_پورو
    form->children = g_list_remove_link(form->children, tmp_list);
    g_list_free_1(tmp_list);
    g_free(ٻار);
}

    ساكن فراغ
form_forall(GtkContainer	*container,
		gboolean	include_internals اڻ_استعمال_ٿيل,
		GtkCallback	callback,
		gpointer	callback_معلومات)
{
    GtkForm *form;
    Gtk_Form_ٻار_2 *ٻار;
    GList *tmp_list;

    g_return_if_fail(GTK_IS_FORM(container));
    g_return_if_fail(callback != عدم);

    form = GTK_FORM(container);

    tmp_list = form->children;
    طالما (tmp_list)
    {
	ٻار = tmp_list->معلومات;
	tmp_list = tmp_list->ايندڙ;

	(*callback) (ٻار->widget, callback_معلومات);
    }
}

// Operations on children

    ساكن فراغ
form_attach_ٻار_دري(GtkForm *form, Gtk_Form_ٻار_2 *ٻار)
{
    إذا (ٻار->دري_2 != عدم)
	أرجع; // been there, done that

    إذا (!gtk_widget_get_has_دري(ٻار->widget))
    {
	GtkWidget	*widget;
	Gdk_دري_Attr	خاصيتون;
	gint		خاصيتون_نقاب;
	GtkRequisition	requisition;

	widget = GTK_WIDGET(form);

#إذا GTK_CHECK_نسخو(3,0,0)
	gtk_widget_get_preferred_سائيز(ٻار->widget, &requisition, عدم);
#وإلا
	requisition = ٻار->widget->requisition;
#جيڪڏهن_پورو
	خاصيتون.دري_type = GDK_دري_ٻار;
	خاصيتون.x = ٻار->x;
	خاصيتون.y = ٻار->y;
	خاصيتون.ويڪر = requisition.ويڪر;
	خاصيتون.اوچائي = requisition.اوچائي;
	خاصيتون.wclass = GDK_داخلا_OUTPUT;
	خاصيتون.visual = gtk_widget_get_visual(widget);
#إذا !GTK_CHECK_نسخو(3,0,0)
	خاصيتون.colormap = gtk_widget_get_colormap(widget);
#جيڪڏهن_پورو
	خاصيتون.event_نقاب = GDK_EXPOSURE_نقاب;

#إذا GTK_CHECK_نسخو(3,0,0)
	خاصيتون_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#وإلا
	خاصيتون_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#جيڪڏهن_پورو
	ٻار->دري_2 = gdk_دري_new(form->bin_دري,
				       &خاصيتون, خاصيتون_نقاب);
	gdk_دري_سيٽ_user_معلومات(ٻار->دري_2, widget);

#إذا GTK_CHECK_نسخو(3,0,0)
	{
	    GtkStyleContext * ثابت sctx = gtk_widget_get_style_context(widget);

	    gtk_style_context_سيٽ_state(sctx, GTK_STATE_FLAG_عام);
# إذا !GTK_CHECK_نسخو(3,18,0)
	    gtk_style_context_سيٽ_background(sctx, ٻار->دري_2);
# endif
	}
#وإلا
	gtk_style_سيٽ_background(widget->style,
				 ٻار->دري_2,
				 GTK_STATE_عام);
#جيڪڏهن_پورو

	gtk_widget_سيٽ_والد_دري(ٻار->widget, ٻار->دري_2);
	/*
	 * Install signal handlers to map/unmap child->window
	 * alongside with the actual widget.
	 */
	g_signal_connect(G_OBJECT(ٻار->widget), "map",
			 G_CALLBACK(&form_ٻار_map), ٻار);
	g_signal_connect(G_OBJECT(ٻار->widget), "unmap",
			 G_CALLBACK(&form_ٻار_unmap), ٻار);
    }
    وإلا إذا (!gtk_widget_get_realized(ٻار->widget))
    {
	gtk_widget_سيٽ_والد_دري(ٻار->widget, form->bin_دري);
    }
}

    ساكن فراغ
form_realize_ٻار(GtkForm *form, Gtk_Form_ٻار_2 *ٻار)
{
    form_attach_ٻار_دري(form, ٻار);
    gtk_widget_realize(ٻار->widget);
}

    ساكن فراغ
form_position_ٻار(
    GtkForm		*form,
    Gtk_Form_ٻار_2	*ٻار,
    gboolean		force_allocate)
{
    gint x;
    gint y;

    x = ٻار->x;
    y = ٻار->y;

    إذا ((x >= G_MINSHORT) && (x <= G_MAXSHORT) &&
	(y >= G_MINSHORT) && (y <= G_MAXSHORT))
    {
	إذا (!ٻار->mapped)
	{
	    إذا (gtk_widget_get_mapped(GTK_WIDGET(form))
		    && gtk_widget_get_visible(ٻار->widget))
	    {
		إذا (!gtk_widget_get_mapped(ٻار->widget))
		    gtk_widget_map(ٻار->widget);

		ٻار->mapped = TRUE;
		force_allocate = TRUE;
	    }
	}

	إذا (force_allocate)
	{
	    Gtk_ورهاست ورهاست;
	    GtkRequisition requisition;

#إذا GTK_CHECK_نسخو(3,0,0)
	    gtk_widget_get_preferred_سائيز(ٻار->widget, &requisition, عدم);
#وإلا
	    requisition = ٻار->widget->requisition;
#جيڪڏهن_پورو

	    إذا (!gtk_widget_get_has_دري(ٻار->widget))
	    {
		إذا (ٻار->دري_2)
		{
		    gdk_دري_move_resize(ٻار->دري_2,
			    x, y,
			    requisition.ويڪر,
			    requisition.اوچائي);
		}

		ورهاست.x = 0;
		ورهاست.y = 0;
	    }
	    وإلا
	    {
		ورهاست.x = x;
		ورهاست.y = y;
	    }

	    ورهاست.ويڪر = requisition.ويڪر;
	    ورهاست.اوچائي = requisition.اوچائي;

	    gtk_widget_سائيز_allocate(ٻار->widget, &ورهاست);
	}
    }
    وإلا
    {
	إذا (ٻار->mapped)
	{
	    ٻار->mapped = FALSE;

	    إذا (gtk_widget_get_mapped(ٻار->widget))
		gtk_widget_unmap(ٻار->widget);
	}
    }
}

    ساكن فراغ
form_position_children(GtkForm *form)
{
    GList *tmp_list;

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ايندڙ)
	form_position_ٻار(form, tmp_list->معلومات, FALSE);
}

    فراغ
gui_gtk_form_move_resize(GtkForm *form, GtkWidget *widget,
		     gint x, gint y, gint w, gint h)
{
#إذا GTK_CHECK_نسخو(3,0,0)
    gtk_widget_سيٽ_سائيز_request(widget, w, h);
#وإلا
    widget->requisition.ويڪر  = w;
    widget->requisition.اوچائي = h;
#جيڪڏهن_پورو

    gui_gtk_form_move(form, widget, x, y);
}

    ساكن فراغ
form_send_configure(GtkForm *form)
{
    GtkWidget *widget;
    GdkEventConfigure event;
    Gtk_ورهاست ورهاست;

    widget = GTK_WIDGET(form);

    gtk_widget_get_ورهاست(widget, &ورهاست);
    event.type = GDK_CONFIGURE;
    event.دري_2 = gtk_widget_get_دري(widget);
    event.x = ورهاست.x;
    event.y = ورهاست.y;
    event.ويڪر = ورهاست.ويڪر;
    event.اوچائي = ورهاست.اوچائي;

    gtk_main_do_event((GdkEvent*)&event);
}

    ساكن فراغ
form_ٻار_map(GtkWidget *widget اڻ_استعمال_ٿيل, gpointer user_معلومات)
{
    Gtk_Form_ٻار_2 *ٻار;

    ٻار = (Gtk_Form_ٻار_2 *)user_معلومات;

    ٻار->mapped = TRUE;
    gdk_دري_show(ٻار->دري_2);
}

    ساكن فراغ
form_ٻار_unmap(GtkWidget *widget اڻ_استعمال_ٿيل, gpointer user_معلومات)
{
    Gtk_Form_ٻار_2 *ٻار;

    ٻار = (Gtk_Form_ٻار_2 *)user_معلومات;

    ٻار->mapped = FALSE;
    gdk_دري_hide(ٻار->دري_2);
}
