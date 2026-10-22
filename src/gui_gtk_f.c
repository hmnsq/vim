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
#د_که_پای
#که_تعریف_شوی MSWIN
# include <gdk/gdkwin32.h>
#وإلا
# include <gdk/gdkx.h>
#د_که_پای

تعريفنوع بنية Gtk_Form_ماشوم Gtk_Form_ماشوم_2;

بنية Gtk_Form_ماشوم
{
    GtkWidget *widget;
    Gdk_کړکۍ *کړکۍ_2;
    gint x;		// relative subwidget x position
    gint y;		// relative subwidget y position
    gint mapped;
};


ساكن فراغ gui_gtk_form_class_init(GtkFormClass *klass);
#إذا GTK_CHECK_نسخه(3,0,0)
ساكن فراغ gui_gtk_form_init(GtkForm *form);
#وإلا
ساكن فراغ gui_gtk_form_init(GtkForm *form, فراغ *g_class);
#د_که_پای

ساكن فراغ form_realize(GtkWidget *widget);
ساكن فراغ form_unrealize(GtkWidget *widget);
ساكن فراغ form_map(GtkWidget *widget);
ساكن فراغ form_اندازه_request(GtkWidget *widget, GtkRequisition *requisition);
#إذا GTK_CHECK_نسخه(3,0,0)
ساكن فراغ form_get_preferred_پلنوالی(GtkWidget *widget, gint *اصغري_پلنوالی, gint *natural_پلنوالی);
ساكن فراغ form_get_preferred_لوړوالی(GtkWidget *widget, gint *اصغري_لوړوالی, gint *natural_لوړوالی);
#د_که_پای
ساكن فراغ form_اندازه_allocate(GtkWidget *widget, Gtk_ویش *ویش);
#إذا GTK_CHECK_نسخه(3,0,0)
ساكن gboolean form_draw(GtkWidget *widget, cairo_t *cr);
#وإلا
ساكن gint form_expose(GtkWidget *widget, GdkEventExpose *event);
#د_که_پای

ساكن فراغ form_remove(GtkContainer *container, GtkWidget *widget);
ساكن فراغ form_forall(GtkContainer *container, gboolean include_internals, GtkCallback callback, gpointer callback_معلومات);

ساكن فراغ form_attach_ماشوم_کړکۍ(GtkForm *form, Gtk_Form_ماشوم_2 *ماشوم);
ساكن فراغ form_realize_ماشوم(GtkForm *form, Gtk_Form_ماشوم_2 *ماشوم);
ساكن فراغ form_position_ماشوم(GtkForm *form, Gtk_Form_ماشوم_2 *ماشوم, gboolean force_allocate);
ساكن فراغ form_position_children(GtkForm *form);

ساكن فراغ form_send_configure(GtkForm *form);

ساكن فراغ form_ماشوم_map(GtkWidget *widget, gpointer user_معلومات);
ساكن فراغ form_ماشوم_unmap(GtkWidget *widget, gpointer user_معلومات);

#إذا !GTK_CHECK_نسخه(3,0,0)
ساكن GtkWidgetClass *پلار_class = عدم;
#د_که_پای

// Public interface

    GtkWidget *
gui_gtk_form_new(فراغ)
{
    GtkForm *form;

#إذا GTK_CHECK_نسخه(3,0,0)
    form = g_object_new(GTK_TYPE_FORM, عدم);
#وإلا
    form = gtk_type_new(gui_gtk_form_get_type());
#د_که_پای

    أرجع GTK_WIDGET(form);
}

    فراغ
