#include "stdafx.h"
#pragma hdrstop
#include "tntQAVI.h"

CAviPlayerCustom::CAviPlayerCustom	()
{
    std::memset(this, 0, sizeof(*this));
	m_dwFrameCurrent	= 0xfffffffd;	// страхуемся от 0xffffffff + 1 == 0
	m_dwFirstFrameOffset= 0;
}

CAviPlayerCustom::~CAviPlayerCustom()
{
	if(m_aviIC) 
	{
		ICDecompressEnd(m_aviIC);
		ICClose(m_aviIC);
	}

	if(m_pDecompressedBuf)	xr_free(m_pDecompressedBuf);
	if(m_pMovieData )	xr_free(m_pMovieData);
	if(m_pMovieIndex ) xr_free(m_pMovieIndex);

	xr_delete(alpha);
}

//---------------------------------
BOOL CAviPlayerCustom::Load (char* fname)
{
	// Check for alpha
	string_path		aname;
	strconcat		(sizeof(aname),aname,fname,"_alpha");
	if (FS.exist(aname))	
	{
		alpha		= xr_new<CAviPlayerCustom>	();
		alpha->Load	(aname);
	}

	// Open with mmioOpen( ) AVI file
	HMMIO hmmioFile = mmioOpen( fname, NULL, MMIO_READ /*MMIO_EXCLUSIVE*/ );
	if( hmmioFile == NULL ) {

		return FALSE;
	}

	// Find chunk FOURCC('movi')

	MMCKINFO mmckinfoParent;
    std::memset( &mmckinfoParent, 0, sizeof(mmckinfoParent) );
	mmckinfoParent.fccType = mmioFOURCC('A', 'V', 'I', ' ');				//	O	L	E	S	Style: ON
	MMRESULT res;
	if( MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, NULL, MMIO_FINDRIFF)) ) {

		mmioClose( hmmioFile, 0 );
		return FALSE;
	}

    std::memset( &mmckinfoParent, 0, sizeof(mmckinfoParent) );
	mmckinfoParent.fccType = mmioFOURCC('h', 'd', 'r', 'l'); 
	if( MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, NULL, MMIO_FINDLIST)) ) {

		mmioClose( hmmioFile, 0 );
		return FALSE;
	}
