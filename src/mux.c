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

#અન્તર્ભાવ "mux.h"


પૂર્ણાંક mux__init(શૂન્ય)
{
#ifdef WITH_EPOLL
	પાછા mux_epoll__init();
#અથવાજો defined(WITH_KQUEUE)
	પાછા mux_kqueue__init();
#નહીંતર
	પાછા mux_poll__init();
#endif
}


પૂર્ણાંક mux__add_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
#ifdef WITH_EPOLL
	પાછા mux_epoll__add_listeners(listensock, listensock_ગણતરી);
#અથવાજો defined(WITH_KQUEUE)
	પાછા mux_kqueue__add_listeners(listensock, listensock_ગણતરી);
#નહીંતર
	પાછા mux_poll__add_listeners(listensock, listensock_ગણતરી);
#endif
}


પૂર્ણાંક mux__delete_listeners(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
#ifdef WITH_EPOLL
	પાછા mux_epoll__delete_listeners(listensock, listensock_ગણતરી);
#અથવાજો defined(WITH_KQUEUE)
	પાછા mux_kqueue__delete_listeners(listensock, listensock_ગણતરી);
#નહીંતર
	પાછા mux_poll__delete_listeners(listensock, listensock_ગણતરી);
#endif
}


પૂર્ણાંક mux__add_out(સંરચના mosquitto *context)
{
#ifdef WITH_EPOLL
	પાછા mux_epoll__add_out(context);
#અથવાજો defined(WITH_KQUEUE)
	પાછા mux_kqueue__add_out(context);
#નહીંતર
	પાછા mux_poll__add_out(context);
#endif
}


પૂર્ણાંક mux__remove_out(સંરચના mosquitto *context)
{
#ifdef WITH_EPOLL
	પાછા mux_epoll__remove_out(context);
#અથવાજો defined(WITH_KQUEUE)
	પાછા mux_kqueue__remove_out(context);
#નહીંતર
	પાછા mux_poll__remove_out(context);
#endif
}


પૂર્ણાંક mux__new(સંરચના mosquitto *context)
{
#ifdef WITH_EPOLL
	પાછા mux_epoll__new(context);
#અથવાજો defined(WITH_KQUEUE)
	પાછા mux_kqueue__new(context);
#નહીંતર
	પાછા mux_poll__new(context);
#endif
}


પૂર્ણાંક mux__delete(સંરચના mosquitto *context)
{
#ifdef WITH_EPOLL
	પાછા mux_epoll__delete(context);
#અથવાજો defined(WITH_KQUEUE)
	પાછા mux_kqueue__delete(context);
#નહીંતર
	પાછા mux_poll__delete(context);
#endif
}


પૂર્ણાંક mux__handle(સંરચના mosquitto__listener_sock *listensock, પૂર્ણાંક listensock_ગણતરી)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_ગણતરી);
	પાછા mux_epoll__handle();
#અથવાજો defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_ગણતરી);
	પાછા mux_kqueue__handle();
#નહીંતર
	પાછા mux_poll__handle(listensock, listensock_ગણતરી);
#endif
}


પૂર્ણાંક mux__cleanup(શૂન્ય)
{
#ifdef WITH_EPOLL
	પાછા mux_epoll__cleanup();
#અથવાજો defined(WITH_KQUEUE)
	પાછા mux_kqueue__cleanup();
#નહીંતર
	પાછા mux_poll__cleanup();
#endif
}
