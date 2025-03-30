#include "mainwindow.h"
#include "logger.h"
#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Инициализируем логгер
    Logger::getInstance();
    
    // Устанавливаем стиль приложения
    a.setStyle(QStyleFactory::create("Fusion"));
    
    // Загружаем и применяем стили
    QFile styleFile(":/styles/styles.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream styleStream(&styleFile);
        a.setStyleSheet(styleStream.readAll());
        styleFile.close();
        LOG_INFO("Styles loaded successfully");
    } else {
        LOG_ERROR("Failed to load styles");
    }
    
    // Устанавливаем информацию о приложении
    QApplication::setApplicationName("QuizOwn");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("QuizOwn");
    QApplication::setOrganizationDomain("quizown.com");
    
    MainWindow w;
    w.show();
    
    LOG_INFO("Application started");
    
    return a.exec();
} 