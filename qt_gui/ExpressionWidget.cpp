#include "ExpressionWidget.hpp"
#include <QMessageBox>
#include <QInputDialog>
#include <sstream>

ExpressionWidget::ExpressionWidget(PolinomManager* manager, QWidget *parent)
    : QWidget(parent)
    , m_manager(manager)
{
    setupUI();
    setupConnections();
}

void ExpressionWidget::setupUI()
{
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    leftLayout->addWidget(new QLabel("Enter expression:"));
    m_expressionEdit = new QLineEdit();
    m_expressionEdit->setPlaceholderText("e.g., p1 + p2 * 2.5 - 3.0");
    leftLayout->addWidget(m_expressionEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_evaluateButton = new QPushButton("Evaluate");
    m_evaluateAndSaveButton = new QPushButton("Evaluate and Save");
    m_clearButton = new QPushButton("Clear");
    m_helpButton = new QPushButton("Help");
    buttonLayout->addWidget(m_evaluateButton);
    buttonLayout->addWidget(m_evaluateAndSaveButton);
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addWidget(m_helpButton);
    leftLayout->addLayout(buttonLayout);

    leftLayout->addWidget(new QLabel("Result:"));
    m_resultDisplay = new QTextEdit();
    m_resultDisplay->setReadOnly(true);
    m_resultDisplay->setMaximumHeight(120);
    m_resultDisplay->setPlainText("Result will appear here...");
    leftLayout->addWidget(m_resultDisplay);

    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->addWidget(new QLabel("Evaluation History:"));
    m_historyList = new QListWidget();
    rightLayout->addWidget(m_historyList);

    m_clearHistoryButton = new QPushButton("Clear History");
    rightLayout->addWidget(m_clearHistoryButton);
    rightPanel->setMaximumWidth(300);

    m_mainSplitter->addWidget(leftPanel);
    m_mainSplitter->addWidget(rightPanel);
    m_mainSplitter->setSizes({500, 300});

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_mainSplitter);
}

void ExpressionWidget::setupConnections()
{
    connect(m_evaluateButton, &QPushButton::clicked, this, &ExpressionWidget::evaluateExpression);
    connect(m_evaluateAndSaveButton, &QPushButton::clicked, this, &ExpressionWidget::evaluateAndSave);
    connect(m_clearButton, &QPushButton::clicked, this, &ExpressionWidget::clearExpression);
    connect(m_helpButton, &QPushButton::clicked, this, &ExpressionWidget::showExpressionHelp);
    connect(m_historyList, &QListWidget::itemClicked, this, &ExpressionWidget::onHistoryItemSelected);
    connect(m_clearHistoryButton, &QPushButton::clicked, this, &ExpressionWidget::clearHistory);
    connect(m_expressionEdit, &QLineEdit::returnPressed, this, &ExpressionWidget::evaluateExpression);
}

void ExpressionWidget::evaluateExpression()
{
    QString expression = m_expressionEdit->text().trimmed();
    if (expression.isEmpty()) {
        return;
    }

    try {
        PostfixCalculator calc;
        calc.SetTable(m_manager->GetActiveTable());
        polijop result = calc.Evaluate(expression.toStdString());

        QString resultStr = formatResult(result);
        m_resultDisplay->setPlainText(resultStr);
        addToHistory(expression, resultStr);
        emit expressionEvaluated(expression, resultStr);
    } catch (const std::exception& e) {
        QString error = QString("Error: %1").arg(e.what());
        m_resultDisplay->setPlainText(error);
        QMessageBox::warning(this, "Evaluation Error", error);
    }
}

void ExpressionWidget::evaluateAndSave()
{
    QString expression = m_expressionEdit->text().trimmed();
    if (expression.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter an expression to evaluate");
        return;
    }

    bool ok;
    QString saveName = QInputDialog::getText(this, "Save Result",
        "Enter name for the result:", QLineEdit::Normal,
        "result", &ok);
    if (!ok || saveName.trimmed().isEmpty()) {
        return;
    }
    saveName = saveName.trimmed();

    try {
        PostfixCalculator calc;
        calc.SetTable(m_manager->GetActiveTable());
        polijop result = calc.Evaluate(expression.toStdString());

        m_manager->AddPolinom(saveName.toStdString(), result);
        QString resultStr = formatResult(result);
        m_resultDisplay->setPlainText(resultStr);
        addToHistory(expression + " -> " + saveName, resultStr);
        QMessageBox::information(this, "Success",
            QString("Result saved as '%1'").arg(saveName));
        emit expressionEvaluated(expression, resultStr);
    } catch (const std::exception& e) {
        QString error = QString("Error: %1").arg(e.what());
        m_resultDisplay->setPlainText(error);
        QMessageBox::warning(this, "Error", error);
    }
}

void ExpressionWidget::clearExpression()
{
    m_expressionEdit->clear();
    m_resultDisplay->setPlainText("Result will appear here...");
}

void ExpressionWidget::clearHistory()
{
    m_historyList->clear();
}

void ExpressionWidget::onHistoryItemSelected()
{
    QListWidgetItem* item = m_historyList->currentItem();
    if (item) {
        QString historyText = item->text();
        int equalPos = historyText.indexOf('=');
        if (equalPos > 0) {
            QString expression = historyText.left(equalPos).trimmed();
            int arrowPos = expression.indexOf("->");
            if (arrowPos > 0) {
                expression = expression.left(arrowPos).trimmed();
            }
            m_expressionEdit->setText(expression);
        }
    }
}

void ExpressionWidget::showExpressionHelp()
{
    QString helpText =
        "<h3>Expression Help</h3>"
        "<p><b>Supported operations:</b></p>"
        "<ul>"
        "<li><b>+</b> - addition</li>"
        "<li><b>-</b> - subtraction</li>"
        "<li><b>*</b> - multiplication</li>"
        "<li><b>( )</b> - grouping</li>"
        "</ul>"
        "<p><b>Examples:</b></p>"
        "<ul>"
        "<li><code>p1 + p2</code></li>"
        "<li><code>p1 * 2.5</code></li>"
        "<li><code>(p1 + p2) * p3</code></li>"
        "</ul>"
        "<p>Polynomial names must exist in the active table. Constants are decimal numbers.</p>";
    QMessageBox::information(this, "Expression Help", helpText);
}

void ExpressionWidget::addToHistory(const QString& expression, const QString& result)
{
    QString historyItem = QString("%1 = %2").arg(expression, result);
    m_historyList->insertItem(0, historyItem);
    // Ограничим размер истории
    while (m_historyList->count() > 50) {
        delete m_historyList->takeItem(m_historyList->count() - 1);
    }
}

QString ExpressionWidget::formatResult(const polijop& result)
{
    std::ostringstream oss;
    oss << result;
    QString str = QString::fromStdString(oss.str());
    if (str.endsWith('\n')) str.chop(1);
    return str;
}