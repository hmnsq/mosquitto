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
#ifndef PASSWORD_FILE_H
#define PASSWORD_FILE_H

#অন্তর্ভাব <uthash.h>

কাঠামো mosquitto__unpwd {
	UT_hash_handle hh;
	অক্ষর *username;
	অক্ষর *clientid;
	কাঠামো mosquitto_pw *pw;
};

কাঠামো password_file_তথ্য {
	কাঠামো mosquitto__unpwd *unpwd;
	অক্ষর *password_file;
};

পূর্ণসংখ্যা password_file__parse(কাঠামো password_file_তথ্য *তথ্য);
পূর্ণসংখ্যা password_file__check(পূর্ণসংখ্যা event, শূন্য *event_তথ্য, শূন্য *userdata);
পূর্ণসংখ্যা password_file__reload(পূর্ণসংখ্যা event, শূন্য *event_তথ্য, শূন্য *userdata);
শূন্য password_file__cleanup(কাঠামো password_file_তথ্য *তথ্য);

#endif
