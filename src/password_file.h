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
#اگر_تعریف_نشده PASSWORD_FILE_H
#تعریف_کن PASSWORD_FILE_H

#ضمن <uthash.h>

بنية mosquitto__unpwd {
	UT_hash_handle hh;
	حرف *username;
	حرف *clientid;
	بنية mosquitto_pw *pw;
};

بنية password_file_داده {
	بنية mosquitto__unpwd *unpwd;
	حرف *password_file;
};

صحيح password_file__parse(بنية password_file_داده *داده);
صحيح password_file__check(صحيح event, فراغ *event_داده, فراغ *userdata);
صحيح password_file__reload(صحيح event, فراغ *event_داده, فراغ *userdata);
فراغ password_file__cleanup(بنية password_file_داده *داده);

#پایان_اگر
