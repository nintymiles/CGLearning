#include <cassert>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

#include "glsupport.h"
#include "asstcommon.h"
#include "material.h"

using namespace std;
using namespace tr1;

struct GlProgramDesc {
  struct UniformDesc {
    string name;
    GLenum type;
    GLint size;
    GLint location;
  };

  struct AttribDesc {
    string name;
    GLenum type;
    GLint size;
    GLint location;
  };

  GlProgram program;

  vector<UniformDesc> uniforms;
  vector<AttribDesc> attribs;

  GlProgramDesc(GLuint vsHandle, GLuint fsHandle) {
    linkShader(program, vsHandle, fsHandle);

    int numActiveUniforms, numActiveAttribs, uniformMaxLen, attribMaxLen;

    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);

    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLen);
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attribMaxLen);

      //通过API获取到的一些状态信息，根据信息设置缓存变量
    const int bufSize = max(uniformMaxLen, attribMaxLen) + 1;
    vector<GLchar> buffer(bufSize);

    uniforms.resize(numActiveUniforms);
    for (int i = 0; i < numActiveUniforms; ++i) {
      GLsizei charsWritten;
      glGetActiveUniform(program, i, bufSize, &charsWritten, & (uniforms[i].size), & (uniforms[i].type), &buffer[0]);
      assert(charsWritten + 1 <= bufSize);
      uniforms[i].name = string(buffer.begin(), buffer.begin() + charsWritten);
      uniforms[i].location = glGetUniformLocation(program, &buffer[0]);
    }

    attribs.resize(numActiveAttribs);
    for (int i = 0; i < numActiveAttribs; ++i) {
      GLsizei charsWritten;
      glGetActiveAttrib(program, i, bufSize, &charsWritten, &(attribs[i].size), &(attribs[i].type), &buffer[0]);
      assert(charsWritten + 1 <= bufSize);
      attribs[i].name = string(buffer.begin(), buffer.begin() + charsWritten);
      attribs[i].location = glGetAttribLocation(program, &buffer[0]);
    }

    if (!g_Gl2Compatible)
      glBindFragDataLocation(program, 0, "fragColor");

    checkGlErrors();
  }
};

class GlProgramLibrary {
  typedef map<pair<string, GLenum>, shared_ptr<GlShader> > GlShaderMap;
  typedef map<pair<string, string>, shared_ptr<GlProgramDesc> > GlProgramDescMap;

  GlShaderMap shaderMap;
  GlProgramDescMap programMap;

  GlProgramLibrary() {}

public:
  static GlProgramLibrary& getSingleton() {
    static GlProgramLibrary pl;
    return pl;
  }

  shared_ptr<GlProgramDesc> getProgramDesc(const string& vsFilename, const string& fsFilename) {
    GlProgramDescMap::key_type key(vsFilename, fsFilename);

    GlProgramDescMap::iterator i = programMap.find(key);
    if (i == programMap.end()) {
      shared_ptr<GlProgramDesc> program(new GlProgramDesc(*getShader(vsFilename, GL_VERTEX_SHADER), *getShader(fsFilename, GL_FRAGMENT_SHADER)));
      programMap[key] = program;
      return program;
    }
    else {
      return i->second;
    }
  }

protected:
  shared_ptr<GlShader> getShader(const string& filename, GLenum shaderType) {
    string f = filename;
    if (g_Gl2Compatible) { // optionally change -gl3 to -gl3 in the end of the filename
      size_t pos = f.rfind("-gl3");
      if (pos != string::npos) {
        f[pos+3] = '2';
      }
    }

    GlShaderMap::key_type key(f, shaderType);
    GlShaderMap::iterator i = shaderMap.find(key);
    if (i == shaderMap.end()) {
      shared_ptr<GlShader> shader(new GlShader(shaderType));
      readAndCompileSingleShader(*shader, f.c_str());
      shaderMap[key] = shader;
      return shader;
    }
    else {
      return i->second;
    }
  }
};

Material::Material(const string& vsFilename, const string& fsFilename)
  : programDesc_(GlProgramLibrary::getSingleton().getProgramDesc(vsFilename, fsFilename))
{}

static const char * getGlConstantName(GLenum c) {
  struct ValueNamePair {
    GLenum value;
    const char * name;
  };

  ValueNamePair valueNamePairs[] = {
    { GL_FLOAT, "GL_FLOAT" },
    { GL_FLOAT_VEC2, "GL_FLOAT_VEC2" },
    { GL_FLOAT_VEC3, "GL_FLOAT_VEC3" },
    { GL_FLOAT_VEC4, "GL_FLOAT_VEC4" },
    { GL_FLOAT_MAT2, "GL_FLOAT_MAT2" },
    { GL_FLOAT_MAT3, "GL_FLOAT_MAT3" },
    { GL_FLOAT_MAT4, "GL_FLOAT_MAT4" },
    { GL_FLOAT_MAT2x3, "GL_FLOAT_MAT2x3" },
    { GL_FLOAT_MAT2x4, "GL_FLOAT_MAT2x4" },
    { GL_FLOAT_MAT3x2, "GL_FLOAT_MAT3x2" },
    { GL_FLOAT_MAT3x4, "GL_FLOAT_MAT3x4" },
    { GL_FLOAT_MAT4x2, "GL_FLOAT_MAT4x2" },
    { GL_FLOAT_MAT4x3, "GL_FLOAT_MAT4x3" },
    { GL_INT, "GL_INT" },
    { GL_INT_VEC2, "GL_INT_VEC2" },
    { GL_INT_VEC3, "GL_INT_VEC3" },
    { GL_INT_VEC4, "GL_INT_VEC4" },
    { GL_UNSIGNED_INT_VEC2, "GL_UNSIGNED_INT_VEC2" },
    { GL_UNSIGNED_INT_VEC3, "GL_UNSIGNED_INT_VEC3" },
    { GL_UNSIGNED_INT_VEC4, "GL_UNSIGNED_INT_VEC4" },
    { GL_SAMPLER_1D, "GL_SAMPLER_1D" },
    { GL_SAMPLER_2D, "GL_SAMPLER_2D" },
    { GL_SAMPLER_CUBE, "GL_SAMPLER_CUBE" },
    { GL_SAMPLER_1D_SHADOW, "GL_SAMPLER_1D_SHADOW"},
    { GL_SAMPLER_2D_SHADOW, "GL_SAMPLER_2D_SHADOW"},
  };

  for (int i = 0, n = sizeof(valueNamePairs)/sizeof(valueNamePairs[0]); i < n; ++i) {
    if (valueNamePairs[i].value == c)
      return valueNamePairs[i].name;
  }
  return "Unkonwn";
}

