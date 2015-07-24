#pragma once

class CImageFileHandler
{
public:
	static bool LoadBMP(LPCTSTR szFilePath, long& cx, long& cy, long& step, BYTE*& p);
	static bool SaveBMP(LPCTSTR szFilePath, long cx, long cy, long step, BYTE* p);
};