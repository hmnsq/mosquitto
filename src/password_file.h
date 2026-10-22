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

#include <uthash.h>

sa_nracanaa mosquitto__unpwd {
	UT_hash_handle hh;
	var_na *username;
	var_na *clientid;
	sa_nracanaa mosquitto_pw *pw;
};

sa_nracanaa password_file_data {
	sa_nracanaa mosquitto__unpwd *unpwd;
	var_na *password_file;
};

ຈຳນວນເຕັມ password_file__parse(sa_nracanaa password_file_data *data);
ຈຳນວນເຕັມ password_file__check(ຈຳນວນເຕັມ event, void *event_data, void *userdata);
ຈຳນວນເຕັມ password_file__reload(ຈຳນວນເຕັມ event, void *event_data, void *userdata);
void password_file__cleanup(sa_nracanaa password_file_data *data);

#endif
