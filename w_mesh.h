
#pragma once
#include "iqm.h"

const char* iqmformatstring(u32 type)
{
   switch (type) {
      case (IQM_BYTE): return("IQM_BYTE");
      case (IQM_UBYTE): return("IQM_UBYTE");
      case (IQM_SHORT): return("IQM_SHORT");
      case (IQM_USHORT): return("IQM_USHORT");
      case (IQM_INT): return("IQM_INT");
      case (IQM_UINT): return("IQM_UINT");
      case (IQM_HALF): return("IQM_HALF");
      case (IQM_FLOAT): return("IQM_FLOAT");
      case (IQM_DOUBLE): return("IQM_DOUBLE");
      default: return("NOT A TYPE");
   }
}

enum meshflags {
   MF_HARDWARE       = BIT(0),
   MF_VERTICES       = BIT(1),
   MF_TEXCOORDS      = BIT(2),
   MF_COLORS         = BIT(3),
   MF_INDEXED        = BIT(4),
   MF_BLENDINDS      = BIT(5),
   MF_BLENDWEIGHTS   = BIT(6),
   MF_SKELETON       = BIT(7),
   MF_NORMALS        = BIT(8),
};

struct joint {
   //// FIXME(afox): 
   //m34 inverse;
   //m34 transform;
   int parent;
};

struct skeleton {
   joint * joints;
   u16 joint_count;
};

struct blendind {
   u8 bone[4];
};

#define MAX_BONES 64

struct mesh {
   v3 * vertices;
   v2 * texcoords;
   color * colors;
   blendind * blendinds;
   u16 * indices;
   v3 * normals;

   skeleton skel;

   bitflag flags;
   GLuint vbo;

   u16 vertex_count;
   u16 index_count;

   //hardware infos
   u16 stride;
   u8 color_ofs;
   u8 texcoord_ofs;
   u8 blendind_ofs;
   u8 normal_ofs;

   void destroy() {
      SDL_free(vertices);
      SDL_free(colors);
      SDL_free(indices);
      if (vbo) glDeleteBuffers(1, &vbo);
      SDL_zerop(this);
   }
   
   // FIXME(afox): 
#if 0
   void print(const char* name) {
      if (name) {
         printf("mesh %s:\n", name);
      } else {
         printf("mesh:\n");
      }
      printf("vertex count: %u\n", vertex_count);
      for (u32 i = 0; i < vertex_count; i++) {
         vertices[i].print("vertex");
      }
      if (colors) {
         for (u32 i = 0; i < vertex_count; i++) {
            printColor(colors[i], 0);
         }
      } else {
         printf("no vertex colors\n");
      }
      if (blendinds) {
         for (u32 i = 0; i < vertex_count; i++) {
            blendind * bi = blendinds + i;
            printf("blend index: [%u, %u, %u, %u]\n", bi->bone[0], bi->bone[1], bi->bone[2], bi->bone[3]);
         }
      } else {
         printf("no blend indices\n");
      }
      if (indices) {
         printf("index count: %u\n", index_count);
         for (u32 i = 0; i < index_count; i++) {
            printf("index: ( %u )\n", indices[i]);
         }
      } else {
         printf("no indices\n");
      }
   }

   void print() {
      print(0);
   }
#endif

