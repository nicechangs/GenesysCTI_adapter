//
//  iRecSet.h
//  iCore
//
//  Created by Tony Ryu on 2012. 10. 4..
//  Copyright (c) 2012년 Tony Ryu. All rights reserved.
//

/**
 * @file    iRecSet.h
 * @brief   iSqlite에서 사용되는 Recording Set
 *
 * @version v0.1
 * @see     None
 * @date    2012. 10. 4.
 */

#pragma once

#include <map>
#include <string>
#include <sqlite3.h>
#include "iGrid.h"

typedef std::map<std::string, uint16>	iColIdx;			// Map for Column Searching
typedef std::map<uint16, uint8>			iColType;			// Column Type 저장.
typedef std::map<uint16, std::string>	iColName;			// Column Name 저장.
typedef iColIdx::iterator				iColIter;

enum
{
	iRecNull        = SQLITE_NULL,
	iRecBlob        = SQLITE_BLOB,
	iRecInt         = SQLITE_INTEGER,
	iRecFloat       = SQLITE_FLOAT,
	iRecText        = SQLITE_TEXT,
};

enum
{
	iRecDml_None			= 0,
	iRecDml_Insert,
	iRecDml_Update,
	iRecDml_Delete,
	iRecDml_Select,
	iRecDml_UsrType			= 0x1000,
};

const uint16 IGRIDX_NULL = (0xffff);
const char IGRSTR_NULL[] = "";

class ILIBAPI iRecSet : public iGrid
{
public:
	iRecSet();
	virtual ~iRecSet();

	void Clear();

	// Data Getting Methods
	inline NPCSTR GetDbName() { return mszDbName; };
	inline NPCSTR GetTable() { return mszTable; };
	inline NPCSTR GetTrigger() { return mszTrigger; };
	inline int32 GetDmlType() { return mnDmlType; };
	inline uint16 GetRowCnt() { return mnRows; };
	inline uint16 GetColCnt() { return mnCols; };

	bool IsExistCol(NPCSTR szColumn);
	uint16 GetColIdx(NPCSTR szColumn);
	NPCSTR GetColumn(uint16 nColumn);
	uint8 GetColType(uint16 nColumn);

	inline bool IsExistBefore(uint16 nRow, NPCSTR szColumn)
	{ return IsExistBefore(nRow, GetColIdx(szColumn)); };
	inline bool IsChanged(uint16 nRow, NPCSTR szColumn)
	{ return IsChanged(nRow, GetColIdx(szColumn)); };
	inline NPCSTR GetItemTxt(uint16 nRow, NPCSTR szColumn, bool bBefore = false)
	{ return GetItemTxt(nRow, GetColIdx(szColumn), bBefore); };
	inline int64 GetItemInt(uint16 nRow, NPCSTR szColumn, bool bBefore = false)
	{ return GetItemInt(nRow, GetColIdx(szColumn), bBefore); };

	bool IsExistBefore(uint16 nRow, uint16 nCol);
	bool IsChanged(uint16 nRow, uint16 nCol);
	NPCSTR GetItemTxt(uint16 nRow, uint16 nCol, bool bBefore = false);
	int64 GetItemInt(uint16 nRow, uint16 nCol, bool bBefore = false);

	// Data Setting Method
	void SetDbName(NPCSTR szDbName);
	void SetTable(NPCSTR szTable);
	void SetTrigger(NPCSTR szTrigger);
	void SetDmlType(int32 nDmlType);

	uint16 AddColumn(NPCSTR szColumn, uint8 nColType);
	bool SetColType(uint16 nColumn, uint8 nColType);

	inline void SetRowCnt(uint16 nRow) { mnRows = nRow; };
	inline bool SetItemTxt(uint16 nRow, NPCSTR szColumn, NPCSTR szVal, bool bBefore = false)
	{ return SetItemTxt(nRow, GetColIdx(szColumn), szVal, bBefore); };
	inline bool SetItemInt(uint16 nRow, NPCSTR szColumn, int64 nVal, bool bBefore = false)
	{ return SetItemInt(nRow, GetColIdx(szColumn), nVal, bBefore); };
	inline bool SetItemNil(uint16 nRow, NPCSTR szColumn, bool bBefore = false)
	{ return SetItemNil(nRow, GetColIdx(szColumn), bBefore); };

	bool SetItemTxt(uint16 nRow, uint16 nCol, NPCSTR szVal, bool bBefore = false);
	bool SetItemInt(uint16 nRow, uint16 nCol, int64 nVal, bool bBefore = false);
	bool SetItemNil(uint16 nRow, uint16 nCol, bool bBefore = false);

	// Data Import Methods
	// nRow = -1의 경우 New Row를 추가함.
	bool ImpRow2Old(iRecSet& rOther, uint16 nOther, int32 nRow = -1);
	bool ImpRow2New(iRecSet& rOther, uint16 nOther, int32 nRow = -1);

	// Data Export Methods
	NPCSTR ExpInsertQry(uint16 nRow);
	NPCSTR ExpUpdateQry(uint16 nRow, NPCSTR szTail, ...);
	NPCSTR ExpSelectQry(NPCSTR szTail, ...);

	NPCSTR ExpColsQry();
	NPCSTR ExpValsQry(uint16 nRow);
	NPCSTR ExpPairQry(uint16 nRow);

	size_t ExpSelString(NPSTR szBuf, size_t nBuf, bool bIncCols = false, NPCSTR szDeliRow = NULL, NPCSTR szDeliCol = NULL);

protected:
	iColType mapColType;
	iColName mapColName;
	iColIdx mapColIdx;

	char mszQuery[MAXLEN_LINE64];
	char mszDbName[MAXLEN_PATH];
	char mszTable[MAXLEN_PATH];
	char mszTrigger[MAXLEN_PATH];
	int32 mnDmlType;

	uint16 mnRows;
	uint16 mnCols;

	DISALLOW_COPY_AND_ASSIGN(iRecSet);
};


