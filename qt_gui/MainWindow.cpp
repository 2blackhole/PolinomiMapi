#include "MainWindow.hpp"
#include "PolynomialWidget.hpp"
#include "TableWidget.hpp"
#include "ExpressionWidget.hpp"
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(nullptr)
    , m_polynomialWidget(nullptr)
    , m_tableWidget(nullptr)
    , m_expressionWidget(nullptr)
    , m_tableCountLabel(nullptr)
    , m_activeTableLabel(nullptr)
    , m_polynomManager(new PolinomManager())
{
    setupUI();
    setupMenuBar();
    setupStatusBar();
    setupConnections();
    updateStatusBar();
    setWindowTitle("Polynomial Algebra System");
}

MainWindow::~MainWindow()
{
    delete m_polynomManager;
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    m_tabWidget = new QTabWidget();

    m_polynomialWidget = new PolynomialWidget(m_polynomManager);
    m_tabWidget->addTab(m_polynomialWidget, "Polynomials");

    m_tableWidget = new TableWidget(m_polynomManager);
    m_tabWidget->addTab(m_tableWidget, "Tables");

    m_expressionWidget = new ExpressionWidget(m_polynomManager);
    m_tabWidget->addTab(m_expressionWidget, "Expressions");

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(m_tabWidget);

    setMinimumSize(800, 600);
    resize(1000, 700);
}

void MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");

    m_newAction = new QAction("&New Polynomial", this);
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setStatusTip("Create new polynomial");
    fileMenu->addAction(m_newAction);

    fileMenu->addSeparator();

    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("Exit application");
    fileMenu->addAction(m_exitAction);

    QMenu *helpMenu = menuBar()->addMenu("&Help");

    m_helpAction = new QAction("&Help", this);
    m_helpAction->setShortcut(QKeySequence::HelpContents);
    m_helpAction->setStatusTip("Show help");
    helpMenu->addAction(m_helpAction);

    helpMenu->addSeparator();

    m_aboutAction = new QAction("&About", this);
    m_aboutAction->setStatusTip("About application");
    helpMenu->addAction(m_aboutAction);

    QAction *saveAction = new QAction("&Save Table to File...", this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip("Save all polynomials to file");
    fileMenu->addAction(saveAction);

    QAction *loadAction = new QAction("&Load Table from File...", this);
    loadAction->setShortcut(QKeySequence::Open);
    loadAction->setStatusTip("Load polynomials from file");
    fileMenu->addAction(loadAction);

    connect(saveAction, &QAction::triggered, [this]() {
        m_tableWidget->saveToFile();
    });

    connect(loadAction, &QAction::triggered, [this]() {
        m_tableWidget->loadFromFile();
    });
}

void MainWindow::setupStatusBar()
{
    m_activeTableLabel = new QLabel("Active table: Unordered Vector");
    statusBar()->addPermanentWidget(m_activeTableLabel);

    statusBar()->addPermanentWidget(new QLabel(" | "));

    m_tableCountLabel = new QLabel("Polynomials: 0");
    statusBar()->addPermanentWidget(m_tableCountLabel);
}

void MainWindow::setupConnections()
{
    connect(m_newAction, &QAction::triggered, this, &MainWindow::onNewPolynomial);
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::onExit);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    connect(m_helpAction, &QAction::triggered, this, &MainWindow::onShowHelp);
    connect(m_tableWidget, &TableWidget::tableChanged, this, &MainWindow::onTableChanged);
    connect(m_polynomialWidget, &PolynomialWidget::polynomialChanged, this, &MainWindow::onPolynomialChanged);
}

void MainWindow::updateStatusBar()
{
    if (m_polynomManager) {
        size_t count = m_polynomManager->GetPolinomCount();
        m_tableCountLabel->setText(QString("Polynomials: %1").arg(count));
        m_activeTableLabel->setText(QString("Active table: %1").arg(QString::fromStdString(m_polynomManager->GetActiveTableName())));
    }
}

void MainWindow::onNewPolynomial()
{
    m_tabWidget->setCurrentWidget(m_polynomialWidget);
    m_polynomialWidget->createNewPolynomial();
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About",
        "<h3>Polynomial Algebra System</h3>"
        "<p>Version 1.0</p>"
        "<p>Data Structures Laboratory Work</p>"
        "<p><b>Features:</b></p>"
        "<ul>"
        "<li>Polynomials of 3 variables</li>"
        "<li>6 types of storage tables</li>"
        "<li>Arithmetic operations</li>"
        "<li>Differentiation and integration</li>"
        "<li>Postfix expression evaluation</li>"
        "</ul>");
}

void MainWindow::onShowHelp()
{
    QMessageBox::information(this, "Help",
        "<h3>How to use:</h3>"
        "<p><b>Polynomials Tab:</b></p>"
        "<ul>"
        "<li>Create and edit polynomials</li>"
        "<li>View existing polynomials</li>"
        "<li>Differentiate and integrate</li>"
        "</ul>"
        "<p><b>Tables Tab:</b></p>"
        "<ul>"
        "<li>Switch between table types</li>"
        "<li>View table contents</li>"
        "<li>Manage polynomials in tables</li>"
        "</ul>"
        "<p><b>Expressions Tab:</b></p>"
        "<ul>"
        "<li>Evaluate arithmetic expressions</li>"
        "<li>Use polynomial names in expressions</li>"
        "<li>Save evaluation results</li>"
        "</ul>");
}

void MainWindow::onTableChanged()
{
    updateStatusBar();
}

void MainWindow::onPolynomialChanged()
{
    updateStatusBar();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}