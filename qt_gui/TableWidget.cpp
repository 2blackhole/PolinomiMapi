// qt_gui/TableWidget.cpp
#include "TableWidget.hpp"
#include <QMessageBox>
#include <QHeaderView>
#include <sstream>

TableWidget::TableWidget(PolinomManager* manager, QWidget *parent)
    : QWidget(parent)
    , m_manager(manager)
    , m_currentTableType(0)
{
    setupUI();
    setupConnections();
    refreshTableView();
}

void TableWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *controlGroup = new QGroupBox("Table Control");
    QGridLayout *controlLayout = new QGridLayout(controlGroup);

    controlLayout->addWidget(new QLabel("Table Type:"), 0, 0);
    m_tableTypeCombo = new QComboBox();
    m_tableTypeCombo->addItems({
        "Unordered Vector",
        "Ordered Vector",
        "AVL Tree",
        "Red-Black Tree",
        "Chain Hash",
        "Open Addressing Hash"
    });
    controlLayout->addWidget(m_tableTypeCombo, 0, 1);

    controlLayout->addWidget(new QLabel("Search:"), 0, 2);
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Enter polynomial name");
    controlLayout->addWidget(m_searchEdit, 0, 3);

    m_refreshButton = new QPushButton("Refresh");
    m_clearButton = new QPushButton("Clear All");
    controlLayout->addWidget(m_refreshButton, 1, 0);
    controlLayout->addWidget(m_clearButton, 1, 1);

    mainLayout->addWidget(controlGroup);

    m_tableWidget = new QTableWidget();
    m_tableWidget->setColumnCount(2);
    m_tableWidget->setHorizontalHeaderLabels({"Name", "Polynomial"});
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setAlternatingRowColors(true);
    mainLayout->addWidget(m_tableWidget);

    QHBoxLayout *tableButtonLayout = new QHBoxLayout();
    m_removeButton = new QPushButton("Remove Selected");
    tableButtonLayout->addWidget(m_removeButton);
    tableButtonLayout->addStretch();
    mainLayout->addLayout(tableButtonLayout);
}

void TableWidget::setupConnections()
{
    connect(m_tableTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &TableWidget::onTableTypeChanged);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &TableWidget::onSearchTextChanged);
    connect(m_refreshButton, &QPushButton::clicked, this, &TableWidget::refreshTableView);
    connect(m_clearButton, &QPushButton::clicked, this, &TableWidget::clearAllPolynomials);
    connect(m_removeButton, &QPushButton::clicked, this, &TableWidget::removeSelectedPolynomial);
}

void TableWidget::onTableTypeChanged(int index)
{
    TableType type = static_cast<TableType>(index);

    try {
        m_manager->SetActiveTable(type);
        m_currentTableType = index;
        refreshTableView();
        emit tableChanged();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Error",
            QString("Error changing table: %1").arg(e.what()));
        m_tableTypeCombo->setCurrentIndex(m_currentTableType);
    }
}

void TableWidget::onSearchTextChanged()
{
    QString searchText = m_searchEdit->text().trimmed();

    if (searchText.isEmpty()) {
        populateTableWidget();
        return;
    }

    try {
        polijop* found = m_manager->FindPolinom(searchText.toStdString());
        if (!found) {
            m_tableWidget->setRowCount(0);
            return;
        }

        m_tableWidget->setRowCount(1);
        m_tableWidget->setItem(0, 0, new QTableWidgetItem(searchText));

        std::ostringstream oss;
        oss << *found;
        QString polyStr = QString::fromStdString(oss.str());
        if (polyStr.endsWith('\n')) polyStr.chop(1);
        m_tableWidget->setItem(0, 1, new QTableWidgetItem(polyStr));
    } catch (const std::exception& e) {
        m_tableWidget->setRowCount(0);
    }
}

void TableWidget::refreshTableView()
{
    populateTableWidget();
}

void TableWidget::removeSelectedPolynomial()
{
    int currentRow = m_tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, "Information", "Select a polynomial to remove");
        return;
    }

    QString name = m_tableWidget->item(currentRow, 0)->text();
    int ret = QMessageBox::question(this, "Confirm",
        QString("Remove polynomial '%1'?").arg(name),
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        try {
            m_manager->RemovePolinom(name.toStdString());
            refreshTableView();
            QMessageBox::information(this, "Success", QString("Polynomial '%1' removed").arg(name));
            emit tableChanged();
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Error", QString("Error removing: %1").arg(e.what()));
        }
    }
}

void TableWidget::clearAllPolynomials()
{
    int ret = QMessageBox::question(this, "Confirm",
        "Clear all polynomials from the active table?",
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        try {
            m_manager->ClearAll();
            refreshTableView();
            QMessageBox::information(this, "Success", "Active table cleared");
            emit tableChanged();
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Error", QString("Error clearing: %1").arg(e.what()));
        }
    }
}

void TableWidget::populateTableWidget()
{
    try {
        auto items = m_manager->GetAll();
        m_tableWidget->setRowCount(static_cast<int>(items.size()));
        int row = 0;
        for (const auto& pair : items) {
            m_tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(pair.first)));

            std::ostringstream oss;
            oss << pair.second;
            QString polyStr = QString::fromStdString(oss.str());
            if (polyStr.endsWith('\n')) polyStr.chop(1);
            m_tableWidget->setItem(row, 1, new QTableWidgetItem(polyStr));
            ++row;
        }
    } catch (const std::exception& e) {
        m_tableWidget->setRowCount(0);
        QMessageBox::warning(this, "Error", QString("Error loading table: %1").arg(e.what()));
    }
}