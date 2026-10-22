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
#إذا !GTK_CHECK_نسخہ(3,0,0)
# include <gtk/gtksignal.h>
#جے_انت
#جے_تعریف_شدہ MSWIN
# include <gdk/gdkwin32.h>
#وإلا
# include <gdk/gdkx.h>
#جے_انت

تعريفنوع بنية Gtk_Form_بچہ Gtk_Form_بچہ_2;

بنية Gtk_Form_بچہ
{
    GtkWidget *widget;
    Gdk_کھڑکی *کھڑکی_2;
    gint x;		// relative subwidget x position
    gint y;		// relative subwidget y position
    gint mapped;
};


ساكن فراغ gui_gtk_form_class_init(GtkFormClass *klass);
#إذا GTK_CHECK_نسخہ(3,0,0)
ساكن فراغ gui_gtk_form_init(GtkForm *form);
#وإلا
ساكن فراغ gui_gtk_form_init(GtkForm *form, فراغ *g_class);
#جے_انت

ساكن فراغ form_realize(GtkWidget *widget);
ساكن فراغ form_unrealize(GtkWidget *widget);
ساكن فراغ form_map(GtkWidget *widget);
ساكن فراغ form_ناپ_request(GtkWidget *widget, GtkRequisition *requisition);
#إذا GTK_CHECK_نسخہ(3,0,0)
ساكن فراغ form_get_preferred_چوڑائی(GtkWidget *widget, gint *اقل_چوڑائی, gint *natural_چوڑائی);
ساكن فراغ form_get_preferred_اچائی(GtkWidget *widget, gint *اقل_اچائی, gint *natural_اچائی);
#جے_انت
ساكن فراغ form_ناپ_allocate(GtkWidget *widget, Gtk_ونڈ *ونڈ);
#إذا GTK_CHECK_نسخہ(3,0,0)
ساكن gboolean form_draw(GtkWidget *widget, cairo_t *cr);
#وإلا
ساكن gint form_expose(GtkWidget *widget, GdkEventExpose *event);
#جے_انت

ساكن فراغ form_remove(GtkContainer *container, GtkWidget *widget);
ساكن فراغ form_forall(GtkContainer *container, gboolean include_internals, GtkCallback callback, gpointer callback_انکڑے);

ساكن فراغ form_attach_بچہ_کھڑکی(GtkForm *form, Gtk_Form_بچہ_2 *بچہ);
ساكن فراغ form_realize_بچہ(GtkForm *form, Gtk_Form_بچہ_2 *بچہ);
ساكن فراغ form_position_بچہ(GtkForm *form, Gtk_Form_بچہ_2 *بچہ, gboolean force_allocate);
ساكن فراغ form_position_children(GtkForm *form);

ساكن فراغ form_send_configure(GtkForm *form);

ساكن فراغ form_بچہ_map(GtkWidget *widget, gpointer user_انکڑے);
ساكن فراغ form_بچہ_unmap(GtkWidget *widget, gpointer user_انکڑے);

#إذا !GTK_CHECK_نسخہ(3,0,0)
ساكن GtkWidgetClass *ماپا_class = عدم;
#جے_انت

// Public interface

    GtkWidget *
gui_gtk_form_new(فراغ)
{
    GtkForm *form;

#إذا GTK_CHECK_نسخہ(3,0,0)
    form = g_object_new(GTK_TYPE_FORM, عدم);
#وإلا
    form = gtk_type_new(gui_gtk_form_get_type());
#جے_انت

    أرجع GTK_WIDGET(form);
}

    فراغ
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
    إذا (بچہ == عدم)
	أرجع;

    بچہ->widget = بچہ_widget;
    بچہ->کھڑکی_2 = عدم;
    بچہ->x = x;
    بچہ->y = y;
#إذا GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_سیٹ_ناپ_request(بچہ->widget, -1, -1);
#وإلا
    بچہ->widget->requisition.چوڑائی = 0;
    بچہ->widget->requisition.اچائی = 0;
