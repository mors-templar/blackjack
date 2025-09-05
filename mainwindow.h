#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QRandomGenerator>

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

// Enumeration for game difficulty
enum class Difficulty {
    Easy,
    Normal,
    Hard
};

// Struct to represent a playing card
struct Card {
    QString rank;
    Suit suit;
    int value;
};

// Struct to represent a hand of cards
struct Hand {
    QVector<Card> cards;
    int value = 0;
    bool isBlackjack = false;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slots for button clicks
    void on_hitButton_clicked();
    void on_standButton_clicked();
    void on_doubleButton_clicked();
    void on_splitButton_clicked();
    void on_newGameButton_clicked();

private:
    // Private member variables for game state
    Ui::MainWindow *ui;
    QVector<Card> deck;
    Hand playerHand;
    Hand dealerHand;
    Difficulty gameDifficulty;
    qint64 balance;
    int betAmount;

    // For normal mode, this would store the path to the folder
    QString wagerFolderPath;

    // For hard mode, this would be a reference to the system directory
    QString hardModeFolderPath;

    // UI components for a card, as described by the user
    QWidget* createCardWidget(const Card& card);

    // Private helper functions for game logic
    void initializeGame(Difficulty difficulty);
    void setupConnections();
    void createDeck();
    void shuffleDeck();
    void dealInitialCards();
    Card dealCard();
    void updateUI();
    void updateStatus(const QString& message);
    int calculateHandValue(const Hand& hand);
    void checkBlackjack();
    void checkWinner();

    // Functions for the special game modes
    void handleNormalModeWin();
    void handleNormalModeLoss();
    void handleHardModeWin();
    void handleHardModeLoss();
};

#endif // MAINWINDOW_H
