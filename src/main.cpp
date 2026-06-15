#include <QApplication>
#include <QSurfaceFormat>
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
            Eigen::Vector3f position(data[0], data[1], data[2]);
            Eigen::Vector3f velocity(data[3], data[4], data[5]);
            float mass = data[6];
            CelestialBody body(position, velocity, mass);
            engine.addBody(body);
            bodiesLoaded++;
        } else {
            std::cerr << "Invalid line format (expected 7 values: x,y,z,vx,vy,vz,m): " << line << std::endl;
        }
    }

    file.close();

    std::cout << "Data loaded\n";
    return true;
}

void runHeadlessMode(int steps, float deltaTime, ForceAlgorithm algo, float theta) {
    std::cout << "--- RUNNING IN HEADLESS MODE ---\n";
    std::cout << "Algorithm: " << (algo == ForceAlgorithm::Naive ? "Naive" : "Barnes-Hut") << "\n";
    if (algo == ForceAlgorithm::BarnesHut) {
        std::cout << "Theta: " << theta << "\n";
    }

    GravityEngine engine;
    engine.setForceAlgorithm(algo);
    engine.setTheta(theta);

    std::ofstream csvFile("simulation_results.csv");
    if (!csvFile.is_open()) {
        std::cerr << "Warning: could not open CSV file 'simulation_results.csv' for writing. Continuing without CSV output." << std::endl;
    }

    if (!inputFile.empty()) {
        if (!loadUniverseFromFile(inputFile, engine)) {
            return;
        }

        csvFile << "Step,Time";
        for (int i = 0; i < engine.getBodies().size(); i++) {
            csvFile << ",x" << i+1 << ",y" << i+1 << ",z" << i+1 << ",v_x" << i+1 << ",v_y" << i+1 << ",v_z" << i+1 << ",m" << i+1;
        } csvFile << "\n";

        std::cout << "Step \t Time";
        for (int i = 0; i < engine.getBodies().size(); i++) {
            std::cout << "\t B" << i+1 << "_X \t B" << i+1 << "_Y \t B" << i+1 << "_Z";
        } std::cout << "\n";

        std::cout << "Simulation started...\n";
        for (int i = 0; i < steps; i++) {
            engine.update(deltaTime);
            if (i % 100 == 0 || i == steps - 1) {
                csvFile << i << "," << i * deltaTime;
                for (const auto& body : engine.getBodies()) {
                    csvFile << "," << body.position.x() << "," << body.position.y() << "," << body.position.z()
                            << "," << body.velocity.x() << "," << body.velocity.y() << "," << body.velocity.z()
                            << "," << body.mass;
                } csvFile << "\n";
            }
            if (i % 100 == 0) {
                std::cout << i << "\t" << (i * deltaTime);
                for (const auto& body : engine.getBodies()) {
                    std::cout << "\t" << body.position.x() << "\t" << body.position.y() << "\t" << body.position.z();
                } std::cout << "\n";
            }
        }
    } else {
        std::cout << "No input file specified. Using default universe (Earth only).\n";

        CelestialBody earth(Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), 5.97e24f);
        engine.addBody(earth);
        engine.initializeForces();

        csvFile << "Step,Time,x1,y1,z1,v_x1,v_y1,v_z1,m1\n";
        std::cout << "Step \t Time \t X \t Y \t Z\n";

        std::cout << "Simulation started...\n";
        for (int i = 0; i < steps; i++) {
            engine.update(deltaTime);
            const auto& body = engine.getBodies()[0];
            if (i % 100 == 0 || i == steps - 1) {
                csvFile << i << "," << i * deltaTime << "," << body.position.x() << "," << body.position.y() << "," << body.position.z()
                        << "," << body.velocity.x() << "," << body.velocity.y() << "," << body.velocity.z()
                        << "," << body.mass << "\n";
            }
            if (i % 100 == 0) {
                std::cout << i << "\t" << (i * deltaTime) << "\t" << body.position.x() << "\t" << body.position.y() << "\t" << body.position.z() << "\n";
            }
        }
    }

    std::cout << "--- END OF HEADLESS MODE ---\n";
}

int main(int argc, char *argv[]) {
    bool isHeadless = false;
    ForceAlgorithm algo = ForceAlgorithm::Naive;
    float theta = 0.5f;
    int steps = 1000;
    float dt = 0.01f;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--headless") {
            isHeadless = true;
        } else if (arg == "--file") {
            if (i + 1 < argc) {
                inputFile = argv[++i];
            } else {
                std::cerr << "Error: --file requires a filename argument.\n";
            }
        } else if (arg == "--algo") {
            if (i + 1 < argc) {
                std::string algoStr = argv[++i];
                if (algoStr == "barnes-hut") {
                    algo = ForceAlgorithm::BarnesHut;
                } else if (algoStr == "naive") {
                    algo = ForceAlgorithm::Naive;
                } else {
                    std::cerr << "Unknown algorithm: " << algoStr << ". Using Naive.\n";
                }
            }
        } else if (arg == "--theta") {
            if (i + 1 < argc) {
                theta = std::stof(argv[++i]);
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            }
        } else if (arg == "--dt") {
            if (i + 1 < argc) {
                dt = std::stof(argv[++i]);
            }
        }
    }

    if (isHeadless) {
        runHeadlessMode(steps, dt, algo, theta);
        return 0;
    } // Else -> run in window mode
    QSurfaceFormat format;
    format.setVersion(4, 4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);

    MainWindow window;
    window.resize(1024, 768);
    window.setWindowTitle("N-Body Simulation");
    window.show();

    return a.exec();
}