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

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব <gtk/gtk.h>	// without this it compiles, but gives errors at
			// runtime!
#অন্তর্ভাব "gui_gtk_f.h"
#যদি !GTK_CHECK_VERSION(3,0,0)
# include <gtk/gtksignal.h>
#endif
#ifdef MSWIN
# include <gdk/gdkwin32.h>
#অন্যথা
# include <gdk/gdkx.h>
#endif

প্রকারলেখ কাঠামো _GtkFormChild GtkFormChild;

কাঠামো _GtkFormChild
{
    GtkWidget *widget;
    GdkWindow *window;
    gint x;		// relative subwidget x position
    gint y;		// relative subwidget y position
    gint mapped;
};


স্থির শূন্য gui_gtk_form_class_init(GtkFormClass *klass);
#যদি GTK_CHECK_VERSION(3,0,0)
স্থির শূন্য gui_gtk_form_init(GtkForm *form);
#অন্যথা
স্থির শূন্য gui_gtk_form_init(GtkForm *form, শূন্য *g_class);
#endif

স্থির শূন্য form_realize(GtkWidget *widget);
স্থির শূন্য form_unrealize(GtkWidget *widget);
স্থির শূন্য form_map(GtkWidget *widget);
স্থির শূন্য form_আকার_request(GtkWidget *widget, GtkRequisition *requisition);
#যদি GTK_CHECK_VERSION(3,0,0)
স্থির শূন্য form_get_preferred_width(GtkWidget *widget, gint *minimal_width, gint *natural_width);
স্থির শূন্য form_get_preferred_height(GtkWidget *widget, gint *minimal_height, gint *natural_height);
#endif
স্থির শূন্য form_আকার_allocate(GtkWidget *widget, GtkAllocation *allocation);
#যদি GTK_CHECK_VERSION(3,0,0)
স্থির gboolean form_draw(GtkWidget *widget, cairo_t *cr);
#অন্যথা
স্থির gint form_expose(GtkWidget *widget, GdkEventExpose *event);
#endif

স্থির শূন্য form_remove(GtkContainer *container, GtkWidget *widget);
স্থির শূন্য form_forall(GtkContainer *container, gboolean include_internals, GtkCallback callback, gpointer callback_তথ্য);

স্থির শূন্য form_attach_child_window(GtkForm *form, GtkFormChild *child);
স্থির শূন্য form_realize_child(GtkForm *form, GtkFormChild *child);
স্থির শূন্য form_position_child(GtkForm *form, GtkFormChild *child, gboolean force_allocate);
স্থির শূন্য form_position_children(GtkForm *form);

স্থির শূন্য form_send_configure(GtkForm *form);

স্থির শূন্য form_child_map(GtkWidget *widget, gpointer user_তথ্য);
স্থির শূন্য form_child_unmap(GtkWidget *widget, gpointer user_তথ্য);

#যদি !GTK_CHECK_VERSION(3,0,0)
স্থির GtkWidgetClass *parent_class = ফাঁকা;
#endif

// Public interface

    GtkWidget *
gui_gtk_form_new(শূন্য)
{
    GtkForm *form;

#যদি GTK_CHECK_VERSION(3,0,0)
    form = g_object_new(GTK_TYPE_FORM, ফাঁকা);
#অন্যথা
    form = gtk_type_new(gui_gtk_form_get_type());
#endif

    ফেরত GTK_WIDGET(form);
}

    শূন্য
gui_gtk_form_put(
	GtkForm	*form,
	GtkWidget	*child_widget,
	gint	x,
	gint	y)
{
    GtkFormChild *child;

    g_return_if_fail(GTK_IS_FORM(form));

    // LINTED: avoid warning: conversion to 'unsigned long'
    child = g_new(GtkFormChild, 1);
    যদি (child == ফাঁকা)
	ফেরত;

    child->widget = child_widget;
    child->window = ফাঁকা;
    child->x = x;
    child->y = y;
#যদি GTK_CHECK_VERSION(3,0,0)
    gtk_widget_set_আকার_request(child->widget, -1, -1);
#অন্যথা
    child->widget->requisition.width = 0;
    child->widget->requisition.height = 0;
#endif
    child->mapped = FALSE;

    form->children = g_list_append(form->children, child);

    // child->window must be created and attached to the widget _before_
    // it has been realized, or else things will break with GTK2.  Note
    // that gtk_widget_set_parent() realizes the widget if it's visible
    // and its parent is mapped.
    যদি (gtk_widget_get_realized(GTK_WIDGET(form)))
	form_attach_child_window(form, child);

    gtk_widget_set_parent(child_widget, GTK_WIDGET(form));

    যদি (gtk_widget_get_realized(GTK_WIDGET(form))
	    && !gtk_widget_get_realized(child_widget))
	form_realize_child(form, child);

    form_position_child(form, child, TRUE);
}

    শূন্য
