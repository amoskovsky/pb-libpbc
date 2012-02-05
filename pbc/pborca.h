/*                                                                         */
/* *********************************************************************** */
/*                                                                         */
/*                 Copyright Powersoft Corporation 1991 - 1995             */
/*                                                                         */
/*       Powersoft Corporation ("Powersoft") claims copyright in this      */
/*     program and documentation as an unpublished work, versions of       */
/*     which were first licensed on the date indicated in the foregoing    */
/*     notice. Claim of copyright does not imply waiver of Powersoft's     */
/*     other rights. See Notice of Proprietary Rights.                     */
/*                                                                         */
/*                 NOTICE OF PROPRIETARY RIGHTS                            */
/*                                                                         */
/*       This program and documentation are confidential trade             */
/*     secrets and the property of Powersoft.  Use, examination,           */
/*     reproduction, copying, disassembly, decompilation, transfer         */
/*     and/or disclosure to others are strictly prohibited except by       */
/*     express written agreement with Powersoft.                           */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/*    Filename : pborca.h                                                  */
/*                                                                         */
/*    Purpose  : PowerBuilder Open Repository CASE API                     */
/*                DLL public header file.                                  */
/*                                                                         */
/*    Notes    : 1. This header file should be included after <windows.h>  */
/*               2. This file was editted with tab stops 4.                */
/*                                                                         */
/* *********************************************************************** */

#ifndef PB_ORCA                              /* First time defined?  */
#define PB_ORCA                              /* Yes, make sure of it */

