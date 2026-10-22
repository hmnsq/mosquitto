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

#അന്തര്ഭാവ "config.h"

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#അന്തര്ഭാവ <pwd.h>
#അല്ലെങ്കിൽ
#അന്തര്ഭാവ <process.h>
#അന്തര്ഭാവ <winsock2.h>
#അന്തര്ഭാവ <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#അന്തര്ഭാവ <errno.h>
#അന്തര്ഭാവ <limits.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#അന്തര്ഭാവ <tcpd.h>
#endif

#അന്തര്ഭാവ "mosquitto_broker_internal.h"
#അന്തര്ഭാവ "util_mosq.h"

ഘടന mosquitto_db db;

ഘടന mosquitto__listener_sock *g_listensock = ശൂന്യ;
പൂർണ്ണസംഖ്യ g_listensock_എണ്ണം = 0;

പൂർണ്ണസംഖ്യ g_run = 0;
#ifdef WITH_WRAP
#അന്തര്ഭാവ <syslog.h>
പൂർണ്ണസംഖ്യ allow_severity = LOG_INFO;
പൂർണ്ണസംഖ്യ deny_severity = LOG_INFO;
#endif


സ്ഥിതം പൂർണ്ണസംഖ്യ set_umask(ശൂന്യം)
{
#എങ്കിൽ !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	സ്ഥിരം അക്ഷരം *mask_s;
	അക്ഷരം *endptr = ശൂന്യ;
	long mask;


	mask_s = getenv("UMASK_SET");
	എങ്കിൽ(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		എങ്കിൽ(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			തിരികെ MOSQ_ERR_INVAL;
		}
		എങ്കിൽ(mask < 000 || mask > 0777){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			തിരികെ MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	തിരികെ MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
സ്ഥിതം പൂർണ്ണസംഖ്യ check_uid(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *name)
{
	അക്ഷരം *endptr = ശൂന്യ;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	എങ്കിൽ(errno || endptr == s || *endptr != '\0'){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		തിരികെ -1;
	}
	എങ്കിൽ(id < 0){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		തിരികെ -1;
	}
	എങ്കിൽ(id > INT_MAX){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		തിരികെ -1;
	}
	തിരികെ (പൂർണ്ണസംഖ്യ)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
സ്ഥിതം ശൂന്യം print_pwname(ശൂന്യം)
{
#ifndef WIN32
	ഘടന passwd *pwd = getpwuid(geteuid());
	എങ്കിൽ(!pwd){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
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


സ്ഥിതം പൂർണ്ണസംഖ്യ drop_privileges(ഘടന mosquitto__config *config)
{
#എങ്കിൽ !defined(__CYGWIN__) && !defined(WIN32)
	ഘടന passwd *pwd;
	അക്ഷരം *err;
	പൂർണ്ണസംഖ്യ rc;
	സ്ഥിരം അക്ഷരം *puid_s, *pgid_s;
	പൂർണ്ണസംഖ്യ puid;
	പൂർണ്ണസംഖ്യ pgid;

	സ്ഥിരം അക്ഷരം *snap = getenv("SNAP_NAME");
	എങ്കിൽ(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		തിരികെ MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	എങ്കിൽ(geteuid() == 0){
		എങ്കിൽ(puid_s || pgid_s){
			എങ്കിൽ(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				എങ്കിൽ(pgid < 0){
					തിരികെ MOSQ_ERR_INVAL;
				}അല്ലെങ്കിൽ എങ്കിൽ(pgid > 0){
					rc = setgid((gid_t)pgid);
					എങ്കിൽ(rc == -1){
						err = strerror(errno);
						log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						തിരികെ MOSQ_ERR_ERRNO;
					}
				}
			}
			എങ്കിൽ(puid_s){
				puid = check_uid(puid_s, "PUID");
				എങ്കിൽ(puid < 0){
					തിരികെ MOSQ_ERR_INVAL;
				}അല്ലെങ്കിൽ എങ്കിൽ(puid > 0){
					rc = setuid((uid_t)puid);
					എങ്കിൽ(rc == -1){
						err = strerror(errno);
						log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						തിരികെ MOSQ_ERR_ERRNO;
					}
				}
			}
		}അല്ലെങ്കിൽ എങ്കിൽ(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			എങ്കിൽ(!pwd){
				എങ്കിൽ(strcmp(config->user, "mosquitto")){
					log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					തിരികെ MOSQ_ERR_INVAL;
				}അല്ലെങ്കിൽ{
					log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					എങ്കിൽ(!pwd){
						log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						തിരികെ MOSQ_ERR_ERRNO;
					}
				}
			}
			എങ്കിൽ(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				തിരികെ MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			എങ്കിൽ(rc == -1){
				err = strerror(errno);
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				തിരികെ MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			എങ്കിൽ(rc == -1){
				err = strerror(errno);
				log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				തിരികെ MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		എങ്കിൽ(geteuid() == 0 || getegid() == 0){
			log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}അല്ലെങ്കിൽ{
		print_pwname();
	}

#അല്ലെങ്കിൽ
	UNUSED(config);
#endif
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം ശൂന്യം mosquitto__daemonise(ശൂന്യം)
{
#ifndef WIN32
	അക്ഷരം *err;
	pid_t pid;

	pid = fork();
	എങ്കിൽ(pid < 0){
		err = strerror(errno);
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	എങ്കിൽ(pid > 0){
		exit(0);
	}
	എങ്കിൽ(setsid() < 0){
		err = strerror(errno);
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	എങ്കിൽ(!freopen("/dev/null", "r", stdin)){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	എങ്കിൽ(!freopen("/dev/null", "w", stdout)){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	എങ്കിൽ(!freopen("/dev/null", "w", stderr)){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#അല്ലെങ്കിൽ
	log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


സ്ഥിതം പൂർണ്ണസംഖ്യ pid__write(ശൂന്യം)
{
	FILE *pid;

	എങ്കിൽ(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		എങ്കിൽ(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}അല്ലെങ്കിൽ{
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			തിരികെ MOSQ_ERR_ERRNO;
		}
	}
	തിരികെ MOSQ_ERR_SUCCESS;
}


സ്ഥിതം ശൂന്യം report_features(ശൂന്യം)
{
#ifdef WITH_BRIDGE
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Bridge support available.");
#അല്ലെങ്കിൽ
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Persistence support available.");
#അല്ലെങ്കിൽ
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "TLS support available.");
#അല്ലെങ്കിൽ
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "TLS-PSK support available.");
#അല്ലെങ്കിൽ
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Websockets support available.");
#അല്ലെങ്കിൽ
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	എങ്കിൽ(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(ശൂന്യ, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


സ്ഥിതം ശൂന്യം post_shutdown_cleanup(ശൂന്യം)
{
	ഘടന mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#ifdef WITH_PERSISTENCE
	persist__backup(true);
#endif
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#എങ്കിൽ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		എങ്കിൽ(!ctxt->wsi)
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

	എങ്കിൽ(db.config->pid_file){
		(ശൂന്യം)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


സ്ഥിതം ശൂന്യം cjson_init(ശൂന്യം)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ifdef WITH_FUZZING


പൂർണ്ണസംഖ്യ mosquitto_fuzz_main(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
#അല്ലെങ്കിൽ


പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
#endif
{
	ഘടന mosquitto__config config;
	പൂർണ്ണസംഖ്യ rc;

	mosquitto_time_init();
	cjson_init();

#എങ്കിൽ defined(WIN32) || defined(__CYGWIN__)
	എങ്കിൽ(argc == 2){
		എങ്കിൽ(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			തിരികെ 0;
		}അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			തിരികെ 0;
		}അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			തിരികെ 0;
		}
	}
#endif


#ifdef WIN32
	എങ്കിൽ(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		എങ്കിൽ(_setmaxstdio(2048) != 2048){
			log__printf(ശൂന്യ, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, വലുപ്പം(ഘടന mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(ശൂന്യ);
	mosquitto_broker_കണ്ണി_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	എങ്കിൽ(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		തിരികെ MOSQ_ERR_SUCCESS;
	}അല്ലെങ്കിൽ എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		തിരികെ rc;
	}

	എങ്കിൽ(config.test_configuration){
		എങ്കിൽ(!db.config_file){
			log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			തിരികെ MOSQ_ERR_INVAL;
		}അല്ലെങ്കിൽ{
			log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			തിരികെ MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	എങ്കിൽ(rc){
		post_shutdown_cleanup();
		തിരികെ rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	എങ്കിൽ(rc){
		post_shutdown_cleanup();
		തിരികെ rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	എങ്കിൽ(rc){
		post_shutdown_cleanup();
		തിരികെ rc;
	}


	എങ്കിൽ(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	എങ്കിൽ(rc){
		post_shutdown_cleanup();
		തിരികെ rc;
	}

	rc = db__open(&config);
	എങ്കിൽ(rc != MOSQ_ERR_SUCCESS){
		log__printf(ശൂന്യ, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		തിരികെ rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	എങ്കിൽ(rc){
		post_shutdown_cleanup();
		തിരികെ rc;
	}
	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	എങ്കിൽ(db.config_file){
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}അല്ലെങ്കിൽ{
		log__printf(ശൂന്യ, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	എങ്കിൽ(rc){
		post_shutdown_cleanup();
		തിരികെ rc;
	}
	rc = mosquitto_security_init(false);
	എങ്കിൽ(rc){
		post_shutdown_cleanup();
		തിരികെ rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_സന്ദേശം_store_compact();

#ifdef WITH_SYS_വൃക്ഷം
	sys_വൃക്ഷം_init();
#endif

	rc = mux__init();
	എങ്കിൽ(rc){
		post_shutdown_cleanup();
		തിരികെ rc;
	}

	rc = listeners__start();
	എങ്കിൽ(rc){
		post_shutdown_cleanup();
		തിരികെ rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(ശൂന്യ, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_എണ്ണം);

	post_shutdown_cleanup();

	തിരികെ rc;
}

#ifdef WIN32


പൂർണ്ണസംഖ്യ WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_വരി, പൂർണ്ണസംഖ്യ nCmdShow)
{
	അക്ഷരം **argv;
	പൂർണ്ണസംഖ്യ argc = 1;
	അക്ഷരം *token;
	അക്ഷരം *saveptr = ശൂന്യ;
	പൂർണ്ണസംഖ്യ rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(വലുപ്പം(അക്ഷരം *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_വരി, " ", &saveptr);
	ആകുമ്പോൾ(token){
		argc++;
		argv = mosquitto_realloc(argv, വലുപ്പം(അക്ഷരം *)*argc);
		എങ്കിൽ(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			തിരികെ MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(ശൂന്യ, " ", &saveptr);
	}
	rc = പ്രധാന(argc, argv);
	mosquitto_FREE(argv);
	തിരികെ rc;
}
#endif
