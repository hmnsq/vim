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
#إذا !GTK_CHECK_نسخه(3,0,0)
# include <gtk/gtksignal.h>
#پایان_اگر
#اگر_تعریف_شده MSWIN
# include <gdk/gdkwin32.h>
#وإلا
# include <gdk/gdkx.h>
#پایان_اگر

تعريفنوع بنية Gtk_Form_فرزند Gtk_Form_فرزند_2;

بنية Gtk_Form_فرزند
{
    GtkWidget *widget;
    Gdk_پنجره *پنجره_2;
    gint x;		// relative subwidget x position
    gint y;		// relative subwidget y position
    gint mapped;
};


ساكن فراغ gui_gtk_form_class_init(GtkFormClass *klass);
#إذا GTK_CHECK_نسخه(3,0,0)
ساكن فراغ gui_gtk_form_init(GtkForm *form);
#وإلا
ساكن فراغ gui_gtk_form_init(GtkForm *form, فراغ *g_class);
#پایان_اگر

ساكن فراغ form_realize(GtkWidget *widget);
ساكن فراغ form_unrealize(GtkWidget *widget);
ساكن فراغ form_map(GtkWidget *widget);
ساكن فراغ form_اندازه_request(GtkWidget *widget, GtkRequisition *requisition);
#إذا GTK_CHECK_نسخه(3,0,0)
ساكن فراغ form_get_preferred_عرض(GtkWidget *widget, gint *کمینه_عرض, gint *natural_عرض);
ساكن فراغ form_get_preferred_ارتفاع(GtkWidget *widget, gint *کمینه_ارتفاع, gint *natural_ارتفاع);
#پایان_اگر
ساكن فراغ form_اندازه_allocate(GtkWidget *widget, Gtk_تخصیص *تخصیص);
#إذا GTK_CHECK_نسخه(3,0,0)
ساكن gboolean form_draw(GtkWidget *widget, cairo_t *cr);
#وإلا
ساكن gint form_expose(GtkWidget *widget, GdkEventExpose *event);
#پایان_اگر

ساكن فراغ form_remove(GtkContainer *container, GtkWidget *widget);
ساكن فراغ form_forall(GtkContainer *container, gboolean include_internals, GtkCallback callback, gpointer callback_داده);

ساكن فراغ form_attach_فرزند_پنجره(GtkForm *form, Gtk_Form_فرزند_2 *فرزند);
ساكن فراغ form_realize_فرزند(GtkForm *form, Gtk_Form_فرزند_2 *فرزند);
ساكن فراغ form_position_فرزند(GtkForm *form, Gtk_Form_فرزند_2 *فرزند, gboolean force_allocate);
ساكن فراغ form_position_children(GtkForm *form);

ساكن فراغ form_send_configure(GtkForm *form);

ساكن فراغ form_فرزند_map(GtkWidget *widget, gpointer user_داده);
ساكن فراغ form_فرزند_unmap(GtkWidget *widget, gpointer user_داده);

#إذا !GTK_CHECK_نسخه(3,0,0)
ساكن GtkWidgetClass *والد_class = عدم;
#پایان_اگر

// Public interface

    GtkWidget *
gui_gtk_form_new(فراغ)
{
    GtkForm *form;

#إذا GTK_CHECK_نسخه(3,0,0)
    form = g_object_new(GTK_TYPE_FORM, عدم);
#وإلا
    form = gtk_type_new(gui_gtk_form_get_type());
#پایان_اگر

    أرجع GTK_WIDGET(form);
}

    فراغ
