#ifndef GSPLAT_H
#define GSPLAT_H

#include "vendor/gsplat/config.h"

#if defined(USE_HIP) || defined(USE_CUDA)
#include "vendor/gsplat/bindings.h"
#endif

#if defined(USE_MPS)
#include "vendor/gsplat-metal/bindings.h"
#endif

#include "vendor/gsplat-cpu/bindings.h"

#endif