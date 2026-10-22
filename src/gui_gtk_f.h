/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __GTK_FORM_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __GTK_FORM_H__

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK3
# include <gtk/gtk.h>
#ਨਹੀਂਤਾਂ
# include <gdk/gdk.h>
# include <gtk/gtkcontainer.h>
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
ਬਾਹਰੀ "C" {
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GTK_TYPE_FORM		       (gui_gtk_form_get_type ())
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK3
# define GTK_FORM(obj)		       (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_FORM))
#ਨਹੀਂਤਾਂ
# define GTK_FORM(obj)		       (GTK_CHECK_CAST ((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (GTK_CHECK_TYPE ((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_FORM))
#ਜੇ_ਅੰਤ


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _GtkForm GtkForm;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _GtkFormClass GtkFormClass;

ਬਣਤਰ _GtkForm
{
    GtkContainer container;

    GList *children;
    Gdk_ਖਿੜਕੀ *bin_ਖਿੜਕੀ;
    gint freeze_ਗਿਣਤੀ;
};

ਬਣਤਰ _GtkFormClass
{
    GtkContainerClass ਮਾਪਾ_class;
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GTK3
GType gui_gtk_form_get_type(ਖਾਲੀ);
#ਨਹੀਂਤਾਂ
GtkType gui_gtk_form_get_type(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

GtkWidget *gui_gtk_form_new(ਖਾਲੀ);

ਖਾਲੀ gui_gtk_form_put(GtkForm * form, GtkWidget * widget, gint x, gint y);

ਖਾਲੀ gui_gtk_form_move(GtkForm *form, GtkWidget * widget, gint x, gint y);

ਖਾਲੀ gui_gtk_form_move_resize(GtkForm * form, GtkWidget * widget, gint x, gint y, gint w, gint h);

// These disable and enable moving and repainting respectively.  If you
// want to update the layout's offsets but do not want it to repaint
// itself, you should use these functions.

ਖਾਲੀ gui_gtk_form_freeze(GtkForm *form);
ਖਾਲੀ gui_gtk_form_thaw(GtkForm *form);


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
}
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ	// __GTK_FORM_H__
