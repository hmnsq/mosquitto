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

#ਅਨ੍ਤਰ੍ਭਾਵ "config.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <pwd.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <process.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <winsock2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ws2tcpip.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
#  include <sys/time.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WRAP
#ਅਨ੍ਤਰ੍ਭਾਵ <tcpd.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "mosquitto_broker_internal.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util_mosq.h"

ਬਣਤਰ mosquitto_db db;

ਬਣਤਰ mosquitto__listener_sock *g_listensock = ਨਿਰਾ;
ਪੂਰਨ g_listensock_ਗਿਣਤੀ = 0;

ਪੂਰਨ g_ਲੜੀਵਾਰ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WRAP
#ਅਨ੍ਤਰ੍ਭਾਵ <syslog.h>
ਪੂਰਨ allow_severity = LOG_INFO;
ਪੂਰਨ deny_severity = LOG_INFO;
#ਜੇ_ਅੰਤ


ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_umask(ਖਾਲੀ)
{
#ਜੇ !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	ਅਚਲ ਅੱਖਰ *ਨਕਾਬ_s;
	ਅੱਖਰ *endptr = ਨਿਰਾ;
	ਲੰਮਾ ਨਕਾਬ;


	ਨਕਾਬ_s = getenv("UMASK_SET");
	ਜੇ(ਨਕਾਬ_s){
		errno = 0;
		ਨਕਾਬ = strtol(ਨਕਾਬ_s, &endptr, 8);
		ਜੇ(errno || endptr == ਨਕਾਬ_s || *endptr != '\0'){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		ਜੇ(ਨਕਾਬ < 000 || ਨਕਾਬ > 0777){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}
		umask((mode_t)ਨਕਾਬ);
	}
#ਜੇ_ਅੰਤ
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
ਸਥਿਰ ਪੂਰਨ check_uid(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *name)
{
	ਅੱਖਰ *endptr = ਨਿਰਾ;
	ਲੰਮਾ id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	ਜੇ(errno || endptr == s || *endptr != '\0'){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		ਵਾਪਸ -1;
	}
	ਜੇ(id < 0){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		ਵਾਪਸ -1;
	}
	ਜੇ(id > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
		ਵਾਪਸ -1;
	}
	ਵਾਪਸ (ਪੂਰਨ)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
ਸਥਿਰ ਖਾਲੀ print_pwname(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	ਬਣਤਰ passwd *pwd = getpwuid(geteuid());
	ਜੇ(!pwd){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
	}
#ਜੇ_ਅੰਤ
}


/* mosquitto shouldn't run as root.
 * This function will attempt to change to an unprivileged user and group if
 * running as root. The user is given in config->user.
 * Returns 1 on failure (unknown user, setuid/setgid failure)
 * Returns 0 on success.
 * Note that setting config->user to "root" does not produce an error, but it
 * strongly discouraged.
 */


ਸਥਿਰ ਪੂਰਨ drop_privileges(ਬਣਤਰ mosquitto__config *config)
{
#ਜੇ !defined(__CYGWIN__) && !defined(WIN32)
	ਬਣਤਰ passwd *pwd;
	ਅੱਖਰ *err;
	ਪੂਰਨ rc;
	ਅਚਲ ਅੱਖਰ *puid_s, *pgid_s;
	ਪੂਰਨ puid;
	ਪੂਰਨ pgid;

	ਅਚਲ ਅੱਖਰ *snap = getenv("SNAP_NAME");
	ਜੇ(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	ਜੇ(geteuid() == 0){
		ਜੇ(puid_s || pgid_s){
			ਜੇ(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				ਜੇ(pgid < 0){
					ਵਾਪਸ MOSQ_ERR_INVAL;
				}ਨਹੀਂਤਾਂ ਜੇ(pgid > 0){
					rc = setgid((gid_t)pgid);
					ਜੇ(rc == -1){
						err = strerror(errno);
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						ਵਾਪਸ MOSQ_ERR_ERRNO;
					}
				}
			}
			ਜੇ(puid_s){
				puid = check_uid(puid_s, "PUID");
				ਜੇ(puid < 0){
					ਵਾਪਸ MOSQ_ERR_INVAL;
				}ਨਹੀਂਤਾਂ ਜੇ(puid > 0){
					rc = setuid((uid_t)puid);
					ਜੇ(rc == -1){
						err = strerror(errno);
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						ਵਾਪਸ MOSQ_ERR_ERRNO;
					}
				}
			}
		}ਨਹੀਂਤਾਂ ਜੇ(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			ਜੇ(!pwd){
				ਜੇ(strcmp(config->user, "mosquitto")){
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					ਵਾਪਸ MOSQ_ERR_INVAL;
				}ਨਹੀਂਤਾਂ{
					log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					ਜੇ(!pwd){
						log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						ਵਾਪਸ MOSQ_ERR_ERRNO;
					}
				}
			}
			ਜੇ(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				ਵਾਪਸ MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			ਜੇ(rc == -1){
				err = strerror(errno);
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				ਵਾਪਸ MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			ਜੇ(rc == -1){
				err = strerror(errno);
				log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				ਵਾਪਸ MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		ਜੇ(geteuid() == 0 || getegid() == 0){
			log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}ਨਹੀਂਤਾਂ{
		print_pwname();
	}

#ਨਹੀਂਤਾਂ
	ਅਣਵਰਤਿਆ(config);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਖਾਲੀ mosquitto__daemonise(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32
	ਅੱਖਰ *err;
	pid_t pid;

	pid = fork();
	ਜੇ(pid < 0){
		err = strerror(errno);
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in fork: %s", err);
		ਬਾਹਰ_ਨਿਕਲੋ(1);
	}
	ਜੇ(pid > 0){
		ਬਾਹਰ_ਨਿਕਲੋ(0);
	}
	ਜੇ(setsid() < 0){
		err = strerror(errno);
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		ਬਾਹਰ_ਨਿਕਲੋ(1);
	}

	ਜੇ(!freopen("/dev/null", "r", stdin)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		ਬਾਹਰ_ਨਿਕਲੋ(1);
	}
	ਜੇ(!freopen("/dev/null", "w", stdout)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		ਬਾਹਰ_ਨਿਕਲੋ(1);
	}
	ਜੇ(!freopen("/dev/null", "w", stderr)){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		ਬਾਹਰ_ਨਿਕਲੋ(1);
	}
#ਨਹੀਂਤਾਂ
	log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#ਜੇ_ਅੰਤ
}


ਸਥਿਰ ਪੂਰਨ pid__write(ਖਾਲੀ)
{
	FILE *pid;

	ਜੇ(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		ਜੇ(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}ਨਹੀਂਤਾਂ{
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			ਵਾਪਸ MOSQ_ERR_ERRNO;
		}
	}
	ਵਾਪਸ MOSQ_ERR_SUCCESS;
}


ਸਥਿਰ ਖਾਲੀ report_features(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Bridge support available.");
#ਨਹੀਂਤਾਂ
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Bridge support NOT available.");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Persistence support available.");
#ਨਹੀਂਤਾਂ
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Persistence support NOT available.");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "TLS support available.");
#ਨਹੀਂਤਾਂ
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "TLS support NOT available.");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FINAL_WITH_TLS_PSK
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "TLS-PSK support available.");
#ਨਹੀਂਤਾਂ
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_WEBSOCKETS
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Websockets support available.");
#ਨਹੀਂਤਾਂ
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Websockets support NOT available.");
#ਜੇ_ਅੰਤ
	ਜੇ(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(ਨਿਰਾ, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


ਸਥਿਰ ਖਾਲੀ post_shutdown_cleanup(ਖਾਲੀ)
{
	ਬਣਤਰ mosquitto *ctxt, *ctxt_tmp;

	/* FIXME - this isn't quite right, all wills with will delay zero should be
	 * sent now, but those with positive will delay should be persisted and
	 * restored, pending the client reconnecting in time. */
	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		context__send_will(ctxt);
	}
	will_delay__send_all();
	session_expiry_ਜੋੜੋ_on_shutdown();

	/* Set to true only after persistence events have been processed */
	db.shutdown = true;
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "mosquitto version %s terminating", ਸੰਸਕਰਣ);

	broker_control__cleanup();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_PERSISTENCE
	persist__backup(true);
#ਜੇ_ਅੰਤ
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#ਜੇ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ਜੇ(!ctxt->wsi)
#ਜੇ_ਅੰਤ
		{
			ctxt->is_persisted = false; /* prevent persistence removal */
			context__cleanup(ctxt, true);
		}
	}
	HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
		ctxt->is_persisted = false; /* prevent persistence removal */
		context__cleanup(ctxt, true);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	bridge__db_cleanup();
#ਜੇ_ਅੰਤ
	context__free_disused();
	keepalive__cleanup();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_TLS
	mosquitto_FREE(db.tls_keylog);
#ਜੇ_ਅੰਤ
	db__close();

	plugin__unload_all();
	mosquitto_security_cleanup(false);

	ਜੇ(db.config->pid_file){
		(ਖਾਲੀ)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


ਸਥਿਰ ਖਾਲੀ cjson_init(ਖਾਲੀ)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_FUZZING


ਪੂਰਨ mosquitto_fuzz_main(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
#ਨਹੀਂਤਾਂ


ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
#ਜੇ_ਅੰਤ
{
	ਬਣਤਰ mosquitto__config config;
	ਪੂਰਨ rc;

	mosquitto_ਸਮਾਂ_init();
	cjson_init();

#ਜੇ defined(WIN32) || defined(__CYGWIN__)
	ਜੇ(argc == 2){
		ਜੇ(!strcmp(argv[1], "run")){
			service_ਲੜੀਵਾਰ(argv[0]);
			ਵਾਪਸ 0;
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			ਵਾਪਸ 0;
		}ਨਹੀਂਤਾਂ ਜੇ(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			ਵਾਪਸ 0;
		}
	}
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32
	ਜੇ(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		ਜੇ(_setmaxstdio(2048) != 2048){
			log__printf(ਨਿਰਾ, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#ਜੇ_ਅੰਤ

	memset(&db, 0, ਆਕਾਰ(ਬਣਤਰ mosquitto_db));
	db.now_s = mosquitto_ਸਮਾਂ();
	db.now_real_s = ਸਮਾਂ(ਨਿਰਾ);
	mosquitto_broker_ਗੰਢ_id_ਸੈੱਟ(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config_parse_ਦਲੀਲਾਂ(&config, argc, argv);
	ਜੇ(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		ਵਾਪਸ MOSQ_ERR_SUCCESS;
	}ਨਹੀਂਤਾਂ ਜੇ(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}

	ਜੇ(config.test_configuration){
		ਜੇ(!db.config_file){
			log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			ਵਾਪਸ MOSQ_ERR_INVAL;
		}ਨਹੀਂਤਾਂ{
			log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			ਵਾਪਸ MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	ਜੇ(rc){
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	ਜੇ(rc){
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}
	/* Set umask based on environment variable */
	rc = ਸੈੱਟ_umask();
	ਜੇ(rc){
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}


	ਜੇ(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	ਜੇ(rc){
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}

	rc = db__open(&config);
	ਜੇ(rc != MOSQ_ERR_SUCCESS){
		log__printf(ਨਿਰਾ, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	ਜੇ(rc){
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}
	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "mosquitto version %s starting", ਸੰਸਕਰਣ);
	ਜੇ(db.config_file){
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}ਨਹੀਂਤਾਂ{
		log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin_ਲੱਦੋ_all();
	ਜੇ(rc){
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}
	rc = mosquitto_security_init(false);
	ਜੇ(rc){
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_ਸੁਨੇਹਾ_store_compact();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYS_ਰੁੱਖ
	sys_ਰੁੱਖ_init();
#ਜੇ_ਅੰਤ

	rc = mux__init();
	ਜੇ(rc){
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}

	rc = listeners_ਸ਼ੁਰੂ();
	ਜੇ(rc){
		post_shutdown_cleanup();
		ਵਾਪਸ rc;
	}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_BRIDGE
	bridge_ਸ਼ੁਰੂ_all();
#ਜੇ_ਅੰਤ

	broker_control__init();

	g_ਲੜੀਵਾਰ = 1;
	signal_ਤਿਆਰੀ();

	log__printf(ਨਿਰਾ, MOSQ_LOG_INFO, "mosquitto version %s running", ਸੰਸਕਰਣ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WITH_SYSTEMD
	sd_notify(0, "READY=1");
#ਜੇ_ਅੰਤ

	rc = mosquitto_main_loop(g_listensock, g_listensock_ਗਿਣਤੀ);

	post_shutdown_cleanup();

	ਵਾਪਸ rc;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ WIN32


ਪੂਰਨ WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_ਸਤਰ, ਪੂਰਨ nCmdShow)
{
	ਅੱਖਰ **argv;
	ਪੂਰਨ argc = 1;
	ਅੱਖਰ *token;
	ਅੱਖਰ *saveptr = ਨਿਰਾ;
	ਪੂਰਨ rc;

	ਅਣਵਰਤਿਆ(hInstance);
	ਅਣਵਰਤਿਆ(hPrevInstance);
	ਅਣਵਰਤਿਆ(nCmdShow);

	argv = mosquitto_malloc(ਆਕਾਰ(ਅੱਖਰ *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_ਸਤਰ, " ", &saveptr);
	ਜਦੋਂ(token){
		argc++;
		argv = mosquitto_realloc(argv, ਆਕਾਰ(ਅੱਖਰ *)*argc);
		ਜੇ(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			ਵਾਪਸ MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(ਨਿਰਾ, " ", &saveptr);
	}
	rc = ਮੁੱਖ(argc, argv);
	mosquitto_FREE(argv);
	ਵਾਪਸ rc;
}
#ਜੇ_ਅੰਤ
