
#include <iostream>
#include <math.h>
#include <mpi.h>
#include <pthread.h>
#include <random>
#include <cstring>

// mpicxx -O2 -o main main.cpp
// mpirun -np 2 ./main
// ssh -i new_key -p 22 hpcuser145@84.237.117.10 -X
// scp -i new_key ./lab5/main.cpp hpcuser145@84.237.117.10:~/lab5
// ~/sw/bin/mpecc -mpilog -O2 -o main main.cpp компиляция
// ~/sw/bin/jumpshot

// ssh -i lin_priv -p 2222 opp145@84.237.52.21
// Ov2P7z-qP
// mpecxx -mpilog -O2 -o 1 main.cpp
// scp -i lin_priv -P 2222 /home/dimka/OPP/lab5/main.cpp opp145@84.237.52.21:/home/opp145/lab5/

const int maxIters = 4;             // общее количество итераций всего алгоритма (количество списков задач, которые надо выполнить)
const int criticalTasksNum = 10;    // если у процесса осталось столько задач или меньше, он перестаёт отдавать свои задачи и начинает запрашивать новые
const int L = 2000;                 // параметр для вычисления сложности задачи
const int rootRank = 0;             // номер корневого процесса

int *taskList;                 // список задач
int tasksLeft;                 // количество невыполненных задач
int currentlyRunningTask;      // номер задачи, которая сейчас выполняется потоком-работником
int taskToSend;                // номер задачи, которая отправится, если какой-то процесс запросить задачу
pthread_mutex_t taskListMutex; // для того, чтобы не было конфликтов при изменении taskList и переменных, связанных с taskList
int iterCounter = 0;           // текущий номер итерации
double globalRes;              // сумма результатов вычислений. Изменяется только процессом-работником
int tasksDone;                 // количество выполненных заданий (на данной итерации)
int allTasks;                  // общее кол-во задач
int tasksPerProcess;           // максимальное количество задач на 1 процесс (изначальное количество задач у процесса после начала новой итерации)

// класс исключений для отслеживания ошибок
class myException : public std::exception {
private:
    const char *msg_;     // сообщение
    const int errorCode_; // код ошибки

public:
    explicit myException(const char *msg) : msg_(msg), errorCode_(0){};
    myException(const char *msg, int errorCode) : msg_(msg), errorCode_(errorCode){};
    const char *what() const noexcept override { return msg_; }
    int getErrorCode() const noexcept { return errorCode_; }
};

// инициализация списка задач (без заполнения)
void initTaskList() {
    pthread_mutex_lock(&taskListMutex);
    taskList = (int *)malloc(sizeof(int) * tasksPerProcess);
    if (taskList == nullptr) {
        throw new myException("Cannot initialize taskList");
    }
    pthread_mutex_unlock(&taskListMutex);
}

// заполение списка задач (для конкретного процесса)
void fillTaskList(const int rank, const int size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 10);

    for (int i = 0; i < tasksPerProcess; ++i) {
        // taskList[i] = abs(tasksPerProcess / 2 - i) + abs(rank - (iterCounter % size)) * L;
        taskList[i] = distrib(gen) * L;
    }
}

// заполнение списка задач и переменных перед началом новой итерации
void fillData(const int rank, const int size) {
    pthread_mutex_lock(&taskListMutex);
    fillTaskList(rank, size);
    tasksLeft = tasksPerProcess;
    currentlyRunningTask = 0;
    taskToSend = tasksPerProcess - 1;
    pthread_mutex_unlock(&taskListMutex);
    globalRes = 0;
    tasksDone = 0;
}