#ifdef    __cplusplus
extern "C" {
#endif    /* __cplusplus */

#if defined(macintosh) || defined(__unix) 
#ifndef FAR
#define FAR
#endif
#define far
#ifndef _WINDEF_
typedef TCHAR FAR*       LPTSTR;
typedef unsigned int	UINT;
#endif
#ifndef _WINNT_
typedef void FAR*      	LPVOID;
#endif

#if !defined(macintosh)
#define PASCAL			pascal
#endif

#define LONG			long
#endif

#if defined(macintosh) || defined(__unix) // gmoison 
#ifndef CALLBACK
#define CALLBACK
#endif
#ifndef WINAPI
#define WINAPI
#endif 
#endif

#ifndef PBWINAPI_
#define PBWINAPI_(t) t WINAPI
#endif

#ifndef PBCALLBACK
#define PBCALLBACK(r,n) r ( CALLBACK n )
#endif

#ifndef CHAR                                 /* We have a CHAR definiton? */
#define TCHAR TCHAR                            /* No, then do it */
#endif

#ifndef VOID                                 /* We have a VOID definiton? */
#define VOID void                            /* No, then do it */
#endif

#if !defined(_WINDEF_) && !defined(_INC_WINDOWS)/* We have a INT definiton? */
// #define INT int                             /* No, then do it */
typedef int INT;
#endif

/* *********************************************************************** */
/* ORCA Interface assorted #defines                                        */
/* *********************************************************************** */

#define HPBORCA           LPVOID             /* ORCA session handle */
#define PBORCA_MSGBUFFER  256                /* Suggested msg buffer size */
#define PBORCA_MAXCOMMENT 255                /* Maximum comment size */


/* *********************************************************************** */
/* ORCA Interface result #defines                                          */
/* *********************************************************************** */
#define PBORCA_OK              0             /* Operation successful */
#define PBORCA_INVALIDPARMS   -1             /* Invalid parameter list */
#define PBORCA_DUPOPERATION   -2             /* Duplicate operation */    
#define PBORCA_OBJNOTFOUND    -3             /* Object not found */ 
#define PBORCA_BADLIBRARY     -4             /* Bad library name */
#define PBORCA_LIBLISTNOTSET  -5             /* Library list not set */
#define PBORCA_LIBNOTINLIST   -6             /* Library not in lib list */
#define PBORCA_LIBIOERROR     -7             /* Library I/O error */          
#define PBORCA_OBJEXISTS      -8             /* Object exists */          
#define PBORCA_INVALIDNAME    -9             /* Invalid name */          
#define PBORCA_BUFFERTOOSMALL -10            /* Buffer size is too small */          
#define PBORCA_COMPERROR      -11            /* Compile error */
#define PBORCA_LINKERROR      -12            /* Link error */
#define PBORCA_CURRAPPLNOTSET -13            /* Current appl not set */
#define PBORCA_OBJHASNOANCS   -14            /* Object has no ancestors */          
#define PBORCA_OBJHASNOREFS   -15            /* Object has no references */          
#define PBORCA_PBDCOUNTERROR  -16            /* Invalid # of PBD's */          
#define PBORCA_PBDCREATERROR  -17            /* PBD create error */
#define PBORCA_CHECKOUTERROR  -18            /* check out/in error */
#define PBORCA_CBCREATEERROR  -19            /* component builder create error */
#define PBORCA_CBINITERROR    -20			 /* component builder init error */
#define PBORCA_CBBUILDERROR   -21            /* component builder build error */
#define PBORCA_SCCFAILURE        -22         /* Scc operation Failed */
#define PBORCA_REGREADERROR      -23         /* Error reading Registry for Scc provider */
#define PBORCA_LOADDLLFAILED     -24         /* LoadLibrary for Scc provider failed */
#define PBORCA_SCCINITFAILED     -25         /* Scc Initialize failed */
#define PBORCA_OPENPROJFAILED    -26         /* Scc Open Project failed */
#define PBORCA_TARGETNOTFOUND    -27         /* Target File not found: %s */
#define PBORCA_TARGETREADERR     -28         /* Unable to read Target File %s */ 
#define PBORCA_GETINTERFACEERROR -29         /* Unable to access required PowerBuilder interface */
#define PBORCA_IMPORTONLY_REQ    -30         /* Scc connect offline requires IMPORTONLY refresh option */
#define PBORCA_GETCONNECT_REQ    -31         /* Scc connect offline requires GetConnectProperties with Exclude_Checkout */
#define PBORCA_PBCFILE_REQ       -32         /* Scc connect offline with Exclude_Checkout requires PKC file */
#define PBORCA_DBCSERROR         -33         /* Locale setting error when convert unicode to ANSI_DBCS*/
 




/* *********************************************************************** */
/* ORCA Executable generation #defines for lFlags for
**	PBORCA_ExecutableCreate and PBORCA_DynamicLibraryCreate
**
** For p-code executables, value must be 0.
**
** Each option is specified by a separate bit:                             
**
**	0000 0000 0000 0000 0000 0000 0000 0000
**	^bit 31								  ^bit 0
**										  
**	bit 0:		0 = p-code, 1 = machine-code
**	bit 1:		0 = native, 1 = 16 bit
**	bit 2:		0 = no open server, 1 = open server
**	bit 3:		reserved
** 	bit 4:		0 = no trace info, 1 = trace info
**	bit 5:		0 = no runtime error context (routine & line numbers), 1 = error context
**	bits 6,7: 	reserved
**	bit 8:		0 = no optimization, 1 = optimization
**	bit 9:		0 = optimize for speed, 1 = optimize for space
**  bit 10:     0 = old style visual controls, 1 = new visual style controls (XP) 	//XP Enhancement
**  bit 11:     reserved     
**  bit 12:     Pocket PB Deployment:  1= DESKTOP
**  bit 13:     Pocket PB Deployment:  1 = ARM
**  bit 14:     Pocket PB Deployment:  1 = EM86
**  bit 15:     Pocket PB Deployment:  1 = X86
**	bits 16-31:	reserved //	
** 
**	Example:
**		long	lFlagVariable = 0;	initialize to zero
**
**		or in desired options
**		lFlagVariable |= PBORCA_MACHINE_CODE;
**		lFlagVariable |= PBORCA_MACHINE_CODE_OPT;
**		lFlagVariable |= PBORCA_MACHINE_CODE_OPT_SPACE;
**  Will produce machine code exe optimized for space.
** *********************************************************************** */

#define PBORCA_P_CODE					0x00000000
#define PBORCA_MACHINE_CODE				0x00000001
// Native is the default (32 bit for NT/Win95)
#define PBORCA_MACHINE_CODE_NATIVE		0x00000001
#define PBORCA_MACHINE_CODE_16			0x00000002
#define PBORCA_P_CODE_16				0x00000002

// Open server is available for machine code only
#define PBORCA_OPEN_SERVER				0x00000004
#define PBORCA_TRACE_INFO				0x00000010
#define PBORCA_ERROR_CONTEXT			0x00000020
#define PBORCA_MACHINE_CODE_OPT			0x00000100
#define PBORCA_MACHINE_CODE_OPT_SPEED	0x00000100
#define PBORCA_MACHINE_CODE_OPT_SPACE	0x00000200
#define PBORCA_MACHINE_CODE_OPT_NONE	0x00000000
#define PBORCA_NEW_VISUAL_STYLE_CONTROLS		0x00000400 	//XP Enhancement
#define PBORCA_PK_DESKTOP				0x00001000
#define PBORCA_PK_PPCARM				0x00002000
#define PBORCA_PK_PPCEM86				0x00004000		// obsolete
#define PBORCA_PK_PPCX86				0x00008000
#define PBORCA_PK_SPHONEARM				0x00010000
#define PBORCA_PK_SPHONEX86				0x00020000

// Common combinations

// wrl 12/3/96 -- NOCODEGEN test added to suppress C code generation on 
// platforms where that is not supported.  

#ifndef NOCODEGEN
#define PBORCA_MACHINE_DEFAULT (PBORCA_MACHINE_CODE | PBORCA_MACHINE_CODE_OPT_SPEED)
#else
#define PBORCA_MACHINE_DEFAULT (PBORCA_P_CODE)
#endif
																		
#define PBORCA_MACHINE_DEBUG (PBORCA_MACHINE_CODE | 					\
	PBORCA_TRACE_INFO | PBORCA_ERROR_CONTEXT)

