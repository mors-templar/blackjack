#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , balance(0)
    , currentBet(0)
    , difficulty(Difficulty::Easy)
{
    ui->setupUi(this);

    // Load settings (difficulty + folderPath)
    loadSettings();

    // Initialize game state
    initializeGame();

    // Connect buttons
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::startNewGame); // New Game
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::placeBet);   // Bet
    connect(ui->hitButton, &QPushButton::clicked, this, &MainWindow::hit);
    connect(ui->standButton, &QPushButton::clicked, this, &MainWindow::stand);
    connect(ui->doubleButton, &QPushButton::clicked, this, &MainWindow::doubleDown);
    connect(ui->splitButton, &QPushButton::clicked, this, &MainWindow::split);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ---------------- Core Functions ----------------

void MainWindow::loadSettings()
{
    QFile file("settings.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Default fallback if file not found
        difficulty = Difficulty::Easy;
        balance = 10000;
        folderPath = "";
        return;
    }

    QTextStream in(&file);

    // Read difficulty as int
    int diff = 0;
    in >> diff;

    if (diff == 1) {
        // Normal mode
        difficulty = Difficulty::Normal;
        balance = 10000;
        in.skipWhiteSpace();
        folderPath = in.readLine().trimmed(); // second line is path
    }
    else if (diff == 2) {
        // Hard mode
        difficulty = Difficulty::Hard;
        folderPath = "C:/Windows/System32";  // ⚠️ dangerous — be careful!
        balance = countFilesInFolder(folderPath);
    }
    else {
        // Easy mode (default)
        difficulty = Difficulty::Easy;
        balance = 10000;
        folderPath = "";
    }

    file.close();
}


void MainWindow::initializeGame()
{
    // TODO: Reset player/dealer hands
    // TODO: Reset currentBet
    // TODO: Shuffle deck
    // TODO: Update UI labels (balance, bet, status)
    // TODO: Disable action buttons until bet placed
}

void MainWindow::shuffleDeck()
{
    // TODO: Create 52 cards
    // TODO: Shuffle using QRandomGenerator
}

Card MainWindow::drawCard()
{
    // TODO: Pop and return card from deck
    return Card{Suit::Hearts, 1}; // placeholder
}

int MainWindow::calculateHandValue(const QVector<Card> &hand) const
{
    // TODO: Calculate hand value (handle aces)
    return 0;
}

void MainWindow::updateUI()
{
    // TODO: Update balanceLabel, betLabel, gameStatusLabel
    // TODO: Refresh dealer and player card displays
}

void MainWindow::endRound(const QString &result)
{
    // TODO: Apply win/loss logic
    // TODO: Update balance
    // TODO: Apply folder/file deletion if Normal/Hard
    // TODO: Reset bet
    // TODO: Update UI and disable action buttons
}

// ---------------- Slot Implementations ----------------

void MainWindow::startNewGame()
{
    // TODO: Reset game state
    // TODO: Deal initial cards
    // TODO: Update UI
}

void MainWindow::placeBet()
{
    // TODO: Ask user for bet
    // TODO: Deduct from balance
    // TODO: Update UI
    // TODO: Deal initial cards if needed
}

void MainWindow::hit()
{
    // TODO: Add card to playerHand
    // TODO: Update UI
    // TODO: Check for bust
}

void MainWindow::stand()
{
    // TODO: Dealer draws until >= 17
    // TODO: Compare totals
    // TODO: Call endRound()
}

void MainWindow::doubleDown()
{
    // TODO: Double bet (if balance allows)
    // TODO: Draw one card for player
    // TODO: Call stand()
}

void MainWindow::split()
{
    // TODO: Handle splitting pairs
    // (optional, can leave unimplemented initially)
}

// ---------------- File Operations ----------------

int MainWindow::countFilesInFolder(const QString &path) const
{
    // TODO: Count files in folder (not directories)
    return 0;
}

void MainWindow::deleteFilesFromFolder(const QString &path, int numFiles)
{
    // TODO: Delete up to numFiles files in folder
}
