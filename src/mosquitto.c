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

#অন্তর্ভাব "config.h"

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#অন্তর্ভাব <pwd.h>
#অন্যথা
#অন্তর্ভাব <process.h>
#অন্তর্ভাব <winsock2.h>
#অন্তর্ভাব <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#অন্তর্ভাব <errno.h>
#অন্তর্ভাব <limits.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#অন্তর্ভাব <tcpd.h>
#endif

#অন্তর্ভাব "mosquitto_broker_internal.h"
#অন্তর্ভাব "util_mosq.h"

কাঠামো mosquitto_db db;

কাঠামো mosquitto__listener_sock *g_listensock = ফাঁকা;
পূর্ণসংখ্যা g_listensock_গুনতি = 0;

পূর্ণসংখ্যা g_run = 0;
#ifdef WITH_WRAP
#অন্তর্ভাব <syslog.h>
পূর্ণসংখ্যা allow_severity = LOG_INFO;
পূর্ণসংখ্যা deny_severity = LOG_INFO;
#endif


স্থির পূর্ণসংখ্যা set_umask(শূন্য)
{
#যদি !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	ধ্রুবক অক্ষর *mask_s;
	অক্ষর *endptr = ফাঁকা;
	long mask;


	mask_s = getenv("UMASK_SET");
	যদি(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		যদি(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			ফেরত MOSQ_ERR_INVAL;
		}
		যদি(mask < 000 || mask > 0777){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			ফেরত MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	ফেরত MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
স্থির পূর্ণসংখ্যা check_uid(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *name)
{
	অক্ষর *endptr = ফাঁকা;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	যদি(errno || endptr == s || *endptr != '\0'){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		ফেরত -1;
	}
	যদি(id < 0){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		ফেরত -1;
	}
	যদি(id > INT_MAX){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		ফেরত -1;
	}
	ফেরত (পূর্ণসংখ্যা)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
স্থির শূন্য print_pwname(শূন্য)
{
#ifndef WIN32
	কাঠামো passwd *pwd = getpwuid(geteuid());
	যদি(!pwd){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}অন্যথা{
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
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


স্থির পূর্ণসংখ্যা drop_privileges(কাঠামো mosquitto__config *config)
{
#যদি !defined(__CYGWIN__) && !defined(WIN32)
	কাঠামো passwd *pwd;
	অক্ষর *err;
	পূর্ণসংখ্যা rc;
	ধ্রুবক অক্ষর *puid_s, *pgid_s;
	পূর্ণসংখ্যা puid;
	পূর্ণসংখ্যা pgid;

	ধ্রুবক অক্ষর *snap = getenv("SNAP_NAME");
	যদি(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		ফেরত MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	যদি(geteuid() == 0){
		যদি(puid_s || pgid_s){
			যদি(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				যদি(pgid < 0){
					ফেরত MOSQ_ERR_INVAL;
				}অন্যথা যদি(pgid > 0){
					rc = setgid((gid_t)pgid);
					যদি(rc == -1){
						err = strerror(errno);
						log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						ফেরত MOSQ_ERR_ERRNO;
					}
				}
			}
			যদি(puid_s){
				puid = check_uid(puid_s, "PUID");
				যদি(puid < 0){
					ফেরত MOSQ_ERR_INVAL;
				}অন্যথা যদি(puid > 0){
					rc = setuid((uid_t)puid);
					যদি(rc == -1){
						err = strerror(errno);
						log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						ফেরত MOSQ_ERR_ERRNO;
					}
				}
			}
		}অন্যথা যদি(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			যদি(!pwd){
				যদি(strcmp(config->user, "mosquitto")){
					log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					ফেরত MOSQ_ERR_INVAL;
				}অন্যথা{
					log__printf(ফাঁকা, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					যদি(!pwd){
						log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						ফেরত MOSQ_ERR_ERRNO;
					}
				}
			}
			যদি(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				ফেরত MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			যদি(rc == -1){
				err = strerror(errno);
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				ফেরত MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			যদি(rc == -1){
				err = strerror(errno);
				log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				ফেরত MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		যদি(geteuid() == 0 || getegid() == 0){
			log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}অন্যথা{
		print_pwname();
	}

#অন্যথা
	UNUSED(config);
#endif
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির শূন্য mosquitto__daemonise(শূন্য)
{
#ifndef WIN32
	অক্ষর *err;
	pid_t pid;

	pid = fork();
	যদি(pid < 0){
		err = strerror(errno);
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	যদি(pid > 0){
		exit(0);
	}
	যদি(setsid() < 0){
		err = strerror(errno);
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	যদি(!freopen("/dev/null", "r", stdin)){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	যদি(!freopen("/dev/null", "w", stdout)){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	যদি(!freopen("/dev/null", "w", stderr)){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#অন্যথা
	log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


স্থির পূর্ণসংখ্যা pid__write(শূন্য)
{
	FILE *pid;

	যদি(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		যদি(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}অন্যথা{
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			ফেরত MOSQ_ERR_ERRNO;
		}
	}
	ফেরত MOSQ_ERR_SUCCESS;
}


স্থির শূন্য report_features(শূন্য)
{
#ifdef WITH_BRIDGE
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Bridge support available.");
#অন্যথা
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Persistence support available.");
#অন্যথা
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "TLS support available.");
#অন্যথা
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "TLS-PSK support available.");
#অন্যথা
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Websockets support available.");
#অন্যথা
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	যদি(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(ফাঁকা, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


স্থির শূন্য post_shutdown_cleanup(শূন্য)
{
	কাঠামো mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

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


পূর্ণসংখ্যা mosquitto_fuzz_main(পূর্ণসংখ্যা argc, অক্ষর *argv[])
#অন্যথা


পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
#endif
{
	কাঠামো mosquitto__config config;
	পূর্ণসংখ্যা rc;

	mosquitto_time_init();
	cjson_init();

#যদি defined(WIN32) || defined(__CYGWIN__)
	যদি(argc == 2){
		যদি(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			ফেরত 0;
		}অন্যথা যদি(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			ফেরত 0;
		}অন্যথা যদি(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			ফেরত 0;
		}
	}
#endif


#ifdef WIN32
	যদি(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		যদি(_setmaxstdio(2048) != 2048){
			log__printf(ফাঁকা, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, আকার(কাঠামো mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(ফাঁকা);
	mosquitto_broker_গ্রন্থি_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	যদি(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		ফেরত MOSQ_ERR_SUCCESS;
	}অন্যথা যদি(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		ফেরত rc;
	}

	যদি(config.test_configuration){
		যদি(!db.config_file){
			log__printf(ফাঁকা, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			ফেরত MOSQ_ERR_INVAL;
		}অন্যথা{
			log__printf(ফাঁকা, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			ফেরত MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	যদি(rc){
		post_shutdown_cleanup();
		ফেরত rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	যদি(rc){
		post_shutdown_cleanup();
		ফেরত rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	যদি(rc){
		post_shutdown_cleanup();
		ফেরত rc;
	}


	যদি(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	যদি(rc){
		post_shutdown_cleanup();
		ফেরত rc;
	}

	rc = db__open(&config);
	যদি(rc != MOSQ_ERR_SUCCESS){
		log__printf(ফাঁকা, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		ফেরত rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	যদি(rc){
		post_shutdown_cleanup();
		ফেরত rc;
	}
	log__printf(ফাঁকা, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	যদি(db.config_file){
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}অন্যথা{
		log__printf(ফাঁকা, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	যদি(rc){
		post_shutdown_cleanup();
		ফেরত rc;
	}
	rc = mosquitto_security_init(false);
	যদি(rc){
		post_shutdown_cleanup();
		ফেরত rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_বার্তা_store_compact();

#ifdef WITH_SYS_বৃক্ষ
	sys_বৃক্ষ_init();
#endif

	rc = mux__init();
	যদি(rc){
		post_shutdown_cleanup();
		ফেরত rc;
	}

	rc = listeners__start();
	যদি(rc){
		post_shutdown_cleanup();
		ফেরত rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(ফাঁকা, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_গুনতি);

	post_shutdown_cleanup();

	ফেরত rc;
}

#ifdef WIN32


পূর্ণসংখ্যা WINAPI WinMain(HINSTANCE hInstance, HINSTANCE h_পূর্ব_Instance, LPSTR lp_Cmd_সারি, পূর্ণসংখ্যা nCmdShow)
{
	অক্ষর **argv;
	পূর্ণসংখ্যা argc = 1;
	অক্ষর *token;
	অক্ষর *saveptr = ফাঁকা;
	পূর্ণসংখ্যা rc;

	UNUSED(hInstance);
	UNUSED(h_পূর্ব_Instance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(আকার(অক্ষর *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_সারি, " ", &saveptr);
	যখন(token){
		argc++;
		argv = mosquitto_realloc(argv, আকার(অক্ষর *)*argc);
		যদি(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			ফেরত MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(ফাঁকা, " ", &saveptr);
	}
	rc = প্রধান(argc, argv);
	mosquitto_FREE(argv);
	ফেরত rc;
}
#endif
