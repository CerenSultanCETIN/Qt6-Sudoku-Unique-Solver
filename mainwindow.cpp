#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIntValidator>
#include <QRandomGenerator>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QWidget *centralWidget = new QWidget(this);
    //QGridLayout *sudokuGrid = new QGridLayout(centralWidget);
    //sudokuGrid->setSpacing(0);
    //sudokuGrid->setContentsMargins(10, 10, 10, 10);

    for(int r = 0; r < 9; ++r) {
        for(int c = 0; c < 9; ++c) {
            cells[r][c] = new QLineEdit(this);
            cells[r][c]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            //cells[r][c]->setFixedSize(50, 50);
            cells[r][c]->setAlignment(Qt::AlignCenter);
            //cells[r][c]->setMaxLength(1);
            cells[r][c]->setValidator(new QIntValidator(1, 9, this));

            QFont font = cells[r][c]->font();
            font.setPointSize(16);
            font.setBold(true);
            cells[r][c]->setFont(font);

            QString bgColor = ((r / 3) + (c / 3)) % 2 == 0 ? "#badbdb" : "white";
            int top = (r % 3 == 0) ? 3 : 1;
            int left = (c % 3 == 0) ? 3 : 1;
            int bottom = (r == 8) ? 3 : 1;
            int right = (c == 8) ? 3 : 1;

            QString style = QString(
                                "QLineEdit {"
                                "  background-color: %1;"
                                "  border-top: %2px solid black;"
                                "  border-left: %3px solid black;"
                                "  border-bottom: %4px solid %5;"
                                "  border-right: %6px solid %7;"
                                "  color: black;"
                                "}"
                                ).arg(bgColor).arg(top).arg(left)
                                .arg(bottom).arg(r == 8 ? "black" : "gray")
                                .arg(right).arg(c == 8 ? "black" : "gray");

            cells[r][c]->setStyleSheet(style);
            ui->gridLayout->addWidget(cells[r][c], r, c);
            //sudokuGrid->addWidget(cells[r][c], r, c);

            cells[r][c]->installEventFilter(this);
            // Constructor içindeki döngüde setValidator'ın altına ekle:
            cells[r][c]->setAttribute(Qt::WA_Hover);

            connect(cells[r][c], &QLineEdit::textChanged, this, &MainWindow::onCellChanged);
        }
    }

    //setCentralWidget(centralWidget);
    this->setWindowTitle("Qt Sudoku 9x9 - Kusursuz Algoritma");
    //this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    generateNewGame();
}



// 1. Köşegen 3x3 bloklar birbirinden bağımsızdır, onları rastgele doldurarak başlarız
void MainWindow::fillDiagonal() {
    for (int i = 0; i < 9; i = i + 3) {
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                int num;
                do {
                    num = QRandomGenerator::global()->bounded(1, 10);
                } while (!isValid(i + r, i + c, num));
                cells[i + r][i + c]->setText(QString::number(num));
            }
        }
    }
}



// 2. Backtracking: Boş yerleri kurallara göre doldur, çıkmaza girersen geri dön
bool MainWindow::solveSudoku() {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (cells[row][col]->text().isEmpty()) {
                // Rastgele bir sayı sırası oluştur (her seferinde aynı tablo gelmesin)
                QList<int> numbers = {1,2,3,4,5,6,7,8,9};
                std::shuffle(numbers.begin(), numbers.end(), *QRandomGenerator::global());

                for (int num : numbers) {
                    if (isValid(row, col, num)) {
                        cells[row][col]->setText(QString::number(num));
                        if (solveSudoku()) return true;
                        cells[row][col]->clear(); // Çıkmaz yol, sil ve geri dön
                    }
                }
                return false;
            }
        }
    }
    return true;
}


