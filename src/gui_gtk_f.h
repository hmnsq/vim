/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __GTK_FORM_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __GTK_FORM_H__

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK3
# include <gtk/gtk.h>
#ಇಲ್ಲದಿದ್ದರೆ
# include <gdk/gdk.h>
# include <gtk/gtkcontainer.h>
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
ಬಾಹ್ಯ "C" {
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು GTK_TYPE_FORM		       (gui_gtk_form_get_type ())
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK3
# define GTK_FORM(obj)		       (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), GTK_TYPE_FORM))
#ಇಲ್ಲದಿದ್ದರೆ
# define GTK_FORM(obj)		       (GTK_CHECK_CAST ((obj), GTK_TYPE_FORM, GtkForm))
# define GTK_FORM_CLASS(klass)	       (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_FORM, GtkFormClass))
# define GTK_IS_FORM(obj)	       (GTK_CHECK_TYPE ((obj), GTK_TYPE_FORM))
# define GTK_IS_FORM_CLASS(klass)       (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_FORM))
#ಆದರೆ_ಅಂತ್ಯ


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _GtkForm GtkForm;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _GtkFormClass GtkFormClass;

ರಚನೆ _GtkForm
{
    GtkContainer container;

    GList *children;
    Gdk_ಕಿಟಕಿ *bin_ಕಿಟಕಿ;
    gint freeze_ಎಣಿಕೆ;
};

ರಚನೆ _GtkFormClass
{
    GtkContainerClass ಪೋಷಕ_class;
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GTK3
GType gui_gtk_form_get_type(ಶೂನ್ಯ);
#ಇಲ್ಲದಿದ್ದರೆ
GtkType gui_gtk_form_get_type(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

GtkWidget *gui_gtk_form_new(ಶೂನ್ಯ);

ಶೂನ್ಯ gui_gtk_form_put(GtkForm * form, GtkWidget * widget, gint x, gint y);

ಶೂನ್ಯ gui_gtk_form_move(GtkForm *form, GtkWidget * widget, gint x, gint y);

ಶೂನ್ಯ gui_gtk_form_move_resize(GtkForm * form, GtkWidget * widget, gint x, gint y, gint w, gint h);

// These disable and enable moving and repainting respectively.  If you
// want to update the layout's offsets but do not want it to repaint
// itself, you should use these functions.

ಶೂನ್ಯ gui_gtk_form_freeze(GtkForm *form);
ಶೂನ್ಯ gui_gtk_form_thaw(GtkForm *form);


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ	// __GTK_FORM_H__
