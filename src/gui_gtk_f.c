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

#అన్తర్భావ "vim.h"
#అన్తర్భావ <gtk/gtk.h>	// without this it compiles, but gives errors at
			// runtime!
#అన్తర్భావ "gui_gtk_f.h"
#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
# include <gtk/gtksignal.h>
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
# include <gdk/gdkwin32.h>
#లేకపోతే
# include <gdk/gdkx.h>
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం Gtk_Form_బిడ్డ Gtk_Form_బిడ్డ_2;

నిర్మాణం Gtk_Form_బిడ్డ
{
    GtkWidget *widget;
    Gdk_కిటికీ *కిటికీ_2;
    gint x;		// relative subwidget x position
    gint y;		// relative subwidget y position
    gint mapped;
};


స్థిర శూన్య gui_gtk_form_class_init(GtkFormClass *klass);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
స్థిర శూన్య gui_gtk_form_init(GtkForm *form);
#లేకపోతే
స్థిర శూన్య gui_gtk_form_init(GtkForm *form, శూన్య *g_class);
#అయితే_ముగింపు

స్థిర శూన్య form_realize(GtkWidget *widget);
స్థిర శూన్య form_unrealize(GtkWidget *widget);
స్థిర శూన్య form_map(GtkWidget *widget);
స్థిర శూన్య form_కొలత_request(GtkWidget *widget, GtkRequisition *requisition);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
స్థిర శూన్య form_get_preferred_వెడల్పు(GtkWidget *widget, gint *కనీస_వెడల్పు, gint *natural_వెడల్పు);
స్థిర శూన్య form_get_preferred_ఎత్తు(GtkWidget *widget, gint *కనీస_ఎత్తు, gint *natural_ఎత్తు);
#అయితే_ముగింపు
స్థిర శూన్య form_కొలత_allocate(GtkWidget *widget, Gtk_కేటాయింపు *కేటాయింపు);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
స్థిర gboolean form_draw(GtkWidget *widget, cairo_t *cr);
#లేకపోతే
స్థిర gint form_expose(GtkWidget *widget, GdkEventExpose *event);
#అయితే_ముగింపు

స్థిర శూన్య form_remove(GtkContainer *container, GtkWidget *widget);
స్థిర శూన్య form_forall(GtkContainer *container, gboolean include_internals, GtkCallback callback, gpointer callback_దత్తాంశం);

స్థిర శూన్య form_attach_బిడ్డ_కిటికీ(GtkForm *form, Gtk_Form_బిడ్డ_2 *బిడ్డ);
స్థిర శూన్య form_realize_బిడ్డ(GtkForm *form, Gtk_Form_బిడ్డ_2 *బిడ్డ);
స్థిర శూన్య form_position_బిడ్డ(GtkForm *form, Gtk_Form_బిడ్డ_2 *బిడ్డ, gboolean force_allocate);
స్థిర శూన్య form_position_children(GtkForm *form);

స్థిర శూన్య form_send_configure(GtkForm *form);

స్థిర శూన్య form_బిడ్డ_map(GtkWidget *widget, gpointer user_దత్తాంశం);
స్థిర శూన్య form_బిడ్డ_unmap(GtkWidget *widget, gpointer user_దత్తాంశం);

#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
స్థిర GtkWidgetClass *మాతృ_class = ఖాళీ;
#అయితే_ముగింపు

// Public interface

    GtkWidget *
gui_gtk_form_new(శూన్య)
{
    GtkForm *form;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    form = g_object_new(GTK_TYPE_FORM, ఖాళీ);
#లేకపోతే
    form = gtk_type_new(gui_gtk_form_get_type());
#అయితే_ముగింపు

    తిరిగి GTK_WIDGET(form);
}

    శూన్య
