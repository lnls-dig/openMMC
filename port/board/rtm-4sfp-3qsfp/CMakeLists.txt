#List all modules used by this board
set(RTM_4SFP_3QSFP_MODULES
  "PCA9554"
  "EEPROM_AT24MAC"
  "LM75"
  )

set(RTM_4SFP_3QSFP_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#Include the modules sources
##Sensors SDR list
set( PROJ_SRCS ${PROJ_SRCS}
  ${RTM_4SFP_3QSFP_PATH}/rtm_user.c
  ${RTM_4SFP_3QSFP_PATH}/sdr_list.c
  )

#Include headers path
set(PROJ_HDRS ${PROJ_HDRS} ${RTM_4SFP_3QSFP_PATH})

#Set the variables in the main scope
set(TARGET_MODULES ${TARGET_MODULES} ${RTM_4SFP_3QSFP_MODULES} PARENT_SCOPE)

set(PROJ_SRCS ${PROJ_SRCS} PARENT_SCOPE)
set(PROJ_HDRS ${PROJ_HDRS} PARENT_SCOPE)
