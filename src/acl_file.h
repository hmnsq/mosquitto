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
#ifndef ACL_FILE_H
#define ACL_FILE_H

#अन्तर्भाव <uthash.h>

संरचना acl__entry {
	संरचना acl__entry *next, *prev;
	वर्ण *topic;
	पूर्णाङ्क access;
	पूर्णाङ्क ucount;
	पूर्णाङ्क ccount;
};


संरचना acl__user {
	UT_hash_handle hh;
	वर्ण *username;
	संरचना acl__entry *acl;
};


संरचना acl_file_data {
	वर्ण *acl_file;
	संरचना acl__user *acl_users;
	संरचना acl__user acl_anon;
	संरचना acl__entry *acl_patterns;
};


पूर्णाङ्क acl_file__parse(संरचना acl_file_data *data);
पूर्णाङ्क acl_file__check(पूर्णाङ्क event, शून्य *event_data, शून्य *userdata);
पूर्णाङ्क acl_file__reload(पूर्णाङ्क event, शून्य *event_data, शून्य *userdata);
शून्य acl_file__cleanup(संरचना acl_file_data *data);

#endif
