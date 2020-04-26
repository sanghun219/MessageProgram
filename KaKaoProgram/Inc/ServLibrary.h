#pragma once

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

// �ڷᱸ��
#include <string.h>
#include <vector>
#include <unordered_map>
#include <queue>
#include <deque>
#include <array>

#include <memory>
#include <algorithm>
#include <mutex>

#include <mysql.h>

// Define
#define MAX_IP 32
#define SESSION_ID unsigned int
#define USER_ID_MAX 32

// ����� ���̺귯��
#include "ServErr.h"
#include "Config.h"
#include "Session.h"
#include "Stream.h"
#include "Logger.h"
