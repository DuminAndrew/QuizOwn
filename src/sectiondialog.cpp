#include "sectiondialog.h"
#include <QFileDialog>
#include <QMessageBox>

SectionDialog::SectionDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Добавить раздел"));
    setMinimumWidth(400);

    // Create layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Section name
    QHBoxLayout *nameLayout = new QHBoxLayout;
    QLabel *nameLabel = new QLabel(tr("Название:"), this);
    m_nameEdit = new QLineEdit(this);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(m_nameEdit);
    mainLayout->addLayout(nameLayout);

    // Questions file
    QHBoxLayout *questionsLayout = new QHBoxLayout;
    QLabel *questionsLabel = new QLabel(tr("Файл вопросов:"), this);
    m_questionsFileEdit = new QLineEdit(this);
    m_browseQuestionsButton = new QPushButton(tr("Обзор..."), this);
    questionsLayout->addWidget(questionsLabel);
    questionsLayout->addWidget(m_questionsFileEdit);
    questionsLayout->addWidget(m_browseQuestionsButton);
    mainLayout->addLayout(questionsLayout);

    // Answers file
    QHBoxLayout *answersLayout = new QHBoxLayout;
    QLabel *answersLabel = new QLabel(tr("Файл ответов:"), this);
    m_answersFileEdit = new QLineEdit(this);
    m_browseAnswersButton = new QPushButton(tr("Обзор..."), this);
    answersLayout->addWidget(answersLabel);
    answersLayout->addWidget(m_answersFileEdit);
    answersLayout->addWidget(m_browseAnswersButton);
    mainLayout->addLayout(answersLayout);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    m_okButton = new QPushButton(tr("OK"), this);
    m_cancelButton = new QPushButton(tr("Отмена"), this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(m_browseQuestionsButton, &QPushButton::clicked, this, &SectionDialog::onBrowseQuestions);
    connect(m_browseAnswersButton, &QPushButton::clicked, this, &SectionDialog::onBrowseAnswers);
    connect(m_okButton, &QPushButton::clicked, this, &SectionDialog::onAccepted);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void SectionDialog::setSectionName(const QString &name)
{
    m_nameEdit->setText(name);
}

void SectionDialog::setQuestionsFile(const QString &file)
{
    m_questionsFileEdit->setText(file);
}

void SectionDialog::setAnswersFile(const QString &file)
{
    m_answersFileEdit->setText(file);
}

QString SectionDialog::getSectionName() const
{
    return m_nameEdit->text().trimmed();
}

QString SectionDialog::getQuestionsFile() const
{
    return m_questionsFileEdit->text().trimmed();
}

QString SectionDialog::getAnswersFile() const
{
    return m_answersFileEdit->text().trimmed();
}

void SectionDialog::clear()
{
    m_nameEdit->clear();
    m_questionsFileEdit->clear();
    m_answersFileEdit->clear();
}

void SectionDialog::onBrowseQuestions()
{
    QString file = QFileDialog::getOpenFileName(this,
                                              tr("Выберите файл вопросов"),
                                              QString(),
                                              tr("Текстовые файлы (*.txt);;Все файлы (*.*)"));
    if (!file.isEmpty()) {
        m_questionsFileEdit->setText(file);
    }
}

void SectionDialog::onBrowseAnswers()
{
    QString file = QFileDialog::getOpenFileName(this,
                                              tr("Выберите файл ответов"),
                                              QString(),
                                              tr("Текстовые файлы (*.txt);;Все файлы (*.*)"));
    if (!file.isEmpty()) {
        m_answersFileEdit->setText(file);
    }
}

void SectionDialog::onAccepted()
{
    QString name = getSectionName();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Введите название раздела"));
        return;
    }

    QString questionsFile = getQuestionsFile();
    if (questionsFile.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Выберите файл вопросов"));
        return;
    }

    QString answersFile = getAnswersFile();
    if (answersFile.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Выберите файл ответов"));
        return;
    }

    accept();
} 