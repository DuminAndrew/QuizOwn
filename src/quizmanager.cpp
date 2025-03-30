#include "../include/quizmanager.h"
#include "../include/logger.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QSettings>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QRandomGenerator>

QuizManager::QuizManager(QObject *parent)
    : QObject(parent)
    , m_isTestActive(false)
    , m_isMarathonActive(false)
    , m_currentQuestionIndex(0)
    , m_correctAnswers(0)
    , m_currentMarathonSectionIndex(0)
    , m_currentMarathonQuestionIndex(0)
    , m_marathonCorrectAnswers(0)
{
    // Load sections from file
    QFile file("sections.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject obj = doc.object();
        
        for (const QString& key : obj.keys()) {
            QJsonObject sectionObj = obj[key].toObject();
            Section section;
            section.name = key;
            section.questionsFile = sectionObj["questionsFile"].toString();
            section.answersFile = sectionObj["answersFile"].toString();
            
            if (loadQuestionsFromFile(section.questionsFile, section.questions) &&
                loadAnswersFromFile(section.answersFile, section.answers)) {
                m_sections[key] = section;
                qDebug() << "[INFO] Section loaded:" << key;
            } else {
                qDebug() << "[ERROR] Failed to load section:" << key;
            }
        }
        file.close();
    }
    LOG_INFO("QuizManager initialized");
}

QuizManager::~QuizManager()
{
    saveQuestions();
    LOG_INFO("QuizManager destroyed");
}

bool QuizManager::addSection(const QString& name, const QString& questionsFile, const QString& answersFile)
{
    if (m_sections.contains(name)) {
        LOG_ERROR("Section already exists: " + name);
        return false;
    }

    QVector<QString> questions;
    QVector<QString> answers;

    if (!loadQuestionsFromFile(questionsFile, questions) ||
        !loadAnswersFromFile(answersFile, answers)) {
        return false;
    }

    // Проверяем, что количество ответов в 4 раза больше количества вопросов
    if (answers.size() != questions.size() * 4) {
        LOG_ERROR("Questions and answers count mismatch for section: " + name);
        return false;
    }

    Section section;
    section.name = name;
    section.questionsFile = questionsFile;
    section.answersFile = answersFile;
    section.questions = questions;
    section.answers = answers;

    m_sections[name] = section;
    emit sectionAdded(name);
    return true;
}

bool QuizManager::removeSection(const QString &name)
{
    if (!m_sections.contains(name)) {
        qDebug() << "[ERROR] Section does not exist:" << name;
        return false;
    }

    m_sections.remove(name);
    emit sectionRemoved(name);
    saveQuestions();
    return true;
}

bool QuizManager::editSection(const QString &oldName, const QString &newName, const QString &questionsFile, const QString &answersFile)
{
    if (!m_sections.contains(oldName)) {
        qDebug() << "[ERROR] Section does not exist:" << oldName;
        return false;
    }

    if (oldName != newName && m_sections.contains(newName)) {
        qDebug() << "[ERROR] Section with new name already exists:" << newName;
        return false;
    }

    Section section = m_sections[oldName];
    section.name = newName;
    section.questionsFile = questionsFile;
    section.answersFile = answersFile;

    if (!loadQuestionsFromFile(questionsFile, section.questions)) {
        qDebug() << "[ERROR] Failed to load questions from file:" << questionsFile;
        return false;
    }

    if (!loadAnswersFromFile(answersFile, section.answers)) {
        qDebug() << "[ERROR] Failed to load answers from file:" << answersFile;
        return false;
    }

    if (section.questions.size() != section.answers.size()) {
        qDebug() << "[ERROR] Questions and answers count mismatch for section:" << newName;
        return false;
    }

    if (oldName != newName) {
        m_sections.remove(oldName);
    }
    m_sections[newName] = section;
    emit sectionEdited(newName);
    saveQuestions();
    return true;
}

