#include <typedef.h>
#include <util_g.h>
#include <stdio.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include "common_g.h"
#include "config.h"
void UTL_StrToLower(schar *tmp_str_cp)
{
	while (*tmp_str_cp != '\0')
	{
		if (('A' <= *tmp_str_cp) && (*tmp_str_cp <= 'Z'))
		{
			*tmp_str_cp = *tmp_str_cp + 32;
		}

		tmp_str_cp++;
	}
}

int UTIL_GetLocalip(char *local_ip_addr)
{
	int ret = -1;
	register int fd, intrface;
	struct ifreq ifr[32];
	struct ifconf ifc;

	if (local_ip_addr == NULL)
	{
		return ret;
	}
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) > 0)
	{
		ifc.ifc_len = sizeof ifr;
		ifc.ifc_buf = (caddr_t)ifr;
		if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc)) // 获取所有接口信息
		{
			intrface = ifc.ifc_len / sizeof(struct ifreq);
			while (intrface-- > 0)
			{
				// Get IP Address
				if (!(ioctl(fd, SIOCGIFADDR, (char *)&ifr[intrface])))
				{
					if (ifr[intrface].ifr_name[0] == 'e')
					{
						ret = 0;
						LOG_debug("eth_name: %s, ip: %s\n", ifr[intrface].ifr_name, inet_ntoa(((struct sockaddr_in *)(&ifr[intrface].ifr_addr))->sin_addr));
						sprintf(local_ip_addr, "%s", inet_ntoa(((struct sockaddr_in *)(&ifr[intrface].ifr_addr))->sin_addr));
						break;
					}
				}
			}
		}
	}
	if (fd > 0)
	{
		close(fd);
	}
	return ret;
}
/*==========================================================================*/
/*	Description		:返回某个字符串第一次出现的位置														*/
/*--------------------------------------------------------------------------*/
/*	param			:parameter_list														*/
/*--------------------------------------------------------------------------*/
/*	return			:sint														*/
/*--------------------------------------------------------------------------*/
/*	detail			:														*/
/*--------------------------------------------------------------------------*/
/*	history			:2023/05/09 : DHC Zhang.WY : 新規作成					*/
/*==========================================================================*/
sint UTIL_SubStrByChar(const uchar *rawStr, const int specialChar, uchar *subStr, uint subStrMaxSize)
{
	LOG_entry("UTIL_SubStrByChar");
	/* input parameters check */
	if (NULL == rawStr)
	{
		LOG_error("Parameters exits NULL!");
		LOG_leave("UTIL_SubStrByChar");

		return RET_ERROR_CODE;
	}

	uchar *p;
	p = strchr(rawStr, specialChar);
	sint charSize = 0;
	/* 如果不存在指定字符，首次出现位置索引就是全长*/
	if (NULL == p)
	{
		charSize = strlen(rawStr);
	}
	/* 存在指定字符，*/
	else
	{
		charSize = p - rawStr;
	}
	/* tmp初始化 */
	memset(subStr, '\0', subStrMaxSize);
	/* 将截取后的字符串拷贝到tmp中 */
	strncpy(subStr, rawStr, charSize);

	LOG_debug("subStr: %s\n", subStr);

	LOG_leave("UTIL_SubStrByChar");

	return RET_SUCCESS_CODE;
}