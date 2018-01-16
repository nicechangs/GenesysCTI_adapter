// LineCommand.cpp: implementation of the CLineCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "LineCommand.h"
#include <string.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLineCommand::CLineCommand() : _nCount(0)
{
}



/*****************************************************************
 *                           ~CCommand                           *
 *****************************************************************
 * All Right Reserved                                            *
 *                                                               *
 * Created at  :  2003-11-07   11:56                             *
 * Author      :  KIM, IN-SOO                                    *
 * Company Name:  Bridgetec etc,                                 *
 * File Name   :  VarTest.cpp                                    *
 * Description :  ����� ó�� Ŭ���� �Ҹ���                      *
 * Parameters  :  None                                           *
 * Return Value:                                                 *
 * Remarks     :                                                 *
 *                �����ū ����Ʈ�� �����Ѵ�.                    *
 *                                                               *
 *                                                               *
 *  Updated By     Date      Description                         *
 *  -------------+---------+---------------------                *
 *               |         |                                     *
 * All Right Reserved                                            *
 *****************************************************************/
CLineCommand::~CLineCommand()
{
    removeAll();
}

void CLineCommand::removeAll(void)
{
    TOKENLIST::iterator lt;

    for(lt = _lstToken.begin(); lt != _lstToken.end(); lt++)
        delete (*lt).second;

    _lstToken.clear();
}

/*****************************************************************
 *                          operator[]                           *
 *****************************************************************
 * All Right Reserved                                            *
 *                                                               *
 * Created at  :  2003-11-07   11:57                             *
 * Author      :  KIM, IN-SOO                                    *
 * Company Name:  Bridgetec etc,                                 *
 * File Name   :  VarTest.cpp                                    *
 * Description :  �ش���ġ�� �����ū�� ��ȯ�Ѵ�.                *
 * Parameters  :  nIndex -                                       *
 * Return Value:                                                 *
 * Remarks     :                                                 *
 *                                                               *
 *  Updated By     Date      Description                         *
 *  -------------+---------+---------------------                *
 *               |         |                                     *
 * All Right Reserved                                            *
 *****************************************************************/
CLineCommand::CCommandToken& CLineCommand::operator[] (int nIndex)
{
    TOKENLIST::iterator lt;
    static CCommandToken tmpToken;

    lt = _lstToken.find(nIndex);

    if(lt == _lstToken.end()) return tmpToken;

    return *(*lt).second;
}

//CLineCommand::CCommandToken& CLineCommand::operator[] (char* szName)
//{
//  TOKENLIST::iterator lt;
//  CCommandToken* pToken;
//  static CCommandToken tmpToken;
//
//  for(lt = _lstToken.begin(); lt != _lstToken.end(); lt++)
//  {
//      pToken = (*lt).second;
//      if(strcmp((*pToken).getName(), szName) == 0) return *(*lt).second;
//  }
//
//  return tmpToken;
//}

CLineCommand::CCommandToken& CLineCommand::operator[] (char* szName)
{
//  CCommandToken* pToken;
    static CCommandToken tmpToken;

    TOKENLIST2::iterator pos = _lstToken2.find(szName);

    if (pos != _lstToken2.end())
    {
        return *(*pos).second;
    }

    return tmpToken;
}

bool CLineCommand::IsName(const char* szName)
{
    //CCommandToken* pToken;
    static CCommandToken tmpToken;

    TOKENLIST2::iterator pos = _lstToken2.find(szName);

    if (pos != _lstToken2.end())
        return true;
    else
        return false;
}


/*****************************************************************
 *                           operator=                           *
 *****************************************************************
 * All Right Reserved                                            *
 *                                                               *
 * Created at  :  2003-11-07   11:58                             *
 * Author      :  KIM, IN-SOO                                    *
 * Company Name:  Bridgetec etc,                                 *
 * File Name   :  VarTest.cpp                                    *
 * Description :  ������� �����ū���� �м��Ѵ�.                *
 * Parameters  :  cpCommand -                                    *
 * Return Value:                                                 *
 * Remarks     :                                                 *
 *                                                               *
 *  Updated By     Date      Description                         *
 *  -------------+---------+---------------------                *
 *               |         |                                     *
 * All Right Reserved                                            *
 *****************************************************************/
CLineCommand& CLineCommand::operator= (char* cpCommand)
{
    char* pCommand;
    std::string sName;
    std::string sValue;
    CCommandToken* pToken;
    int nIndex;

    removeAll();

    pCommand = cpCommand;

    nIndex = 0;
    while(true) //** '=' �� �߰� �ߴ�. **/
    {
        pCommand = getName(pCommand, sName);
        if(pCommand == 0x00 || *pCommand == 0x00) break;
        pCommand = getValue(pCommand, sValue);
        if(pCommand == 0x00) break;

        pToken = new CCommandToken;
        pToken->set(sName.c_str(), sValue.c_str());
        _lstToken[nIndex++] = pToken;
        _lstToken2[sName.c_str()] = pToken;
    }

    _nCount = nIndex;

    return *this;
}


