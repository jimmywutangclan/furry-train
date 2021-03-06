#include "SDLGraphicsProgram.hpp"
#include "Camera.hpp"
#include "Terrain.hpp"
#include "Mirror.hpp"
#include "Object.hpp"
// Include the 'Renderer.hpp' which deteremines what
// the graphics API is going to be for OpenGL
#include "Renderer.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
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
    m_window = SDL_CreateWindow( "Lab",
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
    
    // Create a renderer
    std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>(m_width,m_height);    

    // Create our terrain
    std::shared_ptr<Terrain> myTerrain = std::make_shared<Terrain>(512,512,"terrain3.ppm");
    myTerrain->LoadTextures("grass.ppm","grass.ppm");

    // Create a node for our terrain 
    std::shared_ptr<SceneNode> terrainNode;
    terrainNode = std::make_shared<SceneNode>(myTerrain,"./shaders/vert.glsl","./shaders/frag.glsl");
    terrainNode->GetLocalTransform().Rotate(glm::radians(90.0f),0,1,0);

    // Create our "mirror"
    std::shared_ptr<Mirror> myMirror = std::make_shared<Mirror>("./shaders/defaultFrag.glsl", 0);
    myMirror->MakeTexturedQuad("cat3.ppm");
    myMirror->CreateBuffer(m_width, m_height);
    std::shared_ptr<SceneNode> mirrorNode;
    mirrorNode = std::make_shared<SceneNode>(myMirror,"./shaders/vert.glsl","./shaders/frag.glsl");
    mirrorNode->is_mirror = true;

    // the cat in the middle
    std::shared_ptr<Object> simpleQuad = std::make_shared<Object>();
    simpleQuad->MakeTexturedQuad("cat3.ppm");
    std::shared_ptr<SceneNode> simpleQuadNode;
    simpleQuadNode = std::make_shared<SceneNode>(simpleQuad,"./shaders/vert.glsl","./shaders/frag.glsl");

    std::shared_ptr<Object> wall1 = std::make_shared<Object>();
    wall1->MakeTexturedQuad("cat3.ppm");
    std::shared_ptr<SceneNode> wall1Node;
    wall1Node = std::make_shared<SceneNode>(wall1,"./shaders/vert.glsl","./shaders/frag.glsl");

    std::shared_ptr<Object> wall2 = std::make_shared<Object>();
    wall2->MakeTexturedQuad("cat3.ppm");
    std::shared_ptr<SceneNode> wall2Node;
    wall2Node = std::make_shared<SceneNode>(wall2,"./shaders/vert.glsl","./shaders/frag.glsl");

    std::shared_ptr<Object> wall3 = std::make_shared<Object>();
    wall3->MakeTexturedQuad("cat3.ppm");
    std::shared_ptr<SceneNode> wall3Node;
    wall3Node = std::make_shared<SceneNode>(wall3,"./shaders/vert.glsl","./shaders/frag.glsl");

    std::shared_ptr<Mirror> myQuad = std::make_shared<Mirror>("./shaders/defaultFrag.glsl", 0);
    myQuad->MakeTexturedQuad("cat3.ppm");
    myQuad->CreateBuffer(m_width, m_height);
    std::shared_ptr<SceneNode> quadNode;
    quadNode = std::make_shared<SceneNode>(myQuad,"./shaders/vert.glsl","./shaders/frag.glsl");
    quadNode->is_mirror = true;

    // Set our SceneTree up
    terrainNode->AddChild(quadNode.get());
    terrainNode->AddChild(simpleQuadNode.get());
    terrainNode->AddChild(mirrorNode.get());
    terrainNode->AddChild(wall1Node.get());
    terrainNode->AddChild(wall2Node.get());
    terrainNode->AddChild(wall3Node.get());
    renderer->setRoot(terrainNode);

    wall1Node->GetLocalTransform().Translate(95,16.5f,244);
    wall1Node->GetLocalTransform().Scale(15,15,0);

    mirrorNode->GetLocalTransform().Translate(95,16.5f,245);
    mirrorNode->GetLocalTransform().Scale(15,15,0);
    mirrorNode->GetLocalTransform().Rotate(glm::radians(180.0f),0,1,0);
    
    simpleQuadNode->GetLocalTransform().Translate(95,16.5f,315);
    simpleQuadNode->GetLocalTransform().Scale(5,5,0);

    quadNode->GetLocalTransform().Translate(95,16.5f,405);
    quadNode->GetLocalTransform().Scale(15,15,0);

    wall2Node->GetLocalTransform().Translate(95,16.5f,406);
    wall2Node->GetLocalTransform().Scale(15,15,0);

    wall3Node->GetLocalTransform().Translate(135,16.5f,406);
    wall3Node->GetLocalTransform().Scale(15,15,0);

    // Set up the cameras for the mirrors, eventually we want to have the constructor be able to just set it up without any hardcoding.
    Camera * mirrorCamera = new Camera();
    mirrorCamera->SetCameraEyePosition(95,16.5f,245);
    mirrorCamera->SetCameraEyeDirection(0,0,1);
    myMirror->camera_id = renderer->AddCamera(mirrorCamera);
    myMirror->forwards.x = 0;
    myMirror->forwards.y = 0;
    myMirror->forwards.z = 1;

    Camera * quadNodeCamera = new Camera();
    quadNodeCamera->SetCameraEyePosition(95,16.5f,405);
    quadNodeCamera->SetCameraEyeDirection(0,0,-1);
    myQuad->camera_id = renderer->AddCamera(quadNodeCamera);
    myQuad->forwards.x = 0;
    myQuad->forwards.y = 0;
    myQuad->forwards.z = -1;

    // Set a default position for our camera
    renderer->GetCamera(0)->SetCameraEyePosition(125.0f,50.0f,500.0f);
    renderer->GetCamera(1)->SetCameraEyePosition(renderer->GetCamera(0)->GetEyeXPosition(),renderer->GetCamera(0)->GetEyeYPosition(),renderer->GetCamera(0)->GetEyeZPosition());
    renderer->GetCamera(1)->SetCameraEyeDirection(-renderer->GetCamera(0)->GetViewXDirection(),-renderer->GetCamera(0)->GetViewYDirection(),-renderer->GetCamera(0)->GetViewZDirection());
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();

    // Set the camera speed for how fast we move.
    float cameraSpeed = 5.0f;

    // Center our mouse
    SDL_WarpMouseInWindow(m_window,m_width/2,m_height/2);


    // data properties to let the cat image in the middle oscillate for a bit(simpleQuadNode)
    int ticksLapsed = 100;
    int ticksAllowed = 200;
    bool adding = false;

    // While application is running
    while(!quit){

        if (adding) {
            simpleQuadNode->GetLocalTransform().Translate(0.2f,0,0);
            ticksLapsed++;
        }
        else {
            simpleQuadNode->GetLocalTransform().Translate(-0.2f,0,0);
            ticksLapsed--;
        }

        if (ticksLapsed == ticksAllowed) {
            adding = false;
        }
        if (ticksLapsed == 0) {
            adding = true;
        }

        // For our terrain setup the identity transform each frame
        // By default set the terrain node to the identity
        // matrix.
        terrainNode->GetLocalTransform().LoadIdentity();
        // Invoke(i.e. call) the callback function
        callback();

        //Handle events on queue
        while(SDL_PollEvent( &e ) != 0){
            // User posts an event to quit
            // An example is hitting the "x" in the corner of the window.
            if(e.type == SDL_QUIT){
                quit = true;
            }
            // Handle keyboad input for the camera class
            if(e.type==SDL_MOUSEMOTION){
                // Handle mouse movements
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;
                renderer->GetCamera(0)->MouseLook(mouseX, mouseY);
                renderer->GetCamera(1)->SetCameraEyeDirection(-renderer->GetCamera(0)->GetViewXDirection(),-renderer->GetCamera(0)->GetViewYDirection(),-renderer->GetCamera(0)->GetViewZDirection());
                //renderer->GetCamera(1)->MouseLook(mouseX, mouseY);
            }
            switch(e.type){
                // Handle keyboard presses
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym){
                        case SDLK_LEFT:
                            renderer->GetCamera(0)->MoveLeft(cameraSpeed);
                            break;
                        case SDLK_RIGHT:
                            renderer->GetCamera(0)->MoveRight(cameraSpeed);
                            break;
                        case SDLK_UP:
                            renderer->GetCamera(0)->MoveForward(cameraSpeed);
                            break;
                        case SDLK_DOWN:
                            renderer->GetCamera(0)->MoveBackward(cameraSpeed);
                            break;
                        case SDLK_RSHIFT:
                            renderer->GetCamera(0)->MoveUp(cameraSpeed);
                            break;
                        case SDLK_RCTRL:
                            renderer->GetCamera(0)->MoveDown(cameraSpeed);
                            break;
                    }
                break;
            }
            renderer->GetCamera(1)->SetCameraEyePosition(renderer->GetCamera(0)->GetEyeXPosition(),renderer->GetCamera(0)->GetEyeYPosition(),renderer->GetCamera(0)->GetEyeZPosition());
        } // End SDL_PollEvent loop.
		
        // Update our scene through our renderer
        renderer->Update();
        // Render our scene using our selected renderer
        renderer->Render();
        // Delay to slow things down just a bit!
        SDL_Delay(25);  // TODO: You can change this or implement a frame
                        // independent movement method if you like.
      	//Update screen of our specified window
      	SDL_GL_SwapWindow(GetSDLWindow());
	}
    //Disable text input
    SDL_StopTextInput();
}


// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