gui_gtk_form_put(
	GtkForm	*form,
	GtkWidget	*فرزند_widget,
	gint	x,
	gint	y)
{
    Gtk_Form_فرزند_2 *فرزند;

    g_return_if_fail(GTK_IS_FORM(form));

    // LINTED: avoid warning: conversion to 'unsigned long'
    فرزند = g_new(Gtk_Form_فرزند_2, 1);
    إذا (فرزند == عدم)
	أرجع;

    فرزند->widget = فرزند_widget;
    فرزند->پنجره_2 = عدم;
    فرزند->x = x;
    فرزند->y = y;
#إذا GTK_CHECK_نسخه(3,0,0)
    gtk_widget_مجموعه_اندازه_request(فرزند->widget, -1, -1);
#وإلا
    فرزند->widget->requisition.عرض = 0;
    فرزند->widget->requisition.ارتفاع = 0;
#پایان_اگر
    فرزند->mapped = FALSE;

    form->children = g_list_append(form->children, فرزند);

    // child->window must be created and attached to the widget _before_
    // it has been realized, or else things will break with GTK2.  Note
    // that gtk_widget_set_parent() realizes the widget if it's visible
    // and its parent is mapped.
    إذا (gtk_widget_get_realized(GTK_WIDGET(form)))
	form_attach_فرزند_پنجره(form, فرزند);

    gtk_widget_مجموعه_والد(فرزند_widget, GTK_WIDGET(form));

    إذا (gtk_widget_get_realized(GTK_WIDGET(form))
	    && !gtk_widget_get_realized(فرزند_widget))
	form_realize_فرزند(form, فرزند);

    form_position_فرزند(form, فرزند, TRUE);
}

    فراغ
gui_gtk_form_move(
	GtkForm	*form,
	GtkWidget	*فرزند_widget,
	gint	x,
	gint	y)
{
    GList *tmp_list;
    Gtk_Form_فرزند_2 *فرزند;

    g_return_if_fail(GTK_IS_FORM(form));

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بعدی)
    {
	فرزند = tmp_list->داده;
	إذا (فرزند->widget == فرزند_widget)
	{
	    فرزند->x = x;
	    فرزند->y = y;

	    form_position_فرزند(form, فرزند, TRUE);
	    أرجع;
	}
    }
}

    فراغ
gui_gtk_form_freeze(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ++form->freeze_شمار;
}

    فراغ
gui_gtk_form_thaw(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    إذا (!form->freeze_شمار)
	أرجع;

    إذا (!(--form->freeze_شمار))
    {
	form_position_children(form);
	gtk_widget_صف_draw(GTK_WIDGET(form));
    }
}

// Basic Object handling procedures

#إذا GTK_CHECK_نسخه(3,0,0)
G_DEFINE_TYPE(GtkForm, gui_gtk_form, GTK_TYPE_CONTAINER)
#وإلا
    GtkType
gui_gtk_form_get_type(فراغ)
{
    ساكن GtkType form_type = 0;

    إذا (!form_type)
    {
	GtkTypeInfo form_info;

	CLEAR_فیلد(form_info);
	form_info.type_name = "GtkForm";
	form_info.object_اندازه = حجم(GtkForm);
	form_info.class_اندازه = حجم(GtkFormClass);
	form_info.class_init_func = (GtkClassInitFunc)gui_gtk_form_class_init;
	form_info.object_init_func = (GtkObjectInitFunc)gui_gtk_form_init;

	form_type = gtk_type_unique(GTK_TYPE_CONTAINER, &form_info);
    }
    أرجع form_type;
}
#پایان_اگر // !GTK_CHECK_VERSION(3,0,0)

    ساكن فراغ
gui_gtk_form_class_init(GtkFormClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    widget_class = (GtkWidgetClass *) klass;
    container_class = (GtkContainerClass *) klass;

#إذا !GTK_CHECK_نسخه(3,0,0)
    والد_class = gtk_type_class(gtk_container_get_type());
#پایان_اگر

    widget_class->realize = form_realize;
    widget_class->unrealize = form_unrealize;
    widget_class->map = form_map;
#إذا GTK_CHECK_نسخه(3,0,0)
    widget_class->get_preferred_عرض = form_get_preferred_عرض;
    widget_class->get_preferred_ارتفاع = form_get_preferred_ارتفاع;
#وإلا
    widget_class->اندازه_request = form_اندازه_request;
#پایان_اگر
    widget_class->اندازه_allocate = form_اندازه_allocate;
#إذا GTK_CHECK_نسخه(3,0,0)
    widget_class->draw = form_draw;
#وإلا
    widget_class->expose_event = form_expose;
#پایان_اگر

    container_class->remove = form_remove;
    container_class->forall = form_forall;
}

    ساكن فراغ
