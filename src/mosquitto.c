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
*/

#ಅನ್ತರ್ಭಾವ "config.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#ಅನ್ತರ್ಭಾವ <pwd.h>
#ಇಲ್ಲದಿದ್ದರೆ
#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
#  include <sys/time.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WRAP
#ಅನ್ತರ್ಭಾವ <tcpd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

ರಚನೆ mosquitto_db db;

ರಚನೆ mosquitto__listener_sock *g_listensock = ಖಾಲಿ;
ಪೂರ್ಣಾಂಕ g_listensock_ಎಣಿಕೆ = 0;

ಪೂರ್ಣಾಂಕ g_ಸತತ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WRAP
#ಅನ್ತರ್ಭಾವ <syslog.h>
ಪೂರ್ಣಾಂಕ allow_severity = LOG_INFO;
ಪೂರ್ಣಾಂಕ deny_severity = LOG_INFO;
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗಣ_umask(ಶೂನ್ಯ)
{
#ಆದರೆ !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮುಖವಾಡ_s;
	ಅಕ್ಷರ *endptr = ಖಾಲಿ;
	ದೀರ್ಘ ಮುಖವಾಡ;


	ಮುಖವಾಡ_s = getenv("UMASK_SET");
	ಆದರೆ(ಮುಖವಾಡ_s){
		errno = 0;
		ಮುಖವಾಡ = strtol(ಮುಖವಾಡ_s, &endptr, 8);
		ಆದರೆ(errno || endptr == ಮುಖವಾಡ_s || *endptr != '\0'){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		ಆದರೆ(ಮುಖವಾಡ < 000 || ಮುಖವಾಡ > 0777){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}
		umask((mode_t)ಮುಖವಾಡ);
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_uid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ಅಕ್ಷರ *endptr = ಖಾಲಿ;
	ದೀರ್ಘ id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	ಆದರೆ(errno || endptr == s || *endptr != '\0'){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		ಹಿಂತಿರುಗಿ -1;
	}
	ಆದರೆ(id < 0){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		ಹಿಂತಿರುಗಿ -1;
	}
	ಆದರೆ(id > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
		ಹಿಂತಿರುಗಿ -1;
	}
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
ಸ್ಥಿರ ಶೂನ್ಯ print_pwname(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	ರಚನೆ passwd *pwd = getpwuid(geteuid());
	ಆದರೆ(!pwd){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
	}
#ಆದರೆ_ಅಂತ್ಯ
}


/* mosquitto shouldn't run as root.
 * This function will attempt to change to an unprivileged user and group if
 * running as root. The user is given in config->user.
 * Returns 1 on failure (unknown user, setuid/setgid failure)
 * Returns 0 on success.
 * Note that setting config->user to "root" does not produce an error, but it
 * strongly discouraged.
 */


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ drop_privileges(ರಚನೆ mosquitto__config *config)
{
#ಆದರೆ !defined(__CYGWIN__) && !defined(WIN32)
	ರಚನೆ passwd *pwd;
	ಅಕ್ಷರ *err;
	ಪೂರ್ಣಾಂಕ rc;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *puid_s, *pgid_s;
	ಪೂರ್ಣಾಂಕ puid;
	ಪೂರ್ಣಾಂಕ pgid;

	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *snap = getenv("SNAP_NAME");
	ಆದರೆ(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	ಆದರೆ(geteuid() == 0){
		ಆದರೆ(puid_s || pgid_s){
			ಆದರೆ(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				ಆದರೆ(pgid < 0){
					ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pgid > 0){
					rc = setgid((gid_t)pgid);
					ಆದರೆ(rc == -1){
						err = strerror(errno);
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
					}
				}
			}
			ಆದರೆ(puid_s){
				puid = check_uid(puid_s, "PUID");
				ಆದರೆ(puid < 0){
					ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
				}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(puid > 0){
					rc = setuid((uid_t)puid);
					ಆದರೆ(rc == -1){
						err = strerror(errno);
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
					}
				}
			}
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			ಆದರೆ(!pwd){
				ಆದರೆ(strcmp(config->user, "mosquitto")){
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
				}ಇಲ್ಲದಿದ್ದರೆ{
					log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					ಆದರೆ(!pwd){
						log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
					}
				}
			}
			ಆದರೆ(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			ಆದರೆ(rc == -1){
				err = strerror(errno);
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			ಆದರೆ(rc == -1){
				err = strerror(errno);
				log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		ಆದರೆ(geteuid() == 0 || getegid() == 0){
			log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}ಇಲ್ಲದಿದ್ದರೆ{
		print_pwname();
	}

#ಇಲ್ಲದಿದ್ದರೆ
	ಬಳಸದ(config);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ mosquitto__daemonise(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32
	ಅಕ್ಷರ *err;
	pid_t pid;

	pid = fork();
	ಆದರೆ(pid < 0){
		err = strerror(errno);
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error in fork: %s", err);
		ನಿರ್ಗಮಿಸು(1);
	}
	ಆದರೆ(pid > 0){
		ನಿರ್ಗಮಿಸು(0);
	}
	ಆದರೆ(setsid() < 0){
		err = strerror(errno);
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		ನಿರ್ಗಮಿಸು(1);
	}

	ಆದರೆ(!freopen("/dev/null", "r", stdin)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		ನಿರ್ಗಮಿಸು(1);
	}
	ಆದರೆ(!freopen("/dev/null", "w", stdout)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		ನಿರ್ಗಮಿಸು(1);
	}
	ಆದರೆ(!freopen("/dev/null", "w", stderr)){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		ನಿರ್ಗಮಿಸು(1);
	}
#ಇಲ್ಲದಿದ್ದರೆ
	log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#ಆದರೆ_ಅಂತ್ಯ
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pid__write(ಶೂನ್ಯ)
{
	FILE *pid;

	ಆದರೆ(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		ಆದರೆ(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}ಇಲ್ಲದಿದ್ದರೆ{
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_ERRNO;
		}
	}
	ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ report_features(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Bridge support available.");
#ಇಲ್ಲದಿದ್ದರೆ
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Bridge support NOT available.");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Persistence support available.");
#ಇಲ್ಲದಿದ್ದರೆ
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Persistence support NOT available.");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "TLS support available.");
#ಇಲ್ಲದಿದ್ದರೆ
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "TLS support NOT available.");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FINAL_WITH_TLS_PSK
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "TLS-PSK support available.");
#ಇಲ್ಲದಿದ್ದರೆ
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_WEBSOCKETS
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Websockets support available.");
#ಇಲ್ಲದಿದ್ದರೆ
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Websockets support NOT available.");
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(ಖಾಲಿ, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ post_shutdown_cleanup(ಶೂನ್ಯ)
{
	ರಚನೆ mosquitto *ctxt, *ctxt_tmp;

	/* FIXME - this isn't quite right, all wills with will delay zero should be
	 * sent now, but those with positive will delay should be persisted and
	 * restored, pending the client reconnecting in time. */
	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		context__send_will(ctxt);
	}
	will_delay__send_all();
	session_expiry_ಕೂಡಿಸು_on_shutdown();

	/* Set to true only after persistence events have been processed */
	db.shutdown = true;
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "mosquitto version %s terminating", ಆವೃತ್ತಿ);

	broker_control__cleanup();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_PERSISTENCE
	persist__backup(true);
#ಆದರೆ_ಅಂತ್ಯ
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#ಆದರೆ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಆದರೆ(!ctxt->wsi)
#ಆದರೆ_ಅಂತ್ಯ
		{
			ctxt->is_persisted = false; /* prevent persistence removal */
			context__cleanup(ctxt, true);
		}
	}
	HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
		ctxt->is_persisted = false; /* prevent persistence removal */
		context__cleanup(ctxt, true);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	bridge__db_cleanup();
#ಆದರೆ_ಅಂತ್ಯ
	context__free_disused();
	keepalive__cleanup();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_TLS
	mosquitto_FREE(db.tls_keylog);
#ಆದರೆ_ಅಂತ್ಯ
	db__close();

	plugin__unload_all();
	mosquitto_security_cleanup(false);

	ಆದರೆ(db.config->pid_file){
		(ಶೂನ್ಯ)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


ಸ್ಥಿರ ಶೂನ್ಯ cjson_init(ಶೂನ್ಯ)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_FUZZING


ಪೂರ್ಣಾಂಕ mosquitto_fuzz_main(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
#ಇಲ್ಲದಿದ್ದರೆ


ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
#ಆದರೆ_ಅಂತ್ಯ
{
	ರಚನೆ mosquitto__config config;
	ಪೂರ್ಣಾಂಕ rc;

	mosquitto_ಸಮಯ_init();
	cjson_init();

#ಆದರೆ defined(WIN32) || defined(__CYGWIN__)
	ಆದರೆ(argc == 2){
		ಆದರೆ(!strcmp(argv[1], "run")){
			service_ಸತತ(argv[0]);
			ಹಿಂತಿರುಗಿ 0;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			ಹಿಂತಿರುಗಿ 0;
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			ಹಿಂತಿರುಗಿ 0;
		}
	}
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32
	ಆದರೆ(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		ಆದರೆ(_setmaxstdio(2048) != 2048){
			log__printf(ಖಾಲಿ, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#ಆದರೆ_ಅಂತ್ಯ

	memset(&db, 0, ಗಾತ್ರ(ರಚನೆ mosquitto_db));
	db.now_s = mosquitto_ಸಮಯ();
	db.now_real_s = ಸಮಯ(ಖಾಲಿ);
	mosquitto_broker_ಗಂಟು_id_ಗಣ(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config_parse_ವಾದಗಳು(&config, argc, argv);
	ಆದರೆ(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
	}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}

	ಆದರೆ(config.test_configuration){
		ಆದರೆ(!db.config_file){
			log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			ಹಿಂತಿರುಗಿ MOSQ_ERR_INVAL;
		}ಇಲ್ಲದಿದ್ದರೆ{
			log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			ಹಿಂತಿರುಗಿ MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	ಆದರೆ(rc){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	ಆದರೆ(rc){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}
	/* Set umask based on environment variable */
	rc = ಗಣ_umask();
	ಆದರೆ(rc){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}


	ಆದರೆ(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	ಆದರೆ(rc){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}

	rc = db__open(&config);
	ಆದರೆ(rc != MOSQ_ERR_SUCCESS){
		log__printf(ಖಾಲಿ, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	ಆದರೆ(rc){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}
	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "mosquitto version %s starting", ಆವೃತ್ತಿ);
	ಆದರೆ(db.config_file){
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}ಇಲ್ಲದಿದ್ದರೆ{
		log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin_ತುಂಬು_all();
	ಆದರೆ(rc){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}
	rc = mosquitto_security_init(false);
	ಆದರೆ(rc){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_ಸಂದೇಶ_store_compact();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYS_ವೃಕ್ಷ
	sys_ವೃಕ್ಷ_init();
#ಆದರೆ_ಅಂತ್ಯ

	rc = mux__init();
	ಆದರೆ(rc){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}

	rc = listeners_ಆರಂಭ();
	ಆದರೆ(rc){
		post_shutdown_cleanup();
		ಹಿಂತಿರುಗಿ rc;
	}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_BRIDGE
	bridge_ಆರಂಭ_all();
#ಆದರೆ_ಅಂತ್ಯ

	broker_control__init();

	g_ಸತತ = 1;
	signal_ಸಿದ್ಧತೆ();

	log__printf(ಖಾಲಿ, MOSQ_LOG_INFO, "mosquitto version %s running", ಆವೃತ್ತಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WITH_SYSTEMD
	sd_notify(0, "READY=1");
#ಆದರೆ_ಅಂತ್ಯ

	rc = mosquitto_main_loop(g_listensock, g_listensock_ಎಣಿಕೆ);

	post_shutdown_cleanup();

	ಹಿಂತಿರುಗಿ rc;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ WIN32


ಪೂರ್ಣಾಂಕ WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_ಸಾಲು, ಪೂರ್ಣಾಂಕ nCmdShow)
{
	ಅಕ್ಷರ **argv;
	ಪೂರ್ಣಾಂಕ argc = 1;
	ಅಕ್ಷರ *token;
	ಅಕ್ಷರ *saveptr = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ rc;

	ಬಳಸದ(hInstance);
	ಬಳಸದ(hPrevInstance);
	ಬಳಸದ(nCmdShow);

	argv = mosquitto_malloc(ಗಾತ್ರ(ಅಕ್ಷರ *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_ಸಾಲು, " ", &saveptr);
	ಆಗ(token){
		argc++;
		argv = mosquitto_realloc(argv, ಗಾತ್ರ(ಅಕ್ಷರ *)*argc);
		ಆದರೆ(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			ಹಿಂತಿರುಗಿ MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(ಖಾಲಿ, " ", &saveptr);
	}
	rc = ಮುಖ್ಯ(argc, argv);
	mosquitto_FREE(argv);
	ಹಿಂತಿರುಗಿ rc;
}
#ಆದರೆ_ಅಂತ್ಯ
