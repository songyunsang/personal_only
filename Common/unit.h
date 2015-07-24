#pragma once

#include <atlstr.h>
#include <atlcoll.h>

#include <math.h>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;

#define XOR(A, B) ((A || B) && !(A && B))

namespace IUNIT
{
	const double MIN_DIFF_DISTANCE = 0.010;		// 두 점이 똑같은지 평가하는 최소차이. 현재는 10 um

	const double IMILI	= 0.001;
	const double IMICRO	= 0.000001;
	const double INANO	= 0.000000001;
	const double IPI	= 3.1415926535897932384626433832795;
	const double IKRAD	= IPI / 180;

	template <typename T>
	struct tagRect
	{
		T l,t,r,b,width,height;

		tagRect():l(0),t(0),r(0),b(0),width(0),height(0)	{}
		tagRect(T left,T top,T right,T bottom)
			:l(left),t(top),r(right),b(bottom)	
		{
			width	= (left < right )	? (right - left) : (left - right);
			height	= (top < bottom )	? (bottom - top) : (top - bottom);
		}

		tagRect& operator=(const tagRect& rhs)
		{
			l = rhs.l;
			t = rhs.t;
			r = rhs.r;
			b = rhs.b;

			width	= (l < r )	? (r - l) : (l - r);
			height	= (t < b )	? (b - t) : (t - b);

			return *this;
		}

	};

	template <typename T>
	union tagPoint
	{
		struct
		{
			T x,y;
		};
		struct
		{
			T width,height;
		};

		tagPoint():x(0),y(0) {}

		tagPoint(T _x,T _y)
			:x(_x),y(_y)	{}
		
		tagPoint& operator=(const tagPoint& rhs)
		{
			x = rhs.x;
			y = rhs.y;

			return *this;
		}
	};


};

using namespace IUNIT;


namespace IUTIL
{	
	typedef	CString							    STR;
	typedef	CStringElementTraits<CString>	ITERSTR;
	typedef	vector<STR>						 VECSTR;
	typedef VECSTR*							PVECSTR;

	template <typename V>
	class CIDBMap : public CAtlMap<STR, V, CStringElementTraits<STR>> {};

	class CAccumString
	{		
	private:
		TCHAR * m_pString;
		long m_nLength;
		long m_nIncrement;
		long __nAllocLength;
		long __nPow;

	private:
		void __MemeoryAlloc(long newLength)
		{
			if( newLength < __nAllocLength ) return;

			int m = (newLength - __nAllocLength ) / m_nIncrement;

			__nAllocLength += (m_nIncrement * (m + 1));

			TCHAR * temp = new TCHAR [ __nAllocLength ];
			memset(temp, 0, sizeof(TCHAR) * __nAllocLength);
			
			memcpy(temp, m_pString, m_nLength * sizeof(TCHAR));

			delete [] m_pString;
			m_pString = temp;
		}

	public:
		CAccumString(void)
			:m_nLength(0)
			,m_nIncrement(4096)
			,__nAllocLength(m_nIncrement)
			,__nPow(12)
		{
			m_pString = new TCHAR [ __nAllocLength ];
			memset(m_pString, 0, sizeof(TCHAR) * __nAllocLength);
		}

		CAccumString(const CString& str)
			:m_nLength(0)
			,m_nIncrement(4096)
			,__nAllocLength(m_nIncrement)
			,__nPow(12)
		{
			m_pString = new TCHAR [ __nAllocLength ];
			this->Accumulate(str);
		}
		
		~CAccumString(void)
		{
			delete [] m_pString;
			m_pString = NULL;
		}

		void Empty()
		{
			m_nLength = 0;
			*m_pString = '\0';
		}

		operator TCHAR *()	{	return m_pString;	}
		operator LPCTSTR()	{	return (LPCTSTR)m_pString;	}

		CAccumString& operator = ( LPCTSTR str )
		{
			if(m_pString == str)
				return *this;

			int len((int)_tcslen(str));

			if( len > __nAllocLength )
			{
				delete [] m_pString;
				m_pString = NULL;

				__nAllocLength = ( (len / m_nIncrement ) + 1 ) * m_nIncrement;
				m_pString = new TCHAR [ __nAllocLength ];
				memset(m_pString, 0, sizeof(TCHAR) * __nAllocLength);
			}

			memcpy( m_pString, (LPCTSTR)str, len * sizeof(TCHAR));

			m_pString[len] = 0;

			m_nLength = len;

			return *this;

		}
		//CAccumString& operator = ( const CString& str )
		//{
		//	int len(str.GetLength());

