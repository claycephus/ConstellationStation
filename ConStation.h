// ConStation.h : main header file for the CONSTATION application
//

#if !defined(AFX_CONSTATION_H__7FAE70C2_81A3_4453_8F8B_E07257316D56__INCLUDED_)
#define AFX_CONSTATION_H__7FAE70C2_81A3_4453_8F8B_E07257316D56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

// OpenGL Headers
#include <gl/gl.h>
#include <gl/glu.h>
////#include <math.h> 


/////////////////////////////////////////////////////////////////////////////
// Globals
typedef struct
{
	float r;
	float g;
	float b;
} CColor;

void glColor(CColor c);

extern const CColor WHITE, BLACK, RED, GREEN, BLUE, CONSTGREEN, DARKGREEN;


/////////////////////////////////////////////////////////////////////////////
// CConStationApp:
// See ConStation.cpp for the implementation of this class
//

class CConStationApp : public CWinApp
{
public:
	CConStationApp();


// Attributes
	CSingleDocTemplate* pDocTemplate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConStationApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CConStationApp)
	afx_msg void OnAppAbout();
	afx_msg void OnConstellationOpen();
	afx_msg void OnConstellationNew();
	afx_msg void OnConstellationSave();
	afx_msg void OnConstellationSaveas();
	afx_msg void OnStarfieldNewactual();
	afx_msg void OnStarfieldNewrandom();
	afx_msg void OnStarfieldOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONSTATION_H__7FAE70C2_81A3_4453_8F8B_E07257316D56__INCLUDED_)
