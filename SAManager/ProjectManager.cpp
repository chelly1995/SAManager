#include "framework.h"
#include "ProjectManager.h"
#include "tinyxml2.h"
#include <cstdlib>
using namespace tinyxml2;

tinyxml2::XMLDocument m_XMLDoc;
XMLElement* m_pDataElement;

ProjectManager* ProjectManager::GetInst()
{
	static ProjectManager Inst;
	return &Inst;
}

ProjectManager::ProjectManager()
{

}

ProjectManager::~ProjectManager()
{

}

int ProjectManager::GetSlnList(WCHAR* pSolutionPath)
{
	if (NULL == pSolutionPath)
		return ERROR_INVALID_PARAMETER;

	FILE* fp = NULL;
	errno_t err = _wfopen_s(&fp, pSolutionPath, L"rt");

	if (ERROR_SUCCESS != err || NULL == fp)
	{
		return ERROR_OPEN_FAILED;
	}

	WCHAR szLine[2048] = {0, };
	WCHAR szModuleName[MAX_PATH] = { 0, };
	WCHAR szProjectPath[MAX_PATH] = { 0, };
	wcscpy_s(szSolutionPath, MAX_PATH, pSolutionPath);


	while (fgetws(szLine, 2048, fp) != NULL) {
	
		if(0 == _wcsicmp(szLine, L"Global"))
			break;
		
		if (NULL == wcsstr(szLine, L"Project(\"{"))
			continue;

		if (NULL == wcsstr(szLine, L".vcxproj"))
			continue;

		WCHAR szcpyLine[2048] = { 0, };
		wcscpy_s(szcpyLine, 2048, szLine);

		if (ERROR_SUCCESS == ProjectManager::GetInst()->GetModuleName(szLine,szModuleName))
		{
			DebugMsg(L" EJH %s(%d) : Debug Msg [%s][%s]", __FUNCTIONW__, __LINE__, L"Modulename", szModuleName);
		}
		if (ERROR_SUCCESS == ProjectManager::GetInst()->GetPathName(szcpyLine, szProjectPath))
		{
			DebugMsg(L" EJH %s(%d) : Debug Msg [%s][%s]", __FUNCTIONW__, __LINE__, L"Projectpath", szProjectPath);
		}

		m_mapProject.insert(pair<wstring, wstring>(szModuleName,szProjectPath));
		SecureZeroMemory(szLine, 2048*2);
	
	}

	::fclose(fp);

	return ERROR_SUCCESS;

}

int ProjectManager::GetInform()
{
	map<wstring, wstring>::iterator itor;
	for (itor = m_mapProject.begin(); itor != m_mapProject.end(); itor++)
	{
		 DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, itor->second.c_str());
		
		 WCHAR pPath[MAX_PATH] = {0, };
		 wcscpy_s(pPath, MAX_PATH, itor->second.c_str());
	
		 AnalyzeProject((WCHAR*)pPath);

		// c, cpp, h 파일 리스트를 맵에 담는다.
		m_mapSolFiles.insert(pair<wstring, vector<wstring>>(itor->first.c_str(), m_vFilelist));
		m_vFilelist.clear();
		
	}
	DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"GetSlnInform_SUCCESS!!");

	return ERROR_SUCCESS;
}

int ProjectManager::GetProjectInform(WCHAR* pProjectPath)
{
	
	WCHAR szModuleName[MAX_PATH] = {0, };
	WCHAR szProjectPath[MAX_PATH] = { 0, };
	
	wcscpy_s(szProjectPath, MAX_PATH, pProjectPath);
	
	WCHAR* pFind = wcsrchr(pProjectPath, L'\\');
	wcscpy_s(szModuleName, MAX_PATH, pFind+1);

	WCHAR* pPathFind = wcsstr(szModuleName, L".vcxproj");
	if (NULL != pPathFind)
		*pPathFind = 0x00;

	m_mapProject.insert(pair<wstring, wstring>(szModuleName, szProjectPath));
	
	DebugMsg(L" EJH %s(%d) : Debug Msg [%s][%s]", __FUNCTIONW__, __LINE__, szModuleName, szProjectPath);

	return ERROR_SUCCESS;
}

