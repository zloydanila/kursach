#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QSettings>

class Config
{
public:
    static Config& instance();

    QString getDatabaseHost() const;
    int getDatabasePort() const;
    QString getDatabaseName() const;
    QString getDatabaseUser() const;
    QString getDatabasePassword() const;

    QString getWebSocketUrl() const;

    QString getLastFmApiKey() const;
    QString getLastFmSharedSecret() const;
    QString getLastFmSessionKey() const;
    void setLastFmSessionKey(const QString& key);

    QString getYandexClientId() const;
    QString getYandexClientSecret() const;
    QString getYandexRedirectUri() const;
    QString getYandexAccessToken() const;
    void setYandexAccessToken(const QString& token);

    int getMaxCacheSize() const;
    QString getCachePath() const;

    // эти методы у тебя объявлены, но их реализация в другом месте проекта
    QString getAvatarsPath() const;
    bool getAutoLogin() const;
    void setAutoLogin(bool enabled);
    QString getLastUsername() const;
    void setLastUsername(const QString& username);
    int getDefaultVolume() const;
    void setDefaultVolume(int volume);

private:
    Config();
    ~Config();
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    QSettings* m_settings = nullptr;
};

#endif
