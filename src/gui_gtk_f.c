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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>	// without this it compiles, but gives errors at
			// runtime!
#ಅನ್ತರ್ಭಾವ "gui_gtk_f.h"
#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
# include <gtk/gtksignal.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# include <gdk/gdkwin32.h>
#ಇಲ್ಲದಿದ್ದರೆ
# include <gdk/gdkx.h>
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Gtk_Form_ಮಗು Gtk_Form_ಮಗು_2;

ರಚನೆ Gtk_Form_ಮಗು
{
    GtkWidget *widget;
    Gdk_ಕಿಟಕಿ *ಕಿಟಕಿ_2;
    gint x;		// relative subwidget x position
    gint y;		// relative subwidget y position
    gint mapped;
};


ಸ್ಥಿರ ಶೂನ್ಯ gui_gtk_form_class_init(GtkFormClass *klass);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
ಸ್ಥಿರ ಶೂನ್ಯ gui_gtk_form_init(GtkForm *form);
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ gui_gtk_form_init(GtkForm *form, ಶೂನ್ಯ *g_class);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ form_realize(GtkWidget *widget);
ಸ್ಥಿರ ಶೂನ್ಯ form_unrealize(GtkWidget *widget);
ಸ್ಥಿರ ಶೂನ್ಯ form_map(GtkWidget *widget);
ಸ್ಥಿರ ಶೂನ್ಯ form_ಗಾತ್ರ_request(GtkWidget *widget, GtkRequisition *requisition);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
ಸ್ಥಿರ ಶೂನ್ಯ form_get_preferred_ಅಗಲ(GtkWidget *widget, gint *ಕನಿಷ್ಠತಮ_ಅಗಲ, gint *natural_ಅಗಲ);
ಸ್ಥಿರ ಶೂನ್ಯ form_get_preferred_ಎತ್ತರ(GtkWidget *widget, gint *ಕನಿಷ್ಠತಮ_ಎತ್ತರ, gint *natural_ಎತ್ತರ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ form_ಗಾತ್ರ_allocate(GtkWidget *widget, Gtk_ಹಂಚಿಕೆ *ಹಂಚಿಕೆ);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
ಸ್ಥಿರ gboolean form_draw(GtkWidget *widget, cairo_t *cr);
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ gint form_expose(GtkWidget *widget, GdkEventExpose *event);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ form_remove(GtkContainer *container, GtkWidget *widget);
ಸ್ಥಿರ ಶೂನ್ಯ form_forall(GtkContainer *container, gboolean include_internals, GtkCallback callback, gpointer callback_ದತ್ತಾಂಶ);

ಸ್ಥಿರ ಶೂನ್ಯ form_attach_ಮಗು_ಕಿಟಕಿ(GtkForm *form, Gtk_Form_ಮಗು_2 *ಮಗು);
ಸ್ಥಿರ ಶೂನ್ಯ form_realize_ಮಗು(GtkForm *form, Gtk_Form_ಮಗು_2 *ಮಗು);
ಸ್ಥಿರ ಶೂನ್ಯ form_position_ಮಗು(GtkForm *form, Gtk_Form_ಮಗು_2 *ಮಗು, gboolean force_allocate);
ಸ್ಥಿರ ಶೂನ್ಯ form_position_children(GtkForm *form);

ಸ್ಥಿರ ಶೂನ್ಯ form_send_configure(GtkForm *form);

ಸ್ಥಿರ ಶೂನ್ಯ form_ಮಗು_map(GtkWidget *widget, gpointer user_ದತ್ತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ form_ಮಗು_unmap(GtkWidget *widget, gpointer user_ದತ್ತಾಂಶ);

#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
ಸ್ಥಿರ GtkWidgetClass *ಪೋಷಕ_class = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

// Public interface

    GtkWidget *
gui_gtk_form_new(ಶೂನ್ಯ)
{
    GtkForm *form;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    form = g_object_new(GTK_TYPE_FORM, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
    form = gtk_type_new(gui_gtk_form_get_type());
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ GTK_WIDGET(form);
}

    ಶೂನ್ಯ
gui_gtk_form_put(
	GtkForm	*form,
	GtkWidget	*ಮಗು_widget,
	gint	x,
	gint	y)
{
    Gtk_Form_ಮಗು_2 *ಮಗು;

    g_return_if_fail(GTK_IS_FORM(form));

    // LINTED: avoid warning: conversion to 'unsigned long'
    ಮಗು = g_new(Gtk_Form_ಮಗು_2, 1);
    ಆದರೆ (ಮಗು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಮಗು->widget = ಮಗು_widget;
    ಮಗು->ಕಿಟಕಿ_2 = ಖಾಲಿ;
    ಮಗು->x = x;
    ಮಗು->y = y;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_widget_ಗಣ_ಗಾತ್ರ_request(ಮಗು->widget, -1, -1);
#ಇಲ್ಲದಿದ್ದರೆ
    ಮಗು->widget->requisition.ಅಗಲ = 0;
    ಮಗು->widget->requisition.ಎತ್ತರ = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ಮಗು->mapped = FALSE;

    form->children = g_list_append(form->children, ಮಗು);

    // child->window must be created and attached to the widget _before_
    // it has been realized, or else things will break with GTK2.  Note
    // that gtk_widget_set_parent() realizes the widget if it's visible
    // and its parent is mapped.
    ಆದರೆ (gtk_widget_get_realized(GTK_WIDGET(form)))
	form_attach_ಮಗು_ಕಿಟಕಿ(form, ಮಗು);

    gtk_widget_ಗಣ_ಪೋಷಕ(ಮಗು_widget, GTK_WIDGET(form));

    ಆದರೆ (gtk_widget_get_realized(GTK_WIDGET(form))
	    && !gtk_widget_get_realized(ಮಗು_widget))
	form_realize_ಮಗು(form, ಮಗು);

    form_position_ಮಗು(form, ಮಗು, TRUE);
}

    ಶೂನ್ಯ
gui_gtk_form_move(
	GtkForm	*form,
	GtkWidget	*ಮಗು_widget,
	gint	x,
	gint	y)
{
    GList *tmp_list;
    Gtk_Form_ಮಗು_2 *ಮಗು;

    g_return_if_fail(GTK_IS_FORM(form));

    ಗಾಗಿ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ಮುಂದಿನ)
    {
	ಮಗು = tmp_list->ದತ್ತಾಂಶ;
	ಆದರೆ (ಮಗು->widget == ಮಗು_widget)
	{
	    ಮಗು->x = x;
	    ಮಗು->y = y;

	    form_position_ಮಗು(form, ಮಗು, TRUE);
	    ಹಿಂತಿರುಗಿ;
	}
    }
}

    ಶೂನ್ಯ
gui_gtk_form_freeze(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ++form->freeze_ಎಣಿಕೆ;
}

    ಶೂನ್ಯ
gui_gtk_form_thaw(GtkForm *form)
{
    g_return_if_fail(GTK_IS_FORM(form));

    ಆದರೆ (!form->freeze_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!(--form->freeze_ಎಣಿಕೆ))
    {
	form_position_children(form);
	gtk_widget_ಸರತಿ_draw(GTK_WIDGET(form));
    }
}

// Basic Object handling procedures

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
G_DEFINE_TYPE(GtkForm, gui_gtk_form, GTK_TYPE_CONTAINER)
#ಇಲ್ಲದಿದ್ದರೆ
    GtkType
gui_gtk_form_get_type(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ GtkType form_type = 0;

    ಆದರೆ (!form_type)
    {
	GtkTypeInfo form_info;

	CLEAR_ಕ್ಷೇತ್ರ(form_info);
	form_info.type_name = "GtkForm";
	form_info.object_ಗಾತ್ರ = ಗಾತ್ರ(GtkForm);
	form_info.class_ಗಾತ್ರ = ಗಾತ್ರ(GtkFormClass);
	form_info.class_init_func = (GtkClassInitFunc)gui_gtk_form_class_init;
	form_info.object_init_func = (GtkObjectInitFunc)gui_gtk_form_init;

	form_type = gtk_type_unique(GTK_TYPE_CONTAINER, &form_info);
    }
    ಹಿಂತಿರುಗಿ form_type;
}
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_gtk_form_class_init(GtkFormClass *klass)
{
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    widget_class = (GtkWidgetClass *) klass;
    container_class = (GtkContainerClass *) klass;

#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಪೋಷಕ_class = gtk_type_class(gtk_container_get_type());
#ಆದರೆ_ಅಂತ್ಯ

    widget_class->realize = form_realize;
    widget_class->unrealize = form_unrealize;
    widget_class->map = form_map;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    widget_class->get_preferred_ಅಗಲ = form_get_preferred_ಅಗಲ;
    widget_class->get_preferred_ಎತ್ತರ = form_get_preferred_ಎತ್ತರ;
#ಇಲ್ಲದಿದ್ದರೆ
    widget_class->ಗಾತ್ರ_request = form_ಗಾತ್ರ_request;
#ಆದರೆ_ಅಂತ್ಯ
    widget_class->ಗಾತ್ರ_allocate = form_ಗಾತ್ರ_allocate;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    widget_class->draw = form_draw;
#ಇಲ್ಲದಿದ್ದರೆ
    widget_class->expose_event = form_expose;
#ಆದರೆ_ಅಂತ್ಯ

    container_class->remove = form_remove;
    container_class->forall = form_forall;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_gtk_form_init(GtkForm *form
#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	, ಶೂನ್ಯ *g_class ಬಳಸದ
#ಆದರೆ_ಅಂತ್ಯ
	)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_widget_ಗಣ_has_ಕಿಟಕಿ(GTK_WIDGET(form), TRUE);
#ಆದರೆ_ಅಂತ್ಯ
    form->children = ಖಾಲಿ;
    form->bin_ಕಿಟಕಿ = ಖಾಲಿ;
    form->freeze_ಎಣಿಕೆ = 0;
}

/*
 * Widget methods
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
form_realize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gdk_ಕಿಟಕಿ_Attr ಗುಣಲಕ್ಷಣಗಳು;
    gint ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ;
    Gtk_ಹಂಚಿಕೆ ಹಂಚಿಕೆ;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);
    gtk_widget_ಗಣ_realized(widget, TRUE);

    gtk_widget_get_ಹಂಚಿಕೆ(widget, &ಹಂಚಿಕೆ);
    ಗುಣಲಕ್ಷಣಗಳು.ಕಿಟಕಿ_type = GDK_ಕಿಟಕಿ_ಮಗು;
    ಗುಣಲಕ್ಷಣಗಳು.x = ಹಂಚಿಕೆ.x;
    ಗುಣಲಕ್ಷಣಗಳು.y = ಹಂಚಿಕೆ.y;
    ಗುಣಲಕ್ಷಣಗಳು.ಅಗಲ = ಹಂಚಿಕೆ.ಅಗಲ;
    ಗುಣಲಕ್ಷಣಗಳು.ಎತ್ತರ = ಹಂಚಿಕೆ.ಎತ್ತರ;
    ಗುಣಲಕ್ಷಣಗಳು.wclass = GDK_ಒಳಹರಿವು_OUTPUT;
    ಗುಣಲಕ್ಷಣಗಳು.visual = gtk_widget_get_visual(widget);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಗುಣಲಕ್ಷಣಗಳು.event_ಮುಖವಾಡ = GDK_EXPOSURE_ಮುಖವಾಡ;
#ಇಲ್ಲದಿದ್ದರೆ
    ಗುಣಲಕ್ಷಣಗಳು.colormap = gtk_widget_get_colormap(widget);
    ಗುಣಲಕ್ಷಣಗಳು.event_ಮುಖವಾಡ = GDK_VISIBILITY_NOTIFY_ಮುಖವಾಡ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#ಇಲ್ಲದಿದ್ದರೆ
    ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#ಆದರೆ_ಅಂತ್ಯ

    gtk_widget_ಗಣ_ಕಿಟಕಿ(widget,
			  gdk_ಕಿಟಕಿ_new(gtk_widget_get_ಪೋಷಕ_ಕಿಟಕಿ(widget),
					 &ಗುಣಲಕ್ಷಣಗಳು, ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ));
    gdk_ಕಿಟಕಿ_ಗಣ_user_ದತ್ತಾಂಶ(gtk_widget_get_ಕಿಟಕಿ(widget), widget);

    ಗುಣಲಕ್ಷಣಗಳು.x = 0;
    ಗುಣಲಕ್ಷಣಗಳು.y = 0;
    ಗುಣಲಕ್ಷಣಗಳು.event_ಮುಖವಾಡ = gtk_widget_get_events(widget);

    form->bin_ಕಿಟಕಿ = gdk_ಕಿಟಕಿ_new(gtk_widget_get_ಕಿಟಕಿ(widget),
				      &ಗುಣಲಕ್ಷಣಗಳು, ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ);
    gdk_ಕಿಟಕಿ_ಗಣ_user_ದತ್ತಾಂಶ(form->bin_ಕಿಟಕಿ, widget);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    {
	GtkStyleContext * ಸ್ಥಿರಾಂಕ sctx = gtk_widget_get_style_context(widget);

	gtk_style_context_ಕೂಡಿಸು_class(sctx, "gtk-form");
	gtk_style_context_ಗಣ_state(sctx, GTK_STATE_FLAG_ಸಾಮಾನ್ಯ);
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,18,0)
	gtk_style_context_ಗಣ_background(sctx, gtk_widget_get_ಕಿಟಕಿ(widget));
	gtk_style_context_ಗಣ_background(sctx, form->bin_ಕಿಟಕಿ);
# endif
    }
#ಇಲ್ಲದಿದ್ದರೆ
    widget->style = gtk_style_attach(widget->style, widget->ಕಿಟಕಿ_2);
    gtk_style_ಗಣ_background(widget->style, widget->ಕಿಟಕಿ_2, GTK_STATE_ಸಾಮಾನ್ಯ);
    gtk_style_ಗಣ_background(widget->style, form->bin_ಕಿಟಕಿ, GTK_STATE_ಸಾಮಾನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾಗಿ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ಮುಂದಿನ)
    {
	Gtk_Form_ಮಗು_2 *ಮಗು = tmp_list->ದತ್ತಾಂಶ;

	form_attach_ಮಗು_ಕಿಟಕಿ(form, ಮಗು);

	ಆದರೆ (gtk_widget_get_visible(ಮಗು->widget))
	    form_realize_ಮಗು(form, ಮಗು);
    }
}


// After reading the documentation at
// https://www.manpagez.com/html/gtk2/gtk2-2.24.29/gtk-changes-2-0.php
// I think it should be possible to remove this function when compiling
// against gtk-2.0.  It doesn't seem to cause problems, though.
//
// Well, I reckon at least the gdk_window_show(form->bin_window)
// is necessary.  GtkForm is anything but a usual container widget.
    ಸ್ಥಿರ ಶೂನ್ಯ
form_map(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    gtk_widget_ಗಣ_mapped(widget, TRUE);

    gdk_ಕಿಟಕಿ_show(gtk_widget_get_ಕಿಟಕಿ(widget));
    gdk_ಕಿಟಕಿ_show(form->bin_ಕಿಟಕಿ);

    ಗಾಗಿ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ಮುಂದಿನ)
    {
	Gtk_Form_ಮಗು_2 *ಮಗು = tmp_list->ದತ್ತಾಂಶ;

	ಆದರೆ (gtk_widget_get_visible(ಮಗು->widget)
		&& !gtk_widget_get_mapped(ಮಗು->widget))
	    gtk_widget_map(ಮಗು->widget);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
form_unrealize(GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;

    g_return_if_fail(GTK_IS_FORM(widget));

    form = GTK_FORM(widget);

    tmp_list = form->children;

    gdk_ಕಿಟಕಿ_ಗಣ_user_ದತ್ತಾಂಶ(form->bin_ಕಿಟಕಿ, ಖಾಲಿ);
    gdk_ಕಿಟಕಿ_destroy(form->bin_ಕಿಟಕಿ);
    form->bin_ಕಿಟಕಿ = ಖಾಲಿ;

    ಆಗ (tmp_list)
    {
	Gtk_Form_ಮಗು_2 *ಮಗು = tmp_list->ದತ್ತಾಂಶ;

	ಆದರೆ (ಮಗು->ಕಿಟಕಿ_2 != ಖಾಲಿ)
	{
	    g_signal_handlers_disconnect_by_func(G_OBJECT(ಮಗು->widget),
		    FUNC2GENERIC(form_ಮಗು_map),
		    ಮಗು);
	    g_signal_handlers_disconnect_by_func(G_OBJECT(ಮಗು->widget),
		    FUNC2GENERIC(form_ಮಗು_unmap),
		    ಮಗು);

	    gdk_ಕಿಟಕಿ_ಗಣ_user_ದತ್ತಾಂಶ(ಮಗು->ಕಿಟಕಿ_2, ಖಾಲಿ);
	    gdk_ಕಿಟಕಿ_destroy(ಮಗು->ಕಿಟಕಿ_2);

	    ಮಗು->ಕಿಟಕಿ_2 = ಖಾಲಿ;
	}

	tmp_list = tmp_list->ಮುಂದಿನ;
    }

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಆದರೆ (GTK_WIDGET_CLASS (gui_gtk_form_ಪೋಷಕ_class)->unrealize)
	 (* GTK_WIDGET_CLASS (gui_gtk_form_ಪೋಷಕ_class)->unrealize) (widget);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (GTK_WIDGET_CLASS (ಪೋಷಕ_class)->unrealize)
	 (* GTK_WIDGET_CLASS (ಪೋಷಕ_class)->unrealize) (widget);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
form_ಗಾತ್ರ_request(GtkWidget *widget, GtkRequisition *requisition)
{
    g_return_if_fail(GTK_IS_FORM(widget));
    g_return_if_fail(requisition != ಖಾಲಿ);

    requisition->ಅಗಲ = 1;
    requisition->ಎತ್ತರ = 1;
}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ ಶೂನ್ಯ
form_get_preferred_ಅಗಲ(GtkWidget *widget,
			     gint      *ಕನಿಷ್ಠತಮ_ಅಗಲ,
			     gint      *natural_ಅಗಲ)
{
    GtkRequisition requisition;

    form_ಗಾತ್ರ_request(widget, &requisition);

    *ಕನಿಷ್ಠತಮ_ಅಗಲ = requisition.ಅಗಲ;
    *natural_ಅಗಲ = requisition.ಅಗಲ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
form_get_preferred_ಎತ್ತರ(GtkWidget *widget,
			      gint	*ಕನಿಷ್ಠತಮ_ಎತ್ತರ,
			      gint	*natural_ಎತ್ತರ)
{
    GtkRequisition requisition;

    form_ಗಾತ್ರ_request(widget, &requisition);

    *ಕನಿಷ್ಠತಮ_ಎತ್ತರ = requisition.ಎತ್ತರ;
    *natural_ಎತ್ತರ = requisition.ಎತ್ತರ;
}
#ಆದರೆ_ಅಂತ್ಯ // GTK_CHECK_VERSION(3,0,0)

    ಸ್ಥಿರ ಶೂನ್ಯ
form_ಗಾತ್ರ_allocate(GtkWidget *widget, Gtk_ಹಂಚಿಕೆ *ಹಂಚಿಕೆ)
{
    GList *tmp_list;
    GtkForm *form;
    gboolean need_reposition;
    Gtk_ಹಂಚಿಕೆ cur_alloc;

    g_return_if_fail(GTK_IS_FORM(widget));

    gtk_widget_get_ಹಂಚಿಕೆ(widget, &cur_alloc);

    ಆದರೆ (cur_alloc.x == ಹಂಚಿಕೆ->x
	    && cur_alloc.y == ಹಂಚಿಕೆ->y
	    && cur_alloc.ಅಗಲ == ಹಂಚಿಕೆ->ಅಗಲ
	    && cur_alloc.ಎತ್ತರ == ಹಂಚಿಕೆ->ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ;

    need_reposition = cur_alloc.ಅಗಲ != ಹಂಚಿಕೆ->ಅಗಲ
		   || cur_alloc.ಎತ್ತರ != ಹಂಚಿಕೆ->ಎತ್ತರ;
    form = GTK_FORM(widget);

    ಆದರೆ (need_reposition)
    {
	tmp_list = form->children;

	ಆಗ (tmp_list)
	{
	    Gtk_Form_ಮಗು_2 *ಮಗು = tmp_list->ದತ್ತಾಂಶ;
	    form_position_ಮಗು(form, ಮಗು, TRUE);

	    tmp_list = tmp_list->ಮುಂದಿನ;
	}
    }

    ಆದರೆ (gtk_widget_get_realized(widget))
    {
	gdk_ಕಿಟಕಿ_move_resize(gtk_widget_get_ಕಿಟಕಿ(widget),
			       ಹಂಚಿಕೆ->x, ಹಂಚಿಕೆ->y,
			       ಹಂಚಿಕೆ->ಅಗಲ, ಹಂಚಿಕೆ->ಎತ್ತರ);
	gdk_ಕಿಟಕಿ_move_resize(GTK_FORM(widget)->bin_ಕಿಟಕಿ,
			       0, 0,
			       ಹಂಚಿಕೆ->ಅಗಲ, ಹಂಚಿಕೆ->ಎತ್ತರ);
    }
    gtk_widget_ಗಣ_ಹಂಚಿಕೆ(widget, ಹಂಚಿಕೆ);
    ಆದರೆ (need_reposition)
	form_send_configure(form);
}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ ಶೂನ್ಯ
gtk_form_render_background(GtkWidget *widget, cairo_t *cr)
{
    gtk_render_background(gtk_widget_get_style_context(widget), cr,
			  0, 0,
			  gtk_widget_get_allocated_ಅಗಲ(widget),
			  gtk_widget_get_allocated_ಎತ್ತರ(widget));
}

    ಸ್ಥಿರ gboolean
form_draw(GtkWidget *widget, cairo_t *cr)
{
    GList   *tmp_list = ಖಾಲಿ;
    GtkForm *form     = ಖಾಲಿ;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    gtk_form_render_background(widget, cr);

    form = GTK_FORM(widget);
    ಗಾಗಿ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ಮುಂದಿನ)
    {
	Gtk_Form_ಮಗು_2 * ಸ್ಥಿರಾಂಕ formchild = tmp_list->ದತ್ತಾಂಶ;

	ಆದರೆ (!gtk_widget_get_has_ಕಿಟಕಿ(formchild->widget) &&
		gtk_cairo_should_draw_ಕಿಟಕಿ(cr, formchild->ಕಿಟಕಿ_2))
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
	    form_position_ಮಗು(form, formchild, TRUE);

	    gtk_form_render_background(formchild->widget, cr);
	}
    }

    ಹಿಂತಿರುಗಿ GTK_WIDGET_CLASS(gui_gtk_form_ಪೋಷಕ_class)->draw(widget, cr);
}
#ಇಲ್ಲದಿದ್ದರೆ // !GTK_CHECK_VERSION(3,0,0)
    ಸ್ಥಿರ gint
form_expose(GtkWidget *widget, GdkEventExpose *event)
{
    GList   *tmp_list;
    GtkForm *form;

    g_return_val_if_fail(GTK_IS_FORM(widget), FALSE);

    form = GTK_FORM(widget);

    ಆದರೆ (event->ಕಿಟಕಿ_2 == form->bin_ಕಿಟಕಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ಮುಂದಿನ)
	gtk_container_propagate_expose(GTK_CONTAINER(widget),
		GTK_WIDGET(((Gtk_Form_ಮಗು_2 *)tmp_list->ದತ್ತಾಂಶ)->widget),
		event);

    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)

// Container method
    ಸ್ಥಿರ ಶೂನ್ಯ
form_remove(GtkContainer *container, GtkWidget *widget)
{
    GList *tmp_list;
    GtkForm *form;
    Gtk_Form_ಮಗು_2 *ಮಗು = ಖಾಲಿ;	    // init for gcc

    g_return_if_fail(GTK_IS_FORM(container));

    form = GTK_FORM(container);

    tmp_list = form->children;
    ಆಗ (tmp_list)
    {
	ಮಗು = tmp_list->ದತ್ತಾಂಶ;
	ಆದರೆ (ಮಗು->widget == widget)
	    ನಿಲ್ಲಿಸು;
	tmp_list = tmp_list->ಮುಂದಿನ;
    }

    ಆದರೆ (tmp_list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರಾಂಕ gboolean was_visible = gtk_widget_get_visible(widget);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಮಗು->ಕಿಟಕಿ_2)
    {
	g_signal_handlers_disconnect_by_func(G_OBJECT(ಮಗು->widget),
		FUNC2GENERIC(&form_ಮಗು_map), ಮಗು);
	g_signal_handlers_disconnect_by_func(G_OBJECT(ಮಗು->widget),
		FUNC2GENERIC(&form_ಮಗು_unmap), ಮಗು);

	// FIXME: This will cause problems for reparenting NO_WINDOW
	// widgets out of a GtkForm
	gdk_ಕಿಟಕಿ_ಗಣ_user_ದತ್ತಾಂಶ(ಮಗು->ಕಿಟಕಿ_2, ಖಾಲಿ);
	gdk_ಕಿಟಕಿ_destroy(ಮಗು->ಕಿಟಕಿ_2);
    }
    gtk_widget_unparent(widget);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಆದರೆ (was_visible)
	gtk_widget_ಸರತಿ_resize(GTK_WIDGET(container));
#ಆದರೆ_ಅಂತ್ಯ
    form->children = g_list_remove_link(form->children, tmp_list);
    g_list_free_1(tmp_list);
    g_free(ಮಗು);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
form_forall(GtkContainer	*container,
		gboolean	include_internals ಬಳಸದ,
		GtkCallback	callback,
		gpointer	callback_ದತ್ತಾಂಶ)
{
    GtkForm *form;
    Gtk_Form_ಮಗು_2 *ಮಗು;
    GList *tmp_list;

    g_return_if_fail(GTK_IS_FORM(container));
    g_return_if_fail(callback != ಖಾಲಿ);

    form = GTK_FORM(container);

    tmp_list = form->children;
    ಆಗ (tmp_list)
    {
	ಮಗು = tmp_list->ದತ್ತಾಂಶ;
	tmp_list = tmp_list->ಮುಂದಿನ;

	(*callback) (ಮಗು->widget, callback_ದತ್ತಾಂಶ);
    }
}

// Operations on children

    ಸ್ಥಿರ ಶೂನ್ಯ
form_attach_ಮಗು_ಕಿಟಕಿ(GtkForm *form, Gtk_Form_ಮಗು_2 *ಮಗು)
{
    ಆದರೆ (ಮಗು->ಕಿಟಕಿ_2 != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ; // been there, done that

    ಆದರೆ (!gtk_widget_get_has_ಕಿಟಕಿ(ಮಗು->widget))
    {
	GtkWidget	*widget;
	Gdk_ಕಿಟಕಿ_Attr	ಗುಣಲಕ್ಷಣಗಳು;
	gint		ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ;
	GtkRequisition	requisition;

	widget = GTK_WIDGET(form);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	gtk_widget_get_preferred_ಗಾತ್ರ(ಮಗು->widget, &requisition, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
	requisition = ಮಗು->widget->requisition;
#ಆದರೆ_ಅಂತ್ಯ
	ಗುಣಲಕ್ಷಣಗಳು.ಕಿಟಕಿ_type = GDK_ಕಿಟಕಿ_ಮಗು;
	ಗುಣಲಕ್ಷಣಗಳು.x = ಮಗು->x;
	ಗುಣಲಕ್ಷಣಗಳು.y = ಮಗು->y;
	ಗುಣಲಕ್ಷಣಗಳು.ಅಗಲ = requisition.ಅಗಲ;
	ಗುಣಲಕ್ಷಣಗಳು.ಎತ್ತರ = requisition.ಎತ್ತರ;
	ಗುಣಲಕ್ಷಣಗಳು.wclass = GDK_ಒಳಹರಿವು_OUTPUT;
	ಗುಣಲಕ್ಷಣಗಳು.visual = gtk_widget_get_visual(widget);
#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	ಗುಣಲಕ್ಷಣಗಳು.colormap = gtk_widget_get_colormap(widget);
#ಆದರೆ_ಅಂತ್ಯ
	ಗುಣಲಕ್ಷಣಗಳು.event_ಮುಖವಾಡ = GDK_EXPOSURE_ಮುಖವಾಡ;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
#ಇಲ್ಲದಿದ್ದರೆ
	ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
#ಆದರೆ_ಅಂತ್ಯ
	ಮಗು->ಕಿಟಕಿ_2 = gdk_ಕಿಟಕಿ_new(form->bin_ಕಿಟಕಿ,
				       &ಗುಣಲಕ್ಷಣಗಳು, ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ);
	gdk_ಕಿಟಕಿ_ಗಣ_user_ದತ್ತಾಂಶ(ಮಗು->ಕಿಟಕಿ_2, widget);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	{
	    GtkStyleContext * ಸ್ಥಿರಾಂಕ sctx = gtk_widget_get_style_context(widget);

	    gtk_style_context_ಗಣ_state(sctx, GTK_STATE_FLAG_ಸಾಮಾನ್ಯ);
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,18,0)
	    gtk_style_context_ಗಣ_background(sctx, ಮಗು->ಕಿಟಕಿ_2);
# endif
	}
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_style_ಗಣ_background(widget->style,
				 ಮಗು->ಕಿಟಕಿ_2,
				 GTK_STATE_ಸಾಮಾನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

	gtk_widget_ಗಣ_ಪೋಷಕ_ಕಿಟಕಿ(ಮಗು->widget, ಮಗು->ಕಿಟಕಿ_2);
	/*
	 * Install signal handlers to map/unmap child->window
	 * alongside with the actual widget.
	 */
	g_signal_connect(G_OBJECT(ಮಗು->widget), "map",
			 G_CALLBACK(&form_ಮಗು_map), ಮಗು);
	g_signal_connect(G_OBJECT(ಮಗು->widget), "unmap",
			 G_CALLBACK(&form_ಮಗು_unmap), ಮಗು);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!gtk_widget_get_realized(ಮಗು->widget))
    {
	gtk_widget_ಗಣ_ಪೋಷಕ_ಕಿಟಕಿ(ಮಗು->widget, form->bin_ಕಿಟಕಿ);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
form_realize_ಮಗು(GtkForm *form, Gtk_Form_ಮಗು_2 *ಮಗು)
{
    form_attach_ಮಗು_ಕಿಟಕಿ(form, ಮಗು);
    gtk_widget_realize(ಮಗು->widget);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
form_position_ಮಗು(
    GtkForm		*form,
    Gtk_Form_ಮಗು_2	*ಮಗು,
    gboolean		force_allocate)
{
    gint x;
    gint y;

    x = ಮಗು->x;
    y = ಮಗು->y;

    ಆದರೆ ((x >= G_MINSHORT) && (x <= G_MAXSHORT) &&
	(y >= G_MINSHORT) && (y <= G_MAXSHORT))
    {
	ಆದರೆ (!ಮಗು->mapped)
	{
	    ಆದರೆ (gtk_widget_get_mapped(GTK_WIDGET(form))
		    && gtk_widget_get_visible(ಮಗು->widget))
	    {
		ಆದರೆ (!gtk_widget_get_mapped(ಮಗು->widget))
		    gtk_widget_map(ಮಗು->widget);

		ಮಗು->mapped = TRUE;
		force_allocate = TRUE;
	    }
	}

	ಆದರೆ (force_allocate)
	{
	    Gtk_ಹಂಚಿಕೆ ಹಂಚಿಕೆ;
	    GtkRequisition requisition;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	    gtk_widget_get_preferred_ಗಾತ್ರ(ಮಗು->widget, &requisition, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
	    requisition = ಮಗು->widget->requisition;
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (!gtk_widget_get_has_ಕಿಟಕಿ(ಮಗು->widget))
	    {
		ಆದರೆ (ಮಗು->ಕಿಟಕಿ_2)
		{
		    gdk_ಕಿಟಕಿ_move_resize(ಮಗು->ಕಿಟಕಿ_2,
			    x, y,
			    requisition.ಅಗಲ,
			    requisition.ಎತ್ತರ);
		}

		ಹಂಚಿಕೆ.x = 0;
		ಹಂಚಿಕೆ.y = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಹಂಚಿಕೆ.x = x;
		ಹಂಚಿಕೆ.y = y;
	    }

	    ಹಂಚಿಕೆ.ಅಗಲ = requisition.ಅಗಲ;
	    ಹಂಚಿಕೆ.ಎತ್ತರ = requisition.ಎತ್ತರ;

	    gtk_widget_ಗಾತ್ರ_allocate(ಮಗು->widget, &ಹಂಚಿಕೆ);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಮಗು->mapped)
	{
	    ಮಗು->mapped = FALSE;

	    ಆದರೆ (gtk_widget_get_mapped(ಮಗು->widget))
		gtk_widget_unmap(ಮಗು->widget);
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
form_position_children(GtkForm *form)
{
    GList *tmp_list;

    ಗಾಗಿ (tmp_list = form->children; tmp_list; tmp_list = tmp_list->ಮುಂದಿನ)
	form_position_ಮಗು(form, tmp_list->ದತ್ತಾಂಶ, FALSE);
}

    ಶೂನ್ಯ
gui_gtk_form_move_resize(GtkForm *form, GtkWidget *widget,
		     gint x, gint y, gint w, gint h)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_widget_ಗಣ_ಗಾತ್ರ_request(widget, w, h);
#ಇಲ್ಲದಿದ್ದರೆ
    widget->requisition.ಅಗಲ  = w;
    widget->requisition.ಎತ್ತರ = h;
#ಆದರೆ_ಅಂತ್ಯ

    gui_gtk_form_move(form, widget, x, y);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
form_send_configure(GtkForm *form)
{
    GtkWidget *widget;
    GdkEventConfigure event;
    Gtk_ಹಂಚಿಕೆ ಹಂಚಿಕೆ;

    widget = GTK_WIDGET(form);

    gtk_widget_get_ಹಂಚಿಕೆ(widget, &ಹಂಚಿಕೆ);
    event.type = GDK_CONFIGURE;
    event.ಕಿಟಕಿ_2 = gtk_widget_get_ಕಿಟಕಿ(widget);
    event.x = ಹಂಚಿಕೆ.x;
    event.y = ಹಂಚಿಕೆ.y;
    event.ಅಗಲ = ಹಂಚಿಕೆ.ಅಗಲ;
    event.ಎತ್ತರ = ಹಂಚಿಕೆ.ಎತ್ತರ;

    gtk_main_do_event((GdkEvent*)&event);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
form_ಮಗು_map(GtkWidget *widget ಬಳಸದ, gpointer user_ದತ್ತಾಂಶ)
{
    Gtk_Form_ಮಗು_2 *ಮಗು;

    ಮಗು = (Gtk_Form_ಮಗು_2 *)user_ದತ್ತಾಂಶ;

    ಮಗು->mapped = TRUE;
    gdk_ಕಿಟಕಿ_show(ಮಗು->ಕಿಟಕಿ_2);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
form_ಮಗು_unmap(GtkWidget *widget ಬಳಸದ, gpointer user_ದತ್ತಾಂಶ)
{
    Gtk_Form_ಮಗು_2 *ಮಗು;

    ಮಗು = (Gtk_Form_ಮಗು_2 *)user_ದತ್ತಾಂಶ;

    ಮಗು->mapped = FALSE;
    gdk_ಕಿಟಕಿ_hide(ಮಗು->ಕಿಟಕಿ_2);
}
