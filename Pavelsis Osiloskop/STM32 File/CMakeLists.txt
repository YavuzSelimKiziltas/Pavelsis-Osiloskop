cmake_minimum_required(VERSION 3.20)

project("009_Qt_ADC_RX_TX_MAGNITUDE_FREQ_UPDATE" C CXX ASM)

include(cmake/st-project.cmake)

add_executable(${PROJECT_NAME})
add_st_target_properties(${PROJECT_NAME})