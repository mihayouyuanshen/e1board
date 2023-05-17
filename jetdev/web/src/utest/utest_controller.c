#include "test_g.h"
#include "ctrl_g.h"

void TEST_Ctrl_log(void)
{
	CTRL_writeLogWithAuthenToken("VUDHJGEBSGNWTYXA", "26-1", "OP");
}

void TEST_CTRL_GetContentByFormdata()
{
	CtrlContent_t tmp_content_P;
	tmp_content_P.ContentStringMaxLen_SC = 32;
	tmp_content_P.ContentType_SC = CTRL_CONTENT_TYPE_STRING;
	uchar *tmp_formdata_UCP = "noticeToken=XXX&custAplId=&serviceAplId=yyy&hostModel=abc&appVer=1001";

	uchar *tmp_itemName_UCP = "noticeToken";
	CTRL_GetContentByFormdata(tmp_formdata_UCP, tmp_itemName_UCP, &tmp_content_P);
	LOG_debug("noticeToken: %s", tmp_content_P.ContentValueString);

	tmp_itemName_UCP = "custAplId";
	CTRL_GetContentByFormdata(tmp_formdata_UCP, tmp_itemName_UCP, &tmp_content_P);
	LOG_debug("custAplId: %s", tmp_content_P.ContentValueString);

	tmp_itemName_UCP = "serviceAplId";
	CTRL_GetContentByFormdata(tmp_formdata_UCP, tmp_itemName_UCP, &tmp_content_P);
	LOG_debug("serviceAplId: %s", tmp_content_P.ContentValueString);

	tmp_itemName_UCP = "hostModel";
	CTRL_GetContentByFormdata(tmp_formdata_UCP, tmp_itemName_UCP, &tmp_content_P);
	LOG_debug("hostModel: %s", tmp_content_P.ContentValueString);

	tmp_itemName_UCP = "appVer";
	tmp_content_P.ContentType_SC = CTRL_CONTENT_TYPE_INT;
	CTRL_GetContentByFormdata(tmp_formdata_UCP, tmp_itemName_UCP, &tmp_content_P);
	LOG_debug("appVer: %d", tmp_content_P.ContentValueInt_SI);
}