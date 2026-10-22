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

#অন্তর্ভাৱ <uthash.h>

সংরচনা mosquitto__unpwd {
	UT_hash_handle hh;
	ৱর্ণ *username;
	ৱর্ণ *clientid;
	সংরচনা mosquitto_pw *pw;
};

সংরচনা password_file_data {
	সংরচনা mosquitto__unpwd *unpwd;
	ৱর্ণ *password_file;
};

পূর্ণাঙ্ক password_file__parse(সংরচনা password_file_data *data);
পূর্ণাঙ্ক password_file__check(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata);
পূর্ণাঙ্ক password_file__reload(পূর্ণাঙ্ক event, শূন্য *event_data, শূন্য *userdata);
শূন্য password_file__cleanup(সংরচনা password_file_data *data);

#endif
