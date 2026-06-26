cmake_minimum_required(VERSION 3.6)

set(APP_NAME PlaneWar)
project(${APP_NAME})

set(COCOS2DX_ROOT_PATH "E:/coscos-2dx/cocos2d-x-4.0/cocos2d-x-4.0")
set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

include(CocosBuildSet)
add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)

set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp
     Classes/MainMenuScene.cpp
     Classes/GameScene.cpp
     Classes/Player.cpp
     Classes/Enemy.cpp
     Classes/Bullet.cpp
     Classes/PowerUp.cpp
     Classes/GameOverScene.cpp
     )
list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/MainMenuScene.h
     Classes/GameScene.h
     Classes/Player.h
     Classes/Enemy.h
     Classes/Bullet.h
     Classes/PowerUp.h
     Classes/GameOverScene.h
     )

if(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
endif()

set(all_code_files ${GAME_HEADER} ${GAME_SOURCE})

add_executable(${APP_NAME} ${all_code_files})

target_link_libraries(${APP_NAME} cocos2d)
target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)

setup_cocos_app_config(${APP_NAME})

if(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
