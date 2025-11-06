/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessInfo.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 07:17:17 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/06 01:34:24 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <sys/types.h>
# include <ctime>
# include <string>
# include <iostream>

class ProcessInfo {
    private:

        pid_t  _pid;
        int    _restarts;
        int    _startFailures;
        time_t _startedAt;

    public:

        ProcessInfo(pid_t pid = -1);
        ~ProcessInfo();

        pid_t   getPid() const;
        int     getRestarts() const;
        int     getStartFailures() const;
        time_t  getStartedAt() const;

        void    setPid(pid_t pid);
        void    setRestarts(int restart);
        void    setStartFailures(int failure);
        void    setStartedAt(time_t time);
    
        void    incrementRestarts();
        void    incrementStartFailures();
        void    markStartedNow();
        void    markStopped();
        bool    isRunning() const;
        bool    diedTooEarly(int starttime) const;
        bool    hasExceededRetries(int maxRetries) const;
};