#جے_انت
    بچہ->mapped = FALSE;

    form->children = g_list_append(form->children, بچہ);

    // child->window must be created and attached to the widget _before_
    // it has been realized, or else things will break with GTK2.  Note
    // that gtk_widget_set_parent() realizes the widget if it's visible
    // and its parent is mapped.
    إذا (gtk_widget_get_realized(GTK_WIDGET(form)))
	form_attach_بچہ_کھڑکی(form, بچہ);

    gtk_widget_سیٹ_ماپا(بچہ_widget, GTK_WIDGET(form));

    إذا (gtk_widget_get_realized(GTK_WIDGET(form))
	    && !gtk_widget_get_realized(بچہ_widget))
	form_realize_بچہ(form, بچہ);

    form_position_بچہ(form, بچہ, TRUE);
}

    فراغ
gui_gtk_form_move(
	GtkForm	*form,
	GtkWidget	*بچہ_widget,
	gint	x,
	gint	y)
{
    GList *tmp_list;
    Gtk_Form_بچہ_2 *بچہ;

    g_return_if_fail(GTK_IS_FORM(form));

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
    {
	بچہ = tmp_list->انکڑے;
	إذا (بچہ->widget == بچہ_widget)
	{
	    بچہ->x = x;
	    بچہ->y = y;

	    form_position_بچہ(form, بچہ, TRUE);
	    أرجع;
	}
    }
}

    فراغ
gui_gtk_form_freeze(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ++form->freeze_گنتی;
}

    فراغ
gui_gtk_form_thaw(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    إذا (!form->freeze_گنتی)
	أرجع;

    إذا (!(--form->freeze_گنتی))
    {
	form_position_children(form);
	gtk_widget_قطار_draw(GTK_WIDGET(form));
    }
}

// Basic Object handling procedures

#إذا GTK_CHECK_نسخہ(3,0,0)
G_DEFINE_TYPE(GtkForm, gui_gtk_form, GTK_TYPE_CONTAINER)
#وإلا
    GtkType
gui_gtk_form_get_type(فراغ)
{
    ساكن GtkType form_type = 0;

    إذا (!form_type)
    {
	GtkTypeInfo form_info;

	CLEAR_میدان(form_info);
	form_info.type_name = "GtkForm";
	form_info.object_ناپ = حجم(GtkForm);
	form_info.class_ناپ = حجم(GtkFormClass);
	form_info.class_init_func = (GtkClassInitFunc)gui_gtk_form_class_init;
	form_info.object_init_func = (GtkObjectInitFunc)gui_gtk_form_init;

	form_type = gtk_type_unique(GTK_TYPE_CONTAINER, &form_info);
    }
    أرجع form_type;
}
#جے_انت // !GTK_CHECK_VERSION(3,0,0)

    ساكن فراغ
gui_gtk_form_class_init(GtkFormClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    widget_class = (GtkWidgetClass *) klass;
    container_class = (GtkContainerClass *) klass;

#إذا !GTK_CHECK_نسخہ(3,0,0)
    ماپا_class = gtk_type_class(gtk_container_get_type());
#جے_انت

    widget_class->realize = form_realize;
    widget_class->unrealize = form_unrealize;
    widget_class->map = form_map;
#إذا GTK_CHECK_نسخہ(3,0,0)
    widget_class->get_preferred_چوڑائی = form_get_preferred_چوڑائی;
    widget_class->get_preferred_اچائی = form_get_preferred_اچائی;
#وإلا
    widget_class->ناپ_request = form_ناپ_request;
#جے_انت
    widget_class->ناپ_allocate = form_ناپ_allocate;
#إذا GTK_CHECK_نسخہ(3,0,0)
    widget_class->draw = form_draw;
#وإلا
    widget_class->expose_event = form_expose;
#جے_انت

    container_class->remove = form_remove;
    container_class->forall = form_forall;
}

    ساكن فراغ
