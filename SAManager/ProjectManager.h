#pragma once


class ProjectManager
{

public:
	static ProjectManager* GetInst();
	ProjectManager();
	~ProjectManager();

	int GetProjectList(WCHAR* pSolutionPath); // �ַ�� ���Ͽ��� ������Ʈ ����Ʈ ���� ȹ��
	int GetProjectInform(WCHAR* pProjectPath);	// ������Ʈ ���� ���� ȹ��
	int GetInform();
	int AnalyzeProject(WCHAR* pProjectPath);	// ������Ʈ �� �м� ��� ���� (cpp, c, h) ȹ��
	int MakeCdprojFile(WCHAR* pPath, WCHAR* pProductName); // ��������Ʈ��.cdproj�� ���� ����
	int MakePropertiesFile(WCHAR* pPath, WCHAR* pProductName); // ����ǰ��_project.properties�� ���� ����

	int GetModuleName(WCHAR* pLine, WCHAR* pModule);	// �ַ������ ����̸� ȹ��
	int GetPathName(WCHAR* pLine, WCHAR* pProjectPath);	// �ַ������ ��� ȹ��
	int ChangeFullPath(WCHAR* pProjectPath, WCHAR* pSolutionPath);	// ����θ� �����η� ��ȯ

	int LoadXmlFile(WCHAR* pFilePath);
	int SaveXmlFile(WCHAR* pFilePath);

private:
	map<wstring, wstring> m_mapProject;
	WCHAR szSolutionPath[MAX_PATH] = { 0, };
	vector<wstring> m_vFilelist;
	map<wstring, vector<wstring>> m_mapSolFiles;

};