		//	if( len > __nAllocLength )
		//	{
		//		delete [] m_pString;
		//		__nAllocLength = ( (len / m_nIncrement ) + 1 ) * m_nIncrement;
		//		m_pString = new TCHAR [ ( (len / m_nIncrement ) + 1) * m_nIncrement ];
		//	}

		//	memcpy( m_pString, (LPCTSTR)str, len * sizeof(TCHAR));

		//	m_pString[len] = 0;

		//	m_nLength = len;

		//	return *this;
		//}

		CAccumString& operator += ( const TCHAR * str)
		{
			if( str != NULL )
			{
				int len((int)_tcslen(str));

				__MemeoryAlloc(m_nLength + len);

				memcpy(m_pString + m_nLength,  str , len * sizeof(TCHAR));

				m_nLength += len;

				m_pString[m_nLength] = '\0';
			}

			return *this;

		}
		CAccumString& operator += ( CAccumString& str)
		{
			int len(str.GetLength());

			__MemeoryAlloc(m_nLength + len);

			memcpy(m_pString + m_nLength,  str.GetString(), len * sizeof(TCHAR));

			m_nLength += len;

			m_pString[m_nLength] = '\0';

			return *this;
		}
		
		CAccumString& operator += ( const CString& str )
		{
			int len(str.GetLength());
			
			__MemeoryAlloc(m_nLength + len);

			memcpy(m_pString + m_nLength, (const TCHAR *)str, len * sizeof(TCHAR));

			m_nLength += len;

			m_pString[m_nLength] = '\0';

			return *this;
		}

		CAccumString& Accumulate(CAccumString& str)
		{
			return (*this += str);
		}

		CAccumString& Accumulate(const CString& str)
		{
			return (*this += str);
		}

		CAccumString& Accumulate(const TCHAR* str, unsigned int len)
		{
			__MemeoryAlloc(m_nLength + len);

			memcpy(m_pString + m_nLength, str, len * sizeof(TCHAR));

			m_nLength += len;

			m_pString[m_nLength] = '\0';
			return *this;
		}
		
		CAccumString&  AccFormat( TCHAR * format, ... )
		{
			va_list args;
			int len;

			va_start( args, format );

			len = _vsctprintf(format, args ) + 1; 

			if( len > 0 )
			{
				__MemeoryAlloc(m_nLength + len);

				_vstprintf_s(m_pString + m_nLength, len, format, args ); 
				m_nLength += (len - 1);
				m_pString[m_nLength] = '\0';
			}
			return *this;
		}

		CAccumString& Format( TCHAR * format, ... )
		{
			va_list args;
			int len;

			va_start( args, format );

			len = _vsctprintf(format, args ) + 1; 
			
			if( len > 0 )
			{
				__MemeoryAlloc(len);

				_vstprintf_s(m_pString, len, format, args ); 
				m_nLength = (len - 1);
				m_pString[m_nLength] = '\0';
			}
			return *this;
		}


	public:
		TCHAR* GetString() const	{	return m_pString;	}
		long GetLength() const		{	return m_nLength;	}
		long GetIncrement() const	{	return m_nIncrement;}
		void SetIncremet(long incr)	
		{	
			__nPow = (long)(log((double)incr) / log(2.0)) + 1; 
			m_nIncrement = (long)pow(2.0, (int) __nPow);
		}
	};

	__forceinline double	RAD2DEG(double a)	{	return (a / IKRAD);			} 
	__forceinline float		RAD2DEG(float a)	{	return (a / (float)IKRAD);	} 
	__forceinline double	DEG2RAD(double a)	{	return (a * IKRAD);			}
	__forceinline float		DEG2RAD(float a)	{	return (a * (float)IKRAD);	}

	__forceinline float		NormalizeAngle(float angle) 
	{ 
		int		integer = (int)angle;
		float	real	= angle - (float)integer;
		integer			= integer % 360;
		integer			= integer < 0 ? (360 + integer) : integer;

		return			(float)integer + real;
	}


