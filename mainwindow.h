#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLineEdit* cells[9][9];
    int solutionBoard[9][9];
    void generateNewGame();
    bool isValid(int row, int col, int val);
    void fillDiagonal();
    bool solveSudoku();
    int errorCount = 0; // Hata sayacı
    void checkWin();    // Kazanma kontrol fonksiyonu
    void highlightNumbers(const QString &number);
    int countSolutions(int &count); // Kaç farklı çözüm olduğunu kontrol eder

private slots:
    void onCellChanged(); // Her giriş kontrol edilecek

protected:
    // Fare hareketlerini yakalamak için gerekli
    bool eventFilter(QObject *obj, QEvent *event) override;

};
#endif // MAINWINDOW_H