gui_gtk_form_move(
	GtkForm	*form,
	GtkWidget	*child_widget,
	gint	x,
	gint	y)
{
    GList *tmp_list;
    GtkFormChild *child;

    g_return_if_fail(GTK_IS_FORM(form));

    জন্য (tmp_list = form->children; tmp_list; tmp_list = tmp_list->পরবর্তী)
    {
	child = tmp_list->তথ্য;
	যদি (child->widget == child_widget)
	{
	    child->x = x;
	    child->y = y;

	    form_position_child(form, child, TRUE);
	    ফেরত;
	}
    }
}

    শূন্য
gui_gtk_form_freeze(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ++form->freeze_গুনতি;
}

    শূন্য
gui_gtk_form_thaw(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    যদি (!form->freeze_গুনতি)
	ফেরত;

    যদি (!(--form->freeze_গুনতি))
    {
	form_position_children(form);
	gtk_widget_queue_draw(GTK_WIDGET(form));
    }
}

// Basic Object handling procedures

#যদি GTK_CHECK_VERSION(3,0,0)
G_DEFINE_TYPE(GtkForm, gui_gtk_form, GTK_TYPE_CONTAINER)
#অন্যথা
    GtkType
gui_gtk_form_get_type(শূন্য)
{
    স্থির GtkType form_type = 0;

    যদি (!form_type)
    {
	GtkTypeInfo form_info;

	CLEAR_FIELD(form_info);
	form_info.type_name = "GtkForm";
	form_info.object_আকার = আকার(GtkForm);
	form_info.class_আকার = আকার(GtkFormClass);
	form_info.class_init_func = (GtkClassInitFunc)gui_gtk_form_class_init;
	form_info.object_init_func = (GtkObjectInitFunc)gui_gtk_form_init;

	form_type = gtk_type_unique(GTK_TYPE_CONTAINER, &form_info);
    }
    ফেরত form_type;
}
#endif // !GTK_CHECK_VERSION(3,0,0)

    স্থির শূন্য
gui_gtk_form_class_init(GtkFormClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    widget_class = (GtkWidgetClass *) klass;
    container_class = (GtkContainerClass *) klass;

#যদি !GTK_CHECK_VERSION(3,0,0)
    parent_class = gtk_type_class(gtk_container_get_type());
#endif

    widget_class->realize = form_realize;
    widget_class->unrealize = form_unrealize;
    widget_class->map = form_map;
#যদি GTK_CHECK_VERSION(3,0,0)
    widget_class->get_preferred_width = form_get_preferred_width;
    widget_class->get_preferred_height = form_get_preferred_height;
#অন্যথা
    widget_class->আকার_request = form_আকার_request;
#endif
    widget_class->আকার_allocate = form_আকার_allocate;
#যদি GTK_CHECK_VERSION(3,0,0)
    widget_class->draw = form_draw;
#অন্যথা
    widget_class->expose_event = form_expose;
#endif

    container_class->remove = form_remove;
    container_class->forall = form_forall;
}

    স্থির শূন্য
gui_gtk_form_init(GtkForm *form
#যদি !GTK_CHECK_VERSION(3,0,0)
	, শূন্য *g_class UNUSED
#endif
	)
{
#যদি GTK_CHECK_VERSION(3,0,0)
    gtk_widget_set_has_window(GTK_WIDGET(form), TRUE);
#endif
    form->children = ফাঁকা;
    form->bin_window = ফাঁকা;
    form->freeze_গুনতি = 0;
}

/*
 * Widget methods
 */

    স্থির শূন্য
form_realize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GtkAllocation allocation;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);
    gtk_widget_set_realized(widget, TRUE);

    gtk_widget_get_allocation(widget, &allocation);
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = allocation.x;
    attributes.y = allocation.y;
    attributes.width = allocation.width;
    attributes.height = allocation.height;
    attributes.wclass = GDK_প্রবেশ_OUTPUT;
    attributes.visual = gtk_widget_get_visual(widget);
