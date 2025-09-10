#pragma once

#ifdef ENGINE_EXPORTS

#define RENDER_API __declspec(dllexport)

#else

#define RENDER_API __declspec(dllimport)

#endif // ENGINE_EXPORTS
