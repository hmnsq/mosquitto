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

#अन्तर्भाव "config.h"

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#अन्तर्भाव <pwd.h>
#अन्यथा
#अन्तर्भाव <process.h>
#अन्तर्भाव <winsock2.h>
#अन्तर्भाव <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#अन्तर्भाव <errno.h>
#अन्तर्भाव <limits.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#अन्तर्भाव <tcpd.h>
#endif

#अन्तर्भाव "mosquitto_broker_internal.h"
#अन्तर्भाव "util_mosq.h"

संरचना mosquitto_db db;

संरचना mosquitto__listener_sock *g_listensock = रिक्त;
पूर्णाङ्क g_listensock_गिनती = 0;

पूर्णाङ्क g_run = 0;
#ifdef WITH_WRAP
#अन्तर्भाव <syslog.h>
पूर्णाङ्क allow_severity = LOG_INFO;
पूर्णाङ्क deny_severity = LOG_INFO;
#endif


स्थिर पूर्णाङ्क set_umask(शून्य)
{
#यदि !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	ध्रुव वर्ण *mask_s;
	वर्ण *endptr = रिक्त;
	long mask;


	mask_s = getenv("UMASK_SET");
	यदि(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		यदि(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		यदि(mask < 000 || mask > 0777){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			प्रत्यागम MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
स्थिर पूर्णाङ्क check_uid(ध्रुव वर्ण *s, ध्रुव वर्ण *name)
{
	वर्ण *endptr = रिक्त;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	यदि(errno || endptr == s || *endptr != '\0'){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		प्रत्यागम -1;
	}
	यदि(id < 0){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		प्रत्यागम -1;
	}
	यदि(id > INT_MAX){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		प्रत्यागम -1;
	}
	प्रत्यागम (पूर्णाङ्क)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
स्थिर शून्य print_pwname(शून्य)
{
#ifndef WIN32
	संरचना passwd *pwd = getpwuid(geteuid());
	यदि(!pwd){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
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


स्थिर पूर्णाङ्क drop_privileges(संरचना mosquitto__config *config)
{
#यदि !defined(__CYGWIN__) && !defined(WIN32)
	संरचना passwd *pwd;
	वर्ण *err;
	पूर्णाङ्क rc;
	ध्रुव वर्ण *puid_s, *pgid_s;
	पूर्णाङ्क puid;
	पूर्णाङ्क pgid;

	ध्रुव वर्ण *snap = getenv("SNAP_NAME");
	यदि(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	यदि(geteuid() == 0){
		यदि(puid_s || pgid_s){
			यदि(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				यदि(pgid < 0){
					प्रत्यागम MOSQ_ERR_INVAL;
				}अन्यथा यदि(pgid > 0){
					rc = setgid((gid_t)pgid);
					यदि(rc == -1){
						err = strerror(errno);
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						प्रत्यागम MOSQ_ERR_ERRNO;
					}
				}
			}
			यदि(puid_s){
				puid = check_uid(puid_s, "PUID");
				यदि(puid < 0){
					प्रत्यागम MOSQ_ERR_INVAL;
				}अन्यथा यदि(puid > 0){
					rc = setuid((uid_t)puid);
					यदि(rc == -1){
						err = strerror(errno);
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						प्रत्यागम MOSQ_ERR_ERRNO;
					}
				}
			}
		}अन्यथा यदि(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			यदि(!pwd){
				यदि(strcmp(config->user, "mosquitto")){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					प्रत्यागम MOSQ_ERR_INVAL;
				}अन्यथा{
					log__printf(रिक्त, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					यदि(!pwd){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						प्रत्यागम MOSQ_ERR_ERRNO;
					}
				}
			}
			यदि(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				प्रत्यागम MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			यदि(rc == -1){
				err = strerror(errno);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				प्रत्यागम MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			यदि(rc == -1){
				err = strerror(errno);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				प्रत्यागम MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		यदि(geteuid() == 0 || getegid() == 0){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}अन्यथा{
		print_pwname();
	}

#अन्यथा
	UNUSED(config);
#endif
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य mosquitto__daemonise(शून्य)
{
#ifndef WIN32
	वर्ण *err;
	pid_t pid;

	pid = fork();
	यदि(pid < 0){
		err = strerror(errno);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	यदि(pid > 0){
		exit(0);
	}
	यदि(setsid() < 0){
		err = strerror(errno);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	यदि(!freopen("/dev/null", "r", stdin)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	यदि(!freopen("/dev/null", "w", stdout)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	यदि(!freopen("/dev/null", "w", stderr)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


स्थिर पूर्णाङ्क pid__write(शून्य)
{
	FILE *pid;

	यदि(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		यदि(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}अन्यथा{
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			प्रत्यागम MOSQ_ERR_ERRNO;
		}
	}
	प्रत्यागम MOSQ_ERR_SUCCESS;
}


स्थिर शून्य report_features(शून्य)
{
#ifdef WITH_BRIDGE
	log__printf(रिक्त, MOSQ_LOG_INFO, "Bridge support available.");
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(रिक्त, MOSQ_LOG_INFO, "Persistence support available.");
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(रिक्त, MOSQ_LOG_INFO, "TLS support available.");
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(रिक्त, MOSQ_LOG_INFO, "TLS-PSK support available.");
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(रिक्त, MOSQ_LOG_INFO, "Websockets support available.");
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	यदि(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(रिक्त, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


स्थिर शून्य post_shutdown_cleanup(शून्य)
{
	संरचना mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(रिक्त, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#ifdef WITH_PERSISTENCE
	persist__backup(true);
#endif
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#यदि defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		यदि(!ctxt->wsi)
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

	यदि(db.config->pid_file){
		(शून्य)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


स्थिर शून्य cjson_init(शून्य)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ifdef WITH_FUZZING


पूर्णाङ्क mosquitto_fuzz_main(पूर्णाङ्क argc, वर्ण *argv[])
#अन्यथा


पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण *argv[])
#endif
{
	संरचना mosquitto__config config;
	पूर्णाङ्क rc;

	mosquitto_time_init();
	cjson_init();

#यदि defined(WIN32) || defined(__CYGWIN__)
	यदि(argc == 2){
		यदि(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			प्रत्यागम 0;
		}अन्यथा यदि(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			प्रत्यागम 0;
		}अन्यथा यदि(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			प्रत्यागम 0;
		}
	}
#endif


#ifdef WIN32
	यदि(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		यदि(_setmaxstdio(2048) != 2048){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, परिमाण(संरचना mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(रिक्त);
	mosquitto_broker_गाँठ_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	यदि(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		प्रत्यागम MOSQ_ERR_SUCCESS;
	}अन्यथा यदि(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}

	यदि(config.test_configuration){
		यदि(!db.config_file){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			प्रत्यागम MOSQ_ERR_INVAL;
		}अन्यथा{
			log__printf(रिक्त, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			प्रत्यागम MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	यदि(rc){
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	यदि(rc){
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	यदि(rc){
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}


	यदि(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	यदि(rc){
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}

	rc = db__open(&config);
	यदि(rc != MOSQ_ERR_SUCCESS){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	यदि(rc){
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}
	log__printf(रिक्त, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	यदि(db.config_file){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	यदि(rc){
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}
	rc = mosquitto_security_init(false);
	यदि(rc){
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_संदेश_store_compact();

#ifdef WITH_SYS_वृक्ष
	sys_वृक्ष_init();
#endif

	rc = mux__init();
	यदि(rc){
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}

	rc = listeners__start();
	यदि(rc){
		post_shutdown_cleanup();
		प्रत्यागम rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(रिक्त, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_गिनती);

	post_shutdown_cleanup();

	प्रत्यागम rc;
}

#ifdef WIN32


पूर्णाङ्क WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_पंक्ति, पूर्णाङ्क nCmdShow)
{
	वर्ण **argv;
	पूर्णाङ्क argc = 1;
	वर्ण *token;
	वर्ण *saveptr = रिक्त;
	पूर्णाङ्क rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(परिमाण(वर्ण *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_पंक्ति, " ", &saveptr);
	यावत्(token){
		argc++;
		argv = mosquitto_realloc(argv, परिमाण(वर्ण *)*argc);
		यदि(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			प्रत्यागम MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(रिक्त, " ", &saveptr);
	}
	rc = main(argc, argv);
	mosquitto_FREE(argv);
	प्रत्यागम rc;
}
#endif
