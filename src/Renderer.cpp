#include "Renderer.hpp"
#include "Mirror.hpp"
#include <iostream>

// source: https://stackoverflow.com/questions/31064234/find-the-angle-between-two-vectors-from-an-arbitrary-origin
float angleBetween(glm::vec3 a, glm::vec3 b,glm::vec3 origin) {
    glm::vec3 da=glm::normalize(a-origin);
    glm::vec3 db=glm::normalize(b-origin);
    return glm::acos(glm::dot(da, db));
}

// Sets the height and width of our renderer
Renderer::Renderer(unsigned int w, unsigned int h){
    m_screenWidth = w;
    m_screenHeight = h;

    // By default create one camera per render
    // TODO: You could abstract out further functions to create
    //       a camera as a scene node and attach them at various levels.
    Camera* defaultCamera = new Camera();
    Camera* rearViewCamera = new Camera();
    // Add our single camera
    m_cameras.push_back(defaultCamera);
    m_cameras.push_back(rearViewCamera);
    // Initialize the root in our scene
    m_root = nullptr;

    // By derfaflt create one framebuffer within the renderere.
    // create another framebuffer that sharpens the image
    Framebuffer* newFramebuffer = new Framebuffer("./shaders/defaultFrag.glsl",0.0f,0.0f,1.0f,1.0f,0);
    Framebuffer* newFramebuffer2 = new Framebuffer("./shaders/sharperFrag.glsl",-0.7f,0.7f,0.2f,0.2f,1);
    newFramebuffer->Create(w,h);
    newFramebuffer2->Create(w,h);
    m_framebuffers.push_back(newFramebuffer);
    m_framebuffers.push_back(newFramebuffer2);
    active = m_framebuffers[1];
}

// Sets the height and width of our renderer
Renderer::~Renderer(){
    // Delete all of our camera pointers
    for(int i=0; i < m_cameras.size(); i++){
        delete m_cameras[i];
    }
    // Delete all of of our framebuffers
    for(int i=0; i < m_framebuffers.size(); i++){
        delete m_framebuffers[i];
    }
}

void Renderer::Update(){
    // Here we apply the projection matrix which creates perspective.
    // The first argument is 'field of view'
    // Then perspective
    // Then the near and far clipping plane.
    // Note I cannot see anything closer than 0.1f units from the screen.
    //m_projectionMatrix = glm::perspective(45.0f,((float)m_screenWidth)/((float)m_screenHeight),0.1f,512.0f);
}