#যদি GTK_CHECK_VERSION(3,0,0)
    attributes.event_mask = GDK_EXPOSURE_MASK;
#অন্যথা
    attributes.colormap = gtk_widget_get_colormap(widget);
    attributes.event_mask = GDK_VISIBILITY_NOTIFY_MASK;
#endif

#যদি GTK_CHECK_VERSION(3,0,0)
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#অন্যথা
    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#endif

    gtk_widget_set_window(widget,
			  gdk_window_new(gtk_widget_get_parent_window(widget),
					 &attributes, attributes_mask));
    gdk_window_set_user_তথ্য(gtk_widget_get_window(widget), widget);

    attributes.x = 0;
    attributes.y = 0;
    attributes.event_mask = gtk_widget_get_events(widget);

    form->bin_window = gdk_window_new(gtk_widget_get_window(widget),
				      &attributes, attributes_mask);
    gdk_window_set_user_তথ্য(form->bin_window, widget);

#যদি GTK_CHECK_VERSION(3,0,0)
    {
	GtkStyleContext * ধ্রুবক sctx = gtk_widget_get_style_context(widget);

	gtk_style_context_add_class(sctx, "gtk-form");
	gtk_style_context_set_state(sctx, GTK_STATE_FLAG_NORMAL);
# যদি !GTK_CHECK_VERSION(3,18,0)
	gtk_style_context_set_background(sctx, gtk_widget_get_window(widget));
	gtk_style_context_set_background(sctx, form->bin_window);
# endif
    }
#অন্যথা
    widget->style = gtk_style_attach(widget->style, widget->window);
    gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);
    gtk_style_set_background(widget->style, form->bin_window, GTK_STATE_NORMAL);
#endif

    জন্য (tmp_list = form->children; tmp_list; tmp_list = tmp_list->পরবর্তী)
    {
	GtkFormChild *child = tmp_list->তথ্য;

	form_attach_child_window(form, child);

	যদি (gtk_widget_get_visible(child->widget))
	    form_realize_child(form, child);
    }
}


// After reading the documentation at
// https://www.manpagez.com/html/gtk2/gtk2-2.24.29/gtk-changes-2-0.php
// I think it should be possible to remove this function when compiling
// against gtk-2.0.  It doesn't seem to cause problems, though.
//
// Well, I reckon at least the gdk_window_show(form->bin_window)
// is necessary.  GtkForm is anything but a usual container widget.
    স্থির শূন্য
form_map(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    gtk_widget_set_mapped(widget, TRUE);

    gdk_window_show(gtk_widget_get_window(widget));
    gdk_window_show(form->bin_window);

    জন্য (tmp_list = form->children; tmp_list; tmp_list = tmp_list->পরবর্তী)
    {
	GtkFormChild *child = tmp_list->তথ্য;

	যদি (gtk_widget_get_visible(child->widget)
		&& !gtk_widget_get_mapped(child->widget))
	    gtk_widget_map(child->widget);
    }
}

    স্থির শূন্য
form_unrealize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    tmp_list = form->children;

    gdk_window_set_user_তথ্য(form->bin_window, ফাঁকা);
    gdk_window_destroy(form->bin_window);
    form->bin_window = ফাঁকা;

    যখন (tmp_list)
    {
	GtkFormChild *child = tmp_list->তথ্য;

	যদি (child->window != ফাঁকা)
	{
	    g_signal_handlers_disconnect_by_func(G_OBJECT(child->widget),
		    FUNC2GENERIC(form_child_map),
		    child);
	    g_signal_handlers_disconnect_by_func(G_OBJECT(child->widget),
		    FUNC2GENERIC(form_child_unmap),
		    child);

	    gdk_window_set_user_তথ্য(child->window, ফাঁকা);
	    gdk_window_destroy(child->window);

	    child->window = ফাঁকা;
	}

	tmp_list = tmp_list->পরবর্তী;
    }

#যদি GTK_CHECK_VERSION(3,0,0)
    যদি (GTK_WIDGET_CLASS (gui_gtk_form_parent_class)->unrealize)
	 (* GTK_WIDGET_CLASS (gui_gtk_form_parent_class)->unrealize) (widget);
