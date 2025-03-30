#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QCheckBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QButtonGroup>
#include "quizmanager.h"
#include "sectiondialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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

private:
    void setupConnections();
    void updateUI();
    void showError(const QString &message);
    void showInfo(const QString &message);

    QuizManager *m_quizManager;
    SectionDialog *m_sectionDialog;
    QButtonGroup *m_sectionButtonGroup;
    QButtonGroup *m_answerButtonGroup;
    QWidget *m_sectionsContainer;
    QVBoxLayout *m_sectionsLayout;
    QVBoxLayout *m_answersLayout;
    QPushButton *m_addSectionButton;
    QPushButton *m_editSectionButton;
    QPushButton *m_removeSectionButton;
    QPushButton *m_startMarathonButton;
    QPushButton *m_submitButton;
    QPushButton *m_nextButton;
    QPushButton *m_previousButton;
    QLabel *m_questionLabel;
    QLabel *m_progressLabel;
    QLabel *m_scoreLabel;
    QCheckBox *m_marathonCheckBox;
    QStackedWidget *m_stackedWidget;
};

#endif // MAINWINDOW_H 