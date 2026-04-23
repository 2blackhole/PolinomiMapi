#include "PolynomialWidget.hpp"
#include <QMessageBox>
#include <sstream>

PolynomialWidget::PolynomialWidget(PolinomManager* manager, QWidget *parent)
    : QWidget(parent)
    , m_manager(manager)
    , m_currentPolynomial()
{
    setupUI();
    setupConnections();
    refreshPolynomialList();
}

void PolynomialWidget::setupUI()
{
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->addWidget(new QLabel("Saved Polynomials:"));
    m_polynomialList = new QListWidget();
    leftLayout->addWidget(m_polynomialList);

    QHBoxLayout *leftButtonLayout = new QHBoxLayout();
    m_loadButton = new QPushButton("Load");
    m_deleteButton = new QPushButton("Delete");
    m_refreshButton = new QPushButton("Refresh");
    leftButtonLayout->addWidget(m_loadButton);
    leftButtonLayout->addWidget(m_deleteButton);
    leftButtonLayout->addWidget(m_refreshButton);
    leftLayout->addLayout(leftButtonLayout);
    leftPanel->setMaximumWidth(250);

    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("Polynomial Name:"));
    m_polynomialName = new QLineEdit();
    m_polynomialName->setPlaceholderText("Enter polynomial name");
    nameLayout->addWidget(m_polynomialName);
    rightLayout->addLayout(nameLayout);

    rightLayout->addWidget(new QLabel("Current Polynomial:"));
    m_polynomialDisplay = new QTextEdit();
    m_polynomialDisplay->setReadOnly(true);
    m_polynomialDisplay->setMaximumHeight(100);
    m_polynomialDisplay->setPlainText("0");
    rightLayout->addWidget(m_polynomialDisplay);

    m_monomialGroup = new QGroupBox("Add Monomial");
    QGridLayout *monomialLayout = new QGridLayout(m_monomialGroup);

    monomialLayout->addWidget(new QLabel("Coefficient:"), 0, 0);
    m_coefficientSpin = new QDoubleSpinBox();
    m_coefficientSpin->setRange(-1000.0, 1000.0);
    m_coefficientSpin->setDecimals(3);
    m_coefficientSpin->setValue(1.0);
    monomialLayout->addWidget(m_coefficientSpin, 0, 1);

    monomialLayout->addWidget(new QLabel("Power x:"), 1, 0);
    m_xPowerSpin = new QSpinBox();
    m_xPowerSpin->setRange(0, 1023);
    monomialLayout->addWidget(m_xPowerSpin, 1, 1);

    monomialLayout->addWidget(new QLabel("Power y:"), 2, 0);
    m_yPowerSpin = new QSpinBox();
    m_yPowerSpin->setRange(0, 1023);
    monomialLayout->addWidget(m_yPowerSpin, 2, 1);

    monomialLayout->addWidget(new QLabel("Power z:"), 3, 0);
    m_zPowerSpin = new QSpinBox();
    m_zPowerSpin->setRange(0, 1023);
    monomialLayout->addWidget(m_zPowerSpin, 3, 1);

    m_addMonomialButton = new QPushButton("Add Monomial");
    monomialLayout->addWidget(m_addMonomialButton, 4, 0, 1, 2);
    rightLayout->addWidget(m_monomialGroup);

    rightLayout->addWidget(new QLabel("Monomials in Polynomial:"));
    m_monomialList = new QListWidget();
    m_monomialList->setMaximumHeight(120);
    rightLayout->addWidget(m_monomialList);

    QHBoxLayout *monomialButtonLayout = new QHBoxLayout();
    m_clearButton = new QPushButton("Clear All");
    monomialButtonLayout->addWidget(m_clearButton);
    rightLayout->addLayout(monomialButtonLayout);

    m_operationsGroup = new QGroupBox("Polynomial Operations");
    QGridLayout *operationsLayout = new QGridLayout(m_operationsGroup);

    operationsLayout->addWidget(new QLabel("Variable:"), 0, 0);
    m_variableCombo = new QComboBox();
    m_variableCombo->addItems({"x (0)", "y (1)", "z (2)"});
    operationsLayout->addWidget(m_variableCombo, 0, 1);

    m_differentiateButton = new QPushButton("Differentiate");
    operationsLayout->addWidget(m_differentiateButton, 1, 0);

    m_integrateButton = new QPushButton("Integrate");
    operationsLayout->addWidget(m_integrateButton, 1, 1);
    rightLayout->addWidget(m_operationsGroup);

    QHBoxLayout *controlLayout = new QHBoxLayout();
    m_newButton = new QPushButton("New Polynomial");
    m_saveButton = new QPushButton("Save Polynomial");
    controlLayout->addWidget(m_newButton);
    controlLayout->addWidget(m_saveButton);
    rightLayout->addLayout(controlLayout);

    m_mainSplitter->addWidget(leftPanel);
    m_mainSplitter->addWidget(rightPanel);
    m_mainSplitter->setSizes({250, 550});

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_mainSplitter);
}

