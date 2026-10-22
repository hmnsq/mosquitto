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
पूर्णांक g_listensock_मोजणी = 0;

पूर्णांक g_run = 0;
#ifdef WITH_WRAP
#अन्तर्भाव <syslog.h>
पूर्णांक allow_severity = LOG_INFO;
पूर्णांक deny_severity = LOG_INFO;
#endif


स्थिर पूर्णांक set_umask(शून्य)
{
#जर !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	स्थिरांक अक्षर *mask_s;
	अक्षर *endptr = रिक्त;
	दीर्घ mask;


	mask_s = दो_पर्या("UMASK_SET");
	जर(mask_s){
		त्रुटि_सं = 0;
		mask = म_से_दीर्घ(mask_s, &endptr, 8);
		जर(त्रुटि_सं || endptr == mask_s || *endptr != '\0'){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			परत MOSQ_ERR_INVAL;
		}
		जर(mask < 000 || mask > 0777){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			परत MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#endif
	परत MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
स्थिर पूर्णांक check_uid(स्थिरांक अक्षर *s, स्थिरांक अक्षर *name)
{
	अक्षर *endptr = रिक्त;
	दीर्घ id;

	त्रुटि_सं = 0;
	id = म_से_दीर्घ(s, &endptr, 10);
	जर(त्रुटि_सं || endptr == s || *endptr != '\0'){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		परत -1;
	}
	जर(id < 0){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		परत -1;
	}
	जर(id > पूर्णांक_उच्च){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, पूर्णांक_उच्च);
		परत -1;
	}
	परत (पूर्णांक)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
स्थिर शून्य print_pwname(शून्य)
{
#ifndef WIN32
	संरचना passwd *pwd = getpwuid(geteuid());
	जर(!pwd){
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


स्थिर पूर्णांक drop_privileges(संरचना mosquitto__config *config)
{
#जर !defined(__CYGWIN__) && !defined(WIN32)
	संरचना passwd *pwd;
	अक्षर *err;
	पूर्णांक rc;
	स्थिरांक अक्षर *puid_s, *pgid_s;
	पूर्णांक puid;
	पूर्णांक pgid;

	स्थिरांक अक्षर *snap = दो_पर्या("SNAP_NAME");
	जर(snap && !म_भेद(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		परत MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = दो_पर्या("PUID");
	pgid_s = दो_पर्या("PGID");

	जर(geteuid() == 0){
		जर(puid_s || pgid_s){
			जर(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				जर(pgid < 0){
					परत MOSQ_ERR_INVAL;
				}अन्यथा जर(pgid > 0){
					rc = setgid((gid_t)pgid);
					जर(rc == -1){
						err = म_त्रुटि(त्रुटि_सं);
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						परत MOSQ_ERR_ERRNO;
					}
				}
			}
			जर(puid_s){
				puid = check_uid(puid_s, "PUID");
				जर(puid < 0){
					परत MOSQ_ERR_INVAL;
				}अन्यथा जर(puid > 0){
					rc = setuid((uid_t)puid);
					जर(rc == -1){
						err = म_त्रुटि(त्रुटि_सं);
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						परत MOSQ_ERR_ERRNO;
					}
				}
			}
		}अन्यथा जर(config->user && म_भेद(config->user, "root")){
			pwd = getpwnam(config->user);
			जर(!pwd){
				जर(म_भेद(config->user, "mosquitto")){
					log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					परत MOSQ_ERR_INVAL;
				}अन्यथा{
					log__printf(रिक्त, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					जर(!pwd){
						log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						परत MOSQ_ERR_ERRNO;
					}
				}
			}
			जर(initgroups(config->user, pwd->pw_gid) == -1){
				err = म_त्रुटि(त्रुटि_सं);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				परत MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			जर(rc == -1){
				err = म_त्रुटि(त्रुटि_सं);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				परत MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			जर(rc == -1){
				err = म_त्रुटि(त्रुटि_सं);
				log__printf(रिक्त, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				परत MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		जर(geteuid() == 0 || getegid() == 0){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}अन्यथा{
		print_pwname();
	}

#अन्यथा
	UNUSED(config);
#endif
	परत MOSQ_ERR_SUCCESS;
}


स्थिर शून्य mosquitto__daemonise(शून्य)
{
#ifndef WIN32
	अक्षर *err;
	pid_t pid;

	pid = विभाजन();
	जर(pid < 0){
		err = म_त्रुटि(त्रुटि_सं);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in fork: %s", err);
		निकास(1);
	}
	जर(pid > 0){
		निकास(0);
	}
	जर(setsid() < 0){
		err = म_त्रुटि(त्रुटि_सं);
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		निकास(1);
	}

	जर(!ख_व_खोलो("/dev/null", "r", मानक_निवेश)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", म_त्रुटि(त्रुटि_सं));
		निकास(1);
	}
	जर(!ख_व_खोलो("/dev/null", "w", मानक_निकास)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", म_त्रुटि(त्रुटि_सं));
		निकास(1);
	}
	जर(!ख_व_खोलो("/dev/null", "w", मानक_त्रुटि)){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", म_त्रुटि(त्रुटि_सं));
		निकास(1);
	}
#अन्यथा
	log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#endif
}


स्थिर पूर्णांक pid__write(शून्य)
{
	खाता *pid;

	जर(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		जर(pid){
			ख_लिखो(pid, "%d", getpid());
			ख_बंद(pid);
		}अन्यथा{
			log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			परत MOSQ_ERR_ERRNO;
		}
	}
	परत MOSQ_ERR_SUCCESS;
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
	जर(दो_पर्या("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
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
#जर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		जर(!ctxt->wsi)
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

	जर(db.config->pid_file){
		(शून्य)हटाओ(db.config->pid_file);
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


पूर्णांक mosquitto_fuzz_main(पूर्णांक argc, अक्षर *argv[])
#अन्यथा


पूर्णांक main(पूर्णांक argc, अक्षर *argv[])
#endif
{
	संरचना mosquitto__config config;
	पूर्णांक rc;

	mosquitto_time_init();
	cjson_init();

#जर defined(WIN32) || defined(__CYGWIN__)
	जर(argc == 2){
		जर(!म_भेद(argv[1], "run")){
			service_run(argv[0]);
			परत 0;
		}अन्यथा जर(!म_भेद(argv[1], "install")){
			service_install(argv[0]);
			परत 0;
		}अन्यथा जर(!म_भेद(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			परत 0;
		}
	}
#endif


#ifdef WIN32
	जर(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		जर(_setmaxstdio(2048) != 2048){
			log__printf(रिक्त, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#endif

	स_रखो(&db, 0, आकार(संरचना mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = समय(रिक्त);
	mosquitto_broker_गाठ_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	जर(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		परत MOSQ_ERR_SUCCESS;
	}अन्यथा जर(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		परत rc;
	}

	जर(config.test_configuration){
		जर(!db.config_file){
			log__printf(रिक्त, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			परत MOSQ_ERR_INVAL;
		}अन्यथा{
			log__printf(रिक्त, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			परत MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	जर(rc){
		post_shutdown_cleanup();
		परत rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	जर(rc){
		post_shutdown_cleanup();
		परत rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	जर(rc){
		post_shutdown_cleanup();
		परत rc;
	}


	जर(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	जर(rc){
		post_shutdown_cleanup();
		परत rc;
	}

	rc = db__open(&config);
	जर(rc != MOSQ_ERR_SUCCESS){
		log__printf(रिक्त, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		परत rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	जर(rc){
		post_shutdown_cleanup();
		परत rc;
	}
	log__printf(रिक्त, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	जर(db.config_file){
		log__printf(रिक्त, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}अन्यथा{
		log__printf(रिक्त, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	जर(rc){
		post_shutdown_cleanup();
		परत rc;
	}
	rc = mosquitto_security_init(false);
	जर(rc){
		post_shutdown_cleanup();
		परत rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_संदेश_store_compact();

#ifdef WITH_SYS_वृक्ष
	sys_वृक्ष_init();
#endif

	rc = mux__init();
	जर(rc){
		post_shutdown_cleanup();
		परत rc;
	}

	rc = listeners__start();
	जर(rc){
		post_shutdown_cleanup();
		परत rc;
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

	rc = mosquitto_main_loop(g_listensock, g_listensock_मोजणी);

	post_shutdown_cleanup();

	परत rc;
}

#ifdef WIN32


पूर्णांक WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lp_Cmd_ओळ, पूर्णांक nCmdShow)
{
	अक्षर **argv;
	पूर्णांक argc = 1;
	अक्षर *token;
	अक्षर *saveptr = रिक्त;
	पूर्णांक rc;

	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(आकार(अक्षर *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_ओळ, " ", &saveptr);
	पर्यंत(token){
		argc++;
		argv = mosquitto_realloc(argv, आकार(अक्षर *)*argc);
		जर(!argv){
			ख_लिखो(मानक_त्रुटि, "Error: Out of memory.\n");
			परत MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(रिक्त, " ", &saveptr);
	}
	rc = main(argc, argv);
	mosquitto_FREE(argv);
	परत rc;
}
#endif
