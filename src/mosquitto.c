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
#ບໍ່ດັ່ງນັ້ນ
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
ຈຳນວນເຕັມ g_listensock_count = 0;

ຈຳນວນເຕັມ g_run = 0;
#ifdef WITH_WRAP
#include <syslog.h>
ຈຳນວນເຕັມ allow_severity = LOG_INFO;
ຈຳນວນເຕັມ deny_severity = LOG_INFO;
#endif


static ຈຳນວນເຕັມ set_umask(void)
{
#ຖ້າ !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	const var_na *mask_s;
	var_na *endptr = NULL;
	long mask;


	mask_s = getenv("UMASK_SET");
	ຖ້າ(mask_s){
		errno = 0;
		mask = strtol(mask_s, &endptr, 8);
		ຖ້າ(errno || endptr == mask_s || *endptr != '\0'){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			ສົ່ງຄືນ MOSQ_ERR_INVAL;
		}
		ຖ້າ(mask < 000 || mask > 0777){
			log__printf(NULL, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			ສົ່ງຄືນ MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
static ຈຳນວນເຕັມ check_uid(const var_na *s, const var_na *name)
{
	var_na *endptr = NULL;
	long id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	ຖ້າ(errno || endptr == s || *endptr != '\0'){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		ສົ່ງຄືນ -1;
	}
	ຖ້າ(id < 0){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		ສົ່ງຄືນ -1;
	}
	ຖ້າ(id > INT_MAX){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, INT_MAX);
		ສົ່ງຄືນ -1;
	}
	ສົ່ງຄືນ (ຈຳນວນເຕັມ)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
static void print_pwname(void)
{
#ifndef WIN32
	sa_nracanaa passwd *pwd = getpwuid(geteuid());
	ຖ້າ(!pwd){
		log__printf(NULL, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}ບໍ່ດັ່ງນັ້ນ{
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


static ຈຳນວນເຕັມ drop_privileges(sa_nracanaa mosquitto__config *config)
{
#ຖ້າ !defined(__CYGWIN__) && !defined(WIN32)
	sa_nracanaa passwd *pwd;
	var_na *err;
	ຈຳນວນເຕັມ rc;
	const var_na *puid_s, *pgid_s;
	ຈຳນວນເຕັມ puid;
	ຈຳນວນເຕັມ pgid;

	const var_na *snap = getenv("SNAP_NAME");
	ຖ້າ(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	ຖ້າ(geteuid() == 0){
		ຖ້າ(puid_s || pgid_s){
			ຖ້າ(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				ຖ້າ(pgid < 0){
					ສົ່ງຄືນ MOSQ_ERR_INVAL;
				}ບໍ່ດັ່ງນັ້ນ ຖ້າ(pgid > 0){
					rc = setgid((gid_t)pgid);
					ຖ້າ(rc == -1){
						err = strerror(errno);
						log__printf(NULL, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						ສົ່ງຄືນ MOSQ_ERR_ERRNO;
					}
				}
			}
			ຖ້າ(puid_s){
				puid = check_uid(puid_s, "PUID");
				ຖ້າ(puid < 0){
					ສົ່ງຄືນ MOSQ_ERR_INVAL;
				}ບໍ່ດັ່ງນັ້ນ ຖ້າ(puid > 0){
					rc = setuid((uid_t)puid);
					ຖ້າ(rc == -1){
						err = strerror(errno);
						log__printf(NULL, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						ສົ່ງຄືນ MOSQ_ERR_ERRNO;
					}
				}
			}
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			ຖ້າ(!pwd){
				ຖ້າ(strcmp(config->user, "mosquitto")){
					log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					ສົ່ງຄືນ MOSQ_ERR_INVAL;
				}ບໍ່ດັ່ງນັ້ນ{
					log__printf(NULL, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					ຖ້າ(!pwd){
						log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						ສົ່ງຄືນ MOSQ_ERR_ERRNO;
					}
				}
			}
			ຖ້າ(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(NULL, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				ສົ່ງຄືນ MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			ຖ້າ(rc == -1){
				err = strerror(errno);
				log__printf(NULL, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				ສົ່ງຄືນ MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			ຖ້າ(rc == -1){
				err = strerror(errno);
				log__printf(NULL, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				ສົ່ງຄືນ MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		ຖ້າ(geteuid() == 0 || getegid() == 0){
			log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}ບໍ່ດັ່ງນັ້ນ{
		print_pwname();
	}

#ບໍ່ດັ່ງນັ້ນ
	UNUSED(config);
#endif
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


static void mosquitto__daemonise(void)
{
#ifndef WIN32
	var_na *err;
	pid_t pid;

	pid = fork();
	ຖ້າ(pid < 0){
		err = strerror(errno);
		log__printf(NULL, MOSQ_LOG_ERR, "Error in fork: %s", err);
		exit(1);
	}
	ຖ້າ(pid > 0){
		exit(0);
	}
	ຖ້າ(setsid() < 0){
		err = strerror(errno);
		log__printf(NULL, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		exit(1);
	}

	ຖ້າ(!freopen("/dev/null", "r", stdin)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		exit(1);
	}
	ຖ້າ(!freopen("/dev/null", "w", stdout)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		exit(1);
	}
	ຖ້າ(!freopen("/dev/null", "w", stderr)){
		log__printf(NULL, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		exit(1);
	}
#ບໍ່ດັ່ງນັ້ນ
	log__printf(NULL, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


static ຈຳນວນເຕັມ pid__write(void)
{
	FILE *pid;

	ຖ້າ(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		ຖ້າ(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}ບໍ່ດັ່ງນັ້ນ{
			log__printf(NULL, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			ສົ່ງຄືນ MOSQ_ERR_ERRNO;
		}
	}
	ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
}


static void report_features(void)
{
#ifdef WITH_BRIDGE
	log__printf(NULL, MOSQ_LOG_INFO, "Bridge support available.");
#ບໍ່ດັ່ງນັ້ນ
	log__printf(NULL, MOSQ_LOG_INFO, "Bridge support NOT available.");
#endif
#ifdef WITH_PERSISTENCE
	log__printf(NULL, MOSQ_LOG_INFO, "Persistence support available.");
#ບໍ່ດັ່ງນັ້ນ
	log__printf(NULL, MOSQ_LOG_INFO, "Persistence support NOT available.");
#endif
#ifdef WITH_TLS
	log__printf(NULL, MOSQ_LOG_INFO, "TLS support available.");
#ບໍ່ດັ່ງນັ້ນ
	log__printf(NULL, MOSQ_LOG_INFO, "TLS support NOT available.");
#endif
#ifdef FINAL_WITH_TLS_PSK
	log__printf(NULL, MOSQ_LOG_INFO, "TLS-PSK support available.");
#ບໍ່ດັ່ງນັ້ນ
	log__printf(NULL, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#endif
#ifdef WITH_WEBSOCKETS
	log__printf(NULL, MOSQ_LOG_INFO, "Websockets support available.");
#ບໍ່ດັ່ງນັ້ນ
	log__printf(NULL, MOSQ_LOG_INFO, "Websockets support NOT available.");
#endif
	ຖ້າ(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
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
#ຖ້າ defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		ຖ້າ(!ctxt->wsi)
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

	ຖ້າ(db.config->pid_file){
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


ຈຳນວນເຕັມ mosquitto_fuzz_main(ຈຳນວນເຕັມ argc, var_na *argv[])
#ບໍ່ດັ່ງນັ້ນ


ຈຳນວນເຕັມ main(ຈຳນວນເຕັມ argc, var_na *argv[])
#endif
{
	sa_nracanaa mosquitto__config config;
	ຈຳນວນເຕັມ rc;

	mosquitto_time_init();
	cjson_init();

#ຖ້າ defined(WIN32) || defined(__CYGWIN__)
	ຖ້າ(argc == 2){
		ຖ້າ(!strcmp(argv[1], "run")){
			service_run(argv[0]);
			ສົ່ງຄືນ 0;
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			ສົ່ງຄືນ 0;
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			ສົ່ງຄືນ 0;
		}
	}
#endif


#ifdef WIN32
	ຖ້າ(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		ຖ້າ(_setmaxstdio(2048) != 2048){
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
	ຖ້າ(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
	}ບໍ່ດັ່ງນັ້ນ ຖ້າ(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
	}

	ຖ້າ(config.test_configuration){
		ຖ້າ(!db.config_file){
			log__printf(NULL, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			ສົ່ງຄືນ MOSQ_ERR_INVAL;
		}ບໍ່ດັ່ງນັ້ນ{
			log__printf(NULL, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			ສົ່ງຄືນ MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	ຖ້າ(rc){
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	ຖ້າ(rc){
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	ຖ້າ(rc){
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
	}


	ຖ້າ(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	ຖ້າ(rc){
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
	}

	rc = db__open(&config);
	ຖ້າ(rc != MOSQ_ERR_SUCCESS){
		log__printf(NULL, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	ຖ້າ(rc){
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
	}
	log__printf(NULL, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	ຖ້າ(db.config_file){
		log__printf(NULL, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}ບໍ່ດັ່ງນັ້ນ{
		log__printf(NULL, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	ຖ້າ(rc){
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
	}
	rc = mosquitto_security_init(false);
	ຖ້າ(rc){
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db__msg_store_compact();

#ifdef WITH_SYS_TREE
	sys_tree__init();
#endif

	rc = mux__init();
	ຖ້າ(rc){
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
	}

	rc = listeners__start();
	ຖ້າ(rc){
		post_shutdown_cleanup();
		ສົ່ງຄືນ rc;
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

	ສົ່ງຄືນ rc;
}

#ifdef WIN32


ຈຳນວນເຕັມ WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, ຈຳນວນເຕັມ nCmdShow)
{
	var_na **argv;
	ຈຳນວນເຕັມ argc = 1;
	var_na *token;
	var_na *saveptr = NULL;
	ຈຳນວນເຕັມ rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(parimaa_na(var_na *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lpCmdLine, " ", &saveptr);
	ໃນຂະນະທີ່(token){
		argc++;
		argv = mosquitto_realloc(argv, parimaa_na(var_na *)*argc);
		ຖ້າ(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			ສົ່ງຄືນ MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(NULL, " ", &saveptr);
	}
	rc = main(argc, argv);
	mosquitto_FREE(argv);
	ສົ່ງຄືນ rc;
}
#endif