#অন্যথা
    যদি (GTK_WIDGET_CLASS (parent_class)->unrealize)
	 (* GTK_WIDGET_CLASS (parent_class)->unrealize) (widget);
#endif
}

    স্থির শূন্য
form_আকার_request(GtkWidget *widget, GtkRequisition *requisition)
{
    g_return_if_fail(GTK_IS_FORM(widget));
    g_return_if_fail(requisition != ফাঁকা);

    requisition->width = 1;
    requisition->height = 1;
}

#যদি GTK_CHECK_VERSION(3,0,0)
    স্থির শূন্য
form_get_preferred_width(GtkWidget *widget,
			     gint      *minimal_width,
			     gint      *natural_width)
{
    GtkRequisition requisition;

    form_আকার_request(widget, &requisition);

    *minimal_width = requisition.width;
    *natural_width = requisition.width;
}

    স্থির শূন্য
form_get_preferred_height(GtkWidget *widget,
			      gint	*minimal_height,
			      gint	*natural_height)
{
    GtkRequisition requisition;

    form_আকার_request(widget, &requisition);

    *minimal_height = requisition.height;
    *natural_height = requisition.height;
}
#endif // GTK_CHECK_VERSION(3,0,0)

    স্থির শূন্য
form_আকার_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
    GList *tmp_list;
    GtkForm *form;
    gboolean need_reposition;
    GtkAllocation cur_alloc;

    g_return_if_fail(GTK_IS_FORM(widget));

    gtk_widget_get_allocation(widget, &cur_alloc);

    যদি (cur_alloc.x == allocation->x
	    && cur_alloc.y == allocation->y
	    && cur_alloc.width == allocation->width
	    && cur_alloc.height == allocation->height)
	ফেরত;

    need_reposition = cur_alloc.width != allocation->width
		   || cur_alloc.height != allocation->height;
    form = GTK_FORM(widget);

    যদি (need_reposition)
    {
	tmp_list = form->children;

	যখন (tmp_list)
	{
	    GtkFormChild *child = tmp_list->তথ্য;
	    form_position_child(form, child, TRUE);

	    tmp_list = tmp_list->পরবর্তী;
	}
    }

    যদি (gtk_widget_get_realized(widget))
    {
	gdk_window_move_resize(gtk_widget_get_window(widget),
			       allocation->x, allocation->y,
			       allocation->width, allocation->height);
	gdk_window_move_resize(GTK_FORM(widget)->bin_window,
			       0, 0,
			       allocation->width, allocation->height);
    }
    gtk_widget_set_allocation(widget, allocation);
    যদি (need_reposition)
	form_send_configure(form);
}

#যদি GTK_CHECK_VERSION(3,0,0)
    স্থির শূন্য
gtk_form_render_background(GtkWidget *widget, cairo_t *cr)
{
    gtk_render_background(gtk_widget_get_style_context(widget), cr,
			  0, 0,
			  gtk_widget_get_allocated_width(widget),
			  gtk_widget_get_allocated_height(widget));
}

    স্থির gboolean
form_draw(GtkWidget *widget, cairo_t *cr)
{
    GList   *tmp_list = ফাঁকা;
    GtkForm *form     = ফাঁকা;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    gtk_form_render_background(widget, cr);

    form = GTK_FORM(widget);
    জন্য (tmp_list = form->children; tmp_list; tmp_list = tmp_list->পরবর্তী)
    {
	GtkFormChild * ধ্রুবক formchild = tmp_list->তথ্য;

	যদি (!gtk_widget_get_has_window(formchild->widget) &&
		gtk_cairo_should_draw_window(cr, formchild->window))
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
	    form_position_child(form, formchild, TRUE);

	    gtk_form_render_background(formchild->widget, cr);
	}
    }

    ফেরত GTK_WIDGET_CLASS(gui_gtk_form_parent_class)->draw(widget, cr);
}
#অন্যথা // !GTK_CHECK_VERSION(3,0,0)
    স্থির gint
form_expose(GtkWidget *widget, GdkEventExpose *event)
{
    GList   *tmp_list;
    GtkForm *form;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    form = GTK_FORM(widget);

    যদি (event->window == form->bin_window)
	ফেরত FALSE;

    জন্য (tmp_list = form->children; tmp_list; tmp_list = tmp_list->পরবর্তী)
	gtk_container_propagate_expose(GTK_CONTAINER(widget),
		GTK_WIDGET(((GtkFormChild *)tmp_list->তথ্য)->widget),
		event);

    ফেরত FALSE;
}
#endif // !GTK_CHECK_VERSION(3,0,0)

