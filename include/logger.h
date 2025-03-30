#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QDateTime>
#include <QDebug>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger
{
public:
    static Logger& getInstance();
    ~Logger();
    void setLogLevel(LogLevel level);
    void log(LogLevel level, const QString& message);

    void debug(const QString& message) { log(LogLevel::Debug, message); }
    void info(const QString& message) { log(LogLevel::Info, message); }
    void warning(const QString& message) { log(LogLevel::Warning, message); }
    void error(const QString& message) { log(LogLevel::Error, message); }

private:
    explicit Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    QString levelToString(LogLevel level);
    
    LogLevel m_logLevel;
    QFile m_logFile;
};

#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)

#endif // LOGGER_H 