gui_gtk_form_put(
	GtkForm	*form,
	GtkWidget	*బిడ్డ_widget,
	gint	x,
	gint	y)
{
    Gtk_Form_బిడ్డ_2 *బిడ్డ;

    g_return_if_fail(GTK_IS_FORM(form));

    // LINTED: avoid warning: conversion to 'unsigned long'
    బిడ్డ = g_new(Gtk_Form_బిడ్డ_2, 1);
    ఒకవేళ (బిడ్డ == ఖాళీ)
	తిరిగి;

    బిడ్డ->widget = బిడ్డ_widget;
    బిడ్డ->కిటికీ_2 = ఖాళీ;
    బిడ్డ->x = x;
    బిడ్డ->y = y;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_widget_సమితి_కొలత_request(బిడ్డ->widget, -1, -1);
#లేకపోతే
    బిడ్డ->widget->requisition.వెడల్పు = 0;
    బిడ్డ->widget->requisition.ఎత్తు = 0;
#అయితే_ముగింపు
    బిడ్డ->mapped = FALSE;

    form->children = g_list_append(form->children, బిడ్డ);

    // child->window must be created and attached to the widget _before_
    // it has been realized, or else things will break with GTK2.  Note
    // that gtk_widget_set_parent() realizes the widget if it's visible
    // and its parent is mapped.
    ఒకవేళ (gtk_widget_get_realized(GTK_WIDGET(form)))
	form_attach_బిడ్డ_కిటికీ(form, బిడ్డ);

    gtk_widget_సమితి_మాతృ(బిడ్డ_widget, GTK_WIDGET(form));

    ఒకవేళ (gtk_widget_get_realized(GTK_WIDGET(form))
	    && !gtk_widget_get_realized(బిడ్డ_widget))
	form_realize_బిడ్డ(form, బిడ్డ);

    form_position_బిడ్డ(form, బిడ్డ, TRUE);
}

    శూన్య
gui_gtk_form_move(
	GtkForm	*form,
	GtkWidget	*బిడ్డ_widget,
	gint	x,
	gint	y)
{
    GList *tmp_list;
    Gtk_Form_బిడ్డ_2 *బిడ్డ;

    g_return_if_fail(GTK_IS_FORM(form));

    కొరకు (tmp_list = form->children; tmp_list; tmp_list = tmp_list->తదుపరి)
    {
	బిడ్డ = tmp_list->దత్తాంశం;
	ఒకవేళ (బిడ్డ->widget == బిడ్డ_widget)
	{
	    బిడ్డ->x = x;
	    బిడ్డ->y = y;

	    form_position_బిడ్డ(form, బిడ్డ, TRUE);
	    తిరిగి;
	}
    }
}

    శూన్య
gui_gtk_form_freeze(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ++form->freeze_లెక్క;
}

    శూన్య
gui_gtk_form_thaw(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ఒకవేళ (!form->freeze_లెక్క)
	తిరిగి;

    ఒకవేళ (!(--form->freeze_లెక్క))
    {
	form_position_children(form);
	gtk_widget_వరుస_draw(GTK_WIDGET(form));
    }
}

// Basic Object handling procedures

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
G_DEFINE_TYPE(GtkForm, gui_gtk_form, GTK_TYPE_CONTAINER)
#లేకపోతే
    GtkType
gui_gtk_form_get_type(శూన్య)
{
    స్థిర GtkType form_type = 0;

    ఒకవేళ (!form_type)
    {
	GtkTypeInfo form_info;

	CLEAR_క్షేత్రం(form_info);
	form_info.type_name = "GtkForm";
	form_info.object_కొలత = పరిమాణం(GtkForm);
	form_info.class_కొలత = పరిమాణం(GtkFormClass);
	form_info.class_init_func = (GtkClassInitFunc)gui_gtk_form_class_init;
	form_info.object_init_func = (GtkObjectInitFunc)gui_gtk_form_init;

	form_type = gtk_type_unique(GTK_TYPE_CONTAINER, &form_info);
    }
    తిరిగి form_type;
}
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)

    స్థిర శూన్య