int ProjectManager::AnalyzeProject(WCHAR* pProjectPath)
{

	DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"AnalyzeProject_Access");

	if (NULL == pProjectPath)
		return ERROR_INVALID_PARAMETER;

	FILE* fp = NULL;
	errno_t err = _wfopen_s(&fp, pProjectPath, L"rt"); // or r+t

	if (ERROR_SUCCESS != err || NULL == fp)
	{
		return ERROR_OPEN_FAILED;
	}

	WCHAR szLine[2048] = { 0, };
	WCHAR szCopyLine[2048] = { 0, };
	while (fgetws(szLine, 2048, fp) != NULL) {
		
		if (NULL != wcsstr(szLine, L"<ClInclude Include=\"") or NULL != wcsstr(szLine, L"<ClCompile Include=\""))
		{

			 WCHAR* pFind;
		 pFind = wcsstr(szLine, L"Include=\"");

		 if (NULL != pFind)
			 wcscpy_s(szLine, MAX_PATH, pFind + 9);

		 pFind = wcsstr(szLine, L"\"/>");
		 if (NULL != pFind)
			 *pFind = 0x00;

		 pFind = wcsstr(szLine, L"\">");
		 if (NULL != pFind)
			 *pFind = 0x00;

		 pFind = wcsstr(szLine, L"\" />");
		 if (NULL != pFind)
			 *pFind = 0x00;

		 ChangeFullPath(szLine, pProjectPath);



		 WCHAR* pDFind = wcsstr(szLine, L"Development\\");
		 if (NULL != pDFind) {
			 wcscpy_s(szLine, MAX_PATH, pDFind);
		 }

		 WCHAR* pSFind = wcsstr(szLine, L"SWork\\");
		 if (NULL != pSFind)
		 {
			 wcscpy_s(szLine, MAX_PATH, pSFind);
		 }

		  DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, szLine);

		  // szLine copy
			 ChangeSlash(szLine);
			 
			 // TODO : 벡터에 insert하기 전에 szLine이 map에 있는 벡터 value에 있는지 확인한다.
				
			 if (ERROR_SUCCESS == CheckDuplicate(szLine))
			 {
				 vector <wstring>::iterator it;

				 it = m_vFilelist.begin();
				 it = m_vFilelist.insert(it, szLine);
			 }
			  SecureZeroMemory(szLine, 2048 * 2);
			
		}
	}

	::fclose(fp);

	return ERROR_SUCCESS;
}

int ProjectManager::ChangeSlash(WCHAR* pFilePath)
{
	DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"ChangeSlash");

	if (NULL == pFilePath)
		return ERROR_INVALID_PARAMETER;

	// 벡터에 문자열 잘라서 담기

	WCHAR pSrcLine[MAX_PATH] = { 0, };
	WCHAR pDstLine[MAX_PATH] = { 0, };
	WCHAR* buffer = NULL;
	wcscpy_s(pSrcLine, MAX_PATH, pFilePath);
	vector<wstring> m_vector;

	WCHAR* temp = wcstok_s(pSrcLine, L"\\", &buffer);

	while (temp != NULL)
	{
		m_vector.push_back(temp);
		temp = wcstok_s(NULL, L"\\", &buffer);
	}

	vector<wstring>::iterator iter;

	for (iter = m_vector.begin(); iter < m_vector.end(); iter++)
	{
			wcscat_s(pDstLine, iter->c_str());
			if (iter != m_vector.end()-1) {
				wcscat_s(pDstLine, L"/");
			}
		
	}

	DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, pDstLine);

	wcscpy_s(pFilePath, MAX_PATH, pDstLine);

	return ERROR_SUCCESS;
}

int ProjectManager::CheckDuplicate(WCHAR* pPath)
{
	if (NULL == pPath)
		return ERROR_INVALID_PARAMETER;

	WCHAR pLine[MAX_PATH] = { 0, };
	wcscpy_s(pLine, MAX_PATH, pPath);

	map<wstring, vector<wstring>>::iterator iter;
	vector<wstring>::iterator viter;

	for (viter = m_vFilelist.begin(); viter != m_vFilelist.end(); viter++)
	{
		if (viter->c_str() == (wstring)pLine)
		{
			DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, pLine);
			return ERROR_INVALID_DATA;
		}
	}

	for (iter = m_mapSolFiles.begin(); iter != m_mapSolFiles.end(); iter++)
	{
		for (viter = iter->second.begin(); viter != iter->second.end(); viter++)
		{
			if (viter->c_str() == (wstring)pLine)
			{
				DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__,pLine);
				return ERROR_INVALID_DATA;
			}
		}
	}

	return ERROR_SUCCESS;

}

