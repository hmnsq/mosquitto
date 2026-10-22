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

#समावेश "config.h"

#अगर_अघोषित WIN32
/* For initgroups() */
#  include <unistd.h>
#  include <grp.h>
#  include <assert.h>
/* For umask() */
#  include <sys/stat.h>
#पूर्ण_अगर

#अगर_अघोषित WIN32
#समावेश <pwd.h>
#अन्यथा
#समावेश <process.h>
#समावेश <winsock2.h>
#समावेश <ws2tcpip.h>
#पूर्ण_अगर

#अगर_अघोषित WIN32
#  include <sys/time.h>
#पूर्ण_अगर

#समावेश <errno.h>
#समावेश <limits.h>
#समावेश <signal.h>
#समावेश <stdio.h>
#समावेश <string.h>
#अगर_घोषित WITH_SYSTEMD
#  include <systemd/sd-daemon.h>
#पूर्ण_अगर
#अगर_घोषित WITH_WRAP
#समावेश <tcpd.h>
#पूर्ण_अगर

#समावेश "mosquitto_broker_internal.h"
#समावेश "util_mosq.h"

काष्ठा mosquitto_db db;

काष्ठा mosquitto__listener_sock *g_listensock = शून्य;
पूर्णांक g_listensock_गिनती = 0;

पूर्णांक g_run = 0;
#अगर_घोषित WITH_WRAP
#समावेश <syslog.h>
पूर्णांक allow_severity = LOG_INFO;
पूर्णांक deny_severity = LOG_INFO;
#पूर्ण_अगर


अटल पूर्णांक set_umask(व्योम)
{
#अगर !defined(__CYGWIN__) && !defined(WIN32)
	/* This affects files that are written to, apart from those that are
	 * created using mosquitto_fopen(..., restrict_read=true), which sets a
	 * umask of 077. */
	स्थिर अक्षर *mask_s;
	अक्षर *endptr = शून्य;
	दीर्घ mask;


	mask_s = दो_पर्या("UMASK_SET");
	अगर(mask_s){
		त्रुटि_सं = 0;
		mask = म_से_दीर्घ(mask_s, &endptr, 8);
		अगर(त्रुटि_सं || endptr == mask_s || *endptr != '\0'){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable not a valid octal number.");
			वापस MOSQ_ERR_INVAL;
		}
		अगर(mask < 000 || mask > 0777){
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: UMASK_SET environment variable out of range.");
			वापस MOSQ_ERR_INVAL;
		}
		umask((mode_t)mask);
	}
#पूर्ण_अगर
	वापस MOSQ_ERR_SUCCESS;
}


/* coverity[ +tainted_string_sanitize_content : arg-0 ] */
अटल पूर्णांक check_uid(स्थिर अक्षर *s, स्थिर अक्षर *name)
{
	अक्षर *endptr = शून्य;
	दीर्घ id;

	त्रुटि_सं = 0;
	id = म_से_दीर्घ(s, &endptr, 10);
	अगर(त्रुटि_सं || endptr == s || *endptr != '\0'){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s not a valid ID '%s'", name, s);
		वापस -1;
	}
	अगर(id < 0){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s must not be negative", name);
		वापस -1;
	}
	अगर(id > पूर्णांक_उच्च){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: %s must not be less than %d", name, पूर्णांक_उच्च);
		वापस -1;
	}
	वापस (पूर्णांक)id;
}


/* Prints the name of the process user from its user id if not found
 * it simply prints out the user id
 */
अटल व्योम print_pwname(व्योम)
{
#अगर_अघोषित WIN32
	काष्ठा passwd *pwd = getpwuid(geteuid());
	अगर(!pwd){
		log__printf(शून्य, MOSQ_LOG_INFO, "Info: running mosquitto as user id: %i.", geteuid());
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_INFO, "Info: running mosquitto as user: %s.", pwd->pw_name);
	}
#पूर्ण_अगर
}