void PolynomialWidget::setupConnections()
{
    connect(m_loadButton, &QPushButton::clicked, this, &PolynomialWidget::loadPolynomial);
    connect(m_deleteButton, &QPushButton::clicked, this, &PolynomialWidget::deletePolynomial);
    connect(m_refreshButton, &QPushButton::clicked, this, &PolynomialWidget::refreshPolynomialList);
    connect(m_addMonomialButton, &QPushButton::clicked, this, &PolynomialWidget::addMonomial);
    connect(m_clearButton, &QPushButton::clicked, this, &PolynomialWidget::clearCurrentPolynomial);
    connect(m_newButton, &QPushButton::clicked, this, &PolynomialWidget::createNewPolynomial);
    connect(m_saveButton, &QPushButton::clicked, this, &PolynomialWidget::savePolynomial);
    connect(m_differentiateButton, &QPushButton::clicked, this, &PolynomialWidget::differentiatePolynomial);
    connect(m_integrateButton, &QPushButton::clicked, this, &PolynomialWidget::integratePolynomial);
}

void PolynomialWidget::createNewPolynomial()
{
    m_currentPolynomial = polijop();
    m_currentPolynomialName.clear();
    m_polynomialName->clear();
    clearMonomialInputs();
    updatePolynomialDisplay();
    updateMonomialList();
}

void PolynomialWidget::addMonomial()
{
    if (!validateInputs()) {
        return;
    }

    try {
        monon m = createMonomFromInputs();
        m_currentPolynomial.addMonom(m);
        updatePolynomialDisplay();
        updateMonomialList();
        clearMonomialInputs();
        emit polynomialChanged();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Error", QString("Error adding monomial: %1").arg(e.what()));
    }
}

void PolynomialWidget::clearCurrentPolynomial()
{
    m_currentPolynomial = polijop();
    updatePolynomialDisplay();
    updateMonomialList();
    emit polynomialChanged();
}

void PolynomialWidget::savePolynomial()
{
    QString name = m_polynomialName->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter polynomial name");
        return;
    }

    try {
        m_manager->AddPolinom(name.toStdString(), m_currentPolynomial);
        m_currentPolynomialName = name;
        refreshPolynomialList();
        QMessageBox::information(this, "Success", QString("Polynomial '%1' saved").arg(name));
        emit polynomialChanged();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Error", QString("Error saving: %1").arg(e.what()));
    }
}

void PolynomialWidget::loadPolynomial()
{
    QListWidgetItem* item = m_polynomialList->currentItem();
    if (!item) {
        QMessageBox::information(this, "Information", "Select a polynomial to load");
        return;
    }

    QString name = item->text();
    try {
        polijop* polyPtr = m_manager->FindPolinom(name.toStdString());
        if (!polyPtr) {
            QMessageBox::warning(this, "Error", QString("Polynomial '%1' not found").arg(name));
            return;
        }

        m_currentPolynomial = *polyPtr;
        m_currentPolynomialName = name;
        m_polynomialName->setText(name);
        updatePolynomialDisplay();
        updateMonomialList();
        QMessageBox::information(this, "Success", QString("Polynomial '%1' loaded").arg(name));
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Error", QString("Error loading: %1").arg(e.what()));
    }
}