gui_gtk_form_class_init(GtkFormClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    widget_class = (GtkWidgetClass *) klass;
    container_class = (GtkContainerClass *) klass;

#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
    మాతృ_class = gtk_type_class(gtk_container_get_type());
#అయితే_ముగింపు

    widget_class->realize = form_realize;
    widget_class->unrealize = form_unrealize;
    widget_class->map = form_map;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    widget_class->get_preferred_వెడల్పు = form_get_preferred_వెడల్పు;
    widget_class->get_preferred_ఎత్తు = form_get_preferred_ఎత్తు;
#లేకపోతే
    widget_class->కొలత_request = form_కొలత_request;
#అయితే_ముగింపు
    widget_class->కొలత_allocate = form_కొలత_allocate;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    widget_class->draw = form_draw;
#లేకపోతే
    widget_class->expose_event = form_expose;
#అయితే_ముగింపు

    container_class->remove = form_remove;
    container_class->forall = form_forall;
}

    స్థిర శూన్య
gui_gtk_form_init(GtkForm *form
#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
	, శూన్య *g_class వాడని
#అయితే_ముగింపు
	)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_widget_సమితి_has_కిటికీ(GTK_WIDGET(form), TRUE);
#అయితే_ముగింపు
    form->children = ఖాళీ;
    form->bin_కిటికీ = ఖాళీ;
    form->freeze_లెక్క = 0;
}

/*
 * Widget methods
 */

    స్థిర శూన్య
form_realize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gdk_కిటికీ_Attr లక్షణాలు;
    gint లక్షణాలు_ముసుగు;
    Gtk_కేటాయింపు కేటాయింపు;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);
    gtk_widget_సమితి_realized(widget, TRUE);

    gtk_widget_get_కేటాయింపు(widget, &కేటాయింపు);
    లక్షణాలు.కిటికీ_type = GDK_కిటికీ_బిడ్డ;
    లక్షణాలు.x = కేటాయింపు.x;
    లక్షణాలు.y = కేటాయింపు.y;
    లక్షణాలు.వెడల్పు = కేటాయింపు.వెడల్పు;
    లక్షణాలు.ఎత్తు = కేటాయింపు.ఎత్తు;
    లక్షణాలు.wclass = GDK_ఆదానం_OUTPUT;
    లక్షణాలు.visual = gtk_widget_get_visual(widget);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    లక్షణాలు.event_ముసుగు = GDK_EXPOSURE_ముసుగు;
#లేకపోతే
    లక్షణాలు.colormap = gtk_widget_get_colormap(widget);
    లక్షణాలు.event_ముసుగు = GDK_VISIBILITY_NOTIFY_ముసుగు;
#అయితే_ముగింపు

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    లక్షణాలు_ముసుగు = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#లేకపోతే
    లక్షణాలు_ముసుగు = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#అయితే_ముగింపు

    gtk_widget_సమితి_కిటికీ(widget,
			  gdk_కిటికీ_new(gtk_widget_get_మాతృ_కిటికీ(widget),
					 &లక్షణాలు, లక్షణాలు_ముసుగు));
    gdk_కిటికీ_సమితి_user_దత్తాంశం(gtk_widget_get_కిటికీ(widget), widget);

    లక్షణాలు.x = 0;
    లక్షణాలు.y = 0;
    లక్షణాలు.event_ముసుగు = gtk_widget_get_events(widget);

    form->bin_కిటికీ = gdk_కిటికీ_new(gtk_widget_get_కిటికీ(widget),
				      &లక్షణాలు, లక్షణాలు_ముసుగు);
    gdk_కిటికీ_సమితి_user_దత్తాంశం(form->bin_కిటికీ, widget);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    {
	GtkStyleContext * స్థిరరాశి sctx = gtk_widget_get_style_context(widget);

	gtk_style_context_కూడు_class(sctx, "gtk-form");
	gtk_style_context_సమితి_state(sctx, GTK_STATE_FLAG_సాధారణ);
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,18,0)
	gtk_style_context_సమితి_background(sctx, gtk_widget_get_కిటికీ(widget));
	gtk_style_context_సమితి_background(sctx, form->bin_కిటికీ);
# endif
    }
#లేకపోతే
    widget->style = gtk_style_attach(widget->style, widget->కిటికీ_2);
    gtk_style_సమితి_background(widget->style, widget->కిటికీ_2, GTK_STATE_సాధారణ);
    gtk_style_సమితి_background(widget->style, form->bin_కిటికీ, GTK_STATE_సాధారణ);
