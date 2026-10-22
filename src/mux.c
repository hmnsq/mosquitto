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

#ਅਨ੍ਤਰ੍ਭਾਵ "mux.h"


ਪੂਰਨ mux__init(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL
	ਵਾਪਸ mux_epoll__init();
#ਜਾਂਜੇ defined(WITH_KQUEUE)
	ਵਾਪਸ mux_kqueue__init();
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ mux_poll__init();
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ mux_ਜੋੜੋ_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL
	ਵਾਪਸ mux_epoll_ਜੋੜੋ_listeners(listensock, listensock_ਗਿਣਤੀ);
#ਜਾਂਜੇ defined(WITH_KQUEUE)
	ਵਾਪਸ mux_kqueue_ਜੋੜੋ_listeners(listensock, listensock_ਗਿਣਤੀ);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ mux_poll_ਜੋੜੋ_listeners(listensock, listensock_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ mux__delete_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL
	ਵਾਪਸ mux_epoll__delete_listeners(listensock, listensock_ਗਿਣਤੀ);
#ਜਾਂਜੇ defined(WITH_KQUEUE)
	ਵਾਪਸ mux_kqueue__delete_listeners(listensock, listensock_ਗਿਣਤੀ);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ mux_poll__delete_listeners(listensock, listensock_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ mux_ਜੋੜੋ_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL
	ਵਾਪਸ mux_epoll_ਜੋੜੋ_ਬਾਹਰਲਾ(context);
#ਜਾਂਜੇ defined(WITH_KQUEUE)
	ਵਾਪਸ mux_kqueue_ਜੋੜੋ_ਬਾਹਰਲਾ(context);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ mux_poll_ਜੋੜੋ_ਬਾਹਰਲਾ(context);
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ mux_remove_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL
	ਵਾਪਸ mux_epoll_remove_ਬਾਹਰਲਾ(context);
#ਜਾਂਜੇ defined(WITH_KQUEUE)
	ਵਾਪਸ mux_kqueue_remove_ਬਾਹਰਲਾ(context);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ mux_poll_remove_ਬਾਹਰਲਾ(context);
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ mux__new(ਬਣਤਰ mosquitto *context)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL
	ਵਾਪਸ mux_epoll__new(context);
#ਜਾਂਜੇ defined(WITH_KQUEUE)
	ਵਾਪਸ mux_kqueue__new(context);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ mux_poll__new(context);
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ mux__delete(ਬਣਤਰ mosquitto *context)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL
	ਵਾਪਸ mux_epoll__delete(context);
#ਜਾਂਜੇ defined(WITH_KQUEUE)
	ਵਾਪਸ mux_kqueue__delete(context);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ mux_poll__delete(context);
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ mux__handle(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL
	ਅਣਵਰਤਿਆ(listensock);
	ਅਣਵਰਤਿਆ(listensock_ਗਿਣਤੀ);
	ਵਾਪਸ mux_epoll__handle();
#ਜਾਂਜੇ defined(WITH_KQUEUE)
	ਅਣਵਰਤਿਆ(listensock);
	ਅਣਵਰਤਿਆ(listensock_ਗਿਣਤੀ);
	ਵਾਪਸ mux_kqueue__handle();
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ mux_poll__handle(listensock, listensock_ਗਿਣਤੀ);
#ਜੇ_ਅੰਤ
}


ਪੂਰਨ mux__cleanup(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_EPOLL
	ਵਾਪਸ mux_epoll__cleanup();
#ਜਾਂਜੇ defined(WITH_KQUEUE)
	ਵਾਪਸ mux_kqueue__cleanup();
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ mux_poll__cleanup();
#ਜੇ_ਅੰਤ
}
