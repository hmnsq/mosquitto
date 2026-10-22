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

#അന്തര്ഭാവ "mux.h"


പൂർണ്ണസംഖ്യ mux__init(ശൂന്യം)
{
#ifdef WITH_EPOLL
	തിരികെ mux_epoll__init();
#അല്ലെങ്കിലെങ്കിൽ defined(WITH_KQUEUE)
	തിരികെ mux_kqueue__init();
#അല്ലെങ്കിൽ
	തിരികെ mux_poll__init();
#endif
}


പൂർണ്ണസംഖ്യ mux__add_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
#ifdef WITH_EPOLL
	തിരികെ mux_epoll__add_listeners(listensock, listensock_എണ്ണം);
#അല്ലെങ്കിലെങ്കിൽ defined(WITH_KQUEUE)
	തിരികെ mux_kqueue__add_listeners(listensock, listensock_എണ്ണം);
#അല്ലെങ്കിൽ
	തിരികെ mux_poll__add_listeners(listensock, listensock_എണ്ണം);
#endif
}


പൂർണ്ണസംഖ്യ mux__delete_listeners(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
#ifdef WITH_EPOLL
	തിരികെ mux_epoll__delete_listeners(listensock, listensock_എണ്ണം);
#അല്ലെങ്കിലെങ്കിൽ defined(WITH_KQUEUE)
	തിരികെ mux_kqueue__delete_listeners(listensock, listensock_എണ്ണം);
#അല്ലെങ്കിൽ
	തിരികെ mux_poll__delete_listeners(listensock, listensock_എണ്ണം);
#endif
}


പൂർണ്ണസംഖ്യ mux__add_out(ഘടന mosquitto *context)
{
#ifdef WITH_EPOLL
	തിരികെ mux_epoll__add_out(context);
#അല്ലെങ്കിലെങ്കിൽ defined(WITH_KQUEUE)
	തിരികെ mux_kqueue__add_out(context);
#അല്ലെങ്കിൽ
	തിരികെ mux_poll__add_out(context);
#endif
}


പൂർണ്ണസംഖ്യ mux__remove_out(ഘടന mosquitto *context)
{
#ifdef WITH_EPOLL
	തിരികെ mux_epoll__remove_out(context);
#അല്ലെങ്കിലെങ്കിൽ defined(WITH_KQUEUE)
	തിരികെ mux_kqueue__remove_out(context);
#അല്ലെങ്കിൽ
	തിരികെ mux_poll__remove_out(context);
#endif
}


പൂർണ്ണസംഖ്യ mux__new(ഘടന mosquitto *context)
{
#ifdef WITH_EPOLL
	തിരികെ mux_epoll__new(context);
#അല്ലെങ്കിലെങ്കിൽ defined(WITH_KQUEUE)
	തിരികെ mux_kqueue__new(context);
#അല്ലെങ്കിൽ
	തിരികെ mux_poll__new(context);
#endif
}


പൂർണ്ണസംഖ്യ mux__delete(ഘടന mosquitto *context)
{
#ifdef WITH_EPOLL
	തിരികെ mux_epoll__delete(context);
#അല്ലെങ്കിലെങ്കിൽ defined(WITH_KQUEUE)
	തിരികെ mux_kqueue__delete(context);
#അല്ലെങ്കിൽ
	തിരികെ mux_poll__delete(context);
#endif
}


പൂർണ്ണസംഖ്യ mux__handle(ഘടന mosquitto__listener_sock *listensock, പൂർണ്ണസംഖ്യ listensock_എണ്ണം)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_എണ്ണം);
	തിരികെ mux_epoll__handle();
#അല്ലെങ്കിലെങ്കിൽ defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_എണ്ണം);
	തിരികെ mux_kqueue__handle();
#അല്ലെങ്കിൽ
	തിരികെ mux_poll__handle(listensock, listensock_എണ്ണം);
#endif
}


പൂർണ്ണസംഖ്യ mux__cleanup(ശൂന്യം)
{
#ifdef WITH_EPOLL
	തിരികെ mux_epoll__cleanup();
#അല്ലെങ്കിലെങ്കിൽ defined(WITH_KQUEUE)
	തിരികെ mux_kqueue__cleanup();
#അല്ലെങ്കിൽ
	തിരികെ mux_poll__cleanup();
#endif
}
