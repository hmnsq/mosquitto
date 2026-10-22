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

#ضمن "mux.h"


صحيح mux__init(فراغ)
{
#اگر_تعریف_شده WITH_EPOLL
	أرجع mux_epoll__init();
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__init();
#وإلا
	أرجع mux_poll__init();
#پایان_اگر
}


صحيح mux_جمع_کن_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار)
{
#اگر_تعریف_شده WITH_EPOLL
	أرجع mux_epoll_جمع_کن_listeners(listensock, listensock_شمار);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue_جمع_کن_listeners(listensock, listensock_شمار);
#وإلا
	أرجع mux_poll_جمع_کن_listeners(listensock, listensock_شمار);
#پایان_اگر
}


صحيح mux__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار)
{
#اگر_تعریف_شده WITH_EPOLL
	أرجع mux_epoll__delete_listeners(listensock, listensock_شمار);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__delete_listeners(listensock, listensock_شمار);
#وإلا
	أرجع mux_poll__delete_listeners(listensock, listensock_شمار);
#پایان_اگر
}


صحيح mux_جمع_کن_بیرون(بنية mosquitto *context)
{
#اگر_تعریف_شده WITH_EPOLL
	أرجع mux_epoll_جمع_کن_بیرون(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue_جمع_کن_بیرون(context);
#وإلا
	أرجع mux_poll_جمع_کن_بیرون(context);
#پایان_اگر
}


صحيح mux_remove_بیرون(بنية mosquitto *context)
{
#اگر_تعریف_شده WITH_EPOLL
	أرجع mux_epoll_remove_بیرون(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue_remove_بیرون(context);
#وإلا
	أرجع mux_poll_remove_بیرون(context);
#پایان_اگر
}


صحيح mux__new(بنية mosquitto *context)
{
#اگر_تعریف_شده WITH_EPOLL
	أرجع mux_epoll__new(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__new(context);
#وإلا
	أرجع mux_poll__new(context);
#پایان_اگر
}


صحيح mux__delete(بنية mosquitto *context)
{
#اگر_تعریف_شده WITH_EPOLL
	أرجع mux_epoll__delete(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__delete(context);
#وإلا
	أرجع mux_poll__delete(context);
#پایان_اگر
}


صحيح mux__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمار)
{
#اگر_تعریف_شده WITH_EPOLL
	استفادهنشده(listensock);
	استفادهنشده(listensock_شمار);
	أرجع mux_epoll__handle();
#وإلاإذا defined(WITH_KQUEUE)
	استفادهنشده(listensock);
	استفادهنشده(listensock_شمار);
	أرجع mux_kqueue__handle();
#وإلا
	أرجع mux_poll__handle(listensock, listensock_شمار);
#پایان_اگر
}


صحيح mux__cleanup(فراغ)
{
#اگر_تعریف_شده WITH_EPOLL
	أرجع mux_epoll__cleanup();
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__cleanup();
#وإلا
	أرجع mux_poll__cleanup();
#پایان_اگر
}
