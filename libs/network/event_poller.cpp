#include <sys/epoll.h>
#include <boost/log/trivial.hpp>
#include "event_poller.h"
#include "interfaces.h"

namespace xKBEngine { namespace network {

EventPoller::EventPoller():
	epfd_(epoll_create(1)),
	spareTime_(0),
	fdReadHandlers_(),
	fdWriteHandlers_()
{
	if (epfd_ == -1)
	{
		BOOST_LOG_TRIVIAL(error) << "EpollPoller::EpollPoller: epoll_create failed: " << strerror(errno);
	}
}


EventPoller::~EventPoller()
{
	if (epfd_ != -1)
	{
		close(epfd_);
	}
}

bool EventPoller::deregisterForRead(int fd)
{
	fdReadHandlers_.erase(fd);
	return doRegister(fd, true, false);
}

bool EventPoller::deregisterForWrite(int fd)
{
	fdWriteHandlers_.erase(fd);
	return doRegister(fd, false, false);
}

std::shared_ptr<InputNotificationHandler> EventPoller::findForRead(int fd)
{
	auto iter = fdReadHandlers_.find(fd);

	if (iter == fdReadHandlers_.end())
		return NULL;

	return iter->second;
}

std::shared_ptr<OutputNotificationHandler> EventPoller::findForWrite(int fd)
{
	auto iter = fdWriteHandlers_.find(fd);

	if (iter == fdWriteHandlers_.end())
		return NULL;

	return iter->second;
}

int EventPoller::processPendingEvents(int maxWaitInMilliseconds)
{
	const int MAX_EVENTS = 10;
	struct epoll_event events[MAX_EVENTS];

	TIME_STAMP startTime = TIME_NOW;
	int nfds = epoll_wait(epfd_, events, MAX_EVENTS, maxWaitInMilliseconds);
	spareTime_ += TIME_NOW - startTime;

	for (int i = 0; i < nfds; ++i)
	{
		if (events[i].events & (EPOLLERR | EPOLLHUP))
		{
			this->triggerError(events[i].data.fd);
		}
		else
		{
			if (events[i].events & EPOLLIN)
			{
				this->triggerRead(events[i].data.fd);
			}

			if (events[i].events & EPOLLOUT)
			{
				this->triggerWrite(events[i].data.fd);
			}
		}
	}

	return nfds;
}

bool EventPoller::registerForRead(int fd, std::shared_ptr<InputNotificationHandler> handler)
{
	if (!doRegister(fd, true, true))
		return false;

	fdReadHandlers_[fd] = handler;
	return true;
}

bool EventPoller::registerForWrite(int fd, std::shared_ptr<OutputNotificationHandler> handler)
{
	if (!doRegister(fd, false, true))
		return false;

	fdWriteHandlers_[fd] = handler;
	return true;
}

bool EventPoller::doRegister(int fd, bool isRead, bool isRegister)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev)); // stop valgrind warning
	int op;

	ev.data.fd = fd;

	// Handle the case where the file is already registered for the opposite
	// action.
	if (this->isRegistered(fd, !isRead))
	{
		op = EPOLL_CTL_MOD;

		ev.events = isRegister ? EPOLLIN | EPOLLOUT :
			isRead ? EPOLLOUT : EPOLLIN;
	}
	else
	{
		// TODO: Could be good to use EPOLLET (leave like select for now).
		ev.events = isRead ? EPOLLIN : EPOLLOUT;
		op = isRegister ? EPOLL_CTL_ADD : EPOLL_CTL_DEL;
	}

	if (epoll_ctl(epfd_, op, fd, &ev) < 0)
	{
		if (errno == EBADF)
		{
			BOOST_LOG_TRIVIAL(warning) << "EpollPoller::doRegister: Failed to "
				<< (isRegister ? "add" : "remove") 
				<< " " 
				<< (isRead ? "read" : "write") 
				<<" file descriptor " 
				<< fd
				<< " ("
				<< strerror(errno) <<")";
		}
		else
		{
			BOOST_LOG_TRIVIAL(error) << "EpollPoller::doRegister: Failed to "
				<< (isRegister ? "add" : "remove")
				<< " "
				<< (isRead ? "read" : "write")
				<< " file descriptor "
				<< fd
				<< " ("
				<< strerror(errno) << ")";
		}

		return false;
	}

	return true;
}

bool EventPoller::isRegistered(int fd, bool isForRead) const
{
	return isForRead ? (fdReadHandlers_.find(fd) != fdReadHandlers_.end()) :
		(fdWriteHandlers_.find(fd) != fdWriteHandlers_.end());
}

bool EventPoller::triggerRead(int fd)
{
	auto iter = fdReadHandlers_.find(fd);

	if (iter == fdReadHandlers_.end())
	{
		return false;
	}

	iter->second->handleInputNotification(fd);

	return true;
}
	
bool EventPoller::triggerWrite(int fd)
{
	auto iter = fdWriteHandlers_.find(fd);

	if (iter == fdWriteHandlers_.end())
	{
		return false;
	}

	iter->second->handleOutputNotification(fd);

	return true;
}

bool EventPoller::triggerError(int fd)
{
	if (!this->triggerRead(fd))
	{
		return this->triggerWrite(fd);
	}

	return true;
}

}
}