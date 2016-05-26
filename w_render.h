
#pragma once

struct camera {
   v3 position;
   v3 lookat;
   v3 up;
   float fov;
};

enum {
   RA_MESH,
   RA_LINESEG,
};

struct render_atom {
   camera * c;
   u32 type;
   transform t;
   shader * s;
   float percent;
   union {
      mesh * mesh;
      struct {
         v3 point[2];
         color c;
      } lineseg;
   };
};

struct atom_pool POOL(render_atom);
struct atom_array ARRAY(render_atom,GROW_ADD(4));
struct atom_frame ARRAY(render_atom*,GROW_ADD(4));

struct {
   int w, h;
   int stop;
   SDL_Window * window;
   SDL_GLContext context; 
   camera cam;
   camera debugcam;
   atom_pool per;
   atom_array imm;
   atom_frame sortedframe;
} globalrender;

void initCameras(float fov)
{
   globalrender.cam.fov = fov;
   globalrender.cam.up.z = 1;
   globalrender.debugcam.fov = fov;
   globalrender.debugcam.up.z = 1;
}

void loadCameraMatrix( camera * c )
{
   m44 projection = makePerspective(c->fov, (float)globalrender.w/globalrender.h, 0.01, 10);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glLoadMatrixf(projection.m);

   //m44 modelview = makeLookat(c->position, c->lookat, c->up);

   glMatrixMode(GL_MODELVIEW);
#if 0
#if 0 
   glLoadTransposeMatrixf(modelview.m);
#else 
   glLoadMatrixf(modelview.m);
#endif
#else
   glLoadIdentity();
   gluLookAt(c->lookat.x, c->lookat.y, c->lookat.z,
         c->position.x, c->position.y, c->position.z,
         c->up.x, c->up.y, c->up.z);
   glTranslatef(c->lookat.x-c->position.x, c->lookat.y-c->position.y, c->lookat.z-c->position.z);
#endif
}

void render3D() {
   globalrender.sortedframe.clear();
   for (u32 i = 0; i < globalrender.per.count(); i++) {
      render_atom * ra = globalrender.per.get(i);
      if (ra) {
         globalrender.sortedframe.add(ra);
      }
   }
   for (u32 i = 0; i < globalrender.imm.count; i++) {
      render_atom * ra = globalrender.imm + i;
      if (ra) {
         globalrender.sortedframe.add(ra);
      }
   }
   loadCameraMatrix(&globalrender.cam);
   m44 tm;
   for (u32 i = 0; i < globalrender.sortedframe.count; i++) {
      render_atom * ra = globalrender.sortedframe[i];
      assert(ra);
      tm = ra->t.getm44();
      glPushMatrix();
      glMultMatrixf(tm.m);
      switch (ra->type) {
         case RA_MESH:
            {
               ra->mesh->draw(ra->s, ra->percent);
            } break;
         case RA_LINESEG:
            {
               glUseProgram(0);
               glBindBuffer(GL_ARRAY_BUFFER, 0);
               glEnableClientState(GL_VERTEX_ARRAY);
               glColor4ubv(ra->lineseg.c.m);
               glVertexPointer(3, GL_FLOAT, 0, ra->lineseg.point);
               glDisableClientState(GL_VERTEX_ARRAY);
            } break;
         default:
            continue;
      }
      glPopMatrix();
   }
   globalrender.imm.clear();
}

void dimmMesh(const transform& t, mesh * m, shader * s, float p)
{
   render_atom ra = {};
   ra.type = RA_MESH;
   ra.t = t;
   ra.s = s;
   ra.mesh = m;
   ra.percent = p;
   globalrender.imm.add(ra);
}

void dimmLineseg(const v3 & a, const v3 & b, const color & c)
{
   render_atom ra = {};
   ra.type = RA_MESH;
   ra.lineseg.point[0] = a;
   ra.lineseg.point[1] = b;
   ra.lineseg.c = c;
   globalrender.imm.add(ra);
}

void modifyViewport(int w, int h)
{
   globalrender.w = w;
   globalrender.h = h;
   glViewport(0, 0, w, h);
}


