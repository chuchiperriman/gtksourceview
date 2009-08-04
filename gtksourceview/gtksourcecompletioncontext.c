/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*- 
 * gtksourcecompletioncontext.c
 * 
 * Copyright  (C)  2009  Jesús Barbero Rodríguez <chuchiperriman@gmail.com>
 * 
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

#include "gtksourcecompletioncontext.h"
#include "gtksourcecompletionutils.h"
#include "gtksourceview-marshal.h"

#define GTK_SOURCE_COMPLETION_CONTEXT_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), \
										       GTK_TYPE_SOURCE_COMPLETION_CONTEXT, \
										       GtkSourceCompletionContextPrivate))

static void gtk_source_completion_context_finalize (GObject *object);

/* Signals */
enum
{
	PROPOSALS_ADDED,
	FINISHED,
	LAST_SIGNAL
};

struct _GtkSourceCompletionContextPrivate
{
	GtkTextView	*view;
	GtkTextIter 	iter;
	gchar 		*criteria;
	GList		*providers;
	GHashTable	*proposals_table;
	gboolean	invalidated;
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (GtkSourceCompletionContext, gtk_source_completion_context, G_TYPE_OBJECT);

static void
free_table_entry (GtkSourceCompletionProvider *provider,
		  GList *proposals,
		  GtkSourceCompletionContext *context)
{
	g_object_unref (provider);
	g_list_foreach (proposals, (GFunc)g_object_unref, NULL);
	g_list_free (proposals);
}

static void
clear_table_entry (GtkSourceCompletionProvider *provider,
		   GList *proposals,
		   GtkSourceCompletionContext *context)
{
	g_list_foreach (proposals, (GFunc)g_object_unref, NULL);
	g_list_free (proposals);
	g_hash_table_insert (context->priv->proposals_table, provider, NULL);
}

static void
context_clear (GtkSourceCompletionContext	*context)
{
	g_hash_table_foreach (context->priv->proposals_table, (GHFunc)clear_table_entry, context);
}

static void
gtk_source_completion_context_class_init (GtkSourceCompletionContextClass *klass)
{
	GObjectClass *gobject_class = (GObjectClass *)klass;
	
	g_type_class_add_private (klass, sizeof (GtkSourceCompletionContextPrivate));

	gobject_class->finalize = gtk_source_completion_context_finalize;

	signals[PROPOSALS_ADDED] =
		g_signal_new ("proposals-added",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      0,
			      NULL, 
			      NULL,
			      _gtksourceview_marshal_VOID__OBJECT_POINTER, 
			      G_TYPE_NONE,
			      2,
			      G_TYPE_OBJECT,
			      G_TYPE_POINTER);
	signals[FINISHED] =
		g_signal_new ("finished",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      0,
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE,
			      0);
}

static void
gtk_source_completion_context_init (GtkSourceCompletionContext *self)
{
	self->priv = GTK_SOURCE_COMPLETION_CONTEXT_GET_PRIVATE (self);
	
	self->priv->proposals_table = g_hash_table_new (g_direct_hash,
							g_direct_equal);
	self->priv->criteria = NULL;
	self->priv->invalidated = FALSE;

	g_debug ("context init");
}

static void
gtk_source_completion_context_finalize (GObject *object)
{
	GtkSourceCompletionContext *self = (GtkSourceCompletionContext *)object;

	g_debug ("context finalize");
	
	g_free (self->priv->criteria);

	g_hash_table_foreach (self->priv->proposals_table, (GHFunc)free_table_entry, self);
	g_hash_table_destroy (self->priv->proposals_table);

	g_signal_handlers_destroy (object);
	G_OBJECT_CLASS (gtk_source_completion_context_parent_class)->finalize (object);
}

static void
update_criteria (GtkSourceCompletionContext	*context)
{
	GtkTextBuffer 	*buffer;

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (context->priv->view));
	gtk_text_buffer_get_iter_at_mark (buffer,
	                                  &(context->priv->iter),
	                                  gtk_text_buffer_get_insert (buffer));
	
	g_free (context->priv->criteria);
        context->priv->criteria =
                gtk_source_completion_utils_get_word (GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (context->priv->view)));
}

GtkSourceCompletionContext*
gtk_source_completion_context_new (GtkTextView	*view,
				   GList *providers)
{
	GtkSourceCompletionContext *context = g_object_new (GTK_TYPE_SOURCE_COMPLETION_CONTEXT, NULL);

	context->priv->view = view;
	context->priv->providers = g_list_copy (providers);
	update_criteria (context);
	return context;
}

void
gtk_source_completion_context_add_proposals (GtkSourceCompletionContext		*context,
					     GtkSourceCompletionProvider	*provider,
					     GList	 			*proposals)
{
	GList *cached;

	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context));
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_PROVIDER (provider));
	g_return_if_fail (proposals != NULL);
	g_return_if_fail (!context->priv->invalidated);

	//TODO Check if the provider is in the providers list

	g_list_foreach (proposals, (GFunc)g_object_ref, NULL);

	cached = g_hash_table_lookup (context->priv->proposals_table, provider);

	if (cached != NULL)
	{
		cached = g_list_concat (cached, g_list_copy (proposals));
	}
	else
	{
		g_object_ref (provider);
		cached = g_list_copy (proposals);
	}

	g_hash_table_insert (context->priv->proposals_table, provider, cached);
	
	g_signal_emit_by_name (context, "proposals-added", provider, proposals);
}

void
gtk_source_completion_context_finish (GtkSourceCompletionContext	*context)
{
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context));
	g_return_if_fail (!context->priv->invalidated);

	/*TODO We must invalidate the context*/
	g_signal_emit (context, signals[FINISHED], 0);
	context->priv->invalidated = TRUE;
}

GtkTextView*
gtk_source_completion_context_get_view (GtkSourceCompletionContext	*context)
{
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT(context), NULL);

	return context->priv->view;
}

void
gtk_source_completion_context_get_iter (GtkSourceCompletionContext	*context,
					GtkTextIter *iter)
{
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT(context));

	*iter = context->priv->iter;
}

/**
 * gtk_source_completion_context_get_criteria:
 * @context: 
 *
 * TODO
 *
 * Returns: An internal utf-8 encoded string. You must not free it.
 **/
gchar*
gtk_source_completion_context_get_criteria (GtkSourceCompletionContext	*context)
{
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT(context), NULL);

	return context->priv->criteria;
}

/**
 * gtk_source_completion_context_get_providers:
 * @context: 
 *
 * 
 *
 * Returns: The internal list of providers for this context
 **/
GList*
gtk_source_completion_context_get_providers (GtkSourceCompletionContext *context)
{

	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context), NULL);
	
	return context->priv->providers;
}

/**
 * gtk_source_completion_context_get_proposals:
 * @context: 
 * @provider: 
 *
 * 
 *
 * Returns: The internal list of proposals for that provider. Do not modify it.
 **/
GList*
gtk_source_completion_context_get_proposals (GtkSourceCompletionContext		*context,
					     GtkSourceCompletionProvider	*provider)
{
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context), NULL);
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_PROVIDER (provider), NULL);
	
	return g_hash_table_lookup (context->priv->proposals_table, provider);
}

gboolean
gtk_source_completion_context_is_valid (GtkSourceCompletionContext	*context)
{
	return !context->priv->invalidated;
}

void
gtk_source_completion_context_update (GtkSourceCompletionContext	*context)
{
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context));
	//TODO Clear the current proposals
	context_clear (context);
	update_criteria (context);
}