int ProjectManager::MakeCdprojFile(WCHAR* pPath, WCHAR* pProductName)
{
	DebugMsg(L" EJH %s(%d) : Debug Msg [%s][%s]", __FUNCTIONW__, __LINE__, L"MakeCdprojFile",pPath);

	if (NULL == pPath)
		return ERROR_INVALID_PARAMETER;

	map<wstring, wstring>::iterator iter;
	for (iter = m_mapProject.begin(); iter != m_mapProject.end(); iter++)
	{

		WCHAR szExistPath[MAX_PATH];	// 기존 파일 경로
		WCHAR szModuleName[MAX_PATH];	// 새로 생성할 modulename.cdproj 파일 경로
		WCHAR szChangeLine[MAX_PATH];	// 변경할 내용
		WCHAR szLine[MAX_PATH];
		
		WCHAR basedir[MAX_PATH] = L"_Analysis_BaseDir\\CodeAnalysis\\";
		wcscat_s(basedir, MAX_PATH, pProductName);

		GetModuleFileName(NULL, szExistPath, MAX_PATH);
		WCHAR* pFindPath = wcsrchr(szExistPath, L'\\');
		if (NULL != pFindPath)
			*pFindPath = 0x00;

		wcscat_s(szExistPath, MAX_PATH, L"\\");
		wcscat_s(szExistPath, MAX_PATH, L"common.cdproj");

		wcscpy_s(szModuleName, pPath);
		wcscat_s(szModuleName, MAX_PATH, L"\\");
		wcscat_s(szModuleName, MAX_PATH, pProductName);
		_wmkdir(szModuleName);
		wcscat_s(szModuleName, MAX_PATH, L"\\");
		wcscat_s(szModuleName, MAX_PATH, iter->first.c_str());
		wcscat_s(szModuleName, MAX_PATH, L".cdproj");


		wcscpy_s(szChangeLine, iter->second.c_str());
		wcscpy_s(szLine, L"_sourcesDirectory\\");

			
		WCHAR* pDFind = wcsstr(szChangeLine, L"Development\\");
		if (NULL != pDFind) {
			wcscpy_s(szChangeLine, MAX_PATH, pDFind);
			wcscat_s(szChangeLine, L"??Release|x86");
			wcscat_s(szLine, szChangeLine);
		}

		WCHAR* pSFind = wcsstr(szChangeLine, L"SWork\\");
		if (NULL != pSFind)
		{
			wcscpy_s(szChangeLine, MAX_PATH, pSFind);
			wcscat_s(szChangeLine, L"??Release|x86");
			wcscat_s(szLine, szChangeLine);
		}

		// Unicode를 Ansi code로 변환
		size_t i;
		char* str = (char*)malloc(MAX_PATH);
		wcstombs_s(&i, str, MAX_PATH, szLine, MAX_PATH-1);

		size_t j;
		char* strProductname = (char*)malloc(MAX_PATH);
		wcstombs_s(&j, strProductname, MAX_PATH, pProductName, MAX_PATH - 1);

		size_t k;
		char* str2 = (char*)malloc(MAX_PATH);
		wcstombs_s(&k, str2, MAX_PATH, basedir, MAX_PATH - 1);

		LoadXmlFile(szExistPath);

		XMLElement* pRootElement = m_XMLDoc.FirstChildElement("NDepend");

		pRootElement->SetAttribute("AppName", strProductname);

		if (NULL != pRootElement)
		{
			XMLElement* pOutputElement = pRootElement->FirstChildElement("OutputDir");
			if (NULL != pOutputElement)
			{
				pOutputElement->SetText(str2);
			}

			XMLElement* pProjectElement = pRootElement->FirstChildElement("Projects");
			if (NULL != pProjectElement)
			{
				XMLElement* pNameElement = pProjectElement->FirstChildElement("Name");
				pProjectElement->DeleteChild(pNameElement);
				pNameElement = NULL;
				pNameElement = m_XMLDoc.NewElement("Name");
				pNameElement->SetText(str);	
				pProjectElement->LinkEndChild(pNameElement);
			}
		}
		SaveXmlFile(szModuleName);
	}

		return ERROR_SUCCESS;
}

