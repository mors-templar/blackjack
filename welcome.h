#ifndef WELCOME_H
#define WELCOME_H

#include <QDialog>
#include <QString>
#include <QStackedWidget>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui {
class Welcome;
}
QT_END_NAMESPACE

class Welcome : public QDialog
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome();

private:
    // pointer to the generated UI
    Ui::Welcome *ui;

    // wizard state
    int currentPageIndex = 0;   // track stackedWidget index
    QString selectedFolder;     // store chosen folder
    QString difficulty;         // "Easy", "Normal", "Hard"

    // helper methods you’ll define in .cpp
    void goToNextPage();
    void goToPreviousPage();
    void updateSummaryPage();
    void chooseFolder();
    void setDifficulty();

private slots:
    // slots for button actions
    void on_nextButton_clicked();
    void on_backButton_clicked();
    void on_finishButton_clicked();
    void on_browseButton_clicked();

    // slots for difficulty selection
    void on_easyRadioButton_clicked();
    void on_normalRadioButton_clicked();
    void on_hardRadioButton_clicked();
};

#endif // WELCOME_H
