/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/gui/MainWindow/MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[45];
    char stringdata0[625];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 15), // "showProfilePage"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 16), // "showMessagesPage"
QT_MOC_LITERAL(4, 45, 15), // "showFriendsPage"
QT_MOC_LITERAL(5, 61, 21), // "showNotificationsPage"
QT_MOC_LITERAL(6, 83, 16), // "showPlaylistPage"
QT_MOC_LITERAL(7, 100, 11), // "searchMusic"
QT_MOC_LITERAL(8, 112, 13), // "showTopTracks"
QT_MOC_LITERAL(9, 126, 13), // "onTracksFound"
QT_MOC_LITERAL(10, 140, 6), // "tracks"
QT_MOC_LITERAL(11, 147, 14), // "onNetworkError"
QT_MOC_LITERAL(12, 162, 5), // "error"
QT_MOC_LITERAL(13, 168, 26), // "addSelectedTrackToPlaylist"
QT_MOC_LITERAL(14, 195, 17), // "playSelectedTrack"
QT_MOC_LITERAL(15, 213, 13), // "pausePlayback"
QT_MOC_LITERAL(16, 227, 12), // "stopPlayback"
QT_MOC_LITERAL(17, 240, 22), // "onPlaybackStateChanged"
QT_MOC_LITERAL(18, 263, 19), // "QMediaPlayer::State"
QT_MOC_LITERAL(19, 283, 5), // "state"
QT_MOC_LITERAL(20, 289, 17), // "onPositionChanged"
QT_MOC_LITERAL(21, 307, 8), // "position"
QT_MOC_LITERAL(22, 316, 17), // "onDurationChanged"
QT_MOC_LITERAL(23, 334, 8), // "duration"
QT_MOC_LITERAL(24, 343, 11), // "searchUsers"
QT_MOC_LITERAL(25, 355, 12), // "onUsersFound"
QT_MOC_LITERAL(26, 368, 18), // "QList<QVariantMap>"
QT_MOC_LITERAL(27, 387, 5), // "users"
QT_MOC_LITERAL(28, 393, 17), // "sendFriendRequest"
QT_MOC_LITERAL(29, 411, 6), // "userId"
QT_MOC_LITERAL(30, 418, 19), // "acceptFriendRequest"
QT_MOC_LITERAL(31, 438, 19), // "rejectFriendRequest"
QT_MOC_LITERAL(32, 458, 12), // "removeFriend"
QT_MOC_LITERAL(33, 471, 8), // "friendId"
QT_MOC_LITERAL(34, 480, 11), // "loadFriends"
QT_MOC_LITERAL(35, 492, 18), // "loadFriendRequests"
QT_MOC_LITERAL(36, 511, 18), // "refreshFriendsData"
QT_MOC_LITERAL(37, 530, 14), // "refreshDialogs"
QT_MOC_LITERAL(38, 545, 8), // "openChat"
QT_MOC_LITERAL(39, 554, 11), // "sendMessage"
QT_MOC_LITERAL(40, 566, 20), // "onNewMessageReceived"
QT_MOC_LITERAL(41, 587, 10), // "fromUserId"
QT_MOC_LITERAL(42, 598, 8), // "username"
QT_MOC_LITERAL(43, 607, 7), // "message"
QT_MOC_LITERAL(44, 615, 9) // "timestamp"

    },
    "MainWindow\0showProfilePage\0\0"
    "showMessagesPage\0showFriendsPage\0"
    "showNotificationsPage\0showPlaylistPage\0"
    "searchMusic\0showTopTracks\0onTracksFound\0"
    "tracks\0onNetworkError\0error\0"
    "addSelectedTrackToPlaylist\0playSelectedTrack\0"
    "pausePlayback\0stopPlayback\0"
    "onPlaybackStateChanged\0QMediaPlayer::State\0"
    "state\0onPositionChanged\0position\0"
    "onDurationChanged\0duration\0searchUsers\0"
    "onUsersFound\0QList<QVariantMap>\0users\0"
    "sendFriendRequest\0userId\0acceptFriendRequest\0"
    "rejectFriendRequest\0removeFriend\0"
    "friendId\0loadFriends\0loadFriendRequests\0"
    "refreshFriendsData\0refreshDialogs\0"
    "openChat\0sendMessage\0onNewMessageReceived\0"
    "fromUserId\0username\0message\0timestamp"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  159,    2, 0x08 /* Private */,
       3,    0,  160,    2, 0x08 /* Private */,
       4,    0,  161,    2, 0x08 /* Private */,
       5,    0,  162,    2, 0x08 /* Private */,
       6,    0,  163,    2, 0x08 /* Private */,
       7,    0,  164,    2, 0x08 /* Private */,
       8,    0,  165,    2, 0x08 /* Private */,
       9,    1,  166,    2, 0x08 /* Private */,
      11,    1,  169,    2, 0x08 /* Private */,
      13,    0,  172,    2, 0x08 /* Private */,
      14,    0,  173,    2, 0x08 /* Private */,
      15,    0,  174,    2, 0x08 /* Private */,
      16,    0,  175,    2, 0x08 /* Private */,
      17,    1,  176,    2, 0x08 /* Private */,
      20,    1,  179,    2, 0x08 /* Private */,
      22,    1,  182,    2, 0x08 /* Private */,
      24,    0,  185,    2, 0x08 /* Private */,
      25,    1,  186,    2, 0x08 /* Private */,
      28,    1,  189,    2, 0x08 /* Private */,
      30,    1,  192,    2, 0x08 /* Private */,
      31,    1,  195,    2, 0x08 /* Private */,
      32,    1,  198,    2, 0x08 /* Private */,
      34,    0,  201,    2, 0x08 /* Private */,
      35,    0,  202,    2, 0x08 /* Private */,
      36,    0,  203,    2, 0x08 /* Private */,
      37,    0,  204,    2, 0x08 /* Private */,
      38,    1,  205,    2, 0x08 /* Private */,
      39,    0,  208,    2, 0x08 /* Private */,
      40,    4,  209,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QVariantList,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void, QMetaType::LongLong,   21,
    QMetaType::Void, QMetaType::LongLong,   23,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 26,   27,
    QMetaType::Void, QMetaType::Int,   29,
    QMetaType::Void, QMetaType::Int,   29,
    QMetaType::Void, QMetaType::Int,   29,
    QMetaType::Void, QMetaType::Int,   33,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   33,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, QMetaType::QDateTime,   41,   42,   43,   44,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->showProfilePage(); break;
        case 1: _t->showMessagesPage(); break;
        case 2: _t->showFriendsPage(); break;
        case 3: _t->showNotificationsPage(); break;
        case 4: _t->showPlaylistPage(); break;
        case 5: _t->searchMusic(); break;
        case 6: _t->showTopTracks(); break;
        case 7: _t->onTracksFound((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 8: _t->onNetworkError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->addSelectedTrackToPlaylist(); break;
        case 10: _t->playSelectedTrack(); break;
        case 11: _t->pausePlayback(); break;
        case 12: _t->stopPlayback(); break;
        case 13: _t->onPlaybackStateChanged((*reinterpret_cast< QMediaPlayer::State(*)>(_a[1]))); break;
        case 14: _t->onPositionChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 15: _t->onDurationChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 16: _t->searchUsers(); break;
        case 17: _t->onUsersFound((*reinterpret_cast< const QList<QVariantMap>(*)>(_a[1]))); break;
        case 18: _t->sendFriendRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->acceptFriendRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->rejectFriendRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->removeFriend((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->loadFriends(); break;
        case 23: _t->loadFriendRequests(); break;
        case 24: _t->refreshFriendsData(); break;
        case 25: _t->refreshDialogs(); break;
        case 26: _t->openChat((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: _t->sendMessage(); break;
        case 28: _t->onNewMessageReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QDateTime(*)>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QMediaPlayer::State >(); break;
            }
            break;
        case 17:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QVariantMap> >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
