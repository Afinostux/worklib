
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

#if 1
#define ARRAY_START_SIZE 16
template <typename T>
struct array {
   u32 size;
   u32 count;
   T * contents;

   void clear(void) {
      count = 0;
   }

   void destroy(void) {
      count = 0;
      SDL_free(contents);
      contents = 0;
      size = 0;
   }

   void accomodate(u32 t) {
      SDL_assert(t < UINT_MAX);

      u32 newsize = size;

      if (!newsize) {
         newsize = ARRAY_START_SIZE;
      }

      while (newsize < (t)) {
         newsize += newsize;
      }

      if (newsize != size) {
         size = newsize;
         contents = (T*)SDL_realloc(contents, size * sizeof(T));
      }
   }

   T* getNew() {
      accomodate(count + 1);
      T* res = contents + count;
      count++;
      return res;
   }

   void add(const T& n) {
      accomodate(count + 1);

      contents[count] = n;
      count++;
   };

   void rem(u32 location) {
      if (location < count && count > 0) {
         u32 move_seg = count - location;
         if (move_seg) {
            SDL_memmove(contents + location, contents + location + 1, move_seg);
         }
         count--;
      }
   }

   void urem(u32 location) {
      if (location < count && count > 0) {
         contents[location] = contents[--count];
      }
   }

   T* get(u32 location) {
      if (location > count) {
         return 0;
      }
      return contents + location;
   }

   T& operator[](u32 location) {
      return contents[location % count];
   }

   T* operator+(u32 offset) {
      return contents + (offset % count);
   }
};

template <typename T>
struct poolentry {
   u32 inuse;
   T cont;
};

template <typename T>
struct pool {
   array< poolentry<T> > contents;

   void clear(void) {
      contents.clear();
   }

   void destroy(void) {
      contents.destroy();
   }

   void accomodate(u32 t) {
      contents.accomodate(t);
   }

   poolentry<T>* getUnused(u32 * ind) {
      poolentry<T> * e;
      for (uint i = 0; i < contents.count; i++) {
         e = contents + i;
         if (!e->inuse) {
            if (ind) {
               *ind = i;
            }
            return e;
         }
      }
      if (ind) {
         *ind = contents.count;
      }
      return contents.getNew();
   }

   u32 getNew() {
      u32 res = 0;
      poolentry<T> * e = getUnused(&res);
      SDL_assert(e);
      e->inuse = 1;
      return res;
   }

   u32 add(const T& n) {
      u32 res = 0;
      poolentry<T> * e = getUnused(&res);
      assert(e);
      e->inuse = 1;
      return res;
   }

   T* get(u32 location) {
      poolentry<T> * e = contents + location;
      assert(e);
      if (e->inuse) {
         return &e->cont;
      } else {
         return 0;
      }
   }

   void rem(u32 location) {
      poolentry<T> * e = contents + location;
      assert(e);
      e->inuse = 0;
   }

   u32 count(void) {
      return contents.count;
   }
};
#endif
