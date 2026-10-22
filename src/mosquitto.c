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

#ଅନ୍ତର୍ଭାଵ "config.h"

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#ଅନ୍ତର୍ଭାଵ <pwd.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <process.h>
#ଅନ୍ତର୍ଭାଵ <winsock2.h>
#ଅନ୍ତର୍ଭାଵ <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#ଅନ୍ତର୍ଭାଵ <errno.h>
#ଅନ୍ତର୍ଭାଵ <limits.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#ଅନ୍ତର୍ଭାଵ <tcpd.h>
#endif

#ଅନ୍ତର୍ଭାଵ "mosquitto_broker_internal.h"
#ଅନ୍ତର୍ଭାଵ "util_mosq.h"

ସଂରଚନା mosquitto_db db;

ସଂରଚନା mosquitto__listener_sock *g_listensock = ଖାଲି;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_listensock_ଗଣନା = 0;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା g_run = 0;
#ifdef WITH_WRAP
#ଅନ୍ତର୍ଭାଵ <syslog.h>
ପୂର୍ଣ୍ଣସଂଖ୍ୟା allow_severity = LOG_INFO;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା deny_severity = LOG_INFO;
#endif


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_umask(ଶୂନ୍ୟ)
{
#ଯଦି !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	ଅଚଳ ଅକ୍ଷର *mask_s;
	ଅକ୍ଷର *endptr = ଖାଲି;
	long mask;


	mask_s = getenv("UMASK_SET");
	ଯଦି(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		ଯଦି(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		ଯଦି(mask < 000 || mask > 0777){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_uid(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *name)
{
	ଅକ୍ଷର *endptr = ଖାଲି;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	ଯଦି(errno || endptr == s || *endptr != '\0'){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		ଫେରନ୍ତୁ -1;
	}
	ଯଦି(id < 0){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		ଫେରନ୍ତୁ -1;
	}
	ଯଦି(id > INT_MAX){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		ଫେରନ୍ତୁ -1;
	}
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
ସ୍ଥିର ଶୂନ୍ୟ print_pwname(ଶୂନ୍ୟ)
{
#ifndef WIN32
	ସଂରଚନା passwd *pwd = getpwuid(geteuid());
	ଯଦି(!pwd){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
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


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା drop_privileges(ସଂରଚନା mosquitto__config *config)
{
#ଯଦି !defined(__CYGWIN__) && !defined(WIN32)
	ସଂରଚନା passwd *pwd;
	ଅକ୍ଷର *err;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
	ଅଚଳ ଅକ୍ଷର *puid_s, *pgid_s;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା puid;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pgid;

	ଅଚଳ ଅକ୍ଷର *snap = getenv("SNAP_NAME");
	ଯଦି(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	ଯଦି(geteuid() == 0){
		ଯଦି(puid_s || pgid_s){
			ଯଦି(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				ଯଦି(pgid < 0){
					ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
				}ଅନ୍ୟଥା ଯଦି(pgid > 0){
					rc = setgid((gid_t)pgid);
					ଯଦି(rc == -1){
						err = strerror(errno);
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
					}
				}
			}
			ଯଦି(puid_s){
				puid = check_uid(puid_s, "PUID");
				ଯଦି(puid < 0){
					ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
				}ଅନ୍ୟଥା ଯଦି(puid > 0){
					rc = setuid((uid_t)puid);
					ଯଦି(rc == -1){
						err = strerror(errno);
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
					}
				}
			}
		}ଅନ୍ୟଥା ଯଦି(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			ଯଦି(!pwd){
				ଯଦି(strcmp(config->user, "mosquitto")){
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
				}ଅନ୍ୟଥା{
					log__printf(ଖାଲି, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					ଯଦି(!pwd){
						log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
					}
				}
			}
			ଯଦି(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			ଯଦି(rc == -1){
				err = strerror(errno);
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			ଯଦି(rc == -1){
				err = strerror(errno);
				log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		ଯଦି(geteuid() == 0 || getegid() == 0){
			log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}ଅନ୍ୟଥା{
		print_pwname();
	}

#ଅନ୍ୟଥା
	UNUSED(config);
#endif
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ଶୂନ୍ୟ mosquitto__daemonise(ଶୂନ୍ୟ)
{
#ifndef WIN32
	ଅକ୍ଷର *err;
	pid_t pid;

	pid = fork();
	ଯଦି(pid < 0){
		err = strerror(errno);
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	ଯଦି(pid > 0){
		exit(0);
	}
	ଯଦି(setsid() < 0){
		err = strerror(errno);
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	ଯଦି(!freopen("/dev/null", "r", stdin)){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	ଯଦି(!freopen("/dev/null", "w", stdout)){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	ଯଦି(!freopen("/dev/null", "w", stderr)){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#ଅନ୍ୟଥା
	log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid__write(ଶୂନ୍ୟ)
{
	FILE *pid;

	ଯଦି(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		ଯଦି(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}ଅନ୍ୟଥା{
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			ଫେରନ୍ତୁ MOSQ_ERR_ERRNO;
		}
	}
	ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
}


ସ୍ଥିର ଶୂନ୍ୟ report_features(ଶୂନ୍ୟ)
{
#ifdef WITH_BRIDGE
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Bridge support available.");
#ଅନ୍ୟଥା
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Persistence support available.");
#ଅନ୍ୟଥା
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "TLS support available.");
#ଅନ୍ୟଥା
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "TLS-PSK support available.");
#ଅନ୍ୟଥା
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Websockets support available.");
#ଅନ୍ୟଥା
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	ଯଦି(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(ଖାଲି, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


ସ୍ଥିର ଶୂନ୍ୟ post_shutdown_cleanup(ଶୂନ୍ୟ)
{
	ସଂରଚନା mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#ifdef WITH_PERSISTENCE
	persist__backup(true);
#endif
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#ଯଦି defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ଯଦି(!ctxt->wsi)
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

	ଯଦି(db.config->pid_file){
		(ଶୂନ୍ୟ)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


ସ୍ଥିର ଶୂନ୍ୟ cjson_init(ଶୂନ୍ୟ)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ifdef WITH_FUZZING


ପୂର୍ଣ୍ଣସଂଖ୍ୟା mosquitto_fuzz_main(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
#ଅନ୍ୟଥା


ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
#endif
{
	ସଂରଚନା mosquitto__config config;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	mosquitto_time_init();
	cjson_init();

#ଯଦି defined(WIN32) || defined(__CYGWIN__)
	ଯଦି(argc == 2){
		ଯଦି(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			ଫେରନ୍ତୁ 0;
		}ଅନ୍ୟଥା ଯଦି(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			ଫେରନ୍ତୁ 0;
		}ଅନ୍ୟଥା ଯଦି(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			ଫେରନ୍ତୁ 0;
		}
	}
#endif


#ifdef WIN32
	ଯଦି(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		ଯଦି(_setmaxstdio(2048) != 2048){
			log__printf(ଖାଲି, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, ଆକାର(ସଂରଚନା mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(ଖାଲି);
	mosquitto_broker_ଗଣ୍ଠି_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	ଯଦି(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
	}ଅନ୍ୟଥା ଯଦି(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}

	ଯଦି(config.test_configuration){
		ଯଦି(!db.config_file){
			log__printf(ଖାଲି, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			ଫେରନ୍ତୁ MOSQ_ERR_INVAL;
		}ଅନ୍ୟଥା{
			log__printf(ଖାଲି, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			ଫେରନ୍ତୁ MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	ଯଦି(rc){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	ଯଦି(rc){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	ଯଦି(rc){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}


	ଯଦି(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	ଯଦି(rc){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}

	rc = db__open(&config);
	ଯଦି(rc != MOSQ_ERR_SUCCESS){
		log__printf(ଖାଲି, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	ଯଦି(rc){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}
	log__printf(ଖାଲି, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	ଯଦି(db.config_file){
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}ଅନ୍ୟଥା{
		log__printf(ଖାଲି, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	ଯଦି(rc){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}
	rc = mosquitto_security_init(false);
	ଯଦି(rc){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_ସନ୍ଦେଶ_store_compact();

#ifdef WITH_SYS_ବୃକ୍ଷ
	sys_ବୃକ୍ଷ_init();
#endif

	rc = mux__init();
	ଯଦି(rc){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}

	rc = listeners__start();
	ଯଦି(rc){
		post_shutdown_cleanup();
		ଫେରନ୍ତୁ rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(ଖାଲି, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_ଗଣନା);

	post_shutdown_cleanup();

	ଫେରନ୍ତୁ rc;
}

#ifdef WIN32


ପୂର୍ଣ୍ଣସଂଖ୍ୟା WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nCmdShow)
{
	ଅକ୍ଷର **argv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc = 1;
	ଅକ୍ଷର *token;
	ଅକ୍ଷର *saveptr = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(ଆକାର(ଅକ୍ଷର *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_ଧାଡ଼ି, " ", &saveptr);
	ଯେତେବେଳେ(token){
		argc++;
		argv = mosquitto_realloc(argv, ଆକାର(ଅକ୍ଷର *)*argc);
		ଯଦି(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			ଫେରନ୍ତୁ MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(ଖାଲି, " ", &saveptr);
	}
	rc = ମୁଖ୍ୟ(argc, argv);
	mosquitto_FREE(argv);
	ଫେରନ୍ତୁ rc;
}
#endif
