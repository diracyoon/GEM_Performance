CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(GEM_Performance)

MESSAGE(${CMAKE_PROJECT_NAME})

SET(PROJECT_DIR ${PROJECT_SOURCE_DIR}/..)

LIST(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT)
include(${ROOT_USE_FILE})

INCLUDE_DIRECTORIES(${PROJECT_DIR}/Include)

SET(INCLUDE_DIR ${PROJECT_DIR}/Include)
SET(SRC_DIR ${PROJECT_DIR}/Src)

ROOT_GENERATE_DICTIONARY(G__GEM_Performance Const_Def.h Gain.h Read_RO.h Read_MCA_Data.h MC2.h DP5.h ADC_Spectrum_Analyzer.h Rate_Capability.h Discharge_Prob.h Read_Env_Data.h LINKDEF ${INCLUDE_DIR}/GEM_Performance_LinkDef.h)

ADD_COMPILE_OPTIONS(-std=c++11)

ADD_LIBRARY(GEM_Performance SHARED ${SRC_DIR}/Gain.C ${SRC_DIR}/Read_RO.C ${SRC_DIR}/Read_MCA_Data.C ${SRC_DIR}/MC2.C ${SRC_DIR}/DP5.C  ${SRC_DIR}/ADC_Spectrum_Analyzer.C ${SRC_DIR}/Rate_Capability.C ${SRC_DIR}/Discharge_Prob.C ${SRC_DIR}/Read_Env_Data.C G__GEM_Performance.cxx)

TARGET_LINK_LIBRARIES(GEM_Performance ${ROOT_LIBRARIES})
