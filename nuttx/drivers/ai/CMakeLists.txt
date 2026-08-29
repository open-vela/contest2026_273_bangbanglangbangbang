# drivers/ai/CMakeLists.txt

if(CONFIG_AI)

# TinyML engine
if(CONFIG_AI_TINYML)
  list(APPEND SRCS tinyml/tinyml_engine.c)
endif()

# CMSIS-NN operations
if(CONFIG_AI_CMSIS_NN)
  list(APPEND SRCS tinyml/cmsis_nn_ops.c)
endif()

# Image classifier example
if(CONFIG_AI_IMG_CLASSIFIER)
  list(APPEND SRCS tinyml/img_classifier.c)
endif()

# AI Agent bridge
if(CONFIG_AI_AGENT)
  list(APPEND SRCS ai_agent_bridge.c)
endif()

target_sources(drivers PRIVATE ${SRCS})

endif()
