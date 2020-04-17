#pragma once

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

// 자료구조
#include <string.h>
#include <vector>
#include <unordered_map>
#include <queue>
#include <deque>
#include <array>

#include <memory>
#include <algorithm>
#include <mutex>

// 사용자 라이브러리
#include "ServErr.h"
#include "Config.h"
#include "Session.h"
#include "Stream.h"
#include "Logger.h"

// Define
#define MAX_IP 32