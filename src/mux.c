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

#include "mux.h"


පූර්ණ_සංඛ්‍යා mux__init(void)
{
#ifdef WITH_EPOLL
	ආපසු mux_epoll__init();
#elif defined(WITH_KQUEUE)
	ආපසු mux_kqueue__init();
#නැතහොත්
	ආපසු mux_poll__init();
#endif
}


පූර්ණ_සංඛ්‍යා mux__add_listeners(sa_nracanaa mosquitto__listener_sock *listensock, පූර්ණ_සංඛ්‍යා listensock_count)
{
#ifdef WITH_EPOLL
	ආපසු mux_epoll__add_listeners(listensock, listensock_count);
#elif defined(WITH_KQUEUE)
	ආපසු mux_kqueue__add_listeners(listensock, listensock_count);
#නැතහොත්
	ආපසු mux_poll__add_listeners(listensock, listensock_count);
#endif
}


පූර්ණ_සංඛ්‍යා mux__delete_listeners(sa_nracanaa mosquitto__listener_sock *listensock, පූර්ණ_සංඛ්‍යා listensock_count)
{
#ifdef WITH_EPOLL
	ආපසු mux_epoll__delete_listeners(listensock, listensock_count);
#elif defined(WITH_KQUEUE)
	ආපසු mux_kqueue__delete_listeners(listensock, listensock_count);
#නැතහොත්
	ආපසු mux_poll__delete_listeners(listensock, listensock_count);
#endif
}


පූර්ණ_සංඛ්‍යා mux__add_out(sa_nracanaa mosquitto *context)
{
#ifdef WITH_EPOLL
	ආපසු mux_epoll__add_out(context);
#elif defined(WITH_KQUEUE)
	ආපසු mux_kqueue__add_out(context);
#නැතහොත්
	ආපසු mux_poll__add_out(context);
#endif
}


පූර්ණ_සංඛ්‍යා mux__remove_out(sa_nracanaa mosquitto *context)
{
#ifdef WITH_EPOLL
	ආපසු mux_epoll__remove_out(context);
#elif defined(WITH_KQUEUE)
	ආපසු mux_kqueue__remove_out(context);
#නැතහොත්
	ආපසු mux_poll__remove_out(context);
#endif
}


පූර්ණ_සංඛ්‍යා mux__new(sa_nracanaa mosquitto *context)
{
#ifdef WITH_EPOLL
	ආපසු mux_epoll__new(context);
#elif defined(WITH_KQUEUE)
	ආපසු mux_kqueue__new(context);
#නැතහොත්
	ආපසු mux_poll__new(context);
#endif
}


පූර්ණ_සංඛ්‍යා mux__delete(sa_nracanaa mosquitto *context)
{
#ifdef WITH_EPOLL
	ආපසු mux_epoll__delete(context);
#elif defined(WITH_KQUEUE)
	ආපසු mux_kqueue__delete(context);
#නැතහොත්
	ආපසු mux_poll__delete(context);
#endif
}


පූර්ණ_සංඛ්‍යා mux__handle(sa_nracanaa mosquitto__listener_sock *listensock, පූර්ණ_සංඛ්‍යා listensock_count)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_count);
	ආපසු mux_epoll__handle();
#elif defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_count);
	ආපසු mux_kqueue__handle();
#නැතහොත්
	ආපසු mux_poll__handle(listensock, listensock_count);
#endif
}


පූර්ණ_සංඛ්‍යා mux__cleanup(void)
{
#ifdef WITH_EPOLL
	ආපසු mux_epoll__cleanup();
#elif defined(WITH_KQUEUE)
	ආපසු mux_kqueue__cleanup();
#නැතහොත්
	ආපසු mux_poll__cleanup();
#endif
}
