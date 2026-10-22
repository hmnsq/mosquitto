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

#അന്തര്ഭാവ <uthash.h>

ഘടന mosquitto__unpwd {
	UT_hash_handle hh;
	അക്ഷരം *username;
	അക്ഷരം *clientid;
	ഘടന mosquitto_pw *pw;
};

ഘടന password_file_data {
	ഘടന mosquitto__unpwd *unpwd;
	അക്ഷരം *password_file;
};

പൂർണ്ണസംഖ്യ password_file__parse(ഘടന password_file_data *data);
പൂർണ്ണസംഖ്യ password_file__check(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata);
പൂർണ്ണസംഖ്യ password_file__reload(പൂർണ്ണസംഖ്യ event, ശൂന്യം *event_data, ശൂന്യം *userdata);
ശൂന്യം password_file__cleanup(ഘടന password_file_data *data);

#endif
