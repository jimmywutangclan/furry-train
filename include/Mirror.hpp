#ifndef MIRROR_HPP
#define MIRROR_HPP
#include <glad/glad.h>

#include <vector>
#include <string>

// Forward declarations
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Geometry.hpp"
#include "Renderer.hpp"
#include "Object.hpp"

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

//
class Mirror : public Object {
public:
    Mirror(std::string frag_name, int camera);
    ~Mirror();

    virtual void Bind();

    std::shared_ptr<Shader> m_fboShader;
    // Our framebuffer also needs a texture.
    unsigned int m_colorBuffer_id;
    // the camera ID within the m_cameras vector that this framebuffer belongs to
    int camera_id;

    void CreateBuffer(int width, int height);
    void UpdateBuffer();
    void BindBuffer();
    void UnbindBuffer();

    bool drawn_yet;
    glm::vec3 forwards;
// private member variables
private:
    // Framebuffer id
    unsigned int m_fbo_id; 
    // Finally create our render buffer object
    unsigned int m_rbo_id;
};

#endif