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

#شامل "mux.h"


صحیح mux__init(تهی)
{
#ifdef WITH_EPOLL
	بازگشت mux_epoll__init();
#وگرنهاگر defined(WITH_KQUEUE)
	بازگشت mux_kqueue__init();
#وگرنه
	بازگشت mux_poll__init();
#endif
}


صحیح mux__add_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
#ifdef WITH_EPOLL
	بازگشت mux_epoll__add_listeners(listensock, listensock_شمار);
#وگرنهاگر defined(WITH_KQUEUE)
	بازگشت mux_kqueue__add_listeners(listensock, listensock_شمار);
#وگرنه
	بازگشت mux_poll__add_listeners(listensock, listensock_شمار);
#endif
}


صحیح mux__delete_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
#ifdef WITH_EPOLL
	بازگشت mux_epoll__delete_listeners(listensock, listensock_شمار);
#وگرنهاگر defined(WITH_KQUEUE)
	بازگشت mux_kqueue__delete_listeners(listensock, listensock_شمار);
#وگرنه
	بازگشت mux_poll__delete_listeners(listensock, listensock_شمار);
#endif
}


صحیح mux__add_out(ساختار mosquitto *context)
{
#ifdef WITH_EPOLL
	بازگشت mux_epoll__add_out(context);
#وگرنهاگر defined(WITH_KQUEUE)
	بازگشت mux_kqueue__add_out(context);
#وگرنه
	بازگشت mux_poll__add_out(context);
#endif
}


صحیح mux__remove_out(ساختار mosquitto *context)
{
#ifdef WITH_EPOLL
	بازگشت mux_epoll__remove_out(context);
#وگرنهاگر defined(WITH_KQUEUE)
	بازگشت mux_kqueue__remove_out(context);
#وگرنه
	بازگشت mux_poll__remove_out(context);
#endif
}


صحیح mux__new(ساختار mosquitto *context)
{
#ifdef WITH_EPOLL
	بازگشت mux_epoll__new(context);
#وگرنهاگر defined(WITH_KQUEUE)
	بازگشت mux_kqueue__new(context);
#وگرنه
	بازگشت mux_poll__new(context);
#endif
}


صحیح mux__delete(ساختار mosquitto *context)
{
#ifdef WITH_EPOLL
	بازگشت mux_epoll__delete(context);
#وگرنهاگر defined(WITH_KQUEUE)
	بازگشت mux_kqueue__delete(context);
#وگرنه
	بازگشت mux_poll__delete(context);
#endif
}


صحیح mux__handle(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_شمار);
	بازگشت mux_epoll__handle();
#وگرنهاگر defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_شمار);
	بازگشت mux_kqueue__handle();
#وگرنه
	بازگشت mux_poll__handle(listensock, listensock_شمار);
#endif
}


صحیح mux__cleanup(تهی)
{
#ifdef WITH_EPOLL
	بازگشت mux_epoll__cleanup();
#وگرنهاگر defined(WITH_KQUEUE)
	بازگشت mux_kqueue__cleanup();
#وگرنه
	بازگشت mux_poll__cleanup();
#endif
}