#define PBORCA_MACHINE_16_DEFAULT (PBORCA_MACHINE_CODE | PBORCA_MACHINE_CODE_16	|\
	PBORCA_MACHINE_CODE_OPT_SPEED)

#define PBORCA_MACHINE_OPEN_DEFAULT (PBORCA_MACHINE_CODE | PBORCA_OPEN_SERVER |\
	PBORCA_MACHINE_CODE_OPT_SPEED)

/* *********************************************************************** */
/* ORCA entry types                                                        */
/* *********************************************************************** */

typedef enum pborca_type
{
    PBORCA_APPLICATION,     
    PBORCA_DATAWINDOW,      
    PBORCA_FUNCTION,      
    PBORCA_MENU,      
    PBORCA_QUERY,      
    PBORCA_STRUCTURE,      
    PBORCA_USEROBJECT,      
    PBORCA_WINDOW,
    PBORCA_PIPELINE,
    PBORCA_PROJECT,
    PBORCA_PROXYOBJECT,
    PBORCA_BINARY

} PBORCA_TYPE, FAR *PPBORCA_TYPE;

typedef enum pborca_reftype
{
    PBORCA_REFTYPE_SIMPLE,  
    PBORCA_REFTYPE_OPEN  

} PBORCA_REFTYPE, FAR *PPBORCA_REFTYPE;

typedef enum pborca_rebuild_type
{
    PBORCA_FULL_REBUILD,  
    PBORCA_INCREMENTAL_REBUILD,
    PBORCA_MIGRATE

} PBORCA_REBLD_TYPE, FAR *PPBORCA_REBLD_TYPE;

typedef enum pborca_encoding
{
    PBORCA_UNICODE,
    PBORCA_UTF8,
    PBORCA_HEXASCII,
    PBORCA_ANSI_DBCS    		   
} PBORCA_ENCODING;

typedef enum pborca_clobber
{
    PBORCA_NOCLOBBER,
    PBORCA_CLOBBER,
    PBORCA_CLOBBER_ALWAYS,
	PBORCA_CLOBBER_DECIDED_BY_SYSTEM
} PBORCA_ENUM_FILEWRITE_OPTION;

/* *********************************************************************** */
/* Structure for PBORCA_ConfigSession function.                            */
/* *********************************************************************** */
typedef struct  pborca_configsession
{
	PBORCA_ENUM_FILEWRITE_OPTION eClobber;	// Behavior when encountering an existing file
	PBORCA_ENCODING	eExportEncoding;		// Encoding of exported source
	BOOL			bExportHeaders;			// Format the source with appropriate export header
	BOOL			bExportIncludeBinary;	// Automatically include the binary component of the requested object
	BOOL			bExportCreateFile;		// Export source to a file instead of to a buffer		
	LPTSTR			pExportDirectory;		// Directory where exported files are written if bExportCreateFile is TRUE
	PBORCA_ENCODING eImportEncoding;		// Encoding of imported source
	PVOID	 		filler1;				//  Reserved for future use
	PVOID	 		filler2;
	PVOID	 		filler3;
	PVOID	 		filler4;
}  PBORCA_CONFIG_SESSION,  FAR *PPBORCA_CONFIG_SESSION; 

/* *********************************************************************** */
/* Structure for PBORCA_CompileEntryImport callback function.              */
/* *********************************************************************** */
typedef struct pborca_comperr  
{
    INT     iLevel;                         /* Error level */
    LPTSTR   lpszMessageNumber;              /* Pointer to message number */ 
    LPTSTR   lpszMessageText;                /* Pointer to message text */ 
    UINT    iColumnNumber;                  /* Column number */
    UINT	iLineNumber;                    /* Line number */

} PBORCA_COMPERR, FAR *PPBORCA_COMPERR;

/* *********************************************************************** */
/* Prototype for PBORCA_CompileEntryImport callback function.              */
/* *********************************************************************** */
typedef PBCALLBACK(void, *PBORCA_ERRPROC) ( PPBORCA_COMPERR, LPVOID );

/* *********************************************************************** */
/* Structure for PBORCA_ExecutableCreate callback function.                */
/* *********************************************************************** */
typedef struct pborca_linkerr  
{
    LPTSTR   lpszMessageText;                /* Pointer to message text */ 

} PBORCA_LINKERR, FAR *PPBORCA_LINKERR;

/* *********************************************************************** */
/* Prototype for PBORCA_ExecutableCreate callback function.                */
/* *********************************************************************** */
typedef PBCALLBACK(void, *PBORCA_LNKPROC) ( PPBORCA_LINKERR, LPVOID );