gui_gtk_form_init(GtkForm *form
#إذا !GTK_CHECK_نسخہ(3,0,0)
	, فراغ *g_class بن_ورتیا
#جے_انت
	)
{
#إذا GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_سیٹ_has_کھڑکی(GTK_WIDGET(form), TRUE);
#جے_انت
    form->children = عدم;
    form->bin_کھڑکی = عدم;
    form->freeze_گنتی = 0;
}

/*
 * Widget methods
 */

    ساكن فراغ
form_realize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gdk_کھڑکی_Attr گن;
    gint گن_نقاب;
    Gtk_ونڈ ونڈ;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);
    gtk_widget_سیٹ_realized(widget, TRUE);

    gtk_widget_get_ونڈ(widget, &ونڈ);
    گن.کھڑکی_type = GDK_کھڑکی_بچہ;
    گن.x = ونڈ.x;
    گن.y = ونڈ.y;
    گن.چوڑائی = ونڈ.چوڑائی;
    گن.اچائی = ونڈ.اچائی;
    گن.wclass = GDK_داخلہ_OUTPUT;
    گن.visual = gtk_widget_get_visual(widget);
#إذا GTK_CHECK_نسخہ(3,0,0)
    گن.event_نقاب = GDK_EXPOSURE_نقاب;
#وإلا
    گن.colormap = gtk_widget_get_colormap(widget);
    گن.event_نقاب = GDK_VISIBILITY_NOTIFY_نقاب;
#جے_انت

#إذا GTK_CHECK_نسخہ(3,0,0)
    گن_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#وإلا
    گن_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#جے_انت

    gtk_widget_سیٹ_کھڑکی(widget,
			  gdk_کھڑکی_new(gtk_widget_get_ماپا_کھڑکی(widget),
					 &گن, گن_نقاب));
    gdk_کھڑکی_سیٹ_user_انکڑے(gtk_widget_get_کھڑکی(widget), widget);

    گن.x = 0;
    گن.y = 0;
    گن.event_نقاب = gtk_widget_get_events(widget);

    form->bin_کھڑکی = gdk_کھڑکی_new(gtk_widget_get_کھڑکی(widget),
				      &گن, گن_نقاب);
    gdk_کھڑکی_سیٹ_user_انکڑے(form->bin_کھڑکی, widget);

#إذا GTK_CHECK_نسخہ(3,0,0)
    {
	GtkStyleContext * ثابت sctx = gtk_widget_get_style_context(widget);

	gtk_style_context_جوڑو_class(sctx, "gtk-form");
	gtk_style_context_سیٹ_state(sctx, GTK_STATE_FLAG_عام);
# إذا !GTK_CHECK_نسخہ(3,18,0)
	gtk_style_context_سیٹ_background(sctx, gtk_widget_get_کھڑکی(widget));
	gtk_style_context_سیٹ_background(sctx, form->bin_کھڑکی);
# endif
    }
#وإلا
    widget->style = gtk_style_attach(widget->style, widget->کھڑکی_2);
    gtk_style_سیٹ_background(widget->style, widget->کھڑکی_2, GTK_STATE_عام);
    gtk_style_سیٹ_background(widget->style, form->bin_کھڑکی, GTK_STATE_عام);
