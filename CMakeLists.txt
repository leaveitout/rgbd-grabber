CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(rgbd-grabber)

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)
SET(GLAGS_NAMESPACE google)

FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(PCL REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(/usr/include)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PCL_INCLUDE_DIRS})

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib)
LINK_DIRECTORIES(/usr/lib)
LINK_DIRECTORIES(${PCL_LIBRARY_DIRS})

IF(APPLE)
  INCLUDE_DIRECTORIES(/usr/local/include)
  LINK_DIRECTORIES(/usr/local/lib)
ELSEIF(UNIX)
  INCLUDE_DIRECTORIES(/usr/local/include)
  LINK_DIRECTORIES(/usr/local/lib)
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
SET(CMAKE_CXX_FLAGS "-std=c++0x -fpermissive -Wno-deprecated -Wno-sign-compare")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
SET(CMAKE_CXX_LINK_FLAGS_DEBUG "-pg")

ADD_DEFINITIONS(${PCL_DEFINITIONS})

SET(VERSION "0.9.7")
SET(SOVERSION "0.9")

SET(SRC
  src/camera/ColorCamera.cpp src/camera/DepthCamera.cpp
  src/camera/StereoCamera.cpp src/camera/UVCamera.cpp
  src/camera/DistortionCalibrator.cpp src/camera/DepthCalibrator.cpp
  src/camera/ColorCalibrator.cpp src/camera/ColorRotator.cpp src/camera/DepthRotator.cpp)

SET(SRC_DS
  src/camera/DS325.cpp src/camera/DS325Calibrator.cpp)

SET(SRC_PMD
  src/camera/PMDNano.cpp)

SET(SRC_UEYE
  src/camera/UEye.cpp src/camera/ueye_cam_driver.cpp)

SET(LIB_EXTERNAL
  ${Boost_LIBRARIES} ${OpenCV_LIBS} ${PCL_LIBRARIES} gflags)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIB_EXTERNAL})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION ${VERSION} SOVERSION ${SOVERSION})

SET(LIB
  ${PROJECT_NAME} ${LIB_EXTERNAL})

ADD_EXECUTABLE(UVCameraCapture samples/UVCameraCapture.cpp)
ADD_DEPENDENCIES(UVCameraCapture ${SRC})
TARGET_LINK_LIBRARIES(UVCameraCapture ${LIB})
ADD_EXECUTABLE(StereoCameraCalibration samples/StereoCameraCalibration.cpp)
ADD_DEPENDENCIES(StereoCameraCalibration ${SRC})
TARGET_LINK_LIBRARIES(StereoCameraCalibration ${LIB})
INSTALL(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
INSTALL(DIRECTORY include/rgbd DESTINATION include)

OPTION(USE_DS "Use DepthSense DS325 camera" OFF)
IF(USE_DS)
  INCLUDE_DIRECTORIES(/opt/softkinetic/DepthSenseSDK/include)
  LINK_DIRECTORIES(/opt/softkinetic/DepthSenseSDK/lib)
  ADD_LIBRARY(${PROJECT_NAME}-ds SHARED ${SRC_DS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}-ds ${LIB})
  SET_TARGET_PROPERTIES(${PROJECT_NAME}-ds PROPERTIES VERSION ${VERSION} SOVERSION ${SOVERSION})
  ADD_EXECUTABLE(DS325Capture samples/DS325Capture.cpp)
  ADD_DEPENDENCIES(DS325Capture ${SRC} ${SRC_DS})
  TARGET_LINK_LIBRARIES(DS325Capture ${LIB} ${PROJECT_NAME}-ds DepthSense DepthSensePlugins)
  ADD_EXECUTABLE(DS325GetCalibrationData samples/DS325GetCalibrationData.cpp)
  ADD_DEPENDENCIES(DS325GetCalibrationData ${SRC} ${SRC_DS})
  TARGET_LINK_LIBRARIES(DS325GetCalibrationData ${LIB} ${PROJECT_NAME}-ds DepthSense DepthSensePlugins)
  ADD_EXECUTABLE(DS325Calibration samples/DS325Calibration.cpp)
  ADD_DEPENDENCIES(DS325Calibration ${SRC} ${SRC_DS})
  TARGET_LINK_LIBRARIES(DS325Calibration ${LIB} ${PROJECT_NAME}-ds DepthSense DepthSensePlugins)
  INSTALL(TARGETS ${PROJECT_NAME}-ds LIBRARY DESTINATION lib)
ENDIF()

OPTION(USE_PMD "Use PMD Nano camera" OFF)
IF(USE_PMD)
  ADD_LIBRARY(${PROJECT_NAME}-pmd SHARED ${SRC_PMD})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}-pmd ${LIB})
  SET_TARGET_PROPERTIES(${PROJECT_NAME}-pmd PROPERTIES VERSION ${VERSION} SOVERSION ${SOVERSION})
  ADD_EXECUTABLE(PMDNanoCapture samples/PMDNanoCapture.cpp)
  ADD_DEPENDENCIES(PMDNanoCapture ${SRC} ${SRC_PMD})
  TARGET_LINK_LIBRARIES(PMDNanoCapture ${LIB} ${PROJECT_NAME}-pmd pmdaccess2)
  INSTALL(TARGETS ${PROJECT_NAME}-pmd LIBRARY DESTINATION lib)
ENDIF()

OPTION(USE_UEYE "Use uEye camera" OFF)
IF(USE_UEYE)
  ADD_LIBRARY(${PROJECT_NAME}-ueye SHARED ${SRC_UEYE})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}-ueye ${LIB})
  SET_TARGET_PROPERTIES(${PROJECT_NAME}-ueye PROPERTIES VERSION ${VERSION} SOVERSION ${SOVERSION})
  ADD_EXECUTABLE(UEyeCapture samples/UEyeCapture.cpp)
  ADD_DEPENDENCIES(UEyeCapture ${SRC} ${SRC_UEYE})
  TARGET_LINK_LIBRARIES(UEyeCapture ${LIB} ${PROJECT_NAME}-ueye ueye_api)
  ADD_EXECUTABLE(UEyeCalibration samples/UEyeCalibration.cpp)
  ADD_DEPENDENCIES(UEyeCalibration ${SRC} ${SRC_UEYE})
  TARGET_LINK_LIBRARIES(UEyeCalibration ${LIB} ${PROJECT_NAME}-ueye ueye_api)
  ADD_EXECUTABLE(StereoUEyeCapture samples/StereoUEyeCapture.cpp)
  ADD_DEPENDENCIES(StereoUEyeCapture ${SRC} ${SRC_UEYE})
  TARGET_LINK_LIBRARIES(StereoUEyeCapture ${LIB} ${PROJECT_NAME}-ueye ueye_api)
  ADD_EXECUTABLE(StereoUEyeGetCalibrationData samples/StereoUEyeGetCalibrationData.cpp)
  ADD_DEPENDENCIES(StereoUEyeGetCalibrationData ${SRC} ${SRC_UEYE})
  TARGET_LINK_LIBRARIES(StereoUEyeGetCalibrationData ${LIB} ${PROJECT_NAME}-ueye ueye_api)
  INSTALL(TARGETS ${PROJECT_NAME}-ueye LIBRARY DESTINATION lib)
ENDIF()
