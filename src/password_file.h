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
#اگر_غیر_تعریف_شدہ PASSWORD_FILE_H
#تعریف_کرو PASSWORD_FILE_H

#شامل <uthash.h>

ساخت mosquitto__unpwd {
	UT_hash_handle hh;
	حرف *username;
	حرف *clientid;
	ساخت mosquitto_pw *pw;
};

ساخت password_file_ڈیٹا {
	ساخت mosquitto__unpwd *unpwd;
	حرف *password_file;
};

صحیح password_file__parse(ساخت password_file_ڈیٹا *ڈیٹا);
صحیح password_file__check(صحیح event, خالی *event_ڈیٹا, خالی *userdata);
صحیح password_file__reload(صحیح event, خالی *event_ڈیٹا, خالی *userdata);
خالی password_file__cleanup(ساخت password_file_ڈیٹا *ڈیٹا);

#اگر_ختم
