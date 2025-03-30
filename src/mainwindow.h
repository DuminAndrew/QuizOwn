#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QCheckBox>
#include "../include/quizmanager.h"
#include "../include/sectiondialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddSection();
    void onEditSection();
    void onRemoveSection();
    void onSectionSelected();
    void onStartMarathon();
    void onAnswerSubmitted();
    void onNextQuestion();
    void onPreviousQuestion();
    void onAbout();
    void onSettings();
    void onEndTestClicked();
    void onNextQuestionClicked();
    void onPreviousQuestionClicked();
    void showResults();

private:
    void setupConnections();
    void updateUI();
    void showError(const QString &message);
    void showInfo(const QString &message);
    void setupTestUI();

    QuizManager *m_quizManager;
    SectionDialog *m_sectionDialog;
    QButtonGroup *m_sectionButtonGroup;
    QButtonGroup *m_answerButtonGroup;
    QStackedWidget *m_stackedWidget;
    QWidget *m_sectionsContainer;
    QVBoxLayout *m_sectionsLayout;
    QPushButton *m_addSectionButton;
    QPushButton *m_editSectionButton;
    QPushButton *m_removeSectionButton;
    QPushButton *m_startMarathonButton;
    QLabel *m_questionLabel;
    QWidget *m_answersContainer;
    QVBoxLayout *m_answersLayout;
    QPushButton *m_submitButton;
    QPushButton *m_nextButton;
    QPushButton *m_prevButton;
    QPushButton *m_previousButton;
    QPushButton *m_endTestButton;
    QLabel *m_progressLabel;
    QLabel *m_scoreLabel;
    QCheckBox *m_marathonCheckBox;
};

#endif 