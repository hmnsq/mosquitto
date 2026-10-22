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


ಪೂರ್ಣಾಂಕ mux__init(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL
	ಹಿಂತಿರುಗಿ mux_epoll__init();
#ಅಥವಾಆದರೆ defined(WITH_KQUEUE)
	ಹಿಂತಿರುಗಿ mux_kqueue__init();
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mux_poll__init();
#ಆದರೆ_ಅಂತ್ಯ
}


ಪೂರ್ಣಾಂಕ mux_ಕೂಡಿಸು_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL
	ಹಿಂತಿರುಗಿ mux_epoll_ಕೂಡಿಸು_listeners(listensock, listensock_ಎಣಿಕೆ);
#ಅಥವಾಆದರೆ defined(WITH_KQUEUE)
	ಹಿಂತಿರುಗಿ mux_kqueue_ಕೂಡಿಸು_listeners(listensock, listensock_ಎಣಿಕೆ);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mux_poll_ಕೂಡಿಸು_listeners(listensock, listensock_ಎಣಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ
}


ಪೂರ್ಣಾಂಕ mux__delete_listeners(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL
	ಹಿಂತಿರುಗಿ mux_epoll__delete_listeners(listensock, listensock_ಎಣಿಕೆ);
#ಅಥವಾಆದರೆ defined(WITH_KQUEUE)
	ಹಿಂತಿರುಗಿ mux_kqueue__delete_listeners(listensock, listensock_ಎಣಿಕೆ);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mux_poll__delete_listeners(listensock, listensock_ಎಣಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ
}


ಪೂರ್ಣಾಂಕ mux_ಕೂಡಿಸು_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL
	ಹಿಂತಿರುಗಿ mux_epoll_ಕೂಡಿಸು_ಹೊರಹೋಗು(context);
#ಅಥವಾಆದರೆ defined(WITH_KQUEUE)
	ಹಿಂತಿರುಗಿ mux_kqueue_ಕೂಡಿಸು_ಹೊರಹೋಗು(context);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mux_poll_ಕೂಡಿಸು_ಹೊರಹೋಗು(context);
#ಆದರೆ_ಅಂತ್ಯ
}


ಪೂರ್ಣಾಂಕ mux_remove_ಹೊರಹೋಗು(ರಚನೆ mosquitto *context)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL
	ಹಿಂತಿರುಗಿ mux_epoll_remove_ಹೊರಹೋಗು(context);
#ಅಥವಾಆದರೆ defined(WITH_KQUEUE)
	ಹಿಂತಿರುಗಿ mux_kqueue_remove_ಹೊರಹೋಗು(context);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mux_poll_remove_ಹೊರಹೋಗು(context);
#ಆದರೆ_ಅಂತ್ಯ
}


ಪೂರ್ಣಾಂಕ mux__new(ರಚನೆ mosquitto *context)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL
	ಹಿಂತಿರುಗಿ mux_epoll__new(context);
#ಅಥವಾಆದರೆ defined(WITH_KQUEUE)
	ಹಿಂತಿರುಗಿ mux_kqueue__new(context);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mux_poll__new(context);
#ಆದರೆ_ಅಂತ್ಯ
}


ಪೂರ್ಣಾಂಕ mux__delete(ರಚನೆ mosquitto *context)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL
	ಹಿಂತಿರುಗಿ mux_epoll__delete(context);
#ಅಥವಾಆದರೆ defined(WITH_KQUEUE)
	ಹಿಂತಿರುಗಿ mux_kqueue__delete(context);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mux_poll__delete(context);
#ಆದರೆ_ಅಂತ್ಯ
}


ಪೂರ್ಣಾಂಕ mux__handle(ರಚನೆ mosquitto__listener_sock *listensock, ಪೂರ್ಣಾಂಕ listensock_ಎಣಿಕೆ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL
	ಬಳಸದ(listensock);
	ಬಳಸದ(listensock_ಎಣಿಕೆ);
	ಹಿಂತಿರುಗಿ mux_epoll__handle();
#ಅಥವಾಆದರೆ defined(WITH_KQUEUE)
	ಬಳಸದ(listensock);
	ಬಳಸದ(listensock_ಎಣಿಕೆ);
	ಹಿಂತಿರುಗಿ mux_kqueue__handle();
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mux_poll__handle(listensock, listensock_ಎಣಿಕೆ);
#ಆದರೆ_ಅಂತ್ಯ
}


ಪೂರ್ಣಾಂಕ mux__cleanup(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_EPOLL
	ಹಿಂತಿರುಗಿ mux_epoll__cleanup();
#ಅಥವಾಆದರೆ defined(WITH_KQUEUE)
	ಹಿಂತಿರುಗಿ mux_kqueue__cleanup();
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ mux_poll__cleanup();
#ಆದರೆ_ಅಂತ್ಯ
}