int ProjectManager::MakePropertiesFile(WCHAR* pPath, WCHAR* pProductName)
{
	DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"MakePrcpertiesFile");

	if (NULL == pPath)
		return ERROR_INVALID_PARAMETER;

	WCHAR szFilePath[MAX_PATH];
	wcscpy_s(szFilePath, MAX_PATH, pPath);
	wcscat_s(szFilePath, L"\\");
	wcscat_s(szFilePath, pProductName);
	wcscat_s(szFilePath, L"_project.properties");

	FILE* fp = NULL;
	errno_t err = _wfopen_s(&fp, szFilePath, L"wt");

	if (ERROR_SUCCESS != err || NULL == fp)
	{
		return ERROR_OPEN_FAILED;
	}

	// 원본이랑 동일하게
	fputws(L"sonar.projectKey=SoftCamp.", fp);
	fputws(pProductName, fp);
	fputws(L"\nsonar.projectName=",fp);
	fputws(pProductName, fp);
	fputws(L"\n"
		"sonar.projectVersion=1.0\n"
		"sonar.sourceEncoding=x-windows-949\n"
		"sonar.language=cxx\n"
		"sonar.exclusions = *.java , *.xml , *.rc\n"
		"sonar.inclusions=**/*.cpp, **/*.c , **/*.h\n"
		"sonar.cppdepend.output=_Analysis_BaseDir\n"
		"sonar.working.directory=_Analysis_BaseDir/sonartemp\n"
		"sonar.cpp.cppdepend.runAnalysis=false\n"
		"sonar.cpp.cppdepend.reportPath=_Analysis_BaseDir\n"
		"sonar.projectBaseDir=_projectBaseDir\n\n\n\n", fp);
	
	fputws(L"sonar.modules=",fp);
	
	map<wstring, wstring>::iterator itor;
	map<wstring, vector<wstring>>::iterator iter;

	for (itor = m_mapProject.begin(); itor != m_mapProject.end(); itor++)
	{
		fputws(itor->first.c_str(), fp);
		fputws(L",",fp);
	}

	fputws(L"\n\n\n",fp);
	
	for (itor = m_mapProject.begin(); itor != m_mapProject.end(); itor++)
	{
		fputws(itor->first.c_str(), fp);
		fputws(L".sonar.projectBaseDir=_projectBaseDir",fp);
		fputws(L"\n", fp);


		fputws(itor->first.c_str(), fp);
		fputws(L".sonar.sources=.", fp);
		fputws(L"\n", fp);

		fputws(itor->first.c_str(), fp);
		fputws(L".sonar.exclusions=**//*.*", fp);
		fputws(L"\n", fp);

		fputws(itor->first.c_str(), fp);
		fputws(L".sonar.inclusions=", fp);
		
			// .h, .cpp, .c 파일 포함
		iter = m_mapSolFiles.find(itor->first.c_str());	// mapProject에 모듈명과 같으면
		if (iter != m_mapSolFiles.end()) {
				for (int i = 0; i < iter->second.size(); i++)
				{
					fputws(iter->second[i].c_str(), fp);
					fputws(L",", fp);
				}
		}
		
		fputws(L"\n", fp);
	}

	::fclose(fp);

	return ERROR_SUCCESS;
}

