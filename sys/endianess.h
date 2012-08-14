#ifndef LEMON_SYS_ENDIANESS_H
#define LEMON_SYS_ENDIANESS_H

#include <lemon/sys/configure.h>

#if defined(LEMON_BIG_END)
/* All bigendian systems are fine, just ignore the swaps. */
#define __lemon_ntoh16(x) (void)(x)
#define __lemon_ntoh32(x) (void)(x)
#define __lemon_ntoh64(x) (void)(x)

#define __lemon_hton16(x) (void)(x)
#define __lemon_hton32(x) (void)(x)
#define __lemon_hton64(x) (void)(x)

#else

/* For __FreeBSD_version */
#if defined(HAVE_SYS_PARAM_H)
#include <sys/param.h>
#endif

#if defined(__linux__) || defined(__GLIBC__)
#include <byteswap.h>
#define __lemon_ntoh16(x) x = bswap_16(x)
#define __lemon_ntoh32(x) x = bswap_32(x)
#define __lemon_ntoh64(x) x = bswap_64(x)

#define __lemon_hton16(x) x = bswap_16(x)
#define __lemon_hton32(x) x = bswap_32(x)
#define __lemon_hton64(x) x = bswap_64(x)

#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define __lemon_ntoh16(x) x = OSSwapBigToHostInt16(x)
#define __lemon_ntoh32(x) x = OSSwapBigToHostInt32(x)
#define __lemon_ntoh64(x) x = OSSwapBigToHostInt64(x)

#define __lemon_hton16(x) x = OSSwapBigToHostInt16(x)
#define __lemon_hton32(x) x = OSSwapBigToHostInt32(x)
#define __lemon_hton64(x) x = OSSwapBigToHostInt64(x)

#elif defined(__NetBSD__)
#include <sys/endian.h>
#define __lemon_ntoh16(x) BE16TOH(x)
#define __lemon_ntoh32(x) BE32TOH(x)
#define __lemon_ntoh64(x) BE64TOH(x)

#define __lemon_hton16(x) BE16TOH(x)
#define __lemon_hton32(x) BE32TOH(x)
#define __lemon_hton64(x) BE64TOH(x)

#elif defined(__OpenBSD__)
#include <sys/endian.h>
#define __lemon_ntoh16(x) x = swap16(x)
#define __lemon_ntoh32(x) x = swap32(x)
#define __lemon_ntoh64(x) x = swap64(x)

#define __lemon_hton16(x) x = swap16(x)
#define __lemon_hton32(x) x = swap32(x)
#define __lemon_hton64(x) x = swap64(x)

#elif defined(__FreeBSD__) && __FreeBSD_version >= 470000
#include <sys/endian.h>
#define __lemon_ntoh16(x) x = be16toh(x)
#define __lemon_ntoh32(x) x = be32toh(x)
#define __lemon_ntoh64(x) x = be64toh(x)

#define __lemon_hton16(x) x = be16toh(x)
#define __lemon_hton32(x) x = be32toh(x)
#define __lemon_hton64(x) x = be64toh(x)

/* This is a slow but portable implementation, it has multiple evaluation
 * problems so beware.
 * Old FreeBSD's and Solaris don't have <byteswap.h> or any other such
 * functionality!
 */

#elif defined(__FreeBSD__) || defined(__sun) || defined(__bsdi__) || defined(WIN32) || defined(__CYGWIN__) || defined(__BEOS__)
#define __lemon_ntoh16(x) \
 x = ((((x) & 0xff00) >> 8) | \
      (((x) & 0x00ff) << 8))
#define __lemon_ntoh32(x) \
 x = ((((x) & 0xff000000) >> 24) | \
      (((x) & 0x00ff0000) >>  8) | \
      (((x) & 0x0000ff00) <<  8) | \
      (((x) & 0x000000ff) << 24))
#define __lemon_ntoh64(x) \
 x = ((((x) & 0xff00000000000000ULL) >> 56) | \
      (((x) & 0x00ff000000000000ULL) >> 40) | \
      (((x) & 0x0000ff0000000000ULL) >> 24) | \
      (((x) & 0x000000ff00000000ULL) >>  8) | \
      (((x) & 0x00000000ff000000ULL) <<  8) | \
      (((x) & 0x0000000000ff0000ULL) << 24) | \
      (((x) & 0x000000000000ff00ULL) << 40) | \
      (((x) & 0x00000000000000ffULL) << 56))


 #define __lemon_hton16(x) \
 x = ((((x) & 0xff00) >> 8) | \
      (((x) & 0x00ff) << 8))
#define __lemon_hton32(x) \
 x = ((((x) & 0xff000000) >> 24) | \
      (((x) & 0x00ff0000) >>  8) | \
      (((x) & 0x0000ff00) <<  8) | \
      (((x) & 0x000000ff) << 24))
#define __lemon_hton64(x) \
 x = ((((x) & 0xff00000000000000ULL) >> 56) | \
      (((x) & 0x00ff000000000000ULL) >> 40) | \
      (((x) & 0x0000ff0000000000ULL) >> 24) | \
      (((x) & 0x000000ff00000000ULL) >>  8) | \
      (((x) & 0x00000000ff000000ULL) <<  8) | \
      (((x) & 0x0000000000ff0000ULL) << 24) | \
      (((x) & 0x000000000000ff00ULL) << 40) | \
      (((x) & 0x00000000000000ffULL) << 56))

#else

/* If there isn't a header provided with your system with this functionality
 * add the relevant || define( ) to the portable implementation above.
 */
#error "You need to add endian swap macros for you're system"

#endif

#endif /* WORDS_BIGENDIAN */

#endif //

