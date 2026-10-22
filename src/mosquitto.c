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

#అన్తర్భావ "config.h"

#నిర్వచితం_కాకపోతే WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WIN32
#అన్తర్భావ <pwd.h>
#లేకపోతే
#అన్తర్భావ <process.h>
#అన్తర్భావ <winsock2.h>
#అన్తర్భావ <ws2tcpip.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WIN32
#  include <sys/time.h>
#అయితే_ముగింపు

#అన్తర్భావ <errno.h>
#అన్తర్భావ <limits.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#నిర్వచితమైతే WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_WRAP
#అన్తర్భావ <tcpd.h>
#అయితే_ముగింపు

#అన్తర్భావ "mosquitto_broker_internal.h"
#అన్తర్భావ "util_mosq.h"

నిర్మాణం mosquitto_db db;

నిర్మాణం mosquitto__listener_sock *g_listensock = ఖాళీ;
పూర్ణాంకం g_listensock_లెక్క = 0;

పూర్ణాంకం g_నిరంతరం = 0;
#నిర్వచితమైతే WITH_WRAP
#అన్తర్భావ <syslog.h>
పూర్ణాంకం allow_severity = LOG_INFO;
పూర్ణాంకం deny_severity = LOG_INFO;
#అయితే_ముగింపు


స్థిర పూర్ణాంకం సమితి_umask(శూన్య)
{
#ఒకవేళ !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	స్థిరరాశి అక్షరం *ముసుగు_s;
	అక్షరం *endptr = ఖాళీ;
	దీర్ఘ ముసుగు;


	ముసుగు_s = getenv("UMASK_SET");
	ఒకవేళ(ముసుగు_s){
		errno = 0;
		ముసుగు = strtol(ముసుగు_s, &endptr, 8);
		ఒకవేళ(errno || endptr == ముసుగు_s || *endptr != '\0'){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			తిరిగి MOSQ_ERR_INVAL;
		}
		ఒకవేళ(ముసుగు < 000 || ముసుగు > 0777){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			తిరిగి MOSQ_ERR_INVAL;
		}
		umask((mode_t)ముసుగు);
	}
#అయితే_ముగింపు
	తిరిగి MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
స్థిర పూర్ణాంకం check_uid(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *name)
{
	అక్షరం *endptr = ఖాళీ;
	దీర్ఘ id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	ఒకవేళ(errno || endptr == s || *endptr != '\0'){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		తిరిగి -1;
	}
	ఒకవేళ(id < 0){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		తిరిగి -1;
	}
	ఒకవేళ(id > గరిష్ఠ_పూర్ణాంకం){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, గరిష్ఠ_పూర్ణాంకం);
		తిరిగి -1;
	}
	తిరిగి (పూర్ణాంకం)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
స్థిర శూన్య print_pwname(శూన్య)
{
#నిర్వచితం_కాకపోతే WIN32
	నిర్మాణం passwd *pwd = getpwuid(geteuid());
	ఒకవేళ(!pwd){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
	}
#అయితే_ముగింపు
}


/* mosquitto shouldn't run as root.
 * This function will attempt to change to an unprivileged user and group if
 * running as root. The user is given in config->user.
 * Returns 1 on failure (unknown user, setuid/setgid failure)
 * Returns 0 on success.
 * Note that setting config->user to "root" does not produce an error, but it
 * strongly discouraged.
 */


స్థిర పూర్ణాంకం drop_privileges(నిర్మాణం mosquitto__config *config)
{
#ఒకవేళ !defined(__CYGWIN__) && !defined(WIN32)
	నిర్మాణం passwd *pwd;
	అక్షరం *err;
	పూర్ణాంకం rc;
	స్థిరరాశి అక్షరం *puid_s, *pgid_s;
	పూర్ణాంకం puid;
	పూర్ణాంకం pgid;

	స్థిరరాశి అక్షరం *snap = getenv("SNAP_NAME");
	ఒకవేళ(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		తిరిగి MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	ఒకవేళ(geteuid() == 0){
		ఒకవేళ(puid_s || pgid_s){
			ఒకవేళ(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				ఒకవేళ(pgid < 0){
					తిరిగి MOSQ_ERR_INVAL;
				}లేకపోతే ఒకవేళ(pgid > 0){
					rc = setgid((gid_t)pgid);
					ఒకవేళ(rc == -1){
						err = strerror(errno);
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						తిరిగి MOSQ_ERR_ERRNO;
					}
				}
			}
			ఒకవేళ(puid_s){
				puid = check_uid(puid_s, "PUID");
				ఒకవేళ(puid < 0){
					తిరిగి MOSQ_ERR_INVAL;
				}లేకపోతే ఒకవేళ(puid > 0){
					rc = setuid((uid_t)puid);
					ఒకవేళ(rc == -1){
						err = strerror(errno);
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						తిరిగి MOSQ_ERR_ERRNO;
					}
				}
			}
		}లేకపోతే ఒకవేళ(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			ఒకవేళ(!pwd){
				ఒకవేళ(strcmp(config->user, "mosquitto")){
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					తిరిగి MOSQ_ERR_INVAL;
				}లేకపోతే{
					log__printf(ఖాళీ, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					ఒకవేళ(!pwd){
						log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						తిరిగి MOSQ_ERR_ERRNO;
					}
				}
			}
			ఒకవేళ(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				తిరిగి MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			ఒకవేళ(rc == -1){
				err = strerror(errno);
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				తిరిగి MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			ఒకవేళ(rc == -1){
				err = strerror(errno);
				log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				తిరిగి MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		ఒకవేళ(geteuid() == 0 || getegid() == 0){
			log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}లేకపోతే{
		print_pwname();
	}

#లేకపోతే
	వాడని(config);
#అయితే_ముగింపు
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర శూన్య mosquitto__daemonise(శూన్య)
{
#నిర్వచితం_కాకపోతే WIN32
	అక్షరం *err;
	pid_t pid;

	pid = fork();
	ఒకవేళ(pid < 0){
		err = strerror(errno);
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in fork: %s", err);
		నిష్క్రమించు(1);
	}
	ఒకవేళ(pid > 0){
		నిష్క్రమించు(0);
	}
	ఒకవేళ(setsid() < 0){
		err = strerror(errno);
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		నిష్క్రమించు(1);
	}

	ఒకవేళ(!freopen("/dev/null", "r", stdin)){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		నిష్క్రమించు(1);
	}
	ఒకవేళ(!freopen("/dev/null", "w", stdout)){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		నిష్క్రమించు(1);
	}
	ఒకవేళ(!freopen("/dev/null", "w", stderr)){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		నిష్క్రమించు(1);
	}
#లేకపోతే
	log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#అయితే_ముగింపు
}


స్థిర పూర్ణాంకం pid__write(శూన్య)
{
	FILE *pid;

	ఒకవేళ(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		ఒకవేళ(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}లేకపోతే{
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			తిరిగి MOSQ_ERR_ERRNO;
		}
	}
	తిరిగి MOSQ_ERR_SUCCESS;
}


స్థిర శూన్య report_features(శూన్య)
{
#నిర్వచితమైతే WITH_BRIDGE
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Bridge support available.");
#లేకపోతే
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Bridge support NOT available.");
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_PERSISTENCE
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Persistence support available.");
#లేకపోతే
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Persistence support NOT available.");
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_TLS
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "TLS support available.");
#లేకపోతే
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "TLS support NOT available.");
#అయితే_ముగింపు
#నిర్వచితమైతే FINAL_WITH_TLS_PSK
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "TLS-PSK support available.");
#లేకపోతే
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#అయితే_ముగింపు
#నిర్వచితమైతే WITH_WEBSOCKETS
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Websockets support available.");
#లేకపోతే
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "Websockets support NOT available.");
#అయితే_ముగింపు
	ఒకవేళ(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(ఖాళీ, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


స్థిర శూన్య post_shutdown_cleanup(శూన్య)
{
	నిర్మాణం mosquitto *ctxt, *ctxt_tmp;

	/* FIXME - this isn't quite right, all wills with will delay zero should be
	 * sent now, but those with positive will delay should be persisted and
	 * restored, pending the client reconnecting in time. */
	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		context__send_will(ctxt);
	}
	will_delay__send_all();
	session_expiry_కూడు_on_shutdown();

	/* Set to true only after persistence events have been processed */
	db.shutdown = true;
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "mosquitto version %s terminating", సంస్కరణ);

	broker_control__cleanup();

#నిర్వచితమైతే WITH_PERSISTENCE
	persist__backup(true);
#అయితే_ముగింపు
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#ఒకవేళ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ఒకవేళ(!ctxt->wsi)
#అయితే_ముగింపు
		{
			ctxt->is_persisted = false; /* prevent persistence removal */
			context__cleanup(ctxt, true);
		}
	}
	HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
		ctxt->is_persisted = false; /* prevent persistence removal */
		context__cleanup(ctxt, true);
	}
#నిర్వచితమైతే WITH_BRIDGE
	bridge__db_cleanup();
#అయితే_ముగింపు
	context__free_disused();
	keepalive__cleanup();

#నిర్వచితమైతే WITH_TLS
	mosquitto_FREE(db.tls_keylog);
#అయితే_ముగింపు
	db__close();

	plugin__unload_all();
	mosquitto_security_cleanup(false);

	ఒకవేళ(db.config->pid_file){
		(శూన్య)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


స్థిర శూన్య cjson_init(శూన్య)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#నిర్వచితమైతే WITH_FUZZING


పూర్ణాంకం mosquitto_fuzz_main(పూర్ణాంకం argc, అక్షరం *argv[])
#లేకపోతే


పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
#అయితే_ముగింపు
{
	నిర్మాణం mosquitto__config config;
	పూర్ణాంకం rc;

	mosquitto_సమయం_init();
	cjson_init();

#ఒకవేళ defined(WIN32) || defined(__CYGWIN__)
	ఒకవేళ(argc == 2){
		ఒకవేళ(!strcmp(argv[1], "run")){
			service_నిరంతరం(argv[0]);
			తిరిగి 0;
		}లేకపోతే ఒకవేళ(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			తిరిగి 0;
		}లేకపోతే ఒకవేళ(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			తిరిగి 0;
		}
	}
#అయితే_ముగింపు


#నిర్వచితమైతే WIN32
	ఒకవేళ(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		ఒకవేళ(_setmaxstdio(2048) != 2048){
			log__printf(ఖాళీ, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#అయితే_ముగింపు

	memset(&db, 0, పరిమాణం(నిర్మాణం mosquitto_db));
	db.now_s = mosquitto_సమయం();
	db.now_real_s = సమయం(ఖాళీ);
	mosquitto_broker_కణుపు_id_సమితి(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config_parse_వాదనలు(&config, argc, argv);
	ఒకవేళ(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		తిరిగి MOSQ_ERR_SUCCESS;
	}లేకపోతే ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		తిరిగి rc;
	}

	ఒకవేళ(config.test_configuration){
		ఒకవేళ(!db.config_file){
			log__printf(ఖాళీ, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			తిరిగి MOSQ_ERR_INVAL;
		}లేకపోతే{
			log__printf(ఖాళీ, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			తిరిగి MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	ఒకవేళ(rc){
		post_shutdown_cleanup();
		తిరిగి rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	ఒకవేళ(rc){
		post_shutdown_cleanup();
		తిరిగి rc;
	}
	/* Set umask based on environment variable */
	rc = సమితి_umask();
	ఒకవేళ(rc){
		post_shutdown_cleanup();
		తిరిగి rc;
	}


	ఒకవేళ(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	ఒకవేళ(rc){
		post_shutdown_cleanup();
		తిరిగి rc;
	}

	rc = db__open(&config);
	ఒకవేళ(rc != MOSQ_ERR_SUCCESS){
		log__printf(ఖాళీ, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		తిరిగి rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	ఒకవేళ(rc){
		post_shutdown_cleanup();
		తిరిగి rc;
	}
	log__printf(ఖాళీ, MOSQ_LOG_INFO, "mosquitto version %s starting", సంస్కరణ);
	ఒకవేళ(db.config_file){
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}లేకపోతే{
		log__printf(ఖాళీ, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin_ఎక్కించు_all();
	ఒకవేళ(rc){
		post_shutdown_cleanup();
		తిరిగి rc;
	}
	rc = mosquitto_security_init(false);
	ఒకవేళ(rc){
		post_shutdown_cleanup();
		తిరిగి rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_సందేశం_store_compact();

#నిర్వచితమైతే WITH_SYS_వృక్షం
	sys_వృక్షం_init();
#అయితే_ముగింపు

	rc = mux__init();
	ఒకవేళ(rc){
		post_shutdown_cleanup();
		తిరిగి rc;
	}

	rc = listeners_ప్రారంభం();
	ఒకవేళ(rc){
		post_shutdown_cleanup();
		తిరిగి rc;
	}


#నిర్వచితమైతే WITH_BRIDGE
	bridge_ప్రారంభం_all();
#అయితే_ముగింపు

	broker_control__init();

	g_నిరంతరం = 1;
	signal_అమరిక();

	log__printf(ఖాళీ, MOSQ_LOG_INFO, "mosquitto version %s running", సంస్కరణ);
#నిర్వచితమైతే WITH_SYSTEMD
	sd_notify(0, "READY=1");
#అయితే_ముగింపు

	rc = mosquitto_main_loop(g_listensock, g_listensock_లెక్క);

	post_shutdown_cleanup();

	తిరిగి rc;
}

#నిర్వచితమైతే WIN32


పూర్ణాంకం WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_పంక్తి, పూర్ణాంకం nCmdShow)
{
	అక్షరం **argv;
	పూర్ణాంకం argc = 1;
	అక్షరం *token;
	అక్షరం *saveptr = ఖాళీ;
	పూర్ణాంకం rc;

	వాడని(hInstance);
	వాడని(hPrevInstance);
	వాడని(nCmdShow);

	argv = mosquitto_malloc(పరిమాణం(అక్షరం *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_పంక్తి, " ", &saveptr);
	అయితే(token){
		argc++;
		argv = mosquitto_realloc(argv, పరిమాణం(అక్షరం *)*argc);
		ఒకవేళ(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			తిరిగి MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(ఖాళీ, " ", &saveptr);
	}
	rc = ప్రధాన(argc, argv);
	mosquitto_FREE(argv);
	తిరిగి rc;
}
#అయితే_ముగింపు
