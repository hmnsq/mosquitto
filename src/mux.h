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

#यदि_अपरिभाषितम् MUX_H
#परिभाषय MUX_H

#अन्तर्भाव "mosquitto_broker_internal.h"

पूर्णाङ्क mux_epoll__init(शून्य);
पूर्णाङ्क mux_epoll_सङ्कलय_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux_epoll__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux_epoll__new(संरचना mosquitto *context);
पूर्णाङ्क mux_epoll_सङ्कलय_बहिर्गमनम्(संरचना mosquitto *context);
पूर्णाङ्क mux_epoll_remove_बहिर्गमनम्(संरचना mosquitto *context);
पूर्णाङ्क mux_epoll__delete(संरचना mosquitto *context);
पूर्णाङ्क mux_epoll__handle(शून्य);
पूर्णाङ्क mux_epoll__cleanup(शून्य);

पूर्णाङ्क mux_kqueue__init(शून्य);
पूर्णाङ्क mux_kqueue_सङ्कलय_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux_kqueue__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux_kqueue__new(संरचना mosquitto *context);
पूर्णाङ्क mux_kqueue_सङ्कलय_बहिर्गमनम्(संरचना mosquitto *context);
पूर्णाङ्क mux_kqueue_remove_बहिर्गमनम्(संरचना mosquitto *context);
पूर्णाङ्क mux_kqueue__delete(संरचना mosquitto *context);
पूर्णाङ्क mux_kqueue__handle(शून्य);
पूर्णाङ्क mux_kqueue__cleanup(शून्य);

पूर्णाङ्क mux_poll__init(शून्य);
पूर्णाङ्क mux_poll_सङ्कलय_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux_poll__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux_poll__new(संरचना mosquitto *context);
पूर्णाङ्क mux_poll_सङ्कलय_बहिर्गमनम्(संरचना mosquitto *context);
पूर्णाङ्क mux_poll_remove_बहिर्गमनम्(संरचना mosquitto *context);
पूर्णाङ्क mux_poll__delete(संरचना mosquitto *context);
पूर्णाङ्क mux_poll__handle(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्);
पूर्णाङ्क mux_poll__cleanup(शून्य);

#यदि_अन्तः