// запросить и принять задачи у других процессов. Возвращает количество принятых задач
int getNewTasks(const int rank, const int size) {
    // отправка запроса на задачи
    const bool needTasks = true;
    for (int p = 0; p < size; ++p) {
        if (p == rank) {
            continue;
        }
        MPI_Send(&needTasks, 1, MPI_CXX_BOOL, p, p, MPI_COMM_WORLD);
    }

    // приём задач
    int receivedTasksNum = 0;
    int receivedTask;
    for (int p = 0; p < size; ++p) {
        if (p == rank) {
            continue;
        }

        MPI_Recv(&receivedTask, 1, MPI_INT, p, rank + size, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (receivedTask != -1) { // -1 означает, что процесс p не может отдать задачу
            pthread_mutex_lock(&taskListMutex);
            tasksLeft++;
            taskToSend = (taskToSend + 1) % tasksPerProcess;
            taskList[taskToSend] = receivedTask;
            pthread_mutex_unlock(&taskListMutex);
            receivedTasksNum++;
        }
    }
    return receivedTasksNum;
}

// выполнить задачу
void executeTask() {
    double sum = 0;
    pthread_mutex_lock(&taskListMutex);
    tasksLeft--;
    int local_currentlyRunningTask = currentlyRunningTask;
    pthread_mutex_unlock(&taskListMutex);
    for (int i = 0; i < taskList[local_currentlyRunningTask]; ++i) {
        sum += sin(i);
    }
    globalRes += sum;
    tasksDone++;

    pthread_mutex_lock(&taskListMutex);
    taskList[currentlyRunningTask] = 0;
    currentlyRunningTask = (currentlyRunningTask + 1) % tasksPerProcess;
    pthread_mutex_unlock(&taskListMutex);
}

// инициализация массивов для вывода информации
void initBuffersForPrintInfo(int *&tasksDoneArr, double *&globalResArr, int *&tasksBegin,const int size) {
    tasksDoneArr = (int *)malloc(sizeof(int) * size);
    tasksBegin = (int *)malloc(sizeof(int) * size);
    globalResArr = (double *)malloc(sizeof(double) * size);
    if (tasksDoneArr == nullptr || globalResArr == nullptr || tasksBegin == nullptr) {
        free(tasksDoneArr);
        free(globalResArr);
        free(tasksBegin);
        throw new myException("Cannot malloc arrays for printInfo");
    }
}

// вывод информации на экран
void printInfo(const int rank, const int size, const double deltaTime, int *const tasksDoneArr, double *const globalResArr, int *const tasksBegin) {
    // сбор данных со всех процессов
    MPI_Gather(&tasksDone, 1, MPI_INT, tasksDoneArr, 1, MPI_INT, rootRank, MPI_COMM_WORLD);
    MPI_Gather(&globalRes, 1, MPI_DOUBLE, globalResArr, 1, MPI_DOUBLE, rootRank, MPI_COMM_WORLD);
    MPI_Gather(&tasksPerProcess, 1, MPI_INT, tasksBegin, 1, MPI_INT, rootRank, MPI_COMM_WORLD);

    if (rank == rootRank) {
        // вывод информации
        std::cout << "iterCounter = " << iterCounter << ", all tasks = " << allTasks << "\n";
        for (int p = 0; p < size; ++p) {
            std::cout << "P" << p << ":\t";
            std::cout << "Tasks at the beginning = " << tasksBegin[p] << "\t";
            std::cout << "Tasks done = " << tasksDoneArr[p] << "\t";
            std::cout << "Global res = " << globalResArr[p] << "\t";
            std::cout << "\n";
        }
        std::cout << "\n" << std::flush;
    }
}

// функция для потока-работника
void workerFunction(const int rank, const int size) {
    // инициализация списка задач
    initTaskList();

    // инициализация массивов для сбора информации на вывод (только для процесса, который будет собирать информацию)
    int *tasksDoneArr = nullptr;
    double *globalResArr = nullptr;
    int *tasksBegin = nullptr;
    if (rank == rootRank) {
        initBuffersForPrintInfo(tasksDoneArr, globalResArr, tasksBegin, size);
    }

    for (; iterCounter < maxIters; iterCounter++) {
        // заполнение списка задач и переменных перед началом новой итерации
        fillData(rank, size);

        const double startTime = MPI_Wtime();
        while (true) {
            if (tasksLeft == 0) {
                break;
            } else {
                executeTask(); // выполнение задачи
            }
        }
        const double endTime = MPI_Wtime();

        // Вывод результатов на экран
        printInfo(rank, size, endTime - startTime, tasksDoneArr, globalResArr, tasksBegin);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // освобождение памяти
    if (rank == rootRank) {
        free(tasksDoneArr);
        free(globalResArr);
        free(tasksBegin);
    }

    // отправка потоку-слушателю сигнала, что пора закончить работу
    const bool exitCode = false;
    MPI_Send(&exitCode, 1, MPI_CXX_BOOL, rank, rank, MPI_COMM_WORLD);
}

// отправить задачу на выполнение другому процессу
void sendTask(const int rank, const int size, const int requester) {
    int task = -1; // задача на отправку

    pthread_mutex_lock(&taskListMutex);
    if (tasksLeft > criticalTasksNum) {
        task = taskList[taskToSend];
        taskList[taskToSend] = 0;
        tasksLeft--;
        taskToSend = (taskToSend - 1 + tasksPerProcess) % tasksPerProcess;
    }
    pthread_mutex_unlock(&taskListMutex);

    MPI_Send(&task, 1, MPI_INT, requester, requester + size, MPI_COMM_WORLD);
}

// функция для потока-слушателя
void *listenerFunction(void *args) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    while (iterCounter < maxIters) {
        // приём запроса на задачу
        bool needTasks = false;
        MPI_Status status;
        MPI_Recv(&needTasks, 1, MPI_CXX_BOOL, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, &status);
        const int requester = status.MPI_SOURCE;

        // выходит из цикла, если принят сигнал на завершение программы
        if (needTasks == false) {
            break;
        }

        // отправка задачи (если возможно)
        sendTask(rank, size, requester);
    }

    return nullptr;
}

int distributeAscending(int rank, int size) {
    int sumRanks = (size * (size + 1)) / 2;
    return (allTasks * (rank + 1)) / sumRanks;
}

int distributeAllToOne(int rank) {

    return (rank == 0) ? allTasks : 0;
}

int distributeUniform(int rank, int size) {
    int base = allTasks / size;
    int rem = allTasks % size;
    return base + (rank < rem ? 1 : 0);
}

// число задач не должно зависить от числа процессов
// в executeTask исправить синхронизацию currentlyRunningTask, т.к. 
// запускать на кафедральном сервере

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <allTasks> <distribution_mode>\n";
        std::cerr << "distribution_mode: 1 - ascending, 2 - all to one, else ascending\n";
        return 1;
    }
    allTasks = atoi(argv[1]);
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    try {
        const double startTime = MPI_Wtime();
        // распределение задач
        if (strcmp(argv[2], "1") == 0) {
            if (rank == 0) std::cout << "ascending order\n\n";
            tasksPerProcess = distributeAscending(rank, size);
        } else if (strcmp(argv[2], "2") == 0) {
            if (rank == 0) std::cout << "all tasks for 1 process\n\n";
            tasksPerProcess = distributeAllToOne(rank);
        } else {
            if (rank == 0) std::cout << "Uniform distribution\n\n";
            tasksPerProcess = distributeUniform(rank, size);
        }

        // логика потока-работника (основного потока)
        workerFunction(rank, size);
        const double endTime = MPI_Wtime();
        if (rank == rootRank) {
            std::cout << "Total time: " << endTime - startTime << "\n";
        }
    } catch (const myException *const e) {
        const int code = e->getErrorCode();
        if ((code == 1 && rank == rootRank) || code != 1) {
            std::cerr << e->what() << '\n';
        }
    }

    MPI_Finalize();
}