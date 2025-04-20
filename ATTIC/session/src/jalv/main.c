// Copyright 2007-2024 David Robillard <d@drobilla.net>
// SPDX-License-Identifier: ISC

#include "backend.h"
#include "frontend.h"
#include "jalv.h"
#include "jalv_config.h"
#include "types.h"

#include <zix/attributes.h>
#include <zix/sem.h>

#if USE_SUIL
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ZixSem* exit_sem = NULL; ///< Exit semaphore used by signal handler

static void
signal_handler(int ZIX_UNUSED(sig))
{
  zix_sem_post(exit_sem);
}

static void
setup_signals(Jalv* const jalv)
{
  exit_sem = &jalv->done;

#if !defined(_WIN32) && USE_SIGACTION
  struct sigaction action;
  sigemptyset(&action.sa_mask);
  action.sa_flags   = 0;
  action.sa_handler = signal_handler;
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);
#else
  // May not work in combination with fgets in the console interface
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
#endif
}

int xargc = 3;

char **xargv;

char *xxx[3];

void
jalvmain(int argc, char** argv)
{
printf("\njalvmain enter");fflush(stdout);
  Jalv jalv;
printf("\njalvmain 1");fflush(stdout);
  memset(&jalv, '\0', sizeof(Jalv));
printf("\njalvmain 2");fflush(stdout);
  jalv.backend = jalv_backend_allocate();
printf("\njalvmain 3");fflush(stdout);

  // Initialize application
  xxx[0] = "foobar";
  xxx[2] = "http://lv2plug.in/plugins/eg-params";
  xxx[1] = "-i";
  xargv = &xxx[0];
printf("\njalvmain 4");fflush(stdout);
  const int orc = jalv_open(&jalv, &xargc, &xargv);
printf("\njalvmain 5");fflush(stdout);
  if (orc) {
printf("\njalvmain 6");fflush(stdout);
    jalv_close(&jalv);
//    return orc == JALV_EARLY_EXIT_STATUS ? EXIT_SUCCESS : EXIT_FAILURE;
	return;
  }

printf("\njalvmain 7");fflush(stdout);
  // Set up signal handlers and activate audio processing
  setup_signals(&jalv);
printf("\njalvmain 8");fflush(stdout);
  jalv_activate(&jalv);
printf("\njalvmain 9");fflush(stdout);

  // Run UI (or prompt at console)
  jalv_frontend_open(&jalv);
printf("\njalvmain 10");fflush(stdout);

  // Wait for finish signal from UI or signal handler
  zix_sem_wait(&jalv.done);
printf("\njalvmain 10");fflush(stdout);

  // Deactivate audio processing and tear down application
  jalv_deactivate(&jalv);
  const int crc = jalv_close(&jalv);
  jalv_backend_free(jalv.backend);
printf("\njalvmain 10");fflush(stdout);
  return;
}
