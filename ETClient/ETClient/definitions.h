#pragma once

#include <QString>

#define COMPANY_NAME QString("Content Innovations")
#define APPLICATION_TITLE QString("Employee Tracker Agent")

#define COMMUNICATION_HOST_URL QString("ws://localhost:8000/client/")
#define AUTH_URL QString("http://localhost:8000/api/auth/login/")

// Dependency definitions

// Windows configuration
#define WPCAP
//#define WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS

enum STATUSES {ONLINE, OFFLINE, IDLE, STATUS_COUNT};
#define MIN_IDLE_TIME_SECONDS 900 // 15 mins


#define MAX_UKNOWN_HOSTS_RESOLVE 3
