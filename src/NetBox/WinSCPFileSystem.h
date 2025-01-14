#pragma once

#include <Interface.h>
#include "FarPlugin.h"
#include <FileOperationProgress.h>
#include <Terminal.h>
#include <GUIConfiguration.h>
#include <SynchronizeController.h>
#include <Queue.h>
#include <WinInterface.h>

class TTerminal;
class TSessionData;
class TRemoteFile;
class TBookmarkList;
class TWinSCPPlugin;
class TNetBoxPlugin;
class TFarButton;
class TFarDialogItem;
class TFarDialog;
class TTerminalQueue;
class TTerminalQueueStatus;
class TQueueItem;
class TKeepaliveThread;
struct TMessageParams;
#define REMOTE_DIR_HISTORY L"WinscpRemoteDirectory"
#define ASCII_MASK_HISTORY L"WinscpAsciiMask"
#define LINK_FILENAME_HISTORY L"WinscpRemoteLink"
#define LINK_POINT_TO_HISTORY L"WinscpRemoteLinkPointTo"
#define APPLY_COMMAND_HISTORY L"WinscpApplyCmd"
#define APPLY_COMMAND_PARAM_HISTORY L"WinscpApplyCmdParam"
#define LOG_FILE_HISTORY L"WinscpLogFile"
#define REMOTE_SYNC_HISTORY L"WinscpRemoteSync"
#define LOCAL_SYNC_HISTORY L"WinscpLocalSync"
#define MOVE_TO_HISTORY L"WinscpMoveTo"
#define WINSCP_FILE_MASK_HISTORY L"WinscpFileMask"
#define MAKE_SESSION_FOLDER_HISTORY L"WinscpSessionFolder"

// for Properties dialog
//const int cpMode  = 0x01;
//const int cpOwner = 0x02;
//const int cpGroup = 0x04;
// for Copy dialog
//const int coTempTransfer        = 0x01;
//const int coDisableNewerOnly    = 0x04;
//// for Synchronize and FullSynchronize dialogs
//const int spSelectedOnly = 0x800;
//// for Synchronize dialogs
//const int soAllowSelectedOnly = 0x01;
//// for FullSynchronize dialog
//const int fsoDisableTimestamp = 0x01;
//const int fsoAllowSelectedOnly = 0x02;
enum TSessionActionEnum
{
  saAdd,
  saEdit,
  saConnect
};

struct TMultipleEdit : public TObject
{
  UnicodeString FileName;
  UnicodeString FileTitle;
  UnicodeString Directory;
  UnicodeString LocalFileName;
  bool PendingSave;
};

struct TEditHistory : public TObject
{
  UnicodeString FileName;
  UnicodeString Directory;
  bool operator==(const TEditHistory &rh) const { return (FileName == rh.FileName) && (Directory == rh.Directory); }
};

#if 0
typedef void __fastcall (__closure *TProcessSessionEvent)(TSessionData *Data, void *Param);
#endif // #if 0
typedef nb::FastDelegate2<void, TSessionData * /*Data*/, void * /*Param*/> TProcessSessionEvent;

class TWinSCPFileSystem : public TCustomFarFileSystem
{
  friend class TWinSCPPlugin;
  friend class TNetBoxPlugin;
  friend class TKeepaliveThread;
  friend class TQueueDialog;
  NB_DISABLE_COPY(TWinSCPFileSystem)
public:
  static inline bool classof(const TObject *Obj) { return Obj->is(OBJECT_CLASS_TWinSCPFileSystem); }
  virtual bool is(TObjectClassId Kind) const override { return (Kind == OBJECT_CLASS_TWinSCPFileSystem) || TCustomFarFileSystem::is(Kind); }
public:
  explicit TWinSCPFileSystem(TCustomFarPlugin *APlugin);
  void Init(TSecureShell *SecureShell);
  virtual ~TWinSCPFileSystem();

  virtual void Close() override;

protected:
  bool Connect(TSessionData *Data);
  void Disconnect();
  void SaveSession();

