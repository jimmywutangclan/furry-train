// ==================================================================
#version 330 core

layout(location=0)in vec3 position; // We explicitly state which is the vertex information 
                                    // (The first 3 floats are positional data, we are putting in our vector)
layout(location=1)in vec3 vertexColor; // Our second attribute which is the color consisting of R,G,B

// Do not forget this!
out vec3 theColor;

// If we are applying our camera, then we need to add some uniforms.
// Note that the syntax nicely matches glm's mat4!
uniform mat4 modelTransformMatrix;
uniform mat4 projectionMatrix;


void main()
{
  // gl_Position is a special glsl variable that tells us what
  // postiion to put things in.
  // It takes in exactly 4 things.
  // Note that 'w' (the 4th dimension) should be 1.
  vec4 oldPosition = vec4(position.x, position.y, position.z, 1.0f);
// TODO:
  vec4 newPosition = modelTransformMatrix * oldPosition;
  vec4 projectedPosition = projectionMatrix * newPosition;

  gl_Position = projectedPosition;

  // Store the vertex color that we take in as what we will output
  // to the next stage in the graphics pipeline.
  theColor = vertexColor;

}
// ==================================================================