gui_gtk_form_put(
	GtkForm	*form,
	GtkWidget	*ماشوم_widget,
	gint	x,
	gint	y)
{
    Gtk_Form_ماشوم_2 *ماشوم;

    g_return_if_fail(GTK_IS_FORM(form));

    // LINTED: avoid warning: conversion to 'unsigned long'
    ماشوم = g_new(Gtk_Form_ماشوم_2, 1);
    إذا (ماشوم == عدم)
	أرجع;

    ماشوم->widget = ماشوم_widget;
    ماشوم->کړکۍ_2 = عدم;
    ماشوم->x = x;
    ماشوم->y = y;
#إذا GTK_CHECK_نسخه(3,0,0)
    gtk_widget_سېټ_اندازه_request(ماشوم->widget, -1, -1);
#وإلا
    ماشوم->widget->requisition.پلنوالی = 0;
    ماشوم->widget->requisition.لوړوالی = 0;
#د_که_پای
    ماشوم->mapped = FALSE;

    form->children = g_list_append(form->children, ماشوم);

    // child->window must be created and attached to the widget _before_
    // it has been realized, or else things will break with GTK2.  Note
    // that gtk_widget_set_parent() realizes the widget if it's visible
    // and its parent is mapped.
    إذا (gtk_widget_get_realized(GTK_WIDGET(form)))
	form_attach_ماشوم_کړکۍ(form, ماشوم);

    gtk_widget_سېټ_پلار(ماشوم_widget, GTK_WIDGET(form));

    إذا (gtk_widget_get_realized(GTK_WIDGET(form))
	    && !gtk_widget_get_realized(ماشوم_widget))
	form_realize_ماشوم(form, ماشوم);

    form_position_ماشوم(form, ماشوم, TRUE);
}

    فراغ
gui_gtk_form_move(
	GtkForm	*form,
	GtkWidget	*ماشوم_widget,
	gint	x,
	gint	y)
{
    GList *tmp_list;
    Gtk_Form_ماشوم_2 *ماشوم;

    g_return_if_fail(GTK_IS_FORM(form));

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بل)
    {
	ماشوم = tmp_list->معلومات;
	إذا (ماشوم->widget == ماشوم_widget)
	{
	    ماشوم->x = x;
	    ماشوم->y = y;

	    form_position_ماشوم(form, ماشوم, TRUE);
	    أرجع;
	}
    }
}

    فراغ
gui_gtk_form_freeze(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ++form->freeze_شمېر;
}

    فراغ
gui_gtk_form_thaw(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    إذا (!form->freeze_شمېر)
	أرجع;

    إذا (!(--form->freeze_شمېر))
    {
	form_position_children(form);
	gtk_widget_کتار_draw(GTK_WIDGET(form));
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

	CLEAR_ډګر(form_info);
	form_info.type_name = "GtkForm";
	form_info.object_اندازه = حجم(GtkForm);
	form_info.class_اندازه = حجم(GtkFormClass);
	form_info.class_init_func = (GtkClassInitFunc)gui_gtk_form_class_init;
	form_info.object_init_func = (GtkObjectInitFunc)gui_gtk_form_init;

	form_type = gtk_type_unique(GTK_TYPE_CONTAINER, &form_info);
    }
    أرجع form_type;
}
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)

    ساكن فراغ
gui_gtk_form_class_init(GtkFormClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    widget_class = (GtkWidgetClass *) klass;
    container_class = (GtkContainerClass *) klass;

#إذا !GTK_CHECK_نسخه(3,0,0)
    پلار_class = gtk_type_class(gtk_container_get_type());
#د_که_پای

    widget_class->realize = form_realize;
    widget_class->unrealize = form_unrealize;
    widget_class->map = form_map;
#إذا GTK_CHECK_نسخه(3,0,0)
    widget_class->get_preferred_پلنوالی = form_get_preferred_پلنوالی;
    widget_class->get_preferred_لوړوالی = form_get_preferred_لوړوالی;
#وإلا
    widget_class->اندازه_request = form_اندازه_request;
#د_که_پای
    widget_class->اندازه_allocate = form_اندازه_allocate;
#إذا GTK_CHECK_نسخه(3,0,0)
    widget_class->draw = form_draw;
#وإلا
    widget_class->expose_event = form_expose;
#د_که_پای

    container_class->remove = form_remove;
    container_class->forall = form_forall;
}

    ساكن فراغ
gui_gtk_form_init(GtkForm *form
#إذا !GTK_CHECK_نسخه(3,0,0)
	, فراغ *g_class ناکارول_شوی
#د_که_پای
	)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    gtk_widget_سېټ_has_کړکۍ(GTK_WIDGET(form), TRUE);
#د_که_پای
    form->children = عدم;
    form->bin_کړکۍ = عدم;
    form->freeze_شمېر = 0;
}