//-------------------------------------------------------------------
	//++strl
    std::memset( &mmckinfoParent, 0, sizeof(mmckinfoParent) );
	mmckinfoParent.fccType = mmioFOURCC('s', 't', 'r', 'l'); 
	if( MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, NULL, MMIO_FINDLIST)) ) {

		mmioClose( hmmioFile, 0 );
		return FALSE;
	}

	//++strh
    std::memset( &mmckinfoParent, 0, sizeof(mmckinfoParent) );
	mmckinfoParent.fccType = mmioFOURCC('s', 't', 'r', 'h'); 
	if( MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, NULL, MMIO_FINDCHUNK)) ) {

		mmioClose( hmmioFile, 0 );
		return FALSE;
	}

	AVIStreamHeaderCustom	strHeight;

    std::memset(&strHeight,0,sizeof(strHeight));
	if( mmckinfoParent.cksize != (DWORD)mmioRead(hmmioFile, (HPSTR)&strHeight, mmckinfoParent.cksize) ) {

		mmioClose( hmmioFile, 0 );
		return FALSE;
	}



    AVIFileInit		();
	PAVIFILE aviFile = NULL;
	if (AVIERR_OK != AVIFileOpen(&aviFile, fname, OF_READ, NULL)) 
	{ 
		return FALSE; 
	}

	AVIFILEINFO		aviInfo;
    std::memset(&aviInfo,0,sizeof(aviInfo));
	if( AVIERR_OK != AVIFileInfo( aviFile, &aviInfo, sizeof(aviInfo) ) ){
		AVIFileRelease( aviFile );
		return FALSE;
	}

	m_dwFrameTotal	= aviInfo.dwLength;
	m_fCurrentRate	= (float) aviInfo.dwRate / (float)aviInfo.dwScale;

	m_dwWidth			= aviInfo.dwWidth;
	m_dwHeight			= aviInfo.dwHeight;

	AVIFileRelease( aviFile );

	R_ASSERT			( m_dwWidth && m_dwHeight );

	m_pDecompressedBuf	= (BYTE *)xr_malloc( m_dwWidth * m_dwHeight * 4 + 4);

	//++strf
    std::memset(&mmckinfoParent,0,sizeof(mmckinfoParent));
	mmckinfoParent.fccType = mmioFOURCC('s', 't', 'r', 'f'); 
	if( MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoParent, NULL, MMIO_FINDCHUNK)) ) {

		mmioClose( hmmioFile, NULL );
		return FALSE;
	}

	// Getting output format of decompressor in BITMAPINFOHEADER
	if( mmckinfoParent.cksize != (DWORD)mmioRead(hmmioFile, (HPSTR)&m_biInFormat, mmckinfoParent.cksize) ) {

		mmioClose( hmmioFile, NULL );
		return FALSE;
	}

	// Make output format of decompressor	(xRGB)
	m_biOutFormat.biSize	= sizeof( m_biOutFormat );
	m_biOutFormat.biBitCount= 32;
	m_biOutFormat.biCompression	= BI_RGB;
	m_biOutFormat.biPlanes	= 1;
	m_biOutFormat.biWidth	= m_dwWidth;
	m_biOutFormat.biHeight	= m_dwHeight;
	m_biOutFormat.biSizeImage = m_dwWidth * m_dwHeight * 4;

	// Search ready compressor
	m_aviIC = ICLocate( ICTYPE_VIDEO, NULL, &m_biInFormat, &m_biOutFormat, ICMODE_FASTDECOMPRESS);
	if( m_aviIC == NULL) 
	{
		return FALSE;
	}

	// init decompressor
	if( ICERR_OK != ICDecompressBegin(m_aviIC, &m_biInFormat, &m_biOutFormat) ) 
	{
		return FALSE;
	}

	//--strf
	if( MMSYSERR_NOERROR != mmioAscend( hmmioFile, &mmckinfoParent, 0 ) ) 
	{
		mmioClose( hmmioFile, NULL );
		return FALSE;
	}

	//--strh
	if( MMSYSERR_NOERROR != mmioAscend( hmmioFile, &mmckinfoParent, 0 ) ) 
	{
		mmioClose( hmmioFile, NULL );
		return FALSE;
	}

	//--strl
	if( MMSYSERR_NOERROR != mmioAscend( hmmioFile, &mmckinfoParent, 0 ) ) {

		mmioClose( hmmioFile, NULL );
		return FALSE;
	}

//-------------------------------------------------------------------
	MMCKINFO mmckinfoSubchunk;
    std::memset(&mmckinfoSubchunk,0,sizeof(mmckinfoSubchunk));
	mmckinfoSubchunk.fccType = mmioFOURCC('m', 'o', 'v', 'i'); 
	if( MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoSubchunk, NULL, MMIO_FINDLIST)) \
		|| mmckinfoSubchunk.cksize <= 4 )
	{
		mmioClose( hmmioFile, NULL );
		return FALSE;
	}

	mmioSeek( hmmioFile, mmckinfoSubchunk.dwDataOffset, SEEK_SET );
	
	// Pick out memory for compressed data of all clip
	m_pMovieData = (BYTE *)xr_malloc( mmckinfoSubchunk.cksize );
	if( m_pMovieData == NULL ) 
	{
		mmioClose( hmmioFile, NULL );
		return FALSE;
	}

	if( mmckinfoSubchunk.cksize != (DWORD)mmioRead( hmmioFile, (HPSTR)m_pMovieData, mmckinfoSubchunk.cksize ) ) {

		xr_free( m_pMovieData );	m_pMovieData	= NULL;
		mmioClose( hmmioFile, NULL);
		return FALSE;
	}

	if( MMSYSERR_NOERROR != mmioAscend( hmmioFile, &mmckinfoSubchunk, 0 ) ) {

		xr_free( m_pMovieData );	m_pMovieData	= NULL;
		mmioClose( hmmioFile, NULL);
		return FALSE;
	}

	// Find chunk FOURCC('idx1')
    std::memset(&mmckinfoSubchunk,0,sizeof(mmckinfoSubchunk));
	mmckinfoSubchunk.fccType = mmioFOURCC('i', 'd', 'x', '1'); 

	if( MMSYSERR_NOERROR != (res = mmioDescend(hmmioFile, &mmckinfoSubchunk, NULL, MMIO_FINDCHUNK)) \
		|| mmckinfoSubchunk.cksize <= 4 )
	{
		xr_free( m_pMovieData );	m_pMovieData	= NULL;
		mmioClose( hmmioFile, 0 );
		return FALSE;
	}

	// Pick out memory for index(?)
