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

#include "mosquitto_broker_internal.h"

ཧྲིལ mux_epoll__init(void);
ཧྲིལ mux_epoll__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, ཧྲིལ listensock_count);
ཧྲིལ mux_epoll__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, ཧྲིལ listensock_count);
ཧྲིལ mux_epoll__new(sa_nracanaa mosquitto *context);
ཧྲིལ mux_epoll__add_out(sa_nracanaa mosquitto *context);
ཧྲིལ mux_epoll__remove_out(sa_nracanaa mosquitto *context);
ཧྲིལ mux_epoll__delete(sa_nracanaa mosquitto *context);
ཧྲིལ mux_epoll__handle(void);
ཧྲིལ mux_epoll__cleanup(void);

ཧྲིལ mux_kqueue__init(void);
ཧྲིལ mux_kqueue__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, ཧྲིལ listensock_count);
ཧྲིལ mux_kqueue__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, ཧྲིལ listensock_count);
ཧྲིལ mux_kqueue__new(sa_nracanaa mosquitto *context);
ཧྲིལ mux_kqueue__add_out(sa_nracanaa mosquitto *context);
ཧྲིལ mux_kqueue__remove_out(sa_nracanaa mosquitto *context);
ཧྲིལ mux_kqueue__delete(sa_nracanaa mosquitto *context);
ཧྲིལ mux_kqueue__handle(void);
ཧྲིལ mux_kqueue__cleanup(void);

ཧྲིལ mux_poll__init(void);
ཧྲིལ mux_poll__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, ཧྲིལ listensock_count);
ཧྲིལ mux_poll__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, ཧྲིལ listensock_count);
ཧྲིལ mux_poll__new(sa_nracanaa mosquitto *context);
ཧྲིལ mux_poll__add_out(sa_nracanaa mosquitto *context);
ཧྲིལ mux_poll__remove_out(sa_nracanaa mosquitto *context);
ཧྲིལ mux_poll__delete(sa_nracanaa mosquitto *context);
ཧྲིལ mux_poll__handle(sa_nracanaa mosquitto__listener_sock *listensock, ཧྲིལ listensock_count);
ཧྲིལ mux_poll__cleanup(void);

#endif
