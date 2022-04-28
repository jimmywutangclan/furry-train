#include "Mirror.hpp"
#include "Camera.hpp"
#include "Error.hpp"


Mirror::Mirror(std::string frag_name, int camera) {
    // (1) ======= Setup shader
    m_fboShader = std::make_shared<Shader>();
    // Setup shaders for the Framebuffer Object
    std::string fboVertexShader = m_fboShader->LoadShader("./shaders/fboVert.glsl");
    std::string fboFragmentShader = m_fboShader->LoadShader(frag_name);
    // Actually create our shader
    m_fboShader->CreateShader(fboVertexShader,fboFragmentShader);       
    // Skipping the screen quad setup, we do not need it since we will draw on the default textured quad.
    camera_id = camera;

    drawn_yet = false;
}

Mirror::~Mirror() {

}

void Mirror::Bind() {
    // Make sure we are updating the correct 'buffers'
    m_vertexBufferLayout.Bind();
    // Diffuse map is 0 by default, but it is good to set it explicitly
    if (drawn_yet) {
        glBindTexture(GL_TEXTURE_2D, m_colorBuffer_id);
    }
    else {
        m_textureDiffuse.Bind(0);
    }
}

// For our Mirror, we just use a regular Textured Quad, we use the inherited function from Object.hpp
// We will not have to set up a Screen quad
// We also will not have to modify Render() since Render() uses a one size fits all Bind() call to bind both the vertices and the texture

void Mirror::CreateBuffer(int width, int height) {
    // Generate a framebuffer
    glGenFramebuffers(1, &m_fbo_id);
    // Select the buffer we have just generated
    BindBuffer();
    // Create a color attachement texture
    glGenTextures(1, &m_colorBuffer_id);
    glBindTexture(GL_TEXTURE_2D, m_colorBuffer_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // texture size
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,m_colorBuffer_id,0);
    // Create our render buffer object
    glGenRenderbuffers(1,&m_rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER,m_rbo_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo_id);
    // Deselect our buffers
    UnbindBuffer();
}

void Mirror::UpdateBuffer() {
    drawn_yet = true;
    // Select our framebuffer
    m_fboShader->Bind();
    // Set the uniforms in our current shader

    // For our object, we apply the texture in the following way
    // Note that we set the value to 0, because we have bound
    // our texture to slot 0.
    m_fboShader->SetUniform1i("u_DiffuseMap",0);  
}

void Mirror::BindBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
}

void Mirror::UnbindBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}