/*****************************************************************
 *                            getName                            *
 *****************************************************************
 * All Right Reserved                                            *
 *                                                               *
 * Created at  :  2003-11-07   11:59                             *
 * Author      :  KIM, IN-SOO                                    *
 * Company Name:  Bridgetec etc,                                 *
 * File Name   :  VarTest.cpp                                    *
 * Description :  ������� �̸��� �����Ѵ�.                      *
 * Parameters  :  cpCommand - ����� ���ڿ� ������               *
 *                sName     - ������ �����ū�̸� ��ȯ����       *
 * Return Value:                                                 *
 * Remarks     :                                                 *
 *                                                               *
 *  Updated By     Date      Description                         *
 *  -------------+---------+---------------------                *
 *               |         |                                     *
 * All Right Reserved                                            *
 *****************************************************************/
char* CLineCommand::getName(const char* cpCommand, std::string& sName)
{
    #define VARNAME_LEN 127

    char* pCommand;
    char* pDot, *pSpace;
    int nNameLen;
    char szName[VARNAME_LEN+1];

    sName = "";

    if(cpCommand == 0x00) return null;
    if(*cpCommand == 0x00) return null;

    pCommand = const_cast<char*>(cpCommand);

    while(*pCommand == ' ' || (unsigned char)*pCommand == 0x01 || (unsigned char)*pCommand == 0x04 || (unsigned char)*pCommand == 0x1E ) pCommand++;

    pDot = strchr(pCommand, '=');

    if(pDot == 0x00) return null;

    nNameLen = (pDot - pCommand);
    if(nNameLen > VARNAME_LEN) {    //** �̸����̰� VARNAME_LEN ���� ���� **//
        return null;
    }
    memcpy(szName, pCommand, nNameLen);
    szName[nNameLen] = 0x00;

    pSpace = strchr(szName, ' ');   //** ���� �����̽� ���� **//
    if(pSpace != 0x00) *pSpace = 0x00;

    sName = szName;

    pCommand = pDot + 1;    //** '=' skip **//

    return (char*)pCommand;
}


/*****************************************************************
 *                           getValue                            *
 *****************************************************************
 * All Right Reserved                                            *
 *                                                               *
 * Created at  :  2003-11-07   12:00                             *
 * Author      :  KIM, IN-SOO                                    *
 * Company Name:  Bridgetec etc,                                 *
 * File Name   :  VarTest.cpp                                    *
 * Description :  ������� ���� �����Ѵ�.                        *
 * Parameters  :  cpCommand - ����� ���ڿ� ������               *
 *                sValue    - ������ �����ū�� ��ȯ����         *
 * Return Value:                                                 *
 * Remarks     :                                                 *
 *                                                               *
 *  Updated By     Date      Description                         *
 *  -------------+---------+---------------------                *
 *               |         |                                     *
 * All Right Reserved                                            *
 *****************************************************************/
char* CLineCommand::getValue(const char* cpCommand, std::string& sValue)
{
    char cStartCode;
    char* pCommand;
    char* pDot;

    int nValueLen;
    char* pValue;

    sValue = "";

    if(cpCommand == 0x00) return null;
    if(*cpCommand == 0x00) return null;

    pCommand = const_cast<char*>(cpCommand);

    while(*pCommand == ' ') pCommand++;

    cStartCode = *pCommand;

    if(cStartCode == '\'' || cStartCode == '\"') {
        pCommand++;
    } else if(cStartCode == 0x01) { //** STX ETX �� ������ �����ڷ� �Ѵ�.  **//
        cStartCode = 0x04;
        pCommand++;
    }else {
        cStartCode = 0x1E;  //** ������ �����ڰ� ���°�� �����̽��� ���� �Ѵ�. **//
    }

    pDot = strchr(pCommand, cStartCode);

    if(pDot == 0x00) {
        pDot = pCommand + strlen(pCommand);
    }

    nValueLen = (pDot - pCommand);

    pValue = (char*)malloc(nValueLen+1);
    if(pValue == 0x00) {
        return null;
    }

    memcpy(pValue, pCommand, nValueLen);
    pValue[nValueLen] = 0x00;

    sValue = pValue;

    free(pValue);

    pCommand = pDot + 1;    //** '=' skip **//

    return (char*)pCommand;
}



