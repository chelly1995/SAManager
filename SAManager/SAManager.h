#pragma once


// 커맨드라인 매니저
class CmdManager {	// 솔루션파일인지 프로젝트파일인지 구분하는 함수

public:
	CmdManager();
	static CmdManager* GetInst();
	~CmdManager();
	void  ParserCommandLine();
	wstring GetSinPath();
	wstring GetPinPath();
	wstring GetAdd();
	WCHAR* GetOutPath();
	WCHAR* GetProductName();
	WCHAR* GetSolutionPath();
	WCHAR* GetProjectPath();
	
	BOOL CheckSinPath();
	BOOL CheckPinPath();
	BOOL CheckAdd();
	int CheckCmd();

private:
	map<wstring, wstring> m_mapCommandLine;
	

};