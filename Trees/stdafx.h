// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <fstream>
#include <list>
#include <random>
#include <string>
#include <vector>

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL\cl.hpp>
#include <direct.h>

#include <gl\GL.h>
#include <gl\GLU.h>

#include "ops.h"