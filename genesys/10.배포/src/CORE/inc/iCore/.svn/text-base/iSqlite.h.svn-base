//
//  iSqlite.h
//  iCore
//
//  Created by Tony Ryu on 2012. 10. 4..
//  Copyright (c) 2012년 Tony Ryu. All rights reserved.
//

/**
 * @file    iSqlite.h
 * @brief   Sqlite DB 처리
 *
 * @version v0.1
 * @see     None
 * @date    2012. 10. 4.
 */

#pragma once

#include "iRecSet.h"
#include <sqlite3.h>

typedef void (*PSQLFUNC)(sqlite3_context*,int32,sqlite3_value**);
typedef void (*PSQLFINAL)(sqlite3_context*);

typedef int32 (*PAUTHORFUNC)(NPVOID pUsr, int nOper, NPCSTR szTable, NPCSTR szColumn, NPCSTR szDb, NPCSTR szTrigger);
typedef void (*PUPDATEFUNC)(NPVOID pUsr, int nOper, NPCSTR szDb, NPCSTR szTable, sqlite3_int64 nRowId);
typedef int (*PCOMMITFUNC)(NPVOID pUsr);
typedef void (*PROLLBACKFUNC)(NPVOID pUsr);

#define MAXLEN_SQLITE               (MAXLEN_LINE*64)    // 64KB
#define MEMDB_FUNCNAME(N,F,C)       AddUsrFunc(#N, C, F, NULL, NULL)
#define MEMDB_FUNCAGGR(N,S,F,C)     AddUsrFunc(#N, C, NULL, S, F)

enum
{
	ISQLERR_DBBACKUP	= 0x0100,
	ISQLERR_FILEOPEN	,
	ISQLERR_NOTOPENED	,
};

class ILIBAPI iSqlite : public iObj
{
public:
	iSqlite();
	virtual ~iSqlite();

	bool CreateDbInMem();
	bool CreateDb(NPCSTR szDbFile);
	bool OpenDb(NPCSTR szDbFile, bool bNoExistErr = true);
	bool AttachDb(NPCSTR szDbFile, NPCSTR szDbName);
	bool CloseDb();

	inline bool IsOpened() { return bool(mpDb!=NULL); };
	inline NPCSTR GetDbFile() { return mszDbFile; };

	bool NeedReopen(bool bIfDbExist = true);

	bool BackupDb(NPCSTR szBakFile);
	bool RestoreDb(NPCSTR szRstFile);

	bool BackupDb(iSqlite& stBackup);
	bool RestoreDb(iSqlite& stRestore);

	bool ExecSql(NPCSTR szFmt, ...);
	bool ExecSel(iRecSet& rRec, NPCSTR szFmt, ...);
	bool ExecSel(int64& nVal, NPCSTR szFmt, ...);
	bool ExecSel(NPSTR szBuf, size_t nBuf, NPCSTR szFmt, ...);
	bool ExecFile(NPCSTR szSqlFile);

	int32 GetUserVer();
	bool SetUserVer(int32 nUserVer);

	int64 GetLastInsRowId();
	inline sqlite3* GetDbPtr() { return mpDb; };
	inline int32 GetErrCode() { return mnError; };
	inline NPCSTR GetErrMsg() { return mszError; };
	inline uint32 GetLastLine() { return mnLine; };

	bool AddUsrFunc(NPCSTR szFunc, int32 nArg, PSQLFUNC pFunc, PSQLFUNC pStep, PSQLFINAL pFinal);

	NPVOID SetUpdateCallback(PUPDATEFUNC pFunc, NPVOID pUsr);
	NPVOID SetCommitCallback(PCOMMITFUNC pFunc, NPVOID pUsr);
	NPVOID SetRollbackCallback(PROLLBACKFUNC pFunc, NPVOID pUsr);
	void SetAuthorizerCallback(PAUTHORFUNC pFunc, NPVOID pUsr);

	static NPCSTR GetErrMsg(int32 nError);
	static bool IsValidSql(NPCSTR szSql);
	static bool BackupDb(iSqlite& stDb, NPCSTR szBakFile);

protected:
	bool iExecSql(NPCSTR szSql);
	bool iExecSel(iRecSet& rRec, NPCSTR szSql);
	bool iExecSel(int64& nVal, NPCSTR szSql);
	bool iExecSel(NPSTR szBuf, size_t nBuf, NPCSTR szSql);
	bool iExecPos(NPCSTR szSql, NPCSTR& rPos);

	bool SetError(int32 nError, NPCSTR szUsrErr = NULL);
	void ClearErr();

	virtual bool OnOpened(NPCSTR szDbFile) { return true; };

private:
	ino_t miNode;
	sqlite3 *mpDb;
	char mszDbFile[MAXLEN_PATH];

	uint32 mnLine;
	int32 mnError;
	char mszError[MAXLEN_LINE];

};