gui_gtk_form_init(GtkForm *form
#إذا !GTK_CHECK_نسخه(3,0,0)
	, فراغ *g_class استفادهنشده
#پایان_اگر
	)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    gtk_widget_مجموعه_has_پنجره(GTK_WIDGET(form), TRUE);
#پایان_اگر
    form->children = عدم;
    form->bin_پنجره = عدم;
    form->freeze_شمار = 0;
}

/*
 * Widget methods
 */

    ساكن فراغ
form_realize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gdk_پنجره_Attr صفات;
    gint صفات_ماسک;
    Gtk_تخصیص تخصیص;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);
    gtk_widget_مجموعه_realized(widget, TRUE);

    gtk_widget_get_تخصیص(widget, &تخصیص);
    صفات.پنجره_type = GDK_پنجره_فرزند;
    صفات.x = تخصیص.x;
    صفات.y = تخصیص.y;
    صفات.عرض = تخصیص.عرض;
    صفات.ارتفاع = تخصیص.ارتفاع;
    صفات.wclass = GDK_درون_داد_OUTPUT;
    صفات.visual = gtk_widget_get_visual(widget);
#إذا GTK_CHECK_نسخه(3,0,0)
    صفات.event_ماسک = GDK_EXPOSURE_ماسک;
#وإلا
    صفات.colormap = gtk_widget_get_colormap(widget);
    صفات.event_ماسک = GDK_VISIBILITY_NOTIFY_ماسک;
#پایان_اگر

#إذا GTK_CHECK_نسخه(3,0,0)
    صفات_ماسک = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#وإلا
    صفات_ماسک = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#پایان_اگر

    gtk_widget_مجموعه_پنجره(widget,
			  gdk_پنجره_new(gtk_widget_get_والد_پنجره(widget),
					 &صفات, صفات_ماسک));
    gdk_پنجره_مجموعه_user_داده(gtk_widget_get_پنجره(widget), widget);

    صفات.x = 0;
    صفات.y = 0;
    صفات.event_ماسک = gtk_widget_get_events(widget);

    form->bin_پنجره = gdk_پنجره_new(gtk_widget_get_پنجره(widget),
				      &صفات, صفات_ماسک);
    gdk_پنجره_مجموعه_user_داده(form->bin_پنجره, widget);

#إذا GTK_CHECK_نسخه(3,0,0)
    {
	GtkStyleContext * ثابت sctx = gtk_widget_get_style_context(widget);

	gtk_style_context_جمع_کن_class(sctx, "gtk-form");
	gtk_style_context_مجموعه_state(sctx, GTK_STATE_FLAG_نرمال);
# إذا !GTK_CHECK_نسخه(3,18,0)
	gtk_style_context_مجموعه_background(sctx, gtk_widget_get_پنجره(widget));
	gtk_style_context_مجموعه_background(sctx, form->bin_پنجره);
# endif
    }
#وإلا
    widget->style = gtk_style_attach(widget->style, widget->پنجره_2);
    gtk_style_مجموعه_background(widget->style, widget->پنجره_2, GTK_STATE_نرمال);
    gtk_style_مجموعه_background(widget->style, form->bin_پنجره, GTK_STATE_نرمال);
