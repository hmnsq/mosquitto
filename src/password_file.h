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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PASSWORD_FILE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PASSWORD_FILE_H

#ਅਨ੍ਤਰ੍ਭਾਵ <uthash.h>

ਬਣਤਰ mosquitto__unpwd {
	UT_hash_handle hh;
	ਅੱਖਰ *username;
	ਅੱਖਰ *clientid;
	ਬਣਤਰ mosquitto_pw *pw;
};

ਬਣਤਰ password_file_ਅੰਕੜੇ {
	ਬਣਤਰ mosquitto__unpwd *unpwd;
	ਅੱਖਰ *password_file;
};

ਪੂਰਨ password_file__parse(ਬਣਤਰ password_file_ਅੰਕੜੇ *ਅੰਕੜੇ);
ਪੂਰਨ password_file__check(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata);
ਪੂਰਨ password_file__reload(ਪੂਰਨ event, ਖਾਲੀ *event_ਅੰਕੜੇ, ਖਾਲੀ *userdata);
ਖਾਲੀ password_file__cleanup(ਬਣਤਰ password_file_ਅੰਕੜੇ *ਅੰਕੜੇ);

#ਜੇ_ਅੰਤ
