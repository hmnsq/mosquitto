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

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__init(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__add_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__delete_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__new(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__add_out(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__remove_out(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__delete(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__handle(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_epoll__cleanup(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__init(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__add_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__delete_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__new(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__add_out(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__remove_out(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__delete(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__handle(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_kqueue__cleanup(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__init(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__add_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__delete_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__new(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__add_out(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__remove_out(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__delete(ସଂରଚନା mosquitto *context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__handle(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_poll__cleanup(ଶୂନ୍ୟ);

#endif