	template <typename T>
	void rotateVertice(IN float angle, IN long nvertice, IN OUT tagPoint<T>* pvertice)
	{
		float rad((float)DEG2RAD(angle));

		float cosval(cos(rad));
		float sinval(sin(rad));
		float inx(0), iny(0);

		for(long i = 0 ; i < nvertice ; ++i )
		{
			inx = pvertice[i].x;
			iny = pvertice[i].y;
			pvertice[i].x =  (inx) * cosval - (iny) * sinval;
			pvertice[i].y =  (inx) * sinval + (iny) * cosval;
		}		
	}

	template <typename T>
	void shiftVertice(T dx, T dy, long nvertice, IN OUT tagPoint<T>* pvertice)
	{
		for(long i = 0 ; i < nvertice ; ++i )
		{
			pvertice[i].x += dx;
			pvertice[i].y += dy;
		}
	}

	template <typename T>
	inline void rotateVertex(IN T orgx, IN T orgy, IN T inx, IN T iny, IN T cosval, IN T sinval, OUT T& outx, OUT T& outy)
	{
		outx =	(inx - orgx) * cosval + (iny - orgy) * sinval + orgx;
		outy = -(inx - orgx) * sinval + (iny - orgy) * cosval + orgy;
	}

	template <typename T>
	inline void getBoundary(IN tagPoint<T>* pvertice, IN long nvertice, OUT T &l, OUT T &t, OUT T &r, OUT T &b)
	{
		l = (T)99999999.f;
		t = (T)99999999.f;
		r = (T)-99999999.f;
		b = (T)-99999999.f;

		for(long i = 0 ; i < nvertice ; ++i )
		{
			l = min(l, pvertice[i].x);
			t = min(t, pvertice[i].y);
			r = max(r, pvertice[i].x);
			b = max(b, pvertice[i].y);
		}
	}

	template <typename T>
	inline void moveVerticeOrigin(IN tagPoint<T>* pvertice, IN long nvertice, T ox, T oy)
	{
		for(long i = 0 ; i < nvertice ; ++i )
		{
			T px(pvertice[i].x), py(pvertice[i].y);
			T dx(px - ox), dy(py - oy);

			pvertice[i].x = dx;
			pvertice[i].y = dy;
		}
	}
	template <typename T>
	inline BOOL interSectionPoint(T x, T y, T l, T t, T r, T b)
	{
		return ( x >= l && x <= r && y >= t && y <= b );
	}

	template <typename T>
	inline BOOL interSectionRect(T l1, T t1, T r1, T b1, T l2, T t2, T r2, T b2)
	{
		return ( l1 <= r2 && l2 <= r1 && t1 <= b2 && t2 <= b1);
	}

	template <typename T>
	inline BOOL PointInRect(T x, T y, T l, T t, T r, T b)
	{
		return ( x > l && y > t && x < r && y < b );
	}

	template <typename T>
	inline BOOL RectInRect(T l1, T t1, T r1, T b1, T l2, T t2, T r2, T b2)
	{
		return ( l1 > l2 && t1 > t2 && r1 < r2 && b1 < b2 );
	}

	template<typename T>
	void NormalizeRect(tagRect<T>& rt)
	{
		tagRect<T> rect(min(rt.l, rt.r), min(rt.t, rt.b), max(rt.l, rt.r), max(rt.t, rt.b));
		rt = rect;
	}

	template<typename T>
	void NormalizeRect(T& l, T& t, T& r, T& b)
	{
		if( l > r ) swap(l, r);
		if( t > b ) swap(t, b);
	}

	// 메모리해제 유틸리티
	template <typename T>
	inline void safeRelease(T*& p)
	{
		if(p)
		{
			delete p;
			p = NULL;
		}
	}

	template <typename T>
	inline void safeReleaseWindowPtr(T*& pWnd)
	{
		if(pWnd)
		{
			if(pWnd->GetSafeHwnd())
			{
				pWnd->DestroyWindow();
			}

			safeRelease(pWnd);
		}

	}

	template <typename T>
	inline void safeReleaseArray(T*& p)
	{
		if(p)
		{
			delete [] p;
			p = NULL;
		}
	}
	

	template <typename K, typename V, class KTraits, class VTraits>
	void safeReleaseMap(CAtlMap<K,V,KTraits,VTraits>& map)
	{
		POSITION pos = map.GetStartPosition();

		while( pos != NULL )
		{
			delete (map.GetValueAt(pos));		
			map.GetNext(pos);
		}	
		map.RemoveAll();
	}

