#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include "PolinomManager.hpp"

class TableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TableWidget(PolinomManager* manager, QWidget *parent = nullptr);

public slots:
    void onTableTypeChanged(int index);
    void onSearchTextChanged();
    void refreshTableView();
    void removeSelectedPolynomial();
    void clearAllPolynomials();
    void saveToFile();
    void loadFromFile();

    signals:
        void tableChanged();

private:
    void setupUI();
    void setupConnections();
    void populateTableWidget();

    QComboBox *m_tableTypeCombo;
    QLineEdit *m_searchEdit;
    QPushButton *m_refreshButton;
    QPushButton *m_clearButton;
    QPushButton *m_saveButton;
    QPushButton *m_loadButton;
    QTableWidget *m_tableWidget;
    QPushButton *m_removeButton;
    PolinomManager* m_manager;
    int m_currentTableType;
};

#endif