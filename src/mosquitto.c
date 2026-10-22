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

#ضمن "config.h"

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#ضمن <pwd.h>
#وإلا
#ضمن <process.h>
#ضمن <winsock2.h>
#ضمن <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#ضمن <errno.h>
#ضمن <limits.h>
#ضمن <signal.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#ضمن <tcpd.h>
#endif

#ضمن "mosquitto_broker_internal.h"
#ضمن "util_mosq.h"

بنية mosquitto_db db;

بنية mosquitto__listener_sock *g_listensock = عدم;
صحيح g_listensock_كدي = 0;

صحيح g_run = 0;
#ifdef WITH_WRAP
#ضمن <syslog.h>
صحيح allow_severity = LOG_INFO;
صحيح deny_severity = LOG_INFO;
#endif


ساكن صحيح set_umask(فراغ)
{
#إذا !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	ثابت حرف *mask_s;
	حرف *endptr = عدم;
	long mask;


	mask_s = getenv("UMASK_SET");
	إذا(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		إذا(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			أرجع MOSQ_ERR_INVAL;
		}
		إذا(mask < 000 || mask > 0777){
			log__printf(عدم, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			أرجع MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	أرجع MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
ساكن صحيح check_uid(ثابت حرف *s, ثابت حرف *name)
{
	حرف *endptr = عدم;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	إذا(errno || endptr == s || *endptr != '\0'){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		أرجع -1;
	}
	إذا(id < 0){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		أرجع -1;
	}
	إذا(id > INT_MAX){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		أرجع -1;
	}
	أرجع (صحيح)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
ساكن فراغ بغ_pwname(فراغ)
{
#ifndef WIN32
	بنية passwd *pwd = getpwuid(geteuid());
	إذا(!pwd){
		log__printf(عدم, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}وإلا{
		log__printf(عدم, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
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


ساكن صحيح drop_privileges(بنية mosquitto__config *config)
{
#إذا !defined(__CYGWIN__) && !defined(WIN32)
	بنية passwd *pwd;
	حرف *err;
	صحيح rc;
	ثابت حرف *puid_s, *pgid_s;
	صحيح puid;
	صحيح pgid;

	ثابت حرف *snap = getenv("SNAP_NAME");
	إذا(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		أرجع MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	إذا(geteuid() == 0){
		إذا(puid_s || pgid_s){
			إذا(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				إذا(pgid < 0){
					أرجع MOSQ_ERR_INVAL;
				}وإلا إذا(pgid > 0){
					rc = setgid((gid_t)pgid);
					إذا(rc == -1){
						err = strerror(errno);
						log__printf(عدم, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						أرجع MOSQ_ERR_ERRNO;
					}
				}
			}
			إذا(puid_s){
				puid = check_uid(puid_s, "PUID");
				إذا(puid < 0){
					أرجع MOSQ_ERR_INVAL;
				}وإلا إذا(puid > 0){
					rc = setuid((uid_t)puid);
					إذا(rc == -1){
						err = strerror(errno);
						log__printf(عدم, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						أرجع MOSQ_ERR_ERRNO;
					}
				}
			}
		}وإلا إذا(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			إذا(!pwd){
				إذا(strcmp(config->user, "mosquitto")){
					log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					أرجع MOSQ_ERR_INVAL;
				}وإلا{
					log__printf(عدم, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					إذا(!pwd){
						log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						أرجع MOSQ_ERR_ERRNO;
					}
				}
			}
			إذا(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(عدم, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				أرجع MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			إذا(rc == -1){
				err = strerror(errno);
				log__printf(عدم, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				أرجع MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			إذا(rc == -1){
				err = strerror(errno);
				log__printf(عدم, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				أرجع MOSQ_ERR_ERRNO;
			}
		}
		بغ_pwname();
		إذا(geteuid() == 0 || getegid() == 0){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}وإلا{
		بغ_pwname();
	}

#وإلا
	UNUSED(config);
#endif
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ mosquitto__daemonise(فراغ)
{
#ifndef WIN32
	حرف *err;
	pid_t pid;

	pid = fork();
	إذا(pid < 0){
		err = strerror(errno);
		log__printf(عدم, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	إذا(pid > 0){
		exit(0);
	}
	إذا(setsid() < 0){
		err = strerror(errno);
		log__printf(عدم, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	إذا(!freopen("/dev/null", "r", stdin)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	إذا(!freopen("/dev/null", "w", stdout)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	إذا(!freopen("/dev/null", "w", stderr)){
		log__printf(عدم, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#وإلا
	log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


ساكن صحيح pid__write(فراغ)
{
	FILE *pid;

	إذا(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		إذا(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}وإلا{
			log__printf(عدم, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			أرجع MOSQ_ERR_ERRNO;
		}
	}
	أرجع MOSQ_ERR_SUCCESS;
}


ساكن فراغ report_features(فراغ)
{
#ifdef WITH_BRIDGE
	log__printf(عدم, MOSQ_LOG_INFO, "Bridge support available.");
#وإلا
	log__printf(عدم, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(عدم, MOSQ_LOG_INFO, "Persistence support available.");
#وإلا
	log__printf(عدم, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(عدم, MOSQ_LOG_INFO, "TLS support available.");
#وإلا
	log__printf(عدم, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(عدم, MOSQ_LOG_INFO, "TLS-PSK support available.");
#وإلا
	log__printf(عدم, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(عدم, MOSQ_LOG_INFO, "Websockets support available.");
#وإلا
	log__printf(عدم, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	إذا(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(عدم, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


ساكن فراغ post_shutdown_cleanup(فراغ)
{
	بنية mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(عدم, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#ifdef WITH_PERSISTENCE
	persist__backup(true);
#endif
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#إذا defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		إذا(!ctxt->wsi)
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

	إذا(db.config->pid_file){
		(فراغ)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


ساكن فراغ cjson_init(فراغ)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ifdef WITH_FUZZING


صحيح mosquitto_fuzz_main(صحيح argc, حرف *argv[])
#وإلا


صحيح main(صحيح argc, حرف *argv[])
#endif
{
	بنية mosquitto__config config;
	صحيح rc;

	mosquitto_time_init();
	cjson_init();

#إذا defined(WIN32) || defined(__CYGWIN__)
	إذا(argc == 2){
		إذا(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			أرجع 0;
		}وإلا إذا(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			أرجع 0;
		}وإلا إذا(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			أرجع 0;
		}
	}
#endif


#ifdef WIN32
	إذا(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		إذا(_setmaxstdio(2048) != 2048){
			log__printf(عدم, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, حجم(بنية mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(عدم);
	mosquitto_broker_كلي_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config_parse_حجوجي(&config, argc, argv);
	إذا(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		أرجع MOSQ_ERR_SUCCESS;
	}وإلا إذا(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		أرجع rc;
	}

	إذا(config.test_configuration){
		إذا(!db.config_file){
			log__printf(عدم, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			أرجع MOSQ_ERR_INVAL;
		}وإلا{
			log__printf(عدم, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			أرجع MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	إذا(rc){
		post_shutdown_cleanup();
		أرجع rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	إذا(rc){
		post_shutdown_cleanup();
		أرجع rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	إذا(rc){
		post_shutdown_cleanup();
		أرجع rc;
	}


	إذا(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	إذا(rc){
		post_shutdown_cleanup();
		أرجع rc;
	}

	rc = db__open(&config);
	إذا(rc != MOSQ_ERR_SUCCESS){
		log__printf(عدم, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		أرجع rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	إذا(rc){
		post_shutdown_cleanup();
		أرجع rc;
	}
	log__printf(عدم, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	إذا(db.config_file){
		log__printf(عدم, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}وإلا{
		log__printf(عدم, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	إذا(rc){
		post_shutdown_cleanup();
		أرجع rc;
	}
	rc = mosquitto_security_init(false);
	إذا(rc){
		post_shutdown_cleanup();
		أرجع rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_سكو_store_compact();

#ifdef WITH_SYS_بشيا
	sys_بشيا_init();
#endif

	rc = mux__init();
	إذا(rc){
		post_shutdown_cleanup();
		أرجع rc;
	}

	rc = listeners__start();
	إذا(rc){
		post_shutdown_cleanup();
		أرجع rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(عدم, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_كدي);

	post_shutdown_cleanup();

	أرجع rc;
}

#ifdef WIN32


صحيح WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_لي, صحيح nCmdShow)
{
	حرف **argv;
	صحيح argc = 1;
	حرف *token;
	حرف *saveptr = عدم;
	صحيح rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(حجم(حرف *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_لي, " ", &saveptr);
	طالما(token){
		argc++;
		argv = mosquitto_realloc(argv, حجم(حرف *)*argc);
		إذا(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			أرجع MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(عدم, " ", &saveptr);
	}
	rc = main(argc, argv);
	mosquitto_FREE(argv);
	أرجع rc;
}
#endif
