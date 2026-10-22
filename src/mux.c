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


整数 mux__init(void)
{
#ifdef WITH_EPOLL
	返回 mux_epoll__init();
#elif defined(WITH_KQUEUE)
	返回 mux_kqueue__init();
#否则
	返回 mux_poll__init();
#endif
}


整数 mux__add_listeners(struct mosquitto__listener_sock *listensock, 整数 listensock_count)
{
#ifdef WITH_EPOLL
	返回 mux_epoll__add_listeners(listensock, listensock_count);
#elif defined(WITH_KQUEUE)
	返回 mux_kqueue__add_listeners(listensock, listensock_count);
#否则
	返回 mux_poll__add_listeners(listensock, listensock_count);
#endif
}


整数 mux__delete_listeners(struct mosquitto__listener_sock *listensock, 整数 listensock_count)
{
#ifdef WITH_EPOLL
	返回 mux_epoll__delete_listeners(listensock, listensock_count);
#elif defined(WITH_KQUEUE)
	返回 mux_kqueue__delete_listeners(listensock, listensock_count);
#否则
	返回 mux_poll__delete_listeners(listensock, listensock_count);
#endif
}


整数 mux__add_out(struct mosquitto *context)
{
#ifdef WITH_EPOLL
	返回 mux_epoll__add_out(context);
#elif defined(WITH_KQUEUE)
	返回 mux_kqueue__add_out(context);
#否则
	返回 mux_poll__add_out(context);
#endif
}


整数 mux__remove_out(struct mosquitto *context)
{
#ifdef WITH_EPOLL
	返回 mux_epoll__remove_out(context);
#elif defined(WITH_KQUEUE)
	返回 mux_kqueue__remove_out(context);
#否则
	返回 mux_poll__remove_out(context);
#endif
}


整数 mux__new(struct mosquitto *context)
{
#ifdef WITH_EPOLL
	返回 mux_epoll__new(context);
#elif defined(WITH_KQUEUE)
	返回 mux_kqueue__new(context);
#否则
	返回 mux_poll__new(context);
#endif
}


整数 mux__delete(struct mosquitto *context)
{
#ifdef WITH_EPOLL
	返回 mux_epoll__delete(context);
#elif defined(WITH_KQUEUE)
	返回 mux_kqueue__delete(context);
#否则
	返回 mux_poll__delete(context);
#endif
}


整数 mux__handle(struct mosquitto__listener_sock *listensock, 整数 listensock_count)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_count);
	返回 mux_epoll__handle();
#elif defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_count);
	返回 mux_kqueue__handle();
#否则
	返回 mux_poll__handle(listensock, listensock_count);
#endif
}


整数 mux__cleanup(void)
{
#ifdef WITH_EPOLL
	返回 mux_epoll__cleanup();
#elif defined(WITH_KQUEUE)
	返回 mux_kqueue__cleanup();
#否则
	返回 mux_poll__cleanup();
#endif
}
