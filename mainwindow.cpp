#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Initialize UI
    ui->setupUi(this);

    // Set up initial state
    balance = 0;
    betAmount = 0;

    // Disable control buttons until game starts
    ui->hitButton->setEnabled(false);
    ui->standButton->setEnabled(false);
    ui->doubleButton->setEnabled(false);
    ui->splitButton->setEnabled(false);

    // Setup connections
    // connect(ui->newGameButton, &QPushButton::clicked, this, &MainWindow::on_newGameButton_clicked);
    // (Optional) connect a signal for difficulty selection dialog

    // Initial UI update
    updateUI();
}

MainWindow::~MainWindow()
{
    // Cleanup
    delete ui;
}

void MainWindow::on_newGameButton_clicked()
{
    // Prompt for difficulty
    // - Show dialog to let user pick Easy, Normal, or Hard
    // - Save selection into gameDifficulty

    // Initialize game state
    // - clear playerHand and dealerHand
    // - clear UI card display frames

    // Create and shuffle deck
    // - call createDeck()
    // - call shuffleDeck()

    // Deal initial cards (2 each)
    // - one player, one dealer, repeat

    // Check for blackjack
    // - call checkBlackjack()
    // - if blackjack, handle instantly

    // Enable control buttons
    ui->hitButton->setEnabled(true);
    ui->standButton->setEnabled(true);
    ui->doubleButton->setEnabled(true);
    ui->splitButton->setEnabled(true);

    // Update UI
    updateUI();
}

void MainWindow::initializeGame(Difficulty difficulty)
{
    // Handle Easy mode
    // balance = 10000;

    // Handle Normal mode
    // balance = 10000;
    // ask user for wager folder path (QFileDialog)
    // store path in wagerFolderPath

    // Handle Hard mode
    // hardModeFolderPath = "C:/Windows/System32"
    // balance = number of files in that directory
    // fallback if invalid/empty
}

void MainWindow::on_hitButton_clicked()
{
    // Deal one card to player
    // check if > 21 → "You Bust! Dealer Wins!"
    // adjust balance if lost
    // disable controls if busted
    // updateUI()
}

void MainWindow::on_standButton_clicked()
{
    // Disable controls
    ui->hitButton->setEnabled(false);
    ui->standButton->setEnabled(false);
    ui->doubleButton->setEnabled(false);
    ui->splitButton->setEnabled(false);

    // Dealer's turn
    // keep dealing until dealer value >= 17

    // Compare winner
    // call checkWinner()

    // Update UI
    updateUI();
}

void MainWindow::on_doubleButton_clicked()
{
    // Double betAmount
    // Deal exactly one card to player
    // End turn immediately (call on_standButton_clicked)
    // updateUI()
}

void MainWindow::on_splitButton_clicked()
{
    // Check if player's first two cards are same rank
    // If not, show message and return
    // Otherwise → either implement split logic OR show "Not implemented"
}

void MainWindow::checkWinner()
{
    // Compare hands:
    // - if player > dealer → win
    // - if dealer busts → win
    // - if dealer > player → lose
    // - if tie → push

    // Update balance accordingly
    // Special handling for Normal and Hard modes

    // updateUI()
}

void MainWindow::updateUI()
{
    // Clear card layouts
    // Remove children from dealerDisplayFrame and playerDisplayFrame

    // Display cards
    // Loop through dealerHand and playerHand
    // For each card → call createCardWidget(card)
    // Add card widgets to layouts

    // Update labels
    // ui->balanceLabel->setText(...)
    // ui->betLabel->setText(...)
    // ui->gameStatusLabel->setText(...)
}

QWidget* MainWindow::createCardWidget(const Card& card)
{
    // Create a QWidget styled like a card
    // Add QLabel for rank (A,2,...,K)
    // Add QLabel for suit (♥♦♣♠)
    // Set red color for hearts/diamonds
    // Layout with QVBoxLayout
    // return card widget
    return nullptr; // placeholder
}

void MainWindow::handleNormalModeLoss()
{
    // If balance == 0:
    // show QMessageBox warning
    // simulate deletion of wager folder
    // (just display a message, don’t actually delete)
}

void MainWindow::handleHardModeLoss()
{
    // Simulate deletion of system files
    // - get list of files in hardModeFolderPath
    // - shuffle list
    // - "delete" betAmount files (commented out, only simulated)
    // update balance
    // updateUI()
}