  virtual void GetOpenPanelInfoEx(OPENPANELINFO_FLAGS &Flags,
    UnicodeString &HostFile, UnicodeString &CurDir, UnicodeString &AFormat,
    UnicodeString &PanelTitle, TFarPanelModes *PanelModes, intptr_t &StartPanelMode,
    OPENPANELINFO_SORTMODES &StartSortMode, bool &StartSortOrder, TFarKeyBarTitles *KeyBarTitles,
    UnicodeString &ShortcutData) override;
  virtual bool GetFindDataEx(TObjectList *PanelItems, OPERATION_MODES OpMode) override;
  virtual bool ProcessKeyEx(intptr_t Key, uintptr_t ControlState) override;
  virtual bool SetDirectoryEx(UnicodeString Dir, OPERATION_MODES OpMode) override;
  virtual intptr_t MakeDirectoryEx(UnicodeString &Name, OPERATION_MODES OpMode) override;
  virtual bool DeleteFilesEx(TObjectList *PanelItems, OPERATION_MODES OpMode) override;
  virtual intptr_t GetFilesEx(TObjectList *PanelItems, bool Move,
    UnicodeString &DestPath, OPERATION_MODES OpMode) override;
  virtual intptr_t PutFilesEx(TObjectList *PanelItems, bool Move, OPERATION_MODES OpMode) override;
  virtual bool ProcessPanelEventEx(intptr_t Event, void *Param) override;

  void ProcessEditorEvent(intptr_t Event, void *Param);

  virtual void HandleException(Exception *E, OPERATION_MODES OpMode = 0) override;
  void KeepaliveThreadCallback();

