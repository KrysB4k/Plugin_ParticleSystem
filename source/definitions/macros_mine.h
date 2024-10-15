#pragma once

#define RAD(a) ((a) * float(M_PI) / HALF_ANGLE)
#define ANG(a) Round((a) * HALF_ANGLE / float(M_PI))
#define RGBA(r, g, b, a) (b | (g << 8) | (r << 16) | (a << 24))