/*
 * Widget methods
 */

    ساكن فراغ
form_realize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gdk_کړکۍ_Attr ځانګړتیاوې;
    gint ځانګړتیاوې_ماسک;
    Gtk_ویش ویش;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);
    gtk_widget_سېټ_realized(widget, TRUE);

    gtk_widget_get_ویش(widget, &ویش);
    ځانګړتیاوې.کړکۍ_type = GDK_کړکۍ_ماشوم;
    ځانګړتیاوې.x = ویش.x;
    ځانګړتیاوې.y = ویش.y;
    ځانګړتیاوې.پلنوالی = ویش.پلنوالی;
    ځانګړتیاوې.لوړوالی = ویش.لوړوالی;
    ځانګړتیاوې.wclass = GDK_ننوت_OUTPUT;
    ځانګړتیاوې.visual = gtk_widget_get_visual(widget);
#إذا GTK_CHECK_نسخه(3,0,0)
    ځانګړتیاوې.event_ماسک = GDK_EXPOSURE_ماسک;
#وإلا
    ځانګړتیاوې.colormap = gtk_widget_get_colormap(widget);
    ځانګړتیاوې.event_ماسک = GDK_VISIBILITY_NOTIFY_ماسک;
#د_که_پای

#إذا GTK_CHECK_نسخه(3,0,0)
    ځانګړتیاوې_ماسک = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#وإلا
    ځانګړتیاوې_ماسک = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#د_که_پای

    gtk_widget_سېټ_کړکۍ(widget,
			  gdk_کړکۍ_new(gtk_widget_get_پلار_کړکۍ(widget),
					 &ځانګړتیاوې, ځانګړتیاوې_ماسک));
    gdk_کړکۍ_سېټ_user_معلومات(gtk_widget_get_کړکۍ(widget), widget);

    ځانګړتیاوې.x = 0;
    ځانګړتیاوې.y = 0;
    ځانګړتیاوې.event_ماسک = gtk_widget_get_events(widget);

    form->bin_کړکۍ = gdk_کړکۍ_new(gtk_widget_get_کړکۍ(widget),
				      &ځانګړتیاوې, ځانګړتیاوې_ماسک);
    gdk_کړکۍ_سېټ_user_معلومات(form->bin_کړکۍ, widget);

#إذا GTK_CHECK_نسخه(3,0,0)
    {
	GtkStyleContext * ثابت sctx = gtk_widget_get_style_context(widget);

	gtk_style_context_جمع_کړه_class(sctx, "gtk-form");
	gtk_style_context_سېټ_state(sctx, GTK_STATE_FLAG_نورمال);
# إذا !GTK_CHECK_نسخه(3,18,0)
	gtk_style_context_سېټ_background(sctx, gtk_widget_get_کړکۍ(widget));
	gtk_style_context_سېټ_background(sctx, form->bin_کړکۍ);
# endif
    }
#وإلا
    widget->style = gtk_style_attach(widget->style, widget->کړکۍ_2);
    gtk_style_سېټ_background(widget->style, widget->کړکۍ_2, GTK_STATE_نورمال);
    gtk_style_سېټ_background(widget->style, form->bin_کړکۍ, GTK_STATE_نورمال);
