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

#నిర్వచితం_కాకపోతే MUX_H
#నిర్వచించు MUX_H

#అన్తర్భావ "mosquitto_broker_internal.h"

పూర్ణాంకం mux_epoll__init(శూన్య);
పూర్ణాంకం mux_epoll_కూడు_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
పూర్ణాంకం mux_epoll__delete_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
పూర్ణాంకం mux_epoll__new(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_epoll_కూడు_బహిర్గతం(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_epoll_remove_బహిర్గతం(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_epoll__delete(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_epoll__handle(శూన్య);
పూర్ణాంకం mux_epoll__cleanup(శూన్య);

పూర్ణాంకం mux_kqueue__init(శూన్య);
పూర్ణాంకం mux_kqueue_కూడు_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
పూర్ణాంకం mux_kqueue__delete_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
పూర్ణాంకం mux_kqueue__new(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_kqueue_కూడు_బహిర్గతం(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_kqueue_remove_బహిర్గతం(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_kqueue__delete(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_kqueue__handle(శూన్య);
పూర్ణాంకం mux_kqueue__cleanup(శూన్య);

పూర్ణాంకం mux_poll__init(శూన్య);
పూర్ణాంకం mux_poll_కూడు_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
పూర్ణాంకం mux_poll__delete_listeners(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
పూర్ణాంకం mux_poll__new(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_poll_కూడు_బహిర్గతం(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_poll_remove_బహిర్గతం(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_poll__delete(నిర్మాణం mosquitto *context);
పూర్ణాంకం mux_poll__handle(నిర్మాణం mosquitto__listener_sock *listensock, పూర్ణాంకం listensock_లెక్క);
పూర్ణాంకం mux_poll__cleanup(శూన్య);

#అయితే_ముగింపు
