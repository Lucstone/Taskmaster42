/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcInfo.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 07:57:25 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/06 02:23:55 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ProcInfo.hpp"

ProcInfo::ProcInfo (pid_t pid = -1) : _pid(pid), _restarts(0), _startFailures(0), _startedAt(0) 
{
    //std::cout << "ProcInfo Constructor is called" << std::endl;
    return;
}

ProcInfo::~ProcInfo(void)
{
    //std::cout << "ProcInfo Destructor is called" << std::endl;
    return;
}

pid_t ProcInfo::getPid() const
{
    return this->_pid;
}

int ProcInfo::getRestarts() const
{
    return this->_restarts;
}

int ProcInfo::getStartFailures() const
{
    return this->_startFailures;
}

time_t ProcInfo::getStartedAt() const
{
    return this->_startedAt;
}

void ProcInfo::setPid(pid_t pid)
{
    this->_pid = pid;
}

void ProcInfo::setRestarts(int restarts)
{
    this->_restarts = restarts;
}

void ProcInfo::setStartFailures(int failure)
{
    this->_startFailures = failure;
}

void ProcInfo::setStartedAt(time_t time)
{
    this->_startedAt = time;
}

void ProcInfo::incrementRestarts()
{
    _restarts++;
}

void ProcInfo::incrementStartFailures() 
{ 
    _startFailures++; 
}

void ProcInfo::markStartedNow() 
{ 
    _startedAt = std::time(nullptr);
}

void ProcInfo::markStopped() 
{ 
    _pid = -1;
}

bool ProcInfo::isRunning() const 
{ 
    return _pid > 0; 
}

bool ProcInfo::diedTooEarly(int starttime) const 
{
     return _startedAt != 0 && (std::time(nullptr) - _startedAt) < starttime;
}

bool ProcInfo::hasExceededRetries(int maxRetries) const 
{
    return _startFailures > maxRetries;
}