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

#অন্তর্ভাৱ "config.h"

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#অন্তর্ভাৱ <pwd.h>
#অন্যথা
#অন্তর্ভাৱ <process.h>
#অন্তর্ভাৱ <winsock2.h>
#অন্তর্ভাৱ <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#অন্তর্ভাৱ <errno.h>
#অন্তর্ভাৱ <limits.h>
#অন্তর্ভাৱ <signal.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#অন্তর্ভাৱ <tcpd.h>
#endif

#অন্তর্ভাৱ "mosquitto_broker_internal.h"
#অন্তর্ভাৱ "util_mosq.h"

সংরচনা mosquitto_db db;

সংরচনা mosquitto__listener_sock *g_listensock = রিক্ত;
পূর্ণাঙ্ক g_listensock_গুনতি = 0;

পূর্ণাঙ্ক g_run = 0;
#ifdef WITH_WRAP
#অন্তর্ভাৱ <syslog.h>
পূর্ণাঙ্ক allow_severity = LOG_INFO;
পূর্ণাঙ্ক deny_severity = LOG_INFO;
#endif


স্থির পূর্ণাঙ্ক set_umask(শূন্য)
{
#যদি !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	ধ্রুৱ ৱর্ণ *mask_s;
	ৱর্ণ *endptr = রিক্ত;
	long mask;


	mask_s = getenv("UMASK_SET");
	যদি(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		যদি(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		যদি(mask < 000 || mask > 0777){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			ঘূৰাই MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
স্থির পূর্ণাঙ্ক check_uid(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *name)
{
	ৱর্ণ *endptr = রিক্ত;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	যদি(errno || endptr == s || *endptr != '\0'){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		ঘূৰাই -1;
	}
	যদি(id < 0){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		ঘূৰাই -1;
	}
	যদি(id > INT_MAX){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		ঘূৰাই -1;
	}
	ঘূৰাই (পূর্ণাঙ্ক)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
স্থির শূন্য print_pwname(শূন্য)
{
#ifndef WIN32
	সংরচনা passwd *pwd = getpwuid(geteuid());
	যদি(!pwd){
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
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


স্থির পূর্ণাঙ্ক drop_privileges(সংরচনা mosquitto__config *config)
{
#যদি !defined(__CYGWIN__) && !defined(WIN32)
	সংরচনা passwd *pwd;
	ৱর্ণ *err;
	পূর্ণাঙ্ক rc;
	ধ্রুৱ ৱর্ণ *puid_s, *pgid_s;
	পূর্ণাঙ্ক puid;
	পূর্ণাঙ্ক pgid;

	ধ্রুৱ ৱর্ণ *snap = getenv("SNAP_NAME");
	যদি(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	যদি(geteuid() == 0){
		যদি(puid_s || pgid_s){
			যদি(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				যদি(pgid < 0){
					ঘূৰাই MOSQ_ERR_INVAL;
				}অন্যথা যদি(pgid > 0){
					rc = setgid((gid_t)pgid);
					যদি(rc == -1){
						err = strerror(errno);
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						ঘূৰাই MOSQ_ERR_ERRNO;
					}
				}
			}
			যদি(puid_s){
				puid = check_uid(puid_s, "PUID");
				যদি(puid < 0){
					ঘূৰাই MOSQ_ERR_INVAL;
				}অন্যথা যদি(puid > 0){
					rc = setuid((uid_t)puid);
					যদি(rc == -1){
						err = strerror(errno);
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						ঘূৰাই MOSQ_ERR_ERRNO;
					}
				}
			}
		}অন্যথা যদি(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			যদি(!pwd){
				যদি(strcmp(config->user, "mosquitto")){
					log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					ঘূৰাই MOSQ_ERR_INVAL;
				}অন্যথা{
					log__printf(রিক্ত, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					যদি(!pwd){
						log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						ঘূৰাই MOSQ_ERR_ERRNO;
					}
				}
			}
			যদি(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				ঘূৰাই MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			যদি(rc == -1){
				err = strerror(errno);
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				ঘূৰাই MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			যদি(rc == -1){
				err = strerror(errno);
				log__printf(রিক্ত, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				ঘূৰাই MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		যদি(geteuid() == 0 || getegid() == 0){
			log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}অন্যথা{
		print_pwname();
	}

#অন্যথা
	UNUSED(config);
#endif
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির শূন্য mosquitto__daemonise(শূন্য)
{
#ifndef WIN32
	ৱর্ণ *err;
	pid_t pid;

	pid = fork();
	যদি(pid < 0){
		err = strerror(errno);
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	যদি(pid > 0){
		exit(0);
	}
	যদি(setsid() < 0){
		err = strerror(errno);
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	যদি(!freopen("/dev/null", "r", stdin)){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	যদি(!freopen("/dev/null", "w", stdout)){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	যদি(!freopen("/dev/null", "w", stderr)){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#অন্যথা
	log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


স্থির পূর্ণাঙ্ক pid__write(শূন্য)
{
	FILE *pid;

	যদি(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		যদি(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}অন্যথা{
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			ঘূৰাই MOSQ_ERR_ERRNO;
		}
	}
	ঘূৰাই MOSQ_ERR_SUCCESS;
}


স্থির শূন্য report_features(শূন্য)
{
#ifdef WITH_BRIDGE
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Bridge support available.");
#অন্যথা
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Persistence support available.");
#অন্যথা
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(রিক্ত, MOSQ_LOG_INFO, "TLS support available.");
#অন্যথা
	log__printf(রিক্ত, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(রিক্ত, MOSQ_LOG_INFO, "TLS-PSK support available.");
#অন্যথা
	log__printf(রিক্ত, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Websockets support available.");
#অন্যথা
	log__printf(রিক্ত, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	যদি(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(রিক্ত, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


স্থির শূন্য post_shutdown_cleanup(শূন্য)
{
	সংরচনা mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(রিক্ত, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#ifdef WITH_PERSISTENCE
	persist__backup(true);
#endif
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#যদি defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		যদি(!ctxt->wsi)
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

	যদি(db.config->pid_file){
		(শূন্য)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


স্থির শূন্য cjson_init(শূন্য)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ifdef WITH_FUZZING


পূর্ণাঙ্ক mosquitto_fuzz_main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
#অন্যথা


পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
#endif
{
	সংরচনা mosquitto__config config;
	পূর্ণাঙ্ক rc;

	mosquitto_time_init();
	cjson_init();

#যদি defined(WIN32) || defined(__CYGWIN__)
	যদি(argc == 2){
		যদি(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			ঘূৰাই 0;
		}অন্যথা যদি(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			ঘূৰাই 0;
		}অন্যথা যদি(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			ঘূৰাই 0;
		}
	}
#endif


#ifdef WIN32
	যদি(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		যদি(_setmaxstdio(2048) != 2048){
			log__printf(রিক্ত, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, পরিমাণ(সংরচনা mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(রিক্ত);
	mosquitto_broker_গ্ৰন্থি_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	যদি(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		ঘূৰাই MOSQ_ERR_SUCCESS;
	}অন্যথা যদি(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}

	যদি(config.test_configuration){
		যদি(!db.config_file){
			log__printf(রিক্ত, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			ঘূৰাই MOSQ_ERR_INVAL;
		}অন্যথা{
			log__printf(রিক্ত, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			ঘূৰাই MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	যদি(rc){
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	যদি(rc){
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	যদি(rc){
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}


	যদি(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	যদি(rc){
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}

	rc = db__open(&config);
	যদি(rc != MOSQ_ERR_SUCCESS){
		log__printf(রিক্ত, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	যদি(rc){
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}
	log__printf(রিক্ত, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	যদি(db.config_file){
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}অন্যথা{
		log__printf(রিক্ত, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	যদি(rc){
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}
	rc = mosquitto_security_init(false);
	যদি(rc){
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_বাৰ্তা_store_compact();

#ifdef WITH_SYS_বৃক্ষ
	sys_বৃক্ষ_init();
#endif

	rc = mux__init();
	যদি(rc){
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}

	rc = listeners__start();
	যদি(rc){
		post_shutdown_cleanup();
		ঘূৰাই rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(রিক্ত, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_গুনতি);

	post_shutdown_cleanup();

	ঘূৰাই rc;
}

#ifdef WIN32


পূর্ণাঙ্ক WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_সাৰি, পূর্ণাঙ্ক nCmdShow)
{
	ৱর্ণ **argv;
	পূর্ণাঙ্ক argc = 1;
	ৱর্ণ *token;
	ৱর্ণ *saveptr = রিক্ত;
	পূর্ণাঙ্ক rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(পরিমাণ(ৱর্ণ *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_সাৰি, " ", &saveptr);
	যেতিয়ালৈ(token){
		argc++;
		argv = mosquitto_realloc(argv, পরিমাণ(ৱর্ণ *)*argc);
		যদি(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			ঘূৰাই MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(রিক্ত, " ", &saveptr);
	}
	rc = main(argc, argv);
	mosquitto_FREE(argv);
	ঘূৰাই rc;
}
#endif
