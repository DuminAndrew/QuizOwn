#include "logger.h"
#include <QTextStream>
#include <QDir>

Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_logLevel(LogLevel::Info)
{
    QString logPath = QDir::currentPath() + "/quiz.log";
    m_logFile.setFileName(logPath);
    m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

Logger::~Logger()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

void Logger::setLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void Logger::log(LogLevel level, const QString& message)
{
    if (level < m_logLevel) return;

    if (!m_logFile.isOpen()) return;

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString logMessage = QString("[%1] [%2] %3\n")
                            .arg(timestamp)
                            .arg(levelToString(level))
                            .arg(message);

    QTextStream stream(&m_logFile);
    stream << logMessage;
    stream.flush();

    // Также выводим в консоль
    qDebug().noquote() << logMessage;
}

QString Logger::levelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
} 