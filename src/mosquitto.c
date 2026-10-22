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

#அந்தர்பாவ "config.h"

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#அந்தர்பாவ <pwd.h>
#இல்லையெனில்
#அந்தர்பாவ <process.h>
#அந்தர்பாவ <winsock2.h>
#அந்தர்பாவ <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#அந்தர்பாவ <errno.h>
#அந்தர்பாவ <limits.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#அந்தர்பாவ <tcpd.h>
#endif

#அந்தர்பாவ "mosquitto_broker_internal.h"
#அந்தர்பாவ "util_mosq.h"

கட்டமைப்பு mosquitto_db db;

கட்டமைப்பு mosquitto__listener_sock *g_listensock = வெற்று;
முழு g_listensock_மொத்தம் = 0;

முழு g_run = 0;
#ifdef WITH_WRAP
#அந்தர்பாவ <syslog.h>
முழு allow_severity = LOG_INFO;
முழு deny_severity = LOG_INFO;
#endif


நிலையான முழு set_umask(வெறுமை)
{
#எனில் !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	மாறிலி எழுத்து *mask_s;
	எழுத்து *endptr = வெற்று;
	long mask;


	mask_s = getenv("UMASK_SET");
	எனில்(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		எனில்(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			திரும்பு MOSQ_ERR_INVAL;
		}
		எனில்(mask < 000 || mask > 0777){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			திரும்பு MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	திரும்பு MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
நிலையான முழு check_uid(மாறிலி எழுத்து *s, மாறிலி எழுத்து *name)
{
	எழுத்து *endptr = வெற்று;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	எனில்(errno || endptr == s || *endptr != '\0'){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		திரும்பு -1;
	}
	எனில்(id < 0){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		திரும்பு -1;
	}
	எனில்(id > INT_MAX){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		திரும்பு -1;
	}
	திரும்பு (முழு)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
நிலையான வெறுமை print_pwname(வெறுமை)
{
#ifndef WIN32
	கட்டமைப்பு passwd *pwd = getpwuid(geteuid());
	எனில்(!pwd){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
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


நிலையான முழு drop_privileges(கட்டமைப்பு mosquitto__config *config)
{
#எனில் !defined(__CYGWIN__) && !defined(WIN32)
	கட்டமைப்பு passwd *pwd;
	எழுத்து *err;
	முழு rc;
	மாறிலி எழுத்து *puid_s, *pgid_s;
	முழு puid;
	முழு pgid;

	மாறிலி எழுத்து *snap = getenv("SNAP_NAME");
	எனில்(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		திரும்பு MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	எனில்(geteuid() == 0){
		எனில்(puid_s || pgid_s){
			எனில்(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				எனில்(pgid < 0){
					திரும்பு MOSQ_ERR_INVAL;
				}இல்லையெனில் எனில்(pgid > 0){
					rc = setgid((gid_t)pgid);
					எனில்(rc == -1){
						err = strerror(errno);
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						திரும்பு MOSQ_ERR_ERRNO;
					}
				}
			}
			எனில்(puid_s){
				puid = check_uid(puid_s, "PUID");
				எனில்(puid < 0){
					திரும்பு MOSQ_ERR_INVAL;
				}இல்லையெனில் எனில்(puid > 0){
					rc = setuid((uid_t)puid);
					எனில்(rc == -1){
						err = strerror(errno);
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						திரும்பு MOSQ_ERR_ERRNO;
					}
				}
			}
		}இல்லையெனில் எனில்(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			எனில்(!pwd){
				எனில்(strcmp(config->user, "mosquitto")){
					log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					திரும்பு MOSQ_ERR_INVAL;
				}இல்லையெனில்{
					log__printf(வெற்று, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					எனில்(!pwd){
						log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						திரும்பு MOSQ_ERR_ERRNO;
					}
				}
			}
			எனில்(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				திரும்பு MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			எனில்(rc == -1){
				err = strerror(errno);
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				திரும்பு MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			எனில்(rc == -1){
				err = strerror(errno);
				log__printf(வெற்று, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				திரும்பு MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		எனில்(geteuid() == 0 || getegid() == 0){
			log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}இல்லையெனில்{
		print_pwname();
	}

#இல்லையெனில்
	UNUSED(config);
#endif
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான வெறுமை mosquitto__daemonise(வெறுமை)
{
#ifndef WIN32
	எழுத்து *err;
	pid_t pid;

	pid = fork();
	எனில்(pid < 0){
		err = strerror(errno);
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	எனில்(pid > 0){
		exit(0);
	}
	எனில்(setsid() < 0){
		err = strerror(errno);
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	எனில்(!freopen("/dev/null", "r", stdin)){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	எனில்(!freopen("/dev/null", "w", stdout)){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	எனில்(!freopen("/dev/null", "w", stderr)){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#இல்லையெனில்
	log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


நிலையான முழு pid__write(வெறுமை)
{
	FILE *pid;

	எனில்(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		எனில்(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}இல்லையெனில்{
			log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			திரும்பு MOSQ_ERR_ERRNO;
		}
	}
	திரும்பு MOSQ_ERR_SUCCESS;
}


நிலையான வெறுமை report_features(வெறுமை)
{
#ifdef WITH_BRIDGE
	log__printf(வெற்று, MOSQ_LOG_INFO, "Bridge support available.");
#இல்லையெனில்
	log__printf(வெற்று, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(வெற்று, MOSQ_LOG_INFO, "Persistence support available.");
#இல்லையெனில்
	log__printf(வெற்று, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(வெற்று, MOSQ_LOG_INFO, "TLS support available.");
#இல்லையெனில்
	log__printf(வெற்று, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(வெற்று, MOSQ_LOG_INFO, "TLS-PSK support available.");
#இல்லையெனில்
	log__printf(வெற்று, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(வெற்று, MOSQ_LOG_INFO, "Websockets support available.");
#இல்லையெனில்
	log__printf(வெற்று, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	எனில்(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(வெற்று, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


நிலையான வெறுமை post_shutdown_cleanup(வெறுமை)
{
	கட்டமைப்பு mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(வெற்று, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#ifdef WITH_PERSISTENCE
	persist__backup(true);
#endif
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#எனில் defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		எனில்(!ctxt->wsi)
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

	எனில்(db.config->pid_file){
		(வெறுமை)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


நிலையான வெறுமை cjson_init(வெறுமை)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ifdef WITH_FUZZING


முழு mosquitto_fuzz_main(முழு argc, எழுத்து *argv[])
#இல்லையெனில்


முழு முதன்மை(முழு argc, எழுத்து *argv[])
#endif
{
	கட்டமைப்பு mosquitto__config config;
	முழு rc;

	mosquitto_time_init();
	cjson_init();

#எனில் defined(WIN32) || defined(__CYGWIN__)
	எனில்(argc == 2){
		எனில்(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			திரும்பு 0;
		}இல்லையெனில் எனில்(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			திரும்பு 0;
		}இல்லையெனில் எனில்(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			திரும்பு 0;
		}
	}
#endif


#ifdef WIN32
	எனில்(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		எனில்(_setmaxstdio(2048) != 2048){
			log__printf(வெற்று, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, அளவு(கட்டமைப்பு mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(வெற்று);
	mosquitto_broker_முனை_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	எனில்(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		திரும்பு MOSQ_ERR_SUCCESS;
	}இல்லையெனில் எனில்(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		திரும்பு rc;
	}

	எனில்(config.test_configuration){
		எனில்(!db.config_file){
			log__printf(வெற்று, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			திரும்பு MOSQ_ERR_INVAL;
		}இல்லையெனில்{
			log__printf(வெற்று, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			திரும்பு MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	எனில்(rc){
		post_shutdown_cleanup();
		திரும்பு rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	எனில்(rc){
		post_shutdown_cleanup();
		திரும்பு rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	எனில்(rc){
		post_shutdown_cleanup();
		திரும்பு rc;
	}


	எனில்(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	எனில்(rc){
		post_shutdown_cleanup();
		திரும்பு rc;
	}

	rc = db__open(&config);
	எனில்(rc != MOSQ_ERR_SUCCESS){
		log__printf(வெற்று, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		திரும்பு rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	எனில்(rc){
		post_shutdown_cleanup();
		திரும்பு rc;
	}
	log__printf(வெற்று, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	எனில்(db.config_file){
		log__printf(வெற்று, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}இல்லையெனில்{
		log__printf(வெற்று, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	எனில்(rc){
		post_shutdown_cleanup();
		திரும்பு rc;
	}
	rc = mosquitto_security_init(false);
	எனில்(rc){
		post_shutdown_cleanup();
		திரும்பு rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_செய்தி_store_compact();

#ifdef WITH_SYS_மரம்
	sys_மரம்_init();
#endif

	rc = mux__init();
	எனில்(rc){
		post_shutdown_cleanup();
		திரும்பு rc;
	}

	rc = listeners__start();
	எனில்(rc){
		post_shutdown_cleanup();
		திரும்பு rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(வெற்று, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_மொத்தம்);

	post_shutdown_cleanup();

	திரும்பு rc;
}

#ifdef WIN32


முழு WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_வரி, முழு nCmdShow)
{
	எழுத்து **argv;
	முழு argc = 1;
	எழுத்து *token;
	எழுத்து *saveptr = வெற்று;
	முழு rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(அளவு(எழுத்து *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_வரி, " ", &saveptr);
	போது(token){
		argc++;
		argv = mosquitto_realloc(argv, அளவு(எழுத்து *)*argc);
		எனில்(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			திரும்பு MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(வெற்று, " ", &saveptr);
	}
	rc = முதன்மை(argc, argv);
	mosquitto_FREE(argv);
	திரும்பு rc;
}
#endif
