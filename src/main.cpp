#include "Application.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <thread>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

std::atomic<bool> running{true};

int main(int argc, char* argv[])
{
    lava::Application App;
    
    QApplication app(argc, argv);
    QWidget editorWindow;
    editorWindow.setWindowTitle("Editor");
    QVBoxLayout* layout = new QVBoxLayout(&editorWindow);

    QLabel* label = new QLabel("Luminosità:");
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setValue(100); // 100%
    
    layout->addWidget(label);
    layout->addWidget(slider);

    QPushButton* stopButton = new QPushButton("Stop Engine");
    layout->addWidget(stopButton);
    QObject::connect(stopButton, &QPushButton::clicked, [&](){
        running.store(false);
        App.Stop();
        editorWindow.close();
    });


    editorWindow.resize(300, 150);
    editorWindow.show();


    try
    {
        App.Run(running);
        std::thread graphicsThread([&app](){app.exec();});
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // if (graphicsThread.joinable())
    //     graphicsThread.join();
    
    return 0;
};
