/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessInfo.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 07:17:17 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/05 08:07:47 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/types.h>
#include <ctime>

class ProcessInfo {
public:
    enum State { STOPPED, STARTING, RUNNING };

private:
    pid_t  _pid;
    int    _restarts;
    int    _startFailures;
    time_t _startedAt;
    time_t _stoppedAt;
    State  _state;
    bool   _everStarted;

public:
    ProcessInfo(pid_t pid = -1);

    pid_t   getPid() const;
    int     getRestarts() const;
    int     getStartFailures() const;
    time_t  getStartedAt() const;
    time_t  getStoppedAt() const;
    State   getState() const;
    bool    everStarted() const;

    void    setPid(pid_t pid);
    void    setRestarts(int restart);
    void    setStartFailures(int failure);
    void    setStartedAt(time_t t);
    void    setStoppedAt(time_t t);
    void    setState(State s);

    void    markStarting();
    void    markStartedNow();
    void    markStopped();

    bool    isRunning() const;
    bool    diedTooEarly(int starttime) const;
    bool    hasExceededRetries(int maxRetries) const;
};
