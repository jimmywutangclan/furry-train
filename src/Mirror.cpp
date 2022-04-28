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
}

Mirror::~Mirror() {

}

// For our Mirror, we just use a regular Textured Quad, we use the inherited function from Object.hpp
// We will not have to set up a Screen quad
// We also will not have to modify Render() since Render() uses a one size fits all Bind() call to bind both the vertices and the texture

void Mirror::CaptureView() {

}

void Mirror::GenerateBuffer() {

}