int ProjectManager::AddPropertiesFile(WCHAR* pPath, WCHAR* pProductName)
{
	DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"AddPropertiesFile");

	FILE* fp1 = NULL;	// 원본
	FILE* fp2 = NULL;	// 복사본
	WCHAR szFilePath[MAX_PATH];
	WCHAR sztempFilePath[MAX_PATH];
	WCHAR szLine[2048] = { 0, };

	wcscpy_s(szFilePath, MAX_PATH, pPath);
	wcscat_s(szFilePath, L"\\");
	wcscat_s(szFilePath, pProductName);
	wcscat_s(szFilePath, L"_project.properties");
	
	wcscpy_s(sztempFilePath, MAX_PATH, pPath);
	wcscat_s(sztempFilePath, L"\\");
	wcscat_s(sztempFilePath, pProductName);
	wcscat_s(sztempFilePath, L"_tempproject.properties");
	
	if ((_waccess(szFilePath, 00) != -1))
	{
		errno_t err1 = _wfopen_s(&fp1, szFilePath, L"r+t");

		if (ERROR_SUCCESS != err1 || NULL == fp1)
		{
			return ERROR_OPEN_FAILED;
		}

		errno_t err2 = _wfopen_s(&fp2, sztempFilePath, L"w+t");

		if (ERROR_SUCCESS != err2 || NULL == fp2)
		{
			return ERROR_OPEN_FAILED;
		}

		while (fgetws(szLine, MAX_PATH, fp1) != NULL)
		{
			fputws(szLine,fp2);
			
			if (NULL != wcsstr(szLine, L"sonar.modules="))
			{
				DebugMsg(L" EJH %s(%d) : Debug Msg [%s][%d]", __FUNCTIONW__, __LINE__, L"sonar.modules= Found");
					
				map<wstring, wstring>::iterator iter;
				for (iter = m_mapProject.begin(); iter != m_mapProject.end(); iter++)
				{
					fputws(iter->first.c_str(), fp2);
					fputws(L",", fp2);
				}
			}
		}

		::fclose(fp1);
		::fclose(fp2);
		
		DeleteFileW(szFilePath);
		_wrename(sztempFilePath, szFilePath);
		AddData(pPath, pProductName);

	}
	else
	{
		MakePropertiesFile(pPath, pProductName);
	}

	return ERROR_SUCCESS;
}

// properties 파일 뒤에 내용을 추가한다.
int ProjectManager::AddData(WCHAR* pPath, WCHAR* pProductName)
{
	FILE* fp = NULL;
	WCHAR szFilePath[MAX_PATH];

	wcscpy_s(szFilePath, MAX_PATH, pPath);
	wcscat_s(szFilePath, L"\\");
	wcscat_s(szFilePath, pProductName);
	wcscat_s(szFilePath, L"_project.properties");

	errno_t err2 = _wfopen_s(&fp, szFilePath, L"at");

	if (ERROR_SUCCESS != err2 || NULL == fp)
	{
		return ERROR_OPEN_FAILED;
	}
	
	map<wstring, wstring>::iterator itor;
	map<wstring, vector<wstring>>::iterator iter;

	for (itor = m_mapProject.begin(); itor != m_mapProject.end(); itor++)
	{
		fputws(itor->first.c_str(), fp);
		fputws(L".sonar.projectBaseDir=_projectBaseDir", fp);
		fputws(L"\n", fp);


		fputws(itor->first.c_str(), fp);
		fputws(L".sonar.sources=.", fp);
		fputws(L"\n", fp);

		fputws(itor->first.c_str(), fp);
		fputws(L".sonar.exclusions=**//*.*", fp);
		fputws(L"\n", fp);

		fputws(itor->first.c_str(), fp);
		fputws(L".sonar.inclusions=", fp);

		// .h, .cpp, .c 파일 포함
		iter = m_mapSolFiles.find(itor->first.c_str());
		if (iter != m_mapSolFiles.end()) {
			for (int i = 0; i < iter->second.size(); i++)
			{
				fputws(iter->second[i].c_str(), fp);
				fputws(L",", fp);
			}
		}

		fputws(L"\n", fp);
	}

	::fclose(fp);

	return ERROR_SUCCESS;
}

int ProjectManager::GetModuleName(WCHAR* pLine,WCHAR* pModule)
{
	 DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"GetModuleName");
	if (NULL == pLine || NULL == pModule)
		return ERROR_INVALID_PARAMETER;
	
	WCHAR* pModuleFind = wcsstr(pLine, L"\", \"");
	if (NULL != pModuleFind)
		*pModuleFind = 0x00;

	pModuleFind = wcsstr(pLine, L"= \"");
	if (NULL != pModuleFind)
			wcscpy_s(pModule, MAX_PATH, pModuleFind + 3);
	
	return ERROR_SUCCESS;
}