#అయితే_ముగింపు

    కొరకు (tmp_list = form->children; tmp_list; tmp_list = tmp_list->తదుపరి)
    {
	Gtk_Form_బిడ్డ_2 *బిడ్డ = tmp_list->దత్తాంశం;

	form_attach_బిడ్డ_కిటికీ(form, బిడ్డ);

	ఒకవేళ (gtk_widget_get_visible(బిడ్డ->widget))
	    form_realize_బిడ్డ(form, బిడ్డ);
    }
}


// After reading the documentation at
// https://www.manpagez.com/html/gtk2/gtk2-2.24.29/gtk-changes-2-0.php
// I think it should be possible to remove this function when compiling
// against gtk-2.0.  It doesn't seem to cause problems, though.
//
// Well, I reckon at least the gdk_window_show(form->bin_window)
// is necessary.  GtkForm is anything but a usual container widget.
    స్థిర శూన్య
form_map(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    gtk_widget_సమితి_mapped(widget, TRUE);

    gdk_కిటికీ_show(gtk_widget_get_కిటికీ(widget));
    gdk_కిటికీ_show(form->bin_కిటికీ);

    కొరకు (tmp_list = form->children; tmp_list; tmp_list = tmp_list->తదుపరి)
    {
	Gtk_Form_బిడ్డ_2 *బిడ్డ = tmp_list->దత్తాంశం;

	ఒకవేళ (gtk_widget_get_visible(బిడ్డ->widget)
		&& !gtk_widget_get_mapped(బిడ్డ->widget))
	    gtk_widget_map(బిడ్డ->widget);
    }
}

    స్థిర శూన్య
form_unrealize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    tmp_list = form->children;

    gdk_కిటికీ_సమితి_user_దత్తాంశం(form->bin_కిటికీ, ఖాళీ);
    gdk_కిటికీ_destroy(form->bin_కిటికీ);
    form->bin_కిటికీ = ఖాళీ;

    అయితే (tmp_list)
    {
	Gtk_Form_బిడ్డ_2 *బిడ్డ = tmp_list->దత్తాంశం;

	ఒకవేళ (బిడ్డ->కిటికీ_2 != ఖాళీ)
	{
	    g_signal_handlers_disconnect_by_func(G_OBJECT(బిడ్డ->widget),
		    FUNC2GENERIC(form_బిడ్డ_map),
		    బిడ్డ);
	    g_signal_handlers_disconnect_by_func(G_OBJECT(బిడ్డ->widget),
		    FUNC2GENERIC(form_బిడ్డ_unmap),
		    బిడ్డ);

	    gdk_కిటికీ_సమితి_user_దత్తాంశం(బిడ్డ->కిటికీ_2, ఖాళీ);
	    gdk_కిటికీ_destroy(బిడ్డ->కిటికీ_2);

	    బిడ్డ->కిటికీ_2 = ఖాళీ;
	}

	tmp_list = tmp_list->తదుపరి;
    }

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    ఒకవేళ (GTK_WIDGET_CLASS (gui_gtk_form_మాతృ_class)->unrealize)
	 (* GTK_WIDGET_CLASS (gui_gtk_form_మాతృ_class)->unrealize) (widget);
#లేకపోతే
    ఒకవేళ (GTK_WIDGET_CLASS (మాతృ_class)->unrealize)
	 (* GTK_WIDGET_CLASS (మాతృ_class)->unrealize) (widget);
#అయితే_ముగింపు
}

    స్థిర శూన్య
form_కొలత_request(GtkWidget *widget, GtkRequisition *requisition)
{
    g_return_if_fail(GTK_IS_FORM(widget));
    g_return_if_fail(requisition != ఖాళీ);

    requisition->వెడల్పు = 1;
    requisition->ఎత్తు = 1;
}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర శూన్య
form_get_preferred_వెడల్పు(GtkWidget *widget,
			     gint      *కనీస_వెడల్పు,
			     gint      *natural_వెడల్పు)
{
    GtkRequisition requisition;

    form_కొలత_request(widget, &requisition);

    *కనీస_వెడల్పు = requisition.వెడల్పు;
    *natural_వెడల్పు = requisition.వెడల్పు;
}

    స్థిర శూన్య
