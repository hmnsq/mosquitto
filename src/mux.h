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

#که_نه_تعریف_شوی MUX_H
#تعریف_کړه MUX_H

#ضمن "mosquitto_broker_internal.h"

صحيح mux_epoll__init(فراغ);
صحيح mux_epoll_جمع_کړه_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
صحيح mux_epoll__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
صحيح mux_epoll__new(بنية mosquitto *context);
صحيح mux_epoll_جمع_کړه_بهر(بنية mosquitto *context);
صحيح mux_epoll_remove_بهر(بنية mosquitto *context);
صحيح mux_epoll__delete(بنية mosquitto *context);
صحيح mux_epoll__handle(فراغ);
صحيح mux_epoll__cleanup(فراغ);

صحيح mux_kqueue__init(فراغ);
صحيح mux_kqueue_جمع_کړه_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
صحيح mux_kqueue__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
صحيح mux_kqueue__new(بنية mosquitto *context);
صحيح mux_kqueue_جمع_کړه_بهر(بنية mosquitto *context);
صحيح mux_kqueue_remove_بهر(بنية mosquitto *context);
صحيح mux_kqueue__delete(بنية mosquitto *context);
صحيح mux_kqueue__handle(فراغ);
صحيح mux_kqueue__cleanup(فراغ);

صحيح mux_poll__init(فراغ);
صحيح mux_poll_جمع_کړه_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
صحيح mux_poll__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
صحيح mux_poll__new(بنية mosquitto *context);
صحيح mux_poll_جمع_کړه_بهر(بنية mosquitto *context);
صحيح mux_poll_remove_بهر(بنية mosquitto *context);
صحيح mux_poll__delete(بنية mosquitto *context);
صحيح mux_poll__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_شمېر);
صحيح mux_poll__cleanup(فراغ);

#د_که_پای
