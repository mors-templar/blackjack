#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// Enumeration for card suits
enum class Suit {
    Hearts,
    Diamonds,
    Clubs,
    Spades
};

// Enumeration for difficulty modes (matches settings.txt int)
enum class Difficulty {
    Easy = 0,
    Normal = 1,
    Hard = 2
};

// Struct representing a card
struct Card {
    Suit suit; // symbol on card (heart , clubs etc)
    QString rank; // the type of card (jack , king , ace , 2 , 3 , etc)
    int value;    // 1 or 11 = Ace, 2–10 = Number, 10 = Jack, Queen, King
    bool isAce; // handle ace logic

};


// Main Window class
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // UI button actions
    void startNewGame();
    void placeBet();
    void hit();
    void stand();
    void doubleDown();
    void split();

private:
    QString suitToSymbol(Suit suit);
    Ui::MainWindow *ui;

    // Game state
    QVector<Card> deck;
    QVector<Card> playerHand;
    QVector<Card> dealerHand;

    const int DEFAULT_BALANCE = 10000;
    int numDecks = 1;
    int balance;
    int currentBet;
    Difficulty difficulty;
    QString folderPath;

    // --- Core functions ---
    void loadSettings();
    void initializeGame();
    void shuffleDeck();
    Card drawCard();

    int calculateHandValue(const QVector<Card> &hand) const;
    void updateUI();
    void endRound(bool UserBust , bool DealerBust);

    // --- File operations ---
    int countFilesInFolder(const QString &path) const;
    void deleteFilesFromFolder(const QString &path, int numFiles);
};

#endif // MAINWINDOW_H
