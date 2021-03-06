#include "XnLinkResponseMsgParser.h"
#include "XnLinkStatusCodes.h"
#include "XnLinkProtoUtils.h"
#include <XnOS.h>
#include <XnLog.h>

#define XN_MASK_LINK "xnLink"

namespace xn
{

XnStatus LinkResponseMsgParser::ParsePacketImpl(XnLinkFragmentation /*fragmentation*/,
												const XnUInt8* pSrc, 
												const XnUInt8* pSrcEnd, 
												XnUInt8*& pDst, 
												const XnUInt8* pDstEnd)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnSizeT nPacketDataSize = pSrcEnd - pSrc;
	if (nPacketDataSize < sizeof(XnLinkResponseInfo))
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_LINK_MISSING_RESPONSE_INFO;
	}
	XnUInt16 nResponseCode = XN_PREPARE_VAR16_IN_BUFFER(((XnLinkResponseInfo*)pSrc)->m_nResponseCode);

	nPacketDataSize -= sizeof(XnLinkResponseInfo);
	pSrc += sizeof(XnLinkResponseInfo);

	if (pDst + nPacketDataSize > pDstEnd)
	{
		XN_ASSERT(FALSE);
		return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
	}

	////////////////////////////////////////////
	xnOSMemCopy(pDst, pSrc, nPacketDataSize);
	////////////////////////////////////////////
	nRetVal = xnLinkResponseCodeToStatus(nResponseCode);
	if (nRetVal != XN_STATUS_OK)
	{
		xnLogWarning(XN_MASK_LINK, "Received error from link layer response: '%s' (%u)",
			xnGetStatusString(nRetVal), nResponseCode);
		xnLogWriteBinaryData(XN_MASK_LINK, XN_LOG_WARNING, 
			__FILE__, __LINE__, (XnUChar*)pSrc, (XnUInt32)nPacketDataSize, "Response extra data: ");

		XN_ASSERT(FALSE);
		return nRetVal;
	}

	pDst += nPacketDataSize;

	return XN_STATUS_OK;
}



}
