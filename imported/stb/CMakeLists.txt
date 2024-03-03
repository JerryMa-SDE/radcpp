file(GLOB stb_sources
     "*.h"
     "*.c"
)

add_library(stb
    ${stb_sources}
)