form_get_preferred_ఎత్తు(GtkWidget *widget,
			      gint	*కనీస_ఎత్తు,
			      gint	*natural_ఎత్తు)
{
    GtkRequisition requisition;

    form_కొలత_request(widget, &requisition);

    *కనీస_ఎత్తు = requisition.ఎత్తు;
    *natural_ఎత్తు = requisition.ఎత్తు;
}
#అయితే_ముగింపు // GTK_CHECK_VERSION(3,0,0)

    స్థిర శూన్య
form_కొలత_allocate(GtkWidget *widget, Gtk_కేటాయింపు *కేటాయింపు)
{
    GList *tmp_list;
    GtkForm *form;
    gboolean need_reposition;
    Gtk_కేటాయింపు cur_alloc;

    g_return_if_fail(GTK_IS_FORM(widget));

    gtk_widget_get_కేటాయింపు(widget, &cur_alloc);

    ఒకవేళ (cur_alloc.x == కేటాయింపు->x
	    && cur_alloc.y == కేటాయింపు->y
	    && cur_alloc.వెడల్పు == కేటాయింపు->వెడల్పు
	    && cur_alloc.ఎత్తు == కేటాయింపు->ఎత్తు)
	తిరిగి;

    need_reposition = cur_alloc.వెడల్పు != కేటాయింపు->వెడల్పు
		   || cur_alloc.ఎత్తు != కేటాయింపు->ఎత్తు;
    form = GTK_FORM(widget);

    ఒకవేళ (need_reposition)
    {
	tmp_list = form->children;

	అయితే (tmp_list)
	{
	    Gtk_Form_బిడ్డ_2 *బిడ్డ = tmp_list->దత్తాంశం;
	    form_position_బిడ్డ(form, బిడ్డ, TRUE);

	    tmp_list = tmp_list->తదుపరి;
	}
    }

    ఒకవేళ (gtk_widget_get_realized(widget))
    {
	gdk_కిటికీ_move_resize(gtk_widget_get_కిటికీ(widget),
			       కేటాయింపు->x, కేటాయింపు->y,
			       కేటాయింపు->వెడల్పు, కేటాయింపు->ఎత్తు);
	gdk_కిటికీ_move_resize(GTK_FORM(widget)->bin_కిటికీ,
			       0, 0,
			       కేటాయింపు->వెడల్పు, కేటాయింపు->ఎత్తు);
    }
    gtk_widget_సమితి_కేటాయింపు(widget, కేటాయింపు);
    ఒకవేళ (need_reposition)
	form_send_configure(form);
}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర శూన్య
gtk_form_render_background(GtkWidget *widget, cairo_t *cr)
{
    gtk_render_background(gtk_widget_get_style_context(widget), cr,
			  0, 0,
			  gtk_widget_get_allocated_వెడల్పు(widget),
			  gtk_widget_get_allocated_ఎత్తు(widget));
}

    స్థిర gboolean
form_draw(GtkWidget *widget, cairo_t *cr)
{
    GList   *tmp_list = ఖాళీ;
    GtkForm *form     = ఖాళీ;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    gtk_form_render_background(widget, cr);

    form = GTK_FORM(widget);
    కొరకు (tmp_list = form->children; tmp_list; tmp_list = tmp_list->తదుపరి)
    {
	Gtk_Form_బిడ్డ_2 * స్థిరరాశి formchild = tmp_list->దత్తాంశం;

	ఒకవేళ (!gtk_widget_get_has_కిటికీ(formchild->widget) &&
		gtk_cairo_should_draw_కిటికీ(cr, formchild->కిటికీ_2))
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
	    form_position_బిడ్డ(form, formchild, TRUE);

	    gtk_form_render_background(formchild->widget, cr);
	}
    }

    తిరిగి GTK_WIDGET_CLASS(gui_gtk_form_మాతృ_class)->draw(widget, cr);
}
#లేకపోతే // !GTK_CHECK_VERSION(3,0,0)
    స్థిర gint
