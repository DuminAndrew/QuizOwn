#include "quizsection.h"
#include "logger.h"
#include <QRegularExpression>

QuizSection::QuizSection(const QString& name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
    LOG_DEBUG(QString("Created new quiz section: %1").arg(name));
}

QuizSection::~QuizSection()
{
    LOG_DEBUG(QString("Destroyed quiz section: %1").arg(m_name));
}

bool QuizSection::loadFromFiles(const QString& questionsFile, const QString& answersFile)
{
    LOG_INFO(QString("Loading quiz section '%1' from files: %2, %3")
             .arg(m_name, questionsFile, answersFile));
    
    QVector<QString> questions;
    QVector<QString> answers;
    
    if (!readFile(questionsFile, questions) || !readFile(answersFile, answers)) {
        LOG_ERROR(QString("Failed to load files for section '%1'").arg(m_name));
        emit error("Failed to load quiz files");
        return false;
    }
    
    if (questions.size() != answers.size()) {
        LOG_ERROR(QString("Mismatch in questions and answers count for section '%1'")
                 .arg(m_name));
        emit error("Questions and answers count mismatch");
        return false;
    }
    
    m_questions.clear();
    for (int i = 0; i < questions.size(); ++i) {
        m_questions.append(qMakePair(questions[i], answers[i]));
    }
    
    LOG_INFO(QString("Successfully loaded %1 questions for section '%2'")
             .arg(m_questions.size())
             .arg(m_name));
    
    emit questionsChanged();
    return true;
}

bool QuizSection::saveToFiles(const QString& questionsFile, const QString& answersFile)
{
    LOG_INFO(QString("Saving quiz section '%1' to files: %2, %3")
             .arg(m_name, questionsFile, answersFile));
    
    QVector<QString> questions;
    QVector<QString> answers;
    
    for (const auto& pair : m_questions) {
        questions.append(pair.first);
        answers.append(pair.second);
    }
    
    if (!writeFile(questionsFile, questions) || !writeFile(answersFile, answers)) {
        LOG_ERROR(QString("Failed to save files for section '%1'").arg(m_name));
        emit error("Failed to save quiz files");
        return false;
    }
    
    LOG_INFO(QString("Successfully saved %1 questions for section '%2'")
             .arg(m_questions.size())
             .arg(m_name));
    
    return true;
}

QString QuizSection::getName() const
{
    return m_name;
}

void QuizSection::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(name);
        LOG_INFO(QString("Quiz section renamed to: %1").arg(name));
    }
}

int QuizSection::getQuestionCount() const
{
    return m_questions.size();
}

QString QuizSection::getQuestion(int index) const
{
    if (!validateIndex(index)) {
        return QString();
    }
    return m_questions[index].first;
}

QString QuizSection::getAnswer(int index) const
{
    if (!validateIndex(index)) {
        return QString();
    }
    return m_questions[index].second;
}

bool QuizSection::setQuestion(int index, const QString& question)
{
    if (!validateIndex(index)) {
        return false;
    }
    
    if (m_questions[index].first != question) {
        m_questions[index].first = question;
        emit questionsChanged();
        LOG_DEBUG(QString("Updated question %1 in section '%2'")
                 .arg(index + 1)
                 .arg(m_name));
    }
    
    return true;
}

bool QuizSection::setAnswer(int index, const QString& answer)
{
    if (!validateIndex(index)) {
        return false;
    }
    
    if (m_questions[index].second != answer) {
        m_questions[index].second = answer;
        emit questionsChanged();
        LOG_DEBUG(QString("Updated answer %1 in section '%2'")
                 .arg(index + 1)
                 .arg(m_name));
    }
    
    return true;
}

bool QuizSection::addQuestion(const QString& question, const QString& answer)
{
    m_questions.append(qMakePair(question, answer));
    emit questionsChanged();
    LOG_DEBUG(QString("Added new question to section '%1' (total: %2)")
             .arg(m_name)
             .arg(m_questions.size()));
    return true;
}

bool QuizSection::removeQuestion(int index)
{
    if (!validateIndex(index)) {
        return false;
    }
    
    m_questions.removeAt(index);
    emit questionsChanged();
    LOG_DEBUG(QString("Removed question %1 from section '%2' (total: %3)")
             .arg(index + 1)
             .arg(m_name)
             .arg(m_questions.size()));
    return true;
}

void QuizSection::clear()
{
    m_questions.clear();
    emit questionsChanged();
    LOG_INFO(QString("Cleared all questions from section '%1'").arg(m_name));
}

bool QuizSection::validateIndex(int index) const
{
    return index >= 0 && index < m_questions.size();
}

bool QuizSection::readFile(const QString& filename, QVector<QString>& lines)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to open file: %1").arg(filename));
        return false;
    }
    
    QTextStream in(&file);
    lines.clear();
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            lines.append(line);
        }
    }
    
    file.close();
    return true;
}

bool QuizSection::writeFile(const QString& filename, const QVector<QString>& lines)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_ERROR(QString("Failed to open file for writing: %1").arg(filename));
        return false;
    }
    
    QTextStream out(&file);
    for (const QString& line : lines) {
        out << line << "\n";
    }
    
    file.close();
    return true;
} 