/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcInfo.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 07:57:25 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/05 08:28:37 by lnaidu           ###   ########.fr       */
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
    this->_startFailures=failure;
}

void ProcInfo::setStartedAt(time_t t)
{
    this->_startedAt=t;
}