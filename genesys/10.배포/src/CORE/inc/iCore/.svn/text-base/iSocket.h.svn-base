//
//  iSocket.h
//  iCore
//
//  Created by Tony Ryu on 2013. 6. 25..
//  Copyright (c) 2013년 Tony Ryu. All rights reserved.
//

/**
 * @file    iSocket.h
 * @brief   Socket 처리
 *
 * @version v0.1
 * @see     None
 * @date    2013. 6. 25.
 */

#pragma once

#include "iThread.h"

#ifdef IWINDOWS
# pragma comment(lib, "wininet.lib")
# pragma comment(lib, "ws2_32.lib")
# include <ws2tcpip.h>
// Needed for the Windows 2000 IPv6 Tech Preview.
# if (_WIN32_WINNT <= 0x0500)
# include <tpipv6.h>
# endif
  typedef short					sa_family_t;
#else	// IUNIX
# include <netdb.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
typedef sockaddr_storage		SOCKADDR_STORAGE;
#endif

/**
 * @enum    eSockSel
 * @brief   Socket Select 함수 사용 시 Event Masking용 Enum
 * @see     Select
 */
enum eSockSel
{
	ISOCK_RFDS		= 0x01,									///< Read Event Masking.
	ISOCK_WFDS		= 0x02,									///< Write Event Masking.
	ISOCK_RWFDS		= ISOCK_RFDS|ISOCK_WFDS,				///< Read/Write Event Masking.
	ISOCK_EFDS		= 0x04,									///< Error Event Masking.
	ISOCK_ALLFDS	= ISOCK_RFDS|ISOCK_WFDS|ISOCK_EFDS 		///< Read/Write/Error Event Masking.
};



/**
 * @class    iSockAddr
 * @brief    Socket Address(IPv4/IPv6) 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2013.6.25
 */
class ILIBAPI iSockAddr : public SOCKADDR_STORAGE
{
public:
	iSockAddr();		///< 생성자.
	iSockAddr(const iSockAddr& rAddr);				///< 복사 생성자.
	iSockAddr(NPCSTR szIpAddr, uint16 nPort);		///< 생성자.
	iSockAddr(iFamily_t nFamily, uint16 nPort);		///< 생성자.
	~iSockAddr();									///< 소멸자.

	void Reset();		///< 주소 정보를 모드 초기화 함.
	bool IsNull() const;

	///< sockaddr* 형식으로 현재 주소 정보를 반환.
	inline struct sockaddr* SockAddr() const { return (struct sockaddr*)this; };
	///< sockaddr_in* 형식으로 현재 주소 정보를 반환(IPv4).
	inline struct sockaddr_in* SockAddr4() const { return (struct sockaddr_in*)this; };
	///< sockaddr_in6* 형식으로 현재 주소 정보를 반환(IPv6).
	inline struct sockaddr_in6* SockAddr6() const { return (struct sockaddr_in6*)this; };

	///< Address Family Type 반환(AF_INET, AF_INET6).
	inline sa_family_t Family() const { return ss_family; };

	///< 현재 설정된 주소 struct의 크기 반환.
	inline socklen_t Socklen() const { return mnSocklen; };
	///< 주소 struct의 크기 변수의 주소값 반환.
	inline iSockLen_t* SocklenPtr() { return &mnSocklen; };

	///< in_addr* 형식으로 IP Address를 반환(IPv4).
	inline struct in_addr* IPv4() const { return &((struct sockaddr_in*)this)->sin_addr; };
	///< in6_addr* 형식으로 IP Address를 반환(IPv6).
	inline struct in6_addr* IPv6() const { return &((struct sockaddr_in6*)this)->sin6_addr; };

	/**
	 * @brief  IP Address를 문자열로 반환
	 * @return             szBuf 문자열
	 * @param  szBuf       IP Address를 저장할 문자열 버퍼
	 * @param  nBuf        szBuf의 크기
	 */
	NPCSTR IpAddr(NPSTR szBuf, size_t nBuf) const;
	NPCSTR IpAddr() const;

	///< Port 정보를 반환.
	inline uint16 Port() const { return ntohs(((struct sockaddr_in*)this)->sin_port); };