#د_که_پای

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بل)
    {
	Gtk_Form_ماشوم_2 *ماشوم = tmp_list->معلومات;

	form_attach_ماشوم_کړکۍ(form, ماشوم);

	إذا (gtk_widget_get_visible(ماشوم->widget))
	    form_realize_ماشوم(form, ماشوم);
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

    gtk_widget_سېټ_mapped(widget, TRUE);

    gdk_کړکۍ_show(gtk_widget_get_کړکۍ(widget));
    gdk_کړکۍ_show(form->bin_کړکۍ);

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بل)
    {
	Gtk_Form_ماشوم_2 *ماشوم = tmp_list->معلومات;

	إذا (gtk_widget_get_visible(ماشوم->widget)
		&& !gtk_widget_get_mapped(ماشوم->widget))
	    gtk_widget_map(ماشوم->widget);
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

    gdk_کړکۍ_سېټ_user_معلومات(form->bin_کړکۍ, عدم);
    gdk_کړکۍ_destroy(form->bin_کړکۍ);
    form->bin_کړکۍ = عدم;

    طالما (tmp_list)
    {
	Gtk_Form_ماشوم_2 *ماشوم = tmp_list->معلومات;

	إذا (ماشوم->کړکۍ_2 != عدم)
	{
	    g_signal_handlers_disconnect_by_func(G_OBJECT(ماشوم->widget),
		    FUNC2GENERIC(form_ماشوم_map),
		    ماشوم);
	    g_signal_handlers_disconnect_by_func(G_OBJECT(ماشوم->widget),
		    FUNC2GENERIC(form_ماشوم_unmap),
		    ماشوم);

	    gdk_کړکۍ_سېټ_user_معلومات(ماشوم->کړکۍ_2, عدم);
	    gdk_کړکۍ_destroy(ماشوم->کړکۍ_2);

	    ماشوم->کړکۍ_2 = عدم;
	}

	tmp_list = tmp_list->بل;
    }

#إذا GTK_CHECK_نسخه(3,0,0)
    إذا (GTK_WIDGET_CLASS (gui_gtk_form_پلار_class)->unrealize)
	 (* GTK_WIDGET_CLASS (gui_gtk_form_پلار_class)->unrealize) (widget);
#وإلا
    إذا (GTK_WIDGET_CLASS (پلار_class)->unrealize)
	 (* GTK_WIDGET_CLASS (پلار_class)->unrealize) (widget);
#د_که_پای
}

    ساكن فراغ
form_اندازه_request(GtkWidget *widget, GtkRequisition *requisition)
{
    g_return_if_fail(GTK_IS_FORM(widget));
    g_return_if_fail(requisition != عدم);

    requisition->پلنوالی = 1;
    requisition->لوړوالی = 1;
}

#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن فراغ
form_get_preferred_پلنوالی(GtkWidget *widget,
			     gint      *اصغري_پلنوالی,
			     gint      *natural_پلنوالی)
{
    GtkRequisition requisition;

    form_اندازه_request(widget, &requisition);

    *اصغري_پلنوالی = requisition.پلنوالی;
    *natural_پلنوالی = requisition.پلنوالی;
}

    ساكن فراغ
form_get_preferred_لوړوالی(GtkWidget *widget,
			      gint	*اصغري_لوړوالی,
			      gint	*natural_لوړوالی)
{
    GtkRequisition requisition;

    form_اندازه_request(widget, &requisition);

    *اصغري_لوړوالی = requisition.لوړوالی;
    *natural_لوړوالی = requisition.لوړوالی;
}
#د_که_پای // GTK_CHECK_VERSION(3,0,0)

    ساكن فراغ
form_اندازه_allocate(GtkWidget *widget, Gtk_ویش *ویش)
{
    GList *tmp_list;
    GtkForm *form;
    gboolean need_reposition;
    Gtk_ویش cur_alloc;

    g_return_if_fail(GTK_IS_FORM(widget));

    gtk_widget_get_ویش(widget, &cur_alloc);

    إذا (cur_alloc.x == ویش->x
	    && cur_alloc.y == ویش->y
	    && cur_alloc.پلنوالی == ویش->پلنوالی
	    && cur_alloc.لوړوالی == ویش->لوړوالی)
	أرجع;

    need_reposition = cur_alloc.پلنوالی != ویش->پلنوالی
		   || cur_alloc.لوړوالی != ویش->لوړوالی;
    form = GTK_FORM(widget);

    إذا (need_reposition)
    {
	tmp_list = form->children;

	طالما (tmp_list)
	{
	    Gtk_Form_ماشوم_2 *ماشوم = tmp_list->معلومات;
	    form_position_ماشوم(form, ماشوم, TRUE);

	    tmp_list = tmp_list->بل;
	}
    }

    إذا (gtk_widget_get_realized(widget))
    {
	gdk_کړکۍ_move_resize(gtk_widget_get_کړکۍ(widget),
			       ویش->x, ویش->y,
			       ویش->پلنوالی, ویش->لوړوالی);
	gdk_کړکۍ_move_resize(GTK_FORM(widget)->bin_کړکۍ,
			       0, 0,
			       ویش->پلنوالی, ویش->لوړوالی);
    }
    gtk_widget_سېټ_ویش(widget, ویش);
    إذا (need_reposition)
	form_send_configure(form);
}

