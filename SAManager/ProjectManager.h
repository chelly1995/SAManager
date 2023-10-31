#pragma once


class ProjectManager
{

public:
	static ProjectManager* GetInst();
	ProjectManager();
	~ProjectManager();

	int GetSlnList(WCHAR* pSolutionPath); // �ַ�� ���� ����Ʈ ���� ȹ��
	int GetProjectInform(WCHAR* pProjectPath);	// ���� ������Ʈ ���� ���� ȹ��
	int GetInform();// (cpp, c, h) ȹ��
	int AnalyzeProject(WCHAR* pProjectPath);	// (cpp, c, h) ȹ���� ���� ������Ʈ �м�
	int CheckDuplicate(WCHAR* pPath);			// (cpp, c, h) ���� �ߺ� üũ

	int MakeCdprojFile(WCHAR* pPath, WCHAR* pProductName); // ��������Ʈ��.cdproj�� ���� ����
	int MakePropertiesFile(WCHAR* pPath, WCHAR* pProductName); // ����ǰ��_project.properties�� ���� ����
	int AddPropertiesFile(WCHAR* pPath, WCHAR* pProductName); // ����ǰ��_project.properties�� ���� �߰�

	int GetModuleName(WCHAR* pLine, WCHAR* pModule);	// �ַ������ ����̸� ȹ��
	int GetPathName(WCHAR* pLine, WCHAR* pProjectPath);	// �ַ������ ��� ȹ��
	int ChangeFullPath(WCHAR* pProjectPath, WCHAR* pSolutionPath);	// ����θ� �����η� ��ȯ
	int ChangeSlash(WCHAR* pFilePath);
	int AddData(WCHAR* pPath, WCHAR* pProductName); // properties ���ϳ��� �߰�

	int LoadXmlFile(WCHAR* pFilePath);
	int SaveXmlFile(WCHAR* pFilePath);

private:
	map<wstring, wstring> m_mapProject;
	WCHAR szSolutionPath[MAX_PATH] = { 0, };
	vector<wstring> m_vFilelist;
	map<wstring, vector<wstring>> m_mapSolFiles;

};

