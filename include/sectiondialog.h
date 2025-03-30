#ifndef SECTIONDIALOG_H
#define SECTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class SectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SectionDialog(QWidget *parent = nullptr);

    void setSectionName(const QString &name);
    void setQuestionsFile(const QString &file);
    void setAnswersFile(const QString &file);

    QString getSectionName() const;
    QString getQuestionsFile() const;
    QString getAnswersFile() const;

    void clear();

private slots:
    void onBrowseQuestions();
    void onBrowseAnswers();
    void onAccepted();

private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_questionsFileEdit;
    QLineEdit *m_answersFileEdit;
    QPushButton *m_browseQuestionsButton;
    QPushButton *m_browseAnswersButton;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
};

#endif // SECTIONDIALOG_H 