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

#include "config.h"

#ifndef WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#endif

#ifndef WIN32
#include <pwd.h>
#ᯀᯮᯝᯂᯪ
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifndef WIN32
#  include <sys/time.h>
#endif

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#ifdef WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif
#ifdef WITH_WRAP
#include <tcpd.h>
#endif

#include "mosquitto_broker_internal.h"
#include "util_mosq.h"

sa_nracanaa mosquitto_db db;

sa_nracanaa mosquitto__listener_sock *g_listensock = NULL;
puur_naa_nka g_listensock_count = 0;

puur_naa_nka g_run = 0;
#ifdef WITH_WRAP
#include <syslog.h>
puur_naa_nka allow_severity = LOG_INFO;
puur_naa_nka deny_severity = LOG_INFO;
#endif


static puur_naa_nka set_umask(void)
{
#ᯀᯮᯝ !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	const var_na *mask_s;
	var_na *endptr = NULL;
	long mask;


	mask_s = getenv("UMASK_SET");
	ᯀᯮᯝ(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		ᯀᯮᯝ(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			ᯀᯮᯞ MOSQ_ERR_INVAL;
		}
		ᯀᯮᯝ(mask < 000 || mask > 0777){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			ᯀᯮᯞ MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
static puur_naa_nka check_uid(const var_na *s, const var_na *name)
{
	var_na *endptr = NULL;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	ᯀᯮᯝ(errno || endptr == s || *endptr != '\0'){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		ᯀᯮᯞ -1;
	}
	ᯀᯮᯝ(id < 0){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		ᯀᯮᯞ -1;
	}
	ᯀᯮᯝ(id > INT_MAX){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		ᯀᯮᯞ -1;
	}
	ᯀᯮᯞ (puur_naa_nka)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
static void print_pwname(void)
{
#ifndef WIN32
	sa_nracanaa passwd *pwd = getpwuid(geteuid());
	ᯀᯮᯝ(!pwd){
		log__printf(NULL, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}ᯀᯮᯝᯂᯪ{
		log__printf(NULL, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
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


static puur_naa_nka drop_privileges(sa_nracanaa mosquitto__config *config)
{
#ᯀᯮᯝ !defined(__CYGWIN__) && !defined(WIN32)
	sa_nracanaa passwd *pwd;
	var_na *err;
	puur_naa_nka rc;
	const var_na *puid_s, *pgid_s;
	puur_naa_nka puid;
	puur_naa_nka pgid;

	const var_na *snap = getenv("SNAP_NAME");
	ᯀᯮᯝ(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		ᯀᯮᯞ MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	ᯀᯮᯝ(geteuid() == 0){
		ᯀᯮᯝ(puid_s || pgid_s){
			ᯀᯮᯝ(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				ᯀᯮᯝ(pgid < 0){
					ᯀᯮᯞ MOSQ_ERR_INVAL;
				}ᯀᯮᯝᯂᯪ ᯀᯮᯝ(pgid > 0){
					rc = setgid((gid_t)pgid);
					ᯀᯮᯝ(rc == -1){
						err = strerror(errno);
						log__printf(NULL, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						ᯀᯮᯞ MOSQ_ERR_ERRNO;
					}
				}
			}
			ᯀᯮᯝ(puid_s){
				puid = check_uid(puid_s, "PUID");
				ᯀᯮᯝ(puid < 0){
					ᯀᯮᯞ MOSQ_ERR_INVAL;
				}ᯀᯮᯝᯂᯪ ᯀᯮᯝ(puid > 0){
					rc = setuid((uid_t)puid);
					ᯀᯮᯝ(rc == -1){
						err = strerror(errno);
						log__printf(NULL, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						ᯀᯮᯞ MOSQ_ERR_ERRNO;
					}
				}
			}
		}ᯀᯮᯝᯂᯪ ᯀᯮᯝ(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			ᯀᯮᯝ(!pwd){
				ᯀᯮᯝ(strcmp(config->user, "mosquitto")){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					ᯀᯮᯞ MOSQ_ERR_INVAL;
				}ᯀᯮᯝᯂᯪ{
					log__printf(NULL, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					ᯀᯮᯝ(!pwd){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						ᯀᯮᯞ MOSQ_ERR_ERRNO;
					}
				}
			}
			ᯀᯮᯝ(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(NULL, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				ᯀᯮᯞ MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			ᯀᯮᯝ(rc == -1){
				err = strerror(errno);
				log__printf(NULL, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				ᯀᯮᯞ MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			ᯀᯮᯝ(rc == -1){
				err = strerror(errno);
				log__printf(NULL, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				ᯀᯮᯞ MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		ᯀᯮᯝ(geteuid() == 0 || getegid() == 0){
			log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}ᯀᯮᯝᯂᯪ{
		print_pwname();
	}

#ᯀᯮᯝᯂᯪ
	UNUSED(config);
#endif
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


static void mosquitto__daemonise(void)
{
#ifndef WIN32
	var_na *err;
	pid_t pid;

	pid = fork();
	ᯀᯮᯝ(pid < 0){
		err = strerror(errno);
		log__printf(NULL, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	ᯀᯮᯝ(pid > 0){
		exit(0);
	}
	ᯀᯮᯝ(setsid() < 0){
		err = strerror(errno);
		log__printf(NULL, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	ᯀᯮᯝ(!freopen("/dev/null", "r", stdin)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	ᯀᯮᯝ(!freopen("/dev/null", "w", stdout)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	ᯀᯮᯝ(!freopen("/dev/null", "w", stderr)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#ᯀᯮᯝᯂᯪ
	log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


static puur_naa_nka pid__write(void)
{
	FILE *pid;

	ᯀᯮᯝ(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		ᯀᯮᯝ(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}ᯀᯮᯝᯂᯪ{
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			ᯀᯮᯞ MOSQ_ERR_ERRNO;
		}
	}
	ᯀᯮᯞ MOSQ_ERR_SUCCESS;
}


static void report_features(void)
{
#ifdef WITH_BRIDGE
	log__printf(NULL, MOSQ_LOG_INFO, "Bridge support available.");
#ᯀᯮᯝᯂᯪ
	log__printf(NULL, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(NULL, MOSQ_LOG_INFO, "Persistence support available.");
#ᯀᯮᯝᯂᯪ
	log__printf(NULL, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(NULL, MOSQ_LOG_INFO, "TLS support available.");
#ᯀᯮᯝᯂᯪ
	log__printf(NULL, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(NULL, MOSQ_LOG_INFO, "TLS-PSK support available.");
#ᯀᯮᯝᯂᯪ
	log__printf(NULL, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(NULL, MOSQ_LOG_INFO, "Websockets support available.");
#ᯀᯮᯝᯂᯪ
	log__printf(NULL, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	ᯀᯮᯝ(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(NULL, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


static void post_shutdown_cleanup(void)
{
	sa_nracanaa mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(NULL, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#ifdef WITH_PERSISTENCE
	persist__backup(true);
#endif
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#ᯀᯮᯝ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ᯀᯮᯝ(!ctxt->wsi)
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

	ᯀᯮᯝ(db.config->pid_file){
		(void)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


static void cjson_init(void)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#ifdef WITH_FUZZING


puur_naa_nka mosquitto_fuzz_main(puur_naa_nka argc, var_na *argv[])
#ᯀᯮᯝᯂᯪ


puur_naa_nka main(puur_naa_nka argc, var_na *argv[])
#endif
{
	sa_nracanaa mosquitto__config config;
	puur_naa_nka rc;

	mosquitto_time_init();
	cjson_init();

#ᯀᯮᯝ defined(WIN32) || defined(__CYGWIN__)
	ᯀᯮᯝ(argc == 2){
		ᯀᯮᯝ(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			ᯀᯮᯞ 0;
		}ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			ᯀᯮᯞ 0;
		}ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			ᯀᯮᯞ 0;
		}
	}
#endif


#ifdef WIN32
	ᯀᯮᯝ(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		ᯀᯮᯝ(_setmaxstdio(2048) != 2048){
			log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	memset(&db, 0, parimaa_na(sa_nracanaa mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = time(NULL);
	mosquitto_broker_node_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	ᯀᯮᯝ(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		ᯀᯮᯞ MOSQ_ERR_SUCCESS;
	}ᯀᯮᯝᯂᯪ ᯀᯮᯝ(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}

	ᯀᯮᯝ(config.test_configuration){
		ᯀᯮᯝ(!db.config_file){
			log__printf(NULL, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			ᯀᯮᯞ MOSQ_ERR_INVAL;
		}ᯀᯮᯝᯂᯪ{
			log__printf(NULL, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			ᯀᯮᯞ MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	ᯀᯮᯝ(rc){
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	ᯀᯮᯝ(rc){
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	ᯀᯮᯝ(rc){
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}


	ᯀᯮᯝ(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	ᯀᯮᯝ(rc){
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}

	rc = db__open(&config);
	ᯀᯮᯝ(rc != MOSQ_ERR_SUCCESS){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	ᯀᯮᯝ(rc){
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}
	log__printf(NULL, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	ᯀᯮᯝ(db.config_file){
		log__printf(NULL, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}ᯀᯮᯝᯂᯪ{
		log__printf(NULL, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	ᯀᯮᯝ(rc){
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}
	rc = mosquitto_security_init(false);
	ᯀᯮᯝ(rc){
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db__msg_store_compact();

#ifdef WITH_SYS_TREE
	sys_tree__init();
#endif

	rc = mux__init();
	ᯀᯮᯝ(rc){
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}

	rc = listeners__start();
	ᯀᯮᯝ(rc){
		post_shutdown_cleanup();
		ᯀᯮᯞ rc;
	}


#ifdef WITH_BRIDGE
	bridge__start_all();
#endif

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(NULL, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#ifdef WITH_SYSTEMD
	sd_notify(0, "READY=1");
#endif

	rc = mosquitto_main_loop(g_listensock, g_listensock_count);

	post_shutdown_cleanup();

	ᯀᯮᯞ rc;
}

#ifdef WIN32


puur_naa_nka WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, puur_naa_nka nCmdShow)
{
	var_na **argv;
	puur_naa_nka argc = 1;
	var_na *token;
	var_na *saveptr = NULL;
	puur_naa_nka rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(parimaa_na(var_na *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lpCmdLine, " ", &saveptr);
	ᯀᯬᯝ(token){
		argc++;
		argv = mosquitto_realloc(argv, parimaa_na(var_na *)*argc);
		ᯀᯮᯝ(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			ᯀᯮᯞ MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(NULL, " ", &saveptr);
	}
	rc = main(argc, argv);
	mosquitto_FREE(argv);
	ᯀᯮᯞ rc;
}
#endif
