
#pragma once
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define EPSILON 0.00001

#define RADTODEG (180.0/M_PI)
#define DEGTORAD (M_PI/180.0)

//
//    -- scalar functions --
//

float frand()
{
   return (((float)rand()/(float)RAND_MAX) * 2.f) - 1.f;
}

float frand1()
{
   return ((float)rand()/(float)RAND_MAX);
}

int randrange(int min, int max)
{
   return (rand() % (max - min)) + min;
}

float frandrange(float min, float max)
{
   return frand1() * (max - min) + min;
}

//
//    -- 2d vector functions --
//

union v2 {
   float m[2];
   struct {
      float x, y;
   };
   struct {
      float u, v;
   };

   float length() {
      return (SDL_sqrtf(x*x+y*y));
   }

   float sqlen() {
      return (x*x+y*y);
   }

   v2& normalize() {
      float il = 1.f/length();
      x *= il;
      y *= il;
      return *this;
   }

   v2 ncopy() {
      v2 res = {x, y};
      res.normalize();
      return res;
   }

   void print(const char * name) {
      if (name) {
         printf("v2 %s: [%6.3f, %6.3f]\n", name, x, y);
      } else {
         printf("v2: [%6.3f, %6.3f]\n", x, y);
      }
   }

   void print() {
      print(0);
   }
};

v2 makev2(float x, float y)
{
   v2 res = {x, y};
   return res;
}

v2 operator+(const v2& a, const v2& b) {
   v2 res = {a.x + b.x, a.y + b.y};
   return res;
}

v2 operator-(const v2& a, const v2& b) {
   v2 res = {a.x - b.x, a.y - b.y};
   return res;
}

v2 operator-(const v2& a) {
   v2 res = {-a.x, -a.y};
   return res;
}

float operator*(const v2& a, const v2& b) {
   return ((a.x * b.x) + (a.y * b.y));
}

v2 operator*(const v2& a, float b) {
   v2 res = {a.x * b, a.y * b};
   return res;
}

v2 operator*(float b, const v2& a) {
   v2 res = {a.x * b, a.y * b};
   return res;
}

float operator^(const v2& a, const v2& b)
{
   return a.x * b.y - a.y * b.x;
}

v2 operator/(const v2& a, float b) {
   b = 1.f/b;
   return a * b;
}

//
//    -- 3d vector functions --
//

union v3 {
   float m[3];
   struct {
      float x, y, z;
   };
   struct {
      float yaw, pitch, roll;
   };

   struct {
      v2 n;
      float d;
   };

   v3 getAngles() {
      v3 nl = this->ncopy();
      v3 res = {};
      return res;
   }

   v3 eulerVector() {
      float cosp = cos(DEGTORAD * pitch);
      v3 res = {
         cosp*-sin(DEGTORAD * yaw),
         cosp*-cos(DEGTORAD * yaw),
         sin(DEGTORAD * pitch)
      };
      return res;
   }

   float length() const {
      return (sqrtf( x*x + y*y + z*z ));
   }

   float sqlen() const {
      return ( x*x + y*y + z*z );
   }

   v3& normalize() {
      float il = 1.f/length();
      x *= il;
      y *= il;
      z *= il;
      return *this;
   }

   v3 ncopy() const {
      v3 res = {x, y, z};
      res.normalize();
      return res;
   }

   void print(const char * name) const {
      if (name) {
         printf("v3 %s: [%6.3f, %6.3f, %6.3f]\n", name, x, y, z);
      } else {
         printf("v3: [%6.3f, %6.3f, %6.3f]\n", x, y, z);
      }
   }

   void print() const {
      print(0);
   }
   
   v3& operator+=(const v3& other) {
      x += other.x;
      y += other.y;
      z += other.z;
      return *this;
   }

   v3& operator-=(const v3& other) {
      x -= other.x;
      y -= other.y;
      z -= other.z;
      return *this;
   }

   v3& operator*(float other) {
      x *= other;
      y *= other;
      z *= other;
      return *this;
   }
};

//static const v3 viewforward = {0, 0, 1};

