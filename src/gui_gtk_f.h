/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ifndef __GTK_FORM_H__
#define __GTK_FORM_H__

#ifdef USE_GTK3
# include <gtk/gtk.h>
#وإلا
# include <gdk/gdk.h>
# include <gtk/gtkcontainer.h>
#endif


#ifdef __cplusplus
خارجي "C" {
#endif

#define GTK_TYPE_FORM		       (gui_gtk_form_get_type ())
#ifdef USE_GTK3
# define GTK_FORM(obj)		       (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_FORM))
#وإلا
# define GTK_FORM(obj)		       (GTK_CHECK_CAST ((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (GTK_CHECK_TYPE ((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_FORM))
#endif


تعريفنوع بنية _GtkForm GtkForm;
تعريفنوع بنية _GtkFormClass GtkFormClass;

بنية _GtkForm
{
    GtkContainer container;

    GList *children;
    GdkWindow *bin_window;
    gint freeze_تعداد;
};

بنية _GtkFormClass
{
    GtkContainerClass parent_class;
};

#ifdef USE_GTK3
GType gui_gtk_form_get_type(فراغ);
#وإلا
GtkType gui_gtk_form_get_type(فراغ);
#endif

GtkWidget *gui_gtk_form_new(فراغ);

فراغ gui_gtk_form_put(GtkForm * form, GtkWidget * widget, gint x, gint y);

فراغ gui_gtk_form_move(GtkForm *form, GtkWidget * widget, gint x, gint y);

فراغ gui_gtk_form_move_resize(GtkForm * form, GtkWidget * widget, gint x, gint y, gint w, gint h);

// These disable and enable moving and repainting respectively.  If you
// want to update the layout's offsets but do not want it to repaint
// itself, you should use these functions.

فراغ gui_gtk_form_freeze(GtkForm *form);
فراغ gui_gtk_form_thaw(GtkForm *form);


#ifdef __cplusplus
}
#endif
#endif	// __GTK_FORM_H__
