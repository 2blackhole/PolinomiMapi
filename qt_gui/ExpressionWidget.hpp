#ifndef EXPRESSIONWIDGET_H
#define EXPRESSIONWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QListWidget>
#include <QSplitter>
#include "PolinomManager.hpp"
#include "PostfixCalculator.hpp"

class ExpressionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExpressionWidget(PolinomManager* manager, QWidget *parent = nullptr);

public slots:
    void evaluateExpression();
    void evaluateAndSave();
    void clearExpression();
    void clearHistory();
    void onHistoryItemSelected();
    void showExpressionHelp();

    signals:
        void expressionEvaluated(const QString& expression, const QString& result);

private:
    void setupUI();
    void setupConnections();
    void addToHistory(const QString& expression, const QString& result);
    QString formatResult(const polijop& result);

    QSplitter *m_mainSplitter;
    QLineEdit *m_expressionEdit;
    QPushButton *m_evaluateButton;
    QPushButton *m_evaluateAndSaveButton;
    QPushButton *m_clearButton;
    QPushButton *m_helpButton;
    QTextEdit *m_resultDisplay;
    QListWidget *m_historyList;
    QPushButton *m_clearHistoryButton;
    PolinomManager* m_manager;
};

#endif