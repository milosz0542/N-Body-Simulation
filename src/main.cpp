#include <QApplication>
#include "gui/MainWindow.h"
#include "core/GravityEngine.h"
#include "core/CelestialBody.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string inputFile;

bool loadUniverseFromFile(const std::string& filename, GravityEngine& engine) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout << "Could not open file: " << filename << std::endl;
        return false;
    }

    std::string line;

    std::getline(file, line);

    int bodiesLoaded = 0;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string token;
        std::vector<float> data;

        while (std::getline(iss, token, ',')) {
            try {
                data.push_back(std::stof(token));
            } catch (const std::invalid_argument& e) {
                std::cout << "Invalid argument: " << token << std::endl;
                return false;
            }
        }

        if (data.size() == 7) {
            float mass = data[0];
            Eigen::Vector3f position(data[1], data[2], data[3]);
            Eigen::Vector3f velocity(data[4], data[5], data[6]);
            CelestialBody body(position, velocity, mass);
            engine.addBody(body);
            bodiesLoaded++;
        } else {
            std::cerr << "Invalid line format: " << line << std::endl;
        }
    }

    file.close();

    std::cout << "Data loaded\n";
    return true;
}

void runHeadlessMode(int steps, float deltaTime) {
    std::cout << "--- RUNNING IN HEADLESS MODE ---\n";

    GravityEngine engine;

    std::ofstream csvFile("simulation_results.csv");

    if (!inputFile.empty()) {
        loadUniverseFromFile(inputFile, engine);

        csvFile << "Step,Time";
        std::cout << "Step \t Time";
        for (int i = 0; i < engine.getBodies().size(); i++) {
            csvFile << ",Planet_" << i+1 << "_X,Planet_" << i+1 << "_Y,Planet_" << i+1 << "_Z";
            std::cout << "\t Planet_" << i+1 << "_X \t Planet_" << i+1 << "_Y \t Planet_" << i+1 << "_Z";
        } csvFile << "\n"; std::cout << "\n";

        std::cout << "Simulation started...\n";
        for (int i = 0; i < steps; i++) {
            engine.update(deltaTime);
            if (i % 100 == 0) {
                csvFile << i << "," << i * deltaTime;
                for (int j = 0; j < engine.getBodies().size(); j++) {
                    csvFile << "," << engine.getBodies()[j].position.x() << "," << engine.getBodies()[j].position.y() << "," << engine.getBodies()[j].position.z();
                } csvFile << "\n";
            }
            std::cout << i << "\t" << (i * deltaTime);
            for (int j = 0; j < engine.getBodies().size(); j++) {
                std::cout << "\t" << engine.getBodies()[j].position.x() << "\t" << engine.getBodies()[j].position.y() << "\t" << engine.getBodies()[j].position.z();
            } std::cout << "\n";
        }
    } else {
        std::cout << "No input file specified. Using default universe.\n";

        csvFile << "Step,Time,X,Y,Z\n";
        CelestialBody earth(Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), 5.97e24f);
        engine.addBody(earth);
        engine.update(deltaTime);

        std::cout << "Simulation started...\n";
        std::cout << "Step \t Time \t X \t Y \t Z\n";
        for (int i = 0; i < steps; i++) {
            engine.update(deltaTime);
            if (i % 100 == 0) {
                Eigen::Vector3f position = earth.position;
                float currentTime = i * deltaTime;
                csvFile << i << "," << currentTime << "," << position.x() << "," << position.y() << "," << position.z() << "\n";
            }
            std::cout << i << "\t" << (i * deltaTime) << "\t" << earth.position.x() << "\t" << earth.position.y() << "\t" << earth.position.z() << "\n";
        }
    }

    std::cout << "--- END OF HEADLESS MODE ---\n";
}

int main(int argc, char *argv[]) {
    bool isHeadless = false;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--headless") { // Run in headless
            isHeadless = true;
        } else if (std::string(argv[i]) == "--file") { // Provide universe from file
            if (i + 1 < argc) {
                inputFile = argv[i + 1];
                ++i; // Skip the filename argument
            } else {
                std::cerr << "Error: --file option requires a filename argument.\n";
            }
        }
    }

    if (isHeadless) {
        runHeadlessMode(1000, 0.01f);
        return 0;
    } // Else -> run in window mode
    
    QApplication a(argc, argv);

    MainWindow window;
    window.resize(1024, 768);
    window.setWindowTitle("N-Body Simulation");
    window.show();

    return a.exec();
}