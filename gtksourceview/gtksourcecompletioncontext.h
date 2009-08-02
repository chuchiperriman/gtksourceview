/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*- 
 * gtksourcecompletioncontext.h
 * 
 * Copyright  (C)  2009  Jesús Barbero Rodríguez <chuchiperriman@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#ifndef GTK_SOURCE_COMPLETION_CONTEXT_H
#define GTK_SOURCE_COMPLETION_CONTEXT_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_TYPE_SOURCE_COMPLETION_CONTEXT                              \
   (gtk_source_completion_context_get_type())
#define GTK_SOURCE_COMPLETION_CONTEXT(obj)                              \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                GTK_TYPE_SOURCE_COMPLETION_CONTEXT,     \
                                GtkSourceCompletionContext))
#define GTK_SOURCE_COMPLETION_CONTEXT_CLASS(klass)                      \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             GTK_TYPE_SOURCE_COMPLETION_CONTEXT,        \
                             GtkSourceCompletionContextClass))
#define GTK_IS_SOURCE_COMPLETION_CONTEXT(obj)                           \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                GTK_TYPE_SOURCE_COMPLETION_CONTEXT))
#define GTK_IS_SOURCE_COMPLETION_CONTEXT_CLASS(klass)                   \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             GTK_TYPE_SOURCE_COMPLETION_CONTEXT))
#define GTK_SOURCE_COMPLETION_CONTEXT_GET_CLASS(obj)                    \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               GTK_TYPE_SOURCE_COMPLETION_CONTEXT,      \
                               GtkSourceCompletionContextClass))

typedef struct _GtkSourceCompletionContextPrivate GtkSourceCompletionContextPrivate;
typedef struct _GtkSourceCompletionContext      GtkSourceCompletionContext;
typedef struct _GtkSourceCompletionContextClass GtkSourceCompletionContextClass;

#include <gtksourceview/gtksourcecompletionprovider.h>

struct _GtkSourceCompletionContextClass
{
	GObjectClass parent_class;
};

struct _GtkSourceCompletionContext
{
	GObject parent;
	GtkSourceCompletionContextPrivate *priv;
};

GType	 			 gtk_source_completion_context_get_type		(void) G_GNUC_CONST;

GtkSourceCompletionContext	*gtk_source_completion_context_new		(GtkTextView	*view);

void				 gtk_source_completion_context_add_proposals	(GtkSourceCompletionContext	*context,
										 GtkSourceCompletionProvider	*provider,
										 GList	 			*proposals);

void				 gtk_source_completion_context_finish		(GtkSourceCompletionContext	*context);

GtkTextView			*gtk_source_completion_context_get_view		(GtkSourceCompletionContext	*context);

void				 gtk_source_completion_context_get_iter		(GtkSourceCompletionContext	*context,
										 GtkTextIter *iter);

gchar				*gtk_source_completion_context_get_criteria	(GtkSourceCompletionContext	*context);

GList				*gtk_source_completion_context_get_proposals	(GtkSourceCompletionContext	*context,
										 GtkSourceCompletionProvider	*provider);

G_END_DECLS

#endif
