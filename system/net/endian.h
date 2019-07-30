// Find the architecture of the host
#if defined(__BIG_ENDIAN__) || defined(__POWERPC__) || defined(__ppc__)

	#define BIG_ENDIAN

#elif defined(__LITTLE_ENDIAN__) || defined(__AVR__) || defined(__AVR_ARCH__)

	#define LITTLE_ENDIAN

#endif

// If all else fails define the endianess hardcoded.
// #define BIG_ENDIAN
// #define LITTLE_ENDIAN

// Big endian hosts are lucky!
#if defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN)

	#define htons(A) (A)
	#define htonl(A) (A)
	#define ntohs(A) (A)
	#define ntohl(A) (A)

#elif defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)

	int htons(int x);
	long htonl(long x);

	#define ntohs(A)  htons(A)
	#define ntohl(A)  htonl(A)

#else

  #error "Must define one of BIG_ENDIAN or LITTLE_ENDIAN"

#endif