/* ************************************************************************* */
/* Structure for PBORCA_BuildProject/PBORCA_BuildProjectEx callback function.*/
/* ************************************************************************* */
typedef struct pborca_blderr  
{
    LPTSTR   lpszMessageText;                /* Pointer to message text */ 

} PBORCA_BLDERR, FAR *PPBORCA_BLDERR;

/* ************************************************************************* */
/* Prototype for PBORCA_BuildProject/PBORCA_BuildProjectEx callback function */
/* ************************************************************************* */
typedef PBCALLBACK(void, *PBORCA_BLDPROC) ( PPBORCA_BLDERR, LPVOID );

/* *********************************************************************** */
/* Structure for PBORCA_LibraryDirectory callback function.                */
/* *********************************************************************** */
typedef struct pborca_direntry 
{
					                        /* Comments */
    TCHAR            szComments[PBORCA_MAXCOMMENT + 1];

    LONG            lCreateTime;            /* Time of entry create/mod */ 
    LONG            lEntrySize;             /* Size of entry */
    LPTSTR           lpszEntryName;          /* Pointer to entry name */
    PBORCA_TYPE     otEntryType;            /* Entry type */

} PBORCA_DIRENTRY, FAR *PPBORCA_DIRENTRY;


/* *********************************************************************** */
/* Prototype for PBORCA_LibraryDirectory callback function.                */
/* *********************************************************************** */
typedef PBCALLBACK(void, *PBORCA_LISTPROC) ( PPBORCA_DIRENTRY, LPVOID );


/* *********************************************************************** */
/* Structure for PBORCA_LibraryEntryInformation callback function.         */
/* *********************************************************************** */
typedef struct pborca_entryinfo 
{
					     /* Comments */
  TCHAR           szComments[PBORCA_MAXCOMMENT + 1];

  LONG           lCreateTime;                /* Time of entry create/mod */ 
  LONG           lObjectSize;                /* Size of object */
  LONG           lSourceSize;                /* Size of source */

} PBORCA_ENTRYINFO, FAR *PPBORCA_ENTRYINFO;


/* *********************************************************************** */
/* Structure for PBORCA_ObjectQueryHierarchy callback function.            */
/* *********************************************************************** */
typedef struct pborca_hierarchy
{

  LPTSTR          lpszAncestorName;           /* Pointer to ancestor name */ 

} PBORCA_HIERARCHY, FAR *PPBORCA_HIERARCHY;

/* *********************************************************************** */
/* Prototype for PBORCA_ObjectQueryHierarchy callback function.            */
/* *********************************************************************** */
typedef PBCALLBACK(void, *PBORCA_HIERPROC) ( PPBORCA_HIERARCHY, LPVOID );


/* *********************************************************************** */
/* Structure for PBORCA_ObjectQueryReference callback function.            */
/* *********************************************************************** */
typedef struct pborca_reference
{

  LPTSTR          lpszLibraryName;            /* Pointer to library name */ 
  LPTSTR          lpszEntryName;              /* Pointer to entry name */
  PBORCA_TYPE    otEntryType;                /* Entry type */
  PBORCA_REFTYPE otEntryRefType;             /* Entry reference type */

} PBORCA_REFERENCE, FAR *PPBORCA_REFERENCE;

/* *********************************************************************** */
/* Prototype for PBORCA_ObjectQueryReference callback function.            */
/* *********************************************************************** */
typedef PBCALLBACK(void, *PBORCA_REFPROC) ( PPBORCA_REFERENCE, LPVOID );



/* *********************************************************************** */
/* Macintohs specific function prototypes                                  */
/* *********************************************************************** */
#if defined(macintosh)

OSErr 	InitMacOrca( void );
OSErr 	ExitMacOrca( void );

#endif

/****************************************************************************
    String lengths (each length does *not* include the terminating '\0')
****************************************************************************/
#define PBORCA_SCC_NAME_LEN            31      // lpSccName, SCCInitialize
#define PBORCA_SCC_USER_LEN            31      // lpUser, SCCOpenProject
#define PBORCA_SCC_PATH_LEN  	       300     // lpAuxProjPath, SCCGetProjPath

#ifndef _LPTEXTOUTPROC_DEFINED
#define _LPTEXTOUTPROC_DEFINED
typedef long (*LPTEXTOUTPROC) (LPCSTR, DWORD);
#endif /* _LPTEXTOUTPROC_DEFINED */

//***************************************************************************
//  lFlags to control PBORCA_SccRefreshTarget method
//***************************************************************************
#define PBORCA_SCC_REFRESH_ALL      0x00000001
#define PBORCA_SCC_OUTOFDATE        0x00000002
#define PBORCA_SCC_IMPORTONLY       0x00000004
#define PBORCA_SCC_EXCLUDE_CHECKOUT 0x00000008 