#پایان_اگر

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بعدی)
    {
	Gtk_Form_فرزند_2 *فرزند = tmp_list->داده;

	form_attach_فرزند_پنجره(form, فرزند);

	إذا (gtk_widget_get_visible(فرزند->widget))
	    form_realize_فرزند(form, فرزند);
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

    gtk_widget_مجموعه_mapped(widget, TRUE);

    gdk_پنجره_show(gtk_widget_get_پنجره(widget));
    gdk_پنجره_show(form->bin_پنجره);

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بعدی)
    {
	Gtk_Form_فرزند_2 *فرزند = tmp_list->داده;

	إذا (gtk_widget_get_visible(فرزند->widget)
		&& !gtk_widget_get_mapped(فرزند->widget))
	    gtk_widget_map(فرزند->widget);
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

    gdk_پنجره_مجموعه_user_داده(form->bin_پنجره, عدم);
    gdk_پنجره_destroy(form->bin_پنجره);
    form->bin_پنجره = عدم;

    طالما (tmp_list)
    {
	Gtk_Form_فرزند_2 *فرزند = tmp_list->داده;

	إذا (فرزند->پنجره_2 != عدم)
	{
	    g_signal_handlers_disconnect_by_func(G_OBJECT(فرزند->widget),
		    FUNC2GENERIC(form_فرزند_map),
		    فرزند);
	    g_signal_handlers_disconnect_by_func(G_OBJECT(فرزند->widget),
		    FUNC2GENERIC(form_فرزند_unmap),
		    فرزند);

	    gdk_پنجره_مجموعه_user_داده(فرزند->پنجره_2, عدم);
	    gdk_پنجره_destroy(فرزند->پنجره_2);

	    فرزند->پنجره_2 = عدم;
	}

	tmp_list = tmp_list->بعدی;
    }

#إذا GTK_CHECK_نسخه(3,0,0)
    إذا (GTK_WIDGET_CLASS (gui_gtk_form_والد_class)->unrealize)
	 (* GTK_WIDGET_CLASS (gui_gtk_form_والد_class)->unrealize) (widget);
#وإلا
    إذا (GTK_WIDGET_CLASS (والد_class)->unrealize)
	 (* GTK_WIDGET_CLASS (والد_class)->unrealize) (widget);
#پایان_اگر
}

    ساكن فراغ
form_اندازه_request(GtkWidget *widget, GtkRequisition *requisition)
{
    g_return_if_fail(GTK_IS_FORM(widget));
    g_return_if_fail(requisition != عدم);

    requisition->عرض = 1;
    requisition->ارتفاع = 1;
}

#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن فراغ
form_get_preferred_عرض(GtkWidget *widget,
			     gint      *کمینه_عرض,
			     gint      *natural_عرض)
{
    GtkRequisition requisition;

    form_اندازه_request(widget, &requisition);

    *کمینه_عرض = requisition.عرض;
    *natural_عرض = requisition.عرض;
}

    ساكن فراغ
form_get_preferred_ارتفاع(GtkWidget *widget,
			      gint	*کمینه_ارتفاع,
			      gint	*natural_ارتفاع)
{
    GtkRequisition requisition;

    form_اندازه_request(widget, &requisition);

    *کمینه_ارتفاع = requisition.ارتفاع;
    *natural_ارتفاع = requisition.ارتفاع;
}
#پایان_اگر // GTK_CHECK_VERSION(3,0,0)

    ساكن فراغ
form_اندازه_allocate(GtkWidget *widget, Gtk_تخصیص *تخصیص)
{
    GList *tmp_list;
    GtkForm *form;
    gboolean need_reposition;
    Gtk_تخصیص cur_alloc;

    g_return_if_fail(GTK_IS_FORM(widget));

    gtk_widget_get_تخصیص(widget, &cur_alloc);

    إذا (cur_alloc.x == تخصیص->x
	    && cur_alloc.y == تخصیص->y
	    && cur_alloc.عرض == تخصیص->عرض
	    && cur_alloc.ارتفاع == تخصیص->ارتفاع)
	أرجع;

    need_reposition = cur_alloc.عرض != تخصیص->عرض
		   || cur_alloc.ارتفاع != تخصیص->ارتفاع;
    form = GTK_FORM(widget);

    إذا (need_reposition)
    {
	tmp_list = form->children;

	طالما (tmp_list)
	{
	    Gtk_Form_فرزند_2 *فرزند = tmp_list->داده;
	    form_position_فرزند(form, فرزند, TRUE);

	    tmp_list = tmp_list->بعدی;
	}
    }

    إذا (gtk_widget_get_realized(widget))
    {
	gdk_پنجره_move_resize(gtk_widget_get_پنجره(widget),
			       تخصیص->x, تخصیص->y,
			       تخصیص->عرض, تخصیص->ارتفاع);
	gdk_پنجره_move_resize(GTK_FORM(widget)->bin_پنجره,
			       0, 0,
			       تخصیص->عرض, تخصیص->ارتفاع);
    }
    gtk_widget_مجموعه_تخصیص(widget, تخصیص);
    إذا (need_reposition)
	form_send_configure(form);
}

