#define LOWER(x) ((x) & 0xFF)
#define UPPER(x) (((x) >> 8) & 0xFF)

#define RANDOM(x) ((unsigned int)rand()%x)