// Initialize clear color
// Setup our OpenGL State machine
// Then render the scene
void Renderer::Render(){
    // we will likely want to first go through all the items, and if any of them are mirrors, keep track of them because we will want to draw the world from each mirror's POV first
    std::vector<Mirror *> mirrors;
    m_root->FindMirrors(mirrors);

    // iterate through all the mirrors
    for (int i = 0; i < mirrors.size(); i++) {
        // here, calculate the new direction of the camera
        glm::vec3 diffFromPlayerToMirror(m_cameras[mirrors[i]->camera_id]->m_eyePosition.x - m_cameras[0]->m_eyePosition.x,0,m_cameras[mirrors[i]->camera_id]->m_eyePosition.z - m_cameras[0]->m_eyePosition.z);
        diffFromPlayerToMirror = glm::normalize(diffFromPlayerToMirror);
        m_cameras[mirrors[i]->camera_id]->m_viewDirection.x = diffFromPlayerToMirror.x;
        m_cameras[mirrors[i]->camera_id]->m_viewDirection.z = diffFromPlayerToMirror.z;

        // render directly in front of the mirror if the camera reflection ends up behind the mirror
        if (mirrors[i]->forwards.z > 0 && m_cameras[mirrors[i]->camera_id]->m_viewDirection.z < 0) {
            m_cameras[mirrors[i]->camera_id]->m_viewDirection.z = 1;
        }
        else if (mirrors[i]->forwards.z < 0 && m_cameras[mirrors[i]->camera_id]->m_viewDirection.z > 0) {
            m_cameras[mirrors[i]->camera_id]->m_viewDirection.z = -1;
        }


        // Logic for actually imprinting onto the framebuffer goes here
        m_projectionMatrix = glm::perspective(45.0f,((float)m_screenWidth)/((float)m_screenHeight),0.1f,512.0f);
        if(m_root!=nullptr){
            // TODO: By default, we will only have one camera
            //       You may otherwise not want to hardcode
            //       a value of '0' here.
            m_root->Update(m_projectionMatrix, m_cameras[mirrors[i]->camera_id]);
        }

        mirrors[i]->UpdateBuffer();
        mirrors[i]->BindBuffer();

        // What we are doing, is telling opengl to create a depth(or Z-buffer) 
        // for us that is stored every frame.
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D); 
        // This is the background of the screen.
        glViewport(0, 0, m_screenWidth, m_screenHeight);
        glClearColor( 0.55f, 0.45f, 1.0f, 1.f );
        // Clear color buffer and Depth Buffer
        // Remember that the 'depth buffer' is our
        // z-buffer that figures out how far away items are every frame
        // and we have to do this every frame!
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Now we render our objects from our scenegraph
        if(m_root!=nullptr){
            m_root->Draw();
        }

        // Finish with our framebuffer
        mirrors[i]->UnbindBuffer();
    }

    for (int i = 0; i < m_framebuffers.size(); i++) {
        m_projectionMatrix = glm::perspective(45.0f,((float)m_screenWidth)/((float)m_screenHeight),0.1f,512.0f);

        // Update the Uniforms of the game objects to be passed into shaders, based on the camera that this framebuffer belongs to.
        if(m_root!=nullptr){
            // TODO: By default, we will only have one camera
            //       You may otherwise not want to hardcode
            //       a value of '0' here.
            m_root->Update(m_projectionMatrix, m_cameras[m_framebuffers[i]->camera_id]);
        }

        m_framebuffers[i]->Update();
        // Bind to our farmebuffer
        m_framebuffers[i]->Bind();

        // What we are doing, is telling opengl to create a depth(or Z-buffer) 
        // for us that is stored every frame.
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D); 
        // This is the background of the screen.
        glViewport(0, 0, m_screenWidth, m_screenHeight);
        glClearColor( 0.55f, 0.45f, 1.0f, 1.f );
        // Clear color buffer and Depth Buffer
        // Remember that the 'depth buffer' is our
        // z-buffer that figures out how far away items are every frame
        // and we have to do this every frame!
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Nice way to debug your scene in wireframe!
        // TODO: Read this
        // Understand that you should only see a single quad
        // after rendering this, because we are only drawing.
        // one quad
        // This is how we know things are working with our FBO
        //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        // Test to see if the 'w' key is pressed for a quick view to toggle
        // the wireframe view.
        const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
        if( currentKeyStates[ SDL_SCANCODE_W ] )
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        }else{
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        }
        
        // Now we render our objects from our scenegraph
        if(m_root!=nullptr){
            m_root->Draw();
        }

        // Finish with our framebuffer
        m_framebuffers[i]->Unbind();
    }

    // Now draw a new scene
    // We do not need depth since we are drawing a '2D'
    // image over our screen.
    glDisable(GL_DEPTH_TEST);
    // Clear everything away
    // Clear the screen color, and typically I do this
    // to something 'different' than our original as an
    // indication that I am in a FBO. But you may choose
    // to match the glClearColor
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    // We only have 'color' in our buffer that is stored
    glClear(GL_COLOR_BUFFER_BIT); 

    // Then, go through all the quads, and then actually draw the framebuffer onto each quad
    for (int i = 0; i < m_framebuffers.size(); i++) {
        // Use our new 'simple screen shader'
        m_framebuffers[i]->m_fboShader->Bind();
        // Overlay our 'quad' over the screen
        m_framebuffers[i]->DrawFBO();
        // Unselect our shader and continue
        m_framebuffers[i]->m_fboShader->Unbind();
    }
}

// Determines what the root is of the renderer, so the
// scene can be drawn.
void Renderer::setRoot(std::shared_ptr<SceneNode> startingNode){
    m_root = startingNode;
}

// I ADDED THIS(changes the active frame buffer to use)
void Renderer::changeActive(int i) {
    active = m_framebuffers[i];
}

// Add a new camera to the camera list(returns the index of the camera)
int Renderer::AddCamera(Camera * camera) {
    m_cameras.push_back(camera);
    return m_cameras.size() - 1;
}