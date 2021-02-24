# If you create new headers/cpp files, add them to these list!
set(HEADER_FILES
#	touchdecode.hpp
)

set(SOURCE_FILES
  touchdecoder.hpp
)
add_library(touchdecoder ${SOURCE_FILES} ${HEADER_FILES})
set_target_properties(touchdecoder PROPERTIES LINKER_LANGUAGE CXX)
