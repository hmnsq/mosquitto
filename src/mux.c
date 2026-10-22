/*
Copyright (c) 2009-2021 Roger Light <roger@atchoo.org>

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
   Tatsuzo Osawa - Add epoll.
*/

#ଅନ୍ତର୍ଭାଵ "mux.h"


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__init(ଶୂନ୍ୟ)
{
#ifdef WITH_EPOLL
	ଫେରନ୍ତୁ mux_epoll__init();
#ନଚେତଯଦି defined(WITH_KQUEUE)
	ଫେରନ୍ତୁ mux_kqueue__init();
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mux_poll__init();
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__add_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
#ifdef WITH_EPOLL
	ଫେରନ୍ତୁ mux_epoll__add_listeners(listensock, listensock_ଗଣନା);
#ନଚେତଯଦି defined(WITH_KQUEUE)
	ଫେରନ୍ତୁ mux_kqueue__add_listeners(listensock, listensock_ଗଣନା);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mux_poll__add_listeners(listensock, listensock_ଗଣନା);
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__delete_listeners(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
#ifdef WITH_EPOLL
	ଫେରନ୍ତୁ mux_epoll__delete_listeners(listensock, listensock_ଗଣନା);
#ନଚେତଯଦି defined(WITH_KQUEUE)
	ଫେରନ୍ତୁ mux_kqueue__delete_listeners(listensock, listensock_ଗଣନା);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mux_poll__delete_listeners(listensock, listensock_ଗଣନା);
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__add_out(ସଂରଚନା mosquitto *context)
{
#ifdef WITH_EPOLL
	ଫେରନ୍ତୁ mux_epoll__add_out(context);
#ନଚେତଯଦି defined(WITH_KQUEUE)
	ଫେରନ୍ତୁ mux_kqueue__add_out(context);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mux_poll__add_out(context);
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__remove_out(ସଂରଚନା mosquitto *context)
{
#ifdef WITH_EPOLL
	ଫେରନ୍ତୁ mux_epoll__remove_out(context);
#ନଚେତଯଦି defined(WITH_KQUEUE)
	ଫେରନ୍ତୁ mux_kqueue__remove_out(context);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mux_poll__remove_out(context);
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__new(ସଂରଚନା mosquitto *context)
{
#ifdef WITH_EPOLL
	ଫେରନ୍ତୁ mux_epoll__new(context);
#ନଚେତଯଦି defined(WITH_KQUEUE)
	ଫେରନ୍ତୁ mux_kqueue__new(context);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mux_poll__new(context);
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__delete(ସଂରଚନା mosquitto *context)
{
#ifdef WITH_EPOLL
	ଫେରନ୍ତୁ mux_epoll__delete(context);
#ନଚେତଯଦି defined(WITH_KQUEUE)
	ଫେରନ୍ତୁ mux_kqueue__delete(context);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mux_poll__delete(context);
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__handle(ସଂରଚନା mosquitto__listener_sock *listensock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା listensock_ଗଣନା)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_ଗଣନା);
	ଫେରନ୍ତୁ mux_epoll__handle();
#ନଚେତଯଦି defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_ଗଣନା);
	ଫେରନ୍ତୁ mux_kqueue__handle();
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mux_poll__handle(listensock, listensock_ଗଣନା);
#endif
}


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux__cleanup(ଶୂନ୍ୟ)
{
#ifdef WITH_EPOLL
	ଫେରନ୍ତୁ mux_epoll__cleanup();
#ନଚେତଯଦି defined(WITH_KQUEUE)
	ଫେରନ୍ତୁ mux_kqueue__cleanup();
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ mux_poll__cleanup();
#endif
}