QStringList QuizManager::getSectionNames() const
{
    return m_sections.keys();
}

bool QuizManager::startSectionTest(const QString &sectionName)
{
    if (!m_sections.contains(sectionName)) {
        LOG_ERROR("Section does not exist: " + sectionName);
        return false;
    }

    const Section& section = m_sections[sectionName];
    if (section.questions.isEmpty() || section.answers.isEmpty()) {
        LOG_ERROR("Section has no questions or answers");
        return false;
    }

    LOG_INFO("Starting test for section: " + sectionName);
    LOG_INFO("Questions count: " + QString::number(section.questions.size()));
    LOG_INFO("Answers count: " + QString::number(section.answers.size()));

    m_currentSection = sectionName;
    m_currentQuestionIndex = 0;
    m_correctAnswers = 0;
    m_isTestActive = true;

    LOG_INFO("Test started for section: " + sectionName);
    emit testStarted(sectionName);
    emit questionChanged(m_currentQuestionIndex);

    return true;
}

bool QuizManager::startMarathon(const QStringList &sections)
{
    if (sections.isEmpty()) {
        LOG_ERROR("No sections selected for marathon");
        return false;
    }

    for (const QString &section : sections) {
        if (!m_sections.contains(section)) {
            LOG_ERROR("Section does not exist: " + section);
            return false;
        }
    }

    m_marathonSections = sections;
    m_currentMarathonSectionIndex = 0;
    m_currentMarathonSection = sections.first();
    m_currentMarathonQuestionIndex = 0;
    m_marathonCorrectAnswers = 0;
    m_isMarathonActive = true;
    m_marathonStatuses.clear();

    // Подсчитываем общее количество вопросов
    int totalQuestions = 0;
    for (const QString& name : sections) {
        totalQuestions += m_sections[name].questions.size();
    }
    m_marathonStatuses.resize(totalQuestions);

    LOG_INFO("Starting marathon with sections: " + sections.join(", "));
    LOG_INFO("Total questions: " + QString::number(totalQuestions));

    emit marathonStarted();
    emit questionChanged(m_currentMarathonQuestionIndex);
    return true;
}

bool QuizManager::checkAnswer(const QString &answer)
{
    if (!m_isTestActive) {
        return false;
    }

    const Section& section = m_sections[m_currentSection];
    bool correct = (answer == section.answers[m_currentQuestionIndex]);
    updateQuestionStatus(correct);
    emit answerChecked(correct);
    return correct;
}

bool QuizManager::checkMarathonAnswer(const QString& answer)
{
    if (!m_isMarathonActive) {
        return false;
    }

    const Section& section = m_sections[m_currentMarathonSection];
    QString correctAnswer = getCurrentMarathonAnswer();
    bool correct = (answer == correctAnswer);
    updateMarathonStatus(correct);
    emit answerChecked(correct);
    return correct;
}

bool QuizManager::endTest()
{
    if (!m_isTestActive) {
        return false;
    }

    const Section& section = m_sections[m_currentSection];
    emit testEnded(m_currentSection, m_correctAnswers, section.questions.size());
    m_isTestActive = false;
    return true;
}

bool QuizManager::nextQuestion()
{
    if (!m_isTestActive) {
        return false;
    }

    const Section& section = m_sections[m_currentSection];
    if (m_currentQuestionIndex >= section.questions.size() - 1) {
        return false; // Не позволяем перейти к следующему вопросу на последнем
    }

    ++m_currentQuestionIndex;
    emit questionChanged(m_currentQuestionIndex);
    return true;
}

