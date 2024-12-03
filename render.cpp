// Bela-Arduino template project
// Physical Computing (Gizmo) 2024
// Bela.io / Imperial College London

// render.cpp: this file contains code in the native Bela C++ API,
// which deals with streaming signals (e.g. audio, analog, digital)
// in the render() callback function. This file also contains infrastructure
// to enable the Arduino code in ino.cpp to run.

#include <libraries/BelaLibpd/BelaLibpd.h>
#include <libraries/BelaArduino/BelaArduino.h>
#include <libraries/Watcher/Watcher.h>

// These lines set up a GUI (Watcher) to monitor the values of variables
WatcherManager& wm = *Bela_getDefaultWatcherManager();

// Example of how to declare a variable with the Watcher:
// Watcher<float> myvar = {"myvar", 0.5};

// This function runs once at the beginning
bool setup(BelaContext* context, void* userData)
{
	// Initialise the Pd audio environment, which runs the patch 
	// _main.pd within the project folder
	BelaLibpdSettings settings;
	settings.useIoThreaded = false;
	settings.useGui = false; // we want to use the BelaArduino GUI, which is controlled here and in ino.cpp
	settings.messageHook = BelaArduino_messageHook;
	settings.listHook = BelaArduino_listHook;
	settings.floatHook = BelaArduino_floatHook;
	if(!BelaLibpd_setup(context, userData, settings))
		return false;
		
	// Initialise the Arduino environment which runs in ino.cpp
	BelaArduinoSettings arduinoSettings {};
	if(!BelaArduino_setup(context, userData, arduinoSettings))
		return false;
		
	// Initialise the Watcher which lets us observe the values of
	// variables defined with Watcher<> -- see "myvar" example above
	unsigned int wsPort = 5557;
	wm.getGui().setup("/libraries/Watcher/sketch-watcher-example.js", wsPort);
	wm.setup(context->audioSampleRate);
	printf("sketch-watcher.js GUI at bela.local/gui?wsPort=%u\n", wsPort);
	
	// Return true to indicate setup succeeded (false will stop the program)
	return true;
}

// This function is called once per audio block
void render(BelaContext* context, void* userData)
{
	// Manage the Watcher: this line needed to tell it an audio block has occurred
	if(wm.getGui().numConnections())
		wm.tick(context->audioFramesElapsed);
		
	// Run the Arduino and Pd code
	BelaArduino_renderTop(context, userData);
	BelaLibpd_render(context, userData);
	
	// ---
	// Your custom render() code should go here
	// ---
	
	BelaArduino_renderBottom(context, userData);
}

// This function runs once at the end
void cleanup(BelaContext* context, void* userData)
{
	BelaArduino_cleanup(context, userData);
	BelaLibpd_cleanup(context, userData);
}
