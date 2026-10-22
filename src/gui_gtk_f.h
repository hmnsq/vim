/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#جيڪڏهن_وصف_نه_ٿيل __GTK_FORM_H__
#وصف_ڪريو __GTK_FORM_H__

#جيڪڏهن_وصف_ٿيل USE_GTK3
# include <gtk/gtk.h>
#وإلا
# include <gdk/gdk.h>
# include <gtk/gtkcontainer.h>
#جيڪڏهن_پورو


#جيڪڏهن_وصف_ٿيل __cplusplus
خارجي "C" {
#جيڪڏهن_پورو

#وصف_ڪريو GTK_TYPE_FORM		       (gui_gtk_form_get_type ())
#جيڪڏهن_وصف_ٿيل USE_GTK3
# define GTK_FORM(obj)		       (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_FORM))
#وإلا
# define GTK_FORM(obj)		       (GTK_CHECK_CAST ((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (GTK_CHECK_TYPE ((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_FORM))
#جيڪڏهن_پورو


تعريفنوع بنية _GtkForm GtkForm;
تعريفنوع بنية _GtkFormClass GtkFormClass;

بنية _GtkForm
{
    GtkContainer container;

    GList *children;
    Gdk_دري *bin_دري;
    gint freeze_ڳڻپ;
};

بنية _GtkFormClass
{
    GtkContainerClass والد_class;
};

#جيڪڏهن_وصف_ٿيل USE_GTK3
GType gui_gtk_form_get_type(فراغ);
#وإلا
GtkType gui_gtk_form_get_type(فراغ);
#جيڪڏهن_پورو

GtkWidget *gui_gtk_form_new(فراغ);

فراغ gui_gtk_form_put(GtkForm * form, GtkWidget * widget, gint x, gint y);

فراغ gui_gtk_form_move(GtkForm *form, GtkWidget * widget, gint x, gint y);

فراغ gui_gtk_form_move_resize(GtkForm * form, GtkWidget * widget, gint x, gint y, gint w, gint h);

// These disable and enable moving and repainting respectively.  If you
// want to update the layout's offsets but do not want it to repaint
// itself, you should use these functions.

فراغ gui_gtk_form_freeze(GtkForm *form);
فراغ gui_gtk_form_thaw(GtkForm *form);


#جيڪڏهن_وصف_ٿيل __cplusplus
}
#جيڪڏهن_پورو
#جيڪڏهن_پورو	// __GTK_FORM_H__
