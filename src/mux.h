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

#അന്തര്ഭാവ "mosquitto_broker_internal.h"

പൂർണ്ണസംഖ്യ mux_epoll__init(ശൂന്യം);
പൂർണ്ണസംഖ്യ mux_epoll__add_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
പൂർണ്ണസംഖ്യ mux_epoll__delete_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
പൂർണ്ണസംഖ്യ mux_epoll__new(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_epoll__add_out(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_epoll__remove_out(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_epoll__delete(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_epoll__handle(ശൂന്യം);
പൂർണ്ണസംഖ്യ mux_epoll__cleanup(ശൂന്യം);

പൂർണ്ണസംഖ്യ mux_kqueue__init(ശൂന്യം);
പൂർണ്ണസംഖ്യ mux_kqueue__add_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
പൂർണ്ണസംഖ്യ mux_kqueue__delete_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
പൂർണ്ണസംഖ്യ mux_kqueue__new(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_kqueue__add_out(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_kqueue__remove_out(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_kqueue__delete(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_kqueue__handle(ശൂന്യം);
പൂർണ്ണസംഖ്യ mux_kqueue__cleanup(ശൂന്യം);

പൂർണ്ണസംഖ്യ mux_poll__init(ശൂന്യം);
പൂർണ്ണസംഖ്യ mux_poll__add_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
പൂർണ്ണസംഖ്യ mux_poll__delete_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
പൂർണ്ണസംഖ്യ mux_poll__new(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_poll__add_out(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_poll__remove_out(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_poll__delete(ഘടന mosquitto *context);
പൂർണ്ണസംഖ്യ mux_poll__handle(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം);
പൂർണ്ണസംഖ്യ mux_poll__cleanup(ശൂന്യം);

#endif
