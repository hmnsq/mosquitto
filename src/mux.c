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

#অন্তর্ভাব "mux.h"


পূর্ণসংখ্যা mux__init(শূন্য)
{
#ifdef WITH_EPOLL
	ফেরত mux_epoll__init();
#নতুবা_যদি defined(WITH_KQUEUE)
	ফেরত mux_kqueue__init();
#অন্যথা
	ফেরত mux_poll__init();
#endif
}


পূর্ণসংখ্যা mux__add_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি)
{
#ifdef WITH_EPOLL
	ফেরত mux_epoll__add_listeners(listensock, listensock_গুনতি);
#নতুবা_যদি defined(WITH_KQUEUE)
	ফেরত mux_kqueue__add_listeners(listensock, listensock_গুনতি);
#অন্যথা
	ফেরত mux_poll__add_listeners(listensock, listensock_গুনতি);
#endif
}


পূর্ণসংখ্যা mux__delete_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি)
{
#ifdef WITH_EPOLL
	ফেরত mux_epoll__delete_listeners(listensock, listensock_গুনতি);
#নতুবা_যদি defined(WITH_KQUEUE)
	ফেরত mux_kqueue__delete_listeners(listensock, listensock_গুনতি);
#অন্যথা
	ফেরত mux_poll__delete_listeners(listensock, listensock_গুনতি);
#endif
}


পূর্ণসংখ্যা mux__add_out(কাঠামো mosquitto *context)
{
#ifdef WITH_EPOLL
	ফেরত mux_epoll__add_out(context);
#নতুবা_যদি defined(WITH_KQUEUE)
	ফেরত mux_kqueue__add_out(context);
#অন্যথা
	ফেরত mux_poll__add_out(context);
#endif
}


পূর্ণসংখ্যা mux__remove_out(কাঠামো mosquitto *context)
{
#ifdef WITH_EPOLL
	ফেরত mux_epoll__remove_out(context);
#নতুবা_যদি defined(WITH_KQUEUE)
	ফেরত mux_kqueue__remove_out(context);
#অন্যথা
	ফেরত mux_poll__remove_out(context);
#endif
}


পূর্ণসংখ্যা mux__new(কাঠামো mosquitto *context)
{
#ifdef WITH_EPOLL
	ফেরত mux_epoll__new(context);
#নতুবা_যদি defined(WITH_KQUEUE)
	ফেরত mux_kqueue__new(context);
#অন্যথা
	ফেরত mux_poll__new(context);
#endif
}


পূর্ণসংখ্যা mux__delete(কাঠামো mosquitto *context)
{
#ifdef WITH_EPOLL
	ফেরত mux_epoll__delete(context);
#নতুবা_যদি defined(WITH_KQUEUE)
	ফেরত mux_kqueue__delete(context);
#অন্যথা
	ফেরত mux_poll__delete(context);
#endif
}


পূর্ণসংখ্যা mux__handle(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_গুনতি);
	ফেরত mux_epoll__handle();
#নতুবা_যদি defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_গুনতি);
	ফেরত mux_kqueue__handle();
#অন্যথা
	ফেরত mux_poll__handle(listensock, listensock_গুনতি);
#endif
}


পূর্ণসংখ্যা mux__cleanup(শূন্য)
{
#ifdef WITH_EPOLL
	ফেরত mux_epoll__cleanup();
#নতুবা_যদি defined(WITH_KQUEUE)
	ফেরত mux_kqueue__cleanup();
#অন্যথা
	ফেরত mux_poll__cleanup();
#endif
}
