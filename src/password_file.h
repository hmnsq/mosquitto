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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PASSWORD_FILE_H
#ವ್ಯಾಖ್ಯಾನಿಸು PASSWORD_FILE_H

#ಅನ್ತರ್ಭಾವ <uthash.h>

ರಚನೆ mosquitto__unpwd {
	UT_hash_handle hh;
	ಅಕ್ಷರ *username;
	ಅಕ್ಷರ *clientid;
	ರಚನೆ mosquitto_pw *pw;
};

ರಚನೆ password_file_ದತ್ತಾಂಶ {
	ರಚನೆ mosquitto__unpwd *unpwd;
	ಅಕ್ಷರ *password_file;
};

ಪೂರ್ಣಾಂಕ password_file__parse(ರಚನೆ password_file_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ);
ಪೂರ್ಣಾಂಕ password_file__check(ಪೂರ್ಣಾಂಕ event, ಶೂನ್ಯ *event_ದತ್ತಾಂಶ, ಶೂನ್ಯ *userdata);
ಪೂರ್ಣಾಂಕ password_file__reload(ಪೂರ್ಣಾಂಕ event, ಶೂನ್ಯ *event_ದತ್ತಾಂಶ, ಶೂನ್ಯ *userdata);
ಶೂನ್ಯ password_file__cleanup(ರಚನೆ password_file_ದತ್ತಾಂಶ *ದತ್ತಾಂಶ);

#ಆದರೆ_ಅಂತ್ಯ
