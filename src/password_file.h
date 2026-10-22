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

#अन्तर्भाव <uthash.h>

संरचना mosquitto__unpwd {
	UT_hash_handle hh;
	अक्षर *username;
	अक्षर *clientid;
	संरचना mosquitto_pw *pw;
};

संरचना password_file_data {
	संरचना mosquitto__unpwd *unpwd;
	अक्षर *password_file;
};

पूर्णांक password_file__parse(संरचना password_file_data *data);
पूर्णांक password_file__check(पूर्णांक event, शून्य *event_data, शून्य *userdata);
पूर्णांक password_file__reload(पूर्णांक event, शून्य *event_data, शून्य *userdata);
शून्य password_file__cleanup(संरचना password_file_data *data);

#endif