#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن فراغ
gtk_form_render_background(GtkWidget *widget, cairo_t *cr)
{
    gtk_render_background(gtk_widget_get_style_context(widget), cr,
			  0, 0,
			  gtk_widget_get_allocated_عرض(widget),
			  gtk_widget_get_allocated_ارتفاع(widget));
}

    ساكن gboolean
form_draw(GtkWidget *widget, cairo_t *cr)
{
    GList   *tmp_list = عدم;
    GtkForm *form     = عدم;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    gtk_form_render_background(widget, cr);

    form = GTK_FORM(widget);
    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بعدی)
    {
	Gtk_Form_فرزند_2 * ثابت formchild = tmp_list->داده;

	إذا (!gtk_widget_get_has_پنجره(formchild->widget) &&
		gtk_cairo_should_draw_پنجره(cr, formchild->پنجره_2))
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
	    form_position_فرزند(form, formchild, TRUE);

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

    إذا (event->پنجره_2 == form->bin_پنجره)
	أرجع FALSE;

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بعدی)
	gtk_container_propagate_expose(GTK_CONTAINER(widget),
		GTK_WIDGET(((Gtk_Form_فرزند_2 *)tmp_list->داده)->widget),
		event);

    أرجع FALSE;
}
#پایان_اگر // !GTK_CHECK_VERSION(3,0,0)

// Container method
    ساكن فراغ
form_remove(GtkContainer *container, GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gtk_Form_فرزند_2 *فرزند = عدم;	    // init for gcc

    g_return_if_fail(GTK_IS_FORM(container));

    form = GTK_FORM(container);

    tmp_list = form->children;
    طالما (tmp_list)
    {
	فرزند = tmp_list->داده;
	إذا (فرزند->widget == widget)
	    اكسر;
	tmp_list = tmp_list->بعدی;
    }

    إذا (tmp_list == عدم)
	أرجع;

#إذا GTK_CHECK_نسخه(3,0,0)
    ثابت gboolean was_visible = gtk_widget_get_visible(widget);
#پایان_اگر
    إذا (فرزند->پنجره_2)
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(فرزند->widget),
		FUNC2GENERIC(&form_فرزند_map), فرزند);
	g_signal_handlers_disconnect_by_func(G_OBJECT(فرزند->widget),
		FUNC2GENERIC(&form_فرزند_unmap), فرزند);

	// FIXME: This will cause problems for reparenting NO_WINDOW
	// widgets out of a GtkForm
	gdk_پنجره_مجموعه_user_داده(فرزند->پنجره_2, عدم);
	gdk_پنجره_destroy(فرزند->پنجره_2);
    }
    gtk_widget_unparent(widget);
#إذا GTK_CHECK_نسخه(3,0,0)
    إذا (was_visible)
	gtk_widget_صف_resize(GTK_WIDGET(container));
#پایان_اگر
    form->children = g_list_remove_link(form->children, tmp_list);
    g_list_free_1(tmp_list);
    g_free(فرزند);
}

    ساكن فراغ
