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

#اگر_غیر_تعریف_شدہ MUX_H
#تعریف_کرو MUX_H

#شامل "mosquitto_broker_internal.h"

صحیح mux_epoll__init(خالی);
صحیح mux_epoll_جوڑو_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
صحیح mux_epoll__delete_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
صحیح mux_epoll__new(ساخت mosquitto *context);
صحیح mux_epoll_جوڑو_برآمد(ساخت mosquitto *context);
صحیح mux_epoll_remove_برآمد(ساخت mosquitto *context);
صحیح mux_epoll__delete(ساخت mosquitto *context);
صحیح mux_epoll__handle(خالی);
صحیح mux_epoll__cleanup(خالی);

صحیح mux_kqueue__init(خالی);
صحیح mux_kqueue_جوڑو_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
صحیح mux_kqueue__delete_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
صحیح mux_kqueue__new(ساخت mosquitto *context);
صحیح mux_kqueue_جوڑو_برآمد(ساخت mosquitto *context);
صحیح mux_kqueue_remove_برآمد(ساخت mosquitto *context);
صحیح mux_kqueue__delete(ساخت mosquitto *context);
صحیح mux_kqueue__handle(خالی);
صحیح mux_kqueue__cleanup(خالی);

صحیح mux_poll__init(خالی);
صحیح mux_poll_جوڑو_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
صحیح mux_poll__delete_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
صحیح mux_poll__new(ساخت mosquitto *context);
صحیح mux_poll_جوڑو_برآمد(ساخت mosquitto *context);
صحیح mux_poll_remove_برآمد(ساخت mosquitto *context);
صحیح mux_poll__delete(ساخت mosquitto *context);
صحیح mux_poll__handle(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی);
صحیح mux_poll__cleanup(خالی);

#اگر_ختم
