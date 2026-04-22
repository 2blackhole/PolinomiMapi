// qt_gui/PolynomialWidget.hpp
#ifndef POLYNOMIALWIDGET_H
#define POLYNOMIALWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QTextEdit>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QComboBox>
#include <QSplitter>
#include "PolinomManager.hpp"

class PolynomialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PolynomialWidget(PolinomManager* manager, QWidget *parent = nullptr);

public slots:
    void createNewPolynomial();
    void addMonomial();
    void clearCurrentPolynomial();
    void savePolynomial();
    void loadPolynomial();
    void deletePolynomial();
    void differentiatePolynomial();
    void integratePolynomial();
    void refreshPolynomialList();

    signals:
        void polynomialChanged();

private:
    void setupUI();
    void setupConnections();
    void updatePolynomialDisplay();
    void updateMonomialList();
    void clearMonomialInputs();
    bool validateInputs();
    monon createMonomFromInputs();

    QSplitter *m_mainSplitter;
    QListWidget *m_polynomialList;
    QPushButton *m_loadButton;
    QPushButton *m_deleteButton;
    QPushButton *m_refreshButton;
    QLineEdit *m_polynomialName;
    QTextEdit *m_polynomialDisplay;
    QGroupBox *m_monomialGroup;
    QDoubleSpinBox *m_coefficientSpin;
    QSpinBox *m_xPowerSpin;
    QSpinBox *m_yPowerSpin;
    QSpinBox *m_zPowerSpin;
    QPushButton *m_addMonomialButton;
    QListWidget *m_monomialList;
    QPushButton *m_clearButton;
    QGroupBox *m_operationsGroup;
    QComboBox *m_variableCombo;
    QPushButton *m_differentiateButton;
    QPushButton *m_integrateButton;
    QPushButton *m_saveButton;
    QPushButton *m_newButton;

    PolinomManager* m_manager;
    polijop m_currentPolynomial;
    QString m_currentPolynomialName;
};

#endif