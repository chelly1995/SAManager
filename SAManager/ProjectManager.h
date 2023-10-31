#pragma once


class ProjectManager
{

public:
	static ProjectManager* GetInst();
	ProjectManager();
	~ProjectManager();

	int GetSlnList(WCHAR* pSolutionPath); // 솔루션 파일 리스트 정보 획득
	int GetProjectInform(WCHAR* pProjectPath);	// 단일 프로젝트 파일 정보 획득
	int GetInform();// (cpp, c, h) 획득
	int AnalyzeProject(WCHAR* pProjectPath);	// (cpp, c, h) 획득을 위한 프로젝트 분석
	int CheckDuplicate(WCHAR* pPath);			// (cpp, c, h) 파일 중복 체크

	int MakeCdprojFile(WCHAR* pPath, WCHAR* pProductName); // ‘프로젝트명.cdproj’ 파일 생성
	int MakePropertiesFile(WCHAR* pPath, WCHAR* pProductName); // ‘제품명_project.properties’ 파일 생성
	int AddPropertiesFile(WCHAR* pPath, WCHAR* pProductName); // ‘제품명_project.properties’ 내용 추가

	int GetModuleName(WCHAR* pLine, WCHAR* pModule);	// 솔루션파일 모듈이름 획득
	int GetPathName(WCHAR* pLine, WCHAR* pProjectPath);	// 솔루션파일 경로 획득
	int ChangeFullPath(WCHAR* pProjectPath, WCHAR* pSolutionPath);	// 상대경로를 절대경로로 변환
	int ChangeSlash(WCHAR* pFilePath);
	int AddData(WCHAR* pPath, WCHAR* pProductName); // properties 파일내용 추가

	int LoadXmlFile(WCHAR* pFilePath);
	int SaveXmlFile(WCHAR* pFilePath);

private:
	map<wstring, wstring> m_mapProject;
	WCHAR szSolutionPath[MAX_PATH] = { 0, };
	vector<wstring> m_vFilelist;
	map<wstring, vector<wstring>> m_mapSolFiles;

};

