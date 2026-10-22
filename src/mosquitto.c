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

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#ಅನ್ತರ್ಭಾವ <pwd.h>
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ <process.h>
#ಅನ್ತರ್ಭಾವ <winsock2.h>
#ಅನ್ತರ್ಭಾವ <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#ಅನ್ತರ್ಭಾವ <tcpd.h>
#endif

#ಅನ್ತರ್ಭಾವ "mosquitto_broker_internal.h"
#ಅನ್ತರ್ಭಾವ "util_mosq.h"

ಸಂರಚನಾ mosquitto_db db;

ಸಂರಚನಾ mosquitto__listener_sock *g_listensock = ರಿಕ್ತ;
ಪೂರ್ಣಾಙ್ಕ g_listensock_ಎಣಿಕೆ = 0;

ಪೂರ್ಣಾಙ್ಕ g_run = 0;
#ifdef WITH_WRAP
#ಅನ್ತರ್ಭಾವ <syslog.h>
ಪೂರ್ಣಾಙ್ಕ allow_severity = LOG_INFO;
ಪೂರ್ಣಾಙ್ಕ deny_severity = LOG_INFO;
#endif


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_umask(ಶೂನ್ಯ)
{
#ಯದಿ !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	ಧ್ರುವ ವರ್ಣ *mask_s;
	ವರ್ಣ *endptr = ರಿಕ್ತ;
	long mask;


	mask_s = getenv("UMASK_SET");
	ಯದಿ(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		ಯದಿ(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		ಯದಿ(mask < 000 || mask > 0777){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_uid(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *name)
{
	ವರ್ಣ *endptr = ರಿಕ್ತ;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	ಯದಿ(errno || endptr == s || *endptr != '\0'){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಯದಿ(id < 0){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಯದಿ(id > INT_MAX){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
ಸ್ಥಿರ ಶೂನ್ಯ print_pwname(ಶೂನ್ಯ)
{
#ifndef WIN32
	ಸಂರಚನಾ passwd *pwd = getpwuid(geteuid());
	ಯದಿ(!pwd){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
	}
#endif
}


/* mosquitto shouldn't run as root.
 * This function will attempt to change to an unprivileged user and group if
 * running as root. The user is given in config->user.
 * Returns 1 on failure (unknown user, setuid/setgid failure)
 * Returns 0 on success.
 * Note that setting config->user to "root" does not produce an error, but it
 * strongly discouraged.
 */


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ drop_privileges(ಸಂರಚನಾ mosquitto__config *config)
{
#ಯದಿ !defined(__CYGWIN__) && !defined(WIN32)
	ಸಂರಚನಾ passwd *pwd;
	ವರ್ಣ *err;
	ಪೂರ್ಣಾಙ್ಕ rc;
	ಧ್ರುವ ವರ್ಣ *puid_s, *pgid_s;
	ಪೂರ್ಣಾಙ್ಕ puid;
	ಪೂರ್ಣಾಙ್ಕ pgid;

	ಧ್ರುವ ವರ್ಣ *snap = getenv("SNAP_NAME");
	ಯದಿ(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	ಯದಿ(geteuid() == 0){
		ಯದಿ(puid_s || pgid_s){
			ಯದಿ(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				ಯದಿ(pgid < 0){
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
				}ಅನ್ಯಥಾ ಯದಿ(pgid > 0){
					rc = setgid((gid_t)pgid);
					ಯದಿ(rc == -1){
						err = strerror(errno);
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
					}
				}
			}
			ಯದಿ(puid_s){
				puid = check_uid(puid_s, "PUID");
				ಯದಿ(puid < 0){
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
				}ಅನ್ಯಥಾ ಯದಿ(puid > 0){
					rc = setuid((uid_t)puid);
					ಯದಿ(rc == -1){
						err = strerror(errno);
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
					}
				}
			}
		}ಅನ್ಯಥಾ ಯದಿ(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			ಯದಿ(!pwd){
				ಯದಿ(strcmp(config->user, "mosquitto")){
					log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
				}ಅನ್ಯಥಾ{
					log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					ಯದಿ(!pwd){
						log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
					}
				}
			}
			ಯದಿ(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			ಯದಿ(rc == -1){
				err = strerror(errno);
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			ಯದಿ(rc == -1){
				err = strerror(errno);
				log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		ಯದಿ(geteuid() == 0 || getegid() == 0){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}ಅನ್ಯಥಾ{
		print_pwname();
	}

#ಅನ್ಯಥಾ
	UNUSED(config);
#endif
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ mosquitto__daemonise(ಶೂನ್ಯ)
{
#ifndef WIN32
	ವರ್ಣ *err;
	pid_t pid;

	pid = fork();
	ಯದಿ(pid < 0){
		err = strerror(errno);
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	ಯದಿ(pid > 0){
		exit(0);
	}
	ಯದಿ(setsid() < 0){
		err = strerror(errno);
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	ಯದಿ(!freopen("/dev/null", "r", stdin)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	ಯದಿ(!freopen("/dev/null", "w", stdout)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	ಯದಿ(!freopen("/dev/null", "w", stderr)){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#ಅನ್ಯಥಾ
	log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pid__write(ಶೂನ್ಯ)
{
	FILE *pid;

	ಯದಿ(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		ಯದಿ(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}ಅನ್ಯಥಾ{
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_ERRNO;
		}
	}
	ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
}


ಸ್ಥಿರ ಶೂನ್ಯ report_features(ಶೂನ್ಯ)
{
#ifdef WITH_BRIDGE
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Bridge support available.");
#ಅನ್ಯಥಾ
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Persistence support available.");
#ಅನ್ಯಥಾ
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "TLS support available.");
#ಅನ್ಯಥಾ
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "TLS-PSK support available.");
#ಅನ್ಯಥಾ
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Websockets support available.");
#ಅನ್ಯಥಾ
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	ಯದಿ(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ post_shutdown_cleanup(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ mosquitto *ctxt, *ctxt_tmp;

	/* FIXME - this isn't quite right, all wills with will delay zero should be
	 * sent now, but those with positive will delay should be persisted and
	 * restored, pending the client reconnecting in time. */
	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		context__send_will(ctxt);
	}
	will_delay__send_all();
	session_expiry__add_on_shutdown();

	/* Set to true only after persistence events have been processed */
	db.shutdown = true;
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#ifdef WITH_PERSISTENCE
	persist__backup(true);
#endif
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#ಯದಿ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ಯದಿ(!ctxt->wsi)
#endif
		{
			ctxt->is_persisted = false; /* prevent persistence removal */
			context__cleanup(ctxt, true);
		}
	}
	HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
		ctxt->is_persisted = false; /* prevent persistence removal */
		context__cleanup(ctxt, true);
	}
#ifdef WITH_BRIDGE
	bridge__db_cleanup();
#endif
	context__free_disused();
	keepalive__cleanup();

#ifdef WITH_TLS
	mosquitto_FREE(db.tls_keylog);
#endif
	db__close();

	plugin__unload_all();
	mosquitto_security_cleanup(false);

	ಯದಿ(db.config->pid_file){
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

#ifdef WITH_FUZZING


ಪೂರ್ಣಾಙ್ಕ mosquitto_fuzz_main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
#ಅನ್ಯಥಾ


ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
#endif
{
	ಸಂರಚನಾ mosquitto__config config;
	ಪೂರ್ಣಾಙ್ಕ rc;

	mosquitto_time_init();
	cjson_init();

#ಯದಿ defined(WIN32) || defined(__CYGWIN__)
	ಯದಿ(argc == 2){
		ಯದಿ(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			ಪ್ರತ್ಯಾಗಮ 0;
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			ಪ್ರತ್ಯಾಗಮ 0;
		}ಅನ್ಯಥಾ ಯದಿ(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			ಪ್ರತ್ಯಾಗಮ 0;
		}
	}
#endif


#ifdef WIN32
	ಯದಿ(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		ಯದಿ(_setmaxstdio(2048) != 2048){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(ರಿಕ್ತ);
	mosquitto_broker_ಗಂಟು_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	ಯದಿ(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
	}ಅನ್ಯಥಾ ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	ಯದಿ(config.test_configuration){
		ಯದಿ(!db.config_file){
			log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_INVAL;
		}ಅನ್ಯಥಾ{
			log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	ಯದಿ(rc){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	ಯದಿ(rc){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	ಯದಿ(rc){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}


	ಯದಿ(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	ಯದಿ(rc){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	rc = db__open(&config);
	ಯದಿ(rc != MOSQ_ERR_SUCCESS){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	ಯದಿ(rc){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	ಯದಿ(db.config_file){
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}ಅನ್ಯಥಾ{
		log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	ಯದಿ(rc){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}
	rc = mosquitto_security_init(false);
	ಯದಿ(rc){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_ಸಂದೇಶ_store_compact();

#ifdef WITH_SYS_ವೃಕ್ಷ
	sys_ವೃಕ್ಷ_init();
#endif

	rc = mux__init();
	ಯದಿ(rc){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}

	rc = listeners__start();
	ಯದಿ(rc){
		post_shutdown_cleanup();
		ಪ್ರತ್ಯಾಗಮ rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(ರಿಕ್ತ, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_ಎಣಿಕೆ);

	post_shutdown_cleanup();

	ಪ್ರತ್ಯಾಗಮ rc;
}

#ifdef WIN32


ಪೂರ್ಣಾಙ್ಕ WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ nCmdShow)
{
	ವರ್ಣ **argv;
	ಪೂರ್ಣಾಙ್ಕ argc = 1;
	ವರ್ಣ *token;
	ವರ್ಣ *saveptr = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(ಪರಿಮಾಣ(ವರ್ಣ *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_ಸಾಲು, " ", &saveptr);
	ಯಾವತ್(token){
		argc++;
		argv = mosquitto_realloc(argv, ಪರಿಮಾಣ(ವರ್ಣ *)*argc);
		ಯದಿ(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			ಪ್ರತ್ಯಾಗಮ MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(ರಿಕ್ತ, " ", &saveptr);
	}
	rc = main(argc, argv);
	mosquitto_FREE(argv);
	ಪ್ರತ್ಯಾಗಮ rc;
}
#endif
