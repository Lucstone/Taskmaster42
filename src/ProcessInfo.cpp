/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessInfo.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 07:57:25 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/05 08:28:37 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ProcessInfo.hpp"
#include <ctime>

ProcessInfo::ProcessInfo(pid_t pid) 
    : _pid(pid), _restarts(0), _startFailures(0),
      _startedAt(0), _stoppedAt(0), _state(STOPPED),
      _everStarted(false) {}

pid_t ProcessInfo::getPid() const { return _pid; }
int ProcessInfo::getRestarts() const { return _restarts; }
int ProcessInfo::getStartFailures() const { return _startFailures; }
time_t ProcessInfo::getStartedAt() const { return _startedAt; }
time_t ProcessInfo::getStoppedAt() const { return _stoppedAt; }
ProcessInfo::State ProcessInfo::getState() const { return _state; }

bool ProcessInfo::everStarted() const { return _everStarted; }

void ProcessInfo::setPid(pid_t pid) { _pid = pid; }
void ProcessInfo::setRestarts(int restarts) { _restarts = restarts; }
void ProcessInfo::setStartFailures(int failure) { _startFailures = failure; }
void ProcessInfo::setStartedAt(time_t t) { _startedAt = t; }
void ProcessInfo::setStoppedAt(time_t t) { _stoppedAt = t; }
void ProcessInfo::setState(State s) { _state = s; }

void ProcessInfo::markStarting() { _state = STARTING; }
void ProcessInfo::markStartedNow() {
    _startedAt = std::time(nullptr);
    _state = RUNNING;
    _everStarted = true;
}
void ProcessInfo::markStopped() {
    _pid = -1;
    _state = STOPPED;
    _stoppedAt = std::time(nullptr);
}

bool ProcessInfo::isRunning() const { return _state == RUNNING && _pid > 0; }

bool ProcessInfo::diedTooEarly(int starttime) const {
    if (_startedAt == 0) return false;
    return (std::time(nullptr) - _startedAt) < starttime;
}

bool ProcessInfo::hasExceededRetries(int maxRetries) const {
    return _startFailures > maxRetries;
}
