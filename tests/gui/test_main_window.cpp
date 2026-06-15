#include <QtTest/QtTest>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QCheckBox>
#include <fstream>
#include "../../src/gui/MainWindow.h"

class TestMainWindow : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Create a dummy CSV for testing
        std::ofstream file("test_gui_load.csv");
        file << "x,y,z,vx,vy,vz,m\n";
        file << "100.0,0.0,0.0,0.0,10.0,0.0,1e24\n";
        file << "0.0,0.0,0.0,0.0,0.0,0.0,1e30\n";
        file.close();
    }

    void cleanupTestCase() {
        std::remove("test_gui_load.csv");
    }

    void testCSVLoading() {
        MainWindow w;
        w.show();
        QVERIFY(QTest::qWaitForWindowExposed(&w));

        w.loadFromCSV("test_gui_load.csv");

        // We can't easily access the private engine directly, but we can check the planet selector
        QComboBox* selector = w.findChild<QComboBox*>("", Qt::FindDirectChildrenOnly); 
        // Wait, MainWindow has multiple combo boxes. Let's find it by some other means if possible.
        // In MainWindow.cpp: planetSelector = new QComboBox(this);
        // It's not named via setObjectName.
        
        // Let's find all combo boxes
        QList<QComboBox*> combos = w.findChildren<QComboBox*>();
        QCOMPARE(combos.size(), 2); 
        // 1. algoSelector (in visTab)
        // 2. planetSelector (in camTab)
        
        // Let's find the one that has "Obiekt 0"
        QComboBox* planetSelector = nullptr;
        for (auto* cb : combos) {
            if (cb->findText("Obiekt 0", Qt::MatchContains) != -1) {
                planetSelector = cb;
                break;
            }
        }
        
        QVERIFY(planetSelector != nullptr);
        QCOMPARE(planetSelector->count(), 3); // Free camera + 2 objects
    }

    void testAlgorithmSelection() {
        MainWindow w;
        w.show();
        QVERIFY(QTest::qWaitForWindowExposed(&w));

        QList<QComboBox*> combos = w.findChildren<QComboBox*>();
        QComboBox* algoSelector = nullptr;
        for (auto* cb : combos) {
            if (cb->findText("Barnes-Hut", Qt::MatchContains) != -1) {
                algoSelector = cb;
                break;
            }
        }
        
        QVERIFY(algoSelector != nullptr);
        
        // Default should be Naive (index 0)
        QCOMPARE(algoSelector->currentIndex(), 0);
        
        // Change to Barnes-Hut
        algoSelector->setCurrentIndex(1);
        QCOMPARE(algoSelector->currentIndex(), 1);
    }
    
    void testThetaSlider() {
        MainWindow w;
        w.show();
        QVERIFY(QTest::qWaitForWindowExposed(&w));

        QList<QSlider*> sliders = w.findChildren<QSlider*>();
        // Sliders: speed, trail, resolution, theta
        QSlider* thetaSlider = nullptr;
        // In MainWindow.cpp thetaSlider has range 1-20, value 5
        for (auto* s : sliders) {
            if (s->minimum() == 1 && s->maximum() == 20) {
                thetaSlider = s;
                break;
            }
        }
        
        QVERIFY(thetaSlider != nullptr);
        QCOMPARE(thetaSlider->value(), 5);
        
        thetaSlider->setValue(10);
        QCOMPARE(thetaSlider->value(), 10);
    }
};

QTEST_MAIN(TestMainWindow)
#include "test_main_window.moc"
