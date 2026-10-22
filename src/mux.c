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

#include "mux.h"


จำนวนเต็ม mux__init(void)
{
#ifdef WITH_EPOLL
	คืน mux_epoll__init();
#elif defined(WITH_KQUEUE)
	คืน mux_kqueue__init();
#มิฉะนั้น
	คืน mux_poll__init();
#endif
}


จำนวนเต็ม mux__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, จำนวนเต็ม listensock_count)
{
#ifdef WITH_EPOLL
	คืน mux_epoll__add_listeners(listensock, listensock_count);
#elif defined(WITH_KQUEUE)
	คืน mux_kqueue__add_listeners(listensock, listensock_count);
#มิฉะนั้น
	คืน mux_poll__add_listeners(listensock, listensock_count);
#endif
}


จำนวนเต็ม mux__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, จำนวนเต็ม listensock_count)
{
#ifdef WITH_EPOLL
	คืน mux_epoll__delete_listeners(listensock, listensock_count);
#elif defined(WITH_KQUEUE)
	คืน mux_kqueue__delete_listeners(listensock, listensock_count);
#มิฉะนั้น
	คืน mux_poll__delete_listeners(listensock, listensock_count);
#endif
}


จำนวนเต็ม mux__add_out(sa_nracanaa mosquitto *context)
{
#ifdef WITH_EPOLL
	คืน mux_epoll__add_out(context);
#elif defined(WITH_KQUEUE)
	คืน mux_kqueue__add_out(context);
#มิฉะนั้น
	คืน mux_poll__add_out(context);
#endif
}


จำนวนเต็ม mux__remove_out(sa_nracanaa mosquitto *context)
{
#ifdef WITH_EPOLL
	คืน mux_epoll__remove_out(context);
#elif defined(WITH_KQUEUE)
	คืน mux_kqueue__remove_out(context);
#มิฉะนั้น
	คืน mux_poll__remove_out(context);
#endif
}


จำนวนเต็ม mux__new(sa_nracanaa mosquitto *context)
{
#ifdef WITH_EPOLL
	คืน mux_epoll__new(context);
#elif defined(WITH_KQUEUE)
	คืน mux_kqueue__new(context);
#มิฉะนั้น
	คืน mux_poll__new(context);
#endif
}


จำนวนเต็ม mux__delete(sa_nracanaa mosquitto *context)
{
#ifdef WITH_EPOLL
	คืน mux_epoll__delete(context);
#elif defined(WITH_KQUEUE)
	คืน mux_kqueue__delete(context);
#มิฉะนั้น
	คืน mux_poll__delete(context);
#endif
}


จำนวนเต็ม mux__handle(sa_nracanaa mosquitto__listener_sock *listensock, จำนวนเต็ม listensock_count)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_count);
	คืน mux_epoll__handle();
#elif defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_count);
	คืน mux_kqueue__handle();
#มิฉะนั้น
	คืน mux_poll__handle(listensock, listensock_count);
#endif
}


จำนวนเต็ม mux__cleanup(void)
{
#ifdef WITH_EPOLL
	คืน mux_epoll__cleanup();
#elif defined(WITH_KQUEUE)
	คืน mux_kqueue__cleanup();
#มิฉะนั้น
	คืน mux_poll__cleanup();
#endif
}
