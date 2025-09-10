// include header and ui header
#include "welcome.h"
#include "ui_welcome.h"
#include <QDir>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <iostream>
#include <fstream>

// constructor
Welcome::Welcome(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);
    ui->introText->setOpenExternalLinks(true);
    ui->textBrowser->setOpenExternalLinks(true);

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
        selectedFolder = "C:/Windows/System32";
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

// ---------- Slots ----------

void Welcome::on_nextButton_clicked() {
    goToNextPage();
}

void Welcome::on_backButton_clicked() {
    goToPreviousPage();
}

void Welcome::on_finishButton_clicked() {
    //incase user does something stupid
    if(difficulty == 0){selectedFolder = "";}
    if(difficulty == 2){selectedFolder = "C:/Windows/System32";}

    // Write settings to file
    std::ofstream outFile("settings.txt");
    if (outFile.is_open()) {
        outFile << difficulty << "\n";                // line 1: difficulty
        outFile << selectedFolder.toStdString() << "\n"; // line 2: folder
        outFile.close();
    }

    std::cout << "Saved settings:\n"
              << "Difficulty: " << difficulty << "\n"
              << "Folder: " << selectedFolder.toStdString() << std::endl;

    accept();
}

void Welcome::on_browseButton_clicked() {
    if(difficulty == 1)chooseFolder(false);
    else chooseFolder(true);
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
