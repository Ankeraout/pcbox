#ifndef __FRONTEND_H__
#define __FRONTEND_H__

#include <stdint.h>

typedef uint32_t color_t;

int frontend_init();
int frontend_resize(int width, int height);
void frontend_update(const color_t *pixels);

#endif
