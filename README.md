Sudoku Master: Backtracking & Uniqueness Engine (Qt6/C++)
<img width="604" height="637" alt="image" src="https://github.com/user-attachments/assets/1c02be1e-d388-4245-a9b2-931209df35b2" /><img width="612" height="641" alt="image" src="https://github.com/user-attachments/assets/f336d8f2-6200-4792-850b-4407f2eb20f6" />


Sudoku Master: Advanced Puzzle Generation & UI Logic

Bu proje, Ostim Teknik Üniversitesi'nde bir yazılım mühendisi adayı olarak geliştirdiğim, algoritmik derinliği yüksek bir masaüstü uygulamasıdır. Projenin temel amacı, klasik Sudoku kurallarını modern yazılım teknikleriyle harmanlayarak kusursuz bir kullanıcı deneyimi sunmaktır.

Öne Çıkan Mühendislik Detayları:

Backtracking Algoritması: Rekürsif yapıda çalışan motor, saniyeler içinde geçerli ve tam bir Sudoku matrisi üretir.

Zeki Silme (Unique Solution): Rastgele silme yerine, her adımda çözüm sayısını (solCount) denetleyen bir algoritma kullanılarak tablonun tek bir çözüme sahip olması garanti edilir.

Olay Filtreleme (Event Filtering): Fare imlecinin altındaki rakamı tespit ederek tablodaki tüm eşleşen rakamları vurgulayan dinamik bir UI mantığı kurgulanmıştır.

qmake Build Sistemi: Proje, Qt’nun endüstri standardı olan qmake sistemiyle yapılandırılmış ve derlenmiştir.


🌟 Öne Çıkan Teknik Özellikler
🔍 Akıllı Sayı Vurgulama (Smart Number Highlighting): Fare (hover) ile bir rakamın üzerine gelindiğinde, tüm matristeki aynı rakamlar anlık olarak parlar. Bu özellik, eventFilter ve Qt::WA_Hover kullanılarak merkezi bir stil yönetim sistemiyle geliştirilmiştir.

⚖️ Zeki Silme ve Tek Çözüm Garantisi (Smart Erasing): Sudoku tahtasından 45 sayı silinirken, her silme işleminde bir çözüm sayıcı (solution counter) çalışır. Eğer bir sayının silinmesi birden fazla çözüm ihtimali (Deadly Pattern) doğuruyorsa, algoritma o sayıyı geri koyarak farklı bir hücre dener.

🛡️ Hata ve Hak Mekanizması: Kullanıcı girişleri anlık olarak çözüm anahtarıyla kıyaslanır. Hatalı girişlerde rakam kırmızı renge döner. 3 hata sınırına ulaşıldığında oyun otomatik olarak sona erer.

📱 Responsive UI Architecture: QSizePolicy::Expanding politikası sayesinde uygulama penceresi dikey ve yatayda esnerken Sudoku hücreleri formunu bozmadan tüm alanı kaplar.

🛠️ Teknik Mimari ve Metodoloji
1. Motor: Backtracking (Geri İzleme)
Sıfırdan geçerli bir Sudoku tahtası üretmek için Recursive Backtracking algoritması kullanılmıştır. Rastgele sayı yerleşimi sırasında bir çıkmaza girildiğinde, algoritma bellek yığınını (stack) kullanarak önceki adımlara geri döner ve yeni ihtimalleri dener.

2. Derleme Sistemi: qmake
Proje, Qt'nun yerel derleme sistemi olan qmake (.pro) kullanılarak yapılandırılmıştır. Bu, kütüphane bağımlılıklarını yönetmeyi ve çapraz platform (Cross-Platform) derleme süreçlerini standartlaştırır.

3. Kullanıcı Etkileşimi (Event Filtering)
Hücrelerin fare hareketlerine tepki vermesi için Qt'nun eventFilter mekanizması uygulanmıştır. Her bir QLineEdit hücresi merkezi bir MainWindow filtresine takılarak QEvent::Enter ve QEvent::Leave sinyalleri yakalanır.
