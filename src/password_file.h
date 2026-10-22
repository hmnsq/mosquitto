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

#ಅನ್ತರ್ಭಾವ <uthash.h>

ಸಂರಚನಾ mosquitto__unpwd {
	UT_hash_handle hh;
	ವರ್ಣ *username;
	ವರ್ಣ *clientid;
	ಸಂರಚನಾ mosquitto_pw *pw;
};

ಸಂರಚನಾ password_file_data {
	ಸಂರಚನಾ mosquitto__unpwd *unpwd;
	ವರ್ಣ *password_file;
};

ಪೂರ್ಣಾಙ್ಕ password_file__parse(ಸಂರಚನಾ password_file_data *data);
ಪೂರ್ಣಾಙ್ಕ password_file__check(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata);
ಪೂರ್ಣಾಙ್ಕ password_file__reload(ಪೂರ್ಣಾಙ್ಕ event, ಶೂನ್ಯ *event_data, ಶೂನ್ಯ *userdata);
ಶೂನ್ಯ password_file__cleanup(ಸಂರಚನಾ password_file_data *data);

#endif
