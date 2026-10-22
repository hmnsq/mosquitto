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

#અન્તર્ભાવ <uthash.h>

સંરચના mosquitto__unpwd {
	UT_hash_handle hh;
	અક્ષર *username;
	અક્ષર *clientid;
	સંરચના mosquitto_pw *pw;
};

સંરચના password_file_data {
	સંરચના mosquitto__unpwd *unpwd;
	અક્ષર *password_file;
};

પૂર્ણાંક password_file__parse(સંરચના password_file_data *data);
પૂર્ણાંક password_file__check(પૂર્ણાંક event, શૂન્ય *event_data, શૂન્ય *userdata);
પૂર્ણાંક password_file__reload(પૂર્ણાંક event, શૂન્ય *event_data, શૂન્ય *userdata);
શૂન્ય password_file__cleanup(સંરચના password_file_data *data);

#endif