	///< Port를 설정.
	inline void SetPort(uint16 nPort) { ((struct sockaddr_in*)this)->sin_port = htons(nPort); };

	///< iSockAddr 끼리 동일한 주소인지 비교하는 함수.
	bool Compare(const iSockAddr& rAddr);

	/**
	 * @brief  주소 설정 함수
	 * @return                 성공 여부
	 * @param  szIpAddr        문자열 IP Address
	 * @param  nPort           Port 정보
	 * @note   사용 방법
	 * - TCP Client Socket에서 Server측 주소를 설정 시
	 * - TCP Server Socket에서 특정 IP로 Server Port를 Binding 시
	 * - UDP Socket에서 Binding 하거나 상대 주소를 설정할 경우
	 */
	bool SetAddress(NPCSTR szIpAddr, uint16 nPort);

	/**
	 * @brief  주소 설정 함수
	 * @return                 성공 여부
	 * @param  nFamily         Address Family(AF_INET, AF_INET6)
	 * @param  nPort           Port 정보
	 * @note   사용 방법
	 * - TCP Server Socket에서 ANY IP로 Server Port를 Binding 시
	 * - UDP Socket에서 Binding 할 경우
	 */
	bool SetAddress(sa_family_t nFamily, uint16 nPort);

	/**
	 * @brief   IPv4 주소와 IPv6 주소를 서로 변환하는 함수
	 * @param   nFamily 변환하고픈 Address Family(AF_INET, AF_INET6)
	 * @return          성공여부
	 * @warning IPv6 주소를 IPv4로 변환은 IPv6 주소가 IPv4 주소 정보일때만 가능
	 */
	bool ConvertAddr(iFamily_t nFamily);
	IDEPRECATED bool ToggleAddress();

	static const iSockAddr* Null();

protected:
	socklen_t mnSocklen;
	char mszIpAddr[MAXLEN_IPADDR];

	friend class iSocket;
	inline socklen_t& SetSocklen() { return mnSocklen; };
	inline void SetSocklen(socklen_t nSocklen) { mnSocklen = nSocklen; };
};



/**
 * @class    iSocket
 * @brief    Socket 처리 Class
 * @author   Tony Ryu(bridgetec@gmail.com)
 * @date     2013.6.25
 * @note
 * - iThread는 iSocket 내부에서는 사용하지 않음
 * - 필요 시 구현 측에서 사용할 수 있음
 */
class ILIBAPI iSocket : public iThread
{
public:
	iSocket();				///< 생성자.
	virtual ~iSocket();		///< 소멸자.

	///< Windows의 경우 Socket 관련 함수 사용 전에 반드시 호출.
	static void WSStartup();
	static void WSCleanup();

	/**
	 * @brief  Socket Handle을 iSocket Instance에 Attach
	 * @return             성공 여부
	 * @param  hSocket     Socket Handle
	 */
	bool Attach(iFd hSocket);

	/**
	 * @brief  Socket Handle을 iSocket Instance에서 Detach
	 * @return Socket Handle
	 */
	iFd Detach();

	/**
	 * @brief  Socket Open 함수 
	 * @return           성공 여부
	 * @param  nFamily   Address Family(AF_INET/AF_INET6)
	 * @param  nType     Socket Type(SOCK_STREAM/SOCK_DGRAM)
	 */
	bool Open(int32 nFamily, int32 nType);

	/**
	 * @brief  Socket 종료 함수
	 * @return 항상 false를 반환
	 */
	bool Close();

	/**
	 * @brief  Connect 함수(SOCK_STREAM 방식)
	 * @return              성공 여부
	 * @param  szIpAddr     대상 IP Address(IPv4/IPv6)
	 * @param  nPort        대상 Port
	 * @param  nTimeout     Connect시 Timeout Time 지정(미지정 시 무제한)
	 * @note                Socket이 Open 되지 않았다면 내부에서 Open 처리
	 */
	bool Connect(NPCSTR szIpAddr, uint16 nPort, int32 nTimeout = ILIB_INFINITE);

