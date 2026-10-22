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
#جے_تعریف_شدہ WITH_EPOLL
	أرجع mux_epoll__init();
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__init();
#وإلا
	أرجع mux_poll__init();
#جے_انت
}


صحيح mux_جوڑو_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_گنتی)
{
#جے_تعریف_شدہ WITH_EPOLL
	أرجع mux_epoll_جوڑو_listeners(listensock, listensock_گنتی);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue_جوڑو_listeners(listensock, listensock_گنتی);
#وإلا
	أرجع mux_poll_جوڑو_listeners(listensock, listensock_گنتی);
#جے_انت
}


صحيح mux__delete_listeners(بنية mosquitto__listener_sock *listensock, صحيح listensock_گنتی)
{
#جے_تعریف_شدہ WITH_EPOLL
	أرجع mux_epoll__delete_listeners(listensock, listensock_گنتی);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__delete_listeners(listensock, listensock_گنتی);
#وإلا
	أرجع mux_poll__delete_listeners(listensock, listensock_گنتی);
#جے_انت
}


صحيح mux_جوڑو_باہرلا(بنية mosquitto *context)
{
#جے_تعریف_شدہ WITH_EPOLL
	أرجع mux_epoll_جوڑو_باہرلا(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue_جوڑو_باہرلا(context);
#وإلا
	أرجع mux_poll_جوڑو_باہرلا(context);
#جے_انت
}


صحيح mux_remove_باہرلا(بنية mosquitto *context)
{
#جے_تعریف_شدہ WITH_EPOLL
	أرجع mux_epoll_remove_باہرلا(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue_remove_باہرلا(context);
#وإلا
	أرجع mux_poll_remove_باہرلا(context);
#جے_انت
}


صحيح mux__new(بنية mosquitto *context)
{
#جے_تعریف_شدہ WITH_EPOLL
	أرجع mux_epoll__new(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__new(context);
#وإلا
	أرجع mux_poll__new(context);
#جے_انت
}


صحيح mux__delete(بنية mosquitto *context)
{
#جے_تعریف_شدہ WITH_EPOLL
	أرجع mux_epoll__delete(context);
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__delete(context);
#وإلا
	أرجع mux_poll__delete(context);
#جے_انت
}


صحيح mux__handle(بنية mosquitto__listener_sock *listensock, صحيح listensock_گنتی)
{
#جے_تعریف_شدہ WITH_EPOLL
	بن_ورتیا(listensock);
	بن_ورتیا(listensock_گنتی);
	أرجع mux_epoll__handle();
#وإلاإذا defined(WITH_KQUEUE)
	بن_ورتیا(listensock);
	بن_ورتیا(listensock_گنتی);
	أرجع mux_kqueue__handle();
#وإلا
	أرجع mux_poll__handle(listensock, listensock_گنتی);
#جے_انت
}


صحيح mux__cleanup(فراغ)
{
#جے_تعریف_شدہ WITH_EPOLL
	أرجع mux_epoll__cleanup();
#وإلاإذا defined(WITH_KQUEUE)
	أرجع mux_kqueue__cleanup();
#وإلا
	أرجع mux_poll__cleanup();
#جے_انت
}