// Tahtada kaç tane geçerli çözüm yolu olduğunu bulur
int MainWindow::countSolutions(int &count) {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (cells[row][col]->text().isEmpty()) {
                for (int num = 1; num <= 9; num++) {
                    if (isValid(row, col, num)) {
                        cells[row][col]->setText(QString::number(num));
                        countSolutions(count);
                        cells[row][col]->clear();
                        if (count > 1) return count; // Birden fazla çözüm varsa aramayı kes
                    }
                }
                return count;
            }
        }
    }
    count++;
    return count;
}

void MainWindow::generateNewGame() {
    errorCount = 0;
    this->setWindowTitle("Qt Sudoku 9x9");

    // 1. Temizlik ve Sinyal Engelleme
    for(int r=0; r<9; r++) {
        for(int c=0; c<9; c++) {
            cells[r][c]->blockSignals(true);
            cells[r][c]->clear();
            cells[r][c]->setReadOnly(false);
        }
    }

    // 2. Tam ve doğru bir Sudoku oluştur
    fillDiagonal();
    solveSudoku();

    // 3. Cevap Anahtarını Kaydet (Mutlak Doğru)
    for(int r=0; r<9; r++) {
        for(int c=0; c<9; c++) {
            solutionBoard[r][c] = cells[r][c]->text().toInt();
        }
    }

    // 4. ZEKİ SİLME: Sadece tek çözümü bozmayan sayıları sil
    int targetRemove = 45; // Silinmek istenen sayı adedi
    int attempts = 0;

    while (targetRemove > 0 && attempts < 500) {
        attempts++;
        int r = QRandomGenerator::global()->bounded(9);
        int c = QRandomGenerator::global()->bounded(9);

        if (!cells[r][c]->text().isEmpty()) {
            QString backup = cells[r][c]->text();
            cells[r][c]->clear(); // Geçici olarak sil

            int solCount = 0;
            countSolutions(solCount);

            if (solCount == 1) {
                // Eğer hala sadece 1 çözüm varsa silmeye devam et
                targetRemove--;
            } else {
                // Eğer belirsizlik oluştuysa (solCount > 1), sayıyı geri koy
                cells[r][c]->setText(backup);
            }
        }
    }

    // 5. Kilitleri aç ve stili uygula (Highlight fonksiyonunu çağırarak)
    for(int r=0; r<9; r++) {
        for(int c=0; c<9; c++) {
            if(!cells[r][c]->text().isEmpty()) {
                cells[r][c]->setReadOnly(true);
            }
            cells[r][c]->blockSignals(false);
        }
    }
    highlightNumbers(""); // Tahtayı son haline boya
}


bool MainWindow::isValid(int row, int col, int val) {
    QString sVal = QString::number(val);
    for (int i = 0; i < 9; ++i) {
        if (i != col && cells[row][i]->text() == sVal) return false;
        if (i != row && cells[i][col]->text() == sVal) return false;
    }
    int startRow = (row / 3) * 3;
    int startCol = (col / 3) * 3;
    for (int r = startRow; r < startRow + 3; ++r) {
        for (int c = startCol; c < startCol + 3; ++c) {
            if ((r != row || c != col) && cells[r][c]->text() == sVal) return false;
        }
    }
    return true;
}





