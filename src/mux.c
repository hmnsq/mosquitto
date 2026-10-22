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

#अन्तर्भाव "mux.h"


पूर्णाङ्क mux__init(शून्य)
{
#यदि_परिभाषितम् WITH_EPOLL
	प्रत्यागम mux_epoll__init();
#नोचेद्यदि defined(WITH_KQUEUE)
	प्रत्यागम mux_kqueue__init();
#अन्यथा
	प्रत्यागम mux_poll__init();
#यदि_अन्तः
}


पूर्णाङ्क mux_सङ्कलय_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
#यदि_परिभाषितम् WITH_EPOLL
	प्रत्यागम mux_epoll_सङ्कलय_listeners(listensock, listensock_गणनम्);
#नोचेद्यदि defined(WITH_KQUEUE)
	प्रत्यागम mux_kqueue_सङ्कलय_listeners(listensock, listensock_गणनम्);
#अन्यथा
	प्रत्यागम mux_poll_सङ्कलय_listeners(listensock, listensock_गणनम्);
#यदि_अन्तः
}


पूर्णाङ्क mux__delete_listeners(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
#यदि_परिभाषितम् WITH_EPOLL
	प्रत्यागम mux_epoll__delete_listeners(listensock, listensock_गणनम्);
#नोचेद्यदि defined(WITH_KQUEUE)
	प्रत्यागम mux_kqueue__delete_listeners(listensock, listensock_गणनम्);
#अन्यथा
	प्रत्यागम mux_poll__delete_listeners(listensock, listensock_गणनम्);
#यदि_अन्तः
}


पूर्णाङ्क mux_सङ्कलय_बहिर्गमनम्(संरचना mosquitto *context)
{
#यदि_परिभाषितम् WITH_EPOLL
	प्रत्यागम mux_epoll_सङ्कलय_बहिर्गमनम्(context);
#नोचेद्यदि defined(WITH_KQUEUE)
	प्रत्यागम mux_kqueue_सङ्कलय_बहिर्गमनम्(context);
#अन्यथा
	प्रत्यागम mux_poll_सङ्कलय_बहिर्गमनम्(context);
#यदि_अन्तः
}


पूर्णाङ्क mux_remove_बहिर्गमनम्(संरचना mosquitto *context)
{
#यदि_परिभाषितम् WITH_EPOLL
	प्रत्यागम mux_epoll_remove_बहिर्गमनम्(context);
#नोचेद्यदि defined(WITH_KQUEUE)
	प्रत्यागम mux_kqueue_remove_बहिर्गमनम्(context);
#अन्यथा
	प्रत्यागम mux_poll_remove_बहिर्गमनम्(context);
#यदि_अन्तः
}


पूर्णाङ्क mux__new(संरचना mosquitto *context)
{
#यदि_परिभाषितम् WITH_EPOLL
	प्रत्यागम mux_epoll__new(context);
#नोचेद्यदि defined(WITH_KQUEUE)
	प्रत्यागम mux_kqueue__new(context);
#अन्यथा
	प्रत्यागम mux_poll__new(context);
#यदि_अन्तः
}


पूर्णाङ्क mux__delete(संरचना mosquitto *context)
{
#यदि_परिभाषितम् WITH_EPOLL
	प्रत्यागम mux_epoll__delete(context);
#नोचेद्यदि defined(WITH_KQUEUE)
	प्रत्यागम mux_kqueue__delete(context);
#अन्यथा
	प्रत्यागम mux_poll__delete(context);
#यदि_अन्तः
}


पूर्णाङ्क mux__handle(संरचना mosquitto__listener_sock *listensock, पूर्णाङ्क listensock_गणनम्)
{
#यदि_परिभाषितम् WITH_EPOLL
	अप्रयुक्तम्(listensock);
	अप्रयुक्तम्(listensock_गणनम्);
	प्रत्यागम mux_epoll__handle();
#नोचेद्यदि defined(WITH_KQUEUE)
	अप्रयुक्तम्(listensock);
	अप्रयुक्तम्(listensock_गणनम्);
	प्रत्यागम mux_kqueue__handle();
#अन्यथा
	प्रत्यागम mux_poll__handle(listensock, listensock_गणनम्);
#यदि_अन्तः
}


पूर्णाङ्क mux__cleanup(शून्य)
{
#यदि_परिभाषितम् WITH_EPOLL
	प्रत्यागम mux_epoll__cleanup();
#नोचेद्यदि defined(WITH_KQUEUE)
	प्रत्यागम mux_kqueue__cleanup();
#अन्यथा
	प्रत्यागम mux_poll__cleanup();
#यदि_अन्तः
}
