#include "SDLGraphicsProgram.hpp"
#include "glad/glad.h"

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
// Attribution: Mike Shah
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h){
	// The window we'll be rendering to
	m_window = NULL;

    m_width = w;
    m_height = h;

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
	}
    //Use OpenGL 3.3 core
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    // We want to request a double buffer for smooth updating.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //Create window
    m_window = SDL_CreateWindow( "Engine",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            m_width,
                            m_height,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

    // Check if Window did not create.
    if( m_window == NULL ){
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
    }

    //Create an OpenGL Graphics Context
    m_openGLContext = SDL_GL_CreateContext( m_window );
    if( m_openGLContext == NULL){
        std::cerr << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
    }

    // Initialize GLAD Library
    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
        std::cerr << "Failed to iniitalize GLAD\n";
        exit(EXIT_FAILURE);
    }

    // If initialization succeeds then print out a list of errors in the constructor.
    SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL errors detected during initialization\n\n");

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	GetOpenGLVersionInfo();
}

// Proper shutdown of SDL and destroy initialized objects
// Attribution: Mike Shah
SDLGraphicsProgram::~SDLGraphicsProgram(){
    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point m_window to NULL to ensure it points to nothing.
	m_window = nullptr;
	//Quit SDL subsystems
	SDL_Quit();
}

//Loops forever!
void SDLGraphicsProgram::SetLoopCallback(std::function<void(void)> callback){
    bool quit = false;
    while (!quit) {
        //Handle events on queue
        SDL_Event e;

        while(SDL_PollEvent( &e ) != 0){
            // User posts an event to quit
            // An example is hitting the "x" in the corner of the window.
            if(e.type == SDL_QUIT){
                quit = true;
            }
        }
    }
}

// Helper Function to get OpenGL Version Information
// Attribution: Mike Shah
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
