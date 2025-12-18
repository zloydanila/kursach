#include "Config.h"
#include <QDir>
#include <QtGlobal>

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
    const QString env = qEnvironmentVariable("CHORUS_DB_HOST");
    if (!env.isEmpty()) return env;
    return m_settings->value("database/host", "turntable.proxy.rlwy.net").toString();
}

int Config::getDatabasePort() const
{
    const QString env = qEnvironmentVariable("CHORUS_DB_PORT");
    if (!env.isEmpty()) return env.toInt();
    return m_settings->value("database/port", 26533).toInt();
}

QString Config::getDatabaseName() const
{
    const QString env = qEnvironmentVariable("CHORUS_DB_NAME");
    if (!env.isEmpty()) return env;
    return m_settings->value("database/name", "railway").toString();
}

QString Config::getDatabaseUser() const
{
    const QString env = qEnvironmentVariable("CHORUS_DB_USER");
    if (!env.isEmpty()) return env;
    return m_settings->value("database/user", "postgres").toString();
}

QString Config::getDatabasePassword() const
{
    const QString env = qEnvironmentVariable("CHORUS_DB_PASSWORD");
    if (!env.isEmpty()) return env;
    return m_settings->value("database/password",
                             "CHANGE_ME").toString();
}

QString Config::getWebSocketUrl() const
{
    const QString env = qEnvironmentVariable("CHORUS_WS_URL");
    if (!env.isEmpty()) return env;
    return m_settings->value("network/websocket", "ws://localhost:8080").toString();
}

QString Config::getLastFmApiKey() const
{
    const QString env = qEnvironmentVariable("CHORUS_LASTFM_KEY");
    if (!env.isEmpty()) return env;
    return m_settings->value("api/lastfm_key",
                             "CHANGE_ME").toString();
}

QString Config::getLastFmSharedSecret() const
{
    const QString env = qEnvironmentVariable("CHORUS_LASTFM_SECRET");
    if (!env.isEmpty()) return env;
    return m_settings->value("api/lastfm_secret",
                             "CHANGE_ME").toString();
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
    const QString env = qEnvironmentVariable("CHORUS_YANDEX_CLIENT_ID");
    if (!env.isEmpty()) return env;
    return m_settings->value("api/yandex_client_id",
                             "CHANGE_ME").toString();
}

QString Config::getYandexClientSecret() const
{
    const QString env = qEnvironmentVariable("CHORUS_YANDEX_CLIENT_SECRET");
    if (!env.isEmpty()) return env;
    return m_settings->value("api/yandex_client_secret",
                             "CHANGE_ME").toString();
}

QString Config::getYandexRedirectUri() const
{
    const QString env = qEnvironmentVariable("CHORUS_YANDEX_REDIRECT_URI");
    if (!env.isEmpty()) return env;
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
