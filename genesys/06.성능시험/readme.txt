÷������ ����

 

sipp-3.4.1.tar.gz ---> sipp �� �ҽ�

 

sipp.tgz ---> sipp �� �ҽ��� �����ϵ� ���̳ʸ� �� sipp����� ���� ���� shell, config ��

 

libpcap-1.5.3.tar.gz ---> sipp ������ ����ϴ� pcap������ ���� ���̺귯��

 

 

 

 

sipp.tgz �� shell �� config ���� ����

 

===============================================================

 

setup.sh ����

�ּ��� ���� �κ��� ������ �ʿ���� �κ�

 

 

 

 

#!/bin/sh

# WLSS Server
export WLSS_SERVER="100.100.107.189:5060"                            // IVR ���� IP �� ��Ʈ


# UAC Proxy 200
export UAC_PROXY200="uac_dtmf_G729.xml"                            // ���ɽ��� �ó���������

 

# UAC scenario
export UAC_SCENARIO="uac"


# UAS scenario
export UAS_SCENARIO="uas"

 

# UAC host and port
export UAC_HOST="100.100.107.186"                                        // ���ɽ��� ���μ����� �ִ� ���
export UAC_PORT="5160"

 

export DNIS="5000"                                                                // IVR DNIS

 

#CPS Rate
export CPSRATE=5                                                               // ���� �� �� (10�� ��� �ѹ��� 10�ݾ�)
export RATEPERIOD=1000                                                      // �� �ֱ� (1000 �ϰ�� 1��)
export MAXCALL=100                                                            // �׽�Ʈ �� �� ��

                                                                                          // ����� ���� �����̸� 1�� �ֱ�� 5�ݾ� �� 100�� �׽�Ʈ

                            

#Transport Mode
export TMODE=u1

 

# SIPp command
export SIPP_COMMAND=./sipp

 

# User Config File
export CFGFILE=irdb_perf.csv                                                    // DNIS�� �����ʰ� �� ä�κ� DN�� ���ν�Ű�� ������
