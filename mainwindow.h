#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QRandomGenerator>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class Suit {
    Hearts,
    Diamonds,
    Clubs,
    Spades
};

struct Card {
    QString rank;
    Suit suit;
    int value;
    bool isAce;
};

enum class Difficulty {
    Easy,
    Normal,
    Hard
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startNewGame();
    void placeBet();
    void hit();
    void stand();
    void doubleDown();
    void split();

private:
    Ui::MainWindow *ui;

    // Game state
    QVector<Card> deck;
    QVector<Card> playerHand;
    QVector<Card> dealerHand;
    QList<QWidget*> dealerCardWidgets;
    QList<QWidget*> playerCardWidgets;


    int balance;
    int currentBet;
    int numDecks;
    bool gameInProgress;

    Difficulty difficulty;
    QString folderPath;

    // Constants
    const int DEFAULT_BALANCE = 10000;

    // Helpers
    QString suitToSymbol(Suit suit);
    QWidget* createCardWidget(const Card& card);
    void clearCardDisplays();
    void updateCardDisplays();
    void enableGameButtons(bool enabled);

    // Core game
    void loadSettings();
    void initializeGame();
    void shuffleDeck();
    Card drawCard();
    int calculateHandValue(const QVector<Card> &hand) const;
    void updateUI();
    void dealInitialCards();
    void endRound(bool userBust, bool dealerBust);

    // File ops
    int countFilesInFolder(const QString &path) const;
    void deleteFilesFromFolder(const QString &path, int numFiles);
};

#endif // MAINWINDOW_H
