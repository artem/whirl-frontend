#pragma once

#if !defined(NDEBUG) || defined(WHIRL_FORCE_LOGGING)
#define WHIRL_LOGGING_ENABLED
#else
#undef WHIRL_LOGGING_ENABLED
#endif
