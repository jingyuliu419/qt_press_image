/*
 * @Author: W.jy
 * @Date: 2021-08-18 11:42:57
 * @LastEditTime: 2021-08-18 13:07:38
 * @FilePath: \Video_Capture\Refactor\iflytekuser.h
 * @Description: 
 */
#ifndef IFLYTEKUSER_H
#define IFLYTEKUSER_H

#include <QString>
#include <QMutex>
//#include "msp_cmn.h"

class IFlytekUser
{
public:
//    Q_DISABLE_COPY_MOVE(IFlytekUser)

    static IFlytekUser *getInstance()
    {
        static IFlytekUser iFlytekUsrInstance;
        return &iFlytekUsrInstance;
    }

    int login();

    void logout()
    {
        QMutexLocker locker(&m_mutex);
        if (m_appCnt > 1)
            --m_appCnt;
        else
        {
            m_appCnt = 0;
            MSPLogout();
        }
    }

private:
    explicit IFlytekUser();
    static std::string usrId;
    int m_appCnt;
    QMutex m_mutex;
};

#endif // IFLYTEKUSER_H
