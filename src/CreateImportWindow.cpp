#pragma once
#include "WwiseConnectionHandler.h"
#include "resource.h"
#include "PluginWindow.h"
#include "SimonsReaperPlugin.h"
#include "ReaperRenderQueParser.h"
#include <filesystem>
#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>
#include <future>

///Handles to UI elements
HWND tr_buttonConnect;
HWND tr_textConnectionStatus;
HWND tr_B_GetSelectedParent;
HWND tr_s_ImportParentID;
HWND tr_s_ImportParentNameType;
HWND tr_B_CreateObject;
HWND tr_c_CreateType;
std::string s_CreateType;
HWND tr_c_CreateNameConflict;
std::string s_CreateNameConflict;
HWND tr_txt_CreateName;
HWND tr_txt_CreateNotes;
HWND tr_Tree_RenderJobTree;
HWND tr_Progress_Import;
HWND B_RenderImport;
HWND B_RefreshTree;
HWND txt_status;
HWND check_IsVoice;
HWND txt_Language;
std::string defaultLanguage = "English(US)";
HWND check_OrigDirMatchWwise;
HWND txt_OriginalsSubDir;
HWND l_eventOptions;

CreateObjectChoices myCreateChoices;

HWND CreateImportWindow::m_hWindow = NULL;
long CreateImportWindow::m_lSaveThis = 0;
WwiseConnectionHandler* CreateImportWindow::parentWwiseConnectionHnd = NULL;

std::vector<RenderQueJob> GlobalListOfRenderQueJobs;
std::vector<std::string> RenderFilesBackup;

bool AllDone = false;
std::mutex mtx;
int numOfRendersDone = 0;

//=============================================================================
int CreateImportWindow::CreateTransferWindow(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	INT_PTR success = DialogBox(hInst, MAKEINTRESOURCE(IDD_Transfer), 0, DialogProcStatic);
	//HWND success = CreateDialog(hInst, MAKEINTRESOURCE(IDD_Transfer), 0, DialogProcStatic);
	//ShowWindow(success, SW_SHOW);

	if (success == -1)
	{
		ErrMsg(_T("DialogBox failed."));
	}
	return 0;
}

void CreateImportWindow::SetupPluginParent(WwiseConnectionHandler * parent)
{
	parentWwiseConnectionHnd = parent;
	thisPluginWindow = this;
}

INT_PTR CALLBACK CreateImportWindow::DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//PluginWindow* pThis = (PluginWindow*)GetProp(hwnd, "my_class_data");
	//return pThis ? pThis->realDlgProc(hwnd, uMsg, wParam, lParam) : FALSE;
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		OnCommand(hwnd, LOWORD(wParam), HIWORD(wParam),
			reinterpret_cast<HWND>(lParam));
		return 0;
	}
	case WM_INITDIALOG:
	{
		return OnInitDlg(hwnd, lParam);
	}
	default:
		return FALSE;  //let system deal with msg
	}
}

INT_PTR CreateImportWindow::DialogProcStatic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_hWindow = hDlg;

	CreateImportWindow *pThis = (CreateImportWindow*)m_lSaveThis;

	return (pThis->DlgProc(hDlg, message, wParam, lParam));
}


CreateImportWindow::CreateImportWindow()
{
	m_lSaveThis = (long)this;
	saveThis = this;
}

CreateImportWindow::~CreateImportWindow()
{
}