/* mosquitto shouldn't run as root.
 * This function will attempt to change to an unprivileged user and group if
 * running as root. The user is given in config->user.
 * Returns 1 on failure (unknown user, setuid/setgid failure)
 * Returns 0 on success.
 * Note that setting config->user to "root" does not produce an error, but it
 * strongly discouraged.
 */


अटल पूर्णांक drop_privileges(काष्ठा mosquitto__config *config)
{
#अगर !defined(__CYGWIN__) && !defined(WIN32)
	काष्ठा passwd *pwd;
	अक्षर *err;
	पूर्णांक rc;
	स्थिर अक्षर *puid_s, *pgid_s;
	पूर्णांक puid;
	पूर्णांक pgid;

	स्थिर अक्षर *snap = दो_पर्या("SNAP_NAME");
	अगर(snap && !म_भेद(snap, "mosquitto")){
		/* Don't attempt to drop privileges if running as a snap */
		वापस MOSQ_ERR_SUCCESS;
	}

	/* PUID and PGID are docker custom user mappings */
	puid_s = दो_पर्या("PUID");
	pgid_s = दो_पर्या("PGID");

	अगर(geteuid() == 0){
		अगर(puid_s || pgid_s){
			अगर(pgid_s){
				pgid = check_uid(pgid_s, "PGID");
				अगर(pgid < 0){
					वापस MOSQ_ERR_INVAL;
				}अन्यथा अगर(pgid > 0){
					rc = setgid((gid_t)pgid);
					अगर(rc == -1){
						err = म_त्रुटि(त्रुटि_सं);
						log__printf(शून्य, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
						वापस MOSQ_ERR_ERRNO;
					}
				}
			}
			अगर(puid_s){
				puid = check_uid(puid_s, "PUID");
				अगर(puid < 0){
					वापस MOSQ_ERR_INVAL;
				}अन्यथा अगर(puid > 0){
					rc = setuid((uid_t)puid);
					अगर(rc == -1){
						err = म_त्रुटि(त्रुटि_सं);
						log__printf(शून्य, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
						वापस MOSQ_ERR_ERRNO;
					}
				}
			}
		}अन्यथा अगर(config->user && म_भेद(config->user, "root")){
			pwd = getpwnam(config->user);
			अगर(!pwd){
				अगर(म_भेद(config->user, "mosquitto")){
					log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to drop privileges to '%s' because this user does not exist.", config->user);
					वापस MOSQ_ERR_INVAL;
				}अन्यथा{
					log__printf(शून्य, MOSQ_LOG_ERR, "Warning: Unable to drop privileges to '%s' because this user does not exist. Trying 'nobody' instead.", config->user);
					pwd = getpwnam("nobody");
					अगर(!pwd){
						log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to drop privileges to 'nobody'.");
						वापस MOSQ_ERR_ERRNO;
					}
				}
			}
			अगर(initgroups(config->user, pwd->pw_gid) == -1){
				err = म_त्रुटि(त्रुटि_सं);
				log__printf(शून्य, MOSQ_LOG_ERR, "Error setting groups whilst dropping privileges: %s.", err);
				वापस MOSQ_ERR_ERRNO;
			}
			rc = setgid(pwd->pw_gid);
			अगर(rc == -1){
				err = म_त्रुटि(त्रुटि_सं);
				log__printf(शून्य, MOSQ_LOG_ERR, "Error setting gid whilst dropping privileges: %s.", err);
				वापस MOSQ_ERR_ERRNO;
			}
			rc = setuid(pwd->pw_uid);
			अगर(rc == -1){
				err = म_त्रुटि(त्रुटि_सं);
				log__printf(शून्य, MOSQ_LOG_ERR, "Error setting uid whilst dropping privileges: %s.", err);
				वापस MOSQ_ERR_ERRNO;
			}
		}
		print_pwname();
		अगर(geteuid() == 0 || getegid() == 0){
			log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Mosquitto should not be run as root/administrator.");
		}
	}अन्यथा{
		print_pwname();
	}

#अन्यथा
	UNUSED(config);
#पूर्ण_अगर
	वापस MOSQ_ERR_SUCCESS;
}


अटल व्योम mosquitto__daemonise(व्योम)
{
#अगर_अघोषित WIN32
	अक्षर *err;
	pid_t pid;

	pid = विभाजन();
	अगर(pid < 0){
		err = म_त्रुटि(त्रुटि_सं);
		log__printf(शून्य, MOSQ_LOG_ERR, "Error in fork: %s", err);
		निकास(1);
	}
	अगर(pid > 0){
		निकास(0);
	}
	अगर(setsid() < 0){
		err = म_त्रुटि(त्रुटि_सं);
		log__printf(शून्य, MOSQ_LOG_ERR, "Error in setsid: %s", err);
		निकास(1);
	}

	अगर(!ख_व_खोलो("/dev/null", "r", मानक_निवेश)){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdin", म_त्रुटि(त्रुटि_सं));
		निकास(1);
	}
	अगर(!ख_व_खोलो("/dev/null", "w", मानक_निकास)){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stdout", म_त्रुटि(त्रुटि_सं));
		निकास(1);
	}
	अगर(!ख_व_खोलो("/dev/null", "w", मानक_त्रुटि)){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error whilst daemonising (%s): %s", "stderr", म_त्रुटि(त्रुटि_सं));
		निकास(1);
	}
#अन्यथा
	log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Can't start in daemon mode in Windows.");
#पूर्ण_अगर
}


अटल पूर्णांक pid__write(व्योम)
{
	खाता *pid;

	अगर(db.config->pid_file){
		pid = mosquitto_fopen(db.config->pid_file, "wt", false);
		अगर(pid){
			ख_लिखो(pid, "%d", getpid());
			ख_बंद(pid);
		}अन्यथा{
			log__printf(शून्य, MOSQ_LOG_ERR, "Error: Unable to write pid file.");
			वापस MOSQ_ERR_ERRNO;
		}
	}
	वापस MOSQ_ERR_SUCCESS;
}


अटल व्योम report_features(व्योम)
{
#अगर_घोषित WITH_BRIDGE
	log__printf(शून्य, MOSQ_LOG_INFO, "Bridge support available.");
#अन्यथा
	log__printf(शून्य, MOSQ_LOG_INFO, "Bridge support NOT available.");
#पूर्ण_अगर
#अगर_घोषित WITH_PERSISTENCE
	log__printf(शून्य, MOSQ_LOG_INFO, "Persistence support available.");
#अन्यथा
	log__printf(शून्य, MOSQ_LOG_INFO, "Persistence support NOT available.");
#पूर्ण_अगर
#अगर_घोषित WITH_TLS
	log__printf(शून्य, MOSQ_LOG_INFO, "TLS support available.");
#अन्यथा
	log__printf(शून्य, MOSQ_LOG_INFO, "TLS support NOT available.");
#पूर्ण_अगर
#अगर_घोषित FINAL_WITH_TLS_PSK
	log__printf(शून्य, MOSQ_LOG_INFO, "TLS-PSK support available.");
#अन्यथा
	log__printf(शून्य, MOSQ_LOG_INFO, "TLS-PSK support NOT available.");
#पूर्ण_अगर
#अगर_घोषित WITH_WEBSOCKETS
	log__printf(शून्य, MOSQ_LOG_INFO, "Websockets support available.");
#अन्यथा
	log__printf(शून्य, MOSQ_LOG_INFO, "Websockets support NOT available.");
#पूर्ण_अगर
	अगर(दो_पर्या("MOSQUITTO_UNSAFE_ALLOW_SYMLINKS")){
		log__printf(शून्य, MOSQ_LOG_NOTICE, "MOSQUITTO_UNSAFE_ALLOW_SYMLINKS is set, loading of sensitive files through symbolic links is allowed.");
	}
}


अटल व्योम post_shutdown_cleanup(व्योम)
{
	काष्ठा mosquitto *ctxt, *ctxt_tmp;

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
	log__printf(शून्य, MOSQ_LOG_INFO, "mosquitto version %s terminating", VERSION);

	broker_control__cleanup();

#अगर_घोषित WITH_PERSISTENCE
	persist__backup(true);
#पूर्ण_अगर
	session_expiry__remove_all();

	listeners__stop();

	HASH_ITER(hh_id, db.contexts_by_id, ctxt, ctxt_tmp){
#अगर defined(WITH_WEBSOCKETS) && WITH_WEBSOCKETS == WS_IS_LWS
		अगर(!ctxt->wsi)
#पूर्ण_अगर
		{
			ctxt->is_persisted = false; /* prevent persistence removal */
			context__cleanup(ctxt, true);
		}
	}
	HASH_ITER(hh_sock, db.contexts_by_sock, ctxt, ctxt_tmp){
		ctxt->is_persisted = false; /* prevent persistence removal */
		context__cleanup(ctxt, true);
	}
#अगर_घोषित WITH_BRIDGE
	bridge__db_cleanup();
#पूर्ण_अगर
	context__free_disused();
	keepalive__cleanup();

#अगर_घोषित WITH_TLS
	mosquitto_FREE(db.tls_keylog);
#पूर्ण_अगर
	db__close();

	plugin__unload_all();
	mosquitto_security_cleanup(false);

	अगर(db.config->pid_file){
		(व्योम)हटाओ(db.config->pid_file);
	}

	mux__cleanup();

	log__close(db.config);
	config__cleanup(db.config);
	net__broker_cleanup();
}


अटल व्योम cjson_init(व्योम)
{
	cJSON_Hooks hooks = {mosquitto_malloc, mosquitto_free};
	cJSON_InitHooks(&hooks);
}

#अगर_घोषित WITH_FUZZING


पूर्णांक mosquitto_fuzz_main(पूर्णांक argc, अक्षर *argv[])
#अन्यथा


पूर्णांक मुख्य(पूर्णांक argc, अक्षर *argv[])
#पूर्ण_अगर
{
	काष्ठा mosquitto__config config;
	पूर्णांक rc;

	mosquitto_time_init();
	cjson_init();

#अगर defined(WIN32) || defined(__CYGWIN__)
	अगर(argc == 2){
		अगर(!म_भेद(argv[1], "run")){
			service_run(argv[0]);
			वापस 0;
		}अन्यथा अगर(!म_भेद(argv[1], "install")){
			service_install(argv[0]);
			वापस 0;
		}अन्यथा अगर(!म_भेद(argv[1], "uninstall")){
			service_uninstall(argv[0]);
			वापस 0;
		}
	}
#पूर्ण_अगर


#अगर_घोषित WIN32
	अगर(_setmaxstdio(8192) != 8192){
		/* Old limit was 2048 */
		अगर(_setmaxstdio(2048) != 2048){
			log__printf(शून्य, MOSQ_LOG_WARNING, "Warning: Unable to increase maximum allowed connections. This session may be limited to 512 connections.");
		}
	}
#पूर्ण_अगर

	स_रखो(&db, 0, माप(काष्ठा mosquitto_db));
	db.now_s = mosquitto_time();
	db.now_real_s = समय(शून्य);
	mosquitto_broker_गाँठ_id_set(0);

	net__broker_init();

	db.config = &config;
	config__init(&config);
	rc = config__parse_args(&config, argc, argv);
	अगर(rc == MOSQ_ERR_UNKNOWN){
		post_shutdown_cleanup();
		वापस MOSQ_ERR_SUCCESS;
	}अन्यथा अगर(rc != MOSQ_ERR_SUCCESS){
		post_shutdown_cleanup();
		वापस rc;
	}

	अगर(config.test_configuration){
		अगर(!db.config_file){
			log__printf(शून्य, MOSQ_LOG_ERR, "Please provide a configuration file to test.");
			post_shutdown_cleanup();
			वापस MOSQ_ERR_INVAL;
		}अन्यथा{
			log__printf(शून्य, MOSQ_LOG_INFO, "Configuration file is OK.");
			post_shutdown_cleanup();
			वापस MOSQ_ERR_SUCCESS;
		}
	}

	rc = keepalive__init();
	अगर(rc){
		post_shutdown_cleanup();
		वापस rc;
	}

	/* Drop privileges permanently immediately after the config is loaded.
	 * This requires the user to ensure that all certificates, log locations,
	 * etc. are accessible my the `mosquitto` or other unprivileged user.
	 */
	rc = drop_privileges(&config);
	अगर(rc){
		post_shutdown_cleanup();
		वापस rc;
	}
	/* Set umask based on environment variable */
	rc = set_umask();
	अगर(rc){
		post_shutdown_cleanup();
		वापस rc;
	}


	अगर(config.daemon){
		mosquitto__daemonise();
	}

	rc = pid__write();
	अगर(rc){
		post_shutdown_cleanup();
		वापस rc;
	}

	rc = db__open(&config);
	अगर(rc != MOSQ_ERR_SUCCESS){
		log__printf(शून्य, MOSQ_LOG_ERR, "Error: Couldn't open database.");
		post_shutdown_cleanup();
		वापस rc;
	}

	/* Initialise logging only after initialising the database in case we're
	 * logging to topics */
	rc = log__init(&config);
	अगर(rc){
		post_shutdown_cleanup();
		वापस rc;
	}
	log__printf(शून्य, MOSQ_LOG_INFO, "mosquitto version %s starting", VERSION);
	अगर(db.config_file){
		log__printf(शून्य, MOSQ_LOG_INFO, "Config loaded from %s.", db.config_file);
	}अन्यथा{
		log__printf(शून्य, MOSQ_LOG_INFO, "Using default config.");
	}
	report_features();

	rc = plugin__load_all();
	अगर(rc){
		post_shutdown_cleanup();
		वापस rc;
	}
	rc = mosquitto_security_init(false);
	अगर(rc){
		post_shutdown_cleanup();
		वापस rc;
	}

	plugin_persist__handle_restore();
	session_expiry__check();
	retain__expire(&db.retains);
	db_संदेश_store_compact();

#अगर_घोषित WITH_SYS_वृक्ष
	sys_वृक्ष_init();
#पूर्ण_अगर

	rc = mux__init();
	अगर(rc){
		post_shutdown_cleanup();
		वापस rc;
	}

	rc = listeners__start();
	अगर(rc){
		post_shutdown_cleanup();
		वापस rc;
	}


#अगर_घोषित WITH_BRIDGE
	bridge__start_all();
#पूर्ण_अगर

	broker_control__init();

	g_run = 1;
	signal__setup();

	log__printf(शून्य, MOSQ_LOG_INFO, "mosquitto version %s running", VERSION);
#अगर_घोषित WITH_SYSTEMD
	sd_notify(0, "READY=1");
#पूर्ण_अगर

	rc = mosquitto_main_loop(g_listensock, g_listensock_गिनती);

	post_shutdown_cleanup();

	वापस rc;
}

#अगर_घोषित WIN32


पूर्णांक WINAPI WinMain(HINSTANCE hInstance, HINSTANCE h_पूर्व_Instance, LPSTR lp_Cmd_पंक्ति, पूर्णांक nCmdShow)
{
	अक्षर **argv;
	पूर्णांक argc = 1;
	अक्षर *token;
	अक्षर *saveptr = शून्य;
	पूर्णांक rc;

	UNUSED(hInstance);
	UNUSED(h_पूर्व_Instance);
	UNUSED(nCmdShow);

	argv = mosquitto_malloc(माप(अक्षर *)*1);
	argv[0] = "mosquitto";
	token = strtok_r(lp_Cmd_पंक्ति, " ", &saveptr);
	जबतक(token){
		argc++;
		argv = mosquitto_realloc(argv, माप(अक्षर *)*argc);
		अगर(!argv){
			ख_लिखो(मानक_त्रुटि, "Error: Out of memory.\n");
			वापस MOSQ_ERR_NOMEM;
		}
		argv[argc-1] = token;
		token = strtok_r(शून्य, " ", &saveptr);
	}
	rc = मुख्य(argc, argv);
	mosquitto_FREE(argv);
	वापस rc;
}
#पूर्ण_अगर