   void upload() {
      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      int use_colors = flags.check(MF_COLORS);
      int use_texcoords = flags.check(MF_TEXCOORDS);
      int use_blendinds = flags.check(MF_BLENDINDS);
      int use_normals = flags.check(MF_NORMALS);

      const GLsizei position_size = sizeof(v3),
                       color_size = sizeof(color),
                       texcoord_size = sizeof(v2),
                       normal_size = sizeof(v3),
                       blendind_size = sizeof(blendind);

      GLsizei buffer_size = position_size;
      if (use_colors) {
         color_ofs = buffer_size;
         buffer_size += color_size;
      }
      if (use_texcoords) {
         texcoord_ofs = buffer_size;
         buffer_size += texcoord_size;
      }
      if (use_blendinds) {
         blendind_ofs = buffer_size;
         buffer_size += blendind_size;
      }
      if (use_normals) {
         normal_ofs = buffer_size;
         buffer_size += normal_size;
      }

      stride = buffer_size;
      buffer_size *= vertex_count;

      glBufferData(GL_ARRAY_BUFFER, buffer_size, 0, GL_STATIC_DRAW);

      GLintptr offset = 0;
      //interleave vertex data on graphics card
      for (u32 i = 0; i < vertex_count; i++) {
         glBufferSubData(GL_ARRAY_BUFFER, offset, position_size, vertices + i);
         offset += position_size;
         if (use_colors) {
            glBufferSubData(GL_ARRAY_BUFFER, offset, color_size, colors + i);
            offset += color_size;
         }
         if (use_texcoords) {
            glBufferSubData(GL_ARRAY_BUFFER, offset, texcoord_size, texcoords + i);
            offset += texcoord_size;
         }
         if (use_blendinds) {
            glBufferSubData(GL_ARRAY_BUFFER, offset, blendind_size, blendinds + i);
            offset += blendind_size;
         }
         if (use_normals) {
            glBufferSubData(GL_ARRAY_BUFFER, offset, normal_size, normals + i);
            offset += normal_size;
         }
      }

      SDL_free(vertices);
      vertices = 0;
      SDL_free(colors);
      colors = 0;

      flags.set(MF_HARDWARE);
      checkGLerr();
   }

   void draw(shader * s, float percent) {
      int hardware = flags.check(MF_HARDWARE);
      int useshader = s && s->program;
      int usecolor = (flags.check(MF_COLORS) && (!useshader || s->color != -1));
      int usetexture = (flags.check(MF_TEXCOORDS) && (!useshader || s->texcoord != -1));
      int useblendinds = (flags.check(MF_BLENDINDS) && (!useshader || s->texcoord));
      int usenormals = (flags.check(MF_NORMALS) && (!useshader || s->normal));
      int useskeleton = (flags.check(MF_SKELETON) && (!useshader || s->palette != -1));
      int usepercent = (useshader && s->percent != -1);
      //// FIXME(afox): 
      //m34 palette[MAX_BONES];

#if 0
      if (useskeleton) {
         transform t;
         t.setIdentity();
         static float accum = 0.f;
         t.rotation = makequat(accum, 1, 0, 0);
         accum++;
         for (u32 i = 0; i < skel.joint_count; i++) {
            int p = skel.joints[i].parent;
#if 0
#else
            if ( p == -1 ) {
               palette[i] = skel.joints[i].inverse;
            } else {
               palette[i] = skel.joints[i].transform * palette[p] * skel.joints[i].inverse;
            }
#endif
         }
      }
#endif

      void* vert_ptr;
      void* color_ptr;
      void* texcoord_ptr;
      void* blendind_ptr;
      void* normal_ptr;
      u32 stride_;

      if (hardware) {
         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         vert_ptr = (void*)0;
         color_ptr = (void*)color_ofs;
         texcoord_ptr = (void*)texcoord_ofs;
         blendind_ptr = (void*)blendind_ofs;
         normal_ptr = (void*)normal_ofs;
         stride_ = stride;
      } else {
         glBindBuffer(GL_ARRAY_BUFFER, 0);
         vert_ptr = (void*)vertices;
         color_ptr = (void*)colors;
         texcoord_ptr = (void*)texcoords;
         blendind_ptr = (void*)blendinds;
         normal_ptr = (void*)normals;
         stride_ = 0;
      }

      if (useshader) {
         glUseProgram(s->program);
         glEnableVertexAttribArray(s->vert);
         glVertexAttribPointer(s->vert, 3, GL_FLOAT, 0, stride_, vert_ptr);

         if (usecolor) {
            glEnableVertexAttribArray(s->color);
            glVertexAttribPointer(s->color, 4, GL_UNSIGNED_BYTE, 0, stride_, color_ptr);
         }

         if (usetexture) {
            glEnableVertexAttribArray(s->texcoord);
            glVertexAttribPointer(s->texcoord, 2, GL_FLOAT, 0, stride_, texcoord_ptr);
         }

         if (usenormals) {
            glEnableVertexAttribArray(s->normal);
            glVertexAttribPointer(s->normal, 3, GL_FLOAT, 0, stride_, normal_ptr);
         }

#if 0
         if (useskeleton && useblendinds) {
            glEnableVertexAttribArray(s->blendind);
            glVertexAttribPointer(s->blendind, 4, GL_UNSIGNED_BYTE, 0, stride_, blendind_ptr);
         }

         if (useskeleton) {
            assert(skel.joint_count < MAX_BONES);
            glUniformMatrix3x4fv(s->palette, skel.joint_count, 0, palette->m);
         }
#endif

         if (usepercent) {
            glUniform1f(s->percent, percent);
         }

         if (index_count) {
            glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, indices);
         } else {
            glDrawArrays(GL_TRIANGLES, 0, vertex_count);
         }

#if 0
         if (useblendinds) {
            glDisableVertexAttribArray(s->blendind);
         }
#endif

         if (usenormals) {
            glDisableVertexAttribArray(s->normal);
         }
         if (usetexture) {
            glDisableVertexAttribArray(s->texcoord);
         }
         if (usecolor) {
            glDisableVertexAttribArray(s->vert);
         }
         glDisableVertexAttribArray(s->color);
      } else {
         assert(0 && "please use a shader\n");
         glUseProgram(0);

         glEnableClientState(GL_VERTEX_ARRAY);
         glVertexPointer(3, GL_FLOAT, stride_, vert_ptr);

         if (usecolor) {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_UNSIGNED_BYTE, stride_, color_ptr);
         }

