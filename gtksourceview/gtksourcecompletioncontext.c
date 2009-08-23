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
	FINISHED,
	LAST_SIGNAL
};

typedef struct
{
	GtkSourceCompletionProvider 	*provider;
	GList				*proposals;
	gboolean			 needs_update;
}ProviderInfo;

struct _GtkSourceCompletionContextPrivate
{
	GtkSourceCompletionModel 	*model;
	GtkTextView			*view;
	GtkTextIter 			 iter;
	gchar 				*criteria;
	GHashTable			*pinfo_table;
	gboolean			 invalidated;
	GtkSourceCompletionProvider	*filter_provider;
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (GtkSourceCompletionContext, gtk_source_completion_context, G_TYPE_OBJECT);

static void
free_proposals_list (GList *list)
{
	g_list_foreach (list, (GFunc)g_object_unref, NULL);
	g_list_free (list);
}

static void
clear_table_entry (GtkSourceCompletionProvider *provider,
		   ProviderInfo *pinfo,
		   GtkSourceCompletionContext *context)
{
	free_proposals_list (pinfo->proposals);
	pinfo->proposals = NULL;
}

static void
free_table_entry (GtkSourceCompletionProvider *provider,
		  ProviderInfo *pinfo,
		  GtkSourceCompletionContext *context)
{
	clear_table_entry (provider, pinfo, context);
	g_object_unref (pinfo->provider);
	g_slice_free (ProviderInfo, pinfo);
}

static void
context_clear (GtkSourceCompletionContext	*context)
{
	g_hash_table_foreach (context->priv->pinfo_table, (GHFunc)clear_table_entry, context);
}

static void
gtk_source_completion_context_class_init (GtkSourceCompletionContextClass *klass)
{
	GObjectClass *gobject_class = (GObjectClass *)klass;
	
	g_type_class_add_private (klass, sizeof (GtkSourceCompletionContextPrivate));

	gobject_class->finalize = gtk_source_completion_context_finalize;

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
	
	self->priv->pinfo_table = g_hash_table_new (g_direct_hash,
						    g_direct_equal);
	self->priv->criteria = NULL;
	self->priv->invalidated = FALSE;
	self->priv->filter_provider = NULL;

}

static void
gtk_source_completion_context_finalize (GObject *object)
{
	GtkSourceCompletionContext *self = (GtkSourceCompletionContext *)object;

	g_free (self->priv->criteria);

	g_hash_table_foreach (self->priv->pinfo_table, (GHFunc)free_table_entry, self);
	g_hash_table_destroy (self->priv->pinfo_table);

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
gtk_source_completion_context_new (GtkSourceCompletionModel	*model,
				   GtkTextView			*view,
				   GList 			*providers)
{
	GList *l;
	GtkSourceCompletionContext *context;
	ProviderInfo *pinfo;

	g_return_val_if_fail (providers != NULL, NULL);
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_MODEL (model), NULL);
	g_return_val_if_fail (GTK_IS_TEXT_VIEW (view), NULL);
	
	context = g_object_new (GTK_TYPE_SOURCE_COMPLETION_CONTEXT, NULL);
	context->priv->model = model;
	context->priv->view = view;
	for (l = providers; l != NULL; l = g_list_next (l))
	{
		pinfo = g_slice_new (ProviderInfo);
		pinfo->provider = (GtkSourceCompletionProvider*)g_object_ref(l->data);
		pinfo->proposals = NULL;
		pinfo->needs_update = TRUE;
		g_hash_table_insert (context->priv->pinfo_table, pinfo->provider, pinfo);
	}
	update_criteria (context);
	return context;
}

void
gtk_source_completion_context_add_proposals (GtkSourceCompletionContext		*context,
					     GtkSourceCompletionProvider	*provider,
					     GList	 			*proposals)
{
	ProviderInfo *pinfo;
	GList *item;
	GtkSourceCompletionProposal *proposal;

	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context));
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_PROVIDER (provider));
	g_return_if_fail (!context->priv->invalidated);

	pinfo = (ProviderInfo*)g_hash_table_lookup (context->priv->pinfo_table, provider);

	/*The provider is not in the providers list*/
	g_return_if_fail (pinfo);

	g_list_foreach (proposals, (GFunc)g_object_ref, NULL);

	

	if (pinfo->needs_update)
	{
		free_proposals_list (pinfo->proposals);
		pinfo->proposals = g_list_copy (proposals);
		pinfo->needs_update = FALSE;
		gtk_source_completion_model_set_proposals (context->priv->model,
							   provider,
							   pinfo->proposals);
	}
	else
	{
		if (pinfo->proposals != NULL)
		{
			pinfo->proposals = g_list_concat (pinfo->proposals, g_list_copy (proposals));
		}
		else
		{
			pinfo->proposals = g_list_copy (proposals);
		}

		for (item = proposals; item; item = g_list_next (item))
		{
			if (GTK_IS_SOURCE_COMPLETION_PROPOSAL (item->data))
			{
				proposal = GTK_SOURCE_COMPLETION_PROPOSAL (item->data);
				gtk_source_completion_model_append (context->priv->model,
								    provider,
								    proposal);
			}
		}
	}
	
	gtk_source_completion_model_run_add_proposals (context->priv->model);

	g_list_free (proposals);
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
 * Returns: The content of the list is owned by the context and should not be
 * modified or freed. Use g_list_free() when done using the list.
 **/