/* ****************************************************************** */
/* Structure to obtain PBORCA_SCC configuration data.                 */
/* ****************************************************************** */
typedef struct pborca_scc  
{
HWND       hWnd;                                         // User optionally fills this in
TCHAR      szProviderName[PBORCA_SCC_NAME_LEN + 1];      // ConnectScc returns this value
LONG       lCapabilities;                                // ConnectScc returns this value
TCHAR      szUserID[PBORCA_SCC_USER_LEN + 1];            // GetSccWorkspace returns this value
TCHAR      szProject[PBORCA_SCC_PATH_LEN + 1];           // GetSccWorkspace returns this value
TCHAR      szLocalProjPath[PBORCA_SCC_PATH_LEN + 1];     // GetSccWorkspace returns this value
TCHAR      szAuxPath[PBORCA_SCC_PATH_LEN + 1];           // GetSccWorkspace returns this value
TCHAR      szLogFile[PBORCA_SCC_PATH_LEN + 1];           // GetSccWorkspace returns this value
LPTEXTOUTPROC  fpSccMsgHandler;                          // Call back function for Scc provider msgs
PBORCA_BLDPROC fpOrcaMsgHandler;                         // Call back function for Orca Scc messages
LONG       lCommentLen;                                  // ConnectScc returns this value
LONG       lAppend;                                      // GetSccWorkspace returns this value
LPVOID     pCommBlk;                                     // Reserved for internal use
LONG       lDeleteTempFiles;                             // Delete Temp Files in Local Project Path when done
} PBORCA_SCC;


/* ************************************************************************************* */
/* Structure to pass executable version information to PBORCA_ExecutableCreate           */
/* ************************************************************************************* */
typedef struct pborca_exeinfo
{
LPTSTR         lpszCompanyName;  
LPTSTR         lpszProductName;        
LPTSTR         lpszDescription;        
LPTSTR         lpszCopyright;
LPTSTR         lpszFileVersion;
LPTSTR         lpszFileVersionNum;
LPTSTR         lpszProductVersion;
LPTSTR         lpszProductVersionNum;
} PBORCA_EXEINFO;


/* *********************************************************************** */
/* Structure for PBORCA_SccSetTarget callback function.                    */
/* *********************************************************************** */
typedef struct pborca_sccsettarget
{
  LPTSTR          lpszLibraryName;           /* Pointer to library name */
} PBORCA_SETTARGET, FAR *PPBORCA_SETTARGET;

/* *********************************************************************** */
/* Prototype for PBORCA_SccSetTarget callback function.                    */
/* *********************************************************************** */
typedef PBCALLBACK(void, *PBORCA_SETTGTPROC) ( PPBORCA_SETTARGET, LPVOID );


/* *********************************************************************** */
/* Session function prototypes                                             */
/* *********************************************************************** */

PBWINAPI_(VOID) PBORCA_SessionClose          ( HPBORCA hORCASession );

PBWINAPI_(VOID) PBORCA_SessionGetError       ( HPBORCA hORCASession,
                                                  LPTSTR   lpszErrorBuffer,
                                                  INT     iErrorBufferSize );

PBWINAPI_(HPBORCA) PBORCA_SessionOpen           ( void );

PBWINAPI_(INT) PBORCA_SessionSetCurrentAppl ( HPBORCA hORCASession,
                                                  LPTSTR   lpszApplLibName,  
                                                  LPTSTR   lpszApplName );

PBWINAPI_(INT) PBORCA_SessionSetLibraryList ( HPBORCA    hORCASession,
                                                  LPTSTR FAR *pLibNames, 
                                                  INT        iNumberOfLibs );

PBWINAPI_(INT) PBORCA_ConfigureSession		( HPBORCA	hORCASession,
											  PPBORCA_CONFIG_SESSION pSessionConfig );

/* *********************************************************************** */
/* Library management function prototypes                                  */
/* *********************************************************************** */

PBWINAPI_(INT) PBORCA_LibraryCommentModify ( HPBORCA hORCASession,
                                             LPTSTR   lpszLibName,  
                                             LPTSTR   lpszLibComments );

PBWINAPI_(INT) PBORCA_LibraryCreate    ( HPBORCA hORCASession,
                                         LPTSTR   lpszLibName,  
                                         LPTSTR   lpszLibComments );

PBWINAPI_(INT) PBORCA_LibraryDelete    ( HPBORCA hORCASession,
                                         LPTSTR   lpszLibName );   

PBWINAPI_(INT) PBORCA_LibraryDirectory ( HPBORCA         hORCASession,
                                         LPTSTR           lpszLibName,       
                                         LPTSTR           lpszLibComments,  
                                         INT             iCmntsBuffLen, 
                                         PBORCA_LISTPROC pListProc, 
                                         LPVOID          pUserData );

PBWINAPI_(INT) PBORCA_LibraryEntryCopy ( HPBORCA     hORCASession,
                                         LPTSTR       lpszSourceLibName,  
                                         LPTSTR       lpszDestLibName,  
                                         LPTSTR       lpszEntryName, 
                                         PBORCA_TYPE otEntryType );

