#define BLKIN(buf, port, len) blkin((buf), (unsigned char)&(port), (len))
#define BLKOUT(buf, port, len) blkout((buf), (unsigned char)&(port), (len))

unsigned char * blkin(unsigned char *buf, unsigned char port, unsigned char len);
unsigned char * blkout(unsigned char *buf, unsigned char port, unsigned char len);