void PolynomialWidget::deletePolynomial()
{
    QListWidgetItem* item = m_polynomialList->currentItem();
    if (!item) {
        QMessageBox::information(this, "Information", "Select a polynomial to delete");
        return;
    }

    QString name = item->text();
    int ret = QMessageBox::question(this, "Confirm",
        QString("Delete polynomial '%1'?").arg(name),
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        try {
            m_manager->RemovePolinom(name.toStdString());
            refreshPolynomialList();
            if (m_currentPolynomialName == name) {
                createNewPolynomial();
            }
            QMessageBox::information(this, "Success", QString("Polynomial '%1' deleted").arg(name));
            emit polynomialChanged();
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Error", QString("Error deleting: %1").arg(e.what()));
        }
    }
}

void PolynomialWidget::differentiatePolynomial()
{
    int varIndex = m_variableCombo->currentIndex();

    try {
        m_currentPolynomial = m_currentPolynomial.differentiate(varIndex);
        updatePolynomialDisplay();
        updateMonomialList();

        QString varName = (varIndex == 0) ? "x" : (varIndex == 1) ? "y" : "z";
        QMessageBox::information(this, "Success", QString("Polynomial differentiated with respect to %1").arg(varName));
        emit polynomialChanged();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Error", QString("Error differentiating: %1").arg(e.what()));
    }
}

void PolynomialWidget::integratePolynomial()
{
    int varIndex = m_variableCombo->currentIndex();

    try {
        m_currentPolynomial = m_currentPolynomial.integrate(varIndex);
        updatePolynomialDisplay();
        updateMonomialList();

        QString varName = (varIndex == 0) ? "x" : (varIndex == 1) ? "y" : "z";
        QMessageBox::information(this, "Success", QString("Polynomial integrated with respect to %1").arg(varName));
        emit polynomialChanged();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Error", QString("Error integrating: %1").arg(e.what()));
    }
}

void PolynomialWidget::refreshPolynomialList()
{
    m_polynomialList->clear();
    try {
        auto names = m_manager->GetNames();
        for (const auto& name : names) {
            m_polynomialList->addItem(QString::fromStdString(name));
        }
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Error", QString("Error refreshing list: %1").arg(e.what()));
    }
}

void PolynomialWidget::updatePolynomialDisplay()
{
    std::ostringstream oss;
    oss << m_currentPolynomial;
    QString str = QString::fromStdString(oss.str());
    // Убираем лишний перевод строки, если есть
    if (str.endsWith('\n')) str.chop(1);
    m_polynomialDisplay->setPlainText(str);
}

void PolynomialWidget::updateMonomialList()
{
    m_monomialList->clear();
    std::ostringstream oss;
    oss << m_currentPolynomial;
    QString polyStr = QString::fromStdString(oss.str());

    if (polyStr != "0" && !polyStr.isEmpty()) {
        // Простое разбиение по '+', но может быть не идеально для вывода
        QStringList parts = polyStr.split('+');
        for (const QString& part : parts) {
            m_monomialList->addItem(part.trimmed());
        }
    }
}

void PolynomialWidget::clearMonomialInputs()
{
    m_coefficientSpin->setValue(1.0);
    m_xPowerSpin->setValue(0);
    m_yPowerSpin->setValue(0);
    m_zPowerSpin->setValue(0);
}

bool PolynomialWidget::validateInputs()
{
    if (m_coefficientSpin->value() == 0.0) {
        QMessageBox::warning(this, "Error", "Coefficient cannot be zero");
        return false;
    }
    return true;
}

monon PolynomialWidget::createMonomFromInputs()
{
    double coeff = m_coefficientSpin->value();
    int x = m_xPowerSpin->value();
    int y = m_yPowerSpin->value();
    int z = m_zPowerSpin->value();
    return monon(monon::pack(x, y, z), coeff);
}