PBWINAPI_(INT) PBORCA_LibraryEntryDelete ( HPBORCA     hORCASession,
                                           LPTSTR       lpszLibName,  
                                           LPTSTR       lpszEntryName, 
                                           PBORCA_TYPE otEntryType );

PBWINAPI_(INT) PBORCA_LibraryEntryExport ( HPBORCA     hORCASession,
                                           LPTSTR       lpszLibName,  
                                           LPTSTR       lpszEntryName, 
                                           PBORCA_TYPE otEntryType,    
                                           LPTSTR       lpszExportBuffer, 
                                           LONG        lExportBufferSize );

PBWINAPI_(INT) PBORCA_LibraryEntryExportEx ( HPBORCA      hORCASession,
                                           LPTSTR       lpszLibName,  
                                           LPTSTR       lpszEntryName, 
                                           PBORCA_TYPE  otEntryType,    
                                           LPTSTR       lpszExportBuffer, 
                                           LONG         lExportBufferSize,
                                           LONG         *plReturnSize );

PBWINAPI_(INT) PBORCA_LibraryEntryInformation ( HPBORCA           hORCASession,
                                                LPTSTR             lpszLibName,  
                                                LPTSTR             lpszEntryName, 
                                                PBORCA_TYPE       otEntryType,    
                                                PPBORCA_ENTRYINFO pEntryInfo );

PBWINAPI_(INT) PBORCA_LibraryEntryMove ( HPBORCA     hORCASession,
                                         LPTSTR       lpszSourceLibName,  
                                         LPTSTR       lpszDestLibName,  
                                         LPTSTR       lpszEntryName, 
                                         PBORCA_TYPE otEntryType );

/* *********************************************************************** */
/* Source management function prototypes                                   */
/* *********************************************************************** */
PBWINAPI_(INT)  PBORCA_SccGetConnectProperties    ( HPBORCA       hORCASession,
													LPTSTR        pWorkspaceFile,
													PBORCA_SCC   *pConfig );

PBWINAPI_(INT)  PBORCA_SccConnect (	HPBORCA       hORCASession,
									PBORCA_SCC   *pConfig );

PBWINAPI_(INT)  PBORCA_SccConnectOffline (	HPBORCA       hORCASession,
									PBORCA_SCC   *pConfig );

PBWINAPI_(INT)  PBORCA_SccSetTarget  ( HPBORCA  hORCASession,
									   LPTSTR   pTargetFileName,
									   LONG     lFlags,
									   PBORCA_SETTGTPROC pSetTgtProc,
									   LPVOID   pUserData );

PBWINAPI_(INT)  PBORCA_SccExcludeLibraryList (	HPBORCA  hORCASession,
										  		LPTSTR  *pLibNames,
										  		INT      iNumLibs );
											   
PBWINAPI_(INT)  PBORCA_SccRefreshTarget ( HPBORCA  hORCASession,
										  PBORCA_REBLD_TYPE	eRebldType);
											   
PBWINAPI_(INT)  PBORCA_SccClose ( HPBORCA hORCASession );

PBWINAPI_(INT)  PBORCA_SccGetLatestVer ( HPBORCA hORCASession, LONG nFiles, LPTSTR * ppFileNames );

PBWINAPI_(INT)  PBORCA_SccSetPassword ( HPBORCA hORCASession, LPTSTR pPassword);

PBWINAPI_(INT)  PBORCA_SccResetRevisionNumber ( HPBORCA		hORCASession, 
												LPTSTR		lpszLibraryName,  
                                           		LPTSTR		lpszEntryName, 
                                           		PBORCA_TYPE	otEntryType, 
                                           		LPTSTR		lpszRevisionNum);  

/* *********************************************************************** */
/* Compilation function prototypes                                         */
/* *********************************************************************** */

PBWINAPI_(INT) PBORCA_CompileEntryImport ( HPBORCA        hORCASession,
                                           LPTSTR          lpszLibraryName,  
                                           LPTSTR          lpszEntryName, 
                                           PBORCA_TYPE    otEntryType, 
                                           LPTSTR          lpszComments,  
                                           LPTSTR          lpszEntrySyntax,
                                           LONG           lEntrySyntaxBuffSize, 
                                           PBORCA_ERRPROC pCompErrProc, 
                                           LPVOID         pUserData );

PBWINAPI_(INT) PBORCA_CompileEntryImportList ( HPBORCA          hORCASession,
                                               LPTSTR far       *pLibraryNames,  
                                               LPTSTR far       *pEntryNames, 
                                               PBORCA_TYPE far *otEntryType, 
                                               LPTSTR far       *pComments,  
                                               LPTSTR far       *pEntrySyntaxBuffers,
                                               LONG far        *pEntrySyntaxBuffSizes, 
                                               INT              iNumberOfEntries, 
                                               PBORCA_ERRPROC   pCompErrProc, 
                                               LPVOID           pUserData );