#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن فراغ
gtk_form_render_background(GtkWidget *widget, cairo_t *cr)
{
    gtk_render_background(gtk_widget_get_style_context(widget), cr,
			  0, 0,
			  gtk_widget_get_allocated_پلنوالی(widget),
			  gtk_widget_get_allocated_لوړوالی(widget));
}

    ساكن gboolean
form_draw(GtkWidget *widget, cairo_t *cr)
{
    GList   *tmp_list = عدم;
    GtkForm *form     = عدم;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    gtk_form_render_background(widget, cr);

    form = GTK_FORM(widget);
    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بل)
    {
	Gtk_Form_ماشوم_2 * ثابت formchild = tmp_list->معلومات;

	إذا (!gtk_widget_get_has_کړکۍ(formchild->widget) &&
		gtk_cairo_should_draw_کړکۍ(cr, formchild->کړکۍ_2))
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
	    form_position_ماشوم(form, formchild, TRUE);

	    gtk_form_render_background(formchild->widget, cr);
	}
    }

    أرجع GTK_WIDGET_CLASS(gui_gtk_form_پلار_class)->draw(widget, cr);
}
#وإلا // !GTK_CHECK_VERSION(3,0,0)
    ساكن gint
form_expose(GtkWidget *widget, GdkEventExpose *event)
{
    GList   *tmp_list;
    GtkForm *form;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    form = GTK_FORM(widget);

    إذا (event->کړکۍ_2 == form->bin_کړکۍ)
	أرجع FALSE;

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بل)
	gtk_container_propagate_expose(GTK_CONTAINER(widget),
		GTK_WIDGET(((Gtk_Form_ماشوم_2 *)tmp_list->معلومات)->widget),
		event);

    أرجع FALSE;
}
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)

// Container method
    ساكن فراغ
form_remove(GtkContainer *container, GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gtk_Form_ماشوم_2 *ماشوم = عدم;	    // init for gcc

    g_return_if_fail(GTK_IS_FORM(container));

    form = GTK_FORM(container);

    tmp_list = form->children;
    طالما (tmp_list)
    {
	ماشوم = tmp_list->معلومات;
	إذا (ماشوم->widget == widget)
	    اكسر;
	tmp_list = tmp_list->بل;
    }

    إذا (tmp_list == عدم)
	أرجع;

#إذا GTK_CHECK_نسخه(3,0,0)
    ثابت gboolean was_visible = gtk_widget_get_visible(widget);
#د_که_پای
    إذا (ماشوم->کړکۍ_2)
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(ماشوم->widget),
		FUNC2GENERIC(&form_ماشوم_map), ماشوم);
	g_signal_handlers_disconnect_by_func(G_OBJECT(ماشوم->widget),
		FUNC2GENERIC(&form_ماشوم_unmap), ماشوم);

	// FIXME: This will cause problems for reparenting NO_WINDOW
	// widgets out of a GtkForm
	gdk_کړکۍ_سېټ_user_معلومات(ماشوم->کړکۍ_2, عدم);
	gdk_کړکۍ_destroy(ماشوم->کړکۍ_2);
    }
    gtk_widget_unparent(widget);
#إذا GTK_CHECK_نسخه(3,0,0)
    إذا (was_visible)
	gtk_widget_کتار_resize(GTK_WIDGET(container));
#د_که_پای
    form->children = g_list_remove_link(form->children, tmp_list);
    g_list_free_1(tmp_list);
    g_free(ماشوم);
}

    ساكن فراغ
form_forall(GtkContainer	*container,
		gboolean	include_internals ناکارول_شوی,
		GtkCallback	callback,
		gpointer	callback_معلومات)
{
    GtkForm *form;
    Gtk_Form_ماشوم_2 *ماشوم;
    GList *tmp_list;

    g_return_if_fail(GTK_IS_FORM(container));
    g_return_if_fail(callback != عدم);

    form = GTK_FORM(container);

    tmp_list = form->children;
    طالما (tmp_list)
    {
	ماشوم = tmp_list->معلومات;
	tmp_list = tmp_list->بل;

	(*callback) (ماشوم->widget, callback_معلومات);
    }
}

