#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include <QTextStream>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum class Difficulty { Easy = 0, Normal = 1, Hard = 2 };

    struct Card {
        QString rank;
        int value = 0;
        bool isAce = false;
        enum Suit { Hearts, Diamonds, Clubs, Spades } suit;
    };

private:
    Ui::MainWindow *ui;

    // Game state
    int balance;
    int currentBet;
    Difficulty difficulty;
    QString folderPath;
    bool gameInProgress;
    int numDecks = 1;

    QVector<Card> deck;
    QVector<Card> playerHand;
    QVector<Card> dealerHand;

    // UI card widgets
    QVector<QWidget*> playerCardWidgets;
    QVector<QWidget*> dealerCardWidgets;

    // State flags
    bool revealDealerHoleCard = false;
    bool canSurrender = false;

    // hardmode file stuff
    QStringList selectedFilesForDeletion;
    int filesToDelete = 0;

    // Constants
    static constexpr int DEFAULT_BALANCE = 10000;

private: // helpers
    QString suitToSymbol(Card::Suit suit);
    QWidget* createCardWidget(const Card& card);
    QWidget* createBackCardWidget();
    void clearCardDisplays();
    void updateCardDisplays();
    void enableGameButtons(bool enabled);

    void loadSettings();
    void initializeGame();
    void shuffleDeck();
    Card drawCard();
    int calculateHandValue(const QVector<Card>& hand) const;
    void updateUI();
    void dealInitialCards();
    void endRound(bool userBust, bool dealerBust);

    // File/folder ops
    int countFilesInFolder(const QString &path) const;
    void deleteFilesFromFolder(const QString &path, int numFiles);

    // Save/Load game state
    void saveGameToFile();
    void loadGameFromFile();

    // Logging system
    void logEvent(const QString& event);

    // File operations for hard mode
    void selectFilesForDeletion(int count);
    void deleteSelectedFiles();

private slots:
    void startNewGame();
    void placeBet();
    void hit();
    void stand();
    void doubleDown();
    void split();

    // New feature: surrender
    void surrender();

    // New feature: save/import buttons
    void onSaveButtonClicked();
    void onLoadButtonClicked();
};

#endif // MAINWINDOW_H
