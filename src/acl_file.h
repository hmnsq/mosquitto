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

#અન્તર્ભાવ <uthash.h>

સંરચના acl__entry {
	સંરચના acl__entry *next, *prev;
	અક્ષર *topic;
	પૂર્ણાંક access;
	પૂર્ણાંક ucount;
	પૂર્ણાંક ccount;
};


સંરચના acl__user {
	UT_hash_handle hh;
	અક્ષર *username;
	સંરચના acl__entry *acl;
};


સંરચના acl_file_data {
	અક્ષર *acl_file;
	સંરચના acl__user *acl_users;
	સંરચના acl__user acl_anon;
	સંરચના acl__entry *acl_patterns;
};


પૂર્ણાંક acl_file__parse(સંરચના acl_file_data *data);
પૂર્ણાંક acl_file__check(પૂર્ણાંક event, શૂન્ય *event_data, શૂન્ય *userdata);
પૂર્ણાંક acl_file__reload(પૂર્ણાંક event, શૂન્ય *event_data, શૂન્ય *userdata);
શૂન્ય acl_file__cleanup(સંરચના acl_file_data *data);

#endif
