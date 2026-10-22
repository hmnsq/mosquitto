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

#अगर_अघोषित MUX_H
#घोषणा MUX_H

#समावेश "mosquitto_broker_internal.h"

पूर्णांक mux_epoll__init(व्योम);
पूर्णांक mux_epoll__add_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
पूर्णांक mux_epoll__delete_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
पूर्णांक mux_epoll__new(काष्ठा mosquitto *context);
पूर्णांक mux_epoll__add_out(काष्ठा mosquitto *context);
पूर्णांक mux_epoll__remove_out(काष्ठा mosquitto *context);
पूर्णांक mux_epoll__delete(काष्ठा mosquitto *context);
पूर्णांक mux_epoll__handle(व्योम);
पूर्णांक mux_epoll__cleanup(व्योम);

पूर्णांक mux_kqueue__init(व्योम);
पूर्णांक mux_kqueue__add_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
पूर्णांक mux_kqueue__delete_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
पूर्णांक mux_kqueue__new(काष्ठा mosquitto *context);
पूर्णांक mux_kqueue__add_out(काष्ठा mosquitto *context);
पूर्णांक mux_kqueue__remove_out(काष्ठा mosquitto *context);
पूर्णांक mux_kqueue__delete(काष्ठा mosquitto *context);
पूर्णांक mux_kqueue__handle(व्योम);
पूर्णांक mux_kqueue__cleanup(व्योम);

पूर्णांक mux_poll__init(व्योम);
पूर्णांक mux_poll__add_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
पूर्णांक mux_poll__delete_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
पूर्णांक mux_poll__new(काष्ठा mosquitto *context);
पूर्णांक mux_poll__add_out(काष्ठा mosquitto *context);
पूर्णांक mux_poll__remove_out(काष्ठा mosquitto *context);
पूर्णांक mux_poll__delete(काष्ठा mosquitto *context);
पूर्णांक mux_poll__handle(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती);
पूर्णांक mux_poll__cleanup(व्योम);

#पूर्ण_अगर