         if (usetexture) {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableVertexAttribArray(s->texcoord);
            glTexCoordPointer(2, GL_FLOAT, stride_, texcoord_ptr);
         }

         if (index_count) {
            glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, indices);
         } else {
            glDrawArrays(GL_TRIANGLES, 0, vertex_count);
         }

         if (usetexture) {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
         }
         if (usecolor) {
            glDisableClientState(GL_COLOR_ARRAY);
         }
         glDisableClientState(GL_VERTEX_ARRAY);
         assert(!checkGLerr());
      }
   }
};

struct meshmaker {
   u16 current_vertex;
   u16 current_colour;
   u16 current_index;
   u16 current_texcoord;
   u16 current_blendind;
   u16 current_normal;
   u16 current_joint_parent;
   u16 current_joint_transform;
   mesh* target;

   void start(mesh * target, u16 size) {
      assert(target);

      current_vertex = 0;
      current_colour = 0;
      current_index = 0;
      current_texcoord = 0;
      current_blendind = 0;
      current_normal = 0;
      current_joint_parent = 0;
      current_joint_transform = 0;

      target->destroy();

      this->target = target;

      target->flags.set(MF_VERTICES);
      target->vertices = (v3*)SDL_malloc(sizeof(v3) * size);
      target->vertex_count = size;
   }

   void startIndices(u16 count) {
      assert(target);
      assert(!target->flags.check(MF_INDEXED));

      target->flags.set(MF_INDEXED);
      target->index_count = count;
      target->indices = (u16*)SDL_malloc(sizeof(u16) * count);
   }

   void startSkeleton(u16 count) {
      assert(target);
      assert(!target->flags.check(MF_SKELETON));
      target->flags.set(MF_SKELETON);
      target->skel.joint_count = count;
      target->skel.joints = (joint*)SDL_malloc(sizeof(joint) * count);
   }

   void pushIndex(u16 index) {
      assert(target);
      assert(target->flags.check(MF_INDEXED));
      assert(current_index < target->index_count);
      target->indices[current_index++] = index;
   }

   void pushVert(const v3 & vert) {
      assert(target);
      assert(current_vertex < target->vertex_count);
      target->vertices[current_vertex++] = vert;
   }

   void pushVert(float x, float y, float z) {
      v3 v = {x, y, z};
      pushVert(v);
   }

   void pushColor(const color & c) {
      assert(target);
      assert(current_colour < target->vertex_count);
      if (!target->flags.check(MF_COLORS)) {
         target->flags.set(MF_COLORS);
         target->colors = (color*)SDL_realloc(0, target->vertex_count * sizeof(color));
      }
      target->colors[current_colour++] = c;
   }