GList*
gtk_source_completion_context_get_providers (GtkSourceCompletionContext *context)
{
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context), NULL);
	return g_hash_table_get_keys (context->priv->pinfo_table);
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
	ProviderInfo *pinfo;
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context), NULL);
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_PROVIDER (provider), NULL);
	
	pinfo = (ProviderInfo*)g_hash_table_lookup (context->priv->pinfo_table, provider);

	return pinfo->proposals;
}

gboolean
gtk_source_completion_context_is_valid (GtkSourceCompletionContext	*context)
{
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context), FALSE);
	
	return !context->priv->invalidated;
}

void
gtk_source_completion_context_update (GtkSourceCompletionContext	*context)
{
	ProviderInfo *pinfo;
	GList *pinfo_list;
	GList *l;
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context));

	pinfo_list = g_hash_table_get_values (context->priv->pinfo_table);
	for (l = pinfo_list; l != NULL; l = g_list_next (l))
	{
		pinfo = (ProviderInfo*)l->data;
		pinfo->needs_update = TRUE;
	}
	g_list_free (pinfo_list);
	
	context_clear (context);
	update_criteria (context);

	gtk_source_completion_model_cancel_add_proposals (context->priv->model);
}

void
gtk_source_completion_context_set_filter_provider (GtkSourceCompletionContext	*context,
						   GtkSourceCompletionProvider	*provider)
{
	GList *proposals;
	GList *providers;
	GList *current;
	
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context));
	//TODO Check if the provider is in the list of providers

	if (context->priv->filter_provider == provider)
		return;

	gtk_source_completion_model_clear (context->priv->model);
	
	context->priv->filter_provider = provider;

	if (provider != NULL)
	{
		providers = g_list_append (NULL, provider);
	}
	else
	{
		providers = gtk_source_completion_context_get_providers(context);
	}

	for (current = providers; current != NULL; current = g_list_next (current))
	{
		provider = GTK_SOURCE_COMPLETION_PROVIDER (current->data);
		proposals = gtk_source_completion_context_get_proposals (context,
									 provider);
			
		for (;proposals != NULL; proposals = g_list_next (proposals))
		{
			gtk_source_completion_model_append (context->priv->model,
							    provider,
							    GTK_SOURCE_COMPLETION_PROPOSAL (proposals->data));
		}
	}

	g_list_free (providers);

	/*TODO Hide the completion if it is empty
	if (gtk_source_completion_model_is_empty (completion->priv->model_proposals, FALSE))
	{
	}
	*/
	      
	gtk_source_completion_model_run_add_proposals (context->priv->model);
}

GtkSourceCompletionProvider*
gtk_source_completion_context_get_filter_provider (GtkSourceCompletionContext *context)
{
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_CONTEXT (context), NULL);

	return context->priv->filter_provider;
}