form_forall(GtkContainer	*container,
		gboolean	include_internals استفادهنشده,
		GtkCallback	callback,
		gpointer	callback_داده)
{
    GtkForm *form;
    Gtk_Form_فرزند_2 *فرزند;
    GList *tmp_list;

    g_return_if_fail(GTK_IS_FORM(container));
    g_return_if_fail(callback != عدم);

    form = GTK_FORM(container);

    tmp_list = form->children;
    طالما (tmp_list)
    {
	فرزند = tmp_list->داده;
	tmp_list = tmp_list->بعدی;

	(*callback) (فرزند->widget, callback_داده);
    }
}

// Operations on children

    ساكن فراغ
form_attach_فرزند_پنجره(GtkForm *form, Gtk_Form_فرزند_2 *فرزند)
{
    إذا (فرزند->پنجره_2 != عدم)
	أرجع; // been there, done that

    إذا (!gtk_widget_get_has_پنجره(فرزند->widget))
    {
	GtkWidget	*widget;
	Gdk_پنجره_Attr	صفات;
	gint		صفات_ماسک;
	GtkRequisition	requisition;

	widget = GTK_WIDGET(form);

#إذا GTK_CHECK_نسخه(3,0,0)
	gtk_widget_get_preferred_اندازه(فرزند->widget, &requisition, عدم);
#وإلا
	requisition = فرزند->widget->requisition;
#پایان_اگر
	صفات.پنجره_type = GDK_پنجره_فرزند;
	صفات.x = فرزند->x;
	صفات.y = فرزند->y;
	صفات.عرض = requisition.عرض;
	صفات.ارتفاع = requisition.ارتفاع;
	صفات.wclass = GDK_درون_داد_OUTPUT;
	صفات.visual = gtk_widget_get_visual(widget);
#إذا !GTK_CHECK_نسخه(3,0,0)
	صفات.colormap = gtk_widget_get_colormap(widget);
#پایان_اگر
	صفات.event_ماسک = GDK_EXPOSURE_ماسک;

#إذا GTK_CHECK_نسخه(3,0,0)
	صفات_ماسک = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#وإلا
	صفات_ماسک = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#پایان_اگر
	فرزند->پنجره_2 = gdk_پنجره_new(form->bin_پنجره,
				       &صفات, صفات_ماسک);
	gdk_پنجره_مجموعه_user_داده(فرزند->پنجره_2, widget);

#إذا GTK_CHECK_نسخه(3,0,0)
	{
	    GtkStyleContext * ثابت sctx = gtk_widget_get_style_context(widget);

	    gtk_style_context_مجموعه_state(sctx, GTK_STATE_FLAG_نرمال);
# إذا !GTK_CHECK_نسخه(3,18,0)
	    gtk_style_context_مجموعه_background(sctx, فرزند->پنجره_2);
# endif
	}
#وإلا
	gtk_style_مجموعه_background(widget->style,
				 فرزند->پنجره_2,
				 GTK_STATE_نرمال);
#پایان_اگر

	gtk_widget_مجموعه_والد_پنجره(فرزند->widget, فرزند->پنجره_2);
	/*
	 * Install signal handlers to map/unmap child->window
	 * alongside with the actual widget.
	 */
	g_signal_connect(G_OBJECT(فرزند->widget), "map",
			 G_CALLBACK(&form_فرزند_map), فرزند);
	g_signal_connect(G_OBJECT(فرزند->widget), "unmap",
			 G_CALLBACK(&form_فرزند_unmap), فرزند);
    }
    وإلا إذا (!gtk_widget_get_realized(فرزند->widget))
    {
	gtk_widget_مجموعه_والد_پنجره(فرزند->widget, form->bin_پنجره);
    }
}

    ساكن فراغ
form_realize_فرزند(GtkForm *form, Gtk_Form_فرزند_2 *فرزند)
{
    form_attach_فرزند_پنجره(form, فرزند);
    gtk_widget_realize(فرزند->widget);
}

    ساكن فراغ
