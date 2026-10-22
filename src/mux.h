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

#ifndef MUX_H
#define MUX_H

#অন্তর্ভাব "mosquitto_broker_internal.h"

পূর্ণসংখ্যা mux_epoll__init(শূন্য);
পূর্ণসংখ্যা mux_epoll__add_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
পূর্ণসংখ্যা mux_epoll__delete_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
পূর্ণসংখ্যা mux_epoll__new(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_epoll__add_out(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_epoll__remove_out(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_epoll__delete(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_epoll__handle(শূন্য);
পূর্ণসংখ্যা mux_epoll__cleanup(শূন্য);

পূর্ণসংখ্যা mux_kqueue__init(শূন্য);
পূর্ণসংখ্যা mux_kqueue__add_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
পূর্ণসংখ্যা mux_kqueue__delete_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
পূর্ণসংখ্যা mux_kqueue__new(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_kqueue__add_out(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_kqueue__remove_out(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_kqueue__delete(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_kqueue__handle(শূন্য);
পূর্ণসংখ্যা mux_kqueue__cleanup(শূন্য);

পূর্ণসংখ্যা mux_poll__init(শূন্য);
পূর্ণসংখ্যা mux_poll__add_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
পূর্ণসংখ্যা mux_poll__delete_listeners(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
পূর্ণসংখ্যা mux_poll__new(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_poll__add_out(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_poll__remove_out(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_poll__delete(কাঠামো mosquitto *context);
পূর্ণসংখ্যা mux_poll__handle(কাঠামো mosquitto__listener_sock *listensock, পূর্ণসংখ্যা listensock_গুনতি);
পূর্ণসংখ্যা mux_poll__cleanup(শূন্য);

#endif