  bool IsSessionList() const;
  bool Connected() const;
  const TWinSCPPlugin *GetWinSCPPlugin() const;
  TWinSCPPlugin *GetWinSCPPlugin();
  void ShowOperationProgress(TFileOperationProgressType &ProgressData,
    bool Force);
  bool SessionDialog(TSessionData *SessionData, TSessionActionEnum &Action);
  void EditConnectSession(TSessionData *Data, bool Edit);
  void EditConnectSession(TSessionData *Data, bool Edit, bool NewData, bool FillInConnect);
  void DuplicateOrRenameSession(TSessionData *Data,
    bool Duplicate);
  void FocusSession(const TSessionData *Data);
  void DeleteSession(TSessionData *Data, void *AParam);
  void ProcessSessions(TObjectList *PanelItems,
    TProcessSessionEvent ProcessSession, void *AParam);
  void ExportSession(TSessionData *Data, void *AParam);
  bool ImportSessions(TObjectList *PanelItems, bool Move, OPERATION_MODES OpMode);
  void FileProperties();
  void CreateLink();
  void TransferFiles(bool Move);
  void RenameFile();
  void ApplyCommand();
  void ShowInformation();
  void InsertTokenOnCommandLine(UnicodeString Token, bool Separate);
  void InsertSessionNameOnCommandLine();
  void InsertFileNameOnCommandLine(bool Full);
  UnicodeString GetFullFilePath(const TRemoteFile *AFile) const;
  void InsertPathOnCommandLine();
  void CopyFullFileNamesToClipboard();
  void FullSynchronize(bool Source);
  void Synchronize();
  void OpenDirectory(bool Add);
  void HomeDirectory();
  void ToggleSynchronizeBrowsing();
  bool IsSynchronizedBrowsing() const;
  bool PropertiesDialog(TStrings *AFileList,
    UnicodeString Directory,
    const TRemoteTokenList *GroupList, const TRemoteTokenList *UserList,
    TRemoteProperties *Properties, intptr_t AllowedChanges);
  bool ExecuteCommand(UnicodeString Command);
  void TerminalCaptureLog(UnicodeString AddedLine, TCaptureOutputType OutputEvent);
  bool CopyDialog(bool ToRemote, bool Move, const TStrings *AFileList,
    intptr_t Options,
    intptr_t CopyParamAttrs,
    UnicodeString &TargetDirectory,
    TGUICopyParamType *Params);
  bool LinkDialog(UnicodeString &AFileName, UnicodeString &PointTo, bool &Symbolic,
    bool Edit, bool AllowSymbolic);
  void FileSystemInfoDialog(const TSessionInfo &SessionInfo,
    const TFileSystemInfo &FileSystemInfo, UnicodeString SpaceAvailablePath,
    TGetSpaceAvailableEvent OnGetSpaceAvailable);
  bool OpenDirectoryDialog(bool Add, UnicodeString &Directory,
    TBookmarkList *BookmarkList);
  bool ApplyCommandDialog(UnicodeString &Command, intptr_t &Params) const;
  bool FullSynchronizeDialog(TTerminal::TSynchronizeMode &Mode,
    intptr_t &Params, UnicodeString &LocalDirectory, UnicodeString &RemoteDirectory,
    TCopyParamType *CopyParams, bool &SaveSettings, bool &SaveMode, intptr_t Options,
    const TUsableCopyParamAttrs &CopyParamAttrs) const;
  bool SynchronizeChecklistDialog(TSynchronizeChecklist *Checklist,
    TTerminal::TSynchronizeMode Mode, intptr_t Params,
    UnicodeString LocalDirectory, UnicodeString RemoteDirectory);
  bool RemoteTransferDialog(TStrings *AFileList, UnicodeString &Target,
    UnicodeString &FileMask, bool Move);
  bool RenameFileDialog(TRemoteFile *AFile, UnicodeString &NewName);
  uintptr_t MoreMessageDialog(UnicodeString Str, TStrings *MoreMessages,
    TQueryType Type, uintptr_t Answers, const TMessageParams *AParams = nullptr);
  bool PasswordDialog(TSessionData *SessionData,
    TPromptKind Kind, UnicodeString Name, UnicodeString Instructions, TStrings *Prompts,
    TStrings *Results, bool StoredCredentialsTried);
  bool BannerDialog(UnicodeString SessionName, UnicodeString Banner,
    bool &NeverShowAgain, intptr_t Options);
  bool CreateDirectoryDialog(UnicodeString &Directory,
    TRemoteProperties *Properties, bool &SaveSettings);
  bool QueueDialog(TTerminalQueueStatus *Status, bool ClosingPlugin);
  bool SynchronizeDialog(TSynchronizeParamType &Params,
    const TCopyParamType *CopyParams, TSynchronizeStartStopEvent OnStartStop,
    bool &SaveSettings, intptr_t Options, intptr_t CopyParamAttrs,
    TGetSynchronizeOptionsEvent OnGetOptions);
  void DoSynchronize(TSynchronizeController *Sender,
    UnicodeString LocalDirectory, UnicodeString RemoteDirectory,
    const TCopyParamType &CopyParam, const TSynchronizeParamType &Params,
    TSynchronizeChecklist **Checklist, TSynchronizeOptions *Options, bool Full);
  void DoSynchronizeInvalid(TSynchronizeController *Sender,
    UnicodeString Directory, UnicodeString ErrorStr);
  void DoSynchronizeTooManyDirectories(TSynchronizeController *Sender,
    intptr_t &MaxDirectories);
  void Synchronize(UnicodeString LocalDirectory,
    UnicodeString RemoteDirectory, TTerminal::TSynchronizeMode Mode,
    const TCopyParamType &CopyParam, intptr_t Params, TSynchronizeChecklist **AChecklist,
    TSynchronizeOptions *Options);
  bool SynchronizeAllowSelectedOnly();
  void GetSynchronizeOptions(intptr_t Params, TSynchronizeOptions &Options);
  void RequireCapability(intptr_t Capability);
  void RequireLocalPanel(TFarPanelInfo *Panel, UnicodeString Message);
  bool AreCachesEmpty() const;
  void ClearCaches();
  void OpenSessionInPutty();
  void QueueShow(bool ClosingPlugin);
  TTerminalQueueStatus *ProcessQueue(bool Hidden);
  bool EnsureCommandSessionFallback(TFSCapability Capability);
  void ConnectTerminal(TTerminal *Terminal);
  void TemporarilyDownloadFiles(TStrings *AFileList,
    TCopyParamType &CopyParam, UnicodeString &TempDir);
  intptr_t UploadFiles(bool Move, OPERATION_MODES OpMode, bool Edit, UnicodeString &DestPath);
  void UploadOnSave(bool NoReload);
  void UploadFromEditor(bool NoReload, UnicodeString AFileName,
    UnicodeString RealFileName, UnicodeString &DestPath);
  void LogAuthentication(TTerminal *Terminal, UnicodeString Msg);
  void MultipleEdit();
  void MultipleEdit(UnicodeString Directory, UnicodeString AFileName, TRemoteFile *AFile);
  void EditViewCopyParam(TCopyParamType &CopyParam);
  bool SynchronizeBrowsing(UnicodeString NewPath);
  bool IsEditHistoryEmpty() const;
  void EditHistory();
  UnicodeString ProgressBar(intptr_t Percentage, intptr_t Width);
  bool IsLogging() const;
  void ShowLog();