   void pushColor(u8 r, u8 g, u8 b, u8 a) {
      color c = makecolor(r, g, b, a);
      pushColor(c);
   }

   void pushTexcoord(const v2 & tc) {
      assert(target);
      assert(current_texcoord < target->vertex_count);
      if (!target->flags.check(MF_TEXCOORDS)) {
         target->flags.set(MF_TEXCOORDS);
         target->texcoords = (v2*)SDL_realloc(0, target->vertex_count * sizeof(v2));
      }
      target->texcoords[current_texcoord++] = tc;
   }

   void pushTexcoord(float u, float v) {
      v2 tc = {u, v};
      pushTexcoord(tc);
   }

   void pushBlendInd(const blendind& b) {
      assert(target);
      assert(current_blendind < target->vertex_count);
      if (!target->flags.check(MF_BLENDINDS)) {
         target->flags.set(MF_BLENDINDS);
         target->blendinds = (blendind*)SDL_malloc(sizeof(blendind) * target->vertex_count);
      }
      target->blendinds[current_blendind++] = b;
   }

   void pushNormal(const v3& norm) {
      assert(target);
      assert(current_normal < target->vertex_count);
      if (!target->flags.check(MF_NORMALS)) {
         target->flags.set(MF_NORMALS);
         target->normals = (v3*)SDL_malloc(sizeof(v3) * target->vertex_count);
      }
      target->normals[current_normal++] = norm;
   }

   // FIXME(afox): 
#if 0
   void pushSkelParent(int p) {
      assert(target);
      assert(current_joint_parent < target->skel.joint_count);
      target->skel.joints[current_joint_parent++].parent = p;
   }

   void pushSkelTransform(const transform& t) {
      assert(target);
      assert(current_joint_transform < target->skel.joint_count);
      assert(current_joint_parent >= current_joint_transform);

      int p = target->skel.joints[current_joint_transform].parent;

      m34 tf = t.getm34();
      m34 itf = tf.getInverse();

#if 1
      if (p >= 0 ) {
         tf = target->skel.joints[p].transform * tf;
         itf = itf * target->skel.joints[p].inverse;
      }
#endif

      target->skel.joints[current_joint_transform].transform = tf;
      target->skel.joints[current_joint_transform].inverse = itf;
      current_joint_transform++;
   }
#endif
};

mesh makeTestTri()
{
   mesh res = {};
   meshmaker m;
   m.start(&res, 3);
   m.pushVert(0, 0, 0.5);
   m.pushVert(-0.5, -1, 0.5);
   m.pushVert(0.5, -1, 0.5);

#if 1
   m.pushColor(255, 0, 0, 255);
   m.pushColor(0, 255, 0, 255);
   m.pushColor(0, 0, 255, 255);
#endif

   return res;
}