#pragma todo("Vertver to Oles: cho za huynyu ti pridumal?")
	m_pMovieIndex = (AVIINDEXENTRY *)xr_malloc( mmckinfoSubchunk.cksize );
	if( m_pMovieIndex == NULL ) {

		// for index we CAN'T use xr_free method
		xr_free( m_pMovieData );
		m_pMovieData		= NULL;
		mmioClose( hmmioFile, NULL);
		return FALSE;
	}

	if( mmckinfoSubchunk.cksize != (DWORD)mmioRead( hmmioFile, (HPSTR)m_pMovieIndex, mmckinfoSubchunk.cksize ) ) {

		xr_free( m_pMovieIndex );	m_pMovieIndex	= NULL;
		xr_free( m_pMovieData );	m_pMovieData	= NULL;
		mmioClose( hmmioFile, NULL);
		return FALSE;
	}

	// Closing AVI file with mmioClose()
	mmioClose( hmmioFile, 0 );

	if (alpha)	{
		R_ASSERT(m_dwWidth  == alpha->m_dwWidth	);
		R_ASSERT(m_dwHeight == alpha->m_dwHeight);
	}

//-----------------------------------------------------------------
	return TRUE;
}

BOOL CAviPlayerCustom::DecompressFrame( DWORD dwFrameNum )
{
	// Getting element of index
	AVIINDEXENTRY	*pCurrFrameIndex = &m_pMovieIndex[ dwFrameNum ];

	m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
	R_ASSERT( m_biInFormat.biSizeImage != NULL);

	DWORD	dwFlags;
	dwFlags = (pCurrFrameIndex->dwFlags & AVIIF_KEYFRAME) ? 0 : ICDECOMPRESS_NOTKEYFRAME;
	m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
	dwFlags |= (m_biInFormat.biSizeImage) ? 0 : ICDECOMPRESS_NULLFRAME;

	if( ICERR_OK != ICDecompress(m_aviIC, dwFlags, &m_biInFormat, 
		(m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8), 
		&m_biOutFormat, m_pDecompressedBuf) ) 
	{
		return	FALSE;
	}

	if (alpha)	{
		// update
		BYTE*	alpha_buf;
		alpha->GetFrame(&alpha_buf);
		u32*	dst		= (u32*)m_pDecompressedBuf;
		u32*	src		= (u32*)alpha_buf;
		u32*	end		= dst+u32(m_dwWidth*m_dwHeight);
		for (; dst!=end; src++,dst++)
		{
			u32&	d	= *dst;
			u32		s	= *src;
			u32		a	= (color_get_R(s)+color_get_G(s)+color_get_B(s))/3;
			d			= subst_alpha	(d,a);
		}
	}

	return	TRUE;
}

/************************************************
*
* GetFrame
* Return TRUE if frame be changed, else FALSE
*
*************************************************/

BOOL CAviPlayerCustom::GetFrame( BYTE **pDest )
{
	R_ASSERT( pDest );

	DWORD	dwCurrFrame;
	dwCurrFrame	= CalcFrame();
	// If marked frame = previos
	if( dwCurrFrame == m_dwFrameCurrent ) 
	{
		*pDest				= m_pDecompressedBuf;
		return	FALSE;
	} else
	// If marked frame is previos frame++
	if( dwCurrFrame == m_dwFrameCurrent + 1 ) 
	{
		++m_dwFrameCurrent;	
		*pDest	= m_pDecompressedBuf;
		DecompressFrame( m_dwFrameCurrent );
		return	TRUE;
	} else {
		
		// It's arbitrary frame

		if( ! (m_pMovieIndex[ dwCurrFrame ].dwFlags & AVIIF_KEYFRAME) ) {

			// It's not a main frame! -
			// make PreRoll for near previous main frame to marked-1
			PreRoll( dwCurrFrame );
		}			

		m_dwFrameCurrent	= dwCurrFrame;
		
		*pDest	= m_pDecompressedBuf;

		// Decompress this frame
		DecompressFrame( m_dwFrameCurrent );
		return	TRUE;
	}
}

