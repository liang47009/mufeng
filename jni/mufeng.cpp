/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include <jni.h>
#include <errno.h>

#include "engine/Engine.h"

struct android_app;

Engine g_engine;

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(android_app* state) {
	app_dummy();

	g_engine.SetState(state);

	state->userData = &g_engine;
	state->onAppCmd = Engine::HandleCmd;
	state->onInputEvent = Engine::HandleInput;

	// Prepare to monitor accelerometer
	g_engine.InitSensors();

	// loop waiting for stuff to do.
	while (1) {
		// Read all pending events.
		int id;
		int events;
		android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((id = ALooper_pollAll(g_engine.IsReady() ? 0 : -1, NULL, &events,
				(void**) &source)) >= 0) {
			// Process this event.
			if (source != NULL)
				source->process(state, source);

			g_engine.ProcessSensors(id);

			// Check if we are exiting.
			if (state->destroyRequested != 0) {
				g_engine.TermDisplay();
				return;
			}
		}

		if (g_engine.IsReady()) {
			// Drawing is throttled to the screen update rate, so there
			// is no need to do timing here.
			g_engine.DrawFrame();
		}
	}
}
