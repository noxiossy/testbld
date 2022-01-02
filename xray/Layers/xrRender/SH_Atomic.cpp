#include "stdafx.h"


#include "sh_atomic.h"
#include "ResourceManager.h"

#include "dxRenderDeviceRender.h"

// Atomic
//SVS::~SVS								()			{	_RELEASE(vs);		dxRenderDeviceRender::Instance().Resources->_DeleteVS			(this);	}
//SPS::~SPS								()			{	_RELEASE(ps);		dxRenderDeviceRender::Instance().Resources->_DeletePS			(this);	}
//SState::~SState							()			{	_RELEASE(state);	dxRenderDeviceRender::Instance().Resources->_DeleteState		(this);	}
//SDeclaration::~SDeclaration				()			{	_RELEASE(dcl);		dxRenderDeviceRender::Instance().Resources->_DeleteDecl		(this);	}

///////////////////////////////////////////////////////////////////////
//	SVS
SVS::SVS() :
	vs(0)
#ifdef	USE_DX10
//	,signature(0)
#endif	//	USE_DX10
{
	;
}


SVS::~SVS()
{
	DEV->_DeleteVS(this);
#ifdef	USE_DX10
	//_RELEASE(signature);
	//	Now it is release automatically
#endif	//	USE_DX10
	_RELEASE(vs);
}


///////////////////////////////////////////////////////////////////////
//	SPS
SPS::~SPS								()			{	_RELEASE(ps);		DEV->_DeletePS			(this);	}

#ifdef	USE_DX10
///////////////////////////////////////////////////////////////////////
//	SGS
SGS::~SGS								()			{	_RELEASE(gs);		DEV->_DeleteGS			(this);	}

///////////////////////////////////////////////////////////////////////
//	SInputSignature
SInputSignature::SInputSignature(ID3DBlob* pBlob)	{ VERIFY(pBlob); signature=pBlob; signature->AddRef();};
SInputSignature::~SInputSignature		()			{	_RELEASE(signature); DEV->_DeleteInputSignature(this); }
#endif	//	USE_DX10

///////////////////////////////////////////////////////////////////////
//	SState
SState::~SState							()			{	_RELEASE(state);	DEV->_DeleteState		(this);	}

///////////////////////////////////////////////////////////////////////
//	SDeclaration
SDeclaration::~SDeclaration()
{	
	DEV->_DeleteDecl(this);	
#ifdef	USE_DX10
	xr_map<ID3DBlob*, ID3D10InputLayout*>::iterator iLayout;
	iLayout = vs_to_layout.begin();
	for( ; iLayout != vs_to_layout.end(); ++iLayout)
	{
		//	Release vertex layout
		_RELEASE(iLayout->second);
	}
#else	//	USE_DX10
	//	Release vertex layout
	_RELEASE(dcl);
#endif	//	USE_DX10
}
