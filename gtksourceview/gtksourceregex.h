/*  gtksourceregex.h
 *
 *  Copyright (C) 2003 - Paolo Maggi <paolo.maggi@polito.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __GTK_SOURCE_REGEX_H__
#define __GTK_SOURCE_REGEX_H__

#include <regex.h>
#include <glib/gtypes.h>

G_BEGIN_DECLS

typedef struct _GtkSourceBufferMatch     GtkSourceBufferMatch;
typedef struct _GtkSourceRegex           GtkSourceRegex;

struct _GtkSourceBufferMatch
{
	gint 			 startpos;
	gint			 endpos;
	
	gint 			 startindex;
	gint			 endindex;
};

struct _GtkSourceRegex 
{
	struct re_pattern_buffer buf;
	struct re_registers 	 reg;
	gint			 len;
};

gboolean 	gtk_source_regex_compile 	(GtkSourceRegex       *regex,
						 const gchar          *pattern);

void		gtk_source_regex_destroy	(GtkSourceRegex       *regex);

gint		gtk_source_regex_search 	(GtkSourceRegex       *regex,
			 			 const gchar          *text,
			 			 gint                  pos,
						 gint                  length,
			 			 GtkSourceBufferMatch *match);

gint		gtk_source_regex_match 		(GtkSourceRegex       *regex,
						 const gchar          *text, 
						 gint                  len, 
						 gint                  pos);

G_END_DECLS

#endif