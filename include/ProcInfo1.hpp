/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcInfo.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 06:03:18 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/05 06:03:25 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <sys/types.h>
# include <ctime>
# include <string>
# include <iostream>

class ProcInfo {
private:
    pid_t  _pid;
    int    _restarts;
    int    _startFailures;
    time_t _startedAt;

public:
    ProcInfo(pid_t pid = -1)
        : _pid(pid), _restarts(0), _startFailures(0), _startedAt(0) {}

    // Getters
    pid_t  getPid() const { return _pid; }
    int    getRestarts() const { return _restarts; }
    int    getStartFailures() const { return _startFailures; }
    time_t getStartedAt() const { return _startedAt; }

    // Setters
    void setPid(pid_t pid) { _pid = pid; }
    void setRestarts(int r) { _restarts = r; }
    void setStartFailures(int f) { _startFailures = f; }
    void setStartedAt(time_t t) { _startedAt = t; }

    // Helpers
    void incrementRestarts() { ++_restarts; }
    void incrementStartFailures() { ++_startFailures; }
    void markStartedNow() { _startedAt = std::time(nullptr); }
    void markStopped() { _pid = -1; }

    bool isRunning() const { return _pid > 0; }
    bool diedTooEarly(int starttime) const {
        return _startedAt != 0 && (std::time(nullptr) - _startedAt) < starttime;
    }
    bool hasExceededRetries(int maxRetries) const {
        return _startFailures > maxRetries;
    }
};