//=============================================================================
void CreateImportWindow::OnCommand(const HWND hwnd, int id, int notifycode, const HWND hCntrl)
{
	//handles WM_COMMAND message of the modal dialogbox
	switch (id)
	{
	case ID_B_GO:        //RETURN key pressed or 'GO' button selected				 //MessageBox(NULL, "Ok buttor", _T("DEBUG"), MB_OK | MB_ICONEXCLAMATION);
		break;
	case IDC_B_ConnectWwise:
		handleUI_B_Connect();
		break;
	case IDC_B_CreateObject:
		handleUI_B_CreateObject();
		break;
	case IDC_B_GetSelectedParent:
		handleUI_B_GetSelectedParent();
		break;
	case IDC_C_Create_Type:
		handleUI_GetType(notifycode);
		break;
	case IDC_C_CreateOnNameConflict:
		handleUI_GetNameConflict(notifycode);
		break;
	case IDC_B_RenderImport:
		handleUI_RenderImport();
		break;
	case ID_B_CANCEL:    //ESC key pressed or 'cancel' button selected
		m_hWindow = NULL;
		EndDialog(hwnd, id);
		break;
	case ID_B_OK:
		//m_hWindow = NULL;
		//EndDialog(hwnd, id);
		break;
	case IDC_B_RefreshTree:
		FillRenderQueList(hwnd);
		break;
	case IDC_OrigsMatchWwise:
		GetOrigsDirMatchesWwise();
		break;
	case IDC_IsVoice:
		GetIsVoice();
		break;
	}
}
//=============================================================================
INT_PTR CreateImportWindow::OnInitDlg(const HWND hwnd, LPARAM lParam)
{
	//set the small icon for the dialog. IDI_APPLICATION icon is set by default 
	//for winxp
	SendMessage(hwnd, WM_SETICON, ICON_SMALL,
		reinterpret_cast<LPARAM>(LoadImage(0, IDI_APPLICATION, IMAGE_ICON,
			0, 0, LR_SHARED)));
	//ensure focus rectangle is properly draw around control with focus
	PostMessage(hwnd, WM_KEYDOWN, VK_TAB, 0);

	//Init options
	init_ALL_OPTIONS(hwnd);

	//init Wwise Connection
	handleUI_B_Connect();

	return TRUE;
}
//=============================================================================
inline int CreateImportWindow::ErrMsg(const std::string& s)
{
	return MessageBox(0, s.c_str(), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
}

//////////////////////////////////
////	Handle UI notifications 


void CreateImportWindow::handleUI_B_Connect()
{
	if (parentWwiseConnectionHnd->handle_GUI_Connect())
	{
		SetStatusMessageText("Ready");
		SetDlgItemText(m_hWindow, IDC_WwiseConnection, "Wwise Connection Established");
		//		SendMessage(hwnd_combo, CB_SETCURSEL, 0, 0);
		//		textConnectionStatus
	}
	else
	{
		SetStatusMessageText("Error");
		SetDlgItemText(m_hWindow, IDC_WwiseConnection, "!!Wwise Connection Missing!!");
	}
}

void CreateImportWindow::handleUI_B_CreateObject()
{

	/// Get selected wwise object first
	WwiseObject selectedParent = parentWwiseConnectionHnd->GetSelectedObject();

	PrintToConsole("Creating New Wwise Object");
	CreateObjectArgs myCreateObjectArgs;

	handleUI_GetType(1);
	handleUI_GetNameConflict(1);

	if (selectedParent.properties.empty())
	{
		PrintToConsole("No Wwise object selected..");
		return;
	}

	myCreateObjectArgs.ParentID = selectedParent.properties["id"];

	///Get the par text fields
	char buffer[256];

	GetDlgItemTextA(m_hWindow, IDC_text_CreateName, buffer, 256);
	std::string s_name = buffer;
	myCreateObjectArgs.Name = s_name;

	GetDlgItemTextA(m_hWindow, IDC_Text_CreateNotes, buffer, 256);
	std::string s_notes = buffer;
	myCreateObjectArgs.Notes = s_notes;

	myCreateObjectArgs.Type = s_CreateType;
	myCreateObjectArgs.onNameConflict = s_CreateNameConflict;




	AK::WwiseAuthoringAPI::AkJson::Array results;
	if (!parentWwiseConnectionHnd->CreateWwiseObjects(false, myCreateObjectArgs, results))
	{
		//ERROR
		SetStatusMessageText("Error");
		return;
	}
	SetStatusMessageText("Ready");

	//waapi_CreateObjectFromArgs(myCreateObjectArgs, results);
}

void CreateImportWindow::handleUI_B_GetSelectedParent()
{
	WwiseObject selectedParent = parentWwiseConnectionHnd->GetSelectedObject();
	std::string parID = selectedParent.properties["id"];
	std::string parNameType = selectedParent.properties["name"] + " (" + selectedParent.properties["type"] + ")";
	SetDlgItemText(m_hWindow, IDC_ImportParent_ID, parID.c_str());
	SetDlgItemText(m_hWindow, IDC_ImportParent_NameType, parNameType.c_str());

	HandleUI_SetParentForRenderJob(selectedParent);

}

void CreateImportWindow::handleUI_GetType(int notifCode)
{
	int x = 0;
	switch (notifCode)
	{
	case CBN_SELCHANGE:
		x = SendMessage(tr_c_CreateType, CB_GETCURSEL, 0, 0);
		s_CreateType = myCreateChoices.waapiCREATEchoices_TYPE[x];
		break;
	default:
		break;
	}
}

void CreateImportWindow::handleUI_GetNameConflict(int notifCode)
{
	int x = 0;
	switch (notifCode)
	{
	case CBN_SELCHANGE:
		x = SendMessage(tr_c_CreateNameConflict, CB_GETCURSEL, 0, 0);
		s_CreateNameConflict = myCreateChoices.waapiCREATEchoices_NAMECONFLICT[x];
		break;
	default:
		break;
	}
}

bool CreateImportWindow::UpdateProgressDuringRender(int numJobs)
{
	AllDone = false;
	numOfRendersDone = 0;
	
	while (!AllDone)
	{
		
		if (numOfRendersDone == numJobs) {
			AllDone = true;
			//PrintToConsole("ALL Renders complete");
			return true;
		}

		int jobIndex = 0;
		for (auto job : GlobalListOfRenderQueJobs)
		{
			if (!GlobalListOfRenderQueJobs[jobIndex].hasRendered)
			{
				WIN32_FIND_DATA FindFileData;
				HANDLE hFind = FindFirstFile(job.RenderQueFilePath.c_str(), &FindFileData);

				if (hFind == INVALID_HANDLE_VALUE)
				{
					if (GetLastError() == ERROR_FILE_NOT_FOUND)
					{
						if (!GlobalListOfRenderQueJobs[jobIndex].hasRendered)
						{
							PostMessage(tr_Progress_Import, PBM_STEPIT, 0, 0);
							//SendMessage(tr_Progress_Import, PBM_STEPIT, 0, 0);

							GlobalListOfRenderQueJobs[jobIndex].hasRendered = true;
							numOfRendersDone++;
						}
					}
				}
				jobIndex++;
			}
				
		}

	}
	
	
}



void CreateImportWindow::handleUI_RenderImport()
{
	/// Render and import from que list

	//For each job in the global render que
	//check it has valid wwise parent and report errors/exit if not

	for (auto job : GlobalListOfRenderQueJobs)
	{
		if (job.parentWwiseObject.properties.size() == 0 && job.hasPerFileOverrides==false)
		{
			PrintToConsole("  ERROR! A render job has no import settings!   \n");
			PrintToConsole(job.RenderQueFilePath);
			SetStatusMessageText("Error");
			return;
		}

	}

	SetStatusMessageText("Rendering from Reaper Render Que");

	int numJobs = GlobalListOfRenderQueJobs.size();

	//backup render que files in case of errors
	backupRenderQueFiles();

	bool startedRender = false;

	SendMessage(tr_Progress_Import, PBM_SETRANGE, 0, MAKELPARAM(0, numJobs));

	SendMessage(tr_Progress_Import, PBM_SETSTEP, (WPARAM)1, 0);

	std::future<bool> fut = std::async(std::launch::async,&CreateImportWindow::UpdateProgressDuringRender,this,numJobs);

	ReaperRenderObj renderObj;
	renderObj.RenderAllQues();

	PrintToConsole("Render done. Waiting for second thread");
	SendMessage(tr_Progress_Import, PBM_SETPOS, numJobs, 0);

	std::future_status status;

	status = fut.wait_for(std::chrono::seconds(4));
	//bool ret = fut.get();

	PrintToConsole("Rejoined main");

	MSG msg;	//Clears the message que for the progress bar
	PeekMessage(&msg,tr_Progress_Import,0,0,PM_REMOVE);
	
	if (status == std::future_status::ready)
	{
		SetStatusMessageText("Importing into Wwise");
		SendMessage(tr_Progress_Import, PBM_SETPOS, 0, 0);
		
		if (!ImportJobsIntoWwise())
		{
			// something went wrong, restore render que files
			PrintToConsole("Something went wrong, restoring Reaper Render Que files.....");
			restoreRenderQueFiles();
			return;

		}

		FillRenderQueList(m_hWindow);
		return;
	}
	else if (status == std::future_status::timeout)
	{
		PrintToConsole("Timeout error. Something went wrong in Render. Reaper did not remove all render que files after processing.");
		SetStatusMessageText("Error");
		restoreRenderQueFiles();
		return;
	}
	

}



bool CreateImportWindow::ImportJobsIntoWwise()
{
	SetStatusMessageText("Importing into Wwise");

	int jobIndex = 0;
	int importSuccesses = 0;
	for (auto &job : GlobalListOfRenderQueJobs)
	{
		if (job.hasRendered)
		{
			///deal with file overrides here

			if (job.hasPerFileOverrides)
			{
				int fileOverrideIndex = 0;
				for (auto &fileOverride : job.perFileOverridesmap)
				{

					std::string file = fileOverride.second.RenderJobFile;	// file is just the filename here

					// the RenderQueJob filelist is the whole path to the file, need to get this from the render job and use it for the import step

					for (auto &renderJobFilePath : job.RenderQueJobFileList)
					{
						std::size_t found = renderJobFilePath.rfind(file);
						if (found != renderJobFilePath.npos)
						{
							// we found a render file path matching our override file
							fileOverride.second.RenderJobFile = renderJobFilePath;

							//remove this overridden file from the main job render file list
							renderJobFilePath = "";

						}
					}

					std::string existingOriginalsPath = "";
					std::string existingWwisePath = "";
					if (AudioFileExistsInWwise(file,fileOverride.second.parentWwiseObject, existingOriginalsPath, existingWwisePath))
					{
						//audio file already exists under this parent, so replace the originals path
						if (fileOverride.second.ImportLanguage != "SFX")
						{
							size_t pos = existingOriginalsPath.find(fileOverride.second.ImportLanguage);
							existingOriginalsPath.erase(pos, fileOverride.second.ImportLanguage.length() + 1);
							
						}
						fileOverride.second.OrigDirMatchesWwise = false;
						fileOverride.second.userOrigsSubDir = existingOriginalsPath;
					}



					std::vector<std::string> audiofile;
					audiofile.push_back(fileOverride.second.RenderJobFile);

					//deal with importing the overriden files
					ImportObjectArgs curFileOverrideImportArgs = SetupImportArgs
					(
						fileOverride.second.parentWwiseObject,
						fileOverride.second.isVoice,
						fileOverride.second.ImportLanguage,
						fileOverride.second.OrigDirMatchesWwise,
						fileOverride.second.userOrigsSubDir,
						audiofile
						
					);
					if (ImportCurrentRenderJob(curFileOverrideImportArgs))
					{
						fileOverride.second.hasImported = true;
					}

				}

			}

			// import the remaining files from the job that are not overrideen
			ImportObjectArgs curJobImportArgs = SetupImportArgs
			(
				job.parentWwiseObject,
				job.isVoice,
				job.ImportLanguage,
				job.OrigDirMatchesWwise,
				job.userOrigsSubDir,
				job.RenderQueJobFileList
			);
			if (curJobImportArgs.ImportFileList.empty())
			{
				int numOfOverrides = job.perFileOverridesmap.size();
				int numOfImports = 0;
				// job list for importing was empty. Check if all overrides completed
				for (auto overrideJob : job.perFileOverridesmap)
				{
					if (overrideJob.second.hasImported)
					{
						numOfImports += 1;
					}
				}
				if (numOfImports == numOfOverrides)
				{
					GlobalListOfRenderQueJobs[jobIndex].hasImported = true;
					importSuccesses++;
					SendMessage(tr_Progress_Import, PBM_SETPOS, importSuccesses, 0);
				}
			}
			else if (ImportCurrentRenderJob(curJobImportArgs))
			{
				GlobalListOfRenderQueJobs[jobIndex].hasImported = true;
				importSuccesses++;
				SendMessage(tr_Progress_Import, PBM_SETPOS, importSuccesses, 0);
			}
			
		}
		jobIndex++;
	}

	if (importSuccesses == GlobalListOfRenderQueJobs.size())
	{
		PrintToConsole("All jobs imported successfully");
		SetStatusMessageText("All Import jobs complete");
		return true;
	}
	else
	{
		PrintToConsole("Error! At least one import operation failed...");
		SetStatusMessageText("Error");
		return false;
	}



	
}

ImportObjectArgs CreateImportWindow::SetupImportArgs(WwiseObject parent, bool isVoice, std::string ImportLanguage, bool OrigsDirMatchesWwise, std::string userOrigSubDir,std::vector<std::string> ImportFiles)
{
	std::string originalsPath = parent.properties["path"];
	std::string remove = "\\Actor-Mixer Hierarchy";
	originalsPath.erase(0, remove.length());

	ImportObjectArgs importArgs;
	importArgs.ImportLocation = parent.properties["path"];
	importArgs.ImportLanguage = ImportLanguage;
	if (isVoice)
	{
		importArgs.objectType = "<Sound Voice>";
	}
	else
	{
		importArgs.objectType = "<Sound SFX>";
	}
	if (OrigsDirMatchesWwise)
	{
		importArgs.OriginalsSubFolder = originalsPath;
	}
	else
	{
		importArgs.OriginalsSubFolder = userOrigSubDir;//"\\REAPER";
	}
	for (auto file : ImportFiles)
	{
		if (file == "")
		{
			continue;
		}
		std::string audiofile = file.substr(file.find_last_of("/\\")+1);
		std::string rawAudioFile = audiofile.substr(0, audiofile.find_last_of("."));
		std::string objectPath = parent.properties["path"] +"\\"+ importArgs.objectType + rawAudioFile;
		std::pair<std::string, std::string> imports;
		imports = std::make_pair(file, objectPath);
		importArgs.ImportFileList.push_back(imports);
	}

	return importArgs;
}

bool CreateImportWindow::ImportCurrentRenderJob(ImportObjectArgs curJobImportArgs)
{
	AK::WwiseAuthoringAPI::AkJson::Array results;
	return parentWwiseConnectionHnd->ImportAudioToWwise(false, curJobImportArgs, results);
}

bool CreateImportWindow::AudioFileExistsInWwise(std::string audioFile, WwiseObject parent, std::string& existingOriginalDir, std::string& existingWwisePath)
{
	ObjectGetArgs getArgs;
	std::string id = parent.properties["id"];
	getArgs.From = { "id",id };
	getArgs.Select = "descendants";
	getArgs.Where = { "type:isIn","Sound" };
	getArgs.customReturnArgs.push_back("sound:originalWavFilePath"); 
	getArgs.customReturnArgs.push_back("path");

	AK::WwiseAuthoringAPI::AkJson::Array results;
	std::vector<WwiseObject> MyWwiseObjects;
	try {
		MyWwiseObjects = parentWwiseConnectionHnd->GetWwiseObjects(false, getArgs, results);
	}
	catch (std::string e) {
		PrintToConsole(e);
	}

	for (const auto obj : MyWwiseObjects) {
		//PrintToConsole("");
		//PrintToConsole(obj.properties.at("name"));
		std::string name = obj.properties.at("name")+".wav";
		if (name == audioFile)
		{
			std::string fullPath = obj.properties.at("sound:originalWavFilePath");

			fullPath.erase(0, fullPath.find("Originals\\"));

			if (fullPath.find("Originals\\Voices\\") != fullPath.npos)
			{
				fullPath.erase(0, 17);
			}
			if (fullPath.find("Originals\\SFX\\") != fullPath.npos)
			{
				fullPath.erase(0, 14);
			}

			size_t pos = fullPath.find(audioFile);
			fullPath.erase(pos, audioFile.length());

			existingOriginalDir = fullPath;

			
			return true;
		}
	}

	return false;
}

void CreateImportWindow::backupRenderQueFiles()
{
	std::string resourcePath = GetReaperResourcePath();
	std::filesystem::path backupPath = resourcePath + "\\QueuedRenders\\_backup";

	for (auto RenderJob : GlobalListOfRenderQueJobs)
	{
		RenderFilesBackup.push_back(RenderJob.RenderQueFilePath);
		std::filesystem::path source = RenderJob.RenderQueFilePath;
		std::filesystem::path target = backupPath / source.filename();

		try // If you want to avoid exception handling, then use the error code overload of the following functions.
		{
			std::filesystem::create_directories(backupPath); // Recursively create target directory if not existing.
			std::filesystem::copy_file(source, target, std::filesystem::copy_options::overwrite_existing);
		}
		catch (std::exception& e) // Not using fs::filesystem_error since std::bad_alloc can throw too.  
		{
			std::cout << e.what();
		}
	}

}

void CreateImportWindow::restoreRenderQueFiles()
{
	std::string resourcePath = GetReaperResourcePath();
	std::filesystem::path backupPath = resourcePath + "\\QueuedRenders\\_backup";
	std::filesystem::path restorePath = resourcePath + "\\QueuedRenders";

	for (auto RenderQueFile : RenderFilesBackup)
	{
		std::filesystem::path source = RenderQueFile;
		std::filesystem::path sourceFile = source.filename();
		source = backupPath / sourceFile;
		std::filesystem::path target = restorePath / sourceFile;

		try // If you want to avoid exception handling, then use the error code overload of the following functions.
		{
			std::filesystem::copy_file(source, target, std::filesystem::copy_options::overwrite_existing);
			remove(source);
		}
		catch (std::exception& e) // Not using fs::filesystem_error since std::bad_alloc can throw too.  
		{
			std::cout << e.what();
		}
	}
}



/// INIT ALL OPTIONS

bool CreateImportWindow::init_ALL_OPTIONS(HWND hwnd)
{
	tr_buttonConnect = GetDlgItem(hwnd, IDC_B_ConnectWwise);
	tr_textConnectionStatus = GetDlgItem(hwnd, IDC_WwiseConnection);
	tr_B_GetSelectedParent = GetDlgItem(hwnd, IDC_B_GetSelectedParent);
	tr_s_ImportParentID = GetDlgItem(hwnd, IDC_ImportParent_ID);
	tr_s_ImportParentNameType = GetDlgItem(hwnd, IDC_ImportParent_NameType);
	tr_B_CreateObject = GetDlgItem(hwnd, IDC_B_CreateObject);
	tr_c_CreateType = GetDlgItem(hwnd, IDC_C_Create_Type);
	tr_c_CreateNameConflict = GetDlgItem(hwnd, IDC_C_CreateOnNameConflict);
	tr_txt_CreateName = GetDlgItem(hwnd, IDC_text_CreateName);
	tr_txt_CreateNotes = GetDlgItem(hwnd, IDC_Text_CreateNotes);
	tr_Tree_RenderJobTree = GetDlgItem(hwnd, IDC_TREE_RenderJobTree);

	UINT mask = TVS_EX_MULTISELECT;
	DWORD dWord = TVS_EX_MULTISELECT;
	TreeView_SetExtendedStyle(tr_Tree_RenderJobTree, dWord, mask);

	tr_Progress_Import = GetDlgItem(hwnd, IDC_PROGRESS_Import);
	B_RenderImport = GetDlgItem(hwnd, IDC_B_RenderImport);
	txt_status = GetDlgItem(hwnd, IDC_Txt_Status);
	txt_Language = GetDlgItem(hwnd, IDC_Language);
	Edit_SetText(txt_Language, defaultLanguage.c_str());
	check_IsVoice = GetDlgItem(hwnd, IDC_IsVoice);
	SendDlgItemMessage(m_hWindow, IDC_IsVoice, BM_SETCHECK, BST_UNCHECKED, 0);
	check_OrigDirMatchWwise = GetDlgItem(hwnd, IDC_OrigsMatchWwise);
	SendDlgItemMessage(m_hWindow, IDC_OrigsMatchWwise, BM_SETCHECK, BST_CHECKED, 0);
	B_RefreshTree = GetDlgItem(hwnd, IDC_B_RefreshTree);
	txt_OriginalsSubDir = GetDlgItem(hwnd, IDC_txt_OrigsDir);
	Edit_SetText(txt_OriginalsSubDir, "ImportedFromReaper/");
	l_eventOptions = GetDlgItem(hwnd, IDC_LIST_EventOptions);

	init_ComboBox_A(tr_c_CreateType, myCreateChoices.waapiCREATEchoices_TYPE);
	init_ComboBox_A(tr_c_CreateNameConflict, myCreateChoices.waapiCREATEchoices_NAMECONFLICT);
	init_ComboBox_A(l_eventOptions, myCreateChoices.waapiCREATEchoices_EVENTOPTIONS);

	GetOrigsDirMatchesWwise();
	GetIsVoice();

	FillRenderQueList(hwnd);

	SetStatusMessageText("Ready");

	return true;
}

///Initialise dialogue boxes
bool CreateImportWindow::init_ComboBox_A(HWND hwnd_combo, std::vector<std::string> choices)
{
	int i = 0;
	for (auto choice : choices)
	{
		SendMessage(hwnd_combo, CB_ADDSTRING, i, (LPARAM)choice.c_str());
		SendMessage(hwnd_combo, CB_SETCURSEL, 0, 0);
		i++;
	}
	return true;
}

bool CreateImportWindow::init_ListBox_A(HWND hwnd_list, std::vector<std::string> choices)
{
	int i = 0;
	for (auto choice : choices)
	{
		SendMessage(hwnd_list, LB_ADDSTRING, i, (LPARAM)choice.c_str());
		i++;
	}
	return true;
}


void CreateImportWindow::FillRenderQueList(HWND hwnd)	
{
	GlobalListOfRenderQueJobs.clear();
	std::vector<std::string> ListOfRenderQueFiles;
	ListOfRenderQueFiles = GetListOfRenderQues();

	for (auto RenderQueFile : ListOfRenderQueFiles)
	{
		RenderQueJob MyrenderQueJob = CreateRenderQueJobFromRenderQueFile(RenderQueFile);
		if (!MyrenderQueJob.RenderQueJobFileList.empty())
		{
			GlobalListOfRenderQueJobs.push_back(MyrenderQueJob);
		}
	}

	UpdateRenderJob_TreeView(hwnd);

}

void CreateImportWindow::UpdateRenderJob_TreeView(HWND hwnd)
{
	TreeView_DeleteAllItems(tr_Tree_RenderJobTree);

	for (auto RenderJob : GlobalListOfRenderQueJobs)
	{
		std::filesystem::path filePath = RenderJob.RenderQueFilePath;
		std::string filename = filePath.filename().string();

		TV_INSERTSTRUCT tvInsert;
		HTREEITEM Parent;
		HTREEITEM Child;

		tvInsert.hParent = NULL;
		tvInsert.hInsertAfter = TVI_ROOT;
		tvInsert.item.mask = TVIF_TEXT;	// tvinsert.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		tvInsert.item.pszText = &filename[0];	//(LPARAM)choice.c_str()
		Parent = (HTREEITEM)SendDlgItemMessage(m_hWindow, IDC_TREE_RenderJobTree, TVM_INSERTITEM, 0, (LPARAM)&tvInsert);

		//Children = WAV files in this render que job
		for (auto renderFile : RenderJob.RenderQueJobFileList)
		{
			std::filesystem::path filePath = renderFile;
			std::string filename = filePath.filename().string();
			tvInsert.hParent = Parent;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.item.pszText = &filename[0];
			Child = (HTREEITEM)SendDlgItemMessage(m_hWindow, IDC_TREE_RenderJobTree, TVM_INSERTITEM, 0, (LPARAM)&tvInsert);
		}


	}

}


void CreateImportWindow::HandleUI_SetParentForRenderJob(WwiseObject selectedParent)
{
	std::string parentWwiseID = selectedParent.properties["id"];
	std::string parentWwiseName = selectedParent.properties["name"];
	std::string parentWwiseType = selectedParent.properties["type"];
	std::string parentWwisePath = selectedParent.properties["path"];
	TVITEM item;
	TVITEM parentItem;

	if (parentWwiseID == "")
	{
		PrintToConsole("Error - No valid Wwise Parent");
		return;	// Invalid wwise parent selection!
	}

	if (parentWwisePath.find("Actor-Mixer Hierarchy\\") == parentWwisePath.npos)
	{
		PrintToConsole("Import parent must be in Actor-Mixer hierarchy");
		return;	// Invalid wwise parent selection!
	}

	// if type is Work Unit - Need to check if it's a folder

	int selectedcount = TreeView_GetSelectedCount(tr_Tree_RenderJobTree);

	HTREEITEM hSelectedItem = TreeView_GetSelection(tr_Tree_RenderJobTree);
	if (hSelectedItem == NULL) // Nothing selected
	{
		return;
	}
	std::vector<HTREEITEM> selectedItems;
	selectedItems.push_back(hSelectedItem);

	if (selectedcount > 1)
	{	
		for (int i = 1; i < selectedcount; i++)
		{
			HTREEITEM nextSelected = TreeView_GetNextSelected(tr_Tree_RenderJobTree, hSelectedItem);
			selectedItems.push_back(nextSelected);
			hSelectedItem = nextSelected;
		}
	}

	for (auto &selectedItem : selectedItems)
	{

		TCHAR buffer[256];
		item.hItem = selectedItem;
		item.mask = TVIF_TEXT | TVIF_CHILDREN;
		item.cchTextMax = 256;
		item.pszText = buffer;
		bool isItemWav = false;
		if (TreeView_GetItem(tr_Tree_RenderJobTree, &item))
		{
			/// need to clean the nammes IF they contain % (they have been double set)

			std::string curName = item.pszText;
			size_t pos = curName.find("%");
			if (pos != curName.npos)
			{
				curName.erase(curName.begin(), curName.begin() + pos+1);
				std::string newItemName = curName;
				//item.mask = TVIF_TEXT;
				item.pszText = &newItemName[0];
				TreeView_SetItem(tr_Tree_RenderJobTree, &item);
			}
			
			TreeView_GetItem(tr_Tree_RenderJobTree, &item);

			if (item.cChildren != 1)
			{
				// Check if selected thing is wav?
				// Support overriding Wwise imports for child wavs of a render job?
				std::string itemName = item.pszText;
				if (itemName.find(".wav") != itemName.npos)
				{
					//Selected thing is a wav file! This render job has overrides

					isItemWav = true;


				}
				else {
					PrintToConsole("Render Job selected has no children");
					return;

				}

			}
			//Find the matching Render Que Job that we selected OR find the parent job of the selected wav
			if (isItemWav) {

				HTREEITEM hparentItem = TreeView_GetParent(tr_Tree_RenderJobTree, item.hItem);
				if (hparentItem == NULL) // Nothing selected
				{
					return;
				}
				TCHAR buffer[256];
				parentItem.hItem = hparentItem;
				parentItem.mask = TVIF_TEXT;
				parentItem.cchTextMax = 256;
				parentItem.pszText = buffer;
				if (TreeView_GetItem(tr_Tree_RenderJobTree, &parentItem))
				{
					std::string itemName = parentItem.pszText;
				}


			}
			std::string jobName = "";
			if (isItemWav) {
				jobName = parentItem.pszText;
			}
			else {
				jobName = item.pszText;
			}
			int count = 0;
			for (auto &renderJob : GlobalListOfRenderQueJobs)
			{
				std::filesystem::path filePath = renderJob.RenderQueFilePath;
				std::string filename = filePath.filename().string();

				if (jobName.find(filename) != jobName.npos)
				{
					//Found a match
					//PrintToConsole("Found a match");

					if (isItemWav) {
						renderJob.hasPerFileOverrides = true;
						RenderJobFileOverride fileOverride;
						fileOverride.RenderJobFile = item.pszText;
						fileOverride.parentWwiseObject = selectedParent;
						fileOverride.isVoice = GetIsVoice();
						fileOverride.OrigDirMatchesWwise = GetOrigsDirMatchesWwise();
						if (!fileOverride.OrigDirMatchesWwise)
						{
							fileOverride.userOrigsSubDir = GetUserOriginalsSubDir();
						}

						std::string language;
						if (fileOverride.isVoice)
						{
							fileOverride.ImportLanguage = GetLanguage();
							language = fileOverride.ImportLanguage;
						}
						else
						{
							fileOverride.ImportLanguage = "SFX";
							language = "SFX";
						}

						renderJob.perFileOverridesmap[fileOverride.RenderJobFile] = fileOverride;


						//Set the display Text to include wwise parent name and type
						std::string newItemName = parentWwiseName + "(" + parentWwiseType + ")  - " + "%" + fileOverride.RenderJobFile;
						item.mask = TVIF_TEXT;
						item.pszText = &newItemName[0];
						TreeView_SetItem(tr_Tree_RenderJobTree, &item);
						PrintToConsole(fileOverride.RenderJobFile + " Imports into " + renderJob.parentWwiseObject.properties["name"]);

					}
					else {

						renderJob.parentWwiseObject = selectedParent;

						renderJob.isVoice = GetIsVoice();

						renderJob.OrigDirMatchesWwise = GetOrigsDirMatchesWwise();

						if (!renderJob.OrigDirMatchesWwise)
						{
							renderJob.userOrigsSubDir = GetUserOriginalsSubDir();
						}

						std::string language;
						if (renderJob.isVoice)
						{
							renderJob.ImportLanguage = GetLanguage();
							language = renderJob.ImportLanguage;
						}
						else
						{
							renderJob.ImportLanguage = "SFX";
							language = "SFX";
						}
						//Set the display Text to include wwise parent name and type
						std::string newItemName = parentWwiseName + "(" + parentWwiseType + " : " + ")  - " + "%" + filename;
						item.mask = TVIF_TEXT;
						item.pszText = &newItemName[0];
						TreeView_SetItem(tr_Tree_RenderJobTree, &item);
						PrintToConsole(renderJob.RenderQueFilePath + " Imports into " + renderJob.parentWwiseObject.properties["name"]);
					}

				}
				count++;
			}

		}
	}

	SetStatusMessageText("Ready");

	//for (auto renderJob : GlobalListOfRenderQueJobs)
	//{
	//	PrintToConsole(renderJob.RenderQueFilePath + " Imports into " + renderJob.parentWwiseObject.properties["name"]);
	//}


}

bool CreateImportWindow::GetIsVoice()
{

	if (SendDlgItemMessage(m_hWindow, IDC_IsVoice, BM_GETCHECK, 0, 0))
	{
		Edit_Enable(txt_Language, true);
		return true;
	}
	else
	{
		Edit_Enable(txt_Language, false);
		return false;
	}
}

std::string CreateImportWindow::GetLanguage()
{
	///Get the par ID text
	char buffer[256];
	GetDlgItemTextA(m_hWindow, IDC_Language, buffer, 256);
	std::string lang = buffer;
	return lang;
}

bool CreateImportWindow::GetOrigsDirMatchesWwise()
{
	if (SendDlgItemMessage(m_hWindow, IDC_OrigsMatchWwise, BM_GETCHECK, 0, 0))
	{
		//IDC_txt_OrigsDir
		Edit_Enable(txt_OriginalsSubDir, false);
		return true;
	}
	else
	{
		Edit_Enable(txt_OriginalsSubDir, true);
		return false;
	}
}

std::string CreateImportWindow::GetUserOriginalsSubDir()
{
	char buffer[256];

	GetDlgItemTextA(m_hWindow, IDC_txt_OrigsDir, buffer, 256);
	std::string userOrigsDir = buffer;
	return userOrigsDir;
}

void CreateImportWindow::SetStatusMessageText(std::string message)
{
	SetDlgItemText(m_hWindow, IDC_Txt_Status, message.c_str());
}




