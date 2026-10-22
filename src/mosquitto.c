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

#اگر_غیر_تعریف_شدہ WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ WIN32
#شامل <pwd.h>
#ورنہ
#شامل <process.h>
#شامل <winsock2.h>
#شامل <ws2tcpip.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ WIN32
#  include <sys/time.h>
#اگر_ختم

#شامل <errno.h>
#شامل <limits.h>
#شامل <signal.h>
#شامل <stdio.h>
#شامل <string.h>
#اگر_تعریف_شدہ WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#اگر_ختم
#اگر_تعریف_شدہ WITH_WRAP
#شامل <tcpd.h>
#اگر_ختم

#شامل "mosquitto_broker_internal.h"
#شامل "util_mosq.h"

ساخت mosquitto_db db;

ساخت mosquitto__listener_sock *g_listensock = تہی;
صحیح g_listensock_گنتی = 0;

صحیح g_تسلسل = 0;
#اگر_تعریف_شدہ WITH_WRAP
#شامل <syslog.h>
صحیح allow_severity = LOG_INFO;
صحیح deny_severity = LOG_INFO;
#اگر_ختم


ساکن صحیح سیٹ_umask(خالی)
{
#اگر !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	ثابت حرف *نقاب_s;
	حرف *endptr = تہی;
	طویل نقاب;


	نقاب_s = getenv("UMASK_SET");
	اگر(نقاب_s){
		errno = 0;
		نقاب = strtol(نقاب_s, &endptr, 8);
		اگر(errno || endptr == نقاب_s || *endptr != '\0'){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			واپس MOSQ_ERR_INVAL;
		}
		اگر(نقاب < 000 || نقاب > 0777){
			log__printf(تہی, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			واپس MOSQ_ERR_INVAL;
		}
		umask((mode_t)نقاب);
	}
#اگر_ختم
	واپس MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
ساکن صحیح check_uid(ثابت حرف *s, ثابت حرف *name)
{
	حرف *endptr = تہی;
	طویل id;

	errno = 0;
	id = strtol(s, &endptr, 10);
	اگر(errno || endptr == s || *endptr != '\0'){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		واپس -1;
	}
	اگر(id < 0){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		واپس -1;
	}
	اگر(id > زیادہ_سے_زیادہ_عدد){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, زیادہ_سے_زیادہ_عدد);
		واپس -1;
	}
	واپس (صحیح)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
ساکن خالی print_pwname(خالی)
{
#اگر_غیر_تعریف_شدہ WIN32
	ساخت passwd *pwd = getpwuid(geteuid());
	اگر(!pwd){
		log__printf(تہی, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
	}
#اگر_ختم
}


/* mosquitto shouldn't run as root.
 * This function will attempt to change to an unprivileged user and group if
 * running as root. The user is given in config->user.
 * Returns 1 on failure (unknown user, setuid/setgid failure)
 * Returns 0 on success.
 * Note that setting config->user to "root" does not produce an error, but it
 * strongly discouraged.
 */


ساکن صحیح drop_privileges(ساخت mosquitto__config *config)
{
#اگر !defined(__CYGWIN__) && !defined(WIN32)
	ساخت passwd *pwd;
	حرف *err;
	صحیح rc;
	ثابت حرف *puid_s, *pgid_s;
	صحیح puid;
	صحیح pgid;

	ثابت حرف *snap = getenv("SNAP_NAME");
	اگر(snap && !strcmp(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		واپس MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = getenv("PUID");
	pgid_s = getenv("PGID");

	اگر(geteuid() == 0){
		اگر(puid_s || pgid_s){
			اگر(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				اگر(pgid < 0){
					واپس MOSQ_ERR_INVAL;
				}ورنہ اگر(pgid > 0){
					rc = setgid((gid_t)pgid);
					اگر(rc == -1){
						err = strerror(errno);
						log__printf(تہی, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						واپس MOSQ_ERR_ERRNO;
					}
				}
			}
			اگر(puid_s){
				puid = check_uid(puid_s, "PUID");
				اگر(puid < 0){
					واپس MOSQ_ERR_INVAL;
				}ورنہ اگر(puid > 0){
					rc = setuid((uid_t)puid);
					اگر(rc == -1){
						err = strerror(errno);
						log__printf(تہی, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						واپس MOSQ_ERR_ERRNO;
					}
				}
			}
		}ورنہ اگر(config->user && strcmp(config->user, "root")){
			pwd = getpwnam(config->user);
			اگر(!pwd){
				اگر(strcmp(config->user, "mosquitto")){
					log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					واپس MOSQ_ERR_INVAL;
				}ورنہ{
					log__printf(تہی, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					اگر(!pwd){
						log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						واپس MOSQ_ERR_ERRNO;
					}
				}
			}
			اگر(initgroups(config->user, pwd->pw_gid) == -1){
				err = strerror(errno);
				log__printf(تہی, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				واپس MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			اگر(rc == -1){
				err = strerror(errno);
				log__printf(تہی, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				واپس MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			اگر(rc == -1){
				err = strerror(errno);
				log__printf(تہی, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				واپس MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		اگر(geteuid() == 0 || getegid() == 0){
			log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}ورنہ{
		print_pwname();
	}

#ورنہ
	غیر_مستعمل(config);
#اگر_ختم
	واپس MOSQ_ERR_SUCCESS;
}


ساکن خالی mosquitto__daemonise(خالی)
{
#اگر_غیر_تعریف_شدہ WIN32
	حرف *err;
	pid_t pid;

	pid = fork();
	اگر(pid < 0){
		err = strerror(errno);
		log__printf(تہی, MOSQ_LOG_ERR, "Error in fork: %s", err);
		باہر_نکلو(1);
	}
	اگر(pid > 0){
		باہر_نکلو(0);
	}
	اگر(setsid() < 0){
		err = strerror(errno);
		log__printf(تہی, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		باہر_نکلو(1);
	}

	اگر(!freopen("/dev/null", "r", stdin)){
		log__printf(تہی, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", strerror(errno));
		باہر_نکلو(1);
	}
	اگر(!freopen("/dev/null", "w", stdout)){
		log__printf(تہی, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", strerror(errno));
		باہر_نکلو(1);
	}
	اگر(!freopen("/dev/null", "w", stderr)){
		log__printf(تہی, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", strerror(errno));
		باہر_نکلو(1);
	}
#ورنہ
	log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#اگر_ختم
}


ساکن صحیح pid__write(خالی)
{
	FILE *pid;

	اگر(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		اگر(pid){
			fprintf(pid, "%d", getpid());
			fclose(pid);
		}ورنہ{
			log__printf(تہی, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			واپس MOSQ_ERR_ERRNO;
		}
	}
	واپس MOSQ_ERR_SUCCESS;
}


ساکن خالی report_features(خالی)
{
#اگر_تعریف_شدہ WITH_BRIDGE
	log__printf(تہی, MOSQ_LOG_INFO, "Bridge support available.");
#ورنہ
	log__printf(تہی, MOSQ_LOG_INFO, "Bridge support NOT available.");
#اگر_ختم
#اگر_تعریف_شدہ WITH_PERSISTENCE
	log__printf(تہی, MOSQ_LOG_INFO, "Persistence support available.");
#ورنہ
	log__printf(تہی, MOSQ_LOG_INFO, "Persistence support NOT available.");
#اگر_ختم
#اگر_تعریف_شدہ WITH_TLS
	log__printf(تہی, MOSQ_LOG_INFO, "TLS support available.");
#ورنہ
	log__printf(تہی, MOSQ_LOG_INFO, "TLS support NOT available.");
#اگر_ختم
#اگر_تعریف_شدہ FINAL_WITH_TLS_PSK
	log__printf(تہی, MOSQ_LOG_INFO, "TLS-PSK support available.");
#ورنہ
	log__printf(تہی, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#اگر_ختم
#اگر_تعریف_شدہ WITH_WEBSOCKETS
	log__printf(تہی, MOSQ_LOG_INFO, "Websockets support available.");
#ورنہ
	log__printf(تہی, MOSQ_LOG_INFO, "Websockets support NOT available.");
#اگر_ختم
	اگر(getenv("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(تہی, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


ساکن خالی post_shutdown_cleanup(خالی)
{
	ساخت mosquitto *ctxt, *ctxt_tmp;

	/* FIXME - this isn't quite right, all wills with will delay zero should be
	 * sent now, but those with positive will delay should be persisted and
	 * restored, pending the client reconnecting in time. */
	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
		context__send_will(ctxt);
	}
	will_delay__send_all();
	session_expiry_جوڑو_on_shutdown();

	/* Set to true only after persistence events have been processed */
	db.shutdown = true;
	log__printf(تہی, MOSQ_LOG_INFO, "mosquitto version %s terminating", نسخہ);

	broker_control__cleanup();

#اگر_تعریف_شدہ WITH_PERSISTENCE
	persist__backup(true);
#اگر_ختم
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#اگر defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		اگر(!ctxt->wsi)
#اگر_ختم
		{
			ctxt->is_persisted = false; /* prevent persistence removal */
			context__cleanup(ctxt, true);
		}
	}
	HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
		ctxt->is_persisted = false; /* prevent persistence removal */
		context__cleanup(ctxt, true);
	}
#اگر_تعریف_شدہ WITH_BRIDGE
	bridge__db_cleanup();
#اگر_ختم
	context__free_disused();
	keepalive__cleanup();

#اگر_تعریف_شدہ WITH_TLS
	mosquitto_FREE(db.tls_keylog);
#اگر_ختم
	db__close();

	plugin__unload_all();
	mosquitto_security_cleanup(false);

	اگر(db.config->pid_file){
		(خالی)remove(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


ساکن خالی cjson_init(خالی)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#اگر_تعریف_شدہ WITH_FUZZING


صحیح mosquitto_fuzz_main(صحیح argc, حرف *argv[])
#ورنہ


صحیح مین(صحیح argc, حرف *argv[])
#اگر_ختم
{
	ساخت mosquitto__config config;
	صحیح rc;

	mosquitto_وقت_init();
	cjson_init();

#اگر defined(WIN32) || defined(__CYGWIN__)
	اگر(argc == 2){
		اگر(!strcmp(argv[1], "run")){
			service_تسلسل(argv[0]);
			واپس 0;
		}ورنہ اگر(!strcmp(argv[1], "install")){
			service_install(argv[0]);
			واپس 0;
		}ورنہ اگر(!strcmp(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			واپس 0;
		}
	}
#اگر_ختم


#اگر_تعریف_شدہ WIN32
	اگر(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		اگر(_setmaxstdio(2048) != 2048){
			log__printf(تہی, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#اگر_ختم

	memset(&db, 0, حجم(ساخت mosquitto_db));
	db.now_s = mosquitto_وقت();
	db.now_real_s = وقت(تہی);
	mosquitto_broker_گرہ_id_سیٹ(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config_parse_دلائل(&config, argc, argv);
	اگر(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		واپس MOSQ_ERR_SUCCESS;
	}ورنہ اگر(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		واپس rc;
	}

	اگر(config.test_configuration){
		اگر(!db.config_file){
			log__printf(تہی, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			واپس MOSQ_ERR_INVAL;
		}ورنہ{
			log__printf(تہی, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			واپس MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	اگر(rc){
		post_shutdown_cleanup();
		واپس rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	اگر(rc){
		post_shutdown_cleanup();
		واپس rc;
	}
	/* Set umask based on environment variable */
	rc = سیٹ_umask();
	اگر(rc){
		post_shutdown_cleanup();
		واپس rc;
	}


	اگر(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	اگر(rc){
		post_shutdown_cleanup();
		واپس rc;
	}

	rc = db__open(&config);
	اگر(rc != MOSQ_ERR_SUCCESS){
		log__printf(تہی, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		واپس rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	اگر(rc){
		post_shutdown_cleanup();
		واپس rc;
	}
	log__printf(تہی, MOSQ_LOG_INFO, "mosquitto version %s starting", نسخہ);
	اگر(db.config_file){
		log__printf(تہی, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}ورنہ{
		log__printf(تہی, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin_بھرو_all();
	اگر(rc){
		post_shutdown_cleanup();
		واپس rc;
	}
	rc = mosquitto_security_init(false);
	اگر(rc){
		post_shutdown_cleanup();
		واپس rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_پیغام_store_compact();

#اگر_تعریف_شدہ WITH_SYS_درخت
	sys_درخت_init();
#اگر_ختم

	rc = mux__init();
	اگر(rc){
		post_shutdown_cleanup();
		واپس rc;
	}

	rc = listeners_آغاز();
	اگر(rc){
		post_shutdown_cleanup();
		واپس rc;
	}


#اگر_تعریف_شدہ WITH_BRIDGE
	bridge_آغاز_all();
#اگر_ختم

	broker_control__init();

	g_تسلسل = 1;
	signal_تیاری();

	log__printf(تہی, MOSQ_LOG_INFO, "mosquitto version %s running", نسخہ);
#اگر_تعریف_شدہ WITH_SYSTEMD
	sd_notify(0, "READY=1");
#اگر_ختم

	rc = mosquitto_main_loop(g_listensock, g_listensock_گنتی);

	post_shutdown_cleanup();

	واپس rc;
}

#اگر_تعریف_شدہ WIN32


صحیح WINAPI WinMain(HINSTANCE hInstance, HINSTANCE h_پچھلا_Instance, LPSTR lp_Cmd_سطر, صحیح nCmdShow)
{
	حرف **argv;
	صحیح argc = 1;
	حرف *token;
	حرف *saveptr = تہی;
	صحیح rc;

	غیر_مستعمل(hInstance);
	غیر_مستعمل(h_پچھلا_Instance);
	غیر_مستعمل(nCmdShow);

	argv = mosquitto_malloc(حجم(حرف *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_سطر, " ", &saveptr);
	جب(token){
		argc++;
		argv = mosquitto_realloc(argv, حجم(حرف *)*argc);
		اگر(!argv){
			fprintf(stderr, "Error: Out of memory.\n");
			واپس MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(تہی, " ", &saveptr);
	}
	rc = مین(argc, argv);
	mosquitto_FREE(argv);
	واپس rc;
}
#اگر_ختم
