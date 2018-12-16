#pragma once

namespace osomi{
	class RenderObject;
	class Defines {
	public:
		template<class T, class K> static bool instanceof(K *v) {
			return dynamic_cast<T*>(v) != nullptr;
		}
	
	};
}

#ifdef __arm__
#define __RASPBERRY__
#define __EGL__
#define __EGL2__
#endif


#if defined(__linux__) || (defined(__APPLE__) && defined(TARGET_OS_MAC))
#define __UNIX__
#define slp(x) sleep(x)
#else
#define slp(x) Sleep(x)
#endif

#ifdef __EGL2__
#include <GLES2/gl2.h>
#endif