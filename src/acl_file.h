/*
Copyright (c) 2011-2021 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
*/
#अगर_अघोषित ACL_FILE_H
#घोषणा ACL_FILE_H

#समावेश <uthash.h>

काष्ठा acl__entry {
	काष्ठा acl__entry *अग्र, *पूर्व;
	अक्षर *topic;
	पूर्णांक access;
	पूर्णांक ucount;
	पूर्णांक ccount;
};


काष्ठा acl__user {
	UT_hash_handle hh;
	अक्षर *username;
	काष्ठा acl__entry *acl;
};


काष्ठा acl_file_तथ्य {
	अक्षर *acl_file;
	काष्ठा acl__user *acl_users;
	काष्ठा acl__user acl_anon;
	काष्ठा acl__entry *acl_patterns;
};


पूर्णांक acl_file__parse(काष्ठा acl_file_तथ्य *तथ्य);
पूर्णांक acl_file__check(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata);
पूर्णांक acl_file__reload(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata);
व्योम acl_file__cleanup(काष्ठा acl_file_तथ्य *तथ्य);

#पूर्ण_अगर