form_expose(GtkWidget *widget, GdkEventExpose *event)
{
    GList   *tmp_list;
    GtkForm *form;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    form = GTK_FORM(widget);

    ఒకవేళ (event->కిటికీ_2 == form->bin_కిటికీ)
	తిరిగి FALSE;

    కొరకు (tmp_list = form->children; tmp_list; tmp_list = tmp_list->తదుపరి)
	gtk_container_propagate_expose(GTK_CONTAINER(widget),
		GTK_WIDGET(((Gtk_Form_బిడ్డ_2 *)tmp_list->దత్తాంశం)->widget),
		event);

    తిరిగి FALSE;
}
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)

// Container method
    స్థిర శూన్య
form_remove(GtkContainer *container, GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gtk_Form_బిడ్డ_2 *బిడ్డ = ఖాళీ;	    // init for gcc

    g_return_if_fail(GTK_IS_FORM(container));

    form = GTK_FORM(container);

    tmp_list = form->children;
    అయితే (tmp_list)
    {
	బిడ్డ = tmp_list->దత్తాంశం;
	ఒకవేళ (బిడ్డ->widget == widget)
	    ఆపు;
	tmp_list = tmp_list->తదుపరి;
    }

    ఒకవేళ (tmp_list == ఖాళీ)
	తిరిగి;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిరరాశి gboolean was_visible = gtk_widget_get_visible(widget);
#అయితే_ముగింపు
    ఒకవేళ (బిడ్డ->కిటికీ_2)
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(బిడ్డ->widget),
		FUNC2GENERIC(&form_బిడ్డ_map), బిడ్డ);
	g_signal_handlers_disconnect_by_func(G_OBJECT(బిడ్డ->widget),
		FUNC2GENERIC(&form_బిడ్డ_unmap), బిడ్డ);

	// FIXME: This will cause problems for reparenting NO_WINDOW
	// widgets out of a GtkForm
	gdk_కిటికీ_సమితి_user_దత్తాంశం(బిడ్డ->కిటికీ_2, ఖాళీ);
	gdk_కిటికీ_destroy(బిడ్డ->కిటికీ_2);
    }
    gtk_widget_unparent(widget);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    ఒకవేళ (was_visible)
	gtk_widget_వరుస_resize(GTK_WIDGET(container));
#అయితే_ముగింపు
    form->children = g_list_remove_link(form->children, tmp_list);
    g_list_free_1(tmp_list);
    g_free(బిడ్డ);
}

    స్థిర శూన్య
form_forall(GtkContainer	*container,
		gboolean	include_internals వాడని,
		GtkCallback	callback,
		gpointer	callback_దత్తాంశం)
{
    GtkForm *form;
    Gtk_Form_బిడ్డ_2 *బిడ్డ;
    GList *tmp_list;

    g_return_if_fail(GTK_IS_FORM(container));
    g_return_if_fail(callback != ఖాళీ);

    form = GTK_FORM(container);

    tmp_list = form->children;
    అయితే (tmp_list)
    {
	బిడ్డ = tmp_list->దత్తాంశం;
	tmp_list = tmp_list->తదుపరి;

	(*callback) (బిడ్డ->widget, callback_దత్తాంశం);
    }
}

// Operations on children

    స్థిర శూన్య
