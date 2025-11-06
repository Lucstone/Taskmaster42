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

// ---------------------- Constructeur / Destructeur ----------------------
ProcessInfo::ProcessInfo(pid_t pid) 
    : _pid(pid), _restarts(0), _startFailures(0), _startedAt(0) {}

ProcessInfo::~ProcessInfo() {}

// ---------------------- Getters ----------------------
pid_t ProcessInfo::getPid() const { return _pid; }
int ProcessInfo::getRestarts() const { return _restarts; }
int ProcessInfo::getStartFailures() const { return _startFailures; }
time_t ProcessInfo::getStartedAt() const { return _startedAt; }

// ---------------------- Setters ----------------------
void ProcessInfo::setPid(pid_t pid) { _pid = pid; }
void ProcessInfo::setRestarts(int restarts) { _restarts = restarts; }
void ProcessInfo::setStartFailures(int failure) { _startFailures = failure; }
void ProcessInfo::setStartedAt(time_t t) { _startedAt = t; }

// ---------------------- Fonctions utilitaires ----------------------
void ProcessInfo::incrementRestarts() { _restarts++; }
void ProcessInfo::incrementStartFailures() { _startFailures++; }

void ProcessInfo::markStartedNow() { _startedAt = std::time(nullptr); }

void ProcessInfo::markStopped() { _pid = -1; }

bool ProcessInfo::isRunning() const { return _pid > 0; }

bool ProcessInfo::diedTooEarly(int starttime) const {
    if (_startedAt == 0) return false;
    return (std::time(nullptr) - _startedAt) < starttime;
}

bool ProcessInfo::hasExceededRetries(int maxRetries) const {
    return _startFailures > maxRetries;
}