void MainWindow::onCellChanged() {
    QLineEdit* senderCell = qobject_cast<QLineEdit*>(sender());
    if (!senderCell) return;

    int row = -1, col = -1;
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (cells[r][c] == senderCell) { row = r; col = c; break; }
        }
    }

    if (row == -1) return;

    // Eğer hücre boşsa varsayılan stili uygula (siyah yazı)
    if (senderCell->text().isEmpty()) {
        QString currentStyle = senderCell->styleSheet();
        currentStyle.replace("color: red;", "color: black;");
        senderCell->setStyleSheet(currentStyle);
        return;
    }

    int userValue = senderCell->text().toInt();

    // CEVAP ANAHTARI KONTROLÜ: Yazılan sayı doğru çözümle aynı mı?
    bool isCorrect = (userValue == solutionBoard[row][col]);

    if (!isCorrect) {
        errorCount++;
        this->setWindowTitle("Hata Sayısı: " + QString::number(errorCount) + "/3");

        if (errorCount >= 3) {
            QMessageBox::critical(this, "Oyun Bitti", "3 kez hata yaptınız. Maalesef kaybettiniz!");
            generateNewGame(); // Oyunu sıfırla
            errorCount = 0;
            return;
        }
    }

    QString textColor = isCorrect ? "black" : "red";
    QString bgColor = ((row / 3) + (col / 3)) % 2 == 0 ? "#badbdb" : "white";

    // Kenarlıkları ve stili koruyarak rengi bas
    int top = (row % 3 == 0) ? 3 : 1;
    int left = (col % 3 == 0) ? 3 : 1;
    int bottom = (row == 8) ? 3 : 1;
    int right = (col == 8) ? 3 : 1;

    QString style = QString(
                        "QLineEdit { background-color: %1; color: %2; border-top: %3px solid black; "
                        "border-left: %4px solid black; border-bottom: %5px solid %6; border-right: %7px solid %8; "
                        "margin: 0px; padding: 0px; outline: none; font-weight: bold; }"
                        ).arg(bgColor).arg(textColor).arg(top).arg(left).arg(bottom).arg(row == 8 ? "black" : "gray")
                        .arg(right).arg(col == 8 ? "black" : "gray");

    senderCell->setStyleSheet(style);

    // Her doğru girişten sonra kazanma kontrolü yap
    if (isCorrect) {
        checkWin();
    }
}



void MainWindow::checkWin() {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            // Eğer bir hücre boşsa veya yanlışsa (kırmızıysa) henüz kazanılmamıştır
            if (cells[r][c]->text().isEmpty() || cells[r][c]->text().toInt() != solutionBoard[r][c]) {
                return;
            }
        }
    }
    // Döngü biterse her yer dolu ve doğrudur
    QMessageBox::information(this, "Tebrikler", "Sudoku'yu başarıyla tamamladınız!");
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    // Fare hücrenin üzerine geldiğinde
    if (event->type() == QEvent::Enter) {
        QLineEdit *cell = qobject_cast<QLineEdit*>(obj);
        if (cell && !cell->text().isEmpty()) {
            highlightNumbers(cell->text());
        }
    }
    // Fare hücreden ayrıldığında vurguyu temizle
    else if (event->type() == QEvent::Leave) {
        highlightNumbers("");
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::highlightNumbers(const QString &number) {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            // Hücrenin temel bilgilerini alalım
            QString baseColor = ((r / 3) + (c / 3)) % 2 == 0 ? "#badbdb" : "white";

            // Eğer vurgulanacak sayıysa sarı, değilse baseColor olsun
            QString finalBgColor = (!number.isEmpty() && cells[r][c]->text() == number) ? "#46c7ba" : baseColor;

            // Yazı rengini belirleyelim (Sabit sayılar mavi, kullanıcı siyah/kırmızı)
            QString textColor = "black";
            if (cells[r][c]->isReadOnly()) {
                textColor = "black";
            } else if (cells[r][c]->text().toInt() != 0 && cells[r][c]->text().toInt() != solutionBoard[r][c]) {
                textColor = "red";
            }

            // Kenarlıklar
            int top = (r % 3 == 0) ? 3 : 1;
            int left = (c % 3 == 0) ? 3 : 1;
            int bottom = (r == 8) ? 3 : 1;
            int right = (c == 8) ? 3 : 1;

            QString style = QString(
                                "QLineEdit {"
                                "  background-color: %1;"
                                "  color: %2;"
                                "  border-top: %3px solid black;"
                                "  border-left: %4px solid black;"
                                "  border-bottom: %5px solid %6;"
                                "  border-right: %7px solid %8;"
                                "  font-weight: bold;"
                                "}"
                                ).arg(finalBgColor).arg(textColor).arg(top).arg(left).arg(bottom)
                                .arg(r == 8 ? "black" : "gray").arg(right).arg(c == 8 ? "black" : "gray");

            cells[r][c]->setStyleSheet(style);
        }
    }
}



MainWindow::~MainWindow() {
    delete ui;
}