form_attach_బిడ్డ_కిటికీ(GtkForm *form, Gtk_Form_బిడ్డ_2 *బిడ్డ)
{
    ఒకవేళ (బిడ్డ->కిటికీ_2 != ఖాళీ)
	తిరిగి; // been there, done that

    ఒకవేళ (!gtk_widget_get_has_కిటికీ(బిడ్డ->widget))
    {
	GtkWidget	*widget;
	Gdk_కిటికీ_Attr	లక్షణాలు;
	gint		లక్షణాలు_ముసుగు;
	GtkRequisition	requisition;

	widget = GTK_WIDGET(form);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	gtk_widget_get_preferred_కొలత(బిడ్డ->widget, &requisition, ఖాళీ);
#లేకపోతే
	requisition = బిడ్డ->widget->requisition;
#అయితే_ముగింపు
	లక్షణాలు.కిటికీ_type = GDK_కిటికీ_బిడ్డ;
	లక్షణాలు.x = బిడ్డ->x;
	లక్షణాలు.y = బిడ్డ->y;
	లక్షణాలు.వెడల్పు = requisition.వెడల్పు;
	లక్షణాలు.ఎత్తు = requisition.ఎత్తు;
	లక్షణాలు.wclass = GDK_ఆదానం_OUTPUT;
	లక్షణాలు.visual = gtk_widget_get_visual(widget);
#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
	లక్షణాలు.colormap = gtk_widget_get_colormap(widget);
#అయితే_ముగింపు
	లక్షణాలు.event_ముసుగు = GDK_EXPOSURE_ముసుగు;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	లక్షణాలు_ముసుగు = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#లేకపోతే
	లక్షణాలు_ముసుగు = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#అయితే_ముగింపు
	బిడ్డ->కిటికీ_2 = gdk_కిటికీ_new(form->bin_కిటికీ,
				       &లక్షణాలు, లక్షణాలు_ముసుగు);
	gdk_కిటికీ_సమితి_user_దత్తాంశం(బిడ్డ->కిటికీ_2, widget);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	{
	    GtkStyleContext * స్థిరరాశి sctx = gtk_widget_get_style_context(widget);

	    gtk_style_context_సమితి_state(sctx, GTK_STATE_FLAG_సాధారణ);
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,18,0)
	    gtk_style_context_సమితి_background(sctx, బిడ్డ->కిటికీ_2);
# endif
	}
#లేకపోతే
	gtk_style_సమితి_background(widget->style,
				 బిడ్డ->కిటికీ_2,
				 GTK_STATE_సాధారణ);
#అయితే_ముగింపు

	gtk_widget_సమితి_మాతృ_కిటికీ(బిడ్డ->widget, బిడ్డ->కిటికీ_2);
	/*
	 * Install signal handlers to map/unmap child->window
	 * alongside with the actual widget.
	 */
	g_signal_connect(G_OBJECT(బిడ్డ->widget), "map",
			 G_CALLBACK(&form_బిడ్డ_map), బిడ్డ);
	g_signal_connect(G_OBJECT(బిడ్డ->widget), "unmap",
			 G_CALLBACK(&form_బిడ్డ_unmap), బిడ్డ);
    }
    లేకపోతే ఒకవేళ (!gtk_widget_get_realized(బిడ్డ->widget))
    {
	gtk_widget_సమితి_మాతృ_కిటికీ(బిడ్డ->widget, form->bin_కిటికీ);
    }
}

    స్థిర శూన్య
form_realize_బిడ్డ(GtkForm *form, Gtk_Form_బిడ్డ_2 *బిడ్డ)
{
    form_attach_బిడ్డ_కిటికీ(form, బిడ్డ);
    gtk_widget_realize(బిడ్డ->widget);
}

    స్థిర శూన్య
