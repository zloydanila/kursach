cmake_minimum_required(VERSION 3.16)
project(Chorus VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    Network
    Sql
    WebSockets
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED taglib)

set(SOURCES
    src/main.cpp
    src/gui/AuthWindow/AuthWindow.cpp
    src/gui/AuthWindow/widgets/LoginForm.cpp
    src/gui/AuthWindow/widgets/RegisterForm.cpp

    src/gui/Frameless/FramelessWindow.cpp

    src/gui/MainWindow/MainWindow.cpp
    src/gui/MainWindow/MainWindow_UI.cpp
    src/gui/MainWindow/MainWindow_Pages.cpp
    src/gui/MainWindow/MainWindow_AvatarStorage.cpp
    src/gui/MainWindow/MainWindow_Slots.cpp

    src/gui/MainWindow/pages/FriendsPage.cpp
    src/gui/MainWindow/pages/MessagesPage.cpp
    src/gui/MainWindow/pages/SearchMusicPage.cpp
    src/gui/MainWindow/pages/MyMusicPage.cpp
    src/gui/MainWindow/pages/RoomsPage.cpp
    src/gui/MainWindow/pages/ProfilePage.cpp

    src/gui/widgets/ChatWidget.cpp
    src/gui/widgets/FriendCard.cpp
    src/gui/widgets/MessageBubble.cpp
    src/gui/widgets/RoomCard.cpp
    src/gui/widgets/TrackListItem.cpp
    src/gui/widgets/UserAvatar.cpp

    src/core/managers/TrackManager.cpp
    src/core/managers/PlaylistManager.cpp
    src/core/managers/UserManager.cpp

    src/database/DatabaseManager.cpp
    src/database/api/MusicAPIManager.cpp

    src/network/ChatManager.cpp
    src/network/FriendManager.cpp
    src/network/RoomManager.cpp
    src/network/WebSocketClient.cpp
    src/network/RadioBrowserAPI.cpp

    src/gui/dialogs/NotificationDialog.cpp

    src/audio/AudioPlayer.cpp
    src/audio/LocalFileHandler.cpp
    src/audio/PlaybackController.cpp
    src/audio/TrackWidget.cpp

    src/utils/Config.cpp
    src/utils/FileUtils.cpp
    src/utils/ImageUtils.cpp
)

set(HEADERS
    src/audio/AudioPlayer.h
    src/audio/LocalFileHandler.h
    src/audio/PlaybackController.h
    src/audio/TrackWidget.h

    src/core/managers/PlaylistManager.h
    src/core/managers/TrackManager.h
    src/core/managers/UserManager.h

    src/core/models/Message.h
    src/core/models/Playlist.h
    src/core/models/Room.h
    src/core/models/Track.h
    src/core/models/User.h

    src/database/DatabaseManager.h
    src/database/api/MusicAPIManager.h

    src/gui/AuthWindow/AuthWindow.h
    src/gui/AuthWindow/widgets/LoginForm.h
    src/gui/AuthWindow/widgets/RegisterForm.h

    src/gui/Frameless/FramelessWindow.h

    src/gui/MainWindow/MainWindow.h
    src/gui/MainWindow/pages/FriendsPage.h
    src/gui/MainWindow/pages/MessagesPage.h
    src/gui/MainWindow/pages/MyMusicPage.h
    src/gui/MainWindow/pages/RoomsPage.h
    src/gui/MainWindow/pages/SearchMusicPage.h
    src/gui/MainWindow/pages/ProfilePage.h

    src/gui/widgets/ChatWidget.h
    src/gui/widgets/FriendCard.h
    src/gui/widgets/MessageBubble.h
    src/gui/widgets/RoomCard.h
    src/gui/widgets/TrackListItem.h
    src/gui/widgets/UserAvatar.h

    src/gui/dialogs/NotificationDialog.h

    src/network/ChatManager.h
    src/network/FriendManager.h
    src/network/RoomManager.h
    src/network/WebSocketClient.h
    src/network/RadioBrowserAPI.h

    src/utils/Config.h
    src/utils/FileUtils.h
    src/utils/ImageUtils.h
)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${TAGLIB_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
    Qt5::Network
    Qt5::Sql
    Qt5::WebSockets
    ${TAGLIB_LIBRARIES}
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