/*****************************************************************
 *                            setName                            *
 *****************************************************************
 * All Right Reserved                                            *
 *                                                               *
 * Created at  :  2003-11-07   11:39                             *
 * Author      :  KIM, IN-SOO                                    *
 * Company Name:  Bridgetec etc,                                 *
 * File Name   :  VarTest.cpp                                    *
 * Description :  �̸��� ����                                    *
 * Parameters  :  cpName -                                       *
 * Return Value:                                                 *
 * Remarks     :                                                 *
 *                �����ū�� �̸����� �����Ѵ�.                  *
 *                                                               *
 *                                                               *
 *  Updated By     Date      Description                         *
 *  -------------+---------+---------------------                *
 *               |         |                                     *
 * All Right Reserved                                            *
 *****************************************************************/
void CLineCommand::CCommandToken::setName(const char* cpName)
{
    _sName = cpName;
}


/*****************************************************************
 *                           setValue                            *
 *****************************************************************
 * All Right Reserved                                            *
 *                                                               *
 * Created at  :  2003-11-07   11:51                             *
 * Author      :  KIM, IN-SOO                                    *
 * Company Name:  Bridgetec etc,                                 *
 * File Name   :  VarTest.cpp                                    *
 * Description :  �� ����                                        *
 * Parameters  :  cpValue -                                      *
 * Return Value:                                                 *
 * Remarks     :                                                 *
 *                �����ū�� ���� �����Ѵ�.                      *
 *                                                               *
 *  Updated By     Date      Description                         *
 *  -------------+---------+---------------------                *
 *               |         |                                     *
 * All Right Reserved                                            *
 *****************************************************************/
void CLineCommand::CCommandToken::setValue(const char* cpValue)
{
    _sValue = cpValue;
}

void CLineCommand::setKeyValue(const char* cpKey, const char* cpValue, char* pBuff)
{
    strcat(pBuff, cpKey);         // variable
    strcat(pBuff, "=");           // =
    pBuff[strlen(pBuff)] = 0x01;  //��
    strcat(pBuff, cpValue);         // value
    pBuff[strlen(pBuff)] = 0x04;  // ��
    strcat(pBuff, " ");           // space
}


/*****************************************************************
 *                              set                              *
 *****************************************************************
 * All Right Reserved                                            *
 *                                                               *
 * Created at  :  2003-11-07   11:52                             *
 * Author      :  KIM, IN-SOO                                    *
 * Company Name:  Bridgetec etc,                                 *
 * File Name   :  VarTest.cpp                                    *
 * Description :  �̸����� ���� �����Ѵ�.                        *
 * Parameters  :  cpName  -                                      *
 *                cpValue -                                      *
 * Return Value:                                                 *
 * Remarks     :                                                 *
 *                �����ū�� �̸����� ���� �����Ѵ�.             *
 *                                                               *
 *  Updated By     Date      Description                         *
 *  -------------+---------+---------------------                *
 *               |         |                                     *
 * All Right Reserved                                            *
 *****************************************************************/
void CLineCommand::CCommandToken::set(const char* cpName, const char* cpValue)
{
    _sName = cpName;
    _sValue = cpValue;
}


/*****************************************************************
 *                            getName                            *
 *****************************************************************
 * All Right Reserved                                            *
 *                                                               *
 * Created at  :  2003-11-07   11:53                             *
 * Author      :  KIM, IN-SOO                                    *
 * Company Name:  Bridgetec etc,                                 *
 * File Name   :  VarTest.cpp                                    *
 * Description :  �����ū�� �̸����� �д´�.                    *
 * Parameters  :  void -                                         *
 * Return Value:                                                 *
 * Remarks     :                                                 *
 *                �����ū�� �̸����� �д´�.                    *
 *                                                               *
 *  Updated By     Date      Description                         *
 *  -------------+---------+---------------------                *
 *               |         |                                     *
 * All Right Reserved                                            *
 *****************************************************************/
const char* CLineCommand::CCommandToken::getName(void)
{
    return _sName.c_str();
}


/*****************************************************************
 *                           getValue                            *
 *****************************************************************
 * All Right Reserved                                            *
 *                                                               *
 * Created at  :  2003-11-07   11:54                             *
 * Author      :  KIM, IN-SOO                                    *
 * Company Name:  Bridgetec etc,                                 *
 * File Name   :  VarTest.cpp                                    *
 * Description :  �����ū�� ���� �д´�.                        *
 * Parameters  :  void -                                         *
 * Return Value:                                                 *
 * Remarks     :                                                 *
 *                �����ū�� ���� �д´�.                        *
 *                                                               *
 *  Updated By     Date      Description                         *
 *  -------------+---------+---------------------                *
 *               |         |                                     *
 * All Right Reserved                                            *
 *****************************************************************/
const char* CLineCommand::CCommandToken::getValue(void)
{
    return _sValue.c_str();
}

