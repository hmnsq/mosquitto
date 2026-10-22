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
#यदि_अपरिभाषितम् PASSWORD_FILE_H
#परिभाषय PASSWORD_FILE_H

#अन्तर्भाव <uthash.h>

संरचना mosquitto__unpwd {
	UT_hash_handle hh;
	वर्ण *username;
	वर्ण *clientid;
	संरचना mosquitto_pw *pw;
};

संरचना password_file_दत्त {
	संरचना mosquitto__unpwd *unpwd;
	वर्ण *password_file;
};

पूर्णाङ्क password_file__parse(संरचना password_file_दत्त *दत्त);
पूर्णाङ्क password_file__check(पूर्णाङ्क event, शून्य *event_दत्त, शून्य *userdata);
पूर्णाङ्क password_file__reload(पूर्णाङ्क event, शून्य *event_दत्त, शून्य *userdata);
शून्य password_file__cleanup(संरचना password_file_दत्त *दत्त);

#यदि_अन्तः
