#include "mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QButtonGroup>
#include <QListWidget>
#include <QListWidgetItem>
#include <QRadioButton>
#include "logger.h"
#include <QIcon>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_quizManager(new QuizManager(this))
    , m_sectionDialog(new SectionDialog(this))
    , m_sectionButtonGroup(new QButtonGroup(this))
    , m_answerButtonGroup(new QButtonGroup(this))
{
    setWindowTitle(tr("Quiz Own"));
    resize(800, 600);

    // Загрузка стилей
    QFile styleFile(":/styles/main.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString style = QLatin1String(styleFile.readAll());
        setStyleSheet(style);
        styleFile.close();
    }

    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    // Create main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Create left panel
    QWidget *leftPanel = new QWidget;
    leftPanel->setObjectName("leftPanel");
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(leftPanel);
    leftPanel->setMinimumWidth(250);
    leftPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // Sections label
    QLabel *sectionsLabel = new QLabel(tr("Разделы:"), this);

    // Sections container
    m_sectionsContainer = new QWidget(this);
    m_sectionsLayout = new QVBoxLayout(m_sectionsContainer);
    m_sectionsLayout->setSpacing(5);
    m_sectionsLayout->setContentsMargins(0, 0, 0, 0);

    // Section management buttons
    QHBoxLayout *sectionButtonsLayout = new QHBoxLayout;
    m_addSectionButton = new QPushButton(tr("Добавить"), this);
    m_addSectionButton->setIcon(QIcon(":/icons/add.png"));
    m_editSectionButton = new QPushButton(tr("Редактировать"), this);
    m_editSectionButton->setIcon(QIcon(":/icons/edit.png"));
    m_removeSectionButton = new QPushButton(tr("Удалить"), this);
    m_removeSectionButton->setIcon(QIcon(":/icons/remove.png"));
    m_editSectionButton->setEnabled(false);
    m_removeSectionButton->setEnabled(false);

    sectionButtonsLayout->addWidget(m_addSectionButton);
    sectionButtonsLayout->addWidget(m_editSectionButton);
    sectionButtonsLayout->addWidget(m_removeSectionButton);

    // Marathon button
    m_startMarathonButton = new QPushButton(tr("Начать марафон"), this);
    m_startMarathonButton->setIcon(QIcon(":/icons/marathon.png"));
    m_startMarathonButton->setEnabled(true);

    // Add widgets to left panel
    leftPanelLayout->addWidget(sectionsLabel);
    leftPanelLayout->addWidget(m_sectionsContainer);
    leftPanelLayout->addLayout(sectionButtonsLayout);
    leftPanelLayout->addWidget(m_startMarathonButton);
    leftPanelLayout->addStretch();

    // Create right panel (stacked widget)
    m_stackedWidget = new QStackedWidget(this);

    // Welcome page
    QWidget *welcomePage = new QWidget;
    QVBoxLayout *welcomePageLayout = new QVBoxLayout(welcomePage);
    QLabel *welcomeLabel = new QLabel(tr("Добро пожаловать в Quiz Own!\n\n"
                                       "Выберите разделы для начала марафона."), this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomePageLayout->addWidget(welcomeLabel);
    welcomePageLayout->addStretch();

    // Marathon page
    QWidget *marathonPage = new QWidget;
    QVBoxLayout *marathonPageLayout = new QVBoxLayout(marathonPage);
    m_marathonCheckBox = new QCheckBox(tr("Показывать правильные ответы"), this);
    m_marathonCheckBox->setChecked(true);

    m_questionLabel = new QLabel(this);
    m_questionLabel->setObjectName("questionLabel");
    m_questionLabel->setWordWrap(true);
    
    // Answers container
    QWidget *answersContainer = new QWidget(this);
    m_answersLayout = new QVBoxLayout(answersContainer);
    m_answersLayout->setSpacing(10);
    
    m_submitButton = new QPushButton(tr("Ответить"), this);
    m_submitButton->setIcon(QIcon(":/icons/submit.png"));
    m_nextButton = new QPushButton(tr("Следующий"), this);
    m_nextButton->setIcon(QIcon(":/icons/next.png"));
    m_previousButton = new QPushButton(tr("Предыдущий"), this);
    m_previousButton->setIcon(QIcon(":/icons/previous.png"));
    m_progressLabel = new QLabel(this);
    m_scoreLabel = new QLabel(this);

    QHBoxLayout *marathonButtonsLayout = new QHBoxLayout;
    marathonButtonsLayout->addWidget(m_previousButton);
    marathonButtonsLayout->addWidget(m_submitButton);
    marathonButtonsLayout->addWidget(m_nextButton);

    marathonPageLayout->addWidget(m_marathonCheckBox);
    marathonPageLayout->addWidget(m_questionLabel);
    marathonPageLayout->addWidget(answersContainer);
    marathonPageLayout->addLayout(marathonButtonsLayout);
    marathonPageLayout->addWidget(m_progressLabel);
    marathonPageLayout->addWidget(m_scoreLabel);
    marathonPageLayout->addStretch();

    // Add pages to stacked widget
    m_stackedWidget->addWidget(welcomePage);
    m_stackedWidget->addWidget(marathonPage);

    // Add panels to main layout
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(m_stackedWidget);

    // Create menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu(tr("Файл"));
    QAction *exitAction = fileMenu->addAction(tr("Выход"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    QMenu *helpMenu = menuBar->addMenu(tr("Справка"));
    QAction *aboutAction = helpMenu->addAction(tr("О программе"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);

    // Create status bar
    QStatusBar *statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    // Setup connections
    setupConnections();

    // Update UI
    updateUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupConnections()
{
    // Section management
    connect(m_addSectionButton, &QPushButton::clicked, this, &MainWindow::onAddSection);
    connect(m_editSectionButton, &QPushButton::clicked, this, &MainWindow::onEditSection);
    connect(m_removeSectionButton, &QPushButton::clicked, this, &MainWindow::onRemoveSection);
    connect(m_sectionButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            this, &MainWindow::onSectionSelected);
    connect(m_startMarathonButton, &QPushButton::clicked, this, &MainWindow::onStartMarathon);

    // Marathon
    connect(m_submitButton, &QPushButton::clicked, this, &MainWindow::onAnswerSubmitted);
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNextQuestion);
    connect(m_previousButton, &QPushButton::clicked, this, &MainWindow::onPreviousQuestion);

    // QuizManager signals
    connect(m_quizManager, &QuizManager::marathonStarted, this, [this]() {
        LOG_INFO("Marathon started");
        m_stackedWidget->setCurrentIndex(1);
        updateUI();
    });

    connect(m_quizManager, &QuizManager::marathonEnded, this, [this](int correct, int total) {
        LOG_INFO("Marathon ended, correct: " + QString::number(correct) + ", total: " + QString::number(total));
        showInfo(tr("Марафон завершен!\nПравильных ответов: %1 из %2").arg(correct).arg(total));
        m_stackedWidget->setCurrentIndex(0);
        updateUI();
    });

    connect(m_quizManager, &QuizManager::questionChanged, this, [this](int index) {
        LOG_INFO("Question changed to index: " + QString::number(index));
        updateUI();
    });

    connect(m_quizManager, &QuizManager::answerChecked, this, [this](bool correct) {
        LOG_INFO("Answer checked, correct: " + QString::number(correct));
    });

    connect(m_quizManager, &QuizManager::sectionAdded, this, [this](const QString &name) {
        LOG_INFO("Section added: " + name);
        updateUI();
    });

    connect(m_quizManager, &QuizManager::sectionRemoved, this, [this](const QString &name) {
        LOG_INFO("Section removed: " + name);
        updateUI();
    });

    connect(m_quizManager, &QuizManager::sectionEdited, this, [this](const QString &name) {
        LOG_INFO("Section edited: " + name);
        updateUI();
    });

    connect(m_quizManager, &QuizManager::error, this, &MainWindow::showError);
}

void MainWindow::updateUI()
{
    LOG_INFO("Starting updateUI");
    
    // Обновляем список разделов
    QLayoutItem *child;
    while ((child = m_sectionsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }
    
    QStringList sections = m_quizManager->getSectionNames();
    LOG_INFO("Sections count: " + QString::number(sections.size()));
    
    for (const QString &section : sections) {
        QPushButton *button = new QPushButton(section, this);
        button->setObjectName("sectionButton");
        button->setCheckable(true);
        m_sectionButtonGroup->addButton(button);
        m_sectionsLayout->addWidget(button);
    }
    
    // Обновляем состояние кнопок разделов
    m_editSectionButton->setEnabled(!sections.isEmpty());
    m_removeSectionButton->setEnabled(!sections.isEmpty());
    
    // Обновляем состояние кнопки марафона
    m_startMarathonButton->setEnabled(!sections.isEmpty());
    
    if (m_quizManager->isMarathonActive()) {
        LOG_INFO("Marathon is active, updating marathon UI");
        QString question = m_quizManager->getCurrentMarathonQuestion();
        LOG_INFO("Current marathon question: " + question);
        
        if (question.isEmpty()) {
            LOG_ERROR("Empty marathon question received");
            return;
        }
        
        m_questionLabel->setText(question);
        LOG_INFO("Question label updated");
        
        // Clear existing answer buttons
        QLayoutItem *answerChild;
        while ((answerChild = m_answersLayout->takeAt(0)) != nullptr) {
            if (answerChild->widget()) {
                answerChild->widget()->deleteLater();
            }
            delete answerChild;
        }
        LOG_INFO("Cleared existing answer buttons");

        // Create new answer buttons
        QStringList answers = m_quizManager->getCurrentMarathonAnswers();
        LOG_INFO("Marathon answers count: " + QString::number(answers.size()));
        for (const QString &answer : answers) {
            QRadioButton *button = new QRadioButton(answer, this);
            button->setEnabled(true); // Включаем кнопку
            button->setStyleSheet(""); // Сбрасываем стиль
            m_answerButtonGroup->addButton(button);
            m_answersLayout->addWidget(button);
        }
        LOG_INFO("Created new marathon answer buttons");
        
        m_progressLabel->setText(tr("Вопрос %1 из %2")
                               .arg(m_quizManager->getCurrentMarathonQuestionIndex() + 1)
                               .arg(m_quizManager->getTotalMarathonQuestions()));
        LOG_INFO("Progress label updated");
        
        m_scoreLabel->setText(tr("Правильных ответов: %1")
                            .arg(m_quizManager->getMarathonCorrectAnswers()));
        LOG_INFO("Score label updated");

        // Включаем кнопку отправки ответа
        m_submitButton->setEnabled(true);
    } else {
        LOG_INFO("Marathon is not active");
    }
    
    LOG_INFO("updateUI completed");
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, tr("Ошибка"), message);
}

void MainWindow::showInfo(const QString &message)
{
    QMessageBox::information(this, tr("Информация"), message);
}

void MainWindow::onAddSection()
{
    m_sectionDialog->setWindowTitle(tr("Добавить раздел"));
    m_sectionDialog->clear();

    if (m_sectionDialog->exec() == QDialog::Accepted) {
        QString name = m_sectionDialog->getSectionName();
        QString questionsFile = m_sectionDialog->getQuestionsFile();
        QString answersFile = m_sectionDialog->getAnswersFile();

        if (m_quizManager->addSection(name, questionsFile, answersFile)) {
            showInfo(tr("Раздел успешно добавлен"));
            updateUI();
        }
    }
}

void MainWindow::onEditSection()
{
    QAbstractButton *button = m_sectionButtonGroup->checkedButton();
    if (!button) {
        return;
    }

    QString oldName = button->text();
    m_sectionDialog->setWindowTitle(tr("Редактировать раздел"));
    m_sectionDialog->setSectionName(oldName);
    m_sectionDialog->setQuestionsFile(m_quizManager->getSectionQuestionsFile(oldName));
    m_sectionDialog->setAnswersFile(m_quizManager->getSectionAnswersFile(oldName));

    if (m_sectionDialog->exec() == QDialog::Accepted) {
        QString newName = m_sectionDialog->getSectionName();
        QString questionsFile = m_sectionDialog->getQuestionsFile();
        QString answersFile = m_sectionDialog->getAnswersFile();

        if (m_quizManager->editSection(oldName, newName, questionsFile, answersFile)) {
            showInfo(tr("Раздел успешно отредактирован"));
            updateUI();
        }
    }
}

void MainWindow::onRemoveSection()
{
    QAbstractButton *button = m_sectionButtonGroup->checkedButton();
    if (!button) {
        return;
    }

    QString name = button->text();
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Подтверждение"),
        tr("Вы уверены, что хотите удалить раздел \"%1\"?").arg(name),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (m_quizManager->removeSection(name)) {
            showInfo(tr("Раздел успешно удален"));
            updateUI();
        }
    }
}

void MainWindow::onSectionSelected()
{
    QAbstractButton *button = m_sectionButtonGroup->checkedButton();
    if (button) {
        m_editSectionButton->setEnabled(true);
        m_removeSectionButton->setEnabled(true);
    } else {
        m_editSectionButton->setEnabled(false);
        m_removeSectionButton->setEnabled(false);
    }
}

void MainWindow::onStartMarathon()
{
    QStringList sections = m_quizManager->getSectionNames();
    if (sections.isEmpty()) {
        showError(tr("Нет доступных разделов для марафона"));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Выберите разделы для марафона"));
    dialog.setMinimumWidth(300);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QListWidget *listWidget = new QListWidget(&dialog);
    listWidget->setSelectionMode(QListWidget::MultiSelection);
    
    for (const QString &section : sections) {
        QListWidgetItem *item = new QListWidgetItem(section);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::CheckState::Checked);
        listWidget->addItem(item);
    }

    QPushButton *okButton = new QPushButton(tr("Начать"), &dialog);
    QPushButton *cancelButton = new QPushButton(tr("Отмена"), &dialog);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(listWidget);
    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QStringList selectedSections;
        for (int i = 0; i < listWidget->count(); ++i) {
            QListWidgetItem *item = listWidget->item(i);
            if (item->checkState() == Qt::CheckState::Checked) {
                selectedSections.append(item->text());
            }
        }

        if (selectedSections.isEmpty()) {
            showError(tr("Выберите хотя бы один раздел для марафона"));
            return;
        }

        LOG_INFO("Starting marathon with sections: " + selectedSections.join(", "));
        if (m_quizManager->startMarathon(selectedSections)) {
            LOG_INFO("Marathon started successfully");
            
            // Очищаем предыдущие ответы
            QLayoutItem *child;
            while ((child = m_answersLayout->takeAt(0)) != nullptr) {
                if (child->widget()) {
                    child->widget()->deleteLater();
                }
                delete child;
            }
            
            // Создаем новые кнопки ответов
            QStringList answers = m_quizManager->getCurrentMarathonAnswers();
            for (const QString &answer : answers) {
                QRadioButton *button = new QRadioButton(answer, this);
                m_answerButtonGroup->addButton(button);
                m_answersLayout->addWidget(button);
            }
            
            // Обновляем вопрос и прогресс
            m_questionLabel->setText(m_quizManager->getCurrentMarathonQuestion());
            m_progressLabel->setText(tr("Вопрос %1 из %2")
                                   .arg(m_quizManager->getCurrentMarathonQuestionIndex() + 1)
                                   .arg(m_quizManager->getTotalMarathonQuestions()));
            m_scoreLabel->setText(tr("Правильных ответов: %1")
                                .arg(m_quizManager->getMarathonCorrectAnswers()));
            
            // Переключаемся на страницу марафона
            m_stackedWidget->setCurrentIndex(1);
        } else {
            LOG_ERROR("Failed to start marathon");
            showError(tr("Не удалось начать марафон"));
        }
    }
}

void MainWindow::onAnswerSubmitted()
{
    if (!m_answerButtonGroup->checkedButton()) {
        showError(tr("Выберите ответ"));
        return;
    }

    QString answer = m_answerButtonGroup->checkedButton()->text();
    bool correct = m_quizManager->checkMarathonAnswer(answer);

    // Отключаем все кнопки после ответа
    for (QAbstractButton* button : m_answerButtonGroup->buttons()) {
        button->setEnabled(false);
        QRadioButton* radioButton = qobject_cast<QRadioButton*>(button);
        if (radioButton) {
            if (radioButton->text() == answer) {
                // Если это выбранный ответ
                if (correct) {
                    radioButton->setStyleSheet("QRadioButton { background-color: #4CAF50; color: white; }");
                } else {
                    radioButton->setStyleSheet("QRadioButton { background-color: #F44336; color: white; }");
                }
            } else if (radioButton->text() == m_quizManager->getCurrentMarathonAnswer()) {
                // Если это правильный ответ
                radioButton->setStyleSheet("QRadioButton { background-color: #4CAF50; color: white; }");
            }
        }
    }

    // Добавляем метку с правильным ответом
    if (!correct) {
        QLabel* correctAnswerLabel = new QLabel(tr("Правильный ответ: %1").arg(m_quizManager->getCurrentMarathonAnswer()), this);
        correctAnswerLabel->setStyleSheet("QLabel { color: #4CAF50; font-weight: bold; }");
        m_answersLayout->addWidget(correctAnswerLabel);
    }

    // Отключаем кнопку отправки ответа
    m_submitButton->setEnabled(false);

    // Если ответ правильный, переходим к следующему вопросу через 1 секунду
    if (correct) {
        QTimer::singleShot(1000, this, [this]() {
            if (!m_quizManager->nextMarathonQuestion()) {
                m_stackedWidget->setCurrentIndex(0);
            }
            updateUI();
        });
    }
}

void MainWindow::onNextQuestion()
{
    if (m_quizManager->isMarathonActive()) {
        if (m_quizManager->nextMarathonQuestion()) {
            updateUI();
        }
    }
}

void MainWindow::onPreviousQuestion()
{
    if (m_quizManager->isMarathonActive()) {
        if (m_quizManager->previousMarathonQuestion()) {
            updateUI();
        }
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("О программе"),
                      tr("Quiz Own - программа для тестирования знаний\n\n"
                         "Версия 1.0\n"
                         "© 2024"));
}

void MainWindow::onSettings()
{
    // TODO: Implement settings dialog
    showInfo(tr("Настройки пока не реализованы"));
}

void MainWindow::showResults()
{
    int correct = m_quizManager->getCorrectAnswers();
    int total = m_quizManager->getTotalQuestions();
    
    QString message = tr("Тест завершен!\n\n"
                        "Правильных ответов: %1 из %2\n"
                        "Процент правильных ответов: %3%")
                        .arg(correct)
                        .arg(total)
                        .arg((correct * 100.0) / total, 0, 'f', 1);
    
    QMessageBox::information(this, tr("Результаты теста"), message);
    m_stackedWidget->setCurrentIndex(0);
    updateUI();
}

void MainWindow::setupTestUI()
{
    // Создаем виджет для теста
    QWidget* testWidget = new QWidget(this);
    QVBoxLayout* testLayout = new QVBoxLayout(testWidget);
    
    // Создаем метку для вопроса
    m_questionLabel = new QLabel(this);
    m_questionLabel->setWordWrap(true);
    m_questionLabel->setAlignment(Qt::AlignCenter);
    m_questionLabel->setStyleSheet("font-size: 16px; margin: 20px;");
    testLayout->addWidget(m_questionLabel);
    
    // Создаем контейнер для кнопок ответов
    m_answersContainer = new QWidget(this);
    m_answersLayout = new QVBoxLayout(m_answersContainer);
    m_answersLayout->setSpacing(10);
    testLayout->addWidget(m_answersContainer);
    
    // Создаем виджет для навигации
    QWidget* navigationWidget = new QWidget(this);
    QHBoxLayout* navigationLayout = new QHBoxLayout(navigationWidget);
    
    // Кнопка предыдущего вопроса
    m_prevButton = new QPushButton("←", this);
    m_prevButton->setFixedSize(40, 40);
    m_prevButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 20px; } QPushButton:hover { background-color: #45a049; }");
    navigationLayout->addWidget(m_prevButton);
    
    // Кнопка завершения теста
    m_endTestButton = new QPushButton("Завершить тест", this);
    m_endTestButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; border: none; padding: 8px 16px; border-radius: 4px; } QPushButton:hover { background-color: #da190b; }");
    navigationLayout->addWidget(m_endTestButton);
    
    // Кнопка следующего вопроса
    m_nextButton = new QPushButton("→", this);
    m_nextButton->setFixedSize(40, 40);
    m_nextButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 20px; } QPushButton:hover { background-color: #45a049; }");
    navigationLayout->addWidget(m_nextButton);
    
    testLayout->addWidget(navigationWidget);
    
    // Добавляем виджет теста в стек
    m_stackedWidget->addWidget(testWidget);
    
    // Подключаем сигналы
    connect(m_prevButton, &QPushButton::clicked, this, &MainWindow::onPreviousQuestionClicked);
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNextQuestionClicked);
    connect(m_endTestButton, &QPushButton::clicked, this, &MainWindow::onEndTestClicked);
}

void MainWindow::onEndTestClicked()
{
    if (m_quizManager->endTest()) {
        showResults();
    }
}

void MainWindow::onNextQuestionClicked()
{
    if (m_quizManager->nextQuestion()) {
        updateUI();
    }
}

void MainWindow::onPreviousQuestionClicked()
{
    if (m_quizManager->previousQuestion()) {
        updateUI();
    }
}