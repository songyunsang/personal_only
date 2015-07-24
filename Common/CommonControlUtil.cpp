#include "stdafx.h"

#include "CommonControlUtil.h"

namespace CommonControlUtility
{
	std::vector<long> GetCurSels(const CListCtrl& ListCtrl)
	{
		std::vector<long> vecIndex;

		POSITION pos = ListCtrl.GetFirstSelectedItemPosition();

		while( pos != NULL ) vecIndex.push_back(ListCtrl.GetNextSelectedItem(pos));

		return vecIndex;
	}

}