#List all modules used by this board
set(RTM_8SFP_MODULES
  "PCA9554"
  "EEPROM_24XX64"
  )

set(RTM_8SFP_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#Include the modules sources
##Sensors SDR list
set( PROJ_SRCS ${PROJ_SRCS}
  ${RTM_8SFP_PATH}/rtm_user.c
  ${RTM_8SFP_PATH}/sdr_list.c
  )

#Include headers path
set(PROJ_HDRS ${PROJ_HDRS} ${RTM_8SFP_PATH})

#Set the variables in the main scope
set(TARGET_MODULES ${TARGET_MODULES} ${RTM_8SFP_MODULES} PARENT_SCOPE)

set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