// Container method
    স্থির শূন্য
form_remove(GtkContainer *container, GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    GtkFormChild *child = ফাঁকা;	    // init for gcc

    g_return_if_fail(GTK_IS_FORM(container));

    form = GTK_FORM(container);

    tmp_list = form->children;
    যখন (tmp_list)
    {
	child = tmp_list->তথ্য;
	যদি (child->widget == widget)
	    বিরতি;
	tmp_list = tmp_list->পরবর্তী;
    }

    যদি (tmp_list == ফাঁকা)
	ফেরত;

#যদি GTK_CHECK_VERSION(3,0,0)
    ধ্রুবক gboolean was_visible = gtk_widget_get_visible(widget);
#endif
    যদি (child->window)
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(child->widget),
		FUNC2GENERIC(&form_child_map), child);
	g_signal_handlers_disconnect_by_func(G_OBJECT(child->widget),
		FUNC2GENERIC(&form_child_unmap), child);

	// FIXME: This will cause problems for reparenting NO_WINDOW
	// widgets out of a GtkForm
	gdk_window_set_user_তথ্য(child->window, ফাঁকা);
	gdk_window_destroy(child->window);
    }
    gtk_widget_unparent(widget);
#যদি GTK_CHECK_VERSION(3,0,0)
    যদি (was_visible)
	gtk_widget_queue_resize(GTK_WIDGET(container));
#endif
    form->children = g_list_remove_link(form->children, tmp_list);
    g_list_free_1(tmp_list);
    g_free(child);
}

    স্থির শূন্য
form_forall(GtkContainer	*container,
		gboolean	include_internals UNUSED,
		GtkCallback	callback,
		gpointer	callback_তথ্য)
{
    GtkForm *form;
    GtkFormChild *child;
    GList *tmp_list;

    g_return_if_fail(GTK_IS_FORM(container));
    g_return_if_fail(callback != ফাঁকা);

    form = GTK_FORM(container);

    tmp_list = form->children;
    যখন (tmp_list)
    {
	child = tmp_list->তথ্য;
	tmp_list = tmp_list->পরবর্তী;

	(*callback) (child->widget, callback_তথ্য);
    }
}

// Operations on children

    স্থির শূন্য
form_attach_child_window(GtkForm *form, GtkFormChild *child)
{
    যদি (child->window != ফাঁকা)
	ফেরত; // been there, done that

    যদি (!gtk_widget_get_has_window(child->widget))
    {
	GtkWidget	*widget;
	GdkWindowAttr	attributes;
	gint		attributes_mask;
	GtkRequisition	requisition;

	widget = GTK_WIDGET(form);

#যদি GTK_CHECK_VERSION(3,0,0)
	gtk_widget_get_preferred_আকার(child->widget, &requisition, ফাঁকা);
#অন্যথা
	requisition = child->widget->requisition;
#endif
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = child->x;
	attributes.y = child->y;
	attributes.width = requisition.width;
	attributes.height = requisition.height;
	attributes.wclass = GDK_প্রবেশ_OUTPUT;
	attributes.visual = gtk_widget_get_visual(widget);
#যদি !GTK_CHECK_VERSION(3,0,0)
	attributes.colormap = gtk_widget_get_colormap(widget);
#endif
	attributes.event_mask = GDK_EXPOSURE_MASK;

#যদি GTK_CHECK_VERSION(3,0,0)
	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#অন্যথা
	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#endif
	child->window = gdk_window_new(form->bin_window,
				       &attributes, attributes_mask);
	gdk_window_set_user_তথ্য(child->window, widget);

#যদি GTK_CHECK_VERSION(3,0,0)
	{
	    GtkStyleContext * ধ্রুবক sctx = gtk_widget_get_style_context(widget);

	    gtk_style_context_set_state(sctx, GTK_STATE_FLAG_NORMAL);
# যদি !GTK_CHECK_VERSION(3,18,0)
	    gtk_style_context_set_background(sctx, child->window);
# endif
	}
#অন্যথা
	gtk_style_set_background(widget->style,
				 child->window,
				 GTK_STATE_NORMAL);
#endif

	gtk_widget_set_parent_window(child->widget, child->window);
	/*
	 * Install signal handlers to map/unmap child->window
	 * alongside with the actual widget.
	 */
	g_signal_connect(G_OBJECT(child->widget), "map",
			 G_CALLBACK(&form_child_map), child);
	g_signal_connect(G_OBJECT(child->widget), "unmap",
			 G_CALLBACK(&form_child_unmap), child);
    }
    অন্যথা যদি (!gtk_widget_get_realized(child->widget))
    {
	gtk_widget_set_parent_window(child->widget, form->bin_window);
    }
}

    স্থির শূন্য
