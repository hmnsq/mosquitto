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

#ضمن "mux.h"


صحيح mux__init(فراغ)
{
#ifdef WITH_EPOLL
	ܦܢܐ mux_epoll__init();
#وإلاإذا defined(WITH_KQUEUE)
	ܦܢܐ mux_kqueue__init();
#ܐܚܪܢܐ
	ܦܢܐ mux_poll__init();
#endif
}


صحيح mux__add_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_count)
{
#ifdef WITH_EPOLL
	ܦܢܐ mux_epoll__add_listeners(listensock, listensock_count);
#وإلاإذا defined(WITH_KQUEUE)
	ܦܢܐ mux_kqueue__add_listeners(listensock, listensock_count);
#ܐܚܪܢܐ
	ܦܢܐ mux_poll__add_listeners(listensock, listensock_count);
#endif
}


صحيح mux__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_count)
{
#ifdef WITH_EPOLL
	ܦܢܐ mux_epoll__delete_listeners(listensock, listensock_count);
#وإلاإذا defined(WITH_KQUEUE)
	ܦܢܐ mux_kqueue__delete_listeners(listensock, listensock_count);
#ܐܚܪܢܐ
	ܦܢܐ mux_poll__delete_listeners(listensock, listensock_count);
#endif
}


صحيح mux__add_out(بنية mosquitto *context)
{
#ifdef WITH_EPOLL
	ܦܢܐ mux_epoll__add_out(context);
#وإلاإذا defined(WITH_KQUEUE)
	ܦܢܐ mux_kqueue__add_out(context);
#ܐܚܪܢܐ
	ܦܢܐ mux_poll__add_out(context);
#endif
}


صحيح mux__remove_out(بنية mosquitto *context)
{
#ifdef WITH_EPOLL
	ܦܢܐ mux_epoll__remove_out(context);
#وإلاإذا defined(WITH_KQUEUE)
	ܦܢܐ mux_kqueue__remove_out(context);
#ܐܚܪܢܐ
	ܦܢܐ mux_poll__remove_out(context);
#endif
}


صحيح mux__new(بنية mosquitto *context)
{
#ifdef WITH_EPOLL
	ܦܢܐ mux_epoll__new(context);
#وإلاإذا defined(WITH_KQUEUE)
	ܦܢܐ mux_kqueue__new(context);
#ܐܚܪܢܐ
	ܦܢܐ mux_poll__new(context);
#endif
}


صحيح mux__delete(بنية mosquitto *context)
{
#ifdef WITH_EPOLL
	ܦܢܐ mux_epoll__delete(context);
#وإلاإذا defined(WITH_KQUEUE)
	ܦܢܐ mux_kqueue__delete(context);
#ܐܚܪܢܐ
	ܦܢܐ mux_poll__delete(context);
#endif
}


صحيح mux__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_count)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_count);
	ܦܢܐ mux_epoll__handle();
#وإلاإذا defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_count);
	ܦܢܐ mux_kqueue__handle();
#ܐܚܪܢܐ
	ܦܢܐ mux_poll__handle(listensock, listensock_count);
#endif
}


صحيح mux__cleanup(فراغ)
{
#ifdef WITH_EPOLL
	ܦܢܐ mux_epoll__cleanup();
#وإلاإذا defined(WITH_KQUEUE)
	ܦܢܐ mux_kqueue__cleanup();
#ܐܚܪܢܐ
	ܦܢܐ mux_poll__cleanup();
#endif
}