	/**
	 * @brief  Connect 함수(SOCK_STREAM 방식)
	 * @return              성공 여부
	 * @param  pAddr        iSockAddr Pointer, 대상 IP Address(IPv4/IPv6) and Port
	 * @param  nTimeout     Connect시 Timeout Time 지정(미지정 시 무제한)
	 * @note                Socket이 Open 되지 않았다면 내부에서 Open 처리
	 */
	bool Connect(const iSockAddr* pAddr, int32 nTimeout = ILIB_INFINITE);

	/**
	 * @brief  Connect 함수(SOCK_STREAM/SOCK_DGRAM을 지정 가능)
	 * @return              성공 여부
	 * @param  nType        Socket Type(SOCK_STREAM/SOCK_DGRAM)
	 * @param  szIpAddr     대상 IP Address(IPv4/IPv6)
	 * @param  nPort        대상 Port
	 * @param  nTimeout     Connect시 Timeout Time 지정(미지정 시 무제한)
	 */
	bool Connect(int32 nType, NPCSTR szIpAddr, uint16 nPort, int32 nTimeout = ILIB_INFINITE);

	/**
	 * @brief  Connect 함수(SOCK_STREAM/SOCK_DGRAM을 지정 가능)
	 * @return              성공 여부
	 * @param  nType        Socket Type(SOCK_STREAM/SOCK_DGRAM)
	 * @param  pAddr        iSockAddr Pointer, 대상 IP Address(IPv4/IPv6) and Port
	 * @param  nTimeout     Connect시 Timeout Time 지정(미지정 시 무제한)
	 */
	bool Connect(int32 nType, const iSockAddr* pAddr, int32 nTimeout = ILIB_INFINITE);

	/**
	 * @brief  Bind 함수
	 * @return           성공 여부
	 * @param  nPort     Bind 할 Port
	 * @param  szIpAddr  Bind 할 IP Address(NULL일 경우 ANY_IPADDR 지정)
	 */
	bool Bind(uint16 nPort, NPCSTR szIpAddr = NULL);

	/**
	 * @brief  Bind 함수
	 * @return           성공 여부
	 * @param  pAddr     iSockAddr Pointer, 대상 IP Address(IPv4/IPv6) and Port
	 */
	bool Bind(const iSockAddr* pAddr);

	/**
	 * @brief  Listen 힘수
	 * @return             성공 여부
	 * @param  nBacklog    Listen 대기 큐 크기
	 */
	bool Listen(int32 nBacklog = SOMAXCONN);

	/**
	 * @brief  Accept 함수
	 * @return              성공 여부
	 * @param(in,out) pAddr 접속한 Peer 주소 버퍼 Pointer
	 */
	iFd Accept(iSockAddr* pAddr = NULL);

	/**
	 * @brief  Socket Reuse address 설정
	 * @return                   성공 여부
	 * @param  bUse              Reuse 여부
	 */
	bool SetReuseAddr(bool bUse);

	/**
	 * @brief  Socket Block/NonBlock Mode 설정 함수
	 * @return                성공 여부
	 * @param  bNonBlk        true:NonBlock, false:Block
	 */
	bool SetNonBlk(bool bNonBlk);

	/**
	 * @brief  Socket Linger 설정 함수
	 * @return                성공 여부
	 * @param  bLinger        Linger 사용 여부
	 * @param  nLinger        Linger Time 설정
	 * @note   Linger 설정
	 * - bLinger == false : 기본 설정으로 Socket의 모든 데이터를 보낸다.
	 * - bLinger == true && nLinger == 0 : Socket의 모든 데이터를 버린다.
	 * - bLinger == true && nLinger > 0 : Socket의 모든 데이터를 보낸다.
	 *                                    nLinger 시간만큼 Socket Close시 Block 된다.
	 */
	bool SetLinger(bool bLinger = false, int32 nLinger = 0);

	inline iFd GetSocket() { return mhSocket; };
	inline bool IsOpened() { return bool(mhSocket>=0); };

	inline sa_family_t GetDomain() { return mnFamily; };
	inline int32 GetType() { return mnType; };

	/**
	 * @brief  마지막 오류 코드를 반환
	 * @return 오류 코드
	 */
	inline int32 GetErrNo() { return mnErrNo; };
	NPCSTR GetErrStr(NPSTR szBuf, uint32 nBuf, int32 nErrNo = -1);