form_realize_child(GtkForm *form, GtkFormChild *child)
{
    form_attach_child_window(form, child);
    gtk_widget_realize(child->widget);
}

    স্থির শূন্য
form_position_child(
    GtkForm		*form,
    GtkFormChild	*child,
    gboolean		force_allocate)
{
    gint x;
    gint y;

    x = child->x;
    y = child->y;

    যদি ((x >= G_MINSHORT) && (x <= G_MAXSHORT) &&
	(y >= G_MINSHORT) && (y <= G_MAXSHORT))
    {
	যদি (!child->mapped)
	{
	    যদি (gtk_widget_get_mapped(GTK_WIDGET(form))
		    && gtk_widget_get_visible(child->widget))
	    {
		যদি (!gtk_widget_get_mapped(child->widget))
		    gtk_widget_map(child->widget);

		child->mapped = TRUE;
		force_allocate = TRUE;
	    }
	}

	যদি (force_allocate)
	{
	    GtkAllocation allocation;
	    GtkRequisition requisition;

#যদি GTK_CHECK_VERSION(3,0,0)
	    gtk_widget_get_preferred_আকার(child->widget, &requisition, ফাঁকা);
#অন্যথা
	    requisition = child->widget->requisition;
#endif

	    যদি (!gtk_widget_get_has_window(child->widget))
	    {
		যদি (child->window)
		{
		    gdk_window_move_resize(child->window,
			    x, y,
			    requisition.width,
			    requisition.height);
		}

		allocation.x = 0;
		allocation.y = 0;
	    }
	    অন্যথা
	    {
		allocation.x = x;
		allocation.y = y;
	    }

	    allocation.width = requisition.width;
	    allocation.height = requisition.height;

	    gtk_widget_আকার_allocate(child->widget, &allocation);
	}
    }
    অন্যথা
    {
	যদি (child->mapped)
	{
	    child->mapped = FALSE;

	    যদি (gtk_widget_get_mapped(child->widget))
		gtk_widget_unmap(child->widget);
	}
    }
}

    স্থির শূন্য
form_position_children(GtkForm *form)
{
    GList *tmp_list;

    জন্য (tmp_list = form->children; tmp_list; tmp_list = tmp_list->পরবর্তী)
	form_position_child(form, tmp_list->তথ্য, FALSE);
}

    শূন্য
gui_gtk_form_move_resize(GtkForm *form, GtkWidget *widget,
		     gint x, gint y, gint w, gint h)
{
#যদি GTK_CHECK_VERSION(3,0,0)
    gtk_widget_set_আকার_request(widget, w, h);
#অন্যথা
    widget->requisition.width  = w;
    widget->requisition.height = h;
#endif

    gui_gtk_form_move(form, widget, x, y);
}

    স্থির শূন্য
form_send_configure(GtkForm *form)
{
    GtkWidget *widget;
    GdkEventConfigure event;
    GtkAllocation allocation;

    widget = GTK_WIDGET(form);

    gtk_widget_get_allocation(widget, &allocation);
    event.type = GDK_CONFIGURE;
    event.window = gtk_widget_get_window(widget);
    event.x = allocation.x;
    event.y = allocation.y;
    event.width = allocation.width;
    event.height = allocation.height;

    gtk_main_do_event((GdkEvent*)&event);
}

    স্থির শূন্য
form_child_map(GtkWidget *widget UNUSED, gpointer user_তথ্য)
{
    GtkFormChild *child;

    child = (GtkFormChild *)user_তথ্য;

    child->mapped = TRUE;
    gdk_window_show(child->window);
}

    স্থির শূন্য
form_child_unmap(GtkWidget *widget UNUSED, gpointer user_তথ্য)
{
    GtkFormChild *child;

    child = (GtkFormChild *)user_তথ্য;

    child->mapped = FALSE;
    gdk_window_hide(child->window);
}