// Without validates - we're need a SPEED
VOID CAviPlayerCustom::PreRoll( DWORD dwFrameNum )
{
	int i;

	AVIINDEXENTRY	*pCurrFrameIndex;
	DWORD		res;

	// Search in massive first previos main frame
	// or take a frame before compress
	for( i=(int)dwFrameNum-1 ; i>0 ; i-- ) {

		if( m_pMovieIndex[ i ].dwFlags & AVIIF_KEYFRAME )	break;

		if( (int)m_dwFrameCurrent == i ) {

			// Before this we gets decompessed before that frame:
			// decompress all UNmain frames with flags PREROLL & NOTKEYFRAME
			for( i++ ; i<(int)dwFrameNum ; i++ ) {

				pCurrFrameIndex = &m_pMovieIndex[ i ];

				DWORD	dwFlags;
				dwFlags = ICDECOMPRESS_PREROLL | ICDECOMPRESS_NOTKEYFRAME | ICDECOMPRESS_HURRYUP;
				m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
				dwFlags |= (m_biInFormat.biSizeImage) ? 0 : ICDECOMPRESS_NULLFRAME;

				res = ICDecompress(m_aviIC, dwFlags, &m_biInFormat, (m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8) /*m_pCompressedBuf*/, &m_biOutFormat, m_pDecompressedBuf);
				if( ICERR_OK != res && ICERR_DONTDRAW != res ) {	// Checking for ICERR_DONTDRAW added cuz indeo 5.11

					R_ASSERT( 0 );
				}

			} // for(...
		
			return;
		}	// if( (int)m_dwFrameCurrent == i )...
	}	// for( i=(int)dwFrameNum-1 ; i>0 ; i-- )...


	// Getting a index's element
	pCurrFrameIndex = &m_pMovieIndex[ i ];
	m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
	R_ASSERT( m_biInFormat.biSizeImage );

	// Decompress main frames with flags ICDECOMPRESS_PREROLL 
	res = ICDecompress(m_aviIC, ICDECOMPRESS_PREROLL | ICDECOMPRESS_HURRYUP, &m_biInFormat, m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8/*m_pCompressedBuf*/, &m_biOutFormat, m_pDecompressedBuf);
	if( ICERR_OK != res && ICERR_DONTDRAW != res ) 
	{

		R_ASSERT( NULL );
	}

	// Decompress all next UNmain frames with flags PREROLL & NOTKEYFRAME
	for( i++ ; i<(int)dwFrameNum ; i++ ) {

		pCurrFrameIndex = &m_pMovieIndex[ i ];

		DWORD	dwFlags;
		dwFlags = ICDECOMPRESS_PREROLL | ICDECOMPRESS_NOTKEYFRAME | ICDECOMPRESS_HURRYUP;
		m_biInFormat.biSizeImage = pCurrFrameIndex->dwChunkLength;
		dwFlags |= (m_biInFormat.biSizeImage) ? 0 : ICDECOMPRESS_NULLFRAME;

		res = ICDecompress(m_aviIC, dwFlags, &m_biInFormat, (m_pMovieData + pCurrFrameIndex->dwChunkOffset + 8) /*m_pCompressedBuf*/, &m_biOutFormat, m_pDecompressedBuf);
		if( ICERR_OK != res && ICERR_DONTDRAW != res ) {	// Checking for ICERR_DONTDRAW added cuz indeo 5.11

			R_ASSERT( 0 );
		}
	} // for(...

}

void CAviPlayerCustom::GetSize(DWORD *dwWidth, DWORD *dwHeight)
{
	if( dwWidth )	*dwWidth = m_dwWidth;
	if( dwHeight )	*dwHeight = m_dwHeight;
}

int CAviPlayerCustom::SetSpeed( INT nPercent )
{
	m_fCurrentRate	= m_fRate * FLOAT( nPercent / 100.0f );
	return int(m_fCurrentRate / m_fRate * 100);
}

DWORD CAviPlayerCustom::CalcFrame()
	{	
		if(!m_dwFirstFrameOffset)
			m_dwFirstFrameOffset = RDEVICE.dwTimeContinual-1;

	return DWORD( floor( (RDEVICE.dwTimeContinual-m_dwFirstFrameOffset) * m_fCurrentRate / 1000.0f) ) % m_dwFrameTotal;
}
