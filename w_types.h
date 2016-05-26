
#pragma once

#include "glext.h"

typedef int32_t i32;
typedef int16_t i16;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

struct bitflag {
   u32 bits;

   void set(u32 flag) {
      bits |= flag;
   }

   void unset(u32 flag) {
      bits &= ~flag;
   }

   int check(u32 flag) {
      return(bits & flag);
   }
};

// TODO(afox): finish struct compression
u32 packsize(const u8 * data, u32 size)
{
   if (size <= 4) {
      return 0;
   }
   u32 ps = 0;
   u8 cbyte = data[0];
   u8 ccount = 0;
   for (u32 i = 1; i < size; i++) {
      u8 rbyte = data[i];
      if (ccount == 255 || rbyte != cbyte) {
         ps += 2;
         cbyte = rbyte;
         ccount = 0;
      } else {
         ccount++;
      }
   }
   if (ccount > 0) {
      ps += 2;
   }
   return ps;
}

// TODO(afox): finish struct compression
u32 unpacksize(const u8 * data, u32 size) {
   return 0;
}

// TODO(afox): finish struct compression
void testpacksize(const u8 * data, u32 size, const char * name) {
   u32 smaller = packsize(data, size);
   if (smaller && smaller < size) {
      printf("%s packed from %u to %u bytes (%4.2f)\n", name, size, smaller, (float)smaller/(float)size);
   } else {
      printf("packing wouldn't make %s smaller (%d bytes larger)\n", name, (i32)smaller - (i32)size);
   }
}

// TODO(afox): finish struct compression
u32 pack(const u8 * src, u8 * dest, u32 size)
{
   return 0;
}

// TODO(afox): finish struct compression
u32 unpack(const u8 * src, u8 * dest, u32 size)
{
   return 0;
}


#define BIT(X) (1 << X)

#define assert(X) SDL_assert(X)

#define GROW_MULTIPLY(N)\
{\
   if (!newsize) {\
      newsize = N;\
   }\
   while (newsize < (t)) {\
      newsize *= (N);\
   }\
}

#define GROW_ADD(N)\
{\
   newsize = (((t)/(N)) + 1) * (N);\
}

#define ARRAY(T, GROW) {\
   u32 size;\
   u32 count;\
   T * contents;\
   void clear(void) {\
      count = 0;\
   }\
   void destroy(void) {\
      count = 0;\
      SDL_free(contents);\
      contents = 0;\
      size = 0;\
   }\
   void accomodate(u32 t) {\
      SDL_assert(t < UINT_MAX);\
      u32 newsize = size;\
      GROW;\
      if (newsize != size) {\
         size = newsize;\
         contents = (T*)SDL_realloc(contents, size * sizeof(T));\
      }\
   }\
   T* getNew() {\
      accomodate(count + 1);\
      T* res = contents + count;\
      count++;\
      return res;\
   }\
   void add(T n) {\
      accomodate(count + 1);\
      contents[count] = n;\
      count++;\
   };\
   void rem(u32 location) {\
      if (location < count && count > 0) {\
         u32 move_seg = count - location;\
         if (move_seg) {\
            SDL_memmove(contents + location, contents + location + 1, move_seg * sizeof(T));\
         }\
         count--;\
      }\
   }\
   void urem(u32 location) {\
      if (location < count && count > 0) {\
         contents[location] = contents[--count];\
      }\
   }\
   T* get(u32 location) {\
      if (location > count) {\
         return 0;\
      }\
      return contents + location;\
   }\
   T& operator[](u32 location) {\
      return contents[location % count];\
   }\
   T* operator+(u32 offset) {\
      return contents + (offset % count);\
   }\
}


#define POOL(T)\
{\
   struct entry {\
      u32 inuse;\
      T cont;\
   };\
   struct ARRAY(entry,GROW_ADD(4)) contents;\
   void clear(void) {\
      contents.clear();\
   }\
   void destroy(void) {\
      contents.destroy();\
   }\
   void accomodate(u32 t) {\
      contents.accomodate(t);\
   }\
   entry* getUnused(u32 * ind) {\
      entry* e;\
      for (u32 i = 0; i < contents.count; i++) {\
         e = contents + i;\
         if (!e->inuse) {\
            if (ind) {\
               *ind = i;\
            }\
            return e;\
         }\
      }\
      if (ind) {\
         *ind = contents.count;\
      }\
      return contents.getNew();\
   }\
   u32 getNew() {\
      u32 res = 0;\
      entry * e = getUnused(&res);\
      SDL_assert(e);\
      e->inuse = 1;\
      return res;\
   }\
   u32 add(const T& n) {\
      u32 res = 0;\
      entry * e = getUnused(&res);\
      assert(e);\
      e->inuse = 1;\
      return res;\
   }\
   T* get(u32 location) {\
      entry * e = contents + location;\
      assert(e);\
      if (e->inuse) {\
         return &e->cont;\
      } else {\
         return 0;\
      }\
   }\
   void rem(u32 location) {\
      entry * e = contents + location;\
      assert(e);\
      e->inuse = 0;\
   }\
   u32 count(void) {\
      return contents.count;\
   }\
}

#define ENT_ID_MASK 0x00ffffff
#define ENT_GEN_MASK 0xff000000
#define ENT_GEN_OFS 24

struct ent_id {
   u32 id;
   void setGen(u8 gen)
   {
      id = (id & ENT_ID_MASK) | (gen << ENT_GEN_OFS);
   }
   void setID(u32 nid)
   {
      id = (id & ENT_GEN_MASK) | (nid & ENT_ID_MASK);
   }
   u8 getGen()
   {
      return (id >> ENT_GEN_OFS);
   }
   u32 getID()
   {
      return (id & ENT_ID_MASK);
   }
};

struct id_collection {
   struct ent_id_entry {
      u8 gen;
      u8 used;
   };

   struct ARRAY(ent_id_entry,GROW_ADD(16)) ids;

   ent_id grab()
   {
      ent_id res;
      ent_id_entry * ce;
      ent_id_entry * unused = 0;
      u32 i;
      for (i = 0; i < ids.count; i++) {
         ce = ids.get(i);
         assert(ce);
         if (!ce->used) {
            unused = ce;
            break;
         }
      }
      if (unused) {
         res.setGen(unused->gen);
         res.setID(i + 1);
         unused->used = 1;
      } else {
         assert(ids.count < ENT_ID_MASK);
         i = ids.count;
         unused = ids.getNew();
         unused->gen = 0;
         unused->used = 1;
         res.setGen(unused->gen);
         res.setID(i + 1);
      }
      return res;
   }

   u32 isCurrent(ent_id id)
   {
      u32 id_id = id.getID();
      if (id_id == 0) return 0;
      ent_id_entry * c = ids.get(id_id - 1);
      return (c && c->used && c->gen == id.getGen());
   }

   void drop(ent_id id)
   {
      if (isCurrent(id)) {
         u32 id_id = id.getID();
         ent_id_entry * c = ids.get(id_id - 1);
         c->used = 0;
         c->gen += 1;
      }
   }
};