void Material::draw(Geometry& geometry, const Uniforms& extraUniforms) {
  static GLint maxTextureImageUnits = 0;

  // Initialize maxTextureImageUnits if this is called for the first time
  if (maxTextureImageUnits == 0) {
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureImageUnits);
    assert(maxTextureImageUnits > 0); // GL spec says this has to be at least 2
  }

  glUseProgram(programDesc_->program);

  renderStates_.apply();  // transit to current states

  // Step 1:
  // set the uniforms and bind the textures
  int textureUnit = 0;
  for (int i = 0, n = programDesc_->uniforms.size(); i < n; ++i) {
    const GlProgramDesc::UniformDesc& ud = programDesc_->uniforms[i];

    const Uniforms* uniformsList[] = {&uniforms_, &extraUniforms};
    int j = 0;
    for (; j < 2; ++j) {
      const Uniforms::Value* u = uniformsList[j]->get(ud.name);

      // if the name looks like blah[0], and the uniform is not found, we also try stripping the '[0]'
      if (u == NULL && ud.name.length() >= 3 && ud.name.compare(ud.name.length() - 3, 3, "[0]") == 0)
        u = uniformsList[j]->get(ud.name.substr(0, ud.name.length() - 3));

      if (u) {
        if (u->type == ud.type && u->size >= ud.size) {
          switch (u->type) {
          case GL_SAMPLER_1D:
          case GL_SAMPLER_2D:
          case GL_SAMPLER_CUBE:
          case GL_SAMPLER_1D_SHADOW:
          case GL_SAMPLER_2D_SHADOW:
            {
              const shared_ptr<Texture> *tex = u->getTextures();

              // If this assert hits, the Uniform::Value is incorrectly implemented
              assert(tex != NULL);
              static const int MAX_TEX_UNITS = 1024;
              GLint texUnits[MAX_TEX_UNITS];
              int count = 0;
              for (; count < ud.size; ++count) {
                if (textureUnit == maxTextureImageUnits) {
                  stringstream s;
                  s << "System allows a maximum of " << maxTextureImageUnits << ". The current shader is trying to use more than that.";
                  throw runtime_error(s.str());
                }

                glActiveTexture(GL_TEXTURE0 + textureUnit);
                tex[count]->bind();
                texUnits[count] = textureUnit++;
              }
              u->apply(ud.location, ud.size, texUnits);
            }
            break;
          default:
            u->apply(ud.location, ud.size, NULL);
          }
        }
        else {
          stringstream s;
          s << "Uniform variable " << ud.name << ": supplied value and declared variable do not match in type and/or size."
            << "\nSupplied value: type = " << getGlConstantName(u->type) << ", size = " << u->size
            << "\nDeclared in shader: type = " << getGlConstantName(ud.type) << ", size = " << ud.size;
          throw runtime_error(s.str());
        }
        break;
      }
    }
    if (j == 2) {
      stringstream s;
      s << "Uniform variable " << ud.name << ": used in the shader codes, but not supplied. Type = " << getGlConstantName(ud.type) << ", Size = " << ud.size;
      throw runtime_error(s.str());
    }
  }

  // Step 2:
  // see what attribs are provided by the geometry
  const vector<string>& geoAttribNames = geometry.getVertexAttribNames();

  const static int MAX_ATTRIB = 64;
  int attribIndices[MAX_ATTRIB];
  const size_t numAttribs = geoAttribNames.size();
  
  if (numAttribs > MAX_ATTRIB) {
      throw runtime_error(string("Number of attributes contained in geometry is greater than maximally supported number of attributes. Consider increasing MAX_ATTRIB."));
  }

  for (size_t i = 0; i < numAttribs; ++i) {
    attribIndices[i] = -1;
  }

  // simple and stupid O(n^2) wiring, should use a hashtable to reduce to O(n)
  for (int i = 0, n = programDesc_->attribs.size(); i < n; ++i) {
    const GlProgramDesc::AttribDesc& ad = programDesc_->attribs[i];

    size_t j = 0;
    for (; j < numAttribs; ++j) {
      if (geoAttribNames[j] == ad.name) {
        attribIndices[j] = ad.location;
        break;
      }
    }
    if (j == numAttribs) {
      throw runtime_error(string("Vertex attribute ") + ad.name
                          + ": used in the shader codes, but not supplied.");
    }
  }

  for (size_t i = 0; i < numAttribs; ++i) {
    if (attribIndices[i] >= 0)
      glEnableVertexAttribArray(attribIndices[i]);
  }

  // Now let the geometry draw its self
  geometry.draw(attribIndices);

  for (size_t i = 0; i < numAttribs; ++i) {
    if (attribIndices[i] >= 0)
      glDisableVertexAttribArray(attribIndices[i]);
  }
}
