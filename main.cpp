// MODIFIED FROM THIS EXAMPLE: https://doc.qt.io/qt-6/qtcore-threads-semaphores-example.html

#include <QCoreApplication>
#include <QtCore>
#include <iostream>

constexpr int DataSize = 10000;
constexpr int BufferSize = 50;
char buffer[BufferSize];

QSemaphore freeBytes(BufferSize);
QSemaphore usedBytes;

QSemaphore consoleAvailable(1);

class Producer : public QThread
{
public:
    void run() override
    {
        for (int i = 0; i < DataSize; ++i) {
            freeBytes.acquire();
            buffer[i % BufferSize] = "ACGT"[QRandomGenerator::global()->bounded(4)];

            consoleAvailable.acquire();
            std::cout << "INSERTING..." << buffer[i % BufferSize] << '\t' << i % BufferSize << '\t' << buffer << "\n";
            consoleAvailable.release();

            usedBytes.release();
        }
    }
};

class Consumer : public QThread
{
public:
    void run() override
    {
        for (int i = 0; i < DataSize; ++i) {
            usedBytes.acquire();
            const char auxCharacter = buffer[i % BufferSize];
            buffer[i % BufferSize] = '_';

            consoleAvailable.acquire();
            std::cout << "EXTRACTING..." << auxCharacter << '\t' << i % BufferSize << '\t' << buffer << "\n";
            consoleAvailable.release();

            freeBytes.release();
        }
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Starting..." << "\n";

    Producer producer;
    Consumer consumer;
    producer.start();
    consumer.start();
    producer.wait();
    consumer.wait();

    return EXIT_SUCCESS;
}