bool QuizManager::nextMarathonQuestion()
{
    if (!m_isMarathonActive) {
        return false;
    }

    const Section& section = m_sections[m_currentMarathonSection];
    if (m_currentMarathonQuestionIndex >= section.questions.size() - 1) {
        // Если это последний вопрос в текущем разделе
        if (m_currentMarathonSectionIndex >= m_marathonSections.size() - 1) {
            // Если это последний раздел, завершаем марафон
            emit marathonEnded(m_marathonCorrectAnswers, getTotalMarathonQuestions());
            return false;
        }
        // Переходим к следующему разделу
        ++m_currentMarathonSectionIndex;
        m_currentMarathonSection = m_marathonSections[m_currentMarathonSectionIndex];
        m_currentMarathonQuestionIndex = 0;
    } else {
        // Переходим к следующему вопросу в текущем разделе
        ++m_currentMarathonQuestionIndex;
    }

    LOG_INFO("Moving to next marathon question. Section: " + m_currentMarathonSection + 
             ", Question index: " + QString::number(m_currentMarathonQuestionIndex));
    emit questionChanged(m_currentMarathonQuestionIndex);
    return true;
}

bool QuizManager::previousQuestion()
{
    if (!m_isTestActive || m_currentQuestionIndex <= 0) {
        return false;
    }

    --m_currentQuestionIndex;
    emit questionChanged(m_currentQuestionIndex);
    return true;
}

bool QuizManager::previousMarathonQuestion()
{
    if (!m_isMarathonActive) {
        return false;
    }

    if (m_currentMarathonQuestionIndex > 0) {
        // Если есть предыдущий вопрос в текущем разделе
        --m_currentMarathonQuestionIndex;
    } else if (m_currentMarathonSectionIndex > 0) {
        // Если это первый вопрос, переходим к последнему вопросу предыдущего раздела
        --m_currentMarathonSectionIndex;
        m_currentMarathonSection = m_marathonSections[m_currentMarathonSectionIndex];
        m_currentMarathonQuestionIndex = m_sections[m_currentMarathonSection].questions.size() - 1;
    } else {
        // Если это первый вопрос первого раздела, ничего не делаем
        return false;
    }

    LOG_INFO("Moving to previous marathon question. Section: " + m_currentMarathonSection + 
             ", Question index: " + QString::number(m_currentMarathonQuestionIndex));
    emit questionChanged(m_currentMarathonQuestionIndex);
    return true;
}

bool QuizManager::goToQuestion(int index)
{
    if (!m_isTestActive) {
        return false;
    }

    const Section& section = m_sections[m_currentSection];
    if (index < 0 || index >= section.questions.size()) {
        return false;
    }

    m_currentQuestionIndex = index;
    emit questionChanged(m_currentQuestionIndex);
    return true;
}

bool QuizManager::goToMarathonQuestion(int index)
{
    if (!m_isMarathonActive) {
        return false;
    }

    if (index < 0 || index >= getTotalMarathonQuestions()) {
        return false;
    }

    int currentIndex = 0;
    for (int i = 0; i < m_marathonSections.size(); ++i) {
        const Section& section = m_sections[m_marathonSections[i]];
        if (currentIndex + section.questions.size() > index) {
            m_currentMarathonSectionIndex = i;
            m_currentMarathonSection = m_marathonSections[i];
            m_currentMarathonQuestionIndex = index - currentIndex;
            break;
        }
        currentIndex += section.questions.size();
    }

    emit questionChanged(m_currentMarathonQuestionIndex);
    return true;
}

QString QuizManager::getCurrentQuestion() const
{
    if (!m_isTestActive) {
        return QString();
    }
    return m_sections[m_currentSection].questions[m_currentQuestionIndex];
}

QString QuizManager::getCurrentMarathonQuestion() const
{
    if (!m_isMarathonActive) {
        return QString();
    }
    return m_sections[m_currentMarathonSection].questions[m_currentMarathonQuestionIndex];
}

QString QuizManager::getCurrentAnswer() const
{
    if (!m_isTestActive) {
        return QString();
    }
    return m_sections[m_currentSection].answers[m_currentQuestionIndex];
}