mesh makeDebugTriMesh(float l, float w, float z, float ofs, color * c)
{
   mesh res = {};
   meshmaker m;
   m.start(&res, 3);
   m.pushVert(0, (l * ofs), z);
   m.pushVert(-w * 0.5, (l * ofs) - l, z);
   m.pushVert(w * 0.5, (l * ofs) - l, z);
   if (c) {
      m.pushColor(*c);
      m.pushColor(*c);
      m.pushColor(*c);
   } else {
      m.pushColor(255, 255, 255, 255);
      m.pushColor(255, 255, 255, 255);
      m.pushColor(255, 255, 255, 255);
   }
   return res;
}

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
mesh loadIQM(const char * fname)
{
   mesh res = {};
   SDL_RWops * rw = SDL_RWFromFile(fname,"rb");
   int valid = 1;
   if (rw) {
      meshmaker m;
      iqmheader head;
      SDL_RWread(rw, &head, sizeof(iqmheader), 1);
      if (!SDL_strcmp(head.magic, IQM_MAGIC)) {
         m.start(&res, head.num_vertexes);
         m.startIndices(head.num_triangles * 3);

         SDL_RWseek(rw, head.ofs_vertexarrays, RW_SEEK_SET);
         iqmvertexarray * arrays = (iqmvertexarray*)SDL_malloc(sizeof(iqmvertexarray) * head.num_vertexarrays);
         SDL_RWread(rw, arrays, sizeof(iqmvertexarray), head.num_vertexarrays);

         for (u32 i = 0; i < head.num_vertexarrays; i++) {
            iqmvertexarray * cva = arrays + i;
            switch (cva->type) {
               case (IQM_POSITION):
               case (IQM_COLOR):
               case (IQM_TEXCOORD):
               case (IQM_BLENDINDEXES):
               case (IQM_BLENDWEIGHTS):
               case (IQM_NORMAL):
                  break;
               case (IQM_TANGENT):
               default: 
                  continue;
            };
            SDL_RWseek(rw, cva->offset, RW_SEEK_SET);
            switch (cva->type) {
               case (IQM_POSITION): 
                  {
                     assert(cva->format == IQM_FLOAT);
                     assert(cva->size == 3);
                     v3 vpos;
                     for (u32 j = 0; j < head.num_vertexes; j++) {
                        SDL_RWread(rw, &vpos, sizeof(float) * 3, 1);
                        m.pushVert(vpos);
                     }
                  }
                  break;

               case (IQM_TEXCOORD):
                  {
                     assert(cva->format == IQM_FLOAT);
                     assert(cva->size == 2);
                     v2 texc;
                     for (u32 j = 0; j < head.num_vertexes; j++) {
                        SDL_RWread(rw, &texc, sizeof(float) * 2, 1);
                        m.pushTexcoord(texc);
                     }
                  }
                  break;

               case (IQM_NORMAL):
                  {
                     assert(cva->format == IQM_FLOAT);
                     assert(cva->size == 3);
                     v3 norm;
                     for (u32 j = 0; j < head.num_vertexes; j++) {
                        SDL_RWread(rw, &norm, sizeof(float) * 3, 1);
                        m.pushNormal(norm);
                     }
                  }
                  break;

               case (IQM_TANGENT): break;
               case (IQM_BLENDINDEXES):
                  {
                     assert(cva->format == IQM_UBYTE);
                     assert(cva->size == 4);
                     blendind bi;
                     for (u32 j = 0; j < head.num_vertexes; j++) {
                        SDL_RWread(rw, bi.bone, sizeof(u8) * 4, 1);
                        m.pushBlendInd(bi);
                     }
                  }
                  break;
               case (IQM_BLENDWEIGHTS): break;
                  {
                     printf("blendweight info: %s, %u\n", iqmformatstring(cva->format), cva->size);
                  }
                  break;
               case (IQM_COLOR):
                  {
                     assert(cva->format == IQM_UBYTE);
                     assert(cva->size == 4);
                     color c = {};
                     for (u32 j = 0; j < head.num_vertexes; j++) {
                        SDL_RWread(rw, c.m, sizeof(u8) * 4, 1);
                        m.pushColor(c);
                     }
                  }
                  break;
               default: assert(0); break;
            };
         }
         SDL_free(arrays);

         SDL_RWseek(rw, head.ofs_triangles, RW_SEEK_SET);
         for (u32 i = 0; i < res.index_count; i++) {
            u32 ind;
            SDL_RWread(rw, &ind, sizeof(u32), 1);
            m.pushIndex(ind);
         }

         // FIXME(afox): 
#if 0
         if (head.num_joints) {
            m.startSkeleton(head.num_joints);
            SDL_RWseek(rw, head.ofs_joints, RW_SEEK_SET);
            iqmjoint ij;
            transform t;
            for (u32 i = 0; i < res.skel.joint_count; i++) {
               SDL_RWread(rw, &ij, sizeof(iqmjoint), 1);
               m.pushSkelParent(ij.parent);
               t.position = makev3(ij.translate);
               t.rotation = makev4(ij.rotate);
               t.scale = makev3(ij.scale);
               m.pushSkelTransform(t);
            }
         }
#endif
      } else {
         valid = 0;
      }
   } else {
      valid = 0;
   }

   if (!valid) {
      res.destroy();
   }

   if (rw) {
      SDL_RWclose(rw);
   }

   //res.print();

   return res;
}
#endif

