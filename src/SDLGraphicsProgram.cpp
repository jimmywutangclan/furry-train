#include "SDLGraphicsProgram.hpp"

// GLM Stuff
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

    //Initialize OpenGL
    if(!InitGL()){
        std::cerr << "Unable to initialize OpenGL!\n";
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

// Initialize OpenGL
// Setup any of our shaders here. (Attribution: Mike Shah)
bool SDLGraphicsProgram::InitGL(){
	//Success flag
	bool success = true;

	// Setup geometry
	//GenerateBuffers();

  	// Read the contents of each shader from the glsl files as a string
	std::string vertexShader = LoadShader("./shaders/vert.glsl");
	std::string fragmentShader = LoadShader("./shaders/frag.glsl");

    // take the string representation of the glsl files and then attach the shaders
  	unsigned int shader = CreateShader(vertexShader,fragmentShader);

	// Use our shader
	glUseProgram(shader);


    // Setup our OpenGL State machine
    // TODO: Read this
    // The below command is new!
    // What we are doing, is telling opengl to create a depth(or Z-buffer) 
    // for us that is stored every frame.
    glEnable(GL_DEPTH_TEST);


	return success;
}

// Loads a shader and returns a string(Attribution: Mike Shah)
std::string SDLGraphicsProgram::LoadShader(const std::string& fname){
    std::string result;
    // 1.) Get every line of data
    std::string line;
    std::ifstream myFile(fname.c_str());

    if(myFile.is_open()){
        while(getline(myFile,line)){
                result += line + '\n';
                // SDL_Log(line); 	// Uncomment this if you want to see
                                    // the shader code get printed out.
        }
    }
    else{
        SDL_Log("SDLGraphicsProgram::LoadShader - file not found. Try an absolute file path to see if the file exists.\n");
    }
    // Close file
    myFile.close();
    return result;
}

// Create our Shaders by compiling and attaching them(Attribution: Mike Shah)
unsigned int SDLGraphicsProgram::CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource){

    // Create a new program
    unsigned int program = glCreateProgram();
    // Compile our shaders
    unsigned int myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    // Link our program
    // These have been compiled already.
    glAttachShader(program,myVertexShader);
    glAttachShader(program,myFragmentShader);
    // Link our programs that have been 'attached'
    glLinkProgram(program);
    glValidateProgram(program);

    // Once the shaders have been linked in, we can delete them.
    glDetachShader(program,myVertexShader);
    glDetachShader(program,myFragmentShader);

    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    if(!CheckLinkStatus(program)){
        SDL_Log("SDLGraphicsProgram::CreateShader - ERROR, shader did not link! Were there compile errors in the shader?\n");
    }

    shaderID = program;
    return program;
}

// Compiles the shader(Attribution: Mike Shah)
unsigned int SDLGraphicsProgram::CompileShader(unsigned int type, const std::string& source){
  // Compile our shaders
  // id is the type of shader (Vertex, fragment, etc.)
  unsigned int id;

  if(type == GL_VERTEX_SHADER){
    id = glCreateShader(GL_VERTEX_SHADER);
  }else if(type == GL_FRAGMENT_SHADER){
    id = glCreateShader(GL_FRAGMENT_SHADER);
  }
  const char* src = source.c_str();
  // The source of our shader
  glShaderSource(id, 1, &src, nullptr);
  // Now compile our shader
  glCompileShader(id);

  // Retrieve the result of our compilation
  int result;
  // This code is returning any compilation errors that may have occurred!
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if(result == GL_FALSE){
      int length;
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
      char* errorMessages = new char[length]; // Could also use alloca here.
      glGetShaderInfoLog(id, length, &length, errorMessages);
      if(type == GL_VERTEX_SHADER){
		SDL_Log("SDLGraphicsProgram::CompileShader ERROR: GL_VERTEX_SHADER compilation failed!\n");
		SDL_Log("%s\n",errorMessages);
      }else if(type == GL_FRAGMENT_SHADER){
        SDL_Log("SDLGraphicsProgram::CompileShader ERROR: GL_FRAGMENT_SHADER compilation failed!\n");
		SDL_Log("%s\n",errorMessages);
      }
      // Reclaim our memory
      delete[] errorMessages;
      // Delete our broken shader
      glDeleteShader(id);
      return 0;
  }

  return id;
}

// Check to see if linking was successful(Attribution: Mike Shah)
bool SDLGraphicsProgram::CheckLinkStatus(GLuint programID){
    // Retrieve the result of our compilation
    int result;
    // This code is returning any Linker errors that may have occurred!
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    if(result == GL_FALSE){
      int length;
      glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
      char* errorMessages = new char[length]; // Could also use alloca here.
      glGetProgramInfoLog(programID, length, &length, errorMessages);
      // Reclaim our memory
      SDL_Log("ERROR in linking process\n");
	  SDL_Log("%s\n",errorMessages);
      delete[] errorMessages;
      return false;
    }

    return true;
}


//Loops forever!
void SDLGraphicsProgram::Run(){
    bool quit = false;
    while (!quit) {
        SDL_Log("RUn!");
        //Handle events on queue
        SDL_Event e;

        while(SDL_PollEvent( &e ) != 0){
            // User posts an event to quit
            // An example is hitting the "x" in the corner of the window.
            if(e.type == SDL_QUIT){
                quit = true;
                SDL_Log("Exiting window");
            }
        }
    }
}

// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
  return m_window;
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
