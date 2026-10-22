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
#अगर_अघोषित PASSWORD_FILE_H
#घोषणा PASSWORD_FILE_H

#समावेश <uthash.h>

काष्ठा mosquitto__unpwd {
	UT_hash_handle hh;
	अक्षर *username;
	अक्षर *clientid;
	काष्ठा mosquitto_pw *pw;
};

काष्ठा password_file_तथ्य {
	काष्ठा mosquitto__unpwd *unpwd;
	अक्षर *password_file;
};

पूर्णांक password_file__parse(काष्ठा password_file_तथ्य *तथ्य);
पूर्णांक password_file__check(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata);
पूर्णांक password_file__reload(पूर्णांक event, व्योम *event_तथ्य, व्योम *userdata);
व्योम password_file__cleanup(काष्ठा password_file_तथ्य *तथ्य);

#पूर्ण_अगर
