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

#شامل "config.h"

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#شامل <pwd.h>
#وگرنه
#شامل <process.h>
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#شامل <errno.h>
#شامل <limits.h>
#شامل <signal.h>
#شامل <stdio.h>
#شامل <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#شامل <tcpd.h>
#endif

#شامل "mosquitto_broker_internal.h"
#شامل "util_mosq.h"

ساختار mosquitto_db db;

ساختار mosquitto__listener_sock *g_listensock = پوچ;
صحیح g_listensock_شمار = 0;

صحیح g_run = 0;
#ifdef WITH_WRAP
#شامل <syslog.h>
صحیح allow_severity = LOG_INFO;
صحیح deny_severity = LOG_INFO;
#endif


ایستا صحیح set_umask(تهی)
{
#اگر !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	ثابت نویسه *mask_s;
	نویسه *endptr = پوچ;
	long mask;


	mask_s = getenv("UMASK_SET");
	اگر(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		اگر(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			بازگشت MOSQ_ERR_INVAL;
		}
		اگر(mask < 000 || mask > 0777){
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			بازگشت MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	بازگشت MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
ایستا صحیح check_uid(ثابت نویسه *s, ثابت نویسه *name)
{
	نویسه *endptr = پوچ;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	اگر(errno || endptr == s || *endptr != '\0'){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		بازگشت -1;
	}
	اگر(id < 0){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		بازگشت -1;
	}
	اگر(id > INT_MAX){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		بازگشت -1;
	}
	بازگشت (صحیح)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
ایستا تهی print_pwname(تهی)
{
#ifndef WIN32
	ساختار passwd *pwd = getpwuid(geteuid());
	اگر(!pwd){
		log__printf(پوچ, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
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


ایستا صحیح drop_privileges(ساختار mosquitto__config *config)
{
#اگر !defined(__CYGWIN__) && !defined(WIN32)
	ساختار passwd *pwd;
	نویسه *err;
	صحیح rc;
	ثابت نویسه *puid_s, *pgid_s;
	صحیح puid;
	صحیح pgid;

	ثابت نویسه *snap = getenv("SNAP_NAME");
	اگر(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		بازگشت MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	اگر(geteuid() == 0){
		اگر(puid_s || pgid_s){
			اگر(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				اگر(pgid < 0){
					بازگشت MOSQ_ERR_INVAL;
				}وگرنه اگر(pgid > 0){
					rc = setgid((gid_t)pgid);
					اگر(rc == -1){
						err = strerror(errno);
						log__printf(پوچ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						بازگشت MOSQ_ERR_ERRNO;
					}
				}
			}
			اگر(puid_s){
				puid = check_uid(puid_s, "PUID");
				اگر(puid < 0){
					بازگشت MOSQ_ERR_INVAL;
				}وگرنه اگر(puid > 0){
					rc = setuid((uid_t)puid);
					اگر(rc == -1){
						err = strerror(errno);
						log__printf(پوچ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						بازگشت MOSQ_ERR_ERRNO;
					}
				}
			}
		}وگرنه اگر(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			اگر(!pwd){
				اگر(strcmp(config->user, "mosquitto")){
					log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					بازگشت MOSQ_ERR_INVAL;
				}وگرنه{
					log__printf(پوچ, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					اگر(!pwd){
						log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						بازگشت MOSQ_ERR_ERRNO;
					}
				}
			}
			اگر(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(پوچ, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				بازگشت MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			اگر(rc == -1){
				err = strerror(errno);
				log__printf(پوچ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				بازگشت MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			اگر(rc == -1){
				err = strerror(errno);
				log__printf(پوچ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				بازگشت MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		اگر(geteuid() == 0 || getegid() == 0){
			log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}وگرنه{
		print_pwname();
	}

#وگرنه
	UNUSED(config);
#endif
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا تهی mosquitto__daemonise(تهی)
{
#ifndef WIN32
	نویسه *err;
	pid_t pid;

	pid = fork();
	اگر(pid < 0){
		err = strerror(errno);
		log__printf(پوچ, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	اگر(pid > 0){
		exit(0);
	}
	اگر(setsid() < 0){
		err = strerror(errno);
		log__printf(پوچ, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	اگر(!freopen("/dev/null", "r", stdin)){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	اگر(!freopen("/dev/null", "w", stdout)){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	اگر(!freopen("/dev/null", "w", stderr)){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#وگرنه
	log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


ایستا صحیح pid__write(تهی)
{
	FILE *pid;

	اگر(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		اگر(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}وگرنه{
			log__printf(پوچ, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			بازگشت MOSQ_ERR_ERRNO;
		}
	}
	بازگشت MOSQ_ERR_SUCCESS;
}


ایستا تهی report_features(تهی)
{
#ifdef WITH_BRIDGE
	log__printf(پوچ, MOSQ_LOG_INFO, "Bridge support available.");
#وگرنه
	log__printf(پوچ, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(پوچ, MOSQ_LOG_INFO, "Persistence support available.");
#وگرنه
	log__printf(پوچ, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(پوچ, MOSQ_LOG_INFO, "TLS support available.");
#وگرنه
	log__printf(پوچ, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(پوچ, MOSQ_LOG_INFO, "TLS-PSK support available.");
#وگرنه
	log__printf(پوچ, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(پوچ, MOSQ_LOG_INFO, "Websockets support available.");
#وگرنه
	log__printf(پوچ, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	اگر(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(پوچ, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


ایستا تهی post_shutdown_cleanup(تهی)
{
	ساختار mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(پوچ, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#ifdef WITH_PERSISTENCE
	persist__backup(true);
#endif
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(!ctxt->wsi)
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

	اگر(db.config->pid_file){
		(تهی)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


ایستا تهی cjson_init(تهی)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ifdef WITH_FUZZING


صحیح mosquitto_fuzz_main(صحیح argc, نویسه *argv[])
#وگرنه


صحیح اصلی(صحیح argc, نویسه *argv[])
#endif
{
	ساختار mosquitto__config config;
	صحیح rc;

	mosquitto_time_init();
	cjson_init();

#اگر defined(WIN32) || defined(__CYGWIN__)
	اگر(argc == 2){
		اگر(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			بازگشت 0;
		}وگرنه اگر(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			بازگشت 0;
		}وگرنه اگر(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			بازگشت 0;
		}
	}
#endif


#ifdef WIN32
	اگر(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		اگر(_setmaxstdio(2048) != 2048){
			log__printf(پوچ, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, اندازه(ساختار mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(پوچ);
	mosquitto_broker_گره_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	اگر(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		بازگشت MOSQ_ERR_SUCCESS;
	}وگرنه اگر(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		بازگشت rc;
	}

	اگر(config.test_configuration){
		اگر(!db.config_file){
			log__printf(پوچ, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			بازگشت MOSQ_ERR_INVAL;
		}وگرنه{
			log__printf(پوچ, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			بازگشت MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	اگر(rc){
		post_shutdown_cleanup();
		بازگشت rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	اگر(rc){
		post_shutdown_cleanup();
		بازگشت rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	اگر(rc){
		post_shutdown_cleanup();
		بازگشت rc;
	}


	اگر(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	اگر(rc){
		post_shutdown_cleanup();
		بازگشت rc;
	}

	rc = db__open(&config);
	اگر(rc != MOSQ_ERR_SUCCESS){
		log__printf(پوچ, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		بازگشت rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	اگر(rc){
		post_shutdown_cleanup();
		بازگشت rc;
	}
	log__printf(پوچ, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	اگر(db.config_file){
		log__printf(پوچ, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}وگرنه{
		log__printf(پوچ, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	اگر(rc){
		post_shutdown_cleanup();
		بازگشت rc;
	}
	rc = mosquitto_security_init(false);
	اگر(rc){
		post_shutdown_cleanup();
		بازگشت rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_پیام_store_compact();

#ifdef WITH_SYS_درخت
	sys_درخت_init();
#endif

	rc = mux__init();
	اگر(rc){
		post_shutdown_cleanup();
		بازگشت rc;
	}

	rc = listeners__start();
	اگر(rc){
		post_shutdown_cleanup();
		بازگشت rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(پوچ, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_شمار);

	post_shutdown_cleanup();

	بازگشت rc;
}

#ifdef WIN32


صحیح WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_سطر, صحیح nCmdShow)
{
	نویسه **argv;
	صحیح argc = 1;
	نویسه *token;
	نویسه *saveptr = پوچ;
	صحیح rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(اندازه(نویسه *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_سطر, " ", &saveptr);
	وقتی(token){
		argc++;
		argv = mosquitto_realloc(argv, اندازه(نویسه *)*argc);
		اگر(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			بازگشت MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(پوچ, " ", &saveptr);
	}
	rc = اصلی(argc, argv);
	mosquitto_FREE(argv);
	بازگشت rc;
}
#endif
