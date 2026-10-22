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
#جيڪڏهن_وصف_ٿيل WITH_EPOLL
	أرجع mux_epoll__init();
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__init();
#وإلا
	أرجع mux_poll__init();
#جيڪڏهن_پورو
}


صحيح mux_جوڙيو_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_ڳڻپ)
{
#جيڪڏهن_وصف_ٿيل WITH_EPOLL
	أرجع mux_epoll_جوڙيو_listeners(listensock, listensock_ڳڻپ);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue_جوڙيو_listeners(listensock, listensock_ڳڻپ);
#وإلا
	أرجع mux_poll_جوڙيو_listeners(listensock, listensock_ڳڻپ);
#جيڪڏهن_پورو
}


صحيح mux__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_ڳڻپ)
{
#جيڪڏهن_وصف_ٿيل WITH_EPOLL
	أرجع mux_epoll__delete_listeners(listensock, listensock_ڳڻپ);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__delete_listeners(listensock, listensock_ڳڻپ);
#وإلا
	أرجع mux_poll__delete_listeners(listensock, listensock_ڳڻپ);
#جيڪڏهن_پورو
}


صحيح mux_جوڙيو_ٻاهريون(بنية mosquitto *context)
{
#جيڪڏهن_وصف_ٿيل WITH_EPOLL
	أرجع mux_epoll_جوڙيو_ٻاهريون(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue_جوڙيو_ٻاهريون(context);
#وإلا
	أرجع mux_poll_جوڙيو_ٻاهريون(context);
#جيڪڏهن_پورو
}


صحيح mux_remove_ٻاهريون(بنية mosquitto *context)
{
#جيڪڏهن_وصف_ٿيل WITH_EPOLL
	أرجع mux_epoll_remove_ٻاهريون(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue_remove_ٻاهريون(context);
#وإلا
	أرجع mux_poll_remove_ٻاهريون(context);
#جيڪڏهن_پورو
}


صحيح mux__new(بنية mosquitto *context)
{
#جيڪڏهن_وصف_ٿيل WITH_EPOLL
	أرجع mux_epoll__new(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__new(context);
#وإلا
	أرجع mux_poll__new(context);
#جيڪڏهن_پورو
}


صحيح mux__delete(بنية mosquitto *context)
{
#جيڪڏهن_وصف_ٿيل WITH_EPOLL
	أرجع mux_epoll__delete(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__delete(context);
#وإلا
	أرجع mux_poll__delete(context);
#جيڪڏهن_پورو
}


صحيح mux__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_ڳڻپ)
{
#جيڪڏهن_وصف_ٿيل WITH_EPOLL
	اڻ_استعمال_ٿيل(listensock);
	اڻ_استعمال_ٿيل(listensock_ڳڻپ);
	أرجع mux_epoll__handle();
#وإلاإذا defined(WITH_KQUEUE)
	اڻ_استعمال_ٿيل(listensock);
	اڻ_استعمال_ٿيل(listensock_ڳڻپ);
	أرجع mux_kqueue__handle();
#وإلا
	أرجع mux_poll__handle(listensock, listensock_ڳڻپ);
#جيڪڏهن_پورو
}


صحيح mux__cleanup(فراغ)
{
#جيڪڏهن_وصف_ٿيل WITH_EPOLL
	أرجع mux_epoll__cleanup();
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__cleanup();
#وإلا
	أرجع mux_poll__cleanup();
#جيڪڏهن_پورو
}