form_position_فرزند(
    GtkForm		*form,
    Gtk_Form_فرزند_2	*فرزند,
    gboolean		force_allocate)
{
    gint x;
    gint y;

    x = فرزند->x;
    y = فرزند->y;

    إذا ((x >= G_MINSHORT) && (x <= G_MAXSHORT) &&
	(y >= G_MINSHORT) && (y <= G_MAXSHORT))
    {
	إذا (!فرزند->mapped)
	{
	    إذا (gtk_widget_get_mapped(GTK_WIDGET(form))
		    && gtk_widget_get_visible(فرزند->widget))
	    {
		إذا (!gtk_widget_get_mapped(فرزند->widget))
		    gtk_widget_map(فرزند->widget);

		فرزند->mapped = TRUE;
		force_allocate = TRUE;
	    }
	}

	إذا (force_allocate)
	{
	    Gtk_تخصیص تخصیص;
	    GtkRequisition requisition;

#إذا GTK_CHECK_نسخه(3,0,0)
	    gtk_widget_get_preferred_اندازه(فرزند->widget, &requisition, عدم);
#وإلا
	    requisition = فرزند->widget->requisition;
#پایان_اگر

	    إذا (!gtk_widget_get_has_پنجره(فرزند->widget))
	    {
		إذا (فرزند->پنجره_2)
		{
		    gdk_پنجره_move_resize(فرزند->پنجره_2,
			    x, y,
			    requisition.عرض,
			    requisition.ارتفاع);
		}

		تخصیص.x = 0;
		تخصیص.y = 0;
	    }
	    وإلا
	    {
		تخصیص.x = x;
		تخصیص.y = y;
	    }

	    تخصیص.عرض = requisition.عرض;
	    تخصیص.ارتفاع = requisition.ارتفاع;

	    gtk_widget_اندازه_allocate(فرزند->widget, &تخصیص);
	}
    }
    وإلا
    {
	إذا (فرزند->mapped)
	{
	    فرزند->mapped = FALSE;

	    إذا (gtk_widget_get_mapped(فرزند->widget))
		gtk_widget_unmap(فرزند->widget);
	}
    }
}

    ساكن فراغ
form_position_children(GtkForm *form)
{
    GList *tmp_list;

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بعدی)
	form_position_فرزند(form, tmp_list->داده, FALSE);
}

    فراغ
gui_gtk_form_move_resize(GtkForm *form, GtkWidget *widget,
		     gint x, gint y, gint w, gint h)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    gtk_widget_مجموعه_اندازه_request(widget, w, h);
#وإلا
    widget->requisition.عرض  = w;
    widget->requisition.ارتفاع = h;
#پایان_اگر

    gui_gtk_form_move(form, widget, x, y);
}

    ساكن فراغ
form_send_configure(GtkForm *form)
{
    GtkWidget *widget;
    GdkEventConfigure event;
    Gtk_تخصیص تخصیص;

    widget = GTK_WIDGET(form);

    gtk_widget_get_تخصیص(widget, &تخصیص);
    event.type = GDK_CONFIGURE;
    event.پنجره_2 = gtk_widget_get_پنجره(widget);
    event.x = تخصیص.x;
    event.y = تخصیص.y;
    event.عرض = تخصیص.عرض;
    event.ارتفاع = تخصیص.ارتفاع;

    gtk_main_do_event((GdkEvent*)&event);
}

    ساكن فراغ
form_فرزند_map(GtkWidget *widget استفادهنشده, gpointer user_داده)
{
    Gtk_Form_فرزند_2 *فرزند;

    فرزند = (Gtk_Form_فرزند_2 *)user_داده;

    فرزند->mapped = TRUE;
    gdk_پنجره_show(فرزند->پنجره_2);
}

    ساكن فراغ
form_فرزند_unmap(GtkWidget *widget استفادهنشده, gpointer user_داده)
{
    Gtk_Form_فرزند_2 *فرزند;

    فرزند = (Gtk_Form_فرزند_2 *)user_داده;

    فرزند->mapped = FALSE;
    gdk_پنجره_hide(فرزند->پنجره_2);
}
