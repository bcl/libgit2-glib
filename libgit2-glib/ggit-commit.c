/*
 * ggit-commit.c
 * This file is part of libgit2-glib
 *
 * Copyright (C) 2011 - Ignacio Casal Quinteiro
 *
 * libgit2-glib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * libgit2-glib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libgit2-glib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <git2/errors.h>

#include "ggit-commit.h"
#include "ggit-signature.h"
#include "ggit-oid.h"
#include "ggit-convert.h"

G_DEFINE_TYPE (GgitCommit, ggit_commit, GGIT_TYPE_OBJECT)

static void
ggit_commit_class_init (GgitCommitClass *klass)
{
}

static void
ggit_commit_init (GgitCommit *self)
{
	self->priv = GGIT_COMMIT_GET_PRIVATE (self);
}

GgitCommit *
_ggit_commit_wrap (git_commit *commit,
                   gboolean    owned)
{
	GgitCommit *gcommit;

	gcommit = g_object_new (GGIT_TYPE_COMMIT,
	                        "native", commit,
	                        NULL);

	if (owned)
	{
		_ggit_native_set_destroy_func (gcommit,
		                               (GDestroyNotify)git_object_free);
	}

	return gcommit;
}

/**
 * ggit_commit_get_message_encoding:
 * @commit: a #GgitCommit.
 *
 * Get the encoding for the message of a commit,
 * as a string representing a standard encoding name.
 *
 * The encoding may be %NULL if the 'encoding' header
 * in the commit is missing; in that case UTF-8 is assumed.
 *
 * Returns: the encoding of the commit message or %NULL.
 */
const gchar *
ggit_commit_get_message_encoding (GgitCommit *commit)
{
	git_commit *c;

	g_return_val_if_fail (GGIT_IS_COMMIT (commit), NULL);

	c = _ggit_native_get (commit);

	return git_commit_message_encoding (c);
}

/**
 * ggit_commit_get_message:
 * @commit: a #GgitCommit.
 *
 * Gets the full message of @commit.
 *
 * Returns: the message of the commit.
 */
const gchar *
ggit_commit_get_message (GgitCommit *commit)
{
	git_commit *c;

	g_return_val_if_fail (GGIT_IS_COMMIT (commit), NULL);

	c = _ggit_native_get (commit);

	return git_commit_message (c);
}

/* TODO: use gdatetime */
/**
 * ggit_commit_get_time:
 * @commit: a #GgitCommit.
 *
 * Gets the commit time (i.e. committer time) of @commit.
 *
 * Returns: the time of the commit.
 */
gint64
ggit_commit_get_time (GgitCommit *commit)
{
	git_commit *c;

	g_return_val_if_fail (GGIT_IS_COMMIT (commit), 0);

	c = _ggit_native_get (commit);

	return git_commit_time (c);
}

/**
 * ggit_commit_get_time_offset:
 * @commit: a #GgitCommit.
 *
 * Gets the commit timezone offset (i.e. committer's preferred timezone)
 * of @commit.
 *
 * Returns: positive or negative timezone offset, in minutes from UTC.
 */
gint
ggit_commit_get_time_offset (GgitCommit *commit)
{
	git_commit *c;

	g_return_val_if_fail (GGIT_IS_COMMIT (commit), 0);

	c = _ggit_native_get (commit);

	return git_commit_time_offset (c);
}

/**
 * ggit_commit_get_committer:
 * @commit: a #GgitCommit.
 *
 * Gets the committer of @commit. The returned value must be free'd with
 * ggit_signature_free().
 *
 * Returns: (transfer full): the committer of the commit.
 */
GgitSignature *
ggit_commit_get_committer (GgitCommit *commit)
{
	git_commit *c;
	const git_signature *signature;

	g_return_val_if_fail (GGIT_IS_COMMIT (commit), NULL);

	c = _ggit_native_get (commit);
	signature = git_commit_committer (c);

	return _ggit_signature_wrap ((git_signature *)signature);
}

/**
 * ggit_commit_get_author:
 * @commit: a #GgitCommit.
 *
 * Gets the author of @commit. The returned value must be free'd with
 * ggit_signature_free().
 *
 * Returns: (transfer full): the author of the commit.
 */
GgitSignature *
ggit_commit_get_author (GgitCommit *commit)
{
	git_commit *c;
	const git_signature *signature;

	g_return_val_if_fail (GGIT_IS_COMMIT (commit), NULL);

	c = _ggit_native_get (commit);
	signature = git_commit_author (c);

	return _ggit_signature_wrap ((git_signature *)signature);
}

/**
 * ggit_commit_get_parents:
 * @commit: a #GgitCommit.
 *
 * Gets the parents for @commit.
 *
 * Returns: (transfer full) (element-type Ggit.Commit):
 * the parents of the commit.
 */
GList *
ggit_commit_get_parents (GgitCommit *commit)
{
	GList *parents = NULL;
	git_commit *c;
	gint num_parents, i;

	g_return_val_if_fail (GGIT_IS_COMMIT (commit), NULL);


	num_parents = git_commit_parentcount (c);
	c = _ggit_native_get (commit);

	for (i = num_parents - 1; i >= 0; --i)
	{
		git_commit *p;

		if (git_commit_parent (&p, c, i) == GIT_SUCCESS)
		{
			parents = g_list_prepend (parents, _ggit_commit_new (p));
		}
	}

	return parents;
}

/* ex:set ts=8 noet: */