PBWINAPI_(INT) PBORCA_CompileEntryRegenerate ( HPBORCA        hORCASession,
                                               LPTSTR          lpszLibraryName,  
                                               LPTSTR          lpszEntryName, 
                                               PBORCA_TYPE    otEntryType, 
                                               PBORCA_ERRPROC pCompErrProc, 
                                               LPVOID         pUserData );

PBWINAPI_(INT) PBORCA_ApplicationRebuild ( 	HPBORCA        		hORCASession,
                                           	PBORCA_REBLD_TYPE	eRebldType,  
                                           	PBORCA_ERRPROC 		pCompErrProc, 
                                            LPVOID         		pUserData );
// PB Extension Support
PBWINAPI_(INT) PBORCA_ImportPBeXtension  ( 	HPBORCA	hORCASession, 
							LPTSTR		lpszPBLName, 
							LPTSTR 		lpszPBXName);



/* *********************************************************************** */
/* Executable construction function prototypes                             */
/* *********************************************************************** */
PBWINAPI_(INT) PBORCA_SetExeInfo           ( HPBORCA        hORCASession,
											 PBORCA_EXEINFO *pExeInfo );

PBWINAPI_(INT) PBORCA_ExecutableCreate     ( HPBORCA        hORCASession,
                                             LPTSTR          lpszExeName,  
                                             LPTSTR          lpszIconName,        
                                             LPTSTR          lpszPBRName,        
	                                         PBORCA_LNKPROC pLinkErrProc,           
	                                         LPVOID         pUserData,              
                                             INT FAR       *iPBDFlags,  
                                             INT            iNumberOfPBDFlags,       
                                             LONG           lFlags );

PBWINAPI_(INT) PBORCA_DynamicLibraryCreate ( HPBORCA        hORCASession,           
                                             LPTSTR          lpszLibraryName,        
                                             LPTSTR          lpszPBRName,            
                                             LONG           lFlags );

PBWINAPI_(INT) PBORCA_BuildProject         ( HPBORCA        hORCASession,
                                             LPTSTR         lpszLibraryName,        
                                             LPTSTR         lpszProjectName,
	                                         PBORCA_BLDPROC pBuildErrProc,           
	                                         LPVOID         pUserData);

PBWINAPI_(INT) PBORCA_BuildProjectEx       ( HPBORCA        hORCASession,
                                             LPTSTR         lpszLibraryName,        
                                             LPTSTR         lpszProjectName,
											 LPTSTR			lpszServerName,
											 INT			iPort,
	                                         PBORCA_BLDPROC pBuildErrProc,
	                                         LPVOID         pUserData); 
                                                              
/* *********************************************************************** */
/* Object query function prototypes                                        */
/* *********************************************************************** */

PBWINAPI_(INT) PBORCA_ObjectQueryHierarchy ( HPBORCA         hORCASession,
                                             LPTSTR           lpszLibraryName,  
                                             LPTSTR           lpszEntryName, 
                                             PBORCA_TYPE     otEntryType, 
                                             PBORCA_HIERPROC pHierarchyProc, 
                                             LPVOID          pUserData );

PBWINAPI_(INT) PBORCA_ObjectQueryReference ( HPBORCA         hORCASession,
                                             LPTSTR           lpszLibraryName,  
                                             LPTSTR           lpszEntryName, 
                                             PBORCA_TYPE     otEntryType, 
                                             PBORCA_REFPROC  pReferenceProc, 
                                             LPVOID          pUserData );


/******************************************************************************/
/* Open Session function prototypes as a entrance for ANSI ORCA interface     */
/******************************************************************************/

PBWINAPI_(HPBORCA) PBORCA_SessionOpenA     ( void );

#if !defined(UNICODE) && !defined(_UNICODE)
#define PBORCA_SessionOpen PBORCA_SessionOpenA
#endif

/******************************************************************************/
/* Function pointers for ORCA methods                                         */
/******************************************************************************/
typedef INT		(WINAPI *FP_ApplicationRebuild)
		( HPBORCA, PBORCA_REBLD_TYPE, PBORCA_ERRPROC, LPVOID );
typedef INT		(WINAPI *FP_BuildProject)
		( HPBORCA, LPTSTR, LPTSTR, PBORCA_BLDPROC, LPVOID );
typedef INT		(WINAPI *FP_BuildProjectEx)
		( HPBORCA, LPTSTR, LPTSTR, LPTSTR, INT, PBORCA_BLDPROC, LPVOID );
typedef INT		(WINAPI *FP_CompileEntryImport)
		( HPBORCA, LPTSTR, LPTSTR, PBORCA_TYPE, LPTSTR, LPTSTR, LONG, PBORCA_ERRPROC, LPVOID );
typedef INT		(WINAPI *FP_CompileEntryImportList)
		( HPBORCA, LPTSTR *, LPTSTR *, PBORCA_TYPE *, LPTSTR *, LPTSTR *, LONG *, INT, PBORCA_ERRPROC, LPVOID );