// Operations on children

    ساكن فراغ
form_attach_ماشوم_کړکۍ(GtkForm *form, Gtk_Form_ماشوم_2 *ماشوم)
{
    إذا (ماشوم->کړکۍ_2 != عدم)
	أرجع; // been there, done that

    إذا (!gtk_widget_get_has_کړکۍ(ماشوم->widget))
    {
	GtkWidget	*widget;
	Gdk_کړکۍ_Attr	ځانګړتیاوې;
	gint		ځانګړتیاوې_ماسک;
	GtkRequisition	requisition;

	widget = GTK_WIDGET(form);

#إذا GTK_CHECK_نسخه(3,0,0)
	gtk_widget_get_preferred_اندازه(ماشوم->widget, &requisition, عدم);
#وإلا
	requisition = ماشوم->widget->requisition;
#د_که_پای
	ځانګړتیاوې.کړکۍ_type = GDK_کړکۍ_ماشوم;
	ځانګړتیاوې.x = ماشوم->x;
	ځانګړتیاوې.y = ماشوم->y;
	ځانګړتیاوې.پلنوالی = requisition.پلنوالی;
	ځانګړتیاوې.لوړوالی = requisition.لوړوالی;
	ځانګړتیاوې.wclass = GDK_ننوت_OUTPUT;
	ځانګړتیاوې.visual = gtk_widget_get_visual(widget);
#إذا !GTK_CHECK_نسخه(3,0,0)
	ځانګړتیاوې.colormap = gtk_widget_get_colormap(widget);
#د_که_پای
	ځانګړتیاوې.event_ماسک = GDK_EXPOSURE_ماسک;

#إذا GTK_CHECK_نسخه(3,0,0)
	ځانګړتیاوې_ماسک = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#وإلا
	ځانګړتیاوې_ماسک = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#د_که_پای
	ماشوم->کړکۍ_2 = gdk_کړکۍ_new(form->bin_کړکۍ,
				       &ځانګړتیاوې, ځانګړتیاوې_ماسک);
	gdk_کړکۍ_سېټ_user_معلومات(ماشوم->کړکۍ_2, widget);

#إذا GTK_CHECK_نسخه(3,0,0)
	{
	    GtkStyleContext * ثابت sctx = gtk_widget_get_style_context(widget);

	    gtk_style_context_سېټ_state(sctx, GTK_STATE_FLAG_نورمال);
# إذا !GTK_CHECK_نسخه(3,18,0)
	    gtk_style_context_سېټ_background(sctx, ماشوم->کړکۍ_2);
# endif
	}
#وإلا
	gtk_style_سېټ_background(widget->style,
				 ماشوم->کړکۍ_2,
				 GTK_STATE_نورمال);
#د_که_پای

	gtk_widget_سېټ_پلار_کړکۍ(ماشوم->widget, ماشوم->کړکۍ_2);
	/*
	 * Install signal handlers to map/unmap child->window
	 * alongside with the actual widget.
	 */
	g_signal_connect(G_OBJECT(ماشوم->widget), "map",
			 G_CALLBACK(&form_ماشوم_map), ماشوم);
	g_signal_connect(G_OBJECT(ماشوم->widget), "unmap",
			 G_CALLBACK(&form_ماشوم_unmap), ماشوم);
    }
    وإلا إذا (!gtk_widget_get_realized(ماشوم->widget))
    {
	gtk_widget_سېټ_پلار_کړکۍ(ماشوم->widget, form->bin_کړکۍ);
    }
}

    ساكن فراغ
form_realize_ماشوم(GtkForm *form, Gtk_Form_ماشوم_2 *ماشوم)
{
    form_attach_ماشوم_کړکۍ(form, ماشوم);
    gtk_widget_realize(ماشوم->widget);
}

    ساكن فراغ
