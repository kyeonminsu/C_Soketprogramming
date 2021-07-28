#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// 뮤텍스 객체 선언
pthread_mutex_t mutex_lock;

int g_count=0;

void *t_function(void *data)
{
	int i;
	char* thread_name = (char *)data;
	
	pthread_mutex_lock(&mutex_lock);	

	//critical section
	g_count = 0;
	for(i=0;i<3;i++)// 각 쓰레드 마다 0부터 시작
	{
		printf("%s Count %d\n",thread_name,g_count);
		g_count++; // 쓰레드 공유자원
		sleep(1);
	}
	pthread_mutex_unlock(&mutex_lock);
}
int main()
{
	pthread_t p_thread1, p_thread2;
	int status;
	//뮤텍스 초기화 , 기본 특성으로 초기화
	pthread_mutex_init(&mutex_lock, NULL);
	
	pthread_create(&p_thread1, NULL,t_function,(void *)"thread1");
	pthread_create(&p_thread2, NULL,t_function,(void *)"thread2");

	pthread_join(p_thread1, (void *)&status);
	pthread_join(p_thread2, (void *)&status);
}