  TTerminal *GetTerminal() const { return FTerminal; }
  TSessionData *GetSessionData() const { return FTerminal ? FTerminal->GetSessionData() : nullptr; }
  TSessionData *GetSessionData() { return FTerminal ? FTerminal->GetSessionData() : nullptr; }

protected:
  virtual UnicodeString GetCurrDirectory() const override { return FTerminal ? FTerminal->RemoteGetCurrentDirectory() : UnicodeString(); }

private:
  bool TerminalCheckForEsc();
  void TerminalClose(TObject *Sender);
  void TerminalUpdateStatus(TTerminal *Terminal, bool Active);
  void TerminalChangeDirectory(TObject *Sender);
  void TerminalReadDirectory(TObject *Sender, bool ReloadOnly);
  void TerminalStartReadDirectory(TObject *Sender);
  void TerminalReadDirectoryProgress(TObject *Sender, intptr_t Progress,
    intptr_t ResolvedLinks, bool &Cancel);
  void TerminalInformation(TTerminal *Terminal,
    UnicodeString Str, bool Status, intptr_t Phase);
  void TerminalQueryUser(TObject *Sender,
    UnicodeString AQuery, TStrings *MoreMessages, uintptr_t Answers,
    const TQueryParams *AParams, uintptr_t &Answer, TQueryType Type, void *Arg);
  void TerminalPromptUser(TTerminal *Terminal,
    TPromptKind Kind, UnicodeString Name, UnicodeString Instructions,
    TStrings *Prompts, TStrings *Results, bool &AResult,
    void *Arg);
  void TerminalDisplayBanner(TTerminal *Terminal,
    UnicodeString SessionName, UnicodeString Banner, bool &NeverShowAgain,
    intptr_t Options);
  void TerminalShowExtendedException(TTerminal *Terminal,
    Exception *E, void *Arg);
  void TerminalDeleteLocalFile(UnicodeString AFileName, bool Alternative);
  HANDLE TerminalCreateLocalFile(UnicodeString LocalFileName,
    DWORD DesiredAccess, DWORD ShareMode, DWORD CreationDisposition, DWORD FlagsAndAttributes);
  DWORD TerminalGetLocalFileAttributes(UnicodeString LocalFileName) const;
  bool TerminalSetLocalFileAttributes(UnicodeString LocalFileName, DWORD FileAttributes);
  bool TerminalMoveLocalFile(UnicodeString LocalFileName, UnicodeString NewLocalFileName, DWORD Flags);
  bool TerminalRemoveLocalDirectory(UnicodeString LocalDirName);
  bool TerminalCreateLocalDirectory(UnicodeString LocalDirName, LPSECURITY_ATTRIBUTES SecurityAttributes);
  void OperationProgress(
    TFileOperationProgressType &ProgressData);
  void OperationFinished(TFileOperation Operation,
    TOperationSide Side, bool DragDrop, UnicodeString AFileName, bool Success,
    TOnceDoneOperation &DisconnectWhenComplete);
  void CancelConfiguration(TFileOperationProgressType &ProgressData);
  TStrings *CreateFileList(TObjectList *PanelItems,
    TOperationSide Side, bool SelectedOnly = false, UnicodeString Directory = L"",
    bool FileNameOnly = false, TStrings *AFileList = nullptr);
  TStrings *CreateSelectedFileList(TOperationSide Side,
    TFarPanelInfo **APanelInfo = nullptr);
  TStrings *CreateFocusedFileList(TOperationSide Side,
    TFarPanelInfo **APanelInfo = nullptr);
  void CustomCommandGetParamValue(
    UnicodeString AName, UnicodeString &Value);
  void TerminalSynchronizeDirectory(UnicodeString LocalDirectory,
    UnicodeString RemoteDirectory, bool &Continue, bool Collect);
  void QueueListUpdate(TTerminalQueue *Queue);
  void QueueItemUpdate(TTerminalQueue *Queue, TQueueItem *Item);
  void QueueEvent(TTerminalQueue *Queue, TQueueEvent Event);
  void GetSpaceAvailable(UnicodeString APath,
    TSpaceAvailable &ASpaceAvailable, bool &Close);
  void QueueAddItem(TQueueItem *Item);
  UnicodeString GetFileNameHash(UnicodeString AFileName) const;
  intptr_t GetFilesRemote(TObjectList *PanelItems, bool Move,
    UnicodeString &DestPath, OPERATION_MODES OpMode);

private:
  TTerminalQueue *GetQueue();
  TTerminalQueueStatus *GetQueueStatus();

private:
  TTerminal *FTerminal;
  TTerminalQueue *FQueue;
  TTerminalQueueStatus *FQueueStatus;
  TCriticalSection FQueueStatusSection;
  TQueueEvent FQueueEvent;
  HANDLE FProgressSaveScreenHandle;
  HANDLE FSynchronizationSaveScreenHandle;
  HANDLE FAuthenticationSaveScreenHandle;
  TDateTime FSynchronizationStart;
  TStrings *FFileList;
  TList *FPanelItems;
  UnicodeString FSavedFindFolder;
  UnicodeString FOriginalEditFile;
  UnicodeString FLastEditFile;
  UnicodeString FLastMultipleEditFile;
  UnicodeString FLastMultipleEditFileTitle;
  UnicodeString FLastMultipleEditDirectory;
  intptr_t FLastEditorID;
  TGUICopyParamType FLastEditCopyParam;
  TKeepaliveThread *FKeepaliveThread;
  TSynchronizeController *FSynchronizeController;
  TStrings *FCapturedLog;
  TStrings *FAuthenticationLog;
  typedef rde::map<intptr_t, TMultipleEdit> TMultipleEdits;
  TMultipleEdits FMultipleEdits;
  typedef rde::vector<TEditHistory> TEditHistories;
  TEditHistories FEditHistories;
  UnicodeString FLastPath;
  TStrings *FPathHistory;
  UnicodeString FSessionsFolder;
  UnicodeString FNewSessionsFolder;
  UnicodeString FPrevSessionName;
  bool FQueueStatusInvalidated;
  bool FQueueItemInvalidated;
  bool FRefreshLocalDirectory;
  bool FRefreshRemoteDirectory;
  bool FQueueEventPending;
  bool FReloadDirectory;
  bool FLastMultipleEditReadOnly;
  bool FNoProgress;
  bool FSynchronizationCompare;
  bool FEditorPendingSave;
  bool FNoProgressFinish;
  bool FSynchronisingBrowse;
  bool FOutputLog;
  bool FLoadingSessionList;
  bool FCurrentDirectoryWasChanged;
};