	/**
	 * @brief  Local Address iSockAddr Pointer를 반환
	 * @return iSockAddr Pointer
	 */
	inline iSockAddr* GetLcAddr() { return &mstLcAddr; };

	/**
	 * @brief  Remote Address iSockAddr Pointer를 반환
	 * @return iSockAddr Pointer
	 */
	inline iSockAddr* GetRmAddr() { return &mstRmAddr; };
	/**
	 * @brief  Remote Address를 설정
	 * @return                성공 여부
	 * @param  szIpAddr       대상 IP Address(IPv4/IPv6)
	 * @param  nPort          대상 Port
	 */
	bool SetRmAddr(NPCSTR szIpAddr, uint16 nPort);

	/**
	 * @brief  Remote Address를 설정
	 * @return                성공 여부
	 * @param  pAddr          iSockAddr Pointer, 대상 IP Address(IPv4/IPv6) and Port
	 */
	bool SetRmAddr(const iSockAddr* pAddr);

	/**
	 * @brief  Socket Select 함수
	 * @return             select 함수 반환값(0:Timeout, >0:Event가 발생한 FD 갯수, <0:오류)
	 * @param  fdMax       제일 큰 FD 값 + 1
	 * @param  rFds        Read용 FD_SET
	 * @param  wFds        Write용 FD_SET
	 * @param  eFds        Error용 FD_SET
	 * @param  nTimeout    Wait Time(miliseconde 단위, -1 지정 시 무한 기다림)
	 */
	static int32 Select(int32 fdMax, fd_set* rFds, fd_set* wFds, fd_set* eFds, int32 nTimeout = ILIB_INFINITE);

	/**
	 * @brief  Socket Select 함수
	 * @return             select 함수 반환값(0:Timeout, >0:Event가 발생한 FD 갯수, <0:오류)
	 * @param  nFdsMask    FD Event Masking Enum값
	 * @param  nTimeout    Wait Time(miliseconde 단위, -1 지정 시 무한 기다림)
	 * @see    eSockSel
	 */
	int32 Select(eSockSel nFdsMask, int32 nTimeout = ILIB_INFINITE);

	inline int32 RSelect(int32 nTimeout = ILIB_INFINITE) { return Select(ISOCK_RFDS, nTimeout); };
	inline int32 WSelect(int32 nTimeout = ILIB_INFINITE) { return Select(ISOCK_WFDS, nTimeout); };
	inline int32 RWSelect(int32 nTimeout = ILIB_INFINITE) { return Select(ISOCK_RWFDS, nTimeout); };

	/**
	 * @brief  Socket Send 함수
	 * @return           전송 Bytes 수, 실패 시 0, -1 반환
	 * @param  szData    전송 데이터 버퍼
	 * @param  nData     전송 데이터 크기
	 * @note 유의 사항
	 * - Block/Nonblock Socket에 따른 개발은 Application에서 처리 필요
	 * @note Nonblock Mode 시 처리
	 * - 반환값이 0보다 크고 nData보다 작은 값이 반환 될 수 있음
	 * - 반환값이 -1일 경우 errno가 EWOULDBLOCK일 경우 송신 버퍼가 Full이라는 뜻임
	 * - 미 송신분에 대한 재 전송은 WSelect로 기다리거나 일정 시간 이후 재전송 요청 필요함
	 * - 송신 시 자주 EWOULDBLOCK 현상이 일어 난다면 구조적인 Protocol 개선이 필요함
	 */
	int32 Send(NPCSTR szData, size_t nData);

	/**
	 * @brief  Socket Recv 함수
	 * @return           수신 Bytes 수, 실패 시 0, -1 반환
	 * @param  szBuf     수신 데이터 버퍼
	 * @param  nBuf      수신 데이터 버퍼 크기
	 * @note 유의 사항
	 * - Block/Nonblock Socket에 따른 개발은 Application에서 처리 필요
	 * @note Nonblock Mode 시 처리
	 * - 반환값이 -1이고 errno가 EWOULDBLOCK일 경우 수신 버퍼에 데이터가 없다는 뜻임
	 */
	int32 Recv(NPSTR szBuf, size_t nBuf);

