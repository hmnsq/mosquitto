/*
Copyright (c) 2020-2021 Roger Light <roger@atchoo.org>

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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MUX_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MUX_H

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"

ਪੂਰਨ mux_epoll__init(ਖਾਲੀ);
ਪੂਰਨ mux_epoll_ਜੋੜੋ_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਪੂਰਨ mux_epoll__delete_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਪੂਰਨ mux_epoll__new(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_epoll_ਜੋੜੋ_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_epoll_remove_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_epoll__delete(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_epoll__handle(ਖਾਲੀ);
ਪੂਰਨ mux_epoll__cleanup(ਖਾਲੀ);

ਪੂਰਨ mux_kqueue__init(ਖਾਲੀ);
ਪੂਰਨ mux_kqueue_ਜੋੜੋ_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਪੂਰਨ mux_kqueue__delete_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਪੂਰਨ mux_kqueue__new(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_kqueue_ਜੋੜੋ_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_kqueue_remove_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_kqueue__delete(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_kqueue__handle(ਖਾਲੀ);
ਪੂਰਨ mux_kqueue__cleanup(ਖਾਲੀ);

ਪੂਰਨ mux_poll__init(ਖਾਲੀ);
ਪੂਰਨ mux_poll_ਜੋੜੋ_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਪੂਰਨ mux_poll__delete_listeners(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਪੂਰਨ mux_poll__new(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_poll_ਜੋੜੋ_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_poll_remove_ਬਾਹਰਲਾ(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_poll__delete(ਬਣਤਰ mosquitto *context);
ਪੂਰਨ mux_poll__handle(ਬਣਤਰ mosquitto__listener_sock *listensock, ਪੂਰਨ listensock_ਗਿਣਤੀ);
ਪੂਰਨ mux_poll__cleanup(ਖਾਲੀ);

#ਜੇ_ਅੰਤ