#جے_انت

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
    {
	Gtk_Form_بچہ_2 *بچہ = tmp_list->انکڑے;

	form_attach_بچہ_کھڑکی(form, بچہ);

	إذا (gtk_widget_get_visible(بچہ->widget))
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
    ساكن فراغ
form_map(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    gtk_widget_سیٹ_mapped(widget, TRUE);

    gdk_کھڑکی_show(gtk_widget_get_کھڑکی(widget));
    gdk_کھڑکی_show(form->bin_کھڑکی);

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
    {
	Gtk_Form_بچہ_2 *بچہ = tmp_list->انکڑے;

	إذا (gtk_widget_get_visible(بچہ->widget)
		&& !gtk_widget_get_mapped(بچہ->widget))
	    gtk_widget_map(بچہ->widget);
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

    gdk_کھڑکی_سیٹ_user_انکڑے(form->bin_کھڑکی, عدم);
    gdk_کھڑکی_destroy(form->bin_کھڑکی);
    form->bin_کھڑکی = عدم;

    طالما (tmp_list)
    {
	Gtk_Form_بچہ_2 *بچہ = tmp_list->انکڑے;

	إذا (بچہ->کھڑکی_2 != عدم)
	{
	    g_signal_handlers_disconnect_by_func(G_OBJECT(بچہ->widget),
		    FUNC2GENERIC(form_بچہ_map),
		    بچہ);
	    g_signal_handlers_disconnect_by_func(G_OBJECT(بچہ->widget),
		    FUNC2GENERIC(form_بچہ_unmap),
		    بچہ);

	    gdk_کھڑکی_سیٹ_user_انکڑے(بچہ->کھڑکی_2, عدم);
	    gdk_کھڑکی_destroy(بچہ->کھڑکی_2);

	    بچہ->کھڑکی_2 = عدم;
	}

	tmp_list = tmp_list->اگلا;
    }

#إذا GTK_CHECK_نسخہ(3,0,0)
    إذا (GTK_WIDGET_CLASS (gui_gtk_form_ماپا_class)->unrealize)
	 (* GTK_WIDGET_CLASS (gui_gtk_form_ماپا_class)->unrealize) (widget);
#وإلا
    إذا (GTK_WIDGET_CLASS (ماپا_class)->unrealize)
	 (* GTK_WIDGET_CLASS (ماپا_class)->unrealize) (widget);
#جے_انت
}

    ساكن فراغ
form_ناپ_request(GtkWidget *widget, GtkRequisition *requisition)
{
    g_return_if_fail(GTK_IS_FORM(widget));
    g_return_if_fail(requisition != عدم);

    requisition->چوڑائی = 1;
    requisition->اچائی = 1;
}

#إذا GTK_CHECK_نسخہ(3,0,0)
    ساكن فراغ
form_get_preferred_چوڑائی(GtkWidget *widget,
			     gint      *اقل_چوڑائی,
			     gint      *natural_چوڑائی)
{
    GtkRequisition requisition;

    form_ناپ_request(widget, &requisition);

    *اقل_چوڑائی = requisition.چوڑائی;
    *natural_چوڑائی = requisition.چوڑائی;
}

    ساكن فراغ
form_get_preferred_اچائی(GtkWidget *widget,
			      gint	*اقل_اچائی,
			      gint	*natural_اچائی)
{
    GtkRequisition requisition;

    form_ناپ_request(widget, &requisition);

    *اقل_اچائی = requisition.اچائی;
    *natural_اچائی = requisition.اچائی;
}
#جے_انت // GTK_CHECK_VERSION(3,0,0)

    ساكن فراغ
form_ناپ_allocate(GtkWidget *widget, Gtk_ونڈ *ونڈ)
{
    GList *tmp_list;
    GtkForm *form;
    gboolean need_reposition;
    Gtk_ونڈ cur_alloc;

    g_return_if_fail(GTK_IS_FORM(widget));

    gtk_widget_get_ونڈ(widget, &cur_alloc);

    إذا (cur_alloc.x == ونڈ->x
	    && cur_alloc.y == ونڈ->y
	    && cur_alloc.چوڑائی == ونڈ->چوڑائی
	    && cur_alloc.اچائی == ونڈ->اچائی)
	أرجع;

    need_reposition = cur_alloc.چوڑائی != ونڈ->چوڑائی
		   || cur_alloc.اچائی != ونڈ->اچائی;
    form = GTK_FORM(widget);

    إذا (need_reposition)
    {
	tmp_list = form->children;

	طالما (tmp_list)
	{
	    Gtk_Form_بچہ_2 *بچہ = tmp_list->انکڑے;
	    form_position_بچہ(form, بچہ, TRUE);

	    tmp_list = tmp_list->اگلا;
	}
    }

    إذا (gtk_widget_get_realized(widget))
    {
	gdk_کھڑکی_move_resize(gtk_widget_get_کھڑکی(widget),
			       ونڈ->x, ونڈ->y,
			       ونڈ->چوڑائی, ونڈ->اچائی);
	gdk_کھڑکی_move_resize(GTK_FORM(widget)->bin_کھڑکی,
			       0, 0,
			       ونڈ->چوڑائی, ونڈ->اچائی);
    }
    gtk_widget_سیٹ_ونڈ(widget, ونڈ);
    إذا (need_reposition)
	form_send_configure(form);
}

#إذا GTK_CHECK_نسخہ(3,0,0)
    ساكن فراغ
gtk_form_render_background(GtkWidget *widget, cairo_t *cr)
{
    gtk_render_background(gtk_widget_get_style_context(widget), cr,
			  0, 0,
			  gtk_widget_get_allocated_چوڑائی(widget),
			  gtk_widget_get_allocated_اچائی(widget));
}

    ساكن gboolean
form_draw(GtkWidget *widget, cairo_t *cr)
{
    GList   *tmp_list = عدم;
    GtkForm *form     = عدم;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    gtk_form_render_background(widget, cr);

    form = GTK_FORM(widget);
    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
    {
	Gtk_Form_بچہ_2 * ثابت formchild = tmp_list->انکڑے;

	إذا (!gtk_widget_get_has_کھڑکی(formchild->widget) &&
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

    أرجع GTK_WIDGET_CLASS(gui_gtk_form_ماپا_class)->draw(widget, cr);
}
#وإلا // !GTK_CHECK_VERSION(3,0,0)
    ساكن gint
form_expose(GtkWidget *widget, GdkEventExpose *event)
{
    GList   *tmp_list;
    GtkForm *form;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    form = GTK_FORM(widget);

    إذا (event->کھڑکی_2 == form->bin_کھڑکی)
	أرجع FALSE;

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
	gtk_container_propagate_expose(GTK_CONTAINER(widget),
		GTK_WIDGET(((Gtk_Form_بچہ_2 *)tmp_list->انکڑے)->widget),
		event);

    أرجع FALSE;
}
#جے_انت // !GTK_CHECK_VERSION(3,0,0)

// Container method
    ساكن فراغ
form_remove(GtkContainer *container, GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gtk_Form_بچہ_2 *بچہ = عدم;	    // init for gcc

    g_return_if_fail(GTK_IS_FORM(container));

    form = GTK_FORM(container);

    tmp_list = form->children;
    طالما (tmp_list)
    {
	بچہ = tmp_list->انکڑے;
	إذا (بچہ->widget == widget)
	    اكسر;
	tmp_list = tmp_list->اگلا;
    }

    إذا (tmp_list == عدم)
	أرجع;

#إذا GTK_CHECK_نسخہ(3,0,0)
    ثابت gboolean was_visible = gtk_widget_get_visible(widget);
#جے_انت
    إذا (بچہ->کھڑکی_2)
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(بچہ->widget),
		FUNC2GENERIC(&form_بچہ_map), بچہ);
	g_signal_handlers_disconnect_by_func(G_OBJECT(بچہ->widget),
		FUNC2GENERIC(&form_بچہ_unmap), بچہ);

	// FIXME: This will cause problems for reparenting NO_WINDOW
	// widgets out of a GtkForm
	gdk_کھڑکی_سیٹ_user_انکڑے(بچہ->کھڑکی_2, عدم);
	gdk_کھڑکی_destroy(بچہ->کھڑکی_2);
    }
    gtk_widget_unparent(widget);
#إذا GTK_CHECK_نسخہ(3,0,0)
    إذا (was_visible)
	gtk_widget_قطار_resize(GTK_WIDGET(container));
#جے_انت
    form->children = g_list_remove_link(form->children, tmp_list);
    g_list_free_1(tmp_list);
    g_free(بچہ);
}

    ساكن فراغ
form_forall(GtkContainer	*container,
		gboolean	include_internals بن_ورتیا,
		GtkCallback	callback,
		gpointer	callback_انکڑے)
{
    GtkForm *form;
    Gtk_Form_بچہ_2 *بچہ;
    GList *tmp_list;

    g_return_if_fail(GTK_IS_FORM(container));
    g_return_if_fail(callback != عدم);

    form = GTK_FORM(container);

    tmp_list = form->children;
    طالما (tmp_list)
    {
	بچہ = tmp_list->انکڑے;
	tmp_list = tmp_list->اگلا;

	(*callback) (بچہ->widget, callback_انکڑے);
    }
}

// Operations on children

    ساكن فراغ
form_attach_بچہ_کھڑکی(GtkForm *form, Gtk_Form_بچہ_2 *بچہ)
{
    إذا (بچہ->کھڑکی_2 != عدم)
	أرجع; // been there, done that

    إذا (!gtk_widget_get_has_کھڑکی(بچہ->widget))
    {
	GtkWidget	*widget;
	Gdk_کھڑکی_Attr	گن;
	gint		گن_نقاب;
	GtkRequisition	requisition;

	widget = GTK_WIDGET(form);

#إذا GTK_CHECK_نسخہ(3,0,0)
	gtk_widget_get_preferred_ناپ(بچہ->widget, &requisition, عدم);
#وإلا
	requisition = بچہ->widget->requisition;
#جے_انت
	گن.کھڑکی_type = GDK_کھڑکی_بچہ;
	گن.x = بچہ->x;
	گن.y = بچہ->y;
	گن.چوڑائی = requisition.چوڑائی;
	گن.اچائی = requisition.اچائی;
	گن.wclass = GDK_داخلہ_OUTPUT;
	گن.visual = gtk_widget_get_visual(widget);
#إذا !GTK_CHECK_نسخہ(3,0,0)
	گن.colormap = gtk_widget_get_colormap(widget);
#جے_انت
	گن.event_نقاب = GDK_EXPOSURE_نقاب;

#إذا GTK_CHECK_نسخہ(3,0,0)
	گن_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#وإلا
	گن_نقاب = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#جے_انت
	بچہ->کھڑکی_2 = gdk_کھڑکی_new(form->bin_کھڑکی,
				       &گن, گن_نقاب);
	gdk_کھڑکی_سیٹ_user_انکڑے(بچہ->کھڑکی_2, widget);

#إذا GTK_CHECK_نسخہ(3,0,0)
	{
	    GtkStyleContext * ثابت sctx = gtk_widget_get_style_context(widget);

	    gtk_style_context_سیٹ_state(sctx, GTK_STATE_FLAG_عام);
# إذا !GTK_CHECK_نسخہ(3,18,0)
	    gtk_style_context_سیٹ_background(sctx, بچہ->کھڑکی_2);
# endif
	}
#وإلا
	gtk_style_سیٹ_background(widget->style,
				 بچہ->کھڑکی_2,
				 GTK_STATE_عام);
#جے_انت

	gtk_widget_سیٹ_ماپا_کھڑکی(بچہ->widget, بچہ->کھڑکی_2);
	/*
	 * Install signal handlers to map/unmap child->window
	 * alongside with the actual widget.
	 */
	g_signal_connect(G_OBJECT(بچہ->widget), "map",
			 G_CALLBACK(&form_بچہ_map), بچہ);
	g_signal_connect(G_OBJECT(بچہ->widget), "unmap",
			 G_CALLBACK(&form_بچہ_unmap), بچہ);
    }
    وإلا إذا (!gtk_widget_get_realized(بچہ->widget))
    {
	gtk_widget_سیٹ_ماپا_کھڑکی(بچہ->widget, form->bin_کھڑکی);
    }
}

    ساكن فراغ
form_realize_بچہ(GtkForm *form, Gtk_Form_بچہ_2 *بچہ)
{
    form_attach_بچہ_کھڑکی(form, بچہ);
    gtk_widget_realize(بچہ->widget);
}

    ساكن فراغ
form_position_بچہ(
    GtkForm		*form,
    Gtk_Form_بچہ_2	*بچہ,
    gboolean		force_allocate)
{
    gint x;
    gint y;

    x = بچہ->x;
    y = بچہ->y;

    إذا ((x >= G_MINSHORT) && (x <= G_MAXSHORT) &&
	(y >= G_MINSHORT) && (y <= G_MAXSHORT))
    {
	إذا (!بچہ->mapped)
	{
	    إذا (gtk_widget_get_mapped(GTK_WIDGET(form))
		    && gtk_widget_get_visible(بچہ->widget))
	    {
		إذا (!gtk_widget_get_mapped(بچہ->widget))
		    gtk_widget_map(بچہ->widget);

		بچہ->mapped = TRUE;
		force_allocate = TRUE;
	    }
	}

	إذا (force_allocate)
	{
	    Gtk_ونڈ ونڈ;
	    GtkRequisition requisition;

#إذا GTK_CHECK_نسخہ(3,0,0)
	    gtk_widget_get_preferred_ناپ(بچہ->widget, &requisition, عدم);
#وإلا
	    requisition = بچہ->widget->requisition;
#جے_انت

	    إذا (!gtk_widget_get_has_کھڑکی(بچہ->widget))
	    {
		إذا (بچہ->کھڑکی_2)
		{
		    gdk_کھڑکی_move_resize(بچہ->کھڑکی_2,
			    x, y,
			    requisition.چوڑائی,
			    requisition.اچائی);
		}

		ونڈ.x = 0;
		ونڈ.y = 0;
	    }
	    وإلا
	    {
		ونڈ.x = x;
		ونڈ.y = y;
	    }

	    ونڈ.چوڑائی = requisition.چوڑائی;
	    ونڈ.اچائی = requisition.اچائی;

	    gtk_widget_ناپ_allocate(بچہ->widget, &ونڈ);
	}
    }
    وإلا
    {
	إذا (بچہ->mapped)
	{
	    بچہ->mapped = FALSE;

	    إذا (gtk_widget_get_mapped(بچہ->widget))
		gtk_widget_unmap(بچہ->widget);
	}
    }
}

    ساكن فراغ
form_position_children(GtkForm *form)
{
    GList *tmp_list;

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->اگلا)
	form_position_بچہ(form, tmp_list->انکڑے, FALSE);
}

    فراغ
gui_gtk_form_move_resize(GtkForm *form, GtkWidget *widget,
		     gint x, gint y, gint w, gint h)
{
#إذا GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_سیٹ_ناپ_request(widget, w, h);
#وإلا
    widget->requisition.چوڑائی  = w;
    widget->requisition.اچائی = h;
#جے_انت

    gui_gtk_form_move(form, widget, x, y);
}

    ساكن فراغ
form_send_configure(GtkForm *form)
{
    GtkWidget *widget;
    GdkEventConfigure event;
    Gtk_ونڈ ونڈ;

    widget = GTK_WIDGET(form);

    gtk_widget_get_ونڈ(widget, &ونڈ);
    event.type = GDK_CONFIGURE;
    event.کھڑکی_2 = gtk_widget_get_کھڑکی(widget);
    event.x = ونڈ.x;
    event.y = ونڈ.y;
    event.چوڑائی = ونڈ.چوڑائی;
    event.اچائی = ونڈ.اچائی;

    gtk_main_do_event((GdkEvent*)&event);
}

    ساكن فراغ
form_بچہ_map(GtkWidget *widget بن_ورتیا, gpointer user_انکڑے)
{
    Gtk_Form_بچہ_2 *بچہ;

    بچہ = (Gtk_Form_بچہ_2 *)user_انکڑے;

    بچہ->mapped = TRUE;
    gdk_کھڑکی_show(بچہ->کھڑکی_2);
}

    ساكن فراغ
form_بچہ_unmap(GtkWidget *widget بن_ورتیا, gpointer user_انکڑے)
{
    Gtk_Form_بچہ_2 *بچہ;

    بچہ = (Gtk_Form_بچہ_2 *)user_انکڑے;

    بچہ->mapped = FALSE;
    gdk_کھڑکی_hide(بچہ->کھڑکی_2);
}
