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

#அந்தர்பாவ "mosquitto_broker_internal.h"

முழு mux_epoll__init(வெறுமை);
முழு mux_epoll__add_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
முழு mux_epoll__delete_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
முழு mux_epoll__new(கட்டமைப்பு mosquitto *context);
முழு mux_epoll__add_out(கட்டமைப்பு mosquitto *context);
முழு mux_epoll__remove_out(கட்டமைப்பு mosquitto *context);
முழு mux_epoll__delete(கட்டமைப்பு mosquitto *context);
முழு mux_epoll__handle(வெறுமை);
முழு mux_epoll__cleanup(வெறுமை);

முழு mux_kqueue__init(வெறுமை);
முழு mux_kqueue__add_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
முழு mux_kqueue__delete_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
முழு mux_kqueue__new(கட்டமைப்பு mosquitto *context);
முழு mux_kqueue__add_out(கட்டமைப்பு mosquitto *context);
முழு mux_kqueue__remove_out(கட்டமைப்பு mosquitto *context);
முழு mux_kqueue__delete(கட்டமைப்பு mosquitto *context);
முழு mux_kqueue__handle(வெறுமை);
முழு mux_kqueue__cleanup(வெறுமை);

முழு mux_poll__init(வெறுமை);
முழு mux_poll__add_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
முழு mux_poll__delete_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
முழு mux_poll__new(கட்டமைப்பு mosquitto *context);
முழு mux_poll__add_out(கட்டமைப்பு mosquitto *context);
முழு mux_poll__remove_out(கட்டமைப்பு mosquitto *context);
முழு mux_poll__delete(கட்டமைப்பு mosquitto *context);
முழு mux_poll__handle(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்);
முழு mux_poll__cleanup(வெறுமை);

#endif
