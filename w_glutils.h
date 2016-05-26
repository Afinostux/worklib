
#pragma once

//glClientActiveTexture
//glNormalPointer
//glTexCoordPointer
//glDrawElements

#define GLFUNCS(X) \
   X(glGenBuffers,                  void,       (GLsizei n, GLuint * buffers)) \
   X(glDeleteBuffers,               void,       (GLsizei n, const GLuint * buffers)) \
   X(glBufferData,                  void,       (GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)) \
   X(glBufferSubData,               void,       (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)) \
   X(glBindBuffer,                  void,       (GLenum target, GLuint buffers)) \
   X(glCreateShader,                GLuint,     (GLenum shaderType)) \
   X(glShaderSource,                void,       (GLuint shader, GLsizei count, const GLchar **string, const GLint* length)) \
   X(glCompileShader,               void,       (GLuint shader)) \
   X(glDeleteShader,                void,       (GLuint shader)) \
   X(glGetShaderiv,                 void,       (GLuint shader, GLenum pname, GLint *params)) \
   X(glGetShaderInfoLog,            void,       (GLuint shader, GLsizei, GLsizei *length, GLchar *infoLog)) \
   X(glGetProgramInfoLog,           void,       (GLuint program, GLsizei, GLsizei *length, GLchar *infoLog)) \
   X(glCreateProgram,               GLuint,     (void)) \
   X(glAttachShader,                void,       (GLuint program, GLuint shader)) \
   X(glGetProgramiv,                void,       (GLuint program, GLenum pname, GLint * params)) \
   X(glUseProgram,                  void,       (GLuint program)) \
   X(glLinkProgram,                 void,       (GLuint program)) \
   X(glDeleteProgram,               void,       (GLuint program)) \
   X(glGetAttribLocation,           GLint,      (GLuint program, const GLchar *name)) \
   X(glGetUniformLocation,          GLint,      (GLuint program, const GLchar *name)) \
   X(glVertexAttribPointer,         GLint,      (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)) \
   X(glEnableVertexAttribArray,     void,       (GLuint index)) \
   X(glDisableVertexAttribArray,    void,       (GLuint index)) \
   X(glUniform1f,                   void,       (GLint location, GLfloat value)) \
   X(glUniformMatrix3x4fv,          void,       (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)) \