typedef INT		(WINAPI *FP_CompileEntryRegenerate)
		( HPBORCA, LPTSTR, LPTSTR, PBORCA_TYPE, PBORCA_ERRPROC, LPVOID );
typedef INT		(WINAPI *FP_ConfigureSession)
		( HPBORCA, PPBORCA_CONFIG_SESSION );
typedef INT		(WINAPI *FP_DynamicLibraryCreate)
		( HPBORCA, LPTSTR, LPTSTR, LONG );
typedef INT		(WINAPI *FP_ExecutableCreate)
		( HPBORCA, LPTSTR, LPTSTR, LPTSTR, PBORCA_LNKPROC, LPVOID, INT *, INT, LONG );

typedef INT		(WINAPI *FP_LibraryCommentModify)
		( HPBORCA, LPTSTR, LPTSTR );
typedef INT		(WINAPI *FP_LibraryCreate)
		( HPBORCA, LPTSTR, LPTSTR );
typedef INT		(WINAPI *FP_LibraryDelete)
		( HPBORCA, LPTSTR );   
typedef INT     (WINAPI *FP_LibraryDirectory)
		( HPBORCA, LPTSTR, LPTSTR, INT, PBORCA_LISTPROC, LPVOID );
typedef INT		(WINAPI *FP_LibraryEntryCopy)
		( HPBORCA, LPTSTR, LPTSTR, LPTSTR, PBORCA_TYPE );
typedef INT		(WINAPI *FP_LibraryEntryDelete)
		( HPBORCA, LPTSTR, LPTSTR, PBORCA_TYPE );
typedef INT		(WINAPI *FP_LibraryEntryExport)
		( HPBORCA, LPTSTR, LPTSTR, PBORCA_TYPE, LPTSTR, LONG );    
typedef INT		(WINAPI *FP_LibraryEntryExportEx)
		( HPBORCA, LPTSTR, LPTSTR, PBORCA_TYPE, LPTSTR, LONG, LONG * );    
typedef INT     (WINAPI *FP_LibraryEntryInformation)  
		( HPBORCA, LPTSTR, LPTSTR, PBORCA_TYPE, PPBORCA_ENTRYINFO );
typedef INT		(WINAPI *FP_LibraryEntryMove)
		( HPBORCA, LPTSTR, LPTSTR, LPTSTR, PBORCA_TYPE );

typedef INT 	(WINAPI *FP_ObjectQueryHierarchy)
		( HPBORCA, LPTSTR, LPTSTR, PBORCA_TYPE, PBORCA_HIERPROC, LPVOID );
typedef INT		(WINAPI *FP_ObjectQueryReference) 
		( HPBORCA, LPTSTR, LPTSTR, PBORCA_TYPE, PBORCA_REFPROC, LPVOID );

typedef INT		(WINAPI *FP_SccClose) ( HPBORCA );
typedef INT		(WINAPI *FP_SccConnect)
		( HPBORCA, PBORCA_SCC * );
typedef INT		(WINAPI *FP_SccConnectOffline)
		( HPBORCA, PBORCA_SCC * );
typedef INT		(WINAPI *FP_SccExcludeLibraryList)
		( HPBORCA, LPTSTR *, INT );
typedef INT		(WINAPI *FP_SccGetConnectProperties)
		( HPBORCA, LPTSTR, PBORCA_SCC * );
typedef INT		(WINAPI *FP_SccGetLatestVer)
		( HPBORCA, LONG, LPTSTR * );
typedef INT		(WINAPI *FP_SccRefreshTarget)
		( HPBORCA, PBORCA_REBLD_TYPE );
typedef INT		(WINAPI *FP_SccSetPassword) ( HPBORCA, LPTSTR );
typedef INT		(WINAPI *FP_SccSetTarget)
		( HPBORCA, LPTSTR, LONG, PBORCA_SETTGTPROC, LPVOID );

typedef void    (WINAPI *FP_SessionClose) (HPBORCA);
typedef void	(WINAPI *FP_SessionGetError)
		( HPBORCA, LPTSTR, INT );
typedef HPBORCA (WINAPI *FP_SessionOpen) ();
typedef HPBORCA (WINAPI *FP_SessionOpenA) ();
typedef INT		(WINAPI *FP_SessionSetCurrentAppl)
		( HPBORCA, LPTSTR, LPTSTR );
typedef INT		(WINAPI *FP_SessionSetLibraryList)
		( HPBORCA, LPTSTR *, INT );
typedef INT		(WINAPI *FP_SetExeInfo)
		( HPBORCA, PBORCA_EXEINFO * );
typedef INT     (WINAPI *FP_SccResetRevisionNumber)
		( HPBORCA, LPTSTR, LPTSTR, PBORCA_TYPE, LPTSTR);
typedef INT 	(WINAPI *FP_ImportPBeXtension)
		( HPBORCA, LPTSTR, LPTSTR);


#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif	/* __cplusplus */

#endif    /* PB_ORCA */

