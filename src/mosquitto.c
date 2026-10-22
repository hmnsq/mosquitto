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

#અન્તર્ભાવ "config.h"

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#અન્તર્ભાવ <pwd.h>
#નહીંતર
#અન્તર્ભાવ <process.h>
#અન્તર્ભાવ <winsock2.h>
#અન્તર્ભાવ <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#અન્તર્ભાવ <errno.h>
#અન્તર્ભાવ <limits.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#અન્તર્ભાવ <tcpd.h>
#endif

#અન્તર્ભાવ "mosquitto_broker_internal.h"
#અન્તર્ભાવ "util_mosq.h"

સંરચના mosquitto_db db;

સંરચના mosquitto__listener_sock *g_listensock = ખાલી;
પૂર્ણાંક g_listensock_ગણતરી = 0;

પૂર્ણાંક g_run = 0;
#ifdef WITH_WRAP
#અન્તર્ભાવ <syslog.h>
પૂર્ણાંક allow_severity = LOG_INFO;
પૂર્ણાંક deny_severity = LOG_INFO;
#endif


સ્થિર પૂર્ણાંક set_umask(શૂન્ય)
{
#જો !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	અચલ અક્ષર *mask_s;
	અક્ષર *endptr = ખાલી;
	long mask;


	mask_s = getenv("UMASK_SET");
	જો(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		જો(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			પાછા MOSQ_ERR_INVAL;
		}
		જો(mask < 000 || mask > 0777){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			પાછા MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	પાછા MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
સ્થિર પૂર્ણાંક check_uid(અચલ અક્ષર *s, અચલ અક્ષર *name)
{
	અક્ષર *endptr = ખાલી;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	જો(errno || endptr == s || *endptr != '\0'){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		પાછા -1;
	}
	જો(id < 0){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		પાછા -1;
	}
	જો(id > INT_MAX){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		પાછા -1;
	}
	પાછા (પૂર્ણાંક)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
સ્થિર શૂન્ય print_pwname(શૂન્ય)
{
#ifndef WIN32
	સંરચના passwd *pwd = getpwuid(geteuid());
	જો(!pwd){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
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


સ્થિર પૂર્ણાંક drop_privileges(સંરચના mosquitto__config *config)
{
#જો !defined(__CYGWIN__) && !defined(WIN32)
	સંરચના passwd *pwd;
	અક્ષર *err;
	પૂર્ણાંક rc;
	અચલ અક્ષર *puid_s, *pgid_s;
	પૂર્ણાંક puid;
	પૂર્ણાંક pgid;

	અચલ અક્ષર *snap = getenv("SNAP_NAME");
	જો(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		પાછા MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	જો(geteuid() == 0){
		જો(puid_s || pgid_s){
			જો(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				જો(pgid < 0){
					પાછા MOSQ_ERR_INVAL;
				}નહીંતર જો(pgid > 0){
					rc = setgid((gid_t)pgid);
					જો(rc == -1){
						err = strerror(errno);
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						પાછા MOSQ_ERR_ERRNO;
					}
				}
			}
			જો(puid_s){
				puid = check_uid(puid_s, "PUID");
				જો(puid < 0){
					પાછા MOSQ_ERR_INVAL;
				}નહીંતર જો(puid > 0){
					rc = setuid((uid_t)puid);
					જો(rc == -1){
						err = strerror(errno);
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						પાછા MOSQ_ERR_ERRNO;
					}
				}
			}
		}નહીંતર જો(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			જો(!pwd){
				જો(strcmp(config->user, "mosquitto")){
					log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					પાછા MOSQ_ERR_INVAL;
				}નહીંતર{
					log__printf(ખાલી, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					જો(!pwd){
						log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						પાછા MOSQ_ERR_ERRNO;
					}
				}
			}
			જો(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				પાછા MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			જો(rc == -1){
				err = strerror(errno);
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				પાછા MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			જો(rc == -1){
				err = strerror(errno);
				log__printf(ખાલી, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				પાછા MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		જો(geteuid() == 0 || getegid() == 0){
			log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}નહીંતર{
		print_pwname();
	}

#નહીંતર
	UNUSED(config);
#endif
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર શૂન્ય mosquitto__daemonise(શૂન્ય)
{
#ifndef WIN32
	અક્ષર *err;
	pid_t pid;

	pid = fork();
	જો(pid < 0){
		err = strerror(errno);
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	જો(pid > 0){
		exit(0);
	}
	જો(setsid() < 0){
		err = strerror(errno);
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	જો(!freopen("/dev/null", "r", stdin)){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	જો(!freopen("/dev/null", "w", stdout)){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	જો(!freopen("/dev/null", "w", stderr)){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#નહીંતર
	log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


સ્થિર પૂર્ણાંક pid__write(શૂન્ય)
{
	FILE *pid;

	જો(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		જો(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}નહીંતર{
			log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			પાછા MOSQ_ERR_ERRNO;
		}
	}
	પાછા MOSQ_ERR_SUCCESS;
}


સ્થિર શૂન્ય report_features(શૂન્ય)
{
#ifdef WITH_BRIDGE
	log__printf(ખાલી, MOSQ_LOG_INFO, "Bridge support available.");
#નહીંતર
	log__printf(ખાલી, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(ખાલી, MOSQ_LOG_INFO, "Persistence support available.");
#નહીંતર
	log__printf(ખાલી, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(ખાલી, MOSQ_LOG_INFO, "TLS support available.");
#નહીંતર
	log__printf(ખાલી, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(ખાલી, MOSQ_LOG_INFO, "TLS-PSK support available.");
#નહીંતર
	log__printf(ખાલી, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(ખાલી, MOSQ_LOG_INFO, "Websockets support available.");
#નહીંતર
	log__printf(ખાલી, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	જો(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(ખાલી, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


સ્થિર શૂન્ય post_shutdown_cleanup(શૂન્ય)
{
	સંરચના mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(ખાલી, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#ifdef WITH_PERSISTENCE
	persist__backup(true);
#endif
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#જો defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		જો(!ctxt->wsi)
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

	જો(db.config->pid_file){
		(શૂન્ય)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


સ્થિર શૂન્ય cjson_init(શૂન્ય)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ifdef WITH_FUZZING


પૂર્ણાંક mosquitto_fuzz_main(પૂર્ણાંક argc, અક્ષર *argv[])
#નહીંતર


પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
#endif
{
	સંરચના mosquitto__config config;
	પૂર્ણાંક rc;

	mosquitto_time_init();
	cjson_init();

#જો defined(WIN32) || defined(__CYGWIN__)
	જો(argc == 2){
		જો(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			પાછા 0;
		}નહીંતર જો(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			પાછા 0;
		}નહીંતર જો(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			પાછા 0;
		}
	}
#endif


#ifdef WIN32
	જો(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		જો(_setmaxstdio(2048) != 2048){
			log__printf(ખાલી, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, કદ(સંરચના mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(ખાલી);
	mosquitto_broker_ગાંઠ_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	જો(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		પાછા MOSQ_ERR_SUCCESS;
	}નહીંતર જો(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		પાછા rc;
	}

	જો(config.test_configuration){
		જો(!db.config_file){
			log__printf(ખાલી, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			પાછા MOSQ_ERR_INVAL;
		}નહીંતર{
			log__printf(ખાલી, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			પાછા MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	જો(rc){
		post_shutdown_cleanup();
		પાછા rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	જો(rc){
		post_shutdown_cleanup();
		પાછા rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	જો(rc){
		post_shutdown_cleanup();
		પાછા rc;
	}


	જો(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	જો(rc){
		post_shutdown_cleanup();
		પાછા rc;
	}

	rc = db__open(&config);
	જો(rc != MOSQ_ERR_SUCCESS){
		log__printf(ખાલી, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		પાછા rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	જો(rc){
		post_shutdown_cleanup();
		પાછા rc;
	}
	log__printf(ખાલી, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	જો(db.config_file){
		log__printf(ખાલી, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}નહીંતર{
		log__printf(ખાલી, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	જો(rc){
		post_shutdown_cleanup();
		પાછા rc;
	}
	rc = mosquitto_security_init(false);
	જો(rc){
		post_shutdown_cleanup();
		પાછા rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_સંદેશ_store_compact();

#ifdef WITH_SYS_વૃક્ષ
	sys_વૃક્ષ_init();
#endif

	rc = mux__init();
	જો(rc){
		post_shutdown_cleanup();
		પાછા rc;
	}

	rc = listeners__start();
	જો(rc){
		post_shutdown_cleanup();
		પાછા rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(ખાલી, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_ગણતરી);

	post_shutdown_cleanup();

	પાછા rc;
}

#ifdef WIN32


પૂર્ણાંક WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_લીટી, પૂર્ણાંક nCmdShow)
{
	અક્ષર **argv;
	પૂર્ણાંક argc = 1;
	અક્ષર *token;
	અક્ષર *saveptr = ખાલી;
	પૂર્ણાંક rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(કદ(અક્ષર *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_લીટી, " ", &saveptr);
	જ્યારે(token){
		argc++;
		argv = mosquitto_realloc(argv, કદ(અક્ષર *)*argc);
		જો(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			પાછા MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(ખાલી, " ", &saveptr);
	}
	rc = મુખ્ય(argc, argv);
	mosquitto_FREE(argv);
	પાછા rc;
}
#endif
