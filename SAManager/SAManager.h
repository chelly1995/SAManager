#pragma once


// Ŀ�ǵ���� �Ŵ���
class CmdManager {	// �ַ���������� ������Ʈ�������� �����ϴ� �Լ�

public:
	CmdManager();
	static CmdManager* GetInst();
	~CmdManager();
	void  ParserCommandLine();
	wstring GetSinPath();
	wstring GetPinPath();
	WCHAR* GetOutPath();
	WCHAR* GetProductName();
	WCHAR* GetSolutionPath();
	WCHAR* GetProjectPath();
	
	BOOL CheckSinPath();
	BOOL CheckPinPath();
	int CheckCmd();

private:
	map<wstring, wstring> m_mapCommandLine;
	

};