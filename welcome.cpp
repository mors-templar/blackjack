// include header and ui header
#include "welcome.h"
#include "ui_welcome.h"
#include <QDir>
#include <QDebug>

// constructor
Welcome::Welcome(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);

    // initialize current page index
    currentPageIndex = 0;
    ui->stackedWidget->setCurrentIndex(currentPageIndex);

    // start with Easy difficulty
    difficulty = 0;

    // disable Finish until the last page
    ui->finishButton->setEnabled(false);

    // no need to manually connect() because we use auto-connect slots
}

Welcome::~Welcome()
{
    delete ui;
}

// ---------- Private helper methods ----------

void Welcome::goToNextPage() {
    if (currentPageIndex < ui->stackedWidget->count() - 1) {
        currentPageIndex++;
        ui->stackedWidget->setCurrentIndex(currentPageIndex);
    }

    // enable Finish only on the last page
    if (currentPageIndex == ui->stackedWidget->count() - 1) {
        ui->finishButton->setEnabled(true);
        updateSummaryPage();
    }
}

void Welcome::goToPreviousPage() {
    if (currentPageIndex > 0) {
        currentPageIndex--;
        ui->stackedWidget->setCurrentIndex(currentPageIndex);
    }
}

void Welcome::updateSummaryPage() {
    QString diffText;
    if (difficulty == 0) diffText = "Easy";
    else if (difficulty == 1) diffText = "Normal";
    else diffText = "Hard";

    ui->difflabel->setText("You choose: " + diffText);
    ui->folderLabel->setText("Folder: " + selectedFolder);
}

void Welcome::chooseFolder(bool hardmode) {
    if (hardmode) {
        // auto-select Windows folder in Hard mode
        selectedFolder = "C:/Windows";
        ui->folderLineEdit->setText(selectedFolder);
    } else {
        // open folder selection dialog
        QString folder = QFileDialog::getExistingDirectory(
            this,
            "Select a Folder",
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );

        // if user picked a folder
        if (!folder.isEmpty()) {
            selectedFolder = folder;
            ui->folderLineEdit->setText(selectedFolder);
        }
    }
}

void Welcome::setDifficulty() {
    if (ui->easyRadioButton->isChecked()) {
        difficulty = 0;
    } else if (ui->normalRadioButton->isChecked()) {
        difficulty = 1;
    } else if (ui->hardRadioButton->isChecked()) {
        difficulty = 2;
    }
}

// ---------- Slots ----------

void Welcome::on_nextButton_clicked() {
    goToNextPage();
}

void Welcome::on_backButton_clicked() {
    goToPreviousPage();
}

void Welcome::on_finishButton_clicked() {
    accept();   // close need more code to connect game to this
}

void Welcome::on_browseButton_clicked() {
    chooseFolder(difficulty == 2);
}

void Welcome::on_easyRadioButton_clicked() {
    difficulty = 0;
}

void Welcome::on_normalRadioButton_clicked() {
    difficulty = 1;
}

void Welcome::on_hardRadioButton_clicked() {
    difficulty = 2;
}