#define FPTR(name, ret, args) \
   typedef ret (APIENTRY * name##_func) args;\
   extern name##_func name; \
   name##_func name = 0;

#define FBIND(name, ret, args) \
   name = (name##_func) SDL_GL_GetProcAddress(#name); SDL_assert(name);

#define FNUKE(name, ret, args) \
   name = 0;

GLFUNCS(FPTR);

int bindGLFuncs(void)
{
   GLFUNCS(FBIND);
   return 0;
}

void nukeGLFuncs(void)
{
   GLFUNCS(FNUKE);
}

#undef FPTR
#undef FBIND
#undef FNUKE

#undef GLFUNCS

int checkGLerr()
{
   int res = 0;
   GLenum err;
   while ((err = glGetError()) != GL_NO_ERROR) {
      res = 1;
      switch (err) {
         case (GL_INVALID_ENUM): printf("GL_INVALID_ENUM"); break;
         case (GL_INVALID_VALUE): printf("GL_INVALID_VALUE"); break;
         case (GL_INVALID_OPERATION): printf("GL_INVALID_OPERATION"); break;
         case (GL_INVALID_FRAMEBUFFER_OPERATION): printf("GL_INVALID_FRAMEBUFFER_OPERATION"); break;
         case (GL_OUT_OF_MEMORY): printf("GL_OUT_OF_MEMORY"); break;
         case (GL_STACK_UNDERFLOW): printf("GL_STACK_UNDERFLOW"); break;
         case (GL_STACK_OVERFLOW): printf("GL_STACK_OVERFLOW"); break;
      }
   }
   return res;
}

#define LINELEN 512
struct shadersource {
   struct ARRAY(char*,GROW_ADD(16)) lines;

   void destroy() {
      for (u32 i = 0; i < lines.count; i++) {
         SDL_free(lines[i]);
      }
      lines.destroy();
   }

   void print() {
      for (u32 i = 0; i < lines.count; i++) {
         printf("%s", lines[i]);
      }
   }

   void append(const char * fname) {
      SDL_RWops * rw = SDL_RWFromFile(fname, "rb");
      char line[LINELEN];
      if (rw) {
         int done = 0;
         while (!done) {
            u32 ofs = 0;
            u32 comment = 0;
            while (1) {
               done = !SDL_RWread(rw, line + ofs, 1, 1);
               char cchar = *(line + ofs);
               if (done || (cchar == '\n') || (ofs >= LINELEN)) break;

               if (ofs < 2) {
                  if (ofs == 0) {
                     if (cchar == '/') {
                        comment = 1;
                     }
                  } else if (ofs == 1) {
                     if (comment && cchar == '/') {
                        break;
                     } else {
                        comment = 0;
                     }
                  }
               }
               ofs++;
            }

            if (comment) {
               char ichar;
               done = !SDL_RWread(rw, &ichar, 1, 1);
               while (!done && ichar != '\n') {
                  done = !SDL_RWread(rw, &ichar, 1, 1);
               }
               continue;
            }

            SDL_assert(ofs < LINELEN);
            if (ofs && line[ofs - 1] == '\r') {
               ofs--;
            }
            line[ofs+1] = 0;
            line[ofs] = '\n';
            if (!strlen(line)) continue;

            if (!strncmp("#include", line, 8)) {
               char nfname[LINELEN];
               char * src = line;
               while (*src && !(*src=='\"')) src++;
               if (*src) {
                  src++;
                  u32 fnamesize = 1;
                  while (src[fnamesize] && !(src[fnamesize]=='\"')) fnamesize++;
                  if (src[fnamesize]) {
                     SDL_memcpy(nfname, src, fnamesize);
                     nfname[fnamesize] = 0;
                     append(nfname);
                  }
               }
            } else {
               char * nline = (char*)SDL_malloc(sizeof(char) * LINELEN);
               SDL_zerop(nline);
               SDL_memcpy(nline, line, LINELEN);
               lines.add(nline);
            }
         }
      }
      SDL_RWclose(rw);
   }

   void load(const char * fname) {
      destroy();
      append(fname);
   }

   void use(GLuint shader) {
      const GLchar** source = (const char**)lines.contents;
      glShaderSource(shader, lines.count, source, 0);
      glCompileShader(shader);
   }
};

struct shader {
   GLuint program;

   GLint vert;
   GLint color;
   GLint texcoord;
   GLint blendind;
   GLint normal;

   GLint palette;
   GLint percent;
};

int checkShader(GLuint shader, const char * fname)
{
   int success;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

   if (!success) {
      int logsize;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);
      char * log = (char*)SDL_malloc(sizeof(char) * logsize);
      glGetShaderInfoLog(shader, logsize, 0, log);
      printf("error in %s:\n%s\n", fname, log);
      SDL_free(log);
   }

   return success;
}

int checkProgram(GLuint program)
{
   int success = 0;
   glGetProgramiv(program, GL_LINK_STATUS, &success);

   if (!success) {
      int logsize;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logsize);
      char * log = (char*)SDL_malloc(sizeof(char) * logsize);
      glGetProgramInfoLog(program, logsize, 0, log);
      printf("error linking program:\n%s\n", log);
      SDL_free(log);
   }

   return success;
}

shader loadShader(const char * vert, const char * frag)
{
   shader res = {};
   char line[LINELEN];

   shadersource src = {};
   GLuint vertb = glCreateShader(GL_VERTEX_SHADER);
   if (vertb) {
      src.load(vert);
      src.use(vertb);
      if (!checkShader(vertb, vert)) {
         src.destroy();
         glDeleteShader(vertb);
         return res;
      }
   }

   GLuint fragb = glCreateShader(GL_FRAGMENT_SHADER);
   if (fragb) {
      src.load(frag);
      src.use(fragb);
      if (!checkShader(fragb, frag)) {
         src.destroy();
         glDeleteShader(vertb);
         glDeleteShader(fragb);
         return res;
      }
   }

   src.destroy();

   res.program = glCreateProgram();
   if (res.program) {
      glAttachShader(res.program, vertb);
      glAttachShader(res.program, fragb);
      glLinkProgram(res.program);
      glDeleteShader(vertb);
      glDeleteShader(fragb);

      if (!checkProgram(res.program)) {
         glDeleteProgram(res.program);
         res.program = 0;
         return res;
      }
      res.vert = glGetAttribLocation(res.program, "vert");
      res.color = glGetAttribLocation(res.program, "color");
      res.texcoord = glGetAttribLocation(res.program, "texcoord");
      res.blendind = glGetAttribLocation(res.program, "blendind");
      res.normal = glGetAttribLocation(res.program, "normal");
      res.palette = glGetUniformLocation(res.program, "palette");
      res.percent = glGetUniformLocation(res.program, "percent");
   }
   
   return res;
}

