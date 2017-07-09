/*************************************************************************
***************  CinderBot - standalone bot for Twitch.tv ****************
******** Copyright (C) 2017  Ilya Lobanov (exanimoteam@gmail.com) ********
********         Check full copyright header in main.cpp          ********
**************************************************************************/
#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include "ParametersCustomCommandWidget.hpp"
#include "AnswersCustomCommandWidget.hpp"

namespace Ui
{

class EditCustomCommandWindow : public QDialog
{
    Q_OBJECT
public:
    /*!
     * Constructor
     */
    explicit EditCustomCommandWindow(QWidget *parent = 0);
    /*!
     * Destructor
     */
    ~EditCustomCommandWindow();

    /*!
     * Load data related to specified command
     * \param cmdName - name of command
     */
    void LoadCommandData(const QString& cmdName);

private slots:
    /*!
     * Update "Add" button to be active or not, depends on content of text field
     */
    void _CheckAnswerField();
    /*!
     * Handle "Add" button
     */
    void _OnAddButton();
    /*!
     * Handle "Save" button
     */
    void _OnSaveButton();
    /*!
     * Update whole list of answers
     * \param tableName - table which was changed
     */
    void _UpdateAnswers(const QString &tableName);

private:
    /*! Main list of window */
    QHBoxLayout *_mainLayout;
    /*! Left part of window */
    QFrame *_leftPart;
    /*! Layout for left part of window */
    QVBoxLayout *_leftLayout;
    /*! Parameters of command */
    ParametersCustomCommandWidget *_parameters;
    /*! Text field of new answer */
    QPlainTextEdit *_newAnswer;
    /*! Add button */
    QPushButton *_addButton;
    /*! Save button */
    QPushButton *_saveButton;
    /*! List of answers */
    AnswersCustomCommandWidget *_answers;
};

}