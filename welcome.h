#ifndef WELCOME_H
#define WELCOME_H

#include <QDialog>
#include <QString>

namespace Ui {
class Welcome;
}

class Welcome : public QDialog
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome();

signals:
    void accepted(int difficulty, const QString& folderPath);

private slots:
    void on_nextButton_clicked();
    void on_backButton_clicked();
    void on_finishButton_clicked();

    void on_browseButton_clicked();

private:
    Ui::Welcome *ui;
};

#endif // WELCOME_H
