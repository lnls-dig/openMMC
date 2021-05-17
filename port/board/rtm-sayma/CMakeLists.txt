#List all modules used by this board
set(RTM_SAYMA_MODULES
  "PCF8574"
  "ADT7420"
  )

set(RTM_SAYMA_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#Include the modules sources
##Sensors SDR list
set( PROJ_SRCS ${PROJ_SRCS}
  ${RTM_SAYMA_PATH}/rtm_user.c
  ${RTM_SAYMA_PATH}/sdr_list.c
  )

#Include headers path
set(PROJ_HDRS ${PROJ_HDRS} ${RTM_SAYMA_PATH})

#Set the variables in the main scope
set(TARGET_MODULES ${TARGET_MODULES} ${RTM_SAYMA_MODULES} PARENT_SCOPE)

set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
