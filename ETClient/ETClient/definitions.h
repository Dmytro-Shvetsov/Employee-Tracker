#pragma once

#include <QString>
#include <QSize>

#define COMPANY_NAME QString("Content Innovations")
#define APPLICATION_TITLE QString("Employee Tracker Agent")

//#define COMMUNICATION_HOST_URL QString("ws://localhost:8000/client/")
#define COMMUNICATION_HOST_URL QString("ws://track-your-employees.herokuapp.com/client/")

//#define AUTH_URL QString("http://localhost:8000/api/accounts/auth/login/")
#define AUTH_URL QString("http://track-your-employees.herokuapp.com/api/accounts/auth/login/")
// Dependency definitions

// Windows configuration
#define WIN32
#define WPCAP
#define _WINSOCK_DEPRECATED_NO_WARNINGS

enum STATUSES {ONLINE, OFFLINE, IDLE, STATUS_COUNT};
#define MIN_IDLE_TIME_SECONDS 15
//#define MIN_IDLE_TIME_SECONDS 900 // 15 mins

#define DEFAULT_SCREENSHOT_SIZE QSize(1324, 720)
#define SCREENSHOT_TIMEDELTA_SECONDS 10
//#define SCREENSHOT_TIMEDELTA_SECONDS 600 // 10 mins
#define NETWORK_INTERFACE_CONF_FILE QString("network_interface.txt")
