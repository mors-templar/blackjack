#ifndef WELCOME_H
#define WELCOME_H

#include <QStackedWidget>

namespace Ui {
class Welcome;
}

class Welcome : public QStackedWidget
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = nullptr);
    ~Welcome();

private:
    Ui::Welcome *ui;
};

#endif // WELCOME_H
