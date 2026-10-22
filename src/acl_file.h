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

#অন্তর্ভাব <uthash.h>

কাঠামো acl__entry {
	কাঠামো acl__entry *পরবর্তী, *পূর্ব;
	অক্ষর *topic;
	পূর্ণসংখ্যা access;
	পূর্ণসংখ্যা ucount;
	পূর্ণসংখ্যা ccount;
};


কাঠামো acl__user {
	UT_hash_handle hh;
	অক্ষর *username;
	কাঠামো acl__entry *acl;
};


কাঠামো acl_file_তথ্য {
	অক্ষর *acl_file;
	কাঠামো acl__user *acl_users;
	কাঠামো acl__user acl_anon;
	কাঠামো acl__entry *acl_patterns;
};


পূর্ণসংখ্যা acl_file__parse(কাঠামো acl_file_তথ্য *তথ্য);
পূর্ণসংখ্যা acl_file__check(পূর্ণসংখ্যা event, শূন্য *event_তথ্য, শূন্য *userdata);
পূর্ণসংখ্যা acl_file__reload(পূর্ণসংখ্যা event, শূন্য *event_তথ্য, শূন্য *userdata);
শূন্য acl_file__cleanup(কাঠামো acl_file_তথ্য *তথ্য);

#endif
