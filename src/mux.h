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

#شامل "mosquitto_broker_internal.h"

صحیح mux_epoll__init(تهی);
صحیح mux_epoll__add_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
صحیح mux_epoll__delete_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
صحیح mux_epoll__new(ساختار mosquitto *context);
صحیح mux_epoll__add_out(ساختار mosquitto *context);
صحیح mux_epoll__remove_out(ساختار mosquitto *context);
صحیح mux_epoll__delete(ساختار mosquitto *context);
صحیح mux_epoll__handle(تهی);
صحیح mux_epoll__cleanup(تهی);

صحیح mux_kqueue__init(تهی);
صحیح mux_kqueue__add_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
صحیح mux_kqueue__delete_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
صحیح mux_kqueue__new(ساختار mosquitto *context);
صحیح mux_kqueue__add_out(ساختار mosquitto *context);
صحیح mux_kqueue__remove_out(ساختار mosquitto *context);
صحیح mux_kqueue__delete(ساختار mosquitto *context);
صحیح mux_kqueue__handle(تهی);
صحیح mux_kqueue__cleanup(تهی);

صحیح mux_poll__init(تهی);
صحیح mux_poll__add_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
صحیح mux_poll__delete_listeners(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
صحیح mux_poll__new(ساختار mosquitto *context);
صحیح mux_poll__add_out(ساختار mosquitto *context);
صحیح mux_poll__remove_out(ساختار mosquitto *context);
صحیح mux_poll__delete(ساختار mosquitto *context);
صحیح mux_poll__handle(ساختار mosquitto__listener_sock *listensock, صحیح listensock_شمار);
صحیح mux_poll__cleanup(تهی);

#endif
