#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <mutex>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "common/singleton.h"
#include "common/task.h"

namespace xKBEngine { namespace threadpool {

class ThreadPool : public Singleton<ThreadPool>
{
public:
	/**
	   @param initial	: �̳߳ػ�һֱ������ô��������߳�
	   @param max		: �̳߳����ֻ������ô����߳�
	   @param grow		: ��ϵͳ��æʱ�̳߳ػ���������ô����ʱ�߳�
	*/
	ThreadPool(uint32_t initial, uint32_t grow, uint32_t max);
	~ThreadPool();

	/**
	    ��������̳߳�
	*/
	bool execute(std::shared_ptr<Task> t);

	/**
	    �����̳߳�
	*/
	void destroy();

	/**
	    �̳߳س�ʼ����������ʼ�߳�
	*/
	bool initialize();

	/**
	    �̳߳��Ƿ�Ϊ����״̬
	*/
	bool isDestroyed() const { return isDestroyed_.load(); }

	/**
	    �̳߳��Ƿ��Ѿ���ʼ��
	*/
	bool isInitialize(void) const { return isInitialize_.load(); }

	/**
	    ������ʱ�߳�
	*/
	void recycle();

protected:
	/**
	    �������б�ȡ��һ������,�����б���ɾ��
	*/
	std::shared_ptr<Task> fetchTask();

	/**
	    �̳߳�Ĭ���̴߳�����
	*/
	void worker(uint32_t idle);

public:
	static uint32_t timeout; // ��ʱ�߳�������ʱ��

protected:
	std::condition_variable cv_recycle_; // �̳߳ػ����ź�
	std::condition_variable cv_task_; // �����ָ̻߳��ź�
	uint32_t grow_; // ÿ����������ʱ�߳���Ŀ
	uint32_t idles_; // �����߳���Ŀ
	uint32_t initial_; // �̳߳س�ʼ�߳���Ŀ
	std::atomic<bool> isDestroyed_; // �̳߳��Ƿ�Ϊ����״̬
	std::atomic<bool> isInitialize_; // �̳߳��Ƿ��Ѿ���ʼ��
	uint32_t max_; // �̳߳�����߳���Ŀ = ��ʼ + ��ʱ
	std::list<std::thread::id> recycle_; // �������߳��б�
	std::mutex stats_lock_; // �����߳�ͳ����
	std::mutex task_lock_; // �����б������
	std::queue<std::shared_ptr<Task>> tasks_; // �����б�
	std::mutex thread_lock_; // �߳��б������
	std::list<std::thread> threads_; // �߳��б�
};

}
}