class TSessionPanelItem : public TCustomFarPanelItem
{
  NB_DISABLE_COPY(TSessionPanelItem)
public:
  explicit TSessionPanelItem(const TSessionData *ASessionData);
  static void SetPanelModes(TFarPanelModes *PanelModes);
  static void SetKeyBarTitles(TFarKeyBarTitles *KeyBarTitles);

protected:
  UnicodeString FPath;
  const TSessionData *FSessionData;

  virtual void GetData(
    PLUGINPANELITEMFLAGS &Flags, UnicodeString &AFileName, int64_t &Size,
    uintptr_t &FileAttributes,
    TDateTime &LastWriteTime, TDateTime &LastAccess,
    uintptr_t &NumberOfLinks, UnicodeString &Description,
    UnicodeString &Owner, void *&UserData, size_t &CustomColumnNumber) override;
};

class TSessionFolderPanelItem : public TCustomFarPanelItem
{
public:
  explicit TSessionFolderPanelItem(UnicodeString Folder);

protected:
  UnicodeString FFolder;

  virtual void GetData(
    PLUGINPANELITEMFLAGS &Flags, UnicodeString &AFileName, int64_t &Size,
    uintptr_t &FileAttributes,
    TDateTime &LastWriteTime, TDateTime &LastAccess,
    uintptr_t &NumberOfLinks, UnicodeString &Description,
    UnicodeString &Owner, void *&UserData, size_t &CustomColumnNumber) override;
};

class TRemoteFilePanelItem : public TCustomFarPanelItem
{
  NB_DISABLE_COPY(TRemoteFilePanelItem)
public:
  explicit TRemoteFilePanelItem(TRemoteFile *ARemoteFile);
  static void SetPanelModes(TFarPanelModes *PanelModes);
  static void SetKeyBarTitles(TFarKeyBarTitles *KeyBarTitles);

protected:
  TRemoteFile *FRemoteFile;

  virtual void GetData(
    PLUGINPANELITEMFLAGS &Flags, UnicodeString &AFileName, int64_t &Size,
    uintptr_t &FileAttributes,
    TDateTime &LastWriteTime, TDateTime &LastAccess,
    uintptr_t &NumberOfLinks, UnicodeString &Description,
    UnicodeString &Owner, void *&UserData, size_t &CustomColumnNumber) override;
  virtual UnicodeString GetCustomColumnData(size_t Column) override;
  static void TranslateColumnTypes(UnicodeString &AColumnTypes,
    TStrings *ColumnTitles);
};

