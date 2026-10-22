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

#અન્તર્ભાવ "mosquitto_broker_internal.h"

પૂર્ણાંક mux_epoll__init(શૂન્ય);
પૂર્ણાંક mux_epoll__add_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
પૂર્ણાંક mux_epoll__delete_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
પૂર્ણાંક mux_epoll__new(સંરચના mosquitto *context);
પૂર્ણાંક mux_epoll__add_out(સંરચના mosquitto *context);
પૂર્ણાંક mux_epoll__remove_out(સંરચના mosquitto *context);
પૂર્ણાંક mux_epoll__delete(સંરચના mosquitto *context);
પૂર્ણાંક mux_epoll__handle(શૂન્ય);
પૂર્ણાંક mux_epoll__cleanup(શૂન્ય);

પૂર્ણાંક mux_kqueue__init(શૂન્ય);
પૂર્ણાંક mux_kqueue__add_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
પૂર્ણાંક mux_kqueue__delete_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
પૂર્ણાંક mux_kqueue__new(સંરચના mosquitto *context);
પૂર્ણાંક mux_kqueue__add_out(સંરચના mosquitto *context);
પૂર્ણાંક mux_kqueue__remove_out(સંરચના mosquitto *context);
પૂર્ણાંક mux_kqueue__delete(સંરચના mosquitto *context);
પૂર્ણાંક mux_kqueue__handle(શૂન્ય);
પૂર્ણાંક mux_kqueue__cleanup(શૂન્ય);

પૂર્ણાંક mux_poll__init(શૂન્ય);
પૂર્ણાંક mux_poll__add_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
પૂર્ણાંક mux_poll__delete_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
પૂર્ણાંક mux_poll__new(સંરચના mosquitto *context);
પૂર્ણાંક mux_poll__add_out(સંરચના mosquitto *context);
પૂર્ણાંક mux_poll__remove_out(સંરચના mosquitto *context);
પૂર્ણાંક mux_poll__delete(સંરચના mosquitto *context);
પૂર્ણાંક mux_poll__handle(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી);
પૂર્ણાંક mux_poll__cleanup(શૂન્ય);

#endif
