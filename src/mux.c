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

#அந்தர்பாவ "mux.h"


முழு mux__init(வெறுமை)
{
#ifdef WITH_EPOLL
	திரும்பு mux_epoll__init();
#அல்லதெனில் defined(WITH_KQUEUE)
	திரும்பு mux_kqueue__init();
#இல்லையெனில்
	திரும்பு mux_poll__init();
#endif
}


முழு mux__add_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
#ifdef WITH_EPOLL
	திரும்பு mux_epoll__add_listeners(listensock, listensock_மொத்தம்);
#அல்லதெனில் defined(WITH_KQUEUE)
	திரும்பு mux_kqueue__add_listeners(listensock, listensock_மொத்தம்);
#இல்லையெனில்
	திரும்பு mux_poll__add_listeners(listensock, listensock_மொத்தம்);
#endif
}


முழு mux__delete_listeners(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
#ifdef WITH_EPOLL
	திரும்பு mux_epoll__delete_listeners(listensock, listensock_மொத்தம்);
#அல்லதெனில் defined(WITH_KQUEUE)
	திரும்பு mux_kqueue__delete_listeners(listensock, listensock_மொத்தம்);
#இல்லையெனில்
	திரும்பு mux_poll__delete_listeners(listensock, listensock_மொத்தம்);
#endif
}


முழு mux__add_out(கட்டமைப்பு mosquitto *context)
{
#ifdef WITH_EPOLL
	திரும்பு mux_epoll__add_out(context);
#அல்லதெனில் defined(WITH_KQUEUE)
	திரும்பு mux_kqueue__add_out(context);
#இல்லையெனில்
	திரும்பு mux_poll__add_out(context);
#endif
}


முழு mux__remove_out(கட்டமைப்பு mosquitto *context)
{
#ifdef WITH_EPOLL
	திரும்பு mux_epoll__remove_out(context);
#அல்லதெனில் defined(WITH_KQUEUE)
	திரும்பு mux_kqueue__remove_out(context);
#இல்லையெனில்
	திரும்பு mux_poll__remove_out(context);
#endif
}


முழு mux__new(கட்டமைப்பு mosquitto *context)
{
#ifdef WITH_EPOLL
	திரும்பு mux_epoll__new(context);
#அல்லதெனில் defined(WITH_KQUEUE)
	திரும்பு mux_kqueue__new(context);
#இல்லையெனில்
	திரும்பு mux_poll__new(context);
#endif
}


முழு mux__delete(கட்டமைப்பு mosquitto *context)
{
#ifdef WITH_EPOLL
	திரும்பு mux_epoll__delete(context);
#அல்லதெனில் defined(WITH_KQUEUE)
	திரும்பு mux_kqueue__delete(context);
#இல்லையெனில்
	திரும்பு mux_poll__delete(context);
#endif
}


முழு mux__handle(கட்டமைப்பு mosquitto__listener_sock *listensock, முழு listensock_மொத்தம்)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_மொத்தம்);
	திரும்பு mux_epoll__handle();
#அல்லதெனில் defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_மொத்தம்);
	திரும்பு mux_kqueue__handle();
#இல்லையெனில்
	திரும்பு mux_poll__handle(listensock, listensock_மொத்தம்);
#endif
}


முழு mux__cleanup(வெறுமை)
{
#ifdef WITH_EPOLL
	திரும்பு mux_epoll__cleanup();
#அல்லதெனில் defined(WITH_KQUEUE)
	திரும்பு mux_kqueue__cleanup();
#இல்லையெனில்
	திரும்பு mux_poll__cleanup();
#endif
}
