첨부파일 설명

 

sipp-3.4.1.tar.gz ---> sipp 툴 소스

 

sipp.tgz ---> sipp 툴 소스가 컴파일된 바이너리 및 sipp사용을 위한 각종 shell, config 등

 

libpcap-1.5.3.tar.gz ---> sipp 툴에서 사용하는 pcap파일을 위한 라이브러리

 

 

 

 

sipp.tgz 내 shell 및 config 파일 설명

 

===============================================================

 

setup.sh 설명

주석이 없는 부분은 변경이 필요없는 부분

 

 

 

 

#!/bin/sh

# WLSS Server
export WLSS_SERVER="100.100.107.189:5060"                            // IVR 서버 IP 및 포트


# UAC Proxy 200
export UAC_PROXY200="uac_dtmf_G729.xml"                            // 성능시험 시나리오파일

 

# UAC scenario
export UAC_SCENARIO="uac"


# UAS scenario
export UAS_SCENARIO="uas"

 

# UAC host and port
export UAC_HOST="100.100.107.186"                                        // 성능시험 프로세스가 있는 장비
export UAC_PORT="5160"

 

export DNIS="5000"                                                                // IVR DNIS

 

#CPS Rate
export CPSRATE=5                                                               // 동시 콜 수 (10일 경우 한번에 10콜씩)
export RATEPERIOD=1000                                                      // 콜 주기 (1000 일경우 1초)
export MAXCALL=100                                                            // 테스트 총 콜 수

                                                                                          // 현재와 같은 조건이면 1초 주기로 5콜씩 총 100콜 테스트

                            

#Transport Mode
export TMODE=u1

 

# SIPp command
export SIPP_COMMAND=./sipp

 

# User Config File
export CFGFILE=irdb_perf.csv                                                    // DNIS를 쓰지않고 각 채널별 DN을 맵핑시키고 싶을때
