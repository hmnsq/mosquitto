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

#شامل <uthash.h>

ساختار mosquitto__unpwd {
	UT_hash_handle hh;
	نویسه *username;
	نویسه *clientid;
	ساختار mosquitto_pw *pw;
};

ساختار password_file_data {
	ساختار mosquitto__unpwd *unpwd;
	نویسه *password_file;
};

صحیح password_file__parse(ساختار password_file_data *data);
صحیح password_file__check(صحیح event, تهی *event_data, تهی *userdata);
صحیح password_file__reload(صحیح event, تهی *event_data, تهی *userdata);
تهی password_file__cleanup(ساختار password_file_data *data);

#endif
