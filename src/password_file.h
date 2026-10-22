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
#నిర్వచితం_కాకపోతే PASSWORD_FILE_H
#నిర్వచించు PASSWORD_FILE_H

#అన్తర్భావ <uthash.h>

నిర్మాణం mosquitto__unpwd {
	UT_hash_handle hh;
	అక్షరం *username;
	అక్షరం *clientid;
	నిర్మాణం mosquitto_pw *pw;
};

నిర్మాణం password_file_దత్తాంశం {
	నిర్మాణం mosquitto__unpwd *unpwd;
	అక్షరం *password_file;
};

పూర్ణాంకం password_file__parse(నిర్మాణం password_file_దత్తాంశం *దత్తాంశం);
పూర్ణాంకం password_file__check(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata);
పూర్ణాంకం password_file__reload(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata);
శూన్య password_file__cleanup(నిర్మాణం password_file_దత్తాంశం *దత్తాంశం);

#అయితే_ముగింపు
