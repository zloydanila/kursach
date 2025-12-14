#include "Config.h"
#include <QDir>

Config& Config::instance()
{
    static Config instance;
    return instance;
}

Config::Config()
{
    QString configPath = QDir::homePath() + "/.chorus";
    QDir().mkpath(configPath);
    m_settings = new QSettings(configPath + "/config.ini", QSettings::IniFormat);
}

Config::~Config()
{
    delete m_settings;
}

QString Config::getDatabaseHost() const
{
    return m_settings->value("database/host", "turntable.proxy.rlwy.net").toString();
}

int Config::getDatabasePort() const
{
    return m_settings->value("database/port", 26533).toInt();
}

QString Config::getDatabaseName() const
{
    return m_settings->value("database/name", "railway").toString();
}

QString Config::getDatabaseUser() const
{
    return m_settings->value("database/user", "postgres").toString();
}

QString Config::getDatabasePassword() const
{
    return m_settings->value("database/password",
                             "__REMOVED__").toString();
}

QString Config::getWebSocketUrl() const
{
    return m_settings->value("network/websocket", "ws://localhost:8080").toString();
}

QString Config::getLastFmApiKey() const
{
    return m_settings->value("api/lastfm_key",
                             "__REMOVED__").toString();
}

QString Config::getLastFmSharedSecret() const
{
    return m_settings->value("api/lastfm_secret",
                             "__REMOVED__").toString();
}

QString Config::getLastFmSessionKey() const
{
    return m_settings->value("api/lastfm_session_key", "").toString();
}

void Config::setLastFmSessionKey(const QString& key)
{
    m_settings->setValue("api/lastfm_session_key", key);
    m_settings->sync();
}

QString Config::getYandexClientId() const
{
    return m_settings->value("api/yandex_client_id",
                             "__REMOVED__").toString();
}

QString Config::getYandexClientSecret() const
{
    return m_settings->value("api/yandex_client_secret",
                             "__REMOVED__").toString();
}

QString Config::getYandexRedirectUri() const
{
    return m_settings->value("api/yandex_redirect_uri",
                             "http://localhost:8080/callback").toString();
}

QString Config::getYandexAccessToken() const
{
    return m_settings->value("api/yandex_access_token", "").toString();
}

void Config::setYandexAccessToken(const QString& token)
{
    m_settings->setValue("api/yandex_access_token", token);
    m_settings->sync();
}

int Config::getMaxCacheSize() const
{
    return m_settings->value("cache/max_size", 1024).toInt();
}

QString Config::getCachePath() const
{
    QString path = m_settings->value("cache/path",
                                     QDir::homePath() + "/.chorus/cache").toString();
    QDir().mkpath(path);
    return path;
}

QString Config::getAvatarsPath() const
{
    QString path = m_settings->value("avatars/path",
                                     QDir::homePath() + "/.chorus/avatars").toString();
    QDir().mkpath(path);
    return path;
}

bool Config::getAutoLogin() const
{
    return m_settings->value("auth/auto_login", false).toBool();
}

void Config::setAutoLogin(bool enabled)
{
    m_settings->setValue("auth/auto_login", enabled);
    m_settings->sync();
}

QString Config::getLastUsername() const
{
    return m_settings->value("auth/last_username", "").toString();
}

void Config::setLastUsername(const QString& username)
{
    m_settings->setValue("auth/last_username", username);
    m_settings->sync();
}

int Config::getDefaultVolume() const
{
    return m_settings->value("audio/volume", 70).toInt();
}

void Config::setDefaultVolume(int volume)
{
    m_settings->setValue("audio/volume", volume);
    m_settings->sync();
}