	template <typename K, typename V>
	void safeReleaseMap(std::map<K,V>& map)
	{
		std::map<K,V>::iterator end = map.end();
		std::map<K,V>::iterator iter = map.begin();

		while( iter != end )
		{
			delete (iter->second);
			++iter;
		}

		map.clear();
	}

	template <typename T>
	void safeReleaseVector(std::vector<T>& target)
	{
		UINT size = (UINT)target.size();
		
		if( size == 0 ) return;

		T * pBegin = &target[0];

		for( UINT i = 0 ; i < size ; ++i )
		{
			delete *(pBegin + i);
		}
		target.clear();
	}

	// V가 포인터여야 리턴시 복사 생성자로 인한 퍼포먼스 저하를 피할수있습니다.
	template <typename K, typename V, class KTraits, class VTraits>
	inline V findMapZero(const CAtlMap<K,V,KTraits,VTraits>& map, const K& key)
	{
		const CAtlMap<K,V,KTraits,VTraits>::CPair* pPair = map.Lookup(key);
		return (pPair ? pPair->m_value : NULL);
	}

	template <typename K, typename V>
	inline V findMapZero(const std::map<K,V>& map, const K& key)
	{
		std::map<K,V>::const_iterator end = map.end();
		std::map<K,V>::const_iterator iter = map.find(key);
		if( end == iter )
		{
			return NULL;
		}

		return (iter->second);
	}

	template <typename K, typename V, class KTraits, class VTraits>
	inline V findMapNegative(const CAtlMap<K,V,KTraits,VTraits>& map, const K& key)
	{
		const CAtlMap<K,V,KTraits,VTraits>::CPair* pPair = map.Lookup(key);
		return (pPair ? pPair->m_value : INVALID_HANDLE_VALUE);
	}

	template <typename K, typename V>
	inline V findMapNegative(const std::map<K,V>& map, const K& key)
	{
		std::map<K,V>::const_iterator end = map.end();
		std::map<K,V>::const_iterator iter = map.find(key);
		if( end == iter )
		{
			return (V)-1;
		}

		return (iter->second);
	}

	template <typename K, typename V, class KTraits, class VTraits>
	void safeReleaseArrayMap(CAtlMap<K,V,KTraits,VTraits>& map)
	{
		POSITION pos = map.GetStartPosition();

		while( pos != NULL )
		{
			delete [] (map.GetValueAt(pos));		
			map.GetNext(pos);
		}	
		map.RemoveAll();
	}

	template <typename K, typename V>
	void safeReleaseArrayMap(std::map<K,V>& map)
	{
		std::map<K,V>::iterator end = map.end();
		std::map<K,V>::iterator iter = map.begin();

		while( iter != end )
		{
			delete [] (iter->second);
			++iter;
		}

		map.clear();
	}

	template <typename T>
	void safeReleaseArrayVector(std::vector<T>& target)
	{
		size_t size = target.size();

		if( size == 0 ) return;

		T * pBegin = &target[0];

		for( UINT i = 0 ; i < size ; ++i )
		{
			delete [] *(pBegin + i);
		}
		target.clear();
	}

	template <typename T>
	void safeReleaseArrayVector(CAtlArray<T>& target)
	{
		UINT size = (UINT)target.GetCount();

		if( size == 0 ) return;

		T * pBegin = &target[0];

		for( UINT i = 0 ; i < size ; ++i )
		{
			delete [] *(pBegin + i);
		}
		target.RemoveAll();
	}

	template <typename T>
	void safeReleaseArrayVector(CArray<T, T>& target)
	{
		UINT size = target.GetCount();

		if( size == 0 ) return;

		T * pBegin = &target[0];

		for( UINT i = 0 ; i < size ; ++i )
		{
			delete [] *(pBegin + i);
		}
		target.RemoveAll();
	}

	template<typename T>
	void safeReleaseList(list<T>& target)
	{
		list<T>::iterator iter = target.begin();

		while( iter != target.end() )
		{
			safeRelease(*iter);
			++iter;
		}
	}

	template<typename T>
	void safeReleaseListArray(list<T>& target)
	{
		list<T>::iterator iter = target.begin();

		while( iter != target.end() )
		{
			safeReleaseArray(*iter);
			++iter;
		}
	}

	template<typename T>
	BOOL OutOfRange(const vector<T>& v, long nIndex)
	{
		return nIndex < 0 || nIndex >= (long)v.size();
	}

};

using namespace IUTIL;
