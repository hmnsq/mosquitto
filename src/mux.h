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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MUX_H
#ವ್ಯಾಖ್ಯಾನಿಸು MUX_H

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"

ಪೂರ್ಣಾಂಕ mux_epoll__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mux_epoll_ಕೂಡಿಸು_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಂಕ mux_epoll__delete_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಂಕ mux_epoll__new(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_epoll_ಕೂಡಿಸು_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_epoll_remove_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_epoll__delete(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_epoll__handle(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mux_epoll__cleanup(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ mux_kqueue__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mux_kqueue_ಕೂಡಿಸು_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಂಕ mux_kqueue__delete_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಂಕ mux_kqueue__new(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_kqueue_ಕೂಡಿಸು_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_kqueue_remove_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_kqueue__delete(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_kqueue__handle(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mux_kqueue__cleanup(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ mux_poll__init(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ mux_poll_ಕೂಡಿಸು_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಂಕ mux_poll__delete_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಂಕ mux_poll__new(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_poll_ಕೂಡಿಸು_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_poll_remove_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_poll__delete(ರಚನೆ mosquitto *context);
ಪೂರ್ಣಾಂಕ mux_poll__handle(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ);
ಪೂರ್ಣಾಂಕ mux_poll__cleanup(ಶೂನ್ಯ);

#ಆದರೆ_ಅಂತ್ಯ
