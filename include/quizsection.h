#ifndef QUIZOWN_QUIZSECTION_H
#define QUIZOWN_QUIZSECTION_H

#include <QString>
#include <QVector>
#include <QPair>
#include <QFile>
#include <QTextStream>
#include <QObject>

class QuizSection : public QObject
{
    Q_OBJECT

public:
    explicit QuizSection(const QString& name, QObject *parent = nullptr);
    ~QuizSection();

    bool loadFromFiles(const QString& questionsFile, const QString& answersFile);
    bool saveToFiles(const QString& questionsFile, const QString& answersFile);
    
    QString getName() const;
    void setName(const QString& name);
    
    int getQuestionCount() const;
    QString getQuestion(int index) const;
    QString getAnswer(int index) const;
    
    bool setQuestion(int index, const QString& question);
    bool setAnswer(int index, const QString& answer);
    
    bool addQuestion(const QString& question, const QString& answer);
    bool removeQuestion(int index);
    
    void clear();

signals:
    void nameChanged(const QString& newName);
    void questionsChanged();
    void error(const QString& message);

private:
    QString m_name;
    QVector<QPair<QString, QString>> m_questions;
    
    bool validateIndex(int index) const;
    bool readFile(const QString& filename, QVector<QString>& lines);
    bool writeFile(const QString& filename, const QVector<QString>& lines);
};

#endif // QUIZOWN_QUIZSECTION_H 