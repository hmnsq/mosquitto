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
#నిర్వచితం_కాకపోతే ACL_FILE_H
#నిర్వచించు ACL_FILE_H

#అన్తర్భావ <uthash.h>

నిర్మాణం acl_నమోదు {
	నిర్మాణం acl_నమోదు *తదుపరి, *prev;
	అక్షరం *topic;
	పూర్ణాంకం access;
	పూర్ణాంకం ucount;
	పూర్ణాంకం ccount;
};


నిర్మాణం acl__user {
	UT_hash_handle hh;
	అక్షరం *username;
	నిర్మాణం acl_నమోదు *acl;
};


నిర్మాణం acl_file_దత్తాంశం {
	అక్షరం *acl_file;
	నిర్మాణం acl__user *acl_users;
	నిర్మాణం acl__user acl_anon;
	నిర్మాణం acl_నమోదు *acl_patterns;
};


పూర్ణాంకం acl_file__parse(నిర్మాణం acl_file_దత్తాంశం *దత్తాంశం);
పూర్ణాంకం acl_file__check(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata);
పూర్ణాంకం acl_file__reload(పూర్ణాంకం event, శూన్య *event_దత్తాంశం, శూన్య *userdata);
శూన్య acl_file__cleanup(నిర్మాణం acl_file_దత్తాంశం *దత్తాంశం);

#అయితే_ముగింపు
