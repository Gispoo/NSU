#ifndef MYTHREAD_H
#define MYTHREAD_H

typedef struct {
	void *(*start_routine)(void *);	 // Указатель на стартовую функцию потока
	void *arg;						 // Аргументы для стартовой функции
	void *retval;					 // Результат стартовой функции
	void *stack;					 // Указатель на начало стека
	int tid;						 // Идентификатор потока
	volatile int finished;			 // Завершёна ли поточная функция или нет
	int futex_finished_var;			 // Переменная, которую будет ожидать futex при ожидании изменения переменной finished
	volatile int joined;			 // Присоединён поток или нет
	volatile int joining_is_pending; // Ждёт ли уже какой то поток присоединения этого потока
	int futex_joined_var;			 // Переменная, которую будет ожидать futex при ожидании изменения переменной joined
	volatile int detached;			 // Отсоединён поток или нет
} mythread_struct_t;

typedef mythread_struct_t *mythread_t;

/*
`thread`        - указатель на память, куда будет положен указатель на управляющую структуру потока
`start_routine` - начальная функция потока
`arg`           - аргументы для начальной функции
*/
int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg);

/*
`thread` - идентификатор ожидаемого потока
`ret`    - значение, которое вернул ожидаемый поток
*/
int mythread_join(mythread_t thread, void **ret);

/*
`thread` - идентификатор отсоединяемого потока
*/
int mythread_detach(mythread_t thread);

#endif
