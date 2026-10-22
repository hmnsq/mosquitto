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

#شامل "mux.h"


صحیح mux__init(خالی)
{
#اگر_تعریف_شدہ WITH_EPOLL
	واپس mux_epoll__init();
#وگرنہ_اگر defined(WITH_KQUEUE)
	واپس mux_kqueue__init();
#ورنہ
	واپس mux_poll__init();
#اگر_ختم
}


صحیح mux_جوڑو_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
#اگر_تعریف_شدہ WITH_EPOLL
	واپس mux_epoll_جوڑو_listeners(listensock, listensock_گنتی);
#وگرنہ_اگر defined(WITH_KQUEUE)
	واپس mux_kqueue_جوڑو_listeners(listensock, listensock_گنتی);
#ورنہ
	واپس mux_poll_جوڑو_listeners(listensock, listensock_گنتی);
#اگر_ختم
}


صحیح mux__delete_listeners(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
#اگر_تعریف_شدہ WITH_EPOLL
	واپس mux_epoll__delete_listeners(listensock, listensock_گنتی);
#وگرنہ_اگر defined(WITH_KQUEUE)
	واپس mux_kqueue__delete_listeners(listensock, listensock_گنتی);
#ورنہ
	واپس mux_poll__delete_listeners(listensock, listensock_گنتی);
#اگر_ختم
}


صحیح mux_جوڑو_برآمد(ساخت mosquitto *context)
{
#اگر_تعریف_شدہ WITH_EPOLL
	واپس mux_epoll_جوڑو_برآمد(context);
#وگرنہ_اگر defined(WITH_KQUEUE)
	واپس mux_kqueue_جوڑو_برآمد(context);
#ورنہ
	واپس mux_poll_جوڑو_برآمد(context);
#اگر_ختم
}


صحیح mux_remove_برآمد(ساخت mosquitto *context)
{
#اگر_تعریف_شدہ WITH_EPOLL
	واپس mux_epoll_remove_برآمد(context);
#وگرنہ_اگر defined(WITH_KQUEUE)
	واپس mux_kqueue_remove_برآمد(context);
#ورنہ
	واپس mux_poll_remove_برآمد(context);
#اگر_ختم
}


صحیح mux__new(ساخت mosquitto *context)
{
#اگر_تعریف_شدہ WITH_EPOLL
	واپس mux_epoll__new(context);
#وگرنہ_اگر defined(WITH_KQUEUE)
	واپس mux_kqueue__new(context);
#ورنہ
	واپس mux_poll__new(context);
#اگر_ختم
}


صحیح mux__delete(ساخت mosquitto *context)
{
#اگر_تعریف_شدہ WITH_EPOLL
	واپس mux_epoll__delete(context);
#وگرنہ_اگر defined(WITH_KQUEUE)
	واپس mux_kqueue__delete(context);
#ورنہ
	واپس mux_poll__delete(context);
#اگر_ختم
}


صحیح mux__handle(ساخت mosquitto__listener_sock *listensock, صحیح listensock_گنتی)
{
#اگر_تعریف_شدہ WITH_EPOLL
	غیر_مستعمل(listensock);
	غیر_مستعمل(listensock_گنتی);
	واپس mux_epoll__handle();
#وگرنہ_اگر defined(WITH_KQUEUE)
	غیر_مستعمل(listensock);
	غیر_مستعمل(listensock_گنتی);
	واپس mux_kqueue__handle();
#ورنہ
	واپس mux_poll__handle(listensock, listensock_گنتی);
#اگر_ختم
}


صحیح mux__cleanup(خالی)
{
#اگر_تعریف_شدہ WITH_EPOLL
	واپس mux_epoll__cleanup();
#وگرنہ_اگر defined(WITH_KQUEUE)
	واپس mux_kqueue__cleanup();
#ورنہ
	واپس mux_poll__cleanup();
#اگر_ختم
}