v3 makev3(float x, float y, float z)
{
   v3 res = {x, y, z};
   return res;
}

v3 makev3(float * uvec)
{
   v3 res;
   for (u32 i = 0; i < 3; i++) {
      res.m[i] = uvec[i];
   }
   return res;
}

v3 operator+(const v3& a, const v3& b) {
   v3 res = {a.x + b.x, a.y + b.y, a.z + b.z};
   return res;
}

v3 operator-(const v3& a, const v3& b) {
   v3 res = {a.x - b.x, a.y - b.y, a.z - b.z};
   return res;
}

v3 operator-(const v3& a)
{
   return makev3(-a.x, -a.y, -a.z);
}

float operator*(const v3& a, const v3& b) {
   return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

v3 operator*(const v3& a, float b) {
   v3 res = {a.x * b, a.y * b, a.z * b};
   return res;
}

v3 operator*(float b, const v3& a) {
   v3 res = {a.x * b, a.y * b, a.z * b};
   return res;
}

v3 operator^(const v3& a, const v3& b)
{
   v3 res;
   res.x = a.y * b.z - a.z * b.y;
   res.z = a.x * b.y - a.y * b.x;
   res.y = a.z * b.x - a.x * b.z;
   return res;
}

v3 operator/(const v3& a, float b)
{
   b = 1.f/b;
   return a * b;
}

v3 getRightAngle(const v3& norm, const v3& vec)
{
   float t = norm * vec;
   return vec - (norm * t);
}

//
//    -- 4d vector functions --
//

union v4 {
   float m[4];
   struct {
      float x, y, z, w;
   };
   struct {
      float r, g, b, a;
   };
   struct {
      v3 i; float real;
   };
   struct {
      v3 n; float d;
   };
   struct {
      v3 v; float w_;
   };

   float length() {
      return (sqrtf( x*x + y*y + z*z + w*w ));
   }

   float sqlen() {
      return ( x*x + y*y + z*z + w*w );
   }

   v4& normalize() {
      float il = 1.f/length();
      x *= il;
      y *= il;
      z *= il;
      w *= il;
      return *this;
   }

   v4 ncopy() const {
      v4 res = {x, y, z, w};
      res.normalize();
      return res;
   }

   void print(const char * name) {
      if (name) {
         printf("v4 %s: [%6.3f, %6.3f, %6.3f, %6.3f]\n", name, x, y, z, w);
      } else {
         printf("v4: [%6.3f, %6.3f, %6.3f, %6.3f]\n", x, y, z, w);
      }
   }

   void print() {
      print(0);
   }
};

typedef v4 quat;
typedef v4 plane;

v4 makev4(float x, float y, float z, float w)
{
   v4 res;
   res.x = x;
   res.y = y;
   res.z = z;
   res.w = w;
   return res;
}

v4 makev4(const v3& v, float w)
{
   v4 res;
   res.v = v;
   res.w = w;
   return res;
}

v4 makev4(float * uvec)
{
   v4 res;
   for (u32 i = 0; i < 4; i++) {
      res.m[i] = uvec[i];
   }
   return res;
}

v4 operator+(const v4& a, const v4& b)
{
   return makev4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

v4 operator-(const v4& a, const v4& b)
{
   return makev4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

v4 operator*(const v4& a, float b)
{
   return makev4(a.x * b, a.y * b, a.z * b, a.w * b);
}

v4 operator*(float a, const v4& b)
{
   return b * a;
}

v4 operator/(const v4& a, float b)
{
   b = 1.f/b;
   return a * b;
}

float operator*(const v4& a, const v4& b)
{
   return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

//
//    -- planar functions --
//

plane makeplane(float distance, const v3& normal)
{
   plane res;
   res.n = normal.ncopy();
   res.d = distance;
   return res;
}

plane makeplane(const v3& a, const v3& b, const v3& c)
{
   plane res;
   res.n = ((b - a)^(c - a)).normalize();
   res.d = res.i * a;
   return res;
}

plane makeplane(const v3& point, const v3& normal)
{
   plane res;
   res.n = normal.ncopy();
   res.d = res.i * point;
   return res;
}

v3 closestPointOnPlane(const plane& p, const v3& point)
{
   float t = (p.n * point) - p.d;
   return (point - (t * p.n));
}

//
//    -- color functions --
//

union color {
   u32 i;
   u8 m[4];
   struct {
      u8 r, g, b, a;
   };
};

color makecolor(u8 r, u8 g, u8 b, u8 a)
{
   color out;
   out.r = r;
   out.g = g;
   out.b = b;
   out.a = a;
   return out;
}

void printColor(const color & c, const char * name)
{
   if (name) {
      printf("color %s: [%d, %d, %d, %d]\n", name, c.r, c.g, c.b, c.a);
   } else {
      printf("color: [%d, %d, %d, %d]\n", c.r, c.g, c.b, c.a);
   }
}

color toColor(const v4& a)
{
   color out;
   for (u32 i = 0; i < 4; i++) {
      if (a.m[i] > 1.f) {
         out.m[i] = 0xff;
      } else if (a.m[i] < 0.f) {
         out.m[i] = 0;
      } else {
         out.m[i] = round(255.f * a.m[i]);
      }
   }
   return out;
}

v4 toV4(const color& c)
{
   v4 out;
   for (u32 i = 0; i < 4; i++) {
      out.m[i] = (float)c.m[i]/255;
   }
   return out;
}

//
//    -- 3x3 matrix functions --
//

union m33 {
   float m[9];
   v3 v[3];
   struct {
      float
         xx, xy, xz,
         yx, yy, yz,
         zx, zy, zz;
   };
};

m33 m33identity()
{
   m33 res = 
   {
      1, 0, 0,
      0, 1, 0,
      0, 0, 1
   };
   return res;
}

m33 makem33(float y, float p, float r)
{
   float cz = cosf(y*DEGTORAD);
   float cx = cosf(p*DEGTORAD);
   float cy = cosf(r*DEGTORAD);
   float sz = sinf(y*DEGTORAD);
   float sx = sinf(p*DEGTORAD);
   float sy = sinf(r*DEGTORAD);
   m33 m = 
   {
      (cz*cy) - (sx*sy*sz), -(cx*sz), (sy*cz) + (sx*cy*sz),
      (cy*sz) + (sx*sy*cz), (cx*cz), (sy*sz) - (sx*cy*cz),
      -(cx*sy), (sx), (cx*cy)
      /*
      cz*cx, 	-cz*sx*cy + sz*sy, 	cz*sx*sy + sz*cy,
      sx, 	   cx*cy, 	            -cx*sy,
      -sz*cx, 	sz*sx*cy + cz*sy, 	-sz*sx*sy + cz*cy
      */
      /*
      (cz*cy) + (sz*sx*sy),   (-sz*cy) + (cz*sx*sy),  (cx*sy),
      (sz*cx),                (cz*cx),                (-sx),
      (-sy*cz) + (sz*cy*sx),  (sy*sz) + (cz*sx*cy),   (cx*cy)
      */
   };
   return m;
}

m33 makem33(const quat& q)
{
   m33 res = {};

   float x2 = q.x + q.x;
   float y2 = q.y + q.y;

   float z2 = q.z + q.z;

   float xx = q.x * x2;
   float xy = q.x * y2;
   float xz = q.x * z2;

   float yy = q.y * y2;
   float yz = q.y * z2;
   float zz = q.z * z2;

   float wx = q.w * x2;
   float wy = q.w * y2;
   float wz = q.w * z2;

   res.xx = 1.0 - (yy + zz);
   res.yx = xy - wz;
   res.zx = xz + wy;

   res.xy = xy + wz;
   res.yy = 1.0 - (xx + zz);
   res.zy = yz - wx;

   res.xz = xz - wy;
   res.yz = yz + wx;
   res.zz = 1.0 - (xx + yy);

   return res;
}

void printm33(const m33 & a, const char* name)
{
   if (name) {
      printf("~ m33 (%s) ~\n", name);
   } else {
      printf("~ m33 ~\n");
   }

   for (u32 y = 0; y < 3; y++) {
      for (u32 x = 0; x < 3; x++) {
         printf(" [%6.3f]", a.v[y].m[x]);
      }
      printf("\n");
   }

   printf("~~~\n");
}

v3 operator*(const m33& m, const v3& v)
{
   v3 res;
   for (u32 i = 0; i < 3; i++) {
      res.m[i] = v * m.v[i];
   }
   return res;
}

//
//    -- 3x4 matrix functions --
//

union m34 {
   float m[12];
   v4 v[3];
   struct {
      float
         xx, xy, xz, xw,
         yx, yy, yz, yw,
         zx, zy, zz, zw;
   };

   m34 getInverse() const
    {
       m34 res;
       v3 r1 = v[0].v;
       v3 r2 = v[1].v;
       v3 r3 = v[2].v;
       r1 = r1 / (r1 * r1);
       r2 = r2 / (r2 * r2);
       r3 = r3 / (r3 * r3);
       v3 trans = {
          v[0].w,
          v[1].w,
          v[2].w

       };
       res.v[0] = makev4(r1.x, r1.y, r1.z, -(r1 * trans));
       res.v[1] = makev4(r2.x, r2.y, r2.z, -(r2 * trans));
       res.v[2] = makev4(r3.x, r3.y, r3.z, -(r3 * trans));
       return res;
    }
};

v3 operator*(const m34 & a, const v3& b)
{
   v3 res;
   for (u32 i = 0; i < 3; i++) {
      const v4 *v = a.v + i;
      res.m[i] = v->i * b + v->real;
   }
   return res;
}

m34 m34identity()
{
   const m34 res = {
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0};
   return res;
}

void printm34(const m34& a, const char * name)
{
   if (name) {
      printf("~ m44 (%s) ~\n", name);
   } else {
      printf("~ m44 ~\n");
   }

   for (u32 y = 0; y < 3; y++) {
      for (u32 x = 0; x < 4; x++) {
         printf(" [%6.3f]", a.v[y].m[x]);
      }
      printf("\n");
   }

   printf("~~~\n");
}

void printm34(const m34& a)
{
   printm34(a, 0);
}

m34 makem34( float xx, float xy, float xz, float xw,
         float yx, float yy, float yz, float yw,
         float zx, float zy, float zz, float zw)
{
   m34 result = {xx, xy, xz, xw, yx, yy, yz, yw, zx, zy, zz, zw};
   return result;
}

#if 0
m34 operator*(const m34& a, const m34& b)
{
   //this kind of multiplication shouldn't be possible, but what we're
   //"really" doing is multiplying two 4x4 matrices
   return makem34(
         (a.xx * b.xx) + (a.xy * b.yx) + (a.xz * b.zx),
         (a.xx * b.xy) + (a.xy * b.yy) + (a.xz * b.zy),
         (a.xx * b.xz) + (a.xy * b.yz) + (a.xz * b.zz),
         (a.xx * b.xw) + (a.xy * b.yw) + (a.xz * b.zw) + a.xw,

         (a.yx * b.xx) + (a.yy * b.yx) + (a.yz * b.zx),
         (a.yx * b.xy) + (a.yy * b.yy) + (a.yz * b.zy),
         (a.yx * b.xz) + (a.yy * b.yz) + (a.yz * b.zz),
         (a.yx * b.xw) + (a.yy * b.yw) + (a.yz * b.zw) + a.yw,

         (a.zx * b.xx) + (a.zy * b.yx) + (a.zz * b.zx),
         (a.zx * b.xy) + (a.zy * b.yy) + (a.zz * b.zy),
         (a.zx * b.xz) + (a.zy * b.yz) + (a.zz * b.zz),
         (a.zx * b.xw) + (a.zy * b.yw) + (a.zz * b.zw) + a.zw
         );
} 
#else
m34 operator*(const m34& a, const m34& b)
{
   m34 res;
   res.v[0].v = (b.v[0].v*a.v[0].x + b.v[1].v*a.v[0].y + b.v[2].v*a.v[0].z);
   res.v[1].v = (b.v[0].v*a.v[1].x + b.v[1].v*a.v[1].y + b.v[2].v*a.v[1].z);
   res.v[2].v = (b.v[0].v*a.v[2].x + b.v[1].v*a.v[2].y + b.v[2].v*a.v[2].z);
   res.v[0].w = a.v[0].w;
   res.v[1].w = a.v[1].w;
   res.v[2].w = a.v[2].w;

   return res;
}
#endif

//
//    -- 4x4 matrix functions --
//

union m44 {
   float m[16];
   v4 v[4];
   struct {
      float 
         xx, xy, xz, xw,
         yx, yy, yz, yw,
         zx, zy, zz, zw,
         wx, wy, wz, ww;
   };
};

m44 getTranspose(const m44& mat)
{
   m44 res;
   res.xx = mat.xx;
   res.xy = mat.yx;
   res.xz = mat.zx;
   res.xw = mat.wx;
   res.yx = mat.xy;
   res.yy = mat.yy;
   res.yz = mat.zy;
   res.yw = mat.wy;
   res.zx = mat.xz;
   res.zy = mat.yz;
   res.zz = mat.zz;
   res.zw = mat.wz;
   res.wx = mat.xw;
   res.wy = mat.yw;
   res.wz = mat.zw;
   res.ww = mat.ww;
   return res;
}

void printm44(const m44 & a, const char* name)
{
   if (name) {
      printf("~ m44 (%s) ~\n", name);
   } else {
      printf("~ m44 ~\n");
   }

   for (u32 y = 0; y < 4; y++) {
      for (u32 x = 0; x < 4; x++) {
         printf(" [%6.3f]", a.v[y].m[x]);
      }
      printf("\n");
   }

   printf("~~~\n");
}

void printm44(const m44 & a)
{
   printm44(a, 0);
}

m44 makem44( 
         float xx, float xy, float xz, float xw,
         float yx, float yy, float yz, float yw,
         float zx, float zy, float zz, float zw,
         float wx, float wy, float wz, float ww)
{
   m44 res = {xx, xy, xz, xw, yx, yy, yz, yw, zx, zy, zz, zw, wx, wy, wz, ww};
   return res;
}

m44 m44identity()
{
   m44 res = {
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1};
   return res;
}

m44 m44test()
{
   m44 res = {
      1, 2, 3, 4,
      5, 6, 7, 8,
      9, 10, 11, 12,
      13, 14, 15, 16};
   return res;
}

m44 makePerspective(float fov, float aspect, float znear, float zfar)
{
   float h = (tanf(fov * DEGTORAD * 0.5)) * znear;
   float w = h * aspect;
   m44 res = {
      (2 * znear)/w, 0, 0, 0,
      0, (2 * znear)/h, 0, 0,
      0, 0, (zfar + znear)/(znear - zfar), (2 * zfar * znear/ znear - zfar),
      0, 0, -1, 0,
   };
   return res;
}

m44 makeTranslation(const v3& t)
{
   m44 res = m44identity();
   res.wx = t.x;
   res.wy = t.y;
   res.wz = t.z;
   return res;
}

v3 operator*(const m44& a, const v3& b)
{
   v3 res;
   res.x = a.xx * b.x + a.xy * b.y + a.xz * b.z + a.xw;
   res.y = a.yx * b.x + a.yy * b.y + a.yz * b.z + a.yw;
   res.z = a.zx * b.x + a.zy * b.y + a.zz * b.z + a.zw;
   return res;
}

// TODO(afox): fix lookat function
/*
m44 makeLookat(const v3& eye, const v3& target, const v3& up)
{
   v3 localfront, localup, localside;
   localfront = (target - eye).normalize();
   localside = (localfront ^ up).normalize();
   localup = (localside ^ localfront).normalize();
   m44 res;
   res.v[0] = makev4(-localside,     0);
   res.v[1] = makev4(localup,       0);
   res.v[2] = makev4(localfront,   0);
   res.v[3] = makev4(
         -eye.x, 
         -eye.y, 
         -eye.z, 1);
   printm44(res);
   return res;
}

m44 makeLookat(const v3& eye, const v3& target, const v3& up)
{
   v3 f = (target - eye).normalize();
   v3 local_up = up.ncopy();
   local_up.normalize();
   v3 s = f ^ local_up;
   s.normalize();
   v3 u = s ^ f;
   m44 res = {
      s.x, s.y, s.z, eye.x,
      u.x, u.y, u.z, eye.y,
      f.x, -f.y, -f.z, eye.z,
      0,    0,    0,    1
   };
    eye.print("eye pos");
    target.print("target pos");
    up.print("global up");
    printm44(res);
   return res;
}

m44 makeLookat(const v3& eyepos, const v3& target, const v3& gup)
{
    v3 x, y, z;
    z = (target - eyepos).normalize();
    y = gup;
    x = y ^ z;
    x = x.normalize();
    y = z ^ x;
    y = y.normalize();
    // mat is given transposed so OpenGL can handle it.
    m44 res = {
       x.x,  y.x,   z.x,   -eyepos.x,
       x.y,  y.y,   z.y,   -eyepos.y,
       x.z,  y.z,   z.z,   -eyepos.z,
       0,    0,     0,      1};
    x.print("eyepos");
    y.print("target");
    z.print("global up");
    printm44(res);
    return res;
}

m44 makeLookat(const v3& eyepos, const v3& target, const v3& gup)
{
   v3 forward, side, up;
   m44 res;
   //------------------
   forward = (target - eyepos).normalize();
   //------------------
   side = (forward ^ gup).normalize();
   //------------------
   up = (side^forward).normalize();
   //------------------
   res.v[0].v = side;
   res.v[0].w = 0;
   //------------------
   res.v[1].v = up;
   res.v[1].w = 0;
   //------------------
   res.v[2].v = -forward;
   res.v[2].w = 0;
   //------------------
   res.v[3] = makev4(0, 0, 0, 1);
   //------------------
   res.xw = -eyepos.x;
   res.yw = -eyepos.y;
   res.zw = -eyepos.z;

   printm44(res);

   return res;
}
*/
m44 operator* (const m44 & a, const m44 & b)
{
   return makem44(
         (a.xx * b.xx) + (a.xy * b.yx) + (a.xz * b.zx) + (a.xw * b.wx),
         (a.xx * b.xy) + (a.xy * b.yy) + (a.xz * b.zy) + (a.xw * b.wy),
         (a.xx * b.xz) + (a.xy * b.yz) + (a.xz * b.zz) + (a.xw * b.wz),
         (a.xx * b.xw) + (a.xy * b.yw) + (a.xz * b.zw) + (a.xw * b.ww),

         (a.yx * b.xx) + (a.yy * b.yx) + (a.yz * b.zx) + (a.yw * b.wx),
         (a.yx * b.xy) + (a.yy * b.yy) + (a.yz * b.zy) + (a.yw * b.wy),
         (a.yx * b.xz) + (a.yy * b.yz) + (a.yz * b.zz) + (a.yw * b.wz),
         (a.yx * b.xw) + (a.yy * b.yw) + (a.yz * b.zw) + (a.yw * b.ww),

         (a.zx * b.xx) + (a.zy * b.yx) + (a.zz * b.zx) + (a.zw * b.wx),
         (a.zx * b.xy) + (a.zy * b.yy) + (a.zz * b.zy) + (a.zw * b.wy),
         (a.zx * b.xz) + (a.zy * b.yz) + (a.zz * b.zz) + (a.zw * b.wz),
         (a.zx * b.xw) + (a.zy * b.yw) + (a.zz * b.zw) + (a.zw * b.ww),

         (a.wx * b.xx) + (a.wy * b.yx) + (a.wz * b.zx) + (a.ww * b.wx),
         (a.wx * b.xy) + (a.wy * b.yy) + (a.wz * b.zy) + (a.ww * b.wy),
         (a.wx * b.xz) + (a.wy * b.yz) + (a.wz * b.zz) + (a.ww * b.wz),
         (a.wx * b.xw) + (a.wy * b.yw) + (a.wz * b.zw) + (a.ww * b.ww)
         );
}

//
//    -- quaternion functions --
//

quat makequat(float angle, float ax, float ay, float az)
{
   angle = (angle * DEGTORAD)*0.5;
   float c = cosf(angle), s = sinf(angle);
   quat res;
   res.i = makev3(ax, ay, az).normalize() * s;
   res.real = c;
   return res;
}

quat makequat(float angle, const v3& axis)
{
   return makequat(angle, axis.x, axis.y, axis.z);
}

quat rotateQuat(const quat& src, const quat& dest)
{
   quat res;
   res.i =  (src.i^dest.i) + (src.real * dest.i) + (src.i * dest.real);
   res.w = src.real * dest.real - (src.i * dest.i);
   return res;
}

#if 0
quat lookat(const v3& src, const v3& dst)
{
   v3 forward = (dst - src).normalize();

   float ca = (viewforward * forward);

   if (fabs(ca + 1.0f) < EPSILON)
   {
      return makequat(M_PI, viewforward.x, viewforward.y, viewforward.z);
   }
   if (fabs(ca - 1.0f) < EPSILON)
   {
      quat res = {0, 0, 0, 1};
      return res;
   }

   float angle = acosf(ca);
   v3 axis = (viewforward ^ forward).normalize();
   return makequat(RADTODEG * angle, axis);
}
#endif

void quickrotate(const quat& q, v3 * v)
{
   m33 r = makem33(q);
   *v = r * (*v);
}

quat nlerp(const quat& a, const quat& b, float t)
{
   return ((a * (1-t)) + (b * t)).normalize();
}

#if 0
quat slerp(const quat& a, const quat& b, float t)
{
   quat res = {};
   float cosa = a * b;
   return res;
}
#endif

//
//    -- combined transform functions --
//

struct transform {
   v3 position;
   m33 rotation;
   v3 scale;

   void print(const char * name) {
      if (name) {
         printf("~~ xf %s ~~\n", name);
      } else {
         printf("~~ transform ~~\n");
      }
      position.print("position");
      //printm33(rotation, "rotation");
      scale.print("scale");
   }

   void print(void) {
      print(0);
   }

   transform getInverseTransform() const {
      transform res = {};
      //res.position = position;
      //// FIXME(afox):
      //res.rotation.i = rotation.i * -1;
      //res.rotation.real = rotation.real;

      //quickrotate(rotation, &res.position);

      return res;
   }

   void setIdentity() {
      position = makev3(0,0,0);
      rotation = m33identity();
      scale = makev3(1, 1, 1);
   }

   m44 getm44() {
      m44 res = {};
      {
         res.xx = rotation.xx * scale.x;
         res.xy = rotation.yx * scale.x;
         res.xz = rotation.zx * scale.x;
         res.yx = rotation.xy * scale.y;
         res.yy = rotation.yy * scale.y;
         res.yz = rotation.zy * scale.y;
         res.zx = rotation.xz * scale.z;
         res.zy = rotation.yz * scale.z;
         res.zz = rotation.zz * scale.z;
         res.wx = position.x;
         res.wy = position.y;
         res.wz = position.z;
         res.xw = 
         res.yw = 
         res.zw = 0;
         res.ww = 1;
      }

      return res;
   }

   m34 getm34() const {
      m34 res = {};
      {
         res.xx = rotation.xx * scale.x;
         res.yx = rotation.yx * scale.x;
         res.zx = rotation.zx * scale.x;

         res.xy = rotation.xy * scale.y;
         res.yy = rotation.yy * scale.y;
         res.zy = rotation.zy * scale.y;

         res.xz = rotation.xz * scale.z;
         res.yz = rotation.yz * scale.z;
         res.zz = rotation.zz * scale.z;

         res.xw = position.x;
         res.yw = position.y;
         res.zw = position.z;
      }

      return res;
   }
};