	/**
	 * @brief  Socket SendTo 함수(SOCK_DGRAM)
	 * @return             전송 Bytes 수, 실패 시 0, -1 반환
	 * @param  szData      전송 데이터 버퍼
	 * @param  nData       전송 데이터 크기
	 * @param  szIpAddr    대상 IP Address(IPv4/IPv6)
	 * @param  nPort       대상 Port
	 */
	int32 SendTo(NPCSTR szData, size_t nData, NPCSTR szIpAddr, uint16 nPort);

	/**
	 * @brief  Socket SendTo 함수(SOCK_DGRAM)
	 * @return             전송 Bytes 수, 실패 시 0, -1 반환
	 * @param  szData      전송 데이터 버퍼
	 * @param  nData       전송 데이터 크기
	 * @param  pAddr       iSockAddr Pointer, 대상 IP Address(IPv4/IPv6) and Port
	 */
	int32 SendTo(NPCSTR szData, size_t nData, const iSockAddr* pAddr = NULL);

	/**
	 * @brief  Socket RecvFrom 함수(SOCK_DGRAM)
	 * @return               수신 Bytes 수, 실패 시 0, -1 반환
	 * @param  szBuf         수신 데이터 버퍼
	 * @param  nBuf          수신 데이터 버퍼 크기
	 * @param  pAddr         송신 측 주소 정보 저장 버퍼
	 */
	int32 RecvFrom(NPSTR szBuf, size_t nBuf, iSockAddr* pAddr = NULL);

	/**
	 * @brief  Socket 지정 크기 Recv 함수(SOCK_STREAM)
	 * @return               수신 Bytes 수, Timeout시 0반환, 실패 시 -1 반환
	 * @param  szBuf         수신 데이터 버퍼
	 * @param  nBuf          수신 데이터 버퍼 크기
	 * @param  nTimeout      Wait Time(miliseconde 단위, -1 지정 시 무한 기다림)
	 */
	int32 RecvSize(NPSTR szBuf, size_t nBuf, int32 nTimeout = ILIB_INFINITE);

protected:
	void Clear();
	inline void ResetErr() { mnErrNo = 0; };
	inline void SetErrNo(int32 nErrNo) { mnErrNo = nErrNo; };

	iFd mhSocket;
	int32 mnType;
	int32 mnErrNo;

	iSockAddr mstLcAddr;
	iSockAddr mstRmAddr;
	sa_family_t mnFamily;
};

iFd iSockOpen(sa_family_t nFamily, int32 nSockType);
iFd iSockClose(iFd hSocket);
iFd iSockSelect(int32 nFdMax, fd_set* rFds, fd_set* wFds, fd_set* eFds, int32 nTimeout = ILIB_INFINITE);
iFd iSockAccept(iFd hServer, sockaddr_in* pAddr);
bool iSockConnect(iFd hSocket, sockaddr_in* pAddr, bool bNonBlk = false);
bool iSockBind(iFd hSocket, sockaddr_in* pAddr);
bool iSockListen(iFd hSocket, sockaddr_in* pAddr = NULL);

int32 iSockRecv(iFd hSocket, NPSTR szBuf, uint32 nBuf, int32 nTimeout = ILIB_INFINITE);
int32 iSockRecvSize(iFd hSocket, NPSTR szBuf, uint32 nBuf, int32 nTimeout = ILIB_INFINITE);
int32 iSockSend(iFd hSocket, NPCSTR szData, uint32 nData);
int32 iSockRecvFrom(iFd hSocket, NPSTR szBuf, uint32 nBuf, sockaddr_in* pAddr = NULL, int32 nTimeout = ILIB_INFINITE);
int32 iSockSendTo(iFd hSocket, NPCSTR szData, uint32 nData, sockaddr_in* pAddr);

void iSockSetAddr(sockaddr_in* pAddr, uint16 nPort, NPCSTR szIpAddr = NULL);
bool iSockLingerUse(iFd hSocket, bool bLinger = false, int32 nLinger = 0);
bool iSockReuseAddr(iFd hSocket, bool bReuse = true);
bool iSockBroadcast(iFd hSocket, bool bBroadcast = false);
bool iSockKeepAlive(iFd hSocket, bool bUse = true);
bool iSockNoDelay(iFd hSocket, bool bUse = true);
bool iSockNonBlk(iFd hSocket, bool bNonBlk = false);