form_position_బిడ్డ(
    GtkForm		*form,
    Gtk_Form_బిడ్డ_2	*బిడ్డ,
    gboolean		force_allocate)
{
    gint x;
    gint y;

    x = బిడ్డ->x;
    y = బిడ్డ->y;

    ఒకవేళ ((x >= G_MINSHORT) && (x <= G_MAXSHORT) &&
	(y >= G_MINSHORT) && (y <= G_MAXSHORT))
    {
	ఒకవేళ (!బిడ్డ->mapped)
	{
	    ఒకవేళ (gtk_widget_get_mapped(GTK_WIDGET(form))
		    && gtk_widget_get_visible(బిడ్డ->widget))
	    {
		ఒకవేళ (!gtk_widget_get_mapped(బిడ్డ->widget))
		    gtk_widget_map(బిడ్డ->widget);

		బిడ్డ->mapped = TRUE;
		force_allocate = TRUE;
	    }
	}

	ఒకవేళ (force_allocate)
	{
	    Gtk_కేటాయింపు కేటాయింపు;
	    GtkRequisition requisition;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	    gtk_widget_get_preferred_కొలత(బిడ్డ->widget, &requisition, ఖాళీ);
#లేకపోతే
	    requisition = బిడ్డ->widget->requisition;
#అయితే_ముగింపు

	    ఒకవేళ (!gtk_widget_get_has_కిటికీ(బిడ్డ->widget))
	    {
		ఒకవేళ (బిడ్డ->కిటికీ_2)
		{
		    gdk_కిటికీ_move_resize(బిడ్డ->కిటికీ_2,
			    x, y,
			    requisition.వెడల్పు,
			    requisition.ఎత్తు);
		}

		కేటాయింపు.x = 0;
		కేటాయింపు.y = 0;
	    }
	    లేకపోతే
	    {
		కేటాయింపు.x = x;
		కేటాయింపు.y = y;
	    }

	    కేటాయింపు.వెడల్పు = requisition.వెడల్పు;
	    కేటాయింపు.ఎత్తు = requisition.ఎత్తు;

	    gtk_widget_కొలత_allocate(బిడ్డ->widget, &కేటాయింపు);
	}
    }
    లేకపోతే
    {
	ఒకవేళ (బిడ్డ->mapped)
	{
	    బిడ్డ->mapped = FALSE;

	    ఒకవేళ (gtk_widget_get_mapped(బిడ్డ->widget))
		gtk_widget_unmap(బిడ్డ->widget);
	}
    }
}

    స్థిర శూన్య
form_position_children(GtkForm *form)
{
    GList *tmp_list;

    కొరకు (tmp_list = form->children; tmp_list; tmp_list = tmp_list->తదుపరి)
	form_position_బిడ్డ(form, tmp_list->దత్తాంశం, FALSE);
}

    శూన్య
gui_gtk_form_move_resize(GtkForm *form, GtkWidget *widget,
		     gint x, gint y, gint w, gint h)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_widget_సమితి_కొలత_request(widget, w, h);
#లేకపోతే
    widget->requisition.వెడల్పు  = w;
    widget->requisition.ఎత్తు = h;
#అయితే_ముగింపు

    gui_gtk_form_move(form, widget, x, y);
}

    స్థిర శూన్య
form_send_configure(GtkForm *form)
{
    GtkWidget *widget;
    GdkEventConfigure event;
    Gtk_కేటాయింపు కేటాయింపు;

    widget = GTK_WIDGET(form);

    gtk_widget_get_కేటాయింపు(widget, &కేటాయింపు);
    event.type = GDK_CONFIGURE;
    event.కిటికీ_2 = gtk_widget_get_కిటికీ(widget);
    event.x = కేటాయింపు.x;
    event.y = కేటాయింపు.y;
    event.వెడల్పు = కేటాయింపు.వెడల్పు;
    event.ఎత్తు = కేటాయింపు.ఎత్తు;

    gtk_main_do_event((GdkEvent*)&event);
}

    స్థిర శూన్య
form_బిడ్డ_map(GtkWidget *widget వాడని, gpointer user_దత్తాంశం)
{
    Gtk_Form_బిడ్డ_2 *బిడ్డ;

    బిడ్డ = (Gtk_Form_బిడ్డ_2 *)user_దత్తాంశం;

    బిడ్డ->mapped = TRUE;
    gdk_కిటికీ_show(బిడ్డ->కిటికీ_2);
}

    స్థిర శూన్య
form_బిడ్డ_unmap(GtkWidget *widget వాడని, gpointer user_దత్తాంశం)
{
    Gtk_Form_బిడ్డ_2 *బిడ్డ;

    బిడ్డ = (Gtk_Form_బిడ్డ_2 *)user_దత్తాంశం;

    బిడ్డ->mapped = FALSE;
    gdk_కిటికీ_hide(బిడ్డ->కిటికీ_2);
}