int ProjectManager::GetPathName(WCHAR* pLine, WCHAR* pProjectPath)
{
	 DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"GetPathName");
	
	if (NULL == pLine || NULL == pProjectPath)
		return ERROR_INVALID_PARAMETER;

	WCHAR* pPathFind = wcsstr(pLine, L"\", \"{");
	if (NULL != pPathFind)
		*pPathFind = 0x00;

	pPathFind = wcsstr(pLine, L"\", \"");
	if (NULL != pPathFind)
		wcscpy_s(pProjectPath, MAX_PATH, pPathFind + 4);
	
	ChangeFullPath(pProjectPath, szSolutionPath);

	return ERROR_SUCCESS;
}

int ProjectManager::ChangeFullPath(WCHAR* pProjectPath, WCHAR* pSolutionPath)
{
	int nCnt = 1;
	WCHAR* pPath = pProjectPath;

	while (1)
	{
		WCHAR* pFind = wcsstr(pPath, L"..\\");
		if (NULL != pFind)
		{
			pPath = pFind + 3;
			nCnt++;
		}
		else
			break;
	}
	
	WCHAR szPath[MAX_PATH] = { 0, };
	wcscpy_s(szPath, pSolutionPath);

	for (int nIndex = 0; nIndex < nCnt; nIndex++)
	{
		WCHAR* pFind = wcsrchr(szPath, L'\\');
		if (NULL != pFind)
			*pFind = 0x00;
	}

	wcscat_s(szPath, L"\\");
	wcscat_s(szPath, pPath);
	wcscpy_s(pProjectPath, MAX_PATH, szPath);
	
	// DebugMsg(L" EJH %s(%d) : Debug Msg [%s][%s]", __FUNCTIONW__, __LINE__, L"GetFullPath", pProjectPath);

	return ERROR_SUCCESS;
	
}

int ProjectManager::LoadXmlFile(WCHAR* pFilePath)
{
	if (NULL == pFilePath)
		return ERROR_INVALID_PARAMETER;

	int nReturnValue = ERROR_SUCCESS;
	char* pBuffer = NULL;
	unsigned int nBufferSize = 0;

	XMLError xmlError = XML_SUCCESS;
	FILE* fp = NULL;
	errno_t err;

	err = _wfopen_s(&fp, pFilePath, L"rb");

	if (ERROR_SUCCESS != err || NULL == fp)
	{
		if (0 == _waccess_s(pFilePath, 0))
		{
			nReturnValue = -1;
		}

		nReturnValue = -2;
		goto GO_RETURN;
	}

	::fseek(fp, 0x00, SEEK_END);
	nBufferSize = ftell(fp) + 1;
	::fseek(fp, 0x00, SEEK_SET);

	try {
		pBuffer = new char[nBufferSize];
	}
	catch (const std::bad_alloc&)
	{
		::fclose(fp);

		nReturnValue = -3;
		goto GO_RETURN;
	}

	SecureZeroMemory(pBuffer, nBufferSize);

	::fread(pBuffer, nBufferSize - 1, 1, fp);
	::fclose(fp);

	m_XMLDoc.Clear();
	m_pDataElement = NULL;
	xmlError = m_XMLDoc.Parse(pBuffer, nBufferSize);

	if (XML_SUCCESS != xmlError)
	{
		nReturnValue = -4;
		goto GO_RETURN;
	}

GO_RETURN:

	if (NULL != pBuffer)
	{
		delete[] pBuffer;
	}

	return nReturnValue;
}

int ProjectManager::SaveXmlFile(WCHAR* pFilePath)
{
	if (NULL == pFilePath)
		return ERROR_INVALID_PARAMETER;

	DeleteFile(pFilePath);

	char	szAnsiBuffer[1024] = { 0, };
	::WideCharToMultiByte(CP_ACP, 0, pFilePath, -1, szAnsiBuffer, 1024, NULL, NULL);

	XMLError xmlError = m_XMLDoc.SaveFile(szAnsiBuffer);

	if (XML_SUCCESS != xmlError)
	{
		return -1;
	}

	return ERROR_SUCCESS;
}
