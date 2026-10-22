/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

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
   Tatsuzo Osawa - Add epoll.
*/

#అన్తర్భావ "mux.h"


పూర్ణాంకం mux__init(శూన్య)
{
#నిర్వచితమైతే WITH_EPOLL
	తిరిగి mux_epoll__init();
#లేదావేళ defined(WITH_KQUEUE)
	తిరిగి mux_kqueue__init();
#లేకపోతే
	తిరిగి mux_poll__init();
#అయితే_ముగింపు
}


పూర్ణాంకం mux_కూడు_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
#నిర్వచితమైతే WITH_EPOLL
	తిరిగి mux_epoll_కూడు_listeners(listensock, listensock_లెక్క);
#లేదావేళ defined(WITH_KQUEUE)
	తిరిగి mux_kqueue_కూడు_listeners(listensock, listensock_లెక్క);
#లేకపోతే
	తిరిగి mux_poll_కూడు_listeners(listensock, listensock_లెక్క);
#అయితే_ముగింపు
}


పూర్ణాంకం mux__delete_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
#నిర్వచితమైతే WITH_EPOLL
	తిరిగి mux_epoll__delete_listeners(listensock, listensock_లెక్క);
#లేదావేళ defined(WITH_KQUEUE)
	తిరిగి mux_kqueue__delete_listeners(listensock, listensock_లెక్క);
#లేకపోతే
	తిరిగి mux_poll__delete_listeners(listensock, listensock_లెక్క);
#అయితే_ముగింపు
}


పూర్ణాంకం mux_కూడు_బహిర్గతం(నిర్మాణం mosquitto *context)
{
#నిర్వచితమైతే WITH_EPOLL
	తిరిగి mux_epoll_కూడు_బహిర్గతం(context);
#లేదావేళ defined(WITH_KQUEUE)
	తిరిగి mux_kqueue_కూడు_బహిర్గతం(context);
#లేకపోతే
	తిరిగి mux_poll_కూడు_బహిర్గతం(context);
#అయితే_ముగింపు
}


పూర్ణాంకం mux_remove_బహిర్గతం(నిర్మాణం mosquitto *context)
{
#నిర్వచితమైతే WITH_EPOLL
	తిరిగి mux_epoll_remove_బహిర్గతం(context);
#లేదావేళ defined(WITH_KQUEUE)
	తిరిగి mux_kqueue_remove_బహిర్గతం(context);
#లేకపోతే
	తిరిగి mux_poll_remove_బహిర్గతం(context);
#అయితే_ముగింపు
}


పూర్ణాంకం mux__new(నిర్మాణం mosquitto *context)
{
#నిర్వచితమైతే WITH_EPOLL
	తిరిగి mux_epoll__new(context);
#లేదావేళ defined(WITH_KQUEUE)
	తిరిగి mux_kqueue__new(context);
#లేకపోతే
	తిరిగి mux_poll__new(context);
#అయితే_ముగింపు
}


పూర్ణాంకం mux__delete(నిర్మాణం mosquitto *context)
{
#నిర్వచితమైతే WITH_EPOLL
	తిరిగి mux_epoll__delete(context);
#లేదావేళ defined(WITH_KQUEUE)
	తిరిగి mux_kqueue__delete(context);
#లేకపోతే
	తిరిగి mux_poll__delete(context);
#అయితే_ముగింపు
}


పూర్ణాంకం mux__handle(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క)
{
#నిర్వచితమైతే WITH_EPOLL
	వాడని(listensock);
	వాడని(listensock_లెక్క);
	తిరిగి mux_epoll__handle();
#లేదావేళ defined(WITH_KQUEUE)
	వాడని(listensock);
	వాడని(listensock_లెక్క);
	తిరిగి mux_kqueue__handle();
#లేకపోతే
	తిరిగి mux_poll__handle(listensock, listensock_లెక్క);
#అయితే_ముగింపు
}


పూర్ణాంకం mux__cleanup(శూన్య)
{
#నిర్వచితమైతే WITH_EPOLL
	తిరిగి mux_epoll__cleanup();
#లేదావేళ defined(WITH_KQUEUE)
	తిరిగి mux_kqueue__cleanup();
#లేకపోతే
	తిరిగి mux_poll__cleanup();
#అయితే_ముగింపు
}
