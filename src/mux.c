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

#ಅನ್ತರ್ಭಾವ "mux.h"


ಪೂರ್ಣಾಙ್ಕ mux__init(ಶೂನ್ಯ)
{
#ifdef WITH_EPOLL
	ಪ್ರತ್ಯಾಗಮ mux_epoll__init();
#ನೋಚೇದ್ಯದಿ defined(WITH_KQUEUE)
	ಪ್ರತ್ಯಾಗಮ mux_kqueue__init();
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mux_poll__init();
#endif
}


ಪೂರ್ಣಾಙ್ಕ mux__add_listeners(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
#ifdef WITH_EPOLL
	ಪ್ರತ್ಯಾಗಮ mux_epoll__add_listeners(listensock, listensock_ಎಣಿಕೆ);
#ನೋಚೇದ್ಯದಿ defined(WITH_KQUEUE)
	ಪ್ರತ್ಯಾಗಮ mux_kqueue__add_listeners(listensock, listensock_ಎಣಿಕೆ);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mux_poll__add_listeners(listensock, listensock_ಎಣಿಕೆ);
#endif
}


ಪೂರ್ಣಾಙ್ಕ mux__delete_listeners(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
#ifdef WITH_EPOLL
	ಪ್ರತ್ಯಾಗಮ mux_epoll__delete_listeners(listensock, listensock_ಎಣಿಕೆ);
#ನೋಚೇದ್ಯದಿ defined(WITH_KQUEUE)
	ಪ್ರತ್ಯಾಗಮ mux_kqueue__delete_listeners(listensock, listensock_ಎಣಿಕೆ);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mux_poll__delete_listeners(listensock, listensock_ಎಣಿಕೆ);
#endif
}


ಪೂರ್ಣಾಙ್ಕ mux__add_out(ಸಂರಚನಾ mosquitto *context)
{
#ifdef WITH_EPOLL
	ಪ್ರತ್ಯಾಗಮ mux_epoll__add_out(context);
#ನೋಚೇದ್ಯದಿ defined(WITH_KQUEUE)
	ಪ್ರತ್ಯಾಗಮ mux_kqueue__add_out(context);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mux_poll__add_out(context);
#endif
}


ಪೂರ್ಣಾಙ್ಕ mux__remove_out(ಸಂರಚನಾ mosquitto *context)
{
#ifdef WITH_EPOLL
	ಪ್ರತ್ಯಾಗಮ mux_epoll__remove_out(context);
#ನೋಚೇದ್ಯದಿ defined(WITH_KQUEUE)
	ಪ್ರತ್ಯಾಗಮ mux_kqueue__remove_out(context);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mux_poll__remove_out(context);
#endif
}


ಪೂರ್ಣಾಙ್ಕ mux__new(ಸಂರಚನಾ mosquitto *context)
{
#ifdef WITH_EPOLL
	ಪ್ರತ್ಯಾಗಮ mux_epoll__new(context);
#ನೋಚೇದ್ಯದಿ defined(WITH_KQUEUE)
	ಪ್ರತ್ಯಾಗಮ mux_kqueue__new(context);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mux_poll__new(context);
#endif
}


ಪೂರ್ಣಾಙ್ಕ mux__delete(ಸಂರಚನಾ mosquitto *context)
{
#ifdef WITH_EPOLL
	ಪ್ರತ್ಯಾಗಮ mux_epoll__delete(context);
#ನೋಚೇದ್ಯದಿ defined(WITH_KQUEUE)
	ಪ್ರತ್ಯಾಗಮ mux_kqueue__delete(context);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mux_poll__delete(context);
#endif
}


ಪೂರ್ಣಾಙ್ಕ mux__handle(ಸಂರಚನಾ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಙ್ಕ listensock_ಎಣಿಕೆ)
{
#ifdef WITH_EPOLL
	UNUSED(listensock);
	UNUSED(listensock_ಎಣಿಕೆ);
	ಪ್ರತ್ಯಾಗಮ mux_epoll__handle();
#ನೋಚೇದ್ಯದಿ defined(WITH_KQUEUE)
	UNUSED(listensock);
	UNUSED(listensock_ಎಣಿಕೆ);
	ಪ್ರತ್ಯಾಗಮ mux_kqueue__handle();
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mux_poll__handle(listensock, listensock_ಎಣಿಕೆ);
#endif
}


ಪೂರ್ಣಾಙ್ಕ mux__cleanup(ಶೂನ್ಯ)
{
#ifdef WITH_EPOLL
	ಪ್ರತ್ಯಾಗಮ mux_epoll__cleanup();
#ನೋಚೇದ್ಯದಿ defined(WITH_KQUEUE)
	ಪ್ರತ್ಯಾಗಮ mux_kqueue__cleanup();
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ mux_poll__cleanup();
#endif
}
