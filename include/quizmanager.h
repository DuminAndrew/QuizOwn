#ifndef QUIZMANAGER_H
#define QUIZMANAGER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>

class QuizManager : public QObject
{
    Q_OBJECT

public:
    struct Section {
        QString name;
        QString questionsFile;
        QString answersFile;
        QVector<QString> questions;
        QVector<QString> answers;
    };

    explicit QuizManager(QObject* parent = nullptr);
    ~QuizManager();

    bool addSection(const QString& name, const QString& questionsFile, const QString& answersFile);
    bool removeSection(const QString& name);
    bool editSection(const QString& oldName, const QString& newName, const QString& questionsFile, const QString& answersFile);
    QStringList getSectionNames() const;
    QString getSectionQuestionsFile(const QString& name) const;
    QString getSectionAnswersFile(const QString& name) const;
    const Section& getCurrentSection() const;

    bool startSectionTest(const QString& sectionName);
    bool startMarathon(const QStringList& sectionNames);
    bool checkAnswer(const QString& answer);
    bool checkMarathonAnswer(const QString& answer);
    bool nextQuestion();
    bool nextMarathonQuestion();
    bool previousQuestion();
    bool previousMarathonQuestion();
    bool goToQuestion(int index);
    bool goToMarathonQuestion(int index);
    bool endTest();
    QString getCurrentQuestion() const;
    QString getCurrentMarathonQuestion() const;
    QString getCurrentAnswer() const;
    QString getCurrentMarathonAnswer() const;
    QStringList getCurrentAnswers() const;
    QStringList getCurrentMarathonAnswers() const;
    int getCorrectAnswers() const;
    int getMarathonCorrectAnswers() const;
    int getCurrentQuestionIndex() const;
    int getCurrentMarathonQuestionIndex() const;
    int getTotalQuestions() const;
    int getTotalMarathonQuestions() const;
    QVector<int> getQuestionStatuses() const;
    QVector<int> getMarathonStatuses() const;
    QString getCurrentSectionName() const;
    QString getCurrentMarathonSectionName() const;
    int getCurrentSectionQuestionCount() const;
    int getCurrentMarathonSectionQuestionCount() const;

    bool isTestActive() const { return m_isTestActive; }
    bool isMarathonActive() const { return m_isMarathonActive; }

public slots:
    void resetTest();
    void resetMarathon();
    bool saveQuestions();

signals:
    void questionChanged(int index);
    void testStarted(const QString& sectionName);
    void testEnded(const QString& sectionName, int correctAnswers, int totalQuestions);
    void marathonStarted();
    void marathonEnded(int correctAnswers, int totalQuestions);
    void sectionAdded(const QString& name);
    void sectionRemoved(const QString& name);
    void sectionEdited(const QString& name);
    void answerChecked(bool correct);
    void error(const QString& message);

private:
    bool loadQuestionsFromFile(const QString& filePath, QVector<QString>& questions);
    bool loadAnswersFromFile(const QString& filePath, QVector<QString>& answers);
    void updateQuestionStatus(bool correct);
    void updateMarathonStatus(bool correct);

private:
    QMap<QString, Section> m_sections;
    QString m_currentSection;
    int m_currentQuestionIndex;
    int m_correctAnswers;
    QVector<int> m_questionStatuses;
    bool m_isTestActive;

    QString m_currentMarathonSection;
    int m_currentMarathonQuestionIndex;
    int m_marathonCorrectAnswers;
    QVector<int> m_marathonStatuses;
    bool m_isMarathonActive;
    QStringList m_marathonSections;
    int m_currentMarathonSectionIndex;
};

#endif // QUIZMANAGER_H 