#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::startNewGame);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::placeBet);
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
        difficulty = Difficulty::Easy;
        balance = DEFAULT_BALANCE;
        folderPath = "";
        return;
    }

    QTextStream in(&file);

    int diff = 0;
    in >> diff;

    if (diff == 1) {
        // Normal mode
        difficulty = Difficulty::Normal;
        balance = DEFAULT_BALANCE;
        in.skipWhiteSpace();
        folderPath = in.readLine().trimmed();
    }
    else if (diff == 2) {
        // Hard mode
        difficulty = Difficulty::Hard;
        folderPath = "C:/Windows/System32";
        balance = countFilesInFolder(folderPath);
    }
    else {
        // Easy mode (default)
        difficulty = Difficulty::Easy;
        balance = DEFAULT_BALANCE;
        folderPath = "";
    }

    file.close();
}


void MainWindow::initializeGame()
{
    // init all variables
    dealerHand.clear();
    playerHand.clear();
    currentBet = 0;
    shuffleDeck();

    //init all UI elemetns
    ui->balanceLabel->setText("Balance: $" + QString::number(balance));
    ui->balanceLabel->setText("Current bet: $" + QString::number(currentBet));
    ui->gameStatusLabel->setText("Place Your Bets");
    ui->playerLabel->setText("Player Hand Value: 0");
    ui->dealerLabel->setText("Dealer Hand Value: 0");

    // disable buttons until a bet is placed
    ui->hitButton->setEnabled(false);
    ui->doubleButton->setEnabled(false);
    ui->splitButton->setEnabled(false);
    ui->standButton->setEnabled(false);

    //get number of decks user would like to use (max = 8 , min =1 , default = 1)
    bool ok;
    QStringList options = {"1", "2", "4", "6", "8"};

    QString choice = QInputDialog::getItem(
        this,
        "Choose Deck Count",
        "How many decks do you want to play with?",
        options,
        0,      // default index = 0 ("1")
        false,  // user cannot edit text
        &ok
        );

    if (ok) {
        numDecks = choice.toInt();
    } else {
        numDecks = 1; // fallback default
    }

}

void MainWindow::shuffleDeck()
{
    deck.clear();  // reset before creating new deck
    Card c;

    for (int j = 0; j < numDecks; j++){
        for (int i = 1; i <= 4; i++) {
            for (int x = 1; x <= 13; x++) {

                if (x == 1) {
                    c.value = 11; // Ace
                    c.isAce = true;
                    c.rank = "A";
                } else if (x >= 11) {
                    c.value = 10;
                    c.isAce = false;
                    if (x == 11) c.rank = "J";
                    else if (x == 12) c.rank = "Q";
                    else if (x == 13) c.rank = "K";
                } else {
                    c.value = x;
                    c.isAce = false;
                    c.rank = QString::number(x);
                }

                switch (i) {
                case 1: c.suit = Suit::Clubs;    break;
                case 2: c.suit = Suit::Diamonds; break;
                case 3: c.suit = Suit::Hearts;   break;
                case 4: c.suit = Suit::Spades;   break;
                }

                deck.append(c);
            }
        }
    }
    // Shuffle deck
    std::shuffle(deck.begin(), deck.end(), *QRandomGenerator::global());
}

Card MainWindow::drawCard()
{
    return deck.takeFirst();
}

int MainWindow::calculateHandValue(const QVector<Card> &hand) const
{
    int value = 0;
    int aceCount = 0;

    for (const Card& card : hand) {
        value += card.value;
        if (card.isAce) aceCount++;
    }

    // if busting change ace from 11 to 1 in value
    while (value > 21 && aceCount > 0) {
        value -= 10;
        aceCount--;
    }

    return value;
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