QString QuizManager::getCurrentMarathonAnswer() const
{
    if (!m_isMarathonActive) {
        return QString();
    }
    const Section& section = m_sections[m_currentMarathonSection];
    
    // Ищем ответ с маркером {ans} и правильным номером вопроса
    for (const QString& answer : section.answers) {
        if (answer.startsWith(QString::number(m_currentMarathonQuestionIndex + 1) + ".") && 
            answer.contains("{ans}")) {
            // Убираем номер вопроса, точку и маркер правильного ответа
            QString cleanAnswer = answer.mid(2);
            cleanAnswer = cleanAnswer.replace(" {ans}", "");
            return cleanAnswer;
        }
    }
    
    return QString();
}

QStringList QuizManager::getCurrentAnswers() const
{
    if (!m_isTestActive) {
        return QStringList();
    }
    const Section& section = m_sections[m_currentSection];
    QStringList answers;
    answers.append(section.answers[m_currentQuestionIndex]);
    
    // Добавляем случайные неправильные ответы
    QVector<int> usedIndices;
    usedIndices.append(m_currentQuestionIndex);
    
    while (answers.size() < 4) {
        int randomIndex = QRandomGenerator::global()->bounded(section.answers.size());
        if (!usedIndices.contains(randomIndex)) {
            answers.append(section.answers[randomIndex]);
            usedIndices.append(randomIndex);
        }
    }
    
    // Перемешиваем ответы
    for (int i = answers.size() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        answers.swapItemsAt(i, j);
    }
    
    return answers;
}

QStringList QuizManager::getCurrentMarathonAnswers() const
{
    if (!m_isMarathonActive) {
        return QStringList();
    }
    const Section& section = m_sections[m_currentMarathonSection];
    QStringList answers;
    
    // Получаем все варианты ответов для текущего вопроса
    for (int i = 0; i < section.answers.size(); ++i) {
        QString answer = section.answers[i];
        if (answer.startsWith(QString::number(m_currentMarathonQuestionIndex + 1) + ".")) {
            // Убираем номер вопроса, точку и маркер правильного ответа
            QString cleanAnswer = answer.mid(2);
            cleanAnswer = cleanAnswer.replace(" {ans}", "");
            answers.append(cleanAnswer);
        }
    }
    
    // Перемешиваем ответы
    for (int i = answers.size() - 1; i > 0; --i) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        answers.swapItemsAt(i, j);
    }
    
    return answers;
}

int QuizManager::getCorrectAnswers() const
{
    return m_correctAnswers;
}

int QuizManager::getMarathonCorrectAnswers() const
{
    return m_marathonCorrectAnswers;
}

int QuizManager::getCurrentQuestionIndex() const
{
    return m_currentQuestionIndex;
}

int QuizManager::getCurrentMarathonQuestionIndex() const
{
    if (!m_isMarathonActive) {
        return -1;
    }

    int index = 0;
    for (int i = 0; i < m_currentMarathonSectionIndex; ++i) {
        index += m_sections[m_marathonSections[i]].questions.size();
    }
    return index + m_currentMarathonQuestionIndex;
}

int QuizManager::getTotalQuestions() const
{
    if (!m_isTestActive) {
        return 0;
    }
    return m_sections[m_currentSection].questions.size();
}

int QuizManager::getTotalMarathonQuestions() const
{
    if (!m_isMarathonActive) {
        return 0;
    }

    int total = 0;
    for (const QString& name : m_marathonSections) {
        total += m_sections[name].questions.size();
    }
    return total;
}

QVector<int> QuizManager::getQuestionStatuses() const
{
    return m_questionStatuses;
}

QVector<int> QuizManager::getMarathonStatuses() const
{
    return m_marathonStatuses;
}

void QuizManager::resetTest()
{
    m_currentQuestionIndex = 0;
    m_correctAnswers = 0;
    m_questionStatuses.clear();
    if (m_isTestActive) {
        m_questionStatuses.resize(m_sections[m_currentSection].questions.size());
        emit questionChanged(m_currentQuestionIndex);
    }
}

