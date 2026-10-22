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

#समावेश "mux.h"


पूर्णांक mux__init(व्योम)
{
#अगर_घोषित WITH_EPOLL
	वापस mux_epoll__init();
#या_अगर defined(WITH_KQUEUE)
	वापस mux_kqueue__init();
#अन्यथा
	वापस mux_poll__init();
#पूर्ण_अगर
}


पूर्णांक mux__add_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
#अगर_घोषित WITH_EPOLL
	वापस mux_epoll__add_listeners(listensock, listensock_गिनती);
#या_अगर defined(WITH_KQUEUE)
	वापस mux_kqueue__add_listeners(listensock, listensock_गिनती);
#अन्यथा
	वापस mux_poll__add_listeners(listensock, listensock_गिनती);
#पूर्ण_अगर
}


पूर्णांक mux__delete_listeners(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
#अगर_घोषित WITH_EPOLL
	वापस mux_epoll__delete_listeners(listensock, listensock_गिनती);
#या_अगर defined(WITH_KQUEUE)
	वापस mux_kqueue__delete_listeners(listensock, listensock_गिनती);
#अन्यथा
	वापस mux_poll__delete_listeners(listensock, listensock_गिनती);
#पूर्ण_अगर
}


पूर्णांक mux__add_out(काष्ठा mosquitto *context)
{
#अगर_घोषित WITH_EPOLL
	वापस mux_epoll__add_out(context);
#या_अगर defined(WITH_KQUEUE)
	वापस mux_kqueue__add_out(context);
#अन्यथा
	वापस mux_poll__add_out(context);
#पूर्ण_अगर
}


पूर्णांक mux__remove_out(काष्ठा mosquitto *context)
{
#अगर_घोषित WITH_EPOLL
	वापस mux_epoll__remove_out(context);
#या_अगर defined(WITH_KQUEUE)
	वापस mux_kqueue__remove_out(context);
#अन्यथा
	वापस mux_poll__remove_out(context);
#पूर्ण_अगर
}


पूर्णांक mux__new(काष्ठा mosquitto *context)
{
#अगर_घोषित WITH_EPOLL
	वापस mux_epoll__new(context);
#या_अगर defined(WITH_KQUEUE)
	वापस mux_kqueue__new(context);
#अन्यथा
	वापस mux_poll__new(context);
#पूर्ण_अगर
}


पूर्णांक mux__delete(काष्ठा mosquitto *context)
{
#अगर_घोषित WITH_EPOLL
	वापस mux_epoll__delete(context);
#या_अगर defined(WITH_KQUEUE)
	वापस mux_kqueue__delete(context);
#अन्यथा
	वापस mux_poll__delete(context);
#पूर्ण_अगर
}


पूर्णांक mux__handle(काष्ठा mosquitto__listener_sock *listensock, पूर्णांक listensock_गिनती)
{
#अगर_घोषित WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_गिनती);
	वापस mux_epoll__handle();
#या_अगर defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_गिनती);
	वापस mux_kqueue__handle();
#अन्यथा
	वापस mux_poll__handle(listensock, listensock_गिनती);
#पूर्ण_अगर
}


पूर्णांक mux__cleanup(व्योम)
{
#अगर_घोषित WITH_EPOLL
	वापस mux_epoll__cleanup();
#या_अगर defined(WITH_KQUEUE)
	वापस mux_kqueue__cleanup();
#अन्यथा
	वापस mux_poll__cleanup();
#पूर्ण_अगर
}