form_position_ماشوم(
    GtkForm		*form,
    Gtk_Form_ماشوم_2	*ماشوم,
    gboolean		force_allocate)
{
    gint x;
    gint y;

    x = ماشوم->x;
    y = ماشوم->y;

    إذا ((x >= G_MINSHORT) && (x <= G_MAXSHORT) &&
	(y >= G_MINSHORT) && (y <= G_MAXSHORT))
    {
	إذا (!ماشوم->mapped)
	{
	    إذا (gtk_widget_get_mapped(GTK_WIDGET(form))
		    && gtk_widget_get_visible(ماشوم->widget))
	    {
		إذا (!gtk_widget_get_mapped(ماشوم->widget))
		    gtk_widget_map(ماشوم->widget);

		ماشوم->mapped = TRUE;
		force_allocate = TRUE;
	    }
	}

	إذا (force_allocate)
	{
	    Gtk_ویش ویش;
	    GtkRequisition requisition;

#إذا GTK_CHECK_نسخه(3,0,0)
	    gtk_widget_get_preferred_اندازه(ماشوم->widget, &requisition, عدم);
#وإلا
	    requisition = ماشوم->widget->requisition;
#د_که_پای

	    إذا (!gtk_widget_get_has_کړکۍ(ماشوم->widget))
	    {
		إذا (ماشوم->کړکۍ_2)
		{
		    gdk_کړکۍ_move_resize(ماشوم->کړکۍ_2,
			    x, y,
			    requisition.پلنوالی,
			    requisition.لوړوالی);
		}

		ویش.x = 0;
		ویش.y = 0;
	    }
	    وإلا
	    {
		ویش.x = x;
		ویش.y = y;
	    }

	    ویش.پلنوالی = requisition.پلنوالی;
	    ویش.لوړوالی = requisition.لوړوالی;

	    gtk_widget_اندازه_allocate(ماشوم->widget, &ویش);
	}
    }
    وإلا
    {
	إذا (ماشوم->mapped)
	{
	    ماشوم->mapped = FALSE;

	    إذا (gtk_widget_get_mapped(ماشوم->widget))
		gtk_widget_unmap(ماشوم->widget);
	}
    }
}

    ساكن فراغ
form_position_children(GtkForm *form)
{
    GList *tmp_list;

    لكل (tmp_list = form->children; tmp_list; tmp_list = tmp_list->بل)
	form_position_ماشوم(form, tmp_list->معلومات, FALSE);
}

    فراغ
gui_gtk_form_move_resize(GtkForm *form, GtkWidget *widget,
		     gint x, gint y, gint w, gint h)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    gtk_widget_سېټ_اندازه_request(widget, w, h);
#وإلا
    widget->requisition.پلنوالی  = w;
    widget->requisition.لوړوالی = h;
#د_که_پای

    gui_gtk_form_move(form, widget, x, y);
}

    ساكن فراغ
form_send_configure(GtkForm *form)
{
    GtkWidget *widget;
    GdkEventConfigure event;
    Gtk_ویش ویش;

    widget = GTK_WIDGET(form);

    gtk_widget_get_ویش(widget, &ویش);
    event.type = GDK_CONFIGURE;
    event.کړکۍ_2 = gtk_widget_get_کړکۍ(widget);
    event.x = ویش.x;
    event.y = ویش.y;
    event.پلنوالی = ویش.پلنوالی;
    event.لوړوالی = ویش.لوړوالی;

    gtk_main_do_event((GdkEvent*)&event);
}

    ساكن فراغ
form_ماشوم_map(GtkWidget *widget ناکارول_شوی, gpointer user_معلومات)
{
    Gtk_Form_ماشوم_2 *ماشوم;

    ماشوم = (Gtk_Form_ماشوم_2 *)user_معلومات;

    ماشوم->mapped = TRUE;
    gdk_کړکۍ_show(ماشوم->کړکۍ_2);
}

    ساكن فراغ
form_ماشوم_unmap(GtkWidget *widget ناکارول_شوی, gpointer user_معلومات)
{
    Gtk_Form_ماشوم_2 *ماشوم;

    ماشوم = (Gtk_Form_ماشوم_2 *)user_معلومات;

    ماشوم->mapped = FALSE;
    gdk_کړکۍ_hide(ماشوم->کړکۍ_2);
}
