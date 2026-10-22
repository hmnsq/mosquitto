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
#اگر_غیر_تعریف_شدہ ACL_FILE_H
#تعریف_کرو ACL_FILE_H

#شامل <uthash.h>

ساخت acl_اندراج {
	ساخت acl_اندراج *اگلا, *پچھلا;
	حرف *topic;
	صحیح access;
	صحیح ucount;
	صحیح ccount;
};


ساخت acl__user {
	UT_hash_handle hh;
	حرف *username;
	ساخت acl_اندراج *acl;
};


ساخت acl_file_ڈیٹا {
	حرف *acl_file;
	ساخت acl__user *acl_users;
	ساخت acl__user acl_anon;
	ساخت acl_اندراج *acl_patterns;
};


صحیح acl_file__parse(ساخت acl_file_ڈیٹا *ڈیٹا);
صحیح acl_file__check(صحیح event, خالی *event_ڈیٹا, خالی *userdata);
صحیح acl_file__reload(صحیح event, خالی *event_ڈیٹا, خالی *userdata);
خالی acl_file__cleanup(ساخت acl_file_ڈیٹا *ڈیٹا);

#اگر_ختم
