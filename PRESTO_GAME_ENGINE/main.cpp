#include "core.h"

ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_BITMAP *background;
ALLEGRO_EVENT_QUEUE *ev_queue;
ALLEGRO_TIMER *maintimer;
ALLEGRO_AUDIO_STREAM *music;
ALLEGRO_EVENT events;
GameDebugger MainDebugger;
ResourceManager resManager;

ALLEGRO_BITMAP* test;
ALLEGRO_FILE* memfile;

bool __DEBUG__MODE__;

bool exitprogram = false;
bool redraw = false;

bool AudioFail = false;
bool ImageFail = false;

char* testfile; //FOR TESTING ONLY!!
NAMEENTRY* tst;
FILEENTRY* mfile;

/* FOR TESTING ONLY */
void ResourceLoading()
{
	tst = (NAMEENTRY*)HelpUtils::MCreateNameEntry("ILLYASPRITES.PNG");
	resManager.OpenResourceDB("TEST_ASSETS");
	//TODO: Open a package using the information instead of doing it manually.
	resManager.OpenPackage("TEST_ASSETS\\test.gpf");

	mfile = (FILEENTRY*)resManager.SearchFileByPkgName("test", tst);
	if(!mfile)
	{
		MainDebugger.Log("Error loading ILLYASPRITES.PNG", "Error");
		return;
	}
	testfile = resManager.ExtractToMemory(mfile);
	memfile = al_open_memfile(testfile, mfile->FILESIZE, "r");
	test = al_load_bitmap_f(memfile, ".PNG");
	if(!test)
		MainDebugger.Log("Error loading test asset.","Error");
	al_fclose(memfile);
	MemoryManager::MultiAddToGC(3,tst,mfile,testfile);
	MemoryManager::Flush();
	return;
}

void ev_listener()
{
	bool eventTrigger = false;
	
	while(!exitprogram)
	{

	//Wait for an event to occur
	eventTrigger = al_get_next_event(ev_queue, &events);
	/* REGION - EVENT HANDLER - */

	if(eventTrigger)
	{
		switch(events.type)
		{
		case ALLEGRO_EVENT_TIMER:
			redraw = true;
			break;
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			exitprogram = true;
			break;
		case ALLEGRO_EVENT_KEY_DOWN:
			switch(events.keyboard.keycode)
			{
				case ALLEGRO_KEY_ESCAPE:
						//Scape key: exit program
					exitprogram = 1;
				break;
					/* Add more keyboard events here */

					/* End Keyboard Event Handler Region*/
			}
			break;
		}
	}else{
			/* ANIMATION UPDATES */
	}

			/* End Event Handler Region */

	/*--------------- GAME LOGIC HERE ----------------------------------------------*/

			/* REGION - DRAW ON SCREEN - */
			if(redraw && al_event_queue_is_empty(ev_queue)) {
				//Clear Screen
				al_set_target_bitmap(al_get_backbuffer(display));
				al_clear_to_color(al_map_rgb(64,64,128));
				/* DRAWING ROUTINES HERE */
	/*-------------Draw draw and draw more stuff-----------------------------------*/
				al_set_target_bitmap(al_get_backbuffer(display));
				al_draw_bitmap(test, 0, 0, 0);


				/* End Draw On Screen Region*/
	/*-------------------------------------------------------------------------------*/
				//Swap buffers
				al_flip_display();
				//Clear redraw flag and wait for next frame
				redraw = false;
				//Count rendered frames and FPS
				MainDebugger.CountFrame();
			}
	}
	if(__DEBUG__MODE__)
		MainDebugger.Log("Last frame count.", info_type_message);
	return;
}

void game_end()
{
	//Dispose all objects (bitmaps, timers, sounds, etc)
	al_destroy_timer(maintimer);
	if(_ENABLE_SOUND_ && !AudioFail){
		al_detach_audio_stream(music);
		al_drain_audio_stream(music);
	}
	if(!AudioFail){al_destroy_audio_stream(music);}
	al_destroy_display(display);
	MemoryManager::Flush();
	/* End Game Finalization Region */
	//End game
    MainDebugger.Log("End of the program.", info_type_message);
	return;
}

int main(int args, char** argv)
{
	srand (time(NULL));

	//Debug mode can be switch ON/OFF through the core.h configuration header.
	//However, if the executable has the _DEBUG flag defined, it's always
	//set to ON, disgregarding what the header says.
#ifndef _DEBUG
	bool __DEBUG__MODE__ = _ENABLE_DEBUG_MODE_;
#else
	bool __DEBUG__MODE__ = true;
#endif

	if(!al_init()) {
        MainDebugger.Log("Failed to initialize Allegro! (Null pointer returned by al_init function).", info_type_error);
		al_show_native_message_box(NULL, "Error", "Allegro Init", "Failed to initialize allegro!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	
	/* GAME SYSTEM SETUP */
	//Install the keyboard
	al_install_keyboard();
	//Starts the image addon
	al_init_image_addon();

	if(_ENABLE_MOUSE_) {
		//Installs the mouse
		al_install_mouse();
	}

	if(_ENABLE_SOUND_) {
		//Installs the audio system
		al_install_audio();
		//Initialize audio codecs
		al_init_acodec_addon();
		//Reserve 1 sample for initialization
		al_reserve_samples(1);
	}

	if(_FULLSCREEN_)
        al_set_new_display_flags(ALLEGRO_FULLSCREEN);
	display = al_create_display(_SCREEN_W_, _SCREEN_H_);

	if(!display) {
        MainDebugger.Log("Failed to create a display! (Null pointer returned by al_create_display function).", info_type_error);
		al_show_native_message_box(NULL, "Error", "Display Init", "Failed to create a display! (Null pointer returned by al_create_display function).", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	//Sets the title of the window
	al_set_window_title(display, _DISPLAYNAME_);

	//Gets information relevant to the computer's graphic renderer
	MainDebugger.GetSystemSpecs(display);

	//Create an event queue
	ev_queue = al_create_event_queue();

	MainDebugger.Log("Presto Game Engine Version %i . %i", info_type_message, ENGINEVERSION__MAJOR_, ENGINEVERSION__MINOR_);

	if(__DEBUG__MODE__)
		MainDebugger.Log("DEBUG flag is enabled.",info_type_message);

		/* EVENT SYSTEM SETUP */
	//Creates the main game timer
	maintimer = al_create_timer(1.0f/_DESIRED_FPS_);
	//Start the main game timer
	al_start_timer(maintimer);
	//Register main events in the main event queue
	al_register_event_source(ev_queue, al_get_display_event_source(display));
	al_register_event_source(ev_queue, al_get_timer_event_source(maintimer));
	al_register_event_source(ev_queue, al_get_keyboard_event_source());

	/* "Random" blending function required for transparent images */
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
	//End

	ResourceLoading();
	ev_listener();
	game_end();
	return 0;
}