void QuizManager::resetMarathon()
{
    m_currentMarathonSectionIndex = 0;
    m_currentMarathonSection = m_marathonSections.first();
    m_currentMarathonQuestionIndex = 0;
    m_marathonCorrectAnswers = 0;
    m_marathonStatuses.clear();
    if (m_isMarathonActive) {
        int totalQuestions = 0;
        for (const QString& name : m_marathonSections) {
            totalQuestions += m_sections[name].questions.size();
        }
        m_marathonStatuses.resize(totalQuestions);
        emit questionChanged(m_currentMarathonQuestionIndex);
    }
}

QString QuizManager::getCurrentSectionName() const
{
    return m_currentSection;
}

QString QuizManager::getCurrentMarathonSectionName() const
{
    return m_currentMarathonSection;
}

int QuizManager::getCurrentSectionQuestionCount() const
{
    if (m_currentSection.isEmpty()) {
        return 0;
    }
    return m_sections[m_currentSection].questions.size();
}

int QuizManager::getCurrentMarathonSectionQuestionCount() const
{
    if (m_currentMarathonSection.isEmpty()) {
        return 0;
    }
    return m_sections[m_currentMarathonSection].questions.size();
}

void QuizManager::updateQuestionStatus(bool correct)
{
    if (correct) {
        ++m_correctAnswers;
    }
    m_questionStatuses[m_currentQuestionIndex] = correct ? 1 : -1;
}

void QuizManager::updateMarathonStatus(bool correct)
{
    int globalIndex = getCurrentMarathonQuestionIndex();
    // Если вопрос еще не был отвечен (статус 0) и ответ правильный
    if (m_marathonStatuses[globalIndex] == 0 && correct) {
        ++m_marathonCorrectAnswers;
    }
    m_marathonStatuses[globalIndex] = correct ? 1 : -1;
}

bool QuizManager::loadQuestionsFromFile(const QString& filePath, QVector<QString>& questions)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR("Failed to open questions file: " + filePath);
        return false;
    }

    questions.clear();
    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine()).trimmed();
        if (!line.isEmpty()) {
            questions.append(line);
        }
    }

    LOG_INFO("Loaded " + QString::number(questions.size()) + " questions from " + filePath);
    file.close();
    return true;
}

bool QuizManager::loadAnswersFromFile(const QString& filePath, QVector<QString>& answers)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR("Failed to open answers file: " + filePath);
        return false;
    }

    answers.clear();
    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine()).trimmed();
        if (!line.isEmpty()) {
            answers.append(line);
        }
    }

    LOG_INFO("Loaded " + QString::number(answers.size()) + " answers from " + filePath);
    file.close();
    return true;
}

bool QuizManager::saveQuestions()
{
    QJsonObject obj;
    for (auto it = m_sections.constBegin(); it != m_sections.constEnd(); ++it) {
        QJsonObject sectionObj;
        sectionObj["questionsFile"] = it.value().questionsFile;
        sectionObj["answersFile"] = it.value().answersFile;
        obj[it.key()] = sectionObj;
    }

    QJsonDocument doc(obj);
    QFile file("sections.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "[ERROR] Failed to open sections.json for writing";
        return false;
    }

    file.write(doc.toJson());
    file.close();
    return true;
}

QString QuizManager::getSectionQuestionsFile(const QString& name) const
{
    if (!m_sections.contains(name)) {
        return QString();
    }
    return m_sections[name].questionsFile;
}

QString QuizManager::getSectionAnswersFile(const QString& name) const
{
    if (!m_sections.contains(name)) {
        return QString();
    }
    return m_sections[name].answersFile;
}

const QuizManager::Section& QuizManager::getCurrentSection() const
{
    static Section emptySection;
    if (!m_isTestActive || m_currentSection.isEmpty()) {
        return emptySection;
    }
    auto it = m_sections.find(m_currentSection);
    if (it == m_sections.end()) {
        return emptySection;
    }
